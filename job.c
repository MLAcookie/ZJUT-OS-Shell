#include "job.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

job all_jobs[MAX_JOBS];
size_t job_count = 0;

void job_add(pid_t pid, bool is_background)
{
    if (job_count < MAX_JOBS)
    {
        all_jobs[job_count].pid = pid;
        all_jobs[job_count].is_background = is_background;
        if (tcgetattr(STDIN_FILENO, &all_jobs[job_count].termios) == -1)
        {
            perror("tcgetattr failed");
            exit(EXIT_FAILURE);
        }
        job_count++;
    }
    else
    {
        fprintf(stderr, "Job list is full.\n");
    }
}

job *job_find(pid_t pid)
{
    for (int i = 0; i < job_count; i++)
    {
        if (all_jobs[i].pid == pid)
        {
            return &all_jobs[i];
        }
    }
    return NULL;
}

void job_wait_all(void)
{
    for (int i = 0; i < job_count; ++i)
    {
        waitpid(all_jobs[i].pid, NULL, 0);
    }
    job_count = 0; // 清空作业列表
}

bool job_to_forground(job *job)
{
    if (job->is_background == false)
    {
        return false;
    }
    tcsetpgrp(STDIN_FILENO, job->pid);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &job->termios);
    kill(job->pid, SIGCONT);
    job->is_background = false;
    waitpid(job->pid, NULL, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, getpgrp());
    return true;
}

bool job_resume(job *job)
{
    if (job->is_background == true)
    {
        return false;
    }
    kill(job->pid, SIGCONT);
    job->is_background = true;
    return true;
}