#include "num.h"
#include "tgmath.h"

F64 func(F64 x) {
    return x*x*x + 0.3;
}

typedef struct Test Test;
struct Test {
    F64 (*f)(F64);
    F64   x0;
    F64   act;
    NumResult num;
};

int main(void) {
    F64 xtol = 1e-3;
    S64 max_iter = 100;
    Test roots[] = {
        {func, -1, 0.3389, },
    };

    for (S32 i=0; i<countof(roots); ++i) {
        roots[i].num = fixpoint(roots[i].f, roots[i].x0, xtol, max_iter);
    }

    for (S32 i=0; i<countof(roots); ++i) {
        if (roots[i].num.xtol > xtol)
            return 1;
    }
    return 0;
}
