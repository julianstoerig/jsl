#include <stdint.h> // required for the array tests
#include "test.h"
#define CHAINED_ARENA_IMPL
#include "chained_arena.h"

#define BASE_IMPL
#include "base_short.h"

#include "js_readfile.c"

LT_TEST(test_trivial) {
    LT_ASSERT(1);
}

LT_TEST(test_chained_arena_make) {
    chained_arena *a = chained_arena_make(MiB(1));
    LT_ASSERT(a);
    LT_ASSERT(a->base);
    LT_ASSERT(a->capacity == MiB(1));
    LT_ASSERT(a->offset == 0);
    LT_ASSERT(a->next == NULL);

    chained_arena_destroy(a);
}

LT_TEST(test_chained_arena_alloc_m) {
    chained_arena *a = chained_arena_make(MiB(1));
    int *arr = chained_arena_alloc_m(a, int, 500);
    LT_ASSERT(arr);

    chained_arena_destroy(a);
}

LT_TEST(test_chained_arena_alloc_m_can_alloc_new_node) {
    chained_arena *a = chained_arena_make(MiB(1));
    char *arr = chained_arena_alloc_m(a, char, MiB(1));
    char *arr2 = chained_arena_alloc_m(a, char, MiB(1));
    for (int i=0; i<MiB(1); ++i) {
        arr2[i] = 'a';
    }
    LT_ASSERT(arr);
    LT_ASSERT(arr2);
    LT_ASSERT(a->next);

    chained_arena_destroy(a);
}

LT_TEST(test_new_chained_arena_nodes_capacity_can_grow_to_accomodate_large_allocations) {
    chained_arena *a = chained_arena_make(MiB(1));
    char *arr = chained_arena_alloc_m(a, char, MiB(1) + 1);
    LT_ASSERT(arr);
    LT_ASSERT(a->next);
    LT_ASSERT(a->next->capacity > a->capacity);

    chained_arena_destroy(a);
}

LT_TEST(test_chained_arena_reset_does_reset) {
    chained_arena *a = chained_arena_make(MiB(1));
    char *arr1 = chained_arena_alloc_m(a, char, MiB(1));
    char *arr2 = chained_arena_alloc_m(a, char, MiB(1));
    LT_ASSERT(arr1);
    LT_ASSERT(arr2);
    LT_ASSERT(a->next);
    chained_arena_reset(a);
    LT_ASSERT(a->offset == 0);
    LT_ASSERT(a->next->offset == 0);

    chained_arena_destroy(a);
}

LT_TEST(test_chained_arena_get_scratch_returns_live_chained_arena) {
    chained_arena *a = chained_arena_get_scratch();
    LT_ASSERT(a);
}

LT_TEST(test_chained_arena_get_scratch_returns_alternating_chained_arenas_on_consecutive_calls) {
    chained_arena *a = chained_arena_get_scratch();
    chained_arena *b = chained_arena_get_scratch();
    chained_arena *c = chained_arena_get_scratch();
    LT_ASSERT(a);
    LT_ASSERT(b);
    LT_ASSERT(c);
    LT_ASSERT(a != b);
    LT_ASSERT(a == c);
}

LT_TEST(test_arena_init_and_free) {
    arena a = {0};
    int arena_size = 1024;
    arena_init(&a, arena_size);

    LT_ASSERT(a.data);
    LT_ASSERT(a.offset == 0);
    LT_ASSERT(a.count == 1024);

    arena_free(&a);

    LT_ASSERT(!a.data);
    LT_ASSERT(a.offset == 0);
    LT_ASSERT(a.count == 0);
}

LT_TEST(test_arena_push) {
    arena a = {0};
    int arena_size = 1024;
    arena_init(&a, arena_size);

    int *ip = arena_push(&a, int);
    
    LT_ASSERT(ip);
    LT_ASSERT(*ip == 0);

    *ip = 42;

    LT_ASSERT(*ip == 42);

    arena_free(&a);
}

LT_TEST(test_arena_push_many) {
    arena a = {0};
    int arena_size = 1024;
    arena_init(&a, arena_size);

    int array_length = 42;
    int *arrp = arena_push_array(&a, int, array_length);
    
    LT_ASSERT(arrp);

    for (int i=0; i<array_length; ++i) {
        arrp[i] = i*i;
    }

    LT_ASSERT(arrp[0] == 0);
    LT_ASSERT(arrp[9] == 81);
    LT_ASSERT(arrp[41] == 1681);

    arena_free(&a);
}

LT_TEST(test_arena_reset) {
    arena a = {0};
    int arena_size = 1024;
    arena_init(&a, arena_size);

    int *ip = arena_push(&a, int);
    *ip = 42;

    LT_ASSERT(*ip == 42);

    arena_reset(&a);
    int *ip2 = arena_push(&a, int);

    LT_ASSERT(*ip == 0);
    LT_ASSERT(ip == ip2);

    arena_free(&a);
}

typedef struct TestArray {
    int *arr;
    int64_t len;
    int64_t cap;
} TestArray;

LT_TEST(test_array_pushback_and_free) {
    TestArray arr = {0};
    da_pushback(&arr, 42);
    da_pushback(&arr, 13);
    da_free(&arr);
    LT_ASSERT(arr.arr == NULL);
    LT_ASSERT(arr.len == 0);
    LT_ASSERT(arr.cap == 0);
}

LT_TEST(test_array_at_and_popback) {
    TestArray arr = {0};
    da_pushback(&arr, 42);
    da_pushback(&arr, 13);

    LT_ASSERT(da_at(&arr, 0) == 42);
    LT_ASSERT(da_at(&arr, 1) == 13);

    LT_ASSERT(da_popback(&arr) == 13);
    LT_ASSERT(da_popback(&arr) == 42);

    for (int i=0; i<200; ++i) {
        da_pushback(&arr, i);
    }

    for (int i=0; i<200; ++i) {
        LT_ASSERT(da_at(&arr, i) == i);
    }

    da_free(&arr);
}

LT_TEST(test_array_foreach) {
    TestArray arr = {0};

    for (int i=0; i<200; ++i) {
        da_pushback(&arr, i);
    }

    int i=0;
    da_foreach(int, x, &arr) {
        LT_ASSERT(*x == i);
        ++i;
    }

    da_free(&arr);
}

LT_TEST(test_array_clear) {
    TestArray a = {0};

    for (int i=0; i<200; ++i) {
        da_pushback(&a, i);
    }

    int *arr = a.arr;
    int cap = a.cap;

    da_clear(&a);

    LT_ASSERT(a.arr == arr);
    LT_ASSERT(a.len == 0);
    LT_ASSERT(a.cap == cap);

    da_free(&a);
}

LT_TEST(test_scoped_defer) {
    int a = 13;

    scope_start_end(a = 42, a = 0) {
        LT_ASSERT(a == 42);
    }
    LT_ASSERT(a == 0);

    char *buf = NULL;

    scope_start_end((a = 4, buf = malloc(42)), (a = 13, free(buf), buf = NULL)) {
        LT_ASSERT(a == 4);
        sprintf(buf, "Seems ok to me!");
    }
    LT_ASSERT(buf == NULL);
    LT_ASSERT(a == 13);
}

LT_TEST(test_js_read_entire_file_and_null_terminate) {
    char test_buf[] = 
"this is the test file for\n"
"checking, whether or not    the\n"
"base.h functionality for reading in files\n"
"\n"
"    works as expected.";

    char *buf = js_read_entire_file_and_null_terminate("test_file.txt");
    LT_ASSERT(buf);
    LT_ASSERT(!strcmp(buf, test_buf));
    free(buf);
}

LT_TEST_MAIN(
    test_trivial,
    test_chained_arena_make,
    test_chained_arena_alloc_m,
    test_chained_arena_alloc_m_can_alloc_new_node,
    test_new_chained_arena_nodes_capacity_can_grow_to_accomodate_large_allocations,
    test_chained_arena_reset_does_reset,
    test_chained_arena_get_scratch_returns_live_chained_arena,
    test_chained_arena_get_scratch_returns_alternating_chained_arenas_on_consecutive_calls,
    test_arena_init_and_free,
    test_arena_push,
    test_arena_push_many,
    test_arena_reset,
    test_array_pushback_and_free,
    test_array_at_and_popback,
    test_array_foreach,
    test_scoped_defer,
    test_array_clear,
    test_js_read_entire_file_and_null_terminate,
)
