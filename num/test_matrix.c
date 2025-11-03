#include "num.h"


int main(void) {
    Arena *a = arena_create(500 * MiB);

    F64 eps = 1e-14;

    Mat A = mat_empty(1, 1, a);
    A.v[0] = 1;
    Mat B = mat_empty(1, 1, a);
    B.v[0] = 1;
    Mat C = mat_add(A, B, a);
    if (!isclose(*C.v, 2, 1e-4, 1e-4)) return 1;

    Mat E = vec_row(a, 0);
    Mat F = mat_mul(A, E, a);
    if (!mat_equal(E, F, eps, eps)) return 1;

    mat_scale_ip(E, 42);
    if (!mat_equal(E, F, eps, eps)) return 1;

    Mat Id = mat_empty(2, 2, a);
    at2(Id, 0, 0) = 1; at2(Id, 0, 1) = 0;
    at2(Id, 1, 0) = 0; at2(Id, 1, 1) = 1;
    Mat D = mat_ident(2, 2, a);
    if (!mat_equal(Id, D, eps, eps)) return 1;

    Mat x = vec_col(a, 1, 2, 3);
    Mat y = vec_col(a, 1, 0, 2);
    F64 scalar_prod = mat_scalar_prod(x, y);
    if (!isclose(7, scalar_prod, eps, eps)) return 1;

    Mat v = vec(a,
        1,
        0
    );
    Mat u = vec(a,
        0,
        1
    );
    F64 phi = pi / 2;
    Mat rot = mat(a, 2, 2,
        cos(phi), -sin(phi),
        sin(phi),  cos(phi),
    );
    Mat res = mat_matmul(rot, v, a);

    Mat M = mat(a, 3, 3,
        1, 0, 0,   
        0, 3, 5,
        2, 7, 0,
    );

    Mat K = mat(a, 2, 2,
        1, 1, 
        1, 1,
    );
    Mat L = mat(a, 2, 1,
         0,
        42,
    );
    Mat N = mat(a, 2, 3,
        1, 1,  0,
        1, 1, 42,
    );

    if (!mat_equal(mat_augment(K, L, a), N, eps, eps)) return 1;

    Mat O = mat(a, 2, 2,
        0.5, 2,
        3,   1,
    );
    Mat q = vec(a,
        3,
        7,
    );
    Mat P = mat_gauss(O, q, a);
    Mat Pp = vec(a,
        2,
        1,
    );
    if (!mat_equal(P, Pp, eps, eps)) return 1;

    O = mat(a, 2, 2,
        1, 1+1e-10,
        1, 1,
    );
    q = vec(a,
        2+1e-10,
        2,
    );
    P = mat_gauss(O, q, a);
    Pp = vec(a,
        1,
        1,
    );
    if (!mat_equal(P, Pp, eps, eps)) return 1;

    P =  mat(a, 2, 2,
        3, 0,
        0, 2,
    );
    F64 norm = mat_norm_inf_rowwise(P);
    if (!isclose(norm, 3.0, eps, eps)) return 1;
    norm = mat_norm_inf_colwise(P);
    if (!isclose(norm, 3.0, eps, eps)) return 1;

    P = mat(a, 2, 2,
        1, 1,
        0, 1,
    );
    Mat Q = mat(a, 2, 2,
        1, 0,
        1, 1,
    );
    mat_transpose_square_ip(P);
    if (!mat_equal(P, Q, eps, eps)) return 1;

    S64 length = 100;
    Mat z = mat_uniform(length, length, 42, a);
    Mat w = mat_matmul(z, z, a);
    mat_print(w);

    mat_print(P, 3);
    mat_fprint(stdout, P);
    mat_fprint(stdout, P, 9);

    arena_free(a);
}
