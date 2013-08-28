#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "interfaces.h"

void cleanup()
{
}

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

void test()
{
    load("test-game.db");
}

int main(int argc, char* argv[])
{
    // TODO actually implement front-end
    // TODO open main window()

    test();

    return 0;
}
