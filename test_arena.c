#include "base.h"


int main(void) {
    Arena a = {0};
    S64 len = (1 << 22) + 541;
    S64 *xs = make(&a, S64, len);
    for (S64 i=0; i<len; ++i)
        xs[i] = i*i;
    for (S64 i=0; i<len; ++i) {
        if (xs[i] != i*i)
            return(1);
    }
    return(0);
}
