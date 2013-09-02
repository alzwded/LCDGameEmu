#include "machine.h"
#include "interpreter.h"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>

static int _state_comp(void const* a, void const* b)
{
    assert(a);
    assert(b);
    unsigned* first = (unsigned*)a;
    state_t** second = (state_t**)b;
    if(*first == (*second)->id) return 0;
    else if(*first > (*second)->id) return 1;
    else return -1;
}

static code_t* _get_code_of(machine_t* this, unsigned stateId)
{
    state_t** found = (state_t**)bsearch(&stateId, this->game->states, this->game->nstates, sizeof(state_t*), _state_comp);
    return (*found)->code;
}

static void _machine_onclock_impl(struct machine_s* this)
{
    assert(this);
    code_t* code = _get_code_of(this, this->current_state);
    interpreter_eval(this, code);
    assert(this->stack->empty(this->stack));
}

static void _machine_set_input_impl(struct machine_s* this, input_bit_t bit, input_bit_state_t state)
{
    assert(this);
    assert(state == HI || state == LO);
    assert(bit && bit < LAST_BIT);

    this->input &= ~bit;
    this->input |= (state << bit);
}

static void _machine_set_input_mask_impl(struct machine_s* this, input_bit_field_t mask, input_bit_state_t state)
{
    assert(this);
    assert(state == HI || state == LO);
    assert(mask < LAST_BIT);

    this->input &= ~mask;
    if(state) this->input |= mask;
}

static void _machine_get_active_sprites_impl(struct machine_s* this, vector_t* sprites)
{
    assert(this);
    size_t i;
    for(i = 0; i < this->sprite_state->size(this->sprite_state); ++i) {
        if((int)this->sprite_state->get(this->sprite_state, i))
            sprites->append(sprites, (void const*)i);
    }
}

machine_t* new_machine(game_t const* game)
{
    machine_t* ret = (machine_t*)malloc(sizeof(machine_t));

    ret->game = game;
    ret->current_state = 0;
    ret->input = 0x0;
    memset(&ret->registers, 0, 100);
    ret->stack = new_stack();
    ret->sprite_state = new_vector_of(game->nsprites); // sets everything to 0

    ret->onclock = &_machine_onclock_impl;
    ret->set_input = &_machine_set_input_impl;
    ret->set_input_mask = &_machine_set_input_mask_impl;
    ret->get_active_sprites = &_machine_get_active_sprites_impl;

    return ret;
}

void delete_machine(machine_t** this)
{
    if(!*this) return;

    if((*this)->stack) delete_stack(&(*this)->stack);
    if((*this)->sprite_state) delete_vector(&(*this)->sprite_state);

    free(*this);
    *this = NULL;
}
