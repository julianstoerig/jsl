#include "base.h"


typedef struct {
    __DYNAMIC_ARRAY_HEADER__;
    S64 *v;
} TestArray;


int main(void) {
    TestArray arr = {0};
    da_push(arr, 42);
    if (da_get(arr, 0) != 42)
        return(1);

    if (da_len(arr) != 1)
        return(1);

    da_clear(arr);
    
    if (da_len(arr))
        return(1);

    S64 len = 443783;
    for (S64 i=0; i<len; ++i)
        da_push(arr, i*i);

    if (da_len(arr) != len)
        return(1);

    for (S64 i=0; i<da_len(arr); ++i)
        if (da_get(arr, i) != i*i)
            return(1);

    da_clear(arr);
    da_push(arr, 42);
    da_push(arr, 13);
    if (da_pop(arr) != 13)
        return(1);
    if (da_pop(arr) != 42)
        return(1);

    da_push(arr, 69);
    da_push(arr, 131);
    if (da_get(arr, 0) != 69)
        return(1);
    da_del(arr, 0);
    if (da_get(arr, 0) != 131)
        return(1);

    da_clear(arr);

    da_push(arr, 1);
    da_push(arr, 2);
    da_push(arr, 3);

    da_delswap(arr, 0);
    if (da_get(arr, 0) != 3)
        return(1);
    da_delswap(arr, 0);
    if (da_get(arr, 0) != 2)
        return(1);
    da_delswap(arr, 0);
    if (da_len(arr))
        return(1);


    da_clear(arr);

    da_push(arr, 4);
    da_push(arr, 5);
    da_push(arr, 6);

    da_delswap(arr, 2);
    if (da_get(arr, 0) != 4 || da_get(arr, 1) != 5)
        return(1);

    return(0);
}
