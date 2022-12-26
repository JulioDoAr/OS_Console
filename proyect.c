#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define RED "\x1b[31m"
#define GLOW "\x1b[1m"
#define RESET "\x1b[0m"
int MAX_INPUT_LINE = 1024;
int ARROW_UP = 24;
char EXIT[] = "exit";
char LS[] = "ls";
char TAC[] = "tac";
char DIRNAME[] = "dirname";
int CHILD_PROCESS = 0;

/**
 * in - Must be opened
 * out - Must be opened
 */
int *executeProcess(int in[2], char command[])
{
    close(in[0]);
    int out[2];
    execl("/bin/sh", "sh", "-c", command, (char *)NULL);
}

void ls_readFolder(DIR *dir, bool modifiers[])
{
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
    }
}

/**
 * The "ls" command. Parameters that need to be implemented are: -l, -s, -a, -F
 * -a Does not ignore files that starts with .
 * -F Classify. append indicator (one of \*\/=>@|) to entries
 * -l     use a long listing format
 * -S     sort by file size, largest first
 */
void ls_command(int argc, char const *argv[])
{
}

int main(int argc, char const *argv[])
{
    printf("Welcome to Julio Domínguez Arjona console\n");
    bool exit = false;
    char line[MAX_INPUT_LINE];
    u_char lineIndex = 0;

    while (!exit)
    {
        /*
        char c = getchar();
        printf("Key readed %c", c);
        if(c == ARROW_UP){

        } else if (C == NEXT_LINE){

        }*/

        printf("> ");
        fgets(line, MAX_INPUT_LINE, stdin);

        if (strncmp(line, EXIT, strlen(EXIT)) == 0)
        {
            exit = true;
        }
        else
        {
            if (strncmp(line, LS, strlen(LS)))
            {
                char *modifiers = strtok(line, " | ");
                ls_command(line);
            }

            // char *token = strtok(line, " | ");
            // while (token != NULL)
            // {
            //     int fd[2];
            //     if (pipe(fd) < 0)
            //     {
            //         perror("Error opening the pipe\n");
            //         return 1;
            //     }
            //     int id;
            //     if ((id = fork()) < 0)
            //     {
            //         perror("Error creating fork\n");
            //         return 2;
            //     }

            //     if (id == CHILD_PROCESS)
            //     {
            //     }
            //     else
            //     {
            //         printf("%s\n", token);
            //         token = strtok(NULL, " | ");
            //     }
            // }
        }

        // To pipe between processes pipe();
    }
    return 0;
}
