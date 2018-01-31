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

#ifndef _USERGEN_FLS_H
#define _USERGEN_FLS_H
/*
typedef struct _drv_flash_dev_info {
  UINT32 block_size;
  UINT32 sector_size;
  UINT32 total_size;
  BYTE* pBaseAddr;
}DRV_FLASH_DEV_INFO;
*/
#define VFLASH_BASE_ADDR 0x00000000
//INT32 DRV_EraseAllUserDataSector(VOID);
UINT32 DRV_FlashPowerUp(VOID);
UINT32 DRV_FlashPowerDown(VOID);
BOOL DRV_FlashInit(VOID);
UINT32 DRV_ReadFlash (
    UINT32 ulAddrOffset,
    BYTE *pBuffer,
    UINT32 nNumberOfBytesToRead,
    UINT32 *pNumberOfBytesRead
);
UINT32 DRV_WriteFlash( UINT32 ulAddrOffset,
                       CONST BYTE *pBuffer,
                       UINT32 nNumberOfBytesToWrite,
                       UINT32 *pNumberOfBytesWritten
                     );
UINT32 DRV_EraseFlashSector (
    UINT32 ulAddrOffset
);
UINT32 DRV_EraseAllUserDataSector(VOID);
VOID DRV_FlashGetUserFieldInfo(
    UINT32 *userFieldBase,
    UINT32 *userFieldSize,
    UINT32 *sectorSize,
    UINT32 *blockSize
);
#define memd_FlashRead DRV_ReadFlash
#define memd_FlashWrite DRV_WriteFlash
#define memd_FlashErase DRV_EraseFlashSector
#endif
