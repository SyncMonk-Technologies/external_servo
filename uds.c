/**
 * @file uds.c
 * @note Copyright (C) 2022 SyncMonk Technologies <services@syncmonk.net>
 * @note SPDX-License-Identifier: GPL-2.0+
 */
#include <stdint.h>
#include <errno.h>
#include <net/if.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "uds.h"

int
uds_send(int fd, uint8_t* msg, uint16_t msg_len, struct address* daddr, uint16_t flags)
{
    struct sockaddr* sa = &daddr->sa;
    return sendto(fd, msg, msg_len, flags, sa, sizeof(struct sockaddr_un));
}

int
uds_recv(int fd, uint8_t* msg, uint16_t msg_len, struct address* saddr, uint16_t flags)
{
    saddr->len = sizeof(struct sockaddr_un);
    return recvfrom(fd, msg, msg_len, flags, &saddr->sa, &saddr->len);
}

int
uds_destroy(int fd)
{
    struct sockaddr_un sun;
    socklen_t len = sizeof(struct sockaddr_un);

    if (!getsockname(fd, (struct sockaddr*)&sun, &len) && sun.sun_family == AF_LOCAL) {
        unlink(sun.sun_path);
    }
    return close(fd);
}

int
uds_create(const char* path_name, struct address* addr)
{
    struct sockaddr_un sun;
    int fd;

    fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (fd < 0) {
        return -errno;
    }

    memset(&sun, 0, sizeof(struct sockaddr_un));
    sun.sun_family = AF_LOCAL;
    strncpy(sun.sun_path, path_name, sizeof(sun.sun_path) - 1);

    unlink(path_name);

    if (bind(fd, (struct sockaddr*)&sun, sizeof(sun))) {
        close(fd);
        return -errno;
    }
    if (addr) {
        addr->sun = sun;
    }
    return fd;
}
