#include "log.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAGIC_COL_SPAN 70

static log_level_t jaklog_level = jlWARNING;

static int col = -8;

void jaklog_set_level(log_level_t level)
{
    jaklog_level = level;
    extern int yydebug;
    if(level >= jlPARSER_TRACE) {
        yydebug = 1;
    } else {
        yydebug = 0;
    }
}

void jaklog(log_level_t level, int flags, void const* stuff)
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

        fprintf(stderr, "%-8s ", s);
    }

    if(flags & jlTAB) {
        if(col < MAGIC_COL_SPAN / 2) {
            while(col < MAGIC_COL_SPAN / 2) {
                fprintf(stderr, " ");
                col++;
            }
        } else {
            fprintf(stderr, "\n");
            fprintf(stderr, "%-8c ", '+');
            col = 0;
        }
    }
    if(flags & jlNUM) {
        unsigned num = *(unsigned*)stuff;
        unsigned copy = num;
        while(num) {
            col++;
            num /= 10;
        }
        if(col > MAGIC_COL_SPAN) {
            col = 0;
            fprintf(stderr, "\n%-8c ", '+');
        }
        fprintf(stderr, "%d", copy);
    } else if(flags & jlSTR) {
        char* s = (char*)stuff;
        for(; s && *s; ++s) {
            if(col > MAGIC_COL_SPAN) {
                col = 0;
                fprintf(stderr, "\n%-8c ", '+');
            }
            col++;
            fprintf(stderr, "%c", *s);
        }
    }
    if(flags & jlLN) {
        col = -8;
        fprintf(stderr, "\n");
    } else if(col > MAGIC_COL_SPAN) {
        col = 0;
        fprintf(stderr, "\n%-8c ", '+');
    }
}
