SRCS=shell.c tokenizer.c shell_cmd.c shell_program.c shell_signal.c
EXECUTABLES=shell

LOOP_SRC=loop.c
LOOP_EXECUTABLES=loop

PRINT_STDIN_SRC=print_stdin.c
PRINT_STDIN_EXECUTABLES=print_stdin

CC=gcc
CFLAGS=-g3 -Wall -Werror -std=gnu99 

OBJS=$(SRCS:.c=.o)
LOOP_OBJS=$(LOOP_SRC:.c=.o)
PRINT_STDIN_OBJS=$(PRINT_STDIN_SRC:.c=.o)

all: $(EXECUTABLES) $(LOOP_EXECUTABLES) $(PRINT_STDIN_EXECUTABLES) 
$(EXECUTABLES): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(LOOP_EXECUTABLES): $(LOOP_OBJS)
	$(CC) $(CFLAGS) $(LOOP_OBJS) -o $@

$(PRINT_STDIN_EXECUTABLES): $(PRINT_STDIN_SRC)
	$(CC) $(CFLAGS) $(PRINT_STDIN_SRC) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf \
	$(EXECUTABLES) $(OBJS) \
	$(LOOP_EXECUTABLES) $(LOOP_OBJS) \
	$(PRINT_STDIN_EXECUTABLES) $(PRINT_STDIN_OBJS) \