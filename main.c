//including library functions
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

//defining macros
#define MAX_READERS 14

int counter = 0;

int max_read = 250000000;

//declaring two semaphores for read and write
sem_t reader_sem;
sem_t writer_sem;

//flag indicating if writer is in critical section 
int in_cs = 0; 

//waits for a fixed time 
void relax_and_spend_time() {
    int i;
    for (i = 0; i < 250000000; i++) {
        // do nothing
    }
}

//reader function
void* reader(void* arg) {
    int id = (int)arg;

    for (int i = 0; i < max_read; i++) {
        // acquire reader semaphore
        sem_wait(&reader_sem);

        // check if writer is in critical section
        if (in_cs) {
            printf("Reader %d error: writer in critical section\n", id);
        }

        // read shared counter
        int val = counter;

        // release reader semaphore
        sem_post(&reader_sem);

        // calling relax and spend time method
        relax_and_spend_time();

        if (val >= max_read) {
            printf("Reader %d done\n", id);
            return NULL;
        }
    }
}



//writer function
void* writer(void* arg) {
    while(1) {
        // acquire writer semaphore
        sem_wait(&writer_sem);
        
        // set flag to indicate writer is in critical section
        in_cs = 1;

        // shared counter update
        for (int i = 0; i < max_read; i++) {
            counter++;
        }

        // reset flag to indicate writer is leaving critical section
        in_cs = 0;

        // release writer semaphore
        sem_post(&writer_sem);

        printf("Writer done\n");
        return NULL;
    }
}

int main(int argc, char* argv[]) {
    // get number of readers from command line argument
    int num_readers = atoi(argv[1]);
    if (num_readers < 1) {
        fprintf(stderr, "Invalid number of readers\n");
        return 1;
    }
    if (num_readers > MAX_READERS) {
        fprintf(stderr, "Too many readers\n");
        return 1;
    }

    // initialize semaphores
    sem_init(&reader_sem, 0, 1);
    sem_init(&writer_sem, 0, 1);

    // create reader threads
    pthread_t reader_threads[MAX_READERS];
    for (int i = 0; i < num_readers; i++) {
        int id=0;
        id = i;
        pthread_create(&reader_threads[i], NULL, reader, id);
    }

    // start writer thread
    pthread_t writer_thread;
    pthread_create(&writer_thread, NULL, writer, NULL);

    // join reader threads
    for (int i = 0; i < num_readers; i++) {
        pthread_join(reader_threads[i], NULL);
    }

    // join writer thread
    pthread_join(writer_thread, NULL);

    // destroy semaphores
    sem_destroy(&reader_sem);
    sem_destroy(&writer_sem);
return 0;
}
