#pragma once

#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <termios.h>

// 作为课设来说，应该是够大的
#define MAX_JOBS 1024

// 之后可能会写，现在还没实现
typedef enum
{
    DONE,
    RUNNING,
    STOPPED,
} job_status;

typedef struct
{
    pid_t pid;
    bool is_background;
    // struct termios termios;
} job;

extern job all_jobs[];
extern size_t job_count;

void job_add(pid_t pid, bool is_background);
job *job_find(pid_t pid);
void job_wait_all(void);
void job_to_forground(job *job);
void job_resume(job *job);