#ifndef LOG_H
#define LOG_H

#define JAK_NUM 0x01
#define JAK_STR 0x02
#define JAK_LN  0x80
#define JAK_TAB 0x40

typedef enum {
    FATAL = -2,
    ERROR = -1,
    WARNING = 0,
    INFO = 1,
    DEBUG = 2,
    TRACE = 3,
    PARSER_TRACE = 4
} log_level_t;

void jaklog(log_level_t level, int flags, void const* stuff);

void jaklog_set_level(log_level_t);

#endif
