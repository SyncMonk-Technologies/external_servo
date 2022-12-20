/**
 * @file utils.h
 * @note Copyright (C) 2022 SyncMonk Technologies <services@syncmonk.net>
 * @note SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>
#include <stddef.h>

/*
 * This macro borrowed from the Linux kernel.
 */
#define container_of(ptr, type, member)                                                                                \
    ({                                                                                                                 \
        const typeof(((type*)0)->member)* __mptr = (ptr);                                                              \
        (type*)((char*)__mptr - offsetof(type, member));                                                               \
    })

#define CLOCKFD 3
#define FD_TO_CLOCKID(fd) ((~(clockid_t)(fd) << 3) | CLOCKFD)
#define CLOCKID_TO_FD(clk) ((unsigned int)~((clk) >> 3))

#ifndef CLOCK_INVALID
#define CLOCK_INVALID -1
#endif

struct Timestamp
{
    /*! The secondsField member is the integer portion of the timestamp in units of seconds.*/
    uint16_t seconds_msb; /* msb */
    uint32_t seconds_lsb; /* lsb */
    /*! The nanosecondsField member is the fractional portion of the timestamp in units of nanoseconds. */
    uint32_t nanoseconds;
} __attribute__((__packed__));

/** Internal binary time stamp format. */
struct timestamp
{
    uint64_t sec;
    uint32_t nsec;
};

#endif /* __UTILS_H__ */
