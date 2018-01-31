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


#if defined(USER_DATA_CACHE_SUPPORT) && !defined(_T_UPGRADE_PROGRAMMER)

#include "dsm_cf.h"
#ifdef _FS_SIMULATOR_
#else
#include "hal_lps.h"
#include "hal_comregs.h"
#include "hal_fint.h"
#include "hal_timers.h"
#include "hal_dma.h"
#include "hal_rda_audio.h"
#include <base_prv.h>
#include "memd_m.h"
#include "memd_cache.h"
#include <drv_flash.h>
#include "ts.h"
#include "csw_csp.h"
#include "cfw_multi_sim.h"
#endif
#include "dsm_dbg.h"
#include "vds_cd.h"
#include "vds_dbg.h"
#include "vds_local.h"
#include "vds_tools.h"
#include "vds_api.h"
#include "vds_cache.h"
#include "vdsp_debug.h"
#include "csw_profile_codes.h"

#ifdef _FS_SIMULATOR_
#define COS_FREE free
#define COS_MALLOC malloc
#define hal_SysExitCriticalSection(x) do{}while(0)
#define hal_EnterCriticalSection() 0
#endif

#ifdef VDS_CACHE_USER_CLUSTER
#define VDS_CACHE_MALLOC COS_MALLOC
#define VDS_CACHE_FREE      COS_FREE
#else
#define VDS_CACHE_MALLOC CSW_Malloc
#define VDS_CACHE_FREE     CSW_Free
#endif

// macro.
// cache node status.
#define VDS_CACHE_STATUS_FREE                             0x00
#define VDS_CACHE_STATUS_WRITE                          0x01
#define VDS_CACHE_STATUS_REVERT                         0x02

// flush status.
#define VDS_FLUSH_STATUS_IDLE                             0x00
#define VDS_FLUSH_STATUS_FLUSH                            0x01
#define VDS_FLUSH_STATUS_FLUSH_ALL                    0x02
#define VDS_FLUSH_STATUS_FLUSH_MEMD                 0x03
#define VDS_CACHE_TRACE                                         13
#define vds_CacheTrace CSW_TRACE

// data structure.

// vds cache node structure.
typedef struct _vds_cache_node
{
    UINT32 partid;
    UINT32 blknr;
    UINT32 status;
    UINT8 pbuff[VDS_BLOCK_SIZE];
    struct _vds_cache_node *next;
} VDS_CACHE_NODE;

// current flush module id.
typedef enum
{
    FLUSH_MODULE_USERDATA, // userdata.
} E_FLUSH_MODULE_ID;

// vds block cache structure].
typedef struct _vds_block_cache
{
    UINT32 flush_status;
    UINT32 blkcnt;
    UINT32 erase_all;
    E_FLUSH_MODULE_ID cur_module;
    HANDLE sem;
#ifdef __VDS_QUICK_FLUSH__
    UINT32 event_count;
#endif
    VDS_CACHE_NODE *pnode;
    UINT8 r_buff[VDS_BLOCK_SIZE];
    UINT32 wskip_num;
} VDS_BLOCK_CACHE;

// global variable.
VDS_BLOCK_CACHE g_cachenode;

#ifdef __VDS_QUICK_FLUSH__
#ifdef BUILD_ON_PLATFORM_TEST
UINT32 g_htstVdsTask;
#else
extern UINT32 g_htstVdsTask;
#endif
#endif
// extern function.
extern void sxs_RegisterFsIdleHookFunc(void (*funcPtr)(void));
extern VOID hal_BootRegisterBeforeMonCallback(VOID (*funcPtr)(VOID));
extern BOOL hal_GetVocDma(void);
extern PUBLIC BOOL flash_spi_busy(void);
/*
extern UINT32 CFW_SetComm(CFW_COMM_MODE nMode, UINT8 nStorageFlag, UINT16 nUTI, CFW_SIM_ID nSimID);
extern UINT32 CFW_GetFreeUTI ( UINT32 nServiceId, UINT8 *pUTI );
*/
// local function.
static VOID vds_CacheSemInit(VOID);
static VOID vds_CacheSemWaitFor(VOID);
static VOID vds_CacheSemRelease(VOID);
INT32 vds_CacheNodeFlush(UINT32 flush_nr);
BOOL vds_SpiIsIdle(void);
#ifdef __VDS_QUICK_FLUSH__

BOOL vds_SpiIsIdle(void)
{
    return memd_SysIsIdle();
}
#else
BOOL vds_SpiIsIdle(void)
{
    UINT32 busy;

    busy = !hal_DmaDone()
           || hal_GetVocDma()
           || flash_spi_busy()
           || !hal_LpsRemainingFramesReady(VDS_CACHE_FLUSH_REMAIN_FRAMES_NUMBER);
    if(busy)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}
#endif


static VOID vds_CacheSemInit(VOID)
{
    g_cachenode.sem = sxr_NewSemaphore(1);
    if((HANDLE)NULL == g_cachenode.sem)
    {
        D( ( DL_WARNING, "VDS_ModuleSemInit failed."));
    }
}


static  VOID vds_CacheSemWaitFor(VOID)
{
    if((HANDLE)NULL != g_cachenode.sem)
    {
        sxr_TakeSemaphore(g_cachenode.sem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleDown failed."));
    }

}


static VOID vds_CacheSemRelease(VOID)
{
    if((HANDLE)NULL != g_cachenode.sem)
    {
        sxr_ReleaseSemaphore(g_cachenode.sem);
    }
    else
    {
        D( ( DL_WARNING, "VDS_ModuleUp failed."));
    }
}

INT32 vds_CacheNodeFlush(UINT32 flush_nr)
{
    UINT32 flush_count = 0;
    VDS_CACHE_NODE *pnode = NULL;
    VDS_CACHE_NODE *ptmpnode = NULL;
    INT32 result;
    INT32 ret = ERR_SUCCESS;

    // Uninit.
    if(FALSE == vds_IsCacheInited() )
    {
        // vds_CacheTrace(BASE_VDS_TS_ID, "vds_CacheNodeFlush: vds cache uninit.");
        DSM_HstSendEvent(0xE10000AE);
        return ret;
    }

    // unallowed flush
    if(FALSE == vds_IsFlushAllowed())
    {
        // vds_CacheTrace(BASE_VDS_TS_ID, "vds_CacheNodeFlush: vds cache flush unalloc.");
        DSM_HstSendEvent(0xE2000000 | g_cachenode.blkcnt);
        return ret;
    }

    if(TRUE == vds_GcIsActive())
    {
        if(g_cachenode.erase_all == 0)
        {
            if(g_cachenode.flush_status == VDS_FLUSH_STATUS_FLUSH_ALL)
            {
                result = vds_GcExt(0);
            }
            else
            {
                result = vds_GcExt(1);
            }
            if(_ERR_VDS_GC_FINISH == result)
            {
                DSM_HstSendEvent(0xE3000000 | g_cachenode.blkcnt);
                // vds_CacheTrace(BASE_DSM_TS_ID, "vds_CacheNodeFlush: vds_GcExt() ok.");
                vds_GcDeactive();
            }
            else
            {
                // donoting.
            }
            return ret;
         }
         else
         {
             DSM_HstSendEvent(0xEa000000 | g_cachenode.blkcnt);
             vds_GcDeactive();
         }
    }
    else
    {
        pnode = g_cachenode.pnode;
        while(pnode != NULL && flush_count < flush_nr)
        {
            if(VDS_CACHE_STATUS_WRITE == pnode->status)
            {
                if(g_cachenode.erase_all == 0)
                {
                    result = VDS_ReadBlock(pnode->partid, pnode->blknr, g_cachenode.r_buff);
                    if(ERR_SUCCESS != result)
                    {
                        ret = ERR_VDS_CACHE_READ_FAILED;
                        break;
                    }
                    if(DSM_MemCmp(g_cachenode.r_buff,pnode->pbuff,VDS_BLOCK_SIZE) == 0)
                    {
                        ptmpnode = pnode;
                        pnode =   pnode->next;
                        VDS_CACHE_FREE(ptmpnode);
                        g_cachenode.pnode = pnode;
                        g_cachenode.blkcnt --;
                        g_cachenode.wskip_num ++;
                        flush_count ++;
                    }
                    else
                    {
                        result = VDS_WriteBlock(pnode->partid, pnode->blknr, pnode->pbuff);
                        if(ERR_SUCCESS == result)
                        {
                            //vds_CacheTrace(BASE_VDS_TS_ID, "vds_CacheNodeFlush: writeblock(0x%x,0x%x) .",pnode->partid,pnode->blknr);
                            //DSM_HstSendEvent(0xE4000000 | (pnode->partid << 16) |pnode->blknr);
                            ptmpnode = pnode;
                            pnode =   pnode->next;
                            VDS_CACHE_FREE(ptmpnode);
                            g_cachenode.pnode = pnode;
                            g_cachenode.blkcnt --;
                            flush_count ++;
                        }
                        else
                        {
                            if(ERR_VDS_SPACE_FULL == result)
                            {
                                //vds_CacheTrace(BASE_VDS_TS_ID, "vds_CacheNodeFlush: writeblock(0x%x,0x%x)  no more free pb,to gc.",pnode->partid,pnode->blknr);
                                // DSM_HstSendEvent(0xE5000000 | (pnode->partid << 16) |pnode->blknr);
                                ret = _ERR_VDS_CACHE_NO_MORE_FREE_PB;
                            }
                            else
                            {
                                //vds_CacheTrace(BASE_VDS_TS_ID, "vds_CacheNodeFlush: writeblock(0x%x,0x%x)  write vds error.result = %d.",pnode->partid,pnode->blknr,result);
                                // DSM_HstSendEvent(0xE6000000 | (pnode->partid << 16) |pnode->blknr);
                                ret = ERR_VDS_CACHE_WRITE_FAILED;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    ptmpnode = pnode;
                    pnode =   pnode->next;
                    VDS_CACHE_FREE(ptmpnode);
                    g_cachenode.pnode = pnode;
                    g_cachenode.blkcnt --;
                    flush_count ++;
                }
            }
            else
            {
                // vds_CacheTrace(BASE_VDS_TS_ID, "vds_CacheNodeFlush: free unkown statues block(0x%x,0x%x) .",pnode->partid,pnode->blknr);
                DSM_HstSendEvent(0xE7000000 | (pnode->partid << 16) | pnode->blknr);
                ptmpnode = pnode;
                pnode =   pnode->next;
                VDS_CACHE_FREE(ptmpnode);
                g_cachenode.pnode = pnode;
                g_cachenode.blkcnt --;
                flush_count ++;
            }

        }
        if(NULL == g_cachenode.pnode)
        {
            ret = ERR_SUCCESS;
            DSM_HstSendEvent(0xE8000000 | g_cachenode.blkcnt);
        }
        else
        {
            DSM_HstSendEvent(0xE9000000 | g_cachenode.blkcnt);
        }

    }
    return ret;
}
// api fuction body.

INT32 VDS_CacheInit(VOID)
{
    g_cachenode.flush_status = VDS_FLUSH_STATUS_IDLE;
    g_cachenode.blkcnt = 0;
    g_cachenode.cur_module = FLUSH_MODULE_USERDATA;
    g_cachenode.sem = NULL;
    g_cachenode.erase_all = 0;
#ifdef __VDS_QUICK_FLUSH__
    g_cachenode.event_count = 0;
#endif
    g_cachenode.pnode = NULL;
    g_cachenode.wskip_num = 0;
    vds_CacheSemInit();
    vds_InitCache();
#ifdef __VDS_QUICK_FLUSH__
//   vds_SetFlushAllowed();
    //DRV_SuspendEnable();
#else
    vds_SetFlushUnallowed();
#endif
    DSM_HstSendEvent(0xA1000000);
    return ERR_SUCCESS;
}

VOID VDS_CacheFlushAllowed(VOID)
{
    vds_SetFlushAllowed();
#ifdef __VDS_QUICK_FLUSH__
    DRV_SuspendEnable();
#else
    sxs_RegisterFsIdleHookFunc(VDS_CacheFlush);
    hal_BootRegisterBeforeMonCallback(VDS_CacheFlushAll);
#endif
    DSM_HstSendEvent(0xA2000000);
}

VOID VDS_CacheFlushUnAllowed(VOID)
{
    vds_SetFlushUnallowed();
}

BOOL VDS_IsCacheFlushAllowed(VOID)
{
    return vds_IsFlushAllowed();
}


//@iKeepTime disable flush time in tick.
VOID VDS_CacheFlushDisable(UINT32 iKeepTime)
{
    vds_DisableFlush(iKeepTime);
}

VOID VDS_CacheFlushEnable(VOID)
{
    vds_EnableFlush();
}


INT32 VDS_CacheWriteBlock( UINT32 iPartId, UINT32 iBlkNr, UINT8*pBuff )
{
    VDS_CACHE_NODE *pnode = NULL;
    VDS_CACHE_NODE *pprenode = NULL;
    VDS_CACHE_NODE *pnewnode = NULL;
    INT32 ret = ERR_SUCCESS;
    UINT32 cri_status = 0;

    if(FALSE == vds_IsCacheInited())
    {
        // vds_CacheTrace(VDS_CACHE_TRACE, "VDS_CacheWriteBlock: vds cache uninit,write to flash.");
        DSM_HstSendEvent(0xB1000000 | (iPartId << 16) | iBlkNr );
        return VDS_WriteBlock( iPartId, iBlkNr, pBuff );
    }

    vds_CacheSemWaitFor();
    pnode = g_cachenode.pnode;
    pprenode = pnode;
    do
    {
        if(NULL == pnode)
        {
            pnewnode = VDS_CACHE_MALLOC(sizeof(VDS_CACHE_NODE));
            if(NULL == pnewnode)
            {
                // vds_CacheTrace(VDS_CACHE_TRACE, "VDS_CacheWriteBlock: malloc failed. partid = 0x%x,blknr = 0x%x,nodenr = 0x%x.",iPartId,iBlkNr,g_cachenode.blkcnt);
                DSM_HstSendEvent(0xB2000000 | (iPartId << 16) | iBlkNr );
                ret = ERR_VDS_CACHE_VDS_MALLOC_FIALED;
                DSM_ASSERT(0, "VDS_CacheWriteBlock: 1.malloc(0x%x) failed.", (sizeof(VDS_CACHE_NODE)));
                break;
            }
            pnewnode->partid = iPartId;
            pnewnode->blknr = iBlkNr;
            pnewnode->status = VDS_CACHE_STATUS_WRITE;
            DSM_MemCpy(pnewnode->pbuff, pBuff, VDS_BLOCK_SIZE);
            pnewnode->next = NULL;
            cri_status = hal_EnterCriticalSection();
            if(NULL == pprenode)
            {
                // vds_CacheTrace(VDS_CACHE_TRACE, "VDS_CacheWriteBlock: add newnode(0x%x,0x%x) to header.",iPartId,iBlkNr);
                g_cachenode.pnode = pnewnode;
                DSM_HstSendEvent(0xB3000000 | (iPartId << 16) | iBlkNr );
            }
            else
            {
                //  vds_CacheTrace(VDS_CACHE_TRACE, "VDS_CacheWriteBlock: add newnode(0x%x,0x%x) to tail.",iPartId,iBlkNr);
                DSM_HstSendEvent(0xB4000000 | (iPartId << 16) | iBlkNr );
                pprenode->next = pnewnode;
            }
            g_cachenode.blkcnt ++;
            hal_SysExitCriticalSection(cri_status);
            DSM_HstSendEvent(0xB5000000 | (iPartId << 16) | iBlkNr );

            break;
        }
        else if(pnode->partid == iPartId && pnode->blknr == iBlkNr)
        {
            cri_status = hal_EnterCriticalSection();
            pnode->status = VDS_CACHE_STATUS_WRITE;
            DSM_MemCpy(pnode->pbuff, pBuff, VDS_BLOCK_SIZE);
            hal_SysExitCriticalSection(cri_status);
            DSM_HstSendEvent(0xB6000000 | (iPartId << 16) | iBlkNr);
            break;
        }
        else
        {
            pprenode = pnode;
            pnode = pprenode->next;
        }
    }
    while(1);
#ifdef __VDS_QUICK_FLUSH__
    VDS_CacheSendFlushEvent();
#endif
    vds_CacheSemRelease();
    return ret;
}


INT32 VDS_CacheReadBlock( UINT32 iPartId, UINT32 iBlkNr, UINT8* pBuff )
{
    UINT32 nodenr = 0;
    VDS_CACHE_NODE *pnode = NULL;
    VDS_CACHE_NODE *pprenode = NULL;
    //BOOL bFine = FALSE;
    INT32 ret = ERR_SUCCESS;

    if(FALSE == vds_IsCacheInited())
    {
        ret = VDS_ReadBlock( iPartId, iBlkNr, pBuff );
        DSM_HstSendEvent(0xC1000000 | (iPartId << 16) | iBlkNr);
        return ret;
    }

    if(g_cachenode.pnode == NULL)
    {
        ret = VDS_ReadBlock( iPartId, iBlkNr, pBuff );
        return ret;
    }

    vds_CacheSemWaitFor();

    pnode = g_cachenode.pnode;
    while(pnode != NULL)
    {
        if(pnode->partid == iPartId && pnode->blknr == iBlkNr)
        {
            DSM_MemCpy(pBuff, pnode->pbuff, VDS_BLOCK_SIZE);
            //vds_CacheTrace(VDS_CACHE_TRACE, "VDS_CacheReadBlock: memcpy(0x%x,0x%x).",iPartId,iBlkNr);
            break;
        }
        pprenode = pnode;
        pnode = pprenode->next;
        nodenr ++;
    }
    if(NULL == pnode)
    {
        //vds_CacheTrace(VDS_CACHE_TRACE, "VDS_CacheReadBlock:can't find node,VDS_ReadBlock(0x%x,0x%x)",iPartId,iBlkNr);
        ret = VDS_ReadBlock(iPartId, iBlkNr, pBuff);
    }

    vds_CacheSemRelease();
    return ret;
}


INT32 VDS_CacheRevertBlock( UINT32 iPartId, UINT32 iBlkNr)
{
    iPartId = iPartId;
    iBlkNr = iBlkNr;
    return ERR_SUCCESS;
}


VOID VDS_CacheSetEraseAll(VOID)
{
    vds_CacheSemWaitFor();
    DRV_RemoveAllCacheNode();
    g_cachenode.erase_all = 1;
    vds_CacheSemRelease();
}



#ifdef __VDS_QUICK_FLUSH__
VOID VDS_CacheFlush(VOID)
{
    UINT8 is_suspend;

    if (0)
    {
        ; // for syntax
    }
#if defined(REDUNDANT_DATA_REGION)
    else if(memd_CacheGetNodeCount(FLASH_REGION_REMAIN) > 0)
    {
        memd_CacheFlush(FLASH_REGION_REMAIN);
    }
#endif
    else if(memd_CacheGetNodeCount(FLASH_REGION_CALIB) > 0)
    {
        memd_CacheFlush(FLASH_REGION_CALIB);
    }
    else if(DRV_CacheGetNodeCount() > 0)
    {
        DRV_CacheFlush();
    }
    else
    {
        vds_CacheSemWaitFor();
        is_suspend = DRV_SuspendIsEnable();
        if(g_cachenode.flush_status != VDS_FLUSH_STATUS_FLUSH_ALL)
        {
            g_cachenode.flush_status = VDS_FLUSH_STATUS_FLUSH;
        }
        if(NULL != g_cachenode.pnode)
        {
            vds_CacheNodeFlush(1);
        }
        if(g_cachenode.flush_status != VDS_FLUSH_STATUS_FLUSH_ALL)
        {
            g_cachenode.flush_status = VDS_FLUSH_STATUS_IDLE;
        }
        vds_CacheSemRelease();
    }
}


VOID VDS_CacheFlushAll(VOID)
{
    UINT32 flush_star_time;
    UINT32 cri_status = 0;
    UINT32 cache_blk_cnt;
    UINT8 is_enter_cri = 0;

    DSM_HstSendEvent(0xF1000000 | g_cachenode.blkcnt);
    if(FALSE == vds_IsCacheInited() )
    {
        DSM_HstSendEvent(0xF100001E);
        return ;
    }
    vds_SetFlushAllowed();
    DRV_SuspendEnable();
    memd_SetFramsIrqIgnore(TRUE);
    cache_blk_cnt = VDS_CacheGetBlkCnt();
    flush_star_time = hal_TimGetUpTime();
    while(
        VDS_CacheGetBlkCnt() > 0
        || DRV_CacheGetNodeCount() > 0
#if defined(REDUNDANT_DATA_REGION)
        || memd_CacheGetNodeCount(FLASH_REGION_REMAIN) > 0
#endif
        || memd_CacheGetNodeCount(FLASH_REGION_CALIB) > 0
    )
    {
        if(is_enter_cri != 0)
        {
            VDS_CacheFlush();
            DSM_HstSendEvent(0xF2000000);
        }
        else
        {
            if(g_cachenode.flush_status == VDS_FLUSH_STATUS_IDLE
                    && DRV_CacheGetNodeCount() == 0
#if defined(REDUNDANT_DATA_REGION)
                    && memd_CacheGetNodeCount(FLASH_REGION_REMAIN) == 0
#endif
                    && memd_CacheGetNodeCount(FLASH_REGION_CALIB) == 0
              )
            {
                DSM_HstSendEvent(0xF3000000);
                cri_status = hal_EnterCriticalSection();
                if(vds_SpiIsIdle())
                {
                    hal_FintIrqSetMask(FALSE);
                    hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_0));
                    hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_1, hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_1));
                    hal_TimWatchDogClose();
                    hal_AudForcePowerDown();
                    g_cachenode.flush_status = VDS_FLUSH_STATUS_FLUSH_ALL;
                    DRV_SuspendDisable();
                    is_enter_cri = 1;
                }
                else
                {
                    hal_SysExitCriticalSection(cri_status);
                    sxr_Sleep(16);
                }
            }
            else
            {
                sxr_Sleep(16);
            }
        }
        if(hal_TimGetUpTime() - flush_star_time > 16384*80)
        {
            DSM_HstSendEvent(0xF200002E);
            DSM_ASSERT(0,"time out!!!");
            return;
        }
    }
    DSM_HstSendEvent(0xF8000000);
    DSM_HstSendEvent(cache_blk_cnt);
}



#else

VOID VDS_CacheFlush(VOID)
{
    UINT32 cri_status = 0;


    if(g_cachenode.flush_status != VDS_FLUSH_STATUS_FLUSH_ALL)
    {
        g_cachenode.flush_status = VDS_FLUSH_STATUS_FLUSH;
    }
    if(NULL == g_cachenode.pnode)
    {
        return;
    }

    cri_status = hal_EnterCriticalSection();
    if(vds_SpiIsIdle())
    {
        VDS_PROFILE_FUNCTION_ENTER(VDS_CacheFlush);
        if(NULL != g_cachenode.pnode)
        {
            vds_CacheNodeFlush(VDS_CACHE_FLUSH_NODE_NUMBER);
        }
        VDS_PROFILE_FUNCTION_EXIT(VDS_CacheFlush);
    }
    hal_SysExitCriticalSection(cri_status);
    if(g_cachenode.flush_status != VDS_FLUSH_STATUS_FLUSH_ALL)
    {
        g_cachenode.flush_status = VDS_FLUSH_STATUS_IDLE;
    }
    return;
}

VOID VDS_CacheFlushAll(VOID)
{
    UINT32 flush_star_time;
    UINT32 cri_status = 0;
    UINT32 cache_blk_cnt;

    DSM_HstSendEvent(0xF1000000 | g_cachenode.blkcnt);
    if(FALSE == vds_IsCacheInited() )
    {
        DSM_HstSendEvent(0xF100001E);
        return ;
    }
    hal_FintIrqSetMask(FALSE);
    hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_0));
    hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_1, hal_ComregsGetMask(HAL_COMREGS_XCPU_IRQ_SLOT_1));
    hal_TimWatchDogClose();
    hal_AudForcePowerDown();
    vds_SetFlushAllowed();
    cache_blk_cnt = VDS_CacheGetBlkCnt();
    flush_star_time = hal_TimGetUpTime();
    while(VDS_CacheGetBlkCnt() > 0)
    {
        if(cri_status != 0)
        {
            VDS_CacheFlush();
            DSM_HstSendEvent(0xF2000000);
        }
        else
        {
            if(g_cachenode.flush_status == VDS_FLUSH_STATUS_IDLE)
            {
                DSM_HstSendEvent(0xF3000000);
                g_cachenode.flush_status = VDS_FLUSH_STATUS_FLUSH_ALL;
                cri_status = hal_EnterCriticalSection();
            }
            else
            {
                // DSM_HstSendEvent(0xF4000000);
                // sxr_Sleep(16);
            }
        }
        if(hal_TimGetUpTime() - flush_star_time > 16384*50)
        {
            DSM_HstSendEvent(0xF200002E);
            DSM_ASSERT(0,"time out!!!");
            return;
        }
    }
    DSM_HstSendEvent(0xF8000000);
    DSM_HstSendEvent(cache_blk_cnt);
}
#endif


BOOL VDS_CacheNodeIsLess(VOID)
{
    if(g_cachenode.blkcnt >= VDS_CACHE_NODE_NUMBER_MAX)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}


UINT32 VDS_CacheGetBlkCnt(VOID)
{
    return g_cachenode.blkcnt;
}

#ifdef __VDS_QUICK_FLUSH__
BOOL VDS_CacheSyncFlush(UINT32 block_nr)
{
    while(g_cachenode.blkcnt > 0)
    {
        VDS_CacheSendFlushEvent();
    }
    return TRUE;
}


BOOL vds_CacheNeedFlush(VOID)
{
    if(!vds_IsFlushAllowed() || !vds_IsCacheInited())
    {
        return FALSE;
    }
    if(g_cachenode.pnode)
    {
        return TRUE;
    }
    return FALSE;
}


VOID VDS_CacheTaskEntry(void *pData)
{

    COS_EVENT ev;
    TASK_HANDLE *pHTask = NULL;
    pHTask = (TASK_HANDLE *)g_htstVdsTask;
    while (1)
    {
        COS_WaitEvent(pHTask, &ev, COS_WAIT_FOREVER);
        /*
        if(g_cachenode.flush_all == 1)
        {
            if(g_cachenode.event_count != 0)
            {
                g_cachenode.event_count--;
            }
            //sxr_Sleep(16);
            continue;
        }
        */

        if(VDS_CacheGetBlkCnt() == 0
                && DRV_CacheGetNodeCount() == 0
#if defined(REDUNDANT_DATA_REGION)
                && memd_CacheGetNodeCount(FLASH_REGION_REMAIN) == 0
#endif
                && memd_CacheGetNodeCount(FLASH_REGION_CALIB) == 0)
        {
            if(g_cachenode.event_count != 0)
            {
                g_cachenode.event_count--;
            }
            sxr_Sleep(32);
            continue;
        }
        else
        {
            if(vds_SpiIsIdle()
                    && vds_IsFlushAllowed()
                )
            {
#ifdef LTE_NBIOT_SUPPORT
                extern u8 ready_for_cfun0;
                vds_CacheTrace(BASE_VDS_TS_ID, "vds ready_for_cfun0:%d", ready_for_cfun0);

                if (ready_for_cfun0 == 1)
                {
                    UINT32 status = hal_SysEnterCriticalSection();
                    VDS_CacheFlush();
                    hal_SysExitCriticalSection(status);
                }
                else
#endif
                VDS_CacheFlush();
                sxr_Sleep(16);
            }
            else
            {
                if(!vds_IsFlushAllowed())
                {
                    sxr_Sleep(16384);
                }
                else
                {
                    sxr_Sleep(16);
                }
            }
        }

        if(g_cachenode.event_count != 0)
        {
            g_cachenode.event_count--;
        }

        if(VDS_CacheGetBlkCnt() > 0
                || DRV_CacheGetNodeCount() > 0
#if defined(REDUNDANT_DATA_REGION)
                || memd_CacheGetNodeCount(FLASH_REGION_REMAIN) > 0
#endif
                || memd_CacheGetNodeCount(FLASH_REGION_CALIB) > 0
            )
        {
            VDS_CacheSendFlushEvent();
        }
    }
    return;
}


VOID VDS_CacheSendFlushEvent(VOID)
{
    COS_EVENT evt = {1, 0, 0, 0};
    TASK_HANDLE *pHTask;

    pHTask = (TASK_HANDLE *)g_htstVdsTask;
    if(pHTask)
    {
        if(g_cachenode.event_count > 3)
        {
            return;
        }
        COS_SendEvent(pHTask, &evt, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
        g_cachenode.event_count ++;
    }
    else
    {

    }
}

#endif
#endif

