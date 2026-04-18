CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
SRCS    = src/procmem.c src/snapshot.c src/sampler.c src/reporter.c \
          src/tracker.c src/formatter.c src/alert.c src/filter.c \
          src/filter_chain.c src/ringbuf.c src/stats.c src/history.c \
          src/history_query.c
OBJS    = $(SRCS:.c=.o)

TESTS   = tests/test_procmem tests/test_snapshot tests/test_reporter \
          tests/test_tracker tests/test_alert tests/test_filter \
          tests/test_ringbuf tests/test_stats tests/test_history

.PHONY: all clean test

all: $(OBJS)

tests/test_procmem: tests/test_procmem.c src/procmem.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_snapshot: tests/test_snapshot.c src/snapshot.c src/procmem.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_reporter: tests/test_reporter.c src/reporter.c src/snapshot.c src/procmem.c src/formatter.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_tracker: tests/test_tracker.c src/tracker.c src/snapshot.c src/procmem.c src/sampler.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_alert: tests/test_alert.c src/alert.c src/snapshot.c src/procmem.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_filter: tests/test_filter.c src/filter.c src/filter_chain.c src/snapshot.c src/procmem.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_ringbuf: tests/test_ringbuf.c src/ringbuf.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_stats: tests/test_stats.c src/stats.c src/ringbuf.c
	$(CC) $(CFLAGS) $^ -o $@

tests/test_history: tests/test_history.c src/history.c src/history_query.c src/snapshot.c src/procmem.c
	$(CC) $(CFLAGS) $^ -o $@

test: $(TESTS)
	@for t in $(TESTS); do echo "--- $$t ---"; ./$$t; done

clean:
	rm -f $(OBJS) $(TESTS)
