#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <wordexp.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

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
#define RESET "\x1b[0m"
#define COLOR_OFF "\e[m"

int MAX_INPUT_LINE = 1024;
int ARROW_UP = 24;
char EXIT[] = "exit";
char LS[] = "ls";
char TAC[] = "tac";
char DIRNAME[] = "dirname";
char MAN[] = "man";
int CHILD_PROCESS = 0;

static char *line_read = (char *)NULL;
static char current_directory[PATH_MAX];

void utils_openDir(char *path, DIR *directory)
{
    directory = NULL;
    if (path != NULL)
    {
        if (path[0] == '/')
        {
            printf("Triying to open \"%s\"\n", path);
            // if (access(path, F_OK))
            directory = opendir(path);
        }
        else
        {
            char fullPath[PATH_MAX];
            strcpy(fullPath, current_directory);
            strcat(fullPath, "/");
            strcat(fullPath, path);
            printf("Triying to open \"%s\"\n", fullPath);
            // if (access(fullPath, F_OK))
            directory = opendir(fullPath);
            // else
            //     printf("Unable to open \"%s\"\n", fullPath);
        }
    }
}

void utils_printArgcArgv(int const argc, char **argv)
{
    printf("argc=%d\n", argc);
    printf("argv: ");
    for (int i = 0; i < argc; i++)
    {
        printf("%d=%s, ", i, argv[i]);
    }
    printf("\n");
}

void dirname_printFirectory(char path[])
{
    char pathName[PATH_MAX];
    int index = 0, pathIndex = 0;
    while (path[pathIndex] != '\0')
    {
        if (path[pathIndex] == '/')
            while (index < pathIndex)
            {
                pathName[index] = path[index];
                index++;
            }
        pathIndex++;
    }
    if (index == 0)
    {
        pathName[index] = '.';
        index++;
    }
    pathName[index] = '\0';
    printf("%s\n", pathName);
}

void dirname_command(int const argc, char **argv)
{
    if (argc == 1)
    {
        printf("DIRNAME - ERR: Missing operand\n");
        return;
    }

    for (int i = 1; i < argc; i++)
    {
        dirname_printFirectory(argv[i]);
    }
}

void tac_command(int const argc, char **argv)
{
    bool modifierB = false;
    bool modifierS = false;
    char *source = NULL, *destiny = NULL;
    DIR *sourceDIR = NULL, *destinyDIR = NULL;

    int opt;
    while ((opt = getopt(argc, argv, "bs")) != -1)
    {
        switch (opt)
        {
        case 'b':
            modifierB = true;
            break;
        case 's':
            modifierS = true;
            break;
        default:
            printf("TAC - INF: Only -b, -s parameters are accepted\n");
            break;
        }
    }

    for (; optind < argc; optind++)
    {
        if (strncmp(argv[optind], TAC, strlen(TAC)) == 0)
            continue;
        else if (source == NULL)
            source = argv[optind];
        else if (destiny == NULL)
            destiny = argv[optind];
    }

    if (source == NULL)
    {
        printf("TAC - ERR: Need to provide source files.\nExample: tac <source>\n");
        return;
    }

    utils_openDir(source, sourceDIR);
    if (!sourceDIR)
    {
        printf("TAC - ERR: Source file does not exist. %s\n", source);
        return;
    }
    if (destiny)
    {
        utils_openDir(destiny, destinyDIR);
        if (destinyDIR == NULL)
        {
            printf("TAC - ERR: Destiny file does not exist. %s\n", destiny);
            return;
        }
    }

    printf("TAC - INF: Source=%s, Destiny=%s\n", source, destiny);
}

/**
 * Return the number of entries inside a directory. The directory is rewinded after count.
 */
int ls_countEntries(DIR *dir, bool countHidden)
{
    int total = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!countHidden) //-a Does not ignore files that starts with.
            if (entry->d_name[0] == '.')
                continue;
        total++;
    }
    rewinddir(dir);

    return total;
}

void ls_fillEntries(DIR *dir, bool countHidden, int totalEntries, struct dirent **entries)
{
    int index = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!countHidden) //-a Does not ignore files that starts with.
            if (entry->d_name[0] == '.')
                continue;
        entries[index] = entry;
        index++;
    }
}

long int ls_getFileSize(char *basePath, struct dirent *entry)
{
    int size = 0;
    if (entry->d_type == DT_DIR)
        return size;

    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);

    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        printf("File Not Found!\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fclose(fp);

    return size;
}

void ls_sortEntries(struct dirent **entries, int totalEntries, char *basePath)
{
    int entriesSize[totalEntries];

    char path[PATH_MAX];
    for (int i = 0; i < totalEntries; i++)
    {
        strcpy(path, basePath);
        strcat(path, "/");
        strcat(path, entries[i]->d_name);

        struct stat fStat;
        lstat(path, &fStat);
        // entriesSize[i] = ls_getFileSize(basePath, entries[i]);
        entriesSize[i] = fStat.st_size;
    }

    struct dirent *tmp;
    int tmpSize;
    for (int i = 0; i < totalEntries; i++)
    { // Bubble sort algoritm
        for (int j = 0; j < totalEntries - i - 1; j++)
        {
            if (entriesSize[j] < entriesSize[j + 1])
            {
                tmp = entries[j];
                entries[j] = entries[j + 1];
                entries[j + 1] = tmp;

                tmpSize = entriesSize[j];
                entriesSize[j] = entriesSize[j + 1];
                entriesSize[j + 1] = tmpSize;
            }
        }
    }
}

void ls_execute(DIR *dir, bool *modifiers, char *basePath)
{
    int totalEntries = ls_countEntries(dir, modifiers[0]);

    if (totalEntries == 0)
        return;

    struct dirent **entries = (struct dirent **)malloc(totalEntries * sizeof(struct dirent *));

    ls_fillEntries(dir, modifiers[0], totalEntries, entries);

    if (modifiers[3])
    {
        ls_sortEntries(entries, totalEntries, basePath);
    }

    struct dirent *entry = NULL;
    char filename[NAME_MAX + 1];
    char path[PATH_MAX];
    for (int i = 0; i < totalEntries; i++)
    {
        entry = entries[i];
        strcpy(filename, entry->d_name);

        if (modifiers[1])
        { //-F Classify. append indicator (one of \*\/=>@|) to entries
            switch (entry->d_type)
            {
            case DT_DIR: // if the entry is a directory;
                strcat(filename, "\\");
                break;
            case DT_LNK: // if the entry is a symbolic link;
                strcat(filename, "@");
                break;
            case DT_REG: // after each executable file,
                strcat(filename, "*");
                break;
            }
        }

        if (modifiers[2])
        { //-l     use a long listing format
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, filename);

            struct stat fStat;
            lstat(path, &fStat);
            struct passwd *pw = getpwuid(fStat.st_uid);
            struct group *gr = getgrgid(fStat.st_gid);

            // typeOfFile
            printf((S_ISDIR(fStat.st_mode)) ? "d" : (S_ISLNK(fStat.st_mode)) ? "l"
                                                                             : "-");
            // The File permissions
            printf((fStat.st_mode & S_IRUSR) ? "r" : "-");
            printf((fStat.st_mode & S_IWUSR) ? "w" : "-");
            printf((fStat.st_mode & S_IXUSR) ? "x" : "-");
            printf((fStat.st_mode & S_IRGRP) ? "r" : "-");
            printf((fStat.st_mode & S_IWGRP) ? "w" : "-");
            printf((fStat.st_mode & S_IXGRP) ? "x" : "-");
            printf((fStat.st_mode & S_IROTH) ? "r" : "-");
            printf((fStat.st_mode & S_IWOTH) ? "w" : "-");
            printf((fStat.st_mode & S_IXOTH) ? "x" : "-");

            // Hard links
            printf(" %zu ", fStat.st_nlink);

            // Name of file owner
            printf("%s ", pw->pw_name);
            // Name of group
            printf("%s ", gr->gr_name);

            // File size
            printf("%zu ", fStat.st_size);
            // Last updated date and time
            // FIX: 1673451237 - ene 11 17:34
            printf("%zu ", fStat.st_mtime);
            // Dedicated file name
            printf("%s \n", filename);
        }
        else
            printf("%s\n", filename);
    }
    free(entries);
}

/**
 * The "ls" command. Parameters that need to be implemented are: -l, -s, -a, -F
 * -a Does not ignore files that starts with .
 * -F Classify. append indicator (one of \*\/=>@|) to entries
 * -l     use a long listing format
 * -S     sort by file size, largest first
 */
void ls_command(int const argc, char **argv)
{
    char *lsBasePath = NULL;
    DIR *dir = NULL;
    bool modifiers[4];
    modifiers[0] = false;
    modifiers[1] = false;
    modifiers[2] = false;
    modifiers[3] = false;

    int opt;
    while ((opt = getopt(argc, argv, ":aFlS")) != -1)
    {
        switch (opt)
        {
        case 'a':
            modifiers[0] = true;
            break;
        case 'F':
            modifiers[1] = true;
            break;
        case 'l':
            modifiers[2] = true;
            break;
        case 'S':
            modifiers[3] = true;
            break;
        default:
            printf("LS - INF: Only -a, -F, -l, -S parameters are accepted\n");
            break;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        printf("%d=", i);
        printf(modifiers[i] ? "true" : "false");
        printf(", ");
    }
    printf("\n");

    for (int i = optind + 1; i < argc; i++)
    {
        // printf("%s -", argv[i]);
        // if (strncmp(argv[i], LS, strlen(LS)) == 0)
        //     continue;
        // else if (lsBasePath == NULL)
        lsBasePath = argv[i];
    }

    if (lsBasePath == NULL)
        lsBasePath = current_directory;

    dir = opendir(lsBasePath);
    if (dir == NULL)
    {
        exit(EXIT_FAILURE);
    }

    ls_execute(dir, modifiers, lsBasePath);
}

void man_command(int const argc, char **argv)
{
    if (argc == 1)
    {
        printf("\"ls\" command. Implemente with moifiers: -l, -s, -a, -F\n");
        printf("\"tac\" command. Implemente with moifiers: -b, -s\n");
        printf("\"dirname\" command.\n");
        printf("\"exit\" command. Exit the application.\n");
        printf("Any other command can be executed, but it is not full tested.\n");
    }
    else if (argc == 2)
    {
        if (strncmp(argv[1], LS, strlen(LS)) == 0)
        {
            printf("LS(1)  User Commands   LS(1)\n");
            printf("%sNAME%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    ls - list directory contents\n\n");
            printf("%sSYNOPSIS%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    ls [OPTION]... [FILE]...\n\n");
            printf("%sDESCRIPTION%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    List  information  about  the FILEs (the current directory by default).\n\n");
            printf("    Mandatory  arguments  to  long  options are mandatory for short options too.\n\n");

            printf("%s-a%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    do not ignore entries starting with .\n\n");
            printf("%s-F%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    append indicator (one of */=>@|) to entries\n\n");
            printf("%s-l%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    use a long listing format\n\n");
            printf("%s-s%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    sort by file size, largest first\n\n");
        }
        else if (strncmp(argv[1], TAC, strlen(TAC)) == 0)
        {
            printf("TAC(1)  User Commands   TAC(1)\n");
            printf("%sNAME%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    tac - concatenate and print files in reverse\n\n");
            printf("%sSYNOPSIS%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    tac [OPTION]... [FILE]...\n\n");
            printf("%sDESCRIPTION%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    Write each FILE to standard output, last line first.\n\n");
            printf("    Mandatory  arguments  to  long  options are mandatory for short options too.\n\n");

            printf("%s-b%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    attach the separator before instead of after\n\n");
            printf("%s-s%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    use STRING as the separator instead of newline\n\n");
        }
        else if (strncmp(argv[1], DIRNAME, strlen(DIRNAME)) == 0)
        {
            printf("DIRNAME(1)  User Commands   DIRNAME(1)\n");
            printf("%sNAME%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    dirname  -  strip last component from file name\n\n");
            printf("%sSYNOPSIS%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    dirname [OPTION] NAME...\n\n");
            printf("%sDESCRIPTION%s\n", COLOR_BOLD, COLOR_OFF);
            printf("    Output each NAME with its last non-slash component and trailing slashes removed; if NAME contains no /'s, output '.' (meaning the current directory).\n\n");
        }
    }
    return;
}

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *readLineAndAddToHistoryIfNeccesary()
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
    bool exit = false;

    while (!exit)
    {
        readLineAndAddToHistoryIfNeccesary();

        if (strncmp(line_read, EXIT, strlen(EXIT)) == 0)
        {
            exit = true;
        }
        else
        {
            wordexp_t formattedInput;
            wordexp(line_read, &formattedInput, WRDE_REUSE);

            if (strncmp(line_read, MAN, strlen(MAN)) == 0)
                man_command(formattedInput.we_wordc, formattedInput.we_wordv);
            else if (strncmp(line_read, LS, strlen(LS)) == 0)
                ls_command(formattedInput.we_wordc, formattedInput.we_wordv);
            else if (strncmp(line_read, TAC, strlen(TAC)) == 0)
                tac_command(formattedInput.we_wordc, formattedInput.we_wordv);
            else if (strncmp(line_read, DIRNAME, strlen(DIRNAME)) == 0)
                dirname_command(formattedInput.we_wordc, formattedInput.we_wordv);
            else
            {
                execvp(formattedInput.we_wordv[0], formattedInput.we_wordv + 1);
                perror(argv[0]);
            }
        }
    }
    return 0;
}
