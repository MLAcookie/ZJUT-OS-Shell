// 用于测试的需要长时间运行的任务

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int loop_times = argc == 1 ? 8 : atoi(argv[1]);
    for (int i = 0; i < loop_times; i++)
    {
        fprintf(stdout, "LOOP: %d\n", i + 1);
        sleep(1);
    }
    return 0;
}