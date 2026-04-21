#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <stddef.h>
#include <time.h>

#define SCOREBOARD_MAX_ENTRIES 16

typedef struct {
    int   pid;
    char  label[32];
    long  rss_kb;
    long  peak_rss_kb;
    float score;        /* composite health score 0.0 (bad) .. 1.0 (good) */
    time_t last_update;
} ScoreEntry;

typedef struct {
    ScoreEntry entries[SCOREBOARD_MAX_ENTRIES];
    int        count;
} Scoreboard;

/* Lifecycle */
void scoreboard_init(Scoreboard *sb);

/* Insert or update an entry for pid */
int  scoreboard_upsert(Scoreboard *sb, int pid, const char *label,
                       long rss_kb, long peak_rss_kb, float score);

/* Remove entry by pid; returns 0 on success, -1 if not found */
int  scoreboard_remove(Scoreboard *sb, int pid);

/* Retrieve entry by pid; returns NULL if not found */
ScoreEntry *scoreboard_get(Scoreboard *sb, int pid);

/* Return index of worst-scoring entry (-1 if empty) */
int  scoreboard_worst(const Scoreboard *sb);

/* Return index of best-scoring entry (-1 if empty) */
int  scoreboard_best(const Scoreboard *sb);

/* Sort entries in-place by score ascending (worst first) */
void scoreboard_sort(Scoreboard *sb);

#endif /* SCOREBOARD_H */
