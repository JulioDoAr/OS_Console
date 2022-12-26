#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
int main()
{
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Error at creating process\n");
        exit(1);
    }
    if (pid == 0)
    {
        while (1)
        {
            printf("line\n");
            // kill(getppid(),SIGKILL);
        }
    }
    else
    {
        sleep(2);
        kill(pid, SIGKILL);
        wait(NULL);
        for (int i = 0; i < 10; i++)
        {
            printf("hey\n");
        }
    }
    return 0;
}