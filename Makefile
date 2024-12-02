SRCS=shell.c tokenizer.c shell_cmd.c shell_program.c shell_signal.c
EXECUTABLES=shell

CC=gcc
CFLAGS=-g3 -Wall -Werror -std=gnu99 

OBJS=$(SRCS:.c=.o)

all: $(EXECUTABLES)

$(EXECUTABLES): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLES) $(OBJS)
