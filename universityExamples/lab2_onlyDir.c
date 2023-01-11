#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: <exec> <folder>\n");
        return 1;
    }
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(argv[1])))
        return 2;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_DIR)
        {
            char path[512];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", argv[1], entry->d_name);
            printf("%2s[%s]\n", "", entry->d_name);
        }
        else
        {
            printf("%2s- %s\n", "", entry->d_name);
        }
    }
    closedir(dir);

    return 0;
}