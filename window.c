#include "window.h"
#include <assert.h>
#include <stdlib.h>

#include <SDL/SDL.h>

typedef struct {
    machine_t* machine;
    viewer_t* viewer;
    // TODO other sdl stuff
} window_data_t;

static void _window_viewer_impl_nudge(struct viewer_s* this, void* source)
{
    window_t* window = (window_t*)this->_data;
    machine_t* machine = (machine_t*)source;
    window_data_t* data = (window_data_t*)window->_data;
    assert(machine == data->machine);
    window->redraw(window);
}

static viewer_t* new_window_viewer_impl(window_t* parent)
{
    viewer_t* ret = (viewer_t*)malloc(sizeof(viewer_t));
    ret->_data = parent;
    ret->nudge = &_window_viewer_impl_nudge;
    return ret;
}

static unsigned _window_init(struct window_s* this, char const* path)
{
    // init SDL
    // get game.assets path
    // load bg.{bmp|png|jpg}
    // load sprites
    // open window
    // set up input handling
    return 0;
}

static void _window_loop(struct window_s* this)
{
    // set up a timer and start it
}

static void _window_redraw(struct window_s* this)
{
    // -- redraw logic
    // clrscr()
    // draw bg
    // foreach active sprite
    //    render it
}

window_t* new_window(machine_t* machine)
{
    window_t* ret = (window_t*)malloc(sizeof(window_t));
    window_data_t* data = (window_data_t*)malloc(sizeof(window_data_t));
    ret->_data = data;
    data->machine = machine;
    data->viewer = new_window_viewer_impl(ret);

    ret->init = &_window_init;
    ret->loop = &_window_loop;
    ret->redraw = &_window_redraw;

    return ret;
}

void delete_window(window_t** this)
{
    if(!*this) return;

    window_data_t* data = (*this)->_data;

    // TODO other de-init stuff

    free(data->viewer);
    free(data);
    free(*this);
    *this = NULL;
}
