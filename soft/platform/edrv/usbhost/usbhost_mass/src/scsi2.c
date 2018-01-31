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
#include "mcd_config.h"
#include "usbhostp_debug.h"
#include "usb.h"
#include "usbmsc.h"
#include "usbmscp.h"
#include "scsi2.h"
#include "global_macros.h"

#define PERQ_LUN_CONNECTED      0x0
#define PERQ_LUN_DISCONNECTED   0x1
#define PERQ_LUN_INVALID        0x3


#define MAX_LIST_LENGTH     512             // Note: USB Mass Storage Spec says this is max 72 bytes 

#define SIZE_PER_SECTOR     512

static SCSI_DEVICE gUsbScsiInst;

uint32 GetMediumInfo(PSCSI_DEVICE pDevice, uint8 Lun);


PSCSI_DEVICE ScsiUsbDiskAttach(void * configuration, uint8 index)
{
    PSCSI_DEVICE pScsi = NULL;
    HANDLE  hUsbTransport;
    uint8   lun = 0;
    uint32  dwErr = 0;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ScsiUsbDiskAttach 0 ");
    hUsbTransport = UsbDiskTransportInit(configuration, index, &lun);
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ScsiUsbDiskAttach UsbDiskTransportInit hUsbTransport =%d ",hUsbTransport);

    if(!hUsbTransport)
    {

        usb_HstSendEvent(0x888abbbb);
        return NULL;
    }
    usb_HstSendEvent(0x88881234);
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ScsiUsbDiskAttach 2 ");
    pScsi = &gUsbScsiInst;  //(PSCSI_DEVICE)sxr_Malloc(sizeof(SCSI_DEVICE));

    /*
    if(pScsi == NULL )
    {
        // release transport first
        return NULL;
    }
    */

    // set state flags
    pScsi->MediumType = SCSI_MEDIUM_UNKNOWN;
    pScsi->DeviceType = SCSI_DEVICE_UNKNOWN;
    pScsi->Flags.MediumPresent = FALSE;
    pScsi->Flags.MediumChanged = TRUE;
    pScsi->Flags.DeviceRemoved = FALSE;
    pScsi->Flags.PoweredDown   = FALSE;

    pScsi->Timeouts.MediaPollInterval  = SCSI_MEDIA_POLL_INTERVAL;
    pScsi->Timeouts.ReadSector         = SCSI_READ_SECTOR_TIMEOUT;
    pScsi->Timeouts.WriteSector        = SCSI_WRITE_SECTOR_TIMEOUT;
    pScsi->Timeouts.ScsiCommandTimeout = SCSI_COMMAND_TIMEOUT;
    pScsi->Timeouts.UnitAttnRepeat     = UNIT_ATTENTION_REPEAT;

    pScsi->hUsbTransport = hUsbTransport;

    dwErr = GetMediumInfo(pScsi, pScsi->Lun);

    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "usb-host: ScsiUsbDiskAttach done dwErr =%d ",dwErr);
    return pScsi;
}

void ScsiUsbDiskDetach(PSCSI_DEVICE pDevice)
{
    if(pDevice)
    {
        UsbDiskTransportDeinit(pDevice->hUsbTransport);
        pDevice->Flags.DeviceRemoved = 1;
    }
}

uint32 GetMediumInfo(PSCSI_DEVICE pDevice, uint8 Lun)
{
    uint32 dwErr = ERROR_SUCCESS;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "GetMediumInfo");

    do
    {
        if (SCSI_DEVICE_UNKNOWN == pDevice->DeviceType || SCSI_MEDIUM_UNKNOWN == pDevice->MediumType )
        {
            dwErr = ScsiUnitAttention(pDevice, Lun);
            if (ERROR_SUCCESS != dwErr)
                break;
        }

        // determine device type
        if (SCSI_DEVICE_UNKNOWN == pDevice->DeviceType)
        {
            usb_HstSendEvent(0x08290012);
            dwErr = ScsiInquiry(pDevice, Lun);
            if (ERROR_SUCCESS != dwErr)
            {
                break;
            }
        }

        // now that mode sense 6 and mode sense 10 is not mandatory, we don't execute this command
        /*
        if (SCSI_DEVICE_UNKNOWN != pDevice->DeviceType && SCSI_MEDIUM_UNKNOWN == pDevice->MediumType )
        {
            dwErr = ScsiModeSense6(pDevice, Lun);
            usb_HstSendEvent(0x09060001);
            if (ERROR_SUCCESS != dwErr)
            {
                TRANSPORT_DATA tData;
                uint8 senseData[18];

                tData.TransferLength = 0;
                tData.RequestLength = sizeof(senseData);
                tData.DataBlock = senseData;
                memset(senseData,0,sizeof(senseData));

                usb_HstSendEvent(0x09060002);
                dwErr = ScsiRequestSense(pDevice, &tData, Lun);

                dwErr = ScsiModeSense10(pDevice, Lun);
                if (ERROR_SUCCESS != dwErr)
                {
                    break;
                }
            }
        }
        */
        pDevice->Flags.MediumPresent = FALSE;
        // determine disk information
        if (!pDevice->Flags.MediumPresent)
        {
            DISK_INFO di = {0};

            dwErr = ScsiReadCapacity(pDevice, &di, Lun);
            usb_HstSendEvent(0x09090024);
            usb_HstSendEvent(dwErr);
            if (ERROR_SUCCESS != dwErr)
            {
                break;
            }
        }

    }
    while (0);

    dwErr = ERROR_SUCCESS;

    return dwErr;
}


//*****************************************************************************
//      S C S I - 2     I N T E R F A C E
//*****************************************************************************
uint32 ScsiPassthrough( PSCSI_DEVICE pDevice, PTRANSPORT_COMMAND pCommand, PTRANSPORT_DATA pData)
{
    uint32 dwErr;

    do
    {
        if ( !pDevice || !pCommand || !pData )
        {
            dwErr = ERROR_INVALID_PARAM;
            break;
        }

        pCommand->dwLun=pDevice->Lun;
        dwErr = UsbsDataTransfer( pDevice->hUsbTransport, pCommand, pData );

    }
    while (0);

    return dwErr;
}


uint32 ScsiUnitAttention(PSCSI_DEVICE pDevice,  uint8 Lun)
{
    uint32 dwRepeat, dwErr, dwStartErr, dwSenseErr;

    dwRepeat = pDevice->Timeouts.UnitAttnRepeat;

    do
    {
        dwErr = ScsiTestUnitReady(pDevice, Lun);
        if (dwErr == ERROR_GEN_FAILURE)
        {
            // break;
            ;
        }

        if (ERROR_ACCESS_DENIED == dwErr || ERROR_INVALID_HANDLE == dwErr)
        {
            // break;
            ;
        }
        else if (ERROR_SUCCESS != dwErr)
        {
            // some devices require a (re)start
            dwStartErr = ScsiStartStopUnit(pDevice, TRUE,  FALSE, Lun);
            if (ERROR_ACCESS_DENIED == dwStartErr || ERROR_INVALID_HANDLE == dwStartErr)
            {
                dwErr = dwStartErr;
                // break;
                ;
            }
            else if (ERROR_SUCCESS != dwStartErr)
            {
                // clear error
                dwSenseErr = ScsiGetSenseData(pDevice, Lun);
                if (ERROR_ACCESS_DENIED == dwSenseErr || ERROR_INVALID_HANDLE == dwSenseErr)
                {
                    dwErr = dwSenseErr;
                    // break;
                    ;
                }
            }

            sxr_Sleep(10);

        }

    }
    while (ERROR_SUCCESS != dwErr && --dwRepeat != 0);

    return dwErr;

}

uint32 ScsiTestUnitReady(PSCSI_DEVICE pDevice, uint8 Lun)
{
    USBMSC_DEVICE *pUsbDevice = (USBMSC_DEVICE *)pDevice->hUsbTransport;
    TRANSPORT_COMMAND tCommand;
    uint8 bCDB[MAX_CDB];

    uint32 dwErr;

    tCommand.Flags   = DATA_OUT;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ? SCSI_CDB_6 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset(bCDB, 0, sizeof(bCDB));
    bCDB[0] = SCSI_TEST_UNIT_READY;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);

    dwErr = UsbsDataTransfer( pDevice->hUsbTransport, &tCommand, NULL );

    if ( dwErr != ERROR_SUCCESS )
    {
        dwErr = ScsiGetSenseData( pDevice, Lun );
    }

    return dwErr;
}


uint32 ScsiRequestSense( PSCSI_DEVICE pDevice, PTRANSPORT_DATA pTData, uint8 Lun)
{
    TRANSPORT_COMMAND tCommand;
    uint8 bCDB[MAX_CDB];
    uint32 dwErr;

    tCommand.Flags   = DATA_IN;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ? SCSI_CDB_6 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset(bCDB,0,sizeof(bCDB));
    bCDB[0] = SCSI_REQUEST_SENSE;

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 : Lun <= 0x7");
    //ASSERT(Lun <= 0x7);
    bCDB[1] = ((Lun & 0x7) << 5);
    bCDB[4] = (uint8)pTData->RequestLength;

    dwErr = UsbsDataTransfer( pDevice->hUsbTransport, &tCommand, pTData );

    return dwErr;
}


//
// Translates SCSI device error to Win32 error.
//
uint32 ScsiGetSenseData(PSCSI_DEVICE pDevice, uint8 Lun)
{
    TRANSPORT_DATA tData;
    uint8 senseData[18];
    uint32 dwErr;
    uint8 SenseKey, ASC, ASCQ;

    tData.TransferLength = 0;
    tData.RequestLength = sizeof(senseData);
    tData.DataBlock = senseData;
    memset(senseData,0,sizeof(senseData));

    dwErr = ScsiRequestSense(pDevice, &tData, Lun);

    // did we receive a valid SenseKey
    if ( ERROR_SUCCESS == dwErr && ((0x70 & senseData[0]) || (0x71 & senseData[0])) && tData.TransferLength >= 13 )
    {
        // ASSERT( senseData[0] & 0x80 );  // Valid bit
        SenseKey = senseData[2] & 0x0f;
        ASC      = senseData[12];
        ASCQ     = senseData[13];

        switch (SenseKey)
        {
            case SENSE_NONE :
            case SENSE_RECOVERED_ERROR :
                dwErr = ERROR_SUCCESS;
                break;

            case SENSE_NOT_READY :
                switch( ASC )
                {
                    case ASC_LUN:
                        if (0x02 == ASCQ)
                        {
                            // Initialization Required
                            ScsiStartStopUnit(pDevice, START, FALSE, pDevice->Lun);
                        }
                        dwErr = ERROR_NOT_READY;
                        break;

                    case ASC_MEDIUM_NOT_PRESENT :
                        //ASSERT(!ASCQ);
                        USBHOST_ASSERT(!ASCQ, "scsi2 : !ASCQ");

                        pDevice->Flags.MediumPresent = FALSE;
                        pDevice->Flags.MediumChanged = TRUE;
                        pDevice->MediumType = SCSI_MEDIUM_UNKNOWN;
                        memset(&pDevice->DiskInfo, 0, sizeof(DISK_INFO) );
                        dwErr = DISK_REMOVED_ERROR;
                        break;

                    default:
                        dwErr = ERROR_NOT_READY;
                        break;
                }
                break;

            case SENSE_MEDIUM_ERROR :
                dwErr = ERROR_UNRECOGNIZED_MEDIA;
                break;

            case SENSE_HARDWARE_ERROR :
                dwErr = ERROR_DISK_OPERATION_FAILED;
                break;

            case SENSE_ILLEGAL_REQUEST :
                dwErr = ERROR_INVALID_PARAM;
                break;

            case SENSE_UNIT_ATTENTION :
                switch( ASC )
                {
                    case ASC_MEDIA_CHANGED :
                        //ASSERT(!ASCQ);
                        USBHOST_ASSERT(!ASCQ, "scsi2 : 1 !ASCQ");

                        pDevice->Flags.MediumPresent = !pDevice->Flags.MediumPresent;
                        pDevice->Flags.MediumChanged = TRUE;
                        pDevice->MediumType = SCSI_MEDIUM_UNKNOWN;

                        memset(&pDevice->DiskInfo, 0, sizeof(DISK_INFO) );
                        break;

                    case ASC_RESET :
                        break;

                    case ASC_COMMANDS_CLEARED :
                    default:
                        break;
                }
                dwErr = DISK_REMOVED_ERROR;
                break;

            case SENSE_DATA_PROTECT :
                dwErr = ERROR_WRITE_PROTECT;
                break;

            default:
                dwErr = ERROR_FLOPPY_UNKNOWN_ERROR;
                break;
        }

    }

    return dwErr;
}

uint32 ScsiInquiry(PSCSI_DEVICE pDevice, uint8 Lun)
{
    TRANSPORT_COMMAND tCommand;
    uint8 bCDB[MAX_CDB];

    TRANSPORT_DATA tData = {0};
    uint8 bDataBlock[36]; // Standard Inquiry Data

    uint32 dwErr = ERROR_SUCCESS;;

    tCommand.Flags   = DATA_IN;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ? SCSI_CDB_6 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset( bCDB, 0, sizeof(bCDB));
    bCDB[0] = SCSI_INQUIRY;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 : 2  Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);

    // EVPB is tbd
    // PageCode = 0
    bCDB[4] = sizeof(bDataBlock); // Allocation Length

    memset( bDataBlock, 0, sizeof(bDataBlock));
    tData.TransferLength = 0;
    tData.RequestLength = sizeof(bDataBlock);
    tData.DataBlock = bDataBlock;

    dwErr = UsbsDataTransfer(pDevice->hUsbTransport, &tCommand, &tData );

    if (dwErr != ERROR_SUCCESS || (tData.TransferLength != tData.RequestLength))
    {
        dwErr = ScsiGetSenseData( pDevice, Lun );
    }
    else
    {

        CHAR  asBuff[17] = {0};
        uint8 perq; // peripherial qualifier

        if (bDataBlock[0] == 0x7F)
        {
            dwErr = ERROR_INVALID_PARAM;
        }
        else
        {
            pDevice->DeviceType = bDataBlock[0] & SCSI_DEVICE_UNKNOWN;

            perq = (bDataBlock[0] & 0xE0) >> 5;
            //ASSERT(PERQ_LUN_INVALID != perq);
            USBHOST_ASSERT(PERQ_LUN_INVALID != perq, "scsi2 :PERQ_LUN_INVALID != perq");

            // currently developing Direct Access & CD-ROM devices only
            //ASSERT(SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType ||SCSI_DEVICE_CDROM == pDevice->DeviceType);
            USBHOST_ASSERT(SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType ||SCSI_DEVICE_CDROM == pDevice->DeviceType, "scsi2 :SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType ||SCSI_DEVICE_CDROM == pDevice->DeviceType");

            pDevice->Flags.RMB = (bDataBlock[1] & 0x80) >> 7;


        }
    }

    return dwErr;
}


uint32 ScsiSendDiagnostic(PSCSI_DEVICE pDevice, uint8 Lun)
{
    TRANSPORT_COMMAND tCommand;
    uint8 bCDB[MAX_CDB];
    uint32 dwErr;

    tCommand.Flags   = DATA_IN;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ?SCSI_CDB_6 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset( bCDB, 0, sizeof(bCDB));
    bCDB[0] = SCSI_SEND_DIAGNOSTIC;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);
    bCDB[1] |= 0x4; // SelfTest

    dwErr = UsbsDataTransfer(pDevice->hUsbTransport, &tCommand, NULL );

    if ( dwErr != ERROR_SUCCESS )
    {
        dwErr = ScsiGetSenseData( pDevice, Lun );
    }

    return dwErr;
}


uint32 ScsiReadCapacity(PSCSI_DEVICE pDevice, PDISK_INFO pDiskInfo, uint8 Lun)
{
    TRANSPORT_COMMAND tCommand;
    uint8             bCDB[MAX_CDB];
    TRANSPORT_DATA tData;
    uint8    bDataBlock[8];
    uint8 * bDataBlock_unca;

    uint32 dwSizeDB = sizeof(bDataBlock);
    uint32 dwSizeDI = sizeof(DISK_INFO);
    uint32 dwErr    = ERROR_SUCCESS;
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "ScsiReadCapacity");
    bDataBlock_unca = MEM_ACCESS_UNCACHED(bDataBlock);
    memset(pDiskInfo, 0, dwSizeDI);
    tCommand.Flags   = DATA_IN;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ?SCSI_CDB_10 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;
    memset( bCDB, 0, sizeof(bCDB));
    bCDB[0] = SCSI_READ_CAPACITY;
    //ASSERT(Lun <= 0x7);
    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);

    // PMI & LBA support is TBD
    // It's useful in determining where longer access times occur

    memset( bDataBlock_unca, 0, dwSizeDB);
    tData.TransferLength = 0;
    tData.RequestLength = dwSizeDB;
    tData.DataBlock = bDataBlock_unca;

    dwErr = UsbsDataTransfer(pDevice->hUsbTransport, &tCommand, &tData );

    if ( dwErr != ERROR_SUCCESS ||(tData.TransferLength != tData.RequestLength) )
    {

        usb_HstSendEvent(0xc9100011); usb_HstSendEvent(dwErr);

        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "ScsiReadCapacity end err dwErr = %d ,tData.TransferLength =%d tData.RequestLength =%d",
                      dwErr,tData.TransferLength,tData.RequestLength);
        dwErr = ScsiGetSenseData( pDevice, Lun );
    }
    else  //  successed
    {
        usb_HstSendEvent(0xc9100010);
        USBHOST_TRACE(USBHOST_INFO_TRC, 0, "ScsiReadCapacity end ok dwErr = %d ,tData.TransferLength =%d tData.RequestLength =%d",
                      dwErr,tData.TransferLength,tData.RequestLength);

        //memcpy(bDataBlock, bDataBlock, sizeof(bDataBlock));
        // Get total sectors from Last Logical Block Address
        pDiskInfo->di_total_sectors  = GetDWORD(&bDataBlock_unca[0])+1;
        // Block Length in bytes
        pDiskInfo->di_bytes_per_sect = GetDWORD(&bDataBlock_unca[4]);
        pDiskInfo->di_flags |= DISK_INFO_FLAG_PAGEABLE | DISK_INFO_FLAG_CHS_UNCERTAIN;

        if ( pDiskInfo->di_bytes_per_sect && pDiskInfo->di_total_sectors)
        {
            // update our media info & flags
            pDevice->Flags.MediumPresent = TRUE;

            if ( 0 != memcmp(&pDevice->DiskInfo, pDiskInfo, dwSizeDI) )
            {
                memcpy(&pDevice->DiskInfo, pDiskInfo, dwSizeDI);
            }
        }

    }
    //sxr_Free(pDataBlock);
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "ScsiReadCapacity end dwErr = %d",dwErr);
    if(dwErr == 0) usb_HstSendEvent(0x78888888);
    usb_HstSendEvent(pDevice->DiskInfo.di_bytes_per_sect);

    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "ScsiReadCapacity  pDevice di_total_sectors= %d	di_bytes_per_sect =%d  all = %d M",
                  pDiskInfo->di_total_sectors,pDiskInfo->di_bytes_per_sect,
                  (pDiskInfo->di_total_sectors/1024*(pDiskInfo->di_bytes_per_sect))/1024);
    USBHOST_TRACE(USBHOST_INFO_TRC, 0, "ScsiReadCapacity  pDevice di_total_sectors= %d	di_bytes_per_sect =%d  all = %d M",
                  pDevice->DiskInfo.di_total_sectors,pDevice->DiskInfo.di_bytes_per_sect,
                  (pDevice->DiskInfo.di_total_sectors/1024*(pDevice->DiskInfo.di_bytes_per_sect))/1024);

    return dwErr;

}



// Mode sense is dependant on Device Type.
uint32 ScsiModeSense10(PSCSI_DEVICE pDevice, uint8 Lun)
{
    TRANSPORT_DATA    tData;
    TRANSPORT_COMMAND tCommand;
    uint8 bCDB[MAX_CDB];

    uint8 bDataBlock[MAX_LIST_LENGTH] = {0};  // Standard Header + mode pages
    uint16 usPageLength = 8;                   // Standard Header size

    uint32 dwErr = ERROR_SUCCESS;

    //ASSERT( SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType || SCSI_DEVICE_CDROM == pDevice->DeviceType );
    USBHOST_ASSERT(SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType || SCSI_DEVICE_CDROM == pDevice->DeviceType, "SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType || SCSI_DEVICE_CDROM == pDevice->DeviceType");

    memset( bCDB, 0, sizeof(bCDB));

    tCommand.Flags   = DATA_IN;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ?SCSI_CDB_10 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    bCDB[0] = SCSI_MODE_SENSE10;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);
    // bCDB[1] |= 0x8;      // DBD  Note: USB Mass Storage Spec says this is 0

    // PC = current
    // Page Code = ALL
    bCDB[2] = 0x3f;         // this was original set to 0 but 0 is not defined in the USB Mass Storage Spec
    //      some devices return ILLEGAL REQUEST. So use some defines value as default

    switch (pDevice->DeviceType)
    {
        case SCSI_DEVICE_DIRECT_ACCESS:
            if (pDevice->DiskSubClass == USBMSC_SUBCLASS_UFI)
            {
                bCDB[2] = MODE_PAGE_FLEXIBLE_DISK;
                usPageLength += 32;
            }
            else
            {
                usPageLength = sizeof(bDataBlock);
            }
            break;

        default:
            usPageLength = sizeof(bDataBlock);
            break;
    }

    bCDB[7] = (sizeof(bDataBlock) & 0xFF00) >> 8; // MSB
    bCDB[8] =  sizeof(bDataBlock) & 0x00FF;       // LSB

    memset( bDataBlock, 0, sizeof(bDataBlock));
    //ASSERT( usPageLength <= sizeof(bDataBlock));
    USBHOST_ASSERT(usPageLength <= sizeof(bDataBlock), "scsi2 :usPageLength <= sizeof(bDataBlock)");

    tData.TransferLength = 0;
    // Note: some devices fail with only 8 byte header request length,
    // but all should recover with extra buffer space.
    tData.RequestLength = usPageLength;
    tData.DataBlock = bDataBlock;

    dwErr = UsbsDataTransfer( pDevice->hUsbTransport, &tCommand, &tData );

    if ( dwErr != ERROR_SUCCESS || tData.TransferLength < 8 )
    {
        // want at least the header
        dwErr = ScsiGetSenseData( pDevice, Lun );
    }
    else
    {

        //COS_EnterCriticalSection(&pDevice->Lock);

        // look at the Header
        //DEBUGMSG(ZONE_SCSI,(TEXT("Medium Type:0x%x\n"), bDataBlock[2]));

        pDevice->MediumType = bDataBlock[2];

        // bit7 is WP bit
        pDevice->Flags.WriteProtect = bDataBlock[3] & 0x80;

        //COS_ExitCriticalSection(&pDevice->Lock);

        //
        // TBD: look at the the requested page ...
        //
    }

    return dwErr;
}

uint32 ScsiModeSense6(PSCSI_DEVICE pDevice, uint8 Lun)
{
    TRANSPORT_DATA    tData;
    TRANSPORT_COMMAND tCommand;
    uint8             bCDB[SCSI_CDB_6];
    uint8             bDataBlock[MAX_LIST_LENGTH] = {0}; // standard header + mode pages
    uint16            usPageLength = 8;                  // standard header size
    uint32             dwErr = ERROR_SUCCESS;

    //ASSERT(SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType || SCSI_DEVICE_CDROM == pDevice->DeviceType);
    USBHOST_ASSERT(SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType || SCSI_DEVICE_CDROM == pDevice->DeviceType, "scsi2 :SCSI_DEVICE_DIRECT_ACCESS == pDevice->DeviceType || SCSI_DEVICE_CDROM == pDevice->DeviceType");

    memset(bCDB, 0, sizeof(bCDB));
    tCommand.Flags = DATA_IN;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ? SCSI_CDB_6 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun = Lun;

    bCDB[0] = SCSI_MODE_SENSE6;
    bCDB[1] = ((Lun & 0x7) << 5);
    bCDB[2] = 0x3f;

    switch (pDevice->DeviceType)
    {
        case SCSI_DEVICE_DIRECT_ACCESS:
            if (pDevice->DiskSubClass == USBMSC_SUBCLASS_UFI)
            {
                bCDB[2] = MODE_PAGE_FLEXIBLE_DISK;
                usPageLength += 32;
            }
            else
            {
                usPageLength = sizeof(bDataBlock);
            }
            break;
        default:
            usPageLength = sizeof(bDataBlock);
            break;
    }
    usPageLength = 8;

    memset(bDataBlock, 0, sizeof(bDataBlock));

    // a device may fail this command if the header request length is only 8 bytes;
    // if this is the case, then the device should recover with extra buffer space

    tData.TransferLength = 0;
    tData.RequestLength = usPageLength;
    tData.DataBlock = bDataBlock;

    dwErr = UsbsDataTransfer(pDevice->hUsbTransport, &tCommand, &tData);

    if (dwErr != ERROR_SUCCESS || tData.TransferLength < 8 )
    {
        // we want at least the header
        usb_HstSendEvent(0x09090023);
        dwErr = ScsiGetSenseData(pDevice, Lun);
    }
    else
    {
        usb_HstSendEvent(0x09090020);
        pDevice->MediumType = bDataBlock[2];
        pDevice->Flags.WriteProtect = bDataBlock[3] & 0x80; // inspect WP bit
    }
    return dwErr;
}

uint32 ScsiStartStopUnit(PSCSI_DEVICE pDevice, BOOL Start,  BOOL LoEj, uint8 Lun)
{
    TRANSPORT_COMMAND tCommand;
    uint8 bCDB[MAX_CDB];
    uint32 dwErr;

    tCommand.Flags   = DATA_OUT;
    tCommand.Timeout = pDevice->Timeouts.ScsiCommandTimeout;
    tCommand.Length  = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ?SCSI_CDB_6 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset( bCDB, 0, sizeof(bCDB));
    bCDB[0] = SCSI_START_STOP;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);

    bCDB[4] = (LoEj & 0x1) << 1;
    bCDB[4] |= Start & 0x1;

    dwErr = UsbsDataTransfer(pDevice->hUsbTransport, &tCommand, NULL );

    if ( dwErr != ERROR_SUCCESS )
    {
        dwErr = ScsiGetSenseData( pDevice, Lun );
    }

    return dwErr;
}


/*++

    Complete a Scatter/Gather request on a SCSI device.

    Returns: If fail, Win32 error.
             If pass, total number of bytes transferred.
                 Completion status returned in pSqReq->sr_status.
--*/
uint32 ScsiRWSG( PSCSI_DEVICE pDevice, uint8* pBuf, uint32 nstartSec, uint8 Lun, uint8 bRead, int * pStatus)
{
    uint32 uiExpectedBytesXferred = 0; // Expected total size of transfer (bytes).
    uint32 uiActualBytesXferred = 0;   // Actual total size of transfer (bytes).
    uint32 dwFlags = 0;               // Flags indicating need for double buffering.
    uint32 dwErr = ERROR_SUCCESS;                     // Error code.
    uint32 uiBytesXferred = SIZE_PER_SECTOR;                          // Number of bytes to transfer in current sub-transfer.
    uint32 sg;                                          // Index of current Scatter/Gather buffer.

    if (dwFlags == 0)
    {
        // TRANSFER EACH SCATTER/GATHER BUFFER SEPARATELY.
        //
        {

            //
            // Transfer the contiguous sectors in the current
            //     Scatter/Gather buffer.
            // (Recall) Double buffer is not required, as all buffers are
            //     sector aligned.
            //
            usb_HstSendEvent(0x09089a11);
            dwErr = ScsiReadWrite(pDevice, nstartSec, 1, pBuf, &uiBytesXferred, Lun,  bRead);
            *pStatus = dwErr;

            hal_SysInvalidateCache(pBuf, 512);
            if(dwErr == ERROR_SUCCESS)
                return 512; //  gUsbScsiInst.DiskInfo.di_bytes_per_sect
            else
                return 0;

        }
    }

    return uiActualBytesXferred;
}

/* ++

ScsiReadWrite:

    Transfers full sectors.

-- */
#if 1
uint32 ScsiReadWrite(PSCSI_DEVICE pDevice, uint32 dwStartSector, uint32 dwNumSectors, void* pvBuffer, uint32* pdwTransferSize,  uint8 Lun, BOOL bRead)
{
    TRANSPORT_COMMAND tCommand;
    uint8             bCDB[MAX_CDB];
    TRANSPORT_DATA    tData;
    uint32             dwErr;
    uint32             dwSectorBytes;

    //usb_HstSendEvent(0x09140001);
    //usb_HstSendEvent(pDevice->DiskInfo.di_bytes_per_sect);
    if (!pDevice || !pDevice->DiskInfo.di_bytes_per_sect || !pvBuffer || !pdwTransferSize)
    {
        return ERROR_INVALID_PARAM;
    }

    //usb_HstSendEvent(0x09140002);
    // buffer size limitations.
    if (0 == *pdwTransferSize || !dwNumSectors)
    {
        return ERROR_INVALID_PARAM;
    }

    //usb_HstSendEvent(0x09140003);
    dwSectorBytes = dwNumSectors * pDevice->DiskInfo.di_bytes_per_sect;

    // Transfer buffers must be large enough to fit the requested # sectors
    if (*pdwTransferSize < dwSectorBytes)
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }
    //usb_HstSendEvent(0x09140004);

    tCommand.Flags = bRead ? DATA_IN : DATA_OUT;
    tCommand.Timeout = dwNumSectors * (bRead ? pDevice->Timeouts.ReadSector : pDevice->Timeouts.WriteSector);
    tCommand.Length = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ? SCSI_CDB_10 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset(bCDB, 0, sizeof(bCDB) );
    bCDB[0] = bRead ? SCSI_READ10 : SCSI_WRITE10;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);

    // Logical Block Address
    SetDWORD( &bCDB[2], dwStartSector);

    // TransferLength (in sectors)
    SetWORD( &bCDB[7], (WORD)dwNumSectors);

    tData.TransferLength = 0;
    tData.RequestLength  = dwSectorBytes;
    tData.DataBlock =MEM_ACCESS_UNCACHED( pvBuffer);


    //do
    {
        // since we do retries we also have to (re)poke the device
        //dwErr = ScsiUnitAttention(pDevice, pDevice->Lun);
        //if (ERROR_SUCCESS != dwErr)
        //break;

        usb_HstSendEvent(0x10140002);
        dwErr = UsbsDataTransfer( pDevice->hUsbTransport,  &tCommand, &tData );
        usb_HstSendEvent(0x09089012);

        if ( ERROR_SUCCESS == dwErr )
        {
            *pdwTransferSize = tData.TransferLength;
        }
        else
        {
            //dwErr = ScsiGetSenseData(pDevice, Lun);

            //if (ERROR_SUCCESS == dwErr)
            dwErr = ERROR_GEN_FAILURE;

            *pdwTransferSize = 0;
        }

    }// while (0);

    return dwErr;

}
#else


/* ++

ScsiReadWrite:

    Transfers full sectors.

-- */
uint32 ScsiReadWrite(PSCSI_DEVICE pDevice, uint32 dwStartSector, uint32 dwNumSectors, void* pvBuffer, uint32* pdwTransferSize,  uint8 Lun, BOOL bRead)
{
    TRANSPORT_COMMAND tCommand;
    uint8             bCDB[MAX_CDB];
    TRANSPORT_DATA    tData;
    uint32             dwErr;
    uint32             dwSectorBytes;

    //usb_HstSendEvent(0x09140001);
    //usb_HstSendEvent(pDevice->DiskInfo.di_bytes_per_sect);
    if (!pDevice || !pDevice->DiskInfo.di_bytes_per_sect || !pvBuffer || !pdwTransferSize)
    {
        return ERROR_INVALID_PARAM;
    }

    //usb_HstSendEvent(0x09140002);
    // buffer size limitations.
    if (0 == *pdwTransferSize || !dwNumSectors)
    {
        return ERROR_INVALID_PARAM;
    }

    //usb_HstSendEvent(0x09140003);
    dwSectorBytes = dwNumSectors * pDevice->DiskInfo.di_bytes_per_sect;

    // Transfer buffers must be large enough to fit the requested # sectors
    if (*pdwTransferSize < dwSectorBytes)
    {
        return ERROR_INSUFFICIENT_BUFFER;
    }
    //usb_HstSendEvent(0x09140004);

    tCommand.Flags = bRead ? DATA_IN : DATA_OUT;
    tCommand.Timeout = dwNumSectors * (bRead ? pDevice->Timeouts.ReadSector : pDevice->Timeouts.WriteSector);
    tCommand.Length = USBMSC_SUBCLASS_SCSI == pDevice->DiskSubClass ? SCSI_CDB_10 : UFI_CDB;
    tCommand.CommandBlock = bCDB;
    tCommand.dwLun=Lun;

    memset(bCDB, 0, sizeof(bCDB) );
    bCDB[0] = bRead ? SCSI_READ10 : SCSI_WRITE10;

    //ASSERT(Lun <= 0x7);

    USBHOST_ASSERT(Lun <= 0x7, "scsi2 :Lun <= 0x7");
    bCDB[1] = ((Lun & 0x7) << 5);

    // Logical Block Address
    SetDWORD( &bCDB[2], dwStartSector);

    // TransferLength (in sectors)
    SetWORD( &bCDB[7], (WORD)dwNumSectors);

    tData.TransferLength = 0;
    tData.RequestLength  = dwSectorBytes;
    tData.DataBlock =MEM_ACCESS_UNCACHED( pvBuffer);


    do
    {
        // since we do retries we also have to (re)poke the device
        //dwErr = ScsiUnitAttention(pDevice, pDevice->Lun);
        //if (ERROR_SUCCESS != dwErr)
        //break;

        usb_HstSendEvent(0x10140002);
        dwErr = UsbsDataTransfer( pDevice->hUsbTransport,  &tCommand, &tData );
        usb_HstSendEvent(0x09089012);

        if ( ERROR_SUCCESS == dwErr )
        {
            *pdwTransferSize = tData.TransferLength;
        }
        else
        {
            dwErr = ScsiGetSenseData(pDevice, Lun);

            if (ERROR_SUCCESS == dwErr)
                dwErr = ERROR_GEN_FAILURE;

            *pdwTransferSize = 0;
        }

    }
    while (0);

    return dwErr;

}

#endif
// EOF
