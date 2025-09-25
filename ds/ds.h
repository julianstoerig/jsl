#ifndef DS_C_
#define DS_C_

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

static void arena_reset(Arena *a);

static void arena_free(Arena *a);

// the reason this macro casts the void pointer is so that a type mismatch between variable and macro
// call will give a compile error (static type systems ftw!)
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
static void *__alloc(Arena *a, Size size, Size count, Size align, U8 flags,
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
            
static void grow(void *slice, Size size, Size align, Arena *a, U8 flags,
          U8 *typename, U8 *filename, int32_t lineno);

// U8
static B32 u8_iscontrol(U8 c);
static B32 u8_isalpha(U8 c);
static B32 u8_isnumeric(U8 c);
static B32 u8_isalphanum(U8 c);
static B32 u8_isspace(U8 c);
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
static int str__check_invariants(Str s);

#define STR_FMT "%.*s"

#define str_arg(s)\
    (int)(s).len, (char *)(s).arr

#define str_dyn(s)\
    str__from_parts((U8 *)(s), strlen((char *)(s)))

#define str_static(s)\
    (Str){(U8 *)(s), lengthof((s))}
    
#define str_from_parts(buf, len)\
    str__from_parts((U8 *)(s), (Size)(len))

static Str str__from_parts(U8 *buf, Size len);

static Str str_dup(Str s1, Arena *a);

static char *str_to_cstr(Str s, Arena *a);

static Str str_format(Str fmt_str, Arena *a, ...);

static B32 str_equal(Str a, Str b);

#define str_get_first_char(s)\
    str_get_char_at((s), 0)

static U8 str_get_char_at(Str s, Size i);

static Size str_index_of(Str s, U8 c);

static Str str_chop_left(Str s, Size n);

static Str str_chop_right(Str s, Size n);

Str str_readfile(Str filename, Arena *a);

static Str str_shift(int *argc, char ***argv);

static U64 str_hash(Str s);
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

static Str *triemap_lookup(triemap **map, Str key, Arena *a);

//#ifdef DS_C_IMPL
#if 1

Arena arena_new(Size size) {
    assert(size > 0);
    Arena a = {0};
    a.beg = MALLOC(size);
    a.buf = a.beg;
    if (!a.beg) exit(1);
    a.end = a.beg + size;
    return(a);
}

static void arena_reset(Arena *a) {
    a->beg = a->buf;
}

static void arena_free(Arena *a) {
    if (!a) return; // base case
    FREE(a->buf);
    arena_free(a->next); // recurse
}

#if defined(__GNUC__) || defined(__CLANG__)
__attribute((malloc)) // may allow for significantly more aliasing based optimisation
#endif // defined(__GNUC__ || __CLANG__)
static void *__alloc(Arena *a, Size size, Size count, Size align, U8 flags,
              U8 *typename, U8 *filename, int32_t lineno) {
    assert(a);
    assert(a->buf <= a->beg);
    assert(a->beg < a->end);
    assert(size > 0);
    assert(count > 0);
    assert(align > 0);
#if defined (TRUE_ALIGNMENT_INTRINSIC_FOUND)
    // since SIZEOF(T) = k*ALIGNOF(T), k in N, it might be 3*ALIGNOF(T) != 2**n, n in N
    assert(!((Uptr)align & ((Uptr)align-1)));
#endif // defined (TRUE_ALIGNMENT_INTRINSIC_FOUND)
    assert(filename);
    assert(lineno > 0);

    Size padding = -(Uptr)a->beg & ((Uptr)align-1);
    Size available = a->end - a->beg - padding;
    if (available < 0 || count > available/size) {
        if (flags & FLAG_SOFTFAIL) {
            return(0);
        } else {
            if (!a->next) {
                a->next = MALLOC((SIZEOF(Arena)));
                if (!a->next) exit(1);
                *a->next = arena_new(2*((Uptr)a->end - (Uptr)a->buf));
            }
            return(__alloc(a->next, size, count, align, flags, typename, filename, lineno));
        }
    }
    void *p = a->beg + padding;
    a->beg += padding + count*size;
    if (!(flags & FLAG_NOZERO)) {
        MEMSET(p, 0x00, count*size);
    }
    return(p);
}

static void grow(void *slice, Size size, Size align, Arena *a, U8 flags,
          U8 *typename, U8 *filename, int32_t lineno) {
    struct {
        Byte *arr;
        Size len;
        Size cap;
    } replica;

    memcpy(&replica, slice, SIZEOF(replica));

    assert(replica.arr || !replica.len);
    assert(replica.arr || !replica.cap);
    assert(replica.len >= 0);
    assert(replica.cap >= 0);
    assert(replica.cap >= replica.len);

    if (!replica.arr) {
        replica.cap = 1;
        replica.arr = __alloc(a, 2*size, replica.cap, align, flags, typename, filename, lineno);
    } else if (a->beg == replica.arr + size * replica.cap) {
        // if the dynamic array allocation is the last one made on the arena, it can be extended
        // in place by simple telling the arena to mark the space we want to extend into as used,
        // since we already occupy it, there is no need to do anything with the returned pointer.
        // TODO: consider possible error handling for FLAG_SOFTFAIL's `NULL` return
        (void) __alloc(a, 2*size, replica.cap, align, flags, typename, filename, lineno);
    } else {
        void *arr = __alloc(a, 2*size, replica.cap, align, flags, typename, filename, lineno);
        memcpy(arr, replica.arr, size*replica.len);
        replica.arr = arr;
    }

    // double size rather than cap here to prevent overflows.
    // size is a small constant stemming from `SIZEOF` and is therefore safe to double 
    replica.cap *= 2;
    memcpy(slice, &replica, SIZEOF(replica));
}

// U8
static B32 u8_iscontrol(U8 c) {
    B32 v0 = c < 32;
    B32 v1 = c == 127;
    return(v0 | v1);
}

static B32 u8_isalpha(U8 c) {
    B32 v0 = 'a' <= c && c <= 'z';
    B32 v1 = 'A' <= c && c <= 'Z';
    return(v0 | v1);
}

static B32 u8_isnumeric(U8 c) {
    B32 v0 = '0' <= c && c <= '9';
    return(v0);
}

static B32 u8_isalphanum(U8 c) {
    return (u8_isalpha(c) | u8_isnumeric(c));
}

static B32 u8_isspace(U8 c) {
    B32 v0 = c == ' ';
    B32 v1 = c == '\t';
    B32 v2 = c == '\r';
    B32 v3 = c == '\n';
    return(v0 | v1 | v2 | v3);
}
// U8

// STRINGS

// this function's return value is meaningless, but allows its use inside expresisons
static int str__check_invariants(Str s) {
    if (s.arr) {
        assert(s.len >= 0);
    } else {
        assert(s.len == 0);
    }
    return(0);
}

static Str str__from_parts(U8 *buf, Size len) {
    Str s = {0};
    s.arr = buf;
    s.len = len;
    str_check_invariants(s);
    return(s);
}

static Str str_dup(Str s1, Arena *a) {
    str_check_invariants(s1);
    Str s2 = {0};
    s2.arr = new(a, U8, s1.len);
    if (!s2.arr)
        goto defer;
    s2.len = s1.len;
    memcpy(s2.arr, s1.arr, s2.len);
defer:
    str_check_invariants(s2);
    return(s2);
}

static char *str_to_cstr(Str s, Arena *a) {
    char *cstr = 0;
    str_check_invariants(s);
    if (!s.len)
        goto defer;
    cstr = new(a, char, s.len+1);
    memcpy(cstr, s.arr, s.len);
    cstr[s.len] = '\0';
defer:
    return(cstr);
}

static Str str_format(Str fmt_str, Arena *a, ...) {
    Str s = {0};

    str_check_invariants(fmt_str);
    Size n = 0;
    Size size = 0;
    U8 *p = 0;
    va_list ap;
    U8 *fmt = str_to_cstr(fmt_str, a);

    va_start(ap, a);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    if (n < 0)
        goto defer;

    size = n + 1;
    p = new(a, U8, size);
    if (!p)
        goto defer;

    va_start(ap, a);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    if (n < 0)
        goto defer;

    s.arr = p;
    s.len = n;
        
defer:
    return(s);
}

static B32 str_equal(Str a, Str b) {
    str_check_invariants(a);
    str_check_invariants(b);
    if (a.len != b.len)
        return 0;
    return(a.len || memcmp(a.arr, b.arr, a.len));
}

static U8 str_get_char_at(Str s, Size i) {
    str_check_invariants(s);
    if (i < 0) i = 0;
    if (i > s.len) i = s.len;
    return(s.arr[i]);
}

static Size str_index_of(Str s, U8 c) {
    str_check_invariants(s);
    for (Size i=0; i<s.len; ++i)
        if (s.arr[i] == c)
            return(i);
    return(-1);
}

static Str str_chop_left(Str s, Size n) {
    str_check_invariants(s);
    Str a = {0};
    Size min_len = n < s.len ? n : s.len;
    a.arr = s.arr + min_len;
    a.len = s.len - min_len;
    return(a);
}

static Str str_chop_right(Str s, Size n) {
    str_check_invariants(s);
    Str a = {0};
    Size min_len = n < s.len ? n : s.len;
    a.arr = s.arr;
    a.len = s.len - min_len;
    return(a);
}

Str str_readfile(Str filename, Arena *a) {
    Str s = {0};
    FILE *f = 0;
    f = fopen(str_to_cstr(filename, a), "rb");
    if (!f)
        goto defer;

    fseek(f, 0, SEEK_END);
    s.len = ftell(f);
    if (s.len < 0)
        goto defer;

    fseek(f, 0, SEEK_SET);
    s.arr = new(a, U8, s.len);
    if (!s.arr)
        goto defer;

    Size bytes_read = fread(s.arr, 1, s.len, f);
    if (bytes_read != s.len) {
        s.len = bytes_read < s.len ? bytes_read : s.len;
        goto defer;
    }
defer:
    if (f) fclose(f);
    return(s);
}

static Str str_shift(int *argc, char ***argv) {
    Str s = {0};
    if (!argc || !*argv)
        goto defer;

    s.arr = **argv;
    s.len = strlen(**argv);
    --(*argc);
    ++(*argv);
defer:
    return(s);
}

static U64 str_hash(Str s) {
    str_check_invariants(s);
    U64 h = 0x100;
    for (Size i=0; i<s.len; ++i) {
        h ^= s.arr[i] & 255;
        h *= 1111111111111111111;
    }
    return(h);
}
// STRINGS

Str *flatmap_lookup(FlatMap *map, Str key) {
    U64 hash = str_hash(key);
    U32 mask = (1<<FLATMAPEXP) - 1;
    U32 step = (hash >> (64 - FLATMAPEXP)) | 1;
    for (I32 i = (I32)hash;;) {
        i = (i+step) & mask;
        if (!map->keys[i].arr) {
            map->keys[i] = key;
            return map->vals+i;
        } else if (str_equal(map->keys[i], key)) {
            return map->vals+i;
        }
    }
}

static Str *triemap_lookup(triemap **map, Str key, Arena *a) {
    for (U64 h = str_hash(key); *map; h <<= 2) {
        if (str_equal(key, (*map)->key)) {
            return &(*map)->val;
        }
        map = &(*map)->child[h>>62];
    }
    if (!a) return 0;
    *map = new(a, triemap);
    (*map)->key = key;
    return &(*map)->val;
}

#endif // DS_C_IMPL

#endif // DS_C_
