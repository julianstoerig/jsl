#include "num.h"

/*

F64 int_exp_riemann_l(F64 f(F64), F64 dx, F64 xmin, F64 xmax) {
    F64 res = 0;
    F64 x = xmin;
    for (S64 i=0; x<xmax; i+=1) {
        x = xmin + i*dx;
        res += f(x) * dx;
    }
    return res;
}

 * */

F64 int_exp_riemann_l(F64 f(F64), F64 dx, F64 xmin, F64 xmax) {
    F64 res = 0;
    xmax -= dx; // left riemann sum means xmax is considered at x=xmax-dx
    for (F64 x=xmin; x<xmax; x+=dx) {
        res += f(x) * dx;
    }
    return res;
}

F64 int_exp_riemann_r(F64 f(F64), F64 dx, F64 xmin, F64 xmax) {
    F64 res = 0;
    xmin += dx; // right riemann sum means xmin is considered at x=xmin+dx
    for (F64 x=xmin; x<xmax; x+=dx) {
        res += f(x) * dx;
    }
    return res;
}

F64 int_imp_riemann_l(Mat y, F64 dx) {
    F64 res = 0;
    S64 y_len = mat_count(y);
    for (S64 i=0; i<y_len-1; i+=1) {
            res += at1(y, i) * dx;
    }
    return res;
}

F64 int_exp_trapezoid(F64 f(F64), F64 dx, F64 xmin, F64 xmax) {
    F64 res = 0;
    for (F64 x=xmin; x<xmax-dx; x+=dx) {
        res += (f(x)+f(x+dx))/2 * dx;
    }
    return res;
}

F64 int_imp_trapezoid(Mat y, F64 dx) {
    F64 res = 0;
    S64 y_len = mat_count(y);
    for (S64 i=0; i<y_len-1; i+=1) {
        res += (at1(y, i) + at1(y, i+1)) / 2 * dx;
    }
    return res;
}

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
    S64 n = 100001;
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
    n = 100001;
    dx = 4.0 / n;
    y = vec_linspace(0, 4, n, a);
    if (!isclose(int_imp_trapezoid(y, dx), 8, 1e-3, 0))
        return 1;
}
