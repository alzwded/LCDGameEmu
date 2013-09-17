#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include "code.h"
#include "log.h"
#include "cmdargs.h"
#include "machine.h"
#include "console_viewer.h"
#include "inputbit.h"
#include "window.h"

struct _MAIN_ARGS_s {
    int console_viewer:1;
    vector_t* input;
    unsigned g_test;
    vector_t* keymap;
    int which_joystick;
    vector_t* joystick_map;
    int which_hat;
    int which_xaxis;
    int which_yaxis;
};

struct _MAIN_ARGS_s g_MAIN_ARGS_INST = {
    0, // console_viewer
    NULL, // input
    0, // g_test
    NULL, // keymap
    0, // which_joystick
    NULL, // joystick_map
    0, // which_hat
    0, // which_xaxis
    1, // which_yaxis
};

game_t* THEGAME;
static machine_t* g_machine;
static viewer_t* console_viewer;
static window_t* g_window;

void cleanup()
{
    if(g_machine && console_viewer) g_machine->remove_viewer(g_machine, console_viewer);
    delete_machine(&g_machine);
    delete_game(&THEGAME);
    delete_window(&g_window);
}

extern int yydebug;

int load(char const* fileName)
{
    assert(fileName);
    unsigned ret = 1;
    FILE* f = fopen(fileName, "r");
    if(!f) {
        fprintf(stderr, "cannot open %s\n", fileName);
        exit(2);
    }

    // cleanup
    cleanup();

    parser_set_stream(f);

    // launch parser
    ret = yyparse() != 0;

    fclose(f);

    return ret;
}

static char const* fileName = "example/test-game.lge";

void init()
{
    srand(time(NULL));
    g_machine = new_machine(THEGAME);
    if(console_viewer) g_machine->add_viewer(g_machine, console_viewer);

    g_window = new_window(g_machine);
    switch(g_window->init(g_window, fileName, g_MAIN_ARGS_INST.input == NULL)) {
    case WINDOW_ERR_SDL_INIT:
        jaklog(jlFATAL, jlSTR|jlLN, "could not init SDL");
        exit(42);
    case WINDOW_ERR_INVALID_PATH:
        jaklog(jlFATAL, jlSTR|jlLN, "failed to load window because of an invalid path");
        exit(42);
    case WINDOW_ERR_NO_BG:
        jaklog(jlFATAL, jlSTR|jlLN, "no background provided");
        exit(42);
    default:
        jaklog(jlFATAL, jlSTR|jlLN, "unknown error while initializing main window");
        exit(42);
    case WINDOW_ERR_SUCCESS: /* NOTHING */
        break;
    }

    if(g_MAIN_ARGS_INST.keymap) 
        if(g_window->set_keys(g_window, g_MAIN_ARGS_INST.keymap)) {
            jaklog(jlFATAL, jlSTR|jlLN, "there was an error remapping the keys. Check input");
            exit(42);
        }

    g_window->use_joystick(g_window, g_MAIN_ARGS_INST.which_joystick);
    if(g_MAIN_ARGS_INST.joystick_map) g_window->map_joystick(g_window, g_MAIN_ARGS_INST.joystick_map);
    g_window->set_dpads(g_window, g_MAIN_ARGS_INST.which_hat, g_MAIN_ARGS_INST.which_xaxis, g_MAIN_ARGS_INST.which_yaxis);

    g_machine->add_viewer(g_machine, g_window->get_viewer(g_window));
}

void start()
{
    if(g_MAIN_ARGS_INST.g_test) {
        int i;
        for(i = 0; i < g_MAIN_ARGS_INST.g_test; ++i)
            g_machine->onclock(g_machine);
    } else {
        g_window->loop(g_window);
    }
}

void test()
{
    load(fileName);
    init();
    start();
    cleanup();
}

void args_use_nth_joystick(char const* S)
{
    g_MAIN_ARGS_INST.which_joystick = atoi(S);
}

void args_console_viewer_enable(char const* _)
{
    g_MAIN_ARGS_INST.console_viewer = 1;
}

void args_test(char const* times)
{
    if(times) g_MAIN_ARGS_INST.g_test = atoi(times);
    else g_MAIN_ARGS_INST.g_test = 1;
}

void args_set_debug_level(char const* s)
{
    jaklog_set_level((log_level_t)atoi(s));
}

void args_load(char const* s)
{
    fileName = s;
}

void args_joystick_use_hat(char const* s)
{
    g_MAIN_ARGS_INST.which_hat = atoi(s);
}

void args_joystick_xaxis(char const* s)
{
    g_MAIN_ARGS_INST.which_xaxis = atoi(s);
}

void args_joystick_yaxis(char const* s)
{
    g_MAIN_ARGS_INST.which_yaxis = atoi(s);
}

void args_input(char const* S)
{
    char* s = strdup(S);
    char* p = strtok(s, ",");
    g_MAIN_ARGS_INST.input = new_vector();
    while(p) {
        char* colon = strchr(p, ':');
        unsigned val;
        input_bit_t inp;
        frame_button_pair_t el;

        if(!colon) abort();
        *colon = '\0';

        val = atoi(p);
        inp = str2inputbit(colon + 1);
        el.frame = 0xFFFFF & val;
        el.button = 0xFFF & inp;

        g_MAIN_ARGS_INST.input->append(g_MAIN_ARGS_INST.input, (void const*)el.raw);

        p = strtok(NULL, ",");
    }
    free(s);
}

void args_joystick_map_buttons(char const* S)
{
    char* s = strdup(S);
    char* p = strtok(s, ",");
    g_MAIN_ARGS_INST.joystick_map = new_vector();
    for(; p; p = strtok(NULL, ",")) {
        char* colon = strchr(p, ':');
        key_map_pair_t* el = (key_map_pair_t*)malloc(sizeof(key_map_pair_t));

        if(!colon) abort();
        *colon = '\0';

        el->name = (char*)malloc(colon - p);
        strcpy(el->name, p);
        el->keysym = atoi(colon + 1);

        g_MAIN_ARGS_INST.joystick_map->append(g_MAIN_ARGS_INST.joystick_map, el);
    }
    free(s);
}

void args_remap_keys(char const* S)
{
    char* s = strdup(S);
    char* p = strtok(s, ",");
    g_MAIN_ARGS_INST.keymap = new_vector();
    while(p) {
        char* colon = strchr(p, ':');
        key_map_pair_t* el = (key_map_pair_t*)malloc(sizeof(key_map_pair_t));

        if(!colon) abort();
        *colon = '\0';

        el->name = (char*)malloc(colon - p);
        strcpy(el->name, p);
        el->keysym = atoi(colon + 1);

        g_MAIN_ARGS_INST.keymap->append(g_MAIN_ARGS_INST.keymap, el);

        p = strtok(NULL, ",");
    }
    free(s);
}

void ProcessWhatJustHasHappened()
{
    if(g_MAIN_ARGS_INST.console_viewer) {
        console_viewer = new_console_viewer(g_MAIN_ARGS_INST.input);
    }
}

static void clear_keymap(vector_t** this)
{
    if(!*this) return;
    /* else */
    {
        size_t l = (*this)->size(*this);
        size_t i = 0;
        for(; i < l; ++i) {
            key_map_pair_t* p = (key_map_pair_t*)(*this)->get(*this, i);
            free(p->name);
            free(p);
        }
        delete_vector(this);
    }
}

int main(int argc, char* argv[])
{
    jaklog_set_level(jlINFO);

    HandleParameters(argc, argv);
    ProcessWhatJustHasHappened();

    test();

    delete_console_viewer(&console_viewer);
    delete_vector(&g_MAIN_ARGS_INST.input);
    clear_keymap(&g_MAIN_ARGS_INST.joystick_map);
    clear_keymap(&g_MAIN_ARGS_INST.keymap);


    return 0;
}
