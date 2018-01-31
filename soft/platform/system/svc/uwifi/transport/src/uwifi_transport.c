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




#include "uctls_m.h"
#include "uwifi_config.h"
#include "hal_host.h"

typedef HAL_USB_CALLBACK_EP_TYPE_T MsgBody_t;
#include "itf_msg.h"

#include "hal_usb.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include <string.h>
#include "uwifi_m.h"

//#include "uwifip_debug.h"
#include "umssp_debug.h"

typedef enum
{
    UWIFI_BO_TRANSPORT_STATE_IDLE,
    UWIFI_BO_TRANSPORT_STATE_DATA,
    UWIFI_BO_TRANSPORT_STATE_DATA_END,
    UWIFI_BO_TRANSPORT_STATE_STATUS
} UWIFI_BO_TRANSPORT_STATE_T ;

// =============================================================================
// UMSS_BO_TRANSPORT_STATUS_T
// -----------------------------------------------------------------------------
/// Status for CSW
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

PRIVATE sxr_TaskDesc_t               g_uwifiBoTask;

PRIVATE UINT8                        g_uwifiBoIdTask = 0;
PRIVATE UINT8                        g_uwifiBoMbx=0xff;
PRIVATE UINT8                        g_uwifiBoInEp;
PRIVATE UINT8                        g_uwifiBoOutEp;
extern PUBLIC UINT8 USB_UCBSS_INTERNAL      g_UsbBulkOut[4096];
extern PUBLIC UINT8 USB_UCBSS_INTERNAL      g_UsbBulkIn [4096];

PRIVATE Msg_t                        g_uwifiBoMsg;
PRIVATE UWIFI_BO_TRANSPORT_STATE_T    g_uwifiBoState;

UINT8* recive_usb_form_host;
UINT8* txbuffer_to_host;
UINT16 recive_usb_data_length = 0;
UINT16 tx_to_host_length = 0;

UINT8 tx_sdio_data_buffer[1664] = {0x00,};
UINT8 sdio_data_size_hi = 0;
UINT8 sdio_data_size_low = 0;
UINT8 usb_transmit_end_flag = 0xFF;
UINT8 hostdata_type = 0;

extern UINT8 tmp_read_data[16][1664];
extern U8 SDIO_WifiDataWrite(UINT32 blockAddr, const U8* pWrite, UINT32 datalen);

PRIVATE HAL_USB_CALLBACK_RETURN_T
wifiCallback(HAL_USB_CALLBACK_EP_TYPE_T   type,
             HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_CMD:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb Callback Cmd\n");
            break;
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb Callback Cmd data\n");
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb Callback Receive end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb Callback Transmit end\n");
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb Callback Enable\n");
            hal_UsbRecv(g_uwifiBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
            g_uwifiBoState = UWIFI_BO_TRANSPORT_STATE_IDLE;
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb Callback Disable\n");
            break;
    }
    return(HAL_USB_CALLBACK_RETURN_KO);
}

PRIVATE HAL_USB_CALLBACK_RETURN_T
wifiCallbackEp(HAL_USB_CALLBACK_EP_TYPE_T   type,
               HAL_USB_SETUP_T*             setup)
{
    switch(type)
    {
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb EP Callback Cmd data\n");
            break;
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb EP Callback Receive end\n");
            g_uwifiBoMsg.B = HAL_USB_CALLBACK_TYPE_RECEIVE_END;
            if(g_uwifiBoMbx != 0xff)
            {
                sxr_Send(&g_uwifiBoMsg, g_uwifiBoMbx, SXR_SEND_MSG);
            }
            break;
        case HAL_USB_CALLBACK_TYPE_CMD:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb EP Callback Cmd\n");
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb EP Callback Transmit end\n");
            g_uwifiBoMsg.B = HAL_USB_CALLBACK_TYPE_TRANSMIT_END;
            if(g_uwifiBoMbx != 0xff)
            {
                sxr_Send(&g_uwifiBoMsg, g_uwifiBoMbx, SXR_SEND_MSG);
            }
            break;
        case HAL_USB_CALLBACK_TYPE_ENABLE:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb EP Callback Enable\n");
            break;
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            UMSS_TRACE(UMSS_INFO_TRC, 0, "--- Usb EP Callback Disable\n");
            break;
    }
    return(HAL_USB_CALLBACK_RETURN_OK);
}

void sdio_send_host_test(UINT16 txlength,UINT8 txbufIdx)
{
    memcpy(txbuffer_to_host,tmp_read_data[txbufIdx],txlength);

    sdio_data_size_low = txbuffer_to_host[0];
    sdio_data_size_hi = txbuffer_to_host[1];
    tx_to_host_length = sdio_data_size_hi & 0x0f;
    tx_to_host_length = (tx_to_host_length << 8) + sdio_data_size_low ;

    hal_UsbSend(g_uwifiBoInEp, g_UsbBulkIn,tx_to_host_length, 0);
}

void sdio_usbrecv()
{
    hal_UsbRecv(g_uwifiBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
}

VOID uwifi_TransportTask(VOID* param)
{
    Msg_t*                           msg;
    UINT32                           evt[4];
    param = param;
    recive_usb_form_host = (UINT8*) HAL_SYS_GET_UNCACHED_ADDR(g_UsbBulkOut);
    txbuffer_to_host = (UINT8*)HAL_SYS_GET_UNCACHED_ADDR(g_UsbBulkIn);

    while(1)
    {
        msg = sxr_Wait(evt, g_uwifiBoMbx);
        hal_HstSendEvent(0x11115555);
        if(msg == NULL)
        {
            UMSS_TRACE(UMSS_INFO_TRC, 0, "unknown evt:%x:%x:%x:%x\n",evt[0],evt[1],evt[2],evt[3]);
            continue;
        }
        switch(msg->B)
        {
            case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
                UMSS_TRACE(UMSS_INFO_TRC, 0, "Recvice data from host\n");
                sdio_data_size_low = recive_usb_form_host[0];
                sdio_data_size_hi = recive_usb_form_host[1];
                recive_usb_data_length = sdio_data_size_hi & 0x0f;
                recive_usb_data_length = (recive_usb_data_length << 8) + sdio_data_size_low ;
                hostdata_type = recive_usb_form_host[1] & 0xf0;
                SDIO_WifiDataWrite(0x07, recive_usb_form_host, recive_usb_data_length);
                if(hostdata_type & 0x10)
                {
                    hal_UsbRecv(g_uwifiBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
                }
                break;
            case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            case HAL_USB_CALLBACK_TYPE_DATA_CMD:
            case HAL_USB_CALLBACK_TYPE_CMD:
            case HAL_USB_CALLBACK_TYPE_ENABLE:
            case HAL_USB_CALLBACK_TYPE_DISABLE:
                break;
        }
    }
}


PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T*
uwifi_TransportOpen(CONST UWIFI_TRANSPORT_PARAMETERS_T*  cfg)
{
    HAL_USB_EP_DESCRIPTOR_T** epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T**)
             sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T*)*3);

    memset(g_UsbBulkOut, 0, sizeof(g_UsbBulkOut));
    memset(g_UsbBulkIn, 0, sizeof(g_UsbBulkIn));
    if(g_uwifiBoIdTask == 0)
    {
        g_uwifiBoTask.TaskBody  = uwifi_TransportTask;
        g_uwifiBoTask.TaskExit  = 0;
        g_uwifiBoTask.Name      = "UWIFI transport";
        g_uwifiBoTask.StackSize = 512;
        g_uwifiBoTask.Priority  = 189;
        g_uwifiBoMbx            = sxr_NewMailBox();
        g_uwifiBoIdTask         = sxr_NewTask(&g_uwifiBoTask);
        sxr_StartTask(g_uwifiBoIdTask, 0);
    }
    g_uwifiBoInEp  = HAL_USB_EP_DIRECTION_IN (cfg->inEpNum );
    g_uwifiBoOutEp = HAL_USB_EP_DIRECTION_OUT(cfg->outEpNum);

    epList[0] = uctls_NewBulkEpDescriptor(g_uwifiBoOutEp, wifiCallbackEp);
    epList[1] = uctls_NewBulkEpDescriptor(g_uwifiBoInEp , wifiCallbackEp);
    epList[2] = 0;

    // UWIFI_TRACE(UMSS_INFO_TRC, 0, "uwifi Open\n");
    return uctls_NewInterfaceDescriptor(0xFF, 0xFF, 0x01, 0x00,
                                        epList, 0, wifiCallback);

}

PRIVATE VOID uwifi_TransportClose(VOID)
{
    if(g_uwifiBoIdTask != 0)
    {
        sxr_StopTask(g_uwifiBoIdTask);
        sxr_FreeTask(g_uwifiBoIdTask);
        sxr_FreeMailBox(g_uwifiBoMbx);
        g_uwifiBoMbx = 0xff;

    }
    g_uwifiBoIdTask = 0;
}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST UWIFI_TRANSPORT_CALLBACK_T g_uwifitransportCallback =
{
    .open  = uwifi_TransportOpen,
    .close = uwifi_TransportClose
};

// =============================================================================
// FUNCTIONS
// =============================================================================

