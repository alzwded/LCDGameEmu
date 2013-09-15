#ifndef jlH
#define jlH

#define jlNUM 0x01
#define jlSTR 0x02
#define jlLN  0x80
#define jlTAB 0x40

typedef enum {
    jlFATAL = -2,
    jlERROR = -1,
    jlWARNING = 0,
    jlINFO = 1,
    jlDEBUG = 2,
    jlTRACE = 3,
    jlPARSER_TRACE = 4
} log_level_t;

void jaklog(log_level_t level, int flags, void const* stuff);

void jaklog_set_level(log_level_t);

#endif
