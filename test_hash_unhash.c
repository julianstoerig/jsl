#include "base.h"

void test(U64 from, U64 to, U64 prime) {
    for (U64 i=from; i<to; i+=prime)
        assert(i == u64_unhash_bij(u64_hash_bij(i)));
}

int main(void) {
    test(0ULL, 1ULL<<12, 1);
    test(1ULL<<30, 1ULL<<50, 452121773ULL);
    test(1ULL<<50, 1ULL<<63, 1111111111111111111ULL);
}
