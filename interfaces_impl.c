#include <stdio.h>
#include <stdlib.h>
#include "interfaces.h"

static FILE* g_parser_stream;

FILE* const parser_get_stream()
{
    return g_parser_stream;
}

void parser_set_stream(FILE* f)
{
    g_parser_stream = f;
}

void _game_add_state(struct game_s* this, state_t* s)
{
    if(this->nstates >= this->cstates) {
        this->states = (state_t**)realloc(this->states, sizeof(state_t*) * (this->cstates << 1));
        this->cstates <<= 1;
    }
    this->states[this->nstates++] = s;
}

void _game_add_sprite(struct game_s* this, sprite_t* s)
{
    if(this->nsprites >= this->csprites) {
        this->sprites = (sprite_t**)realloc(this->sprites, sizeof(sprite_t*) * (this->csprites << 1));
        this->csprites <<= 1;
    }
    this->sprites[this->nsprites++] = s;
}

game_t* new_game()
{
    game_t* ret = (game_t*)malloc(sizeof(game_t));
    ret->csprites = ret->cstates = 2;
    ret->nsprites = ret->nstates = 0;
    ret->sprites = (sprite_t**)malloc(2 * sizeof(sprite_t*));
    ret->states = (state_t**)malloc(2 * sizeof(state_t*));
    ret->add_state = &_game_add_state;
    ret->add_sprite = &_game_add_sprite;
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

void delete_code(code_t** code)
{
    // TODO
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

void normalize_code(code_t** code)
{
    // TODO
    // left/right = left/right->first;
    // left/right->top = me
}
