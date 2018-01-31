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
#include <itf_msgc.h>
#include <itf_api.h>
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <cos.h>
#include <ts.h>


#include "cfw_emod_tsm.h"




// 说明1、整个TSM外场测试部分启动一个AO
//2、每一个SCL的API对应一个FUNCTON选择，并且每一个FUNCTON都是以5s自动上报的方式上报。
//3、AO给MMI的事件MMI不能释放此指针，MMI可以根据提取自己需要的信息。
UINT32 CFW_EmodOutfieldTestProc (HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI        = 0x00000000;
    UINT32 nEvtId    = 0x00000000;
    BOOL   nStartAoMark = FALSE;
    UINT32 result = ERR_CME_UNKNOWN;
    VOID  *nEvParam                                         = NULL;
    CFW_EV ev;
    CFW_TSM_GET_INFO        *pGetInfo                       = NULL;
    api_CurCellInfoReq_t  *pCurCellInfoReq                  = NULL;
    api_AdjCellInfoReq_t  *pAdjCellInfoReq                  = NULL;
    api_CurCellInfoInd_t            *pCurrCellInfoInd       = NULL;
    api_AdjCellInfoInd_t            *pAdjCellInfoInd            = NULL;
    CFW_TSM_CURR_CELL_INFO              *pCurrCellInfo          = NULL;
    CFW_TSM_ALL_NEBCELL_INFO            *pNeighborCellInfo      = NULL;
    UINT32  nPointer = 0x000000000;
    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hAo);

    pGetInfo = CFW_GetAoUserData(hAo);
    if((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pGetInfo->nState.nNextState = CFW_TSM_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_TSMProc()\r\n",0x08100740)) );
    if (!(pGetInfo->bClose))
    {
        switch(pGetInfo->nState.nNextState)
        {

            case CFW_TSM_IDLE:
                if(pGetInfo->SelecNum.nServingCell == 1)
                {

                    pCurCellInfoReq = (api_CurCellInfoReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CurCellInfoReq_t));
                    if (pCurCellInfoReq == NULL)
                        result = ERR_NO_MORE_MEMORY;
                    if(result == ERR_NO_MORE_MEMORY)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MALLOC failure!!!\r\n",0x08100741)) );
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        hAo = HNULL;
                        CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        return result;
                    }
#ifdef CSW_USER_DBS
                    pCurCellInfoReq->StartReport = 0;
#else
                    pCurCellInfoReq->StartReport = pGetInfo->SelecNum.nServingCell;
#endif

                    result = CFW_SendSclMessage (API_CURCELL_INFO_REQ, pCurCellInfoReq, nSimID);
                    if(ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SendSclMessage() failure error code 0x%x\r\n",0x08100742)) , result);
                        CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                        hAo = HNULL;
                        CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    }
                    nStartAoMark = TRUE;
                }
                if(pGetInfo->SelecNum.nNeighborCell == 1)
                {
                    pAdjCellInfoReq = (api_AdjCellInfoReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_AdjCellInfoReq_t));
                    if(pAdjCellInfoReq == NULL)
                        result = ERR_NO_MORE_MEMORY;
                    if(result == ERR_NO_MORE_MEMORY)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MALLOC failure!!!\r\n",0x08100743)) );
                        CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                        hAo = HNULL;
                        return result;
                    }
#ifdef CSW_USER_DBS
                    pAdjCellInfoReq->StartReport = 0;
#else
                    pAdjCellInfoReq->StartReport = pGetInfo->SelecNum.nNeighborCell;
#endif
                    result = CFW_SendSclMessage (API_ADJCELL_INFO_REQ, pAdjCellInfoReq, nSimID);
                    if(ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SendSclMessage() failure error code 0x%x\r\n",0x08100744)) , result);
                        CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                        hAo = HNULL;

                        CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    }
                    nStartAoMark = TRUE;
                }
                if(nStartAoMark == TRUE)
                {

                    pGetInfo->nState.nCurrState = CFW_TSM_IDLE;
                    pGetInfo->nState.nNextState = CFW_TSM_REC_IND;
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                }
                else
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("No require start tsm function,the ao unregister!!!\n",0x08100745)));
                    CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                    hAo = HNULL;
                    CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                }

                break;

            case CFW_TSM_REC_IND:
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--CFW_TSM_REC_IND pEvent->nEventId=0x%x\r\n",0x08100746), pEvent->nEventId);
                nPointer = (UINT32)((UINT8 *)pGetInfo + SIZEOF(CFW_TSM_GET_INFO));
                if(pGetInfo->SelecNum.nServingCell == 1)
                {
                    pCurrCellInfo = (CFW_TSM_CURR_CELL_INFO *)nPointer;
                    UINT32 nSize = SIZEOF(CFW_TSM_CURR_CELL_INFO);

                    nPointer = nPointer + nSize;
                    /*      CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--nPointer2=0x%x\r\n",0x08100747), nPointer);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--pGetInfo=0x%x\r\n",0x08100748), pGetInfo);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--CFW_TSM_CURR_CELL_INFO nSize=0x%x\r\n",0x08100749), nSize);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--pCurrCellInfo=0x%x\r\n",0x0810074a), pCurrCellInfo);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--CFW_TSM_CURR_CELL_INFO first add=0x%x\r\n",0x0810074b), &pCurrCellInfo->nTSM_Arfcn);
                            */
                }
                if(pGetInfo->SelecNum.nNeighborCell == 1)
                {
                    pNeighborCellInfo = (CFW_TSM_ALL_NEBCELL_INFO *)nPointer;

                    UINT32 nSize = SIZEOF(CFW_TSM_ALL_NEBCELL_INFO);

                    nPointer = nPointer + nSize;
                    /*  CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--pNeighborCellInfo=0x%x\r\n",0x0810074c), pNeighborCellInfo);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--CFW_TSM_ALL_NEBCELL_INFO nSize=0x%x\r\n",0x0810074d), nSize);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--CFW_TSM_ALL_NEBCELL_INFO first add=0x%x\r\n",0x0810074e),&pNeighborCellInfo->nTSM_NebCellNUM);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--nPointer=0x%x\r\n",0x0810074f), nPointer);
                        */
                }

                CFW_GetUTI(hAo, &nUTI);
                switch(pEvent->nEventId)
                {

                    case API_CURCELL_INFO_IND:

                        pCurrCellInfoInd = (api_CurCellInfoInd_t *)nEvParam;
                        //Modify by lixp at 20120615
                        //if(CurrCellDataParse(pCurrCellInfo,pCurrCellInfoInd))
                        CurrCellDataParse(pCurrCellInfo, pCurrCellInfoInd);
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("API_CURCELLINFO_IND   post!\r\n",0x08100750)) );
                            //                  UINT32 pTemp=&pCurrCellInfo->nTSM_Arfcn;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--pCurrCellInfo---------=0x%x\r\n",0x08100751), &pCurrCellInfo->nTSM_Arfcn);
                            CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)0, CFW_TSM_CURRENT_CELL, nUTI | (nSimID << 24), 0x00);
                        }
                        break;
                    case API_ADJCELL_INFO_IND:

                        pAdjCellInfoInd = (api_AdjCellInfoInd_t *)nEvParam;
                        //Modify by lixp at 20120615
                        //if(NeighborCellDataParse(pNeighborCellInfo, pAdjCellInfoInd))
                        NeighborCellDataParse(pNeighborCellInfo, pAdjCellInfoInd);
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("API_ADJCELLSINFO_IND    post!\r\n",0x08100752)) );

                            //          UINT32 pTemp=&pNeighborCellInfo->nTSM_NebCellNUM;
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_TSMProc()--pNeighborCellInfo----------=0x%x\r\n",0x08100753), &pNeighborCellInfo->nTSM_NebCellNUM);
                            CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)0, CFW_TSM_NEIGHBOR_CELL, nUTI | (nSimID << 24), 0x00);
                        }
                        break;
                    default:
                        break;
                }
                pGetInfo->nState.nCurrState = CFW_TSM_REC_IND;
                pGetInfo->nState.nNextState = CFW_TSM_REC_IND;
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                break;
            default:
                CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                hAo = HNULL;
                break;
        }
    }
    else
    {
        CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_AO_PROC_CODE_CONSUMED tsm close!!!\r\n",0x08100754));
    }
    return ERR_SUCCESS;
}
UINT32 CFW_EmodSyncInfoTestProc (HAO hAo, CFW_EV *pEvent)
{
    UINT32 nUTI     = 0x00;
    UINT32 nEvtId   = 0x00;
    UINT32 nRet     = ERR_SUCCESS;
    VOID  *nEvParam     = NULL;
    CFW_EV ev;
    CFW_TSM_GET_INFO            *pGetInfo           = NULL;
    api_SyncInfoReq_t       *pSyncInfoReq       = NULL;


    CFW_SIM_ID nSimID;
    nSimID = CFW_GetSimCardID(hAo);

    pGetInfo = CFW_GetAoUserData(hAo);
    if((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
        pGetInfo->nState.nNextState = CFW_TSM_IDLE;
    }
    else
    {
        nEvtId = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EmodSyncInfoTestProc()\r\n",0x08100755)) );
    CFW_GetUTI(hAo, &nUTI);

    switch(pGetInfo->nState.nNextState)
    {

        case CFW_TSM_IDLE:
        {

            pSyncInfoReq = (api_SyncInfoReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_SyncInfoReq_t));
            if (NULL == pSyncInfoReq)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("MALLOC failure!!!\r\n",0x08100756)) );
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                hAo = HNULL;
                CFW_PostNotifyEvent (EV_CFW_TSM_SYNC_INFO_END_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                return nRet;
            }
            pSyncInfoReq->StartReport = TRUE;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("CFW_EmodSyncInfoTestProc() send API_SYNC_INFO_REQ\r\n",0x08100757)) );
            nRet = CFW_SendSclMessage (API_SYNC_INFO_REQ, pSyncInfoReq, nSimID);
            if(ERR_SUCCESS != nRet)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("CFW_SendSclMessage() failure error code 0x%x\r\n",0x08100758)) , nRet);
                CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                hAo = HNULL;
                CFW_PostNotifyEvent (EV_CFW_TSM_INFO_IND, (UINT32)nRet, 0, nUTI | (nSimID << 24), 0xF0);
                return ERR_SUCCESS;
            }

            pGetInfo->nState.nCurrState = CFW_TSM_IDLE;
            pGetInfo->nState.nNextState = CFW_TSM_REC_IND;

            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);

        }


        break;
        case CFW_TSM_REC_IND:
        {

            if(API_SYNC_INFO_IND ==  pEvent->nEventId )
            {

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Get API_SYNC_INFO_IND\r\n",0x08100759)) );
                api_SyncInfoInd_t *pSyncInfo = (api_SyncInfoInd_t *)nEvParam;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("Get API_SYNC_INFO_IND Arfcn %d pSyncInfo->Band %d\r\n",0x0810075a)), pSyncInfo->Arfcn, pSyncInfo->Band );

                CFW_PostNotifyEvent (EV_CFW_TSM_SYNC_INFO_IND, (UINT32)pSyncInfo->Arfcn + (pSyncInfo->Band << 16), CFW_SYNC_INFO_IND, nUTI | (nSimID << 24), 0x00);

            }
            else if( API_POWERLIST_IND ==  pEvent->nEventId )
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("Get API_POWERLIST_IND\r\n",0x0810075b)) );
                api_PowerListInd_t  *pPowerList = (api_PowerListInd_t *)nEvParam;
                UINT8 i = 0x00;
                while(i < pPowerList->Count)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("Arfcn %d Rssi %d\r\n",0x0810075c)), pPowerList->PowerList[i].Arfcn, pPowerList->PowerList[i].Rssi);
                    i++;
                }
                CFW_EmodPowerListInd *pEmodPowerList = (CFW_EmodPowerListInd *)CSW_TMS_MALLOC(SIZEOF(CFW_EmodPowerListInd));
                if(NULL == pEmodPowerList)
                {

                    CFW_PostNotifyEvent (EV_CFW_TSM_SYNC_INFO_IND, NULL, CFW_POWER_LIST_IND, nUTI | (nSimID << 24), 0xF0);
                    CFW_UnRegisterAO(CFW_EMOD_SRV_ID, hAo);
                    hAo = HNULL;
                    return ERR_NO_MORE_MEMORY;
                }
                SUL_ZeroMemory8(pEmodPowerList, SIZEOF(CFW_EmodPowerListInd));
                SUL_MemCopy8(pEmodPowerList, pPowerList, SIZEOF(CFW_EmodPowerListInd));

                CFW_PostNotifyEvent (EV_CFW_TSM_SYNC_INFO_IND, (UINT32)pEmodPowerList, CFW_POWER_LIST_IND, nUTI | (nSimID << 24), 0x00);

            }
            break;
        }

        break;

    }
    return ERR_SUCCESS;
}

