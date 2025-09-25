#ifndef CHAINED_ARENA_H
#define CHAINED_ARENA_H

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define KiB(x) ((x)*1024)
#define MiB(x) ((x)*1024*1024)
#define GiB(x) ((x)*1024*1024*1024)


#ifndef CA_ALIGNOF
#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#define CA_ALIGNOF sizeof
#elif __STDC_VERSION__ < 202311L
#define CA_ALIGNOF _Alignof
#else
#define CA_ALIGNOF alignof
#endif // __STDC_VERSION__
#endif // CA_ALIGNOF


// CONFIG: You can freely tweak the size of the scratch chained_arenas
#ifndef CHAINED_ARENA_SCRATCH_CAPACITY
#define CHAINED_ARENA_SCRATCH_CAPACITY MiB(5)
#endif // CHAINED_ARENA_SCRATCH_CAPACITY

typedef struct chained_arena {
    char *base;
    int64_t capacity;
    int64_t offset;
    struct chained_arena *next;
} chained_arena;

//#ifndef chained_arena_alloc
#define chained_arena_alloc(a, type) chained_arena_alloc_raw(a, sizeof(type), 1, CA_ALIGNOF(type))
//#endif // chained_arena_alloc

//#ifndef chained_arena_alloc_m
#define chained_arena_alloc_m(a, type, n) chained_arena_alloc_raw(a, sizeof(type), n, CA_ALIGNOF(type))
//#endif // chained_arena_alloc_m

void *chained_arena_alloc_raw(chained_arena *a, int64_t size, int64_t count, int64_t alignment);
chained_arena *chained_arena_get_scratch(void);
chained_arena *chained_arena_make(int64_t capacity);
void chained_arena_reset(chained_arena *a);
void chained_arena_destroy(chained_arena *a);
bool is_power_of_2(uint64_t x);


#ifdef CHAINED_ARENA_IMPL

bool is_power_of_2(uint64_t x) {
    if (!x) {
        return false;
    }
    return (x & (x - 1)) == 0;
}

chained_arena *chained_arena_get_scratch(void) {
    static chained_arena *scratch1 = NULL;
    static chained_arena *scratch2 = NULL;
    if (!scratch1) {
        scratch1 = chained_arena_make(CHAINED_ARENA_SCRATCH_CAPACITY);
        scratch2 = chained_arena_make(CHAINED_ARENA_SCRATCH_CAPACITY);
    }
    static bool toggle = true;
    if (toggle) {
        toggle = !toggle;
        return scratch1;
    } else {
        toggle = !toggle;
        return scratch2;
    }
}

chained_arena *chained_arena_make(int64_t capacity) {
    chained_arena *a = malloc(sizeof(chained_arena));
    memset(a, 0, sizeof(*a));
    assert(a->next == NULL);

    void *p = malloc(capacity);
    if (!p) {
        return NULL;
    }
    a->base = p;
    a->capacity = capacity;
    return a;
}

void chained_arena_reset(chained_arena *a) {
    if (a->next) {
        chained_arena_reset(a->next);
    }
    a->offset = 0;
}

void chained_arena_destroy(chained_arena *a) {
    if (a->next) {
        chained_arena_destroy(a->next);
    }
    free(a->base);
    free(a);
}

void *chained_arena_alloc_raw(chained_arena *a, int64_t size, int64_t count, int64_t alignment) {
    if (!alignment || !is_power_of_2(alignment)) {
        return NULL;
    }
    int64_t padding = ((uintptr_t)a->base + (uintptr_t)a->offset) % alignment;
    int64_t total_size = size * count;
    if (total_size < size || total_size < count) {
        assert(false && "integer overflow in size calculation");
        return NULL;
    }
    if (a->offset + padding + total_size > a->capacity) {
        if (!a->next) {
            int64_t new_capacity = a->capacity;
            while (total_size > new_capacity) {
                new_capacity *= 2;
            }
            a->next = chained_arena_make(new_capacity); 
        }
        return chained_arena_alloc_raw(a->next, size, count, alignment);
    } else {
        a->offset += padding;
        void *p = (void *)((uintptr_t)a->base + (uintptr_t)a->offset);
        memset(p, 0, total_size);
        a->offset += total_size;
        return p;
    }
}

#endif // CHAINED_ARENA_IMPL
#endif // CHAINED_ARENA_H
