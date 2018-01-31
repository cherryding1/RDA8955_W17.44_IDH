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


#include "dsm_cf.h"
#if defined(MMC_SIMU_NULL)
INT32 DRV_OpenTFlash(UINT8 *dev_name, HANDLE *pdev_handle)
{
    dev_name = dev_name;
    pdev_handle = pdev_handle;
    return 0;
}
INT32 DRV_CloseTFlash(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}
INT32 DRV_ReadTFlash(HANDLE dev_handle, UINT32 blk_nr, BYTE *pBuffer)
{
    dev_handle = dev_handle;
    blk_nr = blk_nr;
    pBuffer = pBuffer;
    return 0;
}
INT32 DRV_WriteTFlash(HANDLE dev_handle, UINT32 blk_nr, CONST BYTE *pBuffer)
{
    dev_handle = dev_handle;
    blk_nr = blk_nr;
    pBuffer = pBuffer;
    return 0;
}
INT32 DRV_GetDevInfoTFlash( HANDLE dev_handle, UINT32 *pBlockNr, UINT32 *pBlockSize )
{
    dev_handle = dev_handle;
    pBlockNr = pBlockNr;
    pBlockSize = pBlockSize;
    return 0;
}
VOID  DRV_DeactiveTFlash(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return;
}
INT32 DRV_ActiveTFlash(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}
VOID DRV_SetStatusSleepIn(BOOL bStatus)
{
    bStatus = bStatus;
    return;
}
UINT32 DRV_GetRCacheSizeTFlash(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}
UINT32 DRV_GetWCacheSizeTFlash(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}
VOID DRV_SetWCacheSizeTFlash(HANDLE dev_handle, UINT32 cache_size)
{
    dev_handle = dev_handle;
    cache_size = cache_size;
}
VOID DRV_SetRCacheSizeTFlash(HANDLE dev_handle, UINT32 cache_size)
{
    dev_handle = dev_handle;
    cache_size = cache_size;
}
INT32 DRV_FlushTFlash(HANDLE dev_handle)
{
    dev_handle = dev_handle;
    return 0;
}
#endif

#if defined(MMC_SIMU_FILE)
// TODO.
#endif


