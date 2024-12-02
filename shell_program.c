#include "shell_program.h"

#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int program_run(const char *path, struct tokens *tokens)
{
    char *argv[64];
    int argv_count = tokens_get_length(tokens);
    argv[argv_count] = NULL;
    argv[0] = strdup(path);

    for (int i = 1; i < argv_count; i++)
    {
        argv[i] = strdup(tokens_get_token(tokens, i));
    }
    bool flag = execv(path, argv);
    if (flag)
    {
        fprintf(stderr, "%s failed\n", tokens_get_token(tokens, 0));
        return 1;
    }
    else
    {
        return 0;
    }
}

int program_find_path(const char *name, struct tokens *tokens)
{
    char *env_path = getenv("PATH");
    char path[PATH_MAX];
    if (env_path == NULL)
    {
        fprintf(stderr, "there is no PATH\n");
        exit(1);
    }
    char *path_copy = strdup(env_path);

    char *token = strtok(path_copy, ":");
    while (token != NULL)
    {
        path[0] = '\0';
        strcat(path, token);
        strcat(path, "/");
        if (access(strcat(path, name), F_OK) == 0)
        {
            return program_run(path, tokens);
        }
        token = strtok(NULL, ":");
    }
    return 1;
}

void program_execute(struct tokens *tokens)
{
    const char *program = tokens_get_token(tokens, 0);
    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        fprintf(stderr, "fork failed\n");
        exit(1);
    }
    else if (child_pid == 0)
    {
        char cwd[PATH_MAX];
        getcwd(cwd, sizeof(cwd));
        strcat(cwd, "/");
        // 判断完整路径
        if (access(program, F_OK) == 0)
        {
            exit(program_run(program, tokens));
        }
        // 判断当前路径
        else if (access(strcat(cwd, program), F_OK) == 0)
        {
            exit(program_run(strcat(cwd, program), tokens));
        }
        // 判断path
        else
        {
            exit(program_find_path(program, tokens));
        }
    }
    else
    {
        int status;
        waitpid(child_pid, &status, 0);
    }
}