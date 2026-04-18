#ifndef HISTORY_H
#define HISTORY_H

#include <stddef.h>
#include <time.h>
#include "snapshot.h"

#define HISTORY_MAX_ENTRIES 256

typedef struct {
    MemSnapshot snapshot;
    time_t      timestamp;
} HistoryEntry;

typedef struct {
    HistoryEntry entries[HISTORY_MAX_ENTRIES];
    size_t       count;
    size_t       head;   /* index of oldest entry (circular) */
} MemHistory;

/* Initialise an empty history buffer */
void history_init(MemHistory *h);

/* Append a snapshot taken at 'now' */
void history_push(MemHistory *h, const MemSnapshot *snap, time_t now);

/* Return number of entries currently stored */
size_t history_count(const MemHistory *h);

/*
 * Copy up to 'max' entries into 'out', ordered oldest-first.
 * Returns the number of entries written.
 */
size_t history_get(const MemHistory *h, HistoryEntry *out, size_t max);

/* Return the peak RSS (kB) seen across all stored entries, or 0 if empty */
unsigned long history_peak_rss(const MemHistory *h);

#endif /* HISTORY_H */
