#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "interfaces.h"
#include "log.h"

game_t* THEGAME;

void cleanup()
{
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

    //yydebug = 1;
    jaklog_set_level(TRACE);

    // launch parser
    return yyparse();
}

void test()
{
    load("example/test-game.lge");
}

int main(int argc, char* argv[])
{
    // TODO actually implement front-end
    // TODO open main window()

    test();

    return 0;
}
