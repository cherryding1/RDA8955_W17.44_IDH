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
#include "sxr_tls.h"
#include "sxr_ops.h"
#include "usbhostp_debug.h"

#include "usb.h"
#include "usb_hcd.h"
#include "bot.h"

#include "usbmscp.h"
#include "global_macros.h"


#define min(a,b)    ((a)<(b)?(a):(b))

BOOL BOT_ResetAllPipes(PUSBMSC_DEVICE pUsbDevice);

// 3.1 Bulk-Only Mass Storage Reset.
// This class-specific command shall ready the device for the next CBW.
BOOL BOT_MassStorageReset(PUSBMSC_DEVICE pUsbDevice)
{
    USBSTDREQUEST ControlHeader;
    UINT32 dwBytesTransferred = 0;
    UINT32 dwErr = 0, dwUsbErr = 0;
    BOOL bRc = FALSE;
    UINT8 count = 0;

    if ( MSC_ACCEPT_IO(pUsbDevice) )
    {
        ControlHeader.bmRequestType = USB_REQUEST_HOST_TO_DEVICE | USB_REQUEST_CLASS | USB_REQUEST_FOR_INTERFACE;
        ControlHeader.bRequest = 255;
        ControlHeader.wValue = 0;
        ControlHeader.wIndex = pUsbDevice->pUsbInterface->bInterfaceNumber;
        ControlHeader.wLength = 0;
_retry:
        dwErr = USB_VendorTransfer(&ControlHeader, NULL, &dwBytesTransferred, pUsbDevice->Timeouts.Reset);

        if ( ERROR_SUCCESS == dwErr && USB_NO_ERROR == dwUsbErr )
        {
            bRc = TRUE;
        }
        else if( dwErr == USBHCD_DEVICE_DISCONNECTED )
        {
            usb_HstSendEvent(0x10140001);
            pUsbDevice->Flags.DeviceRemoved = 1;
            pUsbDevice->Flags.AcceptIo = 0;
        }
        else if (ERROR_SUCCESS == USB_ResetDefaultEndpoint() )
        {
            if (++count < MAX_BOT_STALL_COUNT)
            {
                sxr_Sleep(ONE_FRAME_PERIOD);
                goto _retry;
            }
        }
    }

    return bRc;
}


// 3.2 Get Max LUN
UINT32 BOT_GetMaxLUN(PUSBMSC_DEVICE pUsbDevice, UINT8* pLun)
{
    USBSTDREQUEST ControlHeader;
    UINT32  dwBytesTransferred = 0;
    UINT8   bMaxLun = 0xff;
    UINT32  dwErr = 0, dwUsbErr = 0;
    BOOL    bRc = FALSE;

    if ( MSC_ACCEPT_IO(pUsbDevice) &&  pLun )
    {
        ControlHeader.bmRequestType = USB_REQUEST_DEVICE_TO_HOST | USB_REQUEST_CLASS | USB_REQUEST_FOR_INTERFACE;
        ControlHeader.bRequest = 254;
        ControlHeader.wValue = 0;
        ControlHeader.wIndex = pUsbDevice->pUsbInterface->bInterfaceNumber;
        ControlHeader.wLength = 1;

        dwErr = USB_VendorTransfer(&ControlHeader, &bMaxLun, &dwBytesTransferred, pUsbDevice->Timeouts.CommandBlock);

        usb_HstSendEvent(0x08290007);
        if( USB_NO_ERROR != dwUsbErr )
        {
            USB_ResetDefaultEndpoint();
        }
        else
        {
            if (0 <= bMaxLun && bMaxLun < MAX_LUN)
            {
                *pLun = bMaxLun + 1;
            }
            else
            {
                *pLun = 1;
            }
        }

    }

    return dwErr;
}


//
// 5.3.4 Reset Recovery
//
BOOL BOT_ResetRecovery(PUSBMSC_DEVICE pUsbDevice)
{
    BOOL bRc = FALSE;
    UINT32 dwErr;

    // 0. reset endpoint 0
    dwErr = USB_ResetDefaultEndpoint();

    if ( ERROR_SUCCESS == dwErr )
    {
        // 1. send class command
        BOT_MassStorageReset(pUsbDevice);

        // 2. reset BulkIn endpoint
        dwErr = USB_ResetEndpoint(pUsbDevice->BulkIn.hPipe, pUsbDevice->Timeouts.Reset);

        if ( ERROR_SUCCESS == dwErr)
        {
            // 3. reset BulkOut endpoint
            dwErr = USB_ResetEndpoint(pUsbDevice->BulkOut.hPipe, pUsbDevice->Timeouts.Reset);

            if ( ERROR_SUCCESS == dwErr )
            {
                bRc = TRUE;
            }
        }
    }
    return bRc;
}



//
// Reset All Pipes
//
BOOL BOT_ResetAllPipes( PUSBMSC_DEVICE pUsbDevice )
{
    BOOL bRc = FALSE;
    UINT32 dwErr;

    // 0. reset endpoint 0
    dwErr = USB_ResetDefaultEndpoint();
    usb_HstSendEvent(0x9999aaad); usb_HstSendEvent(dwErr);
    if ( USB_NO_ERROR == dwErr )
    {
        // 1. reset BulkIn endpoint
        if ( USB_ResetEndpoint(pUsbDevice->BulkIn.hPipe, USB_NO_WAIT) == USB_NO_ERROR)
        {
            usb_HstSendEvent(0x9999aaab);
            // 2. reset BulkOut endpoint
            if ( USB_ResetEndpoint(pUsbDevice->BulkOut.hPipe, USB_NO_WAIT) == USB_NO_ERROR)
                usb_HstSendEvent(0x9999aaac);
            {
                bRc = TRUE;
            }
        }
    }
    usb_HstSendEvent(0x9999aaaf);  usb_HstSendEvent(bRc);

    return bRc;
}

#if 0
//MEM_ACCESS_UNCACHED
// See 5.3 Data Transfer Conditions
//
// Command Block Wrapper
static CBW Cbw;
// Command Status Wrapper
static CSW Csw;

UINT32 BOT_DataTransfer(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData, BOOL Direction )
{
    // Command Block Wrapper
    //CBW Cbw;
    // Command Status Wrapper
    //CSW Csw;
    u32 * Cbw_unca;
    u32 * Csw_unca;

    Cbw_unca = MEM_ACCESS_UNCACHED(&Cbw);

    Csw_unca = MEM_ACCESS_UNCACHED(&Csw);

    UINT32 dwCbwSize = sizeof(CBW);
    UINT32 dwCswSize = sizeof(CSW);

    UINT32 dwBytesTransferred = 0;
    UINT32 dwErr = USB_NO_ERROR;
    UINT32 dwResetErr, dwCswErr;

    BOOL  bRc = FALSE;
    UINT8 ucStallCount;
    usb_HstSendEvent(0x09090d25);
    usb_HstSendEvent(Direction);

    // parameter check
    if ( !pUsbDevice || !pCommand || !pCommand->CommandBlock || pCommand->Length > MAX_CBWCB_SIZE )
    {
        dwErr = ERROR_INVALID_PARAM;
        return dwErr;
    }

    if ( !MSC_ACCEPT_IO(pUsbDevice) )
    {
        dwErr = ERROR_ACCESS_DENIED;
        return dwErr;
    }
    //usb_HstSendEvent(0x999911ad); usb_HstSendEvent(dwErr);

    // We require exclusive entry into the transport.
    // we could implement command queuing.
    if ( !BOT_ResetAllPipes(pUsbDevice) )
    {
        dwErr = ERROR_GEN_FAILURE;

        usb_HstSendEvent(0x999900ad); usb_HstSendEvent(dwErr);
        goto BOT_SendCommandDone;
    }

    ucStallCount = 0;

    pUsbDevice->dwCurTag++;

    memset( Cbw_unca, 0, dwCbwSize );
    memset(Csw_unca, 0, dwCswSize );





    // build the active CBW
    ((PCBW)Cbw_unca)->dCBWSignature = CBW_SIGNATURE;
    ((PCBW)Cbw_unca)->dCBWTag = pUsbDevice->dwCurTag;
    ((PCBW)Cbw_unca)->dCBWDataTransferLength = pData ? pData->RequestLength : 0;

    if (Direction)
    {
        ((PCBW)Cbw_unca)->bmCBWFlags |= 0x80;
    }

    ((PCBW)Cbw_unca)->bCBWLUN = (UINT8)(pCommand->dwLun & 0xf); // TBD
    ((PCBW)Cbw_unca)->bCBWCBLength = (UINT8)pCommand->Length;
    memcpy( ((PCBW)Cbw_unca)->CBWCB, pCommand->CommandBlock, pCommand->Length);
    usb_HstSendEvent(0x09090a61); usb_HstSendEvent(pCommand->Length); usb_HstSendEvent((u32)&Cbw);
    //sxs_Dump(1, 0, Cbw_unca, 31);

    // 5.3.1 Command Block Transport
    dwErr = USB_BulkOrInterruptTransfer(pUsbDevice->BulkOut.hPipe,Cbw_unca, dwCbwSize, &dwBytesTransferred, pUsbDevice->Timeouts.CommandBlock);

    if ( USB_NO_ERROR != dwErr || dwBytesTransferred != dwCbwSize )
    {
        usb_HstSendEvent(0x09090061);

        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host bot.c: BOT_DataTransfer  dwErr =%d,dwCbwSize = %d ,dwBytesTransferred=%d",dwErr,dwCbwSize,dwBytesTransferred);
        bRc = BOT_ResetRecovery(pUsbDevice);
        goto BOT_SendCommandDone;
    }
    usb_HstSendEvent(0x09150010);

    // 5.3.2 Data Transport
    if (pData && pData->DataBlock && pData->RequestLength)
    {
        const uint16 wMaxPacketSize = Direction ? pUsbDevice->BulkIn.wMaxPacketSize : pUsbDevice->BulkOut.wMaxPacketSize;
        // we don't provide our own phys buffer so are limited by USBD's max buffer size
        const UINT32  dwMaxBlock = MAX_USB_BULK_LENGTH;
        VOID* pDataBlock = MEM_ACCESS_UNCACHED(pData->DataBlock);
        UINT32 dwTransferLength = 0;
        UINT32 dwRequestLength = 0;
        UINT32 dwDataLength = pData->RequestLength;
        UINT32 dwTimeout = pCommand->Timeout;
        UINT32 dwStartTime=0;

        while(dwDataLength)
        {
            dwRequestLength = min(dwMaxBlock, dwDataLength);
            dwTransferLength = 0;

            //ASSERT( dwRequestLength <= MAX_USB_BULK_LENGTH );
            USBHOST_ASSERT(( dwRequestLength <= MAX_USB_BULK_LENGTH ), "scsi2 :( dwRequestLength <= MAX_USB_BULK_LENGTH )");

            dwErr = USB_BulkOrInterruptTransfer(
                        Direction ? pUsbDevice->BulkIn.hPipe : pUsbDevice->BulkOut.hPipe,
                        pDataBlock,
                        dwRequestLength,
                        &dwTransferLength,
                        dwTimeout);

            // 6.7.2.3: the host shall accept the data received
            if ( (USB_NO_ERROR == dwErr || USB_STALL_ERROR == dwErr) )
            {
                // adjust transfer length & buffer
                //ASSERT(dwTransferLength <= dwRequestLength);

                USBHOST_ASSERT((dwTransferLength <= dwRequestLength), "scsi2 :(dwTransferLength <= dwRequestLength)");
                pData->TransferLength += dwTransferLength;
                dwDataLength -= dwTransferLength;
                pDataBlock = (VOID*)((UINT8*)pDataBlock + dwTransferLength);
            }

            if ( USB_NO_ERROR != dwErr )
            {

                UINT8 bIndex = Direction ? pUsbDevice->BulkIn.bIndex : pUsbDevice->BulkOut.bIndex;

                // 6.7.x.3: the host shall clear the Bulk pipe

                dwDataLength = 0;

                // test/reset Bulk endpoint
                dwResetErr = USB_ResetEndpoint((Direction ? pUsbDevice->BulkIn.hPipe : pUsbDevice->BulkOut.hPipe), pUsbDevice->Timeouts.Reset);

                if (ERROR_SUCCESS != dwResetErr)
                {
                    bRc = BOT_ResetRecovery(pUsbDevice);
                    usb_HstSendEvent(0x19090336);

                    if (ERROR_SUCCESS == dwErr)
                        dwErr = ERROR_GEN_FAILURE;
                    goto BOT_SendCommandDone;
                }
            }
        }
    }

    // 5.3.3 Command Status Transport (CSW)
    ucStallCount = 0; // reset Stall count

_RetryCSW:

    //usb_HstSendEvent(0x09150001);
    dwCswErr = USB_BulkOrInterruptTransfer(pUsbDevice->BulkIn.hPipe, &Csw, dwCswSize, &dwBytesTransferred, pUsbDevice->Timeouts.CommandStatus);

    // Figure 2 - Status Transport Flow
    if ( USB_NO_ERROR != dwCswErr )
    {
        if( dwCswErr == USBHCD_DEVICE_DISCONNECTED )
        {
            goto BOT_SendCommandDone;
        }
        else
        {
            // reset BulkIn endpoint
            dwResetErr = USB_ResetEndpoint(pUsbDevice->BulkIn.hPipe, pUsbDevice->Timeouts.Reset );

            if ( ERROR_SUCCESS == dwResetErr && ++ucStallCount < MAX_BOT_STALL_COUNT )
            {
                sxr_Sleep(ONE_FRAME_PERIOD);

                goto _RetryCSW;

            }
            else
            {
                bRc = BOT_ResetRecovery(pUsbDevice);
                usb_HstSendEvent(0x19090335);

                if (ERROR_SUCCESS == dwErr)
                    dwErr = ERROR_GEN_FAILURE;

                goto BOT_SendCommandDone;
            }
        }
    }

    // Validate CSW...
    // 6.3.1 Valid CSW: size
    if ( dwBytesTransferred != dwCswSize )
    {
        usb_HstSendEvent(0x09090025);
        // 6.5 Host shall perform ResetRecovery for invalid CSW
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        goto BOT_SendCommandDone;
    }

//  memcpy(&Csw, HAL_SYS_GET_UNCACHED_ADDR(&Csw), sizeof(CSW));

    // 6.3.1 Valid CSW: Signature
    if ( CSW_SIGNATURE != ((PCSW)Csw_unca)->dCSWSignature)
    {
        usb_HstSendEvent(0x09090026);   // this one

        // 6.5 Host shall perform ResetRecovery for invalid CSW
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
        {
            dwErr = ERROR_GEN_FAILURE; usb_HstSendEvent(0xe9090026);
        }

        //goto BOT_SendCommandDone;
    }

    // 6.3.1 Valid CSW: Tags
    if ( ((PCBW)Cbw_unca)->dCBWTag != ((PCSW)Csw_unca)->dCSWTag )
    {
        usb_HstSendEvent(0x09090027);
        // 6.5 Host shall perform ResetRecovery for invalid CSW
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        goto BOT_SendCommandDone;
    }

    // Command Status?
    if ( 0 == ((PCSW)Csw_unca)->bCSWStatus || 1 == ((PCSW)Csw_unca)->bCSWStatus )
    {
        usb_HstSendEvent(0x09090028);   // this one
        if (pData && pData->TransferLength)
        {
            pData->TransferLength = ((PCBW)Cbw_unca)->dCBWDataTransferLength - ((PCSW)Csw_unca)->dCSWDataResidue;
        }

        if (1 == ((PCSW)Csw_unca)->bCSWStatus )
        {
            dwErr = ERROR_GEN_FAILURE; usb_HstSendEvent(0xe9090028); //  erro here    ScsiReadCapacity end 2 dwErr = 0 ,tData.TransferLength =8 tData.RequestLength =8
        }

        goto BOT_SendCommandDone;
    }

    // Phase Error?
    if ( 2 == ((PCSW)Csw_unca)->bCSWStatus )
    {
        usb_HstSendEvent(0x09090029);
        // ignore the dCSWDataResidue
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        goto BOT_SendCommandDone;
    }

BOT_SendCommandDone:
    // cleanup
    usb_HstSendEvent(0x09090030);
    usb_HstSendEvent(dwErr);
    return dwErr;
}

#else



//
// See 5.3 Data Transfer Conditions
//

UINT32 BOT_DataTransfer(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData, BOOL Direction )
{
    // Command Block Wrapper
    CBW Cbw;
    // Command Status Wrapper
    CSW Csw;

    uint32 dwCbwSize = sizeof(CBW);
    uint32 dwCswSize = sizeof(CSW);

    uint32 dwBytesTransferred = 0;
    uint32 dwErr = USB_NO_ERROR;
    uint32 dwResetErr, dwCswErr;

    BOOL  bRc = FALSE;
    uint8 ucStallCount;

    // parameter check
    if ( !pUsbDevice || !pCommand || !pCommand->CommandBlock || pCommand->Length > MAX_CBWCB_SIZE )
    {
        dwErr = ERROR_INVALID_PARAM;
        return dwErr;
    }

    if ( !MSC_ACCEPT_IO(pUsbDevice) )
    {
        dwErr = ERROR_ACCESS_DENIED;
        return dwErr;
    }

    // We require exclusive entry into the transport.
    // we could implement command queuing.
    if ( !BOT_ResetAllPipes(pUsbDevice) )
    {
        dwErr = ERROR_GEN_FAILURE;
        goto BOT_SendCommandDone;
    }

    ucStallCount = 0;

    pUsbDevice->dwCurTag++;

    memset( &Cbw, 0, dwCbwSize );
    memset( &Csw, 0, dwCswSize );

    // build the active CBW
    Cbw.dCBWSignature = CBW_SIGNATURE;
    Cbw.dCBWTag = pUsbDevice->dwCurTag;
    Cbw.dCBWDataTransferLength = pData ? pData->RequestLength : 0;

    if (Direction)
    {
        Cbw.bmCBWFlags |= 0x80;
    }

    Cbw.bCBWLUN = (uint8)(pCommand->dwLun & 0xf); // TBD
    Cbw.bCBWCBLength = (uint8)pCommand->Length;
    memcpy( &Cbw.CBWCB, pCommand->CommandBlock, pCommand->Length);
    usb_HstSendEvent(0x09150f13);

    // 5.3.1 Command Block Transport
    dwErr = USB_BulkOrInterruptTransfer(pUsbDevice->BulkOut.hPipe, &Cbw, dwCbwSize, &dwBytesTransferred, pUsbDevice->Timeouts.CommandBlock);

    if ( USB_NO_ERROR != dwErr || dwBytesTransferred != dwCbwSize )    //
    {
        usb_HstSendEvent(0x09090061);
        bRc = BOT_ResetRecovery(pUsbDevice);
        goto BOT_SendCommandDone;
    }
    usb_HstSendEvent(0x09150f10);

    // 5.3.2 Data Transport
    if (pData && pData->DataBlock && pData->RequestLength)
    {
        const uint16 wMaxPacketSize = Direction ? pUsbDevice->BulkIn.wMaxPacketSize : pUsbDevice->BulkOut.wMaxPacketSize;
        // we don't provide our own phys buffer so are limited by USBD's max buffer size
        const uint32  dwMaxBlock = MAX_USB_BULK_LENGTH;
        void* pDataBlock = pData->DataBlock;
        uint32 dwTransferLength = 0;
        uint32 dwRequestLength = 0;
        uint32 dwDataLength = pData->RequestLength;
        uint32 dwTimeout = pCommand->Timeout;
        uint32 dwStartTime=0;

        while(dwDataLength)
        {
            dwRequestLength = min(dwMaxBlock, dwDataLength);
            dwTransferLength = 0;

            //ASSERT( dwRequestLength <= MAX_USB_BULK_LENGTH );
            USBHOST_ASSERT(( dwRequestLength <= MAX_USB_BULK_LENGTH ), "scsi2 :( dwRequestLength <= MAX_USB_BULK_LENGTH )");

            dwErr = USB_BulkOrInterruptTransfer(
                        Direction ? pUsbDevice->BulkIn.hPipe : pUsbDevice->BulkOut.hPipe,
                        pDataBlock,
                        dwRequestLength,
                        &dwTransferLength,
                        dwTimeout);

            // 6.7.2.3: the host shall accept the data received
            if ( (USB_NO_ERROR == dwErr || USB_STALL_ERROR == dwErr) )
            {
                // adjust transfer length & buffer
                //ASSERT(dwTransferLength <= dwRequestLength);

                USBHOST_ASSERT(( dwTransferLength <= dwRequestLength ), "scsi2 :( dwTransferLength <= dwRequestLength )");
                pData->TransferLength += dwTransferLength;
                dwDataLength -= dwTransferLength;
                pDataBlock = (void*)((uint8*)pDataBlock + dwTransferLength);
            }

            if ( USB_NO_ERROR != dwErr )
            {

                uint8 bIndex = Direction ? pUsbDevice->BulkIn.bIndex : pUsbDevice->BulkOut.bIndex;

                // 6.7.x.3: the host shall clear the Bulk pipe

                dwDataLength = 0;

                // test/reset Bulk endpoint
                dwResetErr = USB_ResetEndpoint((Direction ? pUsbDevice->BulkIn.hPipe : pUsbDevice->BulkOut.hPipe), pUsbDevice->Timeouts.Reset);

                if (ERROR_SUCCESS != dwResetErr)
                {
                    bRc = BOT_ResetRecovery(pUsbDevice);

                    if (ERROR_SUCCESS == dwErr)
                        dwErr = ERROR_GEN_FAILURE;
                    goto BOT_SendCommandDone;
                }
            }
        }
    }

    // 5.3.3 Command Status Transport (CSW)
    ucStallCount = 0; // reset Stall count

_RetryCSW:

    //usb_HstSendEvent(SYS_EVENT,0x09150001);
    dwCswErr = USB_BulkOrInterruptTransfer(pUsbDevice->BulkIn.hPipe, &Csw, dwCswSize, &dwBytesTransferred, pUsbDevice->Timeouts.CommandStatus);

    // Figure 2 - Status Transport Flow
    if ( USB_NO_ERROR != dwCswErr )
    {
        if( dwCswErr == USBHCD_DEVICE_DISCONNECTED )
        {
            goto BOT_SendCommandDone;
        }
        else
        {
            // reset BulkIn endpoint
            dwResetErr = USB_ResetEndpoint(pUsbDevice->BulkIn.hPipe, pUsbDevice->Timeouts.Reset );

            if ( ERROR_SUCCESS == dwResetErr && ++ucStallCount < MAX_BOT_STALL_COUNT )
            {
                sxr_Sleep(ONE_FRAME_PERIOD);

                goto _RetryCSW;

            }
            else
            {
                bRc = BOT_ResetRecovery(pUsbDevice);

                if (ERROR_SUCCESS == dwErr)
                    dwErr = ERROR_GEN_FAILURE;

                goto BOT_SendCommandDone;
            }
        }
    }

    // Validate CSW...
    // 6.3.1 Valid CSW: size
    if ( dwBytesTransferred != dwCswSize )
    {
        usb_HstSendEvent(0x09090025);
        // 6.5 Host shall perform ResetRecovery for invalid CSW
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        goto BOT_SendCommandDone;
    }

//  memcpy(&Csw, HAL_SYS_GET_UNCACHED_ADDR(&Csw), sizeof(CSW));

    // 6.3.1 Valid CSW: Signature
    if ( CSW_SIGNATURE != Csw.dCSWSignature)
    {
        usb_HstSendEvent(0x09090026);

        // 6.5 Host shall perform ResetRecovery for invalid CSW
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        //goto BOT_SendCommandDone;
    }

    // 6.3.1 Valid CSW: Tags
    if ( Cbw.dCBWTag != Csw.dCSWTag )
    {
        usb_HstSendEvent(0x09090027);
        // 6.5 Host shall perform ResetRecovery for invalid CSW
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        goto BOT_SendCommandDone;
    }

    // Command Status?
    if ( 0 == Csw.bCSWStatus || 1 == Csw.bCSWStatus )
    {
        //usb_HstSendEvent(0x09090028);
        if (pData && pData->TransferLength)
        {
            pData->TransferLength = Cbw.dCBWDataTransferLength - Csw.dCSWDataResidue;
        }

        if (1 == Csw.bCSWStatus )
        {
            dwErr = ERROR_GEN_FAILURE;
        }

        goto BOT_SendCommandDone;
    }

    // Phase Error?
    if ( 2 == Csw.bCSWStatus )
    {
        usb_HstSendEvent(0x09090029);
        // ignore the dCSWDataResidue
        bRc = BOT_ResetRecovery(pUsbDevice);

        if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

        goto BOT_SendCommandDone;
    }

BOT_SendCommandDone:
    // cleanup
    //usb_HstSendEvent(SYS_EVENT,0x09090030);
    //usb_HstSendEvent(SYS_EVENT,dwErr);
    return dwErr;
}


#endif
//
// Set a UINT32 Value into an array of chars pointed to by pbArray
// (Little Endian)
//
VOID SetDWORD(UINT8* pbArray, UINT32 dwValue)
{
    if(pbArray)
    {
        pbArray[3] = (UINT8)dwValue;
        dwValue >>= 8;
        pbArray[2] = (UINT8)dwValue;
        dwValue >>= 8;
        pbArray[1] = (UINT8)dwValue;
        dwValue >>= 8;
        pbArray[0] = (UINT8)dwValue;
    }
    return;
}

VOID SetWORD(UINT8*  pBytes, uint16 wValue )
{
    if (pBytes)
    {
        pBytes[1] = (UINT8)wValue;
        wValue >>= 8;
        pBytes[0] = (UINT8)wValue;
    }
    return;
}


//
// Gets a uint16 Value from an array of chars pointed to by pbArray.
// The return value is promoted to UINT32.
// (Little Endian)
//
UINT32 GetDWORD(UINT8*  pbArray)
{
    UINT32 dwReturn=0;
    if (pbArray)
    {
        dwReturn=*(pbArray++);
        dwReturn=dwReturn*0x100 + *(pbArray++);
        dwReturn=dwReturn*0x100 + *(pbArray++);
        dwReturn=dwReturn*0x100 + *(pbArray++);
    }
    return dwReturn;
}

// EOF
