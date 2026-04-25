CC      = gcc
CFLAGS  = -Wall -Wextra -g -Isrc
BINDIR  = bin
SRCDIR  = src
TESTDIR = tests

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BINDIR)/%.o)

TEST_SRCS = $(wildcard $(TESTDIR)/test_*.c)
TEST_BINS = $(TEST_SRCS:$(TESTDIR)/test_%.c=$(BINDIR)/test_%)

.PHONY: all tests clean

all: $(BINDIR) $(OBJS)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- individual test link rules ---

DEP_procmem      = procmem
DEP_snapshot     = snapshot procmem
DEP_reporter     = reporter snapshot procmem formatter
DEP_tracker      = tracker snapshot procmem
DEP_alert        = alert
DEP_filter       = filter filter_chain
DEP_ringbuf      = ringbuf
DEP_stats        = stats
DEP_history      = history ringbuf stats history_query
DEP_threshold    = threshold
DEP_trend        = trend stats
DEP_decay        = decay
DEP_baseline     = baseline stats decay
DEP_anomaly_report = anomaly anomaly_report anomaly_sink baseline stats decay
DEP_rate         = rate
DEP_pressure     = pressure rate stats
DEP_quota        = quota
DEP_budget       = budget budget_policy quota
DEP_throttle     = throttle cooldown
DEP_cooldown     = cooldown
DEP_backoff      = backoff jitter
DEP_jitter       = jitter
DEP_debounce     = debounce
DEP_suppress     = suppress debounce cooldown
DEP_escalate     = escalate
DEP_clamp        = clamp
DEP_window       = window ringbuf stats
DEP_pivot        = pivot window stats
DEP_scoreboard   = scoreboard score
DEP_ceiling      = ceiling clamp
DEP_fence        = fence fence_policy
DEP_guard        = guard fence fence_policy
DEP_sentinel     = sentinel guard fence fence_policy
DEP_probe        = probe sentinel guard fence fence_policy
DEP_watchdog     = watchdog probe sentinel guard fence fence_policy heartbeat
DEP_heartbeat    = heartbeat
DEP_checkpoint   = checkpoint
DEP_latch        = latch latch_group
DEP_latch_group  = latch_group latch
DEP_cascade      = cascade cascade_policy
DEP_tally        = tally

define TEST_RULE
$(BINDIR)/test_$(1): $(TESTDIR)/test_$(1).c $(foreach d,$(DEP_$(1)),$(BINDIR)/$(d).o) | $(BINDIR)
	$$(CC) $$(CFLAGS) $$^ -o $$@
endef

TEST_NAMES = procmem snapshot reporter tracker alert filter ringbuf stats \
             history threshold trend decay baseline anomaly_report rate \
             pressure quota budget throttle cooldown backoff jitter debounce \
             suppress escalate clamp window pivot scoreboard ceiling fence \
             guard sentinel probe watchdog heartbeat checkpoint latch \
             latch_group cascade tally

$(foreach name,$(TEST_NAMES),$(eval $(call TEST_RULE,$(name))))

tests: $(addprefix $(BINDIR)/test_,$(TEST_NAMES))
	@echo "=== Running all tests ==="
	@for t in $^; do echo "--- $$t"; ./$$t || exit 1; done
	@echo "=== All tests passed ==="

clean:
	rm -rf $(BINDIR)
