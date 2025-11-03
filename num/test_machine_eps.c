#define BASE_IMPL
#include "base.h"


int main(void) {
    F32 heps32 = 1.0f;
    while (1.0f+heps32 != 1.0f)
        heps32 /= 2.0f;
    if (2.0f*heps32 != f32_machine_eps())
        return(1);

    F64 heps64 = 1.0;
    while (1.0+heps64 != 1.0)
        heps64 /= 2.0;
    if (2.0*heps64 != f64_machine_eps())
        return(1);
}
