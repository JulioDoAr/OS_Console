#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *functie(void *threadid);

void *functie(void *threadid)
{
    printf("I am the son! My id is %ld\n", (long)threadid);

    pthread_exit((void *)6);
    return (void *)6;
    // exit(4);
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    long id = 5;

    // char s[] = "this is my name";

    // int v[] = {1, 2, 3, 4, 5, 6};

    int rc = pthread_create(&thread, NULL, functie, (void *)id);
    if (rc)
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    printf("This is the father!\n");

    void *status;
    pthread_join(thread, &status);
    printf("Everything was completed! Status %d\n", (int)status);
    pthread_exit(NULL);
}