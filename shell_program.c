#include "shell_program.h"

#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#include "shell_signal.h"

char *program_get_full_path(const char *path);

// 子进程执行外部程序
int program_run(struct tokens *tokens)
{
    char *argv[64];
    int argv_count = 1;
    argv[0] = program_get_full_path(tokens_get_token(tokens, 0));

    bool is_input_redirect = false;
    bool is_output_redirect = false;
    char input_file_path[PATH_MAX];
    char output_file_path[PATH_MAX];
    input_file_path[0] = '\0';
    output_file_path[0] = '\0';

    for (int i = 1; i < tokens_get_length(tokens); i++)
    {
        char *temp = tokens_get_token(tokens, i);
        if (strcmp(temp, "<") == 0)
        {
            is_input_redirect = true;
            char *p_file_path = tokens_get_token(tokens, i + 1);
            strcat(input_file_path, program_get_full_path(p_file_path));
            break;
        }
        if (strcmp(temp, ">") == 0)
        {
            is_output_redirect = true;
            char *p_file_path = tokens_get_token(tokens, i + 1);
            strcat(output_file_path, program_get_full_path(p_file_path));
            break;
        }
        argv[i] = strdup(temp);
        argv_count++;
    }
    argv[argv_count] = NULL;

    if (is_input_redirect)
    {
        int fd = open(input_file_path, O_RDONLY);
        if (fd < 0)
        {
            fprintf(stderr, "open input failed\n");
            return EXIT_FAILURE;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (is_output_redirect)
    {
        int fd = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            fprintf(stderr, "open output failed\n");
            return EXIT_FAILURE;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if (execv(argv[0], argv))
    {
        fprintf(stderr, "%s failed\n", tokens_get_token(tokens, 0));
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}

// 获取完整路径名
char *program_get_full_path(const char *path)
{
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    strcat(cwd, "/");
    // 判断完整路径
    if (access(path, F_OK) == 0)
    {
        return strdup(path);
    }
    // 判断path
    else
    {
        char *env_path = getenv("PATH");
        char full_path[PATH_MAX];
        if (env_path == NULL)
        {
            fprintf(stderr, "there is no PATH\n");
            return NULL;
        }
        char *path_copy = strdup(env_path);
        char *token = strtok(path_copy, ":");
        while (token != NULL)
        {
            full_path[0] = '\0';
            strcat(full_path, token);
            strcat(full_path, "/");
            if (access(strcat(full_path, path), F_OK) == 0)
            {
                return strdup(full_path);
            }
            token = strtok(NULL, ":");
        }
    }
    return NULL;
}

bool program_check_background(struct tokens *tokens)
{
    const char *end_token = tokens_get_token(tokens, tokens_get_length(tokens) - 1);
    return strcmp(end_token, "&") == 0 ? true : false;
}

// 外部程序运行
void program_execute(struct tokens *tokens)
{
    bool is_background = program_check_background(tokens);
    pid_t child_pid = fork();
    if (child_pid == 0)
    {
        setpgrp();
        if (!is_background)
        {
            tcsetpgrp(STDIN_FILENO, getpgrp());
        }
        signal_child_init();
        exit(program_run(tokens));
    }
    waitpid(child_pid, NULL, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, getpgrp());
}