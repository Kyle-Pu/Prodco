#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

struct params{
  int *buffer;
  pthread_mutex_t mutex;
  int currPos; // Position of first unfilled slot in the buffer
}
  
void produce(void *args){

  struct params *buffArgs = (struct params *) args; // Cast args into pointer type

}

void consume(void *args){

  struct params *buffArgs = (struct params *) args; // Cast args into pointer type
  
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
  
  // Semaphores to track how many slots are open and how many are filled
  sem_t num_open;
  sem_t num_filled;

  sem_init(&num_open, 0, buffSize);
  sem_init(&num_filled, 0, 0);
  
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
