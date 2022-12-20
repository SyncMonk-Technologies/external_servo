/**
 * @file pi.c
 * @brief Implements a Proportional Integral clock servo.
 * @note Copyright (C) 2011 Richard Cochran <richardcochran@gmail.com>
 * @note SPDX-License-Identifier: GPL-2.0+
 */ 
#ifndef _PI_H
#define _PI_H

#include <config.h>
#include <servo.h>

struct servo*
pi_servo_create(struct servo_config* cfg);

#endif /** __PI_H__ **/
