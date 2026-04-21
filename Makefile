CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
LDFLAGS = -lm

SRCS = src/procmem.c src/snapshot.c src/sampler.c src/reporter.c \
       src/tracker.c src/formatter.c src/alert.c src/filter.c \
       src/filter_chain.c src/ringbuf.c src/stats.c src/history.c \
       src/history_query.c src/threshold.c src/trend.c src/decay.c \
       src/baseline.c src/anomaly.c src/anomaly_report.c \
       src/anomaly_sink.c src/rate.c src/pressure.c src/quota.c \
       src/budget.c src/budget_policy.c src/throttle.c src/cooldown.c \
       src/backoff.c src/jitter.c src/debounce.c src/suppress.c \
       src/escalate.c src/clamp.c src/window.c src/pivot.c

TESTS = tests/test_procmem tests/test_snapshot tests/test_reporter \
        tests/test_tracker tests/test_alert tests/test_filter \
        tests/test_ringbuf tests/test_stats tests/test_history \
        tests/test_threshold tests/test_trend tests/test_decay \
        tests/test_baseline tests/test_anomaly_report tests/test_rate \
        tests/test_pressure tests/test_quota tests/test_budget \
        tests/test_throttle tests/test_cooldown tests/test_backoff \
        tests/test_jitter tests/test_debounce tests/test_suppress \
        tests/test_escalate tests/test_clamp tests/test_window \
        tests/test_pivot

.PHONY: all clean test

all: $(TESTS)

tests/test_procmem: tests/test_procmem.c src/procmem.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests/test_%: tests/test_%.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TESTS)
	@for t in $(TESTS); do \
	    echo "=== $$t ==="; \
	    ./$$t || exit 1; \
	done

clean:
	rm -f $(TESTS)
