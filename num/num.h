#pragma once
#include "tgmath.h"
#include "/home/julian/jd/30-39-hobbies/32-programming/32.022-jsl/base.h"

typedef uint8_t        U08;
typedef uint32_t       B32;
typedef int32_t        S32;
typedef int64_t        S64;
typedef double         F64;
typedef float          F32;

typedef struct {
    F64 *v;
    S64 rows;
    S64 cols;
} Mat;

B32 isvector(Mat m);

#define at2(m, i, j) *mat_at((m), (i), (j))
F64 *mat_at(Mat m, S64 i, S64 j);

#define at1(v, i) *vec_at((v), (i))
F64 *vec_at(Mat m, S64 i);

Mat vec_linspace(F64 xmin, F64 xmax, S64 n, Arena *ap);

typedef struct NumResult NumResult;
struct NumResult {
    F64 v;
    F64 xtol;
    S64 iter;
};

void numerr(char *s);

S64 mat_cols(Mat self);

S64 mat_rows(Mat self);

#define vec_len(self) mat_count(self)
S64 mat_count(Mat self);

B32 mat_equal(Mat lhs, Mat rhs, F64 eps_abs, F64 eps_rel);

#define vec_row_empty(len, ap) mat_empty(1, (len), (ap))
#define vec_col_empty(len, ap) mat_empty((len), 1, (ap))
Mat mat_empty(S64 rows, S64 cols, Arena *ap);

#define vec(ap, ...) vec_col((ap), __VA_ARGS__)
#define vec_row(ap, ...)\
    vec_row_from_array(sizeof(((F64[]){__VA_ARGS__})) / sizeof(((F64[]){__VA_ARGS__}[0])), ((F64[]){__VA_ARGS__}), (ap))
#define vec_col(ap, ...)\
    vec_col_from_array(sizeof(((F64[]){__VA_ARGS__})) / sizeof(((F64[]){__VA_ARGS__}[0])), ((F64[]){__VA_ARGS__}), (ap))
#define mat(ap, rows, cols, ...)\
    mat_from_array((rows), (cols), (F64[]){__VA_ARGS__}, (ap))

#define vec_row_from_array(len, v, ap) mat_from_array(1, (len), (v), (ap))
#define vec_col_from_array(len, v, ap) mat_from_array((len), 1, (v), (ap))
Mat mat_from_array(S64 rows, S64 cols, F64 *v, Arena *ap);

Mat mat_dup(Mat m, Arena *ap);

Mat mat_augment(Mat A, Mat B, Arena *ap);

#define vec_row_zeroes(len, ap) mat_zeroes(1, (len), (ap))
#define vec_col_zeroes(len, ap) mat_zeroes((len), 1, (ap))
Mat mat_zeroes(S64 rows, S64 cols, Arena *ap);

Mat mat_ident(S64 rows, S64 cols, Arena *ap);

#define vec_row_uniform(len, x, ap) mat_uniform(1, (len), (x), (ap))
#define vec_col_uniform(len, x, ap) mat_uniform((len), 1, (x), (ap))
Mat mat_uniform(S64 rows, S64 cols, F64 x, Arena *ap);

#define mat_print(...) mat_printx(__VA_ARGS__, mat_print2, mat_print1) (__VA_ARGS__)
#define mat_printx(a, b, c, ...) c
#define mat_print1(self)        mat__fprint(stdout, (self), 2)
#define mat_print2(self, prec)  mat__fprint(stdout, (self), (prec))
#define mat_fprint(...) mat_fprintx(__VA_ARGS__, mat_fprint2, mat_fprint1) (__VA_ARGS__)
#define mat_fprintx(a, b, c, d, ...) d
#define mat_fprint1(f, self)        mat__fprint((f), (self), 2)
#define mat_fprint2(f, self, prec)  mat__fprint((f), (self), (prec))
void mat__fprint(FILE *f, Mat self, S32 precision);

#define mat_reshape_ip(m, rows, cols) mat__reshape_ip(&(m), (rows), (cols))
void mat__reshape_ip(Mat *m, S64 rows, S64 cols);
Mat mat_reshape(Mat m, S64 rows, S64 cols, Arena *ap);

void mat_add_ip(Mat lhs, Mat rhs);
Mat mat_add(Mat lhs, Mat rhs, Arena *ap);

void mat_sub_ip(Mat lhs, Mat rhs);
Mat mat_sub(Mat lhs, Mat rhs, Arena *ap);

void mat_mul_ip(Mat lhs, Mat rhs);
Mat mat_mul(Mat lhs, Mat rhs, Arena *ap);

void mat_div_ip(Mat lhs, Mat rhs);
Mat mat_div(Mat lhs, Mat rhs, Arena *ap);

void mat_scale_ip(Mat lhs, F64 rhs);
Mat mat_scale(Mat lhs, F64 rhs, Arena *ap);

void mat_round_ip(Mat lhs, S64 decimals);
Mat mat_round(Mat lhs, S64 decimals, Arena *ap);

F64 mat_scalar_prod(Mat lhs, Mat rhs);

void mat_transpose_square_ip(Mat self);
Mat mat_transpose(Mat self, Arena *ap);

Mat mat_matmul(Mat lhs, Mat rhs, Arena *ap);

Mat mat_matmul_diag(Mat lhs, Mat rhs, Arena *ap);

Mat mat_gauss(Mat A, Mat b, Arena *ap);

void mat_swap_rows_ip(Mat self, S64 from, S64 into);
Mat mat_swap_rows(Mat self, S64 from, S64 into, Arena *ap);

void mat_swap_cols_ip(Mat self, S64 from, S64 into);
Mat mat_swap_cols(Mat self, S64 from, S64 into, Arena *ap);

F64 mat_norm_euclid(Mat self);
F64 mat_norm_inf_rowwise(Mat self);
F64 mat_norm_inf_colwise(Mat self);

B32 isclose(F64 x, F64 y, F64 eps_abs, F64 eps_rel);

F64 findiff(F64 f(F64), F64 x, F64 dx);

NumResult bisect(F64 f(F64), F64 x0, F64 x1, F64 xtol, S64 max_iterations);

NumResult secant(F64 f(F64), F64 x0, F64 x1, F64 xtol, S64 max_iterations);

NumResult secant_bisect(F64 f(F64), F64 x0, F64 x1, F64 xtol, S64 max_iterations);

NumResult newton(F64 f(F64), F64 df(F64), F64 x0, F64 xtol, S64 max_iterations);

NumResult fixpoint(F64 f(F64), F64 x0, F64 xtol, S64 max_iterations);

F64 int_exp_riemann_l(F64 f(F64), F64 dx, F64 xmin, F64 xmax);

F64 int_exp_riemann_r(F64 f(F64), F64 dx, F64 xmin, F64 xmax);

F64 int_imp_riemann_l(Mat y, F64 dx);

F64 int_exp_trapezoid(F64 f(F64), F64 dx, F64 xmin, F64 xmax);

F64 int_imp_trapezoid(Mat y, F64 dx);

void eval_f_on_range(F64 f(F64), F64 **x, F64 **y, F64 xmin, F64 xmax, S64 n, Arena *ap);

void write_f64v_to_file(FILE *f, F64 *x, F64 *y, S64 n);
