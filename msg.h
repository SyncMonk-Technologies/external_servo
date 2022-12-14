#ifndef __MSG_H__
#define __MSG_H__

#include "utils.h"

#include <stdint.h>
#include <stdbool.h>


#define SYNC 0x0
#define DELAY_REQ 0x1
#define PDELAY_REQ 0x2
#define PDELAY_RESP 0x3
#define FOLLOW_UP 0x8
#define DELAY_RESP 0x9
#define PDELAY_RESP_FOLLOW_UP 0xA
#define ANNOUNCE 0xB
#define SIGNALING 0xC
#define MANAGEMENT 0xD

/**
 * @brief Table 1- Primitive PTP data types (IEEE 1588-2019)
 */

typedef bool Boolean;           /* TRUE or FALSE */
typedef uint8_t Enumeration8;   /* 8-bit enumerated value */
typedef uint16_t Enumeration16; /* 16-bit enumerated value */
typedef int8_t Integer8;        /* 8-bit signed integer */
typedef uint8_t UInteger8;      /* 8-bit unsigned integer */
typedef int16_t Integer16;      /* 16-bit signed integer */
typedef uint16_t UInteger16;    /* 16-bit unsigned integer */
typedef int32_t Integer32;      /* 16-bit signed integer */
typedef uint32_t UInteger32;    /* 16-bit signed integer */
typedef int64_t Integer64;      /* 16-bit signed integer */
typedef uint64_t UInteger64;    /* 16-bit signed integer */
typedef double Float64;         /* IEEE 754 binary64 (64-bit double-precision floating-point format) */
typedef uint8_t Octet;          /* 8-bit field not interpreted as a number */

/**
 * @brief The ClockIdentity type identifies unique entities within a PTP Network
 */
typedef Octet ClockIdentity[8];

typedef Integer64 TimeInterval;

/**
 * @brief The PortIdentity type identifies a PTP Port or a Link Port.
 *
 */
struct PortIdentity
{
    ClockIdentity clockIdentity;
    UInteger16 portNumber;
} __attribute__((__packed__));

/**
 * @brief Common PTP Header Table 35-Common PTP message header.
 *
 */
struct ptp_header
{
    UInteger8 msmt;    /* majorSdoId[4-7] | messageType [0-3] */
    UInteger8 version; /* minorVersion [4-7] | version [0-3] */
    UInteger16 messageLength;
    UInteger8 domainNumber;
    UInteger8 minorSdoId;
    Octet flagField[2];
    Integer64 correctionField;
    Octet messageTypeSpecific[4];
    struct PortIdentity sourcePortIdentity;
    UInteger16 sequenceId;
    UInteger8 controlId;
    Integer8 logMessageInterval;
} __attribute__((__packed__));

#define TLV_SLAVE_RX_SYNC_TIMING_DATA 0x8004

#ifdef LINUX_PTP
#define SLAVE_DELAY_TIMING_DATA_NP 0x7f00
#endif

struct tlv
{
    Enumeration16 type;
    UInteger16 length;
    Octet value[0];
} __attribute__((__packed__));

struct slave_rx_sync_timing_record
{
    UInteger16 sequenceId;
    struct Timestamp syncOriginTimestamp;
    TimeInterval totalCorrectionField;
    Integer32 scaledCumulativeRateOffset;
    struct Timestamp syncEventIngressTimestamp;
} __attribute__((__packed__));

struct slave_rx_sync_timing_data_tlv
{
    Enumeration16 type;
    UInteger16 length;
    struct PortIdentity sourcePortIdentity;
    struct slave_rx_sync_timing_record record[0];
} __attribute__((__packed__));

struct slave_delay_timing_record
{
    UInteger16 sequenceId;
    struct Timestamp delayOriginTimestamp;
    TimeInterval totalCorrectionField;
    struct Timestamp delayResponseTimestamp;
} __attribute__((__packed__));

struct slave_delay_timing_data_tlv
{
    Enumeration16 type;
    UInteger16 length;
    struct PortIdentity sourcePortIdentity;
    struct slave_delay_timing_record record[0];
} __attribute__((__packed__));

struct signaling_msg
{
    struct ptp_header header;
    struct PortIdentity targetPortIdentity;
    uint8_t suffix[0];
};


extern int
process_message(uint8_t* data, uint16_t *tlv_type, int64_t *master_time, int64_t *slave_time);

#endif /* __MSG_H__ */
