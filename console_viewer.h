#ifndef CONSOLE_VIEWER_H
#define CONSOLE_VIEWER_H

#include "viewer.h"
#include "vector.h"

typedef union {
    struct {
        unsigned frame:20;
        unsigned button:12;
    };
    unsigned raw;
} frame_button_pair_t;

viewer_t* new_console_viewer(vector_t* input);
void delete_console_viewer(viewer_t**);

#endif
