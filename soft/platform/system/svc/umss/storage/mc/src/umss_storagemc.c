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




#include "umss_config.h"

#include "sxr_mem.h"
#include "sxs_io.h"

#include "mcd_m.h"

#include "hal_host.h"

#include <string.h>

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// UMSS_STORAGE_MC_HANDLER_T
// -----------------------------------------------------------------------------
/// Mc handler
// =============================================================================
#ifndef _INCREASE_SPEED_UMSS_

typedef struct
{
    UMSS_STORAGE_SIZE_T  size;
} UMSS_STORAGE_MC_HANDLER_T;
#else
typedef struct
{
    HANDLE dev_handle;
    UMSS_STORAGE_SIZE_T  size;
} UMSS_STORAGE_MC_HANDLER_T;
#endif
// =============================================================================
// PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE VOID*
umss_StorageMcOpen   (CONST UMSS_STORAGE_PARAMETERS_T* cfg);

PRIVATE VOID
umss_StorageMcClose  (VOID* handler);

PRIVATE INT32
umss_StorageMcRead   (VOID* handler, VOID* data, UINT32 lba, UINT32 size);

PRIVATE INT32
umss_StorageMcWrite  (VOID* handler, VOID* data, UINT32 lba, UINT32 size);

PRIVATE UMSS_STORAGE_SIZE_T*
umss_StorageMcGetsize(VOID* handler);

PRIVATE BOOL
umss_StorageMcPresent(VOID* handler);

extern PUBLIC MCD_ERR_T mcd_Open(MCD_CARD_ID mcdId,MCD_CSD_T* mcdCsd, MCD_CARD_VER mcdVer);
extern PUBLIC MCD_ERR_T mcd_Close(MCD_CARD_ID mcdId);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST UMSS_STORAGE_CALLBACK_T g_umssStorageMcCallback =
{
    .open    = umss_StorageMcOpen,
    .close   = umss_StorageMcClose,
    .read    = umss_StorageMcRead,
    .write   = umss_StorageMcWrite,
    .getsize = umss_StorageMcGetsize,
    .present = umss_StorageMcPresent
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================
#ifndef _INCREASE_SPEED_UMSS_
#ifdef BUILD_ON_PLATFORM_TEST
BOOL PM_GetBatteryInfo(UINT8 *pBcs, UINT8 *pBcl, UINT16 *pMpc){return TRUE;}
#else
extern BOOL PM_GetBatteryInfo(UINT8 *pBcs, UINT8 *pBcl, UINT16 *pMpc);
#endif
PRIVATE BOOL umss_StorageMcPresent(VOID* handler)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;
    MCD_CSD_T                  csd;
    MCD_CARD_SIZE_T            cardSize;
    MCD_CARD_VER mcdVer = MCD_CARD_V2;
    UINT8 pBcs, pBcl;
    UINT16 pMpc;
    PM_GetBatteryInfo(&pBcs, &pBcl,&pMpc);
    if(pBcs == 0)
    {
        return FALSE;
    }

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;

    switch(mcd_CardStatus(MCD_CARD_ID_0))
    {
        case MCD_STATUS_NOTOPEN_PRESENT:
#ifdef DUAL_TFLASH_SUPPORT
            if(mcd_Open(MCD_CARD_ID_0,&csd,mcdVer) != MCD_ERR_NO)
            {
                mcd_Close(MCD_CARD_ID_0);
                if(mcd_Open(MCD_CARD_ID_0,&csd,mcdVer) != MCD_ERR_NO)
                {
                    mcd_Close(MCD_CARD_ID_0);
                    mcdVer = MCD_CARD_V1;
                    if(mcd_Open(MCD_CARD_ID_0,&csd,mcdVer) != MCD_ERR_NO)
                    {
                        return FALSE;
                    }
                    else
                    {
                        mcd_GetCardSize(&cardSize);
                        handlerMc->size.sizeBlock = cardSize.blockLen;
                        handlerMc->size.nbBlock   = cardSize.nbBlock;
                        return TRUE;
                    }
                }
            }
#else
            if(mcd_Open(MCD_CARD_ID_0,&csd,mcdVer) != MCD_ERR_NO)
            {
                mcd_Close(MCD_CARD_ID_0);
                if(mcd_Open(MCD_CARD_ID_0,&csd,mcdVer) != MCD_ERR_NO)
                {
                    mcd_Close(MCD_CARD_ID_0);
                    mcdVer = MCD_CARD_V1;
                    if(mcd_Open(MCD_CARD_ID_0,&csd,mcdVer) != MCD_ERR_NO)
                    {
                        return FALSE;
                    }
                    else
                    {
                        mcd_GetCardSize(&cardSize);
                        handlerMc->size.sizeBlock = cardSize.blockLen;
                        handlerMc->size.nbBlock   = cardSize.nbBlock;
                        return TRUE;
                    }
                }
            }
#endif
        // Caution: There is no break here!
        // The following statement is executed as well.
        case MCD_STATUS_OPEN:
            mcd_GetCardSize(&cardSize);
            handlerMc->size.sizeBlock = cardSize.blockLen;
            handlerMc->size.nbBlock   = cardSize.nbBlock;
            return TRUE;
        case MCD_STATUS_NOTPRESENT:
            return FALSE;
        case MCD_STATUS_OPEN_NOTPRESENT:
            // ensure the mcd is closed
            mcd_Close(MCD_CARD_ID_0);
            return FALSE;
    }

    return(FALSE);
}

PRIVATE VOID*  umss_StorageMcOpen(CONST UMSS_STORAGE_PARAMETERS_T* cfg)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;

    cfg = cfg;

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*)
                sxr_Malloc(sizeof(UMSS_STORAGE_MC_HANDLER_T));

    return((VOID*)(handlerMc));
}

PRIVATE VOID   umss_StorageMcClose(VOID* handler)
{
    mcd_Close(MCD_CARD_ID_0);

    if(handler)
    {
        sxr_Free(handler);
    }
}

PRIVATE INT32 umss_StorageMcRead(VOID* handler, VOID* data,
                                 UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;
    UINT32                     size;
    MCD_ERR_T                  ret;

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;

    size             = handlerMc->size.sizeBlock*nbBlock;

    ret = mcd_Read(lba, data, size);
    if(ret != MCD_ERR_NO)
    {
        switch(ret)
        {
            case MCD_ERR_DMA_BUSY:
            case MCD_ERR_SPI_BUSY:
                return(-2);
            case MCD_ERR_CARD_TIMEOUT:
            case MCD_ERR_BLOCK_LEN:
            case MCD_ERR_CARD_NO_RESPONSE:
            case MCD_ERR_CARD_RESPONSE_BAD_CRC:
            case MCD_ERR_CMD:
            case MCD_ERR_UNUSABLE_CARD:
            default:
                mcd_Close(MCD_CARD_ID_0);
                return(-1);
        }
    }

    return(nbBlock);
}

PRIVATE INT32 umss_StorageMcWrite(VOID* handler, VOID* data,
                                  UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_MC_HANDLER_T*  handlerMc;
    UINT32                      size;
    MCD_ERR_T                   ret;

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;

    size             = handlerMc->size.sizeBlock*nbBlock;

    ret = mcd_Write(lba, data, size);
    if(ret != MCD_ERR_NO)
    {
        switch(ret)
        {
            case MCD_ERR_DMA_BUSY:
            case MCD_ERR_SPI_BUSY:
                return(-2);
            case MCD_ERR_CARD_TIMEOUT:
            case MCD_ERR_BLOCK_LEN:
            case MCD_ERR_CARD_NO_RESPONSE:
            case MCD_ERR_CARD_RESPONSE_BAD_CRC:
            case MCD_ERR_CMD:
            case MCD_ERR_UNUSABLE_CARD:
            default:
                mcd_Close(MCD_CARD_ID_0);
                return(-1);
        }
    }

    return(nbBlock);
}

PRIVATE UMSS_STORAGE_SIZE_T* umss_StorageMcGetsize(VOID* handler)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;

    return(&handlerMc->size);
}

#else
#error // The efficiency is not high
#define SVC_TRACE(level, tstmap, format, ...)  //hal_DbgTrace(level,tstmap,format, ##__VA_ARGS__)
#define UMSS_TRACE(level, tsmap, format, ...)   //SVC_TRACE(level,tsmap,format, ##__VA_ARGS__)

extern INT32 DRV_OpenTFlash(UINT8 *dev_name, HANDLE *pdev_handle);
extern INT32 DRV_CloseTFlash(HANDLE dev_handle);
extern INT32 DRV_GetDevInfoTFlash (HANDLE dev_handle, UINT32 *pBlockNr, UINT32 *pBlockSize);
extern INT32 DRV_ReadTFlash(HANDLE dev_handle, UINT32 blk_nr, BYTE *pBuffer);
extern INT32 DRV_WriteTFlash(HANDLE dev_handle, UINT32 blk_nr, BYTE *pBuffer);

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================
PRIVATE BOOL umss_StorageMcPresent(VOID* handler)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;
    INT32 result;
    UINT8 *dev_name = "MMC0";
    //HANDLE dev_handle;
    UINT32  uDevNo;
    UINT32 blk_nr = 0;
    UINT32 blk_sz = 0;

    UMSS_TRACE(UMSS_INFO_TRC, 0, "umss_StorageMcPresent() 0,begin.");

    if(NULL == handler)
    {
        return FALSE;
    }

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;

    result = DSM_GetDevSpaceSize(dev_name,&blk_nr,&blk_sz);
    if(result)
    {
        // hal_HstSendEvent(0x11001101);
        UMSS_TRACE(UMSS_INFO_TRC,
                   0,"umss_StorageMcPresent() 0,DSM_GetDevSpaceSize failed,result = %d.",result);
        return FALSE;
    }
    handlerMc->size.sizeBlock = blk_sz;
    handlerMc->size.nbBlock   = blk_nr;
    return TRUE;
}

BOOL g_umss_process = FALSE;
UINT8 g_umss_count = 0; // this function called when telephone power on.
PRIVATE VOID*  umss_StorageMcOpen(CONST UMSS_STORAGE_PARAMETERS_T* cfg)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc = NULL;
    UINT8 *dev_name = "MMC0";
    HANDLE dev_handle;
    INT32 result;
    hal_HstSendEvent(0xfdee1101);
    if(g_umss_count++>0)  g_umss_process = TRUE;

    cfg = cfg;
    dev_handle = DSM_GetDevHandle(dev_name);
    result = DRV_OpenTFlash(dev_name, &dev_handle);
    if(!dev_handle)
    {
        UMSS_TRACE(UMSS_INFO_TRC,
                   0,"umss_StorageMcOpen() 0,DSM_GetDevHandle failed,result = %d.",result);
        return NULL;
    }

    result = DRV_ActiveTFlash(dev_handle);
    if(result)
    {

        UMSS_TRACE(UMSS_INFO_TRC,
                   0,"umss_StorageMcOpen() 0,DRV_ActiveTFlash failed,result = %d.",result);
        return FALSE;
    }

    UMSS_TRACE(UMSS_INFO_TRC,
               0,"umss_StorageMcOpen() 0,open dev ok.");
    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*)sxr_Malloc(sizeof(UMSS_STORAGE_MC_HANDLER_T));
    handlerMc->dev_handle = dev_handle;
    UMSS_TRACE(UMSS_INFO_TRC, 0,"umss_StorageMcOpen() 0,open dev ok.");
    return((VOID*)(handlerMc));
}

PRIVATE VOID   umss_StorageMcClose(VOID* handler)
{
    hal_HstSendEvent(0xfd0011c1);

    g_umss_process = FALSE;
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;

    if(NULL == handler)
    {
        return;
    }
    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*)handler;
    DRV_DeactiveTFlash(handlerMc->dev_handle);
    if(handler)
    {
        sxr_Free(handler);
    }
}

PRIVATE INT32 umss_StorageMcRead(VOID* handler, VOID* data,
                                 UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_MC_HANDLER_T* handlerMc;
    INT32 result;
    UINT8* pbuff;
    UINT32 i;

    UMSS_TRACE(UMSS_INFO_TRC, 0, "umss_StorageMcRead() 0,begin.");

    if(NULL == handler)
    {
        UMSS_TRACE(UMSS_INFO_TRC, 0,"umss_StorageMcRead() 0,handle = NULL.");
        return -1;
    }

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;
    for(i = 0; i <  nbBlock; i++)
    {
        pbuff = (UINT8*)data + i * handlerMc->size.sizeBlock;
        result = DRV_ReadTFlash(handlerMc->dev_handle, lba+i,pbuff);
        if(result)
        {
            UMSS_TRACE(UMSS_INFO_TRC,
                       0,"umss_StorageMcRead() 0,DRV_ReadTFlash failed,result = %d.",result);
            return NULL;
        }
    }
    return(nbBlock);
}


PRIVATE INT32 umss_StorageMcWrite(VOID* handler, VOID* data,
                                  UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_MC_HANDLER_T*  handlerMc;
    INT32 result;
    UINT8* pbuff;
    UINT32 i;

    UMSS_TRACE(UMSS_INFO_TRC, 0, "umss_StorageMcWrite() 0,begin.");
    if(NULL == handler)
    {
        UMSS_TRACE(UMSS_INFO_TRC, 0,"umss_StorageMcWrite() 0,handle = NULL.");
        return -1;
    }

    handlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;
    for(i = 0; i <  nbBlock; i++)
    {
        pbuff = (UINT8*)data + i * handlerMc->size.sizeBlock;
        result = DRV_WriteTFlash(handlerMc->dev_handle, lba+i,pbuff);
        if(result)
        {
            UMSS_TRACE(UMSS_INFO_TRC,
                       0,"umss_StorageMcWrite\(),DRV_WriteTFlash failed,result = %d.",result);
            return NULL;
        }
    }
    return(nbBlock);
}

PRIVATE UMSS_STORAGE_SIZE_T* umss_StorageMcGetsize(VOID* handler)
{
    UMSS_STORAGE_MC_HANDLER_T* phandlerMc;

    phandlerMc = (UMSS_STORAGE_MC_HANDLER_T*) handler;
    UMSS_TRACE(UMSS_INFO_TRC, 0,"umss_StorageMcGetsize() 0 ok.handle = 0x%x.");
    return(&phandlerMc->size);
}

// ==

#endif
// =============================================================================
// FUNCTIONS
// =============================================================================
