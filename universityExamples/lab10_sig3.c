#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

int x;
static void handle_sigusr1(int sig)
{
    if (x == 0)
        printf("Remember multiplication is just repeating addition\n");
}

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
        int i = 5;
        while (i == 5)
        {
            sleep(5);
            kill(getppid(), SIGUSR1);
        }
    }
    else
    {
        struct sigaction sa;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = &handle_sigusr1;

        sigaction(SIGUSR1, &sa, NULL);

        printf("What is 2 * 5? ");
        scanf("%d", &x);
        if (x == 10)
            printf("Correct\n");
        else
            printf("Incorrect\n");

        kill(pid, SIGKILL);
        wait(NULL);
    }

    return 0;
}