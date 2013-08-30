/*
Grammar for LCDGameEmu .db file
.db is a working extension ; TODO come up with a better extension
*/
%{
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "interfaces.h"
#include "log.h"

int yyerror(char const*);

#define YYDEBUG 1
#define YYPRINT(A,B,C) /* empty */

extern game_t* THEGAME;

int yylex();
%}
%union {
    char const* str;
    int num;
    sprite_t* sprite;
    state_t* state;
    code_t* code;
    macro_t* macro;
}

%token BAD_TOKEN
%token <num> REG
%token <str> PATH IDENT
%token <num> INT
%token RNDM "%."
/*PATH : *
IDENT : [a-zA-Z_][a-zA-Z_0-9]*
VAR : '$' INT | '$' IDENT ;*/

%type <sprite> sprite
%type <state> state
%type <macro> macro
%type <code> code codes block statement set_statement conditional_statement transition_statement nop isset_expression arithmetic_expression operand VAR equality_expression rng_expression atomic_condition condition call_statement

%error-verbose
%token_table

%%

file : /* NULL */ {
        jaklog(TRACE, JAK_STR | JAK_LN, "spawning game");
        THEGAME = new_game();
        }
     | file item
     ;
item : sprite {
        jaklog(TRACE, JAK_STR, "adding sprite ");
        jaklog(TRACE, JAK_TAB|JAK_NUM, &$1->id);
        jaklog(TRACE, JAK_TAB|JAK_STR|JAK_LN, $1->path);

        THEGAME->add_sprite(THEGAME, $1);
        }
     | state {
        jaklog(TRACE, JAK_STR, "adding state ");
        jaklog(TRACE, JAK_TAB, NULL);
        jaklog(TRACE, JAK_NUM|JAK_LN, &$1->id);

        THEGAME->add_state(THEGAME, $1);
        }
     | macro {
        jaklog(TRACE, JAK_STR, "adding macro ");
        jaklog(TRACE, JAK_TAB|JAK_NUM|JAK_LN, &$1->id);

        THEGAME->add_macro(THEGAME, $1);
        }
     ;

macro: ".macro" INT code { $$ = new_macro($2, $3); }

sprite : ".sprite" INT INT INT PATH { $$ = new_sprite($2, $3, $4, strdup($5)); } ;
state : ".state" INT code { $$ = new_state($2, $3); } ;

code : codes ".end" {
        assert($1);
        $$ = ($1->first) ? $1->first : $1 ;
        jaklog(TRACE, JAK_STR|JAK_LN, "codes end");
        }
     | ".end" {
        $$ = new_nop();
        jaklog(TRACE, JAK_STR|JAK_LN, "end");
        }
     ;
codes : codes block {
        jaklog(TRACE, JAK_STR|JAK_LN, "codes block");
        assert($1);
        assert($2);
        if($1->first) $2->first = $1->first;
        else $2->first = $1;
        $1 = $1->next = $2;
        $$ = $1;
    }
      | block {
        jaklog(TRACE, JAK_STR|JAK_LN, "block");
        $$ = $1;
    }
      ;
block : block '&' statement {
        assert($1);
        assert($3);
        if($1->first) $3->first = $1->first;
        else $3->first = $1;
        $1 = $1->next = $3;
        $$ = $1;
    }
      | statement {
        $$ = $1;
    }
  ;

statement : set_statement {
        $$ = $1;
        }
          | conditional_statement {
        $$ = $1;
        }
          | transition_statement {
        $$ = $1;
        }
          | nop {
        $$ = $1;
        }
          | call_statement {
        $$ = $1;
        }
          ;

call_statement : ".call" INT {
        $$ = new_call($2);
        }
        ;

nop : ".nop" {
        $$ = new_nop();
        }
    ;

/* VAR : '$' INT | '$' IDENT ; actually processed in lexer */
VAR : REG {
        jaklog(TRACE, JAK_STR, "reading register");
        jaklog(TRACE, JAK_TAB|JAK_LN|JAK_NUM, &$1);
        $$ = new_reg($1);
        }
    | IDENT {
        $$ = new_ident(strdup($1));
        }
    ;
set_statement : ".set" INT {
        $$ = new_set_sprite($2, ssON);
        }
              | ".reset" INT {
        $$ = new_set_sprite($2, ssOFF);
        }
              | ".set" VAR arithmetic_expression {
        $$ = new_set_var($2, $3);
        }
              | ".reset" ".all" {
        $$ = new_reset_all();
        }
              ;
transition_statement : ".transition" INT {
        $$ = new_transition($2);
        }
                     ;

isset_expression : ".set" INT { $$ = new_is_sprite($2, ssON); }
                 | ".reset" INT { $$ = new_is_sprite($2, ssOFF); }
                 | ".set" VAR { $$ = new_is_var($2, ssON); }
                 | ".reset" VAR { $$ = new_is_var($2, ssOFF); }
                 ;

arithmetic_expression : ".mul" operand operand { $$ = new_binfunc(ctMUL, $2, $3); }
                      | ".div" operand operand { $$ = new_binfunc(ctDIV, $2, $3); }
                      | ".mod" operand operand { $$ = new_binfunc(ctMOD, $2, $3); }
                      | ".sum" operand operand { $$ = new_binfunc(ctSUM, $2, $3); }
                      | ".sub" operand operand { $$ = new_binfunc(ctSUB, $2, $3); }
                      | '*' operand operand { $$ = new_binfunc(ctMUL, $2, $3); }
                      | '/' operand operand { $$ = new_binfunc(ctDIV, $2, $3); }
                      | '+' operand operand { $$ = new_binfunc(ctSUM, $2, $3); }
                      | '-' operand operand { $$ = new_binfunc(ctSUB, $2, $3); }
                      | INT { $$ = new_const($1); }
                      ;

operand : VAR { $$ = $1; }
        | arithmetic_expression { $$ = $1; }
        ;

equality_expression : '=' operand operand { $$ = new_binfunc(ctEQ, $2, $3); }
                    | '!' operand operand { $$ = new_binfunc(ctNE, $2, $3); }
                    | '<' operand operand { $$ = new_binfunc(ctLT, $2, $3); }
                    | '>' operand operand { $$ = new_binfunc(ctGT, $2, $3); }
                    | '~' operand { $$ = new_binfunc(ctNOT, $2, NULL); }
                    ;

rng_expression : "%." INT { $$ = new_rng($2); } ;

atomic_condition : isset_expression { $$ = $1; }
                 | equality_expression { $$ = $1; }
                 | rng_expression { $$ = $1; }
                 ;

condition : condition '&' atomic_condition {
        assert($1);
        assert($3);
        if($1->first) $3->first = $1->first;
        else $3->first = $1;
        $1 = $1->next = $3;
        $$ = $1;
        }
          | atomic_condition { $$ = $1; }
          ;

conditional_statement : ".if" condition ';' block ".fi" { $$ = new_if($2, $4); } ;


%%
char* yylex_buf = NULL;
typedef struct node_s {
    char c;
    struct node_s* n;
} node_t, *pNode_t;
//typedef node_t* pNode_t;
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
    *p = '\0';
}

int yylex()
{
    // I've decided not to use flex
    // TODO

    typedef enum {
        lsFIRST = 0,
        lsVARREG,
        lsKEYWORD,
        lsNUMBER,
        lsCOMMENT,
        lsPATH
    } lexer_state_t;

    lexer_state_t state = lsFIRST, prevState = lsFIRST;

    int i = 0;
    char c = EOF;
    size_t size = 0;

    pNode_t n = NULL;
    pNode_t pn = NULL;

    do {
        if(feof(parser_get_stream())) {
            if(state == lsFIRST) return 0;
            else break;
        }
        i = fgetc(parser_get_stream());
        if(i == EOF) { if(state == lsFIRST) return 0; else break; }

        c = (char)i;
#ifdef PERSONAL_TRACE
        printf("read %c %x\n", c, c);
#endif

        if(state == lsCOMMENT) {
            if(c == '\n') {
                state = prevState;
            }
            continue;
        }


        if(c == '#') {
            prevState = state;
            state = lsCOMMENT;
            continue;
        }

        if(state == lsFIRST) {
            switch(c) {
            case '&':
            case ';':
            case '=':
            case '<':
            case '>':
            case '!':
            case '+':
            case '-':
            case '*':
            case '/':
            case '~':
                return c;
            case '%':
                if(feof(parser_get_stream())) return BAD_TOKEN;
                i = fgetc(parser_get_stream());
                if(i == EOF) return BAD_TOKEN;
                c = (char)i;
                if(c != '.') return BAD_TOKEN;
                else return RNDM;
            }
        }

        if(isspace(c)) { if(state == lsFIRST) continue; else break; }

        if(state == lsFIRST && c == '$') {
            state = lsVARREG;
            assert(!n);
            continue;
        }
        if(state == lsFIRST && c == '.') {
            state = lsKEYWORD;
            assert(!n);
            n = new_node('.');
            pn = n;
            size = 1;
            continue;
        }
        if(state == lsFIRST && isdigit(c)) {
            state = lsNUMBER;
            assert(!n);
            n = new_node(c);
            pn = n;
            size = 1;
            continue;
        }
        if(state == lsFIRST && c == '"') {
            state = lsPATH;
            continue;
        }
        if(state == lsPATH && c == '"') {
            break;
        }

        switch(state) {
        case lsNUMBER:
            if(!isdigit(c)) {
                // error?
                return BAD_TOKEN;
            } else {
                assert(pn);
                pn = pn->n = new_node(c);
                ++size;
            }
            break;
        case lsKEYWORD:
            assert(pn);
            pn = pn->n = new_node(c);
            ++size;
            break;
        case lsPATH:
        case lsVARREG:
            if(!n) {
                n = new_node(c);
                pn = n;
                size = 1;
            } else {
                pn = pn->n = new_node(c);
                ++size;
            }
            break;
        default:
            // error?
            return BAD_TOKEN;
        }
    } while(1);

    switch(state) {
    case lsPATH:
        assert(n);
        yylex_move(n, size);
        yylval.str = yylex_buf;
        return PATH;
    case lsKEYWORD:
        assert(n);
        yylex_move(n, size);
        for(i = 3; i < YYNTOKENS; ++i) {
            if(0 == strncmp(yytname[i] + 1, yylex_buf, strlen(yylex_buf))) {
                return yytoknum[i];
            }
        }
        // error?
        return BAD_TOKEN;
    case lsVARREG:
        assert(n);
        yylex_move(n, size);
        if(isdigit(yylex_buf[0])) {
            yylval.num = atoi(yylex_buf);
            return REG;
        }
        yylval.str = yylex_buf;
        return IDENT;
    case lsNUMBER:
        yylval.num = 0;
        assert(n);
        while(n) {
            yylval.num *= 10;
            yylval.num += n->c - '0';
            pNode_t toDelete = n;
            n = n->n;
            free(toDelete);
        }
        return INT;
    default:
        // error?
        return BAD_TOKEN;
    }
}

int yyerror(char const* s)
{
    fprintf(stderr, "%s\n", s);
    return 0;
}
