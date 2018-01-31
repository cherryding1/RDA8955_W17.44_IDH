/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/
//------------------------------------------------------------------------------
//  $Log: llc_func.h,v $
//  Revision 1.4  2006/06/26 12:05:34  fch
//  Now LLC suspend and resume RLC when MMP orders. If LLC receives a XID frame with reset and/or IOV_UI, it sends a RLU_PURGE_REQ to RLU to request purge of PDU (with cause USER_DATA) that are in RLU queue. (Mantis #449)
//
//  Revision 1.3  2006/05/23 10:19:27  fch
//  Change to set array of pointer in ROM
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:46  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is not property of StackCom.
  This source code is from the public domain.
  The information contained in this file is not confidential.
  Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is allowed.

================================================================================
*/
/*
================================================================================
  File       : LLC_FUNC.H
--------------------------------------------------------------------------------

  Scope      : Protoypes of LLC functions

  History    :
--------------------------------------------------------------------------------
  Mar 11 04  |  FCH   | Creation            
================================================================================
*/

#ifndef __LLC_FUNC_H__
#define __LLC_FUNC_H__

#include "sxr_ops.h"
#include "cmn_defs.h"

#include "llc_msg.h"




/*
================================================================================
  llc_evt.c 
================================================================================
*/
InEvtDsc_t    *llc_GiveDsc                ( u32 );


/*
================================================================================
  llc_ctx.c 
================================================================================
*/
void          llc_LleCtxInit              ( void );
void          llc_InitDefParam            ( void );
void          llc_ClearCtx                ( u8, u8 );
void          llc_ClearUsrVar             ( void );


/*
================================================================================
  llc_frm.c 
================================================================================
*/
u8            llc_GetFrmFormatId          ( s16 );
u8            llc_CheckInvalidFrm         ( s16 );
void          llc_BuildSABM               ( void );
void          llc_BuildUFrm               ( u8, u8, u8);
void          llc_BuildAndSndRspFrm       ( u8 , u8 );
void          llc_BuildAndSndDISC         ( void );
bool          llc_IsXidParPresent         ( u8 );
bool          llc_CheckAndGetXid          ( void );
void          llc_UpdateWithXidPar        ( void );
u8            llc_InsertXidParVal         (u8 Size, u8 Type, u32 Val, u8 *Ptr);
void          llc_BuildXidParamField      ( bool );
void          llc_BuildAndSndIFrm         ( llc_TxL3Pdu_t*, bool , bool, u8 );
u8            llc_ProcessAckPart          ( u8, u8, u8* );
bool          llc_ProcessInfoPart         ( u16 );
bool          llc_IsAckReqNeeded          ( llc_TxL3Pdu_t* );
bool          llc_IFrmTransmission        ( bool, u8 );
bool          llc_IFrmRetransmission      ( u8 );
bool          llc_ProcessUnitDataReq      ( Msg_t* );
bool          llc_ProcessDataReq          ( void );
void          llc_ProcessSavedUnitDataReq ( void );
void          llc_BuildAndSndFRMR         ( u8, u8, bool );



/*
================================================================================
  llc_util.c 
================================================================================
*/
bool          llc_IsFcsCorrect            ( u8 *, u16, bool );
void          llc_CipherFrm               ( u8 *, u16, u16, u32, u8, bool );
bool          llc_CheckTLLI               ( void );
//void          llc_TrcUIParamsDwn          ( void );
//void          llc_TrcUIParamsUp           ( void );
bool          llc_CheckUnitDataReqParam   ( llc_UnitDataReq_t * );
void          llc_SaveLlcParam            ( void );
u8            llc_PurgeQueue              ( void *, void * );
void          llc_InitAbmVar              ( void );
void          llc_ReleaseUCmd             ( void );
void          llc_StartRetransTimer       ( u32 );
void          llc_StopRetransTimer        ( u32 );
void          llc_ResetLLE                ( void );
void          llc_RelXidParTab            ( void );
bool          llc_ProcessXidReqRsp        ( u8 );
u8            llc_ProcessSavedMsg         ( void );
void          llc_SaveMsg                 ( void );
u8            llc_ProcessRelease          ( bool, u8 );
void          llc_ClearCtx                ( u8, u8 );
bool          llc_IsValInWindow           ( u16, u16, u16 );
llc_TxL3Pdu_t *llc_SearchTxL3PduQ_Ns      ( u16 );
void          llc_RxIFrmIndShift          ( void );
u8            llc_PduNsSort               (llc_TxL3Pdu_t*, llc_TxL3Pdu_t* );
void          llc_UpdateVs                ( void );
void          llc_UpdateVr                ( void );
bool          llc_InitiateReestab         ( void );
bool          llc_ProcessReestab          ( void );
void          llc_PopInFrame              ( void );

#ifdef __LLC_DEBUG__
void          llc_RxIFrmIndPrint          ( void );
#endif

/*
================================================================================
  llc_out.c 
================================================================================
*/
void          llc_PrepMsg                 ( u16 );
void          llc_SendMsg                 ( u8 );
void          llc_SndNewTlliInd           ( void );
void          llc_SndDataInd              ( bool );
void          llc_SndUIFrmInDataReq       ( Msg_t *Msg );
void          llc_SndUFrm                 ( u8 );
void          llc_SndStatusInd            ( u8, u8 );
void          llc_SndReleaseInd           ( u8 );
void          llc_SndReleaseCnf           ( void );
void          llc_SndEstabCnfOrInd        ( void );
void          llc_SndXidCnfOrInd          ( void );
void          llc_SndResetInd             ( void );
void          llc_SndDataCnf              ( Msg_t * );
void          llc_RluPurgeReq             ( void );

#ifdef __LLC_SPY__
/*
================================================================================
  llc_spy.c 
================================================================================
*/
void          llc_InitSpy                 ( void );
void          llc_UpdateSpy               ( void );
#endif 

#endif // __LLC_FUNC_H__
