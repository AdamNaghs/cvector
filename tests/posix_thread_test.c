#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "../mem_debug.h"

#define NUM_THREADS 5
#define NUM_ALLOCATIONS 10
#define ALLOCATION_SIZE 100

/* Thread function */
void *allocateMemory(void *param)
{
    int i;
    for (i = 0; i < NUM_ALLOCATIONS; i++)
    {
        /* Allocate memory but do not free it */
        char *memory = (char *)malloc(ALLOCATION_SIZE);
        if (memory == NULL)
        {
            printf("Failed to allocate memory\n");
            return (void *)1;
        }
        /* Uncomment the following line to fix the memory leak */
        /* free(memory); */
    }
    return 0;
}

int main()
{
    pthread_t threads[NUM_THREADS];
    /* Create the threads */
    int i;
    for (i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, allocateMemory, NULL) != 0) {
            printf("Failed to create thread\n");
            return 1;
        }
    }
    /* Wait for all threads to finish */
    for (i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    /* Check for memory leaks */
    int *j = malloc(sizeof(int));


    MEM_DEBUG_END(stdout);
    return 0;
}
