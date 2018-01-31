//------------------------------------------------------------------------------
//  $Log: lap_fsm.h,v $
//  Revision 1.4  2006/05/22 12:46:12  aba
//  Copyrigth is updated
//
//  Revision 1.3  2006/02/24 10:33:37  aba
//  Issue 343: L2 SAPI3 establishment by the network, the MS do not re-sent UA if SABM is retransmitted
//
//  Revision 1.2  2005/12/29 17:38:45  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/
/*
================================================================================
  File       : lap_fsm.h
--------------------------------------------------------------------------------

  Scope      : Finite State Machine defintions for lapdm

  History    :
--------------------------------------------------------------------------------
  Sep 28 03  |  ABA   | Creation
================================================================================
*/
#include "fsm.h"

/*
=======================
  LAP state definitions
=======================
*/
#define LAP_GENE                   0
#define LAP_IDLE                   1
#define LAP_ESTAB_SABM_SENT        2
#define LAP_MFE                    3
#define LAP_SUSPENDED              4
#define LAP_RESUME_SABM_SENT       5
#define LAP_RECONNECT_SABM_SENT    6
#define LAP_TIMER_RECOVERY         7
#define LAP_RELEASE_DISC_SENT      8

/*
** Transitions prototypes
*/
extern u8 lap_DoNothing                  (void);
extern u8 lap_Idle_EstablishReq          (void);
extern u8 lap_Idle_RxSabm                (void);
extern u8 lap_Idle_RxDisc                (void);
extern u8 lap_EstabSabmSent_RxUa         (void);
extern u8 lap_EstabSabmSent_RxDm         (void);
extern u8 lap_EstabSabmSent_RxDisc       (void);
extern u8 lap_EstabSabmSent_LastT200     (void);
extern u8 lap_MFE_RxUa                   (void);
extern u8 lap_MFE_RxDm                   (void);
extern u8 lap_MFE_RxDisc                 (void);
extern u8 lap_MFE_RxRej                  (void);
extern u8 lap_MFE_RxSabm                 (void);
extern u8 lap_MFE_SuspendReq             (void);
extern u8 lap_MFE_ReconnectReq           (void);
extern u8 lap_MFE_NormalReleaseReq       (void);
extern u8 lap_MFE_DataReq                (void);
extern u8 lap_MFE_T200                   (void);
extern u8 lap_MFE_RxI                    (void);
extern u8 lap_MFE_RxRr                   (void);
extern u8 lap_Suspended_ResumeReq        (void);
extern u8 lap_Suspended_ReconnectReq     (void);
extern u8 lap_Suspended_DataReq          (void);
extern u8 lap_ResumeSabmSent_RxUa        (void);
extern u8 lap_ResumeSabmSent_RxDm        (void);
extern u8 lap_ResumeSabmSent_ReconnectReq(void);
extern u8 lap_ResumeSabmSent_SuspendReq  (void);
extern u8 lap_ResumeSabmSent_LastT200    (void);
extern u8 lap_ReconnectSabmSent_RxUa     (void);
extern u8 lap_ReconnectSabmSent_RxDm     (void);
extern u8 lap_ReconnectSabmSent_LastT200 (void);
extern u8 lap_TimerRecovery_RxRrOrRej    (void);
extern u8 lap_TimerRecovery_RxDm         (void);
extern u8 lap_TimerRecovery_RxI          (void);
extern u8 lap_TimerRecovery_DataReq      (void);
extern u8 lap_TimerRecovery_T200         (void);
extern u8 lap_TimerRecovery_LastT200     (void);
extern u8 lap_ReleaseDiscSent_RxUa       (void);
extern u8 lap_ReleaseDiscSent_RxDm       (void);
extern u8 lap_ReleaseDiscSent_RxDisc     (void);
extern u8 lap_ReleaseDiscSent_SuspendReq (void);
extern u8 lap_ReleaseDiscSent_LastT200   (void);
extern u8 lap_SabmOrDiscSent_T200        (void);
extern u8 lap_AllState_UnitDataReq       (void);
extern u8 lap_AllState_RxUi              (void);
extern u8 lap_AllState_LocalReleaseReq   (void);
extern u8 lap_AllState_L1TxInd           (void);
extern u8 lap_SabmOrDiscSent_T200        (void);
extern u8 lap_ArmT200                    (void);








