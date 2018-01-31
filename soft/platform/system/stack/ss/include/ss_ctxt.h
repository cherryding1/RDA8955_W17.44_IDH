// //////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2002-2006, Coolsand Technologies, Inc.
// All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
// 
// FILENAME: ss_ctxt.h
// 
// DESCRIPTION:
// Defines SS Context
// 
// REVISION HISTORY:
// NAME            DATE            REMAKS
// DMX | 20081024 | Add dual Sim support
// //////////////////////////////////////////////////////////////////////////////

#ifndef __SS_CTXT_H__
#define __SS_CTXT_H__

// System includes
#include "sxs_type.h"
#include "fsm.h"

// Interface includes
#include "ss_msg.h"
#include "itf_ss.h"

// EnDec includes
// #include "edre.h"

// SS internal includes

#ifdef __SS_C__
#  define DefExtern
#else
#  define DefExtern extern
#endif

// ==============================================================================
// Data       : ss_Ctxt_t
// ------------------------------------------------------------------------------
// Scope      : SS context for a particular TI. This data structure represents 
// the context of an SS transaction for the duration of a
// transaction. Transacton Contexts are stored in a queue according
// to transaction ID.
// ==============================================================================
typedef struct
{
  u8                   TI;             // Transaction ID for the Transaction
  ss_Code_t            ss_Code;        // The SS-Code for the transaction
  ss_Operation_t       ss_Operation;   // The SS-Operation for the transaction
  ss_ActionReqParams_t ss_Params;      // The input parameters received from the MMI in an api_SsActionReq message
  u8                   State;          // The current state of the transaction
  u8                   InvokeID;       // The Invoke ID received from the network
  u8                   L3Msg[225];     // Message to be transmitted
  u8                   L3MsgLenInBytes;
  ss_RxMsgInfo_t       rxMsgInfo;      // Information about a received message
  Msg_t               *rxMsgPtr;       // Pointer to received message
  u8                   msgIndex;       // Index to received message
}ss_Ctxt_t;

// Utility definitions to access members of the current transaction's context
#define SS_OPERATION            (gp_ssCurData->CurCtx->ss_Operation)
#define SS_CODE                 (gp_ssCurData->CurCtx->ss_Code)
#define SS_PARAMS               (gp_ssCurData->CurCtx->ss_Params)
#define SS_INVOKE_ID            (gp_ssCurData->CurCtx->InvokeID)

// ==============================================================================
// Data       : ss_Data_t
// ------------------------------------------------------------------------------
// Scope      : Global SS context
// ==============================================================================
typedef struct
{
  Msg_t       *OMsg;
  OutEvtDsc_t *OEvtDsc;
  ss_Ctxt_t   *CurCtx;
  u8           TIBitmap;
  u8           PtrCtxQ;
  u8           ssTransactionsInProgress;
  u8           bUSSDBusy;
#if !(STK_MAX_SIM==1)
	u32 	SimIndex;
#endif
}ss_Data_t;

DefExtern ss_Data_t *gp_ssCurData;

// create new CTX
#define SS_CTX_VALID 1
#define SS_CTX_INVALID 0

// we need kill FSM
#define SS_CTX_VALID_NO_FSM 2

// need not run fsm for we has sent SS FACILITY in ss_StartMSInitiatedTransaction
#define SS_CTX_VALID_FSM 3

#undef DefExtern
#endif // __SS_CTXT_H__
 
