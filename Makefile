.PHONY = all clean
CC = gcc # compiler
FLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

SRCS := $(wildcard *.c) # Get all C source files
BINS := $(SRCS:%.c=%) # Generate object file names

all: ${BINS:%=%.o} ${BINS}

%: %.o
	${CC} ${FLAGS} $< -o $@

%.o: %.c
	${CC} -c $<

clean:
	rm ${BINS} *.o
