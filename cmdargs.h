#ifndef CMDARGS_H
#define CMDARGS_H

typedef void (*cmdargs_callback_t)(char const*);

void HandleParameters(int argc, char* argv[]);

void args_set_debug_level(char const*);
void args_load(char const*);
void args_test(char const*);
void args_console_viewer_enable(char const*);
void args_input(char const*);
void args_remap_keys(char const*);
void args_list_joysticks(char const*);

#endif
