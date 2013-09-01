#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "interfaces.h"
#include "log.h"
#include "cmdargs.h"
#include "machine.h"

game_t* THEGAME;
static machine_t* g_machine;


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
}

void test()
{
    load(fileName);
    init();
    start();
    cleanup();
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

    test();

    return 0;
}
