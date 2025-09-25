#ifndef DS_H
#define DS_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#if defined(__GCC__) || defined(__CLANG__)
// build with "-fsanitize=undefined,unreachable -fsanitize-trap"
// to enable asserts
#define assert(c) while (!(c)) __builtin_unreachable()
#else // defined(__GCC__) || defined(__CLANG__)
#include <assert.h>
#endif // defined(__GCC__) || defined(__CLANG__)

typedef   uint8_t   Byte;
typedef   uint8_t     U8;
typedef  uint16_t    U16;
typedef  uint32_t    U32;
typedef  uint64_t    U64;
typedef    int8_t     I8;
typedef   int16_t    I16;
typedef   int32_t    I32;
typedef   int64_t    I64;
typedef     float    F32;
typedef    double    F64;
typedef uintptr_t   Uptr;
typedef ptrdiff_t   Size;
typedef    size_t  Usize;
typedef   uint8_t     B8;
typedef  uint32_t    B32;

#define TRUE_ALIGNMENT_INTRINSIC_FOUND
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define ALIGNOF SIZEOF
#undef TRUE_ALIGNMENT_INTRINSIC_FOUND
#elif __STDC_VERSION__ < 202311L
#define ALIGNOF _Alignof
#else
#define ALIGNOF alignof
#endif // __STDC_VERSION__

#define SIZEOF(x)   ((Size)sizeof(x))
#define countof(arr) (SIZEOF(arr) / SIZEOF(*(arr)))
#define lengthof(s)  (countof(s)-1)

#define MALLOC(size) malloc(size)

#define FREE(ptr) free(ptr)

#define MEMSET(dst, src, len) memset(dst, src, len)

#define KiB (Size)(1024LL)
#define MiB (Size)(1024LL*1024LL)
#define GiB (Size)(1024LL*1024LL*1024LL)

struct Arena;

typedef struct Arena Arena;
struct Arena {
    Byte *buf;
    Byte *beg;
    Byte *end;
    Arena *next;
};

typedef enum {
    FLAG_NOFLAGS  =      0,
    FLAG_SOFTFAIL = 1 << 0,
    FLAG_NOZERO   = 1 << 1,
} Flags;

Arena arena_new(Size size);

void arena_reset(Arena *a);

void arena_free(Arena *a);

// the reason this macro casts the void pointer is so that a type mismatch between variable and macro
// call will give a compile error (type systems ftw!)
#define new(...) newx(__VA_ARGS__, new4, new3, new2) (__VA_ARGS__)
#define newx(a, b, c, d, e, ...) e
#define new2(a, t)       (t *)__alloc((a), SIZEOF(t), 1, ALIGNOF(t), FLAG_NOFLAGS,\
            (U8 *)#t, (U8 *)__FILE__, __LINE__) // just arena and type
#define new3(a, t, n)    (t *)__alloc((a), SIZEOF(t), n, ALIGNOF(t), FLAG_NOFLAGS,\
            (U8 *)#t, (U8 *)__FILE__, __LINE__) // add count
#define new4(a, t, n, f) (t *)__alloc((a), SIZEOF(t), n, ALIGNOF(t), f,\
            (U8 *)#t, (U8 *)__FILE__, __LINE__) // add flags

Size arena_free_space(Arena *a);

#if defined(__GNUC__) || defined(__CLANG__)
__attribute((malloc)) // may allow for significantly more aliasing based optimisation
#endif // defined(__GNUC__ || __CLANG__)
void *__alloc(Arena *a, Size size, Size count, Size align, U8 flags,
              U8 *typename, U8 *filename, int32_t lineno);

// C standard does not allow ALIGNOF(expr), GCC & Clang support it as extension, though
// MSVC does not
#if defined(__GNUC__) || defined(__CLANG__)
#define push(xs, a)                                                                                \
    (((xs)->len >= (xs)->cap)                                                                      \
        ? grow((xs), SIZEOF(*(xs)->arr), ALIGNOF(*(xs)->arr), a, FLAG_NOFLAGS,                     \
                (U8 *)"`N/A` (member of " #xs ")", (U8 *)__FILE__, __LINE__),                      \
                (xs)->arr + (xs)->len++                                                            \
        :                                   (xs)->arr + (xs)->len++)
#else // defined(__GNUC__) || defined(__CLANG__)
#define push(xs, a)                                                                                \
    (((xs)->len >= (xs)->cap)                                                                      \
        ? grow((xs), SIZEOF(*(xs)->arr), 16, a, FLAG_NOFLAGS,                                      \
                (U8 *)"`N/A` (member of " #xs ")", (U8 *)__FILE__, __LINE__),                      \
                (xs)->arr + (xs)->len++                                                            \
        :                                   (xs)->arr + (xs)->len++)
#endif // defined(__GNUC__) || defined(__CLANG__)
            
void grow(void *slice, Size size, Size align, Arena *a, U8 flags,
          U8 *typename, U8 *filename, int32_t lineno);

// U8
B32 u8_iscontrol(U8 c);
B32 u8_isalpha(U8 c);
B32 u8_isnumeric(U8 c);
B32 u8_isalphanum(U8 c);
B32 u8_isspace(U8 c);
// U8

// STRINGS
typedef struct {
    U8  *arr;
    Size len;
} Str;

#ifndef NDEUBG
#define str_check_invariants(s)\
    str__check_invariants(s)
#else // NDEUBG
#define str_check_invariants(s)
#endif // NDEUBG

// this function's return value is meaningless, but allows its use inside expresisons
int str__check_invariants(Str s);

#define STR_FMT "%.*s"

#define str_arg(s)\
    (int)(s).len, (char *)(s).arr

#define str_dyn(s)\
    str__from_parts((U8 *)(s), strlen((char *)(s)))

#define str_static(s)\
    (Str){(U8 *)(s), lengthof((s))}
    
#define str_from_parts(buf, len)\
    str__from_parts((U8 *)(s), (Size)(len))

Str str__from_parts(U8 *buf, Size len);

Str str_dup(Str s1, Arena *a);

char *str_to_cstr(Str s, Arena *a);

Str str_format(Str fmt_str, Arena *a, ...);

B32 str_equal(Str a, Str b);

#define str_get_first_char(s)\
    str_get_char_at((s), 0)

U8 str_get_char_at(Str s, Size i);

Size str_index_of(Str s, U8 c);

Str str_chop_left(Str s, Size n);

Str str_chop_right(Str s, Size n);

Str str_readfile(Str filename, Arena *a);

Str str_shift(int *argc, char ***argv);

U64 str_hash(Str s);
// STRINGS

enum { FLATMAPEXP=10 };
typedef struct {
    Str keys[1<<FLATMAPEXP];
    Str vals[1<<FLATMAPEXP];
} FlatMap;

Str *flatmap_lookup(FlatMap *map, Str key);

typedef struct triemap triemap;
struct triemap {
    triemap *child[4];
    Str key;
    Str val;
};

Str *triemap_lookup(triemap **map, Str key, Arena *a);

#endif // DS_H
