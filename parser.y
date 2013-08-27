/*
Grammar for LCDGameEmu .db file
.db is a working extension ; TODO come up with a better extension
*/
%{
#include <stdio.h>
#include "interface.h"

int yylex();
%}
%union {
    char const* str;
    int num;
}

%token <str> PATH IDENT
%token <num> INT
/*PATH : *
IDENT : [a-zA-Z_][a-zA-Z_0-9]*
VAR : '$' INT | '$' IDENT ;*/

/*%token <int> ASD; %token ASD*/
/*%type <int> ASD ; %type ASD ; %left ASD */

%error-verbose

%%

file : file item | item ;
item : sprite | state ;

sprite : ".sprite" INT PATH ;
state : ".state" INT code ;

code : codes ".end" ;
codes : codes block | block ;
block : block '&' statement | statement ;
statement : set_statement
          | conditional_statement
          | transition_statement
          | nop
          ;

nop : ".nop" ;

VAR : '$' INT | '$' IDENT ;
set_statement : ".set" INT | ".reset" INT
              | ".set" VAR arithmetic_expression
              | ".reset" "all"
              ;
transition_statement : ".transition" INT ;

isset_expression : "set" INT | "reset" INT ;

arithmetic_expression : "mul" operand operand
                      | "div" operand operand
                      | "mod" operand operand
                      | "sum" operand operand
                      | "sub" operand operand
                      | INT
                      ;

operand : VAR | arithmetic_expression ;

equality_expression : '=' operand operand
                    | '!' operand operand
                    | '<' operand operand
                    | '>' operand operand
                    ;

rng_expression : "%." INT ;

atomic_condition : isset_expression
                 | equality_expression
                 | rng_expression
                 ;

condition : condition '&' atomic_condition | atomic_condition ;

conditional_statement : ".if" condition ';' block ".fi" ;


%%

int yylex()
{
    // TODO
    // I've decided not to use flex
    return EOF;
}

int yyerror(char* s)
{
    fprintf(stderr, "%s\n", s);
}
