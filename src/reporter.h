#ifndef REPORTER_H
#define REPORTER_H

#include "snapshot.h"
#include <stdio.h>

typedef enum {
    REPORT_FORMAT_TEXT,
    REPORT_FORMAT_CSV
} ReportFormat;

typedef struct {
    ReportFormat format;
    FILE *output;
    int show_delta;
} ReporterConfig;

/* Initialize reporter with given config */
void reporter_init(ReporterConfig *cfg);

/* Print a single snapshot */
void reporter_print_snapshot(const ReporterConfig *cfg, const MemSnapshot *snap);

/* Print delta between two snapshots */
void reporter_print_delta(const ReporterConfig *cfg,
                          const MemSnapshot *prev,
                          const MemSnapshot *curr);

/* Print CSV header line */
void reporter_print_csv_header(const ReporterConfig *cfg);

#endif /* REPORTER_H */
