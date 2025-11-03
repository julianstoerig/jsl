#include "num.h"

F64 f(F64 x) {
    return log(x);
}

int main(void) {
    Arena a = arena_create(MiB);
    Arena *ap = &a;

    F64 *x = 0;
    F64 *y = 0;
    S64 n = 101;
    eval_f_on_range(f, &x, &y, 1e-9, 2, n, ap);
    FILE *f = fopen("test.dat", "wb");
    if (!f)
        return(1);
    write_f64v_to_file(f, x, y, n);
    fclose(f);
}
