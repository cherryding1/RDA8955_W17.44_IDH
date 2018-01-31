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

#include <cfw.h>
#include <sul.h>
#include "api_msg.h"
#include <ts.h>
#include <cfw_prv.h>
#include "csw_debug.h"
#include "csw_cfw_bal.h"
#if (CFW_CM_DUMP_ENABLE == 0) && (CFW_CM_SRV_ENABLE == 1)

#include "cfw_cc_data.h"
#include "cfw_cc_tools.h"

//this value is used to record whether the audio is on. called in CloseAudio, StartAudio and CFW_CcAoProc
//      0     :    audio off
//      1     :    audio on
//....................................................................
static UINT8 gTmpAudio = 0;

//this value only used in DUAL SIM project, when releasing a normal call, function will check SIM status,
//the operation is not allowed if the SIM status is abnormal.  but the EMC call needn't SIM, so when an
// EMC is ongoing, we should allow the release operation when the SIM status is abnormal
//     0    : not have EMC call
//     1    : have EMC call ongoing
//...............................................................................................................
// we may record this in the PROC data structure????
static UINT8 gEmcCallOn = 0;

//these two value are defined in shell module, used in factory test mode, auto dail test.
//      0    :  ???? unsure
//      1    :   ???  unsure
//..............................................................................................................
extern UINT32 g_TestMode_AutoResponse[];
extern UINT32 g_TestMode_AutoEMCDial[];

static UINT32 GetCallStatus(CFW_SIM_ID nSimId);

//-------------------------------------------------------
// useless value list as below, maybe we should delete them directely
//...................................................................................................
//#define SIM_MAX_NUM_LEN  20
//#define CC_AcceptWaitingCall( XP, YP, ZP )  CC_TerminateOne(XP, YP, ZP)

HAO CmMTAoProc(CFW_EV *pEvent);
UINT32 CFW_CMUnRegisterAO(UINT32 nServiceId, HANDLE hAo)
{
    CM_SM_INFO *proc = NULL;
    proc = CFW_GetAoUserData(hAo);
    if ((proc->pMsgBackup) && ((UINT32)(proc->pMsgBackup) - 8))
    {
        UINT8 **p = &(proc->pMsgBackup);
        Stack_MsgFree((UINT32)(proc->pMsgBackup) - 8);

        *p -= 8;
        *p = NULL;
    }
    return CFW_UnRegisterAO(nServiceId, hAo);
}
//Temporarily... For bug of stack.
void CloseAudio(void)
{

    //UINT8 nCall = 0;
    CSW_PROFILE_FUNCTION_ENTER(CloseAudio);

//Modify by lixp for bug 10294 at 20090107
#if 0
    nCall = CFW_GetAoCount(CFW_CC_SRV_ID, CFW_SIM_COUNT);
    CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(2), TSTXT(TSTR("nCall=%d, gTmpAudio=%d\n",0x081002c9)) , nCall, gTmpAudio);
    if(nCall <= 1)
    {
        if ( 1 == gTmpAudio) // add by wuys 2007-05-29
        {
            gTmpAudio = 0;

            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("speech	CloseAudio in CSW\n",0x081002ca)) );
            CFW_BalAudCallStop();
        }
    }
#else
    CFW_SIM_ID nSimID;
    UINT32 nAllState = 0x00;

    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        nAllState |= GetCallStatus(nSimID);
    }

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTXT(TSTR("CloseAudio in CSW , nAllState=0x%x, gTmpAudio=%d\n", 0x081002cb)), nAllState, gTmpAudio);
    if (!((nAllState & CC_STATE_ACTIVE) || (nAllState & CC_STATE_HOLD) || (nAllState & CC_STATE_INCOMING) || (nAllState & CC_STATE_DIALING) || (nAllState & CC_STATE_ALERTLING) || (nAllState & CC_STATE_RELEASE)))
    {
        if (1 == gTmpAudio)
        {
            gTmpAudio = 0;

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("speech	CloseAudio in CSW CC module\n", 0x081002cc)));

            CFW_BalAudCallStop();
        }
    }
#if 0 //def CHIP_HAS_AP

    else if(( nAllState & CC_STATE_DIALING ) || ( nAllState & CC_STATE_ALERTLING ) )
    {
        if ( 1 == gTmpAudio )
        {
            gTmpAudio = 0;

            CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTXT(TSTR("speech	CloseAudio in CSW CC module\n",0x081002cd)) );

            CFW_BalAudCallStop();

        }
    }
#endif

#endif
    CSW_PROFILE_FUNCTION_EXIT(CloseAudio);
    return;
}
/////////////////////////////////////////////
//add by wuys 2007-05-31
//
static void StartAudio(void)
{

    CSW_PROFILE_FUNCTION_ENTER(StartAudio);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTXT(TSTR("StartAudio, gTmpAudio=%d\n", 0x081002ce)), gTmpAudio);
    if (0 == gTmpAudio)
    {
        gTmpAudio = 1;

        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("speech    StartAudio in CSW\n", 0x081002cf)));
        CFW_BalAudCallStart();
    }

    CSW_PROFILE_FUNCTION_EXIT(StartAudio);
    return;
}
#ifndef GCF_TEST
#define AUTO_REDAIL
#endif
#define AUTO_REDAIL_MAX_NUM 2
UINT32 _AutoRedail(HAO hAo)
{
#ifndef AUTO_REDAIL
    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
#endif
    CFW_SIM_ID nSimId;
    CM_SM_INFO *proc = NULL;
    proc = CFW_GetAoUserData(hAo);
    CFW_GetSimID(hAo, &nSimId);
    if (proc->bAudioOnFlag == FALSE)
    {
        hal_HstSendEvent(0xCA110000 + proc->nReDialNum);
        if (AUTO_REDAIL_MAX_NUM > proc->nReDialNum)
        {
            proc->nReDialNum++;
            if (AUTO_REDAIL_MAX_NUM == proc->nReDialNum)
            {
                CFW_SendSclMessage(API_CC_SETUP_REQ, proc->pMsgBackup, nSimId);
                UINT8 **p = &(proc->pMsgBackup);
                *p -= 8;
                *p = NULL;

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()CC===>Stack send SETUP REQ API_CC_SETUP_REQ 0x%x\n", 0x081002eb), API_CC_SETUP_REQ);
            }
            else
            {
                api_CcSetupReq_t *sOutMsgBackup = 0;
                if (!(proc->pMsgBackup))
                {
                    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
                }
                sOutMsgBackup = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));
                memcpy(sOutMsgBackup, proc->pMsgBackup, SIZEOF(api_CcSetupReq_t));

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()CC===>Stack send SETUP REQ API_CC_SETUP_REQ 0x%x\n", 0x081002eb), API_CC_SETUP_REQ);
                CFW_SendSclMessage(API_CC_SETUP_REQ, proc->pMsgBackup, nSimId);
                proc->pMsgBackup = sOutMsgBackup;
            }

            CFW_SetAoState(hAo, CFW_SM_CM_STATE_MOPROC);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
            return ERR_SUCCESS;
        }
        else
        {
            if (proc->pMsgBackup)
            {
                //Stack_MsgFree(proc->pMsgBackup);
                //proc->pMsgBackup = NULL;
            }
        }
    }
    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
}
UINT32 _Check4Redail(HAO hAo, UINT8 nCause)
{
    /*
     3.     No route to destination
     6.     Channel unacceptable
     34.    No circuit/channel available
     38.    Network out of order
     41.    Temporary failure
     102.   Recovery on timer expiry
     127.   Interworking, unspecified
     * */
    if ((3 == nCause) || (6 == nCause) || (34 == nCause) || (38 == nCause) || (41 == nCause) || (102 == nCause) || (127 == nCause))
    {
        if (ERR_SUCCESS == _AutoRedail(hAo))
            return ERR_SUCCESS;
    }
    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
}
//
// Sim Service Init
//
UINT32 CFW_CcInit(VOID)
{
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcInit);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTXT(TSTR("CFW_CcInit \n", 0x081002d0)));
    CFW_SIM_ID nSimID;
    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        CFW_CfgSetClip(TRUE, nSimID);
        CFW_CfgSetSSN(TRUE, TRUE, nSimID);
    }
    CFW_RegisterCreateAoProc(API_CC_SETUP_IND, CmMTAoProc);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcInit);
    return ERR_SUCCESS;
}
//===================================================================================
// function add by dingmx 20080702
//  get the cause value from cause IE according TS24008 10 .5.4.11
static UINT8 S_CFW_GET_CAUSE_VALUE(UINT8 length, UINT8 *pCause)
{
    UINT8 iCauseValue = 0;
    if (length > 1)
    {
        if (pCause[0] & 0x80)
        {
            iCauseValue = pCause[1] & 0x7F;
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(4), TSTR("1,cause length %d ,Value %d,  %x %x\n", 0x081002d1), length, iCauseValue, pCause[0], pCause[1]);
        }
        else
        {
            if (length > 2)
            {
                iCauseValue = pCause[2] & 0x7F;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(5), TSTR("2,cause length %d ,Value %d , %x %x %x \n", 0x081002d2), length, iCauseValue, pCause[0], pCause[1], pCause[2]);
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(4), TSTR("3,get cause error length %d ,Value %d,%x %x \n", 0x081002d3), length, iCauseValue, pCause[0], pCause[1]);
            }
        }
    }
    else
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("4,cause length %d ", 0x081002d4), length);
    }
    return iCauseValue;
}

//===================================================================================
// CM AO process entry.
// There are some action will done in this function:
// 1> ??
// 2> ??
//
PRIVATE UINT32 CFW_CcAoProc(HAO hAo, CFW_EV *pEvent)
{
    UINT32 nEvtId;
    VOID *nEvParam = NULL;
    CM_SM_INFO *proc = NULL;
    UINT32 nUTI;
    UINT32 nState = 0;
    UINT32 ret = ERR_SUCCESS;
    CFW_EV ev;
    //hameina[+] 2009.1.7 move all cfg function here
    UINT8 nCSSI = 0, nCSSU = 0;
    //CFW_IND_EVENT_INFO pIndInfo;
    UINT8 nCallWaiting = 0;
    UINT8 nColp = 0;
    UINT8 bClip = 0;
    CFW_IND_EVENT_INFO sIndInfo;
    CFW_SIM_ID nSimId = CFW_SIM_END;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcAoProc);

    //Move for pEvent == NULL issue.
    if (pEvent == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcAoProc(), pEvent is NULL!\n", 0x081002d5));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if ((UINT32)pEvent == 0xffffffff) //from MMI
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()SetProc & CC send Msg to API!\n", 0x081002d6));

        SUL_MemSet8(&ev, 0xff, SIZEOF(CFW_EV));
        ev.nParam1 = 0xff;
        pEvent = &ev;
    }
    else
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc(), RECEIVED MESSAGE ***EvtID %x\n", 0x081002d7), pEvent->nEventId);

    nEvtId = pEvent->nEventId;
    nEvParam = (VOID *)pEvent->nParam1;

    proc = CFW_GetAoUserData(hAo);
    if (!proc)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcAoProc(), get proc DATA fail!\n", 0x081002d8));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
        return ERR_CME_MEMORY_FAILURE;
    }

    CFW_GetSimID(hAo, &nSimId);
    CFW_CfgGetIndicatorEventInfo(&sIndInfo, nSimId);
    CFW_CfgGetColp(&nColp, nSimId);
    ret = CFW_CfgGetSSN(&nCSSI, &nCSSU, nSimId);
    ret = CFW_CfgGetCallWaiting(&nCallWaiting, nSimId);
    CFW_CfgGetClip(&bClip, nSimId);

    nState = CFW_GetAoState(hAo);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcAoProc(), AO STATE nState=0X%X, proc->nReleaseFlag=%d\n", 0x081002d9), nState, proc->nReleaseFlag);

    CFW_GetUTI(hAo, &nUTI);

    if (proc->nReleaseFlag) //continue process release all call funtion
    {
        //CFW_CfgGetIndicatorEventInfo(&pIndInfo);
        sIndInfo.call = 0;
        CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
        proc->nReleaseFlag = FALSE;
        //modified by zhangzd for bug 1629
        proc->bMODisc = TRUE;

        if (nState == CFW_SM_CM_STATE_MTDISC)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_SM_CM_STATE_MTDISC \n", 0x081002da));
            CC_Release(proc, CFW_CM_NORMAL_CLEARING, nSimId);
        }
        else if (nState == CFW_SM_CM_STATE_ACTIVE)
        {
            if (proc->nHoldFunction == 0x11)
            {
                proc->nCalledFuncFlag = 4;
                proc->nHoldFunction = 0;
            }
            else
            {
                proc->nCalledFuncFlag = 8;
            }
            CC_Disc(proc, CFW_CM_NORMAL_CLEARING, nSimId);

            CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
        }
        else if (nState == CFW_SM_CM_STATE_INMEETING)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), "CFW_SM_CM_STATE_INMEETING nHoldFunction:0x%x\n", proc->nHoldFunction);
            if (proc->nHoldFunction == 0x11)
            {
                proc->nCalledFuncFlag = 4;
                proc->nHoldFunction = 0;
            }
            else
            {
                proc->nCalledFuncFlag = 8; //CFW_CcReleaseCall is called
            }
            CC_Disc(proc, CFW_CM_NORMAL_CLEARING, nSimId);
            CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
        }
        else
        {
            if (proc->nHoldFunction == 0x11)
            {
                proc->nCalledFuncFlag = 4;
                proc->nHoldFunction = 0;
            }
            else
                proc->nCalledFuncFlag = 8; //CFW_CcReleaseCall is called
            CC_Disc(proc, CFW_CM_USER_BUSY, nSimId);

            CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
            //CFW_BalAudCallStop();
        }
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
        return ret;
    }
    //handle DICONNECT,RELEASE,RELEASE COMPLETE AudioOn message at first ,
    //these message can be receive in all most state.
    if (nEvtId == API_CC_DISC_IND)
    {
        //CFW_BalAudCallStop();
        // Close the audio device;
        //CloseAudio ();
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() Received Event: API_CC_DISC_IND 0x%x\n", 0x081002db), nEvtId);
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcAoProc() Received Event: nState=0x%x,CFW_SM_CM_STATE_MODISC=0x%x\n", 0x081002dc), nState, CFW_SM_CM_STATE_MODISC);
        if ((nState != CFW_SM_STATE_IDLE) && (nState != CFW_SM_CM_STATE_MODISC) && (nState != CFW_SM_CM_STATE_MTDISC))
        {
            //CFW_IND_EVENT_INFO pIndInfo;
            UINT8 iCauseValue = 0;
            api_CcDiscInd_t *p = (api_CcDiscInd_t *)nEvParam;
            //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

            sIndInfo.call = 0;

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() not idle_modisc_mtdisc! \n", 0x081002dd));

            // [[changed by dingmx 20080702, according TS24008 10.5.4.11
            iCauseValue = S_CFW_GET_CAUSE_VALUE(p->CauseLen, p->Cause);
            // ]]changed by dingmx end ,

            proc->nCauseVal = iCauseValue;

            //[[hameina[+] 2007-10-26 for cssu=5
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("for cssu=5,proc->nCallState=%d\n", 0x081002de), proc->nCallState);
            CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
            CC_SendCrssInfoNotify(&(p->Facility), nSimId);
            if (proc->nCallState == CFW_CM_STATUS_HELD)
            {
                CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 5, nUTI | (nSimId << 24), 0x11); //cssu=5:call on hold has been released
            }
            //]]hameina[+] 2007-10-26
            CC_ProcNwDisconnectMsg(hAo, proc, nEvParam);
        }
        else if (nState == CFW_SM_CM_STATE_MODISC)
        {
            //------------------------------------------------------------------------------------
            //this branch added by Hameina, 2009.3.9
            //-------------------------------------------------------------------------------------
            //reason: when user hand off the call and the net work also disconnect the call all most at the same time,
            //call state is CFW_SM_CM_STATE_MODISC, and we should deal the conflict of this MO MT disconnect
            //--------------------------------------------------------------------------------------
            //result:  1. call status should be changed to CFW_SM_CM_STATE_MTDISC.
            //       2. send API_CC_RELEASE_REQ
            //--------------------------------------------------------------------------------------
            proc->nCallState = CFW_CM_STATUS_RELEASED;
            CFW_SetAoState(hAo, CFW_SM_CM_STATE_MTDISC);

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("recieve API_CC_DISC_IND, send release msg to nw directly, and changed to CFW_SM_CM_STATE_MTDISC \n", 0x081002df));
            CC_Release(proc, CFW_CM_NORMAL_CLEARING, nSimId);
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
            return ret;
        }
    }
    else if (nEvtId == API_CC_RELEASE_IND)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()! Received Event: API_CC_RELEASE_IND 0x%x\n", 0x081002e0), nEvtId);
        //Modify by lixp for bug 10294 at 20090107
        //CloseAudio();

        if (nState != CFW_SM_STATE_IDLE)
        {
            //CFW_IND_EVENT_INFO pIndInfo;
            UINT8 iCauseValue = 0;
            UINT32 nAllState = 0;

            api_CcReleaseInd_t *p = (api_CcReleaseInd_t *)nEvParam;
            nAllState = GetCallStatus(nSimId);
            //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

            sIndInfo.call = 0;

            //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
            iCauseValue = S_CFW_GET_CAUSE_VALUE(p->Cause1Len, p->Cause1);
            //[[ changed by dingmx end ,

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Receive RELEASE facility info 11, p->Facility.StructValid=%d\n", 0x081002e1), p->Facility.StructValid);
            //[[hameina[+] 2007-10-26 for cssu=10
            if (p->Facility.StructValid)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Receive RELEASE facility info  22, p->Facility.Status=0x%x\n", 0x081002e2), p->Facility.Status);
                CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                if (p->Facility.Status == API_CRSS_MOCALL_HAS_BEEN_FORWARDED)
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 2, 0xff, nUTI | (nSimId << 24), 0x11); //CSSi=2:call has been forwarded
                else if (p->Facility.Status == API_CRSS_MTCALL_HAS_BEEN_FORWARDED)
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 10, nUTI | (nSimId << 24), 0x11); //CSSu=10:call has been forwarded
            }
            //]]hameina[+]2007-10-16
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("proc->nHoldFunction=%d\n", 0x081002e3), proc->nHoldFunction);
            //CRSS: If accept the waiting call...
            if (proc->nHoldFunction == 1)
            {
                proc->nHoldFunction = 0;
                CC_ProcNwReleaseMsg(hAo, proc, iCauseValue);
                //Modify by lixp for bug 10294 at 20090107
                CloseAudio();
                nAllState = GetCallStatus(nSimId);
                if (!(nAllState & CC_STATE_ACTIVE) && !(nAllState & CC_STATE_RELEASE) && (nAllState & CC_STATE_WAITING))
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CC_TerminateOne\n", 0x081002e4));
                    CC_TerminateOne(CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD1, 1, nSimId);
                }
                //fix the bug 3889,3891
                else if (!(nAllState & CC_STATE_ACTIVE) && !(nAllState & CC_STATE_RELEASE) && (nAllState & CC_STATE_HOLD))
                {
                    //it is important for bug[Bug 3852] multi-party2007.01.31
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CC_Retrieve trace\n", 0x081002e5));
                    CC_RetrieveCall(HNULL, nSimId);
                    CC_RetrieveMPTY(nSimId);
                }
            }
            else
            {
                CC_ProcNwReleaseMsg(hAo, proc, iCauseValue);
                //Modify by lixp for bug 10294 at 20090107
                CloseAudio();
            }
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            proc->bMODisc = FALSE;
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
            return ret;
        }
    }
    else if (nEvtId == API_CC_RELEASECMPLT_IND)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() Received Event: API_CC_RELEASECMPLT_IND 0x%x\n", 0x081002e6), nState);
        if (nState != CFW_SM_STATE_IDLE)
        {
            UINT8 iCauseValue = 0;
            api_CcReleaseCmpltInd_t *p = (api_CcReleaseCmpltInd_t *)nEvParam;

            sIndInfo.call = 0;
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()Receive RELEASE COMPLETE message\n", 0x081002e7));

            //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
            if (16 == proc->nCalledFuncFlag)
            {
                iCauseValue = proc->nCauseVal;
                if (ERR_SUCCESS == _Check4Redail(hAo, iCauseValue))
                    return ERR_SUCCESS;
            }
            else
            {
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->CauseLen, p->Cause);
            }
            //]] change end

            CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
            CC_SendCrssInfoNotify(&(p->Facility), nSimId);
            CC_ProcNwReleaseCmplMsg(hAo, proc, iCauseValue);
            //Modify by lixp for bug 10294 at 20090107
            CloseAudio();
        }
        else
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
            return ret;
        }
    }
    else if (nEvtId == API_CC_AUDIOON_IND)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()! Received Event: API_CC_AUDIOON_IND 0x%x\n", 0x081002e8), nEvtId);
        proc->bAudioOnFlag = TRUE;
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("proc->nDir=%d, gTmpAudio=%d\n", 0x081002e9), proc->nDir, gTmpAudio);
        if ((proc->nDir == 0) && (0 == gTmpAudio))
        {
            StartAudio();
        }
    }
    else //handle all event except disconnect,release, releasecomplete
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcAoProc()status machine, status= 0x%x, nEvtId=0x%x\n", 0x081002ea), nState, nEvtId);

        switch (nState)
        {
        case CFW_SM_STATE_IDLE:
        {
            if (pEvent->nParam1 == 0xff)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()CC===>Stack send SETUP REQ API_CC_SETUP_REQ 0x%x\n", 0x081002eb), API_CC_SETUP_REQ);
                CFW_SendSclMessage(API_CC_SETUP_REQ, proc->pMsg, nSimId);
                CFW_SetAoState(hAo, CFW_SM_CM_STATE_MOPROC);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            if (nEvtId == API_CC_SETUP_IND)
            {
                api_CcSetupInd_t *p = (api_CcSetupInd_t *)nEvParam;

                //Add by Lixp For GCF test 20130918
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("Bearer1Len %d (p->Bearer1[0] & 0x7) 0x%x\n", 0x081002ec), p->Bearer1Len, (p->Bearer1[0] & 0x7));
                if (p->Bearer1Len)
                {
                    typedef enum {
                        OCTET_NONE,
                        OCTET_3,
                        OCTET_4,
                        OCTET_5,
                        OCTET_5a,
                        OCTET_6,
                        OCTET_6a,
                        OCTET_6b,
                        OCTET_6c,
                        OCTET_6d,
                        OCTET_6e,
                        OCTET_6f,
                        OCTET_6g,
                        OCTET_7,
                    } BEAR_OCTET;

                    u8 i, octet = OCTET_NONE;
                    BOOL BS_surrport = TRUE;

                    for (i = 0; i < p->Bearer1Len; i++)
                    {
                        if (p->Bearer1[i] & 0x80)
                        {
                            octet++;
                        }

                        switch (octet)
                        {
                        case OCTET_3:
                        {
                            if (0x00 != (p->Bearer1[i] & 0x07))
                            {
                                BS_surrport = FALSE;
                            }
                        }
                        break;
                        case OCTET_5:
                        {
                            if (0x00 != (p->Bearer1[i] & 0x18))
                            {
                                BS_surrport = FALSE;
                            }
                        }
                        break;
                        default:
                            break;
                        }
                    }

                    for (i = 0; i < p->Bearer2Len; i++)
                    {
                        if (p->Bearer2[i] & 0x80)
                        {
                            octet++;
                        }

                        switch (octet)
                        {
                        case OCTET_3:
                        {
                            if (0x00 != (p->Bearer2[i] & 0x07))
                            {
                                BS_surrport = FALSE;
                            }
                        }
                        break;
                        case OCTET_5:
                        {
                            if (0x00 != (p->Bearer2[i] & 0x18))
                            {
                                BS_surrport = FALSE;
                            }
                        }
                        break;
                        default:
                            break;
                        }
                    }
#ifdef GCF_TEST
                    BS_surrport = TRUE;
#endif
                    if (BS_surrport == FALSE)
                    {
                        CFW_SetUTI(hAo, (UINT16)p->TI, 2); //set TI*******
                        proc->nTI = p->TI;
                        CC_ReleaseCmplt(hAo, CFW_CM_INCOMPATIBLE_DESTINATION);
                        CloseAudio();
                        return ERR_SUCCESS;
                    }
                }

                //MMIMALLOC
                CFW_SPEECH_CALL_IND *pSpeechCallInd = (CFW_SPEECH_CALL_IND *)CSW_CC_MALLOC(SIZEOF(CFW_SPEECH_CALL_IND));

                if (pSpeechCallInd == NULL)
                {
                    CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                    return ERR_NO_MORE_MEMORY;
                }

                SUL_ZeroMemory32(pSpeechCallInd, SIZEOF(CFW_SPEECH_CALL_IND));

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Incoming call\n", 0x081002ed));

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("Get nCSSI=%d,nCSSU=%d\n", 0x081002ee), nCSSI, nCSSU);

                CFW_SetUTI(hAo, (UINT16)p->TI, 2); //set TI*******

                proc->nTI = p->TI;
                proc->nCallMode = CFW_CM_MODE_VOICE;
                proc->nDir = CFW_DIRECTION_MOBILE_TERMINATED;
                proc->bIsMpty = FALSE;
                pSpeechCallInd->nCLIValidity = 0xFF;

                /*If cfw get a invalid number, it will be cut...*/
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CC ==> CPN length: %d, p->Facility.Status=%d\n", 0x081002ef), p->CPNLen, p->Facility.Status);

                if (p->CPNLen > TEL_NUMBER_MAX_LEN)
                    p->CPNLen = TEL_NUMBER_MAX_LEN;

                if ((p->Facility.Status == API_CRSS_MTCALL_IS_FORWARDED_CALL) && (TRUE == p->Facility.StructValid))
                {
                    //[[hameina [mod] 2007.08.01 :forwarded call has no number, original one is RPN, but now we add a check
                    if (p->CPNLen)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("cfw cc recieve a forwarded call 2, CPNLen=%d\n", 0x081002f0), p->CPNLen);
                        proc->nAddressType = p->CallingPartyNb[0] | 0x80; //type of address:0x81 or 0x91 or 0xA1

                        pSpeechCallInd->TelNumber.nSize = CallingPartyrRaw2BCD(&(p->CallingPartyNb[0]), proc->pNumber, p->CPNLen);
                    }
                    else if (p->RPNLen)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("cfw cc recieve a forwarded call 1, RPLen=%d\n", 0x081002f1), p->RPNLen);
                        proc->nAddressType = p->RedirectingPartyNb[0] | 0x80; //type of address:0x81 or 0x91 or 0xA1
                        pSpeechCallInd->TelNumber.nSize = CallingPartyrRaw2BCD(&(p->RedirectingPartyNb[0]), proc->pNumber, p->RPNLen);
                    }
                    else
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("cfw cc recieve a forwarded call 3, error!!!!both RPN & CPN is null\n", 0x081002f2));
                    }

                    //]]hameina [mod] 2007.08.01
                }
                else
                {
                    proc->nAddressType = p->CallingPartyNb[0] | 0x80; //type of address:0x81 or 0x91 or 0xA1

                    pSpeechCallInd->TelNumber.nSize = CallingPartyrRaw2BCD(&(p->CallingPartyNb[0]), proc->pNumber, p->CPNLen);
                }
                proc->nNumberSize = pSpeechCallInd->TelNumber.nSize;

                /*Format the invalid number...*/
                if (proc->nNumberSize > TEL_NUMBER_MAX_LEN)
                {
                    proc->nNumberSize = TEL_NUMBER_MAX_LEN;
                    pSpeechCallInd->TelNumber.nSize = TEL_NUMBER_MAX_LEN;
                }
                if (CFW_GetAoCount(CFW_CC_SRV_ID, nSimId) > 1)
                {
                    proc->nCallState = CFW_CM_STATUS_WAITING;

                    //hameina[+] 2009.7.31 AOcount >0, this call should be a waiting call.
                    pSpeechCallInd->nCode = 3; //call is waiting

                    if ((nCallWaiting == 1) && (nCSSU == 1))
                    {
                        //[[hameina[mod] 2007.9.10, out put info doesn't correct
                        if (p->Facility.Status == API_CRSS_MTCALL_IS_FORWARDED_CALL)
                        {
                            pSpeechCallInd->nDisplayType = 6; //+CSSU +CCWA:
                            pSpeechCallInd->nCode = 0;        // call is a forwarded call :CSSU code
                            if (nCSSI)
                            {
                                CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 3, 0xff, nUTI | (nSimId << 24), 0x11); //cssi=3
                            }
                        }
                        else
                        {
                            pSpeechCallInd->nDisplayType = 5; //+CSSI +CCWA:
                        }
                        //]]hameina[mod] 2007.9.10, out put info doesn't correct, bug 13539
                        pSpeechCallInd->TelNumber.nType = proc->nAddressType;

                        SUL_MemCopy8(pSpeechCallInd->TelNumber.nTelNumber, proc->pNumber, proc->nNumberSize);
                    }
                    else if ((nCallWaiting == 1) && (nCSSU == 0))
                    {
                        pSpeechCallInd->nDisplayType = 4; //+CCWA:
                        pSpeechCallInd->TelNumber.nType = proc->nAddressType;
                        SUL_MemCopy8(pSpeechCallInd->TelNumber.nTelNumber, proc->pNumber, proc->nNumberSize);
                    }
                    else if (nCallWaiting == 0 && (nCSSU == 0))
                    {
                        pSpeechCallInd->nDisplayType = 0; //no display
                    }
                    else if (nCallWaiting == 0 && (nCSSU == 1))
                    {
                        pSpeechCallInd->nDisplayType = 3; //+CSSU :
                        if (p->Facility.Status == API_CRSS_MTCALL_IS_FORWARDED_CALL)
                        {
                            pSpeechCallInd->nCode = 0; // call is a forwarded call :CSSU code
                        }
                        else
                            pSpeechCallInd->nCode = 0xff;
                    }
                    CC_CallConfirm(p->TI, CFW_CM_USER_BUSY, nSimId);
                }
                else
                {
                    proc->nCallState = CFW_CM_STATUS_INCOMING;
                    if (bClip)
                    {
                        if ((nCSSI == 1) && (p->Facility.Status == API_CRSS_MTCALL_IS_FORWARDED_CALL))
                        {
                            pSpeechCallInd->nDisplayType = 8; //+CLIP + CSSU:
                            pSpeechCallInd->nCode = 0;        // MT call is forwarded call :CSSU code
                        }
                        else
                        {
                            pSpeechCallInd->nDisplayType = 1; //+CLIP:
                            pSpeechCallInd->nCode = 0xff;
                        }
                        switch (p->CauseOfNoCLI)
                        {
                        case API_CAUSEOFNOCLI_REJECTBYUSER:
                        case API_CAUSEOFNOCLI_INTERACTSERV:
                        case API_CAUSEOFNOCLI_PAYPHONE:
                            pSpeechCallInd->nCLIValidity = 0; //CLIP not provisioned
                            break;
                        case API_CAUSEOFNOCLI_INVALID:
                            pSpeechCallInd->nCLIValidity = 1; //CLIP provisioned
                            break;
                        case API_CAUSEOFNOCLI_UNAVAILABLE:
                            pSpeechCallInd->nCLIValidity = 3; //unknown (e.g. no network, etc.)
                            break;

                        default:
                            pSpeechCallInd->nCLIValidity = 0xFF;
                            break;
                        }
                        pSpeechCallInd->TelNumber.nType = proc->nAddressType;
                        SUL_MemCopy8(pSpeechCallInd->TelNumber.nTelNumber, proc->pNumber, proc->nNumberSize);
                    }
                    else
                    {
                        if ((nCSSU == 1) && (p->Facility.Status == API_CRSS_MTCALL_IS_FORWARDED_CALL))
                        {
                            pSpeechCallInd->nDisplayType = 3; // + CSSU:
                            pSpeechCallInd->nCode = 0;        // MT call is forwarded call :CSSU code
                        }
                        else
                        {
                            pSpeechCallInd->nDisplayType = 0; //no display
                            pSpeechCallInd->nCode = 0xff;
                        }
                    }
                    CC_CallConfirm(p->TI, 0xFF, nSimId);
                }
                CC_Alert(p->TI, nSimId);

                CFW_SetAoState(hAo, CFW_SM_CM_STATE_MTPROC);
                //CFW_GetUTI(hAo, &nUTI);
                if (!g_TestMode_AutoResponse[nSimId])
                {
                    CFW_PostNotifyEvent(EV_CFW_CC_SPEECH_CALL_IND, (UINT32)pSpeechCallInd, SIZEOF(CFW_SPEECH_CALL_IND), nUTI | (nSimId << 24), 0);
                }
                else
                {
                    CFW_PostNotifyEvent(EV_CFW_CC_SPEECH_CALL_IND, (UINT32)pSpeechCallInd, SIZEOF(CFW_SPEECH_CALL_IND), nUTI | (nSimId << 24), 0);
                    CFW_PostNotifyEvent(EV_CFW_CC_SPEECH_CALL_IND, (UINT32)pSpeechCallInd, SIZEOF(CFW_SPEECH_CALL_IND), GENERATE_SHELL_UTI() | (nSimId << 24), 0);
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("send EV_CFW_CC_SPEECH_CALL_IND to MMI\n", 0x081002f3));
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[WARNING]CFW_CcAoProc() Recieve event in IDLE state, not consumed\n", 0x081002f4));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;
        }
        case CFW_SM_CM_STATE_MOPROC:
        {
            if (pEvent->nParam1 == 0xff)
            {
                if (proc->nCurSubState == SUBSTATE_MO_PROC_DTMF)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MOPROC:SUBSTATE_MO_PROC_DTMF\n", 0x081002f5));
                    proc->nCurSubState = proc->nNextSubState;
                    if (proc->pMsg != NULL)
                    {
                        CC_StartDTMF(proc, *((UINT8 *)(proc->pMsg)), nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state,send START DTMF\n", 0x081002f6));
                        //CC_StopDTMF(proc, nSimId);
                        CSW_CC_FREE(proc->pMsg);
                        proc->pMsg = NULL;
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in MOPROC state,send STOP DTMF\n", 0x081002f7));
                    }
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }

            switch (nEvtId)
            {
            case API_CC_TI_IND:
            {
                api_CcTIInd_t *p = (api_CcTIInd_t *)nEvParam;
                proc->nTI = p->TI;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_TI_IND 0x%x\n", 0x081002f8), API_CC_TI_IND);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() IN MOPROC,receive TI is: %d\n", 0x081002f9), p->TI);
                CFW_SetUTI(hAo, (UINT16)p->TI, 2); //set TI*******
                break;
            }
            case API_CC_CALLPROC_IND:
            {
                UINT8 iProgressIndValue = 0;
                api_CcCallProcInd_t *p = (api_CcCallProcInd_t *)nEvParam;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_CALLPROC_IND 0x%x\n", 0x081002fa), API_CC_CALLPROC_IND);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() IN MOPROC,Receive CALL PROCEEDING,TI:%d\n", 0x081002fb), p->TI);

                iProgressIndValue = p->ProgressInd[1] & 0x7F;
                if (iProgressIndValue == 8)
                {
                    proc->bProgressIndFlag = TRUE;
                }

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() CC==>>MMI EV_CFW_CC_CALL_INFO_IND 0x%x\n", 0x081002fc), EV_CFW_CC_CALL_INFO_IND);
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 1, proc->nIndex, nUTI | (nSimId << 24), 1); //Notify MMI the call is in progress
                CC_SendCrssInfoNotify(&(p->Facility), nSimId);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("case API_CC_CALLPROC_IND call CC_SendCrssInfoNotify\n", 0x081002fd));

                break;
            }
            case API_CC_PROGRESS_IND:
            {
                UINT8 iProgressIndValue = 0;
                api_CcProgressInd_t *p = (api_CcProgressInd_t *)nEvParam;

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() IN MOPROC,Receive CALL PROGRESS,TI:%d\n", 0x081002fe), p->TI);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_PROGRESS_IND 0x%x\n", 0x081002ff), API_CC_PROGRESS_IND);

                iProgressIndValue = p->ProgressInd[1] & 0x7F;
                if (iProgressIndValue == 8)
                {
                    proc->bProgressIndFlag = TRUE;
                }
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() CC==>>MMI API_CC_PROGRESS_IND 0x%x\n", 0x08100300), EV_CFW_CC_PROGRESS_IND);
                CFW_PostNotifyEvent(EV_CFW_CC_PROGRESS_IND, 1, proc->nIndex, nUTI | (nSimId << 24), 1); //Notify MMI the call is in progress

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("case API_CC_PROGRESS_IND call CC_SendCrssInfoNotify\n", 0x08100301));
                break;
            }
            case API_CC_ALERT_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_ALERT_IND 0x%x\n", 0x08100302), API_CC_ALERT_IND);

                UINT8 iProgressIndValue = 0;
                api_CcAlertInd_t *p = (api_CcAlertInd_t *)nEvParam;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("IN MOPROC,Receive ALERT,TI:%d\n", 0x08100303), p->TI);

                CC_SendCrssInfoNotify(&(p->Facility), nSimId);
                iProgressIndValue = p->ProgressInd[1] & 0x7F;
                if (iProgressIndValue == 8)
                {
                    proc->bProgressIndFlag = TRUE;
                }
                proc->nCallState = CFW_CM_STATUS_ALERTING;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("case API_CC_ALERT_IND:CFW_CcAoProc() cc==>>mmi EV_CFW_CC_CALL_INFO_IND 0x%x \n", 0x08100304), EV_CFW_CC_CALL_INFO_IND);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("IN MOPROC,p->ProgressInd[1]:0x%x\n", 0x08100305), p->ProgressInd[1]);

                StartAudio();
                CC_SendCrssInfoNotify(&(p->Facility), nSimId);
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 1, proc->nIndex, nUTI | (nSimId << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("send EV_CFW_CC_CALL_INFO_IND to MMI for Sounder opend\n", 0x08100306));

                break;
            }

            case API_CC_CONNECT_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_CONNECT_IND 0x%x\n", 0x08100307), API_CC_CONNECT_IND);
                //CFW_IND_EVENT_INFO pIndInfo;
                //UINT8 nColp;
                //CFW_CfgGetColp(&nColp);
                //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                api_CcConnectInd_t *p = (api_CcConnectInd_t *)nEvParam;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("the return nColp=%d\n", 0x08100308), nColp);

                sIndInfo.call = 1;
                proc->nCallState = CFW_CM_STATUS_ACTIVE;
                proc->nCalledFuncFlag = 0;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("IN MOPROC,Receive CONNECT,TI:%d\n", 0x08100309), p->TI);

                //////////////////////// Start the audio.
                /*
                        if(gTmpAudio == 0)
                        {
                        g_CSWOperateMicAndReceiver=TRUE;

                        }*/
                StartAudio();
                CFW_SetAoState(hAo, CFW_SM_CM_STATE_ACTIVE);
                //??????????
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()case API_CC_CONNECT_IND: cc==>>mmi EV_CFW_CC_CALL_INFO_IND 0x%x\n", 0x0810030a), EV_CFW_CC_CALL_INFO_IND);
                CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, nUTI | (nSimId << 24), 0); //Notify MMI the called party accept call
                CC_SendCrssInfoNotify(&(p->Facility), nSimId);

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("send EV_CFW_CC_CALL_INFO_IND to MMI for Sounder open\n", 0x0810030b));
                //CFW_GetUTI(hAo, &nUTI);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("the connected number length=%d\n", 0x0810030c), p->CNLen);
                if ((nColp == 1) && (p->CNLen > 0))
                {
                    //MMIMALLOC
                    CFW_TELNUMBER *pConnectedNum = (CFW_TELNUMBER *)CSW_CC_MALLOC(SIZEOF(CFW_TELNUMBER));

                    if (pConnectedNum == NULL)
                    {
                        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                        return ERR_NO_MORE_MEMORY;
                    }

                    SUL_ZeroMemory32(pConnectedNum, SIZEOF(CFW_TELNUMBER));
                    pConnectedNum->nType = p->ConnectedNb[0] | 0x80; //type of address:0x81 or 0x91 or 0xA1

                    if (p->CNLen > TEL_NUMBER_MAX_LEN)
                        p->CNLen = TEL_NUMBER_MAX_LEN;

                    SUL_MemCopy8(pConnectedNum->nTelNumber, &(p->ConnectedNb[0]), p->CNLen);
                    CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, (UINT32)pConnectedNum, 0, nUTI | (nSimId << 24), 1);
                }
                else
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()case API_CC_CONNECT_IND: cc==>>mmi EV_CFW_CC_INITIATE_SPEECH_CALL_RSP 0x%x\n", 0x0810030d), EV_CFW_CC_INITIATE_SPEECH_CALL_RSP);
                    CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, 0, 0, nUTI | (nSimId << 24), 0);
                }

                break;
            }
            case API_CC_FACILITY_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_FACILITY_IND 0x%x\n", 0x0810030e), API_CC_FACILITY_IND);
                api_CcFacilityInd_t *p = (api_CcFacilityInd_t *)nEvParam;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in MOPROC state:receive FACILITY,p->Facility[0].Status:%d\n", 0x0810030f), p->Facility[0].Status);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in MOPROC state:receive FACILITY,p->Facility[0].ss_Operation:%d\n", 0x08100310), p->Facility[0].ss_Operation);
                CC_SendCrssInfoNotify(&(p->Facility[0]), nSimId);
                CC_SendCrssInfoNotify(&(p->Facility[1]), nSimId);
                break;
            }
            case API_CC_ERROR_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() stack==>>CC API_CC_ERROR_IND 0x%x\n", 0x08100311), API_CC_ERROR_IND);

                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //Modify by lixp for bug 10294 at 20090107
                //CloseAudio();

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //[[ change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in MOPROC state,CM local Error %x\n", 0x08100312), p->Cause);

                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case: API_LOW_LAYER_FAIL \n", 0x08100313));
                    if (ERR_SUCCESS == _AutoRedail(hAo))
                        return ERR_SUCCESS;
                }
                case API_NORMAL_RELEASE:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case: API_NORMAL_RELEASE \n", 0x08100314));
                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;

                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_NO_CONNECTION_TO_PHONE);
                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, nUTI | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case API_NORMAL_RELEASE:send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x08100315));
                    if (!g_TestMode_AutoEMCDial[nSimId])
                    {
                        CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    }
                    else
                    {
                        CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                        CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause,
                                            GENERATE_SHELL_UTI() | (nSimId << 24), 0xf0);
                    }

                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);

                    break;
                }
                case API_CC_TIMER_EXPIRY: //change state to MODISC
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case: API_LOW_LAYER_FAIL \n", 0x08100316));
                    if (ERR_SUCCESS == _AutoRedail(hAo))
                        return ERR_SUCCESS;
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
                    break;
                default:
                    break;
                }
                //Modify by lixp for bug 10294 at 20090107
                CloseAudio();

                break;
            }
            default:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc() IN MO PROC state, recieve unknown event in this state, not consumed \n", 0x08100317));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            } //switch(nEvtId)
            break;
        }
        case CFW_SM_CM_STATE_MODISC:
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case: CFW_SM_CM_STATE_MODISC \n", 0x08100318));

            if (nEvtId == API_CC_ERROR_IND)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case: CFW_SM_CM_STATE_MODISC API_CC_ERROR_IND \n", 0x08100319));
                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                // [[change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in MODISC state,CM local Error %x\n", 0x0810031a), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case: CFW_SM_CM_STATE_MODISC API_LOW_LAYER_FAIL \n", 0x0810031b));
                case API_NORMAL_RELEASE:
                case API_CC_PROTOCOL_ERROR:
                {
                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;

                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_UNKNOWN);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() CC==>>MMI EV_CFW_CC_CALL_INFO_IND 0x%x\n", 0x0810031c), EV_CFW_CC_CALL_INFO_IND);
                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, nUTI | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()API_CC_PROTOCOL_ERRORsend EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x0810031d));

                    CFW_GetUTI(hAo, &nUTI);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc() CC==>>MMI case %d:\n", 0x0810031e), proc->nCalledFuncFlag);
                    switch (proc->nCalledFuncFlag)
                    {
                    case 1:
                        CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                        break;
                    case 2:
                        CFW_PostNotifyEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                        break;
                    case 4:
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                        break;
                    case 8:
                        CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                        break;
                    default:
                        CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0);
                        break;
                    }

                    //Release the current index

                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();

                    break;
                }

                default:
                    proc->bRelHeldCallFlag = FALSE;
                    break;
                }
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc() IN MO DISC state, recieve unknown event, not consumed \n", 0x0810031f));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;
        }
        case CFW_SM_CM_STATE_MTDISC:
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() CFW_SM_CM_STATE_MTDISC \n", 0x08100320));

            if (nEvtId == API_CC_ERROR_IND)
            {
                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //]] change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in MTDISC state,CM local Error %x\n", 0x08100321), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() CFW_SM_CM_STATE_MTDISC API_LOW_LAYER_FAIL\n", 0x08100322));
                case API_NORMAL_RELEASE:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() CFW_SM_CM_STATE_MTDISC API_NORMAL_RELEASE\n", 0x08100323));
                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;

                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_UNKNOWN);

                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case API_NORMAL_RELEASE:send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x08100324));
                    //CFW_GetUTI(hAo, &nUTI);
                    if (0x11 == proc->nHoldFunction)
                    {
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0x01);
                    }
                    else
                    {
                        CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, nUTI | (nSimId << 24), 0x0);
                    }
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() ===>> mmi CFW_SM_CM_STATE_MTDISC API_NORMAL_RELEASE\n", 0x08100325));

                    //Release the current index
                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();
                    break;
                }
                default:
                    CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, GENERATE_URC_UTI() | (nSimId << 24), 0x0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() ===>> mmi CFW_SM_CM_STATE_MTDISC EV_CFW_CC_RELEASE_CALL_IND\n", 0x08100326));
                    break;
                }
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc() IN MT DISC state, recieve unknown event, not consumed \n", 0x08100327));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;
        }

        case CFW_SM_CM_STATE_MTPROC:
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: \n", 0x08100328));
            if (pEvent->nParam1 == 0xff)
            {
                switch (proc->nCurSubState)
                {
                case SUBSTATE_MT_PROC_ATA:
                    StartAudio();

                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: SUBSTATE_MT_PROC_ATA\n", 0x08100329));
                    proc->bUsedAFlag = TRUE;
                    proc->nCalledFuncFlag = 2;
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()IN MTPROC, before SEND CONNECT 00 \n", 0x0810032a));
                    proc->nCurSubState = proc->nNextSubState;
                    CFW_SendSclMessage(API_CC_CONNECT_REQ, proc->pMsg, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()IN MTPROC, after SEND CONNECT 11 \n", 0x0810032b));
                    //CFW_BalAudCallStart();
                    break;
                case SUBSTATE_MT_PROC_CHLD1: //accept call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: SUBSTATE_MT_PROC_CHLD1 \n", 0x0810032c));
                case SUBSTATE_MT_PROC_CHLD2: //accept call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: SUBSTATE_MT_PROC_CHLD2 \n", 0x0810032d));

                    api_CcConnectReq_t *sOutMsg = (api_CcConnectReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcConnectReq_t));

                    sOutMsg->TI = proc->nTI;
                    sOutMsg->UserUserLen = 0;
                    sOutMsg->CSALen = 0;
                    sOutMsg->Facility.StructValid = FALSE;
                    proc->pMsg = sOutMsg;
                    proc->nNextSubState = proc->nCurSubState;

                    proc->bUsedChldFlag = TRUE;
                    proc->nCalledFuncFlag = 4;
                    proc->nHoldFunction = 0;
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("IN MTPROC, SEND CONNECT\n", 0x0810032e));
                    proc->nCurSubState = proc->nNextSubState;
                    CFW_SendSclMessage(API_CC_CONNECT_REQ, proc->pMsg, nSimId);

                    break;

                case SUBSTATE_MT_PROC_CHLD0: //set UDUB to reject call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: SUBSTATE_MT_PROC_CHLD0 \n", 0x0810032f));
                    proc->bMODisc = TRUE;
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Disc(proc, CFW_CM_USER_BUSY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("IN MTPROC, after SEND RELEASECOMPL to reject\n", 0x08100330));
                    break;
                case SUBSTATE_MT_PROC_REJECT:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: SUBSTATE_MT_PROC_REJECT \n", 0x08100331));
                    proc->bMODisc = TRUE;
                    proc->nCalledFuncFlag = 8; //CFW_CmReleaseCall is called

                    proc->nCurSubState = proc->nNextSubState;
                    CC_Disc(proc, CFW_CM_USER_BUSY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("IN MTPROC, SEND CFW_CM_USER_BUSY to reject\n", 0x08100332));
                    break;

                case SUBSTATE_PROC_EXPLICIT_CT:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, explicitCT, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:SUBSTATE_PROC_EXPLICIT_CT, SEND explicitCT\n", 0x08100333));

                    break;
                default:
                    break;
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }

            if (nEvtId == API_CC_CONNECTACK_IND)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MTPROC: API_CC_CONNECTACK_IND \n", 0x08100334));
                //CFW_IND_EVENT_INFO pIndInfo;
                //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                sIndInfo.call = 1;
                proc->nCallState = CFW_CM_STATUS_ACTIVE;
                CFW_SetAoState(hAo, CFW_SM_CM_STATE_ACTIVE);
                CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 1, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1); //Notify MMI a call is in progress
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()send EV_CFW_CC_CALL_INFO_IND to MMI for InComing call in progress\n", 0x08100335));

                //CFW_GetUTI(hAo, &nUTI);
                if (proc->bUsedAFlag)
                {
                    proc->bUsedAFlag = FALSE;
                    proc->nCalledFuncFlag = 0;
                    CFW_PostNotifyEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, 0, 0, nUTI | (nSimId << 24), 0x0f);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()IN MTPROC, Send NOTIFY to MMI: EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP\n", 0x08100336));
                }
                else if (proc->bUsedChldFlag)
                {
                    proc->bUsedChldFlag = FALSE;
                    proc->nCalledFuncFlag = 0;
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, 0, 0, nUTI | (nSimId << 24), 0x0f);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("IN MTPROC, Send NOTIFY to MMI: EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP\n", 0x08100337));
                }

                StartAudio();
            }
            else if (nEvtId == API_CC_ERROR_IND)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: API_CC_ERROR_IND \n", 0x08100338));
                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //[[ change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in MTPROC state,CM local Error %x\n", 0x08100339), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: API_LOW_LAYER_FAIL\n", 0x0810033a));
                }
                case API_NORMAL_RELEASE:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: API_NORMAL_RELEASE\n", 0x0810033b));
                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_NO_CONNECTION_TO_PHONE);

                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() API_NORMAL_RELEASE send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x0810033c));
                    if (proc->bUsedAFlag)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: ==>mmi proc->bUsedAFlag\n", 0x0810033d));
                        proc->bUsedAFlag = FALSE;
                        CFW_PostNotifyEvent(EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    }
                    else if (proc->bUsedChldFlag)
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: ==>mmi proc->bUsedChldFlag\n", 0x0810033e));
                        proc->bUsedChldFlag = FALSE;
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    }
                    else
                    {
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: bUsedAFlag[NULL] bUsedChldFlag[NULL]\n", 0x0810033f));
                        proc->bUsedChldFlag = FALSE;
                        //CFW_PostNotifyEvent ( EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16)|p->Cause, nUTI |(nSimId<<24), 0xf0);
                        CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_IND, proc->nIndex, iCauseValue, GENERATE_URC_UTI() | (nSimId << 24), 0x00);
                    }

                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();

                    break;
                }
                case API_CC_TIMER_EXPIRY: //change state to MODISC
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: API_CC_TIMER_EXPIRY\n", 0x08100340));
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
                    break;
                default:
                    break;
                }
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc() case CFW_SM_CM_STATE_MTPROC: not consumed\n", 0x08100341));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;
        }
        case CFW_SM_CM_STATE_ACTIVE:
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE:\n", 0x08100342));
            if (pEvent->nParam1 == 0xff)
            {
                switch (proc->nCurSubState)
                {
                case SUBSTATE_ACTIVE_OR_MEETING_DTMF:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE:SUBSTATE_ACTIVE_OR_MEETING_DTMF\n", 0x08100343));
                    proc->nCurSubState = proc->nNextSubState;
                    if (proc->pMsg != NULL)
                    {
                        CC_StartDTMF(proc, *((UINT8 *)(proc->pMsg)), nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state,send START DTMF\n", 0x08100344));
                        //CC_StopDTMF(proc, nSimId);
                        CSW_CC_FREE(proc->pMsg);
                        proc->pMsg = NULL;
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state,send STOP DTMF\n", 0x08100345));
                    }
                    break;
                case SUBSTATE_ACTIVE_CHLD0: //return ERROR Notify
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE:SUBSTATE_ACTIVE_CHLD0\n", 0x08100346));
                    proc->nCurSubState = proc->nNextSubState;
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, CFW_CC_CAUSE_NOUSED_CAUSE,
                                        (proc->nIndex << 16) | CFW_CC_ERR_REQUEST_IMPOSSIBLE, nUTI | (nSimId << 24), 0xf0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:CHLD0,send error NOTIFY to MMI\n", 0x08100347));
                    break;
                case SUBSTATE_ACTIVE_CHLD1: //end call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE:SUBSTATE_ACTIVE_CHLD1\n", 0x08100348));
                    proc->nCurSubState = proc->nNextSubState;
                    proc->bMODisc = TRUE;
                    proc->nCalledFuncFlag = 4;
                    CC_Disc(proc, CFW_CM_NORMAL_CLEARING, nSimId);
                    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:CHLD1,send DISCONNECT\n", 0x08100349));

                    break;

                case SUBSTATE_ACTIVE_CHLD2: //Hold call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: SUBSTATE_ACTIVE_CHLD2\n", 0x0810034a));
                    proc->nCurSubState = proc->nNextSubState;
                    proc->nCalledFuncFlag = 4;
                    CFW_SendSclMessage(API_CC_CALLHOLD_REQ, proc->pMsg, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("in ACTIVE state:CHLD2,send HOLD INDEX: %x TI: %x\n", 0x0810034b), proc->nIndex, ((api_CcCallHoldReq_t *)(proc->pMsg))->TI);

                    break;
                case SUBSTATE_ACTIVE_CHLD3:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: SUBSTATE_ACTIVE_CHLD3\n", 0x0810034c));
                    proc->nCurSubState = proc->nNextSubState;
                    proc->nCalledFuncFlag = 4;
                    CC_Facility(proc, buildMPTY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:CHLD3,send BUILD MPTY\n", 0x0810034d));
                    break;

                case SUBSTATE_PROC_EXPLICIT_CT:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, explicitCT, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:SUBSTATE_PROC_EXPLICIT_CT, SEND explicitCT\n", 0x0810034e));

                    break;

                default:
                    break;
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }

            switch (nEvtId)
            {
            case API_CC_CALLHOLD_CNF:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_CC_CALLHOLD_CNF\n", 0x0810034f));
                //UINT8 nCSSI, nCSSU;
                //CFW_CfgGetSSN (&nCSSI, &nCSSU);

                proc->nCalledFuncFlag = 0;
                proc->nCallState = CFW_CM_STATUS_HELD;

                CFW_SetAoState(hAo, CFW_SM_CM_STATE_HELD);

                if (nCSSU)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE:==>mmi EV_CFW_CC_CRSSINFO_IND\n", 0x08100350));
                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 2, nUTI | (nSimId << 24), 0x11);
                }
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:receive HOLD CNF, SEND NOTIFY to MMI\n", 0x08100351));
                CloseAudio();

                if (proc->nHoldFunction == 0x2F)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:receive HOLD CNF, then check if there is a wating call\n", 0x08100352));
                    CC_TerminateOne(CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD2, 2, nSimId);
                    proc->nHoldFunction = 0;
                }

                break;
            }

            case API_CC_FACILITY_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_CC_FACILITY_IND\n", 0x08100353));

                api_CcFacilityInd_t *p = (api_CcFacilityInd_t *)nEvParam;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in ACTIVE state:receive BuildMPTY CNF,p->Facility[0].Status:%d\n", 0x08100354), p->Facility[0].Status);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in ACTIVE state:receive BuildMPTY CNF,p->Facility[0].ss_Operation:%d\n", 0x08100355), p->Facility[0].ss_Operation);
                CC_SendCrssInfoNotify(&(p->Facility[0]), nSimId);
                CC_SendCrssInfoNotify(&(p->Facility[1]), nSimId); //Added by Qiyan
                proc->nCalledFuncFlag = 0;

                if (p->Facility[0].StructValid)
                {
                    if ((p->Facility[0].Status == API_CRSS_RETURN_RESULT) && (p->Facility[0].ss_Operation == buildMPTY))

                    {
                        proc->nCallState = CFW_CM_STATUS_ACTIVE;
                        proc->bIsMpty = TRUE;

                        CFW_SetAoState(hAo, CFW_SM_CM_STATE_INMEETING);

                        CC_ModifyOtherAoState(CFW_SM_CM_STATE_HELD, CFW_SM_CM_STATE_INMEETING, TRUE, nSimId);
                        CC_ModifyOtherAoState(CFW_SM_CM_STATE_MEETINGHELD, CFW_SM_CM_STATE_INMEETING, FALSE, nSimId);
                        if (nCSSU)
                        {
                            CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 4, GENERATE_URC_UTI() | (nSimId << 24), 0x11);
                        }
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:receive BuildMPTY CNF, SEND NOTIFY to MMI\n", 0x08100356));

                        /* Hold(2,0xff): ACTIVE + HOLD + WAITING */
                        if (proc->nHoldFunction == 0x2)
                        {
                            //[[ change by dingmx 20080620,only hold MPTY OK, we can accept waiting call,
                            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: proc->nHoldFunction\n", 0x08100357));
                            proc->nHoldFunction = 0; // this will be change by CC_HoldMPTY_Acceptcall
                            CC_HoldMPTY_Acceptcall(nSimId);

                            //]] change end 20080620
                        }
                    }
                    else if (p->Facility[0].Status == API_CRSS_ERROR_IND)
                    {
                        UINT32 param1 = 0;
                        UINT32 param2 = 0;
                        param1 = (p->Facility[0].ss_Code << 16) | p->Facility[0].ss_Operation;
                        if (p->Facility[0].crss_ErrorInd.Cause == API_SS_REJECT_COMPONENT)
                            param2 = (proc->nIndex << 16) | (p->Facility[0].crss_ErrorInd.ProblemCodeTag << 8) | p->Facility[0].crss_ErrorInd.Code;
                        else
                            param2 = (proc->nIndex << 16) | CFW_CRSS_ERR_RETURN_ERROR;
                        //CFW_CfgSetExtendedErrorV0(ERR_CME_SS_NOT_EXECUTED);
                        //CFW_GetUTI(hAo, &nUTI);
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, param1, param2, nUTI | (nSimId << 24), 0xf1);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in ACTIVE state:receive BuildMpty ERROR, SEND NOTIFY to MMI\n", 0x08100358));
                    }
                }
                break;
            }
            case API_CC_ERROR_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_CC_ERROR_IND \n", 0x08100359));

                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //[[ change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in ACTIVE state,CM local Error %x\n", 0x0810035a), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_LOW_LAYER_FAIL\n", 0x0810035b));
                case API_NORMAL_RELEASE:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_NORMAL_RELEASE\n", 0x0810035c));

                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    //pIndInfo.call = 0;
                    //CFW_CfgSetIndicatorEventInfo(&pIndInfo);
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_PHONE_FAILURE);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()API_NORMAL_RELEASE:send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x0810035d));
                    CFW_PostNotifyEvent(EV_CFW_CC_ERROR_IND, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    //Release the current index

                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();
                    break;
                }
                case API_CC_TIMER_EXPIRY:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_CC_TIMER_EXPIRY\n", 0x0810035e));
                case API_CALL_HOLD_REJECTED:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_CALL_HOLD_REJECTED\n", 0x0810035f));
                case API_CALL_RTRV_REJECTED:
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_SS_NOT_EXECUTED);
                    //CFW_GetUTI(hAo, &nUTI);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: API_CALL_RTRV_REJECTED ==>>mmi\n", 0x08100360));
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    break;
                default:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: EV_CFW_CC_ERROR_IND ==>>mmi\n", 0x08100361));
                    CFW_PostNotifyEvent(EV_CFW_CC_ERROR_IND, proc->nIndex, CFW_CC_ERR_REQUEST_IMPOSSIBLE, nUTI | (nSimId << 24), 0x10);
                    break;
                }
                break;
            }
            default:
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning] CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: not consumed\n", 0x08100362));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }

            break;
        case CFW_SM_CM_STATE_HELD:
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD: \n", 0x08100363));
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcAoProc() case pEvent->nParam1 %d proc->nCurSubState: %d\n", 0x08100364), pEvent->nParam1, proc->nCurSubState);

            if (pEvent->nParam1 == 0xff)
            {
                switch (proc->nCurSubState)
                {
                case SUBSTATE_HELD_CHLD0:
                    proc->nCurSubState = proc->nNextSubState;
                    proc->bRelHeldCallFlag = TRUE;
                    proc->bMODisc = TRUE;
                    proc->nCalledFuncFlag = 4;
                    CC_Disc(proc, CFW_CM_NORMAL_CLEARING, nSimId);
                    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("in HELD state:CHLD0,Send DISCONNECT\n", 0x08100365));

                    break;
                case SUBSTATE_HELD_CHLD1: //retrieve call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD: SUBSTATE_HELD_CHLD1\n", 0x08100366));
                case SUBSTATE_HELD_CHLD2: //retrieve call
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD: SUBSTATE_HELD_CHLD2\n", 0x08100367));
                case SUBSTATE_HELD_CHLD2X:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CFW_SendSclMessage(API_CC_CALLRETRIEVE_REQ, proc->pMsg, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD: in HELD state:CHLD1,2,2X,Send RETRIEVE ==>>stack\n", 0x08100368));
                    break;
                    //case SUBSTATE_ACTIVE_CHLD1X:
                    //    proc->nCurSubState = proc->nNextSubState;

                    //    break;
                    proc->nCurSubState = proc->nNextSubState;

                case SUBSTATE_HELD_CHLD3:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, buildMPTY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in HELD state:CHLD3,Send BuildMPTY\n", 0x08100369));

                    break;

                case SUBSTATE_PROC_EXPLICIT_CT:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, explicitCT, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:SUBSTATE_PROC_EXPLICIT_CT, SEND explicitCT\n", 0x0810036a));

                    break;
                default:
                    break;
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            switch (nEvtId)
            {
            case API_CC_CALLRETRIEVE_CNF:
            {
                //UINT8 nCSSI, nCSSU;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD: in HELD state:CHLD1,2,2X,Send RETRIEVE ==>>stack\n", 0x0810036b));
                //CFW_CfgGetSSN (&nCSSI, &nCSSU);

                proc->nCalledFuncFlag = 0;
                proc->nCallState = CFW_CM_STATUS_ACTIVE;

                CFW_SetAoState(hAo, CFW_SM_CM_STATE_ACTIVE);

                if (nCSSU)
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_CC_CALLRETRIEVE_CNF ==>>mmi\n", 0x0810036c));

                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 3, GENERATE_URC_UTI() | (nSimId << 24), 0x11);
                }
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in HELD state:receive RETRIEVE CNF,Send NOTIFY to MMI\n", 0x0810036d));
                StartAudio();
                break;
            }

            case API_CC_FACILITY_IND:
            {
                api_CcFacilityInd_t *p = (api_CcFacilityInd_t *)nEvParam;

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in HELD state:receive BuildMPTY CNF,p->Facility[0].Status:%d\n", 0x0810036e), p->Facility[0].Status);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in HELD state:receive BuildMPTY CNF,p->Facility[0].ss_Operation:%d\n", 0x0810036f), p->Facility[0].ss_Operation);

                proc->nCalledFuncFlag = 0;
                CC_SendCrssInfoNotify(&(p->Facility[0]), nSimId);
                CC_SendCrssInfoNotify(&(p->Facility[1]), nSimId);

                if (p->Facility[0].StructValid)
                {
                    if ((p->Facility[0].Status == API_CRSS_RETURN_RESULT) && (p->Facility[0].ss_Operation == buildMPTY))
                    {
                        //UINT8 nCSSI, nCSSU;
                        //CFW_CfgGetSSN (&nCSSI, &nCSSU);

                        proc->nCallState = CFW_CM_STATUS_ACTIVE;
                        proc->bIsMpty = TRUE;

                        CFW_SetAoState(hAo, CFW_SM_CM_STATE_INMEETING);

                        CC_ModifyOtherAoState(CFW_SM_CM_STATE_ACTIVE, CFW_SM_CM_STATE_INMEETING, TRUE, nSimId);
                        CC_ModifyOtherAoState(CFW_SM_CM_STATE_INMEETING, CFW_SM_CM_STATE_INMEETING, FALSE, nSimId);
                        if (nCSSU)
                        {
                            CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 4, GENERATE_URC_UTI() | (nSimId << 24), 0x11); //CSSU=4:call is in MPTY
                        }
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);

                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in HELD state:receive BuildMPTY CNF,Send NOTIFY to MMI\n", 0x08100370));
                    }
                    else if (p->Facility[0].Status == API_CRSS_ERROR_IND)
                    {
                        UINT32 param1 = 0;
                        UINT32 param2 = 0;
                        param1 = (p->Facility[0].ss_Code << 16) | p->Facility[0].ss_Operation;
                        if (p->Facility[0].crss_ErrorInd.Cause == API_SS_REJECT_COMPONENT)
                            param2 = (proc->nIndex << 16) | (p->Facility[0].crss_ErrorInd.ProblemCodeTag << 8) | p->Facility[0].crss_ErrorInd.Code;
                        else
                            param2 = (proc->nIndex << 16) | CFW_CRSS_ERR_RETURN_ERROR;

                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, param1, param2, nUTI | (nSimId << 24), 0xf1);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in HELD state:receive BuildMPTY ERROR,Send NOTIFY to MMI\n", 0x08100371));
                    }
                }

                break;
            }
            break;
            case API_CC_ERROR_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_CC_ERROR_IND\n", 0x08100372));

                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //[[ change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("in HELD state,CC local Error 0x%x\n", 0x08100373), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_LOW_LAYER_FAIL\n", 0x08100374));
                case API_NORMAL_RELEASE:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_NORMAL_RELEASE\n", 0x08100375));

                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    // CFW_CfgSetExtendedErrorV0(ERR_CME_PHONE_FAILURE);

                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x08100376));

                    //CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_CC_ERROR_IND, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    //Release the current index

                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();

                    break;
                }
                case API_CC_TIMER_EXPIRY:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_CC_TIMER_EXPIRY\n", 0x08100377));
                case API_CALL_HOLD_REJECTED:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_CALL_HOLD_REJECTED\n", 0x08100378));
                case API_CALL_RTRV_REJECTED:
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:API_CALL_RTRV_REJECTED\n", 0x08100379));

                    //CFW_CfgSetExtendedErrorV0(ERR_CME_SS_NOT_EXECUTED);
                    proc->nCalledFuncFlag = 0;
                    //CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    break;
                default:
                    break;
                }
                break;
            }
            default:
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc() case CFW_SM_CM_STATE_HELD:not consumed\n", 0x0810037a));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;
        case CFW_SM_CM_STATE_INMEETING:
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() CFW_SM_CM_STATE_INMEETING:\n", 0x0810037b));
            if (pEvent->nParam1 == 0xff)
            {
                switch (proc->nCurSubState)
                {
                case SUBSTATE_ACTIVE_OR_MEETING_DTMF:
                    proc->nCurSubState = proc->nNextSubState;
                    if (proc->pMsg != NULL)
                    {
                        CC_StartDTMF(proc, *((UINT8 *)(proc->pMsg)), nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state,send START DTMF\n", 0x0810037c));
                        //CC_StopDTMF(proc,nSimId);
                        CSW_CC_FREE(proc->pMsg);
                        proc->pMsg = NULL;
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state,send STOP DTMF\n", 0x0810037d));
                    }
                    break;

                case SUBSTATE_INMEETING_CHLD0: //Notify MMI ERROR
                    proc->nCurSubState = proc->nNextSubState;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_UNKNOWN);
                    //CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, CFW_CC_CAUSE_NOUSED_CAUSE,
                                        (proc->nIndex << 16) | CFW_CC_ERR_REQUEST_IMPOSSIBLE, nUTI | (nSimId << 24), 0xf0);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:CHLD0,send error NOTIFY to MMI\n", 0x0810037e));
                    break;
                case SUBSTATE_INMEETING_CHLD1:
                    proc->nCurSubState = proc->nNextSubState;
                    proc->bMODisc = TRUE;
                    proc->nCalledFuncFlag = 4;
                    CC_Disc(proc, CFW_CM_NORMAL_CLEARING, nSimId);
                    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:CHLD1, SEND DISCONNECT\n", 0x0810037f));

                    break;
                // case SUBSTATE_ACTIVE_CHLD1X:
                //     proc->nCurSubState = proc->nNextSubState;

                //     break;
                case SUBSTATE_INMEETING_CHLD2:
                    proc->nCurSubState = proc->nNextSubState;
                    proc->nCalledFuncFlag = 4;
                    CC_Facility(proc, holdMPTY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:CHLD2, SEND Hold MPTY\n", 0x08100380));
                    break;
                case SUBSTATE_INMEETING_CHLD2X:
                    proc->nCurSubState = proc->nNextSubState;
                    proc->nCalledFuncFlag = 4;
                    if (proc->bNeedSplitFlag)
                    {
                        CC_Facility(proc, buildMPTY, nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:CHLD2x, SEND BuildMPTY\n", 0x08100381));
                    }
                    else
                    {
                        CC_Facility(proc, splitMPTY, nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:CHLD2x, SEND SplitMPTY\n", 0x08100382));
                    }

                    break;

                case SUBSTATE_INMEETING_CHLD3:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, buildMPTY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:CHLD3, SEND BuildMPTY\n", 0x08100383));

                    break;

                case SUBSTATE_PROC_EXPLICIT_CT:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, explicitCT, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:SUBSTATE_PROC_EXPLICIT_CT, SEND explicitCT\n", 0x08100384));

                    break;

                default:
                    break;
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            switch (nEvtId)
            {
            case API_CC_FACILITY_IND:
            {
                api_CcFacilityInd_t *p = (api_CcFacilityInd_t *)nEvParam;
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in INMEETING state:receive API_CC_FACILITY_IND,p->Facility[0].Status:%d\n", 0x08100385), p->Facility[0].Status);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in INMEETING state:receive API_CC_FACILITY_IND,p->Facility[0].ss_Operation:%d\n", 0x08100386), p->Facility[0].ss_Operation);

                if (p->Facility[0].StructValid)
                {
                    if (p->Facility[0].Status == API_CRSS_RETURN_RESULT)
                    {
                        switch (p->Facility[0].ss_Operation)
                        {

                        case buildMPTY:
                        {
                            proc->nCallState = CFW_CM_STATUS_ACTIVE;
                            proc->bIsMpty = TRUE;
                            CFW_SetAoState(hAo, CFW_SM_CM_STATE_INMEETING);
                            CC_ModifyOtherAoState(CFW_SM_CM_STATE_HELD, CFW_SM_CM_STATE_INMEETING, TRUE, nSimId);
                            CC_ModifyOtherAoState(CFW_SM_CM_STATE_INMEETING, CFW_SM_CM_STATE_INMEETING, FALSE, nSimId);
                            if (proc->bNeedSplitFlag)
                            {
                                CC_Facility(proc, splitMPTY, nSimId);
                                proc->bNeedSplitFlag = 0; // Added by Qiyan
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:Receive BUildMPTY CNF,then send SplitMPTY\n", 0x08100387));
                            }
                            else
                            {
                                //UINT8 nCSSI, nCSSU;
                                //CFW_CfgGetSSN (&nCSSI, &nCSSU);
                                proc->nCalledFuncFlag = 0;
                                if (nCSSU)
                                    CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 4, GENERATE_URC_UTI() | (nSimId << 24), 0x11); //CSSU=4:call is in MPTY
                                //CFW_GetUTI(hAo, &nUTI);
                                CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:Receive BUildMPTY CNF,Send NOTIFY to MMI\n", 0x08100388));
                                /* Hold(2,0xff): INMEETING + HOLD + WAITING */
                                if (proc->nHoldFunction == 0x2)
                                {
                                    // change by dingmx20080620,only hold MPTY OK, we can accept waiting call,
                                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc() case CFW_SM_CM_STATE_ACTIVE: proc->nHoldFunction\n", 0x08100389));
                                    proc->nHoldFunction = 0;
                                    CC_HoldMPTY_Acceptcall(nSimId);
                                    // CC_HoldMPTY();
                                    // CC_AcceptWaitingCall( CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD2, 2) ;
                                    // proc->nHoldFunction = 0;
                                }
                            }
                            break;
                        }
                        case holdMPTY:
                        {
                            //UINT8 nCSSI, nCSSU;
                            //CFW_CfgGetSSN (&nCSSI, &nCSSU);
                            if (nCSSU)
                            {
                                CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 2, GENERATE_URC_UTI() | (nSimId << 24), 0x11);

                                //CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 4, nUTI, 0x11);//CSSU=4:call is in MPTY hameina[-]2007.11.27
                            }
                            proc->nCallState = CFW_CM_STATUS_HELD;
                            proc->bIsMpty = TRUE;
                            proc->nCalledFuncFlag = 0;
                            CFW_SetAoState(hAo, CFW_SM_CM_STATE_MEETINGHELD);
                            CC_ModifyToMeetingHldState(hAo, CFW_SM_CM_STATE_INMEETING, CFW_SM_CM_STATE_MEETINGHELD, nSimId);
                            CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:Receive HoldMPTY CNF,Send NOTIFY to MMI\n", 0x0810038a));

                            if (proc->nHoldFunction == 0x2F)
                            {
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:Receive HoldMPTY CNF, then check if there is a wating call\n", 0x0810038b));
                                CC_TerminateOne(CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD2, 2, nSimId);
                                proc->nHoldFunction = 0;
                            }
                        }
                        break;
                        case splitMPTY:
                        {
                            proc->nCallState = CFW_CM_STATUS_ACTIVE;
                            proc->bIsMpty = FALSE;
                            proc->nCalledFuncFlag = 0;
                            CFW_SetAoState(hAo, CFW_SM_CM_STATE_ACTIVE);
                            //CFW_GetUTI(hAo, &nUTI);
                            CC_ModifyToMeetingHldState(hAo, CFW_SM_CM_STATE_INMEETING, CFW_SM_CM_STATE_MEETINGHELD, nSimId);
                            CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:Receive SplitMPTY CNF,Send NOTIFY to MMI\n", 0x0810038c));

                            break;
                        }
                        default:
                            break;
                        }
                    }
                    else if (p->Facility[0].Status == API_CRSS_ERROR_IND)
                    {
                        UINT32 param1 = 0;
                        UINT32 param2 = 0;
                        param1 = (p->Facility[0].ss_Code << 16) | p->Facility[0].ss_Operation;
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:facility[0].Status=API_CRSS_ERROR_IND\n", 0x0810038d));
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(3), TSTR("crss_ErrorInd.Cause=%d, ProblemCodeTag=%d, p->Facility[0].crss_ErrorInd.Code=%d\n", 0x0810038e),
                                  p->Facility[0].crss_ErrorInd.Cause,
                                  p->Facility[0].crss_ErrorInd.ProblemCodeTag,
                                  p->Facility[0].crss_ErrorInd.Code);

                        if (p->Facility[0].crss_ErrorInd.Cause == API_SS_REJECT_COMPONENT)
                            param2 = (proc->nIndex << 16) | (p->Facility[0].crss_ErrorInd.ProblemCodeTag << 8) | p->Facility[0].crss_ErrorInd.Code;
                        else
                            param2 = (proc->nIndex << 16) | CFW_CRSS_ERR_RETURN_ERROR;
                        //CFW_CfgSetExtendedErrorV0(ERR_CME_SS_NOT_EXECUTED);
                        proc->nCalledFuncFlag = 0;
                        //CFW_GetUTI(hAo, &nUTI);
                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, param1, param2, nUTI | (nSimId << 24), 0xf1);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in INMEETING state:Send ERROR NOTIFY to MMI\n", 0x0810038f));
                    }
                    //[[hameina[+] 2008.12.8 send meeting notification
                    else if (API_CRSS_CALL_RETRIEVED == p->Facility[0].Status ||
                             API_CRSS_CALL_IS_WAITING == p->Facility[0].Status ||
                             API_CRSS_CALL_ON_HOLD == p->Facility[0].Status)
                    {
                        CC_SendCrssInfoNotify(p->Facility, nSimId);
                    }
                    //]]hameina[+]2008.12.8 send meeting notification
                }

                break;
            }
            case API_CC_ERROR_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()CFW_SM_CM_STATE_INMEETING in INMEETING state:API_CC_ERROR_IND\n", 0x08100390));
                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //[[ change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in INMEETING state,CM local Error %x\n", 0x08100391), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()CFW_SM_CM_STATE_INMEETING :API_CC_ERROR_IND API_LOW_LAYER_FAIL\n", 0x08100392));
                }
                case API_NORMAL_RELEASE:
                {
                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_PHONE_FAILURE);
                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()11send EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x08100393));
                    CFW_PostNotifyEvent(EV_CFW_CC_ERROR_IND, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    //Release the current index
                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();

                    break;
                }
                case API_CC_TIMER_EXPIRY:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()CFW_SM_CM_STATE_INMEETING :API_CC_ERROR_IND API_CC_TIMER_EXPIRY\n", 0x08100394));
                }
                case API_CALL_HOLD_REJECTED:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()CFW_SM_CM_STATE_INMEETING :API_CC_ERROR_IND API_CALL_HOLD_REJECTED\n", 0x08100395));
                }
                case API_CALL_RTRV_REJECTED:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()CFW_SM_CM_STATE_INMEETING :API_CC_ERROR_IND API_CALL_RTRV_REJECTED\n", 0x08100396));
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_SS_NOT_EXECUTED);
                    proc->nCalledFuncFlag = 0;
                    //CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                }
                break;
                default:
                    break;
                }
                break;
            }
            default:
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc()CFW_SM_CM_STATE_INMEETING :not consumed\n", 0x08100397));

                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;
        case CFW_SM_CM_STATE_MEETINGHELD:
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD:\n", 0x08100398));

            if (pEvent->nParam1 == 0xff)
            {
                switch (proc->nCurSubState)
                {
                case SUBSTATE_MEETINGHLD_CHLD0:
                    proc->nCurSubState = proc->nNextSubState;
                    proc->bRelHeldCallFlag = TRUE;
                    proc->bMODisc = TRUE;
                    proc->nCalledFuncFlag = 4;
                    CC_Disc(proc, CFW_CM_NORMAL_CLEARING, nSimId);
                    CFW_SetAoState(hAo, CFW_SM_CM_STATE_MODISC);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:CHLD0,Send DISCONNECT\n", 0x08100399));
                    break;
                case SUBSTATE_MEETINGHLD_CHLD1:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD:SUBSTATE_MEETINGHLD_CHLD1\n", 0x0810039a));
                }
                case SUBSTATE_MEETINGHLD_CHLD2:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, retrieveMPTY, nSimId);

                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcAoProc()in MEETINGHELD state:CHLD1,2,Send retrieveMPTY INDEX: %x TI: %x \n", 0x0810039b), proc->nIndex, proc->nTI);
                    break;

                case SUBSTATE_MEETINGHLD_CHLD2X:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    if (proc->bNeedSplitFlag)
                    {
                        CC_Facility(proc, buildMPTY, nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:CHLD2x,Send BuildMPTY\n", 0x0810039c));
                    }
                    else
                    {
                        CC_Facility(proc, retrieveMPTY, nSimId);
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:CHLD2x,Send RetrieveMPTY\n", 0x0810039d));
                    }

                    break;
                case SUBSTATE_MEETINGHLD_CHLD3:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, buildMPTY, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:CHLD3,Send BuildMPTY\n", 0x0810039e));

                    break;
                case SUBSTATE_PROC_EXPLICIT_CT:
                    proc->nCalledFuncFlag = 4;
                    proc->nCurSubState = proc->nNextSubState;
                    CC_Facility(proc, explicitCT, nSimId);
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:SUBSTATE_PROC_EXPLICIT_CT,Send explicitCT\n", 0x0810039f));

                    break;

                default:
                    break;
                }
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            switch (nEvtId)
            {
            case API_CC_FACILITY_IND:
            {
                api_CcFacilityInd_t *p = (api_CcFacilityInd_t *)nEvParam;

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in MEETINGHELD state:receive BuildMPTY CNF,p->Facility[0].Status:%d\n", 0x081003a0), p->Facility[0].Status);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in MEETINGHELD state:receive BuildMPTY CNF,p->Facility[0].ss_Operation:%d\n", 0x081003a1), p->Facility[0].ss_Operation);

                if (p->Facility[0].StructValid)
                {
                    if (p->Facility[0].Status == API_CRSS_RETURN_RESULT)
                    {
                        switch (p->Facility[0].ss_Operation)
                        {

                        case buildMPTY:
                        {
                            //UINT8 nCSSI, nCSSU;
                            //CFW_CfgGetSSN (&nCSSI, &nCSSU);
                            if (nCSSU)
                            {
                                CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 3, GENERATE_URC_UTI() | (nSimId << 24), 0x11); //CSSU=2:call is Retrieved

                                CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 4, GENERATE_URC_UTI() | (nSimId << 24), 0x11); //CSSU=4:call is in MPTY
                            }
                            proc->nCallState = CFW_CM_STATUS_ACTIVE;
                            proc->bIsMpty = TRUE;
                            CFW_SetAoState(hAo, CFW_SM_CM_STATE_INMEETING);
                            CC_ModifyOtherAoState(CFW_SM_CM_STATE_ACTIVE, CFW_SM_CM_STATE_INMEETING, TRUE, nSimId);
                            CC_ModifyOtherAoState(CFW_SM_CM_STATE_MEETINGHELD, CFW_SM_CM_STATE_INMEETING, FALSE, nSimId);

                            if (proc->bNeedSplitFlag)
                            {
                                CC_Facility(proc, splitMPTY, nSimId);
                                proc->bNeedSplitFlag = 0;
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:Receive BuildMPTY CNF,then send SplitMPTY\n", 0x081003a2));
                            }
                            else
                            {
                                //CFW_GetUTI(hAo, &nUTI);
                                proc->nCalledFuncFlag = 0;
                                CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:Receive BuildMPTY CNF,Send NOTIFY to MMI\n", 0x081003a3));
                            }
                            break;
                        }
                        case splitMPTY:
                        {
                            proc->nCallState = CFW_CM_STATUS_ACTIVE;
                            proc->bIsMpty = FALSE;
                            proc->nCalledFuncFlag = 0;
                            CFW_SetAoState(hAo, CFW_SM_CM_STATE_ACTIVE);
                            CC_ModifyToMeetingHldState(hAo, CFW_SM_CM_STATE_MEETINGHELD, CFW_SM_CM_STATE_MEETINGHELD, nSimId);
                            CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:Receive SplitMPTY CNF,then send NOTIFY to MMI\n", 0x081003a4));

                            break;
                        }
                        case retrieveMPTY:
                        {
                            //UINT8 nCSSI, nCSSU;
                            proc->nCalledFuncFlag = 0;
                            //CFW_CfgGetSSN (&nCSSI, &nCSSU);
                            if (nCSSU)
                            {
                                CFW_PostNotifyEvent(EV_CFW_CC_CRSSINFO_IND, 0xff, 3, GENERATE_URC_UTI() | (nSimId << 24), 0x11); //CSSU=3:call is Held
                            }

                            proc->nCallState = CFW_CM_STATUS_ACTIVE;
                            proc->bIsMpty = TRUE;
                            CFW_SetAoState(hAo, CFW_SM_CM_STATE_INMEETING);
                            CC_ModifyOtherAoState(CFW_SM_CM_STATE_MEETINGHELD, CFW_SM_CM_STATE_INMEETING, FALSE, nSimId);

                            if (proc->bNeedRetrieve)
                            {
                                CC_Facility(proc, splitMPTY, nSimId);
                                proc->bNeedRetrieve = 0;
                                StartAudio();
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:Receive RetrieveMPTY CNF,then send SplitMPTY\n", 0x081003a5));
                            }
                            else
                            {
                                CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, proc->nIndex, 0, nUTI | (nSimId << 24), 0);
                                StartAudio();
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()in MEETINGHELD state:Receive RetrieveMPTY CNF,then send NOTIFY to MMI\n", 0x081003a6));
                            }

                            break;
                        }

                        default:
                            break;
                        }
                    }
                    else if (p->Facility[0].Status == API_CRSS_ERROR_IND)
                    {
                        UINT32 param1 = 0;
                        UINT32 param2 = 0;
                        param1 = (p->Facility[0].ss_Code << 16) | p->Facility[0].ss_Operation;
                        if (p->Facility[0].crss_ErrorInd.Cause == API_SS_REJECT_COMPONENT)
                            param2 = (proc->nIndex << 16) | (p->Facility[0].crss_ErrorInd.ProblemCodeTag << 8) | p->Facility[0].crss_ErrorInd.Code;
                        else
                            param2 = (proc->nIndex << 16) | CFW_CRSS_ERR_RETURN_ERROR;
                        proc->nCalledFuncFlag = 0;

                        CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, param1, param2, nUTI | (nSimId << 24), 0xf1);
                    }
                }
                break;
            }
            case API_CC_ERROR_IND:
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: API_CC_ERROR_IND:\n", 0x081003a7));

                UINT8 iCauseValue = 0;
                api_CcErrorInd_t *p = (api_CcErrorInd_t *)nEvParam;
                proc->nCalledFuncFlag = 0;

                //[[ changed by dingmx 20080702, according TS24008 10.5.4.11
                iCauseValue = S_CFW_GET_CAUSE_VALUE(p->NwkCauseLen, p->NwkCause);
                //]] change end

                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAoProc()in MEETINGHELD state,CM local Error %x\n", 0x081003a8), p->Cause);
                switch (p->Cause)
                {
                case API_LOW_LAYER_FAIL:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: API_CC_ERROR_IND:API_LOW_LAYER_FAIL\n", 0x081003a9));
                }
                case API_NORMAL_RELEASE:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: API_CC_ERROR_IND:API_NORMAL_RELEASE\n", 0x081003aa));
                    //CFW_IND_EVENT_INFO pIndInfo;
                    //CFW_CfgGetIndicatorEventInfo(&pIndInfo);

                    sIndInfo.call = 0;
                    proc->nCallState = CFW_CM_STATUS_RELEASED;
                    //CFW_CfgSetExtendedErrorV0(ERR_CME_PHONE_FAILURE);
                    CFW_CfgSetIndicatorEventInfo(&sIndInfo, nSimId);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, proc->nIndex, GENERATE_URC_UTI() | (nSimId << 24), 1); //Notify MMI the call isn't in progress
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()send22 EV_CFW_CC_CALL_INFO_IND to MMI for Call isn't progress\n", 0x081003ab));
                    CFW_PostNotifyEvent(EV_CFW_CC_ERROR_IND, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                    //Release the current index
                    CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
                    CloseAudio();

                    break;
                }
                case API_CC_TIMER_EXPIRY:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: API_CC_ERROR_IND:API_CC_TIMER_EXPIRY\n", 0x081003ac));
                }
                case API_CALL_HOLD_REJECTED:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: API_CC_ERROR_IND:API_CALL_HOLD_REJECTED\n", 0x081003ad));
                }
                case API_CALL_RTRV_REJECTED:
                {
                    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: API_CC_ERROR_IND:API_CALL_RTRV_REJECTED\n", 0x081003ae));
                    // CFW_CfgSetExtendedErrorV0(ERR_CME_SS_NOT_EXECUTED);
                    //CFW_GetUTI(hAo, &nUTI);
                    CFW_PostNotifyEvent(EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP, iCauseValue, (proc->nIndex << 16) | p->Cause, nUTI | (nSimId << 24), 0xf0);
                }
                break;
                default:
                    break;
                }
                break;
            }
            default:
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[warning]CFW_CcAoProc()case CFW_SM_CM_STATE_MEETINGHELD: :not consumed\n", 0x081003af));
                CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
                return ret;
            }
            break;

        default:
        {
            CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_NOT_CONSUMED);
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcAoProc()default, UNKNOWN AO status\n", 0x081003b0));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
            return ret;
        }
        }
    }
    // the event are processed one time, and don't need to process again in this state.
    CFW_SetAoProcCode(hAo, CFW_AO_PROC_CODE_SINGLE | CFW_AO_PROC_CODE_CONSUMED);

    CSW_PROFILE_FUNCTION_EXIT(CFW_CcAoProc);
    return (ret);
}

//
//===================================================================================
// CM Interface by MMI
//

//
// Initiate Speech Call
//===================================================================================
UINT32 CFW_CcInitiateSpeechCall(
    CFW_DIALNUMBER *pDialNumber,
    UINT16 nUTI, CFW_SIM_ID nSimId)
{
    UINT8 nCPNLen;
    UINT8 nPhoneNumLen;
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;
    api_CcSetupReq_t *sOutMsg = 0;
    api_CcSetupReq_t *sOutMsgBackup = 0;
    UINT32 nAllState = 0;
    UINT8 nAOCount = 0;

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcInitiateSpeechCall() begin, numbertype=%d \n", 0x081003b1), pDialNumber->nType);
    CSW_TC_MEMBLOCK(CFW_CC_TS_ID, pDialNumber->pDialNumber, pDialNumber->nDialNumberSize, 16);
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcInitiateSpeechCall);

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcInitiateSpeechCall(), sim=%d \n", 0x081003b2), nSimId);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcInitiateSpeechCall() check SIM ID failed \n", 0x081003b3));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ret;
    }
    nAllState = GetCallStatus(nSimId);

    if ((nAllState & CC_STATE_ACTIVE) || (nAllState & CC_STATE_WAITING) || (nAllState & CC_STATE_INCOMING) || (nAllState & CC_STATE_DIALING) || (nAllState & CC_STATE_ALERTLING) || (nAllState & CC_STATE_RELEASE))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("[ERROR]Init speech call in wrong status. nAllState: 0x%x\n\n", 0x081003b4), nAllState);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    //check if other SIM in call
    CFW_SIM_ID nSimID;

    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_CC_SRV_ID, nSimID);
    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call when other card in call\n", 0x081003b5));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_CFW_OTHER_SIM_IN_CALL;
    }
    if (pDialNumber == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, INPUT param1 is null\n", 0x081003b6));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (pDialNumber->nDialNumberSize > 40)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, INPUT call number size too long\n", 0x081003b7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_CME_DIAL_STRING_TOO_LONG;
    }

    if (pDialNumber->pDialNumber == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, INPUT call number is null\n", 0x081003b8));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //Frank add for adjust SS status start.
    if (CFW_GetAoCount(CFW_SS_SRV_ID, nSimId) > 0)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]the SS is working now!\n", 0x081003b9));
        return ERR_CME_NO_NETWORK_SERVICE;
    }
    //Frank add for adjust SS status end.
    proc = (CM_SM_INFO *)CSW_CC_MALLOC(SIZEOF(CM_SM_INFO) + pDialNumber->nDialNumberSize);

    if (proc == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, malloc fail\n", 0x081003ba));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory32(proc, SIZEOF(CM_SM_INFO) + pDialNumber->nDialNumberSize);

    sOutMsg = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));

    nPhoneNumLen = pDialNumber->nDialNumberSize;

    // Protect the memory...
    if (nPhoneNumLen > 40)
        return ERR_CME_DIAL_STRING_TOO_LONG;

    //Called party number
    SUL_MemCopy8(&sOutMsg->CalledPartyNb[1], pDialNumber->pDialNumber, nPhoneNumLen);

    //called party number type
    sOutMsg->CalledPartyNb[0] = pDialNumber->nType;
    nCPNLen = 1 + nPhoneNumLen;

    //in order to fill index of call
    proc->nIndex = CC_GetIndex(nSimId);
    proc->bAudioOnFlag = FALSE;
    proc->bRelHeldCallFlag = FALSE;
    proc->bUsedChldFlag = FALSE;
    proc->bUsedAFlag = FALSE;
    proc->bProgressIndFlag = FALSE;
    proc->nTI = 0x80;
    proc->nCallMode = CFW_CM_MODE_VOICE;
    proc->nCallState = CFW_CM_STATUS_DIALING;
    proc->nDir = CFW_DIRECTION_MOBILE_ORIGINATED;
    proc->bIsMpty = FALSE;
    proc->nAddressType = pDialNumber->nType;
    proc->nHoldFunction = 0;
    proc->nNumberSize = nPhoneNumLen;

    if (pDialNumber->pDialNumber)
        SUL_MemCopy8(proc->pNumber, pDialNumber->pDialNumber, nPhoneNumLen);
#ifdef CFW_AMR_SUPPORT
    if (!IsDTMF_Detect_Start())
    {
        //Beaer1
        sOutMsg->Bearer1Len = 6;
        sOutMsg->Bearer1[0] = 0x60;
        sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[2] = 0x02; // Speech version 2
        sOutMsg->Bearer1[3] = 0x00; // Speech version 1
        sOutMsg->Bearer1[4] = 0x05; // HR Speech version 3
        sOutMsg->Bearer1[5] = 0x81; // HR Speech version 1
    }
    else
    {
        //Beaer1
        sOutMsg->Bearer1Len = 4;
        sOutMsg->Bearer1[0] = 0x60;
        //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[1] = 0x02; // Speech version 2
        sOutMsg->Bearer1[2] = 0x00; // Speech version 1
        sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1
        //sOutMsg->Bearer1[5] = 0x85; // HR Speech version 3
    }
#else
    //Beaer1
    sOutMsg->Bearer1Len = 4;
    sOutMsg->Bearer1[0] = 0x60;
    //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
    sOutMsg->Bearer1[1] = 0x02; // Speech version 2
    sOutMsg->Bearer1[2] = 0x00; // Speech version 1
    sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1
//sOutMsg->Bearer1[5] = 0x85; // HR Speech version 3

#endif
    //Bearer2
    sOutMsg->Bearer2Len = 0;
    // Called Party Nb
    sOutMsg->CPNLen = nCPNLen;
    //subaddress
    sOutMsg->CPSALen = 0;
    //call type
    sOutMsg->CallType = API_CC_REGULAR_CALL;
    //UserUser
    sOutMsg->UserUserLen = 0;

    //CLir
    switch (pDialNumber->nClir)
    {
    case 1:
        sOutMsg->CLIRSuppression = FALSE;
        sOutMsg->CLIRInvocation = TRUE;

        break;
    case 2:
        sOutMsg->CLIRSuppression = TRUE;
        sOutMsg->CLIRInvocation = FALSE;

        break;
    default:
        sOutMsg->CLIRSuppression = FALSE;
        sOutMsg->CLIRInvocation = FALSE;
        break;
    }

    //FacilityIE
    sOutMsg->Facility.StructValid = FALSE;
    proc->pMsg = (VOID *)sOutMsg;
#ifdef AUTO_REDAIL
    sOutMsgBackup = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));
    memcpy(sOutMsgBackup, sOutMsg, SIZEOF(api_CcSetupReq_t));
    proc->pMsgBackup = (VOID *)sOutMsgBackup;
#endif
    proc->nReleaseFlag = FALSE;
    proc->bMODisc = FALSE;
    proc->nCalledFuncFlag = 1; //CFW_CcInitiateSpeechCall is called
    proc->nNextSubState = SUBSTATE_IDLE;
    proc->nCurSubState = SUBSTATE_IDLE;
    proc->bNeedSplitFlag = FALSE;
    //register this MO Call instence
    hSm = CFW_RegisterAo(CFW_CC_SRV_ID, proc, CFW_CcAoProc, nSimId);
    if (CFW_SIM_TEST == CFW_GetSimStatus(nSimId))
    {
        vois_SetRxToTxLoopFlag(TRUE);
    }

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcInitiateSpeechCall()Register MO AO\n", 0x081003bb));
    //Set UTI for AT_APP
    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcInitiateSpeechCall() end \n", 0x081003bc));
    return ret;
}

//
//
//
//
UINT32 CFW_CcInitiateSpeechCallEx(
    CFW_DIALNUMBER *pDialNumber,
    UINT8 *pIndex,
    UINT16 nUTI, CFW_SIM_ID nSimId)
{
    UINT8 nCPNLen;
    UINT8 nPhoneNumLen;
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;
    api_CcSetupReq_t *sOutMsg = 0;
    api_CcSetupReq_t *sOutMsgBackup = 0;
    UINT32 nAllState = 0;
    UINT8 nAOCount = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcInitiateSpeechCallEx);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcInitiateSpeechCallEx(), sim=%d \n", 0x081003bd), nSimId);

    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcInitiateSpeechCallEx() check SIM ID failed \n", 0x081003be));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
        return ret;
    }
    nAllState = GetCallStatus(nSimId);

    *pIndex = 0;

    if ((nAllState & CC_STATE_ACTIVE) || (nAllState & CC_STATE_WAITING) || (nAllState & CC_STATE_INCOMING) || (nAllState & CC_STATE_DIALING) || (nAllState & CC_STATE_ALERTLING) || (nAllState & CC_STATE_RELEASE))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("Init speech call return in wrong status, nAllState=0x%x\n", 0x081003bf), nAllState);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    //check if other SIM in call
    CFW_SIM_ID nSimID;

    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_CC_SRV_ID, nSimID);
    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call when other card in call\n", 0x081003c0));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCall);
        return ERR_CFW_OTHER_SIM_IN_CALL;
    }
    if (pDialNumber == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, INPUT param1 is null\n", 0x081003c1));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (pDialNumber->pDialNumber == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, INPUT call number is null\n", 0x081003c2));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
        return ERR_CFW_INVALID_PARAMETER;
    }

    if (pDialNumber->nDialNumberSize > 40)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, INPUT call number size too long\n", 0x081003c3));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
        return ERR_CME_DIAL_STRING_TOO_LONG;
    }

    //Frank add for adjust SS status start.
    if (CFW_GetAoCount(CFW_SS_SRV_ID, nSimId) > 0)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]the SS is working now!\n", 0x081003c4));
        return ERR_CME_NO_NETWORK_SERVICE;
    }
    //Frank add for adjust SS status end.

    proc = (CM_SM_INFO *)CSW_CC_MALLOC(SIZEOF(CM_SM_INFO) + pDialNumber->nDialNumberSize);

    if (proc == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]Init speech call, malloc fail\n", 0x081003c5));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory32(proc, SIZEOF(CM_SM_INFO) + pDialNumber->nDialNumberSize);

    sOutMsg = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));

    nPhoneNumLen = pDialNumber->nDialNumberSize;

    // Protect the memory...
    if (nPhoneNumLen > 40)
        return ERR_CME_DIAL_STRING_TOO_LONG;

    //Called party number
    SUL_MemCopy8(&sOutMsg->CalledPartyNb[1], pDialNumber->pDialNumber, nPhoneNumLen);
    //called party number type
    sOutMsg->CalledPartyNb[0] = pDialNumber->nType;
    nCPNLen = 1 + nPhoneNumLen;

    //in order to fill index of call
    proc->nIndex = CC_GetIndex(nSimId);
    proc->bAudioOnFlag = FALSE;
    proc->bRelHeldCallFlag = FALSE;
    proc->bUsedChldFlag = FALSE;
    proc->bUsedAFlag = FALSE;
    proc->bProgressIndFlag = FALSE;
    proc->nTI = 0x80;
    proc->nCallMode = CFW_CM_MODE_VOICE;
    proc->nCallState = CFW_CM_STATUS_DIALING;
    proc->nDir = CFW_DIRECTION_MOBILE_ORIGINATED;
    proc->bIsMpty = FALSE;
    proc->nAddressType = pDialNumber->nType;
    proc->nHoldFunction = 0;
    proc->nNumberSize = nPhoneNumLen;

    if (pDialNumber->pDialNumber)
    {
        SUL_MemCopy8(proc->pNumber, pDialNumber->pDialNumber, nPhoneNumLen);
        CSW_CC_FREE(pDialNumber->pDialNumber);
        pDialNumber->pDialNumber = NULL;
    }
//Beaer1
#ifdef CFW_AMR_SUPPORT
    if (!IsDTMF_Detect_Start())
    {
        sOutMsg->Bearer1Len = 6;
        sOutMsg->Bearer1[0] = 0x60;
        sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[2] = 0x02; // Speech version 2
        sOutMsg->Bearer1[3] = 0x00; // Speech version 1
        sOutMsg->Bearer1[4] = 0x05; // HR Speech version 3
        sOutMsg->Bearer1[5] = 0x81; // HR Speech version 1
    }
    else
    {
        sOutMsg->Bearer1Len = 4;
        sOutMsg->Bearer1[0] = 0x60;
        //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[1] = 0x02; // Speech version 2
        sOutMsg->Bearer1[2] = 0x00; // Speech version 1
        sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1
    }
#else
    sOutMsg->Bearer1Len = 4;
    sOutMsg->Bearer1[0] = 0x60;
    //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
    sOutMsg->Bearer1[1] = 0x02; // Speech version 2
    sOutMsg->Bearer1[2] = 0x00; // Speech version 1
    sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1

#endif

    //Bearer2
    sOutMsg->Bearer2Len = 0;
    // Called Party Nb
    sOutMsg->CPNLen = nCPNLen;
    //subaddress
    sOutMsg->CPSALen = 0;
    //call type
    sOutMsg->CallType = API_CC_REGULAR_CALL;
    //UserUser
    sOutMsg->UserUserLen = 0;

    //CLir
    switch (pDialNumber->nClir)
    {
    case 1:
        sOutMsg->CLIRSuppression = FALSE;
        sOutMsg->CLIRInvocation = TRUE;

        break;
    case 2:
        sOutMsg->CLIRSuppression = TRUE;
        sOutMsg->CLIRInvocation = FALSE;

        break;
    default:
        sOutMsg->CLIRSuppression = FALSE;
        sOutMsg->CLIRInvocation = FALSE;

        break;
    }

    //FacilityIE
    sOutMsg->Facility.StructValid = FALSE;
    proc->pMsg = (VOID *)sOutMsg;
#ifdef AUTO_REDAIL
    sOutMsgBackup = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));
    memcpy(sOutMsgBackup, sOutMsg, SIZEOF(api_CcSetupReq_t));
    proc->pMsgBackup = (VOID *)sOutMsgBackup;
#endif
    proc->nReleaseFlag = FALSE;
    proc->bMODisc = FALSE;
    proc->nCalledFuncFlag = 1; //CFW_CcInitiateSpeechCall is called
    proc->nNextSubState = SUBSTATE_IDLE;
    proc->nCurSubState = SUBSTATE_IDLE;
    proc->bNeedSplitFlag = FALSE;
    //register this MO Call instence
    hSm = CFW_RegisterAo(CFW_CC_SRV_ID, proc, CFW_CcAoProc, nSimId);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Register MO AO\n", 0x081003c6));
    //Set UTI for AT_APP
    CFW_SetUTI(hSm, nUTI, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);

    *pIndex = proc->nIndex;
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcInitiateSpeechCallEx() end, *pIndex=%d \n", 0x081003c7), *pIndex);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcInitiateSpeechCallEx);
    return ret;
}

PRIVATE UINT32 CFW_CcReleaseProc(HAO hAo, CFW_EV *pEvent)
{
    HANDLE hSm = 0;
    UINT8 i = 0;
    UINT8 j = 0;
    CM_SM_INFO *proc = NULL;
    //UINT32 ret = ERR_SUCCESS;
    CFW_SIM_ID nSimId = CFW_SIM_END;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcReleaseProc);

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcReleaseProc(), BEGIN\n", 0x081003c8));
    CFW_SetServiceId(CFW_CC_SRV_ID);

    if ((UINT32)pEvent == 0xffffffff)
    {
        // get SIM ID from the APP AO, hSm-->hAo
        CFW_GetSimID(hAo, &nSimId);
        while ((hSm = CFW_GetAoHandle(j, CFW_SM_STATE_READY, CFW_CC_SRV_ID, nSimId)) != HNULL)
        {
            j++;
            CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hSm);
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("UnRegister AO in READY state: %d\n", 0x081003c9), (j - 1));
        }

        while (((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL) && (i < 7))

        {
            i++;
            proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm); //get a CM data for hSm

            if (proc == NULL)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcReleaseProc() Get user data failed\n", 0x081003ca));
                CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_RSP, ERR_CME_MEMORY_FAILURE, 0, GENERATE_URC_UTI() | (nSimId << 24), 0x20);
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcReleaseProc);
                return ERR_CME_MEMORY_FAILURE;
            }
            proc->nReleaseFlag = TRUE;

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("RELEASE THE %d call\n", 0x081003cb), (i - 1));
            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        }
        if (i == 0)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcReleaseProc()no call but call this fun CC==>>MMI EV_CFW_CC_RELEASE_CALL_RSP\n", 0x081003cc));
            CFW_PostNotifyEvent(EV_CFW_CC_RELEASE_CALL_RSP, 0, 0, GENERATE_URC_UTI() | (nSimId << 24), 0x20);
        }
    }
    else
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcReleaseProc(), pEvent!=0xffffffff\n", 0x081003cd));
    }
    CFW_UnRegisterAO(CFW_APP_SRV_ID, hAo);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcReleaseProc(), END\n", 0x081003ce));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcReleaseProc);
    return ERR_SUCCESS;
}

//=================================================================================
//
// Release any existing calls. the original one can't release all the calls when when there are more than 4 in progress.
// the release action must be done in CSW task.
//=================================================================================
UINT32 CFW_CcReleaseCall(
    CFW_SIM_ID nSimId)
{
    HANDLE hSm;
    // UINT8 j = 0;
    //  CM_SM_INFO* proc=NULL;
    UINT32 ret = ERR_SUCCESS;
    UINT32 *PCallBackData = NULL;
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcReleaseCall() begin \n", 0x081003cf));
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcReleaseCall);
    CFW_SetServiceId(CFW_CC_SRV_ID);
    PCallBackData = (UINT32 *)CSW_CC_MALLOC(4);
    //check if other SIM in call
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcReleaseCall() SIM ID=%d \n", 0x081003d0), nSimId);
    UINT8 i = 0;
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        if (!(ERR_CFW_SIM_NOT_INITIATE == ret && gEmcCallOn))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("[ERROR]CFW_CcReleaseCall() check SIM id failed, gEmcCallOn=%d \n", 0x081003d1), gEmcCallOn);
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcReleaseCall);
            return ret;
        }
        gEmcCallOn = 0;
    }
    i = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
    if (!i)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcReleaseCall() no call to release \n", 0x081003d2));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcReleaseCall);
        return ERR_CFW_NO_CALL_INPROGRESS;
    }
    i = 0;
    hSm = CFW_RegisterAo(CFW_APP_SRV_ID, PCallBackData, CFW_CcReleaseProc, nSimId);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcReleaseCall() end \n", 0x081003d3));
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcReleaseCall);
    return ERR_SUCCESS;
}
//=====================================================================================
//
//
//=====================================================================================
UINT32 CFW_CcAcceptSpeechCall(
    CFW_SIM_ID nSimId)
{
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT8 i = 0;
    UINT32 ret = ERR_SUCCESS;
    api_CcConnectReq_t *sOutMsg = NULL;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcAcceptSpeechCall);

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAcceptSpeechCall()begin \n", 0x081003d4));
    CFW_SetServiceId(CFW_CC_SRV_ID);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAcceptSpeechCall(), sim=%d \n", 0x081003d5), nSimId);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcAcceptSpeechCall(), check SIM ID failed \n", 0x081003d6));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAcceptSpeechCall);
        return ret;
    }

    while (((hSm = CFW_GetAoHandle(i, CFW_SM_CM_STATE_MTPROC, CFW_CC_SRV_ID, nSimId)) == HNULL) && (i < 7))
    {
        i++;
    }
    if ((i == 7) || (hSm == HNULL))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcAcceptSpeechCall(), no call in progress\n", 0x081003d7));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAcceptSpeechCall);
        return ERR_CFW_NO_CALL_INPROGRESS;
    }

    proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);
    if (proc == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcAcceptSpeechCall(), get user data failed\n", 0x081003d8));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcAcceptSpeechCall);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }
    else
    {
        //[[hameina[+] 2009.8.3 this API can only accept the incoming call, waiting call will return this error
        //  bug 13542
        if (proc->nCallState != CFW_CM_STATUS_INCOMING)
        {
            // when the hold/active call was released, only one call in progress with status waiting, call this function to accept this call.
            // see bug 14039
            UINT8 nCnt = 0;
            nCnt = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcAcceptSpeechCall, nCnt != %d,nCallState=%d \n", 0x081003d9), nCnt, proc->nCallState);
            if (!(proc->nCallState == CFW_CM_STATUS_WAITING && nCnt == 1))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]nCallState != CFW_CM_STATUS_INCOMING\n", 0x081003da));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcAcceptSpeechCall);
                return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
            }
        }
        //]]hameinai[+] 2009.8.3

        if (proc->nCalledFuncFlag == 2) //CFW_CmAcceptSpeechCall is called Added by Qiyan for...
        {

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]duplicated operation, holdmulity had been call to accept this call\n", 0x081003db));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcAcceptSpeechCall);
            return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
        }
#if 0
        UINT8 nUTI = 0;
        TASK_HANDLE *pHTask = NULL;
        CFW_GetUsingUTI(hSm, &nUTI);
        CFW_RelaseUsingUTI(CFW_CC_SRV_ID, nUTI);

        if(NULL == g_hCosMmiTask)
        {
            nUTI = 0; //POST TO AT
        }
        else
        {
            pHTask = (TASK_HANDLE *)g_hCosMmiTask;
            if( g_hCosMmiTask == COS_GetCurrentTaskHandle() )
            {
                CFW_GetFreeUTI(CFW_CC_SRV_ID, &nUTI);////POST TO MMI
            }
            else
            {
                nUTI = 0; //POST TO AT
            }
        }

        CFW_SetUTI(hSm, nUTI, 0);
#endif
        sOutMsg = (api_CcConnectReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcConnectReq_t));

        sOutMsg->TI = proc->nTI;
        sOutMsg->UserUserLen = 0;
        sOutMsg->CSALen = 0;
        sOutMsg->Facility.StructValid = FALSE;
        proc->pMsg = sOutMsg;
        proc->nNextSubState = proc->nCurSubState;
        proc->nCurSubState = SUBSTATE_MT_PROC_ATA;
        CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
        if (CFW_SIM_TEST == CFW_GetSimStatus(nSimId))
        {
            vois_SetRxToTxLoopFlag(TRUE);
        }
    }

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcAcceptSpeechCall() end \n", 0x081003dc));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcAcceptSpeechCall);
    return (ret);
}

//=========================================================================
//
//
//
//=========================================================================
HAO CmMTAoProc(CFW_EV *pEvent)
{
    HANDLE hSm;
    CM_SM_INFO *proc = NULL;
    UINT16 nUTI = 0;
    HAO hAo = HNULL;
    UINT8 i = 0;

    CSW_PROFILE_FUNCTION_ENTER(CmMTAoProc);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CmMTAoProc() begin\n", 0x081003dd));
    UINT32 ret = 0;
    CFW_SIM_ID nSimId = CFW_SIM_END;
    nSimId = pEvent->nFlag;
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_PROFILE_FUNCTION_EXIT(CmMTAoProc);
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CmMTAoProc() ERROR!!!!, the incoming call with invalid SIM id\n", 0x081003de));
        return ret;
    }
    proc = (CM_SM_INFO *)CSW_CC_MALLOC(SIZEOF(CM_SM_INFO) + 80);

    if (proc == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CmMTAoProc() MALLOC FAILED\n", 0x081003df));
        CSW_PROFILE_FUNCTION_EXIT(CmMTAoProc);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory32(proc, SIZEOF(CM_SM_INFO) + 80);

    while ((hAo = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
    {
        UINT32 nUti = 0;

        CM_SM_INFO *pUserData = NULL;

        pUserData = (CM_SM_INFO *)CFW_GetAoUserData(hAo);

        CFW_GetUTI(hAo, &nUti);

        if (pUserData != NULL)
        {
            if (pUserData->nCallState == CFW_CM_STATUS_DIALING)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("\nCmMTAoProc().............EXCEPTION! When MT&MO conflict............\n", 0x081003e0));
                CFW_PostNotifyEvent(EV_CFW_CC_CALL_INFO_IND, 0, pUserData->nIndex, nUti | (nSimId << 24), 1); //Notify MMI the call isn't in progress

                CFW_PostNotifyEvent(EV_CFW_CC_INITIATE_SPEECH_CALL_RSP, 0, (pUserData->nIndex << 16) | API_LOW_LAYER_FAIL, nUti | (nSimId << 24), 0xf0);
                CFW_CMUnRegisterAO(CFW_CC_SRV_ID, hAo);
            }
        }
        i++;
    }

    /*End of patch*/

    CFW_SetServiceId(CFW_CC_SRV_ID);
    proc->nIndex = CC_GetIndex(nSimId);
    proc->bAudioOnFlag = FALSE;
    proc->bRelHeldCallFlag = FALSE;
    proc->bUsedChldFlag = FALSE;
    proc->bUsedAFlag = FALSE;
    proc->bProgressIndFlag = FALSE;
    proc->nCalledFuncFlag = 0; //nothing function is called
    proc->nNextSubState = SUBSTATE_IDLE;
    proc->nCurSubState = SUBSTATE_IDLE;
    proc->bNeedSplitFlag = FALSE;
    //proc->nChldIndex = 0xff;
    proc->nReleaseFlag = FALSE;
    proc->bMODisc = FALSE;
    proc->pMsg = NULL;
    proc->nHoldFunction = 0;

#if 0
    //Find a free UTI for the MT AO.
    while(nUTI <=  0xFF)
    {
        if (CFW_IsFreeUTI(nUTI, CFW_CC_SRV_ID) == ERR_CFW_UTI_IS_BUSY)
        {
            nUTI++;
        }
        else
            break;
    }
#else
    CFW_GetFreeUTI(CFW_CC_SRV_ID, (UINT8 *)&nUTI);
#endif
    //register this MT Call instence
    hSm = CFW_RegisterAo(CFW_CC_SRV_ID, proc, CFW_CcAoProc, nSimId);
    CFW_SetUTI(hSm, nUTI, 0);

    CFW_SetAoState(hSm, CFW_SM_STATE_IDLE);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CmMTAoProc()Register MT AO\n", 0x081003e1));
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CmMTAoProc() end\n", 0x081003e2));
    CSW_PROFILE_FUNCTION_EXIT(CmMTAoProc);
    return hSm;
}

//=============================================================================
//
//
//
//=============================================================================

UINT32 SRVAPI CFW_CcGetCurrentCall(
    CFW_CC_CURRENT_CALL_INFO CallInfo[7],
    UINT8 *pCnt, CFW_SIM_ID nSimId)
{
    HAO hAo = HNULL;
    UINT8 i = 0;
    CM_SM_INFO *proc = NULL;
    *pCnt = 0x00;

    UINT8 nPhoneNumLen = 0;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcGetCurrentCall);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcGetCurrentCall()begin \n", 0x081003e3));

    UINT32 ret = 0;
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcGetCurrentCall(), nSimId=%d \n", 0x081003e4), nSimId);

    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcGetCurrentCall() ret=0x%x, gEmcCallOn=%d \n", 0x081003e5), ret, gEmcCallOn);
        if (!(ERR_CFW_SIM_NOT_INITIATE == ret && gEmcCallOn))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcGetCurrentCall() check SIM id failed\n", 0x081003e6));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcGetCurrentCall);
            return ret;
        }
    }
    else
    {
        ret = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcGetCurrentCall(), CFW_GetAoCountret=%d \n", 0x081003e7), ret);
        if (!ret)
        {
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcGetCurrentCall);
            return ERR_CFW_NO_CALL_INPROGRESS;
        }
    }

    if ((CallInfo == NULL) || (pCnt == NULL))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcGetCurrentCall(), return *pCnt =%d\n", 0x081003e8), *pCnt);
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcGetCurrentCall);
        return ERR_CFW_INVALID_PARAMETER;
    }
    *pCnt = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcGetCurrentCall(), *pCnt =%d\n", 0x081003e9), *pCnt);

    for (i = 0; i < *pCnt; i++)
    {
        if ((hAo = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
        {

            proc = (CM_SM_INFO *)CFW_GetAoUserData(hAo);
            if (proc != NULL)
            {
                nPhoneNumLen = proc->nNumberSize;
                CallInfo[i].idx = proc->nIndex;
                CallInfo[i].direction = proc->nDir;
                CallInfo[i].mode = proc->nCallMode;
                CallInfo[i].multiparty = proc->bIsMpty;
                CallInfo[i].status = proc->nCallState;
                CallInfo[i].dialNumber.nType = proc->nAddressType;
                CallInfo[i].dialNumber.nSize = nPhoneNumLen;

                /*Protect MEMORY...*/
                if (nPhoneNumLen > TEL_NUMBER_MAX_LEN)
                    nPhoneNumLen = TEL_NUMBER_MAX_LEN;
                SUL_MemCopy8(&(CallInfo[i].dialNumber.nTelNumber), proc->pNumber, nPhoneNumLen);
            }
        }
    }
    CSW_TC_MEMBLOCK(CFW_CC_TS_ID, (UINT8 *)CallInfo, sizeof(CFW_CC_CURRENT_CALL_INFO) * (*pCnt), 16);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcGetCurrentCall()end \n", 0x081003ea));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcGetCurrentCall);
    return ERR_SUCCESS;
}

//=====================================================================================
//
//
//=====================================================================================
static UINT32 GetCallStatus(
    CFW_SIM_ID nSimId)
{
    HAO hAO;
    UINT8 i = 0;
    CM_SM_INFO *pUserData = NULL;
    UINT8 nCall = 0;
    UINT32 nAllState = 0;
    UINT32 ret = 0;
    //CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("GetCallStatus(), sim=%d \n",0x081003eb), nSimId);
    //CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("GetCallStatus() \n",0x081003ec));

    CSW_PROFILE_FUNCTION_ENTER(GetCallStatus);
    CFW_SetServiceId(CFW_CC_SRV_ID);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        //CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("GetCallStatus() check SIM id ret=0x%x\n",0x081003ed), ret);
        if (!(ERR_CFW_SIM_NOT_INITIATE == ret && gEmcCallOn))
        {
            //CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]GetCallStatus(), check sim id failed \n",0x081003ee));
            CSW_PROFILE_FUNCTION_EXIT(GetCallStatus);
            return CC_STATE_NULL;
        }
    }
    nCall = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);

    if (nCall > 7)
        nCall = 7;

    for (i = 0; i < nCall; i++)
    {
        if ((hAO = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
        {
            pUserData = (CM_SM_INFO *)CFW_GetAoUserData(hAO);

            if (pUserData != NULL)
            {
                switch (pUserData->nCallState)
                {
                case CFW_CM_STATUS_DIALING:
                    nAllState |= CC_STATE_DIALING;
                    break;

                case CFW_CM_STATUS_ALERTING:
                    nAllState |= CC_STATE_ALERTLING;
                    break;
                case CFW_CM_STATUS_ACTIVE:
                    nAllState |= CC_STATE_ACTIVE;
                    break;

                case CFW_CM_STATUS_HELD:
                    nAllState |= CC_STATE_HOLD;
                    break;

                case CFW_CM_STATUS_WAITING:
                    nAllState |= CC_STATE_WAITING;
                    break;

                case CFW_CM_STATUS_INCOMING:
                    nAllState |= CC_STATE_INCOMING;
                    break;

                case CFW_CM_STATUS_RELEASED:
                    nAllState |= CC_STATE_RELEASE;
                    break;
                }
            }
        }

    } //for(;;)
    //CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID|C_DETAIL)|TDB|TNB_ARG(1), TSTR("GetCallStatus(), nAllState=0x%x\n",0x081003ef), nAllState);
    CSW_PROFILE_FUNCTION_EXIT(GetCallStatus);
    return nAllState;
}

//================================================================================
//
//
//
//================================================================================
UINT32 SRVAPI CFW_CcGetCallStatus(
    CFW_SIM_ID nSimId)
{
    UINT32 ret;
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcGetCallStatus);
    ret = GetCallStatus(nSimId);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcGetCallStatus);
    return ret;
}

//=====================================================================================
//
//Muti-Party
//=====================================================================================
UINT32 CFW_CcCallHoldMultiparty(
    UINT8 nCmd,
    UINT8 nIndex, CFW_SIM_ID nSimId)
{
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT32 nAllState = 0;
    UINT8 nCallNum = 0;
    UINT32 ret = 0;
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcCallHoldMultiparty()begin, ncmd=%d, nindex=%d\n", 0x081003f0), nCmd, nIndex);
    CSW_PROFILE_FUNCTION_ENTER(CFW_CcCallHoldMultiparty);

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcCallHoldMultiparty()SIM ID =%d, gEmcCallOn=%d\n", 0x081003f1), nSimId, gEmcCallOn);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        //emergancy call was also released by this function, return err except it may be an emergancy call
        if (!(ERR_CFW_SIM_NOT_INITIATE == ret && gEmcCallOn && nCmd == 1 && nIndex < 8))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty()check SIM id failed\n", 0x081003f2));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
            return ret;
        }
        gEmcCallOn = 0;
    }
    nCallNum = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
    if (!nCallNum)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() no call in progress\n", 0x081003f3));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
        return ERR_CFW_NO_CALL_INPROGRESS;
    }
    nAllState = GetCallStatus(nSimId);

    switch (nCmd)
    {
    case 0: //Terminate all held calls; or set UDUB (User Determined User Busy)
        //for a waiting call,i.e. reject the waiting call.
        {
            //fix bug 12972, only have active call, do nothing
            if (nAllState == CC_STATE_ACTIVE)
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case 0, only have active call\n", 0x081003f4));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
            }
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 0:\n", 0x081003f5));
            if ((nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 0:if\n", 0x081003f6));
                CC_RejectCall(nSimId);
            }
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 0:else\n", 0x081003f7));
                CC_RejectCall(nSimId);
                CC_TerminateOne(CFW_SM_CM_STATE_HELD, SUBSTATE_HELD_CHLD0, 0, nSimId);
                CC_TerminateMulti(CFW_SM_CM_STATE_MEETINGHELD, SUBSTATE_MEETINGHLD_CHLD0, 0, nSimId);
            }
            break;
        }
    case 1:
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:\n", 0x081003f8));
        UINT8 i;

        if ((1 <= nIndex) && (nIndex <= 7)) //Terminate the active call nIndex (nIndex= 1-7)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:if\n", 0x081003f9));
            for (i = 0; i < 7; i++)
            {
                if ((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
                {
                    proc = CFW_GetAoUserData(hSm);
                    if (proc != NULL)
                    {
                        if (nIndex == proc->nIndex)
                        {
                            proc->nReleaseFlag = TRUE;
                            proc->nHoldFunction = 0x11;
                            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
                            break;
                        }
                    }
                }
            }
            //hameina[+] 2009.8.3 bug 13550. invalid index, return parameter ERROR.
            if ((i == 7) && (hSm == 0))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case 1, invalid index\n", 0x081003fa));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                return ERR_CME_INVALID_INDEX;
            }
        }
        else if (255 == nIndex) //Terminate all active calls (if any) and accept the other call
        //(waiting call or held call)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else\n", 0x081003fb));

            // Hold + Wait
            if (!(nAllState & CC_STATE_ACTIVE) && (nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else 1\n", 0x081003fc));
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:elseCC_AcceptWaitingCall\n", 0x081003fd));
                CC_TerminateOne(CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD1, 1, nSimId);
            }
            // Active + Hold + Wait
            else if ((nAllState & CC_STATE_ACTIVE) && (nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else 2\n", 0x081003fe));
                CC_TerminateOne(CFW_SM_CM_STATE_ACTIVE, SUBSTATE_ACTIVE_CHLD1, 1, nSimId);
                CC_TerminateMulti(CFW_SM_CM_STATE_INMEETING, SUBSTATE_INMEETING_CHLD1, 1, nSimId);
            }
            // Active + Hold/Wait
            else if (nAllState & CC_STATE_ACTIVE) //fix the bug 3889,3891 2007.02.13
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else 3\n", 0x081003ff));

                CC_TerminateOne(CFW_SM_CM_STATE_ACTIVE, SUBSTATE_ACTIVE_CHLD1, 1, nSimId);
                CC_TerminateMulti(CFW_SM_CM_STATE_INMEETING, SUBSTATE_INMEETING_CHLD1, 1, nSimId);
            }
            else if (nAllState & CC_STATE_HOLD) //fix the bug 3889,3891 2007.02.13, this bug is from the bug 3852 because
            //of ugly stack
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else 4\n", 0x08100400));
                CC_RetrieveCall(HNULL, nSimId);
                CC_RetrieveMPTY(nSimId);
            }
            //for bug 12973, only incoming call, just do nothing
            else
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else 5\n", 0x08100401));
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty()case 1: only a incoming call, do nothing\n", 0x08100402));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
            }
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case 1, 222,invalid index\n", 0x08100403));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
            return ERR_CME_INVALID_INDEX;
        }
        break;
    }
    case 2:
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:\n", 0x08100404));
        UINT8 i;

        if ((1 <= nIndex) && (nIndex <= 7)) //Place all active calls except call nIndex(nIndex= 1-7) on hold
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:if\n", 0x08100405));
            UINT32 nState;

            for (i = 0; i < 7; i++)
            {
                if ((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
                {
                    proc = CFW_GetAoUserData(hSm);

                    if (proc != NULL)
                    {
                        if (nIndex == proc->nIndex)
                        {
                            nState = CFW_GetAoState(hSm);

                            switch (nState)
                            {
                            case CFW_SM_CM_STATE_HELD:
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:if CFW_SM_CM_STATE_HELD:\n", 0x08100406));
                                CC_ProcHeldStateCHLD2X(hSm);
                                break;

                            case CFW_SM_CM_STATE_INMEETING:
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:if CFW_SM_CM_STATE_INMEETING:\n", 0x08100407));
                                CC_ProcMeetingStateCHLD2X(hSm, nIndex, nSimId);
                                break;

                            case CFW_SM_CM_STATE_MEETINGHELD:
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:if CFW_SM_CM_STATE_MEETINGHELD:\n", 0x08100408));
                                CC_ProcMeetingHeldStateCHLD2X(hSm, nIndex, nSimId);
                                break;

                            default:
                                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:if default:\n", 0x08100409));
                                CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                                return ERR_CFW_INVALID_PARAMETER;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            //hameina[+] 2009.8.3 bug 13553. invalid index, return parameter ERROR.
            if ((i == 7) && (hSm == 0))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case2,111, invalid index\n", 0x0810040a));
                CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                return ERR_CME_INVALID_INDEX;
            }
        }
        else if (255 == nIndex) //Place all active calls on hold (if any) and accept the other call
        //(waiting call or held call) as the active call

        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcCallHoldMultiparty()case 2:else all state %d\n", 0x0810040b), nAllState);
            // Hold + Wait
            if (!(nAllState & CC_STATE_ACTIVE) && (nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
            {
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 2:elseCC_AcceptWaitingCall\n", 0x0810040c));
                CC_TerminateOne(CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD2, 2, nSimId);
            }
            // Active + Hold + Wait
            else if ((nAllState & CC_STATE_ACTIVE) && (nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
            {
                UINT32 nState;
                UINT8 j = 0;

                if (nCallNum >= 7)
                {
                    CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
                }
                //MPTY
                while (((hSm = CFW_GetAoHandle(j, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL) && (j < 7))
                {
                    j++;

                    nState = CFW_GetAoState(hSm);
                    proc = CFW_GetAoUserData(hSm);
                    proc->nNextSubState = proc->nCurSubState;

                    if (nState == CFW_SM_CM_STATE_ACTIVE)
                    {
                        proc->nHoldFunction = 0x2;
                        proc->nCurSubState = SUBSTATE_ACTIVE_CHLD3;
                        CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
                        break;
                    }
                    else if (nState == CFW_SM_CM_STATE_INMEETING)
                    {
                        proc->nHoldFunction = 0x2;
                        proc->nCurSubState = SUBSTATE_INMEETING_CHLD3;
                        CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_CONTINUE);
                        break;
                    }
                } //while
            }
            else if ((!(nAllState & CC_STATE_ACTIVE)) && (!(nAllState & CC_STATE_HOLD)) && (nAllState & CC_STATE_WAITING))
            {
                //Only accept Waiting but not Active&Hold state
                UINT32 nRet = CFW_CcAcceptSpeechCall(nSimId);
                CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcAcceptSpeechCall 0x%x\n", 0x0810040d), nRet);

                return nRet;
            }
            else
            {
                // Active + Hold/Wait
                CC_Hold_AcceptCall(nSimId);
                CC_HoldMPTY_Acceptcall(nSimId);
                CC_RetrieveCall(HNULL, nSimId);
                CC_RetrieveMPTY(nSimId);
            }
        }
        else //bug 13560
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case 2, invalid index\n", 0x0810040e));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
            return ERR_CME_INVALID_INDEX;
        }
        break;
    }
    case 3: //Add the held call to the active calls
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3:\n", 0x0810040f));

        UINT32 nState;
        UINT8 i = 0;
        BOOL bActiveFlag = FALSE;
        BOOL bHeldFlag = FALSE;
        while (((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL) && (i < 7))
        {
            i++;
            nState = CFW_GetAoState(hSm);
            if ((nState == CFW_SM_CM_STATE_ACTIVE) || (nState == CFW_SM_CM_STATE_INMEETING))
                bActiveFlag = TRUE;

            if ((nState == CFW_SM_CM_STATE_HELD) || (nState == CFW_SM_CM_STATE_MEETINGHELD))
                bHeldFlag = TRUE;
        }
        if ((i != 0) && (i < 7) && bActiveFlag && bHeldFlag)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if:\n", 0x08100410));
            if ((hSm = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
            {
                proc = CFW_GetAoUserData(hSm);
                nState = CFW_GetAoState(hSm);

                if (proc != NULL)
                {
                    proc->nNextSubState = proc->nCurSubState;

                    switch (nState)
                    {
                    case CFW_SM_CM_STATE_ACTIVE:
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if CFW_SM_CM_STATE_ACTIVE:\n", 0x08100411));

                        proc->nCurSubState = SUBSTATE_ACTIVE_CHLD3;

                        break;
                    case CFW_SM_CM_STATE_INMEETING:
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if CFW_SM_CM_STATE_INMEETING:\n", 0x08100412));

                        proc->nCurSubState = SUBSTATE_INMEETING_CHLD3;
                        break;
                    case CFW_SM_CM_STATE_HELD:
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if CFW_SM_CM_STATE_HELD:\n", 0x08100413));

                        proc->nCurSubState = SUBSTATE_HELD_CHLD3;

                        break;
                    case CFW_SM_CM_STATE_MEETINGHELD:
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if CFW_SM_CM_STATE_MEETINGHELD:\n", 0x08100414));

                        proc->nCurSubState = SUBSTATE_MEETINGHLD_CHLD3;

                        break;
                    default:
                        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if default:\n", 0x08100415));

                        CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
                        return ERR_CFW_INVALID_PARAMETER;
                        break;
                    }

                    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
                }
            }
        }
        else //bug 13561
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case 3, meeting condition not fulfill\n", 0x08100416));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
            return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
        }
        break;
    }
    case 4:
        //Only swap, not accept the waiting call
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 4:\n", 0x08100417));

        if ((nAllState & CC_STATE_ACTIVE) && (nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 4if:\n", 0x08100418));
            CC_HoldCall(nSimId);
            CC_HoldMPTY(nSimId);

            CC_RetrieveCall(HNULL, nSimId);
            CC_RetrieveMPTY(nSimId);
        }
        else if (!(nAllState & CC_STATE_ACTIVE) && (nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 4:else if1\n", 0x08100419));

            CC_RetrieveCall(HNULL, nSimId);
            CC_RetrieveMPTY(nSimId);
        }
        else if ((nAllState & CC_STATE_ACTIVE) && !(nAllState & CC_STATE_HOLD) && (nAllState & CC_STATE_WAITING))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 4:else if2\n", 0x0810041a));

            CC_HoldCall(nSimId);
            CC_HoldMPTY(nSimId);
        }
        else
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 4:else\n", 0x0810041b));

            CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
            return ERR_CFW_INVALID_PARAMETER;
        }

        break;

    case 5:
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 5:\n", 0x0810041c));
        CC_HoldMPTY(nSimId);
        break;

    case 6:
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 6CC_AcceptWaitingCall:\n", 0x0810041d));

        CC_TerminateOne(CFW_SM_CM_STATE_MTPROC, SUBSTATE_MT_PROC_CHLD2, 2, nSimId);
        break;

    case 7:
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 7:\n", 0x0810041e));

        UINT32 nState;
        //UINT8 i= 0;

        if (1 == 1)
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 3if:\n", 0x0810041f));
            if ((hSm = CFW_GetAoHandle(0, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) != HNULL)
            {
                proc = CFW_GetAoUserData(hSm);
                nState = CFW_GetAoState(hSm);

                if (proc != NULL)
                {
                    proc->nNextSubState = proc->nCurSubState;
                    proc->nCurSubState = SUBSTATE_PROC_EXPLICIT_CT;

                    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
                }
            }
        }
        else //bug 13561
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcCallHoldMultiparty() case 7, meeting condition not fulfill\n", 0x08100420));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
            return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
        }
        break;
    }

    break;

    default:

        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()default\n", 0x08100421));

        CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
        return ERR_CFW_INVALID_PARAMETER;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()end\n", 0x08100422));
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcCallHoldMultiparty()case 1:else 7\n", 0x08100423));

    CSW_PROFILE_FUNCTION_EXIT(CFW_CcCallHoldMultiparty);
    return ERR_SUCCESS;
}

//
//
//
UINT32 CFW_CcEmcDial(
    UINT8 *pDialNumber,
    UINT8 nDialNumberSize, CFW_SIM_ID nSimId)
{
    UINT8 nCPNLen;
    UINT8 nPhoneNumLen;
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;
    api_CcSetupReq_t *sOutMsg = 0;
    api_CcSetupReq_t *sOutMsgBackup = 0;
    UINT32 nAllState = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcEmcDial);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcEmcDial(), begin\n", 0x08100424));
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcEmcDial(), sim=%d \n", 0x08100425), nSimId);
    //check parameter SIM ID
    if (nSimId > (CFW_SIM_COUNT - 1))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDial(), CHECK simid failed \n", 0x08100426));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
        return ERR_CFW_INVALID_PARAMETER;
    }

    //check if other SIM in call
    //hameina[+] for UNIT TEST debug
    UINT8 nAOCount = 0;
    CFW_SIM_ID nSimID;

    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_CC_SRV_ID, nSimID);
    }

    if (nAOCount)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDial(), other SIM in call\n", 0x08100427));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
        return ERR_CFW_OTHER_SIM_IN_CALL;
    }

    nAllState = GetCallStatus(nSimId);
    if ((nAllState & CC_STATE_ACTIVE) || (nAllState & CC_STATE_WAITING) || (nAllState & CC_STATE_INCOMING) || (nAllState & CC_STATE_DIALING) || (nAllState & CC_STATE_ALERTLING) || (nAllState & CC_STATE_RELEASE))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDial(), call this function in wrong CALL STATUS\n", 0x08100428));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    if (nDialNumberSize != 2 || pDialNumber == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDial(), nDialNumberSize wrong\n", 0x08100429));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
        return ERR_CFW_INVALID_PARAMETER;
    }

//Support 911 EMC num
#if 0
    //check the number, it must be 112
    if(!((pDialNumber[0] == 0x11) && (pDialNumber[1] == 0xf2)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID | C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDial(), invalid number\n",0x0810042a));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
        return ERR_CFW_INVALID_PARAMETER;
    }
#endif

    //Frank add for adjust SS status start.
    if (CFW_GetAoCount(CFW_SS_SRV_ID, nSimId) > 0)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]the SS is working now!\n", 0x0810042b));
        return ERR_CME_NO_NETWORK_SERVICE;
    }
    //Frank add for adjust SS status end.

    proc = (CM_SM_INFO *)CSW_CC_MALLOC(SIZEOF(CM_SM_INFO) + nDialNumberSize);
    if (proc == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDial(), malloc failed\n", 0x0810042c));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
        return ERR_NO_MORE_MEMORY;
    }
    SUL_ZeroMemory32(proc, SIZEOF(CM_SM_INFO) + nDialNumberSize);

    sOutMsg = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));
    nPhoneNumLen = nDialNumberSize;

    //protect memory...
    if (nPhoneNumLen > 40)
        return ERR_CME_DIAL_STRING_TOO_LONG;

    //Called party number
    SUL_MemCopy8(&sOutMsg->CalledPartyNb[1], pDialNumber, nPhoneNumLen);

    //called party number type
    sOutMsg->CalledPartyNb[0] = 0x81;
    nCPNLen = 1 + nPhoneNumLen;

    //in order to fill index of call
    //proc->nIndex = 0xff;  //Updated by Qiyan
    proc->nIndex = CC_GetIndex(nSimId);
    proc->bAudioOnFlag = FALSE;
    proc->bRelHeldCallFlag = FALSE;
    proc->bUsedChldFlag = FALSE;
    proc->bUsedAFlag = FALSE;
    proc->bProgressIndFlag = FALSE;
    proc->nTI = 0x80;
    proc->nCallMode = CFW_CM_MODE_VOICE;
    proc->nCallState = CFW_CM_STATUS_DIALING;
    proc->nDir = CFW_DIRECTION_MOBILE_ORIGINATED;
    proc->bIsMpty = FALSE;
    proc->nAddressType = 0x81;
    proc->nHoldFunction = 0;
    proc->nNumberSize = nPhoneNumLen;

    if (pDialNumber)
        SUL_MemCopy8(proc->pNumber, pDialNumber, nPhoneNumLen);

//Beaer1
#ifdef CFW_AMR_SUPPORT
    if (!IsDTMF_Detect_Start())
    {
        sOutMsg->Bearer1Len = 6;
        sOutMsg->Bearer1[0] = 0x60;
        sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[2] = 0x02; // Speech version 2
        sOutMsg->Bearer1[3] = 0x00; // Speech version 1
        sOutMsg->Bearer1[4] = 0x05; // HR Speech version 3
        sOutMsg->Bearer1[5] = 0x81; // HR Speech version 1
    }
    else
    {
        sOutMsg->Bearer1Len = 4;
        sOutMsg->Bearer1[0] = 0x60;
        //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[1] = 0x02; // Speech version 2
        sOutMsg->Bearer1[2] = 0x00; // Speech version 1
        sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1
    }
#else
    sOutMsg->Bearer1Len = 4;
    sOutMsg->Bearer1[0] = 0x60;
    //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
    sOutMsg->Bearer1[1] = 0x02; // Speech version 2
    sOutMsg->Bearer1[2] = 0x00; // Speech version 1
    sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1

#endif

    //Bearer2
    sOutMsg->Bearer2Len = 0;
    // Called Party Nb
    sOutMsg->CPNLen = nCPNLen;
    //subaddress
    sOutMsg->CPSALen = 0;
    //call type
    sOutMsg->CallType = API_CC_EMERGENCY_CALL;
    //UserUser
    sOutMsg->UserUserLen = 0;

    //CLir
    sOutMsg->CLIRSuppression = FALSE;
    sOutMsg->CLIRInvocation = FALSE;

    //FacilityIE
    sOutMsg->Facility.StructValid = FALSE;
    proc->pMsg = (VOID *)sOutMsg;
#ifdef AUTO_REDAIL
    sOutMsgBackup = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));
    memcpy(sOutMsgBackup, sOutMsg, SIZEOF(api_CcSetupReq_t));
    proc->pMsgBackup = (VOID *)sOutMsgBackup;
#endif
    proc->nReleaseFlag = FALSE;
    proc->bMODisc = FALSE;
    proc->nCalledFuncFlag = 1; //CFW_CcInitiateSpeechCall is called
    proc->nNextSubState = SUBSTATE_IDLE;
    proc->nCurSubState = SUBSTATE_IDLE;
    proc->bNeedSplitFlag = FALSE;
    //register this MO Call instence
    hSm = CFW_RegisterAo(CFW_CC_SRV_ID, proc, CFW_CcAoProc, nSimId);
    gEmcCallOn = 1;
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("Register MO AO\n", 0x0810042d));
    //Set UTI for AT_APP
    CFW_SetUTI(hSm, 0, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);

    CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDial);
    return ret;
}

//===============================================================================
//
//
//===============================================================================
UINT32 CFW_CcEmcDialEx(
    UINT8 *pDialNumber,
    UINT8 nDialNumberSize,
    UINT8 *pIndex, CFW_SIM_ID nSimId)
{
    UINT8 nCPNLen;
    UINT8 nPhoneNumLen;
    HAO hSm;
    CM_SM_INFO *proc = NULL;
    UINT32 ret = ERR_SUCCESS;
    api_CcSetupReq_t *sOutMsg = 0;
    api_CcSetupReq_t *sOutMsgBackup = 0;
    UINT32 nAllState = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcEmcDialEx);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcEmcDialEx(), begin\n", 0x0810042e));
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcEmcDialEx(), sim=%d \n", 0x0810042f), nSimId);
    if (nSimId > (CFW_SIM_COUNT - 1))
    {
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //check if other SIM in call
    //hameina[+] for UNIT TEST debug

    UINT8 nAOCount = 0;
    CFW_SIM_ID nSimID;

    for (nSimID = CFW_SIM_0; nSimID < CFW_SIM_COUNT; nSimID++)
    {
        if (nSimId != nSimID)
            nAOCount += CFW_GetAoCount(CFW_CC_SRV_ID, nSimID);
    }
    if (nAOCount)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDialEx(), Other SIM in call\n", 0x08100430));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_CFW_OTHER_SIM_IN_CALL;
    }

    nAllState = GetCallStatus(nSimId);

    *pIndex = 0;

    if ((nAllState & CC_STATE_ACTIVE) || (nAllState & CC_STATE_WAITING) || (nAllState & CC_STATE_INCOMING) || (nAllState & CC_STATE_DIALING) || (nAllState & CC_STATE_ALERTLING) || (nAllState & CC_STATE_RELEASE))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDialEx(), wrong call status\n", 0x08100431));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    if (!nDialNumberSize || nDialNumberSize > 2)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDialEx(), nDialNumberSize = 0\n", 0x08100432));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_CFW_INVALID_PARAMETER;
    }
    //check the number, it must be 112
    /*
    if(!((pDialNumber[0] == 0x11) &&(pDialNumber[1]==0xf2)))
    {
        CSW_TRACE(_CSW|TLEVEL(CFW_CC_TS_ID|C_DETAIL)|TDB|TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDialEx(), invalid number\n",0x08100433));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_CFW_INVALID_PARAMETER;
    }*/
    //Frank add for adjust SS status start.
    if (CFW_GetAoCount(CFW_SS_SRV_ID, nSimId) > 0)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]the SS is working now!\n", 0x08100434));
        return ERR_CME_NO_NETWORK_SERVICE;
    }
    //Frank add for adjust SS status end.

    proc = (CM_SM_INFO *)CSW_CC_MALLOC(SIZEOF(CM_SM_INFO) + nDialNumberSize);
    if (proc == NULL)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDialEx(), malloc fail\n", 0x08100435));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_NO_MORE_MEMORY;
    }

    SUL_ZeroMemory32(proc, SIZEOF(CM_SM_INFO) + nDialNumberSize);

    sOutMsg = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));

    nPhoneNumLen = nDialNumberSize;

    if (nPhoneNumLen > 40)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcEmcDialEx(), number too long\n", 0x08100436));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
        return ERR_CME_DIAL_STRING_TOO_LONG;
    }
    //Called party number
    SUL_MemCopy8(&sOutMsg->CalledPartyNb[1], pDialNumber, nPhoneNumLen);
    //called party number type
    sOutMsg->CalledPartyNb[0] = 0x81;
    nCPNLen = 1 + nPhoneNumLen;

    //in order to fill index of call
    //proc->nIndex = 0xff;  //Updated by Qiyan
    proc->nIndex = CC_GetIndex(nSimId);
    proc->bAudioOnFlag = FALSE;
    proc->bRelHeldCallFlag = FALSE;
    proc->bUsedChldFlag = FALSE;
    proc->bUsedAFlag = FALSE;
    proc->bProgressIndFlag = FALSE;
    proc->nTI = 0x80;
    proc->nCallMode = CFW_CM_MODE_VOICE;
    proc->nCallState = CFW_CM_STATUS_DIALING;
    proc->nDir = CFW_DIRECTION_MOBILE_ORIGINATED;
    proc->bIsMpty = FALSE;
    proc->nAddressType = 0x81;
    proc->nHoldFunction = 0;
    proc->nNumberSize = nPhoneNumLen;

    if (pDialNumber)
        SUL_MemCopy8(proc->pNumber, pDialNumber, nPhoneNumLen);

#ifdef CFW_AMR_SUPPORT
    if (!IsDTMF_Detect_Start())
    {
        sOutMsg->Bearer1Len = 6;
        sOutMsg->Bearer1[0] = 0x60;
        sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[2] = 0x02; // Speech version 2
        sOutMsg->Bearer1[3] = 0x00; // Speech version 1
        sOutMsg->Bearer1[4] = 0x05; // HR Speech version 3
        sOutMsg->Bearer1[5] = 0x81; // HR Speech version 1
    }
    else
    {
        sOutMsg->Bearer1Len = 4;
        sOutMsg->Bearer1[0] = 0x60;
        //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
        sOutMsg->Bearer1[1] = 0x02; // Speech version 2
        sOutMsg->Bearer1[2] = 0x00; // Speech version 1
        sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1
    }
#else
    sOutMsg->Bearer1Len = 4;
    sOutMsg->Bearer1[0] = 0x60;
    //sOutMsg->Bearer1[1] = 0x04; // Speech version 3
    sOutMsg->Bearer1[1] = 0x02; // Speech version 2
    sOutMsg->Bearer1[2] = 0x00; // Speech version 1
    sOutMsg->Bearer1[3] = 0x81; // HR Speech version 1
#endif

    //Bearer2
    sOutMsg->Bearer2Len = 0;
    // Called Party Nb
    sOutMsg->CPNLen = nCPNLen;
    //subaddress
    sOutMsg->CPSALen = 0;
    //call type
    sOutMsg->CallType = API_CC_EMERGENCY_CALL;
    //UserUser
    sOutMsg->UserUserLen = 0;

    //CLir
    sOutMsg->CLIRSuppression = FALSE;
    sOutMsg->CLIRInvocation = FALSE;

    //FacilityIE
    sOutMsg->Facility.StructValid = FALSE;
#ifdef AUTO_REDAIL
    sOutMsgBackup = (api_CcSetupReq_t *)CFW_MemAllocMessageStructure(SIZEOF(api_CcSetupReq_t));
    memcpy(sOutMsgBackup, sOutMsg, SIZEOF(api_CcSetupReq_t));
    proc->pMsgBackup = (VOID *)sOutMsgBackup;
#endif
    proc->pMsg = (VOID *)sOutMsg;
    proc->nReleaseFlag = FALSE;
    proc->bMODisc = FALSE;
    proc->nCalledFuncFlag = 1; //CFW_CcInitiateSpeechCall is called
    proc->nNextSubState = SUBSTATE_IDLE;
    proc->nCurSubState = SUBSTATE_IDLE;
    proc->bNeedSplitFlag = FALSE;
    //register this MO Call instence
    hSm = CFW_RegisterAo(CFW_CC_SRV_ID, proc, CFW_CcAoProc, nSimId);
    gEmcCallOn = 1;
    //Set UTI for AT_APP
    CFW_SetUTI(hSm, 0, 0);
    CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
    *pIndex = proc->nIndex;
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcEmcDialEx()end,*pIndex=%d \n", 0x08100437), *pIndex);
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcEmcDialEx);
    return ret;
}

//===============================================================================
//
//
//===============================================================================
UINT32 CFW_CcPlayTone(
    INT8 iTone, CFW_SIM_ID nSimId)
{
    HAO hSm;
    CM_SM_INFO *proc;
    UINT8 i = 0;
    UINT32 ret = ERR_SUCCESS;
    UINT8 *sOutMsg = 0;
    UINT32 state = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcPlayTone);
    CFW_SetServiceId(CFW_CC_SRV_ID);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcPlayTone(),iTone=%d\n", 0x08100438), iTone);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcPlayTone(), sim id=%d \n", 0x08100439), nSimId);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcPlayTone(), check sim id ret=0x%x\n", 0x0810043a), ret);
        if (!(ERR_CFW_SIM_NOT_INITIATE == ret && gEmcCallOn))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcPlayTone(), check sim id failed \n", 0x0810043b));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcPlayTone);
            return ret;
        }
    }
    i = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
    if (!i)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcPlayTone(), NO call in progress to playtone\n", 0x0810043c));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcPlayTone);
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
    i = 0;

    if (!IsDTMFChar(iTone))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcPlayTone(), invalid input param iTone\n", 0x0810043d));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcPlayTone);
        return ERR_CME_INVALID_CHARACTERS_IN_DIAL_STRING;
    }
    //first, send DTMF from the active or meeting call.
    while ((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) && (i < 7))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcPlayTone(), first while i=%d\n", 0x0810043e), i);
        i++;
        state = CFW_GetAoState(hSm);
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);

        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcPlayTone(), first while state=0x%x\n", 0x0810043f), state);
        if ((state == CFW_SM_CM_STATE_ACTIVE) || (state == CFW_SM_CM_STATE_INMEETING))
        {
            proc->nNextSubState = proc->nCurSubState;
            proc->nCurSubState = SUBSTATE_ACTIVE_OR_MEETING_DTMF;

            sOutMsg = (UINT8 *)CSW_CC_MALLOC(1);

            *sOutMsg = iTone;
            proc->pMsg = sOutMsg;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcPlayTone(), in active meeting state success end\n", 0x08100440));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcPlayTone);
            return ret;
        }
    }

    //no active/meeting call, then send DTMF from alerting call
    i = 0;
    hSm = HNULL;
    while (!(hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) && (i < 7))
    {
        i++;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcPlayTone(), after second while i=%d, hSm=0x%x\n", 0x08100441), i, hSm);

    if ((i < 7) && (hSm != HNULL))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcPlayTone(), after second while\n", 0x08100442));
        state = CFW_GetAoState(hSm);
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);

        if (proc->nCallState == CFW_CM_STATUS_ALERTING)
        {
            proc->nNextSubState = proc->nCurSubState;
            proc->nCurSubState = SUBSTATE_MO_PROC_DTMF;

            sOutMsg = (UINT8 *)CSW_CC_MALLOC(1);

            *sOutMsg = iTone;
            proc->pMsg = sOutMsg;

            CFW_SetAoProcCode(hSm, CFW_AO_PROC_CODE_MULTIPLE);
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcPlayTone(), MO PROC success end\n", 0x08100443));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcPlayTone);
            return ret;
        }
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcPlayTone()\n", 0x08100444));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcPlayTone);
    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
}

UINT32 CFW_CcStopTone(
    CFW_SIM_ID nSimId)
{
    HAO hSm;
    CM_SM_INFO *proc;
    UINT8 i = 0;
    UINT32 ret = ERR_SUCCESS;
    UINT32 state = 0;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcStopTone);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcStopTone(), sim id=%d \n", 0x08100445), nSimId);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcStopTone(), check sim id ret=0x%x\n", 0x08100446), ret);
        if (!(ERR_CFW_SIM_NOT_INITIATE == ret && gEmcCallOn))
        {
            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcStopTone(), check sim id failed \n", 0x08100447));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcStopTone);
            return ret;
        }
    }
    i = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
    if (!i)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcStopTone(), NO call in progress to CFW_CcStopTone\n", 0x08100448));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcStopTone);
        return ERR_CME_OPERATION_NOT_ALLOWED;
    }
    i = 0;

    //first, send DTMF from the active or meeting call.
    while ((hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) && (i < 7))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcStopTone(), first while i=%d\n", 0x08100449), i);
        i++;
        state = CFW_GetAoState(hSm);
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);

        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("CFW_CcStopTone(), first while state=0x%x\n", 0x0810044a), state);
        if ((state == CFW_SM_CM_STATE_ACTIVE) || (state == CFW_SM_CM_STATE_INMEETING))
        {
            //if(SUBSTATE_ACTIVE_OR_MEETING_DTMF == proc->nCurSubState)
            {
                CC_StopDTMF(proc, nSimId);
            }

            return ret;
        }
    }

    //no active/meeting call, then send DTMF from alerting call
    i = 0;
    hSm = HNULL;
    while (!(hSm = CFW_GetAoHandle(i, CFW_SM_STATE_UNKONW, CFW_CC_SRV_ID, nSimId)) && (i < 7))
    {
        i++;
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(2), TSTR("CFW_CcStopTone(), after second while i=%d, hSm=0x%x\n", 0x0810044b), i, hSm);

    if ((i < 7) && (hSm != HNULL))
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcStopTone(), after second while\n", 0x0810044c));
        state = CFW_GetAoState(hSm);
        proc = (CM_SM_INFO *)CFW_GetAoUserData(hSm);

        if (proc->nCallState == CFW_CM_STATUS_ALERTING)
        {
            //if(SUBSTATE_MO_PROC_DTMF == proc->nCurSubState)
            {
                CC_StopDTMF(proc, nSimId);
            }

            CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcStopTone(), MO PROC success end\n", 0x0810044d));
            CSW_PROFILE_FUNCTION_EXIT(CFW_CcStopTone);
            return ret;
        }
    }
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcStopTone()\n", 0x0810044e));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcStopTone);
    return ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
}

//=================================================================================

//=================================================================================
UINT32 CFW_CcRejectCall(
    CFW_SIM_ID nSimId)
{
    HANDLE hAO;
    UINT8 i = 0;
    CM_SM_INFO *pUserData = NULL;
    UINT32 ret = ERR_SUCCESS;

    CSW_PROFILE_FUNCTION_ENTER(CFW_CcRejectCall);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcRejectCall() begin\n", 0x0810044f));
    CFW_SetServiceId(CFW_CC_SRV_ID);
    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(1), TSTR("[ERROR]CFW_CcRejectCall() input SIM ID=%d\n", 0x08100450), nSimId);
    ret = CFW_CheckSimId(nSimId);
    if (ERR_SUCCESS != ret)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcRejectCall() check SIM ID failed\n", 0x08100451));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcRejectCall);
        return ret;
    }
    i = CFW_GetAoCount(CFW_CC_SRV_ID, nSimId);
    if (!i)
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcRejectCall() get AO COUNT=0\n", 0x08100452));
        CSW_PROFILE_FUNCTION_EXIT(CFW_CcRejectCall);
        return ERR_CFW_NO_CALL_INPROGRESS;
    }
    i = 0; //hameina[+] for UINT TEST debug

    do
    {
        hAO = CFW_GetAoHandle(i, CFW_SM_CM_STATE_MTPROC, CFW_CC_SRV_ID, nSimId);
        i++;
    } while ((!hAO) && (i < 7));

    if ((i <= 7) && hAO) //changyg [mod] for bug 10131
    {
        pUserData = CFW_GetAoUserData(hAO);

        pUserData->nNextSubState = pUserData->nCurSubState;
        pUserData->nCurSubState = SUBSTATE_MT_PROC_REJECT;

        CFW_SetAoProcCode(hAO, CFW_AO_PROC_CODE_CONTINUE);
    }
    else
    {
        CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("[ERROR]CFW_CcRejectCall() no call waiting or incoming\n", 0x08100453));
        ret = ERR_CME_OPERATION_TEMPORARILY_NOT_ALLOWED;
    }

    CSW_TRACE(_CSW | TLEVEL(CFW_CC_TS_ID | C_DETAIL) | TDB | TNB_ARG(0), TSTR("CFW_CcRejectCall() end\n", 0x08100454));
    CSW_PROFILE_FUNCTION_EXIT(CFW_CcRejectCall);

    return (ret);
}

#endif // ENABLE
