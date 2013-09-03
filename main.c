#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "interfaces.h"
#include "log.h"
#include "cmdargs.h"
#include "machine.h"

game_t* THEGAME;
static machine_t* g_machine;
int g_test = 0;

void cleanup()
{
    delete_machine(&g_machine);
    delete_game(&THEGAME);
}

extern int yydebug;

int load(char const* fileName)
{
    assert(fileName);
    FILE* f = fopen(fileName, "r");
    if(!f) {
        fprintf(stderr, "cannot open %s\n", fileName);
        exit(2);
    }

    // cleanup
    cleanup();

    parser_set_stream(f);

    // launch parser
    return yyparse();
}

static char const* fileName = "example/test-game.lge";

void init()
{
    g_machine = new_machine(THEGAME);
    // TODO init gui
}

void start()
{
    // TODO launch timer which calls g_machine->onclock(g_machine)
    int i;
    for(i = 0; i < g_test; ++i)
        g_machine->onclock(g_machine);
}

void test()
{
    load(fileName);
    init();
    start();
    cleanup();
}

void args_test(char const* times)
{
    if(times) g_test = atoi(times);
    else g_test = 1;
}

void args_set_debug_level(char const* s)
{
    jaklog_set_level((log_level_t)atoi(s));
}

void args_load(char const* s)
{
    fileName = s;
}

int main(int argc, char* argv[])
{
    jaklog_set_level(TRACE);

    HandleParameters(argc, argv);

    if(g_test) test();
    else test();

    return 0;
}
