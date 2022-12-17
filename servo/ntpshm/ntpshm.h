#ifndef __NTPSHM_H__
#define __NTPSHM_H__

#include "servo.h"
#include "config.h"

struct servo*
ntpshm_servo_create(struct servo_config* cfg);

#endif /* __NTPSHM_H__ */
