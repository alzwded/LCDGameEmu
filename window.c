#include "window.h"
#include "vector.h"
#include "log.h"
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
    vector_t* sprites;
    vector_t* imgs;
    unsigned char send_input;
    vector_t* input_mapping;
    int joystickid;
    SDL_Joystick* joystick;
    struct {
        int Xaxis;
        int Yaxis;
        int hat;
        int fire;
        int alt;
        int start;
        int toggle;
    } joystick_mapping;
    // TODO other sdl stuff
} window_data_t;

typedef struct {
    char const* name;
    unsigned keysym;
    unsigned inputbit;
} input_mapping_pair_t;
static int _input_mapping_pair_by_keysym(void const* a, void const* b)
{
    input_mapping_pair_t** left = (input_mapping_pair_t**)a;
    input_mapping_pair_t** right = (input_mapping_pair_t**)b;
    jaklog(DEBUG, JAK_STR, "bykeysym comparing: ");
    jaklog(DEBUG, JAK_NUM, &(*left)->keysym);
    jaklog(DEBUG, JAK_STR, " and ");
    jaklog(DEBUG, JAK_NUM|JAK_LN, &(*right)->keysym);
    return (*left)->keysym - (*right)->keysym;
}
static int _input_mapping_pair_by_name(void const* a, void const* b)
{
    input_mapping_pair_t** left = (input_mapping_pair_t**)a;
    input_mapping_pair_t** right = (input_mapping_pair_t**)b;
    return strcmp((*left)->name, (*right)->name);
}
static int _input_mapping_pair_find_keysym(void const* key, void const* b)
{
    unsigned* left = (unsigned*)key;
    input_mapping_pair_t** right = (input_mapping_pair_t**)b;
    jaklog(DEBUG, JAK_STR, "comparing: ");
    jaklog(DEBUG, JAK_NUM, key);
    jaklog(DEBUG, JAK_STR, " and ");
    jaklog(DEBUG, JAK_NUM|JAK_LN, &(*right)->keysym);
    return *left - (*right)->keysym;
}
static int _input_mapping_pair_find_name(void const* key, void const* b)
{
    char* left = (char*)key;
    input_mapping_pair_t** right = (input_mapping_pair_t**)b;
    return strcmp(left, (*right)->name);
}
static input_mapping_pair_t _window_key_mapping[] = {
    { "left", SDLK_LEFT, LEFT },
    { "right", SDLK_RIGHT, RIGHT },
    { "up", SDLK_UP, UP },
    { "down", SDLK_DOWN, DOWN },
    { "fire", SDLK_1, FIRE },
    { "alt", SDLK_2, ALT },
    { "start", SDLK_9, START },
    { "toggle", SDLK_0, TOGGLE },
    { "upleft", SDLK_q, UPLEFT },
    { "upright", SDLK_p, UPRIGHT },
    { "downleft", SDLK_a, DOWNLEFT },
    { "downright", SDLK_l, DOWNRIGHT }
};
static void _window_init_input_mapping(vector_t** this)
{
    size_t i = 0;
    input_mapping_pair_t* p = &_window_key_mapping[0];
    *this = new_vector_of(12);
    for(; i < 12; ++i, ++p) {
        (*this)->set(*this, i, p);
    }
    (*this)->sort(*this, _input_mapping_pair_by_keysym);
}

typedef struct {
    unsigned x, y;
    SDL_Surface* img;
} sdl_sprite_t;

typedef struct {
    char const* name;
    SDL_Surface* img;
} sdl_charimg_t;
int sdl_charimg_sortf(void const* a, void const* b) {
    sdl_charimg_t** left = (sdl_charimg_t**)a;
    sdl_charimg_t** right = (sdl_charimg_t**)b;
    return strcmp((*left)->name, (*right)->name);
}
int sdl_charimg_findf(void const* a, void const* b) {
    char* left = (char*)a;
    sdl_charimg_t** right = (sdl_charimg_t**)b;
    return strcmp(left, (*right)->name);
}

static sdl_sprite_t* new_sdl_sprite(unsigned x, unsigned y, SDL_Surface* img)
{
    sdl_sprite_t* ret = (sdl_sprite_t*)malloc(sizeof(sdl_sprite_t));
    ret->x = x;
    ret->y = y;
    ret->img = img;
    return ret;
}

static void delete_sdl_sprite(sdl_sprite_t** this)
{
    if(!*this) return;
    free(*this);
    *this = NULL;
}

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

static void _window_clear_sprites(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);
    size_t len = data->sprites->size(data->sprites);
    size_t i = 0;
    for(; i < len; ++i) {
        sdl_sprite_t* sprite = (sdl_sprite_t*)data->sprites->get(data->sprites, i);
        delete_sdl_sprite(&sprite);
    }
    delete_vector(&data->sprites);
}

static void _window_clear_imgs(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);
    size_t l = data->imgs->size(data->imgs);
    size_t i = 0;
    for(; i < l; ++i) {
        sdl_charimg_t* s = (sdl_charimg_t*)data->imgs->get(data->imgs, i);
        SDL_FreeSurface(s->img);
    }
    delete_vector(&data->imgs);
}

static unsigned _window_init(struct window_s* this, char const* path, unsigned char send_input)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);

    data->send_input = send_input;

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
        } else if((p >= path && *p == '/' && (*p + 1) != '\0') || p < path) {
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
    if(data->imgs) _window_clear_imgs(this);
    if(data->sprites) _window_clear_sprites(this);
    data->sprites = new_vector();
    data->imgs = new_vector();
    /* nothing */
    {
        size_t l = data->machine->game->nsprites;
        size_t i = 0;
        for(; i < l; ++i) {
            sprite_t* sprite = data->machine->game->sprites[i];
            sdl_charimg_t* found = data->imgs->find(data->imgs, sprite->path, sdl_charimg_findf);
            if(found) {
                data->sprites->append(data->sprites,
                        (void const*)new_sdl_sprite(
                            sprite->x,
                            sprite->y,
                            found->img));
            } else {
                sdl_charimg_t* img = (sdl_charimg_t*)malloc(sizeof(sdl_charimg_t));
                char* fullPath = (char*)malloc(strlen(data->assets_path) + strlen(sprite->path));
                img->name = sprite->path;
                strcpy(fullPath, data->assets_path);
                strcat(fullPath, sprite->path);
                img->img = SDL_LoadBMP(fullPath);

                data->imgs->append(data->imgs, (void const*)img);
                data->imgs->sort(data->imgs, sdl_charimg_sortf);

                free(fullPath);

                data->sprites->append(data->sprites,
                        new_sdl_sprite(
                            sprite->x,
                            sprite->y,
                            img->img));
            }
        }
    }

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
    ++g_tick;
    return interval;
}

static void _window_loop(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    SDL_Event event;
    char loop = 1;
    char activateHat = 0;
    unsigned char tick = g_tick;
    input_bit_field_t current_input = 0x0;
    int xvalue, yvalue;
    Uint8 hatState;
    assert(data);

    // set up a timer and start it
    SDL_SetTimer(100, &_window_ontimer);

    while(loop) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_KEYDOWN:{
                jaklog(DEBUG, JAK_STR, "received input:");
                jaklog(DEBUG, JAK_TAB|JAK_LN|JAK_NUM, &event.key.keysym.sym);
                if(event.key.keysym.sym == SDLK_ESCAPE){
                    loop = 0;
                } else if(data->send_input) {
                    input_mapping_pair_t* found =
                        (input_mapping_pair_t*)
                        data->input_mapping->find(
                                data->input_mapping,
                                &event.key.keysym.sym,
                                _input_mapping_pair_find_keysym);
                    if(found) {
                        jaklog(DEBUG, JAK_STR, "handled with");
                        jaklog(DEBUG, JAK_TAB|JAK_LN|JAK_NUM, &found->inputbit);

                        current_input |= found->inputbit;
                    } else {
                        jaklog(DEBUG, JAK_LN|JAK_STR, "unhandled");
                    }
                }
                break; }
            case SDL_JOYAXISMOTION: {
                if(event.jaxis.which == data->joystickid) {
                    int value = event.jaxis.value;
                    int axis = event.jaxis.axis;
                    jaklog(DEBUG, JAK_STR, "joystick axis input (axis/value)");
                    jaklog(DEBUG, JAK_TAB|JAK_NUM, &axis);
                    jaklog(DEBUG, JAK_TAB|JAK_NUM|JAK_LN, &value);
                }
                break; }
            case SDL_JOYHATMOTION: {
                if(event.jhat.which == data->joystickid) {
                    unsigned value = event.jhat.value;
                    jaklog(DEBUG, JAK_STR, "joystick hat input");
                    jaklog(DEBUG, JAK_TAB|JAK_NUM|JAK_LN, &value);
                }
                break; }
            case SDL_JOYBUTTONDOWN: {
                unsigned which = event.jbutton.which;
                jaklog(TRACE, JAK_STR, "received input from joystick ");
                jaklog(TRACE, JAK_NUM|JAK_LN, &which);
                if(event.jbutton.which == data->joystickid) {
                    int value = event.jbutton.button;
                    jaklog(DEBUG, JAK_STR, "joystick button input");
                    jaklog(DEBUG, JAK_TAB|JAK_NUM|JAK_LN, &value);

                    if(value == data->joystick_mapping.fire) {
                        current_input |= FIRE;
                    } else if(value == data->joystick_mapping.alt) {
                        current_input |= ALT;
                    } else if(value == data->joystick_mapping.start) {
                        current_input |= START;
                    } else if(value == data->joystick_mapping.toggle) {
                        current_input |= TOGGLE;
                    }
                }
                break; }
            }
        }

        if(data->joystick) {
            hatState = SDL_JoystickGetHat(data->joystick, data->joystick_mapping.hat);
            xvalue = SDL_JoystickGetAxis(data->joystick, data->joystick_mapping.Xaxis);
            yvalue = SDL_JoystickGetAxis(data->joystick, data->joystick_mapping.Yaxis);
            if(activateHat) {
                if(hatState == SDL_HAT_CENTERED
                        && xvalue > -8000 && xvalue < 8000
                        && yvalue > -8000 && yvalue < 8000)
                    activateHat = 0;
            }
        }

        // poll clock tick
        if(tick != g_tick) {
            tick = g_tick;
            // TODO query joystick axes and/or hat here
            if(data->joystick)
            {
                if(hatState != SDL_HAT_CENTERED) {
                    activateHat = 1;
                    switch(hatState) {
                    case SDL_HAT_UP: current_input |= UP; break;
                    case SDL_HAT_DOWN: current_input |= DOWN; break;
                    case SDL_HAT_LEFT: current_input |= LEFT; break;
                    case SDL_HAT_RIGHT: current_input |= RIGHT; break;
                    case SDL_HAT_RIGHTUP: current_input |= UPRIGHT; break;
                    case SDL_HAT_LEFTUP: current_input |= UPLEFT; break;
                    case SDL_HAT_RIGHTDOWN: current_input |= DOWNRIGHT; break;
                    case SDL_HAT_LEFTDOWN: current_input |= DOWNLEFT; break;
                    }
                } else if(xvalue < -8000 && yvalue < -8000) {
                    activateHat = 1;
                    current_input |= UPLEFT;
                } else if(xvalue > 8000 && yvalue < -8000) {
                    activateHat = 1;
                    current_input |= UPRIGHT;
                } else if(xvalue < -8000 && yvalue > 8000) {
                    activateHat = 1;
                    current_input |= DOWNLEFT;
                } else if(xvalue > 8000 && yvalue > 8000) {
                    activateHat = 1;
                    current_input |= DOWNRIGHT;
                } else if(xvalue < -8000) {
                    activateHat = 1;
                    current_input |= LEFT;
                } else if(xvalue > 8000) {
                    activateHat = 1;
                    current_input |= RIGHT;
                } else if(yvalue < -8000) {
                    activateHat = 1;
                    current_input |= UP;
                } else if(yvalue > 8000) {
                    activateHat = 1;
                    current_input |= DOWN;
                }
            }
            if(data->send_input) {
                if(current_input) {
                    char s[80];
                    jaklog(DEBUG, JAK_STR, "sending input: ");
                    sprintf(s, "%03X", current_input);
                    jaklog(DEBUG, JAK_TAB|JAK_STR|JAK_LN, s);
                }
                data->machine->set_input_mask(data->machine, ALL_INPUT_BITS ^ current_input, LO);
                data->machine->set_input_mask(data->machine, current_input, HI);
            }
            data->machine->onclock(data->machine);
            current_input = 0x0;
        }
    }
}

static void _window_redraw(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    size_t l = 0, i = 0;
    assert(data);
    // -- redraw logic
    // clrscr()
    // draw bg

    SDL_BlitSurface(data->bg, NULL, data->display, NULL);

    // foreach active sprite
    //    render it
    /* nothing */
    {
        vector_t* sprites = new_vector();
        data->machine->get_active_sprites(data->machine, sprites);
        for(l = sprites->size(sprites), i = 0;
                i < l; ++i)
        {
            unsigned idx = (unsigned)sprites->get(sprites, i);
            sdl_sprite_t* sprite = (sdl_sprite_t*)data->sprites->get(data->sprites, idx);
            unsigned x = sprite->x;
            unsigned y = sprite->y;
            SDL_Surface* img = sprite->img;
            unsigned w = img->w;
            unsigned h = img->h;
            SDL_Rect rect = { x, y, w, h };

            if(SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 255, 0, 255)) < 0) {
                jaklog(ERROR, 0, "Something bad happened:");
                jaklog(ERROR, JAK_LN, SDL_GetError());
                abort();
            }

            SDL_BlitSurface(img, NULL, data->display, &rect);
        }
        delete_vector(&sprites);
    }

    SDL_UpdateRect(data->display, 0, 0, 0, 0);
}

static viewer_t* _window_get_viewer(struct window_s* this)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);
    assert(data->viewer);
    return data->viewer;
}

static int _window_set_keys(struct window_s* this, vector_t* key_map)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    size_t i = 0, l = key_map->size(key_map);
    data->input_mapping->sort(data->input_mapping, _input_mapping_pair_by_name);
    for(; i < l; ++i) {
        key_map_pair_t* km = (key_map_pair_t*)key_map->get(key_map, i);
        input_mapping_pair_t* im = (input_mapping_pair_t*)data->input_mapping->find(data->input_mapping, km->name, _input_mapping_pair_find_name);
        if(!im) {
            jaklog(FATAL, JAK_STR, "no key named ");
            jaklog(FATAL, JAK_STR|JAK_LN, km->name);
            return -1;
        }
        im->keysym = km->keysym;
    }
    data->input_mapping->sort(data->input_mapping, _input_mapping_pair_by_keysym);
    return 0;
}

static void _window_release_previous_joystick(SDL_Joystick** which)
{
    assert(which);
    if(!*which) return;
    SDL_JoystickClose(*which);
    *which = NULL;
}

static void _window_use_joystick(struct window_s* this, int id)
{
    assert(this);
    window_data_t* data = (window_data_t*)this->_data;
    assert(data);
    assert(data->display);
    _window_release_previous_joystick(&data->joystick);
    data->joystickid = id;
    data->joystick = SDL_JoystickOpen(id);
}

static void _window_init_joystick_mapping(window_data_t* data, vector_t* TBD)
{
    if(!TBD) {
        // default mapping
        data->joystick_mapping.Xaxis = 2;
        data->joystick_mapping.Yaxis = 3;
        data->joystick_mapping.hat = 0;
        data->joystick_mapping.fire = 0;
        data->joystick_mapping.alt = 1;
        data->joystick_mapping.start = 9;
        data->joystick_mapping.toggle = 8;
    } else {
    }
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
    data->send_input = 0;
    data->imgs = NULL;
    data->sprites = NULL;
    data->joystickid = 0;
    data->joystick = NULL;

    _window_init_input_mapping(&data->input_mapping);
    _window_init_joystick_mapping(data, NULL);

    ret->init = &_window_init;
    ret->loop = &_window_loop;
    ret->redraw = &_window_redraw;
    ret->get_viewer = &_window_get_viewer;
    ret->set_keys = &_window_set_keys;
    ret->use_joystick = &_window_use_joystick;

    return ret;
}

void delete_window(window_t** this)
{
    if(!*this) return;

    window_data_t* data = (*this)->_data;

    if(data->assets_path) free(data->assets_path);
    if(data->bg) SDL_FreeSurface(data->bg);
    if(data->input_mapping) delete_vector(&data->input_mapping);
    if(data->imgs) {
        size_t l = data->imgs->size(data->imgs), i = 0;
        for(; i < l; ++i) {
            sdl_charimg_t* thing = (sdl_charimg_t*)data->imgs->get(data->imgs, i);
            SDL_FreeSurface(thing->img);
            free(thing);
        }
        delete_vector(&data->imgs);
    }
    if(data->sprites) {
        size_t l = data->sprites->size(data->sprites), i = 0;
        for(; i < l; ++i) {
            sdl_sprite_t* thing = (sdl_sprite_t*)data->sprites->get(data->sprites, i);
            free(thing);
        }
        delete_vector(&data->imgs);
    }
    _window_release_previous_joystick(&data->joystick);

    // do this last for SDL stuff
    if(data->display) SDL_Quit();
    free(data->viewer);
    free(data);

    free(*this);
    *this = NULL;
}
