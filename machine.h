#ifndef MACHINE_H
#define MACHINE_H

#include <stdlib.h>
#include "interfaces.h"
#include "stack.h"
#include "vector.h"

typedef unsigned char reg_t;

#define LEFT        (0x000000000001)
#define RIGHT       (0x000000000010)
#define UP          (0x000000000100)
#define DOWN        (0x000000001000)
#define FIRE        (0x000000010000)
#define ALT         (0x000000100000)
#define START       (0x000001000000)
#define TOGGLE      (0x000010000000)
#define UPLEFT      (0x000100000000)
#define UPRIGHT     (0x001000000000)
#define DOWNLEFT    (0x010000000000)
#define DOWNRIGHT   (0x100000000000)

typedef unsigned bit_t;
typedef enum {
    LO = 0,
    HI
} bit_state_t;

typedef struct machine_s {
    reg_t registers[100];
    bit_t input;
    unsigned current_state;
    stack_t* stack;
    vector_t* sprite_state;

    void (*onclock)(struct machine_s*);
    void (*set_input)(struct machine_s*, bit_t, bit_state_t);
    void (*get_active_sprites)(struct machine_s*, unsigned*, unsigned*);
} machine_t;

machine_t* new_machine(game_t const*);
void delete_machine(machine_t**);

#endif
