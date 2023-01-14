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

static const char EXIT[] = "exit";
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
            char *token = strtok(line_read, "|");
            while (token)
            {
                printf("token=%s\n", token);
                wordexp_t formattedInput;
                wordexp(token, &formattedInput, WRDE_REUSE);

                int status = 0;
                int pid = fork();
                switch (pid)
                {
                case -1: // ERROR
                    perror("Fork: ");
                    exit(EXIT_FAILURE);
                    break;
                case 0: // CHILD
                    if (execvp(formattedInput.we_wordv[0], formattedInput.we_wordv) <= 0)
                        exit(EXIT_SUCCESS);
                    perror("Error: ");
                    exit(EXIT_FAILURE);
                    break;
                default: // PARENT
                    waitpid(pid, &status, 0);
                }
                token = strtok(NULL, "|");
            }
        }
    }
    return 0;
}