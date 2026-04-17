CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=c11 -D_GNU_SOURCE
SRCS    := src/procmem.c
OBJS    := $(SRCS:.c=.o)

TEST_BIN := tests/test_procmem

.PHONY: all test clean

all: $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): tests/test_procmem.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) $(TEST_BIN)
