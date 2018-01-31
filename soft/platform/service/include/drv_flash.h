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












#ifndef _DRV_FLASH_H_
#define _DRV_FLASH_H_

typedef struct _DRV_FLASH_DEV_INFO
{
    UINT32 block_size;
    UINT32 sector_size;
    UINT32 total_size;
    BYTE *pBaseAddr;
} DRV_FLASH_DEV_INFO;


UINT32 SRVAPI DRV_FlashPowerUp (
    VOID
);

UINT32 SRVAPI DRV_FlashPowerDown (
    VOID
);

BOOL DRV_FlashInit(
    VOID
);

UINT32 DRV_FlashRegionMap(
    DRV_FLASH_DEV_INFO *pParam
);

UINT32 DRV_EraseFlashSector (
    UINT32 ulAddrOffset
);

UINT32 DRV_EraseAllUserDataSector(
    VOID
);
UINT32 SRVAPI DRV_ReadFlash (
    UINT32 ulAddrOffset,
    BYTE *pBuffer,
    UINT32 nNumberOfBytesToRead,
    UINT32 *pNumberOfBytesRead
);

UINT32 SRVAPI DRV_WriteFlash (
    UINT32 ulAddrOffset,
    CONST BYTE *pBuffer,
    UINT32 nNumberOfBytesToWrite,
    UINT32 *pNumberOfBytesWritten
);

#if 0
UINT32 SRVAPI DRV_EraseFlash (
    UINT32 ulAddrOffset
);
#endif

UINT32 DRV_FlashLock(
    UINT32 ulStartAddr,
    UINT32 ulEndAddr
);

UINT32 DRV_FlashUnLock(
    UINT32 ulStartAddr,
    UINT32 ulEndAddr
);

VOID DRV_FlashSetCtrRegionSize(
    UINT32 CtrSize
);
UINT32 DRV_FlashGetCtrRegionSize(
    VOID
);

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
UINT32 DRV_EraseFlashSectorResume(
    UINT32 ulAddrOffset,
    UINT32 uTime
);
UINT32 DRV_EraseFlashSectorFirst(
    UINT32 ulAddrOffset,
    UINT32 uTime
);
#ifdef __VDS_QUICK_FLUSH__
VOID DRV_SuspendEnable(VOID);
VOID DRV_SuspendDisable(VOID);
VOID DRV_CacheFlush(VOID);
UINT32  DRV_CacheGetNodeCount(VOID);
BOOL  DRV_SuspendIsEnable(VOID);
VOID DRV_RemoveAllCacheNode(VOID);
#endif
#define ERR_FLASH_UNINIT                          -100
#define ERR_FLASH_ERASE_SUSPEND           -101
#define ERR_FLASH_DEVICE_BUSY                -102
#define ERR_FLASH_PARA_ERROR                 -103
#define ERR_FLASH_WRITE_ERROR               -104
#define ERR_FLASH_READ_ERROR                 -105
#define ERR_FLASH_ERASE_ERROR                 -106
#endif

#endif // _H_

