/**
 * @file ntpshm.c
 * @brief Implements a servo providing the NTP SHM reference clock to
 *        send the samples to another process.
 * @note Copyright (C) 2014 Miroslav Lichvar <mlichvar@redhat.com>
 * @note SPDX-License-Identifier: GPL-2.0+
 */ 
#ifndef __NTPSHM_H__
#define __NTPSHM_H__

#include "servo.h"
#include "config.h"

struct servo*
ntpshm_servo_create(struct servo_config* cfg);

#endif /* __NTPSHM_H__ */
