#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int fd1, fd2, fd3, fd4;
    umask(0);
    if (argc != 4)
    {
        printf("Exec <arg1> <arg2> <arg3>\n");
        return 1;
    }
    fd1 = open(argv[1], O_CREAT, S_IRUSR | S_IWGRP | S_IXOTH);
    if (fd1 < 0)
    {
        printf("Errors at %s opening\n", argv[1]);
    }
    close(fd1);
    fd2 = open(argv[2], O_CREAT | O_RDONLY, S_IXOTH);
    if (fd2 < 0)
    {
        printf("Errors at %s opening\n", argv[2]);
    }
    close(fd2);
    fd3 = open(argv[3], O_CREAT, 0436);
    if (fd3 < 0)
    {
        printf("Errors at %s opening\n", argv[3]);
    }

    close(fd3);
    return 0;
}
