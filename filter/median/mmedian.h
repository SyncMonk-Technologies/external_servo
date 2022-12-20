/**
 * @file mmedian.h
 * @note Copyright (C) 2013 Miroslav Lichvar <mlichvar@redhat.com>
 *
 * @note SPDX-License-Identifier: GPL-2.0+
 */
#ifndef __MMEDIAN_H__
#define __MMEDIAN_H__

#include "filter.h"

struct filter*
mmedian_create(int length);

#endif /* __MMEDIAN_H__ */
