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
# define js_TRAP() __debugbreak()
#elif defined(__clang) || defined(__GNUC__) || defined(__GNUG__)
# define js_TRAP() __builtin_trap()
#else
#error Unknown trap intrinsic for this compiler.
#endif

#define js_CONCATENATE(a, b) a##b

// assert macro

#define js_ASSERT(expr)\
    (\
        (expr) ||\
        fprintf(\
            stderr,\
            "[ERROR] assertion Failure: `%s` in %s:%d\n",\
            #expr, __FILE__, __LINE__) ||\
        (js_TRAP(), 0)\
    )

#define js_EXPECT(type, msg)\
    (\
        fprintf(\
            stderr,\
            "[%s] %s in %s:%d\n",\
            type, msg, __FILE__, __LINE__) ||\
        (js_TRAP(), 0)\
    )

#define js_TODO(msg)\
    js_EXPECT("TODO", msg)

#define js_UNREACHABLE(msg)\
    js_EXPECT("UNREACHABLE", "msg")


#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define js_ALIGNOF sizeof
#elif __STDC_VERSION__ < 202311L
#define js_ALIGNOF _Alignof
#else
#define js_ALIGNOF alignof
#endif // __STDC_VERSION__

#define js_MALLOC malloc

#define js_FREE free

#define js_MEMSET memset

#define js_ALLOC_FAILURE js_ASSERT

#define js_OFFSET_OF(strukt, member)\
    (&(((srukt *)(NULL))->member))

// scoped defer

#define js_scope_start_end_2(start, end, count)\
    for (int js_CONCATENATE(js_scope_start_end_i_, count) = (start, 1);\
    js_CONCATENATE(js_scope_start_end_i_, count);\
    js_CONCATENATE(js_scope_start_end_i_, count)--, end)

#define js_scope_start_end_1(start, end, count)\
    js_scope_start_end_2(start, end, count)

#define js_scope_start_end(start, end)\
    js_scope_start_end_1((start), (end), __COUNTER__)

// arena API

typedef struct js_arena {
    u8 *data;
    i64 count;
    i64 offset;
} js_arena;

int js_arena_init(js_arena *a, i64 size_in_bytes);
void js_arena_free(js_arena *a);
void js_arena_reset(js_arena *a);
#define js_arena_push(a, type)\
    js_arena_push_raw(a, sizeof(type), 1, js_ALIGNOF(type))
#define js_arena_push_array(a, type, count)\
    js_arena_push_raw(a, sizeof(type), count, js_ALIGNOF(type))
void *js_arena_push_raw(js_arena *a, i64 sizeof_type, i64 count, i64 alignof_type);

// array API

// Any type that has the following fields,
// 1. arr, of type "pointer to element type"
//      the pointer to the array. may be realloc'd
// 2. len, of some int type
//      the number of elements currently in the array
// 2. cap, of some int type
//      the number of element that fit in the backing buffer arr
// is considered an array. These structures can be intrusive.


int js__da_check_invariants(void *arr, i64 len, i64 cap);

#ifndef NDEBUG
#define js_da_check_invariants(xs)\
    js__da_check_invariants((xs)->arr, (xs)->len, (xs)->cap)
#else // NDEBUG
#define js_da_check_invariants(xs)
#endif // NDEBUG

#define js_da_at(xs, i)\
    (\
        js_da_check_invariants(xs),\
        js_ASSERT(0 <= (i) && (i) < (xs)->len),\
        (xs)->arr[(i)]\
    )

#define js_da_pushback_unless_full(xs, x)\
    (\
        js_da_check_invariants((xs)),\
        ((xs)->len >= (xs)->cap) ?\
            1 :\
            (xs)->arr[(xs)->len++], 0\
    )

#define js_da_pushback(xs, x)\
    do {\
        js_da_check_invariants(xs);\
        if ((xs)->len >= (xs)->cap) {\
            if (!(xs)->cap) {\
                (xs)->cap = 16;\
            } else {\
                (xs)->cap *= 2;\
            }\
            void *newptr = realloc((xs)->arr, sizeof(*(xs)->arr)*(xs)->cap);\
            if (!newptr) {\
                js_ALLOC_FAILURE(newptr);\
                free((xs)->arr);\
            } else {\
                (xs)->arr = newptr;\
            }\
        }\
        (xs)->arr[(xs)->len++] = x;\
    } while (0)

#define js_da_popback(xs)\
    (\
        js_da_check_invariants(xs),\
        js_ASSERT((xs)->len > 0),\
        (xs)->arr[--(xs)->len]\
    )

#define js_da_ensure_cap(xs, size)\
    do {\
        js_da_check_invariants(xs);\
        if ((xs)->cap < size) {\
            (xs)->cap = size;\
            void *newptr = realloc((xs)->arr, sizeof(*(xs)->arr)*(xs)->cap);\
            if (!newptr) {\
                js_ALLOC_FAILURE(newptr);\
                free((xs)->arr);\
            } else {\
                (xs)->arr = newptr;\
                memset((xs)->arr, 0x00, (xs)->cap);\
            }\
        }\
    } while (0)

#define js_da_clear(xs)\
    do {\
        js_da_check_invariants(xs);\
        (xs)->len = 0;\
    } while (0)

#define js_da_free(xs)\
    do {\
        js_da_check_invariants(xs);\
        free((xs)->arr);\
        (xs)->arr = NULL;\
        (xs)->len = 0;\
        (xs)->cap = 0;\
    } while (0)

#define js_da_foreach(type, x, xs)\
    for (type *x= ((xs)->len > 0 ? (xs)->arr : NULL); x<(xs)->arr+(xs)->len; ++x)

// string API

typedef struct js_string_builder {
    u8 *arr;
    i32 len;
    i32 cap;
} js_string_builder;

typedef struct js_string {
    u8 *arr;
    i32 len;
} js_string;

#define js_S_FMT "%.*s"

#define js_S_ARG(s) (s).len, (s).arr

js_string js_sb_finalise(js_string_builder sb);
int js_str_cmp(js_string a, js_string b);
i32 js_str_index_of(js_string s, u8 c);
js_string js_str_concat(js_string a, js_string b);

#ifdef BASE_IMPL

// dynamic array implementation

int js__da_check_invariants(void *arr, i64 len, i64 cap) {
    if (arr) {
        js_ASSERT(cap > 0);
        js_ASSERT(len >= 0);
        js_ASSERT(len <= cap);
    } else {
        js_ASSERT(cap == 0);
        js_ASSERT(len == 0);
    }
    return 0;
}

// string implementation

js_string js_sb_finalise(js_string_builder sb) {
    js_string s = {0};
    s.arr = sb.arr;
    s.len = sb.len;
    return s;
}

int js_str_cmp(js_string a, js_string b) {
    int min_len = a.len;
    if (b.len < min_len)
        min_len = b.len;
    return memcmp(a.arr, b.arr, min_len);
}

i32 js_str_index_of(js_string s, u8 c) {
    for (i32 i=0; i<s.len; ++i) {
        if (s.arr[i] == c) {
            return i;
        }
    }
    return -1;
}

js_string js_str_concat(js_string a, js_string b) {
    js_string s = {0};
    s.len = a.len + b.len;
    s.arr = malloc(s.len);
    memcpy(s.arr, a.arr, a.len);
    memcpy(s.arr+a.len, b.arr, b.len);
    return s;
}

// arena implementation

int js_arena_init(js_arena *a, i64 size_in_bytes) {
    js_ASSERT(a); 

    u8 *p = js_MALLOC(size_in_bytes);
    if (!p) {
        return 1;
    }

    a->data = p;
    a->count = size_in_bytes;
    a->offset = 0;

    return 0;
}

void js_arena_free(js_arena *a) {
    js_ASSERT(a);

    js_FREE(a->data);
    a->data = NULL;
    a->count = 0;
    a->offset = 0;
}

void js_arena_reset(js_arena *a) {
    js_ASSERT(a);

    a->offset = 0;
}

void *js_arena_push_raw(js_arena *a, i64 sizeof_type, i64 count, i64 alignof_type) {
    js_ASSERT(a);
    js_ASSERT(a->data);
    // alignof_type must be a power of two
    js_ASSERT((alignof_type > 0) && !(((uintptr_t)alignof_type) & ((uintptr_t)alignof_type-1)));
    js_ASSERT(sizeof_type > 0);
    js_ASSERT(count >= 0);

    if (!count) return NULL;

    i64 total_size = sizeof_type * count;

    js_ASSERT((total_size >= sizeof_type) && (total_size >= count));

    uintptr_t padding = ((uintptr_t)a->data + (uintptr_t)a->offset) % (uintptr_t)alignof_type;
    a->offset += padding;
    void *data_ptr = (void *)((uintptr_t)a->data + (uintptr_t)a->offset);
    memset(data_ptr, 0x00, total_size);
    a->offset += total_size;

    return data_ptr;
}

#endif // BASE_IMPL

#endif // BASE_H
