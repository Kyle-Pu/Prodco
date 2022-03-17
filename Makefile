.PHONY = all clean

CC = gcc # compiler
#FLAGS = -lrt -pthread -Wl,-O1,--sort-common,--as-needed,-z,relro,-z,now

SRCS := $(wildcard *.c) # Get all C source files
OBJS := $(SRCS:%.c=%)   # Get C source file names without extension

all: ${OBJS}

%: %.o
	@echo "Checking..."

%.o: %.c
	@echo "Creating objects"
	${CC} -c $<

clean:
	rm *.o ${OBJS}
