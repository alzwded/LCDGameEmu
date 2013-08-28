#ifndef INTERFACES_H
#define INTERFACES_H

struct FILE;

int yyparse();

void parser_set_stream(FILE* f);
FILE* const parser_get_stream();

#endif
