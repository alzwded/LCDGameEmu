#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static log_level_t jaklog_level = WARNING;

static int col = -8;

void jaklog_set_level(log_level_t level)
{
    jaklog_level = level;
    extern int yydebug;
    if(level >= PARSER_TRACE) {
        yydebug = 1;
    } else {
        yydebug = 0;
    }
}

void jaklog(log_level_t level, int flags, void* stuff)
{
    if(jaklog_level < level) return;

    if(col < 0) {
        char s[9];
        time_t now;
        struct tm* t;

        now = time(NULL);
        t = localtime(&now);

        col = 0;
        strftime(s, 9, "%H:%M:%S", t);

        fprintf(stderr, "%s", s);
    }

    if(flags & JAK_TAB) {
        if(col < 72 / 2) {
            while(col < 72 / 2) {
                fprintf(stderr, " ");
                col++;
            }
        } else {
            fprintf(stderr, "\n");
            col = -8;
        }
        return;
    }
    if(flags & JAK_NUM) {
        unsigned num = *(unsigned*)stuff;
        fprintf(stderr, "%d", num);
        while(num) {
            col++;
            num /= 10;
        }
    } else if(flags & JAK_STR) {
        char* s = (char*)stuff;
        fprintf(stderr, "%s", s);
        col += strlen(s);
    }
    if(flags & JAK_LN || col > 72) {
        col = -8;
        fprintf(stderr, "\n");
    }
}
