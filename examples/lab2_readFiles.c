#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SIZE 10

int main(int argc, char **argv)
{
    int fd, n;
    char buffer[SIZE];
    if (argc != 2)
    {
        printf("Please use <exec> <file1>\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        printf("Error opening the file\n");
        return 2;
    }

    while ((n = read(fd, buffer, SIZE)) > 0)
    {
        buffer[n] = '\0';
        // memset(s,SIZE);
        printf("Bytes read: %d\n", n);
        printf("Content: %s\n\n", buffer);
    }
    if (n < 0)
    {
        printf("Error reading the file\n");
        return 3;
    }
    // system("./script");
    return 0;
}