#include "stack.h"
#include "vector.h"
#include <assert.h>
#include <stdlib.h>

#define STACK_MAGIC_NUMBER (256)

typedef struct {
    vector_t* a;
    size_t peak;
} _stack_data_t;

static void _stack_clear_impl(struct stack_s* this)
{
    assert(this);
    _stack_data_t* d = (_stack_data_t*)this->_data;
    if(d->a->size(d->a) > STACK_MAGIC_NUMBER) {
        d->a->resize(d->a, STACK_MAGIC_NUMBER);
        d->a->shrink(d->a);
    }
    d->peak = 0;
}

static void _stack_push_impl(struct stack_s* this, void const* a)
{
    assert(this);
    _stack_data_t* d = (_stack_data_t*)this->_data;

    d->a->set(d->a, d->peak++, a);
}

static void* _stack_pop_impl(struct stack_s* this)
{
    assert(this);
    _stack_data_t* d = (_stack_data_t*)this->_data;
    if(d->peak) {
        return d->a->get(d->a, --d->peak);
    } else {
        return NULL;
    }
}

static size_t _stack_size_impl(struct stack_s* this)
{
    assert(this);

    _stack_data_t* d = (_stack_data_t*)this->_data;
    return d->peak;
}

static int _stack_empty_impl(struct stack_s* this)
{
    assert(this);
    _stack_data_t* d = (_stack_data_t*)this->_data;
    return d->peak == 0;
}

stack_t* new_stack()
{
    stack_t* ret = (stack_t*)malloc(sizeof(stack_t));
    _stack_data_t* data;
    ret->_data = data = (_stack_data_t*)malloc(sizeof(_stack_data_t));
    data->a = new_vector();
    data->peak = 0;

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
    _stack_data_t* data = (_stack_data_t*)(*this)->_data;
    delete_vector(&data->a);
    free(data);
    free(*this);
    *this = NULL;
}
