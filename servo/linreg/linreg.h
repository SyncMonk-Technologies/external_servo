/**
 * @file linreg.h
 * @brief Implements an adaptive servo based on linear regression.
 * @note Copyright (C) 2014 Miroslav Lichvar <mlichvar@redhat.com>
 * @note SPDX-License-Identifier: GPL-2.0+
 */ 
#ifndef __LINREG_H__
#define __LINREG_H__

#include "servo.h"
#include "config.h"

struct servo*
linreg_servo_create(struct servo_config* cfg);

#endif /* __LINREG_H__ */
