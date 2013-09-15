#ifndef INPUTBIT_H
#define INPUTBIT_H

typedef enum {
    LGE_LEFT = 0x1,
    LGE_RIGHT = 0x2,
    LGE_UP = 0x4,
    LGE_DOWN = 0x8,
    LGE_FIRE = 0x10,
    LGE_ALT = 0x20,
    LGE_START = 0x40,
    LGE_TOGGLE = 0x80,
    LGE_UPLEFT = 0x100,
    LGE_UPRIGHT = 0x200,
    LGE_DOWNLEFT = 0x400,
    LGE_DOWNRIGHT = 0x800,
    LGE_ALL_INPUT_BITS = 0xFFF,
} input_bit_t;


typedef unsigned input_bit_field_t;
typedef enum {
    bsLO = 0,
    bsHI = 1
} input_bit_state_t;

struct _ident_mapping_s {
    char const* s;
    unsigned i;
};

input_bit_t str2inputbit(char const*);

#endif
