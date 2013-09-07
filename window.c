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
    // TODO other sdl stuff
} window_data_t;

typedef struct {
    unsigned x, y;
    SDL_Surface* img;
} sdl_sprite_t;

typedef struct {
    char const* name;
    SDL_Surface* img;
} sdl_charimg_t;
int sdl_charimg_sortf(void const* a, void const* b) {
    sdl_charimg_t* left = (sdl_charimg_t*)a;
    sdl_charimg_t* right = (sdl_charimg_t*)b;
    return strcmp(left->name, right->name);
}
int sdl_charimg_findf(void const* a, void const* b) {
    char* left = (char*)a;
    sdl_charimg_t* right = (sdl_charimg_t*)b;
    return strcmp(left, right->name);
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

    // FIXME sprites, imgs, etc memory leak right now

    // TODO other de-init stuff

    free(data->viewer);
    free(data);
    free(*this);
    *this = NULL;
}
