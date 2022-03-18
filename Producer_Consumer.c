#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

struct params{
  int *buffer;
  int maxSize; // Limit of buffer
  int currPos; // Position of first unfilled slot in the buffer

  // For mutual exclusion
  pthread_mutex_t mutex;
  
  // Semaphores to track how many slots are open and how many are filled
  sem_t num_open;
  sem_t num_filled;
};

void printBuff(struct params *buffArgs){
  printf("Buffer now: [");
  for(int k = 0; k < buffArgs->maxSize - 1; k++){
    printf("%d, ", buffArgs->buffer[k]);
  }
  printf("%d]", buffArgs->buffer[buffArgs->maxSize-1]); // Print last element separately
  printf("\n");  
}

void* produce(void *args){
  struct params *buffArgs = (struct params *) args; // Cast args into pointer type

  // Ensure proper ordering
  // If no slots are open, this thread should wait
  sem_wait(&buffArgs->num_open);

  // Enter critical section once appropriate
  pthread_mutex_lock(&buffArgs->mutex);
  int writeVal = 22; // https://www.youtube.com/watch?v=AgFeZr5ptV8
  buffArgs->buffer[buffArgs->currPos] = writeVal;
  buffArgs->currPos++;

  printf("Wrote value %d at slot %d. ", writeVal, buffArgs->currPos-1);
  printBuff(buffArgs);
  
  pthread_mutex_unlock(&buffArgs->mutex); // Leave critical section
  // END of critical section
  
  // Wake any waiting consumers
  sem_post(&buffArgs->num_filled);

  return NULL;
}

void* consume(void *args){
  struct params *buffArgs = (struct params *) args; // Cast args into pointer type

  // Wait until there are items to consume
  sem_wait(&buffArgs->num_filled);

  // BEGIN of critical section
  pthread_mutex_lock(&buffArgs->mutex);
  int val = buffArgs->buffer[buffArgs->currPos-1];
  buffArgs->buffer[buffArgs->currPos-1] = 0; // Reset value at slot in buffer
  buffArgs->currPos--;

  printf("Read value is %d. ", val);
  printBuff(buffArgs);
  
  pthread_mutex_unlock(&buffArgs->mutex);
  // END of critical section
  
  // Wake any waiting producers
  sem_post(&buffArgs->num_open);
  
  return NULL;
}

int main(int argc, char **argv){
  // Check inputs
  // All inputs must be positive integers
  // Number of consumers < number of producers
  // Number of producers - number of consumers must be <= size of buffer
  if(argc != 4 || atoi(argv[1]) <= 0 || atoi(argv[2]) <= 0 || atoi(argv[3]) <= 0 || (atoi(argv[3]) > atoi(argv[2])) || (atoi(argv[2])-atoi(argv[3])>atoi(argv[1]))){
    errno = EINVAL;
    perror("Invalid arguments! Please provide a positive, integer buffer size, number of producer threads, and number of consumer threads. The number of consumers must not exceed the number of producers. The number of producers minus the number of consumers must not exceed the size of the buffer.");
    exit(errno);
  }

  // Dynamically allocate bounded buffer of size specified by user through command line argument 1
  int buffSize = (int) atoi(argv[1]); // Truncates floating points
  int *buff = (int *)calloc(buffSize, sizeof(int));

  struct params *args = malloc(sizeof(struct params));
  args->buffer = buff;
  args->currPos = 0;
  args->maxSize = buffSize;
  sem_init(&args->num_open, 0, buffSize); // Buffer begins completely empty // TODO: sem_t is deprecated on OS X. Test on a Linux platform and write a working version for OS X
  sem_init(&args->num_filled, 0, 0); // Buffer begins with no items to consume
  
  // Create threads
  const int NUM_PRODUCERS = (int) atoi(argv[2]);
  const int NUM_CONSUMERS = (int) atoi(argv[3]);
  const int NUM_THREADS = NUM_PRODUCERS + NUM_CONSUMERS;
  pthread_t tid[NUM_THREADS];
  
  for(int k = 0; k < NUM_THREADS; k++){
    if(k < NUM_CONSUMERS){
      pthread_create(tid+k, NULL, consume, args);
    }else{
      pthread_create(tid+k, NULL, produce, args);
    }
  }
  
  // Wait for threads to terminate
  for(int k = 0; k< NUM_THREADS; k++){
    pthread_join(tid[k], NULL);
  }
  
  // Free dynamically allocated memory
  free(buff);
  free(args);

  // Successful return
  return 0;
}

