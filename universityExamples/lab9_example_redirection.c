#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int inputFile, outputFile, number, number2;
    if (argc != 3)
    {
        perror("Usage <exec> <input file> <output file>\n");
        exit(1);
    }

    if ((inputFile = open(argv[1], O_RDONLY | O_CREAT, 0777)) < 0)
    {
        perror("Error trying to open input file\n");
        exit(2);
    }

    if (dup2(inputFile, STDIN_FILENO) < 0)
    {
        perror("Error trying to duplicate input file\n");
        exit(3);
    }

    if ((outputFile = open(argv[2], O_WRONLY)) < 0)
    {
        perror("Error trying to open output file. Are you sure you created it?\n");
        exit(2);
    }

    if (dup2(outputFile, STDOUT_FILENO) < 0)
    {
        perror("Error trying to duplicate output file");
        exit(3);
    }

    scanf("%d %d", &number, &number2);
    printf("Sum of %d + %d = %d\n", number, number2, number + number2);

    close(inputFile);
    close(outputFile);
    return 0;
}