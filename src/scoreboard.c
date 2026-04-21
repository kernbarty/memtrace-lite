#include "scoreboard.h"
#include <string.h>
#include <stdlib.h>

void scoreboard_init(Scoreboard *sb) {
    memset(sb, 0, sizeof(*sb));
}

int scoreboard_upsert(Scoreboard *sb, int pid, const char *label,
                      long rss_kb, long peak_rss_kb, float score) {
    /* Update existing */
    for (int i = 0; i < sb->count; i++) {
        if (sb->entries[i].pid == pid) {
            ScoreEntry *e = &sb->entries[i];
            strncpy(e->label, label ? label : "", sizeof(e->label) - 1);
            e->rss_kb      = rss_kb;
            e->peak_rss_kb = peak_rss_kb;
            e->score       = score;
            e->last_update = time(NULL);
            return 0;
        }
    }
    /* Insert new */
    if (sb->count >= SCOREBOARD_MAX_ENTRIES)
        return -1;
    ScoreEntry *e = &sb->entries[sb->count++];
    e->pid        = pid;
    strncpy(e->label, label ? label : "", sizeof(e->label) - 1);
    e->rss_kb      = rss_kb;
    e->peak_rss_kb = peak_rss_kb;
    e->score       = score;
    e->last_update = time(NULL);
    return 0;
}

int scoreboard_remove(Scoreboard *sb, int pid) {
    for (int i = 0; i < sb->count; i++) {
        if (sb->entries[i].pid == pid) {
            sb->entries[i] = sb->entries[--sb->count];
            memset(&sb->entries[sb->count], 0, sizeof(ScoreEntry));
            return 0;
        }
    }
    return -1;
}

ScoreEntry *scoreboard_get(Scoreboard *sb, int pid) {
    for (int i = 0; i < sb->count; i++)
        if (sb->entries[i].pid == pid)
            return &sb->entries[i];
    return NULL;
}

int scoreboard_worst(const Scoreboard *sb) {
    if (sb->count == 0) return -1;
    int idx = 0;
    for (int i = 1; i < sb->count; i++)
        if (sb->entries[i].score < sb->entries[idx].score)
            idx = i;
    return idx;
}

int scoreboard_best(const Scoreboard *sb) {
    if (sb->count == 0) return -1;
    int idx = 0;
    for (int i = 1; i < sb->count; i++)
        if (sb->entries[i].score > sb->entries[idx].score)
            idx = i;
    return idx;
}

static int cmp_score(const void *a, const void *b) {
    const ScoreEntry *ea = (const ScoreEntry *)a;
    const ScoreEntry *eb = (const ScoreEntry *)b;
    if (ea->score < eb->score) return -1;
    if (ea->score > eb->score) return  1;
    return 0;
}

void scoreboard_sort(Scoreboard *sb) {
    qsort(sb->entries, (size_t)sb->count, sizeof(ScoreEntry), cmp_score);
}
