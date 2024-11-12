#ifndef VEC_H
#define VEC_H

#include "stdio.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define vec_get_header(vec) ((struct vec_header *)(vec)-1)

#define vec_len(vec) ((vec) == NULL ? 0 : vec_get_header(vec)->len)

#define vec_cap(vec) ((vec) == NULL ? 0 : vec_get_header(vec)->cap)

#define vec_reserve(vec, cap)                                                  \
    do {                                                                       \
        (vec) = vec_reserve_sized((vec), cap, sizeof(*(vec)));                 \
    } while (0)

#define vec_resize(vec, len)                                                   \
    do {                                                                       \
        (vec) = vec_resize_sized((vec), len, sizeof(*(vec)));                  \
    } while (0)

#define vec_grow_cap(vec, grow)                                                \
    do {                                                                       \
        (vec) = vec_grow_cap_sized((vec), grow, sizeof(*(vec)));               \
    } while (0)

#define vec_push(vec, item)                                                    \
    do {                                                                       \
        vec_grow_cap((vec), vec_len(vec) + 1);                                 \
        (vec)[vec_len(vec)] = (item);                                          \
        vec_get_header(vec)->len++;                                            \
    } while (0)

#define vec_pop(vec)                                                           \
    ((vec) = vec_resize_sized((vec), vec_len(vec) - 1, sizeof(*(vec))),        \
     (vec)[vec_len(vec)])

#define vec_remove(vec, at, len)                                               \
    do {                                                                       \
        vec_remove_sized((vec), (at), (len), sizeof(*(vec)));                  \
    } while (0)

#define vec_append(vec, items, n)                                              \
    do {                                                                       \
        size_t __n = (n);                                                      \
        vec_grow_cap((vec), vec_len(vec) + __n);                               \
        memcpy(vec + vec_len(vec), items, __n * sizeof(*(items)));             \
        vec_get_header(vec)->len += n;                                         \
    } while (0)

#define vec_insert(vec, at, items, n)                                          \
    do {                                                                       \
        size_t __n = (n);                                                      \
        size_t __at = (at);                                                    \
        (vec) = vec_insert_sized((vec), (items), __at, __n, sizeof(*(items))); \
    } while (0)

#define vec_free(vec)                                                          \
    do {                                                                       \
        if ((vec) != NULL) {                                                   \
            free(vec_get_header(vec));                                         \
        }                                                                      \
        (vec) = NULL;                                                          \
    } while (0)

typedef struct vec_header {
    size_t len;
    size_t cap;
} vec_header;

static inline void *vec_reserve_sized(void *vec, size_t cap, size_t item_size) {
    if (cap == 0) {
        vec_free(vec);
        return NULL;
    } else if (vec == NULL) {
        vec_header *header =
            (vec_header *)malloc(sizeof(vec_header) + cap * item_size);
        header->cap = cap;
        header->len = 0;
        return header + 1;
    } else {
        vec_header *header = vec_get_header(vec);
        if (header->cap == cap) {
            return vec;
        }
        header =
            (vec_header *)realloc(header, sizeof(vec_header) + cap * item_size);
        header->cap = cap;
        return header + 1;
    }
}

static inline void *vec_resize_sized(void *vec, size_t len, size_t item_size) {
    if (vec_len(vec) < len) {
        vec = vec_reserve_sized(vec, len, item_size);
    }

    if (vec != NULL) {
        vec_get_header(vec)->len = len;
    }

    return vec;
}

static inline void *vec_grow_cap_sized(void *vec, size_t to, size_t item_size) {
    if (to < vec_cap(vec)) {
        return vec;
    }

    size_t new_cap = 2;

    while (new_cap < to) {
        new_cap = new_cap + new_cap / 2;
    }

    vec = vec_reserve_sized(vec, new_cap, item_size);

    return vec;
}

static inline void vec_remove_sized(void *vec, size_t at, size_t len,
                                    size_t item_size) {
    if (len == 0) {
        return;
    }

    memmove((char *)vec + at * item_size, (char *)vec + (at + len) * item_size,
            (vec_len(vec) - at - len) * item_size);

    vec_get_header(vec)->len -= len;
}

static inline void *vec_insert_sized(void *vec, void *data, size_t at,
                                     size_t len, size_t item_size) {
    if (len == 0) {
        return vec;
    }

    vec = vec_grow_cap_sized(vec, vec_len(vec) + len, item_size);

    if (vec_len(vec) != 0 && at < vec_len(vec)) {
        memmove((char *)vec + (at + len) * item_size,
                (char *)vec + at * item_size,
                (vec_len(vec) - at - len + 2) * item_size);
    }

    memcpy((char *)vec + at * item_size, data, len * item_size);

    vec_get_header(vec)->len += len;

    return vec;
}

#endif