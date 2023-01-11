#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int fd[2], length, id;
    if (argc != 2)
    {
        perror("Usage: <exec> <string>\n");
        exit(1);
    }

    if (pipe(fd) < 0)
    {
        perror("Error opening the pipe\n");
        exit(2);
    }
    length = strlen(argv[1]);

    if ((id = fork()) < 0)
    {
        perror("Error creating fork\n");
        exit(3);
    }

    if (id == 0)
    {
        close(fd[0]);
        if (write(fd[1], argv[1], length) < 0)
        {
            perror("Error writing to the pipe printed from child\n");
            exit(4);
        }
        printf("Wrote %s to the pipe\n", argv[1]);
        close(fd[1]);
    }
    else
    {
        // wait(NULL);
        char *buffer = (char *)malloc(sizeof(char) * length);
        printf("Buffer value before reading: %s\n", buffer);
        close(fd[1]);
        if (read(fd[0], buffer, length) < 0)
        {
            perror("Error in parent reading from the pipe\n");
            exit(4);
        }
        printf("Read %s from the pipe\n", buffer);
        close(fd[0]);
        free(buffer);
    }
    return 0;
}