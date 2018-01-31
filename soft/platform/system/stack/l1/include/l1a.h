/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : l1a.h
--------------------------------------------------------------------------------

  Scope      : L1 asynchrone, header file.

  History    :
--------------------------------------------------------------------------------
  Aug 25 03  | ADA   | Creation
================================================================================
*/
#ifndef __L1A_H__
#define __L1A_H__

#include "sxs_type.h"
#include "l1a_msg.h"
#include "itf_l1.h"

typedef struct
{
 Msg_t *Msg;
 u16 NbMeas;
// u8  PrIdx;
} l1a_PowerCtx_t;

#ifndef STK_SINGLE_SIM
void l1a_ChangeState          (u32 State, u32 SimId);
void l1a_ChangeSubState       (u32 SetResetSubState, u32 SimId);
u32  l1a_CheckOtherBusy (u32 SimId);
#else
void l1a_ChangeState          (u32 State);
void l1a_ChangeSubState       (u32 SetResetSubState);
#endif
void l1a_ASynchChangeState    (u32 State);
void l1a_ASynchChangeSubState (u32 SetResetSubState);
void l1a_JobUpdt              (u32 JobBitMap, u32 JobToDelete, u32 JobToReset);
void l1a_JobMapDeletion       (u32 JobBitMap);
void l1a_JobMapStop           (u32 JobBitMap);
void l1a_ASynchJobMapStop     (u32 JobBitMap);
void l1a_JobMapResume         (u32 JobBitMap);
void l1a_ASynchJobMapResume   (u32 JobBitMap);
void l1a_JobDeletion          (u8 JobNb);
void l1a_ASynchJobDeletion    (u8 JobNb);
void l1a_RaiseLongLivedJEvent(u32 EvtId);
void l1a_AsynRaiseLongLivedJEvent(u32 EvtId);
#ifndef __NO_GPRS__
void l1a_GetGPTimAdv    (l1_GPTimAdv_t *GPTimAdv);
//void l1a_GetGPwrCtrl    (l1_GPwrCtrl_t *GPwrCtrl);
#endif

void l1a_PowerReq       (Msg_t *Msg);
void l1a_PowerRsp       (l1a_PowerCtx_t *PowerCtx);
void l1a_SynchReq       (Msg_t *Msg);
void l1a_SynchUpdateReq (Msg_t *Msg);
void l1a_StopSynchReq   (Msg_t *Msg);
void l1a_CchReq         (Msg_t *Msg);
void l1a_CbchReq        (Msg_t *Msg);
void l1a_CbchSkipReq    (Msg_t *Msg);
#ifndef __NO_GPRS__
void l1a_PCchReq        (Msg_t *Msg);
#endif
void l1a_BcchReq        (Msg_t *Msg);
//added by zhanghy
void l1a_BcchModeReq  (Msg_t *Msg);
//end
#ifndef __NO_GPRS__
void l1a_PBcchReq       (Msg_t *Msg);
#endif
void l1a_StopChReq      (Msg_t *Msg);
void l1a_RachOnReq      (Msg_t *Msg);
void l1a_RachUpdtReq    (Msg_t *Msg);
void l1a_RachOffReq     (Msg_t *Msg);
void l1a_ConnectReq     (Msg_t *Msg);
void l1a_CellOptUpdtReq (Msg_t *Msg);
void l1a_FreqRedefReq   (Msg_t *Msg);
void l1a_ChanModeModifyReq (Msg_t *Msg);
void l1a_SetTestLoopReq (Msg_t *Msg);
void l1a_CipherReq      (Msg_t *Msg);
void l1a_ReleaseReq     (Msg_t *Msg);
void l1a_PhyInfoReq     (Msg_t *Msg);
#ifndef __NO_GPRS__
void l1a_TBFCfgReq      (Msg_t *Msg);
void l1a_MacFixedUpdtReq (Msg_t *Msg);
void l1a_PwrCtrlTAReq   (Msg_t *Msg);
void l1a_PDCHRelReq     (Msg_t *Msg);
void l1a_TBFRelReq      (Msg_t *Msg);
void l1a_PSDataReq      (Msg_t *Msg);
#endif
void l1a_MonitorReq     (Msg_t *Msg);
void l1a_ExtdMonitorReq (Msg_t *Msg);
void l1a_MonitorInd     (u8 SendReport);
#ifndef __NO_GPRS__
void l1a_NcMonitorReq   (Msg_t *Msg);
#ifndef STK_SINGLE_SIM
void l1a_NcMonitorInd   (u32 TimerId);
void l1a_NcMonitorStart (bool ResetMeas, bool ForceRestart, u32 SimId);
void l1a_ChnQualityReq  (u32 SimId);
#else
void l1a_NcMonitorInd   (void);
void l1a_NcMonitorStart (bool ResetMeas, bool ForceRestart);
void l1a_ChnQualityReq  (void);
#endif
void l1a_PMeasCtrlReq   (Msg_t *Msg);
void l1a_ExtIntReq      (Msg_t *Msg);
#endif
#ifdef __EGPRS__
void l1a_EgprsTestLoopReq(Msg_t *Msg);
#endif

#endif
