#ifndef __LINREG_H__
#define __LINREG_H__

#include "servo.h"
#include "config.h"

struct servo*
linreg_servo_create(struct servo_config* cfg);

#endif /* __LINREG_H__ */
