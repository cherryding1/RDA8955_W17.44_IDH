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

#ifdef _FS_SIMULATOR_
#include "dsm_cf.h"
#include "mcd_m.h"
#include "drv_mmc.h"
#else
#include <cswtype.h>
#include <string.h>
#include <errorcode.h>
#include <drv_mmc.h>
#include "ts.h"
#include "cos.h"
#include "sxr_tls.h"
#include "hal_debug.h"
#include "sxr_tim.h"
#include "chip_id.h"
#include "ts.h"
#include "csw_mem_prv.h"
#include "pmd_m.h"
#include "mcd_m.h"
#endif

#ifdef _FS_SIMULATOR_
#define DRV_MMC_ASSERT DSM_Assert
#else
#define DRV_MMC_ASSERT(BOOL, format, ...)                   \
{if (!(BOOL)) {                                                            \
    hal_DbgAssert(format, ##__VA_ARGS__);           \
}}

#endif
// Try times
#define TFLASH_TRYING_TIMES    3

//Invalid Dev Id.
#define DRV_INVALID_DEV_ID    0xffff

#ifdef DUAL_TFLASH_SUPPORT
// TFlash number.
#define DRV_TFLASH_NUMBER      2

// max cache buffer size in byte
#define TFLASH_CACHE_BUFFER_MAX_SIZE  (4*1024)

#else
// TFlash number.
#define DRV_TFLASH_NUMBER      1

// max cache buffer size in byte
#define TFLASH_CACHE_BUFFER_MAX_SIZE  (4*1024)
#endif

// Invalid block number.
#define TFLASH_INVALID_BLOCK_NUMBER 0xffffffff

// Device name none.
#define TFLASH_DEVICE_NAME_NONE        "NONE"

// cache sign define
// dirty
#define DRV_CACHE_DIRTY 1
// clean
#define DRV_CACHE_CLEAN 0


// mmc cache structure.
typedef struct _drv_cache_info
{
    UINT32 uSign;             //sign for dirty or clean.
    UINT32 uFstBlkNo;     // the first block number of cache.
    UINT32 uSize;             // cache buffer size.
    UINT8 *pBuff;             // cache buffer.
} DRV_CACHE_INFO;

// t-flash device description structure.
typedef struct _drv_mmc
{
    // UINT16                         dev_id;                                                    // device id, 0,1...
    UINT16                          is_open;                                                  // device status,0:deactive,1,active.
    UINT8                           dev_name[DRV_DEV_NAME_SIZE];           // device name.
    MCD_CSD_T                 mcd_csd;                                                 // MCD_CSD.
    DRV_CACHE_INFO        r_cache;                                                  // r_cache structure.
    DRV_CACHE_INFO        w_cache;                                                 // w_cache structure.
} DRV_TFLASH;

// System statues.
// The MMI shall call the function DRV_SetStatusSleepIn(TRUE) to declare that the LCD is closed when LCD close,
// the g_bSleepIn change to TRUE.
// The MMI shall call the function DRV_SetStatusSleepIn(FALSE) to declare that the LCD is opened when LCD open,
// the g_bSleepIn change to FALSE.
PRIVATE BOOL                  g_bSleepIn = FALSE;

// TFlash device discription chain.
PRIVATE DRV_TFLASH      g_pTflashDev[] = {{0, TFLASH_DEVICE_NAME_NONE, {0,}, {0,}, {0,},}, {0, TFLASH_DEVICE_NAME_NONE, {0,}, {0,}, {0,},}};

PRIVATE UINT32 g_hMmcSem = 0;

#ifdef _FS_SIMULATOR_

#define CSW_Malloc malloc
#define CSW_Free free

#else

#ifndef strnicmp
extern INT16  SUL_StrNCaselessCompare (CONST TCHAR *stringa, CONST TCHAR *stringb, UINT32  count);
#define strnicmp SUL_StrNCaselessCompare
#endif
extern PVOID CSW_Malloc(UINT32 nSize);
extern BOOL CSW_Free (PVOID pMemBlock);
extern u8 sxr_NewSemaphore (u8 InitValue);
extern void sxr_TakeSemaphore (u8 Id);
extern void sxr_ReleaseSemaphore (u8 Id);
#endif



PRIVATE VOID drv_WaitSemTFlash(VOID)
{
    if (0 == g_hMmcSem)
    {
        g_hMmcSem = sxr_NewSemaphore(1);
    }
    if(g_hMmcSem > 0)
    {
        sxr_TakeSemaphore(g_hMmcSem);
    }

}


PRIVATE VOID drv_ReleaseSemTFlash(VOID)
{
    if(g_hMmcSem > 0)
    {
        sxr_ReleaseSemaphore(g_hMmcSem);
    }

}

PRIVATE VOID drv_GetCardSize(UINT32* pblk_nr,UINT32* pblk_sz)
{
    MCD_CARD_SIZE_T  card_size;

    mcd_GetCardSize(&card_size);
    *pblk_nr = card_size.nbBlock*(card_size.blockLen/MMC_DEF_SEC_SIZE);
    *pblk_sz = MMC_DEF_SEC_SIZE;
    return;
}



//-----------------------------------------------------------------------------------------
// drv_OpenDeviceTFlash
// Open the tflash device.
// @dev_handle[in]
//   Device handle.
// Return
//   Upon successful completion, return ERR_SUCCESS,
//   If have not find the tflash device,shall return the err code ERR_DRV_DEVICE_NOT_FOUND.
//   If have not initialize tflash device before using other this function, shall return err code ERR_DRV_INVALID_INITIALIZATION.
//   If The system cannot perform the mmc device at specified time,  shall return err code ERR_BUSY_DRIVE.
//-----------------------------------------------------------------------------------------
PRIVATE INT32 drv_OpenDeviceTFlash(HANDLE dev_handle)
{
    DRV_TFLASH *tflash_dev = NULL;
    MCD_ERR_T result;
    UINT8 try_times = 0;
    MCD_CARD_VER mcdVer = MCD_CARD_V2;
    tflash_dev = (DRV_TFLASH *)dev_handle;


    if(1 == tflash_dev->is_open)
    {
#ifdef DUAL_TFLASH_SUPPORT
        if((HANDLE)g_pTflashDev == dev_handle)
        {
            result = mcd_Close(MCD_CARD_ID_0);
        }
        else
        {
            result = mcd_Close(MCD_CARD_ID_1);
        }
#else
        result = mcd_Close(MCD_CARD_ID_0); // sxr_Sleep(1800);
#endif
        tflash_dev->is_open = 0;
    }

    if(0 == tflash_dev->is_open)
    {
        do
        {
            if(try_times > 0)
            {
                mcdVer = MCD_CARD_V1;
            }
#ifdef DUAL_TFLASH_SUPPORT
            if((HANDLE)g_pTflashDev == dev_handle)
            {
                result = mcd_Open(MCD_CARD_ID_0, &(tflash_dev->mcd_csd), mcdVer);
            }
            else
            {
                result = mcd_Open(MCD_CARD_ID_1, &(tflash_dev->mcd_csd), mcdVer);
            }
#else
            result = mcd_Open(MCD_CARD_ID_0, &(tflash_dev->mcd_csd), mcdVer);
#endif
            if(result == MCD_ERR_NO)
            {
                break;
            }

            try_times++;
        }while(try_times < TFLASH_TRYING_TIMES);

        if(result != MCD_ERR_NO)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR(" mcd Open failed.Trying times:%d,error code:%d",0x08300001), try_times, result);

            if(result == MCD_ERR_CARD_TIMEOUT ||
                    result == MCD_ERR_CARD_TIMEOUT ||
                    result == MCD_ERR_DMA_BUSY     ||
                    result == MCD_ERR_SPI_BUSY)
            {
                return ERR_TFLASH_DEV_BUSY;
            }
            else
            {
                return ERR_DRV_DEVICE_NOT_FOUND;
            }
        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR(" mcd Open ok.Trying times:%d,dev_handle = 0x%x.",0x08300002), try_times, dev_handle);
        }
        tflash_dev->is_open = 1;

    }
    return ERR_SUCCESS;
}

//-----------------------------------------------------------------------------------------
// drv_CloseDeviceTFlash
//  Close the tflash device.
// @dev_handle[in]
//   Device handle.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//  If have not find the tflash  device, shall return the err code ERR_DRV_DEVICE_NOT_FOUND.
//  If have not initialize tflash device  before using  this function, shall return err code ERR_DRV_INVALID_INITIALIZATION.
//  If The system cannot close tflash device at specified time,s shall return err code ERR_BUSY_DRIVE.
//-----------------------------------------------------------------------------------------
PRIVATE INT32 drv_CloseDeviceTFlash(HANDLE dev_handle)
{
    DRV_TFLASH *tflash_dev = NULL;
    MCD_ERR_T result;
    //MCD_ERR_T result1;
    UINT8 try_times = 0;
    tflash_dev = (DRV_TFLASH *)dev_handle;

    if(tflash_dev->is_open == 1)
    {
        do
        {
            try_times++;
#ifdef DUAL_TFLASH_SUPPORT
            // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR("drv_CloseDeviceTFlash:begin.dev_handle= 0x%x.",0x08300003),dev_handle);
            if((HANDLE)g_pTflashDev == dev_handle)
            {
                result = mcd_Close(MCD_CARD_ID_0);
            }
            else
            {
                result = mcd_Close(MCD_CARD_ID_1);
            }
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("drv_CloseDeviceTFlash:dev_handle= 0x%x, close tflash result = %d.",0x08300004), dev_handle, result);

#else
            result = mcd_Close(MCD_CARD_ID_0);
#endif

            if(result == MCD_ERR_NO)
            {
                break;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("DRV_CloseMMC:try to mcd_Close() failed.Trying times:%d,dev_handle = 0x%x,error code:%d.",0x08300005),
                          try_times, dev_handle, result);
            }
            //sxr_Sleep(10);
        }
        while ((result == MCD_ERR_CARD_TIMEOUT ||
                result == MCD_ERR_CARD_TIMEOUT ||
                result == MCD_ERR_DMA_BUSY     ||
                result == MCD_ERR_SPI_BUSY) &&
                (try_times < TFLASH_TRYING_TIMES));

        if (result != MCD_ERR_NO)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR(" mcd_Close() failed.Trying times:%d,dev_handle = 0x%x,error code:%d.",0x08300006), try_times, dev_handle, result);
            if(result == MCD_ERR_CARD_TIMEOUT ||
                    result == MCD_ERR_CARD_TIMEOUT ||
                    result == MCD_ERR_DMA_BUSY     ||
                    result == MCD_ERR_SPI_BUSY)
            {
                return ERR_TFLASH_DEV_BUSY;
            }
            else
            {
                return ERR_DRV_DEVICE_NOT_FOUND;
            }
        }
        tflash_dev->is_open = 0;
    }
#ifdef DUAL_TFLASH_SUPPORT
    if(0 == g_pTflashDev[0].is_open &&
            0 == g_pTflashDev[1].is_open)
    {
        // padding for community power.
        mcd_LowPower();
    }
#endif
    return ERR_SUCCESS;
}


//-----------------------------------------------------------------------------------------
// drv_FlushCacheTFlash
//  Flush the cache buffer.
// @dev_handle[in]
//   Device handle.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//    else return error code.
//-----------------------------------------------------------------------------------------
PRIVATE INT32 drv_FlushCacheTFlash(HANDLE dev_handle)
{
    DRV_TFLASH *tflash_dev = NULL;
    INT32 ret;
    MCD_ERR_T result;
    UINT32 try_times = 0;
    UINT32 size = 0;
    UINT32 blk_nr = 0;
    UINT32 all_blk_nr = 0;
    UINT32 sec_sz = 0;

    tflash_dev = (DRV_TFLASH *)dev_handle;

    // flush old region
    if(NULL != tflash_dev->w_cache.pBuff                          &&
            DRV_CACHE_DIRTY == tflash_dev->w_cache.uSign   &&
            TFLASH_INVALID_BLOCK_NUMBER != tflash_dev->w_cache.uFstBlkNo)
    {
#ifdef DUAL_TFLASH_SUPPORT
        if((HANDLE)g_pTflashDev == dev_handle)
        {
            mcd_SwitchOperationTo(MCD_CARD_ID_0);
        }
        else
        {
            mcd_SwitchOperationTo(MCD_CARD_ID_1);
        }
#endif
        do
        {
            blk_nr = tflash_dev->w_cache.uFstBlkNo;
            drv_GetCardSize(&all_blk_nr, &sec_sz);
            if(blk_nr + (tflash_dev->w_cache.uSize/sec_sz) < all_blk_nr)
            {
                size =  tflash_dev->w_cache.uSize;
            }
            else if(blk_nr < all_blk_nr)
            {
                size = (UINT32)(all_blk_nr-blk_nr)*sec_sz;
            }
            else
            {
                size = 0;
                ret = ERR_TFLASH_DEV_BLK_OVERFLOW;
                break;
            }
            result = mcd_Write(blk_nr, tflash_dev->w_cache.pBuff, size);
            if (result != MCD_ERR_NO)
            {

                try_times ++;
                CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("DRV_FlushTFlash:call mcd_Write(0x%x) returns error(%d)!try times = %d.",0x08300007), tflash_dev->w_cache.uFstBlkNo, result, try_times);
                if(try_times == TFLASH_TRYING_TIMES)
                {
                    ret = ERR_TFLASH_WRITE_FAILED;
                    goto _func_end;
                }
                drv_CloseDeviceTFlash(dev_handle);
                ret = drv_OpenDeviceTFlash(dev_handle);
                if (ret != ERR_SUCCESS)
                {
                    goto _func_end;
                }
            }
            else
            {
                // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("DRV_FlushTFlash:called mcd_Write() ok. blk_nr = 0x%x,write_sz = 0x%x.",0x08300008),g_WCacheInfo.uFstBlkNo,g_WCacheInfo.uSize);
                break;
            }
        }
        while(try_times < TFLASH_TRYING_TIMES);
    }
    // g_WCacheInfo.uFstBlkNo= TFLASH_INVALID_BLOCK_NUMBER;
    tflash_dev->w_cache.uSign = DRV_CACHE_CLEAN;
    tflash_dev->w_cache.uFstBlkNo = TFLASH_INVALID_BLOCK_NUMBER;
    tflash_dev->r_cache.uFstBlkNo = TFLASH_INVALID_BLOCK_NUMBER;
    return ERR_SUCCESS;

_func_end:

    return ret;

}


//-----------------------------------------------------------------------------------------
// DRV_OpenTFlash
//  Open the device.
// @dev_name[in]
//  TFlash device name.
// @pdev_handle[out]
//   Output device handle.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//    else return error code.
//-----------------------------------------------------------------------------------------
INT32 DRV_OpenTFlash(UINT8 *dev_name, HANDLE *pdev_handle)
{
    DRV_TFLASH *pTDev = NULL;
    UINT16 i;
    UINT16 dev_id;

    // drv_InitSemTFlash();
    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2)|TSMAP(1), TSTR("DRV_OpenTFlash:dev_name = %s,pdev_handle = 0x%x.",0x08300009), dev_name, pdev_handle);
    drv_WaitSemTFlash();
    for(i = 0; i < DRV_TFLASH_NUMBER; i++)
    {
        if(strnicmp((UINT8 *)g_pTflashDev[i].dev_name, dev_name, strlen(dev_name)) == 0)
        {
            pTDev = &g_pTflashDev[i];
            *pdev_handle = (HANDLE)pTDev;
            drv_ReleaseSemTFlash();
            return  ERR_SUCCESS;
        }

    }

    dev_id = DRV_TFLASH_NUMBER;
    for(i = 0; i < DRV_TFLASH_NUMBER; i++)
    {
        if(strnicmp((UINT8 *)g_pTflashDev[i].dev_name, TFLASH_DEVICE_NAME_NONE, strlen(TFLASH_DEVICE_NAME_NONE)) == 0)
        {
            pTDev = &g_pTflashDev[i];
            break;
        }
    }

    if(NULL == pTDev)
    {
        drv_ReleaseSemTFlash();
        return  ERR_DRV_DEVICE_NOT_FOUND;
    }

    memset(pTDev, 0, SIZEOF(DRV_TFLASH));
    strcpy(pTDev->dev_name, dev_name);
    pTDev->is_open = 0;
    pTDev->r_cache.uSign = 0;
    pTDev->r_cache.uFstBlkNo = TFLASH_INVALID_BLOCK_NUMBER;
    pTDev->r_cache.pBuff = NULL;
    pTDev->r_cache.uSize = 0;

    pTDev->w_cache.uSign = 0;
    pTDev->w_cache.uFstBlkNo = TFLASH_INVALID_BLOCK_NUMBER;
    pTDev->w_cache.pBuff = NULL;
    pTDev->w_cache.uSize = 0;
#ifdef _FS_SIMULATOR_
    strcpy(pTDev->mcd_csd.img_name, dev_name);
    pTDev->mcd_csd.fd = 0;
    pTDev->mcd_csd.block_number = 0;
    pTDev->mcd_csd.block_size = 0;
    pTDev->mcd_csd.is_open = 0;
#endif

    *pdev_handle = (HANDLE)pTDev;
    drv_ReleaseSemTFlash();
    return ERR_SUCCESS;
}


//-----------------------------------------------------------------------------------------
// DRV_CloseTFlash
//  Close the device.
// @pdev_handle[out]
//   Device handle.
// Return
//  Upon successful completion, return ERR_SUCCESS,
//    else return error code.
//-----------------------------------------------------------------------------------------
INT32 DRV_CloseTFlash(HANDLE dev_handle)
{
    DRV_TFLASH *pTDev = NULL;
    UINT32 i;
    INT32 result;

    CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR("DRV_CloseTFlash:pdev_handle = 0x%x.",0x0830000a), dev_handle);
    drv_WaitSemTFlash();
    for(i = 0; i < DRV_TFLASH_NUMBER; i++)
    {
        pTDev = &g_pTflashDev[i];
        if((HANDLE)pTDev == dev_handle)
        {
            result = drv_CloseDeviceTFlash(dev_handle);
            if(ERR_SUCCESS == result)
            {

            }
            else
            {
                // donoting.
            }
            drv_ReleaseSemTFlash();
            return result;
        }
    }
    drv_ReleaseSemTFlash();
    return ERR_TFLASH_DEV_INVALID_HANDLE;
}


//-----------------------------------------------------------------------------------------
// DRV_FlushTFlash
//   Flush the cache.
// @pdev_handle[out]
//   Device handle.
// Return
//   Upon successful completion, return ERR_SUCCESS.
//   If flush failed, shall return err code.
//-----------------------------------------------------------------------------------------
INT32 DRV_FlushTFlash(HANDLE dev_handle)
{
    DRV_TFLASH *tflash_dev = NULL;
    INT32 ret;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;

    if(tflash_dev->is_open == 0)
    {
        ret = drv_OpenDeviceTFlash(dev_handle);
        if (ret != ERR_SUCCESS)
        {
            drv_ReleaseSemTFlash();
            return ret;
        }
    }

    ret = drv_FlushCacheTFlash(dev_handle);
    drv_ReleaseSemTFlash();
    return ret;
}



//-----------------------------------------------------------------------------------------
// DRV_ReadTFlash
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
INT32 DRV_ReadTFlash(HANDLE dev_handle, UINT32 blk_nr, BYTE* pBuffer)
{
    INT32 ret = 0;
    UINT32 try_times = 0;
    MCD_ERR_T result;
    DRV_TFLASH *tflash_dev = NULL;
    UINT32 size = 0;
    UINT32 all_blk_nr = 0;
    UINT32 sec_sz = 0;
    UINT32 local_blk;

    local_blk = blk_nr;
    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;
    // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("DRV_ReadTFlash:pdev_handle = 0x%x,blk_nr = 0x%x.",0x0830000b),dev_handle,blk_nr);
    if(tflash_dev->is_open == 0)
    {
        ret = drv_OpenDeviceTFlash(dev_handle);
        if (ret != ERR_SUCCESS)
        {
            CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("DRV_ReadTFlash:open tflash failed. dev_handle = 0x%x,ret = %d.",0x0830000c), dev_handle, ret);
            goto func_end;
        }
    }

    // try to read the block from w_cache buffer.if hit the target,goto end.
    if(NULL != tflash_dev->w_cache.pBuff            &&
       local_blk >= tflash_dev->w_cache.uFstBlkNo  &&
       local_blk <   tflash_dev->w_cache.uFstBlkNo + tflash_dev->w_cache.uSize/MMC_DEF_SEC_SIZE
      )
    {
        memcpy(pBuffer,tflash_dev->w_cache.pBuff + (UINT32)(local_blk - tflash_dev->w_cache.uFstBlkNo)* MMC_DEF_SEC_SIZE,MMC_DEF_SEC_SIZE);
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("cache:DRV_ReadTFlash(0x%x) read form w_cache, fst_blk_no = 0x%x.\n",0x0830000d),blk_nr,tflash_dev->w_cache.uFstBlkNo);
        goto func_end;
    }


    // try to read the block from r_cache buffer. if hit the target,goto end.
    if(NULL != tflash_dev->r_cache.pBuff            &&
       local_blk >= tflash_dev->r_cache.uFstBlkNo  &&
       local_blk <   tflash_dev->r_cache.uFstBlkNo + tflash_dev->r_cache.uSize/MMC_DEF_SEC_SIZE
      )
    {
        memcpy(pBuffer,tflash_dev->r_cache.pBuff  + (UINT32)(local_blk - tflash_dev->r_cache.uFstBlkNo)* MMC_DEF_SEC_SIZE,MMC_DEF_SEC_SIZE);
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("cache:DRV_ReadTFlash(0x%x) read form r_cache, fst_blk_no = %d.\n",0x0830000e),blk_nr,tflash_dev->r_cache.uFstBlkNo);
        goto func_end;
    }

    // read from t-flash to cache.
    // if read failed ,try to do it times without number.
#ifdef DUAL_TFLASH_SUPPORT
    if((HANDLE)g_pTflashDev == dev_handle)
    {
        mcd_SwitchOperationTo(MCD_CARD_ID_0);
    }
    else
    {
        mcd_SwitchOperationTo(MCD_CARD_ID_1);
    }
#endif
    do
    {
        drv_GetCardSize(&all_blk_nr, &sec_sz);
        if(local_blk + (tflash_dev->r_cache.uSize/sec_sz) < all_blk_nr)
        {
            size =  tflash_dev->r_cache.uSize;
        }
        else if(local_blk < all_blk_nr)
        {
            size = (UINT32)(all_blk_nr - local_blk)*sec_sz;
        }
        else
        {
            size = 0;
            ret = ERR_TFLASH_DEV_BLK_OVERFLOW;
            break;
        }
        result = mcd_Read((UINT32)local_blk, tflash_dev->r_cache.pBuff, size);
        if (result != MCD_ERR_NO )
        {
            try_times ++;
            if(try_times == TFLASH_TRYING_TIMES)
            {
                ret = result;
                break;
            }

            // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("DRV_ReadMMC:call mcd_Read(0x%x) returns error(%d)!try times = %d.",0x0830000f), blk_nr, result, try_times);

            drv_CloseDeviceTFlash(dev_handle);
            ret = drv_OpenDeviceTFlash(dev_handle);
            if (ret != ERR_SUCCESS)
            {
                 break;
            }
        }
        else
        {
            tflash_dev->r_cache.uFstBlkNo = local_blk;
            memcpy(pBuffer, tflash_dev->r_cache.pBuff, MMC_DEF_SEC_SIZE);
            break;
        }
    }
    while(try_times < TFLASH_TRYING_TIMES);
func_end:
    drv_ReleaseSemTFlash();
    return ret;
}


//-----------------------------------------------------------------------------------------
// DRV_WriteTFlash
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
INT32 DRV_WriteTFlash (HANDLE dev_handle, UINT32 blk_nr, CONST BYTE* pBuffer)
{
    INT32 ret;
    MCD_ERR_T result;
    UINT32 try_times = 0;
    DRV_TFLASH *tflash_dev = NULL;
    UINT32 size = 0;
    UINT32 all_blk_nr = 0;
    UINT32 sec_sz = 0;
    UINT32 local_blk;

    local_blk = blk_nr;
    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;

    if(tflash_dev->is_open == 0)
    {
        ret = drv_OpenDeviceTFlash(dev_handle);
        if (ret != ERR_SUCCESS)
        {
            goto _func_end;
        }
    }

    // write to r_cache if the blk in r_cache.
    if(NULL != tflash_dev->r_cache.pBuff            &&
      local_blk >= tflash_dev->r_cache.uFstBlkNo  &&
      local_blk < tflash_dev->r_cache.uFstBlkNo + tflash_dev->r_cache.uSize/MMC_DEF_SEC_SIZE
     )
   {
       memcpy(tflash_dev->r_cache.pBuff + (UINT32)(local_blk -tflash_dev->r_cache.uFstBlkNo) * MMC_DEF_SEC_SIZE,pBuffer,MMC_DEF_SEC_SIZE);
       // CSW_TRACE(BASE_FFS_TS_ID, "cache: write to r_cache buffer,blk_nr = %d.\n",blk_nr);
   }

   // write to w_cache if the blk in w_cache.
   if(NULL !=  tflash_dev->w_cache.pBuff           &&
      local_blk >= tflash_dev->w_cache.uFstBlkNo  &&
      local_blk < tflash_dev->w_cache.uFstBlkNo + tflash_dev->w_cache.uSize/MMC_DEF_SEC_SIZE)
   {
       memcpy(tflash_dev->w_cache.pBuff + (UINT32)(local_blk - tflash_dev->w_cache.uFstBlkNo) * MMC_DEF_SEC_SIZE,pBuffer,MMC_DEF_SEC_SIZE);
       tflash_dev->w_cache.uSign = DRV_CACHE_DIRTY;
       // CSW_TRACE(BASE_FFS_TS_ID, "cache: write to w_cache buffer,blk_nr = %d.\n",blk_nr);
    }
    else
    {
#ifdef DUAL_TFLASH_SUPPORT
        if((HANDLE)g_pTflashDev == dev_handle)
        {
            mcd_SwitchOperationTo(MCD_CARD_ID_0);
        }
        else
        {
            mcd_SwitchOperationTo(MCD_CARD_ID_1);
        }
#endif
        // flush old region
        if(NULL != tflash_dev->w_cache.pBuff                         &&
                DRV_CACHE_DIRTY == tflash_dev->w_cache.uSign &&
                TFLASH_INVALID_BLOCK_NUMBER != tflash_dev->w_cache.uFstBlkNo)
        {
            do
            {
                drv_GetCardSize(&all_blk_nr,&sec_sz);
                if(local_blk + (tflash_dev->w_cache.uSize/sec_sz) < all_blk_nr)
                {
                    size =  tflash_dev->w_cache.uSize;
                }
                else if(local_blk < all_blk_nr)
                {
                    size = (UINT32)(all_blk_nr-local_blk)*sec_sz;
                }
                else
                {
                    size = 0;
                    ret = ERR_TFLASH_DEV_BLK_OVERFLOW;
                    break;
                }
                result = mcd_Write(tflash_dev->w_cache.uFstBlkNo, tflash_dev->w_cache.pBuff, size);
                if (result != MCD_ERR_NO)
                {
                    try_times ++;
                    // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("DRV_WriteTFlash:call mcd_Write(0x%x) returns error(0x%x)!try times = %d.",0x08300015), tflash_dev->w_cache.uFstBlkNo, result, try_times);
                    if(try_times == TFLASH_TRYING_TIMES)
                    {
                        ret = result;
                        goto _func_end;
                    }

                    drv_CloseDeviceTFlash(dev_handle);
                    ret = drv_OpenDeviceTFlash(dev_handle);
                    if (ret != ERR_SUCCESS)
                    {
                        goto _func_end;
                    }
                }
                else
                {
                    tflash_dev->w_cache.uSign = DRV_CACHE_CLEAN;
                    // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("DRV_WriteMMC:called mcd_Write() ok. blk_nr = 0x%x,write_sz = 0x%x.",0x08300016),tflash_dev->w_cache.uFstBlkNo,tflash_dev->w_cache.uSize);
                    break;
                }
            }
            while(try_times < TFLASH_TRYING_TIMES);
        }
        // cache the new region
        do
        {
            drv_GetCardSize(&all_blk_nr, &sec_sz);
            if(local_blk + (tflash_dev->w_cache.uSize/sec_sz) < all_blk_nr)
            {
                size =  tflash_dev->w_cache.uSize;
            }
            else if(local_blk < all_blk_nr)
            {
                size = (UINT32)(all_blk_nr-local_blk)*sec_sz;
            }
            else
            {
                size = 0;
                ret = ERR_TFLASH_DEV_BLK_OVERFLOW;
                break;
            }
            result = mcd_Read(local_blk, tflash_dev->w_cache.pBuff, size);
            if (result != MCD_ERR_NO)
            {
                try_times ++;
                // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("DRV_WriteTFlash:call mcd_Read(0x%x) returns error(%d)!try times = %d.",0x08300017), blk_nr, result, try_times);
                if(try_times == TFLASH_TRYING_TIMES)
                {
                    ret = result;
                    goto _func_end;
                }
                drv_CloseDeviceTFlash(dev_handle);
                result = drv_OpenDeviceTFlash(dev_handle);
                if (result != ERR_SUCCESS)
                {
                    ret = result;
                }
            }
            else
            {
                // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("DRV_WriteTFlash:called mcd_Read() ok. blk_nr = 0x%x,read_sz = 0x%x.",0x08300018),blk_nr,g_WCacheInfo.uSize);
                tflash_dev->w_cache.uFstBlkNo = local_blk;
                // update the specified block.
                memcpy(tflash_dev->w_cache.pBuff + (UINT32)(local_blk -tflash_dev->w_cache.uFstBlkNo) * MMC_DEF_SEC_SIZE,pBuffer,MMC_DEF_SEC_SIZE);
                tflash_dev->w_cache.uSign = DRV_CACHE_DIRTY;
                break;
            }
        }
        while(try_times < TFLASH_TRYING_TIMES);

    }

    ret = ERR_SUCCESS;

_func_end:
    drv_ReleaseSemTFlash();
    return ret;
}

//-----------------------------------------------------------------------------------------
// DRV_GetDevInfoTFlash
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
INT32 DRV_GetDevInfoTFlash (HANDLE dev_handle, UINT32 *pBlockNr, UINT32 *pBlockSize)
{
    INT32 ret;
    DRV_TFLASH *tflash_dev = NULL;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;

    if(tflash_dev->is_open == 0)
    {
        ret = drv_OpenDeviceTFlash(dev_handle);
        if (ret != ERR_SUCCESS)
        {
            drv_ReleaseSemTFlash();
            return ERR_DRV_DEVICE_NOT_FOUND;
        }
    }

#ifdef DUAL_TFLASH_SUPPORT
    if((HANDLE)g_pTflashDev == dev_handle)
    {
        mcd_SwitchOperationTo(MCD_CARD_ID_0);
    }
    else
    {
        mcd_SwitchOperationTo(MCD_CARD_ID_1);
    }
#endif
    drv_GetCardSize(pBlockNr, pBlockSize);
    // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(3), TSTR("DRV_GetDevInfoTFlash:handle = %d,block size = 0x%x, block number = 0x%x.",0x08300019), tflash_dev, *pBlockSize, *pBlockNr);
#ifdef _INCREASE_SPEED_UMSS_
    if(g_umss_process == FALSE)
    {
        drv_CloseDeviceTFlash(dev_handle);
    }
#else
    drv_CloseDeviceTFlash(dev_handle);
#endif
    drv_ReleaseSemTFlash();
    return ERR_SUCCESS;
}

BOOL g_sd_sleepIn = FALSE;
//-----------------------------------------------------------------------------------------
//DRV_DeactiveTFlash
//   Deactive the device.
// @dev_handle[in]
//  Device handle.
//Return
//  VOID
//-----------------------------------------------------------------------------------------
VOID DRV_DeactiveTFlash(HANDLE dev_handle)
{
    INT32 ret;
    DRV_TFLASH *tflash_dev = NULL;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;

    if(tflash_dev->is_open == 0)
    {
        drv_ReleaseSemTFlash();
        return;
    }

    ret = drv_FlushCacheTFlash(dev_handle);
    if(ERR_SUCCESS != ret)
    {
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR(" cache:FS_Close flush cache error.  return error = 0x%x.",0x0830001a), ret);
    }
    else
    {
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(0), TSTR(" cache:FS_Close flush cache ok.",0x0830001b));
    }
    if(g_bSleepIn == TRUE)
    {
        // hal_HstSendEvent(0x12347777);
        g_sd_sleepIn = TRUE;
        drv_CloseDeviceTFlash(dev_handle);
        g_sd_sleepIn = FALSE;
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(0), TSTR("Close MMC device.",0x0830001c));
    }
    drv_ReleaseSemTFlash();
    return;
}


//-----------------------------------------------------------------------------------------
//DRV_ActiveTFlash
// Active the device.
// @dev_handle[in]
//  Device handle.
// Return
// See the description abount of drv_OpenDeviceTFlash().
//-----------------------------------------------------------------------------------------
INT32 DRV_ActiveTFlash(HANDLE dev_handle)
{
    INT32 iRet;
    drv_WaitSemTFlash();
    iRet = drv_OpenDeviceTFlash(dev_handle);
    drv_ReleaseSemTFlash();
    return iRet;

}


//-----------------------------------------------------------------------------------------
//DRV_SetStatusSleepIn
// Set LCD status.
// @dev_handle[in]
//  Device handle.
// Return
// VOID
//-----------------------------------------------------------------------------------------
VOID DRV_SetStatusSleepIn(BOOL dev_status)
{
    g_bSleepIn = dev_status;
}


//-----------------------------------------------------------------------------------------
//DRV_GetRCacheSizeTFlash
//Get the size of rcache buffer.
//@dev_handle[in]
//  Device handle.
//Return
//   return size of rcache buffer.
//-----------------------------------------------------------------------------------------
UINT32 DRV_GetRCacheSizeTFlash (HANDLE dev_handle)
{
    DRV_TFLASH *tflash_dev = NULL;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;
    if(NULL != tflash_dev->r_cache.pBuff)
    {
        drv_ReleaseSemTFlash();
        return tflash_dev->r_cache.uSize;
    }
    else
    {
        drv_ReleaseSemTFlash();
        return 0;
    }
}


//-----------------------------------------------------------------------------------------
//DRV_SetRCacheSizeTFlash
//Set the size of rcache buffer.
//@dev_handle[in]
//  Device handle.
//@cache_size[in]
//   Size of rcache buffer.
//Return Value:
//   VOID
//-----------------------------------------------------------------------------------------
VOID DRV_SetRCacheSizeTFlash(HANDLE dev_handle, UINT32 cache_size)
{
    // reset the cache size when the current cache size not equal to the input parameter.
    // malloc a buffer with (cache size ) in byte,

    UINT32 align_size = 0;
    UINT32 size_old = 0;
    DRV_TFLASH *tflash_dev = NULL;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;
    size_old = tflash_dev->r_cache.uSize;

    DRV_MMC_ASSERT(cache_size >= MMC_DEF_SEC_SIZE, "cache_size = 0.");
    // align in MMC_DEF_SEC_SIZE(512) byte:
#ifdef DUAL_TFLASH_SUPPORT
    if((HANDLE)g_pTflashDev == dev_handle)
    {
        cache_size = cache_size > TFLASH_CACHE_BUFFER_MAX_SIZE ? TFLASH_CACHE_BUFFER_MAX_SIZE : cache_size;
        align_size = ((cache_size + (MMC_DEF_SEC_SIZE - 1)) / MMC_DEF_SEC_SIZE) * MMC_DEF_SEC_SIZE;
    }
    else
    {

#if USE_SPI_FOR_SD_CARD2
        align_size =  MMC_DEF_SEC_SIZE;
#error "we have do not use spi for sd card in 8806 and 8808 "
#else
        cache_size = cache_size > TFLASH_CACHE_BUFFER_MAX_SIZE ? TFLASH_CACHE_BUFFER_MAX_SIZE : cache_size;
        align_size = ((cache_size + (MMC_DEF_SEC_SIZE - 1)) / MMC_DEF_SEC_SIZE) * MMC_DEF_SEC_SIZE;
#endif
    }
#else
    cache_size = cache_size > TFLASH_CACHE_BUFFER_MAX_SIZE ? TFLASH_CACHE_BUFFER_MAX_SIZE : cache_size;
    align_size = ((cache_size + (MMC_DEF_SEC_SIZE - 1)) / MMC_DEF_SEC_SIZE) * MMC_DEF_SEC_SIZE;
#endif
    // Set write cache structure.
    if(size_old != align_size)
    {
        // Set read cache structure.

        if(NULL != tflash_dev->r_cache.pBuff)
        {
            CSW_Free(tflash_dev->r_cache.pBuff);
            tflash_dev->r_cache.pBuff = NULL;
        }

        // try to malloc with align_size, reduce by half the size if fail.
        do
        {
            tflash_dev->r_cache.pBuff = CSW_Malloc(align_size);
            if(NULL != tflash_dev->r_cache.pBuff)
            {
                break;
            }
            else
            {
                DRV_MMC_ASSERT(0, "DRV_SetRCacheSizeTFlash:malloc(0x%x) failed.", align_size);
                align_size = align_size / 2;
            }

        }
        while(align_size >= MMC_DEF_SEC_SIZE);

        if(NULL == tflash_dev->r_cache.pBuff )
        {
            // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR("Set r_cache size:malloc failed, cache size = %d byte.",0x0830001d), align_size);
            tflash_dev->r_cache.uSize = 0;
            tflash_dev->r_cache.uSign = DRV_CACHE_CLEAN;
        }
        else
        {
            tflash_dev->r_cache.uSize = align_size;
            tflash_dev->r_cache.uSign = DRV_CACHE_CLEAN;
        }
        tflash_dev->r_cache.uFstBlkNo = TFLASH_INVALID_BLOCK_NUMBER;
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("cache:change r_cache size to 0x%x bytes,old_size = 0x%x.",0x0830001e), align_size, size_old);
    }
    drv_ReleaseSemTFlash();
    return;
}


//-----------------------------------------------------------------------------------------
//DRV_GetWCacheSizeTFlash
//Get the size of wcache buffer.
//@dev_handle[in]
//  Device handle.
//Return
//   return size of wcache buffer.
//-----------------------------------------------------------------------------------------
UINT32 DRV_GetWCacheSizeTFlash(HANDLE dev_handle)
{
    DRV_TFLASH *tflash_dev = NULL;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;
    if(NULL != tflash_dev->w_cache.pBuff)
    {
        drv_ReleaseSemTFlash();
        return tflash_dev->w_cache.uSize;
    }
    else
    {
        drv_ReleaseSemTFlash();
        return 0;
    }
}


//-----------------------------------------------------------------------------------------
//DRV_SetWCacheSizeTFlash
//Set the size of wcache buffer.
//@dev_handle[in]
//  Device handle.
//@cache_size[in]
//   Size of wcache buffer.
//Return Value:
//   VOID
//-----------------------------------------------------------------------------------------
VOID DRV_SetWCacheSizeTFlash(HANDLE dev_handle, UINT32 cache_size)
{

    // reset the cache size when the current cache size not equal to the input parameter.
    // malloc a buffer with (cache size ) in byte,

    UINT32 align_size = 0;
    UINT32 size_old = 0;
    DRV_TFLASH *tflash_dev = NULL;
    INT32 ret;

    drv_WaitSemTFlash();
    tflash_dev = (DRV_TFLASH *)dev_handle;
    size_old = tflash_dev->w_cache.uSize;
    DRV_MMC_ASSERT(cache_size >= MMC_DEF_SEC_SIZE, "cache_size = 0.");
    // align in MMC_DEF_SEC_SIZE(512) byte:
#ifdef DUAL_TFLASH_SUPPORT
    if((HANDLE)g_pTflashDev == dev_handle)
    {
        cache_size = cache_size > TFLASH_CACHE_BUFFER_MAX_SIZE ? TFLASH_CACHE_BUFFER_MAX_SIZE : cache_size;
        align_size = ((cache_size + (MMC_DEF_SEC_SIZE - 1)) / MMC_DEF_SEC_SIZE) * MMC_DEF_SEC_SIZE;
    }
    else
    {

#if USE_SPI_FOR_SD_CARD2
        align_size =  MMC_DEF_SEC_SIZE;
#error "we have do not use spi for sd card in 8806 and 8808 "
#else
        cache_size = cache_size > TFLASH_CACHE_BUFFER_MAX_SIZE ? TFLASH_CACHE_BUFFER_MAX_SIZE : cache_size;
        align_size = ((cache_size + (MMC_DEF_SEC_SIZE - 1)) / MMC_DEF_SEC_SIZE) * MMC_DEF_SEC_SIZE;
#endif

    }
#else
    cache_size = cache_size > TFLASH_CACHE_BUFFER_MAX_SIZE ? TFLASH_CACHE_BUFFER_MAX_SIZE : cache_size;
    align_size = ((cache_size + (MMC_DEF_SEC_SIZE - 1)) / MMC_DEF_SEC_SIZE) * MMC_DEF_SEC_SIZE;
#endif

    // Set write cache structure.
    if(size_old != align_size)
    {
        if(NULL != tflash_dev->w_cache.pBuff)
        {
            if(tflash_dev->is_open == 0)
            {
                ret = drv_OpenDeviceTFlash(dev_handle);
                if (ret != ERR_SUCCESS)
                {
                    drv_ReleaseSemTFlash();
                    return;
                }
            }
            drv_FlushCacheTFlash(dev_handle);
            CSW_Free(tflash_dev->w_cache.pBuff);
            tflash_dev->w_cache.pBuff = NULL;
        }

        // try to malloc with align_size, reduce by half the size if fail.
        do
        {
            tflash_dev->w_cache.pBuff = CSW_Malloc(align_size);
            if(NULL != tflash_dev->w_cache.pBuff)
            {
                break;
            }
            else
            {
                DRV_MMC_ASSERT(0, "DRV_SetRCacheSizeTFlash:malloc(0x%x) failed.", align_size);
                align_size = align_size / 2;
            }

        }
        while(align_size >= MMC_DEF_SEC_SIZE);

        if(NULL == tflash_dev->w_cache.pBuff )
        {
            // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(1), TSTR("Set w_cache size:malloc failed, cache size = %d byte.",0x0830001f), align_size);
            tflash_dev->w_cache.uSize = 0;
            tflash_dev->w_cache.uSign = DRV_CACHE_CLEAN;
        }
        else
        {
            tflash_dev->w_cache.uSize = align_size;
            tflash_dev->w_cache.uSign = DRV_CACHE_CLEAN;
        }
        tflash_dev->w_cache.uFstBlkNo = TFLASH_INVALID_BLOCK_NUMBER;
        // CSW_TRACE(_CSW|TLEVEL(BASE_FFS_TS_ID)|TDB|TNB_ARG(2), TSTR("cache:change the w_cache size to 0x%x bytes,old_size = 0x%x.",0x08300020), align_size, size_old);
    }
    drv_ReleaseSemTFlash();
    return;
}

extern VOID pm_TFlashDetect(BOOL cardPlugged);

VOID TFcardPlugHandler(BOOL cardPlugged)
{
#ifdef _T_UPGRADE_PROGRAMMER
    cardPlugged = cardPlugged;
#else // _T_UPGRADE_PROGRAMMER
#ifndef _FS_SIMULATOR_
    pm_TFlashDetect(cardPlugged);
#endif
#endif // _T_UPGRADE_PROGRAMMER
}

void McdSetCardDetectHandler(void)
{
    drv_WaitSemTFlash();
    mcd_SetCardDetectHandler((MCD_CARD_DETECT_HANDLER_T)TFcardPlugHandler );
    drv_ReleaseSemTFlash();
    return;
}



