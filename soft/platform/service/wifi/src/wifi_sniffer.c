#ifdef WIFI_SNIFFER
#include "event.h"
#include "cos.h"
#include "ts.h"
#include "wifi_config.h"
#include "wifi_sniffer.h"

#define printf(format, ...) \
    do \
    { \
        AT_OutputText(14, (format), ##__VA_ARGS__); \
    } while (0)

static UINT8 sniffer_status = SNIFFER_DONE;
static HANDLE sniffer_mutex;
static UINT32 g_saved_filter_type;
static UINT32 g_saved_monfilter_type;
static track_node *track_head = NULL;
static UINT32 track_num = 0;
#define SNIFF_FILTER 0x000FFE7F

static int sniffer_enable()
{
    int ret = 0;
    int i;
    unsigned char filter_type[4];
    UINT32 filter;
    UINT8 filter_type_len;
    printf("wifi_sniffer_start");

    ret = test_rda5890_generic_set_uchar(WID_USRCTL_RX_FRAME_FILTER,1);
    if (ret)
    {
        printf("sniffer_enable WID_USRCTL_RX_FRAME_FILTER fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }

    ret = test_rda5890_generic_get_str(WID_FILTER_TYPE, filter_type, &filter_type_len);
    if (ret)
    {
        printf("sniffer_enable get WID_FILTER_TYPE fail");
        return ret;
    }
    g_saved_filter_type = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    printf("sniffer_enable save filter_type %08x",g_saved_filter_type);

    ret = test_rda5890_generic_get_str(WID_MON_FILTER, filter_type, &filter_type_len);
    if (ret)
    {
        printf("sniffer_enable get WID_MON_FILTER fail");
        return ret;
    }
    g_saved_monfilter_type = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    printf("sniffer_enable save monfilter_type %08x",g_saved_monfilter_type);

    ret = test_rda5890_generic_set_ulong(WID_FILTER_TYPE,SNIFF_FILTER);
    if (ret)
    {
        printf("sniffer_enable set WID_FILTER_TYPE fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }
#if 0
    ret = test_rda5890_generic_get_str(WID_FILTER_TYPE, filter_type, &filter_type_len);
    if (ret)
    {
        printf("sniffer_enable get WID_FILTER_TYPE agian fail");
        return ret;
    }
    filter = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    if(filter != SNIFF_FILTER_TYPE)
    {
        printf("sniffer_enable check WID_FILTER_TYPE fail %d",filter);
        return -1;
    }
#endif

    return 0;
}

static int sniffer_disable()
{
    int ret = 0;
    unsigned char filter_type[4];
    UINT32 filter;
    UINT8 filter_type_len;
    printf("sniffer_disable");

    ret = test_rda5890_generic_set_ulong(WID_FILTER_TYPE,g_saved_filter_type);
    if (ret)
    {
        printf("sniffer_disable WID_FILTER_TYPE fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }
#if 0
    ret = test_rda5890_generic_get_str(WID_FILTER_TYPE, filter_type, &filter_type_len);
    if (ret)
    {
        printf("sniffer_disable get WID_FILTER_TYPE fail");
        return ret;
    }
    filter = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    if(filter != g_saved_filter_type)
    {
        printf("sniffer_disable check WID_FILTER_TYPE fail");
        return -1;
    }
#endif
    ret = test_rda5890_generic_set_ulong(WID_MON_FILTER,g_saved_monfilter_type);
    if (ret)
    {
        printf("sniffer_disable WID_MON_FILTER fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }
#if 0
    ret = test_rda5890_generic_get_str(WID_MON_FILTER, filter_type, &filter_type_len);
    if (ret)
    {
        printf("sniffer_disable get WID_MON_FILTER fail");
        return ret;
    }
    filter = filter_type[3]<<24 | filter_type[2]<<16 | filter_type[1]<<8 | filter_type[0];
    if(filter != g_saved_monfilter_type)
    {
        printf("sniffer_disable check WID_MON_FILTER fail");
        return -1;
    }
#endif
    ret = test_rda5890_generic_set_uchar(WID_USRCTL_RX_FRAME_FILTER,0);
    if (ret)
    {
        printf("sniffer_disable WID_USRCTL_RX_FRAME_FILTER fail");
        return ret;
    }
    if (-1== sdio_getInputData())
    {
        //return -1;
    }

    return 0;
}

void rda_notify_probe_req_handler(UINT8* sa)
{
    printf("rda_notify_probe_req_handler %02x:%02x:%02x:%02x:%02x:%02x",sa[0],sa[1],sa[2],sa[3],sa[4],sa[5]);
    BOOL found = FALSE;

    COS_WaitForSemaphore(sniffer_mutex,COS_WAIT_FOREVER);
    if(sniffer_status != SNIFFER_START)
    {
        return;
    }
    track_node *current_node = track_head;
    while(current_node)
    {
        if(memcmp(current_node->sa, sa, ETH_ALEN) == 0)
        {
            found = TRUE;
            break;
        }
        current_node = current_node->next;
    }

    if(!found)
    {
        track_node *new_node = COS_MALLOC(sizeof(track_node));
        memcpy(new_node->sa, sa, ETH_ALEN);
        new_node->next = track_head;
        track_head = new_node;
        track_num++;
    }
    COS_ReleaseSemaphore(sniffer_mutex);
}

int wifi_sniffer_start(UINT32 SnifferTime)
{
    int ret = sniffer_enable();
    if(ret)
    {
        printf("wifi_sniffer_start fail ret=%d",ret);
        return ret;
    }
    sniffer_status = SNIFFER_START;
    sniffer_mutex = COS_CreateSemaphore(1);
    wifi_SScanAps(1, SnifferTime > 12? 12*1000 : SnifferTime*1000);
    return 0;
}

UINT32 wifi_sniffer_result(track_node ** head)
{
    int ret;
    int num;
    ret = sniffer_disable();
    printf("wifi_sniffer_end ret=%d",ret);

    COS_WaitForSemaphore(sniffer_mutex,COS_WAIT_FOREVER);
    sniffer_status = SNIFFER_DONE;
    *head = track_head;
    num = track_num;
    COS_ReleaseSemaphore(sniffer_mutex);

    return num;
}

void wifi_sniffer_end()
{
    track_node *next, *current_node;
    current_node = track_head;
    while(current_node)
    {
        next = current_node->next;
        COS_FREE(current_node);
        current_node = next;
    }
    track_head = NULL;
    track_num = 0;
    COS_DeleteSemaphore(sniffer_mutex);
}

#endif
