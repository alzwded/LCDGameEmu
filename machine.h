#ifndef MACHINE_H
#define MACHINE_H

#include <stdlib.h>
#include "code.h"
#include "stack.h"
#include "vector.h"
#include "inputbit.h"
#include "viewer.h"

typedef unsigned char reg_t;

typedef struct machine_s {
    reg_t registers[100];
    input_bit_field_t input;
    unsigned current_state;
    stack_t* stack;
    vector_t* sprite_state;
    game_t const* game;
    unsigned running;
    vector_t* viewers;

    void (*onclock)(struct machine_s*);
    void (*set_input)(struct machine_s*, input_bit_t, input_bit_state_t);
    void (*set_input_mask)(struct machine_s*, input_bit_field_t, input_bit_state_t);
    void (*get_active_sprites)(struct machine_s*, vector_t*);
    void (*add_viewer)(struct machine_s*, viewer_t*);
    void (*remove_viewer)(struct machine_s*, viewer_t*);
} machine_t;

machine_t* new_machine(game_t const*);
void delete_machine(machine_t**);

#endif
