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

#include <string.h>

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

// =============================================================================
// UMSS_STORAGE_RAM_HANDLER_T
// -----------------------------------------------------------------------------
/// Ram handler
// =============================================================================
typedef struct
{
    UMSS_STORAGE_SIZE_T  size;
    VOID*                ram;
} UMSS_STORAGE_RAM_HANDLER_T;

// =============================================================================
// PROTOTYPE FUNCTIONS
// =============================================================================

PRIVATE VOID*
umss_StorageRamOpen   (CONST UMSS_STORAGE_PARAMETERS_T* cfg);

PRIVATE VOID
umss_StorageRamClose  (VOID* handler);

PRIVATE INT32
umss_StorageRamRead   (VOID* handler, VOID* data, UINT32 lba, UINT32 size);

PRIVATE INT32
umss_StorageRamWrite  (VOID* handler, VOID* data, UINT32 lba, UINT32 size);

PRIVATE UMSS_STORAGE_SIZE_T*
umss_StorageRamGetsize(VOID* handler);

PRIVATE BOOL
umss_StorageRamPresent(VOID* handler);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST UMSS_STORAGE_CALLBACK_T g_umssStorageRamCallback =
{
    .open    = umss_StorageRamOpen,
    .close   = umss_StorageRamClose,
    .read    = umss_StorageRamRead,
    .write   = umss_StorageRamWrite,
    .getsize = umss_StorageRamGetsize,
    .present = umss_StorageRamPresent
};

// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE BOOL umss_StorageRamPresent(VOID* handler)
{
    UMSS_STORAGE_RAM_HANDLER_T* handlerRam;

    handlerRam = (UMSS_STORAGE_RAM_HANDLER_T*) handler;

    return(TRUE);
}

PRIVATE VOID*  umss_StorageRamOpen(CONST UMSS_STORAGE_PARAMETERS_T* cfg)
{
    UMSS_STORAGE_RAM_HANDLER_T* handlerRam;
    UINT32                      size;

    if(cfg == 0)
    {
        return 0;
    }

    handlerRam = (UMSS_STORAGE_RAM_HANDLER_T*)
                 sxr_Malloc(sizeof(UMSS_STORAGE_RAM_HANDLER_T));

    handlerRam->size.nbBlock   = cfg->size/512;
    handlerRam->size.sizeBlock = 512;
    size                       =
        handlerRam->size.sizeBlock * handlerRam->size.nbBlock;
    handlerRam->ram            = sxr_Malloc(size);

    if(handlerRam->ram == 0)
    {
        sxr_Free(handlerRam);
        handlerRam = 0;
    }

    return((VOID*)(handlerRam));
}

PRIVATE VOID   umss_StorageRamClose(VOID* handler)
{
    UMSS_STORAGE_RAM_HANDLER_T* handlerRam;

    handlerRam = (UMSS_STORAGE_RAM_HANDLER_T*) handler;

    if(handler)
    {
        sxr_Free(handlerRam->ram);
        sxr_Free(handler);
    }
}

PRIVATE INT32 umss_StorageRamRead(VOID* handler, VOID* data, UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_RAM_HANDLER_T* handlerRam;
    VOID*                       dataMem;
    UINT32                      size;

    handlerRam = (UMSS_STORAGE_RAM_HANDLER_T*) handler;

    size             = handlerRam->size.sizeBlock*nbBlock;

    dataMem = &((UINT8*)handlerRam->ram)[lba*handlerRam->size.sizeBlock];

    memcpy(data, dataMem, size);

    return(nbBlock);
}

PRIVATE INT32 umss_StorageRamWrite(VOID* handler, VOID* data, UINT32 lba, UINT32 nbBlock)
{
    UMSS_STORAGE_RAM_HANDLER_T* handlerRam;
    VOID*                       dataMem;
    UINT32                      size;

    handlerRam = (UMSS_STORAGE_RAM_HANDLER_T*) handler;

    size             = handlerRam->size.sizeBlock*nbBlock;

    dataMem = &((UINT8*)handlerRam->ram)[lba*handlerRam->size.sizeBlock];

    memcpy(dataMem, data, size);

    return(nbBlock);
}

PRIVATE UMSS_STORAGE_SIZE_T* umss_StorageRamGetsize(VOID* handler)
{
    UMSS_STORAGE_RAM_HANDLER_T* handlerRam;

    handlerRam = (UMSS_STORAGE_RAM_HANDLER_T*) handler;

    return(&handlerRam->size);
}

// =============================================================================
// FUNCTIONS
// =============================================================================
