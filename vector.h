#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef int (*comp_func_t)(void const*, void const*);

typedef struct vector_s {
    void* _data;
    size_t (*size)(struct vector_s*);
    void (*resize)(struct vector_s*, size_t const);
    void (*append)(struct vector_s*, void const*);
    void (*set)(struct vector_s*, size_t const, void const*);
    void* (*get)(struct vector_s*, size_t const);
    void** (*array)(struct vector_s*);
    void const** (*const_array)(struct vector_s*);
    void (*sort)(struct vector_s*, comp_func_t);
    void const** (*bsearch)(struct vector_s*, void const* key, comp_func_t);
    void* (*find)(struct vector_s*, void const* key, comp_func_t);
    void (*shrink)(struct vector_s*);
} vector_t;

vector_t* new_vector();
vector_t* new_vector_of(size_t initialCapacity);
void delete_vector(vector_t**);

#endif
