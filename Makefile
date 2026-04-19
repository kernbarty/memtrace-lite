CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
SRCDIR  = src
TESTDIR = tests
BUILDDIR= build

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

TESTS = test_procmem test_snapshot test_reporter test_tracker \
        test_filter test_ringbuf test_stats test_history \
        test_threshold test_trend test_decay test_baseline \
        test_anomaly_report test_rate test_pressure test_quota \
        test_budget test_alert test_throttle

.PHONY: all clean test

all: $(BUILDDIR) $(OBJS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

define BUILD_TEST
$(BUILDDIR)/$(1): $(TESTDIR)/$(1).c $(OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $$< $(OBJS) -o $$@
endef

$(foreach t,$(TESTS),$(eval $(call BUILD_TEST,$(t))))

test: $(addprefix $(BUILDDIR)/,$(TESTS))
	@failed=0; \
	for t in $^; do \
	    echo "--- $$t ---"; \
	    $$t || failed=$$((failed+1)); \
	done; \
	echo "Done. Failed suites: $$failed"; \
	exit $$failed

clean:
	rm -rf $(BUILDDIR)
