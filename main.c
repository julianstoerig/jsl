#include <stddef.h>
#include <stdint.h>
#include <string.h>
#define JSL_MALLOC_CHECKED_IMPLEMENTATION
#include "malloc_checked.h"
#include "meta_macros.h"
#include "mathy_macros.h"
#include "assert.h"
#include "bit_fiddling.h"
#include "typedefs.h"
#define JSL_ARENA_IMPLEMENTATION
#include "arena.h"


void testScratchArena1(Arena *a, Arena scratch) {
    int* n = new(&scratch, int);
    int* n2 = new(a, int);
    
    *n = 3;
    *n2 = *n;
}

void testScratchArena2(Arena a) {
    int *n = new(&a, int, 500);
    for(ptrdiff_t i=0; i<500; ++i) *(n+i) = 41;
}

int main () {
    ASSERT(ODD(1));
    ASSERT(ODD(-1));
    ASSERT(!ODD(2));
    ASSERT(!ODD(-2));
    ASSERT(!EVEN(1));
    ASSERT(!EVEN(-1));
    ASSERT(EVEN(2));
    ASSERT(EVEN(-2));

    puts("[SUCCESS] bool\n");

    ASSERT(NEG(5)==-5);
    ASSERT(NEG(-3)==3);

    puts("[SUCCESS] negation\n");

    ASSERT(SQR(2)==4);
    ASSERT(SQR(0.5)-0.25 < 0.1);

    puts("[SUCCESS] square function\n");

    ASSERT(PI - 3.1415 < 0.1);
    ASSERT(E - 2.78 < 0.1);    

    puts("[SUCCESS] mathematical constants\n");

    ASSERT(RAD2DEG(PI)-180 < 0.1);
    ASSERT(DEG2RAD(360)-2*PI < 0.1);
    ASSERT(DEG2RAD(RAD2DEG(PI))-PI<0.1);

    puts("[SUCCESS] angle conversion funcions\n");

    ASSERT(SIGN(5)==1);
    ASSERT(SIGN(-5)==-1);
    ASSERT(SIGN(0)==0);

    ASSERT(SIGN(-2)==-1);    
    ASSERT(SIGN(0)==0);    
    ASSERT(SIGN(2)==1);    

    ASSERT(COMPARE(1,3)==-1);    
    ASSERT(COMPARE(3,3)==0);    
    ASSERT(COMPARE(5,3)==1);    

    puts("[SUCCESS] comparison functions\n");

    ASSERT(MIN(5,9)==5);
    ASSERT(MIN(5,5)==5);
    ASSERT(MAX(5,5)==5);
    ASSERT(MAX(5,9)==9);

    ASSERT(CLAMP(14, 1, 2) == 2);
    ASSERT(CLAMP(1, 1, 2) == 1);
    ASSERT(CLAMP(2, 1, 2) == 2);

    ASSERT(DELTA(6, 5) == 1);
    ASSERT(DELTA(5, 6) == 1);

    puts("[SUCCESS] min max and clamping functions\n");

    {
        int a = 1;
        int b = 3;
        SWAP(a,b, int);
        ASSERT(a==3);
        ASSERT(b==1);
    }

    ASSERT(IN_RANGE_INCL(5, 4, 7));
    ASSERT(IN_RANGE_INCL(4, 4, 7));
    ASSERT(IN_RANGE_INCL(7, 4, 7));
    ASSERT(!IN_RANGE_INCL(3, 4, 7));

    ASSERT(IN_RANGE_UPPER_INCL(5, 4, 7));
    ASSERT(!IN_RANGE_UPPER_INCL(4, 4, 7));
    ASSERT(IN_RANGE_UPPER_INCL(7, 4, 7));
    ASSERT(!IN_RANGE_UPPER_INCL(3, 4, 7));

    ASSERT(IN_RANGE_LOWER_INCL(5, 4, 7));
    ASSERT(IN_RANGE_LOWER_INCL(4, 4, 7));
    ASSERT(!IN_RANGE_LOWER_INCL(7, 4, 7));
    ASSERT(!IN_RANGE_LOWER_INCL(3, 4, 7));

    ASSERT(IN_RANGE_EXCL(5, 4, 7));
    ASSERT(!IN_RANGE_EXCL(4, 4, 7));
    ASSERT(!IN_RANGE_EXCL(7, 4, 7));
    ASSERT(!IN_RANGE_EXCL(3, 4, 7));

    puts("[SUCCESS] ranges\n");

    ASSERT(IMPLIES(true, true));
    ASSERT(!IMPLIES(true, false));
    ASSERT(IMPLIES(false, true));
    ASSERT(IMPLIES(false, false));

    ASSERT(IFF(true, true));
    ASSERT(!IFF(true, false));
    ASSERT(!IFF(false, true));
    ASSERT(IFF(false, false));

    puts("[SUCCESS] boolean algebra\n");

    fflush(stdout);
    ASSERT(!strcmp(TO_STRING(hello), "hello"));

    int arr[5] = {0};
    int *checkedMallocArray = malloc_checked(sizeof(int) * 100);
    puts("[SUCCESSS] checkedMalloc works\n");
    for (size_t i = 0; i < sizeof(arr) / sizeof(arr[0]); i++) {
        arr[i] = 2*i;
    }    
    free_checked(checkedMallocArray);

    puts("[SUCCESSS] checkedFree works\n");

    Arena arena = ArenaCreate(PTRDIFF_MAX);
    ptrdiff_t count = 9999;
    int* arena_int_as = new(&arena, int, count);
    for (ptrdiff_t i=0; i<count; ++i) {
        int stack_int_a = arena_int_as[i];
        fflush(stdout);
        ASSERT(stack_int_a || true);
    }

    Arena arenaScratch = ArenaCreate(sizeof(int));
    Arena arenaCopy = arena;
    Arena arenaScratchCopy = arenaScratch;
    testScratchArena1(&arena, arenaScratch);
    fflush(stdout);
    ASSERT(arena.beg != arenaCopy.beg);
    fflush(stdout);
    ASSERT(arenaScratch.beg != arenaScratchCopy.beg);

    int *arena_int_bs = new(&arena, int, 99, ArenaFlags_NoAbort | ArenaFlags_NoZero);
    fflush(stdout);
    ASSERT(arena_int_bs);
    arena_int_bs[75] = 42;
    fflush(stdout);
    ASSERT(arena_int_bs[75] == 42);
    testScratchArena2(arena);
    fflush(stdout);
    ASSERT(arena_int_bs[75] == 42);
        
    puts("[SUCCESS] arena works\n");

    puts("[SUCCESS] all tests passed fine.\n\n");

    fflush(stdout);
}
