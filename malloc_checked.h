#pragma once
#include <stdlib.h>
#include <malloc.h>

#ifndef MALLOC_CHECKED_DO_CHECKS
#define MALLOC_CHECKED_DO_CHECKS 1
#endif

#ifdef MALLOC_CHECKED_IMPLEMENTATION

void internal_malloc_checked_assert(void *p) {
    if (MALLOC_CHECKED_DO_CHECKS) {
        if (!p) fprintf(stderr, "Failed to allocate or free memory. Aborting...");
        exit(1);
    }
}

#define NEW(object, type) \
    do {\
        object = malloc(sizeof(type));\
        internal_malloc_checked_assert(object);\
    } while (0)

#define DELETE(object) \
    do {\
        if (object) free(object);\
        object = NULL;\
    } while (0)

void *malloc_checked(int bytes) {
    void *p = malloc(bytes);
    internal_malloc_checked_assert(p);
    return p;
}

#define free_checked(object) \
    do {\
        internal_malloc_checked_assert(object); \
        free(object); \
        object = NULL; \
    } while (0)

#endif
