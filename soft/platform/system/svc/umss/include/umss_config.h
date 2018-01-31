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


#ifndef _UMSS_CONFIG_H_
#define _UMSS_CONFIG_H_

#include "cs_types.h"

#include "hal_usb.h"

/// @file umss_config.h
///
/// @addtogroup umss
/// @{
///

// =============================================================================
// MACROS
// =============================================================================

#ifndef UMSS_STORAGE_MAX
#define UMSS_STORAGE_MAX 4
#endif /* UMMS_STORAGE_MAX */

#ifndef SEEK_SET
#define SEEK_SET         0
#endif /* SEEK_SET */

#ifndef SEEK_CUR
#define SEEK_CUR         1
#endif /* SEEK_CUR */

#ifndef SEEK_END
#define SEEK_END         2
#endif /* SEEK_END */


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// UMSS_STORAGE_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of storage configuration
// =============================================================================
typedef struct
{
    /// Size of storage (for RAM storage)
    CONST UINT64 size;
} UMSS_STORAGE_PARAMETERS_T;

// =============================================================================
// UMSS_TRANSPORT_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of storage configuration
// =============================================================================
typedef struct
{
    /// EndPoint for in
    CONST UINT32 inEpNum ;
    /// EndPoint for out
    CONST UINT32 outEpNum;
} UMSS_TRANSPORT_PARAMETERS_T;

// =============================================================================
// UMSS_STORAGE_SIZE_T
// -----------------------------------------------------------------------------
/// Describe the number of sector and size of sector
// =============================================================================
typedef struct
{
    /// Number of block in storage
    UINT32 nbBlock;
    /// Size of block in storage
    UINT32 sizeBlock;
} UMSS_STORAGE_SIZE_T;

// =============================================================================
// UMSS_STORAGE_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call when the storage is open
/// @param cfg Storage configuration
/// @return handler of storage
// =============================================================================
typedef VOID* (*UMSS_STORAGE_OPEN_CALLBACK_T)
(CONST UMSS_STORAGE_PARAMETERS_T* cfg);

// =============================================================================
// UMSS_STORAGE_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call when the storage is close
/// @param handler storage handler
// =============================================================================
typedef VOID (*UMSS_STORAGE_CLOSE_CALLBACK_T)(VOID* handler);


// =============================================================================
// UMSS_STORAGE_READ_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use for read on storage
/// @param handler storage handler
/// @param data buffer use for read data
/// @param lba first sector must be read
/// @param size number of block must be read
/// @return number of block read or -1 on error
// =============================================================================
typedef INT32 (*UMSS_STORAGE_READ_CALLBACK_T)
(VOID* handler, VOID* data, UINT32 lba, UINT32 size);

// =============================================================================
// UMSS_STORAGE_WRITE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use for write on storage
/// @param handler storage handler
/// @param data buffer use for write data
/// @param lba first sector must be write
/// @param size number of block must be write
/// @return number of block write or -1 on error
// =============================================================================
typedef INT32 (*UMSS_STORAGE_WRITE_CALLBACK_T)
(VOID* handler, VOID* data, UINT32 lba, UINT32 size);

// =============================================================================
// UMSS_STORAGE_GETSIZE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use to return the geometry of storage
/// @param handler storage handler
/// @return geometry of storage. See #UMSS_STORAGE_SIZE_T
// =============================================================================
typedef UMSS_STORAGE_SIZE_T* (*UMSS_STORAGE_GETSIZE_CALLBACK_T)(VOID* handler);

// =============================================================================
// UMSS_STORAGE_PRESENT_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback use to send the status of storage
/// @param handler storage handler
/// @return FALSE, if the media is not present or not ready, TRUE when the
/// storage is present and ready
// =============================================================================
typedef BOOL (*UMSS_STORAGE_PRESENT_CALLBACK_T)(VOID* handler);

// =============================================================================
// UMSS_STORAGE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Storage Callback
// =============================================================================
typedef struct
{
    CONST UMSS_STORAGE_OPEN_CALLBACK_T        open;
    CONST UMSS_STORAGE_CLOSE_CALLBACK_T      close;
    CONST UMSS_STORAGE_READ_CALLBACK_T        read;
    CONST UMSS_STORAGE_WRITE_CALLBACK_T       write;
    CONST UMSS_STORAGE_GETSIZE_CALLBACK_T   getsize;
    CONST UMSS_STORAGE_PRESENT_CALLBACK_T   present;
} UMSS_STORAGE_CALLBACK_T;

// =============================================================================
// UMSS_STORAGE_CFG_T
// -----------------------------------------------------------------------------
/// Storage configuration
// =============================================================================
typedef struct
{
    /// storage callback
    CONST UMSS_STORAGE_CALLBACK_T*      storageCallback;
    /// configuration parameter for storage
    CONST UMSS_STORAGE_PARAMETERS_T     storageParameters;
} UMSS_STORAGE_CFG_T;


// =============================================================================
// UMSS_TRANSPORT_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the transport
/// @param cfg Configuration for the transport layers
/// @param storageCfg Configurations for each storages
/// @param storageHandler Handlers for each storages
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*UMSS_TRANSPORT_OPEN_CALLBACK_T)
(CONST UMSS_TRANSPORT_PARAMETERS_T*  cfg,
 CONST UMSS_STORAGE_CFG_T*           storageCfg,
 VOID**                              storageHandler);


// =============================================================================
// UMSS_TRANSPORT_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the transport
// =============================================================================
typedef VOID (*UMSS_TRANSPORT_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// UMSS_TRANSPORT_CALLBACK_T
// -----------------------------------------------------------------------------
/// Transport Callback
// =============================================================================
typedef struct
{
    /// See #UMSS_TRANSPORT_OPEN_CALLBACK_T
    CONST UMSS_TRANSPORT_OPEN_CALLBACK_T  open;
    /// See #UMSS_TRANSPORT_CLOSE_CALLBACK_T
    CONST UMSS_TRANSPORT_CLOSE_CALLBACK_T close;
} UMSS_TRANSPORT_CALLBACK_T;

// =============================================================================
// UMSS_TRANSPORT_CFG_T
// -----------------------------------------------------------------------------
/// Trasnport configuration
// =============================================================================
typedef struct
{
    /// transport callback
    CONST UMSS_TRANSPORT_CALLBACK_T*   transportCallback;
    /// configuration parameter of transport
    CONST UMSS_TRANSPORT_PARAMETERS_T  transportParameters;
} UMSS_TRANSPORT_CFG_T;

// =============================================================================
// UMSS_CFG_T
// -----------------------------------------------------------------------------
/// Structure for mss configuration
// =============================================================================
typedef struct
{
    /// Transport configuration
    CONST UMSS_TRANSPORT_CFG_T         transportCfg;
    /// Storage configuration
    CONST UMSS_STORAGE_CFG_T           storageCfg[UMSS_STORAGE_MAX];
} UMSS_CFG_T;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// g_umssTransportCallbackBoScsi
// -----------------------------------------------------------------------------
/// Callback of SCSI transport + Bulk Only
// =============================================================================
EXPORT PUBLIC CONST UMSS_TRANSPORT_CALLBACK_T   g_umssTransportBoScsiCallback;

// =============================================================================
// g_umssStorageCallbackRam
// -----------------------------------------------------------------------------
/// Callback of RAM storage
// =============================================================================
EXPORT PUBLIC CONST UMSS_STORAGE_CALLBACK_T     g_umssStorageRamCallback;

// =============================================================================
// g_umssStorageFlashCallback
// -----------------------------------------------------------------------------
/// Callback of FLASH storage
// =============================================================================
EXPORT PUBLIC CONST UMSS_STORAGE_CALLBACK_T     g_umssStorageFlashCallback;

#ifdef DUAL_TFLASH_SUPPORT

// =============================================================================
// g_umssStorageCallbackMc0
// -----------------------------------------------------------------------------
/// Callback of MC0 storage
// =============================================================================
EXPORT PUBLIC CONST UMSS_STORAGE_CALLBACK_T     g_umssStorageMc0Callback;


// =============================================================================
// g_umssStorageCallbackMc1
// -----------------------------------------------------------------------------
/// Callback of MC1 storage
// =============================================================================
EXPORT PUBLIC CONST UMSS_STORAGE_CALLBACK_T     g_umssStorageMc1Callback;

#else
// =============================================================================
// g_umssStorageCallbackMc
// -----------------------------------------------------------------------------
/// Callback of MC1 storage
// =============================================================================
EXPORT PUBLIC CONST UMSS_STORAGE_CALLBACK_T     g_umssStorageMcCallback;

#endif
// =============================================================================
// FUNCTIONS
// =============================================================================

/// @} <- End of the umss group

#endif // _UMSS_CONFIG_H_
