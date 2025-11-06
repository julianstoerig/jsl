#include "num.h"
#define BASE_IMPL
#include "/home/julian/jd/30-39-hobbies/32-programming/32.022-jsl/base.h"
#include <stdio.h>


void numerr(char *s) {
    fputs(s, stderr);
    abort();
}

B32 isvector(Mat m) {
    return (m.rows == 1) | (m.cols == 1);
}

F64 *mat_at(Mat m, S64 i, S64 j) {
    assert(0 <= i);
    assert(i < m.rows);
    assert(0 <= j);
    assert(j < m.cols);
    S64 lin_idx = i * m.cols + j;
    return m.v + lin_idx;
}

F64 *vec_at(Mat m, S64 i) {
    assert(0 <= i);
    assert((i < m.rows) | (i < m.cols));
    if (!isvector(m)) assert(0), abort();
    return m.v + i;
}

S64 mat_count(Mat m) {
    return m.rows * m.cols;
}

B32 mat_equal(Mat lhs, Mat rhs, F64 eps_abs, F64 eps_rel) {
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.cols; j+=1) {
            if (!isclose(at2(lhs, i, j), at2(rhs, i, j), eps_abs, eps_rel)) return 0;
        }
    }
    return 1;
}

Mat mat_empty(S64 rows, S64 cols, Arena *ap) {
    F64 *v = make(ap, F64, rows*cols);
    Mat self = {
        .rows = rows,
        .cols = cols,
        .v = v,
    };
    return self;
}

Mat vec_from_row(Mat m, S64 i, Arena *ap) {
    F64 *v = make(ap, F64, m.rows);
    F64 *src = m.v + m.cols * i;
    S64 len_bytes = m.cols * sizeof(*v);
    memcpy(v, src, len_bytes);
    Mat self = {
        .v = v,
        .rows = 1,
        .cols = m.cols,
    };
    return self;
}

Mat vec_from_col(Mat m, S64 j, Arena *ap) {
    F64 *v = make(ap, F64, m.rows);
    for (S64 k=0; k<m.rows; k+=1) {
        v[k] = at2(m, k, j);
    }
    Mat self = {
        .v = v,
        .rows = m.rows,
        .cols = 1,
    };
    return self;
}

Mat vec_linspace(F64 xmin, F64 xmax, S64 n, Arena *ap) {
    Mat v = vec_row_empty(n, ap);
    F64 dx = (xmax - xmin) / n;
    for (S64 i=0; i<n; i+=1) {
        at1(v, i) = xmin + i * dx;
    }
    return v;
}

Mat mat_dup(Mat m, Arena *ap) {
    Mat mat = mat_empty(m.rows, m.cols, ap);
    S64 len_bytes = mat.rows*mat.cols*sizeof(*mat.v);
    memcpy(mat.v, m.v, len_bytes);
    return mat;
}

Mat mat_augment(Mat A, Mat B, Arena *ap) {
    B32 A_and_B_compat = A.rows == B.rows;
    assert(A_and_B_compat);
    Mat aug = mat_empty(A.rows, A.cols+B.cols, ap);
    for (S64 i=0; i<A.rows; i+=1) {
        for (S64 j=0; j<A.cols; j+=1) {
            at2(aug, i, j) = at2(A, i, j);
        }
        for (S64 j=0; j<B.cols; j+=1) {
            at2(aug, i, A.cols+j) = at2(B, i, j);
        }

    }
    return aug;
}

Mat mat_from_array(S64 rows, S64 cols, F64 *v, Arena *ap) {
    Mat mat = mat_empty(rows, cols, ap);
    S64 len_bytes = mat.rows*mat.cols*sizeof(*mat.v);
    memcpy(mat.v, v, len_bytes);
    return mat;
}

Mat mat_zeroes(S64 rows, S64 cols, Arena *ap) {
    Mat self = mat_empty(rows, cols, ap);
    memset(self.v, 0x00, self.rows*self.cols*sizeof(*self.v));
    return self;
}

Mat mat_ident(S64 rows, S64 cols, Arena *ap) {
    Mat self = mat_zeroes(rows, cols, ap);
    S64 min_dim = rows;
    if (cols < min_dim) min_dim = cols;
    for (S64 i=0; i<min_dim; i+=1) {
        at2(self, i, i) = 1.0;
    }
    return self;
}

Mat mat_uniform(S64 rows, S64 cols, F64 x, Arena *ap) {
    Mat self = mat_empty(rows, cols, ap);
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) = x;
        }
    }
    return self;
}

void mat__fprint(FILE *f, Mat self, S32 precision) {
    fprintf(f, "[");
    F64 max_val = 0;
    B32 there_exists_neg_entry = 0;
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            F64 val = at2(self, i, j);
            if (val < 0) there_exists_neg_entry = 1;
            val = fabs(val);
            if (val > max_val) max_val = val;
        }
    }
    S32 max_digits_before_dec_point = (max_val == 0.0) ? 1 : floor(log10(max_val))+1;
    S32 total_width = there_exists_neg_entry + max_digits_before_dec_point + 1 + precision;
    for (S64 i=0; i<self.rows; i+=1) {
        if (i == 0) {
            fprintf(f, "[");
        } else {
            fprintf(f, ", \n [");
        }
        for (S64 j=0; j<self.cols; j+=1) {
            if (j > 0) fprintf(f, ", ");
            fprintf(f, "%*.*f", total_width, precision, at2(self, i, j));
        }
        fprintf(f, " ]");
    }
    fprintf(f, "]\n");
}

void mat__reshape_ip(Mat *m, S64 rows, S64 cols) {
    S64 new_count = rows*cols;
    if (mat_count(*m) != new_count) assert(0), abort();
    m->rows = rows;
    m->cols = cols;
}

Mat mat_reshape(Mat m, S64 rows, S64 cols, Arena *ap) {
    Mat res = mat_dup(m, ap);
    mat_reshape_ip(res, rows, cols);
    return res;
}

void mat_add_ip(Mat lhs, Mat rhs) {
    if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) assert(0), abort();
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.rows; j+=1) {
            at2(lhs, i, j) += at2(rhs, i, j);
        }
    }
}

Mat mat_add(Mat lhs, Mat rhs, Arena *ap) {
    Mat res = mat_dup(lhs, ap);
    mat_add_ip(res, rhs);
    return res;
}

void mat_sub_ip(Mat lhs, Mat rhs) {
    if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) assert(0), abort();
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.rows; j+=1) {
            at2(lhs, i, j) -= at2(rhs, i, j);
        }
    }
}

Mat mat_sub(Mat lhs, Mat rhs, Arena *ap) {
    Mat res = mat_dup(lhs, ap);
    mat_sub_ip(res, rhs);
    return res;
}

void mat_mul_ip(Mat lhs, Mat rhs) {
    if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) assert(0), abort();
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.rows; j+=1) {
            at2(lhs, i, j) *= at2(rhs, i, j);
        }
    }
}

Mat mat_mul(Mat lhs, Mat rhs, Arena *ap) {
    Mat res = mat_dup(lhs, ap);
    mat_mul_ip(res, rhs);
    return res;
}

void mat_div_ip(Mat lhs, Mat rhs) {
    if (lhs.rows != rhs.rows || lhs.cols != rhs.cols) assert(0), abort();
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.rows; j+=1) {
            at2(lhs, i, j) /= at2(rhs, i, j);
        }
    }
}

Mat mat_div(Mat lhs, Mat rhs, Arena *ap) {
    Mat res = mat_dup(lhs, ap);
    mat_div_ip(res, rhs);
    return res;
}

void mat_scale_ip(Mat lhs, F64 rhs) {
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.rows; j+=1) {
            at2(lhs, i, j) *= rhs;
        }
    }
}

Mat mat_scale(Mat lhs, F64 rhs, Arena *ap) {
    Mat res = mat_dup(lhs, ap);
    mat_scale_ip(res, rhs);
    return res;
}


void mat_round_ip(Mat lhs, S64 decimals) {
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.cols; j+=1) {
            F64 factor = pow(10, decimals);
            at2(lhs, i, j) = round(at2(lhs, i, j) * factor) / factor;
        }
    }
}

Mat mat_round(Mat lhs, S64 decimals, Arena *ap) {
    Mat res = mat_dup(lhs, ap);

    mat_round_ip(res, decimals);

    return res;
}

F64 mat_scalar_prod(Mat lhs, Mat rhs) {
    F64 res = 0;
    for (S64 i=0; i<lhs.rows; i+=1) {
        for (S64 j=0; j<lhs.cols; j+=1) {
            res += at2(lhs, i, j) * at2(rhs, i, j);
        }
    }
    return res;
}

void mat_transpose_square_ip(Mat self) {
    assert(self.rows == self.cols);
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=i+1; j<self.cols; j+=1) {
            F64 tmp = at2(self, i, j);
            at2(self, i, j) = at2(self, j, i);
            at2(self, j, i) = tmp;
        }
    }
}

Mat mat_transpose(Mat self, Arena *ap) {
    Mat res = mat_empty(self.cols, self.rows, ap);
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(res, j, i) = at2(self, i, j);
        }
    }
    return res;
}

Mat mat_matmul(Mat lhs, Mat rhs, Arena *ap) {
    if (lhs.cols != rhs.rows) assert(0), abort();
    Mat self = mat_zeroes(lhs.rows, rhs.cols, ap);
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            for (S64 k=0; k<lhs.cols; k+=1) {
                at2(self, i, j) += at2(lhs, i, k) * at2(rhs, k, j);
            }
        }
    }
    return self;
}

Mat mat_matmul_diag(Mat lhs, Mat rhs, Arena *ap) {
    B32 lhs_square = lhs.rows == lhs.cols;
    B32 rhs_square = rhs.rows == rhs.cols;
    B32 lhs_and_rhs_compat = lhs.cols == rhs.rows; 
    assert(lhs_square & rhs_square & lhs_and_rhs_compat);
    Mat self = mat_zeroes(lhs.rows, rhs.cols, ap);
    for (S64 i=0; i<self.rows; i+=1) {
        at2(self, i, i) += at2(lhs, i, i) * at2(rhs, i, i);
    }
    return self;
}

Mat mat_gauss(Mat A, Mat b, Arena *ap) {
    B32 A_square = A.rows == A.cols;
    B32 b_col_vec = b.cols == 1;
    B32 A_and_b_compat = A.rows == b.rows;
    assert(A_square & b_col_vec & A_and_b_compat);

    Mat x = vec_col_empty(b.rows, ap);
    Mat aug = mat_augment(A, b, ap);

    for (S64 i=0; i<aug.rows; i+=1) {
        assert(at2(aug, i, i) != 0.0);
        for (S64 j=i+1; j<aug.rows; j+=1) {
            F64 scaling_factor = at2(aug, j, i) / at2(aug, i, i);
            for (S64 k=0; k<aug.cols; k+=1) {
                at2(aug, j, k) = at2(aug, j, k) - scaling_factor * at2(aug, i, k);
            }
        }
    }
    S64 n = x.rows;
    S64 m = n-1;
    at1(x, m) = at2(aug, m, x.rows) / at2(aug, m, m);

    for (S64 i=n-2; i>=0; i-=1) {

        at1(x, i) = at2(aug, i, n);

        for (S64 j=i+1; j<n; j+=1) {
            at1(x, i) = at1(x, i) - at2(aug, i, j) * at1(x, j);
        }
        at1(x, i) = at1(x, i) / at2(aug, i, i);
    }

    return x;
}

F64 mat_det(Mat lhs) {
    if (lhs.rows != lhs.cols) return 0;
    F64 res = 0;
    // gaussian elimination or something
    return res;
}

void mat_swap_rows_ip(Mat self, S64 from, S64 into) {
    for (S64 i=0; i<self.cols; i+=1) {
        F64 swap = at2(self, from, i);
        at2(self, from, i) = at2(self, into, i);
        at2(self, into, i) = swap;
    }
}

Mat mat_swap_rows(Mat self, S64 from, S64 into, Arena *ap) {
    Mat res = mat_dup(self, ap);

    mat_swap_rows_ip(res, from, into);

    return res;
}

void mat_swap_cols_ip(Mat self, S64 from, S64 into) {
    for (S64 j=0; j<self.rows; j+=1) {
        F64 swap = at2(self, j, from);
        at2(self, j, from) = at2(self, j, into);
        at2(self, j, into) = swap;
    }
}

Mat mat_swap_cols(Mat self, S64 from, S64 into, Arena *ap) {
    Mat res = mat_dup(self, ap);

    mat_swap_cols_ip(res, from, into);

    return res;
}

F64 vec_norm_euclid(Mat self) {
    assert(isvector(self));
    F64 total = 0;
    S64 length = mat_count(self);
    for (S64 i=0; i<length; i+=1) {
        F64 x = at1(self, i);
        total += x*x;
    }
    total = sqrt(total);
    return total;
}

F64 mat_norm_inf_rowwise(Mat self) {
    F64 norm = 0;
    for (S64 i=0; i<self.rows; i+=1) {
        F64 row_sum = 0;
        for (S64 j=0; j<self.cols; j+=1) {
            row_sum += fabs(at2(self, i, j));
        }
        if (row_sum > norm) norm = row_sum;
    }
    return norm;
}

F64 mat_norm_inf_colwise(Mat self) {
    F64 norm = 0;
    for (S64 j=0; j<self.cols; j+=1) {
        F64 col_sum = 0;
        for (S64 i=0; i<self.rows; i+=1) {
            col_sum += fabs(at2(self, i, j));
        }
        if (col_sum > norm) norm = col_sum;
    }
    return norm;
}

B32 isclose(F64 x, F64 y, F64 eps_abs, F64 eps_rel) {
    x = fabs(x);
    y = fabs(y);
    F64 x_min = x - eps_abs - x*eps_rel;
    F64 x_max = x + eps_abs + x*eps_rel;
    return (x_min <= y) & (y <= x_max);
}

F64 findiff(F64 f(F64), F64 x, F64 dx) {
    F64 y1 = f(x+dx);
    F64 y0 = f(x-dx);
    F64 dydx = (y1 - y0) / (dx*2);
    return dydx;
}

NumResult bisect(F64 f(F64), F64 x0, F64 x1, F64 xtol, S64 max_iterations) {
    F64 err = fabs(x1 - x0);
    F64 x2 = 0;
    S32 iterations = 0;
    for (; iterations<max_iterations; iterations+=1) {
        iterations += 1;
        x2 = (x1 + x0) / 2;
        B32 in_upper_bracket = f(x0) * f(x2) > 0;
        if (in_upper_bracket) {
            x0 = x2;
        } else {
            x1 = x2;
        }
        err = fabs(x1 - x0);
        if (err <= xtol)
            break;
    }
    NumResult res = {
        .v = x2,
        .xtol = err,
        .iter = iterations,
    };
    return res;
}

NumResult newton(F64 f(F64), F64 df(F64), F64 x0, F64 xtol, S64 max_iterations) {
    S32 iterations = 0;
    F64 err = f64_inf();
    F64 x1 = x0;
    for (; iterations<max_iterations; iterations+=1) {
        x1 = x0 - f(x0)/df(x0);
        err = fabs(x1 - x0);
        x0 = x1;
        if (err <= xtol)
            break;
    }
    NumResult res = {
        .v = x1,
        .xtol = err,
        .iter = iterations,
    };
    return res;
}

NumResult fixpoint(F64 f(F64), F64 x0, F64 xtol, S64 max_iterations) {
    F64 err = f64_inf();
    S32 iterations = 0;
    F64 x1 = x0;
    for (; iterations<max_iterations; iterations+=1) {
        x1 = f(x0);
        err = fabs(x1 - x0);
        if (err <= xtol)
            break;
        x0 = x1;
    }
    NumResult res = {
        .v = x1,
        .xtol = err,
        .iter = iterations,
    };
    return res;
}


NumResult secant(F64 f(F64), F64 x0, F64 x1, F64 xtol, S64 max_iterations) {
    S32 iterations = 0;
    F64 err = f64_inf();
    for (; iterations<max_iterations; iterations+=1) {
        F64 df = (f(x1)-f(x0)) / (x1-x0);
        x0 = x1;
        x1 = x1 - f(x1)/df;
        err = fabs(x1 - x0);
        if (err <= xtol)
            break;
    }
    NumResult res = {
        .v = x1,
        .xtol = err,
        .iter = iterations,
    };
    return res;
}

NumResult secant_bisect(F64 f(F64), F64 x0, F64 x1, F64 xtol, S64 max_iterations) {
    S64 secant_iterations = max_iterations / 2;
    NumResult res = secant(f, x0, x1, xtol, secant_iterations);
    if (res.xtol > xtol)
        res = bisect(f, res.v-res.xtol, res.v+res.xtol, xtol, max_iterations-res.iter);
    return res;
}

void write_f64v_to_file(FILE *f, F64 *x, F64 *y, S64 n) {
    for (S64 i=0; i<n; i+=1) {
        fprintf(f, "%f %f\n", x[i], y[i]);
    }
}

void eval_f_on_range(F64 f(F64), F64 **xp, F64 **yp, F64 xmin, F64 xmax, S64 n, Arena *ap) {
    *xp = make(ap, F64, n);
    *yp = make(ap, F64, n);
    F64 *x = *xp;
    F64 *y = *yp;
    for (S64 i=0; i<n; i+=1) {
        F64 rel = (F64)i/n;
        x[i] = -0.5 + rel * (1- -2);
        y[i] = f(x[i]);
    }
}
