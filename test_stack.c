#define BASE_IMPL
#include "base.h"

typedef struct Stack Stack;
struct Stack {
    int val;
    Stack *next;
};

int main(void) {
    int ret = 0;
    Arena a = arena_create(MiB);

    Stack *node = 0;
    Stack *top = make(&a, Stack, 1);
    top->val = 0;

    node = make(&a, Stack);
    node->val = 1;
    stack_push(top, node);

    node = make(&a, Stack);
    node->val = 2;
    stack_push(top, node);

    if (top->val != 2) {
        ret = 1;
        goto cleanup;
    }
    stack_pop(top);

    if (top->val != 1) {
        ret = 1;
        goto cleanup;
    }
    stack_pop(top);

    if (top->val != 0) {
        ret = 1;
        goto cleanup;
    }
    stack_pop(top);

cleanup:
    arena_free(&a);
    return(ret);
}
