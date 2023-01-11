#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int lenght, fd, n, i, fd2;
    char *buffer;

    if (argc != 2)
    {
        printf("Usage: <exec> <file1>\n");
        return 1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0)
    {
        printf("Error opening the file\n");
        return 2;
    }

    lenght = lseek(fd, 0, SEEK_END) - lseek(fd, 0, SEEK_SET);

    printf("%s has lenght %d\n", argv[1], lenght);

    buffer = (char *)malloc(lenght * sizeof(char));

    if (buffer == NULL) // if (buffer) <=> if (buffer != 0)
    {
        printf("Could not allocate memory\n");
        return 3;
    }
    else
    {
        if ((n = read(fd, buffer, lenght)) < 0)
        {
            printf("Could not read from %s", argv[1]);
            return 4;
        }
        buffer[lenght] = '\0';
        printf("Number of bytes read: %d\n", n);
        printf("File Content: \n%s\n\n", buffer);
    }

    if ((n = close(fd)) < 0)
    {
        printf("Error closing the file\n");
        return 5;
    }

    free(buffer);

    return 0;
}