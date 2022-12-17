#include <stdio.h>
#include <stdint.h>
#include <error.h>
#include <string.h>
#include <linux/ptp_clock.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#include "msg.h"
#include "logger.h"
#include "config.h"
#include "uds.h"
#include "servo.h"
#include "utils.h"
#include "clockadj.h"
#include "tsproc.h"
#include "servo.h"

struct servo_config servo_config;
#if 0
= {
    .type = PI_SERVO,
#ifndef AURA
    .sw_ts = 1,
#endif
    .step_threshold = 0,
    .first_step_threshold = 0.00002,
    .offset_threshold = 0,
    .num_offset_values = 10,
    .kp = 0,
    .ki = 0,
    .ki_scale = 0.001,
    .kp_scale = 0.1,
    .kp_exponent = -0.3,
    .kp_norm_max = 0.7,
    .ki_exponent = 0.4,
    .ki_norm_max = 0.3,
};
#endif
struct device_config device_config;
#if 0
= {
    .uds_address = "/var/run/monitor",
    .poll_time = 2000,
    .filter_len = 10,
    .filter = 1,
#ifdef AURA
    .tod_device = "/dev/ptp0",
    .freq_device = "/dev/ptp1",
#endif
};
#endif

static int
phc_caps_get(clockid_t clkid, struct ptp_clock_caps* caps)
{
    int fd;
    int rv = 0;
    int max_adj;

    if (clkid == CLOCK_REALTIME) {
        max_adj = sysclk_max_freq();
        caps->max_adj = max_adj;
    } else if (clkid != CLOCK_INVALID) {

        fd = CLOCKID_TO_FD(clkid);
        rv = ioctl(fd, PTP_CLOCK_GETCAPS, caps);
        if (rv) {
            perror("PTP_CLOCK_GETCAPS");
        }
    }
    return rv;
}

clockid_t
phc_init(char* device_name)
{
    int fd;
    clockid_t clkid;

    fd = open(device_name, O_RDWR);
    if (fd < 0) {
        return CLOCK_INVALID;
    }

    clkid = FD_TO_CLOCKID(fd);
    return clkid;
}

static int
device_init(struct device_config* device_config)
{
    clockid_t clock_id;

    if (strlen(device_config->freq_device) == 0 && strlen(device_config->tod_device) == 0) {
        device_config->freq_clk_id = device_config->tod_clk_id = CLOCK_REALTIME;
    } else if (0 == strcmp(device_config->freq_device, device_config->tod_device)) {
        /* same device is controlled for Frequency, phase and TOD */
        clock_id = phc_init(device_config->freq_device);
        if (clock_id == CLOCK_INVALID) {
            return -1;
        }
        device_config->freq_clk_id = device_config->tod_clk_id = clock_id;
    } else {
        clock_id = phc_init(device_config->tod_device);
        if (clock_id == CLOCK_INVALID) {
            pr_err("device_config->tod_device: %s", device_config->tod_device);
            return -1;
        }
        device_config->tod_clk_id = clock_id;
        clock_id = phc_init(device_config->freq_device);
        if (clock_id == CLOCK_INVALID) {
            pr_debug("device_config->freq_device : %s", device_config->freq_device);
            return -1;
        }
        device_config->freq_clk_id = clock_id;
    }
    return 0;
}

static void
clock_update(struct tsproc* tsp, struct servo* servo, int64_t t1, int64_t t2)
{
    double adj;
    tmv_t remote_ts, local_ts;
    tmv_t master_offset;
    double weight;
    int64_t offset;
    enum servo_state state = SERVO_UNLOCKED;

    remote_ts.ns = t1;
    local_ts.ns = t2;

    tsproc_down_ts(tsp, remote_ts, local_ts);
    tsproc_update_offset(tsp, &master_offset, &weight);

    pr_debug("master_offset :%ld", master_offset.ns);
    offset = tmv_to_nanoseconds(master_offset);
    pr_debug("master_offset :%ld", offset);
    adj = servo_sample(servo, offset, t2, weight, &state);
    pr_debug("adj : %f", adj);

    tsproc_set_clock_rate_ratio(tsp, servo_rate_ratio(servo));

    pr_debug("servo_sample: %d", state);
    switch (state) {
    case SERVO_UNLOCKED:
        break;
    case SERVO_JUMP:
        clockadj_set_freq(device_config.freq_clk_id, -adj);
        clockadj_step(device_config.tod_clk_id, -offset);
        tsproc_reset(tsp, 0);
        break;
    case SERVO_LOCKED:
        clockadj_set_freq(device_config.freq_clk_id, adj);
        if (device_config.freq_clk_id == CLOCK_REALTIME) {
            sysclk_set_sync();
        }
        break;
    case SERVO_LOCKED_STABLE:
        clockadj_set_phase(device_config.freq_clk_id, adj);
        if (device_config.freq_clk_id == CLOCK_REALTIME) {
            sysclk_set_sync();
        }
        break;
    }
}
#ifdef LINUX_PTP
static void
path_delay(struct tsproc* tsp, int64_t t3, int64_t t4)
{
    tmv_t local_ts;
    tmv_t remote_ts;
    tmv_t delay;
    local_ts.ns = t3;
    remote_ts.ns = t4;

    tsproc_up_ts(tsp, local_ts, remote_ts);

    if (tsproc_update_delay(tsp, &delay)) {
        return;
    }
}
#endif

void
servo_configure(struct servo_config* config)
{
    servo_config = *config;
}

void
device_configure(struct device_config* config)
{
    device_config = *config;
}

int
main(int argc, char** argv)
{
#define MAX_PKT_LEN 1500
    int rv, opt;
    char* config_file = NULL;
    struct servo* servo = NULL;
    struct pollfd pollfd;
    struct ptp_clock_caps caps;
    double fadj;
    uint16_t msg_type;
    struct tsproc* tsp = NULL;
    uint8_t running = 1;
    uint16_t count;
    uint8_t data[MAX_PKT_LEN];
    int num_events;
    int64_t slave_time, master_time;
    struct address addr;
    int n = 0;
    sys_log_init();

#if 0
    rv = servo_handle_signals();
    if (rv < 0) {
        pr_err("Error in handling term signals");
        goto err;
    }
#endif
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            config_file = strdup(optarg);
            break;
        case 'h':
            // servo_usage();
            break;
        default:
            break;
        }
    }
    if (config_file == NULL) {
        pr_err("Configuration file missing");
        return -1;
    }
    rv = servo_config_parse(config_file);
    if (rv < 0) {
        pr_err("Error in parsing configuration file.");
        goto err;
    }

    /* Create UDS socket */
    device_config.fd = uds_create(device_config.uds_address, &device_config.daddr);
    if (device_config.fd < 0) {
        pr_err("Error in creating UDS socket: %s %s", device_config.uds_address, strerror(errno));
        goto err;
    }

    rv = device_init(&device_config);
    if (rv < 0) {
        pr_err("Error in device initialization");
        goto err;
    }

    tsp = tsproc_create(device_config.mode, device_config.filter, device_config.filter_len);
    if (tsp == NULL) {
        pr_err("Error in tsproc intialization");
        goto err;
    }
    /* PHC frequency adjustments */
    rv = phc_caps_get(device_config.freq_clk_id, &caps);
    if (rv < 0) {
        pr_err("Error in getting capabilities for PHC");
        goto err;
    }

    servo_config.max_frequency = caps.max_adj;
    if (device_config.tod_clk_id == CLOCK_REALTIME) {
        clockadj_init(device_config.tod_clk_id);
        servo_config.max_frequency = sysclk_max_freq();
        sysclk_set_leap(0);
    }
    fadj = clockadj_get_freq(device_config.freq_clk_id);
    clockadj_set_freq(device_config.freq_clk_id, fadj);
    servo_config.intial_adj = -fadj;

    /* Servo parameter config */
    servo = servo_create(&servo_config);
    if (servo == NULL) {
        pr_err("Error in servo initialization");
        goto err;
    }

    servo_sync_interval(servo, n < 0 ? 1.0 / (1 << -n) : 1 << n);
    /* Receive the packets using poll fd and then read the data and then pass the data to servo.
     */
    pollfd.fd = device_config.fd;
    pollfd.events = POLLIN | POLLPRI;

    while (running) {
        num_events = poll(&pollfd, 1, device_config.poll_time);

        if (num_events < 0) {
            pr_emerg("poll_failed");
            return -1;
        } else if (!num_events) {
            continue;
        }
        if (pollfd.revents & (POLLIN | POLLPRI)) {
            count = uds_recv(device_config.fd, data, MAX_PKT_LEN, &addr, 0);
            if (count) {
                rv = process_message(data, &msg_type, &master_time, &slave_time);

                switch (msg_type) {
                case TLV_SLAVE_RX_SYNC_TIMING_DATA:
                    /* Update the time adjust and phase adjust and freq adjust. */
                    clock_update(tsp, servo, master_time, slave_time);
                    break;
#ifdef LINUX_PTP
                case SLAVE_DELAY_TIMING_DATA_NP:
                    /* Update path delay for ts_proc */
                    path_delay(tsp, slave_time, master_time);
                    break;
#endif
                }
            }
        }
    }
err:
    if (servo) {
        servo_destroy(servo);
    }
    return -1;
}
