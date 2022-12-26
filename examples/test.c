#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int id = fork();
    if (id < 0)
    {
        perror("Error creating fork\n");
        exit(3);
    }

    if (id == 0)
    {
        printf("Child 1\n");
        id = fork();
        if (id < 0)
        {
            perror("Error creating fork\n");
            exit(3);
        }
        if (id == 0)
        {
            printf("Child 2\n");
            id = fork();
            if (id < 0)
            {
                perror("Error creating fork\n");
                exit(3);
            }
        }
        else
        {
            printf("Parent 1\n");
        }
    }
    else
    {
        printf("Parent\n");
    }
}