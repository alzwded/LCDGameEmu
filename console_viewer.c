#include "console_viewer.h"
#include "machine.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>

typedef struct {
    vector_t* input;
    unsigned current_frame;
} _console_viewer_data_t;

static int frame_button_pair_finder(void const* a, void const* b)
{
    unsigned* key = (unsigned*)a;
    frame_button_pair_t* value = (frame_button_pair_t*)b;
    assert(key);
    assert(value);
    if(*key < (*value).frame) return -1;
    else if(*key > (*value).frame) return 1;
    else return 0;
}

static int frame_button_pair_sorter(void const* a, void const* b)
{
    frame_button_pair_t* left = (frame_button_pair_t*)a;
    frame_button_pair_t* right = (frame_button_pair_t*)b;
    assert(left);
    assert(right);
    if((*left).frame < (*right).frame) return -1;
    else if((*left).frame > (*right).frame) return 1;
    else return 0;
}

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
    printf("Next state %u\n", machine->current_state);
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
        void const** foundv = data->input->bsearch(data->input, &data->current_frame, &frame_button_pair_finder);
        machine->set_input_mask(machine, ALL_INPUT_BITS, LO);

        if(!foundv) return;
        else {
            void const** inputs = data->input->const_array(data->input);
            void const** end = inputs + data->input->size(data->input);

            while(foundv >= inputs && frame_button_pair_finder(&data->current_frame, foundv) == 0) {
                foundv--;
            }
            while(++foundv < end && frame_button_pair_finder(&data->current_frame, foundv) == 0) {
                frame_button_pair_t* el = (frame_button_pair_t*)foundv;
                machine->set_input(machine, (*el).button, HI);
            }
        }
    }
}

static void _console_viewer_prepare_input(vector_t** input)
{
    assert(input);
    if(!*input) return;
    (*input)->sort(*input, &frame_button_pair_sorter);
}

viewer_t* new_console_viewer(vector_t* input)
{
    viewer_t* ret = (viewer_t*)malloc(sizeof(viewer_t));
    _console_viewer_data_t* data = (_console_viewer_data_t*)malloc(sizeof(_console_viewer_data_t));
    ret->_data = data;
    data->input = input;
    _console_viewer_prepare_input(&data->input);
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
