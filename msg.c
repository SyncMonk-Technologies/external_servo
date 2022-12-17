#include <stdint.h>
#include <asm/byteorder.h>
#include <arpa/inet.h>

#include "msg.h"
#include "logger.h"

#define ntoh64(x) __be64_to_cpu(x)

static uint64_t
timestamp_ntohns(struct Timestamp ts)
{
    uint64_t time_ns;
    time_ns = (ntohl(ts.seconds_lsb) | (ntohs(ts.seconds_msb) << 16));
    time_ns *= 1000000000ULL;

    time_ns += ntohl(ts.nanoseconds);
    return time_ns;
}

static int
process_rx_sync_msg(struct slave_rx_sync_timing_record* sync, int64_t* t1, int64_t* t2)
{
    int64_t corr;

    *t1 = timestamp_ntohns(sync->syncOriginTimestamp);
    *t2 = timestamp_ntohns(sync->syncEventIngressTimestamp);
    corr = ntoh64(sync->totalCorrectionField);
    *t1 = *t1 + (corr >> 16);
    pr_debug("t1: %lld t2: %lld  corr: %lld", *t1, *t2, ntoh64(sync->totalCorrectionField));
    return 0;
}

#ifdef LINUX_PTP
static int
process_delay_timing_msg(struct slave_delay_timing_record* delay, int64_t* t3, int64_t* t4)
{
    int64_t corr;

    *t3 = timestamp_ntohns(delay->delayOriginTimestamp);
    *t4 = timestamp_ntohns(delay->delayResponseTimestamp);
    corr = ntoh64(delay->totalCorrectionField);
    *t4 = *t4 - (corr >> 16);
    pr_debug("t3:%lld t4:%lld", *t3, *t4);
    return 0;
}
#endif

int
process_message(uint8_t* data, uint16_t* tlv_type, int64_t* master_time, int64_t* slave_time)
{
    struct ptp_header* ptp_hdr;
    struct signaling_msg* signal;
    uint8_t msg_type;
    struct tlv* tlv;
    struct slave_rx_sync_timing_data_tlv* rx_sync_tlv;
    struct slave_rx_sync_timing_record* rx_sync_record;
#ifdef LINUX_PTP
    struct slave_delay_timing_data_tlv* delay_tlv;
    struct slave_delay_timing_record* delay_record;
#endif
    ptp_hdr = (struct ptp_header*)data;
    msg_type = ptp_hdr->msmt & 0xf;

    if (msg_type == SIGNALING) {
        signal = (struct signaling_msg*)data;
        tlv = (struct tlv*)(signal->suffix);

        *tlv_type = ntohs(tlv->type);

        if (*tlv_type == TLV_SLAVE_RX_SYNC_TIMING_DATA) {
            pr_debug("TLV_SLAVE_RX_SYNC_TIMING_DATA tlv received.");
            rx_sync_tlv = (struct slave_rx_sync_timing_data_tlv*)tlv;
            rx_sync_record = (struct slave_rx_sync_timing_record*)(rx_sync_tlv->record);
            process_rx_sync_msg(rx_sync_record, master_time, slave_time);
        }
#ifdef LINUX_PTP
        else if (*tlv_type == SLAVE_DELAY_TIMING_DATA_NP) {
            pr_debug("SLAVE_DELAY_TIMING_DATA_NP tlv received");
            delay_tlv = (struct slave_delay_timing_data_tlv*)tlv;
            delay_record = (struct slave_delay_timing_record*)(delay_tlv->record);
            process_delay_timing_msg(delay_record, slave_time, master_time);
        }
#endif
        else {
            pr_debug("Unexpected signalling TLV received: %d", tlv_type);
        }
    } else {
        pr_debug("Unexpected PTP message received: %d\n", msg_type);
    }
    return 0;
}
