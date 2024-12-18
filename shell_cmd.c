#include "shell_cmd.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "job.h"

#define ARRAY_SIZE(ARR) (sizeof(ARR) / sizeof((ARR)[0]))

fun_desc_t cmd_table[] = {{cmd_help, "?", "show this help menu"},
                          {cmd_pwd, "pwd", "show current working path"},
                          {cmd_cd, "cd", "change current working path"},
                          {cmd_cls, "cls", "clean terminal content"},
                          {cmd_exit, "exit", "exit the command shell"},
                          {cmd_wait, "wait", "wait for all background processes to finish"},
                          {cmd_fg, "fg", "Move the process with id pid to the foreground"},
                          {cmd_bg, "bg", "Resume a paused background process"}};

int cmd_lookup(char cmd[])
{
    for (unsigned int i = 0; i < ARRAY_SIZE(cmd_table); i++)
    {
        if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0))
        {
            return i;
        }
    }
    return -1;
}

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
    return EXIT_SUCCESS;
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
        fprintf(stderr, "pwd: show work path failed\n");
        return 1;
    }
    return EXIT_SUCCESS;
}

int cmd_cd(struct tokens *tokens)
{
    if (chdir(tokens_get_token(tokens, 1)) == 0)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        fprintf(stderr, "cd: No such file or directory\n");
        return EXIT_FAILURE;
    }
}

int cmd_cls(struct tokens *tokens)
{
    printf("\033[2J\033[H");
    return EXIT_SUCCESS;
}

int cmd_exit(struct tokens *tokens)
{
    exit(EXIT_SUCCESS);
}

int cmd_wait(struct tokens *tokens)
{
    job_wait_all();
    return EXIT_SUCCESS;
}

int cmd_fg(struct tokens *tokens)
{
    job *p_job;
    if (tokens_get_length(tokens) == 1)
    {
        p_job = NULL;
    }
    else
    {
        pid_t pid = atoi(tokens_get_token(tokens, 1));
        p_job = job_find(pid);
    }
    job_to_forground(p_job);
    return EXIT_SUCCESS;
}

int cmd_bg(struct tokens *tokens)
{
    job *p_job;
    if (tokens_get_length(tokens) == 1)
    {
        p_job = NULL;
    }
    else
    {
        pid_t pid = atoi(tokens_get_token(tokens, 1));
        p_job = job_find(pid);
    }
    job_resume(p_job);
    return EXIT_SUCCESS;
}