#ifndef BASE_H
#define BASE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// type definitions

typedef   int8_t     i8;
typedef  int16_t    i16;
typedef  int32_t    i32;
typedef  int64_t    i64;
typedef  uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

typedef  uint8_t  bool8;
typedef uint32_t bool32;

// find context

#if defined(_MSC_VER)
# define TRAP() __debugbreak()
#elif defined(__clang) || defined(__GNUC__) || defined(__GNUG__)
# define TRAP() __builtin_trap()
#else
#error Unknown trap intrinsic for this compiler.
#endif

#define CONCATENATE(a, b) a##b

// assert macro

#define ASSERT(expr)\
    (\
        (expr) ||\
        fprintf(\
            stderr,\
            "[ERROR] assertion Failure: `%s` in %s:%d\n",\
            #expr, __FILE__, __LINE__) ||\
        (TRAP(), 0)\
    )

#define EXPECT(type, msg)\
    (\
        fprintf(\
            stderr,\
            "[%s] %s in %s:%d\n",\
            type, msg, __FILE__, __LINE__) ||\
        (TRAP(), 0)\
    )

#define TODO(msg)\
    EXPECT("TODO", msg)

#define UNREACHABLE(msg)\
    EXPECT("UNREACHABLE", "msg")


#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define ALIGNOF sizeof
#elif __STDC_VERSION__ < 202311L
#define ALIGNOF _Alignof
#else
#define ALIGNOF alignof
#endif // __STDC_VERSION__

#define MALLOC malloc

#define FREE free

#define MEMSET memset

#define ALLOC_FAILURE ASSERT

#define OFFSET_OF(strukt, member)\
    (&(((srukt *)(NULL))->member))

// scoped defer

#define scope_start_end_2(start, end, count)\
    for (int CONCATENATE(scope_start_end_i_, count) = (start, 1);\
    CONCATENATE(scope_start_end_i_, count);\
    CONCATENATE(scope_start_end_i_, count)--, end)

#define scope_start_end_1(start, end, count)\
    scope_start_end_2(start, end, count)

#define scope_start_end(start, end)\
    scope_start_end_1((start), (end), __COUNTER__)

// arena API

typedef struct arena {
    u8 *data;
    i64 count;
    i64 offset;
} arena;

int arena_init(arena *a, i64 size_in_bytes);
void arena_free(arena *a);
void arena_reset(arena *a);
#define arena_push(a, type)\
    arena_push_raw(a, sizeof(type), 1, ALIGNOF(type))
#define arena_push_array(a, type, count)\
    arena_push_raw(a, sizeof(type), count, ALIGNOF(type))
void *arena_push_raw(arena *a, i64 sizeof_type, i64 count, i64 alignof_type);

// array API

// Any type that has the following fields,
// 1. arr, of type "pointer to element type"
//      the pointer to the array. may be realloc'd
// 2. len, of some int type
//      the number of elements currently in the array
// 2. cap, of some int type
//      the number of element that fit in the backing buffer arr
// is considered an array. These structures can be intrusive.


int _da_check_invariants(void *arr, i64 len, i64 cap);

#ifndef NDEBUG
#define da_check_invariants(xs)\
    _da_check_invariants((xs)->arr, (xs)->len, (xs)->cap)
#else // NDEBUG
#define da_check_invariants(xs)
#endif // NDEBUG

#define da_at(xs, i)\
    (\
        da_check_invariants(xs),\
        ASSERT(0 <= (i) && (i) < (xs)->len),\
        (xs)->arr[(i)]\
    )

#define da_pushback_unless_full(xs, x)\
    (\
        da_check_invariants((xs)),\
        ((xs)->len >= (xs)->cap) ?\
            1 :\
            (xs)->arr[(xs)->len++], 0\
    )

#define da_pushback(xs, x)\
    do {\
        da_check_invariants(xs);\
        if ((xs)->len >= (xs)->cap) {\
            if (!(xs)->cap) {\
                (xs)->cap = 16;\
            } else {\
                (xs)->cap *= 2;\
            }\
            void *newptr = realloc((xs)->arr, sizeof(*(xs)->arr)*(xs)->cap);\
            if (!newptr) {\
                ALLOC_FAILURE(newptr);\
                free((xs)->arr);\
            } else {\
                (xs)->arr = newptr;\
            }\
        }\
        (xs)->arr[(xs)->len++] = x;\
    } while (0)

#define da_popback(xs)\
    (\
        da_check_invariants(xs),\
        ASSERT((xs)->len > 0),\
        (xs)->arr[--(xs)->len]\
    )

#define da_ensure_cap(xs, size)\
    do {\
        da_check_invariants(xs);\
        if ((xs)->cap < size) {\
            (xs)->cap = size;\
            void *newptr = realloc((xs)->arr, sizeof(*(xs)->arr)*(xs)->cap);\
            if (!newptr) {\
                ALLOC_FAILURE(newptr);\
                free((xs)->arr);\
            } else {\
                (xs)->arr = newptr;\
                memset((xs)->arr, 0x00, (xs)->cap);\
            }\
        }\
    } while (0)

#define da_clear(xs)\
    do {\
        da_check_invariants(xs);\
        (xs)->len = 0;\
    } while (0)

#define da_free(xs)\
    do {\
        da_check_invariants(xs);\
        free((xs)->arr);\
        (xs)->arr = NULL;\
        (xs)->len = 0;\
        (xs)->cap = 0;\
    } while (0)

#define da_foreach(type, x, xs)\
    for (type *x= ((xs)->len > 0 ? (xs)->arr : NULL); x<(xs)->arr+(xs)->len; ++x)

// string API

typedef struct string_builder {
    u8 *arr;
    i32 len;
    i32 cap;
} string_builder;

typedef struct string {
    u8 *arr;
    i32 len;
} string;

#define S_FMT "%.*s"

#define S_ARG(s) (s).len, (s).arr

string sb_finalise(string_builder sb);
int str_cmp(string a, string b);
i32 str_index_of(string s, u8 c);
string str_concat(string a, string b);

#ifdef BASE_IMPL

// dynamic array implementation

int _da_check_invariants(void *arr, i64 len, i64 cap) {
    if (arr) {
        ASSERT(cap > 0);
        ASSERT(len >= 0);
        ASSERT(len <= cap);
    } else {
        ASSERT(cap == 0);
        ASSERT(len == 0);
    }
    return 0;
}

// string implementation

string sb_finalise(string_builder sb) {
    string s = {0};
    s.arr = sb.arr;
    s.len = sb.len;
    return s;
}

int str_cmp(string a, string b) {
    int min_len = a.len;
    if (b.len < min_len)
        min_len = b.len;
    return memcmp(a.arr, b.arr, min_len);
}

i32 str_index_of(string s, u8 c) {
    for (i32 i=0; i<s.len; ++i) {
        if (s.arr[i] == c) {
            return i;
        }
    }
    return -1;
}

string str_concat(string a, string b) {
    string s = {0};
    s.len = a.len + b.len;
    s.arr = malloc(s.len);
    memcpy(s.arr, a.arr, a.len);
    memcpy(s.arr+a.len, b.arr, b.len);
    return s;
}

// arena implementation

int arena_init(arena *a, i64 size_in_bytes) {
    ASSERT(a); 

    u8 *p = MALLOC(size_in_bytes);
    if (!p) {
        return 1;
    }

    a->data = p;
    a->count = size_in_bytes;
    a->offset = 0;

    return 0;
}

void arena_free(arena *a) {
    ASSERT(a);

    FREE(a->data);
    a->data = NULL;
    a->count = 0;
    a->offset = 0;
}

void arena_reset(arena *a) {
    ASSERT(a);

    a->offset = 0;
}

void *arena_push_raw(arena *a, i64 sizeof_type, i64 count, i64 alignof_type) {
    ASSERT(a);
    ASSERT(a->data);
    // alignof_type must be a power of two
    ASSERT((alignof_type > 0) && !(((uintptr_t)alignof_type) & ((uintptr_t)alignof_type-1)));
    ASSERT(sizeof_type > 0);
    ASSERT(count >= 0);

    if (!count) return NULL;

    i64 total_size = sizeof_type * count;

    ASSERT((total_size >= sizeof_type) && (total_size >= count));

    uintptr_t padding = ((uintptr_t)a->data + (uintptr_t)a->offset) % (uintptr_t)alignof_type;
    a->offset += padding;
    void *data_ptr = (void *)((uintptr_t)a->data + (uintptr_t)a->offset);
    memset(data_ptr, 0x00, total_size);
    a->offset += total_size;

    return data_ptr;
}

#endif // BASE_IMPL

#endif // BASE_H
