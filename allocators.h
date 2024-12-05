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

void *CHECKED_MALLOC(int bytes) {
    void *ptr = malloc(bytes);
    assert(ptr);
}

#define CHECKED_FREE(object) \
    do {\
        assert(object && "double free"); \
        free(object); \
        object = NULL; \
    } while (0)
