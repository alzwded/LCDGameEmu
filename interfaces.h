#ifndef INTERFACES_H
#define INTERFACES_H

struct FILE;

typedef struct {
    unsigned int id, x, y;
    char* path;
} sprite_t;

typedef enum {
    ctNOP = 0,
    ctREGISTER,
    ctCONDITION,
    ctIF,
    ctBLOCK,
    ctARITHMETIC,
    ctCONST
} code_type_t;

typedef struct code_s {
    code_type_t type;
    union {
        unsigned int num;
        struct code_s* code;
    } left;
    union {
        unsigned int num;
        struct code_s* code;
    } right;
    struct code_s* first;
    struct code_s* next;
    struct code_s* top;
} code_t;

typedef struct {
    unsigned int id;
    code_t* code;
} state_t;

typedef struct game_s {
    size_t csprites;
    size_t nsprites;
    sprite_t** sprites;

    size_t cstates;
    size_t nstates;
    state_t** states;

    void (*add_state)(struct game_s*, state_t*);
    void (*add_sprite)(struct game_s*, sprite_t*);
} game_t;

int yyparse();

game_t* new_game();
state_t* new_state(unsigned, code_t*);
sprite_t* new_sprite(unsigned id, unsigned x, unsigned y, char*);
void delete_game(game_t**);
void delete_state(state_t**);
void delete_sprite(sprite_t**);
void delete_code(code_t**);

code_t* new_nop();
code_t* new_reg(int);
code_t* new_ident(char*);
void normalize_code(code_t**);

void parser_set_stream(FILE* f);
FILE* const parser_get_stream();

#endif
