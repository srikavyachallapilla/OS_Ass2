#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_NUM_READERS 14
#define MAX_NUM_WRITERS 1
#define NUM_READS 250000000
#define NUM_WRITES 25000

/* Shared variables */
int counter = 0;
int in_cs = 0; /* flag indicating if writer is in critical section */
sem_t readSem, writeSem; /* semaphores */

/* Function prototypes */
void *reader_thread(void *arg);
void *writer_thread(void *arg);

/* Reader thread function */
void *reader_thread(void *arg)
{
    int id = (int) arg;
    int i;
    for (i = 0; i < NUM_READS; i++) {
        /* Enter critical section */
        sem_wait(&readSem);
        if (in_cs) {
            fprintf(stderr, "Error: reader %d detected writer in critical section\n", id);
        }
        sem_post(&readSem);
        
        /* Read shared variable */
        int val = counter;
        
        /* Exit critical section */
        sem_wait(&readSem);
        sem_post(&readSem);
        
        /* Relax */
        relaxandspendtime();
    }
    printf("Reader %d done\n", id);
    return NULL;
}

/* Writer thread function */
void *writer_thread(void *arg)
{
    int i;
    for (i = 0; i < NUM_WRITES; i++) {
        /* Enter critical section */
        sem_wait(&writeSem);
        in_cs = 1;
        
        /* Write shared variable */
        counter++;
        
        /* Exit critical section */
        in_cs = 0;
        sem_post(&writeSem);
        
        /* Relax */
        relaxandspendtime();
    }
    printf("Writer done\n");
    return NULL;
}

/* Main function */
int main(int argc, char *argv[])
{
    int i, k;
    int num_readers = atoi(argv[1]);
    
    /* Check input */
    if (num_readers < 1 || num_readers > MAX_NUM_READERS) {
        fprintf(stderr, "Error: invalid number of readers (1-%d)\n", MAX_NUM_READERS);
        exit(1);
    }
    
    /* Initialize semaphores */
    sem_init(&readSem, 0, 1);
    sem_init(&writeSem, 0, 1);
    
    /* Create threads */
    pthread_t readers[num_readers], writer[NUM_WRITES];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    /* Start readers */
    k = num_readers / 2;
    for (i = 0; i < k; i++) {
        pthread_create(&readers[i], &attr, reader_thread, (void*) i);
    }
    /* Start writer */
    pthread_create(&writer[0], &attr, writer_thread, NULL);
    /* Start remaining readers */
    for (i = k; i < num_readers; i++) {
        pthread_create(&readers[i], &attr, reader_thread, (void*) i);
    }
    
    /* Wait for threads to finish */
    for (i = 0; i < num_readers; i++) {
        pthread_join(readers[i], NULL);
    }
    pthread_join(writer[0], NULL);
    
    /* Cleanup */
    pthread_attr_destroy(&attr);
    sem_destroy(&readSem);
    sem_destroy(&writeSem);
    
    return 0;
}
