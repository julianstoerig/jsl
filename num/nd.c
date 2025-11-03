#include <stdio.h>

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
    S64 dim;
    S64 *shape;
    S64 *strides;
    F64 *v;
} NDArray;

#define countof(arr) sizeof(arr)/sizeof(*arr)

#define nd_get(self, ...) nd__get((self), (S64[]){__VA_ARGS__}, countof((S64[]){__VA_ARGS__}))
F64 *nd__get(NDArray *self, S64 *indices, S64 len) {
    S64 idx = 0;
    for (S64 i=0; i<len; i+=1) idx += indices[i] * self->strides[i];
    return &self->v[idx];
}

#define nd__header_get(mm) ((MMHeader *)((char *)mm - offsetof(MMHeader, v)))
void nd_free(NDArray *self) {
    free(self->v);
    free(self->shape);
    free(self->strides);
    free(self);
}

S64 nd_cols(NDArray *self) {
    if (self->dim == 0) return 0;
    return self->shape[0];
}

S64 nd_rows(NDArray *self) {
    if (self->dim > 1) return 0;
    return self->shape[1];
}

S64 nd_count(NDArray *self) {
    S64 count = 1;
    for (S64 i=0; i<self->dim; i+=1) count *= self->shape[i];
    return count;
}

NDArray nd__from_parts(S64 dim, S64 *shape, S64 *strides, F64 *buf) {
    NDArray self = {
        .dim = dim,
        .shape = shape,
        .strides = strides,
        .v = buf,
    };
    return self;
}

#define nd_new(shape, strides, buf) nd__new(countof(shape), (shape), (strides), (buf))
NDArray nd__new(S64 dim, S64 *shape, S64 *strides, F64 *buf) {
    NDArray self = {0};
    self.dim = dim;

    self.shape = malloc(sizeof(*shape)*dim);
    if (self.shape == 0) abort();
    memcpy(self.shape, shape, sizeof(*shape)*dim);

    self.strides = malloc(sizeof(*strides)*dim);
    if (self.strides == 0) abort();
    memcpy(self.strides, strides, sizeof(*strides)*dim);

    S64 count = nd_count(&self);
    S64 bufsize = sizeof(*buf)*count;
    self.v = malloc(bufsize);
    if (self.v == 0) abort();
    memcpy(self.v, buf, bufsize);

    return self;
}

NDArray nd_clone(NDArray *other) {
    NDArray self = nd__new(other->dim, other->shape, other->strides, other->v);
    return self;
}

void nd_print(F64 **self) {
    S64 rows = nd_rows(self);
    S64 cols = nd_cols(self);
    for (S64 j=0; j<rows; j+=1) {
        printf("[");
        for (S64 i=0; i<cols; i+=1) {
            if (i > 0) printf(", ");
            printf("%g", self[j][i]);
        }
        printf("]\n");
    }
}

void nd_add(F64 **self, F64 **other) {
    if (nd_rows(self) != nd_rows(other) || nd_cols(self) != nd_cols(other)) abort();
    for (S64 j=0; j<nd_rows(self); j+=1) {
        for (S64 i=0; i<nd_rows(self); i+=1) {
            self[j][i] += other[j][i];
        }
    }
}

void nd_sub(F64 **self, F64 **other) {
    if (nd_rows(self) != nd_rows(other) || nd_cols(self) != nd_cols(other)) abort();
    for (S64 j=0; j<nd_rows(self); j+=1) {
        for (S64 i=0; i<nd_rows(self); i+=1) {
            self[j][i] -= other[j][i];
        }
    }
}

void nd_mul(F64 **self, F64 **other) {
    if (nd_rows(self) != nd_rows(other) || nd_cols(self) != nd_cols(other)) abort();
    for (S64 j=0; j<nd_rows(self); j+=1) {
        for (S64 i=0; i<nd_rows(self); i+=1) {
            self[j][i] *= other[j][i];
        }
    }
}

void nd_div(F64 **self, F64 **other) {
    if (nd_rows(self) != nd_rows(other) || nd_cols(self) != nd_cols(other)) abort();
    for (S64 j=0; j<nd_rows(self); j+=1) {
        for (S64 i=0; i<nd_rows(self); i+=1) {
            self[j][i] /= other[j][i];
        }
    }
}

void nd_scale(F64 **self, F64 lambda) {
    for (S64 j=0; j<nd_rows(self); j+=1) {
        for (S64 i=0; i<nd_rows(self); i+=1) {
            self[j][i] *= lambda;
        }
    }
}

void nd_swap_rows(F64 **self, S64 from, S64 into) {
    for (S64 i=0; i<nd_cols(self); i+=1) {
        F64 swap = self[from][i];
        self[from][i] = self[into][i];
        self[into][i] = swap;
    }
}

void nd_swap_cols(F64 **self, S64 from, S64 into) {
    for (S64 j=0; j<nd_rows(self); j+=1) {
        F64 swap = self[j][from];
        self[j][from] = self[j][into];
        self[j][into] = swap;
    }
}


void gaussj(F64 **a, F64 **b, S64 m, S64 n) {
}

int main(void) {
    F64 **A = nd_new(2, 2);
    S64 j=0; S64 i=0;
    for (j=0;j<2;j+=1) for (i=0;i<2;i+=1) A[j][i] = 0;
    for (i=0;i<2;i+=1) A[i][i] = 1;
    nd_print(A); printf("\n");
    F64 **B = nd_new(2, 2);
    B[0][0] = 1; B[0][1] =  1.5; 
    B[1][0] = 0; B[1][1] = -0.2; 
    nd_print(B); printf("\n");

    F64 **C = nd_new(2, 2);
    nd_add(C, A);
    nd_print(A); printf("\n");
    nd_add(C, A);
    nd_print(A); printf("\n");

    nd_free(A);
    nd_free(B);
    nd_free(C);
}
