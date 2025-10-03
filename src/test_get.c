#define assert(expr)\
    (failed = !(expr))

#include "base.h"

static U08 failed = 0;

typedef struct {
    S64 *buf;
    S64  len;
    S64  cap;
} TestArray;

int main(void) {
#define LEN 10
    S64 buf[LEN] = {0};

    TestArray xs = {
        .buf = buf,
        .len = LEN,
        .cap = LEN,
    };
    get(xs, 0);
    if (failed)
        return(1);
    get(xs, 9);
    if (failed)
        return(1);
    get(xs, 10);
    if (!failed)
        return(1);
    get(xs, 0) = 3;

    return(0);
}
