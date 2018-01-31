#ifndef __SNIFFER_H__
#define __SNIFFER_H__

typedef struct TRACK_NODE_T
{
    UINT8 sa[ETH_ALEN];
    struct TRACK_NODE_T *next;
}track_node;

enum wifi_sniffer_status{
    SNIFFER_START,
    SNIFFER_DONE
};

extern int wifi_sniffer_start(UINT32 SnifferTime);
extern void wifi_sniffer_end();
extern UINT32 wifi_sniffer_result();
extern void rda_notify_probe_req_handler(UINT8* sa);
#endif
