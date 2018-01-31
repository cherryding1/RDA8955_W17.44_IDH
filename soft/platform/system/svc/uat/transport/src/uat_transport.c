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

// #define LOCAL_LOG_LEVEL LOG_LEVEL_DEBUG

#include "uat_config.h"
//#include "hal_host.h"

typedef HAL_USB_CALLBACK_EP_TYPE_T MsgBody_t;

#include "itf_msg.h"
#include "hal_usb.h"
#include "sxr_ops.h"
#include "sxs_io.h"
#include "uctls_m.h"
#include <string.h>
#include "uat_m.h"
#include "svcp_debug.h"
#include "umssp_debug.h"
#include "usb_acm_cdc.h"

typedef enum {
    UAT_BO_TRANSPORT_STATE_IDLE,
    UAT_BO_TRANSPORT_STATE_DATA,
    UAT_BO_TRANSPORT_STATE_DATA_END,
    UAT_BO_TRANSPORT_STATE_STATUS
} UAT_BO_TRANSPORT_STATE_T;

// =============================================================================
// UMSS_BO_TRANSPORT_STATUS_T
// -----------------------------------------------------------------------------
/// Status for CSW
// =============================================================================

PRIVATE sxr_TaskDesc_t g_uatTaskDesc;
PRIVATE Msg_t g_uatBoMsg;
PRIVATE Msg_t g_uatBiMsg;
PRIVATE UINT8 g_uatTaskId = 0;
PRIVATE UINT8 g_uatTaskMbx = 0xff;
PRIVATE UINT8 g_uatBoInEp;
PRIVATE UINT8 g_uatBoOutEp;
extern PUBLIC UINT8 USB_UCBSS_INTERNAL g_UsbBulkOut[UAT_RX_EP_BUF_SIZE];
extern PUBLIC UINT8 USB_UCBSS_INTERNAL g_UsbBulkIn[UAT_TX_EP_BUF_SIZE];

PRIVATE UAT_BO_TRANSPORT_STATE_T g_uatBoState;
PRIVATE UINT32 g_in_buff, g_out_buff;

PRIVATE HAL_USB_CALLBACK_RETURN_T uatStreamCallback(HAL_USB_CALLBACK_EP_TYPE_T type,
                                                    HAL_USB_SETUP_T *setup)
{
    USBLOGI("UAT interface callback type=%d", type);
    if (setup != NULL)
    {
        USBLOGV("  request dest=%d type=%d dir=%d",
                setup->requestDesc.requestDest, setup->requestDesc.requestType,
                setup->requestDesc.requestDirection);
        USBLOGV("  req=%d val=%d index=%d len=%d",
                setup->request, setup->value, setup->index, setup->lenght);
    }

    switch (type)
    {
    case HAL_USB_CALLBACK_TYPE_CMD:
        break;
    case HAL_USB_CALLBACK_TYPE_DATA_CMD:
        break;
    case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
        break;
    case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
        break;
    case HAL_USB_CALLBACK_TYPE_ENABLE:
        hal_UsbRecv(g_uatBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
        g_uatBoState = UAT_BO_TRANSPORT_STATE_IDLE;
        break;
    case HAL_USB_CALLBACK_TYPE_DISABLE:
        break;
    }
    return (HAL_USB_CALLBACK_RETURN_KO);
}

PRIVATE BOOL g_uat_send_finished = TRUE;
PUBLIC BOOL uat_get_send_state()
{
    return g_uat_send_finished;
}

PUBLIC VOID uat_set_send_state(BOOL state)
{
    g_uat_send_finished = state;
}

PRIVATE HAL_USB_CALLBACK_RETURN_T atCallbackEp(HAL_USB_CALLBACK_EP_TYPE_T type,
                                               HAL_USB_SETUP_T *setup)
{
    USBLOGD("UAT ep callback type=%d", type);
    if (setup != NULL)
    {
        USBLOGV("  request dest=%d type=%d dir=%d",
                setup->requestDesc.requestDest, setup->requestDesc.requestType,
                setup->requestDesc.requestDirection);
        USBLOGV("  req=%d val=%d index=%d len=%d",
                setup->request, setup->value, setup->index, setup->lenght);
    }

    switch (type)
    {
    case HAL_USB_CALLBACK_TYPE_DATA_CMD:
        break;
    case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
        g_uatBoMsg.B = HAL_USB_CALLBACK_TYPE_RECEIVE_END;
        if (g_uatTaskMbx != 0xff)
            sxr_Send(&g_uatBoMsg, g_uatTaskMbx, SXR_SEND_MSG);
        break;
    case HAL_USB_CALLBACK_TYPE_CMD:
        break;
    case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
        uat_set_send_state(TRUE);
        g_uatBiMsg.B = HAL_USB_CALLBACK_TYPE_TRANSMIT_END;
        if (g_uatTaskMbx != 0xff)
            sxr_Send(&g_uatBiMsg, g_uatTaskMbx, SXR_SEND_MSG);
        break;
    case HAL_USB_CALLBACK_TYPE_ENABLE:
        break;
    case HAL_USB_CALLBACK_TYPE_DISABLE:
        break;
    }
    return (HAL_USB_CALLBACK_RETURN_OK);
}

UAT_TRANS_CALLBACK_T g_uat_stran_callbck = NULL;

PUBLIC UAT_TRANS_CALLBACK_T uat_transSetCallback(UAT_TRANS_CALLBACK_T cbFunc)
{
    UAT_TRANS_CALLBACK_T ret = g_uat_stran_callbck;
    g_uat_stran_callbck = cbFunc;
    return ret;
}

PUBLIC UINT32 uat_transGetBufferAdd(UINT8 direction)
{
    if (direction == 1)
    {
        return (UINT32)g_UsbBulkOut;
    }
    else
    {
        return (UINT32)g_UsbBulkIn;
    }
}

PUBLIC INT32 uat_send_data(UINT32 len, UINT32 flag)
{
    USBLOGD("UAT send len=%d", len);
    return hal_UsbSend(g_uatBoInEp, g_UsbBulkIn, len, 0);
}

// len must be a multiple of Max Packet Size 64
PUBLIC INT32 uat_recv_data(UINT32 len, UINT32 flag)
{
    USBLOGD("UAT recv len=%d", len);
    return hal_UsbRecv(g_uatBoOutEp, g_UsbBulkOut, len, 0);
}

char pstr_remind[] = "\n NO AT moudle,register the  function (g_uat_stran_callbck)! Reference usb_uart_init()\n";
VOID uat_transportTask(VOID *param)
{
    Msg_t *msg;
    UINT32 evt[4];
    UINT16 tran_len;
    param = param;
    g_in_buff = (UINT32)HAL_SYS_GET_UNCACHED_ADDR(g_UsbBulkOut);
    g_out_buff = (UINT32)HAL_SYS_GET_UNCACHED_ADDR(g_UsbBulkIn);

    while (1)
    {
        msg = sxr_Wait(evt, g_uatTaskMbx);
        if (msg == NULL)
        {
            USBLOGE("UAT unknown evt:%x:%x:%x:%x", evt[0], evt[1], evt[2], evt[3]);
            continue;
        }

        switch (msg->B)
        {
        case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
            tran_len = hal_UsbEpRxTransferedSize(g_uatBoOutEp);
            USBLOGD("UAT recv end len=%d", tran_len);

            if (g_uat_stran_callbck != NULL)
            {
                g_uat_stran_callbck(UAT_DATA_DIR_IN, tran_len);
            }
            else
            {
                memcpy(g_UsbBulkOut, (UINT8 *)pstr_remind, sizeof(pstr_remind));
                hal_UsbSend(g_uatBoInEp, g_UsbBulkOut, 512, 0); // just for echo test
                memset(g_UsbBulkOut, 0, (UINT16)sizeof(g_UsbBulkOut));
                hal_UsbRecv(g_uatBoOutEp, g_UsbBulkOut, (UINT16)sizeof(g_UsbBulkOut), 0);
            }
            break;
        case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
            USBLOGD("UAT tranmit end");
            tran_len = hal_UsbEpRxTransferedSize(g_uatBoInEp);
            if (g_uat_stran_callbck != NULL)
                g_uat_stran_callbck(UAT_DATA_DIR_OUT, 0xffff);
        case HAL_USB_CALLBACK_TYPE_DATA_CMD:
        case HAL_USB_CALLBACK_TYPE_CMD:
        case HAL_USB_CALLBACK_TYPE_ENABLE:
        case HAL_USB_CALLBACK_TYPE_DISABLE:
            break;
        }
    }
}

#ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T *uat_transportOpen(CONST USBAT_STREAM_PARAMETERS_T *cfg)
{
    HAL_USB_EP_DESCRIPTOR_T **epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T **)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T *) * 3);

    memset(g_UsbBulkOut, 0, sizeof(g_UsbBulkOut));
    memset(g_UsbBulkIn, 0, sizeof(g_UsbBulkIn));
    if (g_uatTaskId == 0)
    {
        g_uatTaskDesc.TaskBody = uat_transportTask;
        g_uatTaskDesc.TaskExit = 0;
        g_uatTaskDesc.Name = "UAT transport";
        g_uatTaskDesc.StackSize = 512;
        g_uatTaskDesc.Priority = 189;
        g_uatTaskMbx = sxr_NewMailBox();
        g_uatTaskId = sxr_NewTask(&g_uatTaskDesc);
        sxr_StartTask(g_uatTaskId, 0);
    }
    uat_set_send_state(TRUE);
    g_uatBoInEp = HAL_USB_EP_DIRECTION_IN(cfg->inEpNum);
    g_uatBoOutEp = HAL_USB_EP_DIRECTION_OUT(cfg->outEpNum);

    epList[0] = uctls_NewBulkEpDescriptor(g_uatBoOutEp, atCallbackEp);
    epList[1] = uctls_NewBulkEpDescriptor(g_uatBoInEp, atCallbackEp);
    epList[2] = 0;

    USBLOGI("UAT transport open");
    return uctls_NewInterfaceDescriptor(COMMUNICATIONS_INTERFACE_CLASS_VENDOR,
                                        DATA_INTERFACE_SUBCLASS_NONE,
                                        DATA_INTERFACE_PROTOCOL_NONE,
                                        0x01,
                                        epList,
                                        0,
                                        uatStreamCallback);
}
#else

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T *uat_transportOpen(CONST USBAT_STREAM_PARAMETERS_T *cfg)
{
    HAL_USB_EP_DESCRIPTOR_T **epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T **)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T *) * 3);

    memset(g_UsbBulkOut, 0, sizeof(g_UsbBulkOut));
    memset(g_UsbBulkIn, 0, sizeof(g_UsbBulkIn));
    if (g_uatTaskId == 0)
    {
        g_uatTaskDesc.TaskBody = uat_transportTask;
        g_uatTaskDesc.TaskExit = 0;
        g_uatTaskDesc.Name = "UAT transport";
        g_uatTaskDesc.StackSize = 512;
        g_uatTaskDesc.Priority = 189;
        g_uatTaskMbx = sxr_NewMailBox();
        g_uatTaskId = sxr_NewTask(&g_uatTaskDesc);
        sxr_StartTask(g_uatTaskId, 0);
    }
    uat_set_send_state(TRUE);
    g_uatBoInEp = HAL_USB_EP_DIRECTION_IN(cfg->inEpNum);
    g_uatBoOutEp = HAL_USB_EP_DIRECTION_OUT(cfg->outEpNum);

    epList[0] = uctls_NewBulkEpDescriptor(g_uatBoOutEp, atCallbackEp);
    epList[1] = uctls_NewBulkEpDescriptor(g_uatBoInEp, atCallbackEp);
    epList[2] = 0;

    USBLOGI("UAT transport open");
    return uctls_NewInterfaceDescriptor(COMMUNICATIONS_INTERFACE_CLASS_DATA,
                                        DATA_INTERFACE_SUBCLASS_NONE,
                                        DATA_INTERFACE_PROTOCOL_NONE,
                                        0x01,
                                        epList,
                                        0,
                                        uatStreamCallback);
}
#endif
PRIVATE VOID uat_transportClose(VOID)
{
    USBLOGI("UAT transport close");
    if (g_uatTaskId != 0)
    {
        sxr_StopTask(g_uatTaskId);
        sxr_FreeTask(g_uatTaskId);
        sxr_FreeMailBox(g_uatTaskMbx);
        g_uatTaskMbx = 0xff;
    }
    g_uatTaskId = 0;
}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST USBAT_STREAM_CALLBACK_T g_uattransportCallback = {
    .open = uat_transportOpen,
    .close = uat_transportClose};

PRIVATE UINT8 g_uatIntrEp, g_uatIntrEpo;

PRIVATE HAL_USB_CALLBACK_RETURN_T uatIntrEpCallback(HAL_USB_CALLBACK_EP_TYPE_T type,
                                                    HAL_USB_SETUP_T *setup)
{
    USBLOGI("UAT: interrupt callback type=%d", type);
    if (setup != NULL)
    {
        USBLOGV("  request dest=%d type=%d dir=%d",
                setup->requestDesc.requestDest, setup->requestDesc.requestType,
                setup->requestDesc.requestDirection);
        USBLOGV("  req=%d val=%d index=%d len=%d",
                setup->request, setup->value, setup->index, setup->lenght);
    }
    return (HAL_USB_CALLBACK_RETURN_OK);
}

PRIVATE HAL_USB_CALLBACK_RETURN_T uatControlCallback(HAL_USB_CALLBACK_EP_TYPE_T type,
                                                     HAL_USB_SETUP_T *setup)
{
    USBLOGI("UAT: control callback type=%d", type);
    if (setup != NULL)
    {
        USBLOGV("  request dest=%d type=%d dir=%d",
                setup->requestDesc.requestDest, setup->requestDesc.requestType,
                setup->requestDesc.requestDirection);
        USBLOGV("  req=%d val=%d index=%d len=%d",
                setup->request, setup->value, setup->index, setup->lenght);
    }

    switch (type)
    {
    case HAL_USB_CALLBACK_TYPE_CMD:
        if (setup->value == 0x0200)
        {
            USBLOGI("UAT: PC closed serial port");
        }
        else if (setup->value == 0x0201)
        {
            USBLOGI("UAT: PC open serial port");
        }
        break;
    case HAL_USB_CALLBACK_TYPE_DATA_CMD:
        break;
    case HAL_USB_CALLBACK_TYPE_RECEIVE_END:
        break;
    case HAL_USB_CALLBACK_TYPE_TRANSMIT_END:
        break;
    case HAL_USB_CALLBACK_TYPE_ENABLE:
        break;
    case HAL_USB_CALLBACK_TYPE_DISABLE:
        break;
    }
    return (HAL_USB_CALLBACK_RETURN_KO);
}

#ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_
PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T *uat_ControlOpen(CONST USBAT_CONTROL_PARAMETERS_T *cfg)
{
    HAL_USB_EP_DESCRIPTOR_T **epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T **)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T *) * 3);

    g_uatIntrEp = HAL_USB_EP_DIRECTION_IN(cfg->inEpNum);

    g_uatIntrEpo = HAL_USB_EP_DIRECTION_OUT(cfg->outEpNum);
    epList[0] = uctls_NewBulkEpDescriptor(g_uatIntrEp, uatIntrEpCallback);
    epList[1] = uctls_NewBulkEpDescriptor(g_uatIntrEpo, uatIntrEpCallback);
    epList[2] = 0;

    USBLOGI("UAT control open");
    return uctls_NewInterfaceDescriptor(COMMUNICATIONS_INTERFACE_CLASS_VENDOR,
                                        DATA_INTERFACE_SUBCLASS_NONE,
                                        DATA_INTERFACE_SUBCLASS_NONE,
                                        0x00,
                                        epList,
                                        NULL,
                                        uatControlCallback);
}

#else
PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T *
uat_ControlOpen(CONST USBAT_CONTROL_PARAMETERS_T *cfg)
{
    HAL_USB_EP_DESCRIPTOR_T **epList;

    epList = (HAL_USB_EP_DESCRIPTOR_T **)
        sxr_Malloc(sizeof(HAL_USB_EP_DESCRIPTOR_T *) * 2);

    g_uatIntrEp = HAL_USB_EP_DIRECTION_IN(cfg->intrEpNum);

    epList[0] = uctls_NewInterruptEpDescriptor(g_uatIntrEp, 0xff, uatIntrEpCallback);
    epList[1] = 0;

    USBLOGI("UAT control open");
    return uctls_NewInterfaceDescriptor(COMMUNICATIONS_INTERFACE_CLASS_CONTROL,
                                        COMMUNICATIONS_ACM_SUBCLASS,
                                        COMMUNICATIONS_V25TER_PROTOCOL,
                                        0x00,
                                        epList,
#ifdef SUPPORT_SPRD_BBAT
                                        "SpreadTrum  modem",
#else
                                        "Coolsand  modem",
#endif
                                        uatControlCallback);
}
#endif

PRIVATE VOID uat_ControlClose(VOID)
{
    USBLOGI("UAT control close");
}

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST USBAT_CONTROL_CALLBACK_T g_uatctrolCallback = {
    .open = uat_ControlOpen,
    .close = uat_ControlClose};

// =============================================================================
// FUNCTIONS
// =============================================================================
