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
#include <ts.h>
#include <sul.h>
#include "csw_debug.h"

#include "cfw_sim.h"
#include "cfw_sim_mis.h"
#include "cfw_sim_boot.h"
#include <csw_mem_prv.h>
#include <base_prv.h>
#include <cos.h>
#include "hal_host.h"
//
// This variable is used to store ecc code,if the first byte of one code is 0xFF,
// and after this will be not any code.
UINT8 nECC_Code[5][3];
UINT8 nUsimECC_Code[5][4];

// The EFECC file contains up to 5 emergency call codes, and three bytes for one call number.
CFW_SIM_LP Sim_Lp = {0, NULL};
UINT32 CFW_SimGetECC(UINT8 *pBuffer, UINT8 *nNum)
{
    UINT8 i = 0;
    UINT8 nSize = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetECC);
    for (i = 0; i < 5; i++)
    {
        if (nECC_Code[i][0] != 0xFF)
            nSize += 3;
        else
            break;
    }

    if (nSize < *nNum)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(2), TSTR("Error CFW_SimGetECC *nNum 0x%x nSize 0x%x\n", 0x08100c06), *nNum, nSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetECC);
        return ERR_CFW_INVALID_PARAMETER; // The Buffer is not enough.
    }
    nSize = 0;
    for (i = 0; i < 5; i++)
    {
        if (nECC_Code[i][0] != 0xFF)
        {
            SUL_MemCopy8((pBuffer + (UINT32)nSize), nECC_Code[i], 3);
            nSize += 3;
        }
        else
            break;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetECC);
    return ERR_SUCCESS;
}

VOID CFW_SimSetECC(UINT8 *pBuffer)
{
    UINT8 i = 0;
    UINT8 Num = 0;
    UINT8 Offset = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetECC);
    if (pBuffer == NULL)
        nECC_Code[0][0] = 0xFF;
    else
    {
        for (i = 0; i < 5; i++)
        {
            if (*(pBuffer + Offset) == 0xFF)
            {
                SUL_MemCopy8(nECC_Code[Num], (pBuffer + Offset), 3);
                Num++;
            }
            Offset += 3;
        }
        if (Num != 5)
            nECC_Code[Num][0] = 0xFF;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetECC);
}

VOID CFW_USimSetECC(UINT8 *pBuffer, UINT16 length)
{
    //  UINT8 i      = 0;
    //  UINT8 Num    = 0;
    UINT8 Offset = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetECC);
    if (pBuffer == NULL)
        nECC_Code[0][0] = 0xFF;
    else
    {
        //    for (i = 0; i < 5; i++)
        {
            //      if (*(pBuffer + Offset) == 0xFF)
            {
                SUL_MemCopy8(nUsimECC_Code[0], (pBuffer + Offset), 3);
                //        Num++;
            }
            Offset += 3;
        }

        nUsimECC_Code[0][3] = pBuffer[length - 1];

        //    if (Num != 5)
        //      nUsimECC_Code[Num][0] = 0xFF;
    }
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetECC);
}

UINT32 CFW_SimGetLP(UINT8 *pBuffer, UINT16 *nNum)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetLP);
    if (Sim_Lp.nLPSize > *nNum)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(2), TSTR("Error CFW_SimGetLP *nNum 0x%x nSize 0x%x\n", 0x08100c07), *nNum, Sim_Lp.nLPSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetLP);
        return ERR_INVALID_PARAMETER;
    }
    SUL_MemCopy8(pBuffer, Sim_Lp.pLP, Sim_Lp.nLPSize);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetLP);
    return ERR_SUCCESS;
}

UINT32 CFW_SimSetLP(UINT8 *pBuffer, UINT16 nNum)
{
    Sim_Lp.pLP = pBuffer;
    Sim_Lp.nLPSize = nNum;
    return ERR_SUCCESS;
}

BOOL CFW_SimGetPS(VOID)
{
    return 0;
}

BOOL CFW_SimGetPF(VOID)
{
    return 0;
}

UINT32 CFW_SimSetPF(BOOL bValue)
{
    return 1;
}

UINT32 CFW_SimSetPS(BOOL bValue)
{
    return 1;
}
UINT32 CFW_SimErrorSpy(HAO hAo, CFW_EV *pEvent)
{
    CFW_UnRegisterAO(CFW_SIM_SRV_ID, hAo);
    return ERR_SUCCESS;
}

extern CFW_SIM_OC_STATUS SIMOCStatus[CFW_SIM_COUNT];
BOOL gSimDropInd[CFW_SIM_COUNT] = {
    FALSE,
};

HAO SimSpySimCard(CFW_EV *pEvent)
{
    HAO hSimAo = NULL;
#if !defined(SIMCARD_HOT_DETECT) || (SIMCARD_HOT_DETECT == 0)
    CSW_PROFILE_FUNCTION_ENTER(SimSpySimCard);

    CFW_SIM_ID nSimID;
    nSimID = pEvent->nFlag;

    if (API_SIM_STATUSERROR_IND == pEvent->nEventId)
    {
        api_SimStatusErrorInd_t *pSimStatusErrorInd = NULL;
        pSimStatusErrorInd = (api_SimStatusErrorInd_t *)pEvent->nParam1;
#ifdef LTE_NBIOT_SUPPORT
		CFW_Exit(2);
#endif
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimSpySimCard StatusError %d\n", 0x08100c08), pSimStatusErrorInd->StatusError);

        //MM read Loci file failed.
        if (0x10 == pSimStatusErrorInd->StatusError)
        {
            CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID); // Fix mm crash issue at 20140411 by XP
            SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
        }
        else
        {
            if (gSimDropInd[nSimID] == FALSE)
            {
#ifdef _DROPCARD_AUTOPIN_
                CFW_SimResetEx(GENERATE_SHELL_UTI(), nSimID, FALSE);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(0), TSTR(" CFW_SimResetEx \n", 0x08100c09));
#else
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(1), TSTR("SimSpySimCard: Send Message to Shell(nSimID = %d)!", 0x08100c0a), nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_DROP, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
#endif
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error SimSpySimCard nSimID[%d]\n", 0x08100c0b), nSimID);
                CFW_SendSclMessage(API_SIM_CLOSE_IND, NULL, nSimID); // added to
                gSimDropInd[nSimID] = TRUE;
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID) | TDB | TNB_ARG(1), TSTR("SimSpySimCard: Send Message to Shell!(_DROPCARD_AUTOPIN_,nSimID = %d)", 0x08100c0c), nSimID);
                CFW_PostNotifyEvent(EV_CFW_SIM_STATUS_IND, CFW_SIM_CARD_DROP, 0, GENERATE_SHELL_UTI() | (nSimID << 24), 0xF0);
            }
            SIMOCStatus[nSimID] = CFW_SIM_OC_IDLE;
        }
    }
    CSW_PROFILE_FUNCTION_EXIT(SimSpySimCard);
#endif
    return hSimAo;
}

UINT32 CFW_SimInitStage3Proc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_USimInitStage3Proc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimInitStage1Proc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_USimInitStage1Proc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimGetProviderId(UINT16 nUTI, CFW_SIM_ID nSimID)
{

    HAO hAo = 0;
    SIM_SM_INFO_GETPROVIDERID *pGetProviderID = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetProviderId);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c0d));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetProviderId);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pGetProviderID = (SIM_SM_INFO_GETPROVIDERID *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETPROVIDERID));
    if (pGetProviderID == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c0e));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetProviderId);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory32(pGetProviderID, SIZEOF(SIM_SM_INFO_GETPROVIDERID));

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetProviderID, CFW_SimMisGetProviderIdProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetProviderId);
    return (ERR_SUCCESS);
}

//index is an index of the PLMN's location in (U)SIM, the value is from 0 to 255
//but 0 mean the first free location
//MCC is an ascii string format, for chinamobile GSM, "460"
//MNC is an ascii string format, for chinamobile GSM, "02"
//if one of MCC and MNC is NULL, the entry will be deleted.
UINT32 CFW_SimWritePreferPLMNProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimWritePreferPLMN(UINT8 index, UINT8* MCC, UINT8* MNC, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimWritePreferPLMN);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "index = %d, MCC=%s, MNC = %s",index, MCC, MNC);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "CFW_SimWritePreferPLMN");
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c0f), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
        return ret;
    }
    
    struct write_prefer_plmn* plmn = (struct write_prefer_plmn*)CSW_SIM_MALLOC(sizeof(struct write_prefer_plmn));
    //struct prefer_plmn* plmn = (struct prefer_plmn*)CSW_SIM_MALLOC(5);
    if(plmn == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
        return ERR_NO_MORE_MEMORY;
    }

    UINT8 mcc_size = strlen(MCC);
    UINT8 mnc_size = strlen(MNC);
    if((mcc_size > 3) || (mnc_size > 3))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "ERROR: mcc_size = %d, mnc_size = %d", mcc_size, mnc_size);
        return ERR_INVALID_PARAMETER;
    }
    if((mcc_size == 0) || (mnc_size == 0))
    {
        if(index == 0)  //delete free location is not allowed.
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "ERROR: index = %d", index);
            return ERR_INVALID_PARAMETER;
        }
        plmn->PLMN[0] = 0xFF;
        plmn->PLMN[1] = 0xFF;
        plmn->PLMN[2] = 0xFF;
    }
    else
    {
        for(int i = 0; i < 3; i++)
        {
            if((MCC[i] < '0') || (MCC[i] > '9'))
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "ERROR: MCC[%d] = %d", i, MCC[i]);
                return ERR_INVALID_PARAMETER;
            }
            else
                MCC[i] = MCC[i] - '0';
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "MCC[%d] = %x", i, MCC[i]);
        }
        for(int i = 0; i < mnc_size; i++)
        {
            if((MNC[i] < '0') || (MNC[i] > '9'))
            {
                CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
                CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "ERROR: MNC[%d] = %d", i, MNC[i]);
                return ERR_INVALID_PARAMETER;
            }
            else
                MNC[i] = MNC[i] - '0';
            CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "MNC[%d] = %x", i, MNC[i]);
        }
        //convert string format MCC and MNC to BCD forat PLMN, according to 3GPP 24008
        plmn->PLMN[0] = (MCC[1] << 4) | MCC[0];
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "PLMN[0]=%x", plmn->PLMN[0]);
        if(mnc_size == 3)
            plmn->PLMN[1] = MNC[2] << 4;
        else
            plmn->PLMN[1] = 0xF0;
        plmn->PLMN[1] |= MCC[2];
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "PLMN[1]=%x, MNC=%x%x", plmn->PLMN[1], MNC[0], MNC[1]);
        plmn->PLMN[2] = (MNC[1] << 4) | MNC[0];
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "PLMN[2]=%x", plmn->PLMN[2]);
    }
    
    plmn->index = index;
    plmn->status = CFW_SIM_MIS_IDLE;
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "PLMN=%x%x%x", plmn->PLMN[0],plmn->PLMN[1],plmn->PLMN[2]);

    HAO hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, plmn, CFW_SimWritePreferPLMNProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimWritePreferPLMN);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "CFW_SimWritePreferPLMN exit");

    return (ERR_SUCCESS);
}

UINT32 CFW_SimReadPreferPLMNProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimReadPreferPLMN(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadPreferPLMN);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "CFW_SimReadPreferPLMN");
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c0f), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadPreferPLMN);
        return ret;
    }
    
    struct read_prefer_plmn* plmn = (struct read_prefer_plmn*)CSW_SIM_MALLOC(sizeof(struct read_prefer_plmn));
    //struct prefer_plmn* plmn = (struct prefer_plmn*)CSW_SIM_MALLOC(5);
    if(plmn == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadPreferPLMN);
        return ERR_NO_MORE_MEMORY;
    }

    //plmn->index = 0;
    plmn->offset = 0;
    plmn->ret_data = 0;
    plmn->status = CFW_SIM_MIS_IDLE;
    //plmn->counter = 1;
    HAO hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, plmn, CFW_SimReadPreferPLMNProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadPreferPLMN);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR), "CFW_SimReadPreferPLMN exit");

    return (ERR_SUCCESS);
}

UINT32 CFW_SimSetPrefOperatorList(UINT8 *pOperatorList, UINT8 nSize, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    UINT8 *temp = NULL;

    SIM_SM_INFO_SETPREFOPREATORLIST *pSetPrefLsit = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetPrefOperatorList);
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c0f), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ret;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c10));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pSetPrefLsit = (SIM_SM_INFO_SETPREFOPREATORLIST *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_SETPREFOPREATORLIST));
    if (pSetPrefLsit == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c11));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory32(pSetPrefLsit, SIZEOF(SIM_SM_INFO_SETPREFOPREATORLIST));
    temp = (UINT8 *)CSW_SIM_MALLOC(nSize / 2);
    if (temp == NULL)
    {
        CSW_Free(pSetPrefLsit);
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c12));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSetPrefLsit, CFW_SimMisSetPrefListProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    // CSW_TC_MEMBLOCK(CFW_SIM_TS_ID, pOperatorList, nSize, 16);

    pSetPrefLsit->nSize = nSize / 2;
    cfw_BCDtoPLMN(pOperatorList, temp, nSize);
    SUL_MemSet8(pSetPrefLsit->nOperatorList, 0xFF, 255);
    SUL_MemCopy8(pSetPrefLsit->nOperatorList, temp, pSetPrefLsit->nSize);
    CSW_SIM_FREE(temp);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);

    return (ERR_SUCCESS);
}

// ------------------------------------------------------------
// Add by wulc  2009-10-25 begin
// ------------------------------------------------------------

UINT32 CFW_SimGetServiceProviderName(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    SIM_SM_INFO_GETSERVERPROVIDERNAME *pGetSPN = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetServiceProviderName);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c13));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetServiceProviderName);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pGetSPN = (SIM_SM_INFO_GETSERVERPROVIDERNAME *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETSERVERPROVIDERNAME));
    if (pGetSPN == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c14));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetServiceProviderName);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(pGetSPN, SIZEOF(SIM_SM_INFO_GETSERVERPROVIDERNAME));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetSPN, CFW_SimGetServiceProviderNameProc, nSimID);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetServiceProviderName()\r\n", 0x08100c15)));
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetServiceProviderName);

    return (ERR_SUCCESS);
}
// ------------------------------------------------------------
// Add by wulc  2009-10-25 end
// ------------------------------------------------------------

UINT32 CFW_SimGetPrefOperatorList(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    SIM_SM_INFO_GETPREFOPREATORLIST *pGetPrefList = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPrefOperatorList);
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c16));

        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorList);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pGetPrefList = (SIM_SM_INFO_GETPREFOPREATORLIST *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETPREFOPREATORLIST));
    if (pGetPrefList == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_ERROR) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c17));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorList);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory32(pGetPrefList, SIZEOF(SIM_SM_INFO_GETPREFOPREATORLIST));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetPrefList, CFW_SimMisGetPrefListProcEX, nSimID);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetPrefOperatorList()\r\n", 0x08100c18)));
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorList);

    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetPrefOperatorListMaxNum(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;

    SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM *pGetPrefListMaxNum = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPrefOperatorListMaxNum);

    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c19), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListMaxNum);
        return ret;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c1a));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListMaxNum);
        return ERR_CFW_UTI_IS_BUSY;
    }
#endif
    pGetPrefListMaxNum =
        (SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM));
    if (pGetPrefListMaxNum == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c1b));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListMaxNum);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory32(pGetPrefListMaxNum, SIZEOF(SIM_SM_INFO_GETPREFOPREATORLISTMAXNUM));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetPrefListMaxNum, CFW_SimMisGetPrefListMAXNumProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListMaxNum);

    return (ERR_SUCCESS);
}

// =============================================================================
// ACM Reference
//
// =============================================================================

#ifndef SIM_ARG_CHK_EN
#define SIM_ARG_CHK_EN (1 == 1)
#endif

UINT32 CFW_SimSetACMMax(UINT32 iACMMaxValue, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    ACM_INFO_SETACMMAX *pSetAcmMax = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetACMMax);

#if SIM_ARG_CHK_EN > 0
#if 0
    if (iACMMaxValue > 0x1000000)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error iACMMaxValue 0x%x\n",0x08100c1c), iACMMaxValue);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMax);
        return ERR_CFW_INVALID_PARAMETER;
    }
#endif

    if (pPin2 == (UINT8 *)NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error pPin2 NULL\n", 0x08100c1d));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMax);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((nPinSize < 4) || (nPinSize > 8))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error nPinSize 0x%x\n", 0x08100c1e), nPinSize);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMax);
        return ERR_CFW_INVALID_PARAMETER;
    }
#endif
    pSetAcmMax = (ACM_INFO_SETACMMAX *)CSW_SIM_MALLOC(SIZEOF(ACM_INFO_SETACMMAX));
#if SIM_ARG_CHK_EN > 0
    if (pSetAcmMax == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c1f));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMax);
        return ERR_NO_MORE_MEMORY;
    }
#endif
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("--iACMMaxValue = 0x%X--\n", 0x08100c20), iACMMaxValue);
    UINT8 nACMMaxValue[4];

    UINT32 order = 0x11223344;
    if (((UINT8 *)&order)[0] == 0x44) //little endian
    {
        nACMMaxValue[0] = *((UINT8 *)(&iACMMaxValue) + 2);
        nACMMaxValue[1] = *((UINT8 *)(&iACMMaxValue) + 1);
        nACMMaxValue[2] = *(UINT8 *)(&iACMMaxValue);
        nACMMaxValue[3] = 0;
    }
    else //big endian
    {
        nACMMaxValue[0] = *((UINT8 *)(&iACMMaxValue) + 1);
        nACMMaxValue[1] = *((UINT8 *)(&iACMMaxValue) + 2);
        nACMMaxValue[2] = *((UINT8 *)(&iACMMaxValue) + 3);
        nACMMaxValue[3] = 0;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("--nACMMaxValue[0] = 0x%X--\n", 0x08100c21), nACMMaxValue[0]);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("--nACMMaxValue[1] = 0x%X--\n", 0x08100c22), nACMMaxValue[1]);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("--nACMMaxValue[2] = 0x%X--\n", 0x08100c23), nACMMaxValue[2]);

    pSetAcmMax->nPinSize = nPinSize;
    pSetAcmMax->iACMMaxValue = *((UINT32 *)nACMMaxValue);
    SUL_MemSet8(pSetAcmMax->pPin2, 0xFF, 8);
    SUL_MemCopy8(pSetAcmMax->pPin2, pPin2, nPinSize);

    pSetAcmMax->n_CurrStatus = CFW_SIM_MIS_IDLE;
    pSetAcmMax->n_PrevStatus = CFW_SIM_MIS_IDLE;
    pSetAcmMax->nTryCount = 0x01;

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSetAcmMax, CFW_SimSetACMMaxProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetServiceId(CFW_SIM_SRV_ID);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("--CFW_SimSetACMMax--pSetAcmMax->iACMMaxValue = 0x%X\n", 0x08100c24), pSetAcmMax->iACMMaxValue);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACMMax);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetACMMax(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    ACM_INFO_GETACMMAX *pGetAcmMax = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetACMMax);
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c25), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMax);
        return ret;
    }
//
// Parameter validity check
//
#if SIM_ARG_CHK_EN > 0
#if 0
    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c26));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMax);
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }
#endif
#endif
    pGetAcmMax = (ACM_INFO_GETACMMAX *)CSW_SIM_MALLOC(SIZEOF(ACM_INFO_GETACMMAX));
#if SIM_ARG_CHK_EN > 0
    if (pGetAcmMax == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c27));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMax);
        return ERR_NO_MORE_MEMORY;
    }
#endif
    pGetAcmMax->n_CurrStatus = CFW_SIM_MIS_IDLE;
    pGetAcmMax->n_PrevStatus = CFW_SIM_MIS_IDLE;
    pGetAcmMax->nTryCount = 0x01;

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetAcmMax, CFW_SimGetACMMaxProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetServiceId(CFW_SIM_SRV_ID);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetACMMax()\r\n", 0x08100c28)));
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACMMax);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetACM(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetACM);

    SIM_SM_INFO_GETACM *pGetACM = (SIM_SM_INFO_GETACM *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETACM));
    if (pGetACM == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c29));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACM);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory32(pGetACM, SIZEOF(SIM_SM_INFO_GETACM));

    pGetACM->nState.nCurrState = CFW_SIM_MIS_IDLE;
    pGetACM->nState.nPreState = CFW_SIM_MIS_IDLE;

    CFW_SetServiceId(CFW_SIM_SRV_ID);
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetACM, CFW_SimMisGetACMProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 0);

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetACM);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_SimGetACM return ERR_SUCCESS\n", 0x08100c2a));
    return (ERR_SUCCESS);
}

UINT32 CFW_SimSetACM(UINT32 iCurValue, UINT8 *pPIN2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetACM);
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x\n", 0x08100c2b), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACM);
        return ret;
    }

#if 0 //lines below is for test, the curValue should be array.
    iCurValue = 0x123456;
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("Error nPinSize 0x%x,iCurValue 0x%x\n",0x08100c2c), nPinSize, iCurValue);
#endif
    //ACM value should be three byte array, if store it to UINT32(iCurValue), it looks like:
    // highest byte of iCurValue = array[0]
    // second highest byte of iCurValue = array[1]
    // second lowest byte of iCurValue = array[2]
    // lowest byte of byte = array[3]
    // so don't need to check iCurValue for the range, just use first three bytes.
    if (nPinSize > 8)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("Error nPinSize 0x%x,iCurValue 0x%x\n", 0x08100c2d), nPinSize, iCurValue);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACM);
        return ERR_CFW_INVALID_PARAMETER;
    }
    SIM_SM_INFO_SETACM *pSetACM = (SIM_SM_INFO_SETACM *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_SETACM));
    if (pSetACM == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c2e));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACM);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory8(pSetACM, SIZEOF(SIM_SM_INFO_SETACM));

    SUL_MemSet8(pSetACM->nPIN2, 0xFF, 8);
    SUL_MemCopy8(pSetACM->nPIN2, pPIN2, nPinSize);
    //    pSetACM->nPIN2Size = nPinSize;
    UINT8 nACM[4];
    UINT32 order = 0x11223344;
    if (((UINT8 *)&order)[0] == 0x44) //little endian
    {
        nACM[0] = *(((UINT8 *)&iCurValue) + 2);
        nACM[1] = *(((UINT8 *)&iCurValue) + 1);
        nACM[2] = *((UINT8 *)&iCurValue);
        nACM[3] = 0;
    }
    else //big endian
    {
        nACM[0] = *(((UINT8 *)&iCurValue) + 1);
        nACM[1] = *(((UINT8 *)&iCurValue) + 2);
        nACM[2] = *(((UINT8 *)&iCurValue) + 3);
        nACM[3] = 0;
    }
    pSetACM->iACMValue = *((UINT32 *)nACM);
    pSetACM->nState.nCurrState = CFW_SIM_MIS_IDLE;
    pSetACM->nState.nPreState = CFW_SIM_MIS_IDLE;

    CFW_SetServiceId(CFW_SIM_SRV_ID);
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSetACM, CFW_SimMisSetACMProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_SimSetACM exit iACMValue = 0x%x", 0x08100c2f), pSetACM->iACMValue);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetACM);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimGetPUCT(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetPUCT()\r\n", 0x08100c30)));
    HAO hSimGetPUCTAo = 0;
    CFW_SIMGETPUCT *pSimGetPUCTData = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPUCT);
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c31), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCT);
        return ret;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c32));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCT);
        return ERR_CFW_NOT_EXIST_FREE_UTI;  // 替换掉了ERR_CFW_UTI_EXIST
    }
#endif
    pSimGetPUCTData = (CFW_SIMGETPUCT *)CSW_SIM_MALLOC(SIZEOF(CFW_SIMGETPUCT));
    if (pSimGetPUCTData == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c33));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCT);
        return ERR_NO_MORE_MEMORY;
    }

    hSimGetPUCTAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimGetPUCTData, CFW_SimGetPUCTProc, nSimID);
    CFW_SetUTI(hSimGetPUCTAo, nUTI, 0);
    pSimGetPUCTData->nSm_GetPUCT_prevStatus = CFW_SIM_MIS_IDLE;
    pSimGetPUCTData->nSm_GetPUCT_currStatus = CFW_SIM_MIS_IDLE;
    CFW_SetAoProcCode(hSimGetPUCTAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPUCT);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetPUCT() return ERR_SUCCESS\r\n", 0x08100c34)));
    return (ERR_SUCCESS);
}

UINT32 CFW_SimSetPUCT(CFW_SIM_PUCT_INFO *pPUCT, UINT8 *pPin2, UINT8 nPinSize, UINT16 nUTI, CFW_SIM_ID nSimID)
{
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimSetPUCT() begin\r\n", 0x08100c35)));
    UINT16 iEPPU = pPUCT->iEPPU;
    INT8 iEX = pPUCT->iEX; // iEX的值不大于7.
    HAO hSimSetPUCTAo = 0;

    CFW_SIMSETPUCT *pSimSetPUCTData = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetPUCT);
    UINT32 ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n", 0x08100c36), ret);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCT);
        return ret;
    }

    if ((iEPPU > 0xFFF) || (nPinSize < 4) || (nPinSize > 8) || (pPin2 == NULL)) // iEPPU占用12 bit
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(4), TSTR("Error iEPPU 0x%x,nPinSize 0x%x,nPinSize 0x%x,pPin2 0x%x\n", 0x08100c37), iEPPU, nPinSize, nPinSize, pPin2);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCT);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (pPUCT->iSign == 1) // EX为负数
    {
        iEX = -iEX;
    }

    if ((iEX < -7) || (iEX > 7)) // iEX占用三个bit，绝对值不应超过7
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Error iEX 0x%x \n", 0x08100c38), iEX);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCT);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Error UTI Busy\n",0x08100c39));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCT);
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }
#endif
    pSimSetPUCTData = (CFW_SIMSETPUCT *)CSW_SIM_MALLOC(SIZEOF(CFW_SIMSETPUCT));
    if (pSimSetPUCTData == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Error Malloc failed\n", 0x08100c3a));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCT);
        return ERR_NO_MORE_MEMORY;
    }

    hSimSetPUCTAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimSetPUCTData, CFW_SimSetPUCTProc, nSimID);
    CFW_SetUTI(hSimSetPUCTAo, nUTI, 0);

    pSimSetPUCTData->iCurrency[0] = pPUCT->iCurrency[0];
    pSimSetPUCTData->iCurrency[1] = pPUCT->iCurrency[1];
    pSimSetPUCTData->iCurrency[2] = pPUCT->iCurrency[2];
    pSimSetPUCTData->iEPPU = iEPPU;
    pSimSetPUCTData->iEX = iEX;
    SUL_MemCopy8(pSimSetPUCTData->pPin2, pPin2, nPinSize);
    UINT8 i = 0;
    for (i = nPinSize; i < 8; i++)
    {
        pSimSetPUCTData->pPin2[i] = 0xFF;
    }
    pSimSetPUCTData->nSm_SetPUCT_prevStatus = CFW_SIM_MIS_IDLE;
    pSimSetPUCTData->nSm_SetPUCT_currStatus = CFW_SIM_MIS_IDLE;

    CFW_SetAoProcCode(hSimSetPUCTAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPUCT);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimSetPUCT() return ERR_SUCCESS\r\n", 0x08100c3b)));
    return (ERR_SUCCESS);
}

/*
================================================================================
  Function   : CFW_SimGetICCID
--------------------------------------------------------------------------------
  Scope      :
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimGetICCID(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    SIM_SM_INFO_GETICCID *pGetICCID = NULL;

    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID))
    {
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }
    pGetICCID = (SIM_SM_INFO_GETICCID *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_GETICCID));

    if (pGetICCID == NULL)
    {
        return ERR_NO_MORE_MEMORY;
    }
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetICCID, CFW_SimGetICCIDProc, nSimID);
    SUL_ZeroMemory32(pGetICCID, SIZEOF(SIM_SM_INFO_GETICCID));
    TS_OutputText(CFW_SIM_TS_ID, TSTXT("CFW_SimGetICCID\r\n"));

    CFW_SetUTI(hAo, nUTI, 0);
    pGetICCID->n_CurrStatus = CFW_SIM_MIS_IDLE;
    pGetICCID->n_PrevStatus = CFW_SIM_MIS_IDLE;
    pGetICCID->nTryCount = 0x01;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    return (ERR_SUCCESS);
}

//#ifdef CFW_EXTENDED_API
#if 1
#if 0 //remove by wulc
UINT32 cfw_SimReadElementaryFile(   UINT16 nUTI,
                                    UINT32 fileId
                                    , CFW_SIM_ID nSimID
                                )
{
    HAO hAo;
    SIM_SM_INFO_EF *pUserData;

    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID))
    {
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }

    if (!(pUserData = (SIM_SM_INFO_EF *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_EF))))
    {
        return ERR_NO_MORE_MEMORY;
    }

    pUserData->nState.nNextState    = CFW_SIM_MIS_IDLE;
    pUserData->nState.nCurrState    = CFW_SIM_MIS_IDLE;
    pUserData->nTryCount            = 1;
    pUserData->fileSize             = 0;
    pUserData->fileId               = fileId;
    pUserData->update               = FALSE;

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pUserData, cfw_SimReadUpdateElementaryFileProc
                         , nSimID
                        );

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    return ERR_SUCCESS;
}
#endif
/*
================================================================================
  Function   : CFW_SimSetPrefOperatorListEX
--------------------------------------------------------------------------------
  Scope      : Set the PLMNs in SIM
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimSetPrefOperatorListEX(
    UINT8 *pOperatorList,
    UINT8 nSize,
    UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    UINT8 *temp = NULL;
    SIM_SM_INFO_SETPREFOPREATORLIST *pSetPrefLsit = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimSetPrefOperatorListEX);
    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SimSetPrefOperatorListEX()\r\n", 0x08100c3c)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_CFW_UTI_IS_BUSY;
    }
    pSetPrefLsit = (SIM_SM_INFO_SETPREFOPREATORLIST *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_SETPREFOPREATORLIST));
    if (pSetPrefLsit == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    SUL_ZeroMemory32(pSetPrefLsit, SIZEOF(SIM_SM_INFO_SETPREFOPREATORLIST));
    temp = (UINT8 *)CSW_SIM_MALLOC(nSize / 2);
    if (temp == NULL)
    {
        CSW_SIM_FREE(pSetPrefLsit);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);
        return ERR_NO_MORE_MEMORY; // should be updated by macro
    }
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSetPrefLsit, CFW_SimMisSetPrefListProcEX, nSimID);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimSetPrefOperatorList()\r\n", 0x08100c3d)));

    CFW_SetUTI(hAo, nUTI, 0);
    pSetPrefLsit->nSize = nSize / 2;

    cfw_BCDtoPLMN(pOperatorList, temp, nSize);
    SUL_MemSet8(pSetPrefLsit->nOperatorList, 0xFF, 255);
    SUL_MemCopy8(pSetPrefLsit->nOperatorList, temp, pSetPrefLsit->nSize);
    CSW_SIM_FREE(temp);

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimSetPrefOperatorList);

    return (ERR_SUCCESS);
}

/*
================================================================================
  Function   : CFW_SimGetPrefOperatorListEX
--------------------------------------------------------------------------------
  Scope      : Get the PLMNs in SIM
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimGetPrefOperatorListEX(
    UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo = 0;
    SIM_SM_INFO_GETPREFOPREATORLIST *pGetPrefList = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetPrefOperatorListEX);
    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SimGetPrefOperatorListEX()\r\n", 0x08100c3e)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListEX);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListEX);
        return ERR_CFW_UTI_IS_BUSY;
    }
    pGetPrefList = (SIM_SM_INFO_GETPREFOPREATORLIST *)CSW_SIM_MALLOC(
        SIZEOF(SIM_SM_INFO_GETPREFOPREATORLIST));
    if (NULL == pGetPrefList)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetPrefOperatorListEX()malloc failed\r\n", 0x08100c3f)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorList);
        return (ERR_NO_MORE_MEMORY); // should be updated by macro
    }

    SUL_ZeroMemory32(pGetPrefList, SIZEOF(SIM_SM_INFO_GETPREFOPREATORLIST));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetPrefList, CFW_SimMisGetPrefListProcEX, nSimID);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetPrefOperatorListEX()\r\n", 0x08100c40)));

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetPrefOperatorListEX);
    return (ERR_SUCCESS);
}
#if 0
UINT32 cfw_SimReadElementaryFile(   UINT16 nUTI,
                                    UINT32 fileId
                                    , CFW_SIM_ID nSimID
                                )
{
    HAO hAo;
    SIM_SM_INFO_EF *pUserData;

    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID))
    {
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }

    if (!(pUserData = (SIM_SM_INFO_EF *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_INFO_EF))))
    {
        return ERR_NO_MORE_MEMORY;
    }

    pUserData->nState.nNextState    = CFW_SIM_MIS_IDLE;
    pUserData->nState.nCurrState    = CFW_SIM_MIS_IDLE;
    pUserData->nTryCount            = 1;
    pUserData->fileSize             = 0;
    pUserData->fileId               = fileId;
    pUserData->update               = FALSE;

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pUserData, cfw_SimReadUpdateElementaryFileProc
                         , nSimID
                        );

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    return ERR_SUCCESS;
}
#endif

/*
================================================================================
  Function   : CFW_SimGetFileStatus
--------------------------------------------------------------------------------

  Scope      : Get the Sim File Status
  Parameters :
  Return     :
================================================================================
*/
//#else

UINT32 CFW_SimGetFileStatus(
    UINT8 nFileID,
    UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    GET_FILE_STATUS_INFO *pGetFileStatus = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimGetFileStatus);
    //
    //Parameter validity check
    //
    if (0xFF == nFileID)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR !nFileID error\r\n", 0x08100c41)));
        return ERR_FILE_NOT_FOUND;
    }

    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SimGetFileStatus()\r\n", 0x08100c42)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatus);
        return ERR_CFW_INVALID_PARAMETER;
    }
    if (ERR_SUCCESS != CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatus);
        return ERR_CFW_UTI_IS_BUSY;
    }
    pGetFileStatus = (GET_FILE_STATUS_INFO *)CSW_SIM_MALLOC(SIZEOF(GET_FILE_STATUS_INFO));
    if (pGetFileStatus == NULL)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatus);
        return ERR_NO_MORE_MEMORY;
    }
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pGetFileStatus, CFW_SimGetFileStatusProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    SUL_ZeroMemory32(pGetFileStatus, SIZEOF(GET_FILE_STATUS_INFO));
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimGetFileStatus()\r\n", 0x08100c43)));
    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pGetFileStatus->n_CurrStatus = CFW_SIM_MIS_IDLE;
    pGetFileStatus->n_PrevStatus = CFW_SIM_MIS_IDLE;
    pGetFileStatus->nFileID = nFileID;
    pGetFileStatus->nTryCount = 0x01;
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimGetFileStatus);
    return (ERR_SUCCESS);
}

/*
================================================================================
  Function   : CFW_SimReadRecord
--------------------------------------------------------------------------------
  Scope      :
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimReadRecord(
    UINT8 nFileID,
    UINT8 nRecordNum,
    UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadRecord);
    SIM_INFO_READRECORD *pReadRecord = NULL;

    if (0xFF == nFileID)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error\r\n", 0x08100c44)));
        return ERR_FILE_NOT_FOUND;
    }

    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SimReadRecord()\r\n", 0x08100c45)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
        return ERR_CFW_INVALID_PARAMETER;
    }

    pReadRecord = (SIM_INFO_READRECORD *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_READRECORD));
    if (pReadRecord == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100c46), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadMessage);
        return ERR_NO_MORE_MEMORY;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100c47), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }

    SUL_ZeroMemory32(pReadRecord, SIZEOF(SIM_INFO_READRECORD));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReadRecord, CFW_SimReadRecordProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimReadRecord()\r\n", 0x08100c48)));
    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pReadRecord->nFileId = nFileID;
    pReadRecord->n_CurrStatus = SIM_STATUS_IDLE;
    pReadRecord->n_PrevStatus = SIM_STATUS_IDLE;
    pReadRecord->nTryCount = 0x01;
    pReadRecord->nMode = SIM_MODE_ABSOLUTE;
    pReadRecord->nRecordNum = nRecordNum;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
    return (ERR_SUCCESS);
}

UINT32 CFW_SimReadRecordWithLen(
    UINT8 nFileID,
    UINT8 nRecordNum,
    UINT16 nLen,
    UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimReadRecord);
    SIM_INFO_READRECORD *pReadRecord = NULL;

    if (0xFF == nFileID)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error\r\n", 0x08100c49)));
        return ERR_FILE_NOT_FOUND;
    }

    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SimReadRecord()\r\n", 0x08100c4a)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    if((nFileID >= 15 && nFileID != 22 && nFileID != 43 && nFileID != 55 && nFileID != 151)  )
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2)|TSMAP(2), TSTR("The line is %d,the file is %s\n",0x08100c4b), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
        return ERR_INVALID_PARAMETER;
    }
#endif
    pReadRecord = (SIM_INFO_READRECORD *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_READRECORD));
    if (pReadRecord == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100c4c), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadMessage);
        return ERR_NO_MORE_MEMORY;
    }

    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100c4d), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
        return ERR_CFW_UTI_IS_BUSY;
    }

    SUL_ZeroMemory32(pReadRecord, SIZEOF(SIM_INFO_READRECORD));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReadRecord, CFW_SimReadRecordWithLenProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimReadRecord()\r\n", 0x08100c4e)));
    CFW_SetServiceId(CFW_SIM_SRV_ID);

    pReadRecord->nFileId = nFileID;
    pReadRecord->n_CurrStatus = SIM_STATUS_IDLE;
    pReadRecord->n_PrevStatus = SIM_STATUS_IDLE;
    pReadRecord->nTryCount = 0x01;
    pReadRecord->nMode = SIM_MODE_ABSOLUTE;
    pReadRecord->nRecordNum = nRecordNum;
    pReadRecord->nRecordSize = nLen;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimReadRecord);
    return (ERR_SUCCESS);
}
/*
================================================================================
  Function   : CFW_SimUpdateRecord
--------------------------------------------------------------------------------
  Scope      :
  Parameters :
  Return     :
================================================================================
*/
UINT32 CFW_SimUpdateRecord(
    UINT8 nFileID,
    UINT8 nRecordNum,
    UINT8 nRecordSize,
    //UINT8  * pPIN2,
    //UINT8    nPinSize,//Remove by lixp for AT issue
    UINT8 *pData,
    UINT16 nUTI, CFW_SIM_ID nSimID)
{
    HAO hAo;
    SIM_INFO_UPDATERECORD *pUpdateRecord = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimUpdateRecord);

    if (0xFF == nFileID)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error\r\n", 0x08100c4f)));
        return ERR_FILE_NOT_FOUND;
    }

    if (nSimID >= CFW_SIM_COUNT)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("ERROR ! SIM id error CFW_SimUpdateRecord()\r\n", 0x08100c50)));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecord);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0

    if((nFileID >= 15 &&  nFileID != 22 && nFileID != 43 && nFileID != 55 && nFileID != 151))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2)|TSMAP(2), TSTR("The line is %d,the file is %s\n",0x08100c51), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecord);
        return ERR_INVALID_PARAMETER;
    }
    if((nFileID == 7) || (nFileID == 22))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(2)|TSMAP(2), TSTR("The line is %d,the file is %s\n",0x08100c52), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecord);
        return ERR_INVALID_PARAMETER;
    }
#endif
    if (CFW_IsFreeUTI(nUTI, CFW_SIM_SRV_ID) != ERR_SUCCESS)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100c53), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecord);
        return ERR_CFW_NOT_EXIST_FREE_UTI;
    }
    pUpdateRecord = (SIM_INFO_UPDATERECORD *)CSW_SIM_MALLOC(SIZEOF(SIM_INFO_UPDATERECORD));
    if (pUpdateRecord == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(2) | TSMAP(2), TSTR("The line is %d,the file is %s\n", 0x08100c54), __LINE__, __FILE__);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecord);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory32(pUpdateRecord, SIZEOF(SIM_INFO_UPDATERECORD));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pUpdateRecord, CFW_SimUpdateRecordProc, nSimID);
    CFW_SetUTI(hAo, nUTI, 1);

    CFW_SetServiceId(CFW_SIM_SRV_ID);
    CSW_TRACE(_CSW | TLEVEL(CFW_SIM_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_SimUpdateRecord()\r\n", 0x08100c55)));

    pUpdateRecord->nRecordNum = nRecordNum;
    pUpdateRecord->nRecordSize = nRecordSize;
    pUpdateRecord->nFileId = nFileID;
    pUpdateRecord->nMode = SIM_MODE_ABSOLUTE;

    //Add by lixp 2007-12-06
    //Remove for AT issue
    //pUpdateRecord->nPIN2Size = nPinSize;
    //SUL_MemSet8(pUpdateRecord->nPIN2, 0xff, 8);
    //SUL_MemCopy8(pUpdateRecord->nPIN2,pPIN2,nPinSize);
    SUL_MemCopy8(pUpdateRecord->pData, pData, nRecordSize);

    pUpdateRecord->n_CurrStatus = SIM_STATUS_IDLE;
    pUpdateRecord->n_PrevStatus = SIM_STATUS_IDLE;
    pUpdateRecord->nTryCount = 0x01;

    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimUpdateRecord);
    return ERR_SUCCESS;
}
#endif
