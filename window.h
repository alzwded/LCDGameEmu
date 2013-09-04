#ifndef WINDOW_H
#define WINDOW_H

#include "machine.h"

typedef struct window_s {
    void* _data;

    unsigned (*init)(struct window_s*);
    void (*loop)(struct window_s*);
    void (*redraw)(struct window_s*);
} window_t;

window_t* new_window(machine_t*);
void delete_window(window_t**);

#endif
