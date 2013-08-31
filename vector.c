#include "vector.h"
#include <stdlib.h>

void _vector_resize_impl(struct vector_s* this, size_t const new_size)
{
    assert(this);
    if(this->size != new_size && this->capacity < new_size) {
        this->data = (void**)realloc(this->data, sizeof(void*) * new_size);
        this->capacity = new_size;
    }
    if(new_size > this->size) {
        size_t i = this->size;
        for(; i < new_size; ++i) {
            this->data[i] = NULL;
        }
    }
    this->size = new_size;
}

void _vector_append_impl(struct vector_s* this, void const* a)
{
    assert(this);
    if(this->size >= this->capacity) {
        this->data = (void**)realloc(this->data, sizeof(void*) * this->capacity << 1);
        this->capacity <<= 1;
    }
    this->data[this->size++] = (void*)a;
}

void _vector_set_impl(struct vector_s* this, size_t const idx, void const* a)
{
    if(idx >= this->capacity) {
        this->data = (void**)realloc(this->data, sizeof(void*) * this->capacity << 1);
        this->capacity <<= 1;
    }

    if(idx >= this->size) {
        size_t i;

        for(i = this->size; i < idx; ++i) {
            this->data[i] = NULL;
        }

        this->size = idx + 1;
    }

    this->data[idx] = (void*)a;
}

void* _vector_get_impl(struct vector_s* this, size_t const idx)
{
    assert(this);
    if(idx >= this->size) return NULL;
    return this->data[idx];
}

vector_t* new_vector()
{
    vector_t* ret = (vector_t*)malloc(sizeof(vector_t));
    ret->capacity = 2;
    ret->size = 0;
    ret->data = (void**)malloc(sizeof(void*) * 2);
    return ret;
}

vector_t* new_vector_of(size_t initialCapacity)
{
    size_t i;
    vector_t* ret = (vector_t*)malloc(sizeof(vector_t));
    ret->capacity = ret->size = initialCapacity;
    ret->data = (void**)malloc(sizeof(void*) * initialCapacity);
    for(i = 0; i < initialCapacity; ++i) {
        ret->data[i] = NULL;
    }

    ret->resize = _vector_resize_impl;
    ret->append = _vector_append_impl;
    ret->set = _vector_set_impl;
    ret->get = _vector_get_impl;

    return ret;
}

void delete_vector(vector_t** this)
{
    if(!*this) return;
    if((*this)->data) free((*this)->data);
    free(*this);
    *this = NULL;
}
