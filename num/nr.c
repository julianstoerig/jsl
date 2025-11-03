#include <stdio.h>

#define BASE_IMPL
#include "/home/julian/jd/30-39-hobbies/32-programming/32.022-jsl/base.h"
#include <tgmath.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <complex.h>

typedef uint8_t        U08;
typedef int64_t        S64;
typedef double         F64;
typedef float          F32;
typedef float complex  C32;
typedef double complex C64;

typedef struct {
    S64 rows;
    S64 cols;
    F64 *v[];
} MMHeader;

#define mm__header_get(mm) ((MMHeader *)((char *)mm - offsetof(MMHeader, v)))

S64 mm_cols(F64 **self) {
    return mm__header_get(self)->cols;
}

S64 mm_rows(F64 **self) {
    return mm__header_get(self)->rows;
}

S64 mm_count(F64 **self) {
    MMHeader m = *mm__header_get(self);
    return m.rows*m.cols;
}

F64 **mm_new(int64_t rows, int64_t cols, Arena *ap) {
    int64_t size = offsetof(MMHeader, v) + rows*sizeof(void*);

    F64 *m = make(ap, F64, rows*cols);
    MMHeader *hdr = (MMHeader*)make(ap, U08, size); // have to cast since make
                                                    // implicitly casts already
    for (S64 i=0; i<rows; i+=1) {
        hdr->v[i] = m + rows*i;
    }

    hdr->rows = rows;
    hdr->cols = cols;

    return hdr->v;
}

F64 **mm_zeroes(S64 rows, S64 cols, Arena *ap) {
    F64 **self = mm_new(rows, cols, ap);
    memset(*self, 0x00, rows*cols*sizeof(**self));
    return self;
}

F64 **mm_ident(S64 rows, S64 cols, Arena *ap) {
    F64 **self = mm_zeroes(rows, cols, ap);
    S64 min_dim = rows; if (cols < min_dim) min_dim = cols;
    for (S64 i=0; i<min_dim; i+=1) {
        self[i][i] = 1;
    }
    return self;
}

F64 **mm_uniform(S64 rows, S64 cols, F64 x, Arena *ap) {
    F64 **self = mm_new(rows, cols, ap);
    for (S64 i=0; i<mm_rows(self); i+=1) {
        for (S64 j=0; j<mm_cols(self); j+=1) {
            self[i][j] = x;
        }
    }
    return self;
}

void mm_print(F64 **self) {
    S64 rows = mm_rows(self);
    S64 cols = mm_cols(self);
    for (S64 i=0; i<rows; i+=1) {
        printf("[");
        for (S64 j=0; j<cols; j+=1) {
            if (j > 0) printf(", ");
            printf("%g", self[i][j]);
        }
        printf("]\n");
    }
}

F64 **mm_add(F64 **lhs, F64 **rhs, Arena *ap) {
    if (mm_rows(lhs) != mm_rows(rhs) || mm_cols(lhs) != mm_cols(rhs)) assert(0), abort();
    F64 **res = mm_new(mm_rows(lhs), mm_cols(lhs), ap);

    for (S64 i=0; i<mm_rows(lhs); i+=1) {
        for (S64 j=0; j<mm_rows(lhs); j+=1) {
            res[i][j] = lhs[i][j] + rhs[i][j];
        }
    }
    return res;
}

F64 **mm_sub(F64 **lhs, F64 **rhs, Arena *ap) {
    if (mm_rows(lhs) != mm_rows(rhs) || mm_cols(lhs) != mm_cols(rhs)) assert(0), abort();
    F64 **res = mm_new(mm_rows(lhs), mm_cols(lhs), ap);

    for (S64 i=0; i<mm_rows(lhs); i+=1) {
        for (S64 j=0; j<mm_rows(lhs); j+=1) {
            res[i][j] = lhs[i][j] - rhs[i][j];
        }
    }
    return res;
}

F64 **mm_mul(F64 **lhs, F64 **rhs, Arena *ap) {
    if (mm_rows(lhs) != mm_rows(rhs) || mm_cols(lhs) != mm_cols(rhs)) assert(0), abort();
    F64 **res = mm_new(mm_rows(lhs), mm_cols(lhs), ap);

    for (S64 i=0; i<mm_rows(lhs); i+=1) {
        for (S64 j=0; j<mm_rows(lhs); j+=1) {
            res[i][j] = lhs[i][j] * rhs[i][j];
        }
    }
    return res;
}

F64 **mm_div(F64 **lhs, F64 **rhs, Arena *ap) {
    if (mm_rows(lhs) != mm_rows(rhs) || mm_cols(lhs) != mm_cols(rhs)) assert(0), abort();
    F64 **res = mm_new(mm_rows(lhs), mm_cols(lhs), ap);

    for (S64 i=0; i<mm_rows(lhs); i+=1) {
        for (S64 j=0; j<mm_rows(lhs); j+=1) {
            res[i][j] = lhs[i][j] / rhs[i][j];
        }
    }
    return res;
}

F64 **mm_scale(F64 **lhs, F64 lambda, Arena *ap) {
    F64 **res = mm_new(mm_rows(lhs), mm_cols(lhs), ap);

    for (S64 i=0; i<mm_rows(lhs); i+=1) {
        for (S64 j=0; j<mm_rows(lhs); j+=1) {
            res[i][j] = lhs[i][j] * lambda;
        }
    }
    return res;
}

F64 **mm_round(F64 **lhs, S64 decimals, Arena *ap) {
    F64 **res = mm_new(mm_rows(lhs), mm_cols(lhs), ap);

    for (S64 i=0; i<mm_rows(lhs); i+=1) {
        for (S64 j=0; j<mm_cols(lhs); j+=1) {
            F64 factor = pow(10, decimals);
            res[i][j] = round(lhs[i][j] * factor) / factor;
        }
    }
    return res;
}

F64 **mm_matmul(F64 **lhs, F64 **rhs, Arena *ap) {
    if (mm_cols(lhs) != mm_rows(rhs)) abort();
    F64 **self = mm_zeroes(mm_rows(lhs), mm_cols(rhs), ap);
    for (S64 i=0; i<mm_rows(self); i+=1) {
        for (S64 j=0; j<mm_cols(self); j+=1) {
            for (S64 k=0; k<mm_cols(lhs); k+=1) {
                self[i][j] += lhs[i][k]*rhs[k][j];
            }
        }
    }
    return self;
}

void mm_swap_rows(F64 **self, S64 from, S64 into) {
    for (S64 i=0; i<mm_cols(self); i+=1) {
        F64 swap = self[from][i];
        self[from][i] = self[into][i];
        self[into][i] = swap;
    }
}

void mm_swap_cols(F64 **self, S64 from, S64 into) {
    for (S64 j=0; j<mm_rows(self); j+=1) {
        F64 swap = self[j][from];
        self[j][from] = self[j][into];
        self[j][into] = swap;
    }
}

void gaussj(F64 **a, F64 **b, S64 m, S64 n) {
}

int main(void) {
    Arena a = arena_create(10 * MiB);
    Arena *ap = &a;

    F64 **A = mm_uniform(2, 2, 3.0, ap);
    F64 **B = mm_ident(2, 2, ap);
    F64 **C = mm_add(A, B, ap);

    mm_print(A); printf("\n");
    mm_print(B); printf("\n");
    mm_print(C); printf("\n");

    F64 **D = mm_matmul(mm_ident(2, 2, ap), mm_ident(2, 2, ap), ap);
    mm_print(D); printf("\n");

    D = mm_matmul(mm_scale(mm_ident(2, 2, ap), 42, ap), mm_ident(2, 2, ap), ap);
    mm_print(D); printf("\n");

    arena_free(ap);
}
