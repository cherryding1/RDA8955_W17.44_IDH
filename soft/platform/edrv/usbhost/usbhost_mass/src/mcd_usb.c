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


#include "cs_types.h"
#include "usbhostp_debug.h"
#include "hal_gpio.h"
#include "usb.h"
#include "mcd_usb.h"
#include "hal_sdmmc.h"
#include "hal_sys.h"
#include "stdlib.h"
#include "stdio.h"
#include "sxr_tls.h"
#include "pmd_m.h"
#include "sxs_io.h"
#include "sxr_sbx.h"
#include "usb_hcd.h"
#include "uctls_m.h"

#define MAX_USB_CLIENT      2

enum
{
    USB_HOST_SCH_INIT,
    USB_HOST_SCH_RESET,
    USB_HOST_SCH_GET_DEVICE_DES,
    USB_HOST_SCH_SET_ADDRESS,
    USB_HOST_SCH_GET_CONFIGURATION1,    /*get configuration descriptor itself */
    USB_HOST_SCH_GET_CONFIGURATION2,    /*get a whole configuration info*/
    USB_HOST_SCH_SET_CONFIGURATION,
    USB_HOST_SCH_INIT_END
};

typedef struct _USB_HOST_EVENT
{
    UINT32 nEventId;
    UINT32 nParam1;
    UINT32 nParam2;
    UINT32 nParam3;
} USB_HOST_EVENT;

PRIVATE USBHOST_STATUS_T      g_mcdUSBStatus = USBHOST_STATUS_NOTOPEN_PRESENT;
PRIVATE USBHOST_CARD_DETECT_HANDLER_T g_usbhostUSBDiskDetectHandler;

typedef struct tagUSBHOSTUSBHOST
{
    USBDEVICEDESC *pDeviceDesc;
    USBCONFIGURATIONDESC *pConfigDes;
    uint32  UsbSchOp;
    uint8   nClientNum;
    int         nCurrentDev;
    USBCLIENTDRIVER ClientDriver[MAX_USB_CLIENT];

} USBHOSTUSBHOST;

USBHOSTUSBHOST g_USBHOSTUsbInst;


extern UINT8     g_UctlsMbx_for_host;

//extern volatile int gUsbParam;
//void *gUsbData = NULL;
//uint8 gUsbStatus = 0;

UINT8 USBHOST_usbHostInitOpCompleteCb_status = 0;//added for delete nParam2
// this callback fuction runs in ISR
static void USBHOST_usbHostInitOpCompleteCb(void *pData, UINT8 status)
{

    USB_HOST_EVENT sEv = {0,};

    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USBHOST_usbHostInitOpCompleteCb");

    USBHOST_usbHostInitOpCompleteCb_status = status;

    if(g_UctlsMbx_for_host != 0xFF)
    {
        usb_HstSendEvent(0x22222222);

        sEv.nEventId = UCTLS_HEADER_ID;
        sEv.nParam1 = (UINT32)pData;
        sEv.nParam2 = status;
        sEv.nParam3 = 0;
        sxr_Send(&sEv, g_UctlsMbx_for_host, SXR_SEND_EVT);
    }

#if 0
    COS_EVENT ev;

    ev.nEventId = EV_FS_USB_INIT;
    ev.nParam1  = (uint32)pData;
    USBHOST_usbHostInitOpCompleteCb_status = status;
//  ev.nParam2  = status;//for delete nParam2
//  ev.nParam3  = 0;//for delete nParam3
    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);

    //gUsbParam  = 1;
    //gUsbData = pData;
    //gUsbStatus = status;

#endif



}

int RDA_UsbReset(void (*CompleteCb)(void* pData, uint8 status));

int USBHOST_USBHostSchOperation(void *pData, UINT8 status)
{
    static uint8 index = 0;
    static uint8 count = 0;
    int     handled = 1,ret=0;

    //pData = gUsbData;
    //status = gUsbStatus;

    usb_HstSendEvent(0x55555555);
    usb_HstSendEvent(status); usb_HstSendEvent(g_USBHOSTUsbInst.UsbSchOp); usb_HstSendEvent(count);
    if( status == 0)
    {
        count = 0;
        g_USBHOSTUsbInst.UsbSchOp++;
    }
    else
    {
        count ++;
        if( count < 3)
        {
            // let's try again...

            if( g_USBHOSTUsbInst.UsbSchOp > USB_HOST_SCH_GET_DEVICE_DES)
            {
                //g_USBHOSTUsbInst.UsbSchOp = USB_HOST_SCH_RESET;
                //RDA_UsbReset(USBHOST_usbHostInitOpCompleteCb);
                return 0;
            }
            else
            {
                RDA_UsbReset(NULL);
            }
        }
        else
        {
            return 0;
        }
    }
    switch( g_USBHOSTUsbInst.UsbSchOp)
    {
        case USB_HOST_SCH_RESET:
            usb_HstSendEvent(0x09170010);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_RESET");
            RDA_UsbReset(USBHOST_usbHostInitOpCompleteCb);
            break;
        case USB_HOST_SCH_GET_DEVICE_DES:
            usb_HstSendEvent(0x09170011);

            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_GET_DEVICE_DES");
            USB_GetDescriptor(USB_GET_DESCRIPTOR, USB_DES_DEVICE<<8, 0, USBHOST_usbHostInitOpCompleteCb);
            break;
        case USB_HOST_SCH_SET_ADDRESS:
            usb_HstSendEvent(0x09170012);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_SET_ADDRESS");
            if( pData != NULL)
            {
                //g_USBHOSTUsbInst.pDeviceDesc = (USBDEVICEDESC*)pData;
                index = 0;
            }
            USB_SetAddress(USBHOST_usbHostInitOpCompleteCb);
            break;
        case USB_HOST_SCH_GET_CONFIGURATION1:
        case USB_HOST_SCH_GET_CONFIGURATION2:
            //usb_HstSendEvent(0x09170013);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_GET_CONFIGURATION index =%d ",index);
            USB_GetDescriptor(USB_GET_DESCRIPTOR, USB_DES_CONFIGURATION<<8|index, 0, USBHOST_usbHostInitOpCompleteCb);
            break;
        case USB_HOST_SCH_SET_CONFIGURATION:
        {
            //usb_HstSendEvent(0x09170014);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_SET_CONFIGURATION pData =0x%x",pData);
            //usb_HstSendEvent(pData);
            if( pData != NULL)
            {
                USBCONFIGURATIONDESC * pConfig  = (USBCONFIGURATIONDESC*)pData;
                uint8 * ptemp = (uint8*)pData;
                USBINTERFACEDESC *pInterface = (USBINTERFACEDESC*)(ptemp+sizeof(USBCONFIGURATIONDESC));
                int j = 0;

                index = pConfig->bConfigurationValue;

                USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_SET_CONFIGURATION index =0x%x ,g_USBHOSTUsbInst.nClientNum =%d ,pInterface=0x%x",index,g_USBHOSTUsbInst.nClientNum,pInterface);
                for(j = 0; j<g_USBHOSTUsbInst.nClientNum; j++)
                {
                    if( pInterface->bInterfaceClass == g_USBHOSTUsbInst.ClientDriver[j].InterfaceClass
                            && pInterface->bInterfaceSubClass == g_USBHOSTUsbInst.ClientDriver[j].InterfaceSubClass)
                    {
                        g_USBHOSTUsbInst.nCurrentDev = j;
                        g_USBHOSTUsbInst.pConfigDes = pConfig;

                        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_SET_CONFIGURATION pInterface->bInterfaceClass =%d ,pInterface->bInterfaceSubClass =%d",pInterface->bInterfaceClass,pInterface->bInterfaceSubClass);
                        ret = USB_SetConfiguration(index, USBHOST_usbHostInitOpCompleteCb);
                        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_SET_CONFIGURATION ret =%d  ",ret);
                        break;
                    }
                }
            }
            else
            {
                /* try again? */
            }
        }
        break;
        case USB_HOST_SCH_INIT_END:
        {
            usb_HstSendEvent(0x08090003); USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_INIT_END");
            if(g_USBHOSTUsbInst.ClientDriver[g_USBHOSTUsbInst.nCurrentDev].UsbDeviceAttach)
            {

                USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_HOST_SCH_INIT_END - UsbDeviceAttach"); usb_HstSendEvent(0x080900a1);
                g_USBHOSTUsbInst.ClientDriver[g_USBHOSTUsbInst.nCurrentDev].UsbDeviceAttach(g_USBHOSTUsbInst.pConfigDes, index);
            }
        }
        break;
        default:
            handled = 0;
            break;
    }
    //usb_HstSendEvent(0x08260015);
    return handled;
}

int USBHOST_USBHostInit()
{
    USBOPERATION *pOps = USB_HcdInit();

    g_USBHOSTUsbInst.UsbSchOp = USB_HOST_SCH_INIT;
    g_USBHOSTUsbInst.nCurrentDev = -1;

    if( pOps != NULL)
    {
        usb_HstSendEvent(0x08150004);
        RDA_UsbHostInit(pOps, USBHOST_usbHostInitOpCompleteCb);
    }
}

int USBHOST_UsbHostDeInit(uint8 port)
{
    USB_HcdDeinit();

    //since that we only register client driver when power on, don't delete driver info
    if( g_USBHOSTUsbInst.nCurrentDev < 0)
    {
        return 0;
    }

    if( g_USBHOSTUsbInst.ClientDriver[g_USBHOSTUsbInst.nCurrentDev].UsbDeviceDetach)
    {
        g_USBHOSTUsbInst.ClientDriver[g_USBHOSTUsbInst.nCurrentDev].UsbDeviceDetach(NULL);
    }
    g_USBHOSTUsbInst.nCurrentDev = -1;
    usb_HstSendEvent(0x09260002);

    pmd_EnablePower(PMD_POWER_USB, FALSE);
    return 0;
}

int USBHOST_RegisterClientDriver(USBCLIENTDRIVER *pdriverInst)
{
    if( g_USBHOSTUsbInst.nClientNum >= MAX_USB_CLIENT  || pdriverInst == NULL)
    {
        return USBHOSTUSB_FAILED;
    }

    g_USBHOSTUsbInst.ClientDriver[g_USBHOSTUsbInst.nClientNum++] = *pdriverInst;

    return USBHOSTUSB_SUCCESS;
}


/// Update g_USBHOSTStatus
/// @return TRUE is card present (only exact when GPIO is used for card detect.)
PRIVATE BOOL USBHOST_USBDetectUpdateStatus(VOID)
{
    return TRUE;
}

PRIVATE VOID USBHOST_USBDiskDetectHandler(VOID)
{
    BOOL CardPresent = USBHOST_USBDetectUpdateStatus();

    g_USBHOSTUSBDiskDetectHandler(CardPresent);
}

// =============================================================================
// USBHOST_SetCardDetectHandler
// -----------------------------------------------------------------------------
/// Register a handler for card detection
///
/// @param handler function called when insertion/removal is detected.
// =============================================================================
PUBLIC USBHOST_ERR_T USBHOST_SetUSBDiskDetectHandler(USBHOST_CARD_DETECT_HANDLER_T handler)
{
    /*    if(NULL == g_USBHOSTConfig)
        {
            g_USBHOSTConfig = tgt_GetUSBHOSTConfig();
        }

        if(g_USBHOSTConfig->cardDetectGpio == HAL_GPIO_NONE)
        {
            return USBHOST_ERR_NO_HOTPLUG;
        }

        if(NULL != handler)*/
    {
        HAL_GPIO_CFG_T cfg  =
        {
            .direction      = HAL_GPIO_DIRECTION_INPUT,
            .irqMask        =
            {
                .rising     = TRUE,
                .falling    = TRUE,
                .debounce   = TRUE,
                .level      = FALSE
            },
            .irqHandler     = USBHOST_USBDiskDetectHandler
        };

        hal_GpioOpen(HAL_GPIO_6, &cfg);
        g_usbhostUSBDiskDetectHandler = handler;
    }
    /*    else
        {
            hal_GpioClose(g_USBHOSTConfig->cardDetectGpio);
            g_USBHOSTCardDetectHandler = NULL;
        }*/

    return USBHOST_ERR_NO;
}

extern void  OpenUSBDisk_test( void);

extern VOID hal_ConfigUsb_Host(BOOL enable);
extern VOID hal_Usb_host_Open(void);

void test_8809phost(void)
{
    usb_HstSendEvent(0xfffffff8);

    OpenUSBDisk_test();
    hal_Usb_host_Open();

    USBHOST_USBHostInit();
    usb_HstSendEvent(0xfffffff9);
}

void test_8809_test_plug_out_usb(void)
{

    usb_HstSendEvent(0xfffffff1);
    USBHOST_UsbHostDeInit(0);

//hal_UsbReset(TRUE);
//hal_UsbClose();


}

