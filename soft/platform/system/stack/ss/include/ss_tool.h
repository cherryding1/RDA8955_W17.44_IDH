////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: ss_tool.h
// 
// DESCRIPTION:
//   Header file for generic tools for SS
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
// DMX | 20081022 | Add ss_StartTimer
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SS_TOOL_H
#define _SS_TOOL_H

// System includes
#include "itf_msg.h"
#include "sxs_type.h"
#include "fsm.h"
#include "ss_msg.h"
#include "ss_ctxt.h"

//==============================================================================
//  Function   : ss_SendNetworkReturnErrorMsg
//------------------------------------------------------------------------------
//  Scope      :  This is a utility function that prepares an L3 message containing 
//             a Facility or Release Complete MessageType, 
//             a Facility    IE, 
//             and a Return Error Component
//             and Sends the Message to the Network
//             The caller may specify the invokeID and the error Code
//            
//  Parameters :  ss_MessageTag_t messageType - either Facility or ReleaseComplete (spec might not allow RC)
//                u8 TI - The TI for the transaction
//                u8 invokeID  - The invoke ID must match the ID sent in the Register Message
//                ss_USSDErrors_t USSDError - The Error Code to Send
//  Return     :  12 if message is constructed successfuly,
//                else the MSG_LEN where the construction failed
//==============================================================================
extern u8 ss_SendNetworkReturnErrorMsg( ss_MessageType_t messageType, u8 TI, u8 invokeID, ss_USSDErrors_t USSDError );

//==============================================================================
//  Function   : ss_SendNetworkRejectMsg
//------------------------------------------------------------------------------
//  Scope      : This is a utility function that prepares an L3 message containing 
//               a Facility or Release Complete MessageType, 
//               a Facility    IE, 
//               and a Reject Component
//               and Sends the Message to the Network
//               The caller may specify the invokeID and the error Code
//            
//  Parameters : ss_MessageTag_t messageType - either Facility or ReleaseComplete (spec might not allow RC)
//               u8 invokeID  - The invoke ID must match the ID sent in the Register Message
//               ss_RejectComponentTag_t rejectCompTag - the problem code tag and problem code
//  Return     : 12 if message is constructed successfuly,
//               else the MSG_LEN where the construction failed
//==============================================================================
extern u8 ss_BuildNetworkRejectMsg( ss_MessageType_t messageType, u8 invokeID, ss_RejectComponentTag_t rejectCompTag );

//==============================================================================
//  Function   : ss_StartMSInitiatedTransaction
//------------------------------------------------------------------------------
//  Scope      : Starts a new SS transaction.  Function decodes and validates ss_ActionReq
//               If the ss_ActionReq contains a valid SS Code and Operation pair, a
//               an appropriate L3 message will be prepared and stored in the Ctxt and the
//               state machine will be triggered to establish an MM connection.  If the 
//               ss_ActionReq is invalid, an appropriate error is returned.  This allows
//               the caller to send an ss_ErrorInd back to the API.
//  Parameters : Msg_t *Msg - a pointer to the input message containing the ss_ActionReq.
//  Return     : An error indicator to be included in an ss_ErrorInd message
//==============================================================================

extern u8            ss_StartMSInitiatedTransaction(Msg_t *Msg);

#ifdef __SS_TIMER__

//==============================================================================
//  Function   : ss_StopAllTimers
//------------------------------------------------------------------------------
//  Scope      : Utility function that stops the SS timer for the current transaction
//  Parameters : None 
//  Return     : None
//==============================================================================
extern void            ss_StopAllTimers    ( void );
extern void ss_StartTimer (u32 Period, u32 Id, void *Data, u8 ReStart, u8 Mbx);
#endif

//==============================================================================
//  Function   : ss_AllocTI
//------------------------------------------------------------------------------
//  Scope      : Allocates a TI
//  Parameters :
//  Return     : New allocated TI or SS_INVALID_TI if no more TI available.
//==============================================================================

extern u8            ss_AllocTI            ( void );


//==============================================================================
//  Function   : ss_TICmp
//----------------------------------------------------------------------------
//  Scope      : Searches the Ctxt for a Ctxt using a TI
//  Parameters : address of TI to compare to and pointer to the context for the 2nd TI
//  Return     : TRUE if equal
//==============================================================================

extern    u8            ss_TICmp            ( void* CmpTI, void* FoundCtx );

//==============================================================================
//Function    : ss_getCtxtPtr
//----------------------------------------------------------------------
//Scope       : Looks up a Ctxt in the Ctxt queue based on TI
//Parameters  : a TI
//Return      : a pointer to a Ctxt or NULL
//==============================================================================

extern ss_Ctxt_t    *ss_getCtxtPtr( u8 TI );


//==============================================================================
//  Function   : ss_PrepMsg
//------------------------------------------------------------------------------

//  Scope      : Allocation of a Message, and fill header 
//  Parameters :
//  Return     :
//==============================================================================

extern void            ss_PrepMsg (OutEvtDsc_t * Evt, bool IsIL);


//==============================================================================
//  Function   : ss_SendMsg
//------------------------------------------------------------------------------

//  Scope      : Sending of Prepared Message              
//  Parameters : The destination mailbox
//  Return     :
//==============================================================================
extern void            ss_SendMsg (u8 Mbx);

//
// TI_Flag and TI_Value extraction macros + TI Release macro
//
#define SS_GET_TI_FLAG( _TI )  (_TI & 0x08) >> 3
#define SS_GET_TI_VAL( _TI )   (_TI & 0x07)
#define SS_RELEASE_TI( _TI )   gp_ssCurData->TIBitmap &= ~( 1 << _TI )

#define SS_MSG_MAX_SIZE CMN_L3_MAX_LENGTH
#define SS_INVALID_TI    0xFF

#ifdef __SS_TIMER__
//
// Timers duration
//
#define SS_TIMER0_DURATION  ( 30 SECONDS )
#endif

//
// Trace levels definition (5 first levels used for generic)
//
#define SS_ENDEC_TRC  (_SS | TLEVEL(6) )
#define SS_RRITF_TRC  (_SS | TLEVEL(7) )
#define SS_MMITF_TRC  (_SS | TLEVEL(8) )
#define SS_APIITF_TRC (_SS | TLEVEL(9) )
#define SS_TIMER_TRC  (_SS | TLEVEL(10))
#define SS_PROT_TRC   (_SS | TLEVEL(11))

#endif //end of file

