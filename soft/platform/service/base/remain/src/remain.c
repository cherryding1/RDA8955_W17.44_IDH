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

#if !defined(_T_UPGRADE_PROGRAMMER) && defined(REDUNDANT_DATA_REGION)

#include "dsm_cf.h"
#include "memd_m.h"
#include "vds_cache.h"
#include "remain.h"
#include "remain_local.h"
#ifdef _FS_SIMULATOR_
#else
#include <base_prv.h>
#include <drv_flash.h>
#include <memd_m.h>
#include "ts.h"
#endif

extern UINT32 _calib_start;
extern UINT32 _remain_start;
extern UINT32 _remain_end;
REMAIN_FLASH_INFO g_remain_flash_info = {0, 0, 0, 0};
REMAIN_INFO g_remain_info;
REMAIN_REC_INFO g_rec_info;
BOOL g_is_init = 0;

static UINT32 remain_header_crc32(UINT8 *data,
                                  UINT32 size
                                 )
{
    UINT32 begin_off = 0;
    UINT32 end_off = 0;
    begin_off = REMAIN_OFFSETOF(REMAIN_HEADER, erase_counter);
    end_off = REMAIN_OFFSETOF(REMAIN_HEADER, reserved);
    return remain_crc32(data + begin_off, (end_off - begin_off));
}

static VOID remain_init_flash_info(VOID)
{
    UINT32 userFieldBase = 0;
    UINT32 userFieldSize = 0;
    UINT32 sectorSize = 0;
    UINT32 blockSize = 0;
    UINT32 calib_base_addr;
    UINT32 remain_base;
    UINT32 remain_end;
    UINT32 remain_count;

    memd_FlashGetUserFieldInfo(&userFieldBase, &userFieldSize, &sectorSize, &blockSize);
    calib_base_addr = (UINT32)&_calib_start;
    calib_base_addr &=  0xffffff;

    remain_base = (UINT32)&_remain_start;// userFieldBase + userFieldSize;
    remain_base &= 0xffffff;

    remain_end = (UINT32)&_remain_end;
    remain_end &= 0xffffff;
    if(remain_base < calib_base_addr &&
            calib_base_addr - remain_base >= sectorSize)
    {
        remain_count = (remain_end - remain_base) / sectorSize;
        if(remain_count > 0)
        {
            g_remain_flash_info.iFlashStartAddr = remain_base;
            g_remain_flash_info.iFlashSectorSize = sectorSize;
            g_remain_flash_info.iNrSector = remain_count;
            g_remain_flash_info.bHasInit = 1;
            // hal_HstSendEvent(0x12345678);
            // hal_HstSendEvent(remain_base);
            // hal_HstSendEvent(remain_count);
        }
        else
        {
            g_remain_flash_info.iFlashStartAddr = 0;
            g_remain_flash_info.iFlashSectorSize = 0;
            g_remain_flash_info.iNrSector = 0;
            g_remain_flash_info.bHasInit = 0;
            hal_HstSendEvent(0x12345679);
            hal_HstSendEvent(remain_base);
            hal_HstSendEvent(remain_count);
        }
    }
    else
    {
        g_remain_flash_info.iFlashStartAddr = 0;
        g_remain_flash_info.iFlashSectorSize = 0;
        g_remain_flash_info.iNrSector = 0;
        g_remain_flash_info.bHasInit = 0;
        hal_HstSendEvent(0x1234567a);
        hal_HstSendEvent(remain_base);
        hal_HstSendEvent(remain_count);
    }
}

static INT32 remain_WriteFlash(UINT32 addr,
                               CONST BYTE *buff,
                               UINT32 to_write,
                               UINT32 *pwritten
                              )
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 cri_status;

    if(g_remain_flash_info.bHasInit == 0)
    {

        REMAIN_TRACE(REMAIN_TS_ID, "remain_WriteFlash: remain flash not init.");
        return ERR_REMAIN_NOT_INIT;
    }
    if(addr  < g_remain_flash_info.iFlashStartAddr ||
            (addr + to_write) > \
            (g_remain_flash_info.iFlashStartAddr + \
             g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector) ||
            addr < g_remain_flash_info.iFlashStartAddr )
    {
        DSM_Assert(0, "remain_WriteFlash: Check addr error.addr = 0x%x.", addr);
        return ERR_REMAIN_SECTOR_CHECK_ERROR;
    }
    cri_status = hal_SysEnterCriticalSection();
    result = memd_FlashWrite((UINT8 *)addr, to_write, pwritten, (UINT8 *)buff);
    hal_SysExitCriticalSection(cri_status);
    if(0 != result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_WriteFlash: memd_FlashWrite fail!result = %d,addr = 0x%x.", result, addr);
        ret = result;
    }
    return ret;
}


static INT32 remain_ReadFlash(UINT32 addr,
                              CONST BYTE *buff,
                              UINT32 to_read,
                              UINT32 *pread
                             )
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 cri_status;

    if(g_remain_flash_info.bHasInit == 0)
    {

        REMAIN_TRACE(REMAIN_TS_ID, "remain_ReadFlash: remain flash not init.");
        return ERR_REMAIN_NOT_INIT;
    }
    if(addr  < g_remain_flash_info.iFlashStartAddr ||
            (addr + to_read) > \
            (g_remain_flash_info.iFlashStartAddr + \
             g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector) ||
            addr < g_remain_flash_info.iFlashStartAddr )
    {
        DSM_Assert(0, "remain_ReadFlash: Check addr error. 0x%x,0x%x,0x%x,0x%x,0x%x.",
                   addr,
                   to_read,
                   g_remain_flash_info.iFlashSectorSize,
                   g_remain_flash_info.iNrSector,
                   g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector
                  );
        return ERR_REMAIN_SECTOR_CHECK_ERROR;
    }
    cri_status = hal_SysEnterCriticalSection();
    result = memd_FlashRead((UINT8 *)addr, to_read, pread, (UINT8 *)buff);
    hal_SysExitCriticalSection(cri_status);
    if(0 != result)
    {
        DSM_Assert(0, "remain_ReadFlash: memd_FlashRead failed.result = %d,addr = 0x%x.", result, addr);
        ret = result;
    }
    return ret;
}


static INT32 remain_EraseFlash(UINT32 addr)
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 cri_status;

    if(g_remain_flash_info.bHasInit == 0)
    {

        REMAIN_TRACE(REMAIN_TS_ID, "remain_EraseFlash: remain flash not init.");
        return ERR_REMAIN_MALLOC_FAILED;
    }
    if(addr % g_remain_flash_info.iFlashSectorSize != 0 ||
            addr + g_remain_flash_info.iFlashSectorSize > \
            (g_remain_flash_info.iFlashStartAddr + \
             g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector))
    {
        DSM_Assert(0, "remain_EraseFlash: Check addr error. 0x%x,0x%x,0x%x,0x%x,0x%x.",
                   addr, g_remain_flash_info.iFlashSectorSize, g_remain_flash_info.iNrSector,
                   addr + g_remain_flash_info.iFlashSectorSize,
                   g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector
                  );
        return ERR_REMAIN_SECTOR_CHECK_ERROR;
    }
    cri_status = hal_SysEnterCriticalSection();
    result = memd_FlashErase((UINT8 *)addr, (UINT8 *)(addr + g_remain_flash_info.iFlashSectorSize));
    hal_SysExitCriticalSection(cri_status);
    if(0 != result)
    {
        DSM_Assert(0, "remain_EraseFlash:Erase sector fail!result = %d,addr = 0x%x.", result, addr);
        ret = result;
    }
    return ret;
}


static INT32 remain_EreaseBegin(UINT32 addr,
                                UINT8 is_suspend
                               )
{
    INT32 result;
    INT32 ret = 0;
    UINT32 cri_status = 0;

    if(g_remain_flash_info.bHasInit == 0)
    {

        REMAIN_TRACE(REMAIN_TS_ID, "remain_EreaseBegin: remain flash not init.");
        return ERR_REMAIN_MALLOC_FAILED;
    }
    if(addr % g_remain_flash_info.iFlashSectorSize != 0 ||
            addr + g_remain_flash_info.iFlashSectorSize > \
            (g_remain_flash_info.iFlashStartAddr + \
             g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector))
    {
        DSM_Assert(0, "remain_EreaseBegin: Check addr error. 0x%x,0x%x,0x%x,0x%x,0x%x.",
                   addr, g_remain_flash_info.iFlashSectorSize, g_remain_flash_info.iNrSector,
                   addr + g_remain_flash_info.iFlashSectorSize,
                   g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector
                  );
        return ERR_REMAIN_SECTOR_CHECK_ERROR;
    }

    cri_status = hal_SysEnterCriticalSection();
    if(is_suspend == 1)
    {
        result = memd_FlashErase_First((UINT8 *)addr, (UINT8 *)(addr + g_remain_flash_info.iFlashSectorSize),
                                       memd_FlashErase2SuspendTime());
    }
    else
    {
        result = memd_FlashErase((UINT8 *)addr, (UINT8 *)(addr + g_remain_flash_info.iFlashSectorSize));
    }
    hal_SysExitCriticalSection(cri_status);
    if(MEMD_ERR_SUSPEND != result &&
            MEMD_ERR_BUSY != result &&
            MEMD_ERR_NO != result)
    {
        DSM_Assert(0, "remain_EreaseBegin:erase fail!addr = 0x%x.", addr);
    }
    ret = result;
    return ret;
}


static INT32 remain_EreaseContinue(UINT32 addr)
{
    INT32 result;
    INT32 ret = 0;
    UINT32 cri_status = 0;

    if(g_remain_flash_info.bHasInit == 0)
    {

        REMAIN_TRACE(REMAIN_TS_ID, "tgt_EreaseContinue: remain flash not init.");
        return ERR_REMAIN_MALLOC_FAILED;
    }
    if(addr % g_remain_flash_info.iFlashSectorSize != 0 ||
            (addr + g_remain_flash_info.iFlashSectorSize) >
            (g_remain_flash_info.iFlashStartAddr +
             g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector))
    {
        DSM_Assert(0, "remain_EreaseContinue: Check addr error. 0x%x,0x%x,0x%x,0x%x,0x%x.",
                   addr, g_remain_flash_info.iFlashSectorSize, g_remain_flash_info.iNrSector,
                   addr + g_remain_flash_info.iFlashSectorSize,
                   g_remain_flash_info.iFlashSectorSize * g_remain_flash_info.iNrSector
                  );
        return ERR_REMAIN_SECTOR_CHECK_ERROR;
    }
    cri_status = hal_SysEnterCriticalSection();
    result = memd_FlashErase_Continue((UINT8 *)addr,
                                      (UINT8 *)(addr + g_remain_flash_info.iFlashSectorSize),
                                      memd_FlashErase2SuspendTime());
    hal_SysExitCriticalSection(cri_status);
    if(MEMD_ERR_SUSPEND != result &&
            MEMD_ERR_BUSY != result &&
            MEMD_ERR_NO != result)
    {
        DSM_Assert(0, "remain_EreaseContinue:erase fail!addr = 0x%x.", addr);
    }
    ret = result;
    return ret;
}


static REMAIN_SECTORS *remain_FindGcSector(REMAIN_INFO *premain_info)
{
    REMAIN_SECTORS *psect = NULL;
    REMAIN_SECTORS *pret = NULL;
    UINT16 i;
    UINT16 NUM = 0;

    for(i = 0; i < premain_info->sec_count; i++)
    {
        psect = &premain_info->remain_sectors[i];
        if(psect->state ==  REMAIN_STATE_DIRTY)
        {

            if(!pret)
            {
                pret = psect;
                NUM = i;
            }
            else
            {
                pret = pret->erase_counter < psect->erase_counter ? psect : pret;
                NUM = i;
            }
        }
    }
    return pret;
}


static BOOL remain_CheckSector(UINT32 addr,
                               UINT32 offset
                              )
{
    INT32 result;
    BOOL ret = TRUE;
    UINT8 *sec_buff = NULL;
    UINT32 size;
    UINT32 has_size = 0;
    UINT8 *p;

    size = g_remain_flash_info.iFlashSectorSize;
    sec_buff = DSM_MAlloc(size);
    if(!sec_buff)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_CheckSector:malloc(0x%x) failed.", size);
        DSM_Assert(0, "remain_CheckSector:malloc(0x%x) failed.", size);
        ret = FALSE;
        goto _func_end;
    }
    result = remain_ReadFlash(addr, sec_buff, size, &has_size);
    if(0 != result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_CheckSector:ret = %d,addr = 0x%x,offs = 0x%x.", ret, addr, has_size);
        DSM_Assert(0, "remain_CheckSector:ret = %d,addr = 0x%x,offs = 0x%x.", ret, addr, has_size);
        ret = FALSE;
        goto _func_end;
    }
    p = sec_buff + offset;
    while((UINT32)(p - sec_buff) < size)
    {
        if(*p != 0xff)
        {
            ret = FALSE;
            break;
        }
        p++;
    }
_func_end:
    if(sec_buff)
    {
        DSM_Free(sec_buff);
    }
    return ret;
}


static BOOL remain_CheckRec(UINT32 rec_addr,
                            UINT32 size,
                            UINT32 checksum
                           )
{
    UINT8 *buff = NULL;
    BOOL bret = FALSE;
    INT32 result;
    UINT32 has_size = 0;
    UINT32 local_checksum;

    buff = DSM_MAlloc(size);
    if(!buff)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_CheckRec:malloc(0x%x) failed.", size);
        DSM_Assert(0, "remain_CheckRec:malloc(0x%x) failed.", size);
        bret = FALSE;
        goto _func_end;
    }

    result = remain_ReadFlash(rec_addr, buff, size, &has_size);
    if(0 != result)
    {
        REMAIN_TRACE(REMAIN_TS_ID,
                     "remain_CheckRec:ret = %d,rec_addr = 0x%x,offs = 0x%x.",
                     result, rec_addr, has_size);
        DSM_Assert(0,
                   "remain_CheckRec:ret = %d,rec_addr = 0x%x,offs = 0x%x.",
                   result, rec_addr, has_size);
        bret = FALSE;
        goto _func_end;
    }
    local_checksum = remain_crc32(buff, size);
    if(checksum == local_checksum)
    {
        bret = TRUE;
    }
    else
    {
        bret = FALSE;
    }
_func_end:
    if(buff)
    {
        DSM_Free(buff);
    }
    return bret;
}


static INT32 remain_GetIndex(UINT32 rec_desc_addr,
                             UINT32 rec_addr,
                             UINT32 rec_desc_size,
                             UINT32 rec_size,
                             UINT32 rec_real_size,
                             UINT32 rec_count,
                             UINT16 *pvalid_index,
                             UINT16 *pcur_index
                            )
{
    INT32 result;
    INT32 ret = 0;
    UINT8 *buff = NULL;
    UINT32 size;
    UINT32 has_size = 0;
    UINT8 *p;
    REMAIN_REC_DESC *prec_desc;
    UINT16 i;
    UINT32 cur_addr;

    //*pvalid_index = REMAIN_REC_INVALID_INDEX;
    //*pcur_index = REMAIN_REC_INVALID_INDEX;
    size = rec_desc_size * rec_count;
    buff = DSM_MAlloc(size);
    if(!buff)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_GetIndex:malloc(0x%x) failed.", size);
        DSM_Assert(0, "remain_GetIndex:malloc(0x%x) failed.", size);
        ret = ERR_REMAIN_READ_FLASH_FAILED;
        goto _func_end;
    }
    result = remain_ReadFlash(rec_desc_addr, buff, size, &has_size);
    if(0 != result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_GetIndex:ret = %d,rec_desc_addr = 0x%x,offs = 0x%x.", result, rec_desc_addr, has_size);
        DSM_Assert(0, "remain_GetIndex:ret = %d,rec_desc_addr = 0x%x,offs = 0x%x.", result, rec_desc_addr, has_size);
        ret = ERR_REMAIN_READ_FLASH_FAILED;
        goto _func_end;
    }
    p = buff;
    *pvalid_index = REMAIN_REC_INVALID_INDEX;
    *pcur_index = REMAIN_REC_INVALID_INDEX;
    for(i = 0; i < rec_count; i++)
    {
        prec_desc = (REMAIN_REC_DESC *)(p + i * rec_desc_size);
        if(prec_desc->state == REMAIN_REC_STATE_VALID)
        {
            cur_addr = rec_addr + i * rec_size;
            if(remain_CheckRec(cur_addr, rec_real_size, prec_desc->checksum))
            {
                *pvalid_index = i;
            }
        }
        if(prec_desc->state == REMAIN_REC_STATE_FREE)
        {
            *pcur_index = i;
            break;
        }
    }
_func_end:
    if(buff)
    {
        DSM_Free(buff);
    }
    return ret;
}


static UINT16 remain_SectorIndex(REMAIN_INFO *premain_info,
                                 REMAIN_SECTORS *premain_sector
                                )
{
    UINT16 i;
    UINT16 index = REMAIN_SEC_INVALID_INDEX;

    for(i = 0; i < premain_info->sec_count; i++)
    {
        if(&premain_info->remain_sectors[i] == premain_sector)
        {
            index = i;
            break;
        }
    }
    return index;
}

static INT32 remain_Format(REMAIN_SECTORS *premain_sector)
{
    INT32 ret = 0;
    UINT32 addr;
    REMAIN_HEADER remain_header;
    UINT16 erase_counter = 0;
    UINT32 has_rw = 0;
    UINT32 offset;
    BOOL bresult;
    INT32 result;
    UINT8 is_dirty = 0;

    addr = premain_sector->addr;
    erase_counter = premain_sector->erase_counter;
    offset = REMAIN_OFFSETOF(REMAIN_HEADER, erase_counter);
    bresult = remain_CheckSector(addr, offset);
    is_dirty = bresult == TRUE ? 0 : 1;
    if(is_dirty)
    {
        result = remain_EraseFlash(addr);
        if(result)
        {
            REMAIN_TRACE(REMAIN_TS_ID, "remain_Format::erase sector fail. err= %d.", result);
            ret = ERR_REMAIN_ERASE_FLASH_FAILED;
            goto _func_end;
        }
    }
    DSM_MemSet(&remain_header, 0xff, sizeof(REMAIN_HEADER));
    remain_header.magic = REMAIN_MAIGIC;
    remain_header.state = REMAIN_STATE_FREE;
    remain_header.erase_counter = erase_counter == 0xffff ? 0 :  erase_counter + 1;
    remain_header.version = g_rec_info.version;
    remain_header.checksum = remain_header_crc32((UINT8 *)&remain_header, sizeof(REMAIN_HEADER));
    result = remain_WriteFlash(addr, (UINT8 *)&remain_header, sizeof(REMAIN_HEADER), &has_rw);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain format write magic fail. err= %d.", result);
        ret = ERR_REMAIN_WRITE_FLASH_FAILED;
        goto _func_end;
    }

_func_end:
    return ret;
}


static INT32 remain_GetSectorsInfo(REMAIN_INFO *premain_info)
{
    INT32 ret = ERR_SUCCESS;
    INT32 result;
    UINT32 addr;
    UINT16 sec_count = 0;
    UINT16 temp_size = 0;
    UINT32 has_size = 0;
    REMAIN_HEADER remain_header;
    REMAIN_INFO remain_info;
    REMAIN_SECTORS *premain_sector = NULL;
    UINT16 sec_state;
    UINT16 cur_index;
    UINT16 valid_index;
    UINT16 cur_sec_index;
    UINT16 valid_sec_index;
    UINT32 checksum = 0;
    UINT16 i;
    UINT32 rec_desc_addr;
    UINT32 rec_addr;

    DSM_MemSet(&remain_info, 0, sizeof(REMAIN_INFO));
    remain_info.rec_desc_off = REMAIN_ALIGN(sizeof(REMAIN_HEADER), REMAIN_ALIGN_SIZE);
    remain_info.rec_desc_size = sizeof(REMAIN_REC_DESC);
    remain_info.real_size = g_rec_info.size;
    remain_info.rec_size = REMAIN_ALIGN(g_rec_info.size, REMAIN_ALIGN_SIZE);
    temp_size = (remain_info.rec_desc_size + remain_info.rec_size);
    remain_info.rec_count = ((UINT16)g_remain_flash_info.iFlashSectorSize - remain_info.rec_desc_off) /
                            REMAIN_ALIGN(temp_size, (2 * REMAIN_ALIGN_SIZE));
    temp_size = remain_info.rec_desc_off + remain_info.rec_desc_size * remain_info.rec_count;
    remain_info.rec_off = REMAIN_ALIGN(temp_size, REMAIN_ALIGN_SIZE);
    remain_info.gc.is_active = 0;
    remain_info.gc.state = REMAIN_GC_IDLE;
    remain_info.gc.psect = NULL;
    /*
    REMAIN_TRACE(REMAIN_TS_ID,"remain_GetSectorsInfo:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x.",
        remain_info.sec_count,
        remain_info.valid_sec_index,
        remain_info.cur_sec_index,
        remain_info.rec_desc_off,
        remain_info.rec_off,
        remain_info.rec_desc_size,
        remain_info.rec_size,
        remain_info.real_size,
        remain_info.rec_count,
        g_remain_flash_info.iFlashSectorSize,
        REMAIN_ALIGN(temp_size,(2*REMAIN_ALIGN_SIZE))
        );
    REMAIN_SendEvent(0xbbccdd01);
    REMAIN_SendEvent(remain_info.sec_count);
    REMAIN_SendEvent(remain_info.valid_sec_index);
    REMAIN_SendEvent(remain_info.cur_sec_index);
    REMAIN_SendEvent(remain_info.rec_desc_off);
    REMAIN_SendEvent(remain_info.rec_off);
    REMAIN_SendEvent(remain_info.rec_desc_size);
    REMAIN_SendEvent(remain_info.rec_size);
    REMAIN_SendEvent(remain_info.real_size);
    REMAIN_SendEvent(remain_info.rec_count);
    */
    do
    {
        REMAIN_TRACE(REMAIN_TS_ID, "sec_count = %d", sec_count);
        addr = g_remain_flash_info.iFlashStartAddr + sec_count * g_remain_flash_info.iFlashSectorSize;
        if(sec_count == REMAIN_SECTORS_COUNT_MAX ||
                sec_count == g_remain_flash_info.iNrSector)
        {
            break;
        }
        result = remain_ReadFlash(addr, (UINT8 *)&remain_header, sizeof(REMAIN_HEADER), &has_size);
        if(0 != result)
        {
            DSM_Assert(0, "read remain fail!ret = %d,addr = 0x%x,offs = 0x%x.", ret, addr, has_size);
            ret = ERR_REMAIN_READ_FLASH_FAILED;
            goto _func_end;
        }

        /*
        REMAIN_TRACE(REMAIN_TS_ID,"magic = 0x%x,state = 0x%x,erase_counter = 0x%x,timestamp = 0x%x,reserved =0x%x,version = 0x%x,checksum = 0x%x.",
         remain_header.magic,
         remain_header.state,
         remain_header.erase_counter,
         remain_header.timestamp,
         remain_header.reserved,
         remain_header.version,
         remain_header.checksum);

        REMAIN_SendEvent(0xbbccdd02);
        REMAIN_SendEvent(sec_count);
        REMAIN_SendEvent(remain_header.magic);
        REMAIN_SendEvent(remain_header.state);
        REMAIN_SendEvent(remain_header.erase_counter);
        REMAIN_SendEvent(remain_header.timestamp);
        REMAIN_SendEvent(remain_header.reserved);
        REMAIN_SendEvent(remain_header.version);
        REMAIN_SendEvent(remain_header.checksum);
        */
        valid_index = REMAIN_REC_INVALID_INDEX;
        cur_index = REMAIN_REC_INVALID_INDEX;
        sec_state = remain_header.state;
        switch(sec_state)
        {
            case REMAIN_STATE_FREE:
                // REMAIN_SendEvent(0xbbccdd03);
                // REMAIN_SendEvent(checksum);
                if(remain_header.magic != REMAIN_UNFORMAT_MAIGIC)
                {
                    sec_state = REMAIN_STATE_ERASE;
                    break;
                }
                /*
                offset = REMAIN_OFFSETOF(REMAIN_HEADER, erase_counter);
                if(!remain_CheckSector(addr,offset))
                {
                   sec_state = REMAIN_STATE_ERASE;
                }
                */
                break;
            case REMAIN_STATE_CURRENT:
                REMAIN_TRACE(REMAIN_TS_ID, "current:sec_count = %d,valid_index = %d,cur_index = %d.",
                             sec_count,
                             valid_index,
                             cur_index);
                checksum = remain_header_crc32((UINT8 *)&remain_header, sizeof(REMAIN_HEADER));

                REMAIN_SendEvent(0xbbccdd04);
                //REMAIN_SendEvent(checksum);

                if(!(remain_header.magic == REMAIN_MAIGIC &&
                        remain_header.version    == g_rec_info.version &&
                        remain_header.checksum == checksum)
                  )
                {
                    REMAIN_SendEvent(0xbbccdd14);
                    REMAIN_SendEvent(remain_header.magic);
                    REMAIN_SendEvent(remain_header.version);
                    REMAIN_SendEvent(g_rec_info.version);
                    REMAIN_SendEvent(remain_header.checksum);
                    REMAIN_SendEvent(checksum);
                    sec_state = REMAIN_STATE_ERASE;
                    break;
                }
                rec_desc_addr = addr + remain_info.rec_desc_off;
                rec_addr = addr + remain_info.rec_off;
                result = remain_GetIndex(
                             rec_desc_addr,
                             rec_addr,
                             remain_info.rec_desc_size,
                             remain_info.rec_size,
                             remain_info.real_size,
                             remain_info.rec_count,
                             &valid_index,
                             &cur_index
                         );
                if(valid_index >= remain_info.rec_count)
                {
                    sec_state = REMAIN_STATE_ERASE;
                }
                REMAIN_SendEvent(0xbbccdd24);
                REMAIN_SendEvent(valid_index);
                REMAIN_SendEvent(cur_index);
                break;
            case REMAIN_STATE_DIRTY:
                REMAIN_SendEvent(0xbbccdd05);
                sec_state = REMAIN_STATE_ERASE;
                break;
            case REMAIN_STATE_ERASE:
            case REMAIN_STATE_UNFORMAT:
                REMAIN_SendEvent(0xbbccdd06);
                sec_state = REMAIN_STATE_ERASE;
                break;
            default:
                sec_state = REMAIN_STATE_ERASE;
                REMAIN_SendEvent(0xbbccdd07);
                break;
        }
        premain_sector = &remain_info.remain_sectors[sec_count];
        premain_sector->addr = addr;
        premain_sector->state = sec_state;
        if(remain_header.version == g_rec_info.version)
        {
            REMAIN_SendEvent(0xbbccdd08);
            premain_sector->erase_counter = remain_header.erase_counter == 0xffff ? 0 : remain_header.erase_counter;
        }
        else
        {
            REMAIN_SendEvent(0xbbccdd09);
            premain_sector->erase_counter = 0;
        }
        premain_sector->valid_index = valid_index;
        premain_sector->cur_index = cur_index;
        sec_count ++;
    }
    while(1);
    remain_info.sec_count = sec_count;
    REMAIN_SendEvent(0xbbccdd0a);
    REMAIN_SendEvent(sec_count);
    // find valid sector and cur sector.
    valid_sec_index = REMAIN_SEC_INVALID_INDEX;
    cur_sec_index = REMAIN_SEC_INVALID_INDEX;

    premain_sector = NULL;
    for(i = 0; i < sec_count; i++)
    {
        if(remain_info.remain_sectors[i].state == REMAIN_STATE_CURRENT &&
                remain_info.remain_sectors[i].valid_index < REMAIN_REC_INVALID_INDEX)
        {
            REMAIN_SendEvent(0xbbccdd0b);
            REMAIN_SendEvent(i);
            if(!premain_sector)
            {
                premain_sector = &remain_info.remain_sectors[i];
                if(premain_sector->cur_index < remain_info.rec_count)
                {
                    cur_sec_index = i;
                }
                if(premain_sector->valid_index < remain_info.rec_count)
                {
                    valid_sec_index = i;
                }
            }
            else
            {
                REMAIN_SendEvent(0xbbccdd0c);
                REMAIN_SendEvent(i);
                if(premain_sector->timestamp < remain_info.remain_sectors[i].timestamp ||
                        (premain_sector->timestamp == 0xffff &&
                         remain_info.remain_sectors[i].timestamp < 0x00ff)
                  )
                {
                    if(remain_info.remain_sectors[i].valid_index <= remain_info.rec_count)
                    {
                        premain_sector->state = REMAIN_STATE_DIRTY;
                        premain_sector = &remain_info.remain_sectors[i];
                        valid_sec_index = i;
                    }
                    if(remain_info.remain_sectors[i].cur_index < remain_info.rec_count)
                    {
                        cur_sec_index = i;
                    }
                }
            }
        }
    }
    remain_info.valid_sec_index = valid_sec_index;
    remain_info.cur_sec_index = cur_sec_index;
    REMAIN_SendEvent(0xbbccdd0f);
    REMAIN_SendEvent(valid_sec_index);
    REMAIN_SendEvent(cur_sec_index);
    DSM_MemCpy(premain_info, &remain_info, sizeof(REMAIN_INFO));
_func_end:
    return ret;
}

static INT32 remain_SetSecHeader(REMAIN_INFO *premain_info,
                                 REMAIN_SECTORS *remain_sector
                                )
{
    INT32 result;
    REMAIN_HEADER remain_header;
    INT32 ret = ERR_SUCCESS;
    UINT32 addr;
    UINT32 writen = 0;

    addr = remain_sector->addr;
    remain_header.magic = REMAIN_MAIGIC;
    remain_header.state = remain_sector->state;
    remain_header.erase_counter = remain_sector->erase_counter;
    remain_header.version = g_rec_info.version;
    remain_header.reserved = 0xffff;
    remain_header.checksum = remain_header_crc32((UINT8 *)&remain_header, sizeof(REMAIN_HEADER));
    /*
    REMAIN_TRACE(REMAIN_TS_ID, "remain_SetSecHeader:remain_header:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x.",
            remain_header.magic,
            remain_header.state,
            remain_header.erase_counter,
            remain_header.timestamp,
            remain_header.reserved,
            remain_header.version,
            remain_header.checksum);
    REMAIN_TRACE(REMAIN_TS_ID, "addr = 0x%x",addr);
    */
    result = remain_WriteFlash(addr, (UINT8 *)&remain_header, sizeof(REMAIN_HEADER), &writen);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_SetSecHeader. write state failed,err= %d,addr = 0x%x.",
                     result, addr);
        ret =  ERR_REMAIN_WRITE_FLASH_FAILED;
    }
    return ret;
}


static INT32 remain_ChangeSecState(REMAIN_INFO *premain_info,
                                   REMAIN_SECTORS *remain_sector,
                                   UINT16 state
                                  )
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 addr;
    UINT32 writen = 0;

    addr = remain_sector->addr + REMAIN_OFFSETOF(REMAIN_HEADER, state);

    result = remain_WriteFlash(addr, (UINT8 *)&state, sizeof(state), &writen);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_ChangeSecState. write state failed,err= %d,addr = 0x%x.",
                     result, addr);
        ret =  ERR_REMAIN_WRITE_FLASH_FAILED;
    }
    return ret;
}

static REMAIN_SECTORS *remain_GetFreeSector(REMAIN_INFO *premain_info)
{
    UINT32 i;
    REMAIN_SECTORS *ptemp = NULL;

    for(i = 0; i < premain_info->sec_count; i++)
    {
        if(premain_info->remain_sectors[i].state == REMAIN_STATE_FREE)
        {
            if(!ptemp)
            {
                ptemp = &premain_info->remain_sectors[i];
            }
            else
            {
                if(ptemp->erase_counter > premain_info->remain_sectors[i].erase_counter)
                {
                    ptemp = &premain_info->remain_sectors[i];
                }
            }
        }
    }
    return ptemp;
}


static INT32 remain_WriteRec(REMAIN_INFO *premain_info,
                             REMAIN_SECTORS *old_rec_sector,
                             REMAIN_SECTORS *new_rec_sector,
                             UINT8 *buff, UINT16 timestamp
                            )
{
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 addr;
    UINT32 has = 0;
    UINT8  state;
    REMAIN_REC_DESC rec_new_desc;


    // Write new rec desc ----TEMP(0xF2)
    rec_new_desc.state =  REMAIN_REC_STATE_TEMP;
    rec_new_desc.reserved = 0;
    rec_new_desc.checksum = remain_crc32(buff, premain_info->real_size);
    rec_new_desc.timestamp = timestamp;
    addr = new_rec_sector->addr + premain_info->rec_desc_off + premain_info->rec_desc_size * new_rec_sector->cur_index;
    result = remain_WriteFlash(addr, (UINT8 *)&rec_new_desc, premain_info->rec_desc_size, &has);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "rremain_WriteRec. Write new desc(to temp) fail,err= %d,addr = 0x%x,size = 0x%x.",
                     result, addr, premain_info->rec_desc_size);
        return ERR_REMAIN_WRITE_FLASH_FAILED;
    }

    // Write new rec.
    addr = new_rec_sector->addr + premain_info->rec_off + premain_info->rec_size * new_rec_sector->cur_index;
    result = remain_WriteFlash(addr, buff, premain_info->real_size, &has);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "rremain_WriteRec. Write new rec fail,err= %d,addr = 0x%x,size = 0x%x.",
                     result, addr, premain_info->real_size);
        return ERR_REMAIN_WRITE_FLASH_FAILED;
    }

    // Write new rec desc ----VALID(0xF0)
    state =  REMAIN_REC_STATE_VALID;
    addr = new_rec_sector->addr + premain_info->rec_desc_off + premain_info->rec_desc_size * new_rec_sector->cur_index;
    result = remain_WriteFlash(addr, (UINT8 *)&state, sizeof(state), &has);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "rremain_WriteRec. Write new desc(to valid) fail,err= %d,addr = 0x%x,size = 0x%x.",
                     result, addr, sizeof(state));
        return ERR_REMAIN_WRITE_FLASH_FAILED;
    }

    // Write old rec desc ----INVALID(0x00)
    if(old_rec_sector)
    {

        state =  REMAIN_REC_STATE_INVALID;
        addr = old_rec_sector->addr + premain_info->rec_desc_off + premain_info->rec_desc_size * old_rec_sector->valid_index;
        result = remain_WriteFlash(addr, (UINT8 *)&state, sizeof(state), &has);
        if(result)
        {
            REMAIN_TRACE(REMAIN_TS_ID, "rremain_WriteRec. Write old desc(to invalid) fail,err= %d,addr = 0x%x,size = 0x%x.",
                         result, addr, sizeof(state));
            return ERR_REMAIN_WRITE_FLASH_FAILED;
        }
    }
    return ret;
}


static INT32 remain_ReadRec(REMAIN_INFO *premain_info,
                            REMAIN_SECTORS *rec_sector,
                            UINT8 *buff,
                            UINT16 *ptimestamp
                           )
{
    INT32 result;
    INT32 ret;
    UINT32 addr;
    UINT32 read = 0;
    REMAIN_REC_DESC rec_desc;
    UINT32 checksum;

    // read rec desc.
    REMAIN_SendEvent(0xbbccbbb1);
    REMAIN_SendEvent(rec_sector->valid_index);
    addr = rec_sector->addr + premain_info->rec_desc_off + premain_info->rec_desc_size * rec_sector->valid_index;
    result = remain_ReadFlash(addr, (UINT8 *)&rec_desc, premain_info->rec_desc_size, &read);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_ReadRec. read valid rec desc fail,result = %d,addr = 0x%x,size = 0x%x.",
                     result, addr, premain_info->rec_desc_size);
        return ERR_REMAIN_READ_FLASH_FAILED;
    }

    if(rec_desc.state !=  REMAIN_REC_STATE_VALID)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_ReadRec. read valid rec desc fail,result = %d,addr = 0x%x,state = 0x%x.",
                     result, addr, rec_desc.state);
        return ERR_REMAIN_REC_STATE_FAILED;
    }
    // read rec desc.
    addr = rec_sector->addr + premain_info->rec_off + premain_info->rec_size * rec_sector->valid_index;
    result = remain_ReadFlash(addr, buff, premain_info->real_size, &read);
    if(result)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain_ReadRec. read valid rec fail,err= %d,addr = 0x%x,size = 0x%x.",
                     result, addr, premain_info->real_size);
        return ERR_REMAIN_READ_FLASH_FAILED;
    }
    checksum = remain_crc32(buff, premain_info->real_size);
    REMAIN_SendEvent(checksum);
    if(checksum == rec_desc.checksum)
    {
        REMAIN_SendEvent(0xbbccbbb2);
        REMAIN_SendEvent(rec_desc.timestamp);
        *ptimestamp = rec_desc.timestamp;
        ret = ERR_SUCCESS;
    }
    else
    {
        REMAIN_SendEvent(0xbbccbbb3);
        REMAIN_TRACE(REMAIN_TS_ID, "remain_ReadRec. crc error,checksum = %0x%x,desc_checksum = 0x%x, addr = 0x%x,size = 0x%x.",
                     result, addr, checksum, rec_desc.checksum, premain_info->real_size);
        ret = ERR_REMAIN_REC_CHECKSUM_ERROR;
    }

    return ret;
}


VOID Remain_SetRecInfo(UINT16 size,
                       UINT32 version
                      )
{
    g_rec_info.size = size;
    g_rec_info.version = version;
}


INT32 Remain_Init(VOID)
{
    INT32 ret;
    UINT32 i;
    REMAIN_SECTORS *premain_sector;
    UINT16 size;
    UINT32 version;
    // UINT16 sec_timestamp = 0;

    if(g_is_init == 1)
    {
        return 0;
    }
    remain_init_flash_info();
    if(g_remain_flash_info.iNrSector == 0)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain sectors  is null.");
        return ERR_REMAIN_SECTORS_IS_NULL;
    }

    size = tgt_get_factory_setting_size();
    version = REMAIN_VERSION;
    Remain_SetRecInfo(size, version);
    DSM_MemSet(&g_remain_info, 0, sizeof(REMAIN_INFO));
    ret = remain_GetSectorsInfo(&g_remain_info);
    if(ret)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain init fail. err= %d.", ret);
        return ERR_REMAIN_GET_SECTOR_ERROR;
    }
    if(g_remain_info.sec_count == 0)
    {
        REMAIN_TRACE(REMAIN_TS_ID, "remain sec_count  = 0.");
        return ERR_REMAIN_SECTORS_IS_NULL;
    }
    REMAIN_SendEvent(0xbbccaa02);
    for(i = 0; i < g_remain_info.sec_count; i++)
    {
        premain_sector = &g_remain_info.remain_sectors[i];
        if(premain_sector->state != REMAIN_STATE_FREE &&
                premain_sector->state != REMAIN_STATE_CURRENT)
        {
            // REMAIN_SendEvent(0xbbccaa03);
            // REMAIN_SendEvent(i);
            ret = remain_Format(&g_remain_info.remain_sectors[i]);
            if(ret)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "remain sectors  is null.");
                return ERR_REMAIN_SECTORS_FORMAT_FAILED;
            }
            premain_sector->state = REMAIN_STATE_FREE;
        }
    }
    g_is_init = 1;
    return 0;
}

INT32 Remain_Write(UINT8 *buff,
                   UINT16 timestamp,
                   UINT32 size
                  )
{
    REMAIN_SECTORS *pold = NULL;
    REMAIN_SECTORS *pnew = NULL;
    UINT16 valid_sec_index = REMAIN_SEC_INVALID_INDEX;
    UINT16 cur_sec_index = REMAIN_SEC_INVALID_INDEX;
    // UINT32 rec_index;
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    UINT32 is_new_sector = 0;

    size = size;
    if(g_is_init == 0)
    {
        return ERR_REMAIN_NOT_INIT;
    }

    // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:begin,timestamp = 0x%x.",timestamp);
    if(g_remain_info.valid_sec_index < g_remain_info.sec_count)
    {
        valid_sec_index = g_remain_info.valid_sec_index;
        pold = &g_remain_info.remain_sectors[valid_sec_index];
    }
    else
    {
        valid_sec_index = REMAIN_SEC_INVALID_INDEX;
        pold = NULL;
    }

    if(g_remain_info.cur_sec_index < g_remain_info.sec_count)
    {
        cur_sec_index = g_remain_info.cur_sec_index;
        pnew = &g_remain_info.remain_sectors[cur_sec_index];
        if(pnew->cur_index >= g_remain_info.rec_count)
        {
            pnew = NULL;
        }
    }
    else
    {
        cur_sec_index = REMAIN_SEC_INVALID_INDEX;
        pnew = NULL;
    }

    if(!pnew)
    {
        // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:in !pnew.");
        pnew = remain_GetFreeSector(&g_remain_info);
        if(!pnew)
        {
            g_remain_info.gc.is_active = 1;
            ret = ERR_REMAIN_FREE_RECORD_IS_NULL;
            REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write: Free sectors is null.");
            goto _func_end;
        }

        pnew->state = REMAIN_STATE_CURRENT;
        pnew->cur_index = 0;
        result = remain_ChangeSecState(&g_remain_info, pnew, REMAIN_STATE_TEMP);
        if(result)
        {
            REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write: remain_ChangeSecState failed,result = %d.", result);
            ret = ERR_REMAIN_WRITE_FLASH_FAILED;
            goto _func_end;
        }
        is_new_sector = 1;
        g_remain_info.cur_sec_index = remain_SectorIndex(&g_remain_info, pnew);
        REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:new cur_sec_index = 0x%x.", g_remain_info.cur_sec_index );
    }

    // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:call remain_WriteRec.");
    result = remain_WriteRec(&g_remain_info, pold, pnew, buff, timestamp);
    if(result == ERR_SUCCESS)
    {
        // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:remain_WriteRec ok.");
        if(is_new_sector == 1)
        {
            result = remain_ChangeSecState(&g_remain_info, pnew, REMAIN_STATE_CURRENT);
            if(result)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write: remain_ChangeSecState failed,result = %d.", result);
                ret = ERR_REMAIN_WRITE_FLASH_FAILED;
                goto _func_end;
            }
        }

        if(pold != NULL && pold != pnew)
        {
            pold->state = REMAIN_STATE_DIRTY;
            // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:call remain_ChangeSecState.");
            result = remain_ChangeSecState(&g_remain_info, pold, REMAIN_STATE_DIRTY);
            if(result)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write: remain_ChangeSecState(to dirty) failed,result = %d.", result);
                ret = ERR_REMAIN_WRITE_FLASH_FAILED;
                goto _func_end;
            }
        }
        pnew->valid_index = pnew->cur_index;
        pnew->cur_index ++;
        g_remain_info.valid_sec_index = remain_SectorIndex(&g_remain_info, pnew);
        // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:g_remain_info.valid_sec_index = 0x%x.",g_remain_info.valid_sec_index);
    }
    else
    {
        REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:remain_WriteRec failed,result = %d.", result);
        ret = result;
    }

_func_end:
    // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Write:remain_WriteRec end,ret = %d.",ret);
    return ret;
}


INT32 Remain_Read(UINT8 *buff,
                  UINT16 *ptimestamp,
                  UINT32 size
                 )
{
    REMAIN_SECTORS *prec = NULL;
    UINT32 valid_sec_index = REMAIN_SEC_INVALID_INDEX;
    INT32 result;
    INT32 ret = ERR_SUCCESS;
    size = size;
    if(g_is_init == 0)
    {
        return ERR_REMAIN_NOT_INIT;
    }
    // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Read:begin.");
    if(g_remain_info.valid_sec_index < g_remain_info.sec_count)
    {
        valid_sec_index = g_remain_info.valid_sec_index;
        prec = &g_remain_info.remain_sectors[valid_sec_index];
    }
    else
    {
        REMAIN_TRACE(REMAIN_TS_ID, "Remain_Read: valid_sec_index error.valid_sec_index = %d,rec_count= %d.",
                     g_remain_info.valid_sec_index, g_remain_info.rec_count);
        ret = ERR_REMAIN_REC_NOT_FOUND;
        goto _func_end;
    }
    result = remain_ReadRec(&g_remain_info, prec, buff, ptimestamp);
    if(result != ERR_SUCCESS)
    {
        REMAIN_SendEvent(0xbbccab06);
        REMAIN_TRACE(REMAIN_TS_ID, "Remain_Read: remain_ReadRec failed,result = %d.", result);
        ret = result;
    }
_func_end:
    // REMAIN_TRACE(REMAIN_TS_ID, "Remain_Read:end. ret = %d.",ret);
    return ret;
}


INT32 Remain_Gc(BOOL suspend)
{
    INT32 result;
    INT32 ret = 0;
    UINT32 addr;
    // REMAIN_HEADER remain_header;
    // UINT32 has_rw = 0;

    if(g_is_init == 0 ||
            g_remain_info.gc.is_active == 0)
    {
        return 0;
    }

    switch(g_remain_info.gc.state)
    {
        case REMAIN_GC_INIT:
            g_remain_info.gc.psect = remain_FindGcSector(&g_remain_info);
            if(!g_remain_info.gc.psect)
            {
                // hal_HstSendEvent(0xabcd000f);
                g_remain_info.gc.state = REMAIN_GC_FINISH;
                break;
            }
            result = remain_ChangeSecState(&g_remain_info,
                                           g_remain_info.gc.psect,
                                           REMAIN_REC_STATE_INVALID);
            if(result)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "warning:Remain_Gc:remain_ChangeSecState failed. ret = %d.", result);
            }
            g_remain_info.gc.state = REMAIN_GC_ERASE_BEGIN;
            break;
        case REMAIN_GC_ERASE_BEGIN:
            addr = g_remain_info.gc.psect->addr;
            result = remain_EreaseBegin(addr, suspend);
            if(result == MEMD_ERR_BUSY)
            {
                ;
            }
            else if(result == MEMD_ERR_SUSPEND)
            {
                g_remain_info.gc.state = REMAIN_GC_ERASE_CONTINUE;
            }
            else if(result == MEMD_ERR_NO)
            {
                g_remain_info.gc.state = REMAIN_GC_ERASE_END;
            }
            else
            {
                REMAIN_TRACE(REMAIN_TS_ID, "Remain_Gc:remain_EreaseBegin failed. ret = %d.", result);
                g_remain_info.gc.state = REMAIN_GC_FINISH;
                ret = ERR_REMAIN_ERASE_FLASH_FAILED;
            }
            break;
        case REMAIN_GC_ERASE_CONTINUE:
            addr = g_remain_info.gc.psect->addr;
            result = remain_EreaseContinue(addr);
            if(result == MEMD_ERR_BUSY ||
                    result == MEMD_ERR_SUSPEND)
            {
                ;
            }
            else if(result == MEMD_ERR_NO)
            {
                g_remain_info.gc.state = REMAIN_GC_ERASE_END;
            }
            else
            {
                REMAIN_TRACE(REMAIN_TS_ID, "Remain_Gc:tgt_EreaseContinue failed. ret = %d.", result);
                g_remain_info.gc.state = REMAIN_GC_FINISH;
                ret = ERR_REMAIN_ERASE_FLASH_FAILED;
            }
            break;
        case REMAIN_GC_ERASE_END:
            /*
            addr = g_remain_info.gc.psect->addr;
            DSM_MemSet(&remain_header,0xff,sizeof(REMAIN_HEADER));
            remain_header.magic = REMAIN_MAIGIC;
            remain_header.state = REMAIN_STATE_FREE;
            remain_header.erase_counter = g_remain_info.gc.psect->erase_counter;
            result = remain_WriteFlash(addr,(UINT8*)&remain_header,sizeof(REMAIN_HEADER),&has_rw);
            if(result)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "remain format write magic fail. err = %d.",result);
                ret = ERR_REMAIN_WRITE_FLASH_FAILED;
                break;
            }
            */
            g_remain_info.gc.psect->erase_counter ++;
            g_remain_info.gc.psect->state = REMAIN_STATE_FREE;
            g_remain_info.gc.psect->cur_index = 0xffff;
            g_remain_info.gc.psect->valid_index = 0xffff;
            g_remain_info.gc.psect->reserved = 0xabcd;
            result = remain_SetSecHeader(&g_remain_info, g_remain_info.gc.psect);
            if(result)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "Remain_Gc: remain_SetSecHeader fail. result = %d,addr = 0x%x.",
                             result, g_remain_info.gc.psect->addr);
                ret = ERR_REMAIN_WRITE_FLASH_FAILED;
            }
            g_remain_info.gc.state = REMAIN_GC_INIT;
            break;
        case REMAIN_GC_FINISH:
        default:
            if(g_remain_info.gc.psect)
            {
                REMAIN_TRACE(REMAIN_TS_ID, "Remain_Gc: finish.sector_index = %d erase_count = %d.",
                             remain_SectorIndex(&g_remain_info, g_remain_info.gc.psect),
                             g_remain_info.gc.psect->erase_counter);
            }
            g_remain_info.gc.psect = NULL;
            g_remain_info.gc.state = REMAIN_GC_IDLE;
            g_remain_info.gc.is_active = 0;
            break;
    }
    return ret;
}


BOOL Remain_NeedGc(VOID)
{
    UINT16 i;
    UINT16 dirty_count = 0;

    if(g_is_init == 0)
    {
        return FALSE;
    }

    if(g_remain_info.gc.is_active == 1)
    {
        return TRUE;
    }

    for(i = 0; i < g_remain_info.sec_count; i++)
    {
        if(g_remain_info.remain_sectors[i].state == REMAIN_STATE_DIRTY)
        {
            dirty_count ++;
        }
    }

    if(dirty_count >= g_remain_info.sec_count)
    {
        g_remain_info.gc.is_active = 1;
        if(g_remain_info.gc.state  == REMAIN_GC_IDLE)
        {
            //hal_HstSendEvent(0xabcd0001);
            g_remain_info.gc.state = REMAIN_GC_INIT;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL Remain_GcIsActive(VOID)
{
    if(g_is_init == 0 ||
            g_remain_info.gc.is_active == 0)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL Remain_GcIsSuspend(VOID)
{
    if(g_is_init == 0 ||
            g_remain_info.gc.is_active == 0)
    {
        return FALSE;
    }
    else if(g_remain_info.gc.state == REMAIN_GC_ERASE_BEGIN ||
            g_remain_info.gc.state == REMAIN_GC_ERASE_CONTINUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#endif
