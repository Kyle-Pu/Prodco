#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

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
  
void produce(void *args){

  struct params *buffArgs = (struct params *) args; // Cast args into pointer type

  // Ensure proper ordering
  // If no slots are open, this thread should wait
  sem_wait(&buffArgs->num_open);

  // Enter critical section once appropriate
  pthread_mutex_lock(&buffArgs->mutex);
  buffArgs->buffer[buffArgs->currPos] = 22; // https://www.youtube.com/watch?v=AgFeZr5ptV8
  buffArgs->currPos++;
  pthread_mutex_unlock(&buffArgs->mutex); // Leave critical section

  // Wake any waiting consumers
  sem_post(&buffArgs->num_filled);
  
}

void consume(void *args){

  struct params *buffArgs = (struct params *) args; // Cast args into pointer type

  // Wait until there are items to consume
  sem_wait(&buffArgs->num_filled);

  // Critical section
  pthread_mutex_lock(&buffArgs->mutex);
  int val = buffArgs->buffer[buffArgs->currPos];
  buffArgs->currPos--;
  pthread_mutex_unlock(&buffArgs->mutex);

  // Wake any waiting producers
  sem_post(&buffArgs->num_open);
  
  printf("Read value is %d", val);
  
}

int main(int argc, char **argv){

  // Check inputs
  if(argc != 2 || atoi(argv[1]) <= 0){
    errno = EINVAL;
    perror("Invalid arguments! Please provide a positive, integer argument");
  }

  // Dynamically allocate bounded buffer of size specified by user through command line argument 1
  int buffSize = (int) atoi(argv[1]); // Truncates floating points
  int *buff = (int *)calloc(buffSize, sizeof(int));

  struct params *args = malloc(sizeof(struct params));
  args->buffer = buff;
  args->currPos = 0;
  args->maxSize = buffSize;
  sem_init(&args->num_open, 0, buffSize); // Buffer begins completely empty
  sem_init(&args->num_filled, 0, 0); // Buffer begins with no items to consume
  
  // Create threads
  const int NUM_THREADS = 10;
  pthread_t tid[NUM_THREADS];

  for(int k = 0; k < NUM_THREADS; k++){
    if(k % 2 == 0){
      pthread_create(tid+k, NULL, produce, args);
    }else{
      pthread_create(tid+k, NULL, consume, args);
    }
  }
  
  // Wait for threads to terminate
  for(int k = 0; k< NUM_THREADS; k++){
    pthread_join(tid[k], NULL);
  }
  
  // Free dynamically allocated memory
  free(buff);
  
}
