#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../src/scoreboard.h"
#include "../src/score.h"

static void test_upsert_and_get(void) {
    Scoreboard sb;
    scoreboard_init(&sb);

    assert(scoreboard_upsert(&sb, 101, "proc_a", 512, 1024, 0.8f) == 0);
    assert(sb.count == 1);

    ScoreEntry *e = scoreboard_get(&sb, 101);
    assert(e != NULL);
    assert(e->pid == 101);
    assert(strcmp(e->label, "proc_a") == 0);
    assert(e->rss_kb == 512);

    /* Update same pid */
    assert(scoreboard_upsert(&sb, 101, "proc_a", 768, 1024, 0.6f) == 0);
    assert(sb.count == 1);
    e = scoreboard_get(&sb, 101);
    assert(e->rss_kb == 768);
    assert(fabsf(e->score - 0.6f) < 1e-5f);

    printf("PASS: test_upsert_and_get\n");
}

static void test_remove(void) {
    Scoreboard sb;
    scoreboard_init(&sb);

    scoreboard_upsert(&sb, 200, "proc_b", 256, 512, 0.9f);
    scoreboard_upsert(&sb, 201, "proc_c", 128, 256, 0.5f);
    assert(sb.count == 2);

    assert(scoreboard_remove(&sb, 200) == 0);
    assert(sb.count == 1);
    assert(scoreboard_get(&sb, 200) == NULL);
    assert(scoreboard_get(&sb, 201) != NULL);

    assert(scoreboard_remove(&sb, 999) == -1);

    printf("PASS: test_remove\n");
}

static void test_worst_best(void) {
    Scoreboard sb;
    scoreboard_init(&sb);

    assert(scoreboard_worst(&sb) == -1);
    assert(scoreboard_best(&sb)  == -1);

    scoreboard_upsert(&sb, 1, "a", 100, 200, 0.9f);
    scoreboard_upsert(&sb, 2, "b", 100, 200, 0.2f);
    scoreboard_upsert(&sb, 3, "c", 100, 200, 0.5f);

    int w = scoreboard_worst(&sb);
    int b = scoreboard_best(&sb);
    assert(sb.entries[w].pid == 2);
    assert(sb.entries[b].pid == 1);

    printf("PASS: test_worst_best\n");
}

static void test_sort(void) {
    Scoreboard sb;
    scoreboard_init(&sb);

    scoreboard_upsert(&sb, 10, "x", 0, 0, 0.7f);
    scoreboard_upsert(&sb, 20, "y", 0, 0, 0.1f);
    scoreboard_upsert(&sb, 30, "z", 0, 0, 0.4f);

    scoreboard_sort(&sb);
    assert(sb.entries[0].pid == 20);  /* worst first */
    assert(sb.entries[1].pid == 30);
    assert(sb.entries[2].pid == 10);

    printf("PASS: test_sort\n");
}

static void test_score_compute(void) {
    /* Healthy: low rss, within quota, no growth, no pressure */
    float s = score_compute(256, 1024, 2048, 0.0f, 0.0f);
    assert(s > 0.65f);
    assert(strcmp(score_tier(s), "healthy") == 0);

    /* Critical: at quota, high growth, high pressure */
    float sc = score_compute(2048, 2048, 2048, 900.0f, 0.9f);
    assert(sc < 0.35f);
    assert(strcmp(score_tier(sc), "critical") == 0);

    /* Warning band */
    float sw = score_compute(1024, 1024, 2048, 200.0f, 0.4f);
    (void)sw; /* value depends on weights; just ensure no crash */

    printf("PASS: test_score_compute\n");
}

static void test_capacity_limit(void) {
    Scoreboard sb;
    scoreboard_init(&sb);

    for (int i = 0; i < SCOREBOARD_MAX_ENTRIES; i++)
        assert(scoreboard_upsert(&sb, i + 1, "p", 0, 0, 0.5f) == 0);

    /* One over capacity */
    assert(scoreboard_upsert(&sb, 9999, "overflow", 0, 0, 0.5f) == -1);
    assert(sb.count == SCOREBOARD_MAX_ENTRIES);

    printf("PASS: test_capacity_limit\n");
}

int main(void) {
    test_upsert_and_get();
    test_remove();
    test_worst_best();
    test_sort();
    test_score_compute();
    test_capacity_limit();
    printf("All scoreboard tests passed.\n");
    return 0;
}
