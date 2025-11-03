#include "num.h"
#include "tgmath.h"

F64 func0(F64 x) {
    F64 y = x - 1.0;
    return y;
}

F64 dfunc0(F64 x) {
    F64 y = 1.0;
    return y;
}

F64 func1(F64 x) {
    F64 y = x*x - 1;
    return y;
}

F64 dfunc1(F64 x) {
    F64 y = 2*x;
    return y;
}

F64 func2(F64 x) {
    F64 y = log(x);
    return y;
}

F64 dfunc2(F64 x) {
    F64 y = 1/x;
    return y;
}

F64 func3(F64 x) {
    F64 y = (x-5)*(x+11);
    return y;
}

F64 dfunc3(F64 x) {
    F64 y = 2*(x+3);
    return y;
}

F64 func4(F64 x) {
    F64 y = (x-1)*(x-1)*(x-1);
    return y;
}

F64 dfunc4(F64 x) {
    F64 y = (x-1);
    y = y*y;
    y = 3*y;
    return y;
}

typedef struct Test Test;
struct Test {
    F64 (*f)(F64);
    F64 (*df)(F64);
    F64   min;
    F64   max;
    F64   act;
    NumResult num;
};

int main(void) {
    F64 xtol = 1e-3;
    S64 max_iter = 100;
    Test roots[] = {
        {func0, dfunc0, -5,    5,    1, },
        {func1, dfunc1, -5,    1.5, -1, },
        {func2, dfunc2,  0.1, 10,    1, },
        {func3, dfunc3, -50,   0,  -11, },
        {func3, dfunc3,   0,  50,    5, },
        {func4, dfunc4, -5,    5,   +1, },
    };

    for (S32 i=0; i<countof(roots); ++i)
        roots[i].num = newton(roots[i].f, roots[i].df, roots[i].min, xtol, max_iter);

    for (S32 i=0; i<countof(roots); ++i)
        if (roots[i].num.xtol > xtol)
            return 1;
    return 0;
}
