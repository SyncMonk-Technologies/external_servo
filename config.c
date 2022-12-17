/**
 * @file config.c
 * Configuration parser.
 */

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>
#include <limits.h>

#include "config.h"
#include "logger.h"
/******************************************************************************
 * Local Definitions
 *****************************************************************************/
#define MAX_BLOCK_LEVEL 2

static struct key_val servo_types[] = { { "pi", PI_SERVO }, { "linreg", LINEAR_REG }, { "ntpshm", NTP_SHM } };

static struct key_val tsproc_types[] = { { "filter", FILTER },
                                         { "raw", RAW },
                                         { "filter_weight", FILTER_WEIGHT },
                                         { "raw_weight", RAW_WEIGHT } };

static struct key_val delay_filters[] = {
    { "moving_average", AVERAGE },
    { "moving_median", MEDIAN },
};

static struct field_info logger_tbl[] = {
    /* logging level. */
    {
      .field_name = "logging_level",
      .idx = LOGGING_LEVEL,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = 7,
      .def = 6,
    },
    /* use_syslog */
    {
      .field_name = "use_syslog",
      .idx = USE_SYSLOG,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = 1,
      .def = 0,
    },
    /* msg_tag */
    {
      .field_name = "msg_tag",
      .idx = MESSAGE_TAG,
      .var_type = VAR_TYPE_STRING,
      .default_str = "servo",
    },
    /*! use_stdout */
    {
      .field_name = "use_stdout",
      .idx = USE_STDOUT,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = 1,
      .def = 1,
    },
};

static struct field_info servo_config_tbl[] = {
    /* servo type */
    {
      .field_name = "type",
      .idx = SERVO_TYPE,
      .var_type = VAR_TYPE_ENUM,
      .enum_list = servo_types,
      .enum_sz = COUNTOF(servo_types),
    },
    /* software timestamp */
    {
      .field_name = "software_timestamp",
      .idx = SW_TS,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = 1,
      .def = 1,
    },
    /* Max frequency */
    {
      .field_name = "max_frequency",
      .idx = MAX_FREQUENCY,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = INT_MAX,
      .def = 900000000,
    },
    /* Step threshold */
    {
      .field_name = "step_threshold",
      .idx = STEP_THRESHOLD,
      .var_type = VAR_TYPE_DOUBLE,
      .min = 0.0,
      .max = DBL_MAX,
      .def = 0.0,
    },
    /* first_step_threshold */
    {
      .field_name = "first_step_threshold",
      .idx = FIRST_STEP_THRESHOLD,
      .var_type = VAR_TYPE_DOUBLE,
      .min = 0.0,
      .max = DBL_MAX,
      .def = 0.00002,
    },
    /* offset_threshold */
    {
      .field_name = "servo_offset_threshold",
      .idx = OFFSET_THRESHOLD,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = INT_MAX,
      .def = 0,
    },
    /* num_offset_values */
    {
      .field_name = "servo_num_offset_values",
      .idx = NUM_OFFSET_VALUES,
      .var_type = VAR_TYPE_INTEGER,
      .min = 0,
      .max = INT_MAX,
      .def = 0,
    },
    /* pi proportional constant */
    {
      .field_name = "pi_proportional_const",
      .idx = PI_PROPORTIONAL,
      .var_type = VAR_TYPE_DOUBLE,
      .min = 0.0,
      .max = DBL_MAX,
      .def = 0.0,
    },
    /* pi integral constant */
    {
      .field_name = "pi_integral_const",
      .idx = PI_INTEGRAL,
      .var_type = VAR_TYPE_DOUBLE,
      .min = 0.0,
      .max = DBL_MAX,
      .def = 0.0,
    },
    /* pi proportional scale */
    {
      .field_name = "pi_proportional_scale",
      .idx = PI_PROPORTIONAL_SCALE,
      .var_type = VAR_TYPE_DOUBLE,
      .min = 0.0,
      .max = DBL_MAX,
      .def = 0.0,
    },
    /* pi proportional exponent */
    {
      .field_name = "pi_proportional_exponent",
      .idx = PI_PROPORTIONAL_EXPONENT,
      .var_type = VAR_TYPE_DOUBLE,
      .min = DBL_MIN,
      .max = DBL_MAX,
      .def = -0.3,
    },
    /* pi proportional normal max. */
    {
      .field_name = "pi_proportional_norm_max",
      .idx = PI_PROPORTIONAL_NORM_MAX,
      .var_type = VAR_TYPE_DOUBLE,
      .min = DBL_MIN,
      .max = DBL_MAX,
      .def = 0.7,
    },
    /* pi_integral_scale */
    {
      .field_name = "pi_integral_scale",
      .idx = PI_INTEGRAL_SCALE,
      .var_type = VAR_TYPE_DOUBLE,
      .min = 0.0,
      .max = DBL_MAX,
      .def = 0.0,
    },
    /* pi_integral_exponent */
    {
      .field_name = "pi_integral_exponent",
      .idx = PI_INTEGRAL_EXPONENT,
      .var_type = VAR_TYPE_DOUBLE,
      .min = DBL_MIN,
      .max = DBL_MAX,
      .def = 0.4,
    },
    /* pi intergral normal max */
    {
      .field_name = "pi_integral_norm_max",
      .idx = PI_INTEGRAL_NORMAL_MAX,
      .var_type = VAR_TYPE_DOUBLE,
      .min = DBL_MIN,
      .max = 2.0,
      .def = 0.3,
    },
    /* ntp shm segment */
    {
      .field_name = "ntp_shm_segment",
      .idx = NTPSHM_SEGMENT,
      .var_type = VAR_TYPE_INTEGER,
      .min = INT_MIN,
      .max = INT_MAX,
      .def = 0,
    },
    /* logMinDelayReqInterval */
    {
      .field_name = "logMinDelayReqInterval",
      .idx = LOGMIN_DELAY_REQ_INTERVAL,
      .var_type = VAR_TYPE_INTEGER,
      .min = INT8_MIN,
      .max = INT8_MAX,
      .def = 0,
    },
    /* logSyncInterval */
    {
      .field_name = "logSyncInterval",
      .idx = LOG_SYNC_INTERVAL,
      .var_type = VAR_TYPE_INTEGER,
      .min = INT8_MIN,
      .max = INT8_MAX,
      .def = 0,
    }
};

static struct field_info device_tbl[] = {
    /* TOD device. */
    {
      .field_name = "tod_device",
      .idx = TOD_DEVICE,
      .var_type = VAR_TYPE_STRING,
    },
    /* Frequency device. */
    {
      .field_name = "freq_device",
      .idx = FREQ_DEVICE,
      .var_type = VAR_TYPE_STRING,
    },
    /* Monitor UDS address. */
    {
      .field_name = "monitor_uds_address",
      .idx = MONITOR_UDS_ADDRESS,
      .var_type = VAR_TYPE_STRING,
    },
    /* POLL timer */
    { .field_name = "poll_time", .idx = POLL_TIME, .var_type = VAR_TYPE_INTEGER, .def = 1, .min = 0, .max = UINT8_MAX },
    /* tsproc */
    {
      .field_name = "tsproc_mode",
      .idx = TSPROC_MODE,
      .var_type = VAR_TYPE_ENUM,
      .enum_list = tsproc_types,
      .enum_sz = COUNTOF(tsproc_types),
    },
    /* delay_filter */
    {
      .field_name = "delay_filter",
      .idx = DELAY_FILTER,
      .var_type = VAR_TYPE_ENUM,
      .enum_list = delay_filters,
      .enum_sz = COUNTOF(delay_filters),
    },
    /* delay_filter_length */
    {
      .field_name = "delay_filter_length",
      .idx = FILTER_LEN,
      .var_type = VAR_TYPE_INTEGER,
      .min = 1,
      .max = INT_MAX,
      .def = 10,
    },
};

/* external servo parse state. */
enum servo_parser_state
{
    START_PARSE,
    START_BLOCK,
    STOP_PARSE,
    START_SERVO_BLOCK,
    START_DEVICE_BLOCK,
    START_LOGGER_BLOCK,
};

/**
 * @brief parser data.
 *
 * This contains intermediate stage data for the yaml parser.
 * This save data for all the configuration block based on
 * parser state.
 */
struct servo_parser_data
{
    enum servo_parser_state state;
    uint8_t block_id;
    char* block_name[MAX_BLOCK_LEVEL];
    char* key;
    char* val;
    union config
    {
        struct logger_config logger_config;
        struct device_config device_config;
        struct servo_config servo_config;
    } config;
};

/******************************************************************************
 * Private Functions
 *****************************************************************************/
/**
 * @brief Get the value associated with string key.
 *
 * @param [in] key Key to search.
 * @param [in] tbl Table where to search
 * @param [in] sz Size of the table.
 *
 * @retval returns the value if key match is found.
 * @retval -1 If the key match is not found.
 */
static int
get_key_val(char* key, struct key_val* tbl, int sz)
{
    int idx = 0;
    for (idx = 0; idx < sz; idx++) {
        if (!strcmp((const char*)key, tbl[idx].key)) {
            return tbl[idx].val;
        }
    }
    return -1;
}

/**
 * @brief Get the field info for the key in the configuration
 * block.
 *
 * @param [in] key  Key field.
 * @param [in] tbl  Table for key search.
 * @param [in] sz   Size of the table.
 *
 * @return struct field_info*  field data based on match.
 * @return NULL if the field not found in the table.
 */
static struct field_info*
get_field_info(char* key, struct field_info* tbl, int sz)
{
    int idx = 0;
    for (idx = 0; idx < sz; idx++) {
        if (!strcmp((const char*)key, tbl[idx].field_name)) {
            return &tbl[idx];
        }
    }
    return NULL;
}

/**
 * @brief Validates the values for the field.
 *
 * @param [in] field Field data for the key.
 * @param [in] val  Value configured.
 * @return -1 Incorrect value configured.
 * @return  0 Value OK.
 */
static int
value_range_check(struct field_info* field, char* val, double* value)
{
    int rv = 0;
    char* ptr;
    *value = 0;

    switch (field->var_type) {
    case VAR_TYPE_INTEGER:
        *value = atoi(val);
        if (*value<field->min&& * value> field->max) {
            rv = -1;
        }
        break;
    case VAR_TYPE_DOUBLE:
        *value = strtod(val, &ptr);
        if (*value<field->min&& * value> field->max) {
            rv = -1;
        }
        break;
    case VAR_TYPE_STRING:
        /* No action needed. Nothing to be returned. */
        rv = 0;
        break;
    case VAR_TYPE_ENUM:
        if (field->enum_list) {
            *value = get_key_val(val, field->enum_list, field->enum_sz);
            if (*value == (uint32_t)-1) {
                rv = -1;
            }
        }
        break;
    default:
        pr_err("Undefined variable type: %s", field->field_name);

        break;
    }
    return rv;
}

/**
 * @brief Update the logger configuration to parser state.
 *
 * This saves the configured value in parser data before configuring
 * the values to logging database.
 *
 * @param [in] config servo logger config.
 * @param [in] key  Key for logger config.
 * @param [in] key_value Key Value for logger config.
 *
 * @return -1 Key not find the Logging Config block.
 *            Key value not in range.
 * @return 0 Success.
 */
static int
update_logger_config(struct logger_config* config, char* key, char* key_value)
{
    struct field_info* field_info;
    int rv;
    double value;

    pr_info("%s: %s", key, key_value);
    field_info = get_field_info(key, logger_tbl, sizeof(logger_tbl) / sizeof(struct field_info));
    if (field_info == NULL) {
        pr_err("Error in getting field info Logger Block");
        return -1;
    }

    rv = value_range_check(field_info, key_value, &value);
    if (rv == -1) {
        pr_err("Value range check failed for key %s: data: %s", key, key_value);
        return -1;
    }

    switch (field_info->idx) {
    case LOGGING_LEVEL:
        config->log_level = value;
        break;
    case USE_SYSLOG:
        config->use_syslog = value;
        break;
    case MESSAGE_TAG:
        strncpy(config->msg_tag, key_value, MAX_MSG_TAG_LEN - 1);
        break;
    case USE_STDOUT:
        config->use_stdout = value;
        break;
    default:
        pr_err("Logging: Undefined field: %s", key);
        break;
    }
    return 0;
}

/**
 * @brief Update servo configuration.
 *
 * @param [in] config servo config.
 * @param [in] key  Key for servo config.
 * @param [in] key_value Key Value for servo config.
 *
 * @return -1 Key not find the servo Config block.
 *            Key value not in range.
 * @return 0 Success.
 */
static int
update_servo_config(struct servo_config* config, char* key, char* key_val)
{
    struct field_info* field_info;
    int rv;
    double value;

    pr_info("%s: %s", key, key_val);
    field_info = get_field_info(key, servo_config_tbl, sizeof(servo_config_tbl) / sizeof(struct field_info));
    if (field_info == NULL) {
        pr_err("Error in getting field info in Servo Block");
        return -1;
    }
    rv = value_range_check(field_info, key_val, &value);
    if (rv == -1) {
        pr_err("Value range check failed for key %s: data: %s", key, key_val);
        return -1;
    }
    /* add function to check the valid range. */
    switch (field_info->idx) {
    case SERVO_TYPE:
        config->type = value;
        break;
    case SW_TS:
        config->sw_ts = value;
        break;
    case MAX_FREQUENCY:
        config->max_frequency = value;
        break;
    case STEP_THRESHOLD:
        config->step_threshold = value;
        break;
    case FIRST_STEP_THRESHOLD:
        config->first_step_threshold = value;
        break;
    case OFFSET_THRESHOLD:
        config->offset_threshold = value;
        break;
    case NUM_OFFSET_VALUES:
        config->num_offset_values = value;
        break;
    case CURRENT_OFFSET_VALUES:
        config->curr_offset_values = value;
        break;
    case PI_PROPORTIONAL:
        config->kp = value;
        break;
    case PI_INTEGRAL:
        config->ki = value;
        break;
    case PI_PROPORTIONAL_SCALE:
        config->kp_scale = value;
        break;
    case PI_PROPORTIONAL_EXPONENT:
        config->kp_exponent = value;
        break;
    case PI_PROPORTIONAL_NORM_MAX:
        config->kp_norm_max = value;
        break;
    case PI_INTEGRAL_SCALE:
        config->ki_scale = value;
        break;
    case PI_INTEGRAL_EXPONENT:
        config->ki_exponent = value;
        break;
    case PI_INTEGRAL_NORMAL_MAX:
        config->ki_norm_max = value;
        break;
    case NTPSHM_SEGMENT:
        config->ntpshm_segment = value;
        break;
    case LOGMIN_DELAY_REQ_INTERVAL:
        break;
    case LOG_SYNC_INTERVAL:
        break;
    default:
        pr_err("Servo: Undefined field: %s", key);
        /* code should not reach here. */
        break;
    }
    return 0;
}

/**
 * @brief Update device configuration.
 *
 * @param [in] config servo device config.
 * @param [in] key  Key for device config.
 * @param [in] key_value Key Value for device config.
 *
 * @return -1 Key not find the device Config block.
 *            Key value not in range.
 * @return 0 Success.
 */
static int
update_device_config(struct device_config* config, char* key, char* key_val)
{
    struct field_info* field_info;
    int rv;
    double value;

    pr_info("%s: %s", key, key_val);
    field_info = get_field_info(key, device_tbl, sizeof(device_tbl) / sizeof(struct field_info));
    if (field_info == NULL) {
        pr_err("Error in getting field info in Device Block ");
        return -1;
    }

    /* Evaluate enum list if it is NULL for dynamic case. */
    rv = value_range_check(field_info, key_val, &value);
    if (rv == -1) {
        pr_err("Value range check failed for key %s: data: %s", key, key_val);
        return -1;
    }

    switch (field_info->idx) {
    case TOD_DEVICE:
        strncpy(config->tod_device, key_val, MAX_CONFIG_STR_LEN - 1);
        break;
    case FREQ_DEVICE:
        strncpy(config->freq_device, key_val, MAX_CONFIG_STR_LEN - 1);
        break;
    case MONITOR_UDS_ADDRESS:
        strncpy(config->uds_address, key_val, MAX_CONFIG_STR_LEN - 1);
        break;
    case POLL_TIME:
        config->poll_time = value;
        break;
    case TSPROC_MODE:
        config->mode = value;
        break;
    case DELAY_FILTER:
        config->filter = value;
        break;
    case FILTER_LEN:
        config->filter_len = value;
        break;
    default:
        pr_err("Device config: Undefined field: %s", key);
        break;
    }
    return 0;
}

/**
 * @brief update configuration to device database.
 *
 * @param [in] data Parser data.
 *
 */
static void
servo_update_config(struct servo_parser_data* data)
{
    switch (data->state) {
    case START_LOGGER_BLOCK:
        logger_configure(&data->config.logger_config);
        pr_debug("logger configuration done");
        break;
    case START_SERVO_BLOCK:
        servo_configure(&data->config.servo_config);
        pr_debug("servo configuration done.");
        break;
    case START_DEVICE_BLOCK:
        device_configure(&data->config.device_config);
        pr_debug("device configuration done for device.");
        break;
    default:
        pr_err("Undefined parser state: %d", data->state);
        break;
    }
}

/**
 * @brief Servo prepare configuration. On parsing configuration
 * file the device database is prepared.
 *
 * @param [in] data Parser data.
 *
 * @return 0 Success.
 * @return -1 Failure.
 */
static int
servo_prepare_config(struct servo_parser_data* data)
{
    int rv = 0;

    switch (data->state) {
    case START_LOGGER_BLOCK:
        rv = update_logger_config(&data->config.logger_config, data->key, data->val);
        break;
    case START_DEVICE_BLOCK:
        rv = update_device_config(&data->config.device_config, data->key, data->val);
        break;
    case START_SERVO_BLOCK:
        rv = update_servo_config(&data->config.servo_config, data->key, data->val);
        break;
    default:
        break;
    }
    return rv;
}

/**
 * @brief Update blocks based on yaml parser token types.
 *
 * @param [in] data Parser data.
 * @param [in] token Yaml token.
 *
 * @return 0 Success.
 * @return -1 Failure.
 */
static int
update_block_config(struct servo_parser_data* data, yaml_token_t* token)
{
    static uint8_t key, value;
    char* t_value;

    switch (token->type) {
    case YAML_KEY_TOKEN:
        key = 1;
        break;
    case YAML_VALUE_TOKEN:
        if (key) {
            key = 0;
            value = 1;
        }
        break;
    case YAML_BLOCK_MAPPING_START_TOKEN:
        key = value = 0;
        break;
    case YAML_SCALAR_TOKEN:
        t_value = (char*)(token->data.scalar.value);
        if (key) {
            if (data->key) {
                free(data->key);
            }
            data->key = strdup(t_value);
        } else if (value) {
            if (data->val) {
                free(data->val);
            }
            data->val = strdup(t_value);
            return servo_prepare_config(data);
        }
        break;
    case YAML_BLOCK_END_TOKEN:
        if (data->block_id) {
            servo_update_config(data);
            free(data->block_name[--data->block_id]);
            data->block_name[data->block_id] = NULL;
        }
        if (data->block_id == 0) {
            data->state = START_BLOCK;
        }
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief YAML block parsing.
 *
 * @param [in] data Servo parser data.
 * @param [in] token YAML token.
 *
 * @return -1 Failure.
 * @return 0 Success.
 */
static int
parser_block_management(struct servo_parser_data* data, yaml_token_t* token)
{
    char* value;
    static uint8_t key = 0;
    switch (token->type) {
    case YAML_KEY_TOKEN:
        key = 1;
        break;
    case YAML_BLOCK_MAPPING_START_TOKEN:
        value = data->key;
        if (!strcmp(value, "logging")) {
            memset(&data->config.logger_config, 0, sizeof(struct logger_config));
            data->state = START_LOGGER_BLOCK;
            pr_info("[Logger configuration]");
        } else if (!strcmp(value, "servo")) {
            memset(&data->config.servo_config, 0, sizeof(struct servo_config));
            data->state = START_SERVO_BLOCK;
            pr_info("[Servo configuration]");
        } else if (!strcmp(value, "device")) {
            memset(&data->config.device_config, 0, sizeof(struct device_config));
            data->state = START_DEVICE_BLOCK;
            pr_info("[Device configuration]");
        } else {
            return 0;
        }
        data->block_name[data->block_id++] = strdup(data->key);
        break;
    case YAML_SCALAR_TOKEN:
        if (key) {
            value = (char*)token->data.scalar.value;
            if (data->key) {
                free(data->key);
            }
            data->key = strdup(value);
            key = 0;
        }
        break;
    case YAML_STREAM_END_TOKEN:
        data->state = STOP_PARSE;
        if (data->key) {
            free(data->key);
        }
        if (data->val) {
            free(data->val);
        }
    default:
        break;
    }
    return 0;
}

/**
 * @brief config parser.
 *
 * @param [in] data Parser data.
 * @param [in] token Yaml token.
 *
 * @return 0 Success.
 * @return -1 Failure.
 */
static int
servo_config_parser(struct servo_parser_data* data, yaml_token_t* token)
{
    switch (data->state) {
    case START_PARSE:
        switch (token->type) {
        case YAML_BLOCK_MAPPING_START_TOKEN:
            data->state = START_BLOCK;
            break;
        default:
            break;
        }
        break;
    case START_BLOCK:
        return parser_block_management(data, token);
    case START_DEVICE_BLOCK:
    case START_SERVO_BLOCK:
    case START_LOGGER_BLOCK:
        return update_block_config(data, token);
    default:
        break;
    }
    return 0;
}
/******************************************************************************
 * Public Functions
 *****************************************************************************/
int
servo_config_parse(char* filename)
{
    int rv;
    FILE* fp;
    yaml_parser_t parser;
    yaml_token_t token;
    struct servo_parser_data parser_data;

    /* Open and check the file. */
    fp = fopen(filename, "r");
    if (fp == NULL) {
        pr_err("Error in opening file: %s", filename);
        return -1;
    }
    memset(&parser_data, 0, sizeof(struct servo_parser_data));
    parser_data.state = START_PARSE;

    rv = yaml_parser_initialize(&parser);
    if (rv == 0) {
        rv = -1;
        goto err;
    }
    yaml_parser_set_input_file(&parser, fp);
    do {
        rv = yaml_parser_scan(&parser, &token);
        if (rv == 0) {
            rv = -1;
            pr_err("Error in parsing yaml file: %s", filename);
            goto err;
        }

        rv = servo_config_parser(&parser_data, &token);
        if (rv == -1) {
            goto err;
        }
        yaml_token_delete(&token);
    } while (parser_data.state != STOP_PARSE);
    /*! Close the file pointer */
    yaml_parser_delete(&parser);
err:
    fclose(fp);
    return rv;
}
