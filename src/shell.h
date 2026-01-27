#ifndef SHELL_H

#define SHELL_H 

#include <stdbool.h>

typedef struct 
{
    bool running;
    int exit_code;
} ShellState;

void shell_init(ShellState* state);
void shell_cleanup(ShellState* state);

void shell_run(ShellState* state);

void shell_process_command(ShellState* state, const char* comand);

#endif