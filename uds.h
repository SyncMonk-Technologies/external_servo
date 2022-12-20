/**
 * @file uds.h
 * @note Copyright (C) 2022 SyncMonk Technologies <services@syncmonk.net>
 * @note SPDX-License-Identifier: GPL-2.0+
 */

#ifndef __UDS_H__
#define __UDS_H__

#include <netpacket/packet.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>

/**
 * @brief Socket address.
 *
 */
struct address
{
    socklen_t len;
    union
    {
        struct sockaddr_un sun;
        struct sockaddr sa;
    };
};

/**
 * @brief
 *
 *
 * @param fd
 * @param msg
 * @param msg_len
 * @param daddr
 * @param flags
 * @return
 */
extern int
uds_send(int fd, uint8_t* msg, uint16_t msg_len, struct address* daddr, uint16_t flags);

/**
 * @brief
 *
 *
 * @param fd
 * @param msg
 * @param msg_len
 * @param saddr
 * @param flags
 * @return
 */
extern int
uds_recv(int fd, uint8_t* msg, uint16_t msg_len, struct address* saddr, uint16_t flags);

/**
 * @brief
 *
 *
 * @param fd
 * @return
 */
extern int
uds_destroy(int fd);

/**
 * @brief
 *
 *
 * @param iface_name
 * @param addr
 * @return
 */
extern int
uds_create(const char* iface_name, struct address* addr);

#endif /*__UDS_H__ */
