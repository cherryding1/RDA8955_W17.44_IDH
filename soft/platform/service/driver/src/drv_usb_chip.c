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

#if (USB_HOST_SUPPORT ==1)
#include "cs_types.h"
#include "ts.h"
#include "drv_usb.h"
#include "chip_id.h"
#include "sxr_tls.h"
#include "hal_debug.h"
#include "sxr_tim.h"
#include "chip_id.h"
#include "pmd_m.h"
#include "mcd_usb.h"
#include "scsi2.h"


#define ERR_SUCCESS             0x0 //#include <errorcode.h>


#define MASS_INTERFACE_CLASS        0x08    // storage class
#define MASS_INTERFACE_SUBCLASS     0x06    // SCSI command set
#define USB_BLOCK_CACHE             0x200

typedef struct tagUsbDiskInst
{
    UINT8           udiskReady;
    PSCSI_DEVICE    pScsiDevice;
    //  UINT8           cache[USB_BLOCK_CACHE];
} USBDISKINST;

volatile USBDISKINST  gUsbDiskInst;


// ytt  TODO
void  UsbDiskAttach(void *confiuration, UINT8 index)
{

    hal_HstSendEvent(0x09060007);
    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(0) , TSTR("usb-host: UsbDiskAttach ",0x0830003b));
#if 1
    PSCSI_DEVICE  pScsiDevice = ScsiUsbDiskAttach(confiuration, index);
    gUsbDiskInst.pScsiDevice = pScsiDevice;
    gUsbDiskInst.udiskReady = 1;

#else
    COS_EVENT ev;
    PSCSI_DEVICE  pScsiDevice = ScsiUsbDiskAttach(confiuration, index);

    if( pScsiDevice == NULL)
    {
        return ;
    }

    gUsbDiskInst.pScsiDevice = pScsiDevice;
    gUsbDiskInst.udiskReady = 1;

    hal_HstSendEvent(0x09060007);

    /* notify filesystem, usb disk is ready */
    ev.nEventId = EV_FS_UDISK_READY;
    ev.nParam1 = 0;
    COS_SendEvent(MOD_APP, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
#endif
}


void UsbDiskDetach(void *param)
{
    hal_HstSendEvent(0x10140006);

    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(0) , TSTR("usb-host: UsbDiskAttach ",0x0830003c));
    gUsbDiskInst.udiskReady = 0;
    hal_HstSendEvent(gUsbDiskInst.udiskReady);
    ScsiUsbDiskDetach(gUsbDiskInst.pScsiDevice);
}

static int USBDisk_ReadWrite(UINT8 *pBuf, UINT32 blk_nr, UINT32 *pdwBytesTransferred, UINT8 bRead)
{
    int dwErr = ERROR_SUCCESS;
    PSCSI_DEVICE pDevice = gUsbDiskInst.pScsiDevice;

    if( gUsbDiskInst.udiskReady == 0)
    {
        return  ERROR_FAILED;
    }

    //hal_HstSendEvent(0x09089001);
    if (!bRead && pDevice->Flags.WriteProtect)
    {
        dwErr = ERROR_WRITE_PROTECT;
    }
    else
    {
        //dwErr = ScsiUnitAttention(pDevice, pDevice->Lun);
        //hal_HstSendEvent(0x09090031);
        //hal_HstSendEvent(dwErr);
        if(ERROR_SUCCESS == dwErr)
        {
            pDevice->Flags.MediumChanged = FALSE;
            {
                UINT32 dwBytesTransferred = ScsiRWSG(pDevice, pBuf, blk_nr, pDevice->Lun, bRead, &dwErr);
                //hal_HstSendEvent(0x09100001);
                //hal_HstSendEvent(pBuf);
                if (pdwBytesTransferred)
                    *pdwBytesTransferred = dwBytesTransferred;
            }
        }
        else if (pDevice->Flags.MediumPresent == FALSE)
        {
            dwErr = ERROR_INVALID_PARAM;
        }
    }
    return dwErr;
}


// returns number of bytes written in the pDiskInfo
static UINT32 GetDiskInfo(PSCSI_DEVICE pDevice, DISK_INFO *pDiskInfo,  UINT8 Lun)
{
    UINT32 dwErr;
    UINT32 dwSize = sizeof(DISK_INFO);
    UINT32 dwRepeat = 1;

    hal_HstSendEvent(0x09089014);
    dwErr = ScsiUnitAttention(pDevice, Lun);
    if (dwErr == ERROR_GEN_FAILURE)
    {
        return 0;
    }

    if ( ERROR_SUCCESS == dwErr )
    {
        dwRepeat = pDevice->Timeouts.UnitAttnRepeat;

        do
        {
            dwErr = ScsiReadCapacity(pDevice, pDiskInfo, Lun);
            dwRepeat--;

        }
        while (ERROR_SUCCESS != dwErr && dwRepeat != 0 );
    }

    if ( ERROR_SUCCESS != dwErr || 0 == pDiskInfo->di_total_sectors || 0 == pDiskInfo->di_bytes_per_sect )
    {
        dwSize = 0;
    }

    return dwSize;
}


//-----------------------------------------------------------------------------------------
// DRV_OpenUSBDisk
//  Open the device.
// @dev_name[in]
//  USBDisk device name.
// @pdev_handle[out]
//   Output device handle.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//    else return error code.
//-----------------------------------------------------------------------------------------
INT32 DRV_OpenUSBDisk(UINT8 *dev_name, HANDLE *pdev_handle)
{
    USBCLIENTDRIVER drvInst;
    drvInst.InterfaceClass = MASS_INTERFACE_CLASS;
    drvInst.InterfaceSubClass = MASS_INTERFACE_SUBCLASS;
    drvInst.UsbDeviceAttach = UsbDiskAttach;
    drvInst.UsbDeviceDetach = UsbDiskDetach;

    hal_HstSendEvent(0x08150002);
    USBHOST_RegisterClientDriver(&drvInst);

    return ERR_SUCCESS;
}


//-----------------------------------------------------------------------------------------
// DRV_CloseUSBDisk
//  Close the device.
// @pdev_handle[out]
//   Device handle.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//    else return error code.
//-----------------------------------------------------------------------------------------
INT32 DRV_CloseUSBDisk(HANDLE dev_handle)
{
    return ERR_SUCCESS;
}


//-----------------------------------------------------------------------------------------
// DRV_FlushUSBDisk
//   Flush the cache.
// @pdev_handle[out]
//   Device handle.
// Return
//   Upon successful completion, return ERR_SUCCESS.
//   If flush failed, shall return err code.
//-----------------------------------------------------------------------------------------
INT32 DRV_FlushUSBDisk(HANDLE dev_handle)
{
    return ERR_SUCCESS;
}



//-----------------------------------------------------------------------------------------
// DRV_ReadUSBDisk
//  Read a specifically block.
// @dev_handle[in]
//  Device handle.
// @blk_nr[in]
//  Block number.it range 0 -  max block number.
// @pBuffer[out]
//  Output the  readed content.
// Return
//  Upon successful completion, return ERR_SUCCESS.
//   If read block failed, shall return err code.
//-----------------------------------------------------------------------------------------

INT32 DRV_ReadUSBDisk(HANDLE dev_handle, UINT32 blk_nr, BYTE *pBuffer)
{
    UINT32 nBytesRead = 0;
    INT32 status;
    status = USBDisk_ReadWrite(pBuffer, blk_nr, &nBytesRead, 1);
    return status;
    //memcpy(pBuffer, HAL_SYS_GET_UNCACHED_ADDR(pBuffer), USB_BLOCK_CACHE);
}


//-----------------------------------------------------------------------------------------
// DRV_WriteUSBDisk
//  Write a specifically block.
// @dev_handle[in]
//  Device handle.
// @blk_nr[in]
//   Block number.it range 0 -  max block number
// @pBuffer[in]
//  Points to the buffer in which the data to be written is stored.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//   If write block failed, shall return err code.
//-----------------------------------------------------------------------------------------
INT32 DRV_WriteUSBDisk (HANDLE dev_handle, UINT32 blk_nr, CONST BYTE *pBuffer)
{
    UINT32 nBytesWrite = 0;

    return USBDisk_ReadWrite(pBuffer, blk_nr, &nBytesWrite, 0);
}

//-----------------------------------------------------------------------------------------
// DRV_GetDevInfoUSBDisk
// Get device information.
// @dev_handle[in]
// Device handle.
// @pNrBlock[out]
// output put the max block number.
// @pBlockSize[out]
// output block size.
// Return
// Upon successful completion, return ERR_SUCCESS,
// If get device informatioin failed, shall return err code.
//-----------------------------------------------------------------------------------------
INT32 DRV_GetDevInfoUSBDisk (HANDLE dev_handle, UINT32 *pBlockNr, UINT32 *pBlockSize)
{
    UINT32 dwSize = sizeof(DISK_INFO);
    UINT32 dwErr = ERR_SUCCESS;
    PSCSI_DEVICE pDevice = gUsbDiskInst.pScsiDevice;//((USBDISKINST*)dev_handle)->pScsiDevice;

    hal_HstSendEvent(0x09089002);
    if ( !pBlockNr || !pBlockSize )
    {
        dwErr = ERROR_INVALID_PARAM;
    }
    else
    {
        // return cached disk geometry, if possible, otherwise,
        // query device
        if (pDevice->Flags.MediumPresent &&
                0 != pDevice->DiskInfo.di_total_sectors &&
                0 != pDevice->DiskInfo.di_bytes_per_sect)
        {
            *pBlockNr = pDevice->DiskInfo.di_total_sectors;
            *pBlockSize = pDevice->DiskInfo.di_bytes_per_sect;
        }
        else
        {
            UINT32 dwBytesTransferred;
            DISK_INFO diskInfo;

            dwBytesTransferred = GetDiskInfo(pDevice, &diskInfo, pDevice->Lun);
            if (dwSize == dwBytesTransferred)
            {
                // the device returned geometry data, thus, a medium
                // is present
                pDevice->Flags.FSDMounted = TRUE;
                pDevice->Flags.MediumChanged = FALSE;
                *pBlockNr = diskInfo.di_total_sectors;
                *pBlockSize = diskInfo.di_bytes_per_sect;
            }
            else
            {
                if (pDevice->Flags.MediumPresent == FALSE)
                {
                    //DEBUGMSG(ZONE_ERR, (TEXT("Usbdisk6!DSK_IOControl> IOCTL_GET_INFO; media not present\r\n")));
                    *pBlockNr = 0;
                    *pBlockSize = 0;
                    dwErr = ERROR_FAILED;
                }
            }

        }
    }
    return dwErr;
}


//-----------------------------------------------------------------------------------------
//DRV_DeactiveUSBDisk
//   Deactive the device.
// @dev_handle[in]
//  Device handle.
//Return
//  VOID
//-----------------------------------------------------------------------------------------
VOID DRV_DeactiveUSBDisk(HANDLE dev_handle)
{
    hal_HstSendEvent(0x09089003);
    return;
}


//-----------------------------------------------------------------------------------------
//DRV_ActiveUSBDisk
// Active the device.
// @dev_handle[in]
//  Device handle.
// Return
// See the description abount of drv_OpenDeviceUSBDisk().
//-----------------------------------------------------------------------------------------
INT32 DRV_ActiveUSBDisk(HANDLE dev_handle)
{
    hal_HstSendEvent(0x09089004);
    return ERR_SUCCESS;
}


//-----------------------------------------------------------------------------------------
//DRV_SetStatusSleepIn
// Set LCD status.
// @dev_handle[in]
//  Device handle.
// Return
// VOID
//-----------------------------------------------------------------------------------------
VOID DRV_SetStatusSleepInUSBDisk(BOOL dev_status)
{
    hal_HstSendEvent(0x09089005);
}


//-----------------------------------------------------------------------------------------
//DRV_GetRCacheSizeUSBDisk
//Get the size of rcache buffer.
//@dev_handle[in]
//  Device handle.
//Return
//   return size of rcache buffer.
//-----------------------------------------------------------------------------------------
UINT32 DRV_GetRCacheSizeUSBDisk (HANDLE dev_handle)
{
    hal_HstSendEvent(0x09089006);
    return 0;
}


//-----------------------------------------------------------------------------------------
//DRV_SetRCacheSizeUSBDisk
//Set the size of rcache buffer.
//@dev_handle[in]
//  Device handle.
//@cache_size[in]
//   Size of rcache buffer.
//Return Value:
//   VOID
//-----------------------------------------------------------------------------------------
VOID DRV_SetRCacheSizeUSBDisk(HANDLE dev_handle, UINT32 cache_size)
{
    hal_HstSendEvent(0x09089007);
}


//-----------------------------------------------------------------------------------------
//DRV_GetWCacheSizeUSBDisk
//Get the size of wcache buffer.
//@dev_handle[in]
//  Device handle.
//Return
//   return size of wcache buffer.
//-----------------------------------------------------------------------------------------
UINT32 DRV_GetWCacheSizeUSBDisk(HANDLE dev_handle)
{
    hal_HstSendEvent(0x09089008);
    return 0;
}


//-----------------------------------------------------------------------------------------
//DRV_SetWCacheSizeUSBDisk
//Set the size of wcache buffer.
//@dev_handle[in]
//  Device handle.
//@cache_size[in]
//   Size of wcache buffer.
//Return Value:
//   VOID
//-----------------------------------------------------------------------------------------
VOID DRV_SetWCacheSizeUSBDisk(HANDLE dev_handle, UINT32 cache_size)
{
    hal_HstSendEvent(0x09089009);
}


extern VOID pm_USBDiskDetect(BOOL cardPlugged);

VOID USBPlugHandler(BOOL cardPlugged)
{
#ifdef _T_UPGRADE_PROGRAMMER
    cardPlugged = cardPlugged;
#else // _T_UPGRADE_PROGRAMMER
#ifndef _MS_VC_VER_
    pm_USBDiskDetect(cardPlugged);
#endif
#endif // _T_UPGRADE_PROGRAMMER
}

void McdSetUSBDetectHandler(void)
{
    USBHOST_SetUSBDiskDetectHandler((USBHOST_CARD_DETECT_HANDLER_T)USBPlugHandler );
}




void  OpenUSBDisk_test( void)
{
    USBCLIENTDRIVER drvInst;
    drvInst.InterfaceClass = MASS_INTERFACE_CLASS;
    drvInst.InterfaceSubClass = MASS_INTERFACE_SUBCLASS;
    drvInst.UsbDeviceAttach = UsbDiskAttach;
    drvInst.UsbDeviceDetach = UsbDiskDetach;

    hal_HstSendEvent(0x08150002);
    USBHOST_RegisterClientDriver(&drvInst);

}

UINT8 buff_usb_w[512];
UINT8 buff_usb_r[512];
void Test_UsbDisk2(void)

{
    u32 i, k = 0, n = 0, count = 0;
    INT32 ret;
    for(n = 0; n < 512; n++) buff_usb_w[n] = n;



    count++;
    ret = DRV_WriteUSBDisk(1, 3 * 1024, buff_usb_w);
    hal_HstSendEvent(0xeeee0001);
    hal_HstSendEvent(ret);

    //ret = DRV_WriteUSBDisk(1,4*1024,buff_usb_w);
    //hal_HstSendEvent(0xeeeea001);hal_HstSendEvent(ret);
    memset(buff_usb_r, 0xa, 512);
    ret = DRV_ReadUSBDisk(1, 3 * 1024, buff_usb_r);
    hal_HstSendEvent(0xeeee0002);
    hal_HstSendEvent(ret);

    ret = DRV_ReadUSBDisk(1, 3 * 1024, buff_usb_r);
    hal_HstSendEvent(0xeeee0002);
    hal_HstSendEvent(ret);
    for(i = 0; i < 512; i++)
    {
        if(buff_usb_w[i] != buff_usb_r[i])
        {

            hal_HstSendEvent(0xeeee0000);
            hal_HstSendEvent(buff_usb_r[i]);
            k++;
            hal_HstSendEvent(buff_usb_w[i]);
            if(k > 12) break;
        }
    }
    hal_HstSendEvent(0xeeeeffff);
    hal_HstSendEvent(k);
    hal_HstSendEvent(buff_usb_w);
    hal_HstSendEvent(buff_usb_r);

    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR("usb-host: Test_UsbDisk err = %d ",0x0830003d), k);


}

void Test_UsbDisk(void)
{
    u32 i, k = 0, n = 0, count = 0;
    INT32 ret;
    for(n = 0; n < 512; n++) buff_usb_w[n] = n;

    //ret = DRV_WriteUSBDisk(1,4*1024,buff_usb_w);
    hal_HstSendEvent(0xeeee0001);
    hal_HstSendEvent(ret);
    while(count < 30)
    {
        count++;
        memset(buff_usb_r, 0xa, 512);
        sxr_Sleep(163);
        ret = DRV_ReadUSBDisk(1, 4 * 1024, buff_usb_r);
        if(ERROR_SUCCESS != ret)
        {
            hal_DbgAssert( "DRV_ReadUSBDisk error ");
        };
        hal_HstSendEvent(0xeeee0002);
        hal_HstSendEvent(ret);
        for(i = 0; i < 512; i++)
        {
            if(buff_usb_w[i] != buff_usb_r[i])
            {

                hal_HstSendEvent(0xeeee0000);
                hal_HstSendEvent(buff_usb_r[i]);
                k++;
                hal_HstSendEvent(buff_usb_w[i]);
                if(k > 4) break;
            }
        }
        hal_HstSendEvent(0xeeeeffff);
        hal_HstSendEvent(k);
        hal_HstSendEvent(buff_usb_w);
        hal_HstSendEvent(buff_usb_r);
        sxr_Sleep(40);
    }
    hal_HstSendEvent(0x888a888);
    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR("usb-host: Test_UsbDisk err = %d ",0x0830003e), k);


}

#endif
