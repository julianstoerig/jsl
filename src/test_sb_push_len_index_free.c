#include "base.h"

int main(void) {
    S64 *xs = 0;

    S64 len = 5;
    for (S64 i=0; i<len; ++i)
        sb_push(xs, i*i);

    for (S64 i=0; i<sb_len(xs); ++i) {
        if (xs[i] != i*i)
            return(1);
    }

    sb_clear(xs);
    if (sb_len(xs))
        return(1);
    sb_clear(xs);
    if (sb_len(xs))
        return(1);

    sb_reserve(xs, 5);
    if (sb_len(xs))
        return(1);
    if (sb_cap(xs) < 5 || sb_cap(xs) > 16)
        return(1);

    sb_reserve(xs, 64);
    if (sb_len(xs))
        return(1);
    if (sb_cap(xs) != 64)
        return(1);

    len = (1LL << 25LL) + 471LL;
    for (S64 i=0; i<len; ++i)
        sb_push(xs, i*i);

    for (S64 i=0; i<sb_len(xs); ++i) {
        if (xs[i] != i*i)
            return(1);
    }

    sb_free(xs);

    return(0);
}
