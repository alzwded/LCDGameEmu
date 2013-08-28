#include <stdio.h>
#include "interfaces.h"

static FILE* g_parser_stream;

FILE* const parser_get_stream()
{
    return g_parser_stream;
}

void parser_set_stream(FILE* f)
{
    g_parser_stream = f;
}
