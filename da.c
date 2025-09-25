
#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

#define da_reserve(da, expected_cap)                                                               \
    do {                                                                                           \
        if ((expected_cap) > (da)->cap) {                                                          \
            if ((da)->cap == 0) {                                                                  \
                (da)->cap = DA_INIT_CAP;                                                           \
            }                                                                                      \
            while ((expected_cap) > (da)->cap) {                                                   \
                (da)->cap *= 2;                                                                    \
            }                                                                                      \
            (da)->buf = DECLTYPE_CAST((da)->buf)REALLOC((da)->buf, (da)->cap * sizeof(*(da)->buf));\
            ASSERT((da)->buf && "Buy more RAM lol");                                               \
        }                                                                                          \
    } while (0)

// Append an item to a dynamic array
#define da_append(da, item)                    \
    do {                                       \
        da_reserve((da), (da)->len + 1);       \
        (da)->buf[(da)->len++] = (item);       \
    } while (0)

#define da_free(da) FREE((da).buf)

// Append several buf to a dynamic array
#define da_append_many(da, new_buf, new_buf_len)                                                \
    do {                                                                                        \
        da_reserve((da), (da)->len + (new_buf_len));                                            \
        memcpy((da)->buf + (da)->len, (new_buf), (new_buf_len)*sizeof(*(da)->buf));             \
        (da)->len += (new_buf_len);                                                             \
    } while (0)

#define da_resize(da, new_size)         \
    do {                                \
        da_reserve((da), new_size);     \
        (da)->len = (new_size);         \
    } while (0)

#define da_last(da) (da)->buf[(ASSERT((da)->len > 0), (da)->len-1)]

#define da_remove_unordered(da, i)                   \
    do {                                             \
        size_t j = (i);                              \
        ASSERT(j < (da)->len);                       \
        (da)->buf[j] = (da)->buf[--(da)->len];       \
    } while(0)

#define da_foreach(da, type, it)\
    for (type *it = (da)->buf; it < (da)->buf + (da)->len; ++it)
