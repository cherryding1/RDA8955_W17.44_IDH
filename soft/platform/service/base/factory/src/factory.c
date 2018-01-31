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

#if !defined(_T_UPGRADE_PROGRAMMER) && !defined(CHIP_HAS_AP)
#include "dsm_cf.h"
#include "memd_m.h"
#include "factory.h"
#include "factory_local.h"
#ifdef _FS_SIMULATOR_

#else
#include <base_prv.h>
#include <drv_flash.h>
#include "ts.h"
#include "cos.h"
#include "fs/sf_api.h"
#include "hal_spi_flash.h"
#endif

extern UINT32 _factory_start;
static FACTORY_INFO g_factory_info;

static VOID factory_SemInit(VOID)
{
    COS_SemaInit(&g_factory_info.sem, 1);
}

static VOID factory_SemWaitFor(VOID)
{
    COS_SemaTake(&g_factory_info.sem);
}

static VOID factory_SemRelease(VOID)
{
    COS_SemaRelease(&g_factory_info.sem);
}

static BOOL CheckFree(UINT8 *buff, UINT32 size)
{
    UINT32 i;
    for (i = 0; i < size; i++)
    {
        if (*(buff + i) != 0xff)
            return FALSE;
    }
    return TRUE;
}

static bool factory_WriteFlash(UINT32 addr, UINT8 *buff, UINT32 to_write)
{
    if (addr < g_factory_info.sec_info.start_addr ||
        (addr + to_write) >
            (g_factory_info.sec_info.start_addr +
             g_factory_info.sec_info.sec_size * g_factory_info.sec_info.sec_count) ||
        addr < g_factory_info.sec_info.start_addr)
    {
        DSM_Assert(0, "factory_WriteFlash: Check addr error.addr = 0x%x.", addr);
        return false;
    }

    if (!hal_SpiFlashWrite(addr, buff, to_write))
    {
        FACTORY_TRACE(REMAIN_TS_ID, "factory_WriteFlash: memd_FlashWrite fail! addr = 0x%x.", addr);
        return false;
    }

    return true;
}

static bool factory_ReadFlash(UINT32 addr, UINT8 *buff, UINT32 to_read)
{
    if (addr < g_factory_info.sec_info.start_addr ||
        (addr + to_read) >
            (g_factory_info.sec_info.start_addr +
             g_factory_info.sec_info.sec_size * g_factory_info.sec_info.sec_count) ||
        addr < g_factory_info.sec_info.start_addr)
    {
        DSM_Assert(0, "factory_ReadFlash: Check addr error. 0x%x,0x%x,0x%x,0x%x,0x%x.",
                   addr,
                   to_read,
                   g_factory_info.sec_info.sec_size,
                   g_factory_info.sec_info.sec_count,
                   (g_factory_info.sec_info.sec_size * g_factory_info.sec_info.sec_count));
        return false;
    }

    memcpy(buff, hal_SpiFlashMapUncacheAddress(addr), to_read);
    return true;
}

static BOOL sec_CheckFree(UINT32 addr)
{
    UINT8 *pbuff;
    UINT32 blk_count;
    UINT32 to_read;
    UINT32 i;

    blk_count = g_factory_info.blk_info.blk_count / 2;
    to_read = sizeof(FACTORY_BLOCK_T);
    pbuff = (UINT8 *)&g_factory_info.tmp_data;
    for (i = 0; i < blk_count; i++)
    {
        if (!factory_ReadFlash(addr, pbuff, to_read))
            return false;
        if (!CheckFree(pbuff, to_read))
            return FALSE;
        addr += to_read;
    }
    return TRUE;
}

static bool factory_EraseFlash(UINT32 addr)
{
    if (addr % g_factory_info.sec_info.sec_size != 0 ||
        addr + g_factory_info.sec_info.sec_size >
            (g_factory_info.sec_info.start_addr +
             g_factory_info.sec_info.sec_size * g_factory_info.sec_info.sec_count))
    {
        DSM_Assert(0, "factory_EraseFlash: Check addr error. 0x%x,0x%x,0x%x,0x%x,0x%x.",
                   addr,
                   g_factory_info.sec_info.sec_size,
                   g_factory_info.sec_info.sec_count,
                   (addr + g_factory_info.sec_info.sec_size),
                   (g_factory_info.sec_info.sec_size * g_factory_info.sec_info.sec_count));
        return false;
    }

    if (sec_CheckFree(addr))
        return true;

    if (!hal_SpiFlashErase(addr, g_factory_info.sec_info.sec_size))
    {
        DSM_Assert(0, "factory_EraseFlash:Erase sector fail! addr = 0x%x.", addr);
        return false;
    }
    return true;
}

bool factory_FlushEraseBegin(UINT32 index)
{
    UINT32 addr;
    if (index >= g_factory_info.blk_info.blk_count)
        return false;

    addr = g_factory_info.sec_info.start_addr + index * g_factory_info.blk_info.blk_size;
    return factory_EraseFlash(addr);
}

static bool factory_WrtiteBlk(UINT32 index)
{
    UINT32 addr;
    UINT32 write_size;
    UINT8 *p;
    FACTORY_BLOCK_T *prw_blk_info;
    FACTORY_BLOCK_T *ptmp_blk_info;
    UINT32 offs;

    prw_blk_info = &g_factory_info.rw_data;
    ptmp_blk_info = &g_factory_info.tmp_data;
    DSM_MemCpy((UINT8 *)ptmp_blk_info, (UINT8 *)prw_blk_info, sizeof(FACTORY_BLOCK_T));
    g_factory_info.blk_info.cur_sn = (g_factory_info.blk_info.cur_sn == INVALID_SN) ? 0 : g_factory_info.blk_info.cur_sn + 2;
    ptmp_blk_info->version = FACTORY_VERSION;
    ptmp_blk_info->sequence = g_factory_info.blk_info.cur_sn;
    offs = FIELD_OFFSETOF(FACTORY_BLOCK_T, crc);
    ptmp_blk_info->crc = BLK_CRC32((VOID *)ptmp_blk_info, offs);
    ptmp_blk_info->crcInverted = ~ptmp_blk_info->crc;

    if (index >= g_factory_info.blk_info.blk_count)
        return false;

    addr = g_factory_info.sec_info.start_addr + g_factory_info.blk_info.blk_size * index;
    write_size = sizeof(FACTORY_BLOCK_T);
    p = (UINT8 *)ptmp_blk_info;
    return factory_WriteFlash(addr, (UINT8 *)ptmp_blk_info, write_size);
}

static bool blk_Check(FACTORY_BLOCK_T *fact_blk)
{
    UINT32 size;
    UINT32 crcValue;
    UINT8 *p;

    if (fact_blk->version != FACTORY_VERSION)
        return FALSE;

    if ((fact_blk->sequence & 1) != 0)
        return FALSE;

    if (fact_blk->crc != ~fact_blk->crcInverted)
        return FALSE;

    p = (UINT8 *)fact_blk;
    size = FIELD_OFFSETOF(FACTORY_BLOCK_T, crc);
    crcValue = BLK_CRC32(p, size);
    if (fact_blk->crc != crcValue)
        return FALSE;
    return TRUE;
}

// return valid block index in sectors.
// pfact_blk: output invalid factory block data.
static UINT32 blk_GetValidBlk(FACTORY_BLOCK_T *pfact_blk)
{
    UINT32 i;
    INT32 result;
    UINT32 index;
    UINT32 addr;
    UINT32 to_read;
    FACTORY_BLOCK_T *ptmp_blk;

    index = INVALID_BLOCK_INDEX;
    ptmp_blk = &g_factory_info.tmp_data;
    to_read = g_factory_info.blk_info.blk_size;
    for (i = 0; i < g_factory_info.blk_info.blk_count; i++)
    {
        addr = g_factory_info.sec_info.start_addr + i * g_factory_info.blk_info.blk_size;
        if (!factory_ReadFlash(addr, (UINT8 *)ptmp_blk, to_read))
            DSM_ASSERT(0, "blk_GetValidBlk factory_ReadFlash(0x%x) fail result = %d.", addr, result);

        if (blk_Check(ptmp_blk))
        {
            if (index == INVALID_BLOCK_INDEX)
            {
                index = i;
                DSM_MemCpy((UINT8 *)pfact_blk, (UINT8 *)ptmp_blk, sizeof(FACTORY_BLOCK_T));
            }
            else if (ptmp_blk->sequence > pfact_blk->sequence)
            {
                index = i;
                DSM_MemCpy((UINT8 *)pfact_blk, (UINT8 *)ptmp_blk, sizeof(FACTORY_BLOCK_T));
            }
        }
    }
    if (index == INVALID_BLOCK_INDEX)
    {
        DSM_MemSet((UINT8 *)pfact_blk, 0xff, sizeof(FACTORY_BLOCK_T));
    }
    else
    {
        addr = g_factory_info.sec_info.start_addr + index * g_factory_info.blk_info.blk_size;
        if (!factory_ReadFlash(addr, (UINT8 *)ptmp_blk, to_read))
            DSM_ASSERT(0, "blk_GetValidBlk factory_ReadFlash(0x%x) fail.", addr);
    }
    return index;
}

static VOID factory_init_flash_info(VOID)
{
    UINT32 userFieldBase = 0;
    UINT32 userFieldSize = 0;
    UINT32 sectorSize = 0;
    UINT32 blockSize = 0;
    UINT32 factory_base;
    UINT32 factory_end;
#ifdef CHIP_HAS_AP
    DRV_FlashGetUserFieldInfo(&userFieldBase, &userFieldSize, &sectorSize, &blockSize);
#else
    memd_FlashGetUserFieldInfo(&userFieldBase, &userFieldSize, &sectorSize, &blockSize);
#endif
    factory_base = (UINT32)&_factory_start;
    factory_base &= 0xffffff;

    factory_end = factory_base + 0x2000; //(UINT32)&_factory_end;
    factory_end &= 0xffffff;

    g_factory_info.sec_info.sec_size = sectorSize;
    g_factory_info.sec_info.start_addr = factory_base;
    g_factory_info.sec_info.sec_count = (factory_end - factory_base) / sectorSize;
}

static VOID factory_init_blk_info(VOID)
{
    UINT32 blk_index;

    g_factory_info.blk_info.blk_size = sizeof(FACTORY_BLOCK_T);
    g_factory_info.blk_info.blk_count =
        (g_factory_info.sec_info.sec_count * g_factory_info.sec_info.sec_size) / sizeof(FACTORY_BLOCK_T);
    blk_index = blk_GetValidBlk(&g_factory_info.rw_data);
    if (blk_index == INVALID_BLOCK_INDEX)
    {
        g_factory_info.blk_info.cur_blk_idx = INVALID_BLOCK_INDEX;
        g_factory_info.blk_info.cur_sn = INVALID_SN;
    }
    else
    {
        g_factory_info.blk_info.cur_blk_idx = blk_index;
        g_factory_info.blk_info.cur_sn = (g_factory_info.rw_data.sequence);
    }
}

static void FACTORY_Writeback(void *param)
{
    if (g_factory_info.has_init == 0)
        return;

    UINT32 half_index = g_factory_info.blk_info.blk_count / 2;
    if (g_factory_info.cur_index == INVALID_BLOCK_INDEX)
    {
        if (!factory_FlushEraseBegin(0))
            goto error;
        if (!factory_FlushEraseBegin(half_index))
            goto error;
        g_factory_info.cur_index = 0;
    }
    else
    {
        UINT32 next_index = g_factory_info.cur_index;
        UINT32 end_index = (next_index < half_index) ? half_index : (half_index * 2);
        for (next_index += 1; next_index < end_index; next_index++)
        {
            FACTORY_BLOCK_T *ptmp_blk = &g_factory_info.tmp_data;
            UINT32 addr = g_factory_info.sec_info.start_addr + next_index * g_factory_info.blk_info.blk_size;
            if (!factory_ReadFlash(addr, (UINT8 *)ptmp_blk, g_factory_info.blk_info.blk_size))
                DSM_ASSERT(0, "factory_ReadFlash(0x%x) fail.", addr);

            if (CheckFree((UINT8 *)ptmp_blk, sizeof(FACTORY_BLOCK_T)))
                break;
        }

        if (next_index == end_index)
        {
            if (next_index == g_factory_info.blk_info.blk_count)
                next_index = 0;
            if (!factory_FlushEraseBegin(next_index))
                goto error;
        }
        g_factory_info.cur_index = next_index;
    }

    if (!factory_WrtiteBlk(g_factory_info.cur_index))
        goto error;

    return;

error:
    DSM_Assert(0, "flush:FACT_STATUS_ERROR.");
}

//FACTORY APIS.

INT32 FACTORY_Init(VOID)
{
    factory_SemInit();
    factory_init_flash_info();
    factory_init_blk_info();
    g_factory_info.cur_index = g_factory_info.blk_info.cur_blk_idx;
    g_factory_info.has_init = 1;
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_ReadBlock(FACTORY_BLOCK_T *facory_blk)
{
    if (g_factory_info.has_init == 0)
        return ERR_FACTORY_NOT_INIT;

    factory_SemWaitFor();
    DSM_MemCpy((UINT8 *)facory_blk, (UINT8 *)&g_factory_info.rw_data, sizeof(FACTORY_BLOCK_T));
    factory_SemRelease();
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_WriteBlock(FACTORY_BLOCK_T *facory_blk)
{
    if (g_factory_info.has_init == 0)
        return ERR_FACTORY_NOT_INIT;

    factory_SemWaitFor();
    DSM_MemCpy((UINT8 *)&g_factory_info.rw_data, (UINT8 *)facory_blk, sizeof(FACTORY_BLOCK_T));
    factory_SemRelease();

    COS_TaskCallbackNotif(sf_async_task_handle(), FACTORY_Writeback, NULL);
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_ReadSupple(UINT8 *buff)
{
    if (g_factory_info.has_init == 0)
        return ERR_FACTORY_NOT_INIT;

    factory_SemWaitFor();
    DSM_MemCpy(buff, (UINT8 *)&g_factory_info.rw_data.supplementary, FACT_SUPPLEMENTARY_LEN);
    factory_SemRelease();
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_WriteSupple(UINT8 *buff)
{
    if (g_factory_info.has_init == 0)
        return ERR_FACTORY_NOT_INIT;

    factory_SemWaitFor();
    DSM_MemCpy((UINT8 *)g_factory_info.rw_data.supplementary, buff, FACT_SUPPLEMENTARY_LEN);
    factory_SemRelease();

    COS_TaskCallbackNotif(sf_async_task_handle(), FACTORY_Writeback, NULL);
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_ReadField(UINT32 offset, UINT32 size, UINT8 *buff)
{
    UINT8 *field_buff;
    if (g_factory_info.has_init == 0)
        return ERR_FACTORY_NOT_INIT;

    factory_SemWaitFor();
    field_buff = (UINT8 *)&g_factory_info.rw_data + offset;
    DSM_MemCpy(buff, field_buff, size);
    factory_SemRelease();
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_WriteField(UINT32 offset, UINT32 size, UINT8 *buff)
{
    UINT8 *field_buff;
    if (g_factory_info.has_init == 0)
        return ERR_FACTORY_NOT_INIT;

    factory_SemWaitFor();
    field_buff = (UINT8 *)&g_factory_info.rw_data + offset;
    DSM_MemCpy(field_buff, buff, size);
    factory_SemRelease();

    COS_TaskCallbackNotif(sf_async_task_handle(), FACTORY_Writeback, NULL);
    return ERR_FACTORY_SUCCESS;
}

INT32 FACTORY_SyncReadField(UINT32 offset, UINT32 size, UINT8 *buff)
{
    return FACTORY_ReadField(offset, size, buff);
}

INT32 FACTORY_SyncWriteField(UINT32 offset, UINT32 size, UINT8 *buff)
{
    INT32 result = FACTORY_WriteField(offset, size, buff);
    if (result == 0)
        COS_TaskWaitEventHandled(sf_async_task_handle());
    return result;
}

#endif
