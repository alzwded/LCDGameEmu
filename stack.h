#ifndef STACK_H
#define STACK_H

#include <stddef.h>

typedef struct stack_s {
    void* _data;
    void (*push)(struct stack_s*, void const*);
    void* (*pop)(struct stack_s*);
    size_t (*size)(struct stack_s*);
    int (*empty)(struct stack_s*);
    void (*clear)(struct stack_s*);
} stack_t;

stack_t* new_stack();
void delete_stack(stack_t**);

#endif
