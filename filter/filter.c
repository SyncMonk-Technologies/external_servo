#include "filter.h"
#include "mave.h"
#include "mmedian.h"
#include "tmv.h"

struct filter*
filter_create(enum filter_type type, int length)
{
    switch (type) {
    case FILTER_MOVING_AVERAGE:
        return mave_create(length);
    case FILTER_MOVING_MEDIAN:
        return mmedian_create(length);
    default:
        return NULL;
    }
}

void
filter_destroy(struct filter* filter)
{
    filter->destroy(filter);
}

tmv_t
filter_sample(struct filter* filter, tmv_t sample)
{
    return filter->sample(filter, sample);
}

void
filter_reset(struct filter* filter)
{
    filter->reset(filter);
}
