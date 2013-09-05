#include "console_viewer.h"
#include "machine.h"
#include <stdio.h>
#include <time.h>

typedef struct {
    vector_t* input;
    unsigned current_frame;
} _console_viewer_data_t;

static void _console_viewer_nudge(struct viewer_s* this, void* source)
{
    machine_t* machine = (machine_t*)source;
    char buf[40];
    time_t now = time(NULL);
    size_t i;
    struct tm* lnow = localtime(&now);
    vector_t* sprites = new_vector();
    _console_viewer_data_t* data = (_console_viewer_data_t*)this->_data;
    strftime(buf, sizeof(buf), "%H:%M:%S", lnow);
    printf("%s: Console Viewer Entry #%u\n", buf, data->current_frame);
    printf("In state %u\n", machine->current_state);
    printf("Active sprites: ");
    machine->get_active_sprites(machine, sprites);
    for(i = 0; i < sprites->size(sprites); ++i) {
        unsigned idx = (unsigned)sprites->get(sprites, i);
        printf("%u ", machine->game->sprites[idx]->id);
    }
    printf("\n");
    printf("Input: %03X\n", machine->input);
    printf("Registers:");
    for(i = 0; i < 100; ++i) {
        if(i % 20 == 0) printf("\n");
        printf("%02X ", machine->registers[i]);
    }
    printf("\n");

    printf("\n");
    delete_vector(&sprites);

    data->current_frame++;
    if(data->input) {
        size_t i = 0; 
        machine->set_input_mask(machine, ALL_INPUT_BITS, LO);
        for(; i < data->input->size(data->input); ++i) {
            frame_button_pair_t el = (frame_button_pair_t)(unsigned)data->input->get(data->input, i);
            if(el.frame == data->current_frame) {
                //machine->set_input_mask(machine, ~el.button, LO);
                //machine->set_input_mask(machine, el.button, HI);
                machine->set_input(machine, el.button, HI);
            }
        }
    }
}

viewer_t* new_console_viewer(vector_t* input)
{
    viewer_t* ret = (viewer_t*)malloc(sizeof(viewer_t));
    _console_viewer_data_t* data = (_console_viewer_data_t*)malloc(sizeof(_console_viewer_data_t));
    ret->_data = data;
    data->input = input;
    data->current_frame = 0;
    ret->nudge = _console_viewer_nudge;
    return ret;
}

void delete_console_viewer(viewer_t** this)
{
    if(!*this) return;
    free((*this)->_data);
    free(*this);
    *this = NULL;
}
