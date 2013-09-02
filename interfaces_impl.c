#include <stdio.h>
#include <stdlib.h>
#include "interfaces.h"
#include "log.h"

static FILE* g_parser_stream;

FILE* const parser_get_stream()
{
    return g_parser_stream;
}

void parser_set_stream(FILE* f)
{
    g_parser_stream = f;
}

// comparator used to sort states, sprites and macros
// relies on the fact that all of these have an int id as the first member
int _pint_comparator(void const* a, void const* b)
{
    unsigned* left = (unsigned*)a;
    unsigned* right = (unsigned*)b;

    if(*left == *right) return 0;
    else if(*left < *right) return -1;
    else return 1;
}

#define DECL_GAME_ADD_STUFF_METHOD(TYPE) \
void _game_add_##TYPE(struct game_s* this, TYPE##_t * s) \
{ \
    if(this->n##TYPE##s >= this->c##TYPE##s) { \
        this->TYPE##s = (TYPE##_t**)realloc(this->TYPE##s, sizeof(TYPE##_t*) * (this->c##TYPE##s << 1)); \
        this->c##TYPE##s <<= 1; \
    } \
    this->TYPE##s[this->n##TYPE##s++] = s; \
    qsort(this->TYPE##s, this->n##TYPE##s, sizeof(TYPE##_t*), _pint_comparator); \
}

#define GET_NAME_OF_GAME_ADD_STUFF_METHOD(TYPE) \
    _game_add_##TYPE

DECL_GAME_ADD_STUFF_METHOD(state)
DECL_GAME_ADD_STUFF_METHOD(sprite)
DECL_GAME_ADD_STUFF_METHOD(macro)

game_t* new_game()
{
    game_t* ret = (game_t*)malloc(sizeof(game_t));
    ret->csprites = ret->cstates = ret->cmacros = 2;
    ret->nsprites = ret->nstates = ret->nmacros = 0;
    ret->sprites = (sprite_t**)malloc(2 * sizeof(sprite_t*));
    ret->states = (state_t**)malloc(2 * sizeof(state_t*));
    ret->macros = (macro_t**)malloc(2 * sizeof(macro_t*));
    ret->add_state = &GET_NAME_OF_GAME_ADD_STUFF_METHOD(state);
    ret->add_sprite = &GET_NAME_OF_GAME_ADD_STUFF_METHOD(sprite);
    ret->add_macro = &GET_NAME_OF_GAME_ADD_STUFF_METHOD(macro);
    return ret;
}

void delete_game(game_t** game)
{
    size_t i;
    if(!game || !*game) return;
    for(i = 0; i < (*game)->nsprites; ++i) {
        delete_sprite( &(*game)->sprites[i] );
    }
    for(i = 0; i < (*game)->nstates; ++i) {
        delete_state( &(*game)->states[i] );
    }
    for(i = 0; i < (*game)->nmacros; ++i) {
        delete_macro( &(*game)->macros[i] );
    }
    free(*game);
    *game = NULL;
}

void delete_sprite(sprite_t** sprite)
{
    if(!sprite || !*sprite) return;
    free( (*sprite)->path );
    free(*sprite);
    *sprite = NULL;
}

void delete_state(state_t** state)
{
    if(!state || !*state) return;
    free(*state);
    delete_code( &(*state)->code );
    *state = NULL;
}

void delete_macro(macro_t** macro)
{
    if(!macro || !*macro) return;
    free(*macro);
    delete_code( &(*macro)->code );
    *macro = NULL;
}

char const* strct(code_type_t type)
{
#ifdef TYPE_TO_STRING
# error "TYPE_TO_STRING already defined"
#endif
#define TYPE_TO_STRING(X) case X: return &(#X)[2]
    switch(type) {
    TYPE_TO_STRING(ctNOP);
    TYPE_TO_STRING(ctREGISTER);
    TYPE_TO_STRING(ctRESETALL);
    TYPE_TO_STRING(ctSETVAR);
    TYPE_TO_STRING(ctSETSPRITE);
    TYPE_TO_STRING(ctTRANSITION);
    TYPE_TO_STRING(ctISSPRITE);
    TYPE_TO_STRING(ctISVAR);
    TYPE_TO_STRING(ctCONST);
    TYPE_TO_STRING(ctMUL);
    TYPE_TO_STRING(ctDIV);
    TYPE_TO_STRING(ctMOD);
    TYPE_TO_STRING(ctSUM);
    TYPE_TO_STRING(ctSUB);
    TYPE_TO_STRING(ctEQ);
    TYPE_TO_STRING(ctNE);
    TYPE_TO_STRING(ctLT);
    TYPE_TO_STRING(ctGT);
    TYPE_TO_STRING(ctNOT);
    TYPE_TO_STRING(ctRNG);
    TYPE_TO_STRING(ctIF);
    TYPE_TO_STRING(ctCALL);
    }
#undef TYPE_TO_STRING
    return "";
}

void delete_code(code_t** code)
{
    code_t** left = NULL;
    code_t** right = NULL;

    if(!code || !*code) return;

    jaklog(TRACE, JAK_STR, "freeing the members of a");
    jaklog(TRACE, JAK_TAB|JAK_STR|JAK_LN, strct((*code)->type));

    delete_code(&(*code)->next);

    switch((*code)->type) {
    case ctSETVAR:
    case ctIF:
    case ctMUL:
    case ctDIV:
    case ctMOD:
    case ctSUM:
    case ctSUB:
    case ctEQ:
    case ctNE:
    case ctLT:
    case ctGT:
        left = &(*code)->left.code;
        right = &(*code)->right.code;
        break;
    case ctNOT:
    case ctISVAR:
        left = &(*code)->left.code;
        break;
    default:
        break;
    }

    delete_code(left);
    delete_code(right);

    jaklog(TRACE, JAK_STR, "freeing a");
    jaklog(TRACE, JAK_TAB|JAK_STR|JAK_LN, strct((*code)->type));

    free(*code);
    *code = NULL;
}

sprite_t* new_sprite(unsigned id, unsigned x, unsigned y, char* path)
{
    sprite_t* ret = (sprite_t*)malloc(sizeof(sprite_t));
    ret->id = id;
    ret->x = x;
    ret->y = y;
    ret->path = path;
    return ret;
}

state_t* new_state(unsigned id, code_t* code)
{
    state_t* ret = (state_t*)malloc(sizeof(state_t));
    ret->id = id;
    ret->code = code;
    return ret;
}

macro_t* new_macro(unsigned id, code_t* code)
{
    macro_t* ret = (macro_t*)malloc(sizeof(macro_t));
    ret->id = id;
    ret->code = code;
    return ret;
}

code_t* new_nop()
{
    code_t* ret = (code_t*)malloc(sizeof(code_t));
    ret->type = ctNOP;
    ret->left.code = NULL;
    ret->right.code = NULL;
    ret->first = ret;
    ret->next = NULL;
    ret->top = NULL;
    return ret;
}

code_t* new_reg(unsigned r)
{
    code_t* ret = new_nop();
    ret->type = ctREGISTER;
    ret->left.num = r;
    return ret;
}

code_t* new_ident(char* s)
{
    code_t* ret = new_nop();
    ret->type = ctREGISTER;
    ret->left.num = -0xFF; // TODO switch(s) => int
    return ret;
}

code_t* new_set_sprite(unsigned sprite, sprite_state_t state)
{
    code_t* ret = new_nop();
    ret->type = ctSETSPRITE;
    ret->left.num = sprite;
    ret->right.num = (unsigned)state;
    return ret;
}

code_t* new_set_var(code_t* var, code_t* value)
{
    code_t* ret = new_binfunc(ctSETVAR, var, value);
    return ret;
}

code_t* new_reset_all()
{
    code_t* ret = new_nop();
    ret->type = ctRESETALL;
    return ret;
}

code_t* new_transition(unsigned t)
{
    code_t* ret = new_nop();
    ret->type = ctTRANSITION;
    ret->left.num = t;
    return ret;
}

code_t* new_is_sprite(unsigned sprite, sprite_state_t state)
{
    code_t* ret = new_nop();
    ret->type = ctISSPRITE;
    ret->left.num = sprite;
    ret->right.num = (unsigned)state;
    return ret;
}

code_t* new_is_var(code_t* var, sprite_state_t state)
{
    code_t* ret = new_nop();
    ret->type = ctISVAR;
    ret->left.code = var;
    ret->right.num = (unsigned)state;
    return ret;
}

code_t* new_const(unsigned c)
{
    code_t* ret = new_nop();
    ret->type = ctCONST;
    ret->left.num = c;
    return ret;
}

code_t* new_not(code_t* left)
{
    code_t* ret = new_nop();
    ret->type = ctNOT;
    ret->left.code = left->first;
    ret->left.code->top = ret;
    return ret;
}

code_t* new_binfunc(code_type_t type, code_t* left, code_t* right)
{
    code_t* ret = new_nop();
    ret->type = type;
    ret->left.code = left->first;
    ret->left.code->top = ret;
    ret->right.code = right->first;
    ret->right.code->top = ret;
    return ret;
}

code_t* new_rng(unsigned p)
{
    code_t* ret = new_nop();
    ret->type = ctRNG;
    ret->left.num = p;
    return ret;
}

code_t* new_if(code_t* left, code_t* right)
{
    code_t* ret = new_binfunc(ctIF, left, right);
    return ret;
}

code_t* new_call(unsigned id)
{
    code_t* ret = new_nop();
    ret->type = ctCALL;
    ret->left.num = id;
    return ret;
}
