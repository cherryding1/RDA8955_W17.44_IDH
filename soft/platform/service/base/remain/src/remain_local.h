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

#ifndef _REMAIN_LOCAL_H
#define _REMAIN_LOCAL_H

#define REMAIN_BITMAP_SIZE     32
#define REMAIN_SECTORS_COUNT_MAX   32
#define REMAIN_MAIGIC                      0xbabefcfe
#define REMAIN_UNFORMAT_MAIGIC   0xffffffff
#define REMAIN_STATE_UNFORMAT       0xffff
#define REMAIN_STATE_FREE                0xcafe
#define REMAIN_STATE_TEMP                0xcaf0
#define REMAIN_STATE_CURRENT          0xca00
#define REMAIN_STATE_DIRTY               0xc000
#define REMAIN_STATE_ERASE               0x0

#define REMAIN_REC_STATE_FREE        0xff
#define REMAIN_REC_STATE_TEMP      0xf2
#define REMAIN_REC_STATE_VALID      0xf0
#define REMAIN_REC_STATE_INVALID  0x00

#define REMAIN_SEC_INVALID_INDEX      0xffff
#define REMAIN_REC_INVALID_INDEX      0xffff
#define REMAIN_STATE_OFFSET               4
#define REMAIN_ALIGN_SIZE                    32



// Remain flash information structure.
typedef  struct _remain_flash_info
{
    UINT32  bHasInit;                    // the flag of has initialized.
    UINT32 iFlashStartAddr;         // flash start address.
    UINT32 iFlashSectorSize;       // sector size.
    UINT32 iNrSector;                   // sector number.
}
REMAIN_FLASH_INFO;

typedef struct _user_remain_header
{
    UINT32 magic;
    UINT16 state;
    UINT16 erase_counter;
    UINT32 version;
    UINT16 reserved;
    UINT32 checksum;
} REMAIN_HEADER;

typedef struct _remain_sectors
{
    UINT32 addr;
    UINT16 state;
    UINT16 erase_counter;
    UINT16 timestamp;
    UINT16 valid_index;
    UINT16 cur_index;
    UINT16 reserved;
} REMAIN_SECTORS;

typedef struct _remain_rec_info
{
    UINT16 size;
    UINT32 version;
} REMAIN_REC_INFO;

typedef enum
{
    REMAIN_GC_IDLE,
    REMAIN_GC_INIT,
    REMAIN_GC_ERASE_BEGIN,
    REMAIN_GC_ERASE_CONTINUE,
    REMAIN_GC_ERASE_END,
    REMAIN_GC_FINISH,
} REMAIN_GC_STATE;

typedef struct _remain_gc
{
    UINT8 is_active;
    REMAIN_GC_STATE state;
    REMAIN_SECTORS *psect;
} REMAIN_GC;

typedef struct _remain_info
{
    REMAIN_SECTORS remain_sectors[REMAIN_SECTORS_COUNT_MAX];
    UINT16 sec_count;
    UINT16 valid_sec_index;
    UINT16 cur_sec_index;
    UINT16 rec_desc_off;
    UINT16 rec_off;
    UINT16 rec_desc_size;
    UINT16 rec_size;
    UINT16 real_size;
    UINT16 rec_count;
    REMAIN_GC gc;
} REMAIN_INFO;

typedef struct _remain_rec_desc
{
    UINT8 state;
    UINT8 reserved;
    UINT16 timestamp;
    UINT32 checksum;
} REMAIN_REC_DESC;

extern UINT32 vds_CRC32( CONST VOID *pvData, INT32 iLen );
extern PUBLIC UINT32 tgt_get_factory_setting_size(VOID);
#define remain_crc32 vds_CRC32
#define REMAIN_OFFSETOF(TYPE, MEMBER) ((UINT32) &((TYPE *)0)->MEMBER)
#define REMAIN_ALIGN(size, align) ((((size) +(align) -1)/(align))*(align))
//#define REMAIN_TRACE CSW_TRACE
#define REMAIN_TS_ID 13
#define REMAIN_TRACE(id,pFormat, ...)  // TS_OutputText(id, (PCSTR)pFormat, ##__VA_ARGS__)
#define REMAIN_SendEvent(x)  // hal_HstSendEvent(x)
#endif
