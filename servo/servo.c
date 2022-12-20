/**
 * @file servo.c
 * @note Copyright (C) 2011 Richard Cochran <richardcochran@gmail.com>
 * @note SPDX-License-Identifier: GPL-2.0+
 */
 
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "linreg.h"
#include "ntpshm.h"
#include "pi.h"
#include "servo.h"

#include "logger.h"

#define NSEC_PER_SEC 1000000000

struct servo*
servo_create(struct servo_config* cfg)
{
    double servo_first_step_threshold;
    double servo_step_threshold;
    int servo_max_frequency;
    struct servo* servo;

    switch (cfg->type) {
    case PI_SERVO:
        servo = pi_servo_create(cfg);
        break;
    case LINEAR_REG:
        servo = linreg_servo_create(cfg);
        break;
    case NTP_SHM:
        servo = ntpshm_servo_create(cfg);
        break;
    default:
        return NULL;
    }

    if (!servo)
        return NULL;

    servo_step_threshold = cfg->step_threshold;
    if (servo_step_threshold > 0.0) {
        servo->step_threshold = servo_step_threshold * NSEC_PER_SEC;
    } else {
        servo->step_threshold = 0.0;
    }

    servo_first_step_threshold = cfg->first_step_threshold;

    if (servo_first_step_threshold > 0.0) {
        servo->first_step_threshold = servo_first_step_threshold * NSEC_PER_SEC;
    } else {
        servo->first_step_threshold = 0.0;
    }

    servo_max_frequency = cfg->max_frequency;
    servo->max_frequency = cfg->max_frequency;
    if (servo_max_frequency && servo->max_frequency > servo_max_frequency) {
        servo->max_frequency = servo_max_frequency;
    }

    servo->first_update = 1;
    servo->offset_threshold = cfg->offset_threshold;
    servo->num_offset_values = cfg->num_offset_values;
    servo->curr_offset_values = cfg->num_offset_values;

    pr_debug("step_threshold: %f", servo->step_threshold);
    pr_debug("first_step_threshold: %f", servo->first_step_threshold);
    pr_debug("offset_threshold: %ld", servo->offset_threshold);
    pr_debug("num_offset_values: %d", servo->num_offset_values);

    return servo;
}

void
servo_destroy(struct servo* servo)
{
    servo->destroy(servo);
}

static int
check_offset_threshold(struct servo* s, int64_t offset)
{
    long long int abs_offset = llabs(offset);

    if (s->offset_threshold) {
        if (abs_offset < s->offset_threshold && s->curr_offset_values)
            s->curr_offset_values--;
        return s->curr_offset_values ? 0 : 1;
    }
    return 0;
}

double
servo_sample(struct servo* servo, int64_t offset, uint64_t local_ts, double weight, enum servo_state* state)
{
    double r;

    pr_debug("offset: %ld local_ts: %lu, weight: %f", offset, local_ts, weight);
    r = servo->sample(servo, offset, local_ts, weight, state);

    switch (*state) {
    case SERVO_UNLOCKED:
        servo->curr_offset_values = servo->num_offset_values;
        break;
    case SERVO_JUMP:
        servo->curr_offset_values = servo->num_offset_values;
        servo->first_update = 0;
        break;
    case SERVO_LOCKED:
        if (check_offset_threshold(servo, offset)) {
            *state = SERVO_LOCKED_STABLE;
        }
        servo->first_update = 0;
        break;
    case SERVO_LOCKED_STABLE:
        /*
         * This case will never occur since the only place
         * SERVO_LOCKED_STABLE is set is in this switch/case block
         * (case SERVO_LOCKED).
         */
        break;
    }

    return r;
}

void
servo_sync_interval(struct servo* servo, double interval)
{
    servo->sync_interval(servo, interval);
}

void
servo_reset(struct servo* servo)
{
    servo->reset(servo);
}

double
servo_rate_ratio(struct servo* servo)
{
    if (servo->rate_ratio)
        return servo->rate_ratio(servo);

    return 1.0;
}

void
servo_leap(struct servo* servo, int leap)
{
    if (servo->leap)
        servo->leap(servo, leap);
}

int
servo_offset_threshold(struct servo* servo)
{
    return servo->offset_threshold;
}
