.PHONY = all clean
CC = gcc # compiler
FLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

SRCS := $(wildcard *.c) # Get all C source files
OBJS := $(SRCS:%.c=%.o) # Generate C source file names with `.o` extension

all: ${OBJS:%.o=%}

%: %.o
	${CC} ${FLAGS} $< -o $@

%o: %.c
	${CC} -c $<

clean:
	rm ${OBJS} *.o
