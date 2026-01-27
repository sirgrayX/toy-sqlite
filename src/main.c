#include "shell.h"


int main()
{
    ShellState shell;
    shell_init(&shell);
    shell_run(&shell);
    shell_cleanup(&shell);
    return shell.exit_code;
}