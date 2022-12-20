/**
 * @file mave.c
 * @note Copyright (C) 2011 Richard Cochran <richardcochran@gmail.com>
 * @note SPDX-License-Identifier: GPL-2.0+
 */
 
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "tmv.h"
#include "mave.h"
#include "filter.h"

struct mave
{
    struct filter filter;
    int cnt;
    int len;
    int index;
    tmv_t sum;
    tmv_t* val;
};

static void
mave_destroy(struct filter* filter)
{
    struct mave* m = container_of(filter, struct mave, filter);
    free(m->val);
    free(m);
}

static tmv_t
mave_accumulate(struct filter* filter, tmv_t val)
{
    struct mave* m = container_of(filter, struct mave, filter);

    m->sum = tmv_sub(m->sum, m->val[m->index]);
    m->val[m->index] = val;
    m->index = (1 + m->index) % m->len;
    m->sum = tmv_add(m->sum, val);
    if (m->cnt < m->len) {
        m->cnt++;
    }
    return tmv_div(m->sum, m->cnt);
}

static void
mave_reset(struct filter* filter)
{
    struct mave* m = container_of(filter, struct mave, filter);

    m->cnt = 0;
    m->index = 0;
    m->sum = tmv_zero();
    memset(m->val, 0, m->len * sizeof(*m->val));
}

struct filter*
mave_create(int length)
{
    struct mave* m;
    m = calloc(1, sizeof(*m));
    if (!m) {
        return NULL;
    }
    m->filter.destroy = mave_destroy;
    m->filter.sample = mave_accumulate;
    m->filter.reset = mave_reset;
    m->val = calloc(1, length * sizeof(*m->val));
    if (!m->val) {
        free(m);
        return NULL;
    }
    m->len = length;
    return &m->filter;
}
