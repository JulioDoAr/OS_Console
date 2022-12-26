#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>

void readFolder(DIR *dir, int level)
{
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        for (int i = 0; i < level; i++)
        {
            printf("    ");
        }

        printf(entry->d_name[0] + '\n');
        if (entry->d_type == DT_DIR)
        {
            if (entry->d_name[0] != '.')
            {
                DIR *daux = opendir(entry->d_name);
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: <exec> <file>\n");
        exit(EXIT_FAILURE);
    }

    DIR *dir = opendir(argv[1]);
    if (dir == NULL)
    {
        printf("Error opening folder.\n");
        exit(EXIT_FAILURE);
    }
    readFolder(dir, 0);
}