#include "job.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

job all_jobs[MAX_JOBS];
size_t job_count = 0;

job *recent_job = NULL;

void job_add(pid_t pid, bool is_background)
{
    if (job_count < MAX_JOBS)
    {
        all_jobs[job_count].pid = pid;
        all_jobs[job_count].is_background = is_background;
        // 神秘测试机调tcgetattr会报错，注释算了
        // if (tcgetattr(STDIN_FILENO, &all_jobs[job_count].termios) == -1)
        // {
        //     perror("tcgetattr failed");
        //     exit(EXIT_FAILURE);
        // }
        recent_job = &all_jobs[job_count];
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
        fprintf(stdout, "wait PID: %d\n", all_jobs[i].pid);
        waitpid(all_jobs[i].pid, NULL, 0);
    }
    job_count = 0; // 清空作业列表
}

void job_to_forground(job *job)
{
    job = job == NULL ? recent_job : job;
    tcsetpgrp(STDIN_FILENO, job->pid);
    // tcsetattr(STDIN_FILENO, TCSADRAIN, &job->termios);
    kill(job->pid, SIGCONT);
    job->is_background = false;
    waitpid(job->pid, NULL, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, getpgrp());
}

void job_resume(job *job)
{
    job = job == NULL ? recent_job : job;
    kill(job->pid, SIGCONT);
    job->is_background = true;
}