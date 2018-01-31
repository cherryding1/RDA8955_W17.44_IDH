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

#ifdef __MEMD_CACHE_TEST__
#include "cs_types.h"
#include "ts.h"
#include "hal_ebc.h"
#include "hal_sys.h"
#include "string.h"
#include "memd_m.h"
#include "memd_cache.h"

typedef struct _node_info
{
    UINT32 type_style;
    UINT32 bufferSize_max;
    UINT32 row;
    UINT32 info_array[1000][3];
} NODE_INFO;


extern UINT32 VDS_CacheGetBlkCnt(VOID);
extern UINT32 _remain_start;
extern UINT32 _remain_end;
extern FLASH_CACHE g_flash_cache;
NODE_INFO node_info = {0,0,0,{{0,}}};


VOID mend_CacheNodeStatistics(VOID)
{
    F_NODE *pnode = NULL;
    F_NODE *pprenode = NULL;
    W_FLASH* w_flash;
    pnode = g_flash_cache.pnode;
    node_info.type_style = 0;
    node_info.bufferSize_max = 0;
    INT32 flag = 0;
    while(pnode != NULL)
    {
        if(pnode->type == NODE_TYPE_W)
        {
            w_flash = &pnode->node.w_flash;
            node_info.bufferSize_max += w_flash->byteSize;
            node_info.type_style = 16*node_info.type_style + 1;
        }
        else
        {
            node_info.type_style = 16*node_info.type_style;
        }

        pprenode = pnode;
        pnode = pprenode->next;
    }

    if(node_info.row == 0)
    {
        node_info.info_array[0][0] = g_flash_cache.node_cnt;
        node_info.info_array[0][1] = node_info.type_style;
        node_info.info_array[0][2] = node_info.bufferSize_max;
        node_info.row++;
    }

    for(int i = 0; i<node_info.row; i++)
    {
        if((g_flash_cache.node_cnt == node_info.info_array[i][0]) && (node_info.type_style == node_info.info_array[i][1]) && (node_info.bufferSize_max == node_info.info_array[i][2]))
        {
            flag = 1;
            break;
        }
    }

    if(flag != 1)
    {
        MEMD_EVENT(0xA1000000 | g_flash_cache.node_cnt);
        MEMD_EVENT(0xA2000000 | node_info.type_style);
        MEMD_EVENT(0xA3000000 | node_info.bufferSize_max);
        node_info.info_array[node_info.row][0] = g_flash_cache.node_cnt;
        node_info.info_array[node_info.row][1] = node_info.type_style;
        node_info.info_array[node_info.row][2] = node_info.bufferSize_max;
        node_info.row++;
        pnode = g_flash_cache.pnode;
        while(pnode != NULL)
        {
            if(pnode->type == NODE_TYPE_W)
            {
                w_flash = &pnode->node.w_flash;
                node_info.bufferSize_max += w_flash->byteSize;
                MEMD_EVENT(0xA4000000 | w_flash->byteSize);
            }

            pprenode = pnode;
            pnode = pprenode->next;
        }
    }
}

#if 0
void tst_memd_cache(void)
{
    UINT8* addr;
    addr = (UINT8*)0x3F9000;
    UINT8 PbyteSize;
    //INT32 result;
    INT32 ret;
    UINT8 *buffer_w;
    UINT8 *buffer_r;
    UINT8 str_w[9] = {0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00};
    UINT8 str_r[512];
    UINT32 offs = 0;

    buffer_w = str_w;
    buffer_r = str_r;
    for(int i = 0; i < 45; i++)
    {
        ret = memd_CacheErase(addr,(addr + 4096),0);
        while(memd_CacheGetNodeCount())
        {
            memd_CacheFlush(TRUE);
        }

        memd_FlashWrite(addr, 1, &PbyteSize, buffer_w + 3);
        memd_FlashWrite(addr + 10, 1, &PbyteSize, buffer_w + 4);
        MEMD_EVENT(0xA1000000 | memd_CacheGetNodeCount());
        for(int j = 0; j < 9; j++)
        {
            for(int k = 0; k < 9 - j; k++)
            {
                ret = memd_CacheWrite(addr + j + 1, 1, &PbyteSize, buffer_w + k);
            }

        }

        while(memd_CacheGetNodeCount() > 45 - i)
        {
            memd_CacheFlush(TRUE);
        }

        ret = memd_CacheRead(addr, 11, &PbyteSize, buffer_r);
        MEMD_EVENT(0xA2000000 | memd_CacheGetNodeCount());
        //CSW_TRACE(7, "open hd = 0x%x", buffer_r);
        if(ret == 0)
        {
            for(int m = 0; m < 11; m++)
            {
                MEMD_EVENT(0xAA000000 | *(buffer_r + m));
            }
        }

        sxr_Sleep(2000);
    }

}
#endif


#define REMAIN_START (UINT32)0x003F6000//(((UINT32)&_remain_start) & 0xffffff)
#define REMAIN_END (UINT32)0x003FA000//(((UINT32)&_remain_end) & 0xffffff)
#define FLASH_HIGH_VALUE 0xffff0000
#define FLASH_LOW_VALUE  0x0000ffff
#define FLASH_BASE_VALUE    0xffffffff
#define OFFSET_TIME_VALUE(offs,tm) (((FLASH_BASE_VALUE >> (offs/512)) & \
                                                               FLASH_HIGH_VALUE) | ((FLASH_BASE_VALUE >> tm) & \
                                                               FLASH_LOW_VALUE))

//#define MEMD_EVENT(x)     hal_HstSendEvent(x)

void tst_memd_get_value(UINT32 addr,UINT32 time,UINT32 size,UINT8* buff)
{
    UINT32 read_size = 0;
    INT32 result;
    UINT32 offs,base_offs;
    UINT8* p;
    UINT32 tmp;
    UINT32 pre;
    UINT8* base_buff;


    if(addr < REMAIN_START || addr + size > REMAIN_END)
    {
        MEMD_Assert(0,"tst_memd_get_value overfllow, addr = 0x%x,size = 0x%x.",addr,size);
    }

    result = memd_FlashRead((UINT8*)addr,size, &read_size, buff);
    if(result)
    {
        MEMD_Assert(0,"tst_memd_get_value fail, result = %d,addr = 0x%x,size = 0x%x.",
                    result,addr,size);
    }
    base_buff = buff;
    base_offs = addr - REMAIN_START;
    offs = base_offs;
    pre = base_offs - (base_offs/4)*4;
    offs = offs - pre;
    tmp = time == 0 ? FLASH_BASE_VALUE : OFFSET_TIME_VALUE(offs,time);
    p = (UINT8*)(&tmp);
    for(offs = base_offs; offs - base_offs < size; offs++)
    {
        if((offs/4)*4 == offs)
        {
            tmp = time == 0 ? FLASH_BASE_VALUE : OFFSET_TIME_VALUE(offs,time);
            p = (UINT8*)(&tmp);
        }
        *buff = *p;
        buff++;
        p++;
    }
    return;
}


UINT8 tst_check(UINT32 tms)
{
    INT32 result;
    UINT32 addr = 0;
    UINT32 size = 0;
    UINT32 readsize = 0;
    UINT8 read_buff[100];
    UINT8 preset_buff[100];

    addr = REMAIN_START;
    while(addr < REMAIN_END)
    {
        size = (REMAIN_END - addr) < 100 ? (REMAIN_END - addr) : 100;
        readsize = 0;
        result = memd_CacheRead((UINT8*)addr, size, &readsize,read_buff);
        if(result)
        {
            MEMD_Assert(0,"tst_check fail, result = %d.",result);
        }
        tst_memd_get_value(addr,tms,size,preset_buff);
        if(memcmp(read_buff,preset_buff,size) != 0)
        {
            MEMD_Assert(0,"tst_check memcpm error, read_buff = 0x%x,preset_buff = 0x%x.",
                        read_buff,preset_buff);
            return 1;
        }
        addr += size;
    }
    return 0;
}


UINT8 tst_check_1(UINT32 tms,UINT32 addr,UINT32 size)
{
    INT32 result;
    UINT32 tmp_size = 0;
    UINT32 readsize = 0;
    UINT32 end_addr;
    UINT8 read_buff[100];
    UINT8 preset_buff[100];

    end_addr = addr + size;

    while(addr < end_addr)
    {
        tmp_size = (end_addr - addr) < 100 ? (end_addr - addr) : 100;
        readsize = 0;
        result = memd_CacheRead((UINT8*)addr, tmp_size, &readsize,read_buff);
        if(result)
        {
            MEMD_Assert(0,"tst_check fail, result = %d.",result);
        }
        tst_memd_get_value(addr,tms,tmp_size,preset_buff);
        if(memcmp(read_buff,preset_buff,tmp_size) != 0)
        {
            MEMD_Assert(0,"tst_check memcpm error, read_buff = 0x%x,preset_buff = 0x%x.",
                        read_buff,preset_buff);
            return 1;
        }
        addr += tmp_size;
    }
    return 0;
}

void tst_memd_cache(void)
{
    UINT32 addr;
    UINT32 base_addr;
    INT32 ret;
    UINT8 buff[512];
    UINT32 size;
    UINT32 written_size;
    UINT32 all_written_size = 0;
    UINT32 tms;
    UINT32 node_cnt;
    int i;

    sxr_Sleep(20);
    CSW_TRACE(0, "TST_MEMD:tst_memd_cache begin");
    sxr_Sleep(20);
    base_addr = REMAIN_START;
    for(i = 0; i < 10; i++)
    {
        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:test i = %d.",i);
        sxr_Sleep(20);

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:1.cache flush begin.");
        sxr_Sleep(20);

        node_cnt = memd_CacheGetNodeCount(0);
        while(memd_CacheGetNodeCount(0))
        {
            memd_CacheFlush(TRUE);
            sxr_Sleep(20);
        }

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:cache flush(%d) ok.",node_cnt);
        sxr_Sleep(20);

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:2.cache erase begin.");
        sxr_Sleep(20);

        //cache erase.
        for(addr = REMAIN_START; addr < REMAIN_END; addr += 4096)
        {
            ret = memd_CacheErase((UINT8*)addr,(UINT8*)(addr + 4096),0);
            if(ret)
            {
                sxr_Sleep(20);
                CSW_TRACE(0, "TST_MEMD:cache erase fail.");
                sxr_Sleep(20);
                break;
            }
        }

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:cache erase ok.");
        CSW_TRACE(0, "TST_MEMD:3.cache erase check begin.");
        sxr_Sleep(20);

        if(tst_check(0))
        {
            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:memd_CacheErase check error.");
            sxr_Sleep(20);
            return;
        }
        else
        {
            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:cache erase check ok.");
            sxr_Sleep(20);
        }

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:4.cache erase flush begin.");
        sxr_Sleep(20);
        //cache erase node flush.
        node_cnt = memd_CacheGetNodeCount(0);
        while(memd_CacheGetNodeCount(0))
        {
            memd_CacheFlush(TRUE);
        }
        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:cache erase flush(%d) ok.",node_cnt);
        sxr_Sleep(20);
        // cache erase,cache write.
        for(tms = 0; tms < 16; tms ++)
        {

            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:5.tms = %d.",tms);
            sxr_Sleep(20);

            size = 512;
            all_written_size = 0;
            for(addr = REMAIN_START; addr < REMAIN_END; addr += size)
            {
                if((addr % 4096) == 0)
                {
                    ret = memd_CacheErase((UINT8*)addr,(UINT8*)(addr + 4096),0);
                    if(tst_check_1(0,addr,4096))
                    {
                        sxr_Sleep(20);
                        CSW_TRACE(0, "TST_MEMD:tst_check_1(0,0x%x,4096) check error.",addr);
                        sxr_Sleep(20);
                        return;
                    }
                    else
                    {
                        sxr_Sleep(20);
                        CSW_TRACE(0, "TST_MEMD:tst_check_1(0,0x%x,4096) check ok.",addr);
                        sxr_Sleep(20);
                    }
                }
                tst_memd_get_value(addr,tms,size,buff);
                ret = memd_CacheWrite((UINT8*)addr, 512, &written_size, buff);
                if(ret)
                {
                    sxr_Sleep(20);
                    CSW_TRACE(0, "TST_MEMD:memd_CacheWrite error.ret = %d",ret);
                    sxr_Sleep(20);
                    return;
                }
                else
                {
                    all_written_size += written_size;
                    sxr_Sleep(20);
                    CSW_TRACE(0, "TST_MEMD:memd_CacheWrite(0x%x,0x%x) ok.",addr,size);
                    sxr_Sleep(20);
                }

                if(tst_check_1(tms,base_addr,all_written_size))
                {
                    sxr_Sleep(20);
                    CSW_TRACE(0, "TST_MEMD:tst_check_1(0x%x,0x%x,0x%x) error.",tms,base_addr,all_written_size);
                    sxr_Sleep(20);
                    return;
                }
                else
                {
                    sxr_Sleep(20);
                    CSW_TRACE(0, "TST_MEMD:tst_check_1(0x%x,0x%x,0x%x) ok.",tms,base_addr,all_written_size);
                    sxr_Sleep(20);
                }
            }
        }

        // check write cache.
        if(tst_check(tms - 1))
        {
            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:6.tst_check(0x%x) error.",tms -1);
            sxr_Sleep(20);
            return;
        }
        else
        {
            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:6.tst_check(0x%x) ok.",tms -1);
            sxr_Sleep(20);
        }

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:7.cache flush begin.");
        sxr_Sleep(20);
        // cache flush.
        node_cnt = memd_CacheGetNodeCount(0);
        while(memd_CacheGetNodeCount(0))
        {
            memd_CacheFlush(TRUE);
        }

        sxr_Sleep(20);
        CSW_TRACE(0, "TST_MEMD:7.cache flush(%d) ok.",node_cnt);
        sxr_Sleep(20);

        // check phy-flash.
        if(tst_check(tms - 1))
        {
            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:8.tst_check(0x%x) error.",tms -1);
            sxr_Sleep(20);
            return;
        }
        else
        {
            sxr_Sleep(20);
            CSW_TRACE(0, "TST_MEMD:8.tst_check(0x%x) ok.",tms -1);
            sxr_Sleep(20);
        }
    }
    sxr_Sleep(20);
    CSW_TRACE(0, "TST_MEMD:tst_memd_cache end.");
    sxr_Sleep(20);
}

#endif

