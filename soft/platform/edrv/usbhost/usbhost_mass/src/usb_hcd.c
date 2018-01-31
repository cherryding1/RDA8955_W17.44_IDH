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
#include "usb.h"
#include "usbhostp_debug.h"
#include "sxr_mem.h"
#include "hal_usb.h"
#include "hal_debug.h"
#include "global_macros.h"
#include "usb_hcd.h"
//  HCD  HostControlDriver
static USBHCD gHcdInst;
static volatile int VendorCmdFinish = 0;
static volatile int BulkTransFinish = 0;
void VendorCmdCallBack(void* pData, uint8 status)
{
    VendorCmdFinish = 1;

    /*
    COS_EVENT ev;

    ev.nEventId = EV_FS_USB_INIT;
    ev.nParam1  = (uint32)pData;
    ev.nParam2  = status;
    ev.nParam3  = 0;
    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    */
}

void BulkTransferCompleteCallBack(HANDLE handle, uint8 status)
{
    BulkTransFinish = 1;
    /*
    COS_EVENT ev;

    ev.nEventId = EV_FS_USB_TRANSFERRED;
    ev.nParam1  = handle;
    ev.nParam2  = status;
    ev.nParam3  = 0;
    COS_SendEvent(MOD_SYS, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    */
}

USBOPERATION *USB_HcdInit()
{
    VendorCmdFinish = 0;
    BulkTransFinish = 0;
    memset(&gHcdInst, 0, sizeof(USBHCD));
    return &gHcdInst.usbops;
}

int USB_HcdDeinit()
{
    int i = 0;
    u32 temp;
    temp = gHcdInst.pdeviceDesc;
    temp = MEM_ACCESS_CACHED(temp);
    if( gHcdInst.pdeviceDesc != NULL )
    {
        sxr_Free(temp);
    }

    temp = gHcdInst.pconfig;
    temp = MEM_ACCESS_CACHED(temp);

    if( gHcdInst.pconfig != NULL)
    {
        sxr_Free(temp);
    }

    for(; i<3; i++)
    {

        if( gHcdInst.pConfigDesc[i]!= NULL )
        {

            temp =gHcdInst.pConfigDesc[i];
            temp = MEM_ACCESS_CACHED(temp);
            sxr_Free(temp);
        }
    }
    VendorCmdFinish = 1;
    BulkTransFinish = 1;
    gHcdInst.flag = USB_HCD_DEVICE_DETACHED;
    return 0;
}

int USB_SetAddress(void (*Cb)(void*pData, uint8 status))
{
    USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
    memset(pReq, 0, sizeof(USBCONTROLREQUEST));

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return USBHCD_DEVICE_DISCONNECTED;
    }

    gHcdInst.address ++;
    if( gHcdInst.address == 0)
    {
        gHcdInst.address  = 1;
    }

    pReq->completeCb =  Cb;
    pReq->stdRequest.bRequest = USB_SET_ADDRESS;
    pReq->stdRequest.wValue = gHcdInst.address;

    if(gHcdInst.usbops.ControlTransfer)
    {
        return gHcdInst.usbops.ControlTransfer(pReq);
    }
    else
    {
        return  -1;
    }
}

int USB_SetConfiguration(uint8 bValue, void (*Cb)(void*pData, uint8 status))
{
    USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
    memset(pReq, 0, sizeof(USBCONTROLREQUEST));

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return -1;
    }

    pReq->completeCb =  Cb;
    pReq->stdRequest.wValue = bValue;
    pReq->stdRequest.bRequest = USB_SET_CONFIGURATION;

    if( gHcdInst.usbops.ControlTransfer)
    {
        return gHcdInst.usbops.ControlTransfer( pReq );
    }
    else
    {
        return -1;
    }
}
u8 tetet = 0;
int USB_GetDescriptor(uint8 type, uint16 wValue, uint16 wIndex, void (*Cb)(void*pData, uint8 status))
{
    uint8 desType = wValue>>8;
    uint8 desIndex = wValue;
    USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
    memset(pReq, 0, sizeof(USBCONTROLREQUEST));

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return USBHCD_DEVICE_DISCONNECTED;
    }
    hal_HstSendEvent(0x77770001); hal_HstSendEvent(wValue);
    pReq->stdRequest.bmRequestType = 0x80;  //USB_END0_DIRIN
    pReq->stdRequest.bRequest = USB_GET_DESCRIPTOR;
    pReq->stdRequest.wValue = wValue;

    if( desType == USB_DES_DEVICE )
    {
        hal_HstSendEvent(0x77770002);
        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_GetDescriptor USB_DES_DEVICE");
        gHcdInst.pdeviceDesc = (USBDEVICEDESC*)(MEM_ACCESS_UNCACHED(sxr_Malloc(2*sizeof(USBDEVICEDESC))));

        //gHcdInst.pdeviceDesc = (USBDEVICEDESC*)((sxr_Malloc(2*sizeof(USBDEVICEDESC))));
        memset(gHcdInst.pdeviceDesc, 0xaa, 2*sizeof(USBDEVICEDESC)-1);
        pReq->nBufLen = pReq->stdRequest.wLength = sizeof(USBDEVICEDESC);
        pReq->pBuf = MEM_ACCESS_UNCACHED((uint8*)gHcdInst.pdeviceDesc);

        //hal_DbgPageProtectSetup(HAL_DBG_PAGE_13, HAL_DBG_PAGE_WRITE_TRIGGER,(u32)gHcdInst.pdeviceDesc+sizeof(USBDEVICEDESC),(u32)gHcdInst.pdeviceDesc+sizeof(USBDEVICEDESC)+15);
        //hal_DbgPageProtectEnable(HAL_DBG_PAGE_13);
        hal_HstSendEvent(0x44444441); hal_HstSendEvent(gHcdInst.pdeviceDesc); hal_HstSendEvent(pReq->pBuf); hal_HstSendEvent(sizeof(USBDEVICEDESC));
    }
    else if( desType == USB_DES_CONFIGURATION )
    {
        //the first time we read a specific configuration
        hal_HstSendEvent(0x77770003);
        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_GetDescriptor USB_DES_CONFIGURATION");
        if( gHcdInst.pconfig == NULL)
        {
            //gHcdInst.pconfig = (USBCONFIGURATIONDESC*)sxr_Malloc(2*(sizeof(USBCONFIGURATIONDESC)));
            gHcdInst.pconfig = (USBCONFIGURATIONDESC*)(MEM_ACCESS_UNCACHED(sxr_Malloc(2*(sizeof(USBCONFIGURATIONDESC)))));
            //gHcdInst.pconfig = (USBCONFIGURATIONDESC*)((sxr_Malloc(2*(sizeof(USBCONFIGURATIONDESC)))));
            memset(gHcdInst.pconfig, 0, 1*(sizeof(USBCONFIGURATIONDESC)));//memset((u32)(gHcdInst.pconfig)+(sizeof(USBCONFIGURATIONDESC)), 0x5a, 1*(sizeof(USBCONFIGURATIONDESC))-2);
            //hal_DbgPageProtectSetup(HAL_DBG_PAGE_15, HAL_DBG_PAGE_WRITE_TRIGGER,(UINT32)gHcdInst.pconfig+(sizeof(USBCONFIGURATIONDESC)),(UINT32)( (UINT32)gHcdInst.pconfig +2*(sizeof(USBCONFIGURATIONDESC))-1));
            //hal_DbgPageProtectEnable(HAL_DBG_PAGE_15);
            hal_HstSendEvent(0x44444444); hal_HstSendEvent(gHcdInst.pconfig); hal_HstSendEvent(sizeof(USBCONFIGURATIONDESC)); hal_HstSendEvent(pReq->nBufLen);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_GetDescriptor gHcdInst.pconfig =0x%x ,size = %d",gHcdInst.pconfig,sizeof(USBCONFIGURATIONDESC));


            //hal_DbgAssert( "hal_UsbOpen now in host modes");
        }
        tetet++;


        hal_HstSendEvent(0x4444443a); hal_HstSendEvent(gHcdInst.pconfig); hal_HstSendEvent(gHcdInst.pconfig->wTotalLength); hal_HstSendEvent(tetet);
        if ((tetet == 2)&&(0==gHcdInst.pconfig->wTotalLength))
        {
            hal_DbgAssert( "wTotalLength is 0 ");
        }
        if(gHcdInst.pconfig->wTotalLength == 0)
        {


            pReq->nBufLen =     pReq->stdRequest.wLength = sizeof(USBCONFIGURATIONDESC);  // ytt
            pReq->pBuf = MEM_ACCESS_UNCACHED((uint8*)gHcdInst.pconfig);
            hal_HstSendEvent(0x09060003); hal_HstSendEvent(pReq->stdRequest.wLength); hal_HstSendEvent(pReq->pBuf);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_GetDescriptor pReq->stdRequest.wLength  = %d",pReq->stdRequest.wLength );
        }
        else
        {

            hal_HstSendEvent(0x09060004); hal_HstSendEvent(gHcdInst.pconfig->wTotalLength);
            gHcdInst.pConfigDesc[desIndex]=(MEM_ACCESS_UNCACHED(sxr_Malloc(16+ gHcdInst.pconfig->wTotalLength)));
            //gHcdInst.pConfigDesc[desIndex]=((sxr_Malloc(16+ gHcdInst.pconfig->wTotalLength)));
            memset(gHcdInst.pConfigDesc[desIndex],0, 16+gHcdInst.pconfig->wTotalLength);


            hal_DbgPageProtectSetup(HAL_DBG_PAGE_15, HAL_DBG_PAGE_WRITE_TRIGGER,gHcdInst.pConfigDesc[desIndex]+gHcdInst.pconfig->wTotalLength,(UINT32)( (UINT32)gHcdInst.pconfig +gHcdInst.pconfig->wTotalLength+15));
            hal_DbgPageProtectEnable(HAL_DBG_PAGE_15);


            hal_HstSendEvent(0x44444442); hal_HstSendEvent(gHcdInst.pConfigDesc[desIndex]); hal_HstSendEvent(gHcdInst.pconfig->wTotalLength);
            USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: USB_GetDescriptor else  gHcdInst.pconfig->wTotalLength =%d desIndex=%d  gHcdInst.pConfigDesc[desIndex]=0x%x",gHcdInst.pconfig->wTotalLength,desIndex, gHcdInst.pConfigDesc[desIndex]);
            if( gHcdInst.pConfigDesc[desIndex] == NULL)
            {
                return USBHCD_NO_RESOURCE;
            }

            pReq->nBufLen =               pReq->stdRequest.wLength = gHcdInst.pconfig->wTotalLength;//ytt
            pReq->pBuf = MEM_ACCESS_UNCACHED((uint8*)gHcdInst.pConfigDesc[desIndex]);

            //memset(g_pHcdInst->pconfig, 0, sizeof(USBCONFIGURATIONDESC));
        }
    }
    else
    {
        return USBHCD_INVALID_PARAM;
    }
    hal_HstSendEvent(0x0906bbb4); hal_HstSendEvent(wIndex);
    pReq->stdRequest.wIndex = wIndex;
    pReq->completeCb =  Cb;  // mcd_usbHostInitOpCompleteCb  send a message

    if(gHcdInst.usbops.ControlTransfer)
    {
        return  gHcdInst.usbops.ControlTransfer(pReq);   // RDAUsb_ControlTransfer
    }
    else
    {
        return USBHCD_ERROR_FAILED;
    }

}

int USB_SetInterface(uint16 bAlternateSetting, uint16 Interface, void (*Cb)(void*pData, uint8 status))
{
    USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
    memset(pReq, 0, sizeof(USBCONTROLREQUEST));

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return USBHCD_DEVICE_DISCONNECTED;
    }

    pReq->stdRequest.bmRequestType = 0x01;  /*dest: interface */
    pReq->stdRequest.bRequest = USB_SET_INTERFACE;
    pReq->stdRequest.wIndex = Interface;
    pReq->stdRequest.wValue = bAlternateSetting;

    pReq->completeCb = Cb;

    if(gHcdInst.usbops.ControlTransfer)
    {
        return  gHcdInst.usbops.ControlTransfer(pReq);
    }
    else
    {
        return USBHCD_ERROR_FAILED;
    }

}

USB_PIPE USB_OpenPipe(USBENDPOINTDESC *pEndDes)
{
    if(gHcdInst.usbops.OpenPipe)
    {
        return gHcdInst.usbops.OpenPipe(pEndDes);
    }
    return NULL;
}

int USB_VendorTransfer(USBSTDREQUEST *pStdRequest, void *pBuf, uint32* pnDataTransferred, uint32 timeout)
{
    USBCONTROLREQUEST  *pReq = &gHcdInst.ctrlReq;
    memset(pReq, 0, sizeof(USBCONTROLREQUEST));

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return USBHCD_DEVICE_DISCONNECTED;
    }
    hal_HstSendEvent(0x6666aaa1);
    pReq->stdRequest = *pStdRequest;
    pReq->completeCb = VendorCmdCallBack;
    pReq->pBuf = pBuf;
    pReq->nBufLen = pStdRequest->wLength;

    if(gHcdInst.usbops.ControlTransfer)
    {
        if( gHcdInst.usbops.ControlTransfer(pReq) != USB_NO_ERROR )
        {
            return USBHCD_ERROR_FAILED;
        }
    }
    else
    {
        return USBHCD_ERROR_FAILED;
    }

    while(1) //timeout
    {
        timeout--;
        //sxr_Sleep(1 MS_WAITING);
        if( VendorCmdFinish > 0)
            break;
    }
    VendorCmdFinish = 0;
    return 0;
}

#if 0 // dma maybe has size/4  bug  

u8 dma_buffer[1024];
int USB_BulkOrInterruptTransfer(USB_PIPE pipe, void* pBuf, uint32 BufLen, uint32* pnBytesTransferred, uint32 timeout)
{
    uint32 status = 0;
    u8 * dma_bf;
    dma_bf = MEM_ACCESS_UNCACHED(dma_buffer);
    USBBULKREQUEST *pReq = &gHcdInst.bulkReq;
    //hal_HstSendEvent(0x22dd5555);
    memset(pReq, 0, sizeof(USBBULKREQUEST));
    memcpy(dma_buffer,pBuf,BufLen);

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return USBHCD_DEVICE_DISCONNECTED;
    }

    pReq->nBufLen = BufLen;
    pReq->pBuf =dma_bf;// pBuf;
    pReq->pipe = pipe;
    pReq->pnByteTransferred = pnBytesTransferred;
    pReq->completeCb = BulkTransferCompleteCallBack;
    pReq->pstatus = &status;
    //pReq->handle = COS_CreateSemaphore(1);

    if( gHcdInst.usbops.BulkTransfer )
    {
        if( gHcdInst.usbops.BulkTransfer(pReq) != USB_NO_ERROR )   //  RDAUsb_BulkTransfer
        {
            return USBHCD_ERROR_FAILED;
        }
    }
    else
    {
        return USBHCD_ERROR_FAILED;
    }

WAIT_FRAME_PERIOD:
    while(1)    //timeout
    {
        timeout --;
        if( BulkTransFinish > 0)
        {
            break;
        }
    }
    //hal_HstSendEvent(0x22555555);hal_HstSendEvent(pReq->nBufLen);

    if(pReq->nBufLen>512)
        hal_DbgAssert( "USB_BulkOrInterruptTransfer pReq->nBufLen = 512 ");

    hal_SysInvalidateCache(pReq->pBuf,pReq->nBufLen);
    memcpy(pBuf,pReq->pBuf,pReq->nBufLen);
    BulkTransFinish = 0;

    if( status == USB_STALL_ERROR)
    {
        //clear feature
        return USB_STALL_ERROR;
    }
    else if(status == USB_RECEIVED_NAK)
    {
        int RDAUsb_PacketTransfer(int chn);
        RDAUsb_PacketTransfer(pipe);
        // try to send again;
        goto WAIT_FRAME_PERIOD;
    }
    else if( status == USBHCD_DEVICE_DISCONNECTED )
    {
        hal_HstSendEvent(0x88888888);
        gHcdInst.flag = USB_HCD_DEVICE_DETACHED;
        return USBHCD_DEVICE_DISCONNECTED;
    }

    return USB_NO_ERROR;

}

#else

u8 cou_dma=0;
int USB_BulkOrInterruptTransfer(USB_PIPE pipe, void* pBuf, uint32 BufLen, uint32* pnBytesTransferred, uint32 timeout)
{
    uint32 status = 0;
    USBBULKREQUEST *pReq = &gHcdInst.bulkReq;
    //hal_HstSendEvent(0x22dd5555);
    memset(pReq, 0, sizeof(USBBULKREQUEST));

    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
    {
        return USBHCD_DEVICE_DISCONNECTED;
    }
#if 0
    hal_HstSendEvent(0x22555558); hal_HstSendEvent(BufLen);
    if(BufLen == 0x1f)
    {
        hal_HstSendEvent(0x22333338);
        //cou_dma++;
        //if(cou_dma ==18)  hal_DbgAssert( "USB_BulkOrInterruptTransfers");
        for(u8 kk = 0; kk<0x1f; kk++) hal_HstSendEvent(((u8*)pBuf)[kk]);
    }
#endif
    pReq->nBufLen = BufLen;
    pReq->pBuf = pBuf;
    pReq->pipe = pipe;
    pReq->pnByteTransferred = pnBytesTransferred;
    pReq->completeCb = BulkTransferCompleteCallBack;
    pReq->pstatus = &status;
    //pReq->handle = COS_CreateSemaphore(1);
    hal_HstSendEvent(0x22555aaa);

    if( gHcdInst.usbops.BulkTransfer )
    {
        if( gHcdInst.usbops.BulkTransfer(pReq) != USB_NO_ERROR )   //  RDAUsb_BulkTransfer
        {
            return USBHCD_ERROR_FAILED;
        }
    }
    else
    {
        return USBHCD_ERROR_FAILED;
    }

WAIT_FRAME_PERIOD:

    while(1)    //timeout
    {
        timeout --;
        if( BulkTransFinish > 0)
        {
            break;
        }
    }


    hal_HstSendEvent(0x22555555); hal_HstSendEvent(pReq->nBufLen); hal_HstSendEvent(status); hal_HstSendEvent(&status);

    if(pReq->nBufLen>512)
        hal_DbgAssert( "USB_BulkOrInterruptTransfer pReq->nBufLen = 512 ");

    hal_SysInvalidateCache(pReq->pBuf,pReq->nBufLen);
    BulkTransFinish = 0;
    // *pEnd->pBulkReq->pstatus = USB_STALL_ERROR;
    if( status == USB_STALL_ERROR)
    {
        //clear feature
        hal_HstSendEvent(0x8888aaa3);
        return USB_STALL_ERROR;
    }
    else if(status == USB_RECEIVED_NAK)
    {
        hal_HstSendEvent(0x8888aaa2);
        int RDAUsb_PacketTransfer(int chn);
        RDAUsb_PacketTransfer(pipe);
        // try to send again;
        goto WAIT_FRAME_PERIOD;
    }
    else if( status == USBHCD_DEVICE_DISCONNECTED )
    {
        hal_HstSendEvent(0x8888aaa1);
        gHcdInst.flag = USB_HCD_DEVICE_DETACHED;
        return USBHCD_DEVICE_DISCONNECTED;
    }

    return USB_NO_ERROR;

}

#endif
int USB_ResetDefaultEndpoint()
{
    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
        return USBHCD_DEVICE_DISCONNECTED;
    else
        return 0;
}

int USB_ResetEndpoint(void *pipe, uint32 timeout)
{
    if( gHcdInst.flag & USB_HCD_DEVICE_DETACHED)
        return USBHCD_DEVICE_DISCONNECTED;
    else
        return 0;
}




