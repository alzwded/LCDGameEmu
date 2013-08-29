#ifndef CMDARGS_H
#define CMDARGS_H

typedef void (*cmdargs_callback_t)(char const*);

void HandleParameters(int argc, char* argv[]);

void args_set_debug_level(char const*);
void args_load(char const*);

#endif
