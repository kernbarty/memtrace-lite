CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
SRCDIR  = src
TESTDIR = tests
BUILDDIR = build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

TESTS = test_procmem test_snapshot test_reporter test_tracker \
        test_alert test_filter test_ringbuf test_stats \
        test_history test_threshold test_trend test_decay \
        test_baseline test_anomaly_report test_rate test_pressure \
        test_quota

.PHONY: all clean test

all: $(BUILDDIR) $(OBJS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(BUILDDIR) $(addprefix $(BUILDDIR)/,$(TESTS))
	@for t in $(addprefix $(BUILDDIR)/,$(TESTS)); do \
		echo "Running $$t"; ./$$t || exit 1; \
	done

$(BUILDDIR)/test_%: $(TESTDIR)/test_%.c $(OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $^ -o $@ -lm

clean:
	rm -rf $(BUILDDIR)
