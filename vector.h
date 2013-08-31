#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct vector_s {
    void** data;
    size_t capacity, size;
    void (*resize)(struct vector_s*, size_t const);
    void (*append)(struct vector_s*, void const*);
    void (*set)(struct vector_s*, size_t const, void const*);
    void* (*get)(struct vector_s*, size_t const);
} vector_t;

vector_t* new_vector();
vector_t* new_vector_of(size_t initialCapacity);
void delete_vector(vector_t**);

#endif
