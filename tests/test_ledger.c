#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/ledger.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond) do { \
    if (cond) { printf("  PASS: %s\n", #cond); passed++; } \
    else      { printf("  FAIL: %s  (line %d)\n", #cond, __LINE__); failed++; } \
} while(0)

static void test_init(void)
{
    puts("[test_init]");
    Ledger l;
    ledger_init(&l);
    CHECK(l.count == 0);
    CHECK(ledger_balance(&l, "heap") == 0);
    CHECK(ledger_find(&l, "heap") == NULL);
}

static void test_record_and_balance(void)
{
    puts("[test_record_and_balance]");
    Ledger l;
    ledger_init(&l);

    CHECK(ledger_record(&l, "heap", 1024) == 0);
    CHECK(ledger_balance(&l, "heap") == 1024);

    CHECK(ledger_record(&l, "heap", 512) == 0);
    CHECK(ledger_balance(&l, "heap") == 1536);

    CHECK(ledger_record(&l, "heap", -256) == 0);
    CHECK(ledger_balance(&l, "heap") == 1280);

    CHECK(ledger_balance(&l, "stack") == 0);
}

static void test_multiple_categories(void)
{
    puts("[test_multiple_categories]");
    Ledger l;
    ledger_init(&l);

    ledger_record(&l, "heap",  4096);
    ledger_record(&l, "mmap",  8192);
    ledger_record(&l, "heap", -1024);

    CHECK(l.count == 2);
    CHECK(ledger_balance(&l, "heap") == 3072);
    CHECK(ledger_balance(&l, "mmap") == 8192);
}

static void test_peak_and_events(void)
{
    puts("[test_peak_and_events]");
    Ledger l;
    ledger_init(&l);

    ledger_record(&l, "heap", 2000);
    ledger_record(&l, "heap", 1000);
    ledger_record(&l, "heap", -2500);

    const LedgerEntry *e = ledger_find(&l, "heap");
    CHECK(e != NULL);
    CHECK(e->peak_balance == 3000);
    CHECK(e->event_count  == 3);
    CHECK(ledger_balance(&l, "heap") == 500);
}

static void test_reset(void)
{
    puts("[test_reset]");
    Ledger l;
    ledger_init(&l);

    ledger_record(&l, "heap", 512);
    ledger_reset(&l);

    CHECK(l.count == 1);  /* entry retained */
    CHECK(ledger_balance(&l, "heap") == 0);
    const LedgerEntry *e = ledger_find(&l, "heap");
    CHECK(e->event_count == 0);
    CHECK(e->peak_balance == 0);
}

static void test_format(void)
{
    puts("[test_format]");
    Ledger l;
    ledger_init(&l);
    ledger_record(&l, "heap", 4096);
    ledger_record(&l, "heap", -1024);

    char buf[512];
    int n = ledger_format(&l, buf, sizeof(buf));
    CHECK(n > 0);
    CHECK(strstr(buf, "heap")   != NULL);
    CHECK(strstr(buf, "4096")   != NULL);
    CHECK(strstr(buf, "3072")   != NULL);
}

int main(void)
{
    test_init();
    test_record_and_balance();
    test_multiple_categories();
    test_peak_and_events();
    test_reset();
    test_format();

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}
