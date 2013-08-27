/*
Grammar for LCDGameEmu .db file
.db is a working extension ; TODO come up with a better extension
*/
%{
#include <stdio.h>
#include <assert.h>
#include "interface.h"

int yylex();
%}
%union {
    char const* str;
    int num;
}

/* optional, alternatively use for(YYNTOKENS) strncmp(yytname[i] + 1, s, strlen(s)) return yytoknum[i]
%token <str> SPRITE ".sprite"
%token <str> STATE ".state"
%token <str> END ".end"
for chars like '&' just return the char
*/
%token <num> REG
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

/* VAR : '$' INT | '$' IDENT ; actually processed in lexer */
VAR : REG | IDENT ;
set_statement : ".set" INT | ".reset" INT
              | ".set" VAR arithmetic_expression
              | ".reset" ".all"
              ;
transition_statement : ".transition" INT ;

isset_expression : ".set" INT | ".reset" INT ;

arithmetic_expression : ".mul" operand operand
                      | ".div" operand operand
                      | ".mod" operand operand
                      | ".sum" operand operand
                      | ".sub" operand operand
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
char* yylex_buf = NULL;
typedef struct node_s {
    char c;
    struct node_s* n;
} node_t;
typedef node_t* pNode_t;
pNode_t new_node(char const c)
{
    pNode_t ret = (pNode_t)malloc(sizeof(node_t));
    ret->c = c;
    ret->n = NULL;
    return ret;
}
void yylex_move(pNode_t n, size_t const size)
{
    assert(size);
    assert(n);
    if(yylex_buf) free(yylex_buf);
    yylex_buf = (char*)malloc(size);
    char* p = yylex_buf;
    while(n) {
        *p = n->c;
        pNode_t toDelete = n;
        n = n->n;
        free(toDelete);
        ++p;
    }
    *p = NULL;
}

int yylex()
{
    // I've decided not to use flex
    // TODO
    char c;
    size_t size;
    // read char
    // process
    // n->n = new_node(c);
    // size++;
    // loop
    // 
    // yylex_move(n, size)
    // yylval.str = yylex_buf;
    // return STRING?
    return STRING;
}

int yyerror(char* s)
{
    fprintf(stderr, "%s\n", s);
}
