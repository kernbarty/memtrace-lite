CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc -lm
SRCDIR  = src
TESTDIR = tests
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

TESTS = test_procmem test_snapshot test_reporter test_tracker \
        test_alert test_filter test_ringbuf test_stats \
        test_history test_threshold test_trend test_decay \
        test_baseline

.PHONY: all clean test

all: $(BUILDDIR) $(OBJS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

define make_test
$(BUILDDIR)/$(1): $(TESTDIR)/$(1).c $(SRCS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(TESTDIR)/$(1).c $(SRCS) -o $$@ -lm
endef

$(foreach t,$(TESTS),$(eval $(call make_test,$(t))))

test: $(addprefix $(BUILDDIR)/,$(TESTS))
	@echo "=== Running all tests ==="
	@for t in $^; do echo "--- $$t ---"; ./$$t || exit 1; done
	@echo "=== All tests passed ==="

clean:
	rm -rf $(BUILDDIR)
