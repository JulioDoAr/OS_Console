#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    int aflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    char *dvalue = NULL;
    int dval = 0;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "abc:d:")) != -1)
        switch (c)
        {
        case 'a':
            aflag = 1;
            break;
        case 'b':
            bflag = 1;
            break;
        case 'c':
            cvalue = optarg;
            // Check if cvalue is between '0' and '9'. Its simpler...
            if ((*cvalue) != NULL)
            {
                if ((*cvalue) < '0' || (*cvalue) > '9')
                {
                    printf('c arg must be a number');
                    return -1;
                }
            }
            break;
        case 'd':
            dvalue = optarg;
            dval = (*cvalue - '0') + aflag + bflag + (*dvalue - '0');
            break;
        case '?':
            if (optopt == 'c')
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            if (optopt == 'd')
                dval = 0;
            else if (isprint(optopt))
                fprintf(stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Unknown option character `\\x%x'.\n",
                        optopt);
            return 1;
        default:
            abort();
        }

    printf("aflag=%d, bflag=%d, cvalue=%s, dvalue=%d\n",
           aflag, bflag, cvalue, dval);

    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
    return 0;
}