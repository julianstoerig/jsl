#include <assert.h>
#include <stddef.h>
#include <malloc.h>

#define NEW(object, type) \
    do {\
    object = malloc(sizeof(type));\
    assert(object && "allocation failed");\
    } while (0)

#define DELETE(object) free(object)

void *checkedMalloc(int bytes) {
    void *ptr = malloc(bytes);
    assert(ptr);
}

void checkedFree(void *ptr) {
    assert(ptr);
    free(ptr);
}
