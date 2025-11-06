#include "num.h"

F64 f0(F64 x) {
    return 1;
}

F64 f1(F64 x) {
    return x;
}

F64 f2(F64 x) {
    return x*x;
}

F64 f3(F64 x) {
    return log(x);
}

F64 f4(F64 x) {
    return pow(2, x);
}

int main(void) {
    Arena *a = arena_create(10 * MiB);

    if (
        !isclose(int_exp_riemann_l(f0, 1e-3, 0, 2), 2, 1e-9, 0) ||
        !isclose(int_exp_riemann_r(f0, 1e-3, 0, 2), 2, 1e-9, 0) ||
        !isclose(int_exp_riemann_l(f1, 4e-4, 0, 4), 8, 1e-3, 0) ||
        !isclose(int_exp_riemann_r(f1, 4e-4, 0, 4), 8, 1e-3, 0) ||
        !isclose(int_exp_riemann_l(f2, 4e-4, 0, 2), 8./3, 1e-3, 0) ||
        !isclose(int_exp_riemann_r(f2, 4e-4, 0, 2), 8./3, 1e-3, 0) ||
        !isclose(int_exp_riemann_l(f3, 6e-5, 1e-9, 1), -1, 1e-3, 0) ||
        !isclose(int_exp_riemann_r(f3, 6e-5, 1e-9, 1), -1, 1e-3, 0) ||
        !isclose(int_exp_riemann_l(f4, 5e-4, 0, 1), 1.4427, 1e-3, 0) ||
        !isclose(int_exp_riemann_r(f4, 5e-4, 0, 1), 1.4427, 1e-3, 0) ||
    0)
        return 1;

    Mat y = vec(a, 1, 1, 1, 1);
    F64 dx = 1;
    if (!isclose(int_imp_riemann_l(y, dx), vec_len(y)-1, 1e-9, 0))
        return 1;
    S64 n = 30001;
    dx = 4.0 / n;
    y = vec_linspace(0, 4, n, a);
    if (!isclose(int_imp_riemann_l(y, dx), 8, 1e-3, 0))
        return 1;



    if (
        !isclose(int_exp_trapezoid(f0, 1e-3, 0, 2), 2, 1e-9, 0) ||
        !isclose(int_exp_trapezoid(f0, 1e-3, 0, 2), 2, 1e-9, 0) ||
        !isclose(int_exp_trapezoid(f1, 4e-4, 0, 4), 8, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f1, 4e-4, 0, 4), 8, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f2, 4e-4, 0, 2), 8./3, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f2, 4e-4, 0, 2), 8./3, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f3, 6e-5, 1e-9, 1), -1, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f3, 6e-5, 1e-9, 1), -1, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f4, 5e-4, 0, 1), 1.4427, 1e-3, 0) ||
        !isclose(int_exp_trapezoid(f4, 5e-4, 0, 1), 1.4427, 1e-3, 0) ||
    0)
        return 1;

    y = vec(a, 1, 1, 1, 1);
    dx = 1;
    if (!isclose(int_imp_trapezoid(y, dx), vec_len(y)-1, 1e-9, 0))
        return 1;
    n = 20001;
    dx = 4.0 / n;
    y = vec_linspace(0, 4, n, a);
    if (!isclose(int_imp_trapezoid(y, dx), 8, 1e-3, 0))
        return 1;
}
