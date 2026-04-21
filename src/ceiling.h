#ifndef CEILING_H
#define CEILING_H

#include <stdbool.h>

/* Severity levels for ceiling breaches */
typedef enum {
    CEILING_OK   = 0,
    CEILING_SOFT = 1,
    CEILING_HARD = 2
} CeilingSeverity;

/* Configuration for a ceiling check */
typedef struct {
    long hard_limit;   /* absolute upper bound — breach triggers HARD */
    long soft_limit;   /* warning threshold — breach triggers SOFT (0 = disabled) */
} CeilingConfig;

/* Result returned by ceiling_check() */
typedef struct {
    long            value;       /* the value that was checked */
    long            limit;       /* the hard limit in effect */
    bool            breached;    /* true if either soft or hard limit hit */
    unsigned int    consecutive; /* consecutive hard breaches */
    CeilingSeverity severity;    /* severity of this sample */
} CeilingResult;

/* Ceiling state */
typedef struct {
    CeilingConfig cfg;
    bool          active;            /* currently in hard-breach state */
    unsigned int  breach_count;      /* total hard breaches since init */
    unsigned int  consecutive;       /* current run of hard breaches */
    long          last_breach_value; /* last value that caused hard breach */
} Ceiling;

void           ceiling_init(Ceiling *c, const CeilingConfig *cfg);
void           ceiling_reset(Ceiling *c);
CeilingResult  ceiling_check(Ceiling *c, long value);
bool           ceiling_is_active(const Ceiling *c);
unsigned int   ceiling_breach_count(const Ceiling *c);
const char    *ceiling_severity_str(CeilingSeverity sev);

#endif /* CEILING_H */
