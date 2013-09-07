#include "window.h"
#include <assert.h>
#include <stdlib.h>

#include <SDL/SDL.h>

unsigned char g_tick = 0;

typedef struct {
    machine_t* machine;
    viewer_t* viewer;
    char* assets_path;
    SDL_Surface* bg;
    SDL_Surface* display;
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
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);

    // init SDL
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_TIMER) < 0) {
        return WINDOW_ERR_SDL_INIT;
    }

    // get game.assets path
    if(data->assets_path) free(data->assets_path);
    /* nothing */
    {
        size_t pathlen = strlen(path);
        char const* p = path + pathlen - 1;
        if(p < path) return WINDOW_ERR_INVALID_PATH;
        while(p >= path && *p != '.' && *p != '/') --p;
        if(p >= path && *p == '.') {
            // .assets/
            size_t l = p - path + 8;
            data->assets_path = (char*)malloc(sizeof(char) * l);
            strncpy(data->assets_path, path, p - path);
            strcpy(data->assets_path + (p - path), ".assets/");
            data->assets_path[l] = '\0';
        } else if((p >= path && *p == '/') || p < path) {
            size_t l = pathlen + 8;
            data->assets_path = (char*)malloc(sizeof(char) * l);
            strcpy(data->assets_path, path);
            strcpy(data->assets_path + l, ".assets/");
            data->assets_path[l] = '\0';
        } else {
            return WINDOW_ERR_INVALID_PATH;
        }
    }

    // load bg.{bmp|png|jpg}
    if(data->bg) SDL_FreeSurface(data->bg);
    /* nothing */
    {
        char* tmp = (char*)malloc(strlen(data->assets_path) + 6);
        strcpy(tmp, data->assets_path);
        data->bg = SDL_LoadBMP(strcat(tmp, "bg.bmp"));
        if(!data->bg) {
            SDL_Quit();
            return WINDOW_ERR_NO_BG;
        }
    }

    // load sprites
    // TODO

    // open window
    if(!data->display) {
        unsigned w = data->bg->w;
        unsigned h = data->bg->h;
        data->display = SDL_SetVideoMode(w, h, 24, SDL_HWSURFACE);
    } else {
        fprintf(stderr, "PANIC! don't know what to do if I already have an SDL window :-/\n");
        abort();
    }

    // set up input handling
    // TODO

    return 0;
}

static SDLCALL Uint32 _window_ontimer(Uint32 interval)
{
    g_tick = 1;
    return interval;
}

static void _window_loop(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    SDL_Event event;
    char loop = 1;
    unsigned char tick = g_tick;
    assert(data);

    // set up a timer and start it
    SDL_SetTimer(100, &_window_ontimer);

    while(loop) {
        while(SDL_PollEvent(&event)){
            /*check if event type is keyboard press*/
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    loop = 0;
                }
            }
        }
        if(tick != g_tick) {
            tick = g_tick;
            data->machine->onclock(data->machine);
        }
    }
}

static void _window_redraw(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);
    // -- redraw logic
    // clrscr()

    SDL_BlitSurface(data->bg, NULL, data->display, NULL);
    SDL_UpdateRect(data->display, 0, 0, 0, 0);

    // draw bg
    // foreach active sprite
    //    render it
}

static viewer_t* _window_get_viewer(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);
    assert(data->viewer);
    return data->viewer;
}

window_t* new_window(machine_t* machine)
{
    window_t* ret = (window_t*)malloc(sizeof(window_t));
    window_data_t* data = (window_data_t*)malloc(sizeof(window_data_t));
    ret->_data = data;
    data->machine = machine;
    data->viewer = new_window_viewer_impl(ret);
    data->assets_path = NULL;
    data->bg = NULL;
    data->display = NULL;

    ret->init = &_window_init;
    ret->loop = &_window_loop;
    ret->redraw = &_window_redraw;
    ret->get_viewer = &_window_get_viewer;

    return ret;
}

void delete_window(window_t** this)
{
    if(!*this) return;

    window_data_t* data = (*this)->_data;

    if(data->assets_path) free(data->assets_path);
    if(data->bg) SDL_FreeSurface(data->bg);
    if(data->display) SDL_Quit();

    // TODO other de-init stuff

    free(data->viewer);
    free(data);
    free(*this);
    *this = NULL;
}
