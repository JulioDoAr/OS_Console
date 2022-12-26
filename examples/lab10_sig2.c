#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

static void handle_sigstp(int sig)
{
    printf("Cannot stop this executable. Your destruction is imminent\n");
}
// ctrl + \

int main()
{
    struct sigaction sa;

    sa.sa_handler = &handle_sigstp;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGCONT, &sa, NULL);

    int x;
    printf("get me a number: ");
    scanf("%d", &x);
    printf("Number got is %d\n", x);
    return 0;
}