#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdio.h>

typedef struct {
    unsigned id, x, y;
    char* path;
} sprite_t;

typedef enum {
    ctNOP = 0,
    ctREGISTER,
    ctIDENT,
    ctRESETALL,
    ctSETVAR,
    ctSETSPRITE,
    ctTRANSITION,
    ctISSPRITE,
    ctISVAR,
    ctCONST,
    ctMUL, ctDIV, ctMOD, ctSUM, ctSUB,
    ctEQ, ctNE, ctLT, ctGT, ctNOT,
    ctRNG, ctIF, ctCALL
} code_type_t;

typedef enum {
    ssOFF = 0,
    ssON = 1
} sprite_state_t;

typedef struct code_s {
    code_type_t type;
    union {
        unsigned num;
        struct code_s* code;
    } left;
    union {
        unsigned num;
        struct code_s* code;
    } right;
    struct code_s* first;
    struct code_s* next;
    struct code_s* top;
} code_t;

typedef struct {
    unsigned id;
    code_t* code;
} state_t;

typedef struct {
    unsigned id;
    code_t* code;
} macro_t;

typedef struct game_s {
    size_t csprites;
    size_t nsprites;
    sprite_t** sprites;

    size_t cstates;
    size_t nstates;
    state_t** states;

    size_t cmacros;
    size_t nmacros;
    macro_t** macros;

    void (*add_state)(struct game_s*, state_t*);
    void (*add_sprite)(struct game_s*, sprite_t*);
    void (*add_macro)(struct game_s*, macro_t*);
} game_t;

int yyparse();

game_t* new_game();
state_t* new_state(unsigned, code_t*);
sprite_t* new_sprite(unsigned id, unsigned x, unsigned y, char*);
macro_t* new_macro(unsigned, code_t*);
void delete_game(game_t**);
void delete_state(state_t**);
void delete_sprite(sprite_t**);
void delete_code(code_t**);
void delete_macro(macro_t**);

// code_t factories
code_t* new_nop();
code_t* new_reg(unsigned);
code_t* new_ident(char const*);
code_t* new_set_sprite(unsigned, sprite_state_t);
code_t* new_set_var(code_t* var, code_t* value);
code_t* new_reset_all();
code_t* new_transition(unsigned);
code_t* new_is_sprite(unsigned, sprite_state_t);
code_t* new_is_var(code_t*, sprite_state_t);
code_t* new_const(unsigned);
code_t* new_binfunc(code_type_t, code_t*, code_t*);
code_t* new_rng(unsigned);
code_t* new_if(code_t*, code_t*);
code_t* new_call(unsigned);
code_t* new_not(code_t*);
void normalize_code(code_t**);

// set the stream the lexer uses
void parser_set_stream(FILE* f);
FILE* parser_get_stream();

#endif
