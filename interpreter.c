#include "interpreter.h"
#include "interfaces.h"
#include "log.h"
#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <limits.h>

/*
#define INTERPRETER_NEXTI() do{ \
    ip = (ip->next) ? ip->next : ip->top; \
}while(0)
*/
#define INTERPRETER_NEXTI() do{ \
    if(!this->running) return; \
    ip = ip->next; \
}while(0)

#define CLEANUP() do{ \
    this->stack->clear(this->stack); \
}while(0)

static int _macro_id_comp(void const* key, void const* a)
{
    assert(key);
    assert(a);
    unsigned* left = (unsigned*)key;
    macro_t** right = (macro_t**)a;
    if(*left < (*right)->id) return -1;
    else if(*left == (*right)->id) return 0;
    else return 1;
}

static code_t* _get_code_of_macro(machine_t* this, unsigned id)
{
    macro_t** ret = (macro_t**)bsearch(&id, this->game->macros, this->game->nmacros, sizeof(macro_t*), &_macro_id_comp);
    return (*ret)->code;
}

static int _sprite_id_comp(void const* key, void const* a)
{
    assert(key);
    assert(a);
    unsigned* left = (unsigned*)key;
    sprite_t** right = (sprite_t**)a;
    if(*left < (*right)->id) return -1;
    else if(*left == (*right)->id) return 0;
    else return 1;
}

static size_t _get_sprite_index(machine_t* this, unsigned id)
{
    sprite_t** found = (sprite_t**)bsearch(&id, this->game->sprites, this->game->nsprites, sizeof(sprite_t*), &_sprite_id_comp);
    return found - this->game->sprites;
}

void interpreter_eval(machine_t* this, code_t* beginI)
{
    code_t* ip = beginI;

    while(ip) {
        switch(ip->type) {
        case ctTRANSITION:
            this->current_state = ip->left.num;
            this->running = 0;
            CLEANUP();
            return;
        case ctNOP:
            break;
        case ctREGISTER:
            this->stack->push(this->stack, (void const*)ip->left.num);
            break;
        case ctIDENT:
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
        case ctISSPRITE: {
            unsigned val = 1; 
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            if(!val) return;
            val = (ip->right.num == (unsigned)this->sprite_state->get(this->sprite_state, _get_sprite_index(this, ip->left.num)));
            this->stack->push(this->stack, (void const*)val);
            break; }
        case ctCONST:
            this->stack->push(this->stack, (void const*)ip->left.num);
            break;
        case ctMUL: {
            unsigned left;
            unsigned right;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(left * right));
            break; }
        case ctDIV: {
            unsigned left;
            unsigned right;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(left / right));
            break; }
        case ctSUM: {
            unsigned left;
            unsigned right;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(left + right));
            break; }
        case ctSUB: {
            unsigned left;
            unsigned right;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(left - right));
            break; }
        case ctMOD: {
            unsigned left;
            unsigned right;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(left % right));
            break; }
        case ctISVAR: {
            unsigned val = 1;
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            interpreter_eval(this, ip->left.code);
            unsigned mask = (unsigned)this->stack->pop(this->stack);
            if(ip->right.num) {
                val = (this->input & mask) != 0;
            } else {
                val = (this->input & mask) == 0;
            }
            this->stack->push(this->stack, (void const*)val);
            break; }
        case ctRNG: {
            double rval = (double)rand() / RAND_MAX;
            double distrib = ip->left.num;
            while(distrib > 1.0) distrib /= 10.0;
            unsigned rez = distrib > rval;
            this->stack->push(this->stack, (void const*)rez);
            break; }
        case ctIF: {
            interpreter_eval(this, ip->left.code);
            unsigned retVal = (unsigned)this->stack->pop(this->stack);
            if(retVal) interpreter_eval(this, ip->right.code);
            break; } 
        case ctEQ: {
            unsigned val = 1;
            unsigned left, right;
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            if(!val) return;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(val && (left == right)));
            break; } 
        case ctNE: {
            unsigned val = 1;
            unsigned left, right;
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            if(!val) return;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(val && (left != right)));
            break; }
        case ctLT: {
            unsigned val = 1;
            unsigned left, right;
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            if(!val) return;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(val && (left < right)));
            break; }
        case ctGT: {
            unsigned val = 1;
            unsigned left, right;
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            if(!val) return;
            interpreter_eval(this, ip->left.code);
            interpreter_eval(this, ip->right.code);
            right = (unsigned)this->stack->pop(this->stack);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(val && (left > right)));
            break; }
        case ctNOT: {
            unsigned val = 1;
            unsigned left;
            if(ip != ip->first) {
                val = (unsigned)this->stack->pop(this->stack);
            }
            if(!val) return;
            interpreter_eval(this, ip->left.code);
            left = (unsigned)this->stack->pop(this->stack);
            this->stack->push(this->stack, (void const*)(val && !left));
            break; }
        case ctCALL: {
            interpreter_eval(this, _get_code_of_macro(this, ip->left.num));
            break; }
        }
        INTERPRETER_NEXTI();
    }
}
