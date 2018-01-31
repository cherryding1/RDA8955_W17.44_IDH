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

#ifdef __VDS_QUICK_FLUSH__
#include "cs_types.h"
#include "ts.h"
#include "hal_ebc.h"
#include "hal_sys.h"
#include "string.h"
#include "memd_m.h"
#include "memd_cache.h"
#include "cos.h"

extern UINT32 _user_data_start;
#if defined(REDUNDANT_DATA_REGION)
extern UINT32 _remain_start;
#endif
extern UINT32 _calib_start;
extern UINT32 _factory_start;
extern UINT32 _factory_end;

FLASH_CACHE g_flash_cache;
static COS_SEMA g_halSpiFlashSem = COS_SEMA_UNINIT;

VOID memd_SemWaitFor(VOID)
{
    if(g_flash_cache.sem == 0)
    {
        g_flash_cache.sem = sxr_NewSemaphore(1);
    }
    if((HANDLE)NULL != g_flash_cache.sem)
    {
        sxr_TakeSemaphore(g_flash_cache.sem);
    }
}


VOID memd_SemRelease(VOID)
{
    if(g_flash_cache.sem == 0)
    {
        g_flash_cache.sem = sxr_NewSemaphore(1);
    }
    if((HANDLE)NULL != g_flash_cache.sem)
    {
        sxr_ReleaseSemaphore(g_flash_cache.sem);
    }
}

VOID memd_Init(VOID)
{
    memset(&g_flash_cache,0,sizeof(FLASH_CACHE));
    g_flash_cache.flash_region_cur=FLASH_REGION_UNDEFINE;
    
    COS_SemaInit(&g_halSpiFlashSem, 1);
    hal_SpiFlashSetLockCallback(COS_SemaTake, &g_halSpiFlashSem);
    hal_SpiFlashSetUnlockCallback(COS_SemaRelease, &g_halSpiFlashSem);
    hal_SpiFlashSetCheckIrq(TRUE);
}

FLASH_REGION memd_GetRegionId(UINT32 addr)
{
    UINT32 user_data_start;
#if defined(REDUNDANT_DATA_REGION)
    UINT32 remain_start;
#endif
    UINT32 calib_start;
    UINT32 factory_start;

    user_data_start = (((UINT32 ) &_user_data_start) & 0x00ffffff);
#if defined(REDUNDANT_DATA_REGION)
    remain_start = (((UINT32)&_remain_start) & 0x00ffffff);
#endif
    calib_start = (((UINT32)&_calib_start) & 0x00ffffff);
    factory_start = (((UINT32)&_factory_start) & 0x00ffffff);

#if defined(REDUNDANT_DATA_REGION)
    if(addr >= user_data_start && addr < remain_start)
    {
        return FLASH_REGION_USER_DATA;
    }
    else if(addr >= remain_start && addr < calib_start)
    {
        return FLASH_REGION_REMAIN;
    }
#else
    if(addr >= user_data_start && addr < calib_start)
    {
        return FLASH_REGION_USER_DATA;
    }
#endif
    else if(addr >= calib_start && addr < factory_start)
    {
        return FLASH_REGION_CALIB;
    }
    else
    {
        return FLASH_REGION_UNDEFINE;
    }
}


F_NODE* memd_GetNode(FLASH_REGION region_id)
{
    if(region_id == FLASH_REGION_USER_DATA)
    {
        return g_flash_cache.pnode_user;
    }
#if defined(REDUNDANT_DATA_REGION)
    else if(region_id == FLASH_REGION_REMAIN)
    {
        return g_flash_cache.pnode_remain;
    }
#endif
    else if(region_id == FLASH_REGION_CALIB)
    {
        return g_flash_cache.pnode_calib;
    }
    else
    {
        return NULL;
    }
}


UINT32 memd_GetNodeCount(FLASH_REGION region_id)
{
    if(region_id == FLASH_REGION_USER_DATA)
    {
        return g_flash_cache.node_cnt_user;
    }
#if defined(REDUNDANT_DATA_REGION)
    else if(region_id == FLASH_REGION_REMAIN)
    {
        return g_flash_cache.node_cnt_remain;
    }
#endif
    else if(region_id == FLASH_REGION_CALIB)
    {
        return g_flash_cache.node_cnt_calib;
    }
    else
    {
        return 0;
    }
}

VOID memd_FreeNode(F_NODE* pnode)
{
    UINT32 cri_status;

    MEMD_Assert(pnode,"pnode is null!");
    if(pnode == g_flash_cache.pnode_user)
    {
        cri_status = hal_SysEnterCriticalSection();
        g_flash_cache.pnode_user = pnode->next;
        g_flash_cache.node_cnt_user --;
        hal_SysExitCriticalSection(cri_status);
        if(pnode->type == NODE_TYPE_W)
        {
            MEMD_FREE(pnode->node.w_flash.buffer);
        }
        MEMD_FREE(pnode);

    }
#if defined(REDUNDANT_DATA_REGION)
    else if(pnode == g_flash_cache.pnode_remain)
    {
        cri_status = hal_SysEnterCriticalSection();
        g_flash_cache.pnode_remain = pnode->next;
        g_flash_cache.node_cnt_remain --;
        hal_SysExitCriticalSection(cri_status);
        if(pnode->type == NODE_TYPE_W)
        {
            MEMD_FREE(pnode->node.w_flash.buffer);
        }
        MEMD_FREE(pnode);

    }
#endif
    else if(pnode == g_flash_cache.pnode_calib)
    {
        cri_status = hal_SysEnterCriticalSection();
        g_flash_cache.pnode_calib = pnode->next;
        g_flash_cache.node_cnt_calib --;
        hal_SysExitCriticalSection(cri_status);
        if(pnode->type == NODE_TYPE_W)
        {
            MEMD_FREE(pnode->node.w_flash.buffer);
        }
        MEMD_FREE(pnode);

    }
    else
    {
        MEMD_Assert(0,"pnode = 0x%x.",(UINT32)pnode);
    }
    g_flash_cache.pnode_cur = NULL;
}


VOID memd_AddNode(FLASH_REGION region_id,F_NODE* pprenode,F_NODE* pnewnode)
{
    UINT32 cri_status;

    MEMD_Assert(pnewnode,"pnewnode is null!");
    if(region_id == FLASH_REGION_USER_DATA)
    {
        if(pprenode)
        {
            cri_status = hal_SysEnterCriticalSection();
            pprenode->next = pnewnode;
            g_flash_cache.node_cnt_user ++;
            hal_SysExitCriticalSection(cri_status);
        }
        else
        {
            cri_status = hal_SysEnterCriticalSection();
            g_flash_cache.pnode_user = pnewnode;
            g_flash_cache.node_cnt_user ++;
            hal_SysExitCriticalSection(cri_status);
        }
    }
#if defined(REDUNDANT_DATA_REGION)
    else if(region_id == FLASH_REGION_REMAIN)
    {
        if(pprenode)
        {
            cri_status = hal_SysEnterCriticalSection();
            pprenode->next = pnewnode;
            g_flash_cache.node_cnt_remain ++;
            hal_SysExitCriticalSection(cri_status);
        }
        else
        {
            cri_status = hal_SysEnterCriticalSection();
            g_flash_cache.pnode_remain = pnewnode;
            g_flash_cache.node_cnt_remain ++;
            hal_SysExitCriticalSection(cri_status);
        }
    }
#endif
    else if(region_id == FLASH_REGION_CALIB)
    {
        if(pprenode)
        {
            cri_status = hal_SysEnterCriticalSection();
            pprenode->next = pnewnode;
            g_flash_cache.node_cnt_calib ++;
            hal_SysExitCriticalSection(cri_status);
        }
        else
        {
            cri_status = hal_SysEnterCriticalSection();
            g_flash_cache.pnode_calib = pnewnode;
            g_flash_cache.node_cnt_calib ++;
            hal_SysExitCriticalSection(cri_status);
        }
    }
    else
    {
        MEMD_Assert(0,"region_id = 0x%x error.",region_id);
    }
}


PUBLIC MEMD_ERR_T memd_CacheWrite(UINT8 *flashAddress,
                                  UINT32 byteSize,
                                  UINT32 * pWrittenByteSize,
                                  UINT8* buffer)
{
    W_FLASH* w_flash;
    F_NODE *pnode = NULL;
    F_NODE *pprenode = NULL;
    F_NODE *pnewnode = NULL;
    INT32 ret = 0;
    FLASH_REGION region_id;

    MEMD_EVENT(0xD1000000 | (UINT32)flashAddress);
    MEMD_Assert(byteSize <= 8192, "memd_CacheWrite(0x%x) too larget",byteSize);
    memd_SemWaitFor();
    region_id = memd_GetRegionId((UINT32)flashAddress);
    if(region_id == FLASH_REGION_UNDEFINE)
    {
        MEMD_Assert(0,"memd_CacheWrite addr error. addr = 0x%x",(UINT32)flashAddress);
    }
    pnode = memd_GetNode(region_id);
    pprenode = pnode;
    do
    {
        if(!pnode)
        {
            pnewnode = MEMD_MALLOC(sizeof(F_NODE));
            if(!pnewnode)
            {
                ret = ERR_DRV_CACHE_MALLOC ;
                MEMD_EVENT(0xD2000000 |sizeof(F_NODE));
                MEMD_Assert(0,"write cache 1 malloc(%d) fail.",sizeof(F_NODE));
                break;
            }
            pnewnode->type = NODE_TYPE_W;
            pnewnode->next = NULL;
            pnewnode->repeat = 0;

            w_flash = &(pnewnode->node.w_flash);
            w_flash->flashAddress = (UINT32)flashAddress;
            w_flash->byteSize = byteSize;
            w_flash->buffer = MEMD_MALLOC(byteSize);
            if(!w_flash->buffer)
            {
                ret = ERR_DRV_CACHE_MALLOC ;
                MEMD_EVENT(0xD3000000 | byteSize);
                MEMD_Assert(0,"write cache 2 malloc(%d) fail.",byteSize);
                break;
            }
            w_flash->status = FLASH_NODE_STATUS_INIT;
            memcpy((UINT8*)w_flash->buffer,(UINT8*)buffer,byteSize);
            w_flash->WrittenByteSize = 0;
            *pWrittenByteSize = byteSize;
            memd_AddNode(region_id,pprenode,pnewnode);
            if(NULL == pprenode)
            {
                MEMD_EVENT(0xD3000000 | (UINT32)flashAddress);
                MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheWrite: node_index = fst,flashAddress= 0x%x,byteSize = 0x%x.",
                                 (UINT32)flashAddress,
                                 (UINT32)byteSize);
            }
            else
            {
                MEMD_EVENT(0xD4000000 | (UINT32)flashAddress);
                MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheWrite: flashAddress= 0x%x,byteSize = 0x%x.",
                                 (UINT32)flashAddress,
                                 (UINT32)byteSize);
            }
            break;
        }
        else
        {
            pprenode = pnode;
            pnode = pprenode->next;
        }
    }
    while(1);
    memd_SemRelease();
    return ret;
}



//MEMD_TEST g_test_memd[8] = {{0,},};
//UINT32 g_test_index = 0;

PUBLIC MEMD_ERR_T memd_CacheRead(UINT8 *flashAddress,
                                 UINT32 byteSize,
                                 UINT32 * pReadByteSize,
                                 UINT8* buffer)
{
    F_NODE *pnode = NULL;
    F_NODE *pprenode = NULL;
    W_FLASH* w_flash;
    E_FLASH* e_flash;
    UINT32 byte_offset = 0;
    UINT32 byte_cache = 0;
    UINT32 flashAddress_end;
    INT32 ret = 0;
    FLASH_REGION region_id;

    memd_SemWaitFor();
    flashAddress_end = (UINT32)flashAddress + byteSize;
    ret = memd_FlashRead(flashAddress, byteSize, pReadByteSize, buffer);
    if(ret != 0)
    {
        MEMD_EVENT(0xC2000000 | (UINT32)flashAddress);
        MEMD_Assert(0,"memd_FlashRead(0x%x,0x%x) fail.",(UINT32)flashAddress,byteSize);
        memd_SemRelease();
        return ERR_DRV_CACHE_READ_ERROR;
    }

    *pReadByteSize = byteSize;

    region_id = memd_GetRegionId((UINT32)flashAddress);
    if(region_id == FLASH_REGION_UNDEFINE)
    {
        MEMD_Assert(0,"memd_CacheRead addr error. addr = 0x%x",(UINT32)flashAddress);
    }
    pnode = memd_GetNode(region_id);
    pprenode = pnode;
    while(pnode != NULL)
    {
        if(pnode->type == NODE_TYPE_W)
        {
            w_flash = &pnode->node.w_flash;
            if(w_flash->flashAddress < (UINT32)flashAddress)
            {
                byte_offset = (UINT32)(flashAddress - w_flash->flashAddress);
                if((byte_offset + byteSize) <= w_flash->byteSize)
                {
                    memcpy(buffer, ((w_flash->buffer) + byte_offset), byteSize);
                    //MEMD_EVENT(0xC1000000 | (UINT32)byteSize);
                }
                else
                {
                    if(byte_offset < w_flash->byteSize)
                    {
                        byte_cache = w_flash->byteSize - byte_offset;
                        memcpy(buffer, (w_flash->buffer + byte_offset), byte_cache);
                        //MEMD_EVENT(0xC2000000 | (UINT32)byteSize);
                    }
                    else
                    {
                        // do nothing.
                        //MEMD_EVENT(0xC3000000 | (UINT32)byteSize);
                    }
                }
            }
            else
            {
                byte_offset = (UINT32)(w_flash->flashAddress - (UINT32)flashAddress);
                if(byte_offset < byteSize)
                {
                    byte_cache = byteSize - byte_offset;
                    if(byte_cache < w_flash->byteSize)
                    {
                        memcpy(buffer + byte_offset, w_flash->buffer, byte_cache);
                        //MEMD_EVENT(0xC4000000 | (UINT32)byteSize);
                    }
                    else
                    {
                        memcpy(buffer + byte_offset, w_flash->buffer, w_flash->byteSize);
                        //MEMD_EVENT(0xC5000000 | (UINT32)byteSize);
                    }
                }
                else
                {
                    // do nothing.
                    //MEMD_EVENT(0xC6000000 | (UINT32)byteSize);
                }
            }
        }

        else if(pnode->type == NODE_TYPE_E)
        {
            e_flash = &pnode->node.e_flash;
            if(e_flash->startFlashAddress < (UINT32)flashAddress)
            {
                if(e_flash->endFlashAddress > flashAddress_end)
                {
                    memset(buffer,0xff,byteSize);
                    //buffer += byteSize;
                    //MEMD_EVENT(0xE1000000 | (UINT32)byteSize);
                }
                else
                {
                    if(e_flash->endFlashAddress > (UINT32)flashAddress)
                    {
                        byte_cache = (UINT32)(e_flash->endFlashAddress - (UINT32)flashAddress);
                        memset(buffer,0xff,byte_cache);
                        //buffer += byte_cache;
                        // MEMD_EVENT(0xE2000000 | (UINT32)byteSize);
                    }
                    else
                    {
                        // do nothing.
                        //MEMD_EVENT(0xE3000000 | (UINT32)byteSize);
                    }
                }
            }
            else
            {
                byte_offset = e_flash->startFlashAddress - (UINT32)flashAddress;
                if(e_flash->startFlashAddress < flashAddress_end)
                {
                    if(e_flash->endFlashAddress > flashAddress_end)
                    {
                        byte_cache = (UINT32)(flashAddress_end - e_flash->startFlashAddress);
                        memset(buffer + byte_offset,0xff,byte_cache);
                        //buffer += byte_cache;
                        //MEMD_EVENT(0xE4000000 | (UINT32)byteSize);
                    }
                    else
                    {
                        memset(buffer + byte_offset,0xff,e_flash->endFlashAddress - e_flash->startFlashAddress);
                        //buffer += byteSize;
                        //MEMD_EVENT(0xE5000000 | (UINT32)byteSize);
                    }
                }
                else
                {
                    // do nothing.
                    //MEMD_EVENT(0xE6000000 | (UINT32)byteSize);
                }
            }
        }

        pprenode = pnode;
        pnode = pprenode->next;
    }
    //sxr_Sleep(20);
    MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheRead: node_cnt = 0x%x,addr = 0x%x,bytesize = 0x%x,pReadByteSize = 0x%x,buffer = 0x%x.",
                     g_flash_cache.node_cnt,
                     (UINT32)flashAddress,
                     byteSize,
                     *pReadByteSize,
                     buffer);
    //sxr_Sleep(20);
    TS_DUMP(buffer,byteSize,16);
    memd_SemRelease();
    return ret;
}


PUBLIC MEMD_ERR_T memd_CacheErase(UINT8 *startFlashAddress, UINT8 *endFlashAddress,UINT32 time)
{
    E_FLASH* e_flash;
    F_NODE *pnode = NULL;
    F_NODE *pprenode = NULL;
    F_NODE *pnewnode = NULL;
    INT32 ret = 0;
    FLASH_REGION region_id;

    MEMD_EVENT(0xE1000000 |(UINT32)startFlashAddress);
    MEMD_Assert((endFlashAddress - startFlashAddress) == 4096, "memd_CacheErase(0x%x) too larget",(endFlashAddress - startFlashAddress));
    memd_SemWaitFor();
    region_id = memd_GetRegionId((UINT32)startFlashAddress);
    if(region_id == FLASH_REGION_UNDEFINE)
    {
        MEMD_Assert(0,"memd_CacheErase addr error. addr = 0x%x",(UINT32)startFlashAddress);
    }
    pnode = memd_GetNode(region_id);
    do
    {
        if(!pnode)
        {
            pnewnode = MEMD_MALLOC(sizeof(F_NODE));
            if(!pnewnode)
            {
                ret = ERR_DRV_CACHE_MALLOC ;
                MEMD_Assert(0,"erase cache 1 malloc(%d) fail.",sizeof(F_NODE));
                MEMD_EVENT(0xE2000000 |(UINT32)startFlashAddress);
                break;
            }
            pnewnode->type = NODE_TYPE_E;
            pnewnode->next = NULL;
            pnewnode->repeat = 0;
            e_flash = &pnewnode->node.e_flash;
            e_flash->startFlashAddress = (UINT32)startFlashAddress;
            e_flash->endFlashAddress = (UINT32)endFlashAddress;
            e_flash->time = time;
            e_flash->status = FLASH_NODE_STATUS_INIT;
            memd_AddNode(region_id,pprenode,pnewnode);
            if(NULL == pprenode)
            {
                MEMD_EVENT(0xE3000000 | (UINT32)startFlashAddress);
                MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheErase: node_index = fst,startFlashAddress= 0x%x,endFlashAddress = 0x%x,time = 0x%x.",
                                 (UINT32)startFlashAddress,
                                 (UINT32)endFlashAddress,
                                 time);
            }
            else
            {
                MEMD_EVENT(0xE4000000 | (UINT32)startFlashAddress);
                MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheErase: node_index = 0x%x,startFlashAddress= 0x%x,time = 0x%x.",
                                 g_flash_cache.node_cnt,
                                 (UINT32)startFlashAddress,
                                 (UINT32)endFlashAddress,
                                 time);
            }
            break;
        }
        else
        {
            pprenode = pnode;
            pnode = pprenode->next;
        }
    }
    while(1);
    memd_SemRelease();
    return ret;
}



PUBLIC MEMD_ERR_T memd_CacheFlush(FLASH_REGION flash_region)
{
    UINT32 flush_count = 0;
    F_NODE *pnode = NULL;
    W_FLASH *w_flash;
    E_FLASH *e_flash;
    UINT32 cri_status;
    //FLASH_REGION region_id;
    INT32 result;
    INT32 ret = 0;

    cri_status = hal_SysEnterCriticalSection();
    hal_SysExitCriticalSection(cri_status);
    MEMD_Assert(cri_status != 0,"cri_status = %d ",cri_status);
    memd_SemWaitFor();
    if(g_flash_cache.pnode_cur)
    {
        pnode = g_flash_cache.pnode_cur;
        flash_region = g_flash_cache.flash_region_cur;
    }
    else
    {
        pnode = memd_GetNode(flash_region);
        g_flash_cache.pnode_cur = pnode;
        g_flash_cache.flash_region_cur = flash_region;
    }
    if(pnode != NULL)
    {
        if(NODE_TYPE_W == pnode->type)
        {
            w_flash = &pnode->node.w_flash;
            switch(w_flash->status)
            {
                case FLASH_NODE_STATUS_INIT:
                    MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheFlush:write fst,node_cnt = 0x%x,flashAddress= 0x%x,byteSize = 0x%x,buffer = 0x%x.",
                                     g_flash_cache.node_cnt,
                                     w_flash->flashAddress,
                                     w_flash->byteSize,
                                     w_flash->buffer);
                    result = memd_FlashWrite_Respond_Irq((UINT8*)w_flash->flashAddress,
                                                  w_flash->byteSize,
                                                  &w_flash->WrittenByteSize,
                                                  w_flash->buffer);
                    if(result == MEMD_ERR_NO)
                    {
                        UINT32 read_size = 0;
                        ret = memd_FlashRead((UINT8*)w_flash->flashAddress, w_flash->byteSize, &read_size, (UINT8*)g_flash_cache.read_buff);
                        if(ret != 0 ||read_size !=  w_flash->byteSize)
                        {
                            MEMD_EVENT(0xC2000000 | (UINT32)w_flash->flashAddress);
                            MEMD_Assert(0,"memd_FlashRead(0x%x,0x%x) fail.",(UINT32)w_flash->flashAddress,w_flash->byteSize);
                            memd_SemRelease();
                            return ERR_DRV_CACHE_READ_ERROR;
                        }
                        if(memcmp(w_flash->buffer,g_flash_cache.read_buff,w_flash->byteSize) != 0)
                        {
                            if(pnode->repeat >= 3)
                            {
                                MEMD_Assert(0,"write fst check error!buff = 0x%x,read buff = 0x%x,size = %d.",w_flash->buffer,g_flash_cache.read_buff,w_flash->byteSize);
                            }
                            else
                            {
                                MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheFlush:write fst check error.repeat = %d,"
                                                 "region id = %d,cnt = %d,flashAddress= 0x%x,byteSize = 0x%x,buffer = 0x%x.",
                                                 flash_region,
                                                 memd_GetNodeCount(flash_region),
                                                 pnode->repeat,
                                                 w_flash->flashAddress,
                                                 w_flash->byteSize,
                                                 w_flash->buffer);
                                w_flash->status = FLASH_NODE_STATUS_INIT;
                                w_flash->WrittenByteSize = 0;
                                pnode->repeat ++;;
                                memd_SemRelease();
                                return ERR_DRV_CACHE_CHECK_ERROR;
                            }
                        }
                        memd_FreeNode(pnode);
                        //sxr_Sleep(20);
                        MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheFlush:write fst succ.");
                        //sxr_Sleep(20);
                        ret = ERR_DRV_CACHE_NO;
                    }
                    else if(result == FLASH_STATUS_BUSY)
                    {
                        // do nothing.
                        ret = ERR_DRV_CACHE_BUSY;
                    }
                    else if(result == MEMD_ERR_SPI_BUSY)
                    {
                        w_flash->status = FLASH_NODE_STATUS_DOING;
                        ret = ERR_DRV_CACHE_SPI_BUSY;
                    }
                    else
                    {
                        MEMD_Assert(0,"memd_FlashWrite_Respond_Irq,result = 0x%x.",result);
                        ret = ERR_DRV_CACHE_WRITE_ERROR;
                    }
                    break;
                case FLASH_NODE_STATUS_DOING:
                default:
                    break;
            }

        }
        else if(NODE_TYPE_E == pnode->type)
        {
            e_flash = &pnode->node.e_flash;
            //MEMD_EVENT(0xF9000000);
            switch(e_flash->status)
            {
                case FLASH_NODE_STATUS_INIT:
                    //sxr_Sleep(20);
                    MEMD_CACHE_TRACE(BASE_DSM_TS_ID,
                                     "memd_CacheFlush:erase fst, flash_region = %d,node_cnt = 0x%x,startFlashAddress = 0x%x,endFlashAddress = 0x%x,time = 0x%x.",
                                     flash_region,
                                     memd_GetNodeCount(flash_region),
                                     (UINT32)e_flash->startFlashAddress,
                                     (UINT32)e_flash->endFlashAddress,
                                     e_flash->time);
                    //sxr_Sleep(20);
                    result = memd_FlashErase_Respond_Irq((UINT8*)e_flash->startFlashAddress,
                                                   (UINT8*)e_flash->endFlashAddress,
                                                   e_flash->time);
                    if(result == MEMD_ERR_NO)
                    {
                        UINT32 read_size = 0;
                        UINT8* tmp;
                        ret = memd_FlashRead((UINT8*)e_flash->startFlashAddress, 4096, &read_size, (UINT8*)g_flash_cache.read_buff);
                        if(ret != 0)
                        {
                            MEMD_EVENT(0xC2000000 | (UINT32)e_flash->startFlashAddress);
                            MEMD_Assert(0,"memd_FlashRead(0x%x,4096) fail.",(UINT32)e_flash->startFlashAddress);
                            memd_SemRelease();
                            return ERR_DRV_CACHE_READ_ERROR;
                        }
                        tmp = (UINT8*)g_flash_cache.read_buff;
                        for(int i = 0; i < 4096; i++)
                        {
                            if(*tmp != 0xff)
                            {
                                if(pnode->repeat >= 3)
                                {
                                    MEMD_Assert(0,"erase  fst check error!read addr = 0x%x,buff = 0x%x.",e_flash->startFlashAddress,g_flash_cache.read_buff);
                                }
                                else
                                {
                                    e_flash->status = FLASH_NODE_STATUS_INIT;
                                    pnode->repeat ++;
                                    memd_SemRelease();
                                    return ERR_DRV_CACHE_CHECK_ERROR;
                                }
                            }
                        }
                        memd_FreeNode(pnode);
                        MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheFlush:erase fst succ.");
                        //sxr_Sleep(20);
                        ret = ERR_DRV_CACHE_NO;
                    }
                    else if(result == MEMD_ERR_BUSY)
                    {
                        ret = ERR_DRV_CACHE_BUSY;
                    }
                    else
                    {
                        MEMD_Assert(0,"memd_FlashErase_Respond_Irq,result = 0x%x.",result);
                    }
                    break;
                default:
                    MEMD_Assert(0,"e_flash->status error ,status = 0x%x.",e_flash->status);
                    break;
            }
        }
        else
        {
            MEMD_Assert(0,"node type error.type = 0x%x.",pnode->type);
            memd_FreeNode(pnode);
            flush_count ++;
        }
    }
    if(NULL == pnode)
    {
        MEMD_EVENT(0x7F000000 | memd_GetNodeCount(flash_region));
        MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheFlush:end.");
    }
    else
    {
        MEMD_EVENT(0x8F000000 | memd_GetNodeCount(flash_region));
    }
    memd_SemRelease();
    return ret;
}

PUBLIC UINT32 memd_CacheGetNodeCount(FLASH_REGION flash_region)
{
    MEMD_CACHE_TRACE(BASE_DSM_TS_ID,"memd_CacheGetNodeCount:node_cnt = 0x%x.",memd_GetNodeCount(flash_region));
    return memd_GetNodeCount(flash_region);
}

PUBLIC VOID memd_RemoveAllNode(FLASH_REGION flash_region)
{
    F_NODE *pnode = NULL;
    W_FLASH *w_flash;
    E_FLASH *e_flash;

    MEMD_EVENT(0xDD000000 | (UINT32)flash_region);

    memd_SemWaitFor();
    do
    {
        pnode = memd_GetNode(flash_region);
        if(!pnode)
        {
            break;
        }
        if(NODE_TYPE_W == pnode->type)
        {
            w_flash = &pnode->node.w_flash;
            if(FLASH_NODE_STATUS_INIT == w_flash->status)
            {
                memd_FreeNode(pnode);
            }
        }
        else if(NODE_TYPE_E == pnode->type)
        {
            e_flash = &pnode->node.e_flash;
            if(FLASH_NODE_STATUS_INIT == e_flash->status)
            {
                memd_FreeNode(pnode);
            }
        }
        else
        {
            MEMD_Assert(0, "memd_RemoveAllNode(pnode->type = 0x%x)",pnode->type);
        }
    }
    while(pnode);
    memd_SemRelease();
}
#endif
