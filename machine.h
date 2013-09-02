#ifndef MACHINE_H
#define MACHINE_H

#include <stdlib.h>
#include "interfaces.h"
#include "stack.h"
#include "vector.h"

typedef unsigned char reg_t;

typedef enum {
    LEFT = 0x000000000001,
    RIGHT = 0x000000000010,
    UP = 0x000000000100,
    DOWN = 0x000000001000,
    FIRE = 0x000000010000,
    ALT = 0x000000100000,
    START = 0x000001000000,
    TOGGLE = 0x000010000000,
    UPLEFT = 0x000100000000,
    UPRIGHT = 0x001000000000,
    DOWNLEFT = 0x010000000000,
    DOWNRIGHT = 0x100000000000,
    LAST_BIT = 0x1000000000000
} input_bit_t;

typedef unsigned input_bit_field_t;
typedef enum {
    LO = 0,
    HI = 1
} input_bit_state_t;

typedef struct machine_s {
    reg_t registers[100];
    input_bit_field_t input;
    unsigned current_state;
    stack_t* stack;
    vector_t* sprite_state;
    game_t const* game;

    void (*onclock)(struct machine_s*);
    void (*set_input)(struct machine_s*, input_bit_t, input_bit_state_t);
    void (*set_input_mask)(struct machine_s*, input_bit_field_t, input_bit_state_t);
    void (*get_active_sprites)(struct machine_s*, vector_t*, unsigned*);
} machine_t;

machine_t* new_machine(game_t const*);
void delete_machine(machine_t**);

#endif
