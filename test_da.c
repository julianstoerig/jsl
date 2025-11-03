#define BASE_IMPL
#include "base.h"

typedef struct {
    _ArrayHeader_;
    int *v;
} Array;


int main(void) {
    Arena *a = arena_create(MiB);

    Array xs = {0};

    array_push(a, xs, 5);

    if (xs.v[0] != 5) return(1);
    if (array_len(xs) != 1) return(1);

    for (S64 i=0; i<541; ++i)
        array_push(a, xs, i);
    
    for (S64 i=1; i<542; ++i)
        if (xs.v[i] != i-1) return(1);

    array_clear(xs);
    if (array_len(xs) != 0) return(1);

    for (S64 i=0; i<541; ++i)
        array_push(a, xs, i);
    
    int prev = -1;
    for (
        int *it = array_first(xs);
        it     != array_end(xs);
        it      = array_next(xs, it)
    ) {
        if (prev + 1 != *it) return(1);
        prev = *it;
    }

    return(0);
}

