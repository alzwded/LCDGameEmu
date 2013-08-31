#include "machine.h"
#include <assert.h>

machine_t* new_machine(game_t const* game)
{
    return NULL;
}

void delete_machine(machine_t** this)
{
    if(!*this) return;

    if((*this)->stack) delete_stack(&(*this)->stack);
    if((*this)->sprite_state) delete_vector(&(*this)->sprite_state);

    free(*this);
    *this = NULL;
}
