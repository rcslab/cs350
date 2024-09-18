
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#define NTHR 8

#define NARR (1024*1024)
int array[NARR];

typedef struct Args {
    int start;
    int end;
    int max;
} Args;

void *
search(void *arg)
{
    Args *args = arg;

    args->max = array[0];

    for (int i = args->start; i < args->end; i++) {
	if (args->max < array[i])
	    args->max = array[i];
    }

    pthread_exit(args);
}

int
main(int argc, const char *argv[])
{
    pthread_t thr[NTHR];
    Args args[NTHR];
    
    void *rval;

    // Fill in random values
    arc4random_buf(&array[0], sizeof(array));

    uint64_t start = __builtin_readcyclecounter();
    for (int i = 0; i < NTHR; i++) {
	args[i].start = NARR/NTHR*i;
	args[i].end = NARR/NTHR*(i+1);
	pthread_create(&thr[i], NULL, &search, &args[i]);
    }

    for (int i = 0; i < NTHR; i++) {
	pthread_join(thr[i], &rval);
    }
    uint64_t end = __builtin_readcyclecounter();

    int max = args[0].max;
    for (int i = 0; i < NTHR; i++) {
	if (max < args[i].max)
	    max = args[i].max;
    }

    printf("Threads: %d Cycles: %llu\n", NTHR, end - start);
    printf("Max: %d\n", max);
    printf("Max (Hex) %08x\n", max);

    return 0;
}

