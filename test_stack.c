#define BASE_IMPL
#include "base.h"

typedef struct Stack Stack;
struct Stack {
    int val;
    Stack *next;
};

int main(void) {
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

    if (top->val != 2)
        goto cleanup;
    stack_pop(top);
    if (top->val != 1)
        goto cleanup;
    stack_pop(top);
    if (top->val != 0)
        goto cleanup;

cleanup:
    arena_free(&a);
    return(0);
}
