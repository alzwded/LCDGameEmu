#include "stack.h"

struct _stack_data {
    size_t capacity;
    size_t size;
    void** elements;
};

void _stack_push_impl(struct stack_s* this, void const* a)
{
    struct _stack_data* d = (struct _stack_data*)this->_data;
    if(d->size >= d->capacity) {
        d->elements = (void**)realloc(d->elements, d->capacity << 1);
        d->capacity <<= 1;
    }
    d->elements[d->size++] = (void*)a;
}

void* _stack_pop_impl(struct stack_s* this)
{
    struct _stack_data* d = (struct _stack_data*)this->_data;
    if(d->size == 0) return NULL;
    return d->elements[d->size--];
}

size_t _stack_size_impl(struct stack_s* this)
{
    struct _stack_data* d = (struct _stack_data*)this->_data;
    return d->size;
}

stack_t* new_stack()
{
    stack_t* ret = (stack_t*)malloc(sizeof(stack_t));
    ret->_data = (struct _stack_data*)malloc(sizeof(struct _stack_data));

    struct _stack_data* d = (struct _stack_data*)ret->_data;
    d->capacity = 2;
    d->size = 0;
    d->elements = (void**)malloc(sizeof(void*) * 2);

    ret->push = &_stack_push_impl;
    ret->pop = &_stack_pop_impl;
    ret->size = &_stack_size_impl;

    return ret;
}

void delete_stack(stack_t** this)
{
    if(!*this) return;
    struct _stack_data* d = (struct _stack_data*)(*this)->_data;
    if(d) {
        if(d->elements) free(d->elements);
        free(d);
    }
    free(*this);
    *this = NULL;
}
