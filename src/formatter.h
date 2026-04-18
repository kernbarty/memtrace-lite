#ifndef FORMATTER_H
#define FORMATTER_H

#include "snapshot.h"
#include "tracker.h"
#include <stdio.h>

/* Output formats supported by the formatter */
typedef enum {
    FORMAT_PLAIN = 0,
    FORMAT_CSV,
    FORMAT_JSON
} OutputFormat;

/*
 * Format a single memory snapshot to the given file stream.
 * Returns 0 on success, -1 on error.
 */
int formatter_write_snapshot(FILE *out, const MemSnapshot *snap, OutputFormat fmt);

/*
 * Format a full tracker report (all snapshots) to the given file stream.
 * Returns 0 on success, -1 on error.
 */
int formatter_write_report(FILE *out, const MemTracker *tracker, OutputFormat fmt);

/*
 * Convert bytes to a human-readable string (e.g. "4.2 MB").
 * buf must be at least 32 bytes.
 */
void formatter_human_bytes(long kb, char *buf, size_t buf_len);

#endif /* FORMATTER_H */
