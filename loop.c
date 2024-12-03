// 用于测试的需要长时间运行的任务

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    for (int i = 0; i < 8; i++)
    {
        fprintf(stdout, "LOOP: %d\n", i + 1);
        sleep(1);
    }
    return 0;
}