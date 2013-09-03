#ifndef INPUTBIT_H
#define INPUTBIT_H

typedef enum {
    LEFT = 0x1,
    RIGHT = 0x2,
    UP = 0x4,
    DOWN = 0x8,
    FIRE = 0x10,
    ALT = 0x20,
    START = 0x40,
    TOGGLE = 0x80,
    UPLEFT = 0x100,
    UPRIGHT = 0x200,
    DOWNLEFT = 0x400,
    DOWNRIGHT = 0x800,
    LAST_BIT = 0x1000,
} input_bit_t;

typedef unsigned input_bit_field_t;
typedef enum {
    LO = 0,
    HI = 1
} input_bit_state_t;

#endif
