#include "shell_cmd.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define unused __attribute__((unused))
#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

fun_desc_t cmd_table[] = {
    {cmd_help, "?", "show this help menu"},
    {cmd_pwd, "pwd", "show current working path"},
    {cmd_cd, "cd", "change current working path"},
    {cmd_exit, "exit", "exit the command shell"},
};

int cmd_get_size()
{
    return ARRAY_SIZE(cmd_table);
}

int cmd_help(struct tokens *tokens)
{
    for (unsigned int i = 0; i < ARRAY_SIZE(cmd_table); i++)
    {
        printf("%s - %s\n", cmd_table[i].cmd, cmd_table[i].doc);
    }
    return 1;
}

int cmd_pwd(struct tokens *tokens)
{
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        fprintf(stderr, "show work path failed\n");
        return 1;
    }
    return 0;
}

int cmd_cd(struct tokens *tokens)
{
    if (chdir(tokens_get_token(tokens, 1)) == 0)
    {
        return 0;
    }
    else
    {
        fprintf(stderr, "change work path failed\n");
        return -1;
    }
}

int cmd_exit(struct tokens *tokens)
{
    exit(0);
}
