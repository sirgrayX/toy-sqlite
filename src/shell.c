#include "shell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void shell_init(ShellState* state)
{
    state->running = true;
    state->exit_code = 0;

}

void shell_cleanup(ShellState* state)
{
    (void)state;
}

void shell_process_command(ShellState* state, const char* command)
{
    while(*command == ' ' || *command == '\t') command++;

    if (strlen(command) == 0) return;
    if (strcmp(command, ".exit") == 0)
    {
        printf("Завершение работы...\n");
        state->running = false;
        state->exit_code = 0;
        return;
    }
    if (strcmp(command, ".help") == 0)
    {
        printf("Специальные команды:\n");
        printf(".exit - выход из программы\n");
        printf(".help - показать эту справку\n");
        printf(".tables - показать список таблиц (позже)\n");
        printf("\nSQL команды (частично поддерживаются):\n");
        printf("CREATE TABLE ...\n");
        printf("INSERT INTO ...\n");
        printf("SELECT ...\n");
        return;
    }
    printf("Команда распознана: '%s'\n", command);
    printf("(Реализация в процессе разработки)\n");

}

void shell_run(ShellState* state)
{
    char* line = NULL;
    printf("Добро пожаловать в SQLite Lite!\n");
    printf("Введите '.help' для справки, '.exit' для выхода.\n\n");

    while(state->running)
    {
        line = readline("toy-sqlite > ");

        if (!line)
        {
            printf("\n");
            break;
        }
        
        if (strlen(line) > 0) add_history(line);
        shell_process_command(state, line);
        free(line);
    }
}