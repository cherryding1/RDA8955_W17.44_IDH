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

#ifndef _FACTORY_LOCAL_H
#define _FACTORY_LOCAL_H


#define INVALID_BLOCK_INDEX                  0xffffffff
#define INVALID_SN                                    0xffffffff
#define INVALID_SECTOR_INDEX                0xffffffff

#define FLUSH_SUCCESS                   0
#define FLUSH_SUSPEND                   1
#define FLUSH_BUSY                         2
#define FLUSH_ERROR                       3


typedef enum
{
    FACT_STATUS_INIT,
    FACT_STATUS_IDLE,
    FACT_STATUS_FLUSH_BEGIN,
    FACT_STATUS_WRITE_BLOCK,
    FACT_STATUS_ERASE_BEGIN,
    FACT_STATUS_ERASE_CONTINUE,
    FACT_STATUS_ERASE_FINISH,
    FACT_STATUS_FLUSH_FINISH,
    FACT_STATUS_ERROR,
} FACTORY_STATUS;

// Block information structure.
typedef  struct _sector_info
{
    UINT32 start_addr;                // flash start address.
    UINT32 sec_size;                   // sector size.
    UINT32 sec_count;                // sector count.
} SECTOR_INFO;


typedef  struct _block_info
{
    UINT32 blk_size;                   // block size
    UINT32 blk_count;                // block count
    UINT32 cur_blk_idx;            // Current block index;
    UINT32 cur_sn;                    // Current sn;
} BLOCK_INFO;

typedef struct
{
    UINT8 has_init;
    COS_SEMA sem;
    UINT32 cur_index;
    SECTOR_INFO sec_info;
    BLOCK_INFO blk_info;
    FACTORY_BLOCK_T rw_data;
    FACTORY_BLOCK_T tmp_data;
} FACTORY_INFO;

extern UINT32 vds_CRC32( CONST VOID *pvData, INT32 iLen );
#define BLK_CRC32 vds_CRC32

//#define REMAIN_TRACE CSW_TRACE
#define FACTORY_TS_ID 13
#define FACTORY_TRACE(id,pFormat, ...)  // TS_OutputText(id, (PCSTR)pFormat, ##__VA_ARGS__)
#define FACTORY_SendEvent(x)   hal_HstSendEvent(x)
#endif
