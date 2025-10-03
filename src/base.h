#pragma once // literally every compiler supports this now

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// CONTEXT CRACKING

#define JSL_STD_C89 0
#define JSL_STD_C99 0
#define JSL_STD_C11 0
#define JSL_STD_C17 0
#define JSL_STD_C23 0

#if !defined(__STDC__)
    #error compiler not even C89 compliant
#else
    #if __STDC_VERSION__ > 201710L
        #undef JSL_STD_C23
        #define JSL_STD_C23 1
    #elif __STDC_VERSION__ > 201112L
        #undef JSL_STD_C17
        #define JSL_STD_C17 1
    #elif __STDC_VERSION__ > 199901L
        #undef JSL_STD_C11
        #define JSL_STD_C11 1
    #elif __STDC_VERSION__ > 199409L
        #undef JSL_STD_C99
        #define JSL_STD_C99 1
    #else
        #undef JSL_STD_C89
        #define JSL_STD_C89 1
    #endif
#endif


#define JSL_LANG_CPP 0
#define JSL_LANG_C 0

#if defined(__STDC__)
    #undef JSL_LANG_C
    #define JSL_LANG_C 1
#else
    #undef JSL_LANG_CPP
    #define JSL_LANG_CPP 1
#endif


#define JSL_TOOLCHAIN_GCC 0
#define JSL_TOOLCHAIN_CLANG 0
#define JSL_TOOLCHAIN_CL 0

#if defined(__GNUC__) && !defined(__clang__)
    #undef JSL_TOOLCHAIN_GCC
    #define JSL_TOOLCHAIN_GCC 1
#elif _MSC_VER
    #undef JSL_TOOLCHAIN_CL
    #define JSL_TOOLCHAIN_CL 1
#elif defined(__clang__)
    #undef JSL_TOOLCHAIN_CLANG
    #define JSL_TOOLCHAIN_CLANG 1
#endif


#define JSL_OS_LIN 0
#define JSL_OS_MAC 0
#define JSL_OS_WIN 0

#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
    #undef JSL_OS_WIN
    #define JSL_OS_WIN 1
#elif __linux__
    #undef JSL_OS_LIN
    #define JSL_OS_LIN 1
#elif defined(macintosh) || defined(Macintosh) || defined(__APLE__) || defined(__MACH__)
    #undef JSL_OS_MAC
    #define JSL_OS_MAC 1
#endif // JSL_OS


#define JSL_ARCH_x64 0
#define JSL_ARCH_ARM 0

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)
    #undef JSL_ARCH_x64
    #define JSL_ARCH_x64 1
#elif defined(__arm__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT)
    #undef JSL_ARCH_ARM
    #define JSL_ARCH_ARM 1
#endif // JSL_ARCH

// ASSERT

#if !defined(JSL_ASSERT_BREAK)
    #if JSL_TOOLCHAIN_CLANG || JSL_TOOLCHAIN_GCC
        #define JSL_ASSERT_BREAK() __builtin_trap()
    #elif JSL_TOOLCHAIN_CL
        #define JSL_ASSERT_BREAK() __debugbreak()
    #else
        #define JSL_ASSERT_BREAK() *(volatile int *)0 = 0
    #endif // JSL_TOOLCHAIN
#endif // !defined(JSL_ASSERT_BREAK)

#if !defined(assert)
    #if !defined(NDEBUG)
        #define assert(c) (!(c) ? JSL_ASSERT_BREAK(), 1 : 1)
    #else // !defined(NDEBUG)
        #define assert(c) (1)
    #endif // !defined(NDEBUG)
#endif // !defined(assert)

#if JSL_TOOLCHAIN_CLANG
    #define u64_from_ptr(p) (U64)(uintptr_t)(p)
#else
    #define u64_from_ptr(p) (uintptr_t)((char*)p - (char*)0)
#endif // JSL_TOOLCHAIN
#define void_ptr_from_int(p) (void*)((char*)0 + (n)))

#define member_of(T, m)  (((T*)0)->m)
#define offfset_of(T, m) uint_from_ptr(&member_of(T, m))

#define memzero_bytes(p, num_bytes) memset((p), 0x00, (num_bytes))

#define min(a, b)       ((a)<(b) ? (a) : (b))
#define max(a, b)       ((a)>(b) ? (a) : (b))
#define clamp_top(x, b) min(x, b)
#define clamp_bot(x, a) max(x, a)
#define clamp(x, a, b)  clamp_top(b, clamp_bot(a, x))

#define global   static
#define persist  static
#define function static

typedef   int8_t S08;
typedef  int16_t S16;
typedef  int32_t S32;
typedef  int64_t S64;
typedef  uint8_t U08;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef  uint8_t B08;
typedef uint32_t B32;
typedef    float F32;
typedef   double F64;

function F32 f32_inf();

function F32 f32_inf_neg();

function F64 f64_inf();

function F64 f64_inf_neg();

#define alignof(T) (S64)alignof(T)
#define sizeof(E) (S64)sizeof(E)
#define countof(A) sizeof(A)/sizeof(*A)

#define KiB (1024LL)
#define MiB (1024LL*KiB)
#define GiB (1024LL*MiB)

typedef struct Arena Arena;
typedef void (Oom)(Arena *a);
struct Arena {
    U08 *buf;
    S64  len;
    S64  cap;
    Oom *oom;
};

typedef enum {
    ARENA_FLAG_DEFAULT = 0 << 0,
    ARENA_FLAG_NO_ZERO = 1 << 0,
    ARENA_FLAG_NO_OOM  = 1 << 1,
} ArenaFlag;

void arena__default_oom(Arena *a);

void arena__check_invariants(Arena *a);

void arena_init(Arena *a, S64 size);

void arena_free(Arena *a);

void arena_reset(Arena *a); 

#define make(...) makex(__VA_ARGS__, make4, make3, make2) (__VA_ARGS__)
#define makex(a, b, c, d, e, ...) e
#define make2(a, T)       (T*)arena__alloc((a), sizeof(T), (0), alignof(T), ARENA_FLAG_DEFAULT)
#define make3(a, T, n)    (T*)arena__alloc((a), sizeof(T), (n), alignof(T), ARENA_FLAG_DEFAULT)
#define make4(a, T, n, f) (T*)arena__alloc((a), sizeof(T), (n), alignof(T), (f))

void *arena__alloc(Arena *a, S64 element_size, S64 element_count, S64 element_alignment, U08 flags);

// dynamic arrays
#ifndef JSL_DA_DEFAULT_CAP
#define JSL_DA_DEFAULT_CAP 128
#endif // JSL_DA_DEFAULT_CAP

#if !defined(NDEBUG)
    #define get(xs, i)                                          \
        assert(0 <= (i)),                                       \
         assert(i < (xs).len),                                  \
         (xs).buf[(i)]
#else // !defined(NDEBUG)
    #define get(xs, i)\
         ((xs)->buf[(i)])
#endif // !defined(NDEBUG)

#if JSL_TOOLCHAIN_GCC || JSL_TOOLCHAIN_CLANG
#define push(xs, a)                                                                                \
    (((xs)->len >= (xs)->cap)                                                                      \
        ? da_grow((xs), SIZEOF(*(xs)->buf), ALIGNOF(*(xs)->buf), a, FLAG_NOFLAGS,                  \
        :                                   (xs)->buf + (xs)->len++)
#else // JSL_TOOLCHAIN_GCC || JSL_TOOLCHAIN_CLANG
#define push(xs, a)                                                                                \
    (((xs)->len >= (xs)->cap)                                                                      \
        ? da_grow((xs), SIZEOF(*(xs)->buf), 16, a, FLAG_NOFLAGS,                                   \
                (U08 *)"`N/A` (member of " #xs ")", (U08 *)__FILE__, __LINE__),                    \
                (xs)->buf + (xs)->len++                                                            \
        :                                   (xs)->buf + (xs)->len++)
#endif // JSL_TOOLCHAIN_GCC || JSL_TOOLCHAIN_CLANG

void da_grow(void *slice, S64 size, S64 align, Arena *a, U08 flags);

// "Char"/U08 stuf

B32 u08_iscontrol(U08 c);

B32 u08_isalpha(U08 c);

B32 u08_isnumeric(U08 c);

B32 u08_isalphanum(U08 c);

B32 u08_isspace(U08 c);

// Strings

typedef struct {
    U08 *buf;
    S64  len;
    S64  cap;
} Str;

#define STR_FMT\
    "%.*s"
#define str_arg(s)\
    (int)(s).len, (char*)(s).buf

#define str_dyn(buf)\
    str__from_parts((U08*)(buf), strlen((char*)(buf)))
#define str_sta(buf)\
    Str{(U08*)(buf), strlen((char*)(buf))}
#define str_from_parts(buf, len)\
    str__from_parts((U08*)(buf), (S64)(len))

#ifndef NDEBUG
    #define str_check_invariants(s) str__check_invariants(s)
#else // NDEBUG
    #define str_check_invariants(s)
#endif // NDEBUG

// this function's return value is meaningless, but allows its use inside expresisons
int str__check_invariants(Str s);

Str str__from_parts(U08 *buf, S64 len);

Str str_dup(Str s1, Arena *a);

char *str_to_cstr(Str s, Arena *a);

Str str_format(Str fmt_str, Arena *a, ...);

B32 str_equal(Str a, Str b);

U08 str_get_char_at(Str s, S64 i);

S64 str_index_of(Str s, U08 c);

Str str_chop_left(Str s, S64 n);

Str str_chop_right(Str s, S64 n);

Str str_tok(Str s, Str seps, S64 *idx_state);

Str str_readfile(Str filename, Arena *a);

Str str_shift(int *argc, char ***argv);

U64 str_hash(Str s);

// StringList

typedef struct {
    Str *buf;
    S64 len;
    S64 sum_len;
    S64 cap;
} StrList;

#ifndef NDEBUG
    #define strlist_check_invariants(s)\
        strlist__check_invariants(s)
#else // NDEBUG
    #define strlist_check_invariants(s)
#endif // NDEBUG

int strlist__check_invariants(StrList l);

Str strlist_to_str(StrList l, Arena *a);

// hash

U64 u64_hash(U64 x);

U64 u64_unhash(U64 x);

/* Example MSI Hash Table
* refer to skeeto @ nullprogram.com for more info
#define EXP 5
struct MSIHashTable {
    Str *table[1 << (EXP)];
    S64 len;
};
*/

S64 msi_lookup(U64 hash, S64 exp, S64 idx);
