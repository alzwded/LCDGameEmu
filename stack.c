#include "stack.h"
#include <assert.h>

struct _stack_data {
    struct _stack_data* prev;
    void const* data;
};

void _stack_push_impl(struct stack_s* this, void const* a)
{
    struct _stack_data* d = (struct _stack_data*)malloc(sizeof(struct _stack_data));
    assert(this);

    if(this->_data) {
        struct _stack_data* prev = (struct _stack_data*)this->_data;
        d->prev = prev;
    } else {
        d->prev = NULL;
    }

    this->_data = d;
    d->data = a;
}

void* _stack_pop_impl(struct stack_s* this)
{
    assert(this);
    if(this->_data) {
        struct _stack_data* d = (struct _stack_data*)this->_data;
        void* ret = (void*)d->data;
        this->_data = d->prev;
    } else {
        return NULL;
    }
}

size_t _stack_size_impl(struct stack_s* this)
{
    struct _stack_data* d = (struct _stack_data*)this->_data;
    size_t size = 0;

    assert(this);

    while(d) {
        size++;
        d = d->prev;
    }

    return size;
}

stack_t* new_stack()
{
    stack_t* ret = (stack_t*)malloc(sizeof(stack_t));
    ret->_data = NULL;

    ret->push = &_stack_push_impl;
    ret->pop = &_stack_pop_impl;
    ret->size = &_stack_size_impl;

    return ret;
}

void delete_stack(stack_t** this)
{
    if(!*this) return;
    struct _stack_data* d = (struct _stack_data*)(*this)->_data;
    while(d) {
        struct _stack_data* prev = d->prev;
        free(d);
        d = prev;
    }
    free(*this);
    *this = NULL;
}
