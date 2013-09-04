#include "console_viewer.h"
#include "machine.h"
#include <stdio.h>
#include <time.h>

static void _console_viewer_nudge(struct viewer_s* this, void* source)
{
    machine_t* machine = (machine_t*)source;
    char buf[40];
    time_t now = time(NULL);
    size_t i;
    struct tm* lnow = localtime(&now);
    vector_t* sprites = new_vector();
    strftime(buf, sizeof(buf), "%H:%M:%S", lnow);
    printf("%s: Console Viewer Entry\n", buf);
    printf("In state %u\n", machine->current_state);
    printf("Active sprites: ");
    machine->get_active_sprites(machine, sprites);
    for(i = 0; i < sprites->size(sprites); ++i) {
        unsigned idx = (unsigned)sprites->get(sprites, i);
        printf("%u ", machine->game->sprites[idx]->id);
    }
    printf("\n");
    printf("Input: %012X\n", machine->input);
    printf("Registers:");
    for(i = 0; i < 100; ++i) {
        if(i % 20 == 0) printf("\n");
        printf("%02X ", machine->registers[i]);
    }
    printf("\n");

    printf("\n");
    delete_vector(&sprites);
}

viewer_t* new_console_viewer()
{
    viewer_t* ret = (viewer_t*)malloc(sizeof(viewer_t));
    ret->_data = NULL;
    ret->nudge = _console_viewer_nudge;
    return ret;
}

void delete_console_viewer(viewer_t** this)
{
    if(!*this) return;
    free(*this);
    *this = NULL;
}
