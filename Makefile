CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
BINDIR  = bin
SRCDIR  = src
TESTDIR = tests

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BINDIR)/%.o)

TESTS = \
	$(BINDIR)/test_procmem \
	$(BINDIR)/test_snapshot \
	$(BINDIR)/test_reporter \
	$(BINDIR)/test_tracker \
	$(BINDIR)/test_filter \
	$(BINDIR)/test_ringbuf \
	$(BINDIR)/test_stats \
	$(BINDIR)/test_history \
	$(BINDIR)/test_threshold \
	$(BINDIR)/test_trend \
	$(BINDIR)/test_decay \
	$(BINDIR)/test_baseline \
	$(BINDIR)/test_anomaly_report \
	$(BINDIR)/test_rate \
	$(BINDIR)/test_pressure \
	$(BINDIR)/test_quota \
	$(BINDIR)/test_budget \
	$(BINDIR)/test_throttle \
	$(BINDIR)/test_cooldown \
	$(BINDIR)/test_backoff \
	$(BINDIR)/test_jitter \
	$(BINDIR)/test_debounce \
	$(BINDIR)/test_suppress \
	$(BINDIR)/test_escalate \
	$(BINDIR)/test_clamp \
	$(BINDIR)/test_window \
	$(BINDIR)/test_pivot \
	$(BINDIR)/test_scoreboard \
	$(BINDIR)/test_ceiling \
	$(BINDIR)/test_fence \
	$(BINDIR)/test_guard \
	$(BINDIR)/test_sentinel \
	$(BINDIR)/test_probe \
	$(BINDIR)/test_watchdog \
	$(BINDIR)/test_heartbeat \
	$(BINDIR)/test_checkpoint

.PHONY: all clean test

all: $(BINDIR) $(TESTS)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Generic test rule: link test + all src objects
$(BINDIR)/test_%: $(TESTDIR)/test_%.c $(SRCS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@

test: all
	@echo "=== Running all tests ==="
	@fail=0; for t in $(TESTS); do \
		$$t || fail=1; \
	done; \
	[ $$fail -eq 0 ] && echo "All tests passed." || echo "Some tests FAILED."

clean:
	rm -rf $(BINDIR)
