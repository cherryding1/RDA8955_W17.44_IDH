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


#include "sxr_mem.h"
#include "umss_m.h"

#ifndef ERR_SUCCESS
#define ERR_SUCCESS 0
#endif

typedef struct
{
    UMSS_STORAGE_SIZE_T  size;
    UINT32  nDevNo;
} UMSS_STORAGE_FLASH_HANDLER_T;

UMSS_STORAGE_FLASH_HANDLER_T g_umss_flash_handler;

extern UFLASH_WRTBLK_FUNC_P  gpf_uflash_write_block;
extern UFLASH_RDBLK_FUNC_P    gpf_uflash_read_block;

extern void * memset(void *,int,size_t);
extern void * memcpy(void *,const void *,size_t);

PRIVATE VOID* umss_StorageFlashOpen   (CONST UMSS_STORAGE_PARAMETERS_T* cfg);
PRIVATE VOID umss_StorageFlashClose  (VOID* handler);
PRIVATE INT32 umss_StorageFlashRead   (VOID* handler, VOID* data, UINT32 lba, UINT32 size);
PRIVATE INT32 umss_StorageFlashWrite  (VOID* handler, VOID* data, UINT32 lba, UINT32 size);
PRIVATE UMSS_STORAGE_SIZE_T* umss_StorageFlashGetsize(VOID* handler);
PRIVATE BOOL umss_StorageFlashPresent(VOID* handler);

PUBLIC CONST UMSS_STORAGE_CALLBACK_T g_umssStorageFlashCallback =
{
    .open    = umss_StorageFlashOpen,
    .close   = umss_StorageFlashClose,
    .read    = umss_StorageFlashRead,
    .write   = umss_StorageFlashWrite,
    .getsize = umss_StorageFlashGetsize,
    .present = umss_StorageFlashPresent
};


void init_uFlash_Param(UINT32 devNo, UINT32 nrBlock, UINT32 blkSize)
{
    g_umss_flash_handler.nDevNo = devNo;
    g_umss_flash_handler.size.nbBlock = nrBlock;
    g_umss_flash_handler.size.sizeBlock = blkSize;
}

PRIVATE VOID* umss_StorageFlashOpen   (CONST UMSS_STORAGE_PARAMETERS_T* cfg)
{
    UMSS_STORAGE_FLASH_HANDLER_T* handle;

    cfg = cfg;

    if ((gpf_uflash_write_block == NULL)
            || (gpf_uflash_read_block == NULL)
            || (g_umss_flash_handler.size.nbBlock == 0)
            || (g_umss_flash_handler.size.sizeBlock == 0))
    {
        return NULL;
    }

    handle = (UMSS_STORAGE_FLASH_HANDLER_T*)sxr_Malloc(sizeof(UMSS_STORAGE_FLASH_HANDLER_T));

    return((VOID*)(handle));
}

PRIVATE VOID umss_StorageFlashClose  (VOID* handler)
{
    if(handler)
    {
        sxr_Free(handler);
    }
}

PRIVATE INT32 umss_StorageFlashRead   (VOID* handler, VOID* data, UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_FLASH_HANDLER_T* handlerFlash;
    INT32   iRet = -1;
    UINT32  i = 0;
    UINT8*  pData = NULL;

    handlerFlash = (UMSS_STORAGE_FLASH_HANDLER_T*) handler;
    if( !handlerFlash || !data )
    {
        return -2;
    }
    if( handlerFlash->size.nbBlock < (lba + nbBlock))
    {
        return -2;
    }

    pData = (UINT8*)data;
    for( i = 0; i < nbBlock; i++ )
    {
        iRet = (*gpf_uflash_read_block)(handlerFlash->nDevNo, (lba + i), (UINT8*)pData);
        if( iRet != 0 )
        {
            return -2;
        }
        pData += (handlerFlash->size.sizeBlock);
    }

    return nbBlock;
}

PRIVATE INT32 umss_StorageFlashWrite  (VOID* handler, VOID* data, UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_FLASH_HANDLER_T* handlerFlash;
    INT32   iRet = -1;
    UINT32  i = 0;
    UINT8*  pData = NULL;

    handlerFlash = (UMSS_STORAGE_FLASH_HANDLER_T*) handler;
    if( !handlerFlash || !data )
    {
        return -2;
    }
    if( handlerFlash->size.nbBlock < (lba + nbBlock))
    {
        return -2;
    }

    pData = (UINT8*)data;
    for( i = 0; i < nbBlock; i++ )
    {
        iRet = (*gpf_uflash_write_block)( handlerFlash->nDevNo, (lba + i), (UINT8*)pData);
        if( iRet != 0 )
        {
            return -2;
        }
        pData += (handlerFlash->size.sizeBlock);
    }
    return nbBlock;
}

PRIVATE UMSS_STORAGE_SIZE_T* umss_StorageFlashGetsize(VOID* handler)
{
    UMSS_STORAGE_FLASH_HANDLER_T* handlerFlash =
        (UMSS_STORAGE_FLASH_HANDLER_T*) handler;

    if( !handlerFlash )
    {
        return NULL;
    }

    return(&handlerFlash->size);
}

PRIVATE BOOL umss_StorageFlashPresent(VOID* handler)
{
    UMSS_STORAGE_FLASH_HANDLER_T* handlerFlash =
        (UMSS_STORAGE_FLASH_HANDLER_T*)handler;

    if( !handlerFlash)
    {
        return FALSE;
    }

    memcpy(handlerFlash, &g_umss_flash_handler, sizeof(UMSS_STORAGE_FLASH_HANDLER_T));

    return(TRUE);
}

