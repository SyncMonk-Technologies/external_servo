#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "tmv.h"
#include "mmedian.h"
#include "filter.h"

struct mmedian {
	struct filter filter;
	int cnt;
	int len;
	int index;
	/* Indices sorted by value. */
	int *order;
	/* Values stored in circular buffer. */
	tmv_t *samples;
};

static void mmedian_destroy(struct filter *filter)
{
	struct mmedian *m = container_of(filter, struct mmedian, filter);
	free(m->order);
	free(m->samples);
	free(m);
}

static tmv_t mmedian_sample(struct filter *filter, tmv_t sample)
{
	struct mmedian *m = container_of(filter, struct mmedian, filter);
	int i;

	m->samples[m->index] = sample;
	if (m->cnt < m->len) {
		m->cnt++;
	} else {
		/* Remove index of the replaced value from order. */
		for (i = 0; i < m->cnt; i++)
			if (m->order[i] == m->index)
				break;
		for (; i + 1 < m->cnt; i++)
			m->order[i] = m->order[i + 1];
	}

	/* Insert index of the new value to order. */
	for (i = m->cnt - 1; i > 0; i--) {
		if (tmv_cmp(m->samples[m->order[i - 1]],
			    m->samples[m->index]) <= 0)
			break;
		m->order[i] = m->order[i - 1];
	}
	m->order[i] = m->index;

	m->index = (1 + m->index) % m->len;

	if (m->cnt % 2)
		return m->samples[m->order[m->cnt / 2]];
	else
		return tmv_div(tmv_add(m->samples[m->order[m->cnt / 2 - 1]],
				       m->samples[m->order[m->cnt / 2]]), 2);
}

static void mmedian_reset(struct filter *filter)
{
	struct mmedian *m = container_of(filter, struct mmedian, filter);
	m->cnt = 0;
	m->index = 0;
}

struct filter *mmedian_create(int length)
{
	struct mmedian *m;

	if (length < 1)
		return NULL;
	m = calloc(1, sizeof(*m));
	if (!m)
		return NULL;
	m->filter.destroy = mmedian_destroy;
	m->filter.sample = mmedian_sample;
	m->filter.reset = mmedian_reset;
	m->order = calloc(1, length * sizeof(*m->order));
	if (!m->order) {
		free(m);
		return NULL;
	}
	m->samples = calloc(1, length * sizeof(*m->samples));
	if (!m->samples) {
		free(m->order);
		free(m);
		return NULL;
	}
	m->len = length;
	return &m->filter;
}
