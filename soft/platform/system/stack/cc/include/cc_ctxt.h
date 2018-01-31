// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2006, Coolsand Technologies, Inc.
// All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
// 
// FILENAME: cc_ctxt.c
// 
// DESCRIPTION:
// CC Context
// 
// REVISION HISTORY:
// NAME|  DATE  |           REMAKS
// DMX | 20081024 | Add dual Sim support
// DMX | 2007-01-20 | Add CAI data
// DMX | 2006-10-09 | Enlarge DTMF BUFFER from 5 to 20
// DMX | 2006-00-00 | To fix status queue bug ,remove CC_AUX_CALLWAITING flag 20060523
// //////////////////////////////////////////////////////////////////////////////

#ifndef __CC_CTXT_H__
#define __CC_CTXT_H__

// System includes
#include "sxs_type.h"
#include "fsm.h"

// Interface includes
#include "cc_msg.h"
#include "itf_cc.h"

// EnDec includes
#include "edre.h"

// CC internal includes

#ifdef __CC_C__
#  define DefExtern
#else
#  define DefExtern extern
#endif

// ==============================================================================
// Data       : cc_DtmfProc_t
// ------------------------------------------------------------------------------
// Scope      : DTMF Procedure Management      
// ==============================================================================

// changed DTMF buffer size from 5 to 20 by DingMX
#define CC_MAX_DTMF            20 // Max # of anticipated Dtmf
#define CC_INVALID_KEY       255

// 
// Flags Definition
// 
#define CC_STOP_DTMF_2SEND   (1<<0) // Stop  has been sent by API
#define CC_STOP_DTMF_SENT    (1<<1) // Stop  has been sent to Network
#define CC_START_DTMF_SENT   (1<<2) // Start has been sent to Network
#define CC_START_ACK_REC     (1<<3) // StartAck has been rec. from Network

typedef struct
{
  u8 Key; // [ 0..9,A,B,C,D,*,# ]
  u8 Flags; // Acknowledged, Stop Requested

} cc_DtmfElem_t;

typedef struct
{
  cc_DtmfElem_t DtmfBuf[CC_MAX_DTMF]; // Buffer of DTMF to handle
  u8 RIdx;  // Read  Index in the buffer
  u8 WIdx;  // Write Index in the buffer
  bool ProcRunning; // A Proc for one Dtmf is ongoing
  u8 ProcIdx; // Index of the Running proc
} cc_DtmfProc_t;

// 
// ==============================================================================
// Data       : cc_Ctxt_t
// ------------------------------------------------------------------------------
// Scope      : CC context for a particular TI
// ==============================================================================

typedef struct
{
  u8 Flags;
  u8 TI;
  u8 State;
  u8 AuxState;
  u8 OldAuxState; /* Used in MPTY in case of failure */
  u8 FsmState;
  u8 CurBearIdx;
  u8 BearerLen[2];
  u8 DiscCause[3];
    u8* Bearer[2];
  cc_SsOperation_t CurCrssOpe;
  cc_DtmfProc_t DtmfProc;
    Msg_t* ApiMsg;
    Msg_t* ApiMsg2;

//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
    Msg_t* SavedApiSetupMsg;    
	u32 MMErrorCount;
	u32 RetryTimeRuning; // if this runing, not send abort to MM when API ask release the CALL
    u32 TimeReceivedSetupReq;
	u8  LastMMErrorCause;
//]] __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX	
} cc_Ctxt_t ;
#define CC_MAX_RETRY_COUNT 2
#define CC_MAX_RETRY_TIME  (30 SECONDS)
// 
// Defines for the Flags field of cc_Ctxt_t
// 
#define CC_PROGRESS_RCVD     ( 1 << 0 ) /* A progress ind. has been received */
#define CC_RVRSSETUP_MODIFY  ( 1 << 1 ) /* The flag Reverse call setup was  */
                                        /* present in the MODIFY message    */
#define CC_SIGNAL_RCVD       ( 1 << 2 )

#define CC_ALERT_ASKED       ( 1 << 3 )
#define CC_FACILITY_AOC_RCVD ( 1 << 4 )

// 
// Defines for the CallType field
// 
#define CC_NO_CALL 0  /* The 1 and 2 values are defined in itf_api.h */

// 
// Defines for the State field
// 
#define CC_STATE_U0  0
#define CC_STATE_U01 2
#define CC_STATE_U1  1
#define CC_STATE_U3  3
#define CC_STATE_U4  4
#define CC_STATE_U6  6
#define CC_STATE_U7  7
#define CC_STATE_U8  8
#define CC_STATE_U9  9
#define CC_STATE_U10 10
#define CC_STATE_U11 11
#define CC_STATE_U12 12
#define CC_STATE_U19 19
#define CC_STATE_U26 26
#define CC_STATE_U27 27

#define CC_STATE_INVALID 0xFF

// add 20081211 for Status message 
#define CC_STATE_N0  0
#define CC_STATE_N01 2
#define CC_STATE_N1  1
#define CC_STATE_N3  3
#define CC_STATE_N4  4
#define CC_STATE_N6  6
#define CC_STATE_N7  7
#define CC_STATE_N8  8
#define CC_STATE_N9  9
#define CC_STATE_N10 10
#define CC_STATE_N12 12
#define CC_STATE_N19 19
#define CC_STATE_N26 26
#define CC_STATE_N27 27
#define CC_STATE_N28 28
#ifdef __CC_C__

u8 ccStateTab[11] = { 0xFF, CC_STATE_U0, CC_STATE_U01, CC_STATE_U1, CC_STATE_U6,
                      CC_STATE_U8, CC_STATE_U9, CC_STATE_U10, CC_STATE_U11,
                      CC_STATE_U12, CC_STATE_U19 };
#else
extern u8 ccStateTab[11];
#endif

// 
// Defines for the AuxState field
// 
#define CC_AUX_ASSIGN( _Ctx, _Value )                                          \
  {                                                                            \
  _Ctx->AuxState = _Value;                                                     \
  SXS_TRACE( CC_CRSS_TRC, "New AuxState 0x%x of Ctx TI %d\n",                \
               _Ctx->AuxState, _Ctx->TI );                                     \
  }

#define CC_AUX_MODIFY( _Mask, _Value )                                         \
  {                                                                            \
  CC_AUX_ASSIGN( gp_ccCurData->CurCtx, (gp_ccCurData->CurCtx->AuxState & ~_Mask) | _Value ); \
  }

#define CC_AUX_IDLE     0
// Call Hold, Call Retrieve
#define CC_AUX_HLD_MASK 0x0C
#define CC_AUX_HOLDREQ  ( 1 << 2 )
#define CC_AUX_CALLHELD ( 2 << 2 )
#define CC_AUX_RTRVREQ  ( 3 << 2 )
// MPTY
#define CC_AUX_MPTY_MASK  0x03
#define CC_AUX_MPTYREQ    1
#define CC_AUX_CALLINMPTY 2
#define CC_AUX_SPLITREQ   3
#define CC_NO_CUR_CRSS_OPE (cc_SsOperation_t)0

// ==============================================================================
// Data       : cc_Data_t
// ------------------------------------------------------------------------------
// Scope      : Global CC context
// ==============================================================================

typedef struct
{
  TccDl_Data* IRadioMsg;
  Msg_t* IMsg;
  Msg_t* OMsg;
  OutEvtDsc_t* OEvtDsc;
  cc_Ctxt_t* CurCtx;
  u8 SST[13];
  u8 CurTI;
  u8 TIBitmap;
  u8 Flags;

  u8 PtrCtxQ;
  u8 NbReest;

  u8 qSavedMessageForDtmf; // save message for DTMF


// add by dingmx
api_StackCfg_t* sap_StackCfg;

#if !(STK_MAX_SIM==1)
  u32 SimIndex;
#endif


} cc_Data_t;

// 
// Defines for the Flags field of cc_Data_t
// 
#define CC_TCH_ON        ( 1 << 1 ) /* The TCH is on, speech can be enabled */
#define CC_T308_ONE_EXP  ( 1 << 2 ) /* T308 has expired once                */
#define CC_KEEP_MSG      ( 1 << 3 ) /* Incoming msg shall not be deleted    */
#define CC_AUDIO_RDY     ( 1 << 4 ) /* The nwk has enabled the audio        */
#define CC_API_AWARE     ( 1 << 5 ) /* The API knows that the audio are on  */
#define CC_NWK_CONG      ( 1 << 6 ) /* Nwk is congested for the User Info   */
#define CC_AOC_STARTED   ( 1 << 7 )


DefExtern cc_Data_t* gp_ccCurData;

#undef DefExtern
#endif // __CC_CTXT_H__
 
