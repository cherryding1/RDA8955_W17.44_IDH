//------------------------------------------------------------------------------
//  $Log: mmp_tool.h,v $
//  Revision 1.8  2006/06/05 16:06:28  fch
//  Review of the RR_SUSPEND_REQ (do not lead to change state).
//  When a reselection occurs during the GPRS Idle period (Suspend + CellInd) while MMP is waiting for an answer from NW, on the CellInd the CellUpdate is performed before continuing the ongoing procedure (Mantis #433)
//
//  Revision 1.7  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.6  2006/05/16 09:17:24  fch
//  Added __GPRS__ flag for GSM only compilation
//
//  Revision 1.5  2006/02/28 13:14:31  fch
//  Correction for T3302 handling
//
//  Revision 1.4  2006/02/15 15:45:41  fch
//  Add RR_SUSPEND_REQ handling.
//  Flag __MMP_NO_SUSPEND_REQ__ (temporary) allows to revert to MMP not handling RR_SUSPEND_REQ
//
//  Revision 1.3  2006/01/17 15:09:01  oth
//  Merge from FCH version
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : mmp_tool.h
--------------------------------------------------------------------------------

  Scope      : Header file for the generic Packet Switch MM functions

  History    :
--------------------------------------------------------------------------------
  Apr 09 03  |  OTH   | Creation            
================================================================================
*/
#ifdef __GPRS__

#ifndef __MMP_TOOL_H__
#define __MMP_TOOL_H__

// System includes
#include "sxs_type.h"

// Interface includes
#include "itf_llc.h"

// Endec includes
#include "ed_c.h"

void mmp_UpdRandAndRes(u8* rand, u8* res);

void mmp_CheckEncode      ( s16, bool, u8 );
u32  mmp_HdleGPRSTimer    ( u8, u8 );
bool mmp_HdleReadyTimer   ( bool, c_GPRSTimerIE * );
void mmp_HdleT3302        ( c_GPRSTimerIE *T3302Ie );
void mmp_DeriveTlli       ( u32, u8 );
u32  mmp_ComputeTlli      ( u8 );
void mmp_HdleIdReq        ( void );
void mmp_HdleAuthCiphReq  ( void );
void mmp_StartAttachReq   ( bool );
void mmp_StartRAUReq      ( u8, bool );
void mmp_AbortAttach      ( bool );
void mmp_HdlForceToStdby  ( u8 );
void mmp_SndRauStatus     ( u8, bool, u8, u8* );
void mmp_StopTim          ( u32 );
void mmp_StopAllTim       ( void );
void mmp_StartTim         ( u32 , u32 );
void mmp_StartT3312       ( void );
void mmp_SndRrMmpProcInd  ( bool );
void mmp_SndRrGprsReq     ( bool );
void mmp_ResetGprsLociPar ( u8, bool );
void mmp_AttFailure       ( void );
u8   mmp_RauFailure       ( void );
u8   *mmp_GiveFPlmnPtr    ( u8 * );
void mmp_InsFPlmn         ( u8 * );
void mmp_SndLlcTriggerReq ( u8, bool );
void mmp_ChkInitCellUpd   ( u8 );
void mmp_HdleAuthCiphRej  ( void );
void mmp_HandleOtherPd    ( void );
void mmp_FwdL3DataReq     ( Msg_t * );
void mmp_SuspendLlc       ( void );

#if !(STK_MAX_SIM==1)
void mmp_SuspendOthSimLlc (void);
void mmp_ResumeOthSimLlc (void);
void mmp_GetMsgSaved(void);
#endif

void mmp_ResumeLlc        ( void );
void mmp_StartRAUReq      ( u8, bool );
void mmp_AbortRau         ( bool );
void mmp_T33x0ExpGene     ( u8 );
void mmp_SndAbortInd      ( u8 );
void mmp_WriteFileSim     ( u8, u8*, u8, u8 );
void mmp_SndRluTReadyReq  ( u8 );
void mmp_ChgReadyStateTo  ( bool );
void mmp_HdleRlcTReadyInd ( void );
void mmp_ClearReady       ( void );
void mmp_SndWaitingL3Data ( void );
void mmp_InvalidSim       ( void );
void mmp_StartDetach      ( bool );
void mmp_EndOfDetach      ( bool );
void mmp_AbortDetach      ( void );
u8   mmp_GetDetachOrigin  ( void );
void mmp_SndStopCnf       ( u8 );
void mmp_SndDetachAcc     ( u8 );
u8   mmp_HdleDetachReq    ( void );
u8   mmp_HdlePagingInd    ( u8 );
void mmp_SndStatus        ( u8 );
void mmp_BackInactif      ( void );
//  dmx 20150706
void mmp_SetStkSimCloseCnfToSend(void);
void mmp_SendSimCloseCnf(void);



// MMP macros

#define MMP_START_PROC    TRUE
#define MMP_STOP_PROC     FALSE

  // BE CARREFUL
  // Those value must be aligned on Action field in UnitDataReq_t
#define MMP_MSG_CIPHERED      LLC_CIPHER
#define MMP_MSG_NOT_CIPHERED  0
#define MMP_START_TREADY      LLC_START_TREADY
#define MMP_NOT_START_TREADY  0
#define MMP_INFORM_ON_TX      LLC_INFORM_ON_TX
#define MMP_INFORM_ON_ACK     LLC_INFORM_ON_ACK


#define MMP_RESET_ATT_ATMPT_CNT()                                     \
  {                                                                   \
    pMMCtx->mmpCtx.AttAtmptCnt = 0;                                     \
    SXS_TRACE ( MMP_SPEPROC_TRC, "Attach attempt counter reseted" );\
  }

#define MMP_INC_ATT_ATMPT_CNT()                                       \
  {                                                                   \
    pMMCtx->mmpCtx.AttAtmptCnt++;                                       \
    SXS_TRACE ( MMP_SPEPROC_TRC, "Attach attempt counter = %u",     \
                                        pMMCtx->mmpCtx.AttAtmptCnt);    \
  }

#define MMP_RESET_RAU_ATMPT_CNT()                                     \
  {                                                                   \
    pMMCtx->mmpCtx.RauAtmptCnt = 0;                                     \
    SXS_TRACE ( MMP_SPEPROC_TRC, "RAU attempt counter reseted" );   \
  }

#define MMP_INC_RAU_ATMPT_CNT()                                       \
  {                                                                   \
    pMMCtx->mmpCtx.RauAtmptCnt++;                                       \
    SXS_TRACE ( MMP_SPEPROC_TRC, "RAU attempt counter = %u",        \
                                        pMMCtx->mmpCtx.RauAtmptCnt);    \
  }

#define MMP_SND_DATA_REQ_ALLOWED                                      \
    ( (pMMCtx->CellInd.AvailableServ & RR_GPRS_OFFERED) &&              \
      (mm_GiveFLaiPtr(pMMCtx->CellInd.u.rr.Lai) == NULL) &&             \
      ((pMMCtx->mmpCtx.FlagsProc & (MMP_UPDATE_NEEDED | MMP_UPDATE_INTERRUPTED)) == 0) &&  \
      !(pMMCtx->mmpCtx.FlagsSusp & (MMP_LLC_SUSPENDED | MMP_GPRS_SUSPENDED)) &&      \
      ( pMMCtx->mmsimIf.RAUpStatus == MM_UPDATED)&&  \
      !(pMMCtx->mmActif & MM_MMC_ACTIF) )

// DMX add 20161031.  avoid to crash
#define MMP_SAVE_DATA_MAX       20
#define MMP_SAVE_DATA_REQ(_Msg)                                           \
  {                                                                       \
    SXS_TRACE (MM_OTHER_TRC, "DATA_REQ saved for latter processing");  \
    if (sxr_QueueGetLoad(pMMCtx->mmpCtx.DataReqQ) >MMP_SAVE_DATA_MAX)    {	\
		while ( sxr_QueueGetLoad(pMMCtx->mmpCtx.DataReqQ) ){   \
			Msg_t* Msg = sxr_PopOut(pMMCtx->mmpCtx.DataReqQ);   \
			sxr_Free(Msg);   \
	}}             \
    sxr_PopIn(_Msg, pMMCtx->mmpCtx.DataReqQ);                               \
  }

#define MMP_UNASSIGNED_TLLI                                               \
  {                                                                       \
    if (pMMCtx->mmpCtx.Tlli != LLC_TLLI_UNASSIGNED)                         \
    {                                                                     \
      mmp_DeriveTlli( pMMCtx->mmpCtx.Tlli, MMP_NOT_ASSIGNED_TLLI );         \
      pMMCtx->mmpCtx.FlagsSusp &= ~MMP_LLC_SUSPENDED;                       \
    }                                                                     \
  }

/*added start by yangyang for EDGE at 2007-04-04*/
#ifdef __EGPRS__
#define API_MSNWKCAP_SIZE         9
#define API_MSRACAP_SIZE      52

//DefExtern u8  api_MsNwkCap[API_MSNWKCAP_SIZE];   // 24.008
//DefExtern u8  api_MsRACap[API_MSRACAP_SIZE];     // 24.008
 u16 api_ComputeMsRACapFun ( u8 *DstPara, u8 Dstlen ,u8 * PriorAccessTech,u8 length, bool module );
 #endif
/*added end by yangyang for EDGE at 2007-04-04*/

#endif // __MMP_TOOL_H__

#endif // __GPRS__
