#include "vector.h"
#include <stdlib.h>
#include <assert.h>

struct _vector_data {
    void** data;
    size_t capacity, size;
};

void _vector_resize_impl(struct vector_s* obj, size_t const new_size)
{
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(obj);
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

void _vector_append_impl(struct vector_s* obj, void const* a)
{
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(obj);
    if(this->size >= this->capacity) {
        this->data = (void**)realloc(this->data, sizeof(void*) * this->capacity << 1);
        this->capacity <<= 1;
    }
    this->data[this->size++] = (void*)a;
}

void _vector_set_impl(struct vector_s* obj, size_t const idx, void const* a)
{
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(obj);
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

void* _vector_get_impl(struct vector_s* obj, size_t const idx)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    if(idx >= this->size) return NULL;
    return this->data[idx];
}

void** _vector_array_impl(struct vector_s* obj)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    return this->data;
}

vector_t* new_vector()
{
    vector_t* ret = (vector_t*)malloc(sizeof(vector_t));
    struct _vector_data* d = (struct _vector_data*)malloc(sizeof(struct _vector_data));
    d->capacity = 2;
    d->size = 0;
    d->data = (void**)malloc(sizeof(void*) * 2);
    ret->_data = d;

    ret->resize = _vector_resize_impl;
    ret->append = _vector_append_impl;
    ret->set = _vector_set_impl;
    ret->get = _vector_get_impl;
    ret->array = _vector_array_impl;

    return ret;
}

vector_t* new_vector_of(size_t initialCapacity)
{
    size_t i;
    vector_t* ret = (vector_t*)malloc(sizeof(vector_t));
    struct _vector_data* d = (struct _vector_data*)malloc(sizeof(struct _vector_data));
    ret->_data = d;
    d->capacity = d->size = initialCapacity;
    d->data = (void**)malloc(sizeof(void*) * initialCapacity);
    for(i = 0; i < initialCapacity; ++i) {
        d->data[i] = NULL;
    }

    ret->resize = _vector_resize_impl;
    ret->append = _vector_append_impl;
    ret->set = _vector_set_impl;
    ret->get = _vector_get_impl;
    ret->array = _vector_array_impl;

    return ret;
}

void delete_vector(vector_t** this)
{
    if(!*this) return;
    if((*this)->_data) {
        struct _vector_data* d = (struct _vector_data*)(*this)->_data;
        if(d->data) free(d->data);
        free((*this)->_data);
    }
    free(*this);
    *this = NULL;
}
