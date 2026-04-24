#include "checkpoint.h"
#include <string.h>
#include <stdio.h>

void checkpoint_init(checkpoint_store_t *store) {
    if (!store) return;
    memset(store, 0, sizeof(*store));
}

int checkpoint_record(checkpoint_store_t *store,
                      const char *label,
                      uint64_t rss_kb,
                      uint64_t vms_kb) {
    if (!store || !label) return -1;

    /* Update existing entry if label already present */
    for (int i = 0; i < store->count; i++) {
        if (store->entries[i].valid &&
            strncmp(store->entries[i].label, label, CHECKPOINT_MAX_LABEL) == 0) {
            store->entries[i].rss_kb    = rss_kb;
            store->entries[i].vms_kb    = vms_kb;
            store->entries[i].timestamp = time(NULL);
            return 0;
        }
    }

    if (store->count >= CHECKPOINT_MAX_ENTRIES) return -1;

    checkpoint_entry_t *e = &store->entries[store->count++];
    snprintf(e->label, CHECKPOINT_MAX_LABEL, "%s", label);
    e->rss_kb    = rss_kb;
    e->vms_kb    = vms_kb;
    e->timestamp = time(NULL);
    e->valid     = 1;
    return 0;
}

const checkpoint_entry_t *checkpoint_find(const checkpoint_store_t *store,
                                          const char *label) {
    if (!store || !label) return NULL;
    for (int i = 0; i < store->count; i++) {
        if (store->entries[i].valid &&
            strncmp(store->entries[i].label, label, CHECKPOINT_MAX_LABEL) == 0) {
            return &store->entries[i];
        }
    }
    return NULL;
}

int checkpoint_delta_rss(const checkpoint_store_t *store,
                         const char *start_label,
                         const char *end_label,
                         int64_t *delta_kb) {
    if (!store || !start_label || !end_label || !delta_kb) return -1;
    const checkpoint_entry_t *s = checkpoint_find(store, start_label);
    const checkpoint_entry_t *e = checkpoint_find(store, end_label);
    if (!s || !e) return -1;
    *delta_kb = (int64_t)e->rss_kb - (int64_t)s->rss_kb;
    return 0;
}

int checkpoint_remove(checkpoint_store_t *store, const char *label) {
    if (!store || !label) return -1;
    for (int i = 0; i < store->count; i++) {
        if (store->entries[i].valid &&
            strncmp(store->entries[i].label, label, CHECKPOINT_MAX_LABEL) == 0) {
            /* Shift remaining entries down */
            for (int j = i; j < store->count - 1; j++) {
                store->entries[j] = store->entries[j + 1];
            }
            store->count--;
            return 0;
        }
    }
    return -1;
}

void checkpoint_reset(checkpoint_store_t *store) {
    if (!store) return;
    memset(store, 0, sizeof(*store));
}
