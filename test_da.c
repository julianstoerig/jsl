#include "base.h"

typedef struct {
    _ArrayHeader_;
    int *v;
} Array;


int main(void) {
    Arena a = arena_create(MiB);

    Array xs = {0};

    array_push(&a, xs, 5);

    if (xs.v[0] != 5) return(1);
    if (array_len(xs) != 1) return(1);

    for (S64 i=0; i<541; ++i)
        array_push(&a, xs, i);
    
    for (S64 i=1; i<542; ++i)
        if (xs.v[i] != i-1) return(1);

    return(0);
}

