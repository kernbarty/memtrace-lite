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
        test_baseline test_anomaly_report test_rate test_pressure

.PHONY: all clean test

all: $(BUILDDIR) $(OBJS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

define make_test
$(BUILDDIR)/$(1): $(TESTDIR)/$(1).c $(OBJS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $$< $(OBJS) -o $$@
endef

$(foreach t,$(TESTS),$(eval $(call make_test,$(t))))

test: $(addprefix $(BUILDDIR)/,$(TESTS))
	@for t in $^; do echo "--- $$t ---"; ./$$t; done

clean:
	rm -rf $(BUILDDIR)
