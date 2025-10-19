#define BASE_IMPL
#include "base.h"

typedef struct Queue Queue;
struct Queue {
    Queue *next;
    int val;
};

int main(void) {
    int ret = 0;
    Arena a = arena_create(MiB);
    Arena *ap = &a;

    Queue *first = 0, *last = 0, *node = 0;
    node = make(ap, Queue);
    node->val = 0;
    queue_push(first, last, node);

    node = make(ap, Queue);
    node->val = 1;
    queue_push(first, last, node);

    node = make(ap, Queue);
    node->val = 2;
    queue_push(first, last, node);

    if (first->val != 0) {
        ret = 1;
        goto cleanup;
    }
    queue_pop(first, last);

    if (first->val != 1) {
        ret = 1;
        goto cleanup;
    }
    queue_pop(first, last);

    if (first->val != 2) {
        ret = 1;
        goto cleanup;
    }
    queue_pop(first, last);

cleanup:
    arena_free(ap);
    return(ret);
}
