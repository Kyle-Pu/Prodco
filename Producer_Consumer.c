#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

  // Check inputs
  if(argc != 2 || atoi(argv[1]) <= 0){
    errno = EINVAL;
    perror("Invalid arguments! Please provide a positive, integer argument");
  }

  // Dynamically allocate bounded buffer of size specified by user through command line argument 1
  int buffSize = (int) atoi(argv[1]); // Truncates floating points
  int* buff = (int*)calloc(buffSize, sizeof(int));   
  // Create an instance of a semaphore struct for tracking open slots
  
  
  // Create an instance of a semaphore struct for tracking filled slots
  // Create functions for producing and consuming. Use global pointer to track index
  
  // Free memory
  
}
