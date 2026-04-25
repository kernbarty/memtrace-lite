#include "ledger.h"

#include <string.h>
#include <stdio.h>

void ledger_init(Ledger *l)
{
    memset(l, 0, sizeof(*l));
}

static LedgerEntry *find_or_create(Ledger *l, const char *category)
{
    for (int i = 0; i < l->count; i++) {
        if (strncmp(l->entries[i].name, category, LEDGER_NAME_MAX - 1) == 0)
            return &l->entries[i];
    }
    if (l->count >= LEDGER_MAX_ENTRIES)
        return NULL;

    LedgerEntry *e = &l->entries[l->count++];
    memset(e, 0, sizeof(*e));
    strncpy(e->name, category, LEDGER_NAME_MAX - 1);
    e->name[LEDGER_NAME_MAX - 1] = '\0';
    return e;
}

int ledger_record(Ledger *l, const char *category, int64_t bytes)
{
    if (!l || !category) return -1;

    LedgerEntry *e = find_or_create(l, category);
    if (!e) return -1;

    if (bytes > 0) {
        e->total_allocated += (uint64_t)bytes;
    } else if (bytes < 0) {
        uint64_t freed = (uint64_t)(-bytes);
        e->total_freed += freed;
    }
    e->event_count++;

    uint64_t balance = (e->total_allocated > e->total_freed)
                       ? e->total_allocated - e->total_freed : 0;
    if (balance > e->peak_balance)
        e->peak_balance = balance;

    return 0;
}

int64_t ledger_balance(const Ledger *l, const char *category)
{
    if (!l || !category) return 0;
    for (int i = 0; i < l->count; i++) {
        if (strncmp(l->entries[i].name, category, LEDGER_NAME_MAX - 1) == 0) {
            const LedgerEntry *e = &l->entries[i];
            return (int64_t)(e->total_allocated - e->total_freed);
        }
    }
    return 0;
}

const LedgerEntry *ledger_find(const Ledger *l, const char *category)
{
    if (!l || !category) return NULL;
    for (int i = 0; i < l->count; i++) {
        if (strncmp(l->entries[i].name, category, LEDGER_NAME_MAX - 1) == 0)
            return &l->entries[i];
    }
    return NULL;
}

void ledger_reset(Ledger *l)
{
    for (int i = 0; i < l->count; i++) {
        LedgerEntry *e = &l->entries[i];
        e->total_allocated = 0;
        e->total_freed     = 0;
        e->peak_balance    = 0;
        e->event_count     = 0;
    }
}

int ledger_format(const Ledger *l, char *buf, size_t bufsz)
{
    if (!l || !buf || bufsz == 0) return -1;
    int off = 0;
    off += snprintf(buf + off, bufsz - (size_t)off,
                    "ledger: %d entr%s\n", l->count,
                    l->count == 1 ? "y" : "ies");
    for (int i = 0; i < l->count && off < (int)bufsz - 1; i++) {
        const LedgerEntry *e = &l->entries[i];
        int64_t bal = (int64_t)(e->total_allocated - e->total_freed);
        off += snprintf(buf + off, bufsz - (size_t)off,
                        "  [%s] alloc=%llu freed=%llu balance=%lld peak=%llu events=%llu\n",
                        e->name,
                        (unsigned long long)e->total_allocated,
                        (unsigned long long)e->total_freed,
                        (long long)bal,
                        (unsigned long long)e->peak_balance,
                        (unsigned long long)e->event_count);
    }
    return off;
}
