#define BASE_IMPL
#include "base.h"

typedef struct DLL DLL;
struct DLL {
    DLL *prev, *next;
    int val;
};

int main(void) {
    int ret = 0;
    Arena *a = arena_create(MiB);

    DLL *first = 0, *last = 0, *node = 0;

    node = make(a, DLL);
    node->val = 0;
    dll_pushback(first, last, node);

    node = make(a, DLL);
    node->val = 1;
    dll_pushback(first, last, node);

    node = make(a, DLL);
    node->val = 2;
    dll_pushback(first, last, node);

    node = make(a, DLL);
    node->val = 3;
    dll_pushback(first, last, node);

    if (first->val != 0) {
        ret = 1;
        goto cleanup;
    }
    
    dll_remove_first(first, last);
    if (first->val != 1) {
        ret = 1;
        goto cleanup;
    }

    if (last->val != 3) {
        ret = 1;
        goto cleanup;
    }

    dll_remove_last(first, last);
    if (last->val != 2) {
        ret = 1;
        goto cleanup;
    }

    node = make(a, DLL);
    node->val = 3;
    dll_insert_after(first, last, first, node);
    if (first->next->val != 3) {
        ret = 1;
        goto cleanup;
    }

    node = make(a, DLL);
    node->val = 4;
    dll_insert_before(first, last, last, node);
    if (last->prev->val != 4) {
        ret = 1;
        goto cleanup;
    }

cleanup:
    arena_free(a);
    return(ret);
}
