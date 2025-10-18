#include "base.h"

S08 s08_min_val = (S08)0x80;
S08 s08_max_val = (S08)0x7f;
S16 s16_min_val = (S16)0x8000;
S16 s16_max_val = (S16)0x7fff;
S32 s32_min_val = (S32)0x80000000;
S32 s32_max_val = (S32)0x7fffffff;
S64 s64_min_val = (S64)0x8000000000000000;
S64 s64_max_val = (S64)0x7fffffffffffffff;

U08 u08_min_val = 0;
U08 u08_max_val = 0xff;
U16 u16_min_val = 0;
U16 u16_max_val = 0xffff;
U32 u32_min_val = 0;
U32 u32_max_val = 0xffffffff;
U64 u64_min_val = 0;
U64 u64_max_val = 0xffffffffffffffff;

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

Arena arena_create(S64 size) {
    Arena a = {0};
    arena_init(&a, size);
    return(a);
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

    if (!a->buf) {
        arena_init(a, ARENA__DEFAULT_SIZE);
    }

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

// Dynamic Array

void da__check_invariants(da__DynamicArray *xs) {
    da__DynamicArray arr;
    memcpy(&arr, xs, sizeof(da__DynamicArray));

    assert(0 <= arr.len);
    assert(0 <= arr.cap);
    assert(arr.len <= arr.cap);
    assert(!arr.cap || arr.buf); // left implies right
}

void da__grow(void *xs, S64 size) {
    da__DynamicArray arr;
    memcpy(&arr, xs, sizeof(da__DynamicArray));
    da__check_invariants(&arr);

    arr.cap = arr.cap ? 2*arr.cap : 128;
    void *p = 0;
    if (arr.a) {
        p = arena__alloc(arr.a, size, arr.cap, size, ARENA_FLAG_DEFAULT);
    } else {
        p = malloc(size*arr.cap);
    }
    if (arr.len)
        memcpy(p, arr.buf, size*arr.len);
    arr.buf = p;
    memcpy(xs, &arr, sizeof(arr));
}

void da__clear(void *xs) {
    da__DynamicArray arr;
    memcpy(&arr, xs, sizeof(da__DynamicArray));
    da__check_invariants(xs);

    arr.len = 0;
    memcpy(xs, &arr, sizeof(arr));
}

void da__free(void *xs) {
    da__DynamicArray arr;
    memcpy(&arr, xs, sizeof(da__DynamicArray));
    da__check_invariants(xs);

    arr.len = 0;
    arr.cap = 0;
    arr.buf = 0;
    memcpy(xs, &arr, sizeof(arr));
}

B32 da__is_in_bounds(void *xs, S64 i) {
    da__DynamicArray arr;
    memcpy(&arr, xs, sizeof(da__DynamicArray));
    da__check_invariants(xs);
    B32 in_bounds = (0 <= i) & (i < arr.len);
    if (in_bounds) {
        return 1;
    } else {
        return 0;
    }
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
    str_check_invariants(s);
    str_check_invariants(seps);
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
    str_check_invariants(filename);
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
    str_check_invariants(s);
    return(s);
}

U64 str_hash(Str s) {
    return(xxh64(s.buf, s.len, 0));
}

// String Builder

int sb__check_invariants(StringBuilder s) {
    if (s.buf) {
        assert(s.len >= 0);
        assert(s.cap >= 0);
        assert(s.cap >= s.len);
    } else {
        assert(s.len == 0);
        assert(s.cap == 0);
    }
    return(0);
}

void sb_extend(StringBuilder *sb, Str s) {
    S64 combined_len = sb->len + s.len;
    da__grow(sb, combined_len);
    memcpy(sb->buf+sb->len, s.buf, s.len * sizeof(*s.buf));
    sb->len += s.len;
    assert(sb->len == combined_len);
}

void sb_extend_sb(StringBuilder *sb, StringBuilder s) {
    S64 combined_len = sb->len + s.len;
    da__grow(sb, combined_len);
    memcpy(sb->buf+sb->len, s.buf, s.len * sizeof(*s.buf));
    sb->len += s.len;
    assert(sb->len == combined_len);
}

void sb_consume_sb(StringBuilder *sb, StringBuilder *s) {
    sb_extend_sb(sb, *s);
    da_free(s);
}

void sb_extend_cstr(StringBuilder *sb, char *s) {
    S64 s_len = strlen(s);
    S64 combined_len = sb->len + s_len;
    da__grow(sb, combined_len);
    memcpy(sb->buf+sb->len, s, s_len * sizeof(*s));
    sb->len += s_len;
    assert(sb->len == combined_len);
}

void sb_consume_cstr(StringBuilder *sb, char *s) {
    sb_extend_cstr(sb, s);
    free(s);
}

void sb_format(StringBuilder *sb, U08 *fmt, ...) {
    sb_check_invariants(*sb);
    S64 n = 0;
    S64 size = 0;
    U08 *p = 0;
    va_list ap;

    va_start(ap, fmt);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);
    if (n < 0) return;

    da__grow(sb, sb->len + n);
    size = n + 1;
    p = sb->buf + sb->len;
    if (!p) return;

    va_start(ap, fmt);
    n = vsnprintf(p, size, fmt, ap);
    va_end(ap);
    if (n < 0) return;

    sb->len += n;
}

// hash

// impl stolen from
// https://github.com/demetri/scribbles/blob/master/hashing/

U64 xxh64(const void *key, S64 len, U64 h) {
  // primes used in mul-rot updates
  U64 p1 = 0x9e3779b185ebca87, p2 = 0xc2b2ae3d27d4eb4f,
    p3 = 0x165667b19e3779f9, p4 =0x85ebca77c2b2ae63, p5 = 0x27d4eb2f165667c5;

  // inital 32-byte (4x8) wide hash state
  U64 s[4] = {h+p1+p2, h+p2, h, h-p1};

  // bulk work: process all 32 byte blocks 
  for (S64 i=0; i < (len/32); i++) {
    U64 b[4];
    memcpy(b, key + 4*i, sizeof(b));

    for (S64 j=0;j<4;j++) b[j] = b[j]*p2+s[j];
    for (S64 j=0;j<4;j++) s[j] = ((b[j] << 31) | (b[j] >> 33))*p1;
  }

  // mix 32-byte state down to 8-byte state, initalize to value for short keys
  U64 s64 = (s[2] + p5);
  if (len >= 32) {
    s64  = ((s[0] << 1)  | (s[0] >> 63)) + ((s[1] << 7)  | (s[1] >> 57)) +
           ((s[2] << 12) | (s[2] >> 52)) + ((s[3] << 18) | (s[3] >> 46));
    for (S64 i=0; i<4;i++) {
      U64 ps = (((s[i]*p2) << 31) | ((s[i]*p2) >> 33))*p1;
      s64 = (s64 ^ ps)*p1 + p4;
    }
  }
  s64 += len;

  // up to 31 bytes remain, process 0-3 8 byte blocks
  U08 *tail = (U08 *) (key + (len/32)*32);
  for (S64 i=0;i < (len & 31) / 8; i++,tail+=8) {  
    U64 b;
    memcpy(&b, tail, sizeof(U64));

    b *= p2;
    b = (((b << 31)| (b >> 33))*p1) ^ s64;
    s64 = ((b << 27) | (b >> 37))*p1 + p4;
  }

  // up to 7 bytes remain, process 0-1 4 byte block
  for (S64 i=0;i< (len & 7) / 4; i++, tail +=4) {
    U64 b;
    memcpy(&b, tail, sizeof(b));

    b = (s64 ^ b) * p1;
    s64 = ((b << 23) | (b >> 41))*p2 + p3;
  }

  // up to 3 bytes remain, process 0-3 1 byte blocks
  for (S64 i=0;i<(len & 3); i++,tail++) {
    U64 b = s64 ^ (*tail)*p5;
    s64 = ((b << 11) | (b >> 53))*p1;
  }

  // finalization mix
  s64 =  (s64 ^ (s64 >> 33))*p2;
  s64 =  (s64 ^ (s64 >> 29))*p3;
  return (s64 ^ (s64 >> 32)); 
}

// bijective hash function

U64 u64_hash_bij(U64 x) {
    x = (x ^ (x >> 30ULL)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27ULL)) * 0x94d049bb133111ebULL;
    x = x ^ (x >> 31ULL);
    return(x);
}

U64 u64_unhash_bij(U64 x) {
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
