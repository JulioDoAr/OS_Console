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

static char *line_read = (char *)NULL;

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
void ls_command(char *input)
{
    bool modifiers[4];
    modifiers[0] = (strstr(input, "-a") != NULL);
    modifiers[1] = (strstr(input, "-F") != NULL);
    modifiers[2] = (strstr(input, "-l") != NULL);
    modifiers[3] = (strstr(input, "-S") != NULL);

    // int cwd = current_path();
    //  DIR actualDir;
}

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *readLine()
{
    if (line_read)
    {
        free(line_read);
        line_read = (char *)NULL;
    }
    line_read = readline("");
    if (line_read && *line_read)
        add_history(line_read);
    return (line_read);
}

int main(int argc, char const *argv[])
{
    printf("Welcome to Julio Domínguez Arjona console\n");
    bool exit = false;

    while (!exit)
    {
        readLine();

        if (strncmp(line_read, EXIT, strlen(EXIT)) == 0)
        {
            exit = true;
        }
        else
        {
            if (strncmp(line_read, LS, strlen(LS)))
            {
                ls_command(line_read);
            }
        }
    }
    return 0;
}
