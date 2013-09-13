#include "stack.h"
#include <assert.h>
#include <stdlib.h>

struct _stack_data {
    struct _stack_data* prev;
    void const* data;
};

static void _stack_clear_impl(struct stack_s* this)
{
    assert(this);
    struct _stack_data* d = (struct _stack_data*)this->_data;
    while(d) {
        struct _stack_data* prev = d->prev;
        free(d);
        d = prev;
    }
}

static void _stack_push_impl(struct stack_s* this, void const* a)
{
    assert(this);
    struct _stack_data* d = (struct _stack_data*)malloc(sizeof(struct _stack_data));

    if(this->_data) {
        struct _stack_data* prev = (struct _stack_data*)this->_data;
        d->prev = prev;
    } else {
        d->prev = NULL;
    }

    this->_data = d;
    d->data = a;
}

static void* _stack_pop_impl(struct stack_s* this)
{
    assert(this);
    if(this->_data) {
        struct _stack_data* d = (struct _stack_data*)this->_data;
        void* ret = (void*)d->data;
        this->_data = d->prev;
        return ret;
    } else {
        return NULL;
    }
}

static size_t _stack_size_impl(struct stack_s* this)
{
    assert(this);

    struct _stack_data* d = (struct _stack_data*)this->_data;
    size_t size = 0;

    while(d) {
        size++;
        d = d->prev;
    }

    return size;
}

static int _stack_empty_impl(struct stack_s* this)
{
    assert(this);
    struct _stack_data* d = (struct _stack_data*)this->_data;
    return d == NULL;
}

stack_t* new_stack()
{
    stack_t* ret = (stack_t*)malloc(sizeof(stack_t));
    ret->_data = NULL;

    ret->push = &_stack_push_impl;
    ret->pop = &_stack_pop_impl;
    ret->size = &_stack_size_impl;
    ret->empty = &_stack_empty_impl;
    ret->clear = &_stack_clear_impl;

    return ret;
}

void delete_stack(stack_t** this)
{
    if(!*this) return;
    _stack_clear_impl(*this);
    free(*this);
    *this = NULL;
}
