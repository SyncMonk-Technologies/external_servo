#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <string.h>
#include <time.h>

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

struct field_info
{
    /*! field_name */
    char* field_name;
    /*! index. */
    int idx;
    /*! variable type. */
    uint8_t var_type;
    /*! minimum value. */
    uint32_t min;
    /*! maximum value. */
    uint32_t max;
    /*! default value. */
    uint32_t def;
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
#define UDS_ADDRESS 0
#define TOD_DEV 1
#define FREQ_DEV 2
#define SERVO_TYPE 3
#define SYNC_INTERVAL 4
#define DELAY_INTERVAL 5
#define STEP_THRESHOLD 6
#define FIRST_STEP_THRESHOLD 7
#define SERVO_OFFSET_THRESHOLD 8
#define SERVO_NUM_OFFSET_VALUES 9
/** @} */

/**
 * @defgroup pi servo config.
 *
 * @{
 */
#define PROPORTIONAL_CONSTANT 0
#define INTEGRAL_CONSTANT 1
#define PROPORTIONAL_SCALE 2
#define PROPORTIONAL_EXPONENT 3
#define PROPORTIONAL_NORM_MAX 4
#define INTEGRAL_SCALE 5
#define INTEGRAL_EXPONENT 6
#define INTEGRAL_NORM_MAX 7
/** @} */

/**
 * @defgroup ntpshm NTP SHM config
 *
 */
#define SEGMENT 0

#define MAX_MSG_TAG_LEN 16

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

struct servo_config
{
    enum servo_type type;
    uint8_t sw_ts;
    double servo_step_threshold;
    double servo_first_step_threshold;
    double max_frequency;
    double intial_adj;
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
};

struct device_config
{
    clockid_t tod_clk_id;
    clockid_t freq_clk_id;
    char* tod_device;
    char* freq_device;
    int fd;
    struct address daddr;
    char* uds_address;
    char* ptp_uds_address;
	uint16_t poll_time;
	uint8_t tsproc_mode;
	uint8_t delay_filter;
	int filter_len;
};

#endif /*! __CONFIG_H__*/
