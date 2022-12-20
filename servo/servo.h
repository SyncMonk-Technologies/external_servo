/**
 * @file servo.c
 * @note Copyright (C) 2011 Richard Cochran <richardcochran@gmail.com>
 * @note SPDX-License-Identifier: GPL-2.0+
 */
 
#ifndef __SERVO_H__
#define __SERVO_H__

#include <linux/ptp_clock.h>
#include "utils.h"
#include "config.h"

/**
 * @brief Servo states.
 *
 */
enum servo_state
{
    /* Servo is not synchronized to master. */
    SERVO_UNLOCKED,
    /**
     * Servo is tracking a valid master. Servo is getting time stamps
     */
    SERVO_JUMP,
    /**
     * The servo is tracking the master clock and is stable.
     */
    SERVO_LOCKED,
    /**
     * The available master is lost. In this state servo is trying to
     * control the performance based on previous set of data available.
     */
    SERVO_LOCKED_STABLE,
};

struct ptp_servo_data
{
    uint16_t msg_type;
    int64_t t1;
    int64_t t2;
    int64_t t3;
    int64_t t4;
};

/**
 * @brief Servo structure.
 *
 */
struct servo
{
    double max_frequency;
    double step_threshold;
    double first_step_threshold;
    int first_update;
    int64_t offset_threshold;
    int num_offset_values;
    int curr_offset_values;

    void (*destroy)(struct servo* servo);
    double (*sample)(struct servo* servo, int64_t offset, uint64_t local_ts, double weight, enum servo_state* state);
    void (*sync_interval)(struct servo* servo, double interval);
    void (*reset)(struct servo* servo);
    double (*rate_ratio)(struct servo* servo);
    void (*leap)(struct servo* servo, int leap);
};

extern double
servo_sample(struct servo* servo, int64_t offset, uint64_t local_ts, double weight, enum servo_state* state);

extern double
servo_rate_ratio(struct servo* servo);

extern struct servo*
servo_create(struct servo_config* cfg);

extern void
servo_destroy(struct servo* servo);

extern void
servo_sync_interval(struct servo* servo, double interval);

#endif /* __SERVO_H__ */
