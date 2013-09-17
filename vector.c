#include "vector.h"
#include <stdlib.h>
#include <assert.h>
#include <memory.h>

struct _vector_data {
    void** data;
    size_t capacity, size;
    unsigned char sorted;
};

static size_t _vector_size_impl(struct vector_s* this)
{
    assert(this);
    struct _vector_data* obj = (struct _vector_data*)this->_data;
    assert(obj);
    return obj->size;
}

static void _vector_resize_impl(struct vector_s* obj, size_t const new_size)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(this);
    if(this->size != new_size && this->capacity < new_size) {
        this->data = (void**)realloc(this->data, sizeof(void*) * new_size);
        this->capacity = new_size;
    }
    if(new_size > this->size) {
        memset(&this->data[this->size], 0, new_size - this->size);
    }
    this->size = new_size;
    this->sorted = 0;
}

static void _vector_append_impl(struct vector_s* obj, void const* a)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(this);
    if(this->size >= this->capacity) {
        this->data = (void**)realloc(this->data, sizeof(void*) * this->capacity << 1);
        this->capacity <<= 1;
    }
    this->data[this->size++] = (void*)a;
    this->sorted = 0;
}

static void _vector_set_impl(struct vector_s* obj, size_t const idx, void const* a)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(this);
    if(idx >= this->capacity) {
        this->data = (void**)realloc(this->data, sizeof(void*) * this->capacity << 1);
        while(idx < this->capacity) {
            this->capacity <<= 1;
            assert(this->capacity);
        }
    }

    if(idx >= this->size) {
        size_t i;

        for(i = this->size; i > idx; ++i) {
            this->data[i] = NULL;
        }

        this->size = idx + 1;
    }

    this->data[idx] = (void*)a;
    this->sorted = 0;
}

static void* _vector_get_impl(struct vector_s* obj, size_t const idx)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(this);
    if(idx >= this->size) return NULL;
    return this->data[idx];
}

static void** _vector_array_impl(struct vector_s* obj)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(this);
    this->sorted = 0;
    return this->data;
}

static void const** _vector_const_array_impl(struct vector_s* obj)
{
    assert(obj);
    struct _vector_data* this = (struct _vector_data*)obj->_data;
    assert(this);
    /* don't touch sorted */
    return (void const**)this->data;
}

static void _vector_sort_impl(struct vector_s* this, comp_func_t func)
{
    assert(this);
    struct _vector_data* data = (struct _vector_data*)this->_data;
    assert(data);
    assert(data->data);
    qsort(data->data, data->size, sizeof(void*), func);
    data->sorted = 1;
}

static void const** _vector_bsearch_impl(struct vector_s* this, void const* key, comp_func_t func)
{
    assert(this);
    struct _vector_data* data = (struct _vector_data*)this->_data;
    assert(data);
    assert(data->data);
    void** found = bsearch(key, data->data, data->size, sizeof(void*), func);
    if(!found) return NULL;
    else return (void const**)found;
}

static void* _vector_find_impl(struct vector_s* this, void const* key, comp_func_t func)
{
    assert(this);
    struct _vector_data* d = (struct _vector_data*)this->_data;
    assert(d);
    if(d->sorted) {
        void const** found = this->bsearch(this, key, func);
        if(!found) return NULL;
        else return (void*)(*found);
    } else {
        size_t i = 0;
        for(; i < d->size; ++i) {
            if(func(key, d->data + i) == 0) {
                return (void*)(d->data + i);
            }
        }
        return NULL;
    }
}

static void _vector_shrink_impl(struct vector_s* this)
{
    assert(this);
    struct _vector_data* d = (struct _vector_data*)this->_data;
    assert(d);
    if(d->capacity > d->size) {
        d->data = (void**)realloc(d->data, d->size);
        d->capacity = d->size;
    }
}

vector_t* new_vector()
{
    vector_t* ret = (vector_t*)malloc(sizeof(vector_t));
    struct _vector_data* d = (struct _vector_data*)malloc(sizeof(struct _vector_data));
    d->capacity = 2;
    d->size = 0;
    d->data = (void**)malloc(sizeof(void*) * 2);
    d->sorted = 0;
    ret->_data = d;

    ret->size = &_vector_size_impl;
    ret->resize = &_vector_resize_impl;
    ret->append = &_vector_append_impl;
    ret->set = &_vector_set_impl;
    ret->get = &_vector_get_impl;
    ret->array = &_vector_array_impl;
    ret->const_array = _vector_const_array_impl;
    ret->sort = &_vector_sort_impl;
    ret->bsearch = &_vector_bsearch_impl;
    ret->find = &_vector_find_impl;
    ret->shrink = &_vector_shrink_impl;

    return ret;
}

vector_t* new_vector_of(size_t initialCapacity)
{
    vector_t* ret = (vector_t*)malloc(sizeof(vector_t));
    struct _vector_data* d = (struct _vector_data*)malloc(sizeof(struct _vector_data));
    ret->_data = d;
    d->sorted = 0;
    d->capacity = d->size = initialCapacity;
    d->data = (void**)malloc(sizeof(void*) * initialCapacity);
    memset(d->data, 0, initialCapacity);

    ret->size = _vector_size_impl;
    ret->resize = _vector_resize_impl;
    ret->append = _vector_append_impl;
    ret->set = _vector_set_impl;
    ret->get = _vector_get_impl;
    ret->array = _vector_array_impl;
    ret->const_array = _vector_const_array_impl;
    ret->sort = &_vector_sort_impl;
    ret->bsearch = &_vector_bsearch_impl;
    ret->find = &_vector_find_impl;
    ret->shrink = &_vector_shrink_impl;

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
