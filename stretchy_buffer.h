#ifndef STRETCHY_BUFFER_H
#define STRETCHY_BUFFER_H
#include <stdint.h>

typedef struct {
    int64_t len;
    int64_t cap;
    char buf[];
} SbHeader;

#define sb__header_get(sb) ((SbHeader *)((char *)sb - offsetof(SbHeader, buf)))
#define sb__fits(sb, n) (sb_len(sb) + (n) <= sb_cap(sb))
#define sb__fit(sb, n)\
    (sb__fits((sb), (n)) ?\
            0 :\
            ((sb) = sb__grow((sb), sb_len(sb) + (n), sizeof(*(sb)))))

#define sb_len(sb) ((sb) ? sb__header_get(sb)->len : 0)
#define sb_cap(sb) ((sb) ? sb__header_get(sb)->cap : 0)
#define sb_push(sb, e)\
    (sb__fit((sb), 1),\
     sb[sb_len(sb)] = (e),\
     ++(sb__header_get(sb)->len))
#define sb_reserve(sb, n) sb__fit((sb), (n)-sb__header_get(sb)->len)
#define sb_clear(sb) sb__header_get(sb)->len = 0
#define sb_free(sb) free((sb) ? sb__header_get(sb) : 0)

void *sb__grow(void *sb, int64_t new_len, int64_t element_size);

#endif // STRETCHY_BUFFER_H

#ifdef STRETCHY_BUFFER_IMPL

void *sb__grow(void *sb, int64_t new_len, int64_t element_size) {
    int64_t new_cap = max(1 + 2*sb_cap(sb), new_len);
    assert(new_len <= new_cap);
    int64_t new_size = offsetof(SbHeader, buf) + new_cap*element_size;
    SbHeader *new_sb = 0;
    if (sb) {
        int64_t len = sb__header_get(sb)->len;
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

#endif // STRETCHY_BUFFER_IMPL
