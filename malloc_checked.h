#include <assert.h>
#include <stddef.h>
#include <malloc.h>

#define NEW(object, type) \
    do {\
        object = malloc(sizeof(type));\
        assert(object && "allocation failed");\
    } while (0)

#define DELETE(object) \
    do {\
        if (object) free(object);\
        object = NULL;\
    } while (0)

void *malloc_checked(int bytes) {
    void *ptr = malloc(bytes);
    assert(ptr);
}

#define free_checked(object) \
    do {\
        assert(object && "double free"); \
        free(object); \
        object = NULL; \
    } while (0)
