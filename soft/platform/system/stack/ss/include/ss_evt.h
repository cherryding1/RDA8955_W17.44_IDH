////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: ss_evt.h 
// 
// DESCRIPTION:
//   Declaration of all Events od ss Task
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
// W F | 20070529 | add ss_apiOActionInd  declaration
// DMX | 20070402 | add ss_USSDIReleaseComplete declaration  in Derived Events 
////////////////////////////////////////////////////////////////////////////////

#ifndef __SS_EVT_H__
#define __SS_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition

// Interface includes
#include "itf_msgc.h"
#include "itf_mm.h"
#include "itf_ss.h"

#include "itf_sim.h"

//==========================================
//             Incoming Events
//==========================================

//
// Flags for the incoming events
//
#define SS_MM_EVT  ( 1 << 0 )
#define SS_ALL     ( 1 << 1 )      /* Evt to be broadcasted to all events */

//
//  Internal Interface with MM
//
extern InEvtDsc_t ss_mmIReleaseInd;
extern InEvtDsc_t ss_mmIEstabCnf  ;
extern InEvtDsc_t ss_mmIEstabErr  ;
extern InEvtDsc_t ss_mmIServReq   ;
extern InEvtDsc_t ss_mmIDataInd   ;

//
//  Internal Interface with API
//
extern InEvtDsc_t ss_apiIActionReq;

#ifdef __SS_TIMER__
//
//  Timers expiration
//
extern InEvtDsc_t ss_Timer0Exp;
#endif

//
// Derived Events
//
extern InEvtDsc_t ss_networkIActionReq;
extern InEvtDsc_t ss_ComponentRcvd;    
extern InEvtDsc_t ss_USSDIReleaseComplete; // add by dingmx 20070402

void ss_ParseMessage( Msg_t *Msg );

//==========================================
//             Outgoing Events
//==========================================

//
// Flags for the outgoing events
//
#define SS_IL_MSG ( 1 << 0 )

//
// Interface with MM
//
extern OutEvtDsc_t ss_mmOEstabReq  ;
extern OutEvtDsc_t ss_mmOEstabInd  ;
extern OutEvtDsc_t ss_mmOReleaseReq;
extern OutEvtDsc_t ss_mmODataReq   ;

//
// Interface with API
//
extern OutEvtDsc_t ss_apiOActionCnf;
extern OutEvtDsc_t ss_apiOTIInd;
extern OutEvtDsc_t ss_apiOErrorInd;
extern OutEvtDsc_t ss_apiOActionInd; // add by wangf 20070529

#ifdef __SS_TIMER__
//
// Timers expiry
//
extern InEvtDsc_t ss_Timer0Exp;
#endif

#endif
