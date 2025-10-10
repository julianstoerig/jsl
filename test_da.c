#include "base.h"

typedef struct {
    __DYNAMIC_ARRAY_HEADER__(int)
} Array;


int main(void) {
    Array xs = {0};

    for (int i=0; i<5; ++i)
        *da_push(xs) = i;

    for (int i=0; i<5; ++i)
        if (xs.buf[i] != i) return(1);

    da_clear(xs);
    if (xs.len) return(1);

    da_free(xs);

    return(0);
}

