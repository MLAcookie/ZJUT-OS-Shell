#include "shell_signal.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void signal_init(void)
{
    signal(SIGINT, signal_handle);
}

void signal_handle(int sig)
{
    fprintf(stdout, "SIG %d\n", sig);
}