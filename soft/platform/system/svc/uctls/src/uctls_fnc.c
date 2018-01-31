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

#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxr_tim.h"

#include "tgt_uctls_cfg.h"

typedef UCTLS_BODY_MSG_T MsgBody_t;
#include "itf_msg.h"

#include "string.h"
#include "uctlsp_debug.h"
#include "usb_acm_cdc_id.h"

// FIXME TODO Add comments.


PRIVATE UINT8                        g_UctlsModeNum         =  0;

PRIVATE UINT8                        g_UctlsMbx;

PRIVATE UCTLS_ID_T                   g_UctlsId              = -1;
PRIVATE HAL_USB_DEVICE_DESCRIPTOR_T* g_UctlsDeviceDesc      =  0;
PRIVATE UCTLS_CHARGER_STATUS_T       g_UctlsChargerStatus;

UCTLS_STATUS_T                       g_UctlsStatus          =
    UCTLS_STATUS_CLOSE;

PRIVATE VOID uctls_UsbDeviceNotDetected(VOID*);

PRIVATE HAL_USB_DEVICE_DESCRIPTOR_T* uctls_GetDescriptor(BOOL SPEED_ENUM)
{
    Msg_t*                           msg;

    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_GetDescriptor");

    switch(g_UctlsStatus)
    {
        case UCTLS_STATUS_CHARGER:
            if(SPEED_ENUM)
                g_UctlsStatus = UCTLS_STATUS_SPEED_ENUM;
            break;
        case UCTLS_STATUS_SPEED_ENUM:
            if(g_UctlsMbx != 0xFF && g_UctlsStatus != UCTLS_STATUS_CONNECTED)
            {
                msg              = (Msg_t*) sxr_Malloc(sizeof(Msg_t));
                msg->H.Id        = UCTLS_HEADER_ID;
                msg->B           = UCTLS_MSG_USB_DEVICE_CONNECTED;
                sxr_Send(msg, g_UctlsMbx, SXR_SEND_MSG);
            }

            g_UctlsStatus = UCTLS_STATUS_CONNECTED;

            sxr_StopFunctionTimer(uctls_UsbDeviceNotDetected);
            break;
        default :
            break;
    }

    return(g_UctlsDeviceDesc);
}
#ifdef USB_WIFI_SUPPORT
PRIVATE HAL_USB_DEVICE_DESCRIPTOR_T* uctls_GetDescriptor_wifi(BOOL SPEED_ENUM)
{
    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_GetDescriptor");
    g_UctlsStatus = UCTLS_STATUS_CONNECTED;

    return(g_UctlsDeviceDesc);
}

PUBLIC VOID uctls_Open_wifi(
    UINT16                   usbVendor,
    UINT16                   usbProduct,
    UINT16                   usbVersion,
    UINT8*                   usbSerialNumber)
{
    CONST UCTLS_SERVICE_LIST_T*      modes;
    HAL_USB_CONFIG_DESCRIPTOR_T**    usbConfigs;
    UINT8 num;

    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_Open_wifi");

    modes = tgt_GetUctlsConfig();
    num = 0;
    while (modes->servicesList[0].serviceCallback != 0)
    {
        num++;
        modes++;
    }
    g_UctlsModeNum = num;

    if(g_UctlsStatus == UCTLS_STATUS_CLOSE)
    {
        usbConfigs        = (HAL_USB_CONFIG_DESCRIPTOR_T**)
                            sxr_Malloc(sizeof(HAL_USB_CONFIG_DESCRIPTOR_T*)*2);

        // Set config MaxPower 200mA
        usbConfigs[0]     = uctls_NewConfigDescriptor(0xfa, 0x64, 0, 0);
        usbConfigs[1]     = 0;
        // Set device Class = Not defined
        g_UctlsDeviceDesc =  uctls_NewDeviceDescriptor(0xFF, 0xFF, 0x01, usbVendor,
                             usbProduct, usbVersion,
                             usbConfigs,
                             usbSerialNumber,
                             "coolsand");

        uctls_ChangeMode(UCTLS_ID_UCDCACM);
    }
    hal_UsbOpen(uctls_GetDescriptor_wifi);

}

PUBLIC VOID uctls_Close_wifi(VOID)
{
    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_Close");

    if(g_UctlsStatus == UCTLS_STATUS_CONNECTED)
    {
        hal_UsbClose();
        if(g_UctlsDeviceDesc)
        {
            uctls_CleanDeviceDescriptor(g_UctlsDeviceDesc);
            g_UctlsDeviceDesc = 0;
        }
    }
    g_UctlsStatus = UCTLS_STATUS_CLOSE;
}

#endif

PUBLIC VOID uctls_Open(UINT8                    mbx,
                       UINT16                   usbVendor,
                       UINT16                   usbProduct,
                       UINT16                   usbVersion,
                       UINT8*                   usbSerialNumber)
{
    CONST UCTLS_SERVICE_LIST_T*      modes;
    HAL_USB_CONFIG_DESCRIPTOR_T**    usbConfigs;
    UINT8 num;
    if(g_UctlsStatus != UCTLS_STATUS_CLOSE) return;

    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_Open");

    modes = tgt_GetUctlsConfig();
    num = 0;
    while (modes->servicesList[0].serviceCallback != 0)
    {
        num++;
        modes++;
    }
    g_UctlsModeNum = num;

    g_UctlsMbx        = mbx;

    if(g_UctlsModeNum > 0 &&
            (g_UctlsStatus == UCTLS_STATUS_CLOSE || g_UctlsDeviceDesc == 0))
    {
        usbConfigs        = (HAL_USB_CONFIG_DESCRIPTOR_T**)
                            sxr_Malloc(sizeof(HAL_USB_CONFIG_DESCRIPTOR_T*)*2);

        // Set config MaxPower 100mA
        usbConfigs[0]     = uctls_NewConfigDescriptor(50, 0, 0, 0);
        usbConfigs[1]     = 0;
        // Set device Class = Not defined

        g_UctlsDeviceDesc =  uctls_NewDeviceDescriptor(0, 0, 0, usbVendor,
                             usbProduct, usbVersion,
                             usbConfigs,
                             usbSerialNumber,
                             "Coolsand"
                                                      );
#ifdef SUPPORT_QUICK_VCOM
        if(pmd_IsVcomGpioDetect())
        {
            uctls_ChangeMode(UCTLS_ID_UCDCACM);
        }
#endif
    }

    g_UctlsStatus        = UCTLS_STATUS_OPEN;
    g_UctlsChargerStatus = UCTLS_CHARGER_STATUS_DISCONNECTED;
}

PRIVATE VOID uctls_UsbDeviceNotDetected(VOID* data)
{
    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_UsbDeviceNotDetected: No device detected");

    data = data;

    hal_UsbClose();

    /// @todo add link with pmd/pms
}

PUBLIC VOID uctls_ChargerStatus(UCTLS_CHARGER_STATUS_T status)
{
    Msg_t*                           msg;

    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_ChargerStatus %i", status);

    if(status != g_UctlsChargerStatus)
    {
        UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_ChargerStatus: Change %i", status);
        switch(status)
        {
            case UCTLS_CHARGER_STATUS_CONNECTED:
                g_UctlsStatus = UCTLS_STATUS_CHARGER;
                sxr_StartFunctionTimer(163840 , uctls_UsbDeviceNotDetected,
                                       (VOID*)NULL, 0x02);
                hal_UsbOpen(uctls_GetDescriptor);
                break;
            case UCTLS_CHARGER_STATUS_DISCONNECTED:
                uctls_ChangeDeviceDescriptor(g_UctlsDeviceDesc,0,0,0);
                uctls_ChangeDeviceDescriptorSerialNmber(g_UctlsDeviceDesc,"USB Controller 1.0");
                hal_UsbClose();
                if(g_UctlsMbx != 0xFF)
                {
                    msg              = (Msg_t*) sxr_Malloc(sizeof(Msg_t));
                    msg->H.Id        = UCTLS_HEADER_ID;
                    msg->B           = UCTLS_MSG_USB_DEVICE_DISCONNECTED;
                    sxr_Send(msg, g_UctlsMbx, SXR_SEND_MSG);
                }
                g_UctlsStatus = UCTLS_STATUS_OPEN;
                sxr_StopFunctionTimer(uctls_UsbDeviceNotDetected);
                break;
        }

        g_UctlsChargerStatus = status;
    }
}

PUBLIC VOID uctls_Close(VOID)
{
    Msg_t*                           msg;

    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_Close");

    if(g_UctlsStatus == UCTLS_STATUS_CONNECTED ||
            g_UctlsStatus == UCTLS_STATUS_CHARGER ||
            g_UctlsStatus == UCTLS_STATUS_SPEED_ENUM)
    {
        hal_UsbClose();
        if(g_UctlsDeviceDesc)
        {
            uctls_CleanDeviceDescriptor(g_UctlsDeviceDesc);
            g_UctlsDeviceDesc = 0;
        }
    }
    if(g_UctlsStatus == UCTLS_STATUS_CONNECTED)
    {
        if(g_UctlsMbx != 0xFF)
        {
            msg              = (Msg_t*) sxr_Malloc(sizeof(Msg_t));
            msg->H.Id        = UCTLS_HEADER_ID;
            msg->B           = UCTLS_MSG_USB_DEVICE_DISCONNECTED;
            sxr_Send(msg, g_UctlsMbx, SXR_SEND_MSG);
        }
    }

    sxr_StopFunctionTimer(uctls_UsbDeviceNotDetected);

    g_UctlsMbx    = 0xFF;
    g_UctlsStatus = UCTLS_STATUS_CLOSE;
}

PUBLIC VOID uctls_ChangeMode(UCTLS_ID_T mode)
{
    CONST UCTLS_SERVICE_LIST_T*      modes;
    UINT8                            nbInterface;
    UINT8                            i;
    UINT8                            j;
    UINT8                            classid;
    UINT8                            indexInterfaceList;
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceList;
    HAL_USB_INTERFACE_DESCRIPTOR_T** interfaceTmpList;

    if(g_UctlsId == mode)
    {
        // return;
    }
#ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_
    classid = 0;
#else

#ifdef SUPPORT_SPRD_CDCID
    classid = 0;
#else
    classid =2;
#endif
#endif

#ifdef _UCDCACM_
    if(mode == UCTLS_ID_UCDCACM)
    {
        uctls_ChangeDeviceDescriptor(g_UctlsDeviceDesc,classid,CONFIG_USBD_VENDORID,CONFIG_USBD_PRODUCTID_CDCACM);
        uctls_ChangeDeviceDescriptorSerialNmber(g_UctlsDeviceDesc,NULL);
    }
    else
#endif
#ifdef _URNDIS_
    if(mode == UCTLS_ID_URNDIS)
    {
        uctls_ChangeDeviceDescriptor(g_UctlsDeviceDesc,USB_CLASS_WIRELESS_CONTROLLER,idVendor,idProduct);
        uctls_ChangeDeviceDescriptorSerialNmber(g_UctlsDeviceDesc,"2c6fc3486ad55b56");
    }
    else
#endif
    {
        uctls_ChangeDeviceDescriptor(g_UctlsDeviceDesc,0,0,0);
    }

    UCTLS_ASSERT(mode < g_UctlsModeNum, "Invalid UCTLS ID: %u (shoud < %u)",
                 mode, g_UctlsModeNum);

    modes = tgt_GetUctlsConfig();
    //close previous service
    if(g_UctlsId != -1)
    {
        for(i = 0; i < UCTLS_ID_MAX; ++i)
        {
            if(modes[g_UctlsId].servicesList[i].serviceCallback == NULL)
            {
                continue;
            }
            modes[g_UctlsId].servicesList[i].serviceCallback->close();
        }
    }

    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls_ChangeMode: Change %i", mode);

    g_UctlsDeviceDesc->usbMode = mode;

    if(g_UctlsDeviceDesc->configList[0]->interfaceList)
    {
        uctls_CleanInterfaceListDescriptor(
            g_UctlsDeviceDesc->configList[0]->interfaceList);
        g_UctlsDeviceDesc->configList[0]->interfaceList = 0;
    }

    g_UctlsId                                   = mode;
    g_UctlsDeviceDesc->configList[0]->configIdx = mode + 1;

    for(i = 0, nbInterface = 0; i < UCTLS_ID_MAX; ++i)
    {
        if(modes[g_UctlsId].servicesList[i].serviceCallback == NULL)
        {
            continue;
        }
        nbInterface++;
    }

    if(nbInterface == 0)
    {
        if(g_UctlsStatus == UCTLS_STATUS_CONNECTED ||
                g_UctlsStatus == UCTLS_STATUS_CHARGER ||
                g_UctlsStatus == UCTLS_STATUS_SPEED_ENUM)
        {
            hal_UsbReset(TRUE);
            sxr_Sleep(8000); // Wait 0.5s
            hal_UsbReset(FALSE);
        }
        return;
    }

    if(nbInterface != 0)
    {
        interfaceList = (HAL_USB_INTERFACE_DESCRIPTOR_T**)
                        sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*) * 16);

        UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls: interface list %#x\n", interfaceList);

        memset(interfaceList, 0,
               sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*) * 16);

        for(i = 0, indexInterfaceList = 0; i < UCTLS_ID_MAX; ++i)
        {
            if(modes[g_UctlsId].servicesList[i].serviceCallback == NULL)
            {
                continue;
            }
            interfaceTmpList =
                modes[g_UctlsId].servicesList[i].serviceCallback->open(
                    &modes[g_UctlsId].servicesList[i].serviceCfg);
            if(interfaceTmpList)
            {
                for(j = 0; interfaceTmpList[j]; ++j)
                {
                    interfaceList[indexInterfaceList] = interfaceTmpList[j];
                    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls: interface %i %#x\n",
                                indexInterfaceList, interfaceTmpList[j]);
                    ++indexInterfaceList;
                }
                sxr_Free(interfaceTmpList);
            }
        }


        UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls: end of descriptor generation"
                    " nb interfaces %i\n", indexInterfaceList);

        // Set interfaces list
        g_UctlsDeviceDesc->configList[0]->interfaceList = interfaceList;
#ifdef _UVIDEO_
        //video iad
        if(g_UctlsId == UCTLS_ID_UVIDEO)
            g_UctlsDeviceDesc->configList[0]->iad = modes[g_UctlsId].servicesList[0].serviceCfg.videos.iad;
        else
#endif
#ifdef _URNDIS_
        //rndis iad
        if(g_UctlsId == UCTLS_ID_URNDIS)
            g_UctlsDeviceDesc->configList[0]->iad = modes[g_UctlsId].servicesList[0].serviceCfg.rndis.iad;
        else
#endif
            g_UctlsDeviceDesc->configList[0]->iad = NULL;

    }


    if(g_UctlsStatus == UCTLS_STATUS_CONNECTED ||
            g_UctlsStatus == UCTLS_STATUS_CHARGER ||
            g_UctlsStatus == UCTLS_STATUS_SPEED_ENUM)
    {
        hal_UsbReset(TRUE);
        sxr_Sleep(8000); // Wait 0.5s
        hal_UsbReset(FALSE);
    }
}

PUBLIC VOID uctls_SetMode(UCTLS_ID_T mode)
{
    g_UctlsId = mode;
}

PUBLIC UCTLS_ID_T uctls_GetMode(VOID)
{
    return(g_UctlsId);
}

PUBLIC  UCTLS_STATUS_T uctls_GetUsbStatus(VOID)
{
    return g_UctlsStatus;
}

PUBLIC BOOL uctls_IsMSSMode(VOID)
{
    return(g_UctlsId==UCTLS_ID_UMSS)?TRUE:FALSE;
}

#ifdef _UCDCACM_
PUBLIC VOID uctls_Modem_Open(VOID)
{
    uctls_ChangeMode(UCTLS_ID_UCDCACM);
    uctls_ChargerStatus(UCTLS_CHARGER_STATUS_CONNECTED);
}

PUBLIC VOID uctls_Modem_Close(VOID)
{
    uctls_ChargerStatus(UCTLS_CHARGER_STATUS_DISCONNECTED);
}

PUBLIC VOID uctls_TestUsbAtOpen(void)
{
    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls: test usb cdc \n");
    uctls_Modem_Open();

}
PUBLIC VOID uctls_TestUsbAtClose(void)
{
    UCTLS_TRACE(UCTLS_INFO_TRC, 0, "uctls: test usb cdc \n");
    uctls_Modem_Close();

}

BOOL usb_at_is_ready(void)
{
    if(hal_UsbGetControlFlag() == FALSE)
        return FALSE;

    if(uctls_GetUsbStatus() != UCTLS_STATUS_CONNECTED )
    {
        return FALSE;
    }
    if(uctls_GetMode() == UCTLS_ID_UCDCACM)
        return TRUE;
    return FALSE;

}
#endif

#ifdef _URNDIS_
PUBLIC BOOL uctls_rndis_is_used(void)
{
    if(hal_UsbGetControlFlag() == FALSE)
        return FALSE;

    if(uctls_GetUsbStatus() != UCTLS_STATUS_CONNECTED )
    {
        return FALSE;
    }
    if(uctls_GetMode() == UCTLS_ID_URNDIS)
        return TRUE;
    return FALSE;

}
#endif

