#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

struct Monitor{
  int *buffer;
  int maxSize; // Limit of buffer
  int currPos; // Position of first unfilled slot in the buffer

  // For mutual exclusion
  pthread_cond_t has_empty; // Keep track of if buffer has empty slots
  pthread_cond_t has_filled; // Keep track of if buffer has filled slots
  pthread_mutex_t mutex;
};

void printBuff(struct Monitor *buffArgs){
  printf("Buffer now: [");
  for(int k = 0; k < buffArgs->maxSize - 1; k++){
    printf("%d, ", buffArgs->buffer[k]);
  }
  printf("%d]", buffArgs->buffer[buffArgs->maxSize-1]); // Print last element separately
  printf("\n");  
}

void* produce(void *args){
  struct Monitor *buffArgs = (struct Monitor *) args; // Cast args into pointer type

  pthread_mutex_lock(&buffArgs->mutex);
  
  // Join wait queue and release lock if producer thread has no empty slots to which it can fill
  if(buffArgs->currPos == buffArgs->maxSize){
    pthread_cond_wait(&buffArgs->has_empty, &buffArgs->mutex); // Re-acquires lock upon wakeup by call to signal on the has_empty condition variable
  }
  
  int writeVal = 22; // https://www.youtube.com/watch?v=AgFeZr5ptV8
  buffArgs->buffer[buffArgs->currPos] = writeVal;
  buffArgs->currPos++;

  printf("Wrote value %d at slot %d. ", writeVal, buffArgs->currPos-1);
  printBuff(buffArgs);
  
  pthread_mutex_unlock(&buffArgs->mutex); // Leave critical section
  // END of critical section
  
  // Wake a waiting consumer if any
  pthread_cond_signal(&buffArgs->has_filled);

  return NULL;
}

void* consume(void *args){
  struct Monitor *buffArgs = (struct Monitor *) args; // Cast args into pointer type

  // BEGIN of critical section
  pthread_mutex_lock(&buffArgs->mutex);

  // If there are no items to consume, wait until there are
  if(buffArgs->currPos == 0){
    pthread_cond_wait(&buffArgs->has_filled, &buffArgs->mutex);
  }
  
  int val = buffArgs->buffer[buffArgs->currPos-1];
  buffArgs->buffer[buffArgs->currPos-1] = 0; // Reset value at slot in buffer
  buffArgs->currPos--;

  printf("Read value is %d. ", val);
  printBuff(buffArgs);

  // Wake any waiting producers if any
  pthread_cond_signal(&buffArgs->has_empty);
  
  pthread_mutex_unlock(&buffArgs->mutex);
  // END of critical section
  
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
  int buffSize = (int) atoi(argv[1]); // Truncates floating point
  struct Monitor *args = malloc(sizeof(struct Monitor));
  args->buffer = (int *)calloc(buffSize, sizeof(int));
  args->currPos = 0;
  args->maxSize = buffSize;
  pthread_cond_init(&args->has_empty, NULL);
  pthread_cond_init(&args->has_filled, NULL);
  
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
  free(args->buffer); // Deallocate member in struct first
  free(args); // Deallocate struct

  // Successful return
  return 0;
}

