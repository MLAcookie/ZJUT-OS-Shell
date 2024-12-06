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

#include "job.h"
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
            strcat(input_file_path, tokens_get_token(tokens, i + 1));
            i++;
            continue;
        }
        if (strcmp(temp, ">") == 0)
        {
            is_output_redirect = true;
            strcat(output_file_path, tokens_get_token(tokens, i + 1));
            i++;
            continue;
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
    int status = execv(argv[0], argv);
    if (status != 0)
    {
        fprintf(stderr, "%s failed\n", tokens_get_token(tokens, 0));
        fprintf(stderr, "exit: %d\n", status);
    }
    return status;
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

// 判断是否是后台启动
bool program_check_background(struct tokens *tokens)
{
    const char *end_token = tokens_get_token(tokens, tokens_get_length(tokens) - 1);
    return strcmp(end_token, "&") == 0 ? true : false;
}

int program_get_pipe_count(struct tokens *tokens)
{
    int count = 0;
    for (int i = 0; i < tokens_get_length(tokens); i++)
    {
        if (strcmp(tokens_get_token(tokens, i), "|") == 0)
        {
            count++;
        }
    }
    return count;
}

void program_close_all_pipe(int pipes[][2], int pipe_count)
{
    for (int i = 0; i < pipe_count; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

// 外部程序运行
void program_execute(struct tokens *tokens)
{
    int pipe_count = program_get_pipe_count(tokens);
    int pipes[pipe_count][2];
    // pipe的初始化
    for (int i = 0; i < pipe_count; i++)
    {
        pipe(pipes[i]);
    }
    int start_index = 0;
    int pipe_index = 0;
    bool is_background = program_check_background(tokens);
    size_t tokens_length = tokens_get_length(tokens);
    // 如果是后台执行，就排除最后的&参数
    tokens_length = is_background ? tokens_length - 1 : tokens_length;
    pid_t pgid = 0;
    pid_t child_pid;
    for (int token_index = 0; token_index < tokens_length; token_index++)
    {
        // 如果遇到 | 和末尾就运行
        if (strcmp(tokens_get_token(tokens, token_index), "|") == 0 || token_index == tokens_length - 1)
        {
            child_pid = fork();
            // 最后一个token需要特别处理
            size_t end_index = token_index == tokens_length - 1 ? tokens_length - 1 : token_index - 1;
            struct tokens *sub_tokens = tokens_get_sub_tokens(tokens, start_index, end_index);
            if (child_pid == 0)
            {
                signal_child_init();
                // 子进程设置自己的进程组
                if (pipe_index == 0)
                {
                    pgid = getpid();
                    setpgrp();
                }
                else
                {
                    setpgid(0, pgid);
                }

                if (pipe_index > 0)
                {
                    dup2(pipes[pipe_index - 1][0], STDIN_FILENO);
                }
                if (pipe_index < pipe_count)
                {
                    dup2(pipes[pipe_index][1], STDOUT_FILENO);
                }
                program_close_all_pipe(pipes, pipe_count);
                int status = program_run(sub_tokens);
                exit(status);
            }
            // 貌似比较重复，先注释了
            // if (pipe_index == 0)
            // {
            //     setpgid(child_pid, child_pid);
            //     pgid = child_pid;
            // }
            // else
            // {
            //     setpgid(child_pid, pgid);
            // }
            // 前后台处理
            job_add(child_pid, is_background);
            if (!is_background)
            {
                // 前台给到子进程组
                tcsetpgrp(STDIN_FILENO, pgid);
            }
            else
            {
                // 这行只用于fg/bg的debug测试，提交的话要注释掉
                // fprintf(stdout, "background task: [%d] %s\n", child_pid, tokens_get_token(sub_tokens, 0));
            }
            tokens_destroy(sub_tokens);
            start_index = token_index + 1;
            pipe_index++;
        }
    }
    program_close_all_pipe(pipes, pipe_count);
    // 如果是后台任务，就不阻塞等待
    waitpid(-pgid, NULL, WUNTRACED | (is_background ? WNOHANG : 0));
    // 前台归还给shell
    tcsetpgrp(STDIN_FILENO, getpgrp());
}