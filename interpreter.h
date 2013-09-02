#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "machine.h"
#include "interfaces.h"

void interpreter_eval(machine_t*, code_t*);

#endif
