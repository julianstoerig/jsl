#include <stdio.h>

#include <tgmath.h>
#include <stdlib.h>
#include <stdint.h>

typedef int64_t S64;
typedef double  F64;


typedef struct Matrix Matrix;
struct Matrix {
    F64 *v;
    S64 col_stride;
    S64 col_offset;
    S64 col_step;
    S64 cols;
    S64 row_stride;
    S64 row_offset;
    S64 row_step;
    S64 rows;
};

Matrix matrix_get_row_vector(Matrix self, S64 i) {
    Matrix row = {0};
    row.v = self.v;
    row.col_stride = self.col_stride;
    row.col_step = self.col_step;
    row.rows = 1;
    row.cols = self.cols;
    return row;
}

Matrix matrix_get_col_vector(Matrix self, S64 j) {
    Matrix col = {0};
    col.v = self.v;
    col.row_stride = self.row_stride;
    col.row_step = self.row_step;
    col.rows = self.rows;
    col.cols = 1;
    return col;
}

Matrix matrix_row_to_col_vector(Matrix self) {
    Matrix col = {0};
    col.v = self.v;
    col.col_stride = self.row_stride;
    col.col_step = self.row_step;
    col.cols = self.rows;
    col.cols = 1;
    return col;
}

#define at2(self, i, j) *matrix_at2((self), (i), (j))
F64 *matrix_at2(Matrix self, S64 i, S64 j) {
    S64 lin_idx = (self.row_offset+i*self.row_step)*self.row_stride +
                    (self.col_offset+j*self.col_step)*self.col_stride;
    return self.v+lin_idx;
}

Matrix matrix_slice(Matrix other, S64 i, S64 j, S64 rows, S64 cols) {
    Matrix self = other;
    self.row_offset = i;
    self.row_step = 1;
    self.rows = rows;
    self.col_offset = j;
    self.col_step = 1;
    self.cols = cols;
    return self;
}

void matrix_print(Matrix self) {
    for (S64 i=0; i<self.rows; i+=1) {
        printf("\n");
        for (S64 j=0; j<self.cols; j+=1) {
            if (j > 0) printf(" ");
            F64 x = at2(self, i, j);
            printf("%g", x);
        }
    }
    printf("\n");
}

Matrix matrix_empty(S64 rows, S64 cols) {
    Matrix self = {
        .row_stride = cols,
        .row_offset = 0,
        .row_step = 1,
        .rows = rows,
        .col_stride = 1,
        .col_offset = 0,
        .col_step = 1,
        .cols = cols,
    };
    S64 bytes_n = rows*cols*sizeof(*self.v);
    self.v = malloc(bytes_n);
    if (self.v == 0) abort();
    return self;
}

void matrix_free(Matrix self) {
    free(self.v);
}

Matrix matrix_uniform(S64 rows, S64 cols, F64 x) {
    Matrix self = matrix_empty(rows, cols);
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) = x;
        }
    }
    return self;
}

Matrix matrix_ident(S64 rows, S64 cols) {
    Matrix self = matrix_uniform(rows, cols, 0);
    S64 smaller_max = self.rows;
    if (self.cols < smaller_max) smaller_max = self.cols;
    for (S64 i=0; i<smaller_max; i+=1) {
            at2(self, i, i) = 1;
    }
    return self;
}

void matrix_scale_ip(Matrix self, F64 x) {
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) *= x;
        }
    }
}

void matrix_add_ip(Matrix self, Matrix other) {
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) += at2(other, i, j);
        }
    }
}

void matrix_sub_ip(Matrix self, Matrix other) {
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) -= at2(other, i, j);
        }
    }
}

void matrix_mul_ip(Matrix self, Matrix other) {
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) *= at2(other, i, j);
        }
    }
}

void matrix_div_ip(Matrix self, Matrix other) {
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            at2(self, i, j) /= at2(other, i, j);
        }
    }
}

F64 matrix_scalar_product(Matrix self, Matrix other) {
    F64 res = 0;
    if (self.rows != other.rows || self.cols != other.cols) abort();
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.cols; j+=1) {
            res += at2(self, i, j) * at2(other, i, j);
        }
    }
    return res;
}

Matrix matrix_matmul(Matrix self, Matrix other) {
    if (self.cols != other.rows) abort();
    Matrix new = matrix_uniform(self.rows, other.cols, 0);
    for (S64 i=0; i<self.rows; i+=1) {
        for (S64 j=0; j<self.rows; j+=1) {
            Matrix row = matrix_get_row_vector(self, i);
            Matrix col = matrix_get_col_vector(other, j);
            col = matrix_row_to_col_vector(col);
            at2(new, i, j) = matrix_scalar_product(row, col);
        }
    }
    return new;
}


int main(void) {
    Matrix B = matrix_ident(3, 3);
    matrix_print(B);
    matrix_scale_ip(B, 42.69);
    matrix_print(B);
    Matrix C = matrix_slice(B, 0, 1, 3, 2);
    matrix_print(C);
    matrix_print(B);
    matrix_free(B);
    Matrix A = matrix_empty(3, 1);
    at2(A, 0, 0) = 1;
    at2(A, 1, 0) = 2;
    at2(A, 2, 0) = 3;
    matrix_print(A);
    F64 s = matrix_scalar_product(A, A);
    printf("A·A=%g\n", s);
    matrix_free(A);
}
