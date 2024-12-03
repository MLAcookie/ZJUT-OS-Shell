SRCS=shell.c tokenizer.c shell_cmd.c shell_program.c shell_signal.c
EXECUTABLES=shell

TEST_SRC=loop.c
TEST_EXECUTABLES=loop

CC=gcc
CFLAGS=-g3 -Wall -Werror -std=gnu99 

OBJS=$(SRCS:.c=.o)
TEST_OBJS=$(TEST_SRC:.c=.o)

all: $(EXECUTABLES) $(TEST_EXECUTABLES)

$(EXECUTABLES): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(TEST_EXECUTABLES): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $@

$(OTHER_EXECUTABLES): $(OTHER_SRC)
	$(CC) $(CFLAGS) $(OTHER_SRC) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLES) $(OBJS) $(TEST_EXECUTABLES) $(TEST_OBJS)