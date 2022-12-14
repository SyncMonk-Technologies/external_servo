#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdint.h>
#include <syslog.h>

/*! Maximum message tag length. */
#define MAX_MSG_TAG_LEN 16

#define LOG_LEVEL_MIN LOG_EMERG
#define LOG_LEVEL_MAX LOG_DEBUG

#define pr_emerg(...) logger(LOG_EMERG, "EMERG", __VA_ARGS__)
#define pr_alert(...) logger(LOG_ALERT, "ALERT", __VA_ARGS__)
#define pr_crit(...) logger(LOG_CRIT, "CRITICAL", __VA_ARGS__)
#define pr_err(...) logger(LOG_ERR, "ERROR", __VA_ARGS__)
#define pr_warning(...) logger(LOG_WARNING, "WARN", __VA_ARGS__)
#define pr_notice(...) logger(LOG_NOTICE, "NOTICE", __VA_ARGS__)
#define pr_info(...) logger(LOG_INFO, "INFO", __VA_ARGS__)
#define pr_debug(...) logger(LOG_DEBUG, "DEBUG", __VA_ARGS__)

/**
 * @brief servo logging mechanism configuration.
 *
 */
struct logger_config
{
    /*!
     * Log level.
     * Minimum log level required to appear in a log.
     * Defaults to 6, valid values are 0-7.
     */
    uint8_t log_level;
    /*!
     * Set to 1 if syslog should be used.
     * Defaults to 0, Valid values are 0 or 1.*/
    uint8_t use_syslog;
    /*! Tag reported in logs. */
    char msg_tag[MAX_MSG_TAG_LEN];
    /*!
     * Set to 1 to log information on stdout.
     * Defaults to 1, Valid values are 0 or 1.
     */
    uint8_t use_stdout;
} __attribute__((__packed__));

/**
 * @brief logging configuration set.
 *
 * @param [in] config config servo logging configuration.
 * @return 0 on success.
 */
extern int
logger_configure(struct logger_config* config);

/**
 * @brief  Get the device logging configuration.
 *
 * @return Logger configuration for the device.
 */
extern struct logger_config*
logger_config_get();

/**
 * @brief Logging function.
 *
 * @param [in] level  Log level.
 * @param [in] msg Log message.
 * @param [in] format Logging message with the format.
 * @param [in] ... Variable argument.
 */
extern void
logger(int level, char* msg, char const* format, ...);

/**
 * @brief Enable system logging for the device.
 *
 */
extern void
sys_log_init();
#endif /* __LOGGER_H__ */