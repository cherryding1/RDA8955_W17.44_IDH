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

#ifndef _MEMD_CACHE_H_
#define _MEMD_CACHE_H_

typedef enum
{
    NODE_TYPE_W,
    NODE_TYPE_E,
} NODE_TYPE;

typedef enum
{
    FLASH_NODE_STATUS_INIT,
    FLASH_NODE_STATUS_DOING,
    FLASH_NODE_STATUS_SUSPEND,
} NODE_STATUS;

typedef enum
{
    FLASH_REGION_USER_DATA,
#if defined(REDUNDANT_DATA_REGION)
    FLASH_REGION_REMAIN,
#endif
    FLASH_REGION_CALIB,
    FLASH_REGION_UNDEFINE,
} FLASH_REGION;

typedef struct _w_flash
{
    UINT32 flashAddress;
    UINT32 byteSize;
    UINT32 WrittenByteSize;
    UINT8* buffer;
    NODE_STATUS status;
} W_FLASH;

typedef struct _e_flash
{
    UINT32 startFlashAddress;
    UINT32 endFlashAddress;
    UINT32 time;
    NODE_STATUS status;
} E_FLASH;

union F_OPR
{
    W_FLASH w_flash;
    E_FLASH e_flash;
};
typedef struct _f_node
{
    NODE_TYPE type;
    UINT32 repeat;
    union F_OPR node;
    struct _f_node *next;
} F_NODE;

typedef struct _flash_cache
{
    UINT32 node_cnt_user;
#if defined(REDUNDANT_DATA_REGION)
    UINT32 node_cnt_remain;
#endif
    UINT32 node_cnt_calib;
    F_NODE* pnode_user;
#if defined(REDUNDANT_DATA_REGION)
    F_NODE* pnode_remain;
#endif
    F_NODE* pnode_calib;
    F_NODE* pnode_cur;
    FLASH_REGION flash_region_cur;
    UINT8 read_buff[8192];
    UINT8 sem;
} FLASH_CACHE;

extern VOID hal_DbgAssert(CONST CHAR *format, ...);
extern PVOID CSW_Malloc(UINT32 nSize);
extern BOOL CSW_Free (PVOID pMemBlock);
extern UINT8 sxr_NewSemaphore (UINT8 InitValue);
extern VOID sxr_TakeSemaphore (UINT8 Id);
extern VOID sxr_ReleaseSemaphore (UINT8 Id);
extern BOOL hal_HstSendEvent(UINT32 ch);
extern VOID DSM_Dump(UINT8 *pBuff, UINT32 iTotalSize, INT32 iLineSize);
extern VOID sxr_Sleep (UINT32 Period);

#define MEMD_Assert(bl, format, ...)         \
{if (!(bl)) {                                                            \
    hal_DbgAssert(format, ##__VA_ARGS__);       \
}}
#define MEMD_MALLOC CSW_Malloc
#define MEMD_FREE CSW_Free
#define MEMD_EVENT(x)    // hal_HstSendEvent(x)
//#define MEMD_CACHE_TRACE(level, tsmap, ...)  TS_OutputText(level, tsmap, ##__VA_ARGS__)
//#define TS_DUMP(a,b,c)  {DSM_Dump(a, b, c);}
#define MEMD_CACHE_TRACE(level, tsmap, ...)
#define TS_DUMP(a,b,c)

VOID memd_Init(VOID);
MEMD_ERR_T memd_CacheWrite(UINT8 *flashAddress,
                           UINT32 byteSize,
                           UINT32 * pWrittenByteSize,
                           UINT8* buffer);
MEMD_ERR_T memd_CacheRead(UINT8 *flashAddress,
                          UINT32 byteSize,
                          UINT32 * pReadByteSize,
                          UINT8* buffer);
MEMD_ERR_T memd_CacheErase(UINT8 *startFlashAddress, UINT8 *endFlashAddress,UINT32 time);
MEMD_ERR_T memd_CacheFlush(FLASH_REGION flash_region);
UINT32 memd_CacheGetNodeCount(FLASH_REGION flash_region);
VOID memd_RemoveAllNode(FLASH_REGION flash_region);

#define ERR_DRV_CACHE_NO                                    0
#define ERR_DRV_CACHE_UNINIT                          -100
#define ERR_DRV_CACHE_SUSPEND                       -101
#define ERR_DRV_CACHE_BUSY                              -102
#define ERR_DRV_CACHE_SPI_BUSY                       -103
#define ERR_DRV_CACHE_PARA_ERROR                  -104
#define ERR_DRV_CACHE_WRITE_ERROR                -105
#define ERR_DRV_CACHE_READ_ERROR                  -106
#define ERR_DRV_ERASE_ERROR                             -107
#define ERR_DRV_CACHE_MALLOC                          -108
#define ERR_DRV_CACHE_CHECK_ERROR                  -109

#endif
