#ifndef __FILTER_H__
#define __FILTER_H__

#include "utils.h"
#include "tmv.h"

/** Opaque type */
struct filter;

/**
 * Defines the available filters.
 */
enum filter_type
{
    FILTER_MOVING_AVERAGE,
    FILTER_MOVING_MEDIAN,
};

/**
 * Create a new instance of a filter.
 * @param type    The type of the filter to create.
 * @param length  The filter's length.
 * @return A pointer to a new filter on success, NULL otherwise.
 */
struct filter*
filter_create(enum filter_type type, int length);

/**
 * Destroy an instance of a filter.
 * @param filter Pointer to a filter obtained via @ref filter_create().
 */
void
filter_destroy(struct filter* filter);

/**
 * Feed a sample into a filter.
 * @param filter    Pointer to a filter obtained via @ref filter_create().
 * @param sample    The input sample.
 * @return The output value.
 */
tmv_t
filter_sample(struct filter* filter, tmv_t sample);

/**
 * Reset a filter.
 * @param filter   Pointer to a filter obtained via @ref filter_create().
 */
void
filter_reset(struct filter* filter);

struct filter
{
    void (*destroy)(struct filter* filter);

    tmv_t (*sample)(struct filter* filter, tmv_t sample);

    void (*reset)(struct filter* filter);
};

#endif /* __FILTER_H__ */
