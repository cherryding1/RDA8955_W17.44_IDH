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

#include "sxs_srl.h"
#include "sxs_type.h"
#include "sxs_lib.h"
#include "sxr_ops.h"
#include "errorcode.h"
#include "sxr_mem.hp"
//#define HAVE_NO_TS_IF
#include "ts.h"
#include "cswtype.h"
#include "csw.h"
#include "csw_config.h"
#include "sxs_io.h"
#include "sxr_mem.h"
#include "sxr_usrmem.h"
#include "hal_host.h"

extern MMI_Default_Value g_MMI_Default_Value;
//extern INT32 CSW_TRACE(UINT32 id, CONST INT8 *fmt, ...);
extern PUBLIC CONST CSW_CONFIG_T *tgt_GetCswConfig(VOID);

//#define     CSW_HEAP_SIZE_MALLOC      (180*1024)

#define CSW_HEAP_INDEX 2
#define COS_HEAP_INDEX 3
#define JAVA_HEAP_INDEX 4

#if defined(MEM_ULC_3216_SAVE_SRAM) && !defined(CFW_GPRS_SUPPORT) && defined(CSW_COS_USING_ONE_HEAP)
#define VDS_CACHE_CLUSTER_COUNT_16 60
#else
#define VDS_CACHE_CLUSTER_COUNT_16 150
#endif
#define VDS_CACHE_CLUSTER_COUNT_32 200
#ifdef __MMI_SLIM_JAVA_6464__
#define VDS_CACHE_CLUSTER_COUNT_64 300
#else
#define VDS_CACHE_CLUSTER_COUNT_64 400
#endif

PVOID g_CosBaseAdd = NULL;
PVOID g_CswBaseAdd = NULL;

UINT32 g_CosHeapSize = 0;
UINT32 g_CswHeapSize = 0;

UINT32 g_CswMallocNum = 0;
UINT32 g_CosMallocNum = 0;
UINT32 g_MDIMallocNum = 0;

#ifdef _CUSTMEM_SUPPORT_
#define CUST_HEAP_INDEX 5
PVOID g_CustBaseAdd = NULL;
UINT32 g_CustHeapSize = 0;
#endif

VOID COS_UpdateMemCallAdd(VOID *pMemeAddr, UINT32 nCallAdd)
{
    sxr_UpMemCallerAdd(pMemeAddr, nCallAdd);
}

PVOID Stack_MsgMalloc(UINT32 nSize)
{
    PVOID pointer = 0x00;
    UINT32 nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    pointer = _sxr_HMalloc(nSize, 0xFF);
    if (pointer == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("Stack_MsgMalloc failed size 0x%x\n", 0x08000035), nSize);
    }
    else
        COS_UpdateMemCallAdd(pointer, nCallerAdd);

    return pointer;
}

PVOID CSW_Malloc(UINT32 nSize)
{
    PVOID pointer = NULL;
    UINT32 nCallerAdd;
    COS_GET_RA(&nCallerAdd);

#ifdef CSW_COS_USING_ONE_HEAP
    pointer = COS_Malloc(nSize, COS_MMI_HEAP);
    g_CswMallocNum++;
#else
    if (nSize >= g_CswHeapSize)
        return NULL;

    g_CswMallocNum++;
    pointer = _sxr_HMalloc(nSize, CSW_HEAP_INDEX);
#endif
    if (pointer == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("CSW_Malloc failed size 0x%x\n", 0x08000036), nSize);
    }
    else
        COS_UpdateMemCallAdd(pointer, nCallerAdd);

    return pointer;
}

BOOL CSW_Free(PVOID pMemBlock)
{
#ifdef CSW_COS_USING_ONE_HEAP
    g_CswMallocNum--;
    return COS_Free(pMemBlock);
#else
    if ((((UINT32)(UINT32 *)pMemBlock) >= (UINT32)g_CswBaseAdd) &&
        (((UINT32)(UINT32 *)pMemBlock) < (UINT32)(g_CswBaseAdd + g_CswHeapSize)))
    {
        g_CswMallocNum--;

        // CSW_TRACE(_CSW|TLEVEL(BASE_COS_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("CSW_Free pMemBlock = %x g_CswMallocNum %d\n",0x08000037)), pMemBlock, g_CswMallocNum );
        sxr_Free(pMemBlock);
        return TRUE;
    }
    //There are some memory malloc from COS, and CSW layer release. So add the following branch.

    if ((((UINT32)(UINT32 *)pMemBlock) >= (UINT32)g_CosBaseAdd) &&
        (((UINT32)(UINT32 *)pMemBlock) < (UINT32)(g_CosBaseAdd + g_CosHeapSize)))
    {
        g_CosMallocNum--;
        sxr_UsrFree(pMemBlock);
        pMemBlock = NULL;
        return TRUE;
    }

    return FALSE;
#endif
}

BOOL Stack_MsgFree(PVOID pMemBlock)
{
    sxr_Free(pMemBlock);
    return TRUE;
}

// =============================================================================
// COS_Malloc
// -----------------------------------------------------------------------------
/// For application malloc
// =============================================================================
#ifdef __USER_CLUSTER_TEST_FUNC__
extern void sxr_userCluster_check(void);
#endif
PVOID COS_Malloc(UINT32 nSize, COS_HEAP_ID nHeapID)
{
    UINT32 nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    if ((0x02 == nHeapID) || (0x01 == nHeapID))
    {
        nHeapID = COS_MMI_HEAP;
    }
    switch (nHeapID)
    {
    case COS_MMI_HEAP:
    {
        if (nSize >= g_CosHeapSize)
            return NULL;

        PVOID p = sxr_UsrMalloc(nSize);

        if (!p)
        {
            p = _sxr_HMalloc(nSize, COS_HEAP_INDEX);
        }
#ifdef __USER_CLUSTER_TEST_FUNC__
        sxr_userCluster_check();
#endif
        if (p == NULL)
        {
            CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("COS_Malloc Malloc failed, And the size is 0x%x\n", 0x0800003a), nSize);
        }
        else
            COS_UpdateMemCallAdd(p, nCallerAdd);

        return p;
    }
    break;

    default:
        return NULL;
    }
}

PVOID COS_Realloc(VOID *ptr, UINT32 nSize)
{
    UINT32 nCallerAdd;
    PVOID *ret;
    COS_GET_RA(&nCallerAdd);

    if (ptr == NULL)
        ret = COS_Malloc(nSize, COS_MMI_HEAP);
    else if (nSize == 0)
    {
        COS_Free(ptr);
        ret = NULL;
    }
    else
    {
        ret = COS_Malloc(nSize, COS_MMI_HEAP);
        if (ret != NULL)
        {
            memcpy(ret, ptr, nSize);
            COS_Free(ptr);
        }
    }

    return ret;
}

BOOL COS_Free(PVOID pMemBlock)
{
    if ((((UINT32)(UINT32 *)pMemBlock) >= (UINT32)g_CosBaseAdd) &&
        (((UINT32)(UINT32 *)pMemBlock) < (UINT32)(g_CosBaseAdd + g_CosHeapSize)))
    {
        g_CosMallocNum--;
        sxr_UsrFree(pMemBlock);
        pMemBlock = NULL;
        return TRUE;
    }
    if ((((UINT32)(UINT32 *)pMemBlock) >= (UINT32)g_CswBaseAdd) &&
        (((UINT32)(UINT32 *)pMemBlock) < (UINT32)(g_CswBaseAdd + g_CswHeapSize)))
    {
        g_CswMallocNum--;
        sxr_Free(pMemBlock);
        pMemBlock = NULL;
        return TRUE;
    }

    return FALSE;
}

VOID csw_MemIint(VOID)
{
    CONST CSW_CONFIG_T *cswConfig;
    UINT32 vds_clus_cnt = 0;
    UINT32 spare_clus_cnt;
    UINT32 cur_clus_cnt;

    cswConfig = tgt_GetCswConfig();

#ifdef VDS_CACHE_USER_CLUSTER
    if (cswConfig->cswHeapSize + cswConfig->cosHeapSize <= 1400 * 1024)
    {
        vds_clus_cnt = VDS_CACHE_CLUSTER_COUNT_16;
    }
    else if (cswConfig->cswHeapSize + cswConfig->cosHeapSize > 1400 * 1024 &&
             cswConfig->cswHeapSize + cswConfig->cosHeapSize <= 2500 * 1024)
    {
        vds_clus_cnt = VDS_CACHE_CLUSTER_COUNT_32;
    }
    else
    {
        vds_clus_cnt = VDS_CACHE_CLUSTER_COUNT_64;
    }
#else
    vds_clus_cnt = 0;
#endif

#ifdef CSW_COS_USING_ONE_HEAP
    g_CswHeapSize = 0;
    g_CosHeapSize = cswConfig->cosHeapSize + cswConfig->cswHeapSize;
    g_CswBaseAdd = NULL;
    g_CosBaseAdd = sxr_HMalloc(g_CosHeapSize);
#else
    g_CswHeapSize = cswConfig->cswHeapSize;
    g_CosHeapSize = cswConfig->cosHeapSize;

    g_CswBaseAdd = sxr_HMalloc(g_CswHeapSize);
    g_CosBaseAdd = sxr_HMalloc(g_CosHeapSize);
#endif

#if 0 //def MEM_ULC_3216
    if (( ((UINT32)&_heap_size) - (g_CswHeapSize + g_CosHeapSize) ) < 350 * 1024 )
    {
        /*
        hal_HstSendEvent(0x99998888);
        hal_HstSendEvent(((UINT32)&_heap_size));
        hal_HstSendEvent(g_CswHeapSize);
        hal_HstSendEvent(g_CosHeapSize);
        */
        SXS_RAISE ((_SXR | TABORT | TDB, TSTR("_heap_size -(g_CswHeapSize + g_CosHeapSize) <350.\n", 0x06bc3333)));
    }
#endif

    CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("g_CswHeapSize 0x%x\n", 0x0800003d), g_CswHeapSize);
    CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("g_CosHeapSize 0x%x\n", 0x0800003e), g_CosHeapSize);

#ifndef CSW_COS_USING_ONE_HEAP
    _sxr_NewHeap(g_CswBaseAdd, g_CswHeapSize, CSW_HEAP_INDEX);
#endif
    _sxr_NewHeap(g_CosBaseAdd, g_CosHeapSize, COS_HEAP_INDEX);

#if defined(MEM_ULC_3216_SAVE_SRAM) && !defined(CFW_GPRS_SUPPORT) && defined(CSW_COS_USING_ONE_HEAP)
    sxr_NewUsrCluster(12, 250, COS_HEAP_INDEX); // 36*250
    sxr_NewUsrCluster(24, 150, COS_HEAP_INDEX); // 48*150
    sxr_NewUsrCluster(28, 100, COS_HEAP_INDEX); // 52*100
    sxr_NewUsrCluster(32, 150, COS_HEAP_INDEX); // 56*150
    //  ==29800
    sxr_NewUsrCluster(52, 200, COS_HEAP_INDEX);  // 76*200
    sxr_NewUsrCluster(100, 100, COS_HEAP_INDEX); // 124*100
    sxr_NewUsrCluster(108, 25, COS_HEAP_INDEX);  // 132 *25
    sxr_NewUsrCluster(156, 50, COS_HEAP_INDEX);  //  180*50
    sxr_NewUsrCluster(200, 20, COS_HEAP_INDEX);  // 224*20
    // ==44380
    spare_clus_cnt = vds_clus_cnt;
    while (spare_clus_cnt > 0)
    {
        cur_clus_cnt = spare_clus_cnt > 100 ? 100 : spare_clus_cnt;
        sxr_NewUsrCluster(528, cur_clus_cnt, COS_HEAP_INDEX); // 552*60 == 33120
        spare_clus_cnt -= cur_clus_cnt;
    }

    sxr_NewUsrCluster(700, 20, COS_HEAP_INDEX);  // 724 * 20
    sxr_NewUsrCluster(1024, 20, COS_HEAP_INDEX); // 1048 * 20
    sxr_NewUsrCluster(1500, 15, COS_HEAP_INDEX); // 1524 * 15
    sxr_NewUsrCluster(2048, 10, COS_HEAP_INDEX); // 2072 * 10
    sxr_NewUsrCluster(2500, 6, COS_HEAP_INDEX);  // 2524 * 6
    sxr_NewUsrCluster(4200, 10, COS_HEAP_INDEX); // 4224 * 10
//  ==136404        // Total 243704
#elif defined(CSW_COS_USING_ONE_HEAP)
    sxr_NewUsrCluster(12, 250, COS_HEAP_INDEX);  // 36*250
    sxr_NewUsrCluster(12, 250, COS_HEAP_INDEX);  // 36*250
    sxr_NewUsrCluster(12, 250, COS_HEAP_INDEX);  // 36*250
    sxr_NewUsrCluster(24, 250, COS_HEAP_INDEX);  // 48*250
    sxr_NewUsrCluster(24, 250, COS_HEAP_INDEX);  // 48*250
    sxr_NewUsrCluster(48, 250, COS_HEAP_INDEX);  // 72*250
    sxr_NewUsrCluster(88, 250, COS_HEAP_INDEX);  // 112*250
    sxr_NewUsrCluster(128, 250, COS_HEAP_INDEX); // 152*250
    sxr_NewUsrCluster(200, 100, COS_HEAP_INDEX); // 224*100

    spare_clus_cnt = vds_clus_cnt;
    while (spare_clus_cnt > 0)
    {
        cur_clus_cnt = spare_clus_cnt > 100 ? 100 : spare_clus_cnt;
        sxr_NewUsrCluster(528, cur_clus_cnt, COS_HEAP_INDEX); // 552*200 == 110400
        spare_clus_cnt -= cur_clus_cnt;
    }
    sxr_NewUsrCluster(1024, 50, COS_HEAP_INDEX); // 1048 * 50
//Total 320200
#else
    sxr_NewUsrCluster(10, 250, COS_HEAP_INDEX); // 36*250
    sxr_NewUsrCluster(24, 250, COS_HEAP_INDEX); // 48*250
    sxr_NewUsrCluster(28, 250, COS_HEAP_INDEX); // 52*250
    sxr_NewUsrCluster(32, 250, COS_HEAP_INDEX); // 56*250

    sxr_NewUsrCluster(52, 250, COS_HEAP_INDEX);  // 76*250
    sxr_NewUsrCluster(100, 200, COS_HEAP_INDEX); // 124*200
    sxr_NewUsrCluster(156, 200, COS_HEAP_INDEX); //  180*200
    sxr_NewUsrCluster(200, 20, COS_HEAP_INDEX);  // 224*20

    spare_clus_cnt = vds_clus_cnt;
    while (spare_clus_cnt > 0)
    {
        cur_clus_cnt = spare_clus_cnt > 100 ? 100 : spare_clus_cnt;
        sxr_NewUsrCluster(528, cur_clus_cnt, COS_HEAP_INDEX); // 552*200 == 110400
        spare_clus_cnt -= cur_clus_cnt;
    }
    sxr_NewUsrCluster(700, 50, COS_HEAP_INDEX);  // 724*50
    sxr_NewUsrCluster(1024, 50, COS_HEAP_INDEX); // 1048 * 50
//Total 331280
#endif
}

#ifdef _CUSTMEM_SUPPORT_
PVOID Cust_MemIint(UINT32 heapSize)
{

    g_CustBaseAdd = COS_Malloc(heapSize, COS_HEAP_INDEX);

    CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("CustHeapSize 0x%x\n", 0x0800003f), heapSize);

    if (g_CustBaseAdd == NULL)
        return NULL;

    _sxr_NewHeap(g_CustBaseAdd, heapSize, CUST_HEAP_INDEX);
    g_CustHeapSize = heapSize;

    return g_CustBaseAdd;
}

VOID Cust_MemDeinit()
{
    COS_Free(g_CustBaseAdd);
    g_CustHeapSize = 0;
    g_CustBaseAdd = NULL;
}

PVOID Cust_Malloc(UINT32 nSize)
{
    PVOID pointer = NULL;
    UINT32 nCallerAdd;
    COS_GET_RA(&nCallerAdd);

    if (nSize >= g_CustHeapSize)
        return NULL;
    pointer = _sxr_HMalloc(nSize, CUST_HEAP_INDEX);
    if (pointer == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(BASE_BAL_TS_ID) | TDB | TNB_ARG(1), TSTR("CSW_Malloc failed size 0x%x\n", 0x08000040), nSize);
    }
    else
        COS_UpdateMemCallAdd(pointer, nCallerAdd);

    return pointer;
}

BOOL Cust_Free(PVOID pMemBlock)
{
    if ((((UINT32)(UINT32 *)pMemBlock) >= (UINT32)g_CustHeapSize) &&
        (((UINT32)(UINT32 *)pMemBlock) < (UINT32)(g_CustHeapSize + g_CustHeapSize)))
    {

        // CSW_TRACE(_CSW|TLEVEL(BASE_COS_TS_ID)|TDB|TNB_ARG(2), TSTXT(TSTR("CSW_Free pMemBlock = %x g_CswMallocNum %d\n",0x08000041)), pMemBlock, g_CswMallocNum );
        sxr_Free(pMemBlock);
        return TRUE;
    }
    return FALSE;
}
#endif

// =============================================================================
// -----------------------------------------------------------------------------
/// This function get the used heap size
///
/// @param pHeapStatus Heap usage status(Total and Used size)
/// @param Idx heap number
/// @return ERR_SUCCESS or Invalid parameter
// =============================================================================

UINT32 COS_GetHeapUsageStatus(COS_HEAP_STATUS *pHeapStatus, UINT8 nIdx)
{

    UINT32 nNeededSize = 0x00; //allows to identify the target "cluster heap"
    UINT32 nRemainSize = sxr_GetAbsoluteRemainingSize(nNeededSize, nIdx);

    if (CSW_HEAP_INDEX == nIdx)
    {
        pHeapStatus->nUsedSize = g_CswHeapSize - nRemainSize;
        pHeapStatus->nTotalSize = g_CswHeapSize;
    }
    else if (COS_HEAP_INDEX == nIdx)
    {
        pHeapStatus->nUsedSize = g_CosHeapSize - nRemainSize;
        pHeapStatus->nTotalSize = g_CosHeapSize;
    }
    else
    {
        return ERR_INVALID_PARAMETER;
    }

    return ERR_SUCCESS;
}
// COS_PageProtectSetup
// -----------------------------------------------------------------------------
/// This function setups a protection page
///
/// @param nPageNum Name of the page we want to configure
/// @param nMode Protection mode
/// @param nStartAddr Address of the beginning of the page
/// @param nEndAddr  End address of the page. This address is not included in
/// the page
// =============================================================================
PUBLIC UINT32 COS_PageProtectSetup(COS_PAGE_NUM nPageNum, COS_PAGE_SPY_MODE nMode, UINT32 nStartAddr, UINT32 nEndAddr)
{
    if ((nPageNum < COS_PAGE_NUM_4) || (nPageNum > COS_PAGE_NUM_5) || (nMode < COS_PAGE_NO_TRIGGER) || (nMode > COS_PAGE_READWRITE_TRIGGER))
    {
        return ERR_INVALID_PARAMETER;
    }

    hal_DbgPageProtectSetup(nPageNum, nMode, nStartAddr, nEndAddr);

    return ERR_SUCCESS;
}

// =============================================================================
// COS_PageProtectEnable
// -----------------------------------------------------------------------------
/// Enable the protection of a given page.
///
/// @param nPageNum Page to enable
// =============================================================================
PUBLIC UINT32 COS_PageProtectEnable(COS_PAGE_NUM nPageNum)
{
    if ((nPageNum < COS_PAGE_NUM_4) || (nPageNum > COS_PAGE_NUM_5))
    {
        return ERR_INVALID_PARAMETER;
    }

    hal_DbgPageProtectEnable(nPageNum);

    return ERR_SUCCESS;
}

// =============================================================================
// COS_PageProtectDisable
// -----------------------------------------------------------------------------
/// Disable the protection of a given page
///
/// @param nPageNum Page to disable
// =============================================================================

PUBLIC UINT32 COS_PageProtectDisable(COS_PAGE_NUM nPageNum)
{
    if ((nPageNum < COS_PAGE_NUM_4) || (nPageNum > COS_PAGE_NUM_5))
    {
        return ERR_INVALID_PARAMETER;
    }

    hal_DbgPageProtectDisable(nPageNum);

    return ERR_SUCCESS;
}
