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

#ifndef __DEV_DRIVER_H__
#define __DEV_DRIVER_H__

// Device handle validity define.
// Invalid device handle.
#define INVALID_DEVICE_NUMBER                 0

// Device state define.
// Device is opened.
#define DEV_STATE_OPEN                               0

// Device is closed.
#define DEV_STATE_CLOSE                              1

// Partition name size.
#define DSM_PART_NAME_SZ                        16

// Driver adapter defualt cache block  count
#define DSM_DEFAULT_CACHE_CNT                8

// default sector size.
#define DSM_DEFAULT_SECSIZE                     512

// Device manage error code
#define ERR_DRV_INVALID_PARA                    -1001
#define ERR_DRV_INVALID_DEVNO                  -1002
#define ERR_DRV_INVALID_DEVNAME              -1003
#define ERR_DRV_NO_MORE_MEM                    -1004
#define ERR_DRV_GET_DEV_FAILED                -1005
#define ERR_DRV_DEV_NOT_INIT                    -1006
#define ERR_DRV_ERASE_FAILED                    -1007
#define ERR_DRV_READ_FAILED                      -1008
#define ERR_DRV_WRITE_FAILED                    -1009

// DSM patition config define.

// Module id
typedef enum
{
    DSM_MODULE_FS_ROOT,    // FS root directory
    DSM_MODULE_FS,              // FS mount device.
    DSM_MODULE_NVRAM,       // NVRAM module.
    DSM_MODULE_LTENV,       //LTENV module.
    DSM_MODULE_NBPSM,       //NBPSM module.
} DSM_MODULE_ID;

// Check level.
typedef enum
{
    DSM_CHECK_LEVEL_1,        // Checkout level1:writing PBD and PB checkout.
    DSM_CHECK_LEVEL_2,        // Checkout level2:writing PBD checkout.
    DSM_CHECK_LEVEL_3,        // Checkout level3:no checkout..
} DSM_CHECK_LEVEL;

// Device type.
typedef enum
{
    DSM_MEM_DEV_FLASH,       // Flash
    DSM_MEM_DEV_TFLASH,     // T-Flash
} DSM_DEV_TYPE;

// partition config information.
typedef struct _dsm_partition_config
{
    UINT8 szPartName[DSM_PART_NAME_SZ];         // partition name
    DSM_DEV_TYPE eDevType;                               // Device type
    DSM_CHECK_LEVEL eCheckLevel;                     // Checkout level
    UINT16 uSecCnt;                                               // Sector count
    UINT16 uRsvBlkCnt;                                          // Reserved block count
    DSM_MODULE_ID eModuleId;                            // Module id
} DSM_PARTITION_CONFIG;

// DSM Device Information structure. this structure contain the device summary information about devices.
typedef struct _dsm_dev_info
{
    UINT32 dev_no;
    UINT8  dev_name[DSM_PART_NAME_SZ];              // Device name
    DSM_DEV_TYPE dev_type;                                    // Device type.
    DSM_MODULE_ID module_id;                               // Module id
} DSM_DEV_INFO;



// Function point type define.

// Open device.
// Return ERR_SUCCESS if open device successfully
// and device handle is returned by @pDevHandle; else other error code is returned.
typedef INT32 (*PF_DEV_OPEN)( PCSTR pszDevName, UINT32 *pDevHandle );

// Close device.
// Return ERR_SUCCESS if close device successfully ; else other error code is returned.
typedef INT32 (*PF_DEV_ClOSE)( UINT32 dev_handle);

// Read block.
// Return ERR_SUCCESS if read block from device successfully;
// else other error code is returned.
typedef INT32 ( *PF_DEV_BLOCKREAD ) ( UINT32 dev_handle, UINT32 blk_nr, UINT8* buf );

// Write block.
// Return ERR_SUCCESS if write block to device successfully;
// else other error code is returned.
typedef INT32 ( *PF_DEV_BLOCKWRITE ) ( UINT32 dev_handle, UINT32 blk_nr, UINT8* buf );

// Revert block.
// Return ERR_SUCCESS if revert block successfully;
// else other error code is returned.
typedef INT32 ( *PF_DEV_BLOCKREVERT ) ( UINT32 dev_handle, UINT32 blk_nr);

// Get Device information.
// Return ERR_SUCCESS if write block to device successfully;
// else other error code is returned.
typedef INT32 ( *PF_DEV_GETDEVINFO )( UINT32 dev_handle, UINT32 *piNrBlock, UINT32 *piBlockSize );

typedef INT32 (*PF_DEV_SET_CACHE_SIZE)( UINT32 dev_handle, UINT32 cache_size);

// Get cache size.
// Return the cache size if get size successfully; else other zero is returned.
typedef INT32 (*PF_DEV_GET_CACHE_SIZE)( UINT32 dev_handle);

// Active device.
// Return ERR_SUCCESS if active device successfully, else other error code is returned.
typedef INT32 (*PF_DEV_ACTIVE)(UINT32 dev_handle);

// Deactive device.
// Return ERR_SUCCESS if deactive device successfully ; else other error code is returned.
typedef INT32 (*PF_DEV_DEACTIVE)( UINT32 dev_handle);

// Flush cache buffer.
// Return ERR_SUCCESS if flush cache buffer successfully ; else other error code is returned.
typedef INT32 (*PF_DEV_FLUSH)( UINT32 dev_handle);

// DSM Device Description structure.This structure contain all the field about device manage.
typedef struct _dsm_dev_descrip
{
    UINT8 dev_name[DSM_PART_NAME_SZ];            // device name.
    DSM_MODULE_ID module_id;                             // module id.
    INT32 dev_handle;                                            // device handle.
    UINT32 dev_state;                                             // device state.
    DSM_DEV_TYPE dev_type;                                 // device type.
    PF_DEV_OPEN open;                                           // Funcition point-open.
    PF_DEV_ClOSE close;                                         // Function point-close.
    PF_DEV_BLOCKREAD bread;                               // Function point-read.
    PF_DEV_BLOCKWRITE bwrite;                            // Function point-write.
    PF_DEV_BLOCKREVERT brevert;                        // Function point-revert.
    PF_DEV_GETDEVINFO get_dev_info;                 // Function point get device information.
    PF_DEV_SET_CACHE_SIZE set_rcache_size;      // Function point set r_cache size.
    PF_DEV_GET_CACHE_SIZE get_rcache_size;      // Function point get r_cache size.
    PF_DEV_SET_CACHE_SIZE set_wcache_size;      // Function point set w_cache size.
    PF_DEV_GET_CACHE_SIZE get_wcache_size;      // Function point get w_cache size.
    PF_DEV_ACTIVE active;                                       // Funcition point-active device.
    PF_DEV_DEACTIVE deactive;                               // Funcition point-deactive device.
    PF_DEV_FLUSH flush;                                           // Funcition point-flush.
} DSM_DEV_DESCRIP;


// DSM Device access api define.
// Block read.
#define DRV_BLOCK_READ( dev_nr, blk_nr, buf  ) \
    g_pDevDescrip[ dev_nr ].bread( g_pDevDescrip[ dev_nr ].dev_handle,blk_nr,(UINT8*)buf)

// Block write.
#define DRV_BLOCK_WRITE( dev_nr, blk_nr, buf  ) \
    g_pDevDescrip[ dev_nr ].bwrite( g_pDevDescrip[ dev_nr ].dev_handle,blk_nr,(UINT8*)buf)

// Block revert.
#define DRV_BLOCK_REVERT( dev_nr, blk_nr) \
    g_pDevDescrip[ dev_nr ].brevert( g_pDevDescrip[ dev_nr ].dev_handle,blk_nr)

// Get device information.
#define DRV_GET_DEV_INFO( dev_nr, piNrBlock, piBlockSize ) \
    g_pDevDescrip[ dev_nr ].get_dev_info( g_pDevDescrip[ dev_nr ].dev_handle, (UINT32*)piNrBlock, piBlockSize )

#define DRV_SET_RCACHE_SIZE( dev_nr, cache_size ) \
    g_pDevDescrip[ dev_nr ].set_rcache_size( g_pDevDescrip[ dev_nr ].dev_handle, cache_size)

// Get r_cache size.
#define DRV_GET_RCACHE_SIZE( dev_nr) \
    g_pDevDescrip[ dev_nr ].get_rcache_size( g_pDevDescrip[ dev_nr ].dev_handle)

// Setup w_cache size.
#define DRV_SET_WCACHE_SIZE( dev_nr, cache_size ) \
    g_pDevDescrip[ dev_nr ].set_wcache_size( g_pDevDescrip[ dev_nr ].dev_handle, cache_size)

// Get w_cache size.
#define DRV_GET_WCACHE_SIZE( dev_nr) \
    g_pDevDescrip[ dev_nr ].get_wcache_size( g_pDevDescrip[ dev_nr ].dev_handle)

// Active device.
#define DRV_DEV_ACTIVE( dev_nr) \
    g_pDevDescrip[ dev_nr ].active( g_pDevDescrip[ dev_nr ].dev_handle)

// Deactive device.
#define DRV_DEV_DEACTIVE( dev_nr) \
    g_pDevDescrip[ dev_nr ].deactive( g_pDevDescrip[ dev_nr ].dev_handle)

// Flush cache buffer.
#define DRV_DEV_FLUSH( dev_nr) \
    g_pDevDescrip[ dev_nr ].flush( g_pDevDescrip[ dev_nr ].dev_handle)

// Device initializing.
INT32 DSM_DevInit( VOID );

// Clear the data on flash user region.
//  Must reset the system after called this function.
INT32 DSM_UserDataClear(VOID);

// Get device No depend on device name.
UINT32 DSM_DevName2DevNo(
    PCSTR pszDevName
);

// Get device name.
INT32 DSM_DevNr2Name(
    UINT32 uDevNo,
    PSTR pszDevName
);

// Get device type.
INT32 DSM_GetDevType(
    UINT32 uDevNo
);

// Get FS root device name.
INT32 DSM_GetFsRootDevName(
    PSTR pszDevName
);

// Get device no depend on mudule id.
INT32 DSM_GetDevNrOnModuleId(
    DSM_MODULE_ID eModuleId
);

// Get device count.
INT32 DSM_GetDevCount(VOID);

// Get device module id.
DSM_MODULE_ID DSM_GetDevModuleId(
    UINT32 uDevNo
);

// Get device information.
INT32 DSM_GetDeviceInfo(
    UINT32 *pDeviceCount,
    DSM_DEV_INFO **ppDevInfo
);

// DSM read function. Support random bytes read.
INT32 DSM_Read( UINT32 uDevNr,
                UINT32 ulAddrOffset,
                UINT8 *pBuffer,
                UINT32 ulBytesToRead,
                UINT32 *pBytesRead
              );

// DSM write function. Support random bytes write.
INT32 DSM_Write(
    UINT32 uDevNr,
    UINT32 ulAddrOffset,
    CONST UINT8 *pBuffer,
    UINT32 ulBytesToWrite,
    UINT32 *pBytesWritten
);

// DSM get device space size.
INT32 DSM_GetDevSpaceSize(PSTR pszDevName,
               UINT32* pBlkCount,
                          UINT32 *pBlkSize
                         );

// Get dvice handle,
// return the device handle,the handle returned by device open function.
UINT32 DSM_GetDevHandle( PCSTR pszDevName );

// Device information table.
extern DSM_DEV_DESCRIP *g_pDevDescrip;

#endif
