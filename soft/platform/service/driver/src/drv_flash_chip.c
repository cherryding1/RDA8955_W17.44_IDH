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

#include <cswtype.h>
#include <errorcode.h>
#include <ts.h>
#include <cos.h>
#include "chip_id.h"

#include "memd_m.h"
#include "memd_cache.h"
#include "tgt_memd_cfg.h"
#include <sxr_sbx.h>
#include <sxs_io.h>
#include <drv_flash.h>
#include <dbg.h>
#include "csw_csp.h"
#include "fs.h"
#include "drv_debug.h"

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#define  DRV_TakeSemaphore
#define  DRV_ReleaseSemaphore
#define  DRV_FreeSemaphore
#define DRV_NewSemaphore
#ifdef __VDS_QUICK_FLUSH__
extern BOOL flash_spi_busy(void);
extern BOOL hal_LpsRemainingFramesReady(UINT32 frameNum);
extern BOOL vds_IsFlushAllowed(VOID);
extern VOID sxr_Sleep (UINT32 Period);
#endif
#else
#define  DRV_TakeSemaphore sxr_TakeSemaphore
#define  DRV_ReleaseSemaphore sxr_ReleaseSemaphore
#define  DRV_FreeSemaphore sxr_FreeSemaphore
#define DRV_NewSemaphore sxr_NewSemaphore
PRIVATE HANDLE g_hSem = 0;
#endif

#define REB_SMS_SECTOR_NUM   4  // 3
#define KBYTES 1024
#define MBYTES 1024 * (KBYTES)

#ifndef SXS_NO_PRINTF
#define DRV_Trace sxs_fprintf
#else
#define DRV_Trace
#endif
#if (!defined(_T_UPGRADE_PROGRAMMER))
extern BOOL hal_HstSendEvent(UINT32 ch);

#define DRV_FLASH_ASSERT(bl, format, ...)         \
{if (!(bl)) {                                                            \
    hal_DbgAssert(format, ##__VA_ARGS__);       \
}}
#else
#define DRV_FLASH_ASSERT(bl, format, ...)
#endif
// ---------------------------------------------------
// Flash initialize function ,Reserved.
// ---------------------------------------------------
BOOL DRV_FlashInit(void)
{
    return TRUE;

}

UINT32 DRV_FlashRegionMap(DRV_FLASH_DEV_INFO *pParam)
{
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;

    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    pParam->pBaseAddr = (BYTE *)iBaseAddr;
    pParam->total_size = iTotalSize;
    pParam->sector_size = iSectorSize;
    // pParam->sector_size = 0x20000;
    //  pParam->pBaseAddr = (BYTE *)0xE00000; //_user_data_start;
    //  pParam->total_size = (0x200000 - 0x20000);//_user_data_size;
    //  gLogicSectorSize = pParam->sector_size;
    return ERR_SUCCESS;

}

UINT32 DRV_EraseAllUserDataSector(VOID)
{
    UINT32 i;
    UINT32 iSectorCount = 0;
    UINT32 iAddrOffset = 0;
    INT32   iResult = 0;
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 0;
    UINT32 cri_status = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    cri_status = hal_SysEnterCriticalSection();
#else
    cri_status = 0;
    if (g_hSem == 0)
    {
        return ERR_DRV_INVALID_INITIALIZATION;
    }
    DRV_TakeSemaphore(g_hSem);
#endif
    // Get user data information in flash.
    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);

    iSectorCount = iTotalSize / iSectorSize;
    // Erase all sector of user region.
    for(i = 0; i < iSectorCount ; i++)
    {
        iAddrOffset = i * iSectorSize + (UINT32)iBaseAddr;

        // We erase flash chip using logic sector unit
        iResult = hal_flash_Erase((UINT8 *)iAddrOffset, (UINT8 *)(iAddrOffset + iSectorSize));
        if (iResult)
        {
            break;
        }
    }

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    if(cri_status)
    {
        hal_SysExitCriticalSection(cri_status);
    }
#else
    DRV_ReleaseSemaphore(g_hSem);
#endif
    return ERR_SUCCESS;
}

UINT32 DRV_EraseFlashSector(UINT32 ulAddrOffset)
{
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;
    UINT32 ret = ERR_SUCCESS;
    UINT32 cri_status = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#else
    cri_status = 0;
    if (g_hSem == 0)
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSector g_hSem = NULL.");
        return ERR_DRV_INVALID_INITIALIZATION;
    }
    DRV_TakeSemaphore(g_hSem);
#endif

    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    cri_status = hal_SysEnterCriticalSection();
#endif
    if(ulAddrOffset < iBaseAddr || ulAddrOffset > (iBaseAddr + iTotalSize - iSectorSize))
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSector: erase addr(0x%x) overflow.", ulAddrOffset);
    }
    // We erase flash chip using logic sector unit
    ret = memd_FlashErase((UINT8 *)ulAddrOffset, (UINT8 *)(ulAddrOffset + iSectorSize));
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    if(cri_status)
    {
        hal_SysExitCriticalSection(cri_status);
    }
#else
    DRV_ReleaseSemaphore(g_hSem);
#endif
    if(ret != 0)
    {
        //hal_HstSendEvent(0x91000003);
        //hal_HstSendEvent(ulAddrOffset);
        //hal_HstSendEvent(0-ret);
        DRV_FLASH_ASSERT(0, "memd_FlashErase failed. addr = 0x%x,err code = %d.", ulAddrOffset, ret);
    }

    return ret;
}

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
UINT32 DRV_EraseFlashSectorFirst(UINT32 ulAddrOffset, UINT32 uTime)
{
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;
    UINT32 ret = ERR_SUCCESS;
    UINT32 cri_status = 0;

    //  DRV_PROFILE_FUNCTION_ENTER(DRV_EraseFlashSectorFirst);
    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    if(ulAddrOffset < iBaseAddr || ulAddrOffset > (iBaseAddr + iTotalSize - iSectorSize))
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSectorFirst: erase addr(0x%x) overflow.", ulAddrOffset);
    }

    // We erase flash chip using logic sector unit
#ifdef __VDS_QUICK_FLUSH__
    if(memd_SuspendIsEnable())
    {
        ret = memd_CacheErase((UINT8 *)ulAddrOffset, (UINT8 *)(ulAddrOffset + iSectorSize), uTime);
        // DRV_PROFILE_FUNCTION_EXIT(DRV_EraseFlashSectorFirst);
        if(0 == ret)
        {
            ret = ERR_SUCCESS;
        }
        else
        {
            DRV_FLASH_ASSERT(0, "memd_CacheErase failed. addr = 0x%x,err code = %d.", ulAddrOffset, ret);
        }
        return ret;
    }
#endif
    cri_status = hal_SysEnterCriticalSection();
    ret = memd_FlashErase_Respond_Irq((UINT8 *)ulAddrOffset, (UINT8 *)(ulAddrOffset + iSectorSize), uTime);
    hal_SysExitCriticalSection(cri_status);
    // DRV_PROFILE_FUNCTION_EXIT(DRV_EraseFlashSectorFirst);
    if(MEMD_ERR_NO == ret)
    {
        ret = ERR_SUCCESS;
    }
    else if(MEMD_ERR_BUSY == ret)
    {
        ret = ERR_FLASH_DEVICE_BUSY;
    }
    else
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSectorFirst failed. addr = 0x%x,err code = %d.", ulAddrOffset, ret);
    }
    return ret;
}


UINT32 DRV_EraseFlashSectorResume(UINT32 ulAddrOffset, UINT32 uTime)
{
    UINT32 ret = ERR_SUCCESS;
    UINT32 cri_status = 0;
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;

    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    if(ulAddrOffset < iBaseAddr || ulAddrOffset > (iBaseAddr + iTotalSize - iSectorSize))
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSectorResume: erase addr(0x%x) overflow.", ulAddrOffset);
    }
#ifdef __VDS_QUICK_FLUSH__
    if(memd_SuspendIsEnable())
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSectorResume.addr = 0x%x.", ulAddrOffset);
    }
#endif
    // We erase flash chip using logic sector unit
    cri_status = hal_SysEnterCriticalSection();
    ret = MEMD_ERR_NO;//memd_FlashErase_Continue((UINT8 *)ulAddrOffset, NULL, uTime);
    hal_SysExitCriticalSection(cri_status);
    if(MEMD_ERR_NO == ret)
    {
        ret = ERR_SUCCESS;
    }
    else if(MEMD_ERR_BUSY == ret)
    {
        ret = ERR_FLASH_DEVICE_BUSY;
    }
    else
    {
        DRV_FLASH_ASSERT(0, "DRV_EraseFlashSectorResume failed. addr = 0x%x,err code = %d.", ulAddrOffset, ret);
    }
    return ret;
}

#endif

//UINT8 g_tstflash_buff[512];
//UINT32 g_tst_flash_addr = 0;
UINT32 DRV_ReadFlash(UINT32 ulAddrOffset, BYTE *pBuffer, UINT32 nNumberOfBytesToRead, UINT32 *pNumberOfBytesRead)
{
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;
    UINT32 n  = 0;
    INT32 ret = 0;
    UINT32 cri_status = 0;
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    cri_status = hal_SysEnterCriticalSection();
#else
    cri_status = 0;
    if (g_hSem == 0)
    {
        DRV_FLASH_ASSERT(0, "DRV_ReadFlash failed. g_hSem = NULL.");
        return ERR_DRV_INVALID_INITIALIZATION;
    }
    DRV_TakeSemaphore(g_hSem);
#endif

    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    if(ulAddrOffset < iBaseAddr || (ulAddrOffset + nNumberOfBytesToRead) > (iBaseAddr + iTotalSize))
    {
        DRV_FLASH_ASSERT(0, "DRV_ReadFlash: read addr overflow,offset = 0x%x,iBaseAddr = 0x%x,size = 0x%x.", ulAddrOffset, iBaseAddr, nNumberOfBytesToRead);
    }
#ifdef __VDS_QUICK_FLUSH__
    if(memd_SuspendIsEnable())
    {
        ret = memd_CacheRead((UINT8 *)ulAddrOffset, nNumberOfBytesToRead, &n, pBuffer);
    }
    else
    {
        ret = memd_FlashRead((UINT8 *)ulAddrOffset, nNumberOfBytesToRead, &n, pBuffer);
    }
#else
    ret = memd_FlashRead((UINT8 *)ulAddrOffset, nNumberOfBytesToRead, &n, pBuffer);
#endif
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
    if(cri_status)
    {
        hal_SysExitCriticalSection(cri_status);
    }
#else
    DRV_ReleaseSemaphore(g_hSem);
#endif


    if (ret != 0)
    {
        DRV_FLASH_ASSERT(0,  "DRV_ReadFlash: hal_flash_Read failed 0.addr = 0x%x,rsize = 0x%x,n = 0x%x,err code = %d.", ulAddrOffset, nNumberOfBytesToRead, n, ret);
        return ret;
    }
    if (pNumberOfBytesRead != NULL)
    {
        *pNumberOfBytesRead = n;
        if (nNumberOfBytesToRead != n)
        {
            DRV_FLASH_ASSERT(0,  "DRV_ReadFlash: hal_flash_Read failed 1. addr = 0x%x,rsize = 0x%x,n = 0x%x,err code = %d.", ulAddrOffset, nNumberOfBytesToRead, n, ret);
            return ret;
        }
    }
    return ERR_SUCCESS;
}


UINT32 DRV_WriteFlash(UINT32 ulAddrOffset,
                      CONST BYTE *pBuffer, UINT32 nNumberOfBytesToWrite, UINT32 *pNumberOfBytesWritten)
{
    UINT32  iBaseAddr = 0;
    UINT32  iTotalSize = 0;
    UINT32 iSectorSize = 0;
    UINT32 iBlockSize = 512;
    UINT32 n  = 0;
    INT32 ret = 0;
    UINT32 cri_status = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#else
    cri_status = 0;
    if (g_hSem == 0)
    {
        DRV_FLASH_ASSERT(0, "DRV_WriteFlash failed. g_hSem = NULL.");
        return ERR_DRV_INVALID_INITIALIZATION;
    }
    DRV_TakeSemaphore(g_hSem);
#endif
    memd_FlashGetUserFieldInfo(&iBaseAddr, &iTotalSize, &iSectorSize, &iBlockSize);
    if(ulAddrOffset < iBaseAddr || (ulAddrOffset + nNumberOfBytesToWrite) > (iBaseAddr + iTotalSize))
    {
        DRV_FLASH_ASSERT(0, "DRV_WriteFlash: write addr overflow,offset = 0x%x,iBaseAddr = 0x%x,size = 0x%x.", ulAddrOffset, iBaseAddr, nNumberOfBytesToWrite);
    }
#ifdef __VDS_QUICK_FLUSH__
    if(memd_SuspendIsEnable())
    {
        ret = memd_CacheWrite((UINT8 *)ulAddrOffset, nNumberOfBytesToWrite, &n, (UINT8*)pBuffer);
    }
    else
    {
        cri_status = hal_SysEnterCriticalSection();
        ret = memd_FlashWrite((UINT8 *)ulAddrOffset, nNumberOfBytesToWrite, &n, pBuffer);
        hal_SysExitCriticalSection(cri_status);
    }
#else
    cri_status = hal_SysEnterCriticalSection();
    ret = memd_FlashWrite((UINT8 *)ulAddrOffset, nNumberOfBytesToWrite, &n, pBuffer);
    hal_SysExitCriticalSection(cri_status);

#endif
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)
#else
    DRV_ReleaseSemaphore(g_hSem);
#endif

    if (ret != MEMD_ERR_NO)
    {
        DRV_FLASH_ASSERT(0, "DRV_WriteFlash failed. addr = 0x%x,err code = %d.",ulAddrOffset,ret);
        return ret;
    }

    if (pNumberOfBytesWritten != NULL)
    {
        *pNumberOfBytesWritten = n;
        if (nNumberOfBytesToWrite != n)
        {
            DRV_FLASH_ASSERT(0,  "DRV_WriteFlash: hal_flash_Write failed 1. addr = 0x%x,wsize = 0x%x,n = 0x%x,err code = %d.", ulAddrOffset, nNumberOfBytesToWrite, n, ret);
            return ret;
        }
    }
    return ERR_SUCCESS;
}

#ifdef __VDS_QUICK_FLUSH__

VOID DRV_SuspendEnable(VOID)
{
    memd_SuspendEnable();
}


VOID DRV_SuspendDisable(VOID)
{
    memd_SuspendDisable();
}


VOID DRV_CacheFlush(VOID)
{
    memd_CacheFlush(FLASH_REGION_USER_DATA);
}

BOOL  DRV_SuspendIsEnable(VOID)
{
    return memd_SuspendIsEnable();
}

UINT32  DRV_CacheGetNodeCount(VOID)
{
    return memd_CacheGetNodeCount(FLASH_REGION_USER_DATA);
}

VOID DRV_RemoveAllCacheNode(VOID)
{
    memd_RemoveAllNode(FLASH_REGION_USER_DATA);
}
#endif

UINT32 DRV_FlashLock(UINT32 ulStartAddr, UINT32 ulEndAddr)
{
    INT32 ret = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#else
    if (g_hSem == 0)
        return ERR_DRV_INVALID_INITIALIZATION;
    DRV_TakeSemaphore(g_hSem);
#endif

    ret = hal_flash_Lock((UINT8 *)ulStartAddr, (UINT8 *)ulEndAddr);
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#else
    DRV_ReleaseSemaphore(g_hSem);
#endif
    return ret;
}

UINT32 DRV_FlashUnLock(UINT32 ulStartAddr, UINT32 ulEndAddr)
{
    INT32 ret = 0;

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#else
    if (g_hSem == 0)
        return ERR_DRV_INVALID_INITIALIZATION;
    DRV_TakeSemaphore(g_hSem);
#endif

    ret = hal_flash_Unlock((UINT8 *)ulStartAddr, (UINT8 *)ulEndAddr);
#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#else
    DRV_ReleaseSemaphore(g_hSem);
#endif
    return ret;
}

UINT8 *DRV_FlashGetAddress(UINT32 ulAddrOffset)
{
    return (UINT8 *)hal_flash_GetGlobalAddress((UINT8 *)ulAddrOffset);
}

#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

UINT32 DRV_FlashPowerUp(VOID)
{
    return ERR_SUCCESS;
}


UINT32 DRV_FlashPowerDown(VOID)
{
    return ERR_SUCCESS;
}

#else

UINT32 DRV_FlashPowerUp(VOID)
{
    if (g_hSem)
        DRV_FreeSemaphore(g_hSem);

    g_hSem = DRV_NewSemaphore(1);  // for 1 user.

    return ERR_SUCCESS;
}


UINT32 DRV_FlashPowerDown(VOID)
{
    if(g_hSem)
    {
        DRV_FreeSemaphore(g_hSem);
        g_hSem = 0;
    }
    return ERR_SUCCESS;
}

#endif
