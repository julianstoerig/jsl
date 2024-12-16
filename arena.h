#ifndef JSL_ARENA_H
#define JSL_ARENA_H

#include "bit_fiddling.h"
#include "assert.h"

#define   sizeof(x)  (ptrdiff_t)sizeof(x)
#define  countof(a)  (sizeof(a) / sizeof(*(a)))
#define  cstrlen(s)  (countof(s) - 1)

typedef struct {
    char* beg;
    char* rst;
    char* end;
} Arena;

typedef enum {
    ArenaFlags_None           = 0,
    ArenaFlags_NoAbort        = BIT(0),
    ArenaFlags_NoZero         = BIT(1),
    ArenaFlags_NoResizeTODO   = BIT(2),
} ArenaFlags;

#define new(...)            newx(__VA_ARGS__,new4,new3,new2)(__VA_ARGS__)
#define newx(arenaPtr, b,c,d,e,...) e
#define new2(arenaPtr, type)                  (type *)alloc(arenaPtr, sizeof(type), alignof(type), 1,     ArenaFlags_None)
#define new3(arenaPtr, type, count)           (type *)alloc(arenaPtr, sizeof(type), alignof(type), count, ArenaFlags_None)
#define new4(arenaPtr, type, count, flags)    (type *)alloc(arenaPtr, sizeof(type), alignof(type), count, flags)

#ifdef JSL_ARENA_IMPLEMENTATION

#ifndef JSL_ARENA_OOM_POLICY
#define JSL_ARENA_OOM_POLICY abort
#endif

Arena ArenaCreate(ptrdiff_t cap) {
    REQUIRE(cap > 0);
    Arena a = {};
    a.beg = malloc(cap);
    a.rst = a.beg;
    a.end = a.beg ? a.beg + cap : nullptr;
    return a;
}

void* alloc(Arena* a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count, ArenaFlags flags) {
    REQUIRE(a);
    REQUIRE(size > 0);
    REQUIRE(align > 0);
    REQUIRE(count > 0);

    // bitmasking modulo, since align ∈ { 2^n | n ∈ ℕ }
    ptrdiff_t padding = (uintptr_t)a->beg & (align - 1);
    ptrdiff_t available = (uintptr_t)a->end - padding; 
    if (available < 0 || count > available / size) {
        if (!(flags & ArenaFlags_NoAbort)) {
            JSL_ARENA_OOM_POLICY();
        } else {
            return nullptr;
        }
    }
    void *p = a->beg + padding;
    a->beg += padding + count * size;
    return (flags & ArenaFlags_NoZero) ? p : memset(p, 0, count * size);

    ENSURE(a);
}

void reset(Arena* a) {
    a->beg = a->rst;
}

#endif
#endif
