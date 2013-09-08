#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
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
};

struct _MAIN_ARGS_s g_MAIN_ARGS_INST = {
    0, // console_viewer
    NULL, // input
    0, // g_test
    NULL // keymap
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
    g_machine = new_machine(THEGAME);
    if(console_viewer) g_machine->add_viewer(g_machine, console_viewer);
    g_window = new_window(g_machine);
    g_window->init(g_window, fileName, g_MAIN_ARGS_INST.input == NULL);
    if(g_MAIN_ARGS_INST.keymap) g_window->set_keys(g_window, g_MAIN_ARGS_INST.keymap);
    g_machine->add_viewer(g_machine, g_window->get_viewer(g_window));
    // TODO init gui
}

void start()
{
    // TODO launch timer which calls g_machine->onclock(g_machine)
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

int main(int argc, char* argv[])
{
    jaklog_set_level(INFO);

    HandleParameters(argc, argv);
    ProcessWhatJustHasHappened();

    //if(g_MAIN_ARGS_INST.g_test) test();
    //else test(); // TODO :-P
    test();

    delete_console_viewer(&console_viewer);
    delete_vector(&g_MAIN_ARGS_INST.input);

    if(g_MAIN_ARGS_INST.keymap) {
        size_t l = g_MAIN_ARGS_INST.keymap->size(g_MAIN_ARGS_INST.keymap);
        size_t i = 0;
        for(; i < l; ++i) {
            key_map_pair_t* p = (key_map_pair_t*)g_MAIN_ARGS_INST.keymap->get(g_MAIN_ARGS_INST.keymap, i);
            free(p->name);
            free(p);
        }
        delete_vector(&g_MAIN_ARGS_INST.keymap);
    }

    return 0;
}
