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
#include "usbhostp_debug.h"

#include "usb.h"
#include "usb_hcd.h"
#include "usbmsc.h"
#include "cbit.h"

#define min(a,b)    ((a)<(b)?(a):(b))

uint32 CBIT_CommandTransport(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, uint32* pdwBytesTransferred);

BOOL CBIT_ResetAllPipes(PUSBMSC_DEVICE pUsbDevice);

uint32 CBIT_StatusTransport(PUSBMSC_DEVICE pUsbDevice);


// 2.2 Command Block Reset.
//
// Note: CBIT 1.0 sections
//      2.3.2.4 Indefinite Delay, and
//      2.3.2.6 Persistant Command Block Failure
// in the spec create a failure recovery problem on WCE devices
// since we can not yet suspend or perform a port reset.
//
uint32 CBIT_CommandBlockReset(PUSBMSC_DEVICE pUsbDevice)
{
    TRANSPORT_COMMAND tCommand = {0};
    uint8             bCommandBlock[12];

    uint32 dwErr = 0, dwBytesTransferred = 0;

    //DEBUGMSG(ZONE_TRACE,(TEXT("USBMSC>CBIT_CommandBlockReset\n")));

    tCommand.Flags   = DATA_OUT;
    tCommand.Timeout = pUsbDevice->Timeouts.CommandBlock;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pUsbDevice->pUsbInterface->bInterfaceSubClass? 6 : 12;
    tCommand.CommandBlock = bCommandBlock;

    memset( bCommandBlock, 0xFF, sizeof(bCommandBlock));
    bCommandBlock[0] = 0x1D;
    bCommandBlock[1] = 0x04;

    dwErr = CBIT_CommandTransport( pUsbDevice, &tCommand, &dwBytesTransferred );

    if ( ERROR_SUCCESS == dwErr )
    {
        dwErr = CBIT_StatusTransport( pUsbDevice );
    }

    //
    // The host shall send ClearFeature on both Bulk-In and Bulk-Out pipes,
    // which is done in Reset Recovery
    //

    return dwErr;
}


//
// Reset Recovery
//
BOOL CBIT_ResetRecovery( PUSBMSC_DEVICE pUsbDevice)
{
    BOOL bRc = FALSE;
    uint32 dwErr;

    // 0. reset endpoint 0
    dwErr = USB_ResetDefaultEndpoint();

    if ( ERROR_SUCCESS == dwErr )
    {
        // 1. send class command
        // CBI1.0: 2.2 - After a Command Block Reset completes, the Stall condition and endpoint's data toggle
        // are undefined. The host shall send Clear Feature to both bulk in and bulk out.
        //        dwErr = CBIT_CommandBlockReset(pUsbDevice);
        //        if ( ERROR_SUCCESS != dwErr )
        //            TEST_TRAP();

        // 2. reset BulkIn endpoint
        dwErr = USB_ResetEndpoint(pUsbDevice->BulkIn.hPipe, pUsbDevice->Timeouts.Reset);

        if ( ERROR_SUCCESS == dwErr)
        {
            // 3. reset BulkOut endpoint
            dwErr = USB_ResetEndpoint(pUsbDevice->BulkOut.hPipe, pUsbDevice->Timeouts.Reset );

            if ( ERROR_SUCCESS == dwErr && pUsbDevice->Interrupt.hPipe)
            {
                // 4. reset Interrupt endpoint
                dwErr = USB_ResetEndpoint(pUsbDevice->Interrupt.hPipe, pUsbDevice->Timeouts.Reset );
            }
        }
    }

    if (ERROR_SUCCESS == dwErr)
    {
        bRc = TRUE;
    }

    return bRc;
}



//
// Reset All Pipes
//
BOOL CBIT_ResetAllPipes(PUSBMSC_DEVICE pUsbDevice)
{
    BOOL bRc = FALSE;
    uint32 dwErr;

    // 0. reset endpoint 0
    dwErr = USB_ResetDefaultEndpoint();

    if ( ERROR_SUCCESS == dwErr )
    {
        // 1. reset BulkIn endpoint
        if ( USB_ResetEndpoint(pUsbDevice->BulkIn.hPipe, USB_NO_WAIT) )
        {
            // 2. reset BulkOut endpoint
            if ( USB_ResetEndpoint(pUsbDevice->BulkOut.hPipe, USB_NO_WAIT) )
            {
                // 3. reset Interrupt endpoint
                bRc = pUsbDevice->Interrupt.hPipe ? USB_ResetEndpoint(pUsbDevice->Interrupt.hPipe,USB_NO_WAIT) : TRUE;
            }
        }
    }

    return bRc;
}


//
// 4.1 Accept Device-Specific Command (ADSC)
//
uint32 CBIT_CommandTransport(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, uint32* pdwBytesTransferred )
{
    USBSTDREQUEST ADSC = {0};
    uint32 dwErr = 0, dwUsbErr = 0;
    BOOL bRc = FALSE;
    uint8 count = 0;

    if ( !pCommand || !pCommand->CommandBlock || !pdwBytesTransferred )
    {
        dwErr = ERROR_INVALID_PARAM;
        return dwErr;
    }

    if ( !MSC_ACCEPT_IO(pUsbDevice) )
    {
        dwErr = ERROR_ACCESS_DENIED;
        return dwErr;
    }

    // ADSC packet
    ADSC.bmRequestType = USB_REQUEST_HOST_TO_DEVICE | USB_REQUEST_CLASS | USB_REQUEST_FOR_INTERFACE;
    ADSC.bRequest = 0;
    ADSC.wValue   = 0;
    ADSC.wIndex   = pUsbDevice->pUsbInterface->bInterfaceNumber;
    ADSC.wLength  = (uint16)pCommand->Length;

_retry:
    dwErr = USB_VendorTransfer(&ADSC, pCommand->CommandBlock, pdwBytesTransferred, pUsbDevice->Timeouts.CommandBlock);

    if ( ERROR_SUCCESS != dwErr || USB_NO_ERROR != dwUsbErr || *pdwBytesTransferred != pCommand->Length )
    {
        if (ERROR_SUCCESS == USB_ResetDefaultEndpoint() )
        {
            if (++count < MAX_CBIT_STALL_COUNT)
            {
                sxr_Sleep(ONE_FRAME_PERIOD);
                goto _retry;
            }
        }

        if ( USB_NO_ERROR != dwUsbErr && ERROR_SUCCESS == dwErr)
        {
            dwErr = ERROR_GEN_FAILURE;
        }
    }

    return dwErr;

}


uint32 CBIT_StatusTransport(PUSBMSC_DEVICE pUsbDevice)
{
    uint32 dwBytesTransferred = 0;
    uint32 dwErr = ERROR_SUCCESS;
    uint32 dwUsbErr = USB_NO_ERROR;

    uint8  ucStatus[2];
    uint32  dwStatusSize = sizeof(ucStatus);

    BOOL  bRc = FALSE;

    if ( !MSC_ACCEPT_IO(pUsbDevice) )
    {
        dwErr = ERROR_ACCESS_DENIED;
        //DEBUGMSG( ZONE_ERR,(TEXT("CBIT_StatusTransport error:%d\n"), dwErr));

        return dwErr;
    }

    if (pUsbDevice->Interrupt.hPipe && !pUsbDevice->Flags.IgnoreInterrupt)
    {
        memset( ucStatus, 0, dwStatusSize );

        dwErr = USB_BulkOrInterruptTransfer(
                    pUsbDevice->Interrupt.hPipe,
                    ucStatus,
                    dwStatusSize,
                    &dwBytesTransferred,
                    pUsbDevice->Timeouts.CommandStatus);

        // check Status Transport bits
        if ( ERROR_SUCCESS != dwErr || USB_NO_ERROR != dwUsbErr || dwBytesTransferred != dwStatusSize )
        {

            USB_ResetEndpoint(pUsbDevice->Interrupt.hPipe, USB_NO_WAIT);

            pUsbDevice->InterruptErrors++;

            goto CBIT_StatusTransportDone;

        }
        else
        {
            pUsbDevice->InterruptErrors = 0;
        }

        //
        // Check Command Status
        //
        switch (pUsbDevice->pUsbInterface->bInterfaceSubClass)
        {
            case USBMSC_SUBCLASS_UFI:
                // The ASC & (optional) ASCQ are in the data block.
                if ( ucStatus[0] )
                {
                    dwErr = ERROR_PERSISTANT;
                }
                break;

            default:
            {
                if (CBIT_COMMAND_COMPLETION_INTERRUPT != ucStatus[0] )
                {
                    if (ERROR_SUCCESS == dwErr)
                        dwErr = ERROR_GEN_FAILURE;

                    goto CBIT_StatusTransportDone;
                }

                switch ( ucStatus[1] & 0x0F )
                {
                    case CBIT_STATUS_SUCCESS:
                        break;
                    case CBIT_STATUS_FAIL:
                        dwErr = ERROR_GEN_FAILURE;
                        break;

                    case CBIT_STATUS_PHASE_ERROR:
                        // TBD: send CommandBlockReset
                        dwErr = ERROR_GEN_FAILURE;
                        break;

                    case CBIT_STATUS_PERSISTENT_ERROR:
                        // The CBIT spec states that a REQUEST_SENSE command block must be sent,
                        // which must be handled by the disk device.
                        dwErr = ERROR_PERSISTANT;
                        break;

                    default:
                        dwErr = ERROR_GEN_FAILURE;
                        break;
                }
            }
        }
    }

CBIT_StatusTransportDone:
    //
    // TODO: Some devices claim to use the interrupt endpoint but really do not.
    // Currently, the only way to determine if the device really uses the
    // interrupt endpoint is to try it a few times and see. Need to add registry work arounds.
    // Exceptions:
    //  UFI Spec 2.1: Interrupt endpoint required
    //
    if (MAX_INT_RETRIES == pUsbDevice->InterruptErrors &&
            !(USBMSC_SUBCLASS_UFI == pUsbDevice->pUsbInterface->bInterfaceSubClass &&
              USBMSC_INTERFACE_PROTOCOL_CBIT == pUsbDevice->pUsbInterface->bInterfaceProtocol)
       )
    {
        //DEBUGMSG( ZONE_WARN, (TEXT("USBMSC:IgnoreInterrupt:ON\n"), dwErr) );
        //COS_EnterCriticalSection(&pUsbDevice->Lock);
        pUsbDevice->Flags.IgnoreInterrupt = TRUE;
        //COS_ExitCriticalSection(&pUsbDevice->Lock);
        pUsbDevice->InterruptErrors++;
    }

    return dwErr;
}


//
// Command/Data/Status Transport
//
uint32 CBIT_DataTransfer(PUSBMSC_DEVICE pUsbDevice, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData, BOOL Direction)
{
    uint32 dwBytesTransferred = 0;
    uint32 dwErr = ERROR_SUCCESS;
    uint32 dwUsbErr = USB_NO_ERROR;
    uint32 dwResetErr, dwStatusErr;

    BOOL  bRc = FALSE;
    hal_HostUsbEvent(0xffccaaaa);
    USBHOST_ASSERT(FALSE, "cbit :	this type of USB mass storage has interrupt trassport mode  ");

    // parameter checks
    if ( !pCommand || !pCommand->CommandBlock /*|| pCommand->Length > MAX_CBWCB_SIZE*/ )
    {
        dwErr = ERROR_INVALID_PARAM;
        return dwErr;
    }

    if ( !MSC_ACCEPT_IO(pUsbDevice) )
    {
        dwErr = ERROR_ACCESS_DENIED;
        return dwErr;
    }

    //
    // We require exclusive entry into the transport.
    // we could implement command queuing.
    //
    //COS_EnterCriticalSection(&pUsbDevice->Lock);

    if ( !CBIT_ResetAllPipes(pUsbDevice) )
    {
        dwErr = ERROR_GEN_FAILURE;
        goto CBIT_SendCommandDone;
    }

    //
    // Command Block Transport via ADSC
    //
    dwErr = CBIT_CommandTransport( pUsbDevice, pCommand, &dwBytesTransferred );

    if (ERROR_SUCCESS != dwErr || dwBytesTransferred != pCommand->Length)
    {
        // 2.3.2.1: if the device does STALL an ADSC, then the device shall not
        // transport the status for the command block by interrupt pipe.
        // Treat a timeout in the same manner.
        goto CBIT_SendCommandDone;
    }

    //
    // (optional) Data Transport
    //
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
        uint8 resets = 0; // consecutive resets

        while (dwDataLength && ((signed)dwTimeout > 0) && resets < 3)
        {
            dwRequestLength = min(dwMaxBlock, dwDataLength);
            dwTransferLength = 0;

            //ASSERT( dwRequestLength <= MAX_USB_BULK_LENGTH );
            USBHOST_ASSERT(( dwRequestLength <= MAX_USB_BULK_LENGTH ), "cbit.c :( dwRequestLength <= MAX_USB_BULK_LENGTH )");

            dwStartTime = hal_TimGetUpTime();

            dwErr = USB_BulkOrInterruptTransfer(Direction ? pUsbDevice->BulkIn.hPipe : pUsbDevice->BulkOut.hPipe, pDataBlock, dwRequestLength, &dwTransferLength, dwTimeout);

            // accept any data received
            if ( (ERROR_SUCCESS == dwErr   || ERROR_TIMEOUT == dwErr) && (USB_NO_ERROR == dwUsbErr || USB_STALL_ERROR == dwUsbErr) )
            {
                // adjust transfer length & buffer
                //ASSERT(dwTransferLength <= dwRequestLength);
                USBHOST_ASSERT((dwTransferLength <= dwRequestLength), "cbit.c :(dwTransferLength <= dwRequestLength)");
                pData->TransferLength += dwTransferLength;
                dwDataLength -= dwTransferLength;
                pDataBlock = (void*)((uint8*)pDataBlock + dwTransferLength);
            }

            if ((ERROR_SUCCESS != dwErr) || (USB_NO_ERROR != dwUsbErr))
            {
                // reset the Bulk pipe

                resets++;
                dwResetErr = USB_ResetEndpoint(Direction ? pUsbDevice->BulkIn.hPipe : pUsbDevice->BulkOut.hPipe, pUsbDevice->Timeouts.Reset );

                if (ERROR_SUCCESS != dwResetErr)
                {
                    dwDataLength = 0;
                }

                // protocol says its done
                if (USB_NO_ERROR != dwUsbErr)
                {
                    dwDataLength = 0;
                }

            }
            else
            {
                resets = 0;
            }
        }
    }


CBIT_SendCommandDone:
    //
    // cleanup
    //
    if (ERROR_SUCCESS != dwErr)
    {
        bRc = CBIT_ResetRecovery(pUsbDevice);
    }
    else
    {
        //
        // Status Transport
        //
        //DEBUGMSG(ZONE_BOT,(TEXT("Status Transport\n")));

        dwStatusErr = CBIT_StatusTransport( pUsbDevice );

        if (ERROR_SUCCESS != dwStatusErr )
        {
            if (ERROR_SUCCESS == dwErr)
                dwErr = dwStatusErr;
        }

    }

    //COS_ExitCriticalSection(&pUsbDevice->Lock);

    return dwErr;
}

// EOF
