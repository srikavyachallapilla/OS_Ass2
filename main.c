//Name : Sri Kavya Challapilla



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>

/*initializing the shared counter and read counter variable */

int sharedCounter = 0;
int read_count =0;


//declaring the semaphore variables
sem_t readerSemaphore;
sem_t writerSemaphore;
//decalring the flag variable 
int in_cs = 0; 






void *reader_thread(void *args)
{
    int id = ((int)args);/* reading each reader number to integer variable id*/
    for(int i = 0; i < 250000000; i++)
    {
        //ENTRY SECTION //
        sem_wait(&readerSemaphore);/* decrementing the reader semaphore to put a lock while read count is updating*/
        read_count++;/*updating the readcount i.e num of readers has increased by 1 */
        if (read_count == 1)
        {
            sem_wait(&writerSemaphore);//this ensure no writer to enter if there is any reader is accessing the shared counter/
        }
        sem_post(&readerSemaphore);//incrementing the reader semaphore to allow other reader to enter why this reader is in critical section/
        //checking for flag is 1 i.e if flag is set to 1 then writer is updating the shared counter/
        if(in_cs ==1){
            printf("Reader %d writer in critical section\n", id);
        }
        //CRITICAL SECTION //


        

        //EXIT SECTION //
        sem_wait(&readerSemaphore);/* when reader wants to leaves the critial section to provide mutual exclusion between readers */
        read_count--;//decrementing the readers count
        if (read_count == 0)
        {
            sem_post(&writerSemaphore);//no reader is left in the critical section where are incrementing the writer semaphore to allow writer to enter the critical section
        }
        sem_post(&readerSemaphore);//reader is leaving
        //REMAINDER SECTION //
        /* once the reader access the shared counter for 250000000 then printing that reader is done reading */
        
    }
    printf("Reader %d done\n", id);
    
}


void *writer_thread(void *args){
    for (int i = 0; i < 25000; i++) {
        //ENTRY SECTION//

        sem_wait(&writerSemaphore);//incrementing the writer semaphore variable i.e setting lock/
        // as per the question given setting the flag variable to 1 to allow writer to the critical section//
        in_cs = 1;
        //CRITICAL SECTION//
        //shared counter is incremented

        sharedCounter++;

        in_cs = 0;/* setting the flag variable to 0 when writer is done its part with shared counter */
        //EXIT SECTION//
        sem_post(&writerSemaphore);/*when writer leaves the critial decrement the writer semaphore */
    }
    //REMAINDER SECTION//
    printf("Writer done\n");

}





int main(int argc, char *argv[]) 
{
    
    int numOfReaders,k,i;
    //checking whether the input is entered or not
  

    if (argc == 1)
    {
      fprintf (stderr, "please enter the input\n");
      return -1;
    }
    //storing the enterd input into numofreaders variable.
    
    numOfReaders = atoi (argv[1]);
    //checks whether the number of readers are in between 0 and 14
    if (numOfReaders > 0 && numOfReaders < 15)
    {
        int readercount[numOfReaders];
        pthread_t readers[numOfReaders];
        pthread_t writer[1];
        pthread_attr_t attr[1];
        
        //initializing the semaphore variables
        //sem_init() function call will create semaphore variables respectively
        pthread_attr_init(&attr[0]);
        sem_init(&readerSemaphore, 0,1);
        sem_init(&writerSemaphore, 0, 1);
    
        k = (int) (numOfReaders/2);
        /*creating the k number of readerthreads */
        for(i = 0; i < k; i++)
        {
            readercount[i]=i;
            pthread_create(&readers[i], &attr[0], reader_thread,&readercount[i]);
        }
        /* Create the writer thread */
        pthread_create(&writer[0], &attr[0], writer_thread,NULL);
        /*creating the second k number of reader threads */
        for(i = k ; i < numOfReaders ; i++)
        {
            readercount[i]=i;
            pthread_create(&readers[i], &attr[0], reader_thread,&readercount[i]);
        }
        /*joining the first k number of reader threads */
    
        for(i = 0; i < k; i++)
        {
            pthread_join(readers[i], NULL);
        }
        //joining the writer thread/
        pthread_join(writer[0], NULL);
        //joining the second k number of reader threads/
        for(i = k ; i < numOfReaders ; i++)
        {
            pthread_join(readers[i], NULL);
        }
    
    
        
    
        
        //destroying the semaphore variable at the end which are cretaed earlier
        pthread_attr_destroy(&attr[0]);
        sem_destroy(&readerSemaphore);
        sem_destroy(&writerSemaphore);
    }
    //printing the statement if the input enter is greater than 14/
    else{
        printf("number of readers must be less than or equal to 14");
    }

    return 0;
}
