#define BASE_IMPL
#include "base.h"


int main(void) {
    Arena a = {0};
    arena_init(&a, 5*MiB);
    S64 len = (1 << 12) + 541;
    S64 *xs = make(&a, S64, len);
    for (S64 i=0; i<len; ++i)
        xs[i] = i*i;
    for (S64 i=0; i<len; ++i) {
        if (xs[i] != i*i)
            return(1);
    }
    return(0);
}
