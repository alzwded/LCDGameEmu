#ifndef WINDOW_H
#define WINDOW_H

#include "machine.h"

typedef enum {
    WINDOW_ERR_SUCCESS = 0,
    WINDOW_ERR_SDL_INIT,
    WINDOW_ERR_INVALID_PATH,
    WINDOW_ERR_NO_BG
} window_init_err_code_t;

typedef struct window_s {
    void* _data;

    window_init_err_code_t (*init)(struct window_s*, char const*);
    void (*loop)(struct window_s*);
    void (*redraw)(struct window_s*);
    viewer_t* (*get_viewer)(struct window_s*);
} window_t;

window_t* new_window(machine_t*);
void delete_window(window_t**);

#endif
