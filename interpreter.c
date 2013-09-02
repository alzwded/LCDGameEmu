#include "interpreter.h"
#include "interfaces.h"
#include <assert.h>
#include <memory.h>

/*
#define INTERPRETER_NEXTI() do{ \
    ip = (ip->next) ? ip->next : ip->top; \
}while(0)
*/
#define INTERPRETER_NEXTI() ip = ip->next;

#define CLEANUP() do{ \
    this->stack->clear(this->stack); \
}while(0)

static int _sprite_id_comp(void const* key, void const* a)
{
    assert(key);
    assert(a);
    unsigned* left = (unsigned*)key;
    sprite_t* right = (sprite_t*)a;
    if(*left < right->id) return -1;
    else if(*left == right->id) return 0;
    else return 1;
}

static size_t _get_sprite_index(machine_t* this, unsigned id)
{
    sprite_t** found = (sprite_t**)bsearch(&id, this->game->sprites, this->game->nsprites, sizeof(sprite_t), &_sprite_id_comp);
    return found - this->game->sprites;
}

void interpreter_eval(machine_t* this, code_t* beginI)
{
    code_t* ip = beginI;

    while(ip) {
        switch(ip->type) {
        case ctTRANSITION:
            this->stack->clear(this->stack);
            this->current_state = ip->left.num;
            CLEANUP();
            return;
        case ctNOP:
            break;
        case ctREGISTER:
            this->stack->push(this->stack, (void const*)ip->left.num);
            break;
        case ctRESETALL:
            memset(this->sprite_state->array(this->sprite_state), 0, this->sprite_state->size(this->sprite_state) * sizeof(void*));
            break;
        case ctSETVAR: {
            unsigned value = 0;
            unsigned reg = 0;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            value = (unsigned)this->stack->pop(this->stack);
            reg = (unsigned)this->stack->pop(this->stack);
            assert(reg < 100);
            this->registers[reg] = value;
            break; }
        case ctSETSPRITE:
            this->sprite_state->set(this->sprite_state, _get_sprite_index(this, ip->left.num), (void const*)ip->right.num);
            break;
        case ctISSPRITE:
            this->stack->push(this->stack, this->sprite_state->get(this->sprite_state, _get_sprite_index(this, ip->left.num)));
            break;
        }
        INTERPRETER_NEXTI();
    }
}
