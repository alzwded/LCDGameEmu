#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "cmdargs.h"

typedef enum { YES =0, NO =1, FILE_NOT_FOUND =2 } HasParameter_t;

typedef struct Option_s {
    const char* LongName;
    const char ShortName;
    const HasParameter_t HasParameter;
    const char* Description;
    cmdargs_callback_t Callback;
} Option;

void GetHelp(char const*);

static const Option Options[] = {
    { "debug-level", 'd', YES, "set the debug level [-2..3]", &args_set_debug_level },
    { "load", 'l', YES, "load a game!", &args_load },
    { "help", 'h', NO, "print this message", &GetHelp },
    { "usage", '\0', NO, "print this message", &GetHelp },
    { NULL, '\0', FILE_NOT_FOUND, "NULL terminator", NULL }
};

void HandleParameters(int argc, char* argv[])
{
    int i;
    for(i = 1; i < argc; ++i) {
        char* s = argv[i];
        char* param;
        size_t len = strlen(s);
        const Option* p = &Options[0];
        for(;
            p->LongName != NULL && p->ShortName != '\0';
            ++p)
        {
            if(len >= 2) {
                if(s[0] == '-' && s[1] == '-') {
                    int lengthLongName = strlen(p->LongName);
                    if(strncmp(s + 2, p->LongName, lengthLongName) == 0)
                    {
                        if(len == 2 + lengthLongName) {
                            if(p->HasParameter == YES) {
                                if(++i < argc) {
                                    if(p->Callback)
                                    (p->Callback)(argv[i]);
                                    break;
                                } else {
                                    jaklog(FATAL, JAK_STR, "Parameter required for");
                                    jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, p->LongName);
                                    exit(-2);
                                }
                            } else if(p->HasParameter == NO) {
                                if(p->Callback)
                                (p->Callback)(NULL);
                                break;
                            } else if(p->HasParameter == FILE_NOT_FOUND) {
                                if(i+1 < argc) {
                                    if(strlen(argv[i+1]) >= 2 && argv[i+1][0] == '-') {
                                        if(p->Callback)
                                        (p->Callback)(NULL);
                                        break;
                                    } else {
                                        ++i;
                                        if(p->Callback)
                                        (p->Callback)(argv[i]);
                                        break;
                                    }
                                } else {
                                    ++i;
                                    if(p->Callback)
                                    (p->Callback)(argv[i]);
                                }
                            }
                        } else if(s[2 + lengthLongName] == '=') {
                            param = &s[2 + lengthLongName + 1];
                            if(p->HasParameter == NO) {
                                jaklog(FATAL, JAK_STR, "Extraneuous parameter for");
                                jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, p->LongName);
                                exit(-2);
                            } else {
                                if(p->Callback)
                                (p->Callback)(param);
                                break;
                            }
                        } else {
                            jaklog(FATAL, JAK_STR, "Unknown switch");
                            jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, s);
                            exit(-2);
                        }
                    } else {
                        continue;
                    }
                } else if(s[0] == '-') {
                    if(s[1] == p->ShortName) {
                        if(strlen(s) > 2) {
                            param = s + 2;
                            if(p->HasParameter == NO) {
                                jaklog(FATAL, JAK_STR, "Extraneuous parameter for");
                                jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, p->LongName);
                                exit(-2);
                            } else {
                                (p->Callback)(param);
                                break;
                            }
                        } else {
                            if(p->HasParameter == YES) {
                                jaklog(FATAL, JAK_STR, "Missing parameter for");
                                jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, p->LongName);
                                exit(-2);
                            } else {
                                (p->Callback)(NULL);
                                break;
                            }
                        }
                    } else {
                        continue;
                    }
                } else {
                    jaklog(FATAL, JAK_STR, "Unknown switch");
                    jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, s);
                    exit(-2);
                }
            } else {
                jaklog(FATAL, JAK_STR, "Unknown switch");
                jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, s);
                exit(-2);
            }
        }
        if(p->LongName == NULL && p->ShortName == '\0') {
            jaklog(FATAL, JAK_STR, "Unknown switch");
            jaklog(FATAL, JAK_TAB|JAK_STR|JAK_LN, s);
            exit(-2);
        }
    }
}

void GetHelp(char const* _)
{
    const Option* p = &Options[0];

    printf("lcdgameemu\noptions:\n");

    for(; p->LongName != NULL && p->ShortName != '\0'; ++p)
    {
#define INDENT_VALUE (2 + 15 + 1 + 2 + 1 + 3)
        static int const _indent = INDENT_VALUE;
        static int const _remainder = 79 - INDENT_VALUE;
        int lenOfDesc = strlen(p->Description);
        int i;
        printf("--%-15s %c%c   ", p->LongName, (p->ShortName) ? '-' : ' ', (p->ShortName) ? p->ShortName : ' ');
        if(strlen(p->LongName) > 15) printf("\n");

        for(i = 0; i < lenOfDesc / _remainder + 1; ++i)
        {
            int j;
            for(j = 0; j < _remainder; ++j) {
                if(i * _remainder + j > lenOfDesc) break;
                printf("%c", p->Description[i * _remainder + j]);
            }
            if(i * _remainder + j < lenOfDesc) {
                int k;
                printf("\n");
                for(k = 0; k < _indent; ++k) printf("%c", ' ');
            }
        }
        printf("\n");
    }
    exit(255);
}