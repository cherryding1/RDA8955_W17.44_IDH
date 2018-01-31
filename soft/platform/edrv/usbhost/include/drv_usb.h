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











#ifndef _DRV_USB_H_
#define _DRV_USB_H_



#define ERROR_SUCCESS           0x00
#define ERROR_PERSISTANT        0x01
#define ERROR_INVALID_DRIVE     0x02
#define ERROR_ACCESS_DENIED     0x03
#define ERROR_INVALID_PARAM     0x04
#define ERROR_FAILED            0x05
#define ERROR_NO_RESOURCE       0x06
#define ERROR_TIMEOUT           0x07
#define ERROR_WRITE_PROTECT     0x08
#define ERROR_GEN_FAILURE       0x09

INT32 DRV_OpenUSBDisk(UINT8* dev_name,HANDLE* pdev_handle);
INT32 DRV_CloseUSBDisk(HANDLE dev_handle);
INT32 DRV_ReadUSBDisk(HANDLE dev_handle, UINT32 blk_nr, BYTE* pBuffer);
INT32 DRV_WriteUSBDisk(HANDLE dev_handle, UINT32 blk_nr, CONST BYTE* pBuffer);
INT32 DRV_GetDevInfoUSBDisk( HANDLE dev_handle, UINT32 *pBlockNr, UINT32 *pBlockSize );
VOID  DRV_DeactiveUSBDisk(HANDLE dev_handle);
INT32 DRV_ActiveUSBDisk(HANDLE dev_handle);
VOID DRV_SetStatusSleepInUSBDisk(BOOL bStatus);
UINT32 DRV_GetRCacheSizeUSBDisk(HANDLE dev_handle);
UINT32 DRV_GetWCacheSizeUSBDisk(HANDLE dev_handle);
VOID DRV_SetWCacheSizeUSBDisk(HANDLE dev_handle,UINT32 cache_size);
VOID DRV_SetRCacheSizeUSBDisk(HANDLE dev_handle,UINT32 cache_size);
INT32 DRV_FlushUSBDisk(HANDLE dev_handle);


#endif  // _H_

