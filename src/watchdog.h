/*
 * watchdog.h - Watchdog monitor for memory usage violations
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <sys/types.h>
#include "snapshot.h"

typedef enum {
    WD_STATE_IDLE = 0,
    WD_STATE_OK,
    WD_STATE_VIOLATION,
    WD_STATE_CRITICAL,
    WD_STATE_RECOVERING,
    WD_STATE_ERROR
} WatchdogState;

typedef enum {
    WD_RESULT_OK = 0,
    WD_RESULT_VIOLATION,
    WD_RESULT_CRITICAL,
    WD_RESULT_SKIPPED,
    WD_RESULT_ERROR
} WatchdogResult;

typedef struct {
    pid_t        pid;
    MemSnapshot  snap;
    unsigned int violation_count;
    unsigned int consecutive;
} WatchdogEvent;

typedef void (*WatchdogCallback)(const WatchdogEvent *ev, void *user_data);

typedef struct {
    long             interval_ms;      /* minimum ms between checks     */
    long             rss_limit_kb;     /* RSS limit, 0 = disabled        */
    long             vms_limit_kb;     /* VMS limit, 0 = disabled        */
    unsigned int     max_consecutive;  /* consecutive violations -> crit */
    WatchdogCallback on_violation;     /* called on each violation       */
    WatchdogCallback on_critical;      /* called when critical reached   */
    void            *user_data;        /* passed to callbacks            */
} WatchdogConfig;

typedef struct {
    WatchdogConfig cfg;
    WatchdogState  state;
    MemSnapshot    last_snapshot;
    unsigned int   violation_count;
    unsigned int   consecutive_violations;
    long           last_check_ts;
} Watchdog;

void           watchdog_init(Watchdog *wd, const WatchdogConfig *cfg);
void           watchdog_reset(Watchdog *wd);
WatchdogResult watchdog_check(Watchdog *wd, pid_t pid);
WatchdogState  watchdog_state(const Watchdog *wd);
const char    *watchdog_state_str(WatchdogState state);

#endif /* WATCHDOG_H */
