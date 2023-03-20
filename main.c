#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_READERS 14

// shared counter variable
int counter = 0;
int max_counter = 250000000;

// semaphore for readers
sem_t read_sem;

// semaphore for writers
sem_t write_sem;

// flag to indicate if writer is in critical section
int in_cs = 0;

void relaxandspendtime()
{
int i;
for(i = 0; i < 250000000; i++) i=i;
}

// function for readers
void* reader(void* arg) {
    int id = *(int*)arg;
    

    for (int i = 0; i >= 0; i++) {
        // acquire reader semaphore
        sem_wait(&read_sem);

        // check if writer is in critical section
        if (in_cs) {
            printf("Reader %d error: writer in critical section\n", id);
        }

        // read shared counter
        int val = counter;

        // release reader semaphore
        sem_post(&read_sem);

        // relax and spend time
        relaxandspendtime();
    
    if(val > = max_counter){
    printf("Reader %d done\n", id);
    return NULL;}}
}

// function for writer
void* writer(void* arg) {
    for (int i = 0; i >= 0; i++) {
        // acquire writer semaphore
        sem_wait(&write_sem);

        // set flag to indicate writer is in critical section
        in_cs = 1;

        // update shared counter
        for (int i = 0; i < 25000; i++) counter++;

        // reset flag to indicate writer is leaving critical section
        in_cs = 0;

        // release writer semaphore
        sem_post(&write_sem);

    

    printf("Writer done\n");
    return NULL;}
}

int main(int argc, char* argv[]) {
    // get number of readers from command line argument
    int n_readers = atoi(argv[1]);
    if (n_readers < 1) {
        fprintf(stderr, "Invalid number of readers\n");
        return 1;
    }
    if (n_readers > MAX_READERS) {
        fprintf(stderr, "Too many readers\n");
        return 1;
    }

    // initialize semaphores
    sem_init(&read_sem, 0, MAX_READERS);
    sem_init(&write_sem, 0, 1);

    // create reader threads
    pthread_t reader_threads[n_readers];
    for (int i = 0; i < n_readers; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&reader_threads[i], NULL, reader, id);
    }

    // start writer thread
    pthread_t writer_thread;
    pthread_create(&writer_thread, NULL, writer, NULL);

    // join reader threads
    for (int i = 0; i < n_readers; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    // join writer thread
    pthread_join(writer_thread, NULL);

    // destroy semaphores
    sem_destroy(&read_sem);
    sem_destroy(&write_sem);

    return 0;
}
