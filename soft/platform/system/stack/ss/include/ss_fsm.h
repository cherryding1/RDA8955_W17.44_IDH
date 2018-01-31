////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: ss_fsm.h  
// 
// DESCRIPTION:
//   Finite State Machine defintions for SS
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
// W F | 20070712 | add ss_ussdMSReleaseComplete_U01 declaration  in U01 State 
// DMX | 20070402 | add ss_ussdMSReleaseComplete_U1 declaration  in U1 State 
////////////////////////////////////////////////////////////////////////////////

#ifndef __SS_FSM_H__
#define __SS_FSM_H__

#include "fsm.h"

//===================
//  SS definitions
//===================

extern Fsm_t  ss_Fsm ;

//
// SS States definitions
// FSM ss
//
#define SS_GENE  0
#define SS_U0    1
#define SS_U01   2
#define SS_U1    3

//--------------
// Generic State
//--------------
u8   ss_mmReleaseInd_Gene ( void );

//---------
//  State U0
//---------
void ss_Init_U0( void );             //called when state machine goes back to initial state, not called at reset
u8 ss_apiActionReq_U0( void );        //called when API wants to trigger SS action

//----------
// U01 State
//----------
u8 ss_mmEstabCnf_U01( void );
u8 ss_mmEstabErr_U01( void );
#ifdef __SS_TIMER__
u8 ss_mmServReq_U01( void );
u8 ss_Timer0Exp_U01( void );
#endif
u8 ss_mmReleaseInd_U01( void );
u8 ss_ussdMSReleaseComplete_U01( void ); //20070712

//---------
// U1 State - response received from network for call waiting, caller line id, and call forwarding
//---------
u8 ss_componentReceived_U1 ( void );
u8 ss_ussdMSReleaseComplete_U1(void); // 20070402
#ifdef __SS_TIMER__
u8 ss_Timer0Exp_U1( void );
#endif
u8 ss_mmReleaseInd_U1( void );

//--------
//Utillity Functions
//--------
/*
//==============================================================================
//  Function   : ss_RunFSM
 ----------------------------------------------------------------------------
   Scope      : Run the FSM, the function validates gp_ssCurData->CurCtx
   Parameters : Pointer to the input descriptor
 //  Return     : None.
==============================================================================
*/
void ss_RunFSM( InEvtDsc_t *FoundDsc  );


#endif // __SS_FSM_H__
