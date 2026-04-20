CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
SRCDIR  = src
TESTDIR = tests
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

TESTS = $(wildcard $(TESTDIR)/test_*.c)
TEST_BINS = $(patsubst $(TESTDIR)/%.c,$(BUILDDIR)/%,$(TESTS))

.PHONY: all clean test

all: $(BUILDDIR) $(OBJS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/test_%: $(TESTDIR)/test_%.c $(SRCS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TEST_BINS)
	@total=0; pass=0; fail=0; \
	for t in $(TEST_BINS); do \
		total=$$((total+1)); \
		if $$t; then pass=$$((pass+1)); \
		else fail=$$((fail+1)); fi; \
	done; \
	echo "Tests: $$pass/$$total passed"

clean:
	rm -rf $(BUILDDIR)
