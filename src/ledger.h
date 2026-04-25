#ifndef MEMTRACE_LEDGER_H
#define MEMTRACE_LEDGER_H

#include <stdint.h>
#include <stddef.h>

/*
 * ledger — tracks cumulative memory allocation and deallocation events
 * per named category, providing a running balance for each.
 */

#define LEDGER_MAX_ENTRIES  32
#define LEDGER_NAME_MAX     48

typedef struct {
    char     name[LEDGER_NAME_MAX];
    uint64_t total_allocated;   /* bytes allocated (cumulative) */
    uint64_t total_freed;       /* bytes freed (cumulative) */
    uint64_t peak_balance;      /* highest observed balance */
    uint64_t event_count;       /* number of record() calls */
} LedgerEntry;

typedef struct {
    LedgerEntry entries[LEDGER_MAX_ENTRIES];
    int         count;
} Ledger;

/* Initialise a zeroed ledger. */
void ledger_init(Ledger *l);

/*
 * Record an allocation (+bytes) or deallocation (-bytes) for category.
 * Creates the entry if it does not yet exist.
 * Returns 0 on success, -1 if the ledger is full and the entry is new.
 */
int ledger_record(Ledger *l, const char *category, int64_t bytes);

/* Return the current balance (allocated - freed) for a category, or 0. */
int64_t ledger_balance(const Ledger *l, const char *category);

/* Return a pointer to an entry by name, or NULL if not found. */
const LedgerEntry *ledger_find(const Ledger *l, const char *category);

/* Reset all entries to zero without removing them. */
void ledger_reset(Ledger *l);

/* Write a human-readable summary to buf (null-terminated). */
int ledger_format(const Ledger *l, char *buf, size_t bufsz);

#endif /* MEMTRACE_LEDGER_H */
