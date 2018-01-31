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
#include <sul.h> // only for my test
#include "api_msg.h"
#include <cfw.h>
#include <ts.h>
#include <cfw_prv.h>
#include <event.h>
#include "csw_debug.h"
#if (CFW_CM_DUMP_ENABLE==0) && (CFW_CM_SRV_ENABLE==1)

#include "cfw_cc_data.h"
#include "cfw_cc_tools.h"


UINT32 CFW_CheckSimId(CFW_SIM_ID nSimId)
{
    UINT8 nSimStatus = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CheckSimId);
    //check parameter SIM ID
    if(nSimId >= CFW_SIM_COUNT)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CheckSimId);
        return ERR_CFW_INVALID_PARAMETER;
    }
#if 0
    //check SIM Status
    nSimStatus = CFW_GetSimStatus(nSimId);
    if(CFW_SIM_ABSENT == nSimStatus)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CheckSimId);
        return ERR_CFW_SIM_NOT_INITIATE;
    }
    else if(CFW_SIM_ABNORMAL == nSimStatus)
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CheckSimId);
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
#endif
    CSW_PROFILE_FUNCTION_EXIT(CFW_CheckSimId);
    return ERR_SUCCESS;

}
//extern BOOL gCSSI; //bug 10131
//extern BOOL gCSSU; //bug 10177
//==================================================================
//Param:  pcIn: BCD code;
//        pcOut: string
//        iLength: BCD code length
//==================================================================
VOID CallingPartyBCD2String(UINT8 *pcIn, UINT8 *pcOut, UINT8 iLength)
{
    UINT8 i = 0;
    CSW_PROFILE_FUNCTION_ENTER(CallingPartyBCD2String);
    if (!((*pcIn) & 0x80))
    {
        pcIn++;
        iLength--;
    }

    pcIn++;
    iLength--;

    for (i = 0; i < iLength; i++)
    {
        pcOut[2 * i]     = (pcIn[i] & 0x0F) + 0x30;
        pcOut[2 * i + 1] = ((pcIn[i] & 0xF0) >> 4) + 0x30;
    }

    if (pcOut[2 * i - 1] == 0x3F)
    {
        pcOut[2 * i - 1] = 0;
    }
    else
    {
        pcOut[2 * i] = 0;
    }
    CSW_PROFILE_FUNCTION_EXIT(CallingPartyBCD2String);
}


//==================================================================
//
//
//==================================================================
UINT8 CallingPartyrRaw2BCD(UINT8 *pcIn, UINT8 *pcOut, UINT8 iLength)
{
    CSW_PROFILE_FUNCTION_ENTER(CallingPartyrRaw2BCD);

    if( !iLength )
        return 0;

    if (!((*pcIn) & 0x80))
    {
        pcIn++;
        iLength--;
    }

    pcIn++;
    iLength--;

    SUL_MemCopy8(pcOut, pcIn, iLength);
    CSW_PROFILE_FUNCTION_EXIT(CallingPartyrRaw2BCD);
    return iLength;
}


//==================================================================
//Param:  pcIn: string;
//        pcOut: BCD code
//        iLength: string length
//==================================================================
VOID String2BCD(UINT8 *pcIn, UINT8 *pcOut, UINT8 iLength)
{
    UINT8 i, j;
    CSW_PROFILE_FUNCTION_ENTER(String2BCD);
    for (i = 0; i < (iLength + 1) / 2; i++)
    {
        pcOut[i] = (pcIn[2 * i] & 0xf) |
                   ((pcIn[2 * i + 1] & 0xf ) << 4);
    }
    for(j = 0; j < iLength; j++)
        //SXS_TRACE(_MMI, "MO Call CallingNum phone_number[%d] = %x\n", j, pcIn[j]);

        if ( iLength & 1 )
        {
            pcOut[i - 1] |= 0xf0;
        }

    CSW_PROFILE_FUNCTION_EXIT(String2BCD);
}



//==============================================================================
// Function     : CII_CC_Release
// Description  : The MMI call this function to send a RELEASE message to the
//                network after MMI received E_CC_DISC event.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_Release(
    CM_SM_INFO *proc,
    UINT8    nCause
    , CFW_SIM_ID nSimId
)
{
    api_CcReleaseReq_t *sOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Release() begin \n",0x08100455));

    CSW_PROFILE_FUNCTION_ENTER(CC_Release);
    sOutMsg = (api_CcReleaseReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcReleaseReq_t));

    sOutMsg->TI          = proc->nTI;
    //Cause 1
    sOutMsg->Cause1Len   = 2;
    sOutMsg->Cause1[0] = 0xE0;
    sOutMsg->Cause1[1] = (UINT8)(nCause | (1 << 7));
    //Cause 2
    sOutMsg->Cause2Len   = 0;
    //UserUser
    sOutMsg->UserUserLen = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Release()Send CcReleaseReq\n",0x08100456));

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_Release() Event==>>Stack: API_CC_RELEASE_REQ 0x%x\n",0x08100457), API_CC_RELEASE_REQ);
    CFW_SendSclMessage (API_CC_RELEASE_REQ, sOutMsg, nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Release() end \n",0x08100458));

    CSW_PROFILE_FUNCTION_EXIT(CC_Release);
}


//==============================================================================
// Function     : CC_Disc
// Description  : The MMI call this function to send a RELEASE message to the
//                network after MMI received E_CC_DISC event.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_Disc(
    CM_SM_INFO *proc,
    UINT8    nCause
    , CFW_SIM_ID nSimId
)
{
    api_CcDiscReq_t *sOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Disc() begin \n",0x08100459));

    CSW_PROFILE_FUNCTION_ENTER(CC_Disc);
    sOutMsg = (api_CcDiscReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcDiscReq_t));

    sOutMsg->TI = proc->nTI;
    //Cause
    sOutMsg->CauseLen = 2;
    sOutMsg->Cause[0] = 0xE0;
    sOutMsg->Cause[1] = (UINT8)(nCause | (1 << 7));
    //UserUser
    sOutMsg->UserUserLen = 0;
    //FacilityIE
    sOutMsg->Facility.StructValid = FALSE;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Disc() send discnect API_CC_DISC_REQ ==>>stack \n",0x0810045a));
    CFW_SendSclMessage (API_CC_DISC_REQ, sOutMsg, nSimId);

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Disc() end \n",0x0810045b));

    CSW_PROFILE_FUNCTION_EXIT(CC_Disc);
}


//==============================================================================
// Function     : CII_CC_ReleaseCmplt
// Description  : The MMI call this function when the following two case
//                  happened:
//                (1)When an incoming call arrived, if user wants to reject
//                   the call, so MMI call this function to reject an incoming
//                   call, here, eCause equal to #21(CALL_REJECTED);
//                (2)At the normal call release case: when received RELEASE
//                message, MMI will call this function to release call
//                normally, here, eCause equal to #16(NORMAL_CALL_CLEARING);
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_ReleaseCmplt( HAO hAo, UINT8 nCause)
{
    CM_SM_INFO *proc = NULL;
    api_CcReleaseCmpltReq_t *sOutMsg = NULL;
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ReleaseCmplt()begin\n",0x0810045c));

    CSW_PROFILE_FUNCTION_ENTER(CC_ReleaseCmplt);
    sOutMsg = (api_CcReleaseCmpltReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcReleaseCmpltReq_t));

    proc = (CM_SM_INFO *)CFW_GetAoUserData(hAo);
    if(proc != NULL)
    {
        sOutMsg->TI = proc->nTI;
        //Cause
        sOutMsg->CauseLen = 2;
        sOutMsg->Cause[0] = 0xE0;
        sOutMsg->Cause[1] = (UINT8)(nCause | (1 << 7));
        //UserUser
        sOutMsg->UserUserLen = 0;

        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ReleaseCmplt()Send RELEASE COMPLETE\n",0x0810045d));
        CFW_SendSclMessage (API_CC_RELEASECMPLT_REQ, sOutMsg, nSimId);
        CFW_SetAoState(hAo, CFW_SM_STATE_IDLE);

        //Release the current index
        CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ReleaseCmplt()end\n",0x0810045e));

    CSW_PROFILE_FUNCTION_EXIT(CC_ReleaseCmplt);
}



//==============================================================================
//
//if only this MT call, nCause:0xff
//else, nCause is set :CFW_CM_USER_BUSY
//==============================================================================
VOID CC_CallConfirm(
    UINT8 nTI,
    UINT8 nCause
    , CFW_SIM_ID nSimId
)
{
    api_CcCallConfReq_t *p = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CC_CallConfirm);

    p = (api_CcCallConfReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcCallConfReq_t));
    p->TI = nTI;
    //Cause
    if(nCause == 0xff)
    {
        p->CauseLen = 0;
    }
    else
    {
        p->CauseLen   = 2;
        p->Cause[0] = 0xE0;
        p->Cause[1] = (UINT8)(nCause | (1 << 7));

    }
    //Bearer1



#ifdef CFW_AMR_SUPPORT
    if(!IsDTMF_Detect_Start())
    {
        p->Bearer1Len = 6;
        p->Bearer1[0] = 0x60;
        p->Bearer1[1] = 0x04; // Speech version 3
        p->Bearer1[2] = 0x02; // Speech version 2
        p->Bearer1[3] = 0x00; // Speech version 1
        p->Bearer1[4] = 0x05; // HR Speech version 3
        p->Bearer1[5] = 0x81; // HR Speech version 1
    }
    else
    {
        p->Bearer1Len = 4;
        p->Bearer1[0] = 0x60;
        //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        p->Bearer1[1] = 0x02; // Speech version 2
        p->Bearer1[2] = 0x00; // Speech version 1
        p->Bearer1[3] = 0x81; // HR Speech version 1
    }
#else
    p->Bearer1Len = 4;
    p->Bearer1[0] = 0x60;
    //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
    p->Bearer1[1] = 0x02; // Speech version 2
    p->Bearer1[2] = 0x00; // Speech version 1
    p->Bearer1[3] = 0x81; // HR Speech version 1

#endif


    //Bearer2
    p->Bearer2Len = 0;
    CFW_SendSclMessage (API_CC_CALLCONF_REQ, p, nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_CallConfirm(),CC==>>stack API_CC_CALLCONF_REQ 0x%x\n",0x0810045f), API_CC_CALLCONF_REQ);

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("in CC_CallConfirm(), send CALL CONFIRM MESSAGE\n",0x08100460));
    CSW_PROFILE_FUNCTION_EXIT(CC_CallConfirm);
}


//==============================================================================
// Function     : CC_Alert
// Description:The MMI call this function to send a ALERT message to the
//             network.If the signal information element was present in the
//             SETUP message received from network, user alerting is
//             initiated at the mobile station side; if the signal
//             information element was not present in the SETUP message,
//             user alerting is initiated when MMI receive E_CC_AUDIOON.
//             Here, initiation of user alerting means:
//                - the generation of an appropriate tone or indication
//                      at the mobile station; and
//                    - sending of an ALERTING message
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_Alert(
    UINT8 nTI
    , CFW_SIM_ID nSimId
)
{
    api_CcAlertReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CC_Alert);

    sOutMsg = (api_CcAlertReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcAlertReq_t));

    sOutMsg->TI = nTI;
    //UserUser
    sOutMsg->UserUserLen = 0;
    //FacilityIE
    sOutMsg->Facility.StructValid = FALSE;
    CFW_SendSclMessage (API_CC_ALERT_REQ, sOutMsg, nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("in CC_Alert(), send ALERT MESSAGE\n",0x08100461));
    CSW_PROFILE_FUNCTION_EXIT(CC_Alert);
}




//==============================================================================
//
//
//
//==============================================================================
UINT32 CC_RejectCall (
    CFW_SIM_ID nSimId
)
{
    HANDLE hSm;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_RejectCall()begin\n",0x08100462));

    CSW_PROFILE_FUNCTION_ENTER(CC_RejectCall);
    CFW_SetServiceId(CFW_CC_SRV_ID);
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_MTPROC, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_RejectCall() Loop value i=%d\n",0x08100463), i);
    if((i < 7) && (hSm != HNULL))
    {
        proc = CFW_GetAoUserData(hSm);
        proc->nNextSubState = proc->nCurSubState;
        proc->nCurSubState = SUBSTATE_MT_PROC_CHLD0;
        CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);

    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_RejectCall()end\n",0x08100464));

    CSW_PROFILE_FUNCTION_EXIT(CC_RejectCall);
    return(ERR_SUCCESS);
}


//==============================================================================
// Function     : CC_Facility
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_Facility(
    CM_SM_INFO *proc,
    UINT32 nOperationCode
    , CFW_SIM_ID  nSimId
)
{
    api_CcFacilityReq_t *sOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Facility() begin\n",0x08100465));

    CSW_PROFILE_FUNCTION_ENTER(CC_Facility);
    sOutMsg = (api_CcFacilityReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcFacilityReq_t));

    sOutMsg->TI        = proc->nTI;

    //FacilityIE
    sOutMsg->Facility.StructValid = TRUE;
    sOutMsg->Facility.InvokeId = proc->nTI;
    sOutMsg->Facility.ss_Code = ss_MPTY;
    sOutMsg->Facility.ss_Operation = nOperationCode;

    SUL_ZeroMemory8(&(sOutMsg->Facility.crss_Deflec), SIZEOF(api_CrssDeflec_t ));
    SUL_ZeroMemory8(&(sOutMsg->Facility.UUS), SIZEOF(sOutMsg->Facility.UUS));
    CFW_SendSclMessage (API_CC_FACILITY_REQ, sOutMsg, nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_Facility() ==>>stack TI: %x\n",0x08100466), sOutMsg->TI);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Facility() end\n",0x08100467));

    CSW_PROFILE_FUNCTION_EXIT(CC_Facility);
}


//==============================================================================
// Function     : CC_TerminateOne
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_TerminateOne(
    UINT32 nState,
    UINT32 nSubState,
    UINT8 HoldFunction
    , CFW_SIM_ID nSimId
)
{
    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_TerminateOne()begin\n",0x08100468));
    CSW_PROFILE_FUNCTION_ENTER(CC_TerminateOne);
    CFW_SetServiceId(CFW_CC_SRV_ID);  // Added by Qiyan
    while(((hSm = CFW_GetAoHandle(i, nState, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_TerminateOne() Loop value i=%d\n",0x08100469), i);
    if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_HELD state
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if(proc != NULL)
        {
            proc->nHoldFunction = HoldFunction;
            proc->nNextSubState = proc->nCurSubState;
            proc->nCurSubState = nSubState;
            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);

        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CC_TerminateOne);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_TerminateOne()end\n",0x0810046a));
}


//==============================================================================
// Function     : CC_TerminateMulti
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_TerminateMulti(
    UINT32 nState,
    UINT32 nSubState,
    UINT8 HoldFunction
    , CFW_SIM_ID nSimId
)
{
    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_TerminateMulti()begin\n",0x0810046b));

    CSW_PROFILE_FUNCTION_ENTER(CC_TerminateMulti);
    CFW_SetServiceId(CFW_CC_SRV_ID);
    for(i = 0; i < 7; i++)
    {
        if((hSm = CFW_GetAoHandle(i, nState, CFW_CC_SRV_ID, nSimId)) != HNULL)
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_TerminateMulti() AO index =%d\n",0x0810046c), i);
            proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
            if(proc != NULL)
            {
                proc->nHoldFunction = HoldFunction;
                proc->nNextSubState = proc->nCurSubState;
                proc->nCurSubState = nSubState;
                CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);

            }

        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CC_TerminateMulti);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_TerminateMulti()end\n",0x0810046d));
}



//==============================================================================
// Function     : CC_Retrieve
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_RetrieveCall(
    HAO hOldSm
    , CFW_SIM_ID nSimId
)
{

    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;
    api_CcCallRetrieveReq_t *sOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_RetrieveCall()begin\n",0x0810046e));

    CSW_PROFILE_FUNCTION_ENTER(CC_RetrieveCall);

    sOutMsg = (api_CcCallRetrieveReq_t *) NULL;

    CFW_SetServiceId(CFW_CC_SRV_ID);

    if(hOldSm == HNULL)
    {
        while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_HELD, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
        {
            i++;

        }
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_RetrieveCall() Loop value i=%d\n",0x0810046f), i);

        if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_HELD state
        {

            proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        }
    }
    else
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hOldSm);
    }


    if(proc != NULL)
    {
        sOutMsg = (api_CcCallRetrieveReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcCallRetrieveReq_t));
        sOutMsg->TI        = proc->nTI;
        proc->pMsg = (VOID *)sOutMsg;
        proc->nNextSubState = proc->nCurSubState;

        proc->nCurSubState = SUBSTATE_HELD_CHLD1;

        if(hOldSm == HNULL)
            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        else
            CFW_SetAoProcCode(hOldSm, CFW_AO_PROC_CODE_MULTIPLE);
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_RetrieveCall()end\n",0x08100470));

    CSW_PROFILE_FUNCTION_EXIT(CC_RetrieveCall);
    return;

}

//==============================================================================
// Function     : CC_Hold
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_HoldCall(
    CFW_SIM_ID nSimId
)
{

    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;
    api_CcCallHoldReq_t *sOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_HoldCall()begin\n",0x08100471));

    CSW_PROFILE_FUNCTION_ENTER(CC_HoldCall);
    sOutMsg = (api_CcCallHoldReq_t *)NULL;
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_ACTIVE, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_HoldCall() Loop value i=%d\n",0x08100472), i);
    if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_ACTIVE state
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if(proc != NULL)
        {
            sOutMsg = (api_CcCallHoldReq_t *)CFW_MemAllocMessageStructure( SIZEOF(api_CcCallHoldReq_t) );
            sOutMsg->TI        = proc->nTI;
            proc->pMsg = (VOID *)sOutMsg;
            proc->nNextSubState = proc->nCurSubState;

            proc->nCurSubState = SUBSTATE_ACTIVE_CHLD2;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_HoldCall()end\n",0x08100473));

    CSW_PROFILE_FUNCTION_EXIT(CC_HoldCall);
}


VOID CC_Hold_AcceptCall(
    CFW_SIM_ID nSimId
)
{

    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;
    api_CcCallHoldReq_t *sOutMsg = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Hold_AcceptCall()begin\n",0x08100474));

    CSW_PROFILE_FUNCTION_ENTER(CC_Hold_AcceptCall);
    sOutMsg = (api_CcCallHoldReq_t *)NULL;
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_ACTIVE, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_Hold_AcceptCall() Loop value i=%d\n",0x08100475), i);
    if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_ACTIVE state
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if(proc != NULL)
        {
            sOutMsg = (api_CcCallHoldReq_t *)CFW_MemAllocMessageStructure( SIZEOF(api_CcCallHoldReq_t) );
            sOutMsg->TI        = proc->nTI;
            proc->nHoldFunction = 0x2F;
            proc->pMsg = (VOID *)sOutMsg;
            proc->nNextSubState = proc->nCurSubState;

            proc->nCurSubState = SUBSTATE_ACTIVE_CHLD2;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_Hold_AcceptCall()end\n",0x08100476));

    CSW_PROFILE_FUNCTION_EXIT(CC_Hold_AcceptCall);
}


//==============================================================================
// Function     : CC_HoldMPTY
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_HoldMPTY(
    CFW_SIM_ID nSimId
)
{
    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_HoldMPTY()begin\n",0x08100477));

    CSW_PROFILE_FUNCTION_ENTER(CC_HoldMPTY);
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_INMEETING, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_HoldMPTY() Loop value i=%d\n",0x08100478), i);
    if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_INMEETING state
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if(proc != NULL)
        {

            proc->nNextSubState = proc->nCurSubState;
            proc->nCurSubState = SUBSTATE_INMEETING_CHLD2;
            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_HoldMPTY()end\n",0x08100479));

    CSW_PROFILE_FUNCTION_EXIT(CC_HoldMPTY);
}

VOID CC_HoldMPTY_Acceptcall(
    CFW_SIM_ID nSimId
)
{
    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_HoldMPTY_Acceptcall()begin\n",0x0810047a));

    CSW_PROFILE_FUNCTION_ENTER(CC_HoldMPTY_Acceptcall);
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_INMEETING, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_HoldMPTY_Acceptcall() Loop value i=%d\n",0x0810047b), i);

    if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_INMEETING state
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if(proc != NULL)
        {

            proc->nNextSubState = proc->nCurSubState;
            proc->nHoldFunction = 0x2F;
            proc->nCurSubState = SUBSTATE_INMEETING_CHLD2;
            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_HoldMPTY_Acceptcall()end\n",0x0810047c));

    CSW_PROFILE_FUNCTION_EXIT(CC_HoldMPTY_Acceptcall);
}




//==============================================================================
// Function     : CC_RetrieveMPTY
// Description  : call this function to send an action through the Facility
//                  message for the call related SS. such as MPTY.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_RetrieveMPTY(
    CFW_SIM_ID nSimId
)
{
    HAO hSm = 0;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_RetrieveMPTY()begin\n",0x0810047d));

    CSW_PROFILE_FUNCTION_ENTER(CC_RetrieveMPTY);

    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_MEETINGHELD, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_RetrieveMPTY() Loop value i=%d\n",0x0810047e), i);

    if((i < 7) && (hSm != HNULL))// Have hSm in CFW_SM_CM_STATE_MEETINGHELD state
    {
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if(proc != NULL)
        {
            proc->nNextSubState = proc->nCurSubState;
            proc->nCurSubState = SUBSTATE_MEETINGHLD_CHLD1;
            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        }
    }

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_RetrieveMPTY()end\n",0x0810047f));

    CSW_PROFILE_FUNCTION_EXIT(CC_RetrieveMPTY);
}



//==============================================================================
// Function     : CC_StartDTMF
// Description  : call this function to start transmitting DTMF messages.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_StartDTMF(
    CM_SM_INFO *proc,
    UINT8  iDtmf
    , CFW_SIM_ID  nSimId
)
{
    api_CcStartDTMFReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CC_StartDTMF);
    sOutMsg = (api_CcStartDTMFReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcStartDTMFReq_t));

    //CII_MelodyClose **************
    //CII_MelodyClose *************

    sOutMsg->TI = proc->nTI;
    sOutMsg->KeyPressed = iDtmf;
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_StartDTMF send DTMF:%x\n",0x08100480), iDtmf);
    CFW_SendSclMessage (API_CC_STARTDTMF_REQ, sOutMsg, nSimId);
    CSW_PROFILE_FUNCTION_EXIT(CC_StartDTMF);
}



//==============================================================================
// Function     : CC_StopDTMF
// Description  : call this function to stop transmitting DTMF messages.
// Arguments    :
//
// Return value :
//
//==============================================================================
VOID CC_StopDTMF(
    CM_SM_INFO *proc
    , CFW_SIM_ID  nSimId
)
{
    UINT8 *nTI = 0;

    CSW_PROFILE_FUNCTION_ENTER(CC_StopDTMF);
    nTI = (UINT8 *)CFW_MemAllocMessageStructure(SIZEOF(UINT8));
    *nTI = proc->nTI;
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_StopDTMF\n",0x08100481));

    CFW_SendSclMessage (API_CC_STOPDTMF_REQ, nTI, nSimId);
    CSW_PROFILE_FUNCTION_EXIT(CC_StopDTMF);
}




//==============================================================================
//
//
//==============================================================================
UINT8 CC_GetIndex(
    CFW_SIM_ID nSimId
)
{
    HAO    hSm = 0;
    CM_SM_INFO *GetProc;
    UINT8 nAllIndex = 0;
    UINT8 i = 0;
    UINT8 j = 0;

    CSW_PROFILE_FUNCTION_ENTER(CC_GetIndex);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_GetIndex begin\n",0x08100482));
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId) ) != HNULL) && (i < 7))
    {
        GetProc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
        if (GetProc )
        {
            if((GetProc->nIndex != 0xff) && (GetProc->nIndex >= 1) && (GetProc->nIndex <= 7))
            {
                nAllIndex |= ( 1 << (GetProc->nIndex - 1));
            }
            i++;
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_GetIndex loop value i=%d\n",0x08100483), i);
    while(j < 7)
    {
        if(nAllIndex & (1 << j))
            j++;
        else
        {
            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_GetIndex return (j+1)=%d\n",0x08100484), (j + 1));
            CSW_PROFILE_FUNCTION_EXIT(CC_GetIndex);
            return (j + 1);
        }

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CC_GetIndex return error\n",0x08100485));
    CSW_PROFILE_FUNCTION_EXIT(CC_GetIndex);
    return 0xff;//???????????

}



//==============================================================================
//
//
//
//==============================================================================
VOID CC_ProcNwDisconnectMsg(HAO hAo, CM_SM_INFO *proc, VOID *nEvParam)
{
    UINT8 iProgressIndValue;
    UINT32 nUTI;
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwDisconnectMsg() begin \n",0x08100486));

    CSW_PROFILE_FUNCTION_ENTER(CC_ProcNwDisconnectMsg);
    CFW_GetUTI(hAo, &nUTI);

    api_CcDiscInd_t *p = (api_CcDiscInd_t *)nEvParam;

    iProgressIndValue = p->ProgressInd[1] & 0x7F;

    if (iProgressIndValue == 8)
    {
        proc->bProgressIndFlag = TRUE;
    }
    else
    {
        proc->bProgressIndFlag = FALSE;
    }

    //******************************************
    //mmi_audioCallStop==>stop speech,MIC,Speaker
    //******************************************

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTR("CC_ProcNwDisconnectMsg() iProgressIndValue = 0x%x   proc->bProgressIndFlag = 0x%x\n",0x08100487), iProgressIndValue, proc->bProgressIndFlag);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ProcNwDisconnectMsg() ,proc->nCallState = 0x%x\n",0x08100488), proc->nCallState);
    //[[hameina[+] 2007.9.10 :when no answer of busy, close the sounder
    if(proc->nCallState == CFW_CM_STATUS_DIALING || proc->nCallState == CFW_CM_STATUS_ALERTING)
    {
        CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, nUTI | (nSimId << 24), 0); //hameina[+]2007.9.6 sounder 0 event
    }
#if 0//def CHIP_HAS_AP
    CloseAudio();
#endif
    //]]hameina [+] 2007.9.10 :when no answer of busy, close the sounder
    proc->nCallState = CFW_CM_STATUS_RELEASED;
    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MTDISC);

    //Judge progressIndFlag and decide whether to Send RELEASE to network
    if ((!proc->bProgressIndFlag) || (proc->bProgressIndFlag && (!proc->bAudioOnFlag) ))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwDisconnectMsg() send release msg to nw directly \n",0x08100489));
        CC_Release(proc, CFW_CM_NORMAL_CLEARING, nSimId);
        proc->nCalledFuncFlag = 16 ;
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwDisconnectMsg() end \n",0x0810048a));

    CSW_PROFILE_FUNCTION_EXIT(CC_ProcNwDisconnectMsg);
}



//==============================================================================
//
//
//
//==============================================================================
VOID CC_ProcNwReleaseMsg(HAO hAo, CM_SM_INFO *proc, UINT8 iCauseValue)
{
    UINT32 nUTI;
    UINT8 nCssi, nCssu;
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    CFW_CfgGetSSN(&nCssi, &nCssu, nSimId);

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwReleaseMsg()begin\n",0x0810048b));
    CSW_PROFILE_FUNCTION_ENTER(CC_ProcNwReleaseMsg);

    //******************************************
    //mmi_audioCallStop==>stop speech,MIC,Speaker
    //******************************************
    CFW_GetUTI(hAo, &nUTI);

    proc->nCallState = CFW_CM_STATUS_RELEASED;
    //send RELEASE COMPLETE to network
    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, nUTI | (nSimId << 24), 1); //Notify MMI the call isn't in progress
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwReleaseMsg()send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n",0x0810048c));
    if(proc->bRelHeldCallFlag)
    {
        proc->bRelHeldCallFlag = FALSE;
        if(nCssu)
        {
            CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 5, nUTI | (nSimId << 24), 0x11); //CSSU=5:a held call is released
        }
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_CC_CRSSINFO_IND to MMI:a held call released\n",0x0810048d));
    }
    CFW_GetUTI(hAo, &nUTI);

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->bMODisc %d\n",0x0810048e), proc->bMODisc);
    if(!proc->bMODisc)
    {
        CFW_PostNotifyEvent ( EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0); // Added by Qiyan
    }

    else
    {

        proc->bMODisc = FALSE;
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("proc->nCalledFuncFlag %d\n",0x0810048f), proc->nCalledFuncFlag);

        switch(proc->nCalledFuncFlag)
        {
            case 1:

                CFW_PostNotifyEvent ( EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0x10);
                break;
            case 2:
                CFW_PostNotifyEvent ( EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0x10);

                break;
            case 4:
                //if(iCauseValue)
                //    CFW_PostNotifyEvent ( EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, iCauseValue, nUTI |(nSimId<<24), 0x10);
                //else
                CFW_PostNotifyEvent ( EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0x01);

                break;
            case 8:
                CFW_PostNotifyEvent ( EV_CFW_CC_RELEASE_CALL_RSP, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0x10);

                break;
            default:
                CFW_PostNotifyEvent ( EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0);

                break;

        }
        proc->nCalledFuncFlag = 0;
        //CFW_PostNotifyEvent ( EV_CFW_CC_RELEASE_CALL_RSP, iCauseValue, proc->nIndex, nUTI, 0);
    }
    CC_ReleaseCmplt( hAo, CFW_CM_NORMAL_CLEARING);
    //CFW_SetAoCurrentState(hAo, CFW_SM_STATE_IDLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwReleaseMsg() end\n",0x08100490));
    CSW_PROFILE_FUNCTION_EXIT(CC_ProcNwReleaseMsg);

}



//==============================================================================
//
//
//
//==============================================================================
VOID CC_ProcNwReleaseCmplMsg(HAO hAo, CM_SM_INFO *proc, UINT8 iCauseValue)
{
    UINT32 nUTI;

    CSW_PROFILE_FUNCTION_ENTER(CC_ProcNwReleaseCmplMsg);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcNwReleaseCmplMsg begin\n",0x08100491));
    CFW_SIM_ID nSimId = CFW_SIM_COUNT;
    CFW_GetSimID(hAo, &nSimId);
    //api_CcReleaseInd_t *p = (api_CcReleaseInd_t*)nEvParam;
    CFW_GetUTI(hAo, &nUTI);

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ProcNwReleaseCmplMsg, current call state is :0x%x\n",0x08100492), proc->nCallState);
    proc->nCallState = CFW_CM_STATUS_RELEASED;
    //added by zhangzd for bug 1629
    if(proc->bMODisc)
    {
        iCauseValue = 16;
    }
    //Release the current index
    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, nUTI | (nSimId << 24), 1); //Notify MMI the call isn't in progress
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n",0x08100493));
    CFW_PostNotifyEvent ( EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("send EV_CFW_CC_RELEASE_CALL_IND to MMI iCause Value is %d\n",0x08100494), iCauseValue);

    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
    CSW_PROFILE_FUNCTION_EXIT(CC_ProcNwReleaseCmplMsg);

}



//==============================================================================
//
//
//
//==============================================================================
VOID CC_ProcHeldStateCHLD2X(HAO hSm)
{
    UINT8 nCount = 0;
    UINT32 nAllState = 0;

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcHeldStateCHLD2X()begin\n",0x08100495));

    CSW_PROFILE_FUNCTION_ENTER(CC_ProcHeldStateCHLD2X);
    CFW_SIM_ID nSimid = CFW_SIM_END;
    CFW_GetSimID(hSm, &nSimid);
    nCount = CFW_GetAoCount(CFW_CC_SRV_ID, nSimid);
    nAllState = CFW_CcGetCallStatus(nSimid);

    if(nCount == 1)
    {
        CC_RetrieveCall(hSm, nSimid);
    }
    else if(nCount == 2) //(Held+Active)
    {
        CC_HoldCall(nSimid);
        CC_RetrieveCall(hSm, nSimid);

    }
    else if( (nCount == 3) && (( nAllState & CC_STATE_ACTIVE ) && ( nAllState & CC_STATE_HOLD ) && ( nAllState & CC_STATE_WAITING )) )
    {
        CC_HoldCall(nSimid);
        CC_RetrieveCall(hSm, nSimid);
    }
    else if(nCount > 2)//(Held+Meeting)
    {
        CC_HoldMPTY(nSimid);
        CC_RetrieveCall(hSm, nSimid);

    }


    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcHeldStateCHLD2X()end\n",0x08100496));

    CSW_PROFILE_FUNCTION_EXIT(CC_ProcHeldStateCHLD2X);
}


//==============================================================================
//
//
//
//==============================================================================
VOID CC_ProcMeetingStateCHLD2X(
    HAO hMeetingSm,
    UINT8 nIndex
    , CFW_SIM_ID nSimId
)
{
    HAO hSm;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CC_ProcMeetingStateCHLD2X);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcMeetingStateCHLD2X()begin\n",0x08100497));

    proc = CFW_GetAoUserData(hMeetingSm);

    CFW_SetServiceId(CFW_CC_SRV_ID);
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_HELD, CFW_CC_SRV_ID, nSimId) ) == HNULL) && (i < 7))
    {
        i++;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ProcMeetingStateCHLD2X() Loop value i=%d\n",0x08100498), i);

    if((i < 7) && (hSm != HNULL)) // have held call(Held + meeting)==> build MPTY at first, then splitMPTY
    {
        //      proc->nChldIndex = nIndex;
        proc->bNeedSplitFlag = TRUE;
    } //if only one MPTY, only need to splitMPTY.needn't buildMPTY

    proc->nNextSubState = proc->nCurSubState;
    proc->nCurSubState = SUBSTATE_INMEETING_CHLD2X;

    CFW_SetAoProcCode(hMeetingSm, CFW_AO_PROC_CODE_MULTIPLE);

    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcMeetingStateCHLD2X()end\n",0x08100499));

    CSW_PROFILE_FUNCTION_EXIT(CC_ProcMeetingStateCHLD2X);
}




//==============================================================================
//
//
//
//==============================================================================
VOID CC_ProcMeetingHeldStateCHLD2X(
    HAO hMeetingHeldSm,
    UINT8 nIndex
    , CFW_SIM_ID nSimId
)
{
    HAO hSm;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CC_ProcMeetingHeldStateCHLD2X);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcMeetingHeldStateCHLD2X()begin\n",0x0810049a));

    proc = CFW_GetAoUserData(hMeetingHeldSm);
    while(((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_ACTIVE, CFW_CC_SRV_ID, nSimId) ) == HNULL) && (i < 7))
    {
        i++;

    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ProcMeetingHeldStateCHLD2X() Loop value i=%d\n",0x0810049b), i);

    if((i < 7) && (hSm != HNULL)) // have held call(MeetingHeld + ACTIVE)==> build MPTY at first, then splitMPTY
    {
        //        proc->nChldIndex = nIndex;
        proc->bNeedSplitFlag = TRUE;
    } //if only one MPTY, only need to splitMPTY.needn't buildMPTY
    else
    {
        //Only Held_Meeting, Retrive it firstly
        proc->bNeedRetrieve = 1;
    }
    proc->nNextSubState = proc->nCurSubState;
    proc->nCurSubState = SUBSTATE_MEETINGHLD_CHLD2X;
    CFW_SetAoProcCode(hMeetingHeldSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ProcMeetingHeldStateCHLD2X()end\n",0x0810049c));

    CSW_PROFILE_FUNCTION_EXIT(CC_ProcMeetingHeldStateCHLD2X);
}




//==============================================================================
//
//For INMEETING and MEETINGHELD state, ACTIVE-->INMEETING,HELD-->INMEETING
//
//==============================================================================
VOID CC_ModifyOtherAoState(
    UINT8 nCurState,
    UINT8 nLastStae,
    BOOL bOneTimes
    , CFW_SIM_ID nSimId
)
{
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT8 i = 0;

    CSW_PROFILE_FUNCTION_ENTER(CC_ModifyOtherAoState);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ModifyOtherAoState() begin, bOneTimes=%d\n",0x0810049d), bOneTimes);
    CFW_SetServiceId(CFW_CC_SRV_ID); // added by Qiyan
    if(bOneTimes)
    {
        while(((hSm = CFW_GetAoHandle(i, nCurState, CFW_CC_SRV_ID, nSimId) ) == HNULL) && (i < 7))
        {
            i++;
        }

        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ModifyOtherAoState() loop value i=%d\n",0x0810049e), i);
        if((hSm != HNULL) && (i < 7))
        {
            proc = CFW_GetAoUserData(hSm);
            if(proc != NULL)
            {
                proc->bIsMpty = TRUE;
                proc->nCallState = CFW_CM_STATUS_ACTIVE;
                CFW_SetAoState(hSm, nLastStae);
            }
        }

    }
    else
    {
        for(i = 0; i < 7; i++)
        {
            if((hSm = CFW_GetAoHandle(i, nCurState, CFW_CC_SRV_ID, nSimId) ) != HNULL)
            {
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ModifyOtherAoState() AO ID=%d\n",0x0810049f), i);
                proc = CFW_GetAoUserData(hSm);
                if(proc != NULL)
                {
                    proc->bIsMpty = TRUE;
                    proc->nCallState = CFW_CM_STATUS_ACTIVE;
                    CFW_SetAoState(hSm, nLastStae);
                }
            }
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ModifyOtherAoState() END\n",0x081004a0));
    CSW_PROFILE_FUNCTION_EXIT(CC_ModifyOtherAoState);
    return;
}



//=============================================================================================
//Modify all AO state except hAo from INMEETING ==> MEETINGHELD  or MEETINGHELD ==>MEETINGHELD
//
//
//=============================================================================================
VOID CC_ModifyToMeetingHldState(
    HAO hAo,
    UINT8 nCurState,
    UINT8 nLastStae
    , CFW_SIM_ID nSimId
)
{
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT8 i = 0;

    CSW_PROFILE_FUNCTION_ENTER(CC_ModifyToMeetingHldState);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ModifyToMeetingHldState() BEGIN\n",0x081004a1));
    CFW_SetServiceId(CFW_CC_SRV_ID);
    for(i = 0; i < 7; i++)
    {
        if(((hSm = CFW_GetAoHandle(i, nCurState, CFW_CC_SRV_ID, nSimId) ) != HNULL) && (hSm != hAo))
        {

            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_ModifyToMeetingHldState() AO ID=%d\n",0x081004a2), i);
            proc = CFW_GetAoUserData(hSm);
            if(proc != NULL)
            {
                proc->bIsMpty = TRUE;
                proc->nCallState = CFW_CM_STATUS_HELD;
                CFW_SetAoState(hSm, nLastStae);
            }
        }
    }
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_ModifyToMeetingHldState() end\n",0x081004a3));
    CSW_PROFILE_FUNCTION_EXIT(CC_ModifyToMeetingHldState);
    return;
}



//============================================================================
// Function     : CC_SendCrssInfoNotify
// Description  : Send CRSS info to ATL
// Arguments    :
//
// Return value :
//
//============================================================================
VOID CC_SendCrssInfoNotify(
    api_CrssFacilityIndIE_t *pFacilityInd
    , CFW_SIM_ID  nSimId
)
{
    UINT8 nCSSI, nCSSU;

    CSW_PROFILE_FUNCTION_ENTER(CC_SendCrssInfoNotify);
    CFW_CfgGetSSN (&nCSSI, &nCSSU, nSimId);

    if((pFacilityInd->StructValid) && (pFacilityInd->ss_Operation == ss_Notify))
    {
        switch(pFacilityInd->ss_Code )
        {
            case ss_CLIR:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CLIR:\n",0x081004a4));
                if((pFacilityInd->Status == API_CRSS_CLIR_SUP_REJECTED) && (nCSSI))
                {
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 7, 0xff, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSI=7:CLIR suppression rejected
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify()case ss_CLIR: cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004a5));
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In MOPROC,Send EV_CFW_CC_CRSSINFO_IND to MMI:CFU active\n",0x081004a6));
                }
                break;
            case ss_CFU:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CFU:\n",0x081004a7));
                if((pFacilityInd->Status == API_CRSS_STATUS_ACTIVE_OPERATIVE) && (nCSSI))
                {
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0, 0xff, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSI=0:CFU is active

                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CFU:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004a8));
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In MOPROC,Send EV_CFW_CC_CRSSINFO_IND to MMI:CFU active\n",0x081004a9));
                }
                else if((pFacilityInd->Status == API_CRSS_MOCALL_HAS_BEEN_FORWARDED) && (nCSSI ))
                {
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 2, 0xff, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSI=2:Call been forwarded
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CFU:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004aa));
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In MOPROC,Send EV_CFW_CC_CRSSINFO_IND to MMI:CFU active\n",0x081004ab));
                }

                break;
            case ss_CFC:
            case ss_CFB:
            case ss_CFNRY:
            case ss_CFNRC:
            case ss_CFA:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CFA:\n",0x081004ac));
                {
                    switch(pFacilityInd->Status)
                    {
                        case API_CRSS_STATUS_ACTIVE_OPERATIVE:
                            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case API_CRSS_STATUS_ACTIVE_OPERATIVE\n",0x081004ad));
                            {
                                if(nCSSI)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case API_CRSS_STATUS_ACTIVE_OPERATIVE:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004ae));
                                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 1, 0xff, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSI=1:CFC is active
                                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_CC_CRSSINFO_IND to MMI:CFC active\n",0x081004af));
                                }
                                break;
                            }
                        case API_CRSS_MOCALL_HAS_BEEN_FORWARDED:
                            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case API_CRSS_MOCALL_HAS_BEEN_FORWARDED\n",0x081004b0));
                            //case API_CRSS_MTCALL_HAS_BEEN_FORWARDED:
                            {
                                if(nCSSI)
                                {
                                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case API_CRSS_MOCALL_HAS_BEEN_FORWARDED:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004b1));
                                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 2, 0xff, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSI=2:Call been forwarded
                                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("In MOPROC,Send EV_CFW_CC_CRSSINFO_IND to MMI:CFU active\n",0x081004b2));
                                }
                                break;
                            }
                        default:
                            break;
                    }

                    break;
                }

            case ss_CW:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CW\n",0x081004b3));
                if((pFacilityInd->Status == API_CRSS_CALL_IS_WAITING) && (nCSSI))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_CW:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004b4));
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 3, 0xff, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSI=3:call is waiting
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_CC_CRSSINFO_IND to MMI:Call waiting\n",0x081004b5));
                }
                break;
            case ss_HOLD:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_HOLD\n",0x081004b6));

                if((pFacilityInd->Status == API_CRSS_CALL_ON_HOLD) && (nCSSU))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_HOLD11:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004b7));
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 2, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSU=2:call is held
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_CC_CRSSINFO_IND to MMI:Call Held\n",0x081004b8));
                }
                else if ((pFacilityInd->Status == API_CRSS_CALL_RETRIEVED) && (nCSSU))
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_HOLD22:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004b9));
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 3, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSU=3:call is retrieve
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_CC_CRSSINFO_IND to MMI:Call Retrieved\n",0x081004ba));

                }
                break;
            case ss_MPTY:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_MPTY\n",0x081004bb));

                if ((pFacilityInd->Status == API_CRSS_MPTY_INDICATION) && nCSSU)
                {
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("CC_SendCrssInfoNotify() case ss_MPTY:cc==>>mmi EV_CFW_CC_CRSSINFO_IND \n",0x081004bc));
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 4, (GENERATE_URC_UTI() | (nSimId << 24)), 0x11); //CSSU=4:call in MPTY
                    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("Send EV_CFW_CC_CRSSINFO_IND to MMI:Call in MPTY\n",0x081004bd));
                }
                break;
            default:
                CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(1), TSTR("CC_SendCrssInfoNotify() ss_code default, ss_code=0x%x\n",0x081004be), pFacilityInd->ss_Code);
                break;

        }
    }
    CSW_PROFILE_FUNCTION_EXIT(CC_SendCrssInfoNotify);
    return;
}



//========================================================================
//If is DTMF charators, return TRUE, otherwise return FALSE
//
//========================================================================
BOOL IsDTMFChar(INT8 iTone)
{

    if(((iTone >= '0') && (iTone <= '9')) || ((iTone >= 'A') && (iTone <= 'D')) \
            || (iTone == '#') || (iTone == '*'))
        return TRUE;


    return FALSE;

}

#endif // ENALBE 
