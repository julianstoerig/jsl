#include "base.h"

typedef struct {
    int *buf;
    S64 len;
    S64 cap;
    Arena *a;
} Array;


int main(void) {
    Arena a = {0};
    arena_init(&a, 5 * MiB);
    Array xs = {.a=&a};


    for (int i=0; i<5; ++i)
        *da_push(xs) = i;

    for (int i=0; i<5; ++i)
        if (xs.buf[i] != i) return(1);

    da_clear(xs);
    if (xs.len) return(1);

    for (int i=0; i<256; ++i)
        *da_push(xs) = i;

    da_free(xs);

    return(0);
}

