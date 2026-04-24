#include "latch_group.h"
#include "latch.h"
#include <string.h>
#include <stdio.h>

void latch_group_init(LatchGroup *grp) {
    if (!grp) return;
    memset(grp, 0, sizeof(*grp));
    grp->count = 0;
    grp->any_triggered = false;
    grp->all_triggered = false;
}

bool latch_group_add(LatchGroup *grp, Latch *latch, const char *name) {
    if (!grp || !latch || grp->count >= LATCH_GROUP_MAX) return false;
    grp->entries[grp->count].latch = latch;
    strncpy(grp->entries[grp->count].name, name ? name : "",
            LATCH_NAME_MAX - 1);
    grp->entries[grp->count].name[LATCH_NAME_MAX - 1] = '\0';
    grp->count++;
    return true;
}

void latch_group_evaluate(LatchGroup *grp) {
    if (!grp || grp->count == 0) {
        if (grp) {
            grp->any_triggered = false;
            grp->all_triggered = false;
        }
        return;
    }

    int triggered = 0;
    for (int i = 0; i < grp->count; i++) {
        if (latch_is_set(grp->entries[i].latch)) {
            triggered++;
        }
    }

    grp->any_triggered = (triggered > 0);
    grp->all_triggered = (triggered == grp->count);
    grp->triggered_count = triggered;
}

bool latch_group_any(const LatchGroup *grp) {
    return grp ? grp->any_triggered : false;
}

bool latch_group_all(const LatchGroup *grp) {
    return grp ? grp->all_triggered : false;
}

int latch_group_triggered_count(const LatchGroup *grp) {
    return grp ? grp->triggered_count : 0;
}

void latch_group_reset_all(LatchGroup *grp) {
    if (!grp) return;
    for (int i = 0; i < grp->count; i++) {
        latch_reset(grp->entries[i].latch);
    }
    grp->any_triggered = false;
    grp->all_triggered = false;
    grp->triggered_count = 0;
}

void latch_group_report(const LatchGroup *grp, FILE *out) {
    if (!grp || !out) return;
    fprintf(out, "LatchGroup: %d/%d triggered\n",
            grp->triggered_count, grp->count);
    for (int i = 0; i < grp->count; i++) {
        fprintf(out, "  [%s]: %s\n",
                grp->entries[i].name,
                latch_is_set(grp->entries[i].latch) ? "SET" : "clear");
    }
}
