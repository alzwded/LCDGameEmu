#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "interfaces.h"
#include "log.h"
#include "cmdargs.h"

game_t* THEGAME;

void cleanup()
{
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

void test()
{
    load(fileName);
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
    //yydebug = 1;
    jaklog_set_level(TRACE);

    // TODO actually implement front-end
    // TODO open main window()
    HandleParameters(argc, argv);

    test();

    return 0;
}
