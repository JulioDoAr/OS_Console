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

#define PARENT_READ readpipe[0]
#define CHILD_WRITE readpipe[1]
#define CHILD_READ writepipe[0]
#define PARENT_WRITE writepipe[1]

static const char EXIT[] = "exit";

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
            finish = true;
        else
        {
            int firstFork = 0;
            char *token = strtok(line_read, "|");
            while (token)
            {
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
                case -1: // FORK ERROR
                    perror("Fork: ");
                    exit(EXIT_FAILURE);
                    break;
                case 0:                 // CHILD
                    if (firstFork == 0) // OPEN PIPES
                    {
                        close(PARENT_WRITE);
                        close(PARENT_READ);
                        close(CHILD_READ);
                        dup2(CHILD_WRITE, STDOUT_FILENO);
                    }
                    else
                    {
                        close(PARENT_WRITE);
                        close(PARENT_READ);
                        // close(CHILD_WRITE);
                        // dup2(CHILD_READ, STDIN_FILENO);
                        close(CHILD_READ);
                        dup2(CHILD_WRITE, STDOUT_FILENO);
                    }

                    if (execvp(formattedInput.we_wordv[0], formattedInput.we_wordv) == -1) // EXECUTE COMMAND
                    {
                        perror("Error: ");
                        exit(EXIT_FAILURE);
                    }
                    if (firstFork == 0) // CLOSE PIPES
                    {
                        close(CHILD_WRITE);
                    }
                    else
                    {
                        close(CHILD_READ);
                        close(CHILD_WRITE);
                    }
                    exit(EXIT_SUCCESS);
                    break;
                default: // PARENT
                    token = strtok(NULL, "|");
                    waitpid(pid, &status, 0); // WAIT

                    // OPEN PIPES
                    close(CHILD_READ);
                    close(CHILD_WRITE);
                    close(PARENT_WRITE);
                    dup2(PARENT_WRITE, STDOUT_FILENO);

                    char c;
                    while (read(PARENT_READ, &c, sizeof(char)) > 0)
                    {
                        if (token == NULL) // Will not be more forks
                            printf("%c", c);
                        else
                        {
                            write(PARENT_WRITE, &c, sizeof(char));
                        }
                    }

                    // CLOSE PIPES
                    close(PARENT_READ);
                    close(PARENT_WRITE);
                    break;
                }
                firstFork++;
            }
        }
    }
    return 0;
}