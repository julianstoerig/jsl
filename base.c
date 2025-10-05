#include "base.h"

global S08 s08_min_val = (S08)0x80;
global S08 s08_max_val = (S08)0x7f;
global S16 s16_min_val = (S16)0x8000;
global S16 s16_max_val = (S16)0x7fff;
global S32 s32_min_val = (S32)0x80000000;
global S32 s32_max_val = (S32)0x7fffffff;
global S64 s64_min_val = (S64)0x8000000000000000;
global S64 s64_max_val = (S64)0x7fffffffffffffff;

global U08 u08_min_val = 0;
global U08 u08_max_val = 0xff;
global U16 u16_min_val = 0;
global U16 u16_max_val = 0xffff;
global U32 u32_min_val = 0;
global U32 u32_max_val = 0xffffffff;
global U64 u64_min_val = 0;
global U64 u64_max_val = 0xffffffffffffffff;

function F32 f32_inf(void) {
    union {F32 f; U32 i;} v;
    v.i = 0x7f800000UL;
    return(v.f);
}

function F32 f32_inf_neg(void) {
    union {F32 f; U32 i;} v;
    v.i = 0xff800000UL;
    return(v.f);
}

function F64 f64_inf(void) {
    union {F64 f; U64 i;} v;
    v.i = 0x7ff0000000000000UL;
    return(v.f);
}

function F64 f64_inf_neg(void) {
    union {F64 f; U64 i;} v;
    v.i = 0xfff0000000000000UL;
    return(v.f);
}

void arena__default_oom(Arena *a) {
    (void)a;
    assert(0);
    exit(1);
}

void arena__check_invariants(Arena *a) {
    if (!a)
        return;
    if (!a->buf) {
        assert(!a->len);
        assert(!a->cap);
        return;
    }
    assert(0 <= a->len);
    assert(0 <  a->cap);
    assert(a->len <= a->cap);
}

void arena_init(Arena *a, S64 size) {
    assert(!a->buf);
    arena__check_invariants(a);
    a->buf = (U08*)malloc(size);
    a->len = 0;
    a->cap = size;
    a->oom = arena__default_oom;
}

void arena_free(Arena *a) {
    arena__check_invariants(a);
    free(a->buf);
}

void arena_reset(Arena *a) {
    arena__check_invariants(a);
    a->len = 0;
} 

void *arena__alloc(Arena *a, S64 element_size, S64 element_count, S64 element_alignment, U08 flags) {
    assert(0 <  element_size);
    assert(0 <= element_count);
    assert(0 <  element_alignment);
    arena__check_invariants(a);
    S64 total_size = ((U64)element_size * (U64)element_count);
    if (total_size < element_size || total_size < element_count)
        exit(1); // overflow bug //TODO

    S64 padding = u64_from_ptr(a->buf) & ((U64)element_alignment-1);
    S64 available = a->cap - a->len - padding;

    if (available < 0) {
        if (flags & ARENA_FLAG_NO_OOM) {
            return 0;
        } else {
            a->oom(a);
        }
    }
    void *p = (void*)(a->buf + padding);
    a->buf += padding + total_size;
    if (!(flags & ARENA_FLAG_NO_ZERO))
        memzero_bytes(p, total_size);
    return(p);
}

// Stretchy Buffer

void *sb__grow(void *sb, S64 new_len, S64 element_size) {
    S64 new_cap = max(1 + 2*sb_cap(sb), new_len);
    assert(new_len <= new_cap);
    S64 new_size = offsetof(SbHeader, buf) + new_cap*element_size;
    SbHeader *new_sb = 0;
    if (sb) {
        S64 len = sb__header_get(sb)->len;
        new_sb = realloc(sb__header_get(sb), new_size);
        if (!new_sb) exit(1); // TODO: add customisable exit handling
        new_sb->len = len;
    } else {
        new_sb = malloc(new_size);
        if (!new_sb) exit(1); // TODO: add customisable exit handling
        new_sb->len = 0;
    }
    new_sb->cap = new_cap;
    return(new_sb->buf);
}

// "Char"/U08 stuf

B32 u08_iscontrol(U08 c) {
    B32 b0 = c < 32;
    B32 b1 = c == 127;
    return(b0 | b1);
}

B32 u08_isalpha(U08 c) {
    B32 b0 = 'a' <= c;
    B32 b1 = c <= 'z';
    B32 b2 = 'A' <= c;
    B32 b3 = c <= 'Z';
    return((b0 & b1) | (b2 & b3));
}

B32 u08_isnumeric(U08 c) {
    B32 b0 = '0' <= c;
    B32 b1 = c <= '9';
    return(b0 & b1);
}

B32 u08_isalphanum(U08 c) {
    return(u08_isalpha(c) | u08_isnumeric(c));
}

B32 u08_isspace(U08 c) {
    B32 v0 = c == ' ';
    B32 v1 = c == '\t';
    B32 v2 = c == '\r';
    B32 v3 = c == '\n';
    return(v0 | v1 | v2 | v3);
}

// Strings

// this function's return value is meaningless, but allows its use inside expresisons
int str__check_invariants(Str s) {
    if (s.buf) {
        assert(s.len >= 0);
    } else {
        assert(s.len == 0);
    }
    return(0);
}

Str str__from_parts(U08 *buf, S64 len) {
    Str s = {};
    s.buf = buf;
    s.len = len;
    str_check_invariants(s);
    return(s);
}

Str str_dup(Str s1, Arena *a) {
    str_check_invariants(s1);
    Str s2 = {};
    s2.buf = make(a, U08, s1.len);
    if (!s2.buf)
        goto defer;
    s2.len = s1.len;
    memcpy(s2.buf, s1.buf, s2.len);
defer:
    str_check_invariants(s2);
    return(s2);
}

char *str_to_cstr(Str s, Arena *a) {
    char *cstr = 0;
    str_check_invariants(s);
    if (!s.len)
        goto defer;
    cstr = make(a, char, s.len+1);
    memcpy(cstr, s.buf, s.len);
    cstr[s.len] = '\0';
defer:
    return(cstr);
}

Str str_format(Str fmt_str, Arena *a, ...) {
    Str s = {};

    str_check_invariants(fmt_str);
    S64 n = 0;
    S64 size = 0;
    char *p = 0;
    va_list ap;
    char *fmt = str_to_cstr(fmt_str, a);

    va_start(ap, a);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    if (n < 0)
        return(s);

    size = n + 1;
    p = make(a, char, size);
    if (!p)
        return(s);

    va_start(ap, a);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    if (n < 0)
        return(s);

    s.buf = (U08*)p;
    s.len = n;

    return(s);
}

B32 str_equal(Str a, Str b) {
    str_check_invariants(a);
    str_check_invariants(b);
    if (a.len != b.len)
        return 0;
    return(a.len || memcmp(a.buf, b.buf, a.len));
}

U08 str_get_char_at(Str s, S64 i) {
    str_check_invariants(s);
    if (i < 0) i = 0;
    if (i > s.len) i = s.len;
    return(s.buf[i]);
}

S64 str_index_of(Str s, U08 c) {
    str_check_invariants(s);
    for (S64 i=0; i<s.len; ++i)
        if (s.buf[i] == c)
            return(i);
    return(-1);
}

Str str_chop_left(Str s, S64 n) {
    str_check_invariants(s);
    Str a = {};
    S64 min_len = n < s.len ? n : s.len;
    a.buf = s.buf + min_len;
    a.len = s.len - min_len;
    return(a);
}

Str str_chop_right(Str s, S64 n) {
    str_check_invariants(s);
    Str a = {};
    S64 min_len = n < s.len ? n : s.len;
    a.buf = s.buf;
    a.len = s.len - min_len;
    return(a);
}

Str str_tok(Str s, Str seps, S64 *idx_state) {
    Str w = {};
    S64 idx = *idx_state;
    if (idx < s.len) {
        w.buf = s.buf + idx;
        B32 running = 1;
        while (running) {
            for (S64 i=0; i<seps.len; ++i) {
                if (seps.buf[i] == s.buf[idx]) {
                    running = 0;
                }
            }
            if (running) {
                ++w.len;
                ++idx;
            }
        }
    }
    *idx_state = idx;
    return(w);
}

Str str_readfile(Str filename, Arena *a) {
    Str s = {};
    FILE *f = 0;
    f = fopen(str_to_cstr(filename, a), "rb");
    if (!f)
        return(s);
    fseek(f, 0, SEEK_END);
    s.len = ftell(f);
    if (s.len < 0) {
        return(s);
        fclose(f);
    }
    fseek(f, 0, SEEK_SET);
    s.buf = make(a, U08, s.len);
    if (!s.buf) {
        return(s);
        fclose(f);
    }
    S64 bytes_read = fread(s.buf, 1, s.len, f);
    if (bytes_read != s.len) {
        s.len = bytes_read < s.len ? bytes_read : s.len;
        fclose(f);
        return(s);
    }
    fclose(f);
    return(s);
}

Str str_shift(int *argc, char ***argv) {
    Str s = {};
    if (!argc || !*argv)
        return(s);
    s.buf = **(U08***)argv;
    s.len = strlen(**argv);
    --(*argc);
    ++(*argv);
    return(s);
}

U64 str_hash(Str s) {
    // FNV-1A with murmurhash finaliser
    str_check_invariants(s);
    U64 h = 0x100;
    for (S64 i=0; i<s.len; ++i) {
        h ^= s.buf[i] & 255;
        h *= 1111111111111111111;
    }
    return(h);
}

// StringList

int strlist__check_invariants(StrList l) {
    if (!l.buf) {
        assert(!l.len);
        assert(!l.sum_len);
        assert(!l.cap);
    } else {
        assert(0 <= l.len);
        assert(0 <= l.sum_len);
        assert(0 <= l.cap);
        assert(l.len <= l.cap);
        S64 sum_len = 0;
        for (S64 i=0; i<l.cap; ++i)
            sum_len += l.buf[i].len;
        assert(l.sum_len == sum_len);
    }
    return(0);
}

Str strlist_to_str(StrList l, Arena *a) {
    strlist_check_invariants(l);
    U08 *buf = make(a, U08, l.sum_len);
    S64 cur = 0;
    for (S64 i=0; i<l.len; ++i) {
        Str s = l.buf[i];
        memcpy(buf+cur, s.buf, s.len);
        cur += s.len;
    }
    return (Str){buf, cur};
}

// hash
U64 u64_hash(U64 x) {
    x = (x ^ (x >> 30ULL)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27ULL)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31ULL);
    return(x);
}

U64 u64_unhash(U64 x) {
    x = (x ^ (x >> 31ULL) ^ (x >> 62ULL)) * 0x319642b2d24d8ec3ULL;
    x = (x ^ (x >> 27ULL) ^ (x >> 54ULL)) * 0x96de1b173f119089ULL;
    x =  x ^ (x >> 30ULL) ^ (x >> 60ULL);
    return(x);
}

/* Example MSI Hash Table
* refer to skeeto @ nullprogram.com for more info
#define EXP 5
struct MSIHashTable {
    Str *table[1 << (EXP)];
    S64 len;
};
*/

S64 msi_lookup(U64 hash, S64 exp, S64 idx) {
    assert(0 <= exp);
    assert(0 <= idx);
    assert(idx < (1 << exp));
    U32 mask = ((U32)1 << exp) - 1;
    U32 step = (hash << (64 - exp)) | 1;
    S64 next = (idx + step) & mask;
    return(next);
}
