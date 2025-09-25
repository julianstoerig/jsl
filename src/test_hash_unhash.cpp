#include "base.hpp"

void test(U64 from, U64 to, U64 prime) {
    for (U64 i=from; i<to; i+=prime)
        assert(i == u64_unhash(u64_hash(i)));
}

int main() {
    test(0ULL, 1ULL<<12, 1);
    test(1ULL<<12, 1ULL<<30, 9001);
    test(1ULL<<30, 1ULL<<50, 45212177ULL);
    test(1ULL<<50, 1ULL<<63, 1111111111111111111ULL);
}
