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
// #include <scl.h>
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <cfw_prv.h>
#include <event.h>
#include <sul.h>
#include <cos.h>
#include <ts.h>
#include <csw_mem_prv.h>
#include "csw_debug.h"
#include "cfw_usim.h"

extern struct SimADNInfo ADNInfo[CFW_SIM_COUNT][2];
extern CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;

#if (CFW_SIM_DUMP_ENABLE==0) && (CFW_SIM_SRV_ENABLE==1)

#include "cfw_sim.h"
#include "cfw_sim_pbk.h"
//-------------------add by wulc for smoke test ------------------
//make CFW_SimPBKAddProc( ) can return right parameter2 which used by smoketest demo
UINT8 _GetPBKStorage(UINT32 nStackFileID)
{
    if(( API_SIM_EF_ADN == nStackFileID ) || (API_USIM_EF_GB_ADN == nStackFileID ) || (API_USIM_EF_GB_ADN1 == nStackFileID )
            || (API_USIM_EF_ADN == nStackFileID ) || (API_USIM_EF_ADN1 == nStackFileID ))
        return CFW_PBK_SIM;
    else if((API_SIM_EF_FDN == nStackFileID) || (API_USIM_EF_FDN == nStackFileID))
        return CFW_PBK_SIM_FIX_DIALLING;
    else if(API_SIM_EF_LND == nStackFileID)
        return CFW_PBK_SIM_LAST_DIALLING;
    else if((API_SIM_EF_BDN == nStackFileID) || (API_USIM_EF_BDN == nStackFileID))
        return CFW_PBK_SIM_BARRED_DIALLING;
    else if((API_SIM_EF_MSISDN == nStackFileID) || (API_USIM_EF_MSISDN == nStackFileID))
        return CFW_PBK_ON;
    else
        return 0x00;

}
//------------------------------------------------------------
UINT32 cfw_SimParsePBKRecData(UINT8 *pData,
                              CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry,
                              void *proc,
                              UINT8 txtlength,
                              UINT8 nApiNum
                             );
UINT32 Sim_ParseSW1SW2(UINT8 SW1, UINT8 SW2, CFW_SIM_ID nSimID);


UINT8 getADNRecordNumber(UINT16 nIndex, CFW_SIM_ID nSimID)
{
    UINT16 s0 = ADNInfo[nSimID][0].MaxRecords;
    UINT16 s1 = ADNInfo[nSimID][1].MaxRecords;
    if(nIndex == 0)
        return 0;
    if(nIndex > (s0 + s1))
        return 0;
    else if(nIndex > s0)    //if index is biger than the max number of record in ADN, that means access ADN1
        return (nIndex - s0);
    else
        return nIndex;
}

UINT8 getADNRecordSize(UINT16 nIndex, CFW_SIM_ID nSimID)
{
    UINT16 s0 = ADNInfo[nSimID][0].MaxRecords;
    UINT16 s1 = ADNInfo[nSimID][1].MaxRecords;
    if(nIndex == 0)
        return 0;
    if(nIndex > (s0 + s1))
        return 0;
    else if(nIndex > s0)    //if index is biger than the max number of record in ADN, that means access ADN1
        return ADNInfo[nSimID][1].RecordSize;
    else
        return ADNInfo[nSimID][0].RecordSize;
}

UINT8 getADNEFID(UINT16 nIndex, CFW_SIM_ID nSimID)
{
    UINT16 s0 = ADNInfo[nSimID][0].MaxRecords;
    UINT16 s1 = ADNInfo[nSimID][1].MaxRecords;
    if(nIndex == 0)
        return 0;
    if(nIndex > (s0 + s1))
        return 0;
    else if(nIndex > s0)    //if index is biger than the max number of record in ADN, that means access ADN1
        return ADNInfo[nSimID][1].EFID;
    else
        return ADNInfo[nSimID][0].EFID;
}

UINT32 CFW_SimPBKAddProc(HAO hAo, CFW_EV *pEvent)
{

    UINT32 nEvtId    = 0;
    UINT32 ErrorCode = 0;
    UINT32 result    = 0;
    UINT8 txtlength  = 0;
    VOID *nEvParam   = NULL;
    CFW_EV ev;

    SIM_SM_INFO_ADDPBK *pAddPBK = NULL;
    UINT8 *pPadFData = NULL;

    SIM_CHV_PARAM *pSimCHVPara = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    api_SimUpdateRecordCnf_t *pSimUpdateRecordCnf = NULL;
    api_SimSeekCnf_t *pSimSeekCnf = NULL;
    api_SimReadRecordCnf_t *pSimReadRecordCnf = NULL;
    CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKAddProc);

    UINT32 nUTI = 0;
    CFW_GetUTI(hAo, &nUTI);

    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pAddPBK = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                 = 0xff;
        pEvent                     = &ev;
        pAddPBK->nState.nNextState = CFW_SIM_PBK_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKAddProc) Status: %d File: %d",0x08100d2a), pAddPBK->nState.nNextState, pAddPBK->nCurrentFile);
    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
    switch (pAddPBK->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
            switch (pAddPBK->nCurrentFile)
            {
                case API_SIM_EF_ADN:
                    pAddPBK->nDataSize = pSimPBKPara->nADNRecordSize;
                    break;
                case API_USIM_EF_GB_ADN:
                case API_USIM_EF_GB_ADN1:
                case API_USIM_EF_ADN:
                case API_USIM_EF_ADN1:
                    pAddPBK->nDataSize = getADNRecordSize(pAddPBK->nAddRecInd, nSimID);
                    break;
                case API_SIM_EF_FDN:
                case API_USIM_EF_FDN:
                    pAddPBK->nDataSize = pSimPBKPara->nFDNRecordSize;
                    break;
                case API_SIM_EF_LND:
                    pAddPBK->nDataSize = pSimPBKPara->nLNDRecordSize;
                    break;
                case API_SIM_EF_BDN:
                case API_USIM_EF_BDN:
                    pAddPBK->nDataSize = pSimPBKPara->nBDNRecordSize - 1;
                    break;
                case API_SIM_EF_MSISDN:
                case API_USIM_EF_MSISDN:
                    pAddPBK->nDataSize = pSimPBKPara->nMSISDNRecordSize;
                    break;
                default:
                    break;
            }

            if (pAddPBK->nCurrentFile == API_SIM_EF_LND)
            {
                SUL_MemSet8(pAddPBK->pData + pAddPBK->nNameSize, 0xFF,
                            (UINT16)(pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH - pAddPBK->nNameSize));
                pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH] = pAddPBK->nNumSize + 1;
                pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 1] = pAddPBK->nNumType;
                if (pAddPBK->nNumSize > 0)
                {
                    SUL_MemCopy8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2, pAddPBK->pNum, pAddPBK->nNumSize);
                }
                SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2 + pAddPBK->nNumSize, 0xFF,
                            (UINT16)(10 - pAddPBK->nNumSize));
                SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - 2, 0xFF, 2);

                result = SimUpdateRecordReq(pAddPBK->nCurrentFile, 0x00, 0x03, pAddPBK->nDataSize, pAddPBK->pData, nSimID);

                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUpdateRecordReq return 0x%x \n",0x08100d2b), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pAddPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pAddPBK->nState.nNextState = CFW_SIM_PBK_UPDATERECORD;
            }
            else
            {
                if ((pAddPBK->nCurrentFile == API_SIM_EF_FDN) || (pAddPBK->nCurrentFile == API_SIM_EF_BDN)
                        || (pAddPBK->nCurrentFile == API_USIM_EF_FDN) || (pAddPBK->nCurrentFile == API_USIM_EF_BDN))
                {
                    CFW_CfgSimGetChvParam(&pSimCHVPara, nSimID);
                    if(pSimCHVPara->nPin2Status == CHV_NOVERIFY || pSimCHVPara->nPin2Remain == 0)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, ERR_CME_SIM_PIN2_REQUIRED, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_SUCCESS;
                    }
                }

                if (pAddPBK->nAddRecInd == 0)
                {
                    UINT8 size = pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 1;
                    pPadFData = (UINT8 *)CSW_SIM_MALLOC(size);
                    if (pPadFData == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d2c));
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, ERR_NO_MORE_MEMORY, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_SUCCESS;
                    }
                    SUL_MemSet8(pPadFData, 0xFF, (UINT16)(size));
                    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE) // GSM
                        result = SimSeekReq(pAddPBK->nCurrentFile, 0x10, (UINT8)(size), pPadFData, nSimID);
                    else // USIM; MODE = 04: Start forward search form Currently selected EF
                        result = SimSeekReq(pAddPBK->nCurrentFile, 0x04, (UINT8)(size), pPadFData, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimSeekReq return 0x%x \n",0x08100d2d), result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    CSW_SIM_FREE(pPadFData);
                    pAddPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                    pAddPBK->nState.nNextState = CFW_SIM_PBK_SEEK;
                }
                else if (pAddPBK->nAddRecInd != 0)
                {
                    SUL_MemSet8(pAddPBK->pData + pAddPBK->nNameSize, 0xFF,
                                (UINT16)(pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH - pAddPBK->nNameSize));
                    pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH] = pAddPBK->nNumSize + 1;
                    pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 1] = pAddPBK->nNumType;
                    if (pAddPBK->nNumSize > 0)
                        SUL_MemCopy8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2, pAddPBK->pNum,
                                     pAddPBK->nNumSize);
                    SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2 + pAddPBK->nNumSize, 0xFF,
                                (UINT16)(10 - pAddPBK->nNumSize));
                    SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - 2, 0xFF, 2);
                    if((pAddPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pAddPBK->nCurrentFile == API_USIM_EF_ADN))
                    {
                        UINT8 EFID = getADNEFID(pAddPBK->nAddRecInd,  nSimID);
                        UINT8 nRecordNumber = getADNRecordNumber(pAddPBK->nAddRecInd,  nSimID);
                        UINT8 nRecordSize = getADNRecordSize(pAddPBK->nAddRecInd,  nSimID);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("nRecordNumbern = %d",0x08100d2e), nRecordNumber);
                        result = SimUpdateRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, pAddPBK->pData, nSimID);
                    }
                    else
                        result = SimUpdateRecordReq(pAddPBK->nCurrentFile, pAddPBK->nAddRecInd, 0x04, pAddPBK->nDataSize, pAddPBK->pData, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUpdateRecordReq return 0x%x \n",0x08100d2f), result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    pAddPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                    pAddPBK->nState.nNextState = CFW_SIM_PBK_UPDATERECORD;
                }
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
            break;

        case CFW_SIM_PBK_SEEK:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_SEEK_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimSeekCnf = (api_SimSeekCnf_t *)nEvParam;
            if (((0x90 == pSimSeekCnf->Sw1) && (0x00 == pSimSeekCnf->Sw2)) || (0x9F == pSimSeekCnf->Sw1)
                    || (0x91 == pSimSeekCnf->Sw1))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" SIM seek Success! \n",0x08100d30)));
                pAddPBK->nAddRecInd = pSimSeekCnf->RecordNb;
                SUL_MemSet8(pAddPBK->pData + pAddPBK->nNameSize, 0xFF,
                            (UINT16)(pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH - pAddPBK->nNameSize));
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->nDataSize=%x\r\n",0x08100d31)), pAddPBK->nDataSize);

                pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH] = pAddPBK->nNumSize + 1;

                pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 1] = pAddPBK->nNumType;
                SUL_MemCopy8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2, pAddPBK->pNum, pAddPBK->nNumSize);

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->nDataSize=%x\r\n",0x08100d32)), pAddPBK->nDataSize);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->nNumSize=%x\r\n",0x08100d33)), pAddPBK->nNumSize);

                SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2 + pAddPBK->nNumSize, 0xFF,
                            (UINT16)(10 - pAddPBK->nNumSize));
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->nDataSize=%x\r\n",0x08100d34)), pAddPBK->nDataSize);

                SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - 2, 0xFF, 2);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->nAddRecInd=%x\r\n",0x08100d35)), pAddPBK->nAddRecInd);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->nDataSize=%x\r\n",0x08100d36)), pAddPBK->nDataSize);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pAddPBK->pData=%x\r\n",0x08100d37)), pAddPBK->pData);

                if((pAddPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pAddPBK->nCurrentFile == API_USIM_EF_ADN))
                {
                    UINT8 EFID = getADNEFID(pAddPBK->nAddRecInd,  nSimID);
                    UINT8 nRecordNumber = getADNRecordNumber(pAddPBK->nAddRecInd,  nSimID);
                    UINT8 nRecordSize = getADNRecordSize(pAddPBK->nAddRecInd,  nSimID);
                    result = SimUpdateRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, pAddPBK->pData, nSimID);
                }
                else
                    result = SimUpdateRecordReq(pAddPBK->nCurrentFile, pAddPBK->nAddRecInd, 0x04, pAddPBK->nDataSize, pAddPBK->pData, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUpdateRecordReq return 0x%x \n",0x08100d38), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                pAddPBK->nState.nCurrState = CFW_SIM_PBK_SEEK;
                pAddPBK->nState.nNextState = CFW_SIM_PBK_UPDATERECORD;
            }
            else
            {
                if (0x67 == pSimSeekCnf->Sw1)
                {
                    result = SimReadRecordReq(pAddPBK->nCurrentFile, 0x01, 0x04, pAddPBK->nDataSize, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadRecordReq return 0x%x \n",0x08100d39), result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    pAddPBK->nCoutOfAdd = 1;
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    pAddPBK->nState.nCurrState = CFW_SIM_PBK_SEEK;
                    pAddPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
                }
                else
                {
                    ErrorCode = Sim_ParseSW1SW2(pSimSeekCnf->Sw1, pSimSeekCnf->Sw2, nSimID);
                    CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)ErrorCode, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d3a), ErrorCode);

                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
            }
        }
        break;
        case CFW_SIM_PBK_READRECORD:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READRECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
                if (pGetPBKEntry == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d3b));
                    CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                pGetPBKEntry->pNumber   = (UINT8 *)pGetPBKEntry + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry->pFullName = (UINT8 *)pGetPBKEntry + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);
                txtlength               = pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH;
                cfw_SimParsePBKRecData(pSimReadRecordCnf->Data, pGetPBKEntry, pAddPBK, txtlength, API_GETPBK);

                if ((pGetPBKEntry->iFullNameSize == 0) && (pGetPBKEntry->nNumberSize == 0) && (pGetPBKEntry->nType != CFW_TELNUMBER_TYPE_INTERNATIONAL))  // updated on 20070618 to read "+" entry
                {
                    pAddPBK->nAddRecInd = pAddPBK->nCoutOfAdd;
                    SUL_MemSet8(pAddPBK->pData + pAddPBK->nNameSize, 0xFF,
                                (UINT16)(pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH - pAddPBK->nNameSize));
                    pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH] = pAddPBK->nNumSize + 1;
                    pAddPBK->pData[pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 1] = pAddPBK->nNumType;
                    SUL_MemCopy8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2, pAddPBK->pNum, pAddPBK->nNumSize);
                    SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - CFW_SIM_REMAINLENGTH + 2 + pAddPBK->nNumSize, 0xFF,
                                (UINT16)(10 - pAddPBK->nNumSize));
                    SUL_MemSet8(pAddPBK->pData + pAddPBK->nDataSize - 2, 0xFF, 2);

                    if((pAddPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pAddPBK->nCurrentFile == API_USIM_EF_ADN))
                    {
                        UINT8 EFID = getADNEFID(pAddPBK->nAddRecInd,  nSimID);
                        UINT8 nRecordNumber = getADNRecordNumber (pAddPBK->nAddRecInd,  nSimID);
                        UINT8 nRecordSize= getADNRecordSize(pAddPBK->nAddRecInd,  nSimID);
                        result = SimUpdateRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, pAddPBK->pData, nSimID);
                    }
                    else
                        result = SimUpdateRecordReq(pAddPBK->nCurrentFile, pAddPBK->nAddRecInd, 0x04, pAddPBK->nDataSize, pAddPBK->pData, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUpdateRecordReq return 0x%x \n",0x08100d3c), result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    pAddPBK->nState.nCurrState = CFW_SIM_PBK_READRECORD;
                    pAddPBK->nState.nNextState = CFW_SIM_PBK_UPDATERECORD;
                }
                else
                {
                    result = SimReadRecordReq(pAddPBK->nCurrentFile, pAddPBK->nCoutOfAdd + 1, 0x04, pAddPBK->nDataSize, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadRecordReq return 0x%x \n",0x08100d3d), result);
                        CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    pAddPBK->nCoutOfAdd++;
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                    pAddPBK->nState.nCurrState = CFW_SIM_PBK_READRECORD;
                    pAddPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
                }

            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)ErrorCode, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d3e), ErrorCode);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        break;
        case CFW_SIM_PBK_UPDATERECORD:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_UPDATERECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }
            pSimUpdateRecordCnf = (api_SimUpdateRecordCnf_t *)nEvParam;
            if (((0x90 == pSimUpdateRecordCnf->Sw1) && (0x00 == pSimUpdateRecordCnf->Sw2))
                    || (0x91 == pSimUpdateRecordCnf->Sw1))
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)pAddPBK->nAddRecInd, _GetPBKStorage(pAddPBK->nCurrentFile), nUTI | (nSimID << 24), 0);
                // g_PbkIndex = (UINT32)pAddPBK->nAddRecInd;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("post EV_CFW_SIM_ADD_PBK_RSP Success!nAddRecInd = %d \n",0x08100d3f)),
                          (UINT32)pAddPBK->nAddRecInd);
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimUpdateRecordCnf->Sw1, pSimUpdateRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, (UINT32)ErrorCode, pAddPBK->nAddRecInd, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d40), ErrorCode);
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        default:
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKAddProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            hAo = HNULL;
            break;
    }

    return ERR_SUCCESS;
}

UINT32 CFW_SimPBKDelProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId    = 0;
    UINT32 ErrorCode = 0;
    UINT32 result    = 0;
    VOID *nEvParam   = NULL;
    CFW_EV ev;

    SIM_SM_INFO_DELEPBK *pDelePBK = NULL;
    UINT8 *pPadFData = NULL;

    SIM_PBK_PARAM *pSimPBKPara = NULL;
    SIM_CHV_PARAM *pSimCHVPara = NULL;
    api_SimUpdateRecordCnf_t *pSimUpdateRecordCnf = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKDelProc);

    UINT32 nUTI      = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pDelePBK = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                  = 0xff;
        pEvent                      = &ev;
        pDelePBK->nState.nNextState = CFW_SIM_PBK_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKDelProc) Status: %d File: %d",0x08100d41), pDelePBK->nState.nNextState, pDelePBK->nCurrentFile);

    switch (pDelePBK->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
            switch (pDelePBK->nCurrentFile)
            {
                case API_SIM_EF_ADN:
                    pDelePBK->nPadFDataSize = pSimPBKPara->nADNRecordSize;
                    break;
                case API_USIM_EF_GB_ADN:
                case API_USIM_EF_GB_ADN1:
                case API_USIM_EF_ADN:
                case API_USIM_EF_ADN1:
                    pDelePBK->nPadFDataSize = getADNRecordSize(pDelePBK->nDeleRecInd, nSimID);
                    break;
                case API_SIM_EF_FDN:
                case API_USIM_EF_FDN:
                    pDelePBK->nPadFDataSize = pSimPBKPara->nFDNRecordSize;
                    break;
                case API_SIM_EF_BDN:
                case API_USIM_EF_BDN:
                    pDelePBK->nPadFDataSize = pSimPBKPara->nBDNRecordSize - 1;
                    break;
                case API_SIM_EF_MSISDN:
                case API_USIM_EF_MSISDN:
                    pDelePBK->nPadFDataSize = pSimPBKPara->nMSISDNRecordSize;
                    break;
                case API_SIM_EF_LND:
                    pDelePBK->nPadFDataSize = pSimPBKPara->nLNDRecordSize;
                    break;
                default:
                    break;
            }
            if ((pDelePBK->nCurrentFile == API_SIM_EF_FDN) || (pDelePBK->nCurrentFile == API_SIM_EF_BDN)
                    || (pDelePBK->nCurrentFile == API_USIM_EF_FDN) || (pDelePBK->nCurrentFile == API_USIM_EF_BDN))
            {
                CFW_CfgSimGetChvParam(&pSimCHVPara, nSimID);
                if(pSimCHVPara->nPin2Status == CHV_NOVERIFY || pSimCHVPara->nPin2Remain == 0)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_ADD_PBK_RSP, ERR_CME_SIM_PIN2_REQUIRED, pDelePBK->nDeleRecInd, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKDelProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
            }
            pPadFData = (UINT8 *)CSW_SIM_MALLOC(pDelePBK->nPadFDataSize);
            if (pPadFData == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d42));
                CFW_PostNotifyEvent(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP, ERR_NO_MORE_MEMORY, pDelePBK->nDeleRecInd,
                                    nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKDelProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                hAo = HNULL;
                return ERR_SUCCESS;

            }
            SUL_MemSet8(pPadFData, 0xFF, (UINT16)(pDelePBK->nPadFDataSize));
            if (pDelePBK->nCurrentFile == API_SIM_EF_LND)
            {
                result = SimUpdateRecordReq(pDelePBK->nCurrentFile, 0x00, 0x03, pDelePBK->nPadFDataSize, pPadFData, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUpdateRecordReq return 0x%x \n",0x08100d43), result);

                    CFW_PostNotifyEvent(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP, (UINT32)result, pDelePBK->nDeleRecInd,
                                        nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKDelProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
            }
            else
            {
                if((pDelePBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pDelePBK->nCurrentFile == API_USIM_EF_ADN))
                {
                    UINT8 EFID = getADNEFID(pDelePBK->nDeleRecInd, nSimID);
                    UINT8 nRecordNumber = getADNRecordNumber(pDelePBK->nDeleRecInd, nSimID);
                    result = SimUpdateRecordReq(EFID, nRecordNumber, 0x04, pDelePBK->nPadFDataSize, pPadFData, nSimID);
                }
                else
                    result = SimUpdateRecordReq(pDelePBK->nCurrentFile, pDelePBK->nDeleRecInd, 0x04, pDelePBK->nPadFDataSize, pPadFData, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimUpdateRecordReq return 0x%x \n",0x08100d44), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP, (UINT32)result, pDelePBK->nDeleRecInd,
                                        nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKDelProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
            }

            CSW_SIM_FREE(pPadFData);
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
            pDelePBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
            pDelePBK->nState.nNextState = CFW_SIM_PBK_UPDATERECORD;
        }
        break;

        case CFW_SIM_PBK_UPDATERECORD:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_UPDATERECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimUpdateRecordCnf = (api_SimUpdateRecordCnf_t *)nEvParam;
            if (((0x90 == pSimUpdateRecordCnf->Sw1) && (0x00 == pSimUpdateRecordCnf->Sw2))
                    || (0x91 == pSimUpdateRecordCnf->Sw1))
            {
                CFW_PostNotifyEvent(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP, (UINT32)pDelePBK->nDeleRecInd, 0, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_DELETE_PBK_ENTRY_RSP Success! \n",0x08100d45)));
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimUpdateRecordCnf->Sw1, pSimUpdateRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_DELETE_PBK_ENTRY_RSP, (UINT32)ErrorCode, pDelePBK->nDeleRecInd,
                                    nUTI | (nSimID << 24), 0xF0);
            }

            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKDelProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        default:
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimPBKEntryProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId    = 0;
    UINT32 ErrorCode = 0;
    UINT8 txtlength  = 0;
    UINT32 result    = 0;
    VOID *nEvParam   = NULL;
    CFW_EV ev;

    CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = NULL;
    SIM_SM_INFO_GETPBK *pGetPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    api_SimReadRecordCnf_t *pSimReadRecordCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKEntryProc);

    UINT32 nUTI = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pGetPBK = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                 = 0xff;
        pEvent                     = &ev;
        pGetPBK->nState.nNextState = CFW_SIM_PBK_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKEntryProc) Status: %d File: %d",0x08100d46), pGetPBK->nState.nNextState, pGetPBK->nCurrentFile);
    switch (pGetPBK->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
            switch (pGetPBK->nCurrentFile)
            {
                case API_SIM_EF_ADN:
                    pGetPBK->nRecordSize = pSimPBKPara->nADNRecordSize;
                    break;
                case API_USIM_EF_GB_ADN:
                case API_USIM_EF_GB_ADN1:
                case API_USIM_EF_ADN:
                case API_USIM_EF_ADN1:
                    pGetPBK->nRecordSize = getADNRecordSize(pGetPBK->nCurrRecInd, nSimID);
                    break;
                case API_SIM_EF_FDN:
                case API_USIM_EF_FDN:
                    pGetPBK->nRecordSize = pSimPBKPara->nFDNRecordSize;
                    break;
                case API_SIM_EF_BDN:
                case API_USIM_EF_BDN:
                    pGetPBK->nRecordSize = pSimPBKPara->nBDNRecordSize - 1;
                    break;
                case API_SIM_EF_SDN:
                case API_USIM_EF_SDN:
                    pGetPBK->nRecordSize = pSimPBKPara->nSDNRecordSize;
                    break;
                case API_SIM_EF_MSISDN:
                case API_USIM_EF_MSISDN:
                    pGetPBK->nRecordSize = pSimPBKPara->nMSISDNRecordSize;
                    break;
                case API_SIM_EF_LND:
                    pGetPBK->nRecordSize = pSimPBKPara->nLNDRecordSize;
                    break;
                case API_SIM_EF_ECC:
                    pGetPBK->nRecordSize = pSimPBKPara->nECCTotalNum;
                    break;
                case API_USIM_EF_ECC:
                    pGetPBK->nRecordSize = pSimPBKPara->nECCRecordSize;
                    break;
                default:
                    break;
            }
            if (pGetPBK->nCurrentFile == API_SIM_EF_ECC)
            {
                result = SimReadBinaryReq(pGetPBK->nCurrentFile, 0, pGetPBK->nRecordSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100d47), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pGetPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pGetPBK->nState.nNextState = CFW_SIM_PBK_READBINARY;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("CFW_SimPBKEntryProc: nCurrentFile=%x, Sim Type = %d",0x08100d48)), pGetPBK->nCurrentFile, CFW_GetSimType(nSimID));
                if((pGetPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pGetPBK->nCurrentFile == API_USIM_EF_ADN))
                {
                    UINT8 EFID = getADNEFID(pGetPBK->nCurrRecInd, nSimID);
                    UINT8 nRecordNumber = getADNRecordNumber(pGetPBK->nCurrRecInd, nSimID);
                    UINT8 nRecordSize = getADNRecordSize(pGetPBK->nCurrRecInd, nSimID);
                    result = SimReadRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, nSimID);
                }
                else
                    result = SimReadRecordReq(pGetPBK->nCurrentFile, pGetPBK->nCurrRecInd, 0x04, pGetPBK->nRecordSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pGetPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pGetPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
        }
        break;

        case CFW_SIM_PBK_READRECORD:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            //
            if (API_SIM_READRECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
                if (pGetPBKEntry == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d49));
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                SUL_MemSet8(pGetPBKEntry, 0, (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE)); // hameina[+] 20081006, zero memery
                pGetPBKEntry->pNumber   = (UINT8 *)pGetPBKEntry + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry->pFullName = (UINT8 *)pGetPBKEntry + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);

                SUL_ZeroMemory8(pGetPBKEntry->pFullName, SIM_PBK_NAME_SIZE);
                if(pGetPBK->nCurrentFile == API_USIM_EF_ECC)
                {
                    SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadRecordCnf->Data, 3);
                    pGetPBKEntry->nNumberSize = 3;
                    pGetPBKEntry->nType = CFW_TELNUMBER_TYPE_NATIONAL;
                    pGetPBKEntry->phoneIndex = pGetPBK->nCurrRecInd;
                    if(pGetPBK->nRecordSize > 4)
                    {
                        pGetPBKEntry->iFullNameSize = pGetPBK->nRecordSize - 4;
                        SUL_MemCopy8(pGetPBKEntry->pFullName, &pSimReadRecordCnf->Data[3], pGetPBKEntry->iFullNameSize);
                    }
                    else
                        pGetPBKEntry->iFullNameSize = 0;
                }
                else
                {
                    txtlength = pGetPBK->nRecordSize - CFW_SIM_REMAINLENGTH;
                    cfw_SimParsePBKRecData(pSimReadRecordCnf->Data, pGetPBKEntry, pGetPBK, txtlength, API_GETPBK);
                }

                if ((pGetPBKEntry->iFullNameSize == 0) && (pGetPBKEntry->nNumberSize == 0) && (pGetPBKEntry->nType != CFW_TELNUMBER_TYPE_INTERNATIONAL))  // updated on 20070618 to read "+" entry
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)NULL, pGetPBK->nCurrRecInd, nUTI | (nSimID << 24), 0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("post EV_CFW_SIM_GET_PBK_ENTRY_RSP Success[Record NULL]\n",0x08100d4a)));
                    CSW_SIM_FREE(pGetPBKEntry);
                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry,
                                        ((UINT16)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE) << 16) +
                                        (UINT16)pGetPBK->nCurrRecInd, nUTI | (nSimID << 24), 0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PBK_ENTRY_NOTIFY Success\n",0x08100d4b)));
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR(" NameSize = %d,NumberSize = %d\n",0x08100d4c)),
                              (UINT32)pGetPBKEntry->iFullNameSize, (UINT32)pGetPBKEntry->nNumberSize);

                }
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)ErrorCode, pGetPBK->nCurrRecInd, nUTI | (nSimID << 24), 0xF0);
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        case CFW_SIM_PBK_READBINARY:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READBINARY_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
            if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
            {
                if (pGetPBK->nCurrRecInd > 5)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, ERR_CFW_INVALID_PARAMETER, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }

                pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
                if (pGetPBKEntry == NULL)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d4d));
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                SUL_ZeroMemory32(pGetPBKEntry, SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
                pGetPBKEntry->pNumber   = (UINT8 *)pGetPBKEntry + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry->pFullName = (UINT8 *)pGetPBKEntry + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);
                SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadBinaryCnf->Data + (pGetPBK->nCurrRecInd) * 3, 3);
                pGetPBKEntry->nNumberSize   = 3;
                pGetPBKEntry->iFullNameSize = 0;
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry,
                                    ((UINT16)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE) << 16) +
                                    (UINT16)pGetPBK->nCurrRecInd, nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PBK_ENTRY_RSP Success! \n",0x08100d4e)));
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_ENTRY_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        default:
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            break;
    }
    return ERR_SUCCESS;
}

#define SIM_MODE_ABSOLUTE       0x04
#define SIM_SIMPLE_SEARCH       0x04
#define SIM_ENHANCED_SEARCH     0x06
#define SIM_SEARCH_MODE         0x04
UINT8 getLengthPbkAlpha(SIM_SEARCH_PBK* pSearchPbk, api_SimReadRecordCnf_t *pSimReadRecordCnf)
{
    UINT8 i;
    for(i = 0; i < pSearchPbk->nRecordLength - 14; i++)
    {
        if(pSimReadRecordCnf->Data[i] == 0xFF)
            break;
    }
    return i;
}
UINT8 getLengthPbkNumber(SIM_SEARCH_PBK* pSearchPbk, api_SimReadRecordCnf_t *pSimReadRecordCnf)
{
#if 0    
    UINT8 i;
    for(i = 0; i < 10; i++)
    {
        if(pSimReadRecordCnf->Data[i + pSearchPbk->nRecordLength - 12] == 0xFF)
            break;
    }
    return i;
#else
    return pSimReadRecordCnf->Data[pSearchPbk->nRecordLength - 14];
#endif
}
UINT8 getPbkTonNpi(SIM_SEARCH_PBK* pSearchPbk, api_SimReadRecordCnf_t *pSimReadRecordCnf)
{
    return pSimReadRecordCnf->Data[pSearchPbk->nRecordLength - 13];
}


#define FCP_TEMPLATE_TAG        0x62
#define FILE_DESCRIPTOR_TAG     0x82
UINT8 getNextRecordNb(SIM_SEARCH_PBK* pSearchPbk, CFW_SIM_ID nSimID)
{
    if(CFW_GetSimType(nSimID) == 1)     //for USIM card, nRecordIndex is the index of the image
    {
        UINT8 last = pSearchPbk->nLastIndex;
        UINT8 index = pSearchPbk->nRecordIndex;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "index = %d, last = %d, ", index, last);
        if(pSearchPbk->nImage[index] < last)
        {
            pSearchPbk->nRecordIndex++;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pSearchPbk->nImage[index + 1] = %d", pSearchPbk->nImage[index + 1]);
            return pSearchPbk->nImage[index + 1];
        }
    }
    else        //for SIM card, nRecordIndex is the record number
    {
        UINT8 last = pSearchPbk->nLastRecordNb;
        UINT8 index = pSearchPbk->nRecordIndex;
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "index = %d, last = %d, ", index, last);
        if(index < last)
        {
            pSearchPbk->nRecordIndex++;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), " pSearchPbk->nRecordIndex = %d",  pSearchPbk->nRecordIndex);
            return pSearchPbk->nRecordIndex;
        }
    }
    return 0xFF;
}



UINT32 savePbkInfo(SIM_SEARCH_PBK* pSearchPbk, api_SimReadRecordCnf_t *pSimReadRecordCnf, CFW_SIM_ID nSimID)
{
    if(pSearchPbk == NULL)
        return ERR_CFW_NOT_SUPPORT;
    
    UINT8 nNumber[21]={0};
    UINT8 lengthAl = getLengthPbkAlpha(pSearchPbk, pSimReadRecordCnf);
    UINT8 lengthNb = getLengthPbkNumber(pSearchPbk, pSimReadRecordCnf);     //length is bcd format
    if((lengthNb > 20) && (lengthNb != 0xFF))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--->>lengthAl = %d(>20), lengthNb =%d", lengthAl, lengthNb);
        return ERR_CFW_NOT_SUPPORT;
    }
    UINT8* pData = pSimReadRecordCnf->Data + pSearchPbk->nRecordLength - 12;
    if(lengthNb != 0xFF)
        lengthNb = SUL_gsmbcd2ascii(pData, lengthNb, nNumber);  //length is ascii format
    else
        lengthNb = 0;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--->>lengthAl = %d, lengthNb =%d", lengthAl, lengthNb);

    UINT16 memsize = 0;
    CFW_SIM_SEARCH_PBK* p = pSearchPbk->pPbkInfo;
    if(p == NULL)
    {
        memsize = sizeof(CFW_SIM_SEARCH_PBK) + 2/*nIndex*/ + 1/*nType*/ + lengthAl + lengthNb + 2;
        pSearchPbk->pPbkInfo = CSW_SIM_MALLOC(memsize);
        if(pSearchPbk->pPbkInfo == NULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--->>No more memory(%d)!", memsize);
            return ERR_NO_MORE_MEMORY;
        }
        pSearchPbk->nLengthData = 0;
        pSearchPbk->pPbkInfo->nUsedRecords = 0;
        pSearchPbk->pPbkInfo->nMatchRecords = 0;
        pSearchPbk->pPbkInfo->nTotalRecords = pSearchPbk->nLastRecordNb;
    }
    else
    {
        memsize = sizeof(CFW_SIM_SEARCH_PBK) + pSearchPbk->nLengthData + 2/*nIndex*/ + 1/*nType*/ + lengthAl + lengthNb + 2;
        pSearchPbk->pPbkInfo = CSW_SIM_MALLOC(memsize);
        if(pSearchPbk->pPbkInfo == NULL)
        {
            CSW_SIM_FREE(p);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--->>No more memory(%d)!", memsize);
            return ERR_NO_MORE_MEMORY;
        }

        memcpy(pSearchPbk->pPbkInfo, p, sizeof(CFW_SIM_SEARCH_PBK) + pSearchPbk->nLengthData);
        CSW_SIM_FREE(p);
        //CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->pPbkAlpha, 50, 16);
        //pSearchPbk->pPbkInfo->nTotalRecords += pSearchPbk->nLastRecordNb;
    }
    UINT16 offset = pSearchPbk->nLengthData;
    p = pSearchPbk->pPbkInfo;
    
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--->> offset = %d, lengthAl = %d, lengthNb =%d", offset, lengthAl, lengthNb);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, p->nData, 50, 16);
    //Save an index, offset points to the location of an index
    if(CFW_GetSimType(nSimID) == 1)
    {
        UINT8 n = getADNMaxRecord(pSearchPbk->nIndexADN - 1, nSimID);
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--->> getADNMaxRecord = %d, pSearchPbk->nImage[pSearchPbk->nRecordIndex] = %d",n, pSearchPbk->nImage[pSearchPbk->nRecordIndex]);

        if(pSearchPbk->nIndexADN == 0)
            *(UINT16*)(p->nData + offset) = pSearchPbk->nImage[pSearchPbk->nRecordIndex];
        else
            *(UINT16*)(p->nData + offset) = n + pSearchPbk->nImage[pSearchPbk->nRecordIndex];
    }
    else
    {
        *(UINT16*)(p->nData + offset) = pSearchPbk->nRecordIndex;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--p->nData = %x ,(p->nData + offset) = %x", p->nData, (p->nData + offset));
    offset += 2;
    
    p->nData[offset++] = getPbkTonNpi(pSearchPbk, pSimReadRecordCnf); //TonNpi
    
    p->nData[offset++] = lengthAl;
     CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "---lengthAl = %d", lengthAl);
    memcpy(p->nData + offset, pSimReadRecordCnf->Data, lengthAl);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "---lengthNb = %d", lengthNb);

    offset += lengthAl;
    p->nData[offset++] = lengthNb;
    memcpy(p->nData + offset, nNumber, lengthNb);
    pSearchPbk->nLengthData = offset + lengthNb;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "---pSearchPbk->nLengthData = %d", pSearchPbk->nLengthData);
    if((pSearchPbk->nLengthData & 0x01) != 0)
        pSearchPbk->nLengthData++;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-dd--pSearchPbk->nLengthData = %d", pSearchPbk->nLengthData);
    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, p->nData, 50, 16);
    p->nMatchRecords++;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "---pData2--nUsedRecords-%d", p->nMatchRecords);
    return 0;
}

UINT32 CFW_SimSearchPbkProc(HAO hAo, CFW_EV *pEvent)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSearchPbkProc);
    UINT32 Result = 0;
    UINT32 nUTI = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    SIM_SEARCH_PBK* pSearchPbk = CFW_GetAoUserData(hAo);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "(CFW_SimSearchPbkProc) Status: %d File: %d", pSearchPbk->nState, pSearchPbk->nEF);
    switch (pSearchPbk->nState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            Result = SimElemFileStatusReq(pSearchPbk->nEF, nSimID);   //to get Recordsize
            if(ERR_SUCCESS != Result)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimElemFileStatusReq ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
ERROR_PROCESS:
                CFW_PostNotifyEvent(EV_CFW_SIM_SEARCH_PBK_RSP, Result, 0, nUTI | (nSimID << 24), 0xF0);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimSearchPbkProc);
                return Result;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            pSearchPbk->nState = CFW_SIM_PBK_READELEMENT;
            break;
        }
        case CFW_SIM_PBK_READELEMENT:
        {
            if (pEvent->nEventId != API_SIM_ELEMFILESTATUS_CNF)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }
            api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)pEvent->nParam1;

            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimElemFileStatusCnf sw1 = %x, sw2 = %x",pSimElemFileStatusCnf->Sw1,
                                        pSimElemFileStatusCnf->Sw2);
            if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2))
                    || (0x91 == pSimElemFileStatusCnf->Sw1))
            {
                UINT8 *pResponse = pSimElemFileStatusCnf->Data;
                UINT8 offset = 0;
                //CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimElemFileStatusCnf->Data, 50, 16);
                if(CFW_GetSimType(nSimID) == 1)     //USIM card
                {
                    if(pResponse[offset++] != FCP_TEMPLATE_TAG)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimElemFileStatusCnf Format ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                        Result = ERR_CFW_NOT_SUPPORT;
                        goto ERROR_PROCESS;
                    }
                    offset++;
                    if(pResponse[offset++] != FILE_DESCRIPTOR_TAG)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "FILE_DESCRIPTOR_TAG");
                        Result = ERR_CFW_NOT_SUPPORT;
                        goto ERROR_PROCESS;
                    }
                    if((pResponse[offset] != 2) && (pResponse[offset] != 5))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "FILE_DESCRIPTOR_TAG2");
                        Result = ERR_CFW_NOT_SUPPORT;
                        goto ERROR_PROCESS;
                    }
                    offset += 2;
                    if(pResponse[offset++] != 0x21)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "FILE_DESCRIPTOR_TAG3");
                        Result = ERR_CFW_NOT_SUPPORT;
                        goto ERROR_PROCESS;
                    }
                    pSearchPbk->nRecordLength = (pResponse[offset]<< 8) | pResponse[offset + 1];
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->nRecordLength = %d", pSearchPbk->nRecordLength);
                    pSearchPbk->nLastRecordNb = pResponse[offset + 2];
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->nLastRecordNb = %d", pSearchPbk->nLastRecordNb);

                    UINT8 nLength = strlen(pSearchPbk->nPattern);
                    
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "===2===nLength=%d=====", nLength);
                    UINT8 nMode = SIM_SIMPLE_SEARCH;
                    if(pSearchPbk->nObject == SEEK_SIM_NAME)
                    {
                        if((pSearchPbk->nPattern[0] == 0xFF) && (pSearchPbk->nPattern[1] == 0))
                        {
                            memset(pSearchPbk->nPattern, 0xFF, pSearchPbk->nRecordLength - 14);
                            pSearchPbk->nPattern[pSearchPbk->nRecordLength - 14] = 0;
                            nLength = strlen(pSearchPbk->nPattern);
                            
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "===3===nLength=%d=====", nLength);
                        }
                        else if(strlen(pSearchPbk->nPattern) > (pSearchPbk->nRecordLength - 14))
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "It is too long that the name you search for!");
                            Result = ERR_CFW_NOT_SUPPORT;
                            goto ERROR_PROCESS;
                        }
                    }
                    else if(pSearchPbk->nObject == SEEK_SIM_USED)
                    {
                        if(SIM_PBK_RECORDER_MAX_LEN < pSearchPbk->nRecordLength)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "the length of the recorder is too long!");
                            Result = ERR_CFW_NOT_SUPPORT;
                            goto ERROR_PROCESS;
                        }
                        memset(pSearchPbk->nPattern, 0xFF, pSearchPbk->nRecordLength);
                        pSearchPbk->nPattern[pSearchPbk->nRecordLength] = 0;
                        nLength = pSearchPbk->nRecordLength;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------nLength = %d-------", nLength);
                        CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nPattern, nLength, 16);
                    }
                    else
                    {
                        if((pSearchPbk->nPattern[0] == 0xFF) && (pSearchPbk->nPattern[1] == 0))
                        {
                            memset(pSearchPbk->nPattern + 2, 0xFF, 10);
                            pSearchPbk->nPattern[12] = 0;
                        }
                        pSearchPbk->nPattern[0] = SIM_SEARCH_MODE;
                        pSearchPbk->nPattern[1] = pSearchPbk->nRecordLength - 12;
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "======pattern======");
                        CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nPattern, strlen(pSearchPbk->nPattern), 16);
                        nMode = SIM_ENHANCED_SEARCH;
                        nLength = strlen(pSearchPbk->nPattern);
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "======nLength=%d=====", nLength);

                    Result = SimSeekAllReq(pSearchPbk->nEF, nMode, nLength, pSearchPbk->nPattern, nSimID);
                    if(Result != ERR_SUCCESS)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimSeekAllReq ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                        goto ERROR_PROCESS;
                    }
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                    pSearchPbk->nState= CFW_SIM_PBK_SEEK;
                }
                else    //Sim Card
                {
                    pSearchPbk->nRecordLength = pResponse[14];
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->nRecordLength = %d", pSearchPbk->nRecordLength);

                    if(pSearchPbk->nRecordLength == 0)
                        goto ERROR_PROCESS;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "file size = %x, %x", pResponse[2], pResponse[3]);
                    pSearchPbk->nLastRecordNb = (pResponse[2] << 8) | pResponse[3];
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->nLastRecordNb = %d", pSearchPbk->nLastRecordNb);
                    pSearchPbk->nLastRecordNb = pSearchPbk->nLastRecordNb / pSearchPbk->nRecordLength;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->nRecordLength = %d, pSearchPbk->nLastRecordNb = %d",
                                   pSearchPbk->nRecordLength, pSearchPbk->nLastRecordNb);
                    if((pSearchPbk->nPattern[0] == 0xFF) && (pSearchPbk->nPattern[1] == 0))
                    {
                        UINT8 nLength = 0;
                        if(pSearchPbk->nObject == SEEK_SIM_USED)
                            nLength = pSearchPbk->nRecordLength;        //search null record
                        else
                            nLength = pSearchPbk->nRecordLength - 14;   //search alpha is NULL
                        memset(pSearchPbk->nPattern, 0xFF, nLength);
                        pSearchPbk->nPattern[nLength] = 0;
                    }
READ_RECORED:
                    Result = SimReadRecordReq(pSearchPbk->nEF, pSearchPbk->nRecordIndex, SIM_MODE_ABSOLUTE, pSearchPbk->nRecordLength, nSimID);
                    if(Result != ERR_SUCCESS)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimReadRecordReq ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                        goto ERROR_PROCESS;
                    }
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                    pSearchPbk->nState= CFW_SIM_PBK_READRECORD;
                }
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSimElemFileStatusCnf ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                Result = ERR_CFW_NOT_SUPPORT;
                goto ERROR_PROCESS;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
            break;
        }
        case CFW_SIM_PBK_SEEK:
        {
            if (pEvent->nEventId != API_SIM_SEEK_ALL_CNF)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }
            api_SimSeekAllCnf_t *pSimSeekAllCnf = (api_SimSeekAllCnf_t*)pEvent->nParam1;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------SEEK sw1 = %x, sw2 = %x-------", pSimSeekAllCnf->Sw1, pSimSeekAllCnf->Sw2);
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------pSimSeekAllCnf->length = %d-------", pSimSeekAllCnf->length);
            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimSeekAllCnf->Record, pSimSeekAllCnf->length, 16);
            if(pSearchPbk->pPbkInfo == NULL)
            {
                pSearchPbk->pPbkInfo = CSW_SIM_MALLOC(sizeof(CFW_SIM_SEARCH_PBK));
                if(pSearchPbk->pPbkInfo == NULL)
                {
                    Result = ERR_CFW_NOT_SUPPORT;
                    goto ERROR_PROCESS;
                }
                pSearchPbk->pPbkInfo->nUsedRecords = 0;
                pSearchPbk->pPbkInfo->nMatchRecords = 0;
                pSearchPbk->pPbkInfo->nTotalRecords = 0;
                pSearchPbk->nLengthData = 0;
            }
            
            if (((0x90 == pSimSeekAllCnf->Sw1) && (0x00 == pSimSeekAllCnf->Sw2))
                    || (0x91 == pSimSeekAllCnf->Sw1))
            {
                CFW_SIM_SEARCH_PBK* p = pSearchPbk->pPbkInfo;
                //p->nUsedRecords += pSimSeekAllCnf->length;
                p->nTotalRecords += pSearchPbk->nLastRecordNb;
#if 0
                if(pSearchPbk->nObject == SEEK_SIM_USED)   //only for USIM
                {
                    p->nUsedRecords += pSearchPbk->nLastRecordNb - pSimSeekAllCnf->length;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SEEK pSearchPbk->pPbkInfo = %x, p->nNum = %d, p->nTotalNum = %d",
                        pSearchPbk->pPbkInfo, p->nUsedRecords, p->nTotalRecords);
                    if((pSearchPbk->nEF == API_USIM_EF_GB_ADN) || (pSearchPbk->nEF == API_USIM_EF_ADN))
                        goto SEEK_AGAIN;
                    else
                        goto SEND_EVENT;
                }
                else
#endif
                if(pSearchPbk->nObject == SEEK_SIM_USED)   //only for USIM
                {
                    UINT8 nRecords[255];
                    UINT8 i = 0;
                    UINT8 j = 0;
                    UINT8 k = 0;
                    for(i = 0; i < pSearchPbk->nLastRecordNb; i++)
                    {
                        if(k >= pSimSeekAllCnf->length)
                        {
                            nRecords[j] = i + 1;
                            j++;
                        }
                        if(pSimSeekAllCnf->Record[k] > i + 1)
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "j--%d, i+1--%d", j , i+1);
                            nRecords[j] = i + 1;
                            j++;
                        }
                        else
                            k++;

                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "j--%d, i+1--%d, k = %d", j , i+1, k);
                    }

                    pSimSeekAllCnf->length = pSearchPbk->nLastRecordNb - pSimSeekAllCnf->length;
                    memcpy(pSimSeekAllCnf->Record, nRecords, pSimSeekAllCnf->length);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------update pSimSeekAllCnf->length = %d-------", pSimSeekAllCnf->length);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimSeekAllCnf->Record, pSimSeekAllCnf->length, 16);
                }
READRECORD:
                {
                    pSearchPbk->nImage[0] += pSimSeekAllCnf->length;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------pSimSeekAllCnf->length = %d", pSimSeekAllCnf->length);

                    //nImage format: first byte is the length of an image, followed bytes are images
                    memcpy(pSearchPbk->nImage + 1, pSimSeekAllCnf->Record, pSimSeekAllCnf->length);
                    pSearchPbk->nImage[pSimSeekAllCnf->length + 1] = 0;
                    pSearchPbk->nLastIndex = pSearchPbk->nImage[pSimSeekAllCnf->length];
                    pSearchPbk->nRecordIndex = 1;     //nRecordIndex is an index of a record in an image.

                    Result = SimReadRecordReq(pSearchPbk->nEF, pSearchPbk->nImage[pSearchPbk->nRecordIndex], SIM_MODE_ABSOLUTE, pSearchPbk->nRecordLength, nSimID);
                    if(Result != ERR_SUCCESS)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimReadRecordReq ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                        goto ERROR_PROCESS;
                    }
                    pSearchPbk->nState= CFW_SIM_PBK_READRECORD;
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                }
            }
            else if(((0x6A == pSimSeekAllCnf->Sw1) && (0x83 == pSimSeekAllCnf->Sw2)) ||
                    ((pSimSeekAllCnf->Sw1 == 0x62) && (pSimSeekAllCnf->Sw2 == 0x82)))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "There are not records matched.");

                pSearchPbk->pPbkInfo->nTotalRecords += pSearchPbk->nLastRecordNb;
#if 0
                if(pSearchPbk->nObject == SEEK_SIM_USED)
                {
                    pSearchPbk->pPbkInfo->nUsedRecords += pSearchPbk->nLastRecordNb;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->pPbkInfo->nUsedRecords = %d,pSearchPbk->pPbkInfo->nTotalRecords = %d",
                        pSearchPbk->pPbkInfo->nUsedRecords, pSearchPbk->pPbkInfo->nTotalRecords);
                }
#endif
                if(pSearchPbk->nObject == SEEK_SIM_USED)   //only for USIM
                {
                    pSimSeekAllCnf->length = pSearchPbk->nLastRecordNb;
                    UINT8 i = 0;
                    for(i = 0; i < pSimSeekAllCnf->length; i++)
                        pSimSeekAllCnf->Record[i] = i + 1;
                    goto READRECORD;
                }

                if(pSearchPbk->nIndexADN == 0)
                    goto SEEK_AGAIN;
                else
                    goto SEND_EVENT;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSimSeekAllCnf ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                Result = ERR_CFW_NOT_SUPPORT;
                goto ERROR_PROCESS;
            }
            break;
        }
        case CFW_SIM_PBK_READRECORD:
        {
            if (pEvent->nEventId != API_SIM_READRECORD_CNF)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            api_SimReadRecordCnf_t *pSimReadRecordCnf = (api_SimReadRecordCnf_t*)pEvent->nParam1;
            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2))
                    || (0x91 == pSimReadRecordCnf->Sw1))
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "---------pSimReadRecordCnf->Data --------");
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimReadRecordCnf->Data, 50, 16);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->nLastRecordNb = %d", pSearchPbk->nLastRecordNb);
                CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nPattern, 50, 16);
                CFW_SIM_SEARCH_PBK* p = pSearchPbk->pPbkInfo;

                if(pSearchPbk->pPbkInfo == NULL)
                {
                    pSearchPbk->pPbkInfo = CSW_SIM_MALLOC(sizeof(CFW_SIM_SEARCH_PBK));
                    if(pSearchPbk->pPbkInfo == NULL)
                    {
                        Result = ERR_CFW_NOT_SUPPORT;
                        goto ERROR_PROCESS;
                    }
                    pSearchPbk->pPbkInfo->nUsedRecords = 0;
                    pSearchPbk->pPbkInfo->nMatchRecords = 0;
                    pSearchPbk->nLengthData = 0;
                    pSearchPbk->pPbkInfo->nTotalRecords = pSearchPbk->nLastRecordNb;
                }

                if(pSearchPbk->nObject == SEEK_SIM_USED)   //only for SIM
                {
                    if(memcmp((UINT8*)pSimReadRecordCnf->Data, (UINT8*)pSearchPbk->nPattern, pSearchPbk->nRecordLength) != 0)
                    {
                        pSearchPbk->pPbkInfo->nUsedRecords++;
                        //pSearchPbk->pPbkInfo->nMatchRecords++;
                        if(savePbkInfo(pSearchPbk, pSimReadRecordCnf, nSimID) != 0)
                        {
                            Result = ERR_CFW_NOT_SUPPORT;
                            goto ERROR_PROCESS;
                        }
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->pPbkInfo->nUsedRecords = %d, pSearchPbk->pPbkInfo->nTotalRecords = %d",
                                  pSearchPbk->pPbkInfo->nUsedRecords, pSearchPbk->pPbkInfo->nTotalRecords);
                    }
                    //read next recorder.
                }
                else if(pSearchPbk->nObject == SEEK_SIM_NUMBER)
                {
                    UINT8* pOffset = pSimReadRecordCnf->Data + pSearchPbk->nRecordLength - 12;
                    UINT8 length = strlen(pSearchPbk->nPattern + 2);
                    UINT8 value = memcmp(pOffset, pSearchPbk->nPattern + 2, length);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "--value = %d------Record: %x-------", value, pOffset[length]);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSearchPbk->nPattern + 2, 50, 16);
                    if(value == 0)  //find number
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "----- pOffset[length] = %x, length = %d ----", pOffset[length], length);
                        if((length == 10) || ((length < 10) && (pOffset[length] == 0xFF)))  //check it and save data
                        {
                            if(savePbkInfo(pSearchPbk, pSimReadRecordCnf, nSimID) != 0)
                            {
                                Result = ERR_CFW_NOT_SUPPORT;
                                goto ERROR_PROCESS;
                            }
                            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID,  pSearchPbk->pPbkInfo, 50, 16);
                            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID,  pSearchPbk->pPbkInfo->nData, 50, 16);
                        }
                    }
                    if((pSimReadRecordCnf->Data[0] != 0xFF) || (pOffset[0] != 0xFF))    //NULL record for SIM
                        pSearchPbk->pPbkInfo->nUsedRecords++;

                }
                else if(pSearchPbk->nObject == SEEK_SIM_NAME)
                {
                    UINT8 lengthAl = getLengthPbkAlpha(pSearchPbk, pSimReadRecordCnf);
                    //UINT8 lengthAl = strlen(pSearchPbk->nPattern);
                    if(lengthAl == 0)
                        lengthAl = pSearchPbk->nRecordLength - 14;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "strlen(pSearchPbk->nPattern) = %d, lengthAl = %x", strlen(pSearchPbk->nPattern), lengthAl);
//                    if((pSearchPbk->nMode == WILDCARD_MATCH) || (strlen(pSearchPbk->nPattern) == lengthAl))
                    {
                        if(memcmp(pSimReadRecordCnf->Data, pSearchPbk->nPattern, strlen(pSearchPbk->nPattern)) == 0)  // find out the name
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "====== save data ======");
                            if(savePbkInfo(pSearchPbk, pSimReadRecordCnf, nSimID) != 0)
                            {
                                Result = ERR_CFW_NOT_SUPPORT;
                                goto ERROR_PROCESS;
                            }
                            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID,  pSearchPbk->pPbkInfo, 50, 16);
                            CSW_TC_MEMBLOCK(CFW_SIM_TS_ID,  pSearchPbk->pPbkInfo->nData, 50, 16);
                        }
                    }
                    UINT8* pOffset = pSimReadRecordCnf->Data + pSearchPbk->nRecordLength - 12;
                    if((pSimReadRecordCnf->Data[0] != 0xFF) || (pOffset[0] != 0xFF))    //Null record for SIM
                        pSearchPbk->pPbkInfo->nUsedRecords++;
                }

                if(pSearchPbk->nMode == FIRST_MATCH)
                {
                    //Send event to MMI
SEND_EVENT:
                    if(pSearchPbk->pPbkInfo != NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Match = %d,", pSearchPbk->pPbkInfo->nMatchRecords);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Used = %d,", pSearchPbk->pPbkInfo->nUsedRecords);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Total = %d,", pSearchPbk->pPbkInfo->nTotalRecords);
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pSearchPbk->pPbkInfo = %x,", pSearchPbk->pPbkInfo);
                    CFW_PostNotifyEvent(EV_CFW_SIM_SEARCH_PBK_RSP, pSearchPbk->pPbkInfo, 0, nUTI | (nSimID << 24), 0);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSearchPbkProc);
                    return ERR_SUCCESS;
                }

                UINT8 nRecordNb = getNextRecordNb(pSearchPbk, nSimID);

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Record nRecordNb = %d, pSearchPbk->nLastRecordNb = %d", nRecordNb, pSearchPbk->nLastRecordNb);
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                if(pSearchPbk->nLastRecordNb >= nRecordNb)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "---------SimReadRecordReq-------");
                    Result = SimReadRecordReq(pSearchPbk->nEF, nRecordNb, SIM_MODE_ABSOLUTE, pSearchPbk->nRecordLength, nSimID);
                    if(Result != ERR_SUCCESS)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimReadRecordReq ERROR in Read recorder #%d!", nRecordNb);
                        goto ERROR_PROCESS;
                    }
                }
                else
                {
SEEK_AGAIN:
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "----pSearchPbk->nEF = %d----", pSearchPbk->nEF);
                    if((CFW_GetSimType(nSimID) == 1) && ((pSearchPbk->nEF == API_USIM_EF_ADN) || (pSearchPbk->nEF == API_USIM_EF_GB_ADN)))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------Seek 2--pSearchPbk->nIndexADN-%d----", pSearchPbk->nIndexADN);
                        if(pSearchPbk->nIndexADN != 1)    //for USIM, max number of ADN does not exceed 2.
                        {
                            pSearchPbk->nIndexADN++;
                            pSearchPbk->nEF = getADNID(pSearchPbk->nIndexADN, nSimID);
                            pSearchPbk->nRecordLength = getADNRecordLength(pSearchPbk->nIndexADN, nSimID);
                            pSearchPbk->nLastRecordNb = getADNMaxRecord(pSearchPbk->nIndexADN, nSimID);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------Seek 2--pSearchPbk->nRecordLength-%d----", pSearchPbk->nRecordLength);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------Seek 2--pSearchPbk->nEF-%d----", pSearchPbk->nEF);
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "-------Seek 2--pSearchPbk->nLastRecordNb-%d----", pSearchPbk->nLastRecordNb);
                            if(pSearchPbk->nEF != 0)
                            {
                                UINT8 nMode = SIM_SIMPLE_SEARCH;
                                if(pSearchPbk->nObject == SEEK_SIM_NUMBER)
                                    nMode = SIM_ENHANCED_SEARCH;
                                Result = SimSeekAllReq(pSearchPbk->nEF, nMode, strlen(pSearchPbk->nPattern), pSearchPbk->nPattern, nSimID);
                                if(Result != ERR_SUCCESS)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "SimSeekAllReq ERROR in Read recorder #%d!", pSearchPbk->nRecordIndex);
                                    goto ERROR_PROCESS;
                                }
                                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
                                pSearchPbk->nState= CFW_SIM_PBK_SEEK;
                                break;
                            }
                        }
                    }
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Finished reading, send event to MMI!");
                    //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "pPbkInfo->num = %d", pSearchPbk->pPbkInfo->nNum);
                    goto SEND_EVENT;
                }
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID), "Read record #%d ERROR(sw1=0x%x,sw2=0x%x)!", pSearchPbk->nRecordIndex, \
                                                           pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2);
                Result = ERR_NOT_SUPPORT;
                goto ERROR_PROCESS;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONSUMED);
        }
        break;
    }

    return ERR_SUCCESS;
}

UINT32 CFW_SimPBKListEntryProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId    = 0;
    UINT32 ErrorCode = 0;
    UINT8 txtlength  = 0;
    UINT32 result    = 0;
    VOID *nEvParam   = NULL;
    CFW_EV ev;

    CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = NULL;
    SIM_SM_INFO_LISTPBK *pListPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    api_SimReadRecordCnf_t *pSimReadRecordCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKListEntryProc);

    UINT32 nUTI      = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pListPBK = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                  = 0xff;
        pEvent                      = &ev;
        pListPBK->nState.nNextState = CFW_SIM_PBK_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKListEntryProc) Status: %d File: %d",0x08100d4f), pListPBK->nState.nNextState, pListPBK->nCurrentFile);
    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
    switch (pListPBK->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            switch (pListPBK->nCurrentFile)
            {
                case API_SIM_EF_ADN:
                    pListPBK->nRecordSize = pSimPBKPara->nADNRecordSize;
                    break;
                case API_USIM_EF_GB_ADN:
                case API_USIM_EF_GB_ADN1:
                case API_USIM_EF_ADN:
                case API_USIM_EF_ADN1:
                    pListPBK->nRecordSize = getADNRecordSize(pListPBK->nCurrRecInd, nSimID);
                    break;
                case API_SIM_EF_FDN:
                case API_USIM_EF_FDN:
                    pListPBK->nRecordSize = pSimPBKPara->nFDNRecordSize;
                    break;
                case API_SIM_EF_BDN:
                case API_USIM_EF_BDN:
                    pListPBK->nRecordSize = pSimPBKPara->nBDNRecordSize - 1;
                    break;
                case API_SIM_EF_SDN:
                case API_USIM_EF_SDN:
                    pListPBK->nRecordSize = pSimPBKPara->nSDNRecordSize;
                    break;
                case API_SIM_EF_MSISDN:
                case API_USIM_EF_MSISDN:
                    pListPBK->nRecordSize = pSimPBKPara->nMSISDNRecordSize;
                    break;
                case API_SIM_EF_LND:
                    pListPBK->nRecordSize = pSimPBKPara->nLNDRecordSize;
                    break;
                case API_SIM_EF_ECC:
                    pListPBK->nRecordSize = pSimPBKPara->nECCTotalNum;
                    break;
                case API_USIM_EF_ECC:
                    pListPBK->nRecordSize = pSimPBKPara->nECCRecordSize;
                    break;
                default:
                    break;
            }
            if (pListPBK->nCurrentFile == API_SIM_EF_ECC)
            {
                result = SimReadBinaryReq(pListPBK->nCurrentFile, 0, pListPBK->nRecordSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100d50), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pListPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pListPBK->nState.nNextState = CFW_SIM_PBK_READBINARY;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pListPBK->nStartRecInd = %d",0x08100d51), pListPBK->nStartRecInd);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADNInfo[0].MaxRecords = %d",0x08100d52), ADNInfo[nSimID][0].MaxRecords);
                if((pListPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pListPBK->nCurrentFile == API_USIM_EF_ADN))
                {
                    UINT8 EFID = getADNEFID(pListPBK->nCurrRecInd, nSimID);
                    UINT8 nRecordNumber = getADNRecordNumber(pListPBK->nCurrRecInd, nSimID);
                    UINT8 nRecordSize = getADNRecordSize(pListPBK->nCurrRecInd, nSimID);
                    result = SimReadRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, nSimID);
                }
                else
                    result = SimReadRecordReq(pListPBK->nCurrentFile, pListPBK->nStartRecInd, 0x04, pListPBK->nRecordSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pListPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pListPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
        }
        break;

        case CFW_SIM_PBK_READRECORD:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READRECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                UINT32 nStructSize = 0x00;

                nStructSize = SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                if (pListPBK->nStartRecInd == pListPBK->nCurrRecInd)
                {
                    UINT16 size =
                        (pListPBK->nEndRecInd - pListPBK->nStartRecInd + 1) * (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
                    pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC(size);
                    if (pGetPBKEntry == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d53));
                        CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_SUCCESS;
                    }
                    pListPBK->nCurrentP = (UINT32)pGetPBKEntry;
                }
                else
                    pGetPBKEntry          = (CFW_SIM_PBK_ENTRY_INFO *)(pListPBK->nCurrentP);

                pGetPBKEntry->pNumber   = (UINT8 *)pGetPBKEntry + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry->pFullName = (UINT8 *)pGetPBKEntry + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);

                SUL_ZeroMemory8(pGetPBKEntry->pFullName, SIM_PBK_NAME_SIZE);
                if(pListPBK->nCurrentFile == API_USIM_EF_ECC)
                {
                    SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadRecordCnf->Data, 3);
                    pGetPBKEntry->nNumberSize = 3;
                    pGetPBKEntry->nType = CFW_TELNUMBER_TYPE_NATIONAL;
                    pGetPBKEntry->phoneIndex = pListPBK->nCurrRecInd;
                    if(pListPBK->nRecordSize > 4)
                    {
                        pGetPBKEntry->iFullNameSize = pListPBK->nRecordSize - 4;
                        SUL_MemCopy8(pGetPBKEntry->pFullName, &pSimReadRecordCnf->Data[3], pGetPBKEntry->iFullNameSize);
                    }
                    else
                        pGetPBKEntry->iFullNameSize = 0;
                }
                else
                {
                    txtlength = pListPBK->nRecordSize - CFW_SIM_REMAINLENGTH;
                    cfw_SimParsePBKRecData(pSimReadRecordCnf->Data, pGetPBKEntry, pListPBK, txtlength, API_LISTPBK);
                }

                if ((pGetPBKEntry->iFullNameSize != 0) || (pGetPBKEntry->nNumberSize != 0) || (pGetPBKEntry->nType == CFW_TELNUMBER_TYPE_INTERNATIONAL))  // updated on 20070618 to read out "+" entry
                {
                    pListPBK->nStructNum++;
                    pListPBK->nCurrentP = (UINT32)(&pGetPBKEntry->iFullNameSize - 8 + nStructSize + SIM_PBK_EXTR_SIZE);
                }
                else if ((pGetPBKEntry->iFullNameSize == 0) && (pGetPBKEntry->nNumberSize == 0) && (pGetPBKEntry->nType != CFW_TELNUMBER_TYPE_INTERNATIONAL)) // updated on 20070618 to read out "+" entry
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" record is NULL\n",0x08100d54)));
                }
                if (pListPBK->nCurrRecInd < pListPBK->nEndRecInd)
                {
                    pListPBK->nCurrRecInd += 1;
                    if((pListPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pListPBK->nCurrentFile == API_USIM_EF_ADN))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pListPBK->nStartRecInd = %d",0x08100d55), pListPBK->nCurrRecInd);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADNInfo[0].MaxRecords = %d",0x08100d56), ADNInfo[nSimID][0].MaxRecords);
                        UINT8 EFID = getADNEFID(pListPBK->nCurrRecInd, nSimID);
                        UINT8 nRecordNumber = getADNRecordNumber(pListPBK->nCurrRecInd, nSimID);
                        UINT8 nRecordSize = getADNRecordSize(pListPBK->nCurrRecInd, nSimID);
                        result = SimReadRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, nSimID);
                    }
                    else
                        result = SimReadRecordReq(pListPBK->nCurrentFile, pListPBK->nCurrRecInd, 0x04, pListPBK->nRecordSize, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadRecordReq return 0x%x \n",0x08100d57), result);

                        CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    pListPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
                    pListPBK->nState.nCurrState = CFW_SIM_PBK_READRECORD;
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                }

                else if (pListPBK->nCurrRecInd == pListPBK->nEndRecInd)
                {
                    pGetPBKEntry =
                        (CFW_SIM_PBK_ENTRY_INFO *)(pListPBK->nCurrentP - (pListPBK->nStructNum - 1) * (nStructSize + SIM_PBK_EXTR_SIZE));
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry, pListPBK->nStructNum - 1,
                                        nUTI | (nSimID << 24), 0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_LIST_PBK_ENTRY_NOTIFY Success! \n",0x08100d58)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)ErrorCode, pListPBK->nCurrRecInd,
                                    nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        break;

        case CFW_SIM_PBK_READBINARY:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READBINARY_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
            if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
            {
                UINT32 nStructSize = 0x00000000;
                UINT8 i            = 0x00;

                nStructSize = SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                if (pListPBK->nStructNum == 1)
                {
                    pGetPBKEntry =
                        (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC((pListPBK->nEndRecInd - pListPBK->nStartRecInd + 1) *
                                (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE));
                    if (pGetPBKEntry == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d59));
                        CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_SUCCESS;
                    }
                    SUL_ZeroMemory32(pGetPBKEntry,
                                     (pListPBK->nEndRecInd - pListPBK->nStartRecInd + 1) * (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE));
                }
                else
                    pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)(pListPBK->nCurrentP);
                for (i = pListPBK->nStartRecInd; i < pListPBK->nEndRecInd + 1; i++)
                {

                    if ((pSimReadBinaryCnf->Data[i * 3] & 0x0F) != 0x0F)
                    {
                        pGetPBKEntry->pNumber   = (UINT8 *)pGetPBKEntry + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                        pGetPBKEntry->pFullName = (UINT8 *)pGetPBKEntry + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO)  + SIM_PBK_NUMBER_SIZE);
                        SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadBinaryCnf->Data + i * 3, 3);
                        pGetPBKEntry->nNumberSize   = 3;
                        pGetPBKEntry->iFullNameSize = 0;
                        pListPBK->nStructNum += 1;
                    }
                }
                pGetPBKEntry =
                    (CFW_SIM_PBK_ENTRY_INFO *)(pListPBK->nCurrentP - (UINT32)((pListPBK->nStructNum - 2) * (nStructSize + SIM_PBK_EXTR_SIZE)));

                CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry, pListPBK->nStructNum - 1,
                                    nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_LIST_PBK_ENTRY_RSP Success! \n",0x08100d5a)));
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_LIST_PBK_ENTRY_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        default:
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimPBKListCountEntryProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId      = 0;
    UINT32 ErrorCode   = 0;
    UINT32 nStructSize = 0;
    UINT8 txtlength    = 0;
    UINT32 result      = 0;
    VOID *nEvParam     = NULL;
    CFW_EV ev;

    CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = NULL;
    SIM_SM_INFO_LISTCOUNTPBK *pListCountPBK = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    api_SimReadRecordCnf_t *pSimReadRecordCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKListCountEntryProc);

    UINT32 nUTI      = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pListCountPBK = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                       = 0xff;
        pEvent                           = &ev;
        pListCountPBK->nState.nNextState = CFW_SIM_PBK_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKListCountEntryProc) Status: %d File: %d",0x08100d5b), pListCountPBK->nState.nNextState, pListCountPBK->nCurrentFile);
    switch (pListCountPBK->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
            switch (pListCountPBK->nCurrentFile)
            {
                case API_SIM_EF_ADN:
                    pListCountPBK->nRecordSize = pSimPBKPara->nADNRecordSize;
                    break;
                case API_USIM_EF_GB_ADN:
                case API_USIM_EF_GB_ADN1:
                case API_USIM_EF_ADN:
                case API_USIM_EF_ADN1:
                    pListCountPBK->nRecordSize = getADNRecordSize(pListCountPBK->nCurrRecInd, nSimID);
                    break;
                case API_SIM_EF_FDN:
                case API_USIM_EF_FDN:
                    pListCountPBK->nRecordSize = pSimPBKPara->nFDNRecordSize;
                    break;
                case API_SIM_EF_BDN:
                case API_USIM_EF_BDN:
                    pListCountPBK->nRecordSize = pSimPBKPara->nBDNRecordSize - 1;
                    break;
                case API_SIM_EF_SDN:
                case API_USIM_EF_SDN:
                    pListCountPBK->nRecordSize = pSimPBKPara->nSDNRecordSize;
                    break;
                case API_SIM_EF_MSISDN:
                case API_USIM_EF_MSISDN:
                    pListCountPBK->nRecordSize = pSimPBKPara->nMSISDNRecordSize;
                    break;
                case API_SIM_EF_LND:
                    pListCountPBK->nRecordSize = pSimPBKPara->nLNDRecordSize;
                    break;
                case API_SIM_EF_ECC:
                    pListCountPBK->nRecordSize = pSimPBKPara->nECCTotalNum;
                    break;
                case API_USIM_EF_ECC:
                    pListCountPBK->nRecordSize = pSimPBKPara->nECCRecordSize;
                    break;
                default:
                    break;
            }
            if (pListCountPBK->nCurrentFile == API_SIM_EF_ECC)
            {
                result = SimReadBinaryReq(pListCountPBK->nCurrentFile, 0, pListCountPBK->nRecordSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadBinaryReq return 0x%x \n",0x08100d5c), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pListCountPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pListCountPBK->nState.nNextState = CFW_SIM_PBK_READBINARY;
            }
            else
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("nCurrentFile = %d\r\n",0x08100d5d)), pListCountPBK->nCurrentFile);
                if((pListCountPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pListCountPBK->nCurrentFile == API_USIM_EF_ADN))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pListCountPBK->nStartRecInd = %d",0x08100d5e), pListCountPBK->nStartRecInd);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADNInfo[0].MaxRecords = %d",0x08100d5f), ADNInfo[nSimID][0].MaxRecords);
                    UINT8 EFID = getADNEFID(pListCountPBK->nCurrRecInd, nSimID);
                    UINT8 nRecordNumber = getADNRecordNumber(pListCountPBK->nCurrRecInd, nSimID);
                    UINT8 nRecordSize = getADNRecordSize(pListCountPBK->nCurrRecInd, nSimID);
                    result = SimReadRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, nSimID);
                }
                else
                    result = SimReadRecordReq(pListCountPBK->nCurrentFile, pListCountPBK->nStartRecInd, 0x04, pListCountPBK->nRecordSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadRecordReq return 0x%x \n",0x08100d60), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pListCountPBK->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pListCountPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
        }
        break;

        case CFW_SIM_PBK_READRECORD:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READRECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            nStructSize       = SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                if (pListCountPBK->nStartRecInd == pListCountPBK->nCurrRecInd)
                {
                    UINT16 size = (pListCountPBK->nCount) * (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);

                    pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC(size);

                    if (pGetPBKEntry == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d61));
                        CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_SUCCESS;
                    }
                    SUL_ZeroMemory32(pGetPBKEntry, size);
                    pListCountPBK->nCurrentP = (UINT32)pGetPBKEntry;
                }
                else
                    pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)(pListCountPBK->nCurrentP);

                pGetPBKEntry->pNumber   = (UINT8*)pGetPBKEntry+ SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry->pFullName = (UINT8*)pGetPBKEntry+ SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE;
                SUL_ZeroMemory8(pGetPBKEntry->pFullName, SIM_PBK_NAME_SIZE);

                if(pListCountPBK->nCurrentFile == API_USIM_EF_ECC)
                {
                    SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadRecordCnf->Data, 3);
                    pGetPBKEntry->nNumberSize = 3;
                    pGetPBKEntry->nType = CFW_TELNUMBER_TYPE_NATIONAL;
                    pGetPBKEntry->phoneIndex = pListCountPBK->nCurrRecInd;
                    if(pListCountPBK->nRecordSize > 4)
                    {
                        pGetPBKEntry->iFullNameSize = pListCountPBK->nRecordSize - 4;
                        SUL_MemCopy8(pGetPBKEntry->pFullName, &pSimReadRecordCnf->Data[3], pGetPBKEntry->iFullNameSize);
                    }
                    else
                        pGetPBKEntry->iFullNameSize = 0;
                }
                else
                {
                    txtlength = pListCountPBK->nRecordSize - CFW_SIM_REMAINLENGTH;
                    cfw_SimParsePBKRecData(pSimReadRecordCnf->Data, pGetPBKEntry, pListCountPBK, txtlength, API_LISTPBK);
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pGetPBKEntry->nNumberSize=%x\r\n",0x08100d62)), pGetPBKEntry->nNumberSize);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pListCountPBK->nStructNum is %d \n",0x08100d63)), pListCountPBK->nStructNum);

                if ((pGetPBKEntry->iFullNameSize != 0) || (pGetPBKEntry->nNumberSize != 0) || (pGetPBKEntry->nType == CFW_TELNUMBER_TYPE_INTERNATIONAL))  // updated on 20070618 to read out "+" entry
                {
                    pListCountPBK->nStructNum++;
                    pListCountPBK->nCurrentP = (UINT32)(&pGetPBKEntry->iFullNameSize - 8 + nStructSize + SIM_PBK_EXTR_SIZE);
                }
                else if ((pGetPBKEntry->iFullNameSize == 0) && (pGetPBKEntry->nNumberSize == 0) && (pGetPBKEntry->nType != CFW_TELNUMBER_TYPE_INTERNATIONAL)) // updated on 20070618 to read out "+" entry
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("record is NULL! \n",0x08100d64)));
                }

                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("pListCountPBK->nStructNum is %d \n",0x08100d65)), pListCountPBK->nStructNum);

                if (pListCountPBK->nStructNum < pListCountPBK->nCount)
                {
                    pListCountPBK->nCurrRecInd += 1;
                    if((pListCountPBK->nCurrentFile == API_USIM_EF_GB_ADN) || (pListCountPBK->nCurrentFile == API_USIM_EF_ADN))
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("pListCountPBK->nStartRecInd = %d",0x08100d66), pListCountPBK->nCurrRecInd);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("ADNInfo[0].MaxRecords = %d",0x08100d67), ADNInfo[nSimID][0].MaxRecords);
                        UINT8 EFID = getADNEFID(pListCountPBK->nCurrRecInd, nSimID);
                        UINT8 nRecordNumber = getADNRecordNumber(pListCountPBK->nCurrRecInd, nSimID);
                        UINT8 nRecordSize = getADNRecordSize(pListCountPBK->nCurrRecInd, nSimID);
                        result = SimReadRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, nSimID);
                    }
                    else
                        result = SimReadRecordReq(pListCountPBK->nCurrentFile, pListCountPBK->nCurrRecInd, 0x04, pListCountPBK->nRecordSize, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xf0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }

                    pListCountPBK->nState.nNextState = CFW_SIM_PBK_READRECORD;
                    pListCountPBK->nState.nCurrState = CFW_SIM_PBK_READRECORD;
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                }
                else if (pListCountPBK->nStructNum == pListCountPBK->nCount)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR(" pListCountPBK->nStructNum is %d \n",0x08100d68)), pListCountPBK->nStructNum);
                    pGetPBKEntry =
                        (CFW_SIM_PBK_ENTRY_INFO *)(pListCountPBK->nCurrentP -
                                                   (pListCountPBK->nStructNum) * (nStructSize + SIM_PBK_EXTR_SIZE));

                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry, pListCountPBK->nStructNum,
                                        nUTI | (nSimID << 24), 0);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP Success! \n",0x08100d69)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    hAo = HNULL;
                }
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, nSimID);
                if (ErrorCode == ERR_CME_INVALID_INDEX)
                {
                    pGetPBKEntry =
                        (CFW_SIM_PBK_ENTRY_INFO *)(pListCountPBK->nCurrentP -
                                                   (pListCountPBK->nStructNum) * (nStructSize + SIM_PBK_EXTR_SIZE));
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry, pListCountPBK->nStructNum,
                                        nUTI | (nSimID << 24), 0);
                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)ErrorCode, pListCountPBK->nCurrRecInd,
                                        nUTI | (nSimID << 24), 0xF0);
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        break;
        case CFW_SIM_PBK_READBINARY:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READBINARY_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }
            pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;
            if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
            {
                nStructSize = 0;
                UINT8 i = 0;

                nStructSize  = SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry =
                    (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC((pListCountPBK->nCount) *
                            (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE));
                if (pGetPBKEntry == NULL)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;

                }
                SUL_ZeroMemory32(pGetPBKEntry, (pListCountPBK->nCount) * (SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE));

                for (i = 0; i < pListCountPBK->nCount; i++)
                {

                    if ((pSimReadBinaryCnf->Data[i * 3] & 0x0F) != 0x0F)
                    {
                        pGetPBKEntry->pNumber   = (UINT8*)pGetPBKEntry+ SIZEOF(CFW_SIM_PBK_ENTRY_INFO)  ;
                        pGetPBKEntry->pFullName = (UINT8*)pGetPBKEntry+ SIZEOF(CFW_SIM_PBK_ENTRY_INFO)  + SIM_PBK_NUMBER_SIZE;
                        SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadBinaryCnf->Data + i * 3, 3);
                        pGetPBKEntry->nNumberSize   = 3;
                        pGetPBKEntry->iFullNameSize = 0;
                        pListCountPBK->nStructNum += 1;
                        pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)(&pGetPBKEntry->iFullNameSize - 8 + nStructSize + SIM_PBK_EXTR_SIZE);

                    }
                }
                pGetPBKEntry =
                    (CFW_SIM_PBK_ENTRY_INFO *)(&pGetPBKEntry->iFullNameSize - 8 -
                                               (UINT32)((pListCountPBK->nStructNum) * (nStructSize + SIM_PBK_EXTR_SIZE)));
                CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)pGetPBKEntry, pListCountPBK->nStructNum - 1,
                                    nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP Success! \n",0x08100d6a)));
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        default:
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKListCountEntryProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            hAo = HNULL;
            break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimPBKStorageInfoProc(HAO hAo, CFW_EV *pEvent)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKStorageInfoProc);

    UINT32 nUTI = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    SIM_SM_INFO_STORAGEINFO *pStorageInfo = CFW_GetAoUserData(hAo);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKStorageInfoProc) Status: %d File: %d",0x08100d6b), pStorageInfo->nState.nNextState, pStorageInfo->nCurrentFile);
    switch (pStorageInfo->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            CFW_PBK_STRORAGE_INFO* pReturnData = (CFW_PBK_STRORAGE_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_PBK_STRORAGE_INFO));
            if (pReturnData == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d6c));
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                return ERR_SUCCESS;
            }
            memset(pReturnData->Bitmap, 0xFF, sizeof(pReturnData->Bitmap));
            pStorageInfo->pData = pReturnData;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===pReturnData=%x===", pReturnData);

            UINT32 result = 0;
            if((pStorageInfo->nCurrentFile == API_USIM_EF_GB_ADN) || (pStorageInfo->nCurrentFile == API_USIM_EF_ADN))
            {
                UINT8 size = ADNInfo[nSimID][0].RecordSize;
                UINT8 pattern[50] = {[0 ... 49]=0xFF};
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===Seek pattern ADN==size= %d",size);
                //CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, patten, size, 16);

                result = SimSeekAllReq(pStorageInfo->nCurrentFile, 0x04, (UINT8)(size), pattern, nSimID);
                pStorageInfo->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pStorageInfo->nState.nNextState = CFW_SIM_PBK_SEEK;
            }
            else
            {
                result = SimElemFileStatusReq(pStorageInfo->nCurrentFile, nSimID);
                pStorageInfo->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pStorageInfo->nState.nNextState = CFW_SIM_PBK_READELEMENT;
            }

            if (ERR_SUCCESS != result)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===SimSeekAllReq Failed===");
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)result, 0, nUTI | (nSimID), 0xF0);
                CSW_SIM_FREE(pReturnData);
                pStorageInfo->pData = NULL;
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        break;

        case CFW_SIM_PBK_READELEMENT:
        {
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (pEvent->nEventId != API_SIM_ELEMFILESTATUS_CNF)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            CFW_PBK_STRORAGE_INFO* pReturnData = pStorageInfo->pData;
            api_SimElemFileStatusCnf_t *pSimElemFileStatusCnf = (api_SimElemFileStatusCnf_t *)pEvent->nParam1;
            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===pReturnData=%x===", pReturnData);

            if (((0x90 == pSimElemFileStatusCnf->Sw1) && (0x00 == pSimElemFileStatusCnf->Sw2))
                    || (0x91 == pSimElemFileStatusCnf->Sw1))
            {
                if(g_cfw_sim_status[nSimID].UsimFlag == 1) // USIM
                {
                    CFW_UsimEfStatus SimEfStatus;
                    CFW_UsimDecodeEFFcp (pSimElemFileStatusCnf->Data,  &SimEfStatus);
                    pReturnData->index = SimEfStatus.numberOfRecords;
                    pStorageInfo->nDataSize = SimEfStatus.recordLength;
                    pReturnData->iNumberLen = SIM_PBK_NUMBER_SIZE;
                }
                else // SIM
                {
                    pStorageInfo->nDataSize = pSimElemFileStatusCnf->Data[14];
                    SUL_MemCopy8(&pStorageInfo->nTotalSize, pSimElemFileStatusCnf->Data + 2, 2);
                    UINT16 TotalBytes = pStorageInfo->nTotalSize[0];
                    TotalBytes <<= 8;
                    TotalBytes &= 0xFF00;
                    TotalBytes += pStorageInfo->nTotalSize[1];
                    pReturnData->index      = TotalBytes / (pStorageInfo->nDataSize);
                    pReturnData->iNumberLen = SIM_PBK_NUMBER_SIZE;
                }

                if ((pStorageInfo->nCurrentFile == API_SIM_EF_BDN) || (pStorageInfo->nCurrentFile == API_USIM_EF_BDN))
                {
                    pReturnData->txtLen = pStorageInfo->nDataSize - CFW_SIM_REMAINLENGTH - 1;
                }
                else
                    pReturnData->txtLen = pStorageInfo->nDataSize - CFW_SIM_REMAINLENGTH;
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)pReturnData, SIZEOF(CFW_PBK_STRORAGE_INFO),
                                    nUTI | (nSimID << 24), 0);
                pStorageInfo->pData = NULL;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PBK_STRORAGE_INFO_NOTIFY Success! \n",0x08100d6d)));
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
            else
            {
                UINT32 ErrorCode = Sim_ParseSW1SW2(pSimElemFileStatusCnf->Sw1, pSimElemFileStatusCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_SIM_FREE(pReturnData);
                pStorageInfo->pData = NULL;
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d6e), ErrorCode);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        break;
        case CFW_SIM_PBK_SEEK:
        {
            if(pEvent->nEventId != API_SIM_SEEK_ALL_CNF)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }
            CFW_PBK_STRORAGE_INFO* pReturnData = pStorageInfo->pData;
            api_SimSeekAllCnf_t* pSimSeekAllCnf = (api_SimSeekAllCnf_t *) pEvent->nParam1;

            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===pReturnData=%x - Sw1,Sw2==%x%x", pReturnData,pSimSeekAllCnf->Sw1, pSimSeekAllCnf->Sw2);
            UINT8 i = 0;
            if((pStorageInfo->nCurrentFile == API_USIM_EF_GB_ADN) ||(pStorageInfo->nCurrentFile == API_USIM_EF_ADN))
            {
                if((pSimSeekAllCnf->Sw1 == 0x90) && (pSimSeekAllCnf->Sw2 == 0x00))
                {
                    UINT8 RecordNR= pSimSeekAllCnf->length;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "=0==The number of NULL records =%d===", RecordNR);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimSeekAllCnf->Record, 255/*ADNInfo[nSimID][0].MaxRecords*/, 16);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "=ADNInfo[nSimID][0].MaxRecords =%d===", ADNInfo[nSimID][0].MaxRecords);
                    //Create bitmap according to seek data of ADN
                    for(i = 0; (i < RecordNR) && (pSimSeekAllCnf->Record[i] <= ADNInfo[nSimID][0].MaxRecords); i++)
                    {
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "=pSimSeekAllCnf->Record[i] =%d===", pSimSeekAllCnf->Record[i]);
                        UINT16 j = pSimSeekAllCnf->Record[i];
                        UINT8 bit = (j - 1) & 0x07;
                        UINT8 num = (j - 1)>> 3;
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "j = %d, num = %d, bit = %d", j, num, bit);
                        pReturnData->Bitmap[num] &= ~(UINT8)(1 << bit);
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pReturnData->Bitmap[num] = 0x%x", pReturnData->Bitmap[num]);
                        if(pSimSeekAllCnf->Record[i] > pSimSeekAllCnf->Record[i + 1])
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The next record number(next = %d) is bigger that present(present = %d), break;", pSimSeekAllCnf->Record[i + 1], pSimSeekAllCnf->Record[i]);
                            break;
                        }
                    }
                }
                else if(((pSimSeekAllCnf->Sw1 != 0x6a) && (pSimSeekAllCnf->Sw2 != 0x83)) && ((pSimSeekAllCnf->Sw1 != 0x62) && (pSimSeekAllCnf->Sw2 != 0x82)))
                {
                    UINT32 ErrorCode = Sim_ParseSW1SW2(pSimSeekAllCnf->Sw1, pSimSeekAllCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_SIM_FREE(pReturnData);
                    pStorageInfo->pData = NULL;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d6e), ErrorCode);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }

                pReturnData->index = ADNInfo[nSimID][0].MaxRecords;
                if(ADNInfo[nSimID][1].EFID == 0)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===goto Completed===");
                    goto Completed;
                }
                UINT8 size = ADNInfo[nSimID][1].RecordSize;
                UINT8 pattern[50] = {[0 ... 49]=0xFF};
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "===Seek pattern ADN1==size= %d,  pReturnData->index = %d",size,  pReturnData->index );
                pStorageInfo->nCurrentFile += 1;
                UINT32 result = SimSeekAllReq(pStorageInfo->nCurrentFile, 0x04, (UINT8)(size), pattern, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)result, 0, nUTI | (nSimID), 0xF0);
                    CSW_SIM_FREE(pReturnData);
                    pStorageInfo->pData = NULL;
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
            }
            else
            {
                if((pSimSeekAllCnf->Sw1 == 0x90) && (pSimSeekAllCnf->Sw2 == 0x00))
                {
                    UINT8 RecordNR= pSimSeekAllCnf->length;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "=1==The number of NULL records =%d===", RecordNR);
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pSimSeekAllCnf->Record, 255/*ADNInfo[nSimID][1].MaxRecords*/, 16);
                    //Create bitmap according to seek data of ADN1
                    for(i = 0; (i < RecordNR) && (pSimSeekAllCnf->Record[i] <= ADNInfo[nSimID][1].MaxRecords); i++)
                    {
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pSimSeekAllCnf->Record[i] =%d", pSimSeekAllCnf->Record[i]);
                        UINT16 j = ADNInfo[nSimID][0].MaxRecords + pSimSeekAllCnf->Record[i];
                        UINT8 bit = (j - 1) & 0x07;
                        UINT8 num = (j - 1)>> 3;
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "j = %d, num = %d, bit = %d", j, num, bit);
                        pReturnData->Bitmap[num] &= ~(1 << bit);
                        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "pReturnData->Bitmap[num] = 0x%x", pReturnData->Bitmap[num]);
                        if(pSimSeekAllCnf->Record[i] > pSimSeekAllCnf->Record[i + 1])
                        {
                            CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "The next record number(next = %d) is bigger that present(present = %d), break;", pSimSeekAllCnf->Record[i + 1], pSimSeekAllCnf->Record[i]);
                            break;
                        }
                    }
                }
                else if(((pSimSeekAllCnf->Sw1 != 0x6a) && (pSimSeekAllCnf->Sw2 != 0x83)) && ((pSimSeekAllCnf->Sw1 != 0x62) && (pSimSeekAllCnf->Sw2 != 0x82)))
                {
                    UINT32 ErrorCode = Sim_ParseSW1SW2(pSimSeekAllCnf->Sw1, pSimSeekAllCnf->Sw2, nSimID);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_SIM_FREE(pReturnData);
                    pStorageInfo->pData = NULL;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d6e), ErrorCode);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }
                pReturnData->index += ADNInfo[nSimID][1].MaxRecords;
Completed:
                {
                    UINT16 num = pReturnData->index;
                    UINT16 bits = num - (num & 0xFFF8);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "num & 0xFFF8 = %x", num & 0xFFF8);
                    UINT8 mask = 0;
                    num = num >> 3;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "num = %d, bits = %d, pReturnData->index = %d", num, bits, pReturnData->index);
                    for(UINT8 i = 0 ;i < bits; i++)
                    {
                        mask = mask | (1<<i);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "mask = %x", mask);
                    }
                    pReturnData->Bitmap[num] &= mask;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "mask = %x, num = %d, pReturnData->Bitmap[num] = %x", mask, num, pReturnData->Bitmap[num]);
                    pStorageInfo->nDataSize = ADNInfo[nSimID][0].RecordSize;    //Because recordsize of both ADNs may be different, so this is not correct, but MMI didnot use it.
                    pReturnData->iNumberLen = SIM_PBK_NUMBER_SIZE;
                    pReturnData->txtLen = ADNInfo[nSimID][0].RecordSize - CFW_SIM_REMAINLENGTH;
                    CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pReturnData->Bitmap, sizeof(pReturnData->Bitmap), 16);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL), "=send==pReturnData=%x===", pReturnData);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP, (UINT32)pReturnData, SIZEOF(CFW_PBK_STRORAGE_INFO),
                                    nUTI | (nSimID << 24), 0);
                    pStorageInfo->pData = NULL;
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PBK_STRORAGE_INFO_NOTIFY Success! \n",0x08100d6d)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
            }
        }
        break;
        default:
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageInfoProc);
            break;
    }
    return ERR_SUCCESS;
}

UINT32 CFW_SimPBKStorageProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId    = 0;
    UINT32 ErrorCode = 0;
    UINT8 txtlength  = 0;
    UINT32 result    = 0;
    UINT8 i  = 0;

    VOID *nEvParam = NULL;
    CFW_EV ev;

    CFW_PBK_STORAGE *pPBKStorage = NULL;
    CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry = NULL;
    SIM_SM_INFO_STORAGE *pStorage = NULL;
    SIM_PBK_PARAM *pSimPBKPara = NULL;
    api_SimReadRecordCnf_t *pSimReadRecordCnf = NULL;
    api_SimReadBinaryCnf_t *pSimReadBinaryCnf = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimPBKStorageProc);

    UINT32 nUTI      = 0;
    CFW_GetUTI(hAo, &nUTI);
    CFW_SIM_ID nSimID = CFW_GetSimCardID(hAo);
    pStorage = CFW_GetAoUserData(hAo);

    if ((UINT32)pEvent == 0xffffffff)
    {
        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1                  = 0xff;
        pEvent                      = &ev;
        pStorage->nState.nNextState = CFW_SIM_PBK_IDLE;
    }
    else
    {
        nEvtId   = pEvent->nEventId;
        nEvParam = (VOID *)pEvent->nParam1;
    }

    CFW_CfgSimGetPbkParam(&pSimPBKPara, nSimID);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(2), TSTR("(CFW_SimPBKStorageProc) Status: %d File: %d",0x08100d6f), pStorage->nState.nNextState, pStorage->nCurrentFile);
    switch (pStorage->nState.nNextState)
    {
        case CFW_SIM_PBK_IDLE:
        {
            switch (pStorage->nCurrentFile)
            {
                case API_SIM_EF_ADN:
                    pStorage->nDataSize   = pSimPBKPara->nADNRecordSize;
                    pStorage->nTotalIndex = pSimPBKPara->nADNTotalNum;
                    txtlength             = pSimPBKPara->nADNRecordSize - CFW_SIM_REMAINLENGTH;
                    break;
                case API_USIM_EF_GB_ADN:
                case API_USIM_EF_ADN:
                case API_USIM_EF_ADN1:
                case API_USIM_EF_GB_ADN1:
                    pStorage->nDataSize   = getADNRecordSize(pStorage->nCurrentIndex, nSimID);
                    pStorage->nTotalIndex = pSimPBKPara->nADNTotalNum;
                    txtlength             = pStorage->nDataSize - CFW_SIM_REMAINLENGTH;
                    break;
                case API_SIM_EF_FDN:
                case API_USIM_EF_FDN:
                    pStorage->nDataSize   = pSimPBKPara->nFDNRecordSize;
                    pStorage->nTotalIndex = pSimPBKPara->nFDNTotalNum;
                    txtlength             = pSimPBKPara->nFDNRecordSize - CFW_SIM_REMAINLENGTH;
                    break;
                case API_SIM_EF_MSISDN:
                case API_USIM_EF_MSISDN:
                    pStorage->nDataSize   = pSimPBKPara->nMSISDNRecordSize;
                    pStorage->nTotalIndex = pSimPBKPara->nMSISDNTotalNum;
                    txtlength             = pSimPBKPara->nMSISDNRecordSize - CFW_SIM_REMAINLENGTH;
                    break;
                case API_SIM_EF_LND:
                    pStorage->nDataSize   = pSimPBKPara->nLNDRecordSize;
                    pStorage->nTotalIndex = pSimPBKPara->nLNDTotalNum;
                    txtlength             = pSimPBKPara->nLNDRecordSize - CFW_SIM_REMAINLENGTH;
                    break;
                case API_SIM_EF_ECC:
                    pStorage->nDataSize   = 3;
                    pStorage->nTotalIndex = pSimPBKPara->nECCTotalNum;
                    txtlength             = 0;
                    break;
                case API_USIM_EF_ECC:
                    pStorage->nDataSize   = pSimPBKPara->nECCRecordSize;
                    pStorage->nTotalIndex = pSimPBKPara->nECCTotalNum;
                    txtlength             = pSimPBKPara->nECCRecordSize - 4;
                    break;
                case API_SIM_EF_SDN:
                case API_USIM_EF_SDN:
                    pStorage->nDataSize   = pSimPBKPara->nSDNRecordSize;
                    pStorage->nTotalIndex = pSimPBKPara->nSDNTotalNum;
                    txtlength             = pSimPBKPara->nSDNRecordSize - CFW_SIM_REMAINLENGTH;
                    break;
                default:
                    break;
            }

            if (pStorage->nCurrentFile == API_SIM_EF_ECC)
            {
                result = SimReadBinaryReq(pStorage->nCurrentFile, 0, pStorage->nTotalIndex, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                pStorage->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pStorage->nState.nNextState = CFW_SIM_PBK_READBINARY;
            }
            else
            {
                pStorage->nCurrentIndex = 1;
                pStorage->nNotUsedIndex = 0;

                result = SimReadRecordReq(pStorage->nCurrentFile, 0x01, 0x04, pStorage->nDataSize, nSimID);
                if (ERR_SUCCESS != result)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadRecordReq return 0x%x \n",0x08100d70), result);
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                pStorage->nState.nCurrState = CFW_SIM_PBK_IDLE;
                pStorage->nState.nNextState = CFW_SIM_PBK_READRECORD;
            }
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
        }
        break;

        case CFW_SIM_PBK_READRECORD:
        {
            pSimReadRecordCnf = (api_SimReadRecordCnf_t *)nEvParam;
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            if (API_SIM_READRECORD_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }

            if (((0x90 == pSimReadRecordCnf->Sw1) && (0x00 == pSimReadRecordCnf->Sw2)) || (0x91 == pSimReadRecordCnf->Sw1))
            {
                pGetPBKEntry = (CFW_SIM_PBK_ENTRY_INFO *)CSW_SIM_MALLOC(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_EXTR_SIZE);
                if (pGetPBKEntry == NULL)
                {
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    return ERR_SUCCESS;
                }

                pGetPBKEntry->pNumber   = (UINT8 *)pGetPBKEntry + (UINT32)SIZEOF(CFW_SIM_PBK_ENTRY_INFO);
                pGetPBKEntry->pFullName = (UINT8 *)pGetPBKEntry + (UINT32)(SIZEOF(CFW_SIM_PBK_ENTRY_INFO) + SIM_PBK_NUMBER_SIZE);
                SUL_ZeroMemory8(pGetPBKEntry->pFullName, SIM_PBK_NAME_SIZE);

                if(pStorage->nCurrentFile == API_USIM_EF_ECC)
                {
                    SUL_MemCopy8(pGetPBKEntry->pNumber, pSimReadRecordCnf->Data, 3);
                    if((pSimReadRecordCnf->Data[0] & 0xf) == 0xf)
                        pStorage->nNotUsedIndex += 1;
                }
                else
                {
                    txtlength               = pStorage->nDataSize - CFW_SIM_REMAINLENGTH;
                    cfw_SimParsePBKRecData(pSimReadRecordCnf->Data, pGetPBKEntry, pStorage, txtlength, API_GETSTORAGE);

                    if ((pGetPBKEntry->iFullNameSize == 0) && (pGetPBKEntry->nNumberSize == 0) && (pGetPBKEntry->nType != CFW_TELNUMBER_TYPE_INTERNATIONAL))  // updated on 20070618 to read out "+" entry
                    {
                        pStorage->nNotUsedIndex += 1;
                    }
                }
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("NotUsed RecNum %d \n",0x08100d71), pStorage->nNotUsedIndex);
                if (pStorage->nCurrentIndex < pStorage->nTotalIndex)
                {
                    pStorage->nCurrentIndex += 1;
                    CSW_SIM_FREE(pGetPBKEntry);

                    if((pStorage->nCurrentFile == API_USIM_EF_GB_ADN) ||(pStorage->nCurrentFile == API_USIM_EF_ADN))
                    {
                        UINT8 EFID = getADNEFID(pStorage->nCurrentIndex, nSimID);
                        UINT8 nRecordNumber = getADNRecordNumber(pStorage->nCurrentIndex, nSimID);
                        UINT8 nRecordSize = getADNRecordSize(pStorage->nCurrentIndex, nSimID);
                        result = SimReadRecordReq(EFID, nRecordNumber, 0x04, nRecordSize, nSimID);
                    }
                    else
                        result = SimReadRecordReq(pStorage->nCurrentFile, pStorage->nCurrentIndex, 0x04, pStorage->nDataSize, nSimID);
                    if (ERR_SUCCESS != result)
                    {
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)result, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error SimReadRecordReq return 0x%x \n",0x08100d72), result);

                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                    }
                    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);
                }
                else
                {
                    pPBKStorage = (CFW_PBK_STORAGE *)CSW_SIM_MALLOC(SIZEOF(CFW_PBK_STORAGE));
                    if (pPBKStorage == NULL)
                    {
                        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d73));
                        CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                        CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                        CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                        return ERR_SUCCESS;
                    }
                    pPBKStorage->totalSlot = pStorage->nTotalIndex;
                    pPBKStorage->usedSlot  = pStorage->nTotalIndex - pStorage->nNotUsedIndex;
                    switch (pStorage->nCurrentFile)
                    {
                        case API_SIM_EF_ADN:
                        case API_USIM_EF_GB_ADN:
                        case API_USIM_EF_GB_ADN1:
                        case API_USIM_EF_ADN:
                        case API_USIM_EF_ADN1:
                            pPBKStorage->storageId = CFW_PBK_SIM;
                            break;
                        case API_SIM_EF_FDN:
                        case API_USIM_EF_FDN:
                            pPBKStorage->storageId = CFW_PBK_SIM_FIX_DIALLING;
                            break;
                        case API_SIM_EF_LND:
                            pPBKStorage->storageId = CFW_PBK_SIM_LAST_DIALLING;
                            break;
                        case API_SIM_EF_MSISDN:
                        case API_USIM_EF_MSISDN:
                            pPBKStorage->storageId = CFW_PBK_ON;
                            break;
                        case API_USIM_EF_ECC:
                            pPBKStorage->storageId = CFW_PBK_EN;
                            break;
                        case API_SIM_EF_SDN:
                        case API_USIM_EF_SDN:
                            pPBKStorage->storageId = CFW_PBK_SDN;
                            break;
                        default:
                            break;
                    }
                    pPBKStorage->padding[0] = 0xFF;
                    pPBKStorage->padding[1] = 0xFF;
                    CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)pPBKStorage, SIZEOF(CFW_PBK_STORAGE),
                                        nUTI | (nSimID << 24), 0);
                    CSW_SIM_FREE(pGetPBKEntry);
                    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PBK_STRORAGE_RSP Success! \n",0x08100d74)));
                    CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                }
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadRecordCnf->Sw1, pSimReadRecordCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)ErrorCode, pStorage->nCurrentIndex,
                                    nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            }
        }
        break;

        case CFW_SIM_PBK_READBINARY:
        {
            pSimReadBinaryCnf = (api_SimReadBinaryCnf_t *)nEvParam;

            pPBKStorage = (CFW_PBK_STORAGE *)CSW_SIM_MALLOC(SIZEOF(CFW_PBK_STORAGE));
            if (pPBKStorage == NULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100d75));
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, ERR_NO_MORE_MEMORY, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
                CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
                return ERR_SUCCESS;
            }
            SUL_ZeroMemory8(pPBKStorage, (SIZEOF(CFW_PBK_STORAGE)));
            //
            // Add by lixp for TBM780(PBK Init failed)at 20080121
            // Because a sim fetch Ind event is coming.But the event isnot filtrated
            //
            if (API_SIM_READBINARY_CNF != nEvtId)
            {
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                return ERR_SUCCESS;
            }
            if (((0x90 == pSimReadBinaryCnf->Sw1) && (0x00 == pSimReadBinaryCnf->Sw2)) || (0x91 == pSimReadBinaryCnf->Sw1))
            {
                for (i = 0; i < (pStorage->nTotalIndex) / 3; i++)
                {
                    if (((pSimReadBinaryCnf->Data[i * 3]) & 0x0F) != 0x0F)
                    {
                        pPBKStorage->usedSlot += 1;
                    }
                }
                if (pStorage->nCurrentFile == API_SIM_EF_ECC)
                {
                    pPBKStorage->storageId = CFW_PBK_EN;
                    pPBKStorage->totalSlot = pStorage->nTotalIndex;
                }

                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)pPBKStorage, SIZEOF(CFW_PBK_STORAGE),
                                    nUTI | (nSimID << 24), 0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR(" post EV_CFW_SIM_GET_PBK_STRORAGE_RSP Success! \n",0x08100d76)));
            }
            else
            {
                ErrorCode = Sim_ParseSW1SW2(pSimReadBinaryCnf->Sw1, pSimReadBinaryCnf->Sw2, nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_GET_PBK_STRORAGE_RSP, (UINT32)ErrorCode, 0, nUTI | (nSimID << 24), 0xF0);
                CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error Sim_ParseSW1SW2 ErrorCode 0x%x \n",0x08100d77), ErrorCode);
            }
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
        }
        break;

        default:
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimPBKStorageProc);
            CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
            break;
    }

    return ERR_SUCCESS;
}

UINT32 cfw_SimParsePBKRecData(UINT8 *pData,
                              CFW_SIM_PBK_ENTRY_INFO *pGetPBKEntry,
                              void *proc,
                              UINT8 txtlength,
                              UINT8 nApiNum
                             )
{
    UINT8 i = 0x00;
    WORD wNameStep;
    WORD wBasePoint;
    WORD wConverted;
    UINT16 uLen = 0;
    UINT8 pName[SIM_PBK_NAME_SIZE];

    CSW_PROFILE_FUNCTION_ENTER(cfw_SimParsePBKRecData);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(3), TSTR("(cfw_SimParsePBKRecData) data[0]: 0x%x txtlength: %d ApiNum: %d",0x08100d78), pData[0], txtlength, nApiNum);

    // CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pData, txtlength, 16);
    if (pData[0] < 0x7F)
    {
        for (i = 0; i < txtlength + 1; i++)
        {
            if (pData[i] == 0xFF)
                break;
        }

        if (i > txtlength)
            pGetPBKEntry->iFullNameSize = txtlength;
        else
            pGetPBKEntry->iFullNameSize = i;
        if (pGetPBKEntry->iFullNameSize > SIM_PBK_NAME_SIZE)
        {
            pGetPBKEntry->iFullNameSize = SIM_PBK_NAME_SIZE;
        }

        SUL_MemCopy8(pGetPBKEntry->pFullName, pData, pGetPBKEntry->iFullNameSize);
    }

    // convert to standard UCS2 if necessary
    else if (pData[0] == (BYTE)0x81)
    {
        // UINT8 data[SIM_PBK_NAME_SIZE] = {0x00};
        uLen = pData[1] * 2 + 1;

        // add by wanghb 2007.12.13 begin
        if (uLen > SIM_PBK_NAME_SIZE)
        {
            uLen = SIM_PBK_NAME_SIZE;
        }

        SUL_MemSet8(pName, 0xFF, SIM_PBK_NAME_SIZE);

        pName[0]   = (BYTE)0x80;
        wNameStep  = (WORD) pData[2];
        wBasePoint = (wNameStep << 7) & 0x7F80;

        for (wNameStep = 0; wNameStep < (WORD) ((uLen - 1) / 2); wNameStep++)
        {
            if (0x80 == (pData[3 + wNameStep] & 0x80))
            {
                wConverted = wBasePoint + (WORD) (pData[3 + wNameStep] & 0x7F);
            }
            else
            {
                wConverted = 0x0000 + (WORD) pData[3 + wNameStep];
            }

            pName[1 + wNameStep * 2] = (BYTE)((wConverted & 0xFF00) >> 8);
            pName[1 + wNameStep * 2 + 1] = (BYTE)(wConverted & 0x00FF);
        }

        if (pData[1] == 0x00 || 0xFF == pData[1])
        {
            pGetPBKEntry->iFullNameSize = 0;
        }

        // Add by wanghb at 20071214
        // For TD136 sim pbk issue
        else
        {
            pGetPBKEntry->iFullNameSize = uLen;

        }
        SUL_MemCopy8(pGetPBKEntry->pFullName, pName, pGetPBKEntry->iFullNameSize);
    }
    else if (pData[0] == (BYTE)0x82)
    {
        // add by wanghb 2007.12.13 begin
        // UINT8 data[SIM_PBK_NAME_SIZE] = {0x00};
        uLen = pData[1] * 2 + 1;

        if (uLen > SIM_PBK_NAME_SIZE)
            uLen = SIM_PBK_NAME_SIZE;

        SUL_MemSet8(pName, 0xFF, SIM_PBK_NAME_SIZE);

        pName[0]   = (BYTE)0x80;
        wBasePoint = (WORD) pData[2];
        wBasePoint <<= 8;
        wBasePoint += (BYTE)pData[3];

        for (wNameStep = 0; wNameStep < (WORD) ((uLen - 1) / 2); wNameStep++)
        {
            if (0x80 == (pData[4 + wNameStep] & 0x80))
            {
                wConverted = wBasePoint + (WORD) (pData[4 + wNameStep] & 0x7F);
            }
            else
            {
                wConverted = (WORD) pData[4 + wNameStep];
            }

            pName[1 + wNameStep * 2] = (BYTE)((wConverted & 0xFF00) >> 8);
            pName[1 + wNameStep * 2 + 1] = (BYTE)(wConverted & 0x00FF);
        }

        if (pData[1] == 0xFF)
        {
            pGetPBKEntry->iFullNameSize = 0;
        }
        else
        {
            pGetPBKEntry->iFullNameSize = uLen;
        }

        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTXT(TSTR("0x80 Name size %d\n",0x08100d79)), pGetPBKEntry->iFullNameSize);

        // Add by wanghb at 20071214
        // For Td136issue
        // SUL_MemCopy8(pGetPBKEntry->pFullName, pData, pGetPBKEntry->iFullNameSize);
        SUL_MemCopy8(pGetPBKEntry->pFullName, pName, pGetPBKEntry->iFullNameSize);
    }

    // end conversion
    else if (pData[0] == 0x80)
    {
        if (pData[1] == 0xFF)
        {
            pGetPBKEntry->iFullNameSize = 0;
        }
        for (i = 0; i < (txtlength - 1) / 2; i++)
        {
            if ((pData[2 * i + 1] == 0xFF) && (pData[2 * i + 2] == 0xFF))
                break;

            pGetPBKEntry->iFullNameSize = 2 * (i + 1) + 1;
        }

        if (pGetPBKEntry->iFullNameSize > SIM_PBK_NAME_SIZE)
        {
            pGetPBKEntry->iFullNameSize = SIM_PBK_NAME_SIZE;
        }

        // modify by lixp for TBM780 bug at 20080121
        // (!(pGetPBKEntry->iFullNameSize & 0x01))
        if ((!(pGetPBKEntry->iFullNameSize & 0x01)) && (pGetPBKEntry->iFullNameSize))
        {
            pGetPBKEntry->iFullNameSize--;
        }

        SUL_MemCopy8(pGetPBKEntry->pFullName, pData, pGetPBKEntry->iFullNameSize);
    }
    else
    {
        for (i = 0; i < txtlength + 1; i++)
        {
            if (pData[i] == 0xFF)
                break;
        }

        if (i > txtlength)
            pGetPBKEntry->iFullNameSize = txtlength;
        else
            pGetPBKEntry->iFullNameSize = i;
        if (pGetPBKEntry->iFullNameSize > SIM_PBK_NAME_SIZE)
        {
            pGetPBKEntry->iFullNameSize = SIM_PBK_NAME_SIZE;
        }

        SUL_MemCopy8(pGetPBKEntry->pFullName, pData, pGetPBKEntry->iFullNameSize);
        //CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("REC_Name NULL!\n",0x08100d7a)));
    }

    if (nApiNum == API_GETPBK)
    {
        if (pData[(((SIM_SM_INFO_GETPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] > SIM_PBK_NAME_SIZE
                || (pData[(((SIM_SM_INFO_GETPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] - 1 <= 0))
        {
            pGetPBKEntry->nNumberSize = 0;
        }
        else
        {
            pGetPBKEntry->nNumberSize = pData[(((SIM_SM_INFO_GETPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] - 1;
            if (pGetPBKEntry->nNumberSize > 10)
            {
                pGetPBKEntry->nNumberSize = 10;
            }
            else
            {
                SUL_MemCopy8(pGetPBKEntry->pNumber, pData + ((SIM_SM_INFO_GETPBK *)proc)->nRecordSize - 12,
                             (UINT8)(pData[(((SIM_SM_INFO_GETPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] - 1));
            }
        }
        pGetPBKEntry->phoneIndex = ((SIM_SM_INFO_GETPBK *)proc)->nCurrRecInd;
        pGetPBKEntry->nType      = pData[((SIM_SM_INFO_GETPBK *)proc)->nRecordSize - 13];
    }
    else if (nApiNum == API_LISTPBK)
    {
        if ((pData[(((SIM_SM_INFO_LISTPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] > SIM_PBK_NAME_SIZE)
                || (pData[(((SIM_SM_INFO_LISTPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] - 1 <= 0))
        {
            pGetPBKEntry->nNumberSize = 0;
        }
        else
        {
            pGetPBKEntry->nNumberSize = pData[(((SIM_SM_INFO_LISTPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] - 1;
            if (pGetPBKEntry->nNumberSize > 10)
            {
                pGetPBKEntry->nNumberSize = 10;
            }
            else
            {
                SUL_MemCopy8(pGetPBKEntry->pNumber, pData + ((SIM_SM_INFO_LISTPBK *)proc)->nRecordSize - 12,
                             (UINT8)(pData[(((SIM_SM_INFO_LISTPBK *)proc)->nRecordSize - CFW_SIM_REMAINLENGTH)] - 1));
            }
        }
        pGetPBKEntry->phoneIndex = ((SIM_SM_INFO_LISTPBK *)proc)->nCurrRecInd;
        pGetPBKEntry->nType      = pData[((SIM_SM_INFO_LISTPBK *)proc)->nRecordSize - 13];
    }
    else if (nApiNum == API_GETSTORAGE)
    {
        if (pData[(((SIM_SM_INFO_STORAGE *)proc)->nDataSize - CFW_SIM_REMAINLENGTH)] > SIM_PBK_NAME_SIZE
                || (pData[(((SIM_SM_INFO_STORAGE *)proc)->nDataSize - CFW_SIM_REMAINLENGTH)] - 1 <= 0))
        {
            pGetPBKEntry->nNumberSize = 0;
        }
        else
        {
            pGetPBKEntry->nNumberSize = pData[(((SIM_SM_INFO_STORAGE *)proc)->nDataSize - CFW_SIM_REMAINLENGTH)] - 1;
            if (pGetPBKEntry->nNumberSize > 10)
            {
                pGetPBKEntry->nNumberSize = 10;
            }
            else
            {
                SUL_MemCopy8(pGetPBKEntry->pNumber, pData + ((SIM_SM_INFO_STORAGE *)proc)->nDataSize - 12,
                             (UINT8)(pData[(((SIM_SM_INFO_STORAGE *)proc)->nDataSize - CFW_SIM_REMAINLENGTH)] - 1));
            }
        }
        pGetPBKEntry->phoneIndex = ((SIM_SM_INFO_STORAGE *)proc)->nCurrentIndex;
        pGetPBKEntry->nType      = pData[((SIM_SM_INFO_STORAGE *)proc)->nDataSize - 13];

    }
    CSW_PROFILE_FUNCTION_EXIT(cfw_SimParsePBKRecData);
    return 0;
}

#endif

