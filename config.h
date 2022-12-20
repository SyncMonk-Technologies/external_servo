/**
 * @file config.h
 * @note Copyright (C) 2022 SyncMonk Technologies <services@syncmonk.net>
 * @note SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <float.h>

#include "uds.h"

/**
 * @brief Getting the number of elements in an array.
 *
 */
#define COUNTOF(_arr_) (sizeof(_arr_) / sizeof(_arr_[0]))

/** Maximum key length. */
#define MAX_KEY_LENGTH 32
/** Maximum port name length. */
#define MAX_DEVICE_NAME_LEN 32

/**
 * @struct key_val
 */
struct key_val
{
    /*! key */
    char* key;
    /*! value */
    int val;
};

#define VAR_TYPE_INTEGER 1
#define VAR_TYPE_STRING 2
#define VAR_TYPE_ENUM 3
#define VAR_TYPE_DOUBLE 4

struct field_info
{
    /*! field_name */
    char* field_name;
    /*! index. */
    int idx;
    /*! variable type. */
    uint8_t var_type;
    /*! minimum value. */
    double min;
    /*! maximum value. */
    double max;
    /*! default value. */
    double def;
    /*! default str */
    char* default_str;
    /*! enum list. */
    struct key_val* enum_list;
    /*! enum_size. */
    uint8_t enum_sz;
};

/*!
 * Parse the configuration file.
 *
 * \param [in] filename File to be parsed.
 *
 * retval function return value.
 */
extern int
parse_config_file(char* filename);

/**
 * @defgroup LOGGER_CONFIG logger config.
 *
 * @{
 */
#define LOGGING_LEVEL 0
#define USE_SYSLOG 1
#define MESSAGE_TAG 2
#define USE_STDOUT 3
/** @} */

/**
 * @defgroup SERVO config.
 *
 * @{
 */
#define SERVO_TYPE 0
#define SW_TS 1
#define MAX_FREQUENCY 2
#define STEP_THRESHOLD 3
#define FIRST_STEP_THRESHOLD 4
#define OFFSET_THRESHOLD 5
#define NUM_OFFSET_VALUES 6
#define CURRENT_OFFSET_VALUES 7
#define PI_PROPORTIONAL 8
#define PI_INTEGRAL 9
#define PI_PROPORTIONAL_SCALE 10
#define PI_PROPORTIONAL_EXPONENT 11
#define PI_PROPORTIONAL_NORM_MAX 12
#define PI_INTEGRAL_SCALE 13
#define PI_INTEGRAL_EXPONENT 14
#define PI_INTEGRAL_NORMAL_MAX 15
#define NTPSHM_SEGMENT 16
#define LOGMIN_DELAY_REQ_INTERVAL 17
#define LOG_SYNC_INTERVAL 18
/** @} */

/**
 * @defgroup Device config.
 *
 * @{
 */
#define TOD_DEVICE 0
#define FREQ_DEVICE 1
#define MONITOR_UDS_ADDRESS 2
#define POLL_TIME 3
#define TSPROC_MODE 4
#define DELAY_FILTER 5
#define FILTER_LEN 6
/** @} */

#define MAX_MSG_TAG_LEN 16
#define MAX_CONFIG_STR_LEN 32
/**
 * @brief Available Servo types.
 *
 */
enum servo_type
{
    PI_SERVO,
    LINEAR_REG,
    NTP_SHM,
};

enum tsproc_type
{
    FILTER,
    RAW,
    FILTER_WEIGHT,
    RAW_WEIGHT,
};

enum delay_filter
{
    AVERAGE,
    MEDIAN
};

struct servo_config
{
    enum servo_type type;
    uint8_t sw_ts;
    double max_frequency;
    int intial_adj;
    double step_threshold;
    double first_step_threshold;
    int first_update;
    int64_t offset_threshold;
    int num_offset_values;
    int curr_offset_values;
    double kp;
    double ki;
    double kp_scale;
    double kp_exponent;
    double kp_norm_max;
    double ki_scale;
    double ki_exponent;
    double ki_norm_max;
    int ntpshm_segment;
    int logMinDelayReqInterval;
    int logSyncInterval;
};

struct device_config
{
    int fd;
    clockid_t tod_clk_id;
    clockid_t freq_clk_id;
    char tod_device[MAX_CONFIG_STR_LEN];
    char freq_device[MAX_CONFIG_STR_LEN];
    char uds_address[MAX_CONFIG_STR_LEN];
    struct address daddr;
    uint16_t poll_time;
    enum delay_filter filter;
    int filter_len;
    enum tsproc_type mode;
};

extern int
servo_config_parse(char* filename);

extern void
device_configure(struct device_config* config);

extern void
servo_configure(struct servo_config* config);
#endif /*! __CONFIG_H__*/
