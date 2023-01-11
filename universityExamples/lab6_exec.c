#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    long val;
    char *check;
    if (argc != 2)
    {
        printf("usage <exec> <option>\n");
        exit(1);
    }

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "-help"))
    {
        printf("Option 0: execl\n");
        printf("Option 1: execlp\n");
        printf("Option 2: execle\n");
        printf("Option 3: execv\n");
        printf("Option 4: execvp\n");
        printf("Option 5: execve\n");
        printf("Option 6: system\n");
        return 0;
    }

    val = strtol(argv[1], &check, 10);

    if ((check == argv[1]) || *check != '\0')
    {
        printf("not a valid option");
        exit(2);
    }

    char *envp[2] = {getenv("PATH"), NULL};

    switch (val)
    {
    case 0:
        printf("execl(\"/bin/ls\", \"ls\", \"-la\", NULL);\n");
        execl("/bin/lswehsd", "lssdhsh", "-la", NULL);
        break;
    case 1:
        printf("execlp(\"ls\", \"ls\", \"-la\", NULL);\n");
        execlp("ls", "ls", "-la", NULL);
        break;
    case 2:
        printf("execle(\"/bin/ls\", \"ls\", \"-la\", NULL,envp);\n");
        execle("/bin/ls", "ls", "-la", NULL, envp);
        break;
    case 3:
        printf("execv(\"bin/ls\", exec_arg);\n");
        char *execvArgs[] = {"bin/ls", "-la", ".", NULL};
        execv("/bin/ls", execvArgs);
        break;
    case 4:
        printf("execvp(\"ls\", exec_arg);\n");
        char *execvpArgs[] = {"ls", "-la", NULL};
        execvp("ls", execvpArgs);
        break;
    case 5:
        printf("execve(\"bin/ls\", exec_arg);\n");
        char *execveArgs[] = {"ls", "-la", NULL};
        execve("/bin/ls", execveArgs, envp);
        break;
    case 6:
        printf("system(\"ls -la\");\n");
        system("ls -la");
        exit(0);
        break;
    default:
        printf("Option range: 0-6\n");
        break;
    }

    printf("error at exec functions\n");

    return 0;
}