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
#include "chip_id.h"
#include "sxr_tls.h"
#include "sxr_ops.h"

#include "usb.h"
#include "usb_hcd.h"
#include "usbmscp.h"

#include "bot.h"
#include "cbit.h"
#include "usbhostp_debug.h"




#define USB_ENDPOINT_DIRECTION_OUT(address) (!(address&0x80))
#define USB_ENDPOINT_DIRECTION_IN(address)  (address&0x80)

#define USB_ENDPOINT_TYPE_MASK      0x03
#define USB_ENDPOINT_TYPE_BULK      0x02
#define USB_ENDPOINT_TYPE_INTERRUPT 0x03


//*****************************************************************************
//
// F U N C T I O N    P R O T O T Y P E S
//
//*****************************************************************************
UINT32 g_NumDevices = 0;
volatile INT UsbCmdFinished = 0;

static USBMSC_DEVICE gUsbmscInst;

USBINTERFACEDESC * ParseUsbDescriptors(VOID * configuration, UINT8 *pnInterfaceNum);

UINT32 GetMaxLUN(HANDLE hTransport, UINT8 *pLun);

BOOL SetUsbInterface(PUSBMSC_DEVICE pUsbDevice, UINT8 nInterfaceNum);

VOID UsbControlComplete(VOID *pData, UINT8 status)
{
    UsbCmdFinished = 1;
}

HANDLE UsbDiskTransportInit(VOID * configuration, UINT8 ulConfigIndex, UINT8 *pLun)
{
    BOOL bRc;
    PUSBMSC_DEVICE      pUsbDevice = NULL;
    USBINTERFACEDESC *pUsbInterface = NULL;
    UINT8   bInterfaceSubClass;
    UINT8   bTempInterfaceSubClass = 0xFF; // invalid subclass
    UINT8   uMaxLun = 0;
    UINT32      uiIndex; // Working Unit.
    UINT8   uNumInterface = 0;

    do
    {
        // Parse USB Descriptors
        pUsbInterface = ParseUsbDescriptors(configuration, &uNumInterface);

        usb_HstSendEvent(0xa8290001);
        usb_HstSendEvent(pUsbInterface);
        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: UsbDiskTransportInit  pUsbInterface =0x%x ",pUsbInterface);
        if ( !pUsbInterface )
        {
            bRc = FALSE;
            break;
        }

        // we found a device, interface, & protocol we can control, so create our device context
        pUsbDevice = &gUsbmscInst;      //(PUSBMSC_DEVICE)sxr_Malloc( sizeof(USBMSC_DEVICE) );
        /*
        if ( !pUsbDevice )
        {
            bRc = FALSE;
            break;
        }
        */
        memset(pUsbDevice, 0, sizeof(USBMSC_DEVICE));

        pUsbDevice->Sig = USBMSC_SIG;

        pUsbDevice->pUsbInterface = pUsbInterface;
        pUsbDevice->ConfigIndex   = (uint16)ulConfigIndex;

        pUsbDevice->Flags.AcceptIo      = TRUE;
        pUsbDevice->Flags.DeviceRemoved = FALSE;

        pUsbDevice->dwMaxLun=0;

        bRc = SetUsbInterface(pUsbDevice, uNumInterface);

        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: UsbDiskTransportInit  SetUsbInterface");
        if( bRc == FALSE)
        {
            return bRc;
        }

        // use defaults
        pUsbDevice->Timeouts.Reset         = RESET_TIMEOUT;
        pUsbDevice->Timeouts.CommandBlock  = COMMAND_BLOCK_TIMEOUT;
        pUsbDevice->Timeouts.CommandStatus = COMMAND_STATUS_TIMEOUT;


        if (!pUsbDevice->Timeouts.Reset)
            pUsbDevice->Timeouts.Reset = RESET_TIMEOUT;

        if (!pUsbDevice->Timeouts.Reset)
            pUsbDevice->Timeouts.CommandBlock = COMMAND_BLOCK_TIMEOUT;

        if (!pUsbDevice->Timeouts.Reset)
            pUsbDevice->Timeouts.CommandStatus = COMMAND_STATUS_TIMEOUT;

        //
        // Load the USB Disk Driver based on the bInterfaceSubClass code.
        // The USB Disk Driver is named by convention USBDISKxx.DLL,
        // where 'xx' is a valid bInterfaceSubClass code.
        //
        // To override the default disk driver stuff the replacement driver subkey in the registry.
        // If the named subkey does not exist then retry with SCSI as the default driver.
        //
        bInterfaceSubClass = pUsbDevice->pUsbInterface->bInterfaceSubClass;
        //ASSERT( (bInterfaceSubClass >= USBMSC_SUBCLASS_RBC) && (bInterfaceSubClass <= USBMSC_SUBCLASS_SCSI) );
        USBHOST_ASSERT( (bInterfaceSubClass >= USBMSC_SUBCLASS_RBC) && (bInterfaceSubClass <= USBMSC_SUBCLASS_SCSI), "scsi2 :  (bInterfaceSubClass >= USBMSC_SUBCLASS_RBC) && (bInterfaceSubClass <= USBMSC_SUBCLASS_SCSI)");


        // signal we can take I/O
        pUsbDevice->Flags.AcceptIo = TRUE;

        // Call the driver's DiskAttach.
        // DiskAttach returns non-null Context on success, else null.
        for (uiIndex=0; uiIndex<MAX_LUN; uiIndex++)
        {
            pUsbDevice->DiskContext[uiIndex]=NULL;
        }

        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: UsbDiskTransportInit  GetMaxLUN");
        // Get Max LUN.
        if (GetMaxLUN(pUsbDevice, &uMaxLun)!=ERROR_SUCCESS)
        {
            uMaxLun=1; // Using 1 as default;
        }

        pUsbDevice->dwMaxLun= uMaxLun;
        if( pLun )
        {
            pLun = uMaxLun;
        }

        //ASSERT(pUsbDevice->dwMaxLun>=1);
        USBHOST_ASSERT( (pUsbDevice->dwMaxLun>=1)  , "scsi2 :   (pUsbDevice->dwMaxLun>=1)");


    }
    while(0);

    if (!bRc)
    {
        // If not our device, or error, then clean up
        RemoveDeviceContext( pUsbDevice );
        return NULL;
    }

    return (HANDLE)pUsbDevice;

}

VOID UsbDiskTransportDeinit(HANDLE hTransport)
{
    if(hTransport)
    {
        PUSBMSC_DEVICE pUsbDevice = (PUSBMSC_DEVICE)hTransport;
        RemoveDeviceContext(pUsbDevice);
        pUsbDevice->Flags.AcceptIo = 0;
        pUsbDevice->Flags.DeviceRemoved = 1;
    }
}

#if 0 // ytt
USBINTERFACEDESC *ParseUsbDescriptors(VOID *configuration, UINT8 *pnInterfaceNum)
{
    UINT32 dwNumInterfaces;
    UINT32 dwIndex;
    USBINTERFACEDESC *pUsbInterface, *pDesiredInterface = NULL;
    USBCONFIGURATIONDESC *pConfigDes = (USBCONFIGURATIONDESC*)configuration;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ParseUsbDescriptors ");

    if (!configuration)
    {

        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ParseUsbDescriptors no config!! ");
        return NULL;
    }

    pUsbInterface = (USBINTERFACEDESC*)(pConfigDes+1);
    *pnInterfaceNum = dwNumInterfaces = pConfigDes->bNumInterfaces;

    // walk the interfaces searching for best fit
    for ( dwIndex = 0; dwIndex < dwNumInterfaces; pUsbInterface++, dwIndex++)
    {
        if (  pUsbInterface->bInterfaceClass == USBMSC_INTERFACE_CLASS &&
                (pUsbInterface->bInterfaceSubClass >= USBMSC_SUBCLASS_RBC && pUsbInterface->bInterfaceSubClass <= USBMSC_SUBCLASS_SCSI) &&
                (pUsbInterface->bInterfaceProtocol == USBMSC_INTERFACE_PROTOCOL_CBIT || pUsbInterface->bInterfaceProtocol == USBMSC_INTERFACE_PROTOCOL_CBT || pUsbInterface->bInterfaceProtocol == USBMSC_INTERFACE_PROTOCOL_BOT) )
        {
            // if we do not already have an interface, or the selected Protocol is not Bulk-Only
            // (I personally prefer Bulk-Only since it is well defined)
            if ( !pDesiredInterface || pDesiredInterface->bInterfaceProtocol != USBMSC_INTERFACE_PROTOCOL_BOT)
            {
                pDesiredInterface = pUsbInterface;
            }
        }
    }
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ParseUsbDescriptors end pDesiredInterface =%d ",pDesiredInterface);

    return pDesiredInterface;

}
#else

USBINTERFACEDESC *ParseUsbDescriptors(VOID *configuration, UINT8 *pnInterfaceNum)
{
    UINT32 dwNumInterfaces;
    UINT32 dwIndex;
    USBINTERFACEDESC *pUsbInterface, *pDesiredInterface = NULL;
    USBCONFIGURATIONDESC *pConfigDes = (USBCONFIGURATIONDESC*)configuration;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ParseUsbDescriptors ,pConfigDes =0x%x",pConfigDes);

    if (!configuration)
    {

        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ParseUsbDescriptors no config!! ");
        return NULL;
    }

    *pnInterfaceNum = dwNumInterfaces = pConfigDes->bNumInterfaces;

    //pUsbInterface = (USBINTERFACEDESC*)(pConfigDes+1); 0x820d9c91
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ParseUsbDescriptors ,pUsbInterface =0x%x",pUsbInterface);
    //return pUsbInterface = (USBINTERFACEDESC*)(0x820d9c91);
    return pUsbInterface = (USBINTERFACEDESC*)(pConfigDes+1);

    //return pUsbInterface = (USBINTERFACEDESC*)((UINT8*)pConfigDes+sizeof(USBCONFIGURATIONDESC));

}

#endif


BOOL SetUsbInterface(PUSBMSC_DEVICE pUsbDevice, UINT8 nInterfaceNum)
{
    BOOL bRc = FALSE;
    UINT32 dwIndex = 0;
    UINT32 dwErr = 0;
    USBENDPOINTDESC* pEndpoint = NULL;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: SetUsbInterface enter ");

    if ( !MSC_VALID_CONTEXT( pUsbDevice ) || !pUsbDevice->pUsbInterface )
    {
        usb_HstSendEvent(0x082900aa);
        return FALSE;
    }

    if(nInterfaceNum > 1) // if the usb has only one interface ,maybe do not set .
    {
        dwErr = USB_SetInterface(pUsbDevice->pUsbInterface->bAlternateSetting, pUsbDevice->pUsbInterface->bInterfaceNumber, UsbControlComplete);
        usb_HstSendEvent(0x082900ab);
        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: SetUsbInterface ");

        while(1)
        {
            sxr_Sleep(2 MS_WAITING);
            if( UsbCmdFinished > 0)
                break;
        }
        UsbCmdFinished = 0;
    }

    usb_HstSendEvent(0x08290009);
    usb_HstSendEvent(pUsbDevice->pUsbInterface);
    pEndpoint =(USBENDPOINTDESC*)(pUsbDevice->pUsbInterface + 1);

    // now parse the endpoints
    for ( dwIndex = 0; dwIndex < pUsbDevice->pUsbInterface->bNumEndpoints; dwIndex++)
    {
        usb_HstSendEvent(pEndpoint);

        // Mass Storage Class supports 1 mandatory Bulk OUT, 1 mandatory Bulk IN, and 1 optional INTERRUPT
        if ( USB_ENDPOINT_DIRECTION_OUT(pEndpoint->bEndpointAddress ) )
        {
            if ( NULL == pUsbDevice->BulkOut.hPipe &&
                    (pEndpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_BULK)
            {
                // create the Bulk OUT pipe
                pUsbDevice->BulkOut.hPipe = USB_OpenPipe(pEndpoint);
                if ( !pUsbDevice->BulkOut.hPipe )
                {
                    bRc = FALSE;
                    break;
                }
                pUsbDevice->BulkOut.bIndex         = pEndpoint->bEndpointAddress;
                pUsbDevice->BulkOut.wMaxPacketSize = pEndpoint->wMaxPacketSize;
            }
        }
        else if (USB_ENDPOINT_DIRECTION_IN(pEndpoint->bEndpointAddress ) )
        {
            if ( NULL == pUsbDevice->BulkIn.hPipe &&
                    (pEndpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_BULK)
            {
                // create the Bulk IN pipe
                pUsbDevice->BulkIn.hPipe = USB_OpenPipe(pEndpoint);
                if ( !pUsbDevice->BulkIn.hPipe )
                {
                    bRc = FALSE;
                    break;
                }
                pUsbDevice->BulkIn.bIndex         = pEndpoint->bEndpointAddress;
                pUsbDevice->BulkIn.wMaxPacketSize = pEndpoint->wMaxPacketSize;

            }
            else if ( NULL == pUsbDevice->Interrupt.hPipe &&
                      (pEndpoint->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_INTERRUPT)
            {
                // create the Interrupt pipe
                pUsbDevice->Interrupt.hPipe = USB_OpenPipe(pEndpoint);
                if ( !pUsbDevice->Interrupt.hPipe )
                {
                    bRc = FALSE;
                    break;
                }

                pUsbDevice->Interrupt.bIndex         = pEndpoint->bEndpointAddress;
                pUsbDevice->Interrupt.wMaxPacketSize = pEndpoint->wMaxPacketSize;
            }

        }
        else
        {

        }
        pEndpoint = pEndpoint+1;
    }

    // did we find our endpoints?
    bRc = (pUsbDevice->BulkOut.hPipe && pUsbDevice->BulkIn.hPipe) ? TRUE : FALSE;
    switch (pUsbDevice->pUsbInterface->bInterfaceProtocol)
    {
        case USBMSC_INTERFACE_PROTOCOL_CBIT:
            // CBI Transport 3.4.3: *shall* support the interrupt endpoint
            bRc &= (pUsbDevice->Interrupt.hPipe) ? TRUE : FALSE;
            break;
        case USBMSC_INTERFACE_PROTOCOL_CBT:
            break;

        case USBMSC_INTERFACE_PROTOCOL_BOT:
            break;

        default:
            bRc = FALSE;
            break;
    }
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: SetUsbInterface end ");

    // if we failed to find all of our endpoints then cleanup will occur later
    return (bRc);

}


VOID ClosePipe(PUSBMSC_DEVICE pUsbDevice, PPIPE pPipe)
{
    if ( !pUsbDevice || !pPipe)
    {
        return;
    }

    return;
}


VOID RemoveDeviceContext(PUSBMSC_DEVICE pUsbDevice)
{

    if ( MSC_VALID_CONTEXT( pUsbDevice ) )
    {
        if ( pUsbDevice->Flags.AcceptIo )
        {
            return;
        }

        ClosePipe( pUsbDevice, &pUsbDevice->BulkOut );
        ClosePipe( pUsbDevice, &pUsbDevice->BulkIn );
        ClosePipe( pUsbDevice, &pUsbDevice->Interrupt );

        if (pUsbDevice->hEP0Event)
        {
        }

        if (&pUsbDevice->Lock)
        {

        }

    }
    else
    {

    }

    return;
}


//*****************************************************************************
//
//  U S B M S C      I N T E R F A C E
//
//*****************************************************************************


/*++

UsbsDataTransfer:
   Called by the USB Disk driver to place the block command on the USB.
   Used for reads, writes, commands, etc.

 hTransport:
   The Transport handle passed to DiskAttach.

 pCommand:
    Pointer to Command Block for this transaction.

 pData:
    Pointer to Data Block for this transaction. May be NULL

--*/
static u8 dma_small[516];
UINT32 UsbsDataTransfer(HANDLE hTransport, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData)
{
    PUSBMSC_DEVICE pUsbDevice = (PUSBMSC_DEVICE)hTransport;
    UINT32 dwErr = ERROR_SUCCESS;

    if ( hTransport && pCommand )
    {
        if ( MSC_ACCEPT_IO(pUsbDevice) )
        {
            if (pData)
            {
                // preset the number of bytes transferred
                pData->TransferLength = 0;
            }

            // MUX the Transport Protocol
            switch ( pUsbDevice->pUsbInterface->bInterfaceProtocol )
            {
                case USBMSC_INTERFACE_PROTOCOL_CBIT:
                    dwErr = CBIT_DataTransfer( pUsbDevice, pCommand, pData, (pCommand->Flags & DATA_IN));
                    break;

                case USBMSC_INTERFACE_PROTOCOL_CBT:
                    dwErr = CBIT_DataTransfer( pUsbDevice, pCommand, pData, (pCommand->Flags & DATA_IN));
                    break;

                case USBMSC_INTERFACE_PROTOCOL_BOT:
#if 1
                    if(pData)
                    {
                        USBHOST_ASSERT((pData->RequestLength <= 512),"UsbsDataTransfer length must be less than 512");
                        if((pData->RequestLength<64)&&(pData->RequestLength%4!=0))
                        {
                            hal_HstSendEvent(0xff5d5aca); hal_HstSendEvent(pData->RequestLength);
                            memcpy(pData->DataBlock,dma_small,pData->RequestLength);
                            pData->DataBlock = dma_small;
                        }
                    }
#endif
                    dwErr = BOT_DataTransfer( pUsbDevice, pCommand, pData, (pCommand->Flags & DATA_IN));
#if 1
                    if(pData)
                    {
                        if((pData->RequestLength<64)&&(pData->RequestLength%4!=0))
                        {
                            memcpy(dma_small,pData->DataBlock,pData->RequestLength);
                        }
                    }
#endif
                    break;

                default:
                    dwErr = ERROR_INVALID_DRIVE;
                    break;
            }

        }
        else
        {
            dwErr = ERROR_ACCESS_DENIED;
        }

    }
    else
    {
        dwErr = ERROR_INVALID_PARAM;
    }

    return dwErr;
}

UINT32 GetMaxLUN(HANDLE hTransport, UINT8* pLun)
{
    PUSBMSC_DEVICE pUsbDevice = (PUSBMSC_DEVICE)hTransport;
    UINT32 dwErr = ERROR_INVALID_PARAM;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: GetMaxLUN");

    if ( hTransport  && pLun  && MSC_ACCEPT_IO(pUsbDevice))
    {
        switch ( pUsbDevice->pUsbInterface->bInterfaceProtocol)
        {
            case USBMSC_INTERFACE_PROTOCOL_BOT:
                dwErr = BOT_GetMaxLUN( pUsbDevice, pLun);
                usb_HstSendEvent(0x08290008);
                usb_HstSendEvent(*pLun);
                break;
            default:
                *pLun = 1;
                break;
        }
    }


    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: GetMaxLUN end  dwErr =%d,plun = %d",dwErr,*pLun);
    return dwErr;
}


#if 0

BOOL
UsbDeviceTest(
    PUSBMSC_DEVICE  pUsbDevice
)
{
#if 1
    UNREFERENCED_PARAMETER(pUsbDevice);
#else
    DWORD dwErr;

    //
    // set stall on EP0
    //
    dwErr = ClearOrSetFeature( pUsbDevice->UsbFuncs,
                               pUsbDevice->hUsbDevice,
                               DefaultTransferComplete,
                               pUsbDevice->hEP0Event,
                               USB_SEND_TO_ENDPOINT,
                               USB_FEATURE_ENDPOINT_STALL,
                               0, // bIndex
                               1000,
                               TRUE );

    if ( ERROR_SUCCESS != dwErr )
    {
        DEBUGMSG( ZONE_ERR, (TEXT("SetFeature error:%d\n"), dwErr ));
        TEST_TRAP();
    }

    //
    // clear EP0
    //
    ResetDefaultEndpoint(pUsbDevice->UsbFuncs, pUsbDevice->hUsbDevice);

    //
    // set stall on the BulkOut endpoint
    //
    dwErr = ClearOrSetFeature( pUsbDevice->UsbFuncs,
                               pUsbDevice->hUsbDevice,
                               DefaultTransferComplete,
                               pUsbDevice->hEP0Event,
                               USB_SEND_TO_ENDPOINT,
                               USB_FEATURE_ENDPOINT_STALL,
                               pUsbDevice->BulkOut.bIndex,
                               1000,
                               TRUE );

    if ( ERROR_SUCCESS != dwErr )
    {
        DEBUGMSG( ZONE_ERR, (TEXT("SetFeature error:%d\n"), dwErr));
        TEST_TRAP();
    }

    //
    // clear it
    //
    dwErr = ClearOrSetFeature( pUsbDevice->UsbFuncs,
                               pUsbDevice->hUsbDevice,
                               DefaultTransferComplete,
                               pUsbDevice->hEP0Event,
                               USB_SEND_TO_ENDPOINT,
                               USB_FEATURE_ENDPOINT_STALL,
                               pUsbDevice->BulkOut.bIndex,
                               1000,
                               FALSE );

    if ( ERROR_SUCCESS != dwErr )
    {
        DEBUGMSG( ZONE_ERR, (TEXT("ClearFeature error:%d\n"), dwErr));
        TEST_TRAP();
    }


    //
    // set stall on the BulkIn endpoint
    //
    dwErr = ClearOrSetFeature( pUsbDevice->UsbFuncs,
                               pUsbDevice->hUsbDevice,
                               DefaultTransferComplete,
                               pUsbDevice->hEP0Event,
                               USB_SEND_TO_ENDPOINT,
                               USB_FEATURE_ENDPOINT_STALL,
                               pUsbDevice->BulkIn.bIndex,
                               1000,
                               TRUE );

    if ( ERROR_SUCCESS != dwErr )
    {
        DEBUGMSG( ZONE_ERR, (TEXT("SetFeature error:%d\n"), dwErr));
        TEST_TRAP();
    }

    //
    // clear it
    //
    dwErr = ClearOrSetFeature( pUsbDevice->UsbFuncs,
                               pUsbDevice->hUsbDevice,
                               DefaultTransferComplete,
                               pUsbDevice->hEP0Event,
                               USB_SEND_TO_ENDPOINT,
                               USB_FEATURE_ENDPOINT_STALL,
                               pUsbDevice->BulkIn.bIndex,
                               1000,
                               FALSE );

    if ( ERROR_SUCCESS != dwErr )
    {
        DEBUGMSG( ZONE_ERR, (TEXT("ClearFeature error:%d\n"), dwErr ));
        TEST_TRAP();
    }

#endif // 0

    return TRUE;
}

#endif

// EOF
