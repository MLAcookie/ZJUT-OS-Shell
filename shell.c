#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "shell_cmd.h"
#include "shell_program.h"
#include "shell_signal.h"
#include "tokenizer.h"

/* Convenience macro to silence compiler warnings about unused function parameters. */
#define unused __attribute__((unused))

/* Whether the shell is connected to an actual terminal or not. */
bool shell_is_interactive;

/* File descriptor for the shell input */
int shell_terminal;

/* Terminal mode settings for the shell */
struct termios shell_tmodes;

/* Process group id for the shell */
pid_t shell_pgid;

/* Looks up the built-in command, if it exists. */
int lookup(char cmd[])
{
    for (unsigned int i = 0; i < cmd_get_size(); i++)
    {
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
        {
            return i;
        }
    }
    return -1;
}

/* Intialization procedures for this shell */
void init_shell()
{
    /* Our shell is connected to standard input. */
    shell_terminal = STDIN_FILENO;

    /* Check if we are running interactively */
    shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive)
    {
        /* If the shell is not currently in the foreground, we must pause the shell until it becomes a
         * foreground process. We use SIGTTIN to pause the shell. When the shell gets moved to the
         * foreground, we'll receive a SIGCONT. */
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
        {
            kill(-shell_pgid, SIGTTIN);
        }

        /* Saves the shell's process id */
        shell_pgid = getpid();

        /* Take control of the terminal */
        tcsetpgrp(shell_terminal, shell_pgid);

        /* Save the current termios to a variable, so it can be restored later. */
        tcgetattr(shell_terminal, &shell_tmodes);
    }

    signal_main_init();
}

int main(unused int argc, unused char *argv[])
{
    init_shell();
    static char line[4096];
    int line_num = 0;

    /* Please only print shell prompts when standard input is not a tty */
    if (shell_is_interactive)
    {
        fprintf(stdout, "%d: ", line_num);
    }

    while (fgets(line, 4096, stdin))
    {
        /* Split our line into words. */
        struct tokens *tokens = tokenize(line);

        /* Find which built-in function to run. */
        int fundex = lookup(tokens_get_token(tokens, 0));

        if (fundex >= 0)
        {
            cmd_table[fundex].fun(tokens);
        }
        else
        {
            program_execute(tokens);
        }

        if (shell_is_interactive)
        {
            /* Please only print shell prompts when standard input is not a tty */
            fprintf(stdout, "%d: ", ++line_num);
        }

        /* Clean up memory */
        tokens_destroy(tokens);
    }

    return 0;
}
