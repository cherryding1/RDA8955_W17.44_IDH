/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/

#include "csw.h"
#include "event.h"
#include "base_prv.h"
#include "chip_id.h"
#include "sxs_io.h"
#include "sxr_mem.h"
#include "wifi_config.h"
#include "wifi_wpa_common.h"

HANDLE wifi_task_handle = NULL;
HANDLE wifi_task_rec_handle = NULL;

#define WIFI_TASK_PRIORITY  186
#define WIFI_TASK_REC_PRIORITY  185
#define WIFI_TASK_STACK_SIZE  4*1024
#define WIFI_TS_ID_SXS  9

extern BOOL wifi_sleep_flag ;
extern BOOL ap_connected ;
extern UINT32 wifi_enableSleepTimerCount;
extern BOOL istcpip_inited;
extern BOOL in_interruptHandle;
extern BOOL ap_connecting ;
extern BOOL rda5890_connected;

void TCpIPInit(void);
VOID netif_init(VOID);
void APS_Init(void);
VOID tcpip_mem_init(VOID);
BOOL BAL_ApsTaskTcpipProcForWifi (COS_EVENT *pEvent);
int test_rda5890_data_tx(UINT8 *data, unsigned short len);
UINT8  test_Check_Sdio_DataIn(UINT8 func);
INT  wifi_MAC_sleepEx(VOID);
UINT32 wifi_ConnectRsp(UINT8 flag);
int re_associate_bss_Ex(VOID);
BOOL wifi_poll_timer_start = FALSE;
VOID start_wifi_poll_timer(VOID)
{
    if(!wifi_poll_timer_start)
    {
        CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("start_wifi_poll_timer\n",0x08700001)));

        COS_SetTimerEX( wifi_task_handle, SDIO_POLL_TIMER_ID, COS_TIMER_MODE_SINGLE, 2 * 16384);

        //COS_SetTimerEX(  CFW_bal_GetTaskHandle(CSW_TASK_ID_APS), SDIO_POLL_TIMER_ID, COS_TIMER_MODE_SINGLE,2*16384);
    }
    wifi_poll_timer_start = TRUE;
}
VOID stop_wifi_poll_timer(VOID)
{
    if(wifi_poll_timer_start)
    {
        //  COS_KillTimerEX(  CFW_bal_GetTaskHandle(CSW_TASK_ID_APS), SDIO_POLL_TIMER_ID);

        COS_KillTimerEX( wifi_task_handle, SDIO_POLL_TIMER_ID);
    }
    wifi_poll_timer_start = FALSE;
}

BOOL wifi_preasso_wait_timer_start = FALSE;
VOID start_wifi_PreAssoWait_timer(UINT8 dur)
{

    if(!wifi_preasso_wait_timer_start)
    {
        COS_SetTimerEX(  wifi_task_handle, WIFI_PREASSO_SLEEP_WAIT_TIMER_ID, COS_TIMER_MODE_SINGLE, dur * 16384 / 10);
        CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("start_wifi_PreAssoWait_timer\n",0x08700002)));

    }
    wifi_preasso_wait_timer_start = TRUE;
}
VOID start_wifi_scanWait_timer(UINT8 dur)
{



    COS_SetTimerEX(  wifi_task_handle, WIFI_SCAN_WAIT_TIMER, COS_TIMER_MODE_SINGLE, dur * 16384);
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("start_wifi_scanWait_timer\n",0x08700003)));


}

VOID stop_wifi_PreAssoWait_timer(VOID)
{
    if(wifi_preasso_wait_timer_start)
    {
        CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("stop_wifi_PreAssoWait_timer\n",0x08700004)));

        COS_KillTimerEX(wifi_task_handle, WIFI_PREASSO_SLEEP_WAIT_TIMER_ID);
    }
    wifi_preasso_wait_timer_start = FALSE;
}



VOID start_connect_ap_timer(UINT16 dur)
{
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("start_connect_ap_timer\n",0x08700005)));

    //if(!wifi_poll_timer_start)
    if(dur > 30)
        COS_SetTimerEX( wifi_task_handle, WIFI_CONNECT_AP_TIMER_ID, COS_TIMER_MODE_SINGLE, dur * 16384);
    else
        COS_SetTimerEX( wifi_task_handle, WIFI_CONNECT_AP_TIMER_ID, COS_TIMER_MODE_SINGLE, 30 * 16384);

    //wifi_poll_timer_start = TRUE;
}
VOID stop_connect_ap_timer(VOID)
{
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("stop_connect_ap_timer\n",0x08700006)));

    //if(!wifi_poll_timer_start)
    COS_KillTimerEX(wifi_task_handle, WIFI_CONNECT_AP_TIMER_ID);
    //wifi_poll_timer_start = TRUE;
}
VOID start_assoc_ap_timer(UINT16 dur)
{
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("start_assoc_ap_timer\n",0x08700007)));

    COS_SetTimerEX( wifi_task_handle, WIFI_ASSOC_TIMER_ID, COS_TIMER_MODE_SINGLE, dur * 16384);
}
VOID stop_assoc_ap_timer(VOID)
{
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("stop_assoc_ap_timer\n",0x08700008)));

    //if(!wifi_poll_timer_start)
    COS_KillTimerEX(wifi_task_handle, WIFI_ASSOC_TIMER_ID);
    //wifi_poll_timer_start = TRUE;
}
#ifdef __SCAN_INFO__
VOID start_get_network_info_timer(UINT16 dur)
{
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("start_get_network_info_timer\n",0x08700009)));

    COS_SetTimerEX( wifi_task_handle, WIFI_GET_NETWORK_INFO_TIMER_ID, COS_TIMER_MODE_SINGLE, dur * 16384);
}
VOID stop_get_network_info_timer(VOID)
{
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("stop_get_network_info_timer\n",0x0870000a)));

    //if(!wifi_poll_timer_start)
    COS_KillTimerEX(wifi_task_handle, WIFI_GET_NETWORK_INFO_TIMER_ID);
    //wifi_poll_timer_start = TRUE;
}
#endif

VOID wifi_getAssoApRssi(void);
void wifi_test_rx(void);
UINT32 report_rssi_count = 0;
UINT32 wifi_PowerOffCount = 0;
extern  BOOL InDhcp;
extern UINT8 localMac[6] ;
extern BOOL wifi_no_off;
UINT32 event_count=0;
VOID Mac_WifiDataInd(UINT8 intflag)
{
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_MAC_WIFI_DATA_IND;
    EV.nParam1 = intflag; //0: data in; 1: sleep; 2: wake up
    EV.nParam2 = 0x11;
    EV.nParam3 = 0;
    if (event_count < 5) {
        COS_SendEvent(wifi_task_rec_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        event_count++;
    }
}

VOID Mac_WifiFrameInd(UINT8 bufIdx, UINT16 datalen)
{
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_FRAME_IND;
    EV.nParam1 = bufIdx; //0: data in; 1: sleep; 2: wake up
    EV.nParam2 = datalen;
    EV.nParam3 = 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

VOID wifi_ScanAps(UINT8 dur, UINT8 *ssid)
//VOID wifi_ScanAps(VOID)
{

    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_SCAN_REQ;
    EV.nParam1 = NORMAL_SCAN;
    EV.nParam2 = ssid;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    //COS_SendEvent(CFW_bal_GetTaskHandle(CSW_TASK_ID_APS), &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

VOID wifi_SScanAps(UINT8 channel, UINT16 time)
{

    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_SCAN_REQ;
    EV.nParam1 = SNIFF_SCAN;
    EV.nParam2 = channel;
    EV.nParam3 = time;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    //COS_SendEvent(CFW_bal_GetTaskHandle(CSW_TASK_ID_APS), &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

int wifi_mac_data_txReq(UINT8 *data, unsigned short len ,UINT8 isDataMalloc)
{
    SXS_DUMP((_CSW | TLEVEL(CFW_GPRS_TS_ID)), 0, data, len);
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_MAC_WIFI_DATA_REQ;
    EV.nParam1 = (UINT32)data; //0: data in; 1: sleep; 2: wake up
    EV.nParam2 = len;
    EV.nParam3 = isDataMalloc;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;

}
#ifdef __SCAN_INFO__

int wifi_getNetworkInfo_Req(void)
{

    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_GET_NETWORKINFO_REQ;
    EV.nParam1 = 0;
    EV.nParam2 = 0;
    EV.nParam3 = 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;

}
#endif
int wifi_mac_key_set_Req(UINT32 key_type)
{

    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_MAC_WIFI_KET_SET_REQ;
    EV.nParam1 = key_type; //0: data in; 1: sleep; 2: wake up
    EV.nParam2 = 0;
    EV.nParam3 = 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}
int wifi_ResetReq(VOID)
{

    /*
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_WIFI_WIFI_RESET_REQ;
    EV.nParam1=0;//0: data in; 1: sleep; 2: wake up
    EV.nParam2= 0;
    EV.nParam3= 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    */
    return 0;

}
int wifi_ResetReqEx(VOID)
{


    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_RESET_REQ;
    EV.nParam1 = 0; //0: data in; 1: sleep; 2: wake up
    EV.nParam2 = 0;
    EV.nParam3 = 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    return 0;

}
VOID wifi_ResetInd(VOID)
{

    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_WIFI_MMI_RESET_IND;
    EV.nParam1 = 0; //0: data in; 1: sleep; 2: wake up
    EV.nParam2 = 0;
    EV.nParam3 = 0;
    COS_SendEvent(COS_GetDefaultMmiTaskHandle(DEFAULT_MMI_TASK_ID), &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

}

VOID wifi_PowerOn(UINT8 *local_mac)
{

    COS_EVENT EV;
    SUL_MemCopy8(localMac, local_mac, 6);
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_POWER_ON_REQ;
    //EV.nParam1 = 0; //0: data in; 1: sleep; 2: wake up
    //EV.nParam2 = 0x11;
    //EV.nParam3 = 0;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

void wifi_PowerOff(void)
{
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_POWER_OFF_REQ;
    EV.nParam1 = 0;
    EV.nParam2 = 0;
    EV.nParam3 = 0;

    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}
void wifi_PowerOff_wifi(void);

//VOID wifi_ConnectApReq(VOID)
void store_ConnectInfo(wifi_conn_t *conn);

VOID wifi_Open_wifi(wifi_conn_t *conn, UINT16 dur)
{
    //store_ConnectInfo(conn);

    //power on
    //SUL_MemCopy8(localMac, local_mac, 6);
    wifi_PowerOnEx_1();
    //scan
    wifi_ScanApsEx_1(conn->ssid);
    //join
    /*
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_CONNECT_REQ;
    EV.nParam1 = dur;
    //EV.nParam2 = len;
    //EV.nParam1 = (UINT32)conn;
  


    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);*/
}

VOID wifi_ConnectApEx(wifi_conn_t *conn, UINT16 dur)
{

    store_ConnectInfo(conn);


    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_CONNECT_REQ;
    EV.nParam1 = dur;
    //EV.nParam2 = len;
    //EV.nParam1 = (UINT32)conn;



    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}
UINT32 wifi_test_Rx_ind(void)
{
    //CSW_TRACE(WIFI_TS_ID_SXS, 0,"##########wifi_test_Rx_ind \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_TEST_MODE_DATA_IND;
    ev.nParam1  = 0x11;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(wifi_task_handle, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}
UINT32 wifi_test_info_ind(UINT8 *info, UINT16 len)
{
    //CSW_TRACE(WIFI_TS_ID_SXS, 0,"##########wifi_test_info_ind \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_TEST_MODE_INFO_IND;
    ev.nParam1  = info;
    ev.nParam2  = len;
    ev.nParam3  = 0;
    COS_SendEvent(COS_GetDefaultATTaskHandle(), &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}

VOID wifi_DisconnectAp(void)
{
    COS_EVENT EV;
    SUL_ZeroMemory8(&EV, SIZEOF(COS_EVENT));
    EV.nEventId = EV_CFW_WIFI_DISCONNECT_REQ;
    COS_SendEvent(wifi_task_handle, &EV, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}
UINT32 wifi_poweronContinue(void)
{
    //      CSW_TRACE(WIFI_TS_ID_SXS, 0,"##########wifi_poweronContinue \n");

    COS_EVENT ev;

    ev.nEventId = EV_CFW_WIFI_POWERON_CONTINUE_REQ;
    ev.nParam1  = 0x11;
    ev.nParam2  = 0;
    ev.nParam3  = 0;
    COS_SendEvent(wifi_task_handle, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    return 0;
}


bool BT_IsInquiring(void)
{
	return FALSE;
}
VOID wifi_Reset(VOID);
UINT32 hal_sdioDataInIrqMaskGet(VOID);
VOID wifi_ConnectApEx_wifi(UINT16 dur);
VOID wifi_DisconnectAp_wifi(void);
VOID wifi_PowerOnEx_1(VOID);
VOID wifi_PowerOnEx_2(VOID);
VOID wifi_ScanApsEx_1(UINT8 *ssid);
VOID wifi_ScanApsEx_2(VOID);
VOID wifi_Reset_1(VOID);
int test_rda5890_set_StrWidEx(unsigned short wid,  unsigned char *key, unsigned char key_len);
#ifdef __SCAN_INFO__

int test_rda5890_start_scan_network_info(void);
extern INT  test_rda5890_set_NetworkInfo( UINT8 val);
void show_bss_info_node(void);

extern UINT8 network_info_time;
#endif
//INT16 tmp_rate;
#define WIFI_THROUGHOUT_TEST 0
#if WIFI_THROUGHOUT_TEST
extern INT32 start_time;
extern UINT32 total_rec_count;
#endif

INT32 test_getIPtimerNUM(void);
#ifdef USB_WIFI_SUPPORT
extern UINT8 *recive_usb_form_host;
extern U16 recive_usb_data_length;
#endif
extern WPA_SM_T sm_cxt ;


VOID wifi_Task ( VOID *pData)
{


    UINT32 sdio_int_mask = 0;
    COS_EVENT ev;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi_Task Start.\n",0x0870000b)));
    for(;;)
    {
        COS_WaitEvent(wifi_task_handle, &ev, COS_WAIT_FOREVER);
        CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(2), TSTXT(TSTR("wifi_Task event id:%d   wifi_chip state: %d \n",0x0870000c)), ev.nEventId, wifi_no_off);
        if(ev.nEventId == EV_CFW_WIFI_DATA_IND
        || ev.nEventId == EV_CFW_WIFI_CONNECTED_IND
        || ev.nEventId == EV_CFW_WIFI_IPADDR_CHANGE_REQ)
        {
            BAL_ApsTaskTcpipProcForWifi(&ev);
        }
        else if(ev.nEventId == EV_CFW_WIFI_FRAME_IND)
        {
            process_frame(ev.nParam1,ev.nParam2);
        }
        else if(ev.nEventId == EV_MAC_WIFI_DATA_REQ)
        {
            CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("wifi_task, EV_MAC_WIFI_DATA_REQ, size: %d\n",0x0870000d)), ev.nParam2);

            if(!InDhcp)
                wifi_PowerOffCount = 0;
            if(rda5890_connected)
            {
#ifdef USB_WIFI_SUPPORT
                //test_rda5890_data_tx(recive_usb_form_host, recive_usb_data_length);
#else
                test_rda5890_data_tx((UINT8 *)ev.nParam1, ev.nParam2);
                if (ev.nParam3)
                    sxr_Free(ev.nParam1);
#endif
            }
        }
        else if(ev.nEventId == EV_MAC_WIFI_KET_SET_REQ)
        {
            if(!InDhcp)
                wifi_PowerOffCount = 0;
            if(rda5890_connected)
            {
                switch (ev.nParam1)
                {
                    case WID_ADD_PTK:
                        test_rda5890_set_StrWidEx(WID_ADD_PTK, sm_cxt.set_ptk, sm_cxt.set_ptk_len);
                        break;

                    case WID_ADD_RX_GTK:
                        test_rda5890_set_StrWidEx(WID_ADD_RX_GTK, sm_cxt.set_gtk, sm_cxt.set_gtk_len);
                        break;
                    default :
                        break;

                }
                // test_rda5890_data_tx(ev.nParam1, ev.nParam2);
                sxr_Sleep(1638);

            }

        }
        else if(ev.nEventId  == EV_CFW_WIFI_SCAN_REQ)
        {
            if(ev.nParam1 == NORMAL_SCAN)
            {
                wifi_ScanApsEx_1(ev.nParam2);
            }
            else if(ev.nParam1 == SNIFF_SCAN)
            {
                wifi_SScanApsEx_1(ev.nParam2,ev.nParam3);
            }

        }
        else if(ev.nEventId  == EV_CFW_WIFI_POWER_ON_REQ)
        {
            CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("wifi_task,  EV_CFW_WIFI_POWER_ON_REQ size: %d\n",0x0870000d)), ev.nParam2);
            wifi_PowerOnEx_1();

        }
#ifdef __SCAN_INFO__
        else if(ev.nEventId  == EV_CFW_WIFI_GET_NETWORKINFO_REQ)
        {
            test_rda5890_start_scan_network_info();
            start_get_network_info_timer(2);



        }
#endif
        else if(ev.nEventId  == EV_CFW_WIFI_POWER_OFF_REQ)
        {
            CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("wifi_task,  EV_CFW_WIFI_POWER_OFF_REQ size: %d\n",0x0870000d)), ev.nParam2);
            wifi_PowerOff_wifi();

        }
        else if(ev.nEventId  == EV_CFW_WIFI_POWERON_CONTINUE_REQ)
        {
            wifi_PowerOnEx_2();

        }
        else if(ev.nEventId  == EV_CFW_WIFI_CONNECT_REQ)
        {
            wifi_ConnectApEx_wifi(ev.nParam1);

        }
        else if(ev.nEventId  == EV_CFW_WIFI_DISCONNECT_REQ)
        {
            wifi_DisconnectAp_wifi();

        }
        else if(ev.nEventId == EV_CFW_WIFI_RESET_REQ)
        {

            CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi_Task , receive EV_CFW_WIFI_RESET_REQ\n",0x0870000e)));
            //wifi_Reset();
            sxr_Sleep(16384 * 10);
            //wifi_Reset_1();
#ifndef USB_WIFI_SUPPORT
        }
        else if(ev.nEventId == EV_CFW_WIFI_TEST_MODE_DATA_IND)
        {
            CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi_Task , receive EV_CFW_WIFI_TEST_MODE_DATA_IND\n",0x0870000f)));

            wifi_test_rx();
#endif
        }
        else if(ev.nEventId == EV_TIMER)
        {
            if( wifi_no_off == FALSE )
                continue;
            if(ev.nParam1 == WIFI_CONNECT_AP_TIMER_ID)
            {
                CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("BAL_ApsTaskTcpipProcwifi recive WIFI_CONNECT_AP_TIMER_ID\n",0x08700010)) );

                wifi_ConnectRsp(0);



            }
            else if(ev.nParam1 == WIFI_ASSOC_TIMER_ID)
            {

                CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("BAL_ApsTaskTcpipProcwifi recive WIFI_ASSOC_TIMER_ID\n",0x08700011)) );

                re_associate_bss_Ex();

            }
            else if(ev.nParam1 == WIFI_PREASSO_SLEEP_WAIT_TIMER_ID)
            {


                CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(2), TSTXT(TSTR("wifi_task recive WIFI_PREASSO_SLEEP_WAIT_TIMER_ID,wifi_sleep_flag: %d; ap_connected:%d \n",0x08700012)) , wifi_sleep_flag, ap_connected);

#ifdef USE_WIFI_SLEEP
                if(!wifi_sleep_flag)
                {
                    if(ap_connected)
                        wifi_MAC_sleepEx();
                    else if((!ap_connecting) && (!InDhcp))
                        wifi_MAC_sleep();
                }
#endif
            }
            else if(ev.nParam1 == WIFI_SCAN_WAIT_TIMER)
            {
                wifi_ScanApsEx_2();
            }
#ifdef __SCAN_INFO__
            else if(ev.nParam1 == WIFI_GET_NETWORK_INFO_TIMER_ID)
            {

                CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi_Task , receive WIFI_GET_NETWORK_INFO_TIMER_ID\n",0x08700013)));
                test_rda5890_set_NetworkInfo(0);
                show_bss_info_node();
            }
#endif
            else if(ev.nParam1 == SDIO_POLL_TIMER_ID)
            {
                sdio_int_mask = hal_sdioDataInIrqMaskGet();
                report_rssi_count ++;
                wifi_enableSleepTimerCount++;
                wifi_PowerOffCount++;

                //tmp_rate = total_rec_count/((test_getIPtimerNUM()-start_time)*1024);
                //              CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(1), TSTXT(TSTR("recive SDIO_POLL_TIMER_ID, sdio_int_mask: 0x%x\n",0x08700014)),sdio_int_mask );
#if WIFI_THROUGHOUT_TEST

                //CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(4), TSTXT(TSTR("recive SDIO_POLL_TIMER_ID, total_rec_count: %d B, durition: %d, sdio_int_mask: 0x%x,in_interruptHandle: %d\n",0x08700015)),total_rec_count,(test_getIPtimerNUM()-start_time),sdio_int_mask ,in_interruptHandle);
                if((test_getIPtimerNUM() - start_time) > 0)
                {
                    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(3), TSTXT(TSTR("recive SDIO_POLL_TIMER_ID, total_rec_count: %d B, durition: %d, rate : %d kB/s\n",0x08700016)), total_rec_count, (test_getIPtimerNUM() - start_time), total_rec_count / (1000 * (test_getIPtimerNUM() - start_time)) );
                    //hal_HstSendEvent(0xff000000 + total_rec_count/(1000*(test_getIPtimerNUM()-start_time)));
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(2), TSTXT(TSTR("recive SDIO_POLL_TIMER_ID, total_rec_count: %d B, durition: %d\n",0x08700017)), total_rec_count, (test_getIPtimerNUM() - start_time));
                }
#else
                CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(3), TSTXT(TSTR("recive SDIO_POLL_TIMER_ID, report_rssi_count: %d, wifi_PowerOffCount: %d, sdio_int_mask: 0x%x\n",0x08700018)), report_rssi_count, wifi_PowerOffCount, sdio_int_mask );

#endif

#ifdef __SCAN_INFO__
                //if(network_info_time>15)
                //CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(3), TSTXT(TSTR("recive SDIO_POLL_TIMER_ID, report_rssi_count: %d, network_info_time: %d, sdio_int_mask: 0x%x\n",0x08700019)),report_rssi_count,network_info_time,sdio_int_mask );

                //  show_bss_info_node();
#endif
                if((sdio_int_mask == 0x00) && (in_interruptHandle == FALSE))
                {
                    hal_sdioDataInIrqMaskSet();
                }
#ifdef USE_WIFI_SLEEP
                if(wifi_PowerOffCount < 500)
#endif
                {

                    if(((report_rssi_count % 30) == 0) && (ap_connected))
                    {
                        CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("getting current Ap rssi.\n",0x0870001a)));
#ifndef USB_WIFI_SUPPORT
                        if((!BT_IsInquiring()) && (!InDhcp))
                        {
                            wifi_getAssoApRssi();
                        }
#else

                        wifi_getAssoApRssi();
#endif

                    }
                }
                //if(wifi_enableSleepTimerCount >5)
                //{
                //  #ifdef USE_WIFI_SLEEP

                //wifi_SetSdioSleep();
                //  #endif

                //}
                if(wifi_enableSleepTimerCount > 10)
                {
                    //CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(2), TSTXT(TSTR("BAL_ApsTaskTcpipProcwifi recive SDIO_POLL_TIMER_ID, count: 0x%x , wifi_sleep_enable: %d\n",0x0870001b)),wifi_enableSleepTimerCount,wifi_sleep_flag );
#ifdef USE_WIFI_SLEEP
                    if(!wifi_sleep_flag)
                    {
                        if(ap_connected)
                        {

                            wifi_MAC_sleepEx();
                        }
                        else if((!ap_connecting) && (!InDhcp))
                        {
                            wifi_MAC_sleep();
                        }
                        //else if(!ap_connecting)
                        // wifi_MAC_sleep();

                    }
                    else
                    {
                        wifi_SetSdioSleep();
                    }
#endif
                }

                if(wifi_PowerOffCount == 300)
                {
                    //#ifndef USE_WIFI_SLEEP
#if 0
                    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0) , TSTR("wifi will Power Off \n",0x0870001c));

                    wifi_PowerOffCount = 0;
                    wifi_NeedPowerOffInd();
#endif
                    COS_SetTimerEX(wifi_task_handle, SDIO_POLL_TIMER_ID, COS_TIMER_MODE_SINGLE, 1 * 16384);

                }
                else
                {

                    COS_SetTimerEX(wifi_task_handle, SDIO_POLL_TIMER_ID, COS_TIMER_MODE_SINGLE, 1 * 16384);
                }

                /*
                            if(wifi_enableSleepTimerCount >5)
                            {
                                if(!wifi_sleep_flag)
                                {
                                    //if(ap_connected)
                                     //   wifi_MAC_sleepEx();
                                    //else if(!ap_connecting)
                                    //   wifi_MAC_sleep();

                                }
                                //wifi_SetSdioSleep();
                            }

                            if((wifi_enableSleepTimerCount >50)&&(wifi_sleep_flag))
                            {
                                CSW_TRACE(_CSW|TLEVEL(CFW_TCPIP_TS_ID_SXS)|TDB|TNB_ARG(1) ,TSTR("##########int mask: 0x%x \n",0x0870001d),hal_sdioDataInIrqMaskGet());
                                //test_rda5890_SetWifiSleepEx();
                               //   if(!ap_connected)
                                    {
                                    //    wifi_DisconnectAp();
                                    //  wifi_MAC_sleep();
                                    }
                            //  wifi_enableSleepTimerCount = 0x00;
                            }else{
                            //if(wifi_poll_timer_start)
                            {
                                if(ap_connected)
                                {
                                //start_wifi_poll_timer
                                    COS_SetTimerEX(BAL_TH(BAL_CFW_ADV_TASK_PRIORITY), SDIO_POLL_TIMER_ID, COS_TIMER_MODE_SINGLE, 2*16384);
                                }else
                                {
                                    COS_SetTimerEX(BAL_TH(BAL_CFW_ADV_TASK_PRIORITY), SDIO_POLL_TIMER_ID, COS_TIMER_MODE_SINGLE, 2*16384);
                                }
                            }
                    */
            }
        }
        //BAL_ApsTaskTcpipProcForWifi(&ev);

    }
}
extern BOOL useingIrq;
VOID wifi_Task_rec ( VOID *pData)
{


    UINT32 sdio_int_mask = 0;
    COS_EVENT ev;
    SUL_ZeroMemory32( &ev, SIZEOF(COS_EVENT) );
    CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(0), TSTXT(TSTR("wifi_Task_rec Start.\n",0x0870001e)));
    for(;;)
    {
        COS_WaitEvent(wifi_task_rec_handle, &ev, COS_WAIT_FOREVER);
        CSW_TRACE(_CSW|TLEVEL(WIFI_TS_ID_SXS)|TDB|TNB_ARG(2), TSTXT(TSTR("wifi_Task_rec event id:%d;  useingIrq: %d\n",0x0870001f)), ev.nEventId, useingIrq);

        if((ev.nEventId == EV_MAC_WIFI_DATA_IND) && (useingIrq))
        {
            test_Check_Sdio_DataIn(1);
            event_count--;
        }

    }
}


VOID wifi_task_init(VOID)
{
    wifi_task_handle = COS_CreateTask_Prv(wifi_Task,
                                          NULL, NULL,
                                          WIFI_TASK_STACK_SIZE,
                                          WIFI_TASK_PRIORITY,
                                          COS_CREATE_DEFAULT,
                                          0,    "Wifi_Task");

    wifi_task_rec_handle = COS_CreateTask_Prv(wifi_Task_rec,
                           NULL, NULL,
                           WIFI_TASK_STACK_SIZE,
                           WIFI_TASK_REC_PRIORITY,
                           COS_CREATE_DEFAULT,
                           0,    "Wifi_Task_rec");
}

BOOL wifi_getConnectStatus(){
	return rda5890_connected;
}
