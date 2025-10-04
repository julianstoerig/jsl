#include "base.h"

S64 fib(S64 n) {
    if (n < 0)
        exit(1);
    S64 a = 0;
    S64 b = 1;
    S64 c = a+b;
    if (n == 0)
        return(0);
    for (S64 i=1; i<n; ++i) {
        c = a+b;
        a = b;
        b = c;
    }
    return(c);
}

int main(void) {
    S64 *fibs = 0;
    S64 len = 1 << 14;

    sb_push(fibs, 0);
    sb_push(fibs, 1);
    for (S64 i=2; i<len; ++i) {
        sb_push(fibs, fibs[i-2]+fibs[i-1]);
    }
    for (S64 i=2; i<len; ++i) {
        if (fibs[i] != fib(i))
            return(1);
    }
    return(0);
}
