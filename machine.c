#include "machine.h"
#include <assert.h>
#include <memory.h>

void _machine_onclock_impl(struct machine_s* this)
{
}

void _machine_set_input_impl(struct machine_s* this, input_bit_t bit, input_bit_state_t state)
{
    assert(this);
    assert(state == HI || state == LO);
    assert(bit && bit < LAST_BIT);

    this->input &= ~bit;
    this->input |= (state << bit);
}

void _machine_set_input_mask_impl(struct machine_s* this, input_bit_field_t mask, input_bit_state_t state)
{
    assert(this);
    assert(state == HI || state == LO);
    assert(mask < LAST_BIT);

    this->input &= ~mask;
    if(state) this->input |= mask;
}


void _machine_get_active_sprites_impl(struct machine_s* this, vector_t* sprites, unsigned* nsprites)
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
