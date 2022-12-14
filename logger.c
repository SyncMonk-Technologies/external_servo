/**
 * \file logger.c
 *
 * Logging mechanism for the applications.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <config.h>
#include <logger.h>
/******************************************************************************
 * Local Definitions
 *****************************************************************************/
static struct logger_config logger_config = { .log_level = 6, .use_syslog = 1, .use_stdout = 1, .msg_tag = "servo" };
static FILE* log_fp;

/******************************************************************************
 * Public Functions
 *****************************************************************************/
void
logger(int level, char* msg, char const* format, ...)
{
    struct timespec ts;
    va_list args;
    char buffer[1024];
    FILE* f;

    if (level > logger_config.log_level) {
        return;
    }

    clock_gettime(CLOCK_MONOTONIC, &ts);

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (logger_config.use_stdout) {
        f = level >= LOG_NOTICE ? stdout : stderr;
        fprintf(f,
                "[%s][%lld.%03ld]:[%s] %s\n",
                logger_config.msg_tag ? logger_config.msg_tag : "",
                (long long)ts.tv_sec,
                ts.tv_nsec / 1000000,
                msg,
                buffer);
        fflush(f);
    }

    if (logger_config.use_syslog) {
        fprintf(log_fp,
                "[%s][%lld.%03ld]:[%s] %s\n",
                logger_config.msg_tag ? logger_config.msg_tag : "",
                (long long)ts.tv_sec,
                ts.tv_nsec / 1000000,
                msg,
                buffer);
        fflush(log_fp);
    }
}

struct logger_config*
logger_config_get()
{
    return &logger_config;
}

int
logger_configure(struct logger_config* config)
{
    logger_config = *config;
    if (!log_fp) {
        logger_config.use_syslog = 0;
    }
    return 0;
}

void
sys_log_init()
{
    log_fp = fopen("/var/log/messages", "w");
    if (log_fp == NULL) {
        logger_config.use_syslog = 0;
        pr_warning("System Logging cannot be used.");
    }
}