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

#ifndef _VDS_API_H_
#define _VDS_API_H_

// Error code

#define ERR_VDS_BASE_CODE                                    -2000

// Parameter error.
#define ERR_VDS_PARA_ERROR                       (ERR_VDS_BASE_CODE-1)

// VDS uninitalized.
#define ERR_VDS_UNINIT                                (ERR_VDS_BASE_CODE-2)

// VDS configure error
#define ERR_VDS_CONFIG_ERROR                   (ERR_VDS_BASE_CODE-3)

// This partition not exist.
#define ERR_VDS_PARTITION_NOT_EXIST      (ERR_VDS_BASE_CODE-4)

// Free list is null.
#define ERR_VDS_FREE_LIST_NULL                 (ERR_VDS_BASE_CODE-5)

// Change sector status failed.
#define ERR_VDS_CHANGE_SECTOR_STATUS  (ERR_VDS_BASE_CODE-6)

// Format sector failed.
#define ERR_VDS_FORMAT_SEC_FAILED         (ERR_VDS_BASE_CODE-7)

// Read flash failed.
#define ERR_VDS_FLASH_DRV_READ              (ERR_VDS_BASE_CODE-8)

// Write flash failed.
#define ERR_VDS_FLASH_DRV_WRITE            (ERR_VDS_BASE_CODE-9)

// Erase flash failed.
#define ERR_VDS_FLASH_DRV_ERASE            (ERR_VDS_BASE_CODE-10)

// Space full.
#define ERR_VDS_SPACE_FULL                      (ERR_VDS_BASE_CODE-11)

// No more memory.
#define ERR_VDS_NO_MORE_MEM                  (ERR_VDS_BASE_CODE-12)

// No more sector reference.
#define ERR_VDS_NO_MORE_SECT_REF        (ERR_VDS_BASE_CODE-13)

// PB status is invalid.
#define ERR_VDS_STATUS_INVALID             (ERR_VDS_BASE_CODE-14)

// CRC check error.
#define ERR_VDS_CRC_ERROR                      (ERR_VDS_BASE_CODE-15)

// VDS uninit
#define _ERR_VDS_UNINIT                           (ERR_VDS_BASE_CODE-16)

// Function declaring.
INT32 VDS_Init(VOID);
INT32 VDS_FormatPartition(PCSTR pszPartName);
INT32 VDS_Open(PCSTR pszPartName, INT32 *pDevHandle );
INT32 VDS_Close( PCSTR pszPartName );
INT32 VDS_WriteBlock( UINT32 iPartId, UINT32 iVBNum, UINT8*pBuff );
INT32 VDS_ReadBlock( UINT32 iPartId, UINT32 iVBNum, UINT8* pBuff );
INT32 VDS_RevertBlock( UINT32 iPartId, UINT32 iVBNum);
INT32 VDS_GetPartitionInfo( UINT32 iPartId, UINT32 *piNrBlock, UINT32 *piBlockSize );
VOID  VDS_Deactive(HANDLE dev_handle);
INT32 VDS_Active(HANDLE dev_handle);
UINT32 VDS_GetRCacheSize(HANDLE dev_handle);
UINT32 VDS_GetWCacheSize(HANDLE dev_handle);
VOID VDS_SetWCacheSize(HANDLE dev_handle, UINT32 cache_size);
VOID VDS_SetRCacheSize(HANDLE dev_handle, UINT32 cache_size);
INT32 VDS_Flush(HANDLE dev_handle);
UINT32 VDS_GetPartitionCount(VOID);
UINT32 VDS_GetVersion(VOID);
INT32 VDS_SetAllSectorsEraseFlag(VOID);

#endif // _VDS_API_H_





