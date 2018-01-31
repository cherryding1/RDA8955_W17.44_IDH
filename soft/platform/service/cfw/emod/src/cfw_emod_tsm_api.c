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

#include <cswtype.h>
#include <errorcode.h>
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <cos.h>
#include <ts.h>
#include <itf_msgc.h>
#include <itf_api.h>

#include "cfw_emod_tsm.h"
#include "dbg.h"

UINT32 CFW_GetCellInfo(CFW_TSM_CURR_CELL_INFO *pCurrCellInfo, CFW_TSM_ALL_NEBCELL_INFO *pNeighborCellInfo, CFW_SIM_ID nSimID)
{
    HAO hEm = 0;
    CFW_TSM_GET_INFO *pGetInfo = NULL;
    UINT32  nPointer = 0x000000000;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GetCellInfo!\n",0x081006fc)));

    //UINT32 ret;
    //ret = CFW_CheckSimId(nSimID);
    //if( ERR_SUCCESS != ret)
    //{
    //  CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_GetCellInfo SIM ERROR!\n",0x081006fd)));
    //  return ret;
    //}
    hEm = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_EMOD_SRV_ID, nSimID);
    if(0 == hEm)
    {
        return ERR_CFW_INVALID_PARAMETER;
    }

    pGetInfo = CFW_GetAoUserData(hEm);
    if(NULL == pGetInfo)
    {
        return ERR_CFW_INVALID_PARAMETER;
    }
    nPointer = (UINT32)((UINT8 *)pGetInfo + SIZEOF(CFW_TSM_GET_INFO));

    if(pGetInfo->SelecNum.nServingCell == 1)
    {
        //modify by wulc for bug 13366
        //pCurrCellInfo=(CFW_TSM_CURR_CELL_INFO*)nPointer;
        SUL_MemCopy8(pCurrCellInfo, (void *)nPointer, sizeof(CFW_TSM_CURR_CELL_INFO));

        UINT32 nSize = SIZEOF(CFW_TSM_CURR_CELL_INFO);
        nPointer = nPointer + nSize;
    }
    if(pGetInfo->SelecNum.nNeighborCell == 1)
    {

        //modify by wulc for bug 13366
        //pNeighborCellInfo=(CFW_TSM_ALL_NEBCELL_INFO*)nPointer;
        SUL_MemCopy8(pNeighborCellInfo, (void *)nPointer, sizeof(CFW_TSM_ALL_NEBCELL_INFO));
    }

    return ERR_SUCCESS;
}


UINT32 CFW_EmodOutfieldTestStart(CFW_TSM_FUNCTION_SELECT *pSelecFUN, UINT16 nUTI, CFW_SIM_ID nSimID)
{

    HAO hAo                             = 0x00000000;
    CFW_TSM_GET_INFO    *pGetInfo = NULL;
    UINT32 nExtraSize = 0x00000000;
#if 0
    UINT8 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if( ERR_SUCCESS != ret)
    {
        return ret;
    }
#endif


    if (CFW_IsFreeUTI(nUTI, CFW_EMOD_SRV_ID) != ERR_SUCCESS)
        return ERR_CFW_UTI_IS_BUSY;
    if(pSelecFUN->nServingCell == 1)
    {
        nExtraSize = nExtraSize + SIZEOF(CFW_TSM_CURR_CELL_INFO);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_TSM_CURR_CELL_INFO    ---= 0x%x\r\n",0x081006fe)) , SIZEOF(CFW_TSM_CURR_CELL_INFO));
    }
    if(pSelecFUN->nNeighborCell == 1)
    {
        nExtraSize = nExtraSize + SIZEOF(CFW_TSM_ALL_NEBCELL_INFO);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_TSM_NEIGHBOR_CELL_INFO    ---= 0x%x\r\n",0x081006ff)) , SIZEOF(CFW_TSM_ALL_NEBCELL_INFO));
    }

    pGetInfo = (CFW_TSM_GET_INFO *)CSW_TMS_MALLOC(SIZEOF(CFW_TSM_GET_INFO) + nExtraSize);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_TSM_GET_INFO    ---= 0x%x\r\n",0x08100700)) , SIZEOF(CFW_TSM_GET_INFO));
    if (pGetInfo == NULL)
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    SUL_ZeroMemory32(pGetInfo, SIZEOF(CFW_TSM_GET_INFO) + nExtraSize);

    pGetInfo->SelecNum.nServingCell = pSelecFUN->nServingCell;
    pGetInfo->SelecNum.nNeighborCell = pSelecFUN->nNeighborCell;
    pGetInfo->bClose = FALSE;

    hAo = CFW_RegisterAo(CFW_EMOD_SRV_ID, pGetInfo, CFW_EmodOutfieldTestProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    return(ERR_SUCCESS);
}
UINT32  CFW_EmodOutfieldTestEnd(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    //A TWO MODE:UnRegisterAO  OR NOT POST EVENT TO MMI.
    HAO tsm_hAo;
    UINT32 result;
    CFW_TSM_GET_INFO        *pTsm_procdata                  = NULL;
    api_CurCellInfoReq_t  *pCurCellInfoReq                  = NULL;
    api_AdjCellInfoReq_t  *pAdjCellInfoReq                  = NULL;


    tsm_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_EMOD_SRV_ID, nSimID);
    if(0 == tsm_hAo)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]tsm AO is NULL !!\n",0x08100701)));
        //DBG_ASSERT(FALSE, "Tsm AO is NULL!");
        return ERR_CFW_NOT_EXIST_AO;
    }
    pTsm_procdata = CFW_GetAoUserData(tsm_hAo);
    if( NULL == pTsm_procdata )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]tsm AO-data is NULL !!\n",0x08100702)));
        DBG_ASSERT(FALSE, "Tsm AO-data is NULL!");
        return ERR_CFW_NOT_EXIST_AO;
    }
    pTsm_procdata->bClose = TRUE;
    if(pTsm_procdata->SelecNum.nServingCell == 1)
    {
        pCurCellInfoReq = (api_CurCellInfoReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CurCellInfoReq_t));
        if (pCurCellInfoReq == NULL)
            return ERR_NO_MORE_MEMORY;

        pCurCellInfoReq->StartReport = 0;
        result = CFW_SendSclMessage(API_CURCELL_INFO_REQ, pCurCellInfoReq, nSimID);

        if(ERR_SUCCESS != result)
        {
            return result;
        }
    }
    if(pTsm_procdata->SelecNum.nNeighborCell == 1)
    {
        pAdjCellInfoReq = (api_AdjCellInfoReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_AdjCellInfoReq_t));
        if(pAdjCellInfoReq == NULL)
            return ERR_NO_MORE_MEMORY;
        pAdjCellInfoReq->StartReport = 0;
        result = CFW_SendSclMessage(API_ADJCELL_INFO_REQ, pAdjCellInfoReq, nSimID);
        if(ERR_SUCCESS != result)
        {
            return result;
        }
    }

    result = CFW_UnRegisterAO(CFW_EMOD_SRV_ID, tsm_hAo);

    if(result == ERR_SUCCESS)
    {

        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Close tsm mode success!!!\n",0x08100703)));
        //CFW_UnRegisterAO(CFW_EMOD_SRV_ID,tsm_hAo);
        tsm_hAo = HNULL;
        CFW_PostNotifyEvent (EV_CFW_TSM_INFO_END_RSP, 0, 0, nUTI | (nSimID << 24), 0x00);

    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Close tsm mode failure!!!\n",0x08100704)));
        CFW_PostNotifyEvent (EV_CFW_TSM_INFO_END_RSP, result, 0, nUTI | (nSimID << 24), 0xF0);
    }
    return result;//added by fengwei for warning reduce
}


UINT32 CFW_EmodSyncInfoTest(BOOL bStart, UINT16 nUTI, CFW_SIM_ID nSimID)
{

    HAO hAo = 0x00;
    CFW_TSM_GET_INFO    *pGetInfo = NULL;
    UINT32 nRet = 0x00;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_EmodSyncInfoTest bStart 0x%x\r\n",0x08100705)) , bStart);

    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_EMOD_SRV_ID))
        return ERR_CFW_UTI_IS_BUSY;

    if(bStart)
    {
        pGetInfo = (CFW_TSM_GET_INFO *)CSW_TMS_MALLOC(SIZEOF(CFW_TSM_GET_INFO));
        if (pGetInfo == NULL)
            return ERR_NO_MORE_MEMORY;  // should be updated by macro

        SUL_ZeroMemory32(pGetInfo, SIZEOF(CFW_TSM_GET_INFO));

        pGetInfo->bClose = FALSE;

        hAo = CFW_RegisterAo(CFW_EMOD_SRV_ID, pGetInfo, CFW_EmodSyncInfoTestProc, nSimID);

        CFW_SetUTI(hAo, nUTI, 0);
        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    }
    else
    {
        HAO tsm_hAo;
        CFW_TSM_GET_INFO *pTsm_procdata = NULL;
        api_SyncInfoReq_t *pSyncInfoReq = NULL;

        tsm_hAo = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_EMOD_SRV_ID, nSimID);
        if(0 == tsm_hAo)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]tsm AO is NULL !!\n",0x08100706)));
            return ERR_CFW_NOT_EXIST_AO;
        }
        pTsm_procdata = CFW_GetAoUserData(tsm_hAo);
        if( NULL == pTsm_procdata )
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("[ERROR]tsm AO-data is NULL !!\n",0x08100707)));
            CSW_ASSERT(0);// "Tsm AO-data is NULL!"
            return ERR_CFW_NOT_EXIST_AO;
        }
        pSyncInfoReq = (api_SyncInfoReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SyncInfoReq_t));
        if ( NULL == pSyncInfoReq )
            return ERR_NO_MORE_MEMORY;

        pSyncInfoReq->StartReport = FALSE;

        nRet = CFW_SendSclMessage(API_SYNC_INFO_REQ, pSyncInfoReq, nSimID);

        nRet = CFW_UnRegisterAO(CFW_EMOD_SRV_ID, tsm_hAo);

        if(nRet == ERR_SUCCESS)
        {

            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Close SyncInfo success!!!\n",0x08100708)));
            //CFW_UnRegisterAO(CFW_EMOD_SRV_ID,tsm_hAo);
            tsm_hAo = HNULL;
            CFW_PostNotifyEvent (EV_CFW_TSM_SYNC_INFO_END_RSP, 0, 0, nUTI | (nSimID << 24), 0x00);

        }
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Close SyncInfo failure!!!\n",0x08100709)));
            CFW_PostNotifyEvent (EV_CFW_TSM_SYNC_INFO_END_RSP, nRet, 0, nUTI | (nSimID << 24), 0xF0);
        }

    }

    return(ERR_SUCCESS);
}




