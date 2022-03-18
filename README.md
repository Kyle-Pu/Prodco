# Producer Consumer

An interactive, command-line producer-consumer (bounded buffer) simulation written in C. Logs all actions and buffer status on each action (producing or consuming).

## Building

Clone this repository and run `make` in the repo's directory. Note that the semaphores used here are deprecated on OS X.

## Running

`./ProducerConsumer BUFFER_SIZE NUM_PRODUCERS NUM_CONSUMERS`

- e.g. `./ProducerConsumer 10 15 8` creates a buffer of size limit 10 with 15 producer threads and 8 consumer threads

## Resources and References

1) [Learn about Makefiles](https://opensource.com/article/18/8/what-how-makefile#:~:text=The%20make%20utility%20requires%20a,be%20installed%20using%20make%20install%20.)
2) [Mutex Lock Examples](https://docs.oracle.com/cd/E19455-01/806-5257/sync-12/index.html)

## C Syntax Aside

- `->` has higher precedence than `&`

