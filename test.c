#include <unistd.h>
#include <stdlib.h>
#include <wordexp.h>
#include <limits.h>
#include <stdbool.h>
#include <sys/wait.h>

#if defined(READLINE_LIBRARY)
#include "readline.h"
#include "history.h"
#else
#include <readline/readline.h>
#include <readline/history.h>
#endif

#define COLOR_BOLD "\e[1m"
#define RED "\x1b[31m"
#define GLOW "\x1b[1m"
#define YELLOW "\033[33m"
#define COLOR_OFF "\e[m"
#define RESET "\x1b[0m"

#define PARENT_READ readpipe[0]
#define CHILD_WRITE readpipe[1]
#define CHILD_READ writepipe[0]
#define PARENT_WRITE writepipe[1]

static const char EXIT[] = "exit";
static const int CHILD_PROCESS = 0;
static const int INPUT_END = 1;
static const int OUTPUT_END = 0;

static char current_directory[PATH_MAX];

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *readLineAndAddToHistoryIfNeccesary(char *line_read)
{
    if (line_read)
    {
        free(line_read);
        line_read = NULL;
    }
    line_read = readline("> ");
    if (line_read && *line_read)
        add_history(line_read);
    return (line_read);
}

int main(int const argc, char const *argv[])
{
    getcwd(current_directory, sizeof(current_directory));

    printf("Welcome to Julio Domínguez Arjona console:\n");
    bool finish = false;

    int readpipe[2], writepipe[2];
    char *line_read = NULL;
    while (!finish)
    {
        line_read = readLineAndAddToHistoryIfNeccesary(line_read);

        if (strncmp(line_read, EXIT, strlen(EXIT)) == 0)
        {
            finish = true;
        }
        else
        {
            int commandIndex = 0;
            char *token = strtok(line_read, " | ");
            while (token)
            {
                printf("Next token=%s\n", token);
                wordexp_t formattedInput;
                wordexp(token, &formattedInput, WRDE_REUSE);

                int status = 0;
                if (pipe(readpipe) < 0 || pipe(writepipe) < 0)
                {
                    perror("Cannot create pipe. ");
                    exit(EXIT_FAILURE);
                }
                int pid = fork();
                switch (pid)
                {
                case -1:
                    perror("Fork: ");
                    exit(EXIT_FAILURE);
                    break;
                case 0:

                    close(PARENT_WRITE);
                    // close(PARENT_READ);

                    dup2(CHILD_READ, STDIN_FILENO);
                    close(CHILD_READ);
                    dup2(CHILD_WRITE, STDOUT_FILENO);
                    // close(CHILD_WRITE);

                    if (execvp(formattedInput.we_wordv[0], formattedInput.we_wordv) == -1)
                    {
                        perror("Error: ");
                        exit(EXIT_FAILURE);
                    }
                    exit(EXIT_SUCCESS);
                    break;
                default:
                    printf("Waiting to finish fork...\n");
                    waitpid(pid, &status, 0);
                    printf("Fork finished!\n");

                    dup2(PARENT_READ, STDIN_FILENO);
                    char c;
                    while (read(PARENT_READ, &c, sizeof(char)) < 0)
                    {
                        printf("%c", c);
                    }
                    break;

                    close(CHILD_READ);
                    close(CHILD_WRITE);
                }
                commandIndex++;
                token = strtok(NULL, " | ");
            }
        }
    }
    return 0;
}