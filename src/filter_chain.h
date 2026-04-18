#ifndef FILTER_CHAIN_H
#define FILTER_CHAIN_H

#include "filter.h"
#include "snapshot.h"

#define FILTER_CHAIN_MAX 8

/* A chain of filters applied with AND logic */
typedef struct {
    MemFilter filters[FILTER_CHAIN_MAX];
    int       count;
} FilterChain;

/* Initialise an empty filter chain */
void filter_chain_init(FilterChain *chain);

/* Add a filter to the chain. Returns 0 on success, -1 if chain is full */
int  filter_chain_add(FilterChain *chain, const MemFilter *f);

/* Remove all filters from the chain */
void filter_chain_clear(FilterChain *chain);

/* Returns 1 if snap passes ALL filters in chain */
int  filter_chain_match(const FilterChain *chain, const MemSnapshot *snap);

/* Apply chain to array; writes passing indices into out[], returns count.
 * Returns -1 if out or snaps is NULL, or n < 0. */
int  filter_chain_apply(const FilterChain *chain,
                        const MemSnapshot *snaps, int n,
                        int *out, int out_max);

#endif /* FILTER_CHAIN_H */
