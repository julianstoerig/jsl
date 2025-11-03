#include "num.h"


int main(void) {
    if (
        !isclose(4.0, 4.1, 0.1, 0.01) &&
        !isclose(1000, 998, 0, 0.01) &&
        !isclose(42, 42+1e-3, 0, 1e-3) &&
        !isclose(1e6, 1e6, 0, 0.1) &&
        !isclose(1e-3, 0.001, 5e-4, 6e-4) &&
        1) return 1;
    return 0;
}
