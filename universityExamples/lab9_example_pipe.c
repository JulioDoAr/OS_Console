#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv)
{
    int fd[2], id ,id2;
    if(argc != 2)
    {
        perror ("Usage <exec> <input file>\n");
        exit(1);
    }

    if(pipe(fd) < 0)
    {
        perror ("Error creating pipe\n");
        exit(2);
    }
    if ((id = fork()) < 0)
    {
        perror ("Error creating first child process\n");
        exit(4);
    }

    if(id == 0)
    {
        if (dup2(fd[1], STDOUT_FILENO) < 0)
        {
            perror("Error trying to duplicate output");
            exit(3);
        }
        close(fd[1]);
        close(fd[0]);
        execlp("cat", "cat", argv[1], NULL);
    }

    if ((id2 = fork()) < 0)
    {
        perror ("Error creating second child process\n");
        exit(6);
    }
    
    if (id2 == 0)
    {
        if (dup2(fd[0], STDIN_FILENO) < 0)
        {
            perror("Error trying to duplicate input\n");
            exit(5);
        }
        close(fd[1]);
        close(fd[0]);
        execlp("wc", "wc", "-w", NULL);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(id, NULL,  0);
    waitpid(id2, NULL, 0);
    return 0;
}