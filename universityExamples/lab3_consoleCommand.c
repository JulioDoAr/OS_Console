#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

void copyFile(char *sourc, char *dest)
{
    FILE *source = fopen(sourc, "r");
    FILE *destiny = fopen(dest, "w");

    if (destiny == NULL)
    {
        /* File not created hence exit */
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }

    char ch = getc(source);
    while (ch != EOF)
    {
        fputc(ch, destiny);
        ch = getc(source);
    }

    fclose(source);
    fclose(destiny);
}

void cpCommand(int argc, char **argv)
{
    printf("CP Command detected\n");
    if (argc != 4)
    {
        printf("Usage: cp <fileSource> <fileDestination>\n");
        exit(EXIT_FAILURE);
    }

    // Check if source exist. If does not exist -> ERROR
    if (access(argv[2], F_OK) != 0)
    {
        printf("File source does not exist.\n");
        exit(EXIT_FAILURE);
    }
    if (access(argv[2], R_OK) != 0)
    {
        printf("PERMISSION PROBLEM: Does not have permissions to read.\n");
        exit(EXIT_FAILURE);
    }
    if (access(argv[3], F_OK) == 0) // Destination exist
    {
        if (access(argv[3], W_OK) != 0)
        {
            printf("PERMISSION PROBLEM: Does not have permissions to write.\n");
            exit(EXIT_FAILURE);
        }
    }

    copyFile(argv[2], argv[3]);
}

void copyFolder(char *source, char *destination)
{
    DIR *dir = opendir(source);
    if (dir == NULL)
    {
        printf("ERROR: %s does not exist", source);
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        bool errorInside = false;
        char sourcePath[512];
        snprintf(sourcePath, sizeof(sourcePath), "%s/%s", source, entry->d_name);
        char destinationPath[512];
        snprintf(destinationPath, sizeof(destinationPath), "%s/%s", source, entry->d_name);

        if (entry->d_type == DT_DIR)
        {
            if (entry->d_name[0] != '.')
                copyFolder(sourcePath, destinationPath);
        }
        else
            copyFile(sourcePath, destinationPath);
    }
}

bool deleteFolder(char *actualPath)
{
    DIR *dir = opendir(actualPath);
    if (dir == NULL)
    {
        printf("ERROR: %s does not exist", actualPath);
        exit(EXIT_FAILURE);
    }

    bool error = true;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        bool errorInside = false;
        char path[512];
        snprintf(path, sizeof(path), "%s/%s", actualPath, entry->d_name);
        if (entry->d_type == DT_DIR)
        {
            if (entry->d_name[0] != '.')
            {
                errorInside = deleteFolder(path);
            }
        }
        if (!errorInside)
        {
            if (remove(path) != 0)
            {
                error = true;
            }
        }
    }
    return error;
}

void readFolder(DIR *dir, int level, char *actualPath)
{
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        for (int i = 0; i < level; i++)
            printf("   ");

        if (entry->d_type == DT_DIR)
        {
            printf("DIRECTORY %s\n", entry->d_name);
            if (entry->d_name[0] != '.')
            {
                char path[512];
                snprintf(path, sizeof(path), "%s/%s", actualPath, entry->d_name);
                DIR *dirAux = opendir(path);
                if (dirAux != NULL)
                    readFolder(dirAux, level + 1, path);
            }
        }
        else
        {
            printf("FILE %s\n", entry->d_name);
        }
    }
}

void lsCommand(int argc, char **argv)
{
    printf("LS Command detected\n");
    if (argc != 3)
    {
        printf("Usage: ls <directory>\n");
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(argv[2]);
    if (dir == NULL)
    {
        printf("Error opening folder.\n");
        exit(EXIT_FAILURE);
    }
    readFolder(dir, 0, argv[2]);
}

void mvCommand(int argc, char **argv)
{
    printf("MV Command detected\n");
    if (argc != 4)
    {
        printf("Usage: MV <source> <destination>\n");
        exit(EXIT_FAILURE);
    }

    // Detect if file or directory
    struct stat s;
    if (stat(argv[2], &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            copyFolder(argv[2], argv[3]);
            deleteFolder(argv[2]);
        }
        else if (s.st_mode & S_IFREG)
        {
            copyFile(argv[2], argv[3]);
            remove(argv[2]);
        }
        else
        {
            printf("Can not move that thing...");
        }
    }
    else
    {
        printf("ERROR: ");
    }
}

int main(int argc, char **argv)
{
    printf("Received args:\n");
    for (int i = 0; i < argc; i++)
    {
        printf("    %s\n", argv[i]);
    }

    if (argc < 2)
    {
        printf("Usage: <command>\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp("cp", argv[1]) == 0)
    {
        cpCommand(argc, argv);
    }
    else if (strcmp("ls", argv[1]) == 0)
    {
        lsCommand(argc, argv);
    }
    else if (strcmp("mv", argv[1]) == 0)
    {
        mvCommand(argc, argv);
    }
    else
    {
        printf("Command not found");
    }
}