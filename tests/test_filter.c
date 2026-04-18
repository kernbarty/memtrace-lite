#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/filter.h"
#include "../src/filter_chain.h"
#include "../src/snapshot.h"

static MemSnapshot make_snap(const char *label, long rss, long vsz) {
    MemSnapshot s;
    memset(&s, 0, sizeof(s));
    strncpy(s.label, label, sizeof(s.label) - 1);
    s.rss_kb = rss;
    s.vsz_kb = vsz;
    return s;
}

static void test_filter_none(void) {
    MemFilter f;
    filter_init(&f);
    MemSnapshot s = make_snap("proc", 1000, 2000);
    assert(filter_match(&f, &s) == 1);
    printf("PASS test_filter_none\n");
}

static void test_filter_rss(void) {
    MemFilter f;
    filter_init(&f);
    filter_set_rss(&f, 500, 1500);
    MemSnapshot lo = make_snap("a", 100,  500);
    MemSnapshot ok = make_snap("b", 1000, 2000);
    MemSnapshot hi = make_snap("c", 2000, 4000);
    assert(filter_match(&f, &lo) == 0);
    assert(filter_match(&f, &ok) == 1);
    assert(filter_match(&f, &hi) == 0);
    printf("PASS test_filter_rss\n");
}

static void test_filter_name(void) {
    MemFilter f;
    filter_init(&f);
    filter_set_name(&f, "nginx");
    MemSnapshot yes = make_snap("nginx-worker", 200, 400);
    MemSnapshot no  = make_snap("sshd",         200, 400);
    assert(filter_match(&f, &yes) == 1);
    assert(filter_match(&f, &no)  == 0);
    printf("PASS test_filter_name\n");
}

static void test_filter_chain(void) {
    FilterChain chain;
    filter_chain_init(&chain);

    MemFilter f1, f2;
    filter_init(&f1); filter_set_rss(&f1, 100, 2000);
    filter_init(&f2); filter_set_name(&f2, "app");
    filter_chain_add(&chain, &f1);
    filter_chain_add(&chain, &f2);

    MemSnapshot snaps[3] = {
        make_snap("app-server", 500,  1000),
        make_snap("app-server", 5000, 8000),
        make_snap("sshd",       500,  1000)
    };
    int out[3];
    int n = filter_chain_apply(&chain, snaps, 3, out, 3);
    assert(n == 1);
    assert(out[0] == 0);
    printf("PASS test_filter_chain\n");
}

int main(void) {
    test_filter_none();
    test_filter_rss();
    test_filter_name();
    test_filter_chain();
    printf("All filter tests passed.\n");
    return 0;
}
