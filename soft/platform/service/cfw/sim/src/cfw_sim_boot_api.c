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
#include "api_msg.h"
#include <cfw.h>
#include <base.h>
#include <sul.h>
#include <cfw_prv.h>
#include "csw_debug.h"
#include "cfw_sim.h"
#include "cfw_sim_boot.h"
#include <csw_mem_prv.h>
#include <base_prv.h>

BOOL gSimStage3Finish[CFW_SIM_COUNT] = { FALSE };

extern CFW_SIMSTATUS g_cfw_sim_status[CFW_SIM_COUNT] ;

UINT32 CFW_SimInitStage3Proc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_USimInitStage3Proc(HAO hAo, CFW_EV *pEvent);

UINT32 CFW_SimInitStage3(
    CFW_SIM_ID nSimID
)
{
#ifdef  LTE_NBIOT_SUPPORT
	UINT8 bFM;
#endif

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInitStage3 nSimID %d\n",0x08100aa5), nSimID);

    if (FALSE == gSimStage3Finish[nSimID])
    {
        gSimStage3Finish[nSimID] = TRUE;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error CFW_SimInitStage3 invoked more than one time\n",0x08100aa6));
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }


    HAO hSimInitializeAo = (HAO) 0;

    CFW_SIMINITS3 *pSimInitS3Data = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStage3);

    pSimInitS3Data = (CFW_SIMINITS3 *)CSW_SIM_MALLOC(SIZEOF(CFW_SIMINITS3));
    if (pSimInitS3Data == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100aa7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory8((VOID*)pSimInitS3Data, SIZEOF(CFW_SIMINITS3));

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)
        hSimInitializeAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimInitS3Data, CFW_SimInitStage3Proc, nSimID);
    else
        hSimInitializeAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimInitS3Data, CFW_USimInitStage3Proc, nSimID);

    pSimInitS3Data->nSm_SimInitS3_prevStatus = SM_SIMINITS3_INIT;
    pSimInitS3Data->nSm_SimInitS3_currStatus = SM_SIMINITS3_IDLE;
#ifdef  LTE_NBIOT_SUPPORT
		   CFW_StackInit(nSimID);  
	 CFW_GetComm((CFW_COMM_MODE *)&bFM, nSimID);
	 CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInitStage3 bFM %d\n",0x08100aa8), bFM);
	 if(CFW_DISABLE_COMM == bFM)
	 {
	     bFM = CFW_ENABLE_COMM;
	     CFW_CfgNwSetFM((CFW_COMM_MODE)bFM, nSimID);
	 }
#endif //LTE_NBIOT_SUPPORT
    CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage3);

    return ERR_SUCCESS;
}


//
// 同步函数,TRUE需要启动这ProfileDownload过程;FALSE要求SHELL启动Stage3过程。
//
BOOL CFW_SimInitStage2(CFW_SIM_ID nSimID)
{

    SIM_MISC_PARAM *pG_Mic_Param = NULL;
    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInitStage2);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInitStage2 nSimID %d\n",0x08100aa8), nSimID);
    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInitStage2 PHASE %d\n",0x08100aa9), pG_Mic_Param->nPhase);

    if ( SIM_PHASE2Plus <= pG_Mic_Param->nPhase )
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage2);
        return TRUE;
    }
    else
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(1), TSTR("Error pG_Mic_Param->nPhase 0x%x \n",0x08100aaa), pG_Mic_Param->nPhase);
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInitStage2);
        return FALSE;
    }
}

UINT32 CFW_SimInitStage1Proc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_USimInitStage1Proc(HAO hAo, CFW_EV *pEvent);

extern CFW_SIM_STATUS gSimStatus[];
UINT32 CFW_SimInitStage1(
    CFW_SIM_ID nSimID
)
{
    HAO hSimInitializeAo = (HAO) 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CFW_SimInitStage1\n",0x08100aab));

    CFW_SIMINITS1 *pSimInitS1Data = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInit);

    gSimStatus[nSimID] = CFW_SIM_STATUS_END;

    pSimInitS1Data = (CFW_SIMINITS1 *)CSW_SIM_MALLOC(SIZEOF(CFW_SIMINITS1));
    if (pSimInitS1Data == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100aac));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInit);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory8(pSimInitS1Data, SIZEOF(CFW_SIMINITS1));

    if(g_cfw_sim_status[nSimID].UsimFlag == FALSE)
        hSimInitializeAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimInitS1Data, CFW_SimInitStage1Proc, nSimID);
    else
        hSimInitializeAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimInitS1Data, CFW_USimInitStage1Proc, nSimID);


    pSimInitS1Data->nSm_SimInitS1_prevStatus = SM_SIMINITS1_INIT;
    pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_IDLE;

    CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInit);

    return ERR_SUCCESS;
}



UINT32 CFW_SimInit(
    BOOL bRstSim,
    CFW_SIM_ID nSimID
)
{
    HAO hSimInitializeAo = (HAO) 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInit[Sim %d]\n",0x08100aad), nSimID);

    SIM_SAT_PARAM   *pG_Sat_Param;
    SIM_CHV_PARAM   *pG_Chv_Param;
    SIM_PBK_PARAM   *pG_Pbk_Param;
    SIM_MISC_PARAM  *pG_Mic_Param;

    CFW_SIMINITS1 *pSimInitS1Data = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_SimInit);

    gSimStatus[nSimID]          = CFW_SIM_STATUS_END;
    g_SimInit3Status[nSimID]    = 0;
    gSimStage3Finish[nSimID]    = FALSE;

    pSimInitS1Data = (CFW_SIMINITS1 *)CSW_SIM_MALLOC(SIZEOF(CFW_SIMINITS1));
    if (NULL == pSimInitS1Data)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100aae));
        CSW_PROFILE_FUNCTION_EXIT(CFW_SimInit);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory8(pSimInitS1Data, SIZEOF(CFW_SIMINITS1));

    CFW_CfgSimGetSatParam(&pG_Sat_Param, nSimID);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);
    hSimInitializeAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimInitS1Data, CFW_SimInitStageBootProc, nSimID);

    // initialize sat parameter
    pG_Sat_Param->nCurCmd     = 0;
    pG_Sat_Param->pSatComData = NULL;
    pG_Sat_Param->pSetupMenu  = NULL;

    // initialize chv parameter
    pG_Chv_Param->bCHV2VerifyReq = FALSE;
    pG_Chv_Param->nFunID         = 0xFF;
    pG_Chv_Param->nCHVAuthen     = 0xFF;
    pG_Chv_Param->nPin1Status    = CHV_UNKNOW;
    pG_Chv_Param->nPin2Status    = CHV_UNKNOW;
    pG_Chv_Param->nPuk1Status    = CHV_UNKNOW;
    pG_Chv_Param->nPuk2Status    = CHV_UNKNOW;

    // initialize pbk parameter
    pG_Pbk_Param->nADNRecordSize    = 0;
    pG_Pbk_Param->nADNTotalNum      = 0;
    pG_Pbk_Param->nECCRecordSize    = 0;
    pG_Pbk_Param->nECCTotalNum      = 0;
    pG_Pbk_Param->nFDNRecordSize    = 0;
    pG_Pbk_Param->nFDNTotalNum      = 0;
    pG_Pbk_Param->nLNDRecordSize    = 0;
    pG_Pbk_Param->nLNDTotalNum      = 0;
    pG_Pbk_Param->nMSISDNRecordSize = 0;
    pG_Pbk_Param->nMSISDNTotalNum   = 0;

    // initialize mic parameter
    pG_Mic_Param->bImageDone   = 0;
    pG_Mic_Param->bStartPeriod = TRUE;
    pG_Mic_Param->nSimMode     = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CFW_SimInit bRstSim[Sim%d]\n",0x08100aaf), bRstSim);

    if(bRstSim == TRUE)
    {
        pSimInitS1Data->nSm_SimInitS1_prevStatus = SM_SIMINITS1_INIT;
        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_IDLE;
    }
    else
    {
        pSimInitS1Data->nSm_SimInitS1_prevStatus = SM_SIMINITS1_RESET;
        pSimInitS1Data->nSm_SimInitS1_currStatus = SM_SIMINITS1_RESET;
    }

    CFW_SetAoProcCode(hSimInitializeAo, CFW_AO_PROC_CODE_CONTINUE);

    CSW_PROFILE_FUNCTION_EXIT(CFW_SimInit);

    return ERR_SUCCESS;
}



UINT32 CFW_SimCloseProc(HAO hAo, CFW_EV *pEvent);
UINT32 CFW_SimClose(UINT16 nUTI, CFW_SIM_ID nSimID)
{
    SIM_SM_STATE* pSimClose = (UINT8*)CSW_SIM_MALLOC(sizeof(SIM_SM_STATE));
    if (pSimClose == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("ERROR: CFW_SimClose",0x08100ab0));
        return ERR_NO_MORE_MEMORY;
    }

    memset(pSimClose,0, sizeof(SIM_SM_STATE));
    HAO hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimClose, CFW_SimCloseProc, nSimID);

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("===========CFW_SimClose===========",0x08100ab1));

    return ERR_SUCCESS;
}


UINT32 CFW_SimReset(UINT16 nUTI
                    , CFW_SIM_ID nSimID
                   )
{
    HAO hAo = 0x00;
    SIM_SM_RESET *pReset = NULL;

    UINT32 ret = ERR_SUCCESS;

    ret = CFW_CheckSimId(nSimID);

    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100ab2), ret);
        return ret;
    }

    pReset = (SIM_SM_RESET *)CSW_SIM_MALLOC(SIZEOF(SIM_SM_RESET));

    if (pReset == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ab3));
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }

    SUL_ZeroMemory8((VOID*)pReset, SIZEOF(SIM_SM_RESET));


    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pReset, CFW_SimResetProc
                         , nSimID
                        );

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    return ERR_SUCCESS;
}
#ifdef _DROPCARD_AUTOPIN_

UINT32 CFW_SimResetEx(UINT16 nUTI,CFW_SIM_ID nSimID,BOOL flag)
{
    CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("In CFW_SimResetEx\n",0x08100ab4));
    HAO hAo = 0x00;
    CFW_SIMINITS *pSimInitData = NULL;
    /*
    UINT32 ret = ERR_SUCCESS;
    ret = CFW_CheckSimId(nSimID);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("Error CFW_CheckSimId return 0x%x \n",0x08100ab5), ret);
        return ret;
    }*/
    pSimInitData = (SIM_SM_RESET *)CSW_SIM_MALLOC(SIZEOF(CFW_SIMINITS));
    if (pSimInitData == NULL)
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_SIM_TS_ID | C_ERROR)|TDB|TNB_ARG(0), TSTR("Error Malloc failed\n",0x08100ab6));
        return ERR_NO_MORE_MEMORY;  // should be updated by macro
    }
    SUL_ZeroMemory32(pSimInitData, SIZEOF(CFW_SIMINITS));
    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pSimInitData, CFW_SimResetExProc,nSimID);
    if(TRUE == flag)
    {
    }
    else
    {
        CFW_SetSpecialAo(hAo);
    }
    pSimInitData->nSm_SimInit_currStatus = SM_SIMINITS1_IDLE;
    pSimInitData->nSm_SimInit_prevStatus = SM_SIMINITS1_INIT;
    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    return ERR_SUCCESS;
}
#endif
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

    hAo = CFW_RegisterAo(CFW_SIM_SRV_ID, pUserData, CFW_SimReadUpdateElementaryFileProc
                         , nSimID
                        );

    CFW_SetUTI(hAo, nUTI, 0);
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_CONTINUE);

    return ERR_SUCCESS;
}



UINT32 CFW_SimDeInit(
    CFW_SIM_ID nSimID
)
{
    SIM_SAT_PARAM   *pG_Sat_Param;
    SIM_CHV_PARAM   *pG_Chv_Param;
    SIM_PBK_PARAM   *pG_Pbk_Param;
    SIM_MISC_PARAM  *pG_Mic_Param;

    gSimStatus[nSimID]          = CFW_SIM_STATUS_END;
    g_SimInit3Status[nSimID]    = 0;
    gSimStage3Finish[nSimID]    = FALSE;

    CFW_CfgSimGetSatParam(&pG_Sat_Param, nSimID);
    CFW_CfgSimGetChvParam(&pG_Chv_Param, nSimID);
    CFW_CfgSimGetPbkParam(&pG_Pbk_Param, nSimID);
    CFW_CfgSimGetMicParam(&pG_Mic_Param, nSimID);

    if(pG_Sat_Param->pSetupMenu)
    {
        CSW_SIM_FREE(pG_Sat_Param->pSetupMenu);
    }
    SUL_ZeroMemory8(pG_Sat_Param, SIZEOF(SIM_SAT_PARAM));
    SUL_ZeroMemory8(pG_Chv_Param, SIZEOF(SIM_CHV_PARAM));
    SUL_ZeroMemory8(pG_Pbk_Param, SIZEOF(SIM_PBK_PARAM));
    SUL_ZeroMemory8(pG_Mic_Param, SIZEOF(SIM_MISC_PARAM));
    return ERR_SUCCESS;
}


