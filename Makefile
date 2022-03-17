CC = gcc # compiler
FLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

SRCS := $(wildcard *.c) # Get all C source files
OBJS := $(SRCS:%.c=%.o)   # Get C source file names without extension

.PHONY = ProducerConsumer clean

ProducerConsumer: $(OBJS)
	${CC} ${FLAGS} $^ -o $@

clean:
	rm ${OBJS} ProducerConsumer
