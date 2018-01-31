//------------------------------------------------------------------------------
//  $Log: snd_evt.h,v $
//  Revision 1.5  2006/05/23 14:36:55  aba
//  Memory optimization
//  Update of the copyright
//
//  Revision 1.4  2006/02/23 10:59:20  aba
//  Additional commentaries
//
//  Revision 1.3  2006/01/27 21:04:21  aba
//  Change DefExtern into extern for the event definition
//
//  Revision 1.2  2005/12/29 17:39:17  pca
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
  File       : snd_evt.h
--------------------------------------------------------------------------------

  Scope      : Declaration of all events of sndcp

  History    :
--------------------------------------------------------------------------------
  Jun 24 03  |  ABA   | Creation
================================================================================
*/
#ifndef __SND_EVT_H__
#define __SND_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition


/*
==========================================
                   Flags
==========================================
*/
#define SND_IL_MSG      0x0001    // For interlayer messages

/*
==========================================
             Incoming Events
==========================================
*/
    // Internal event

    // Itf with API
extern InEvtDsc_t snd_apiQueueDataReqEvt     ;

    // Itf with SM
extern InEvtDsc_t snd_smActivateIndEvt       ;
extern InEvtDsc_t snd_smDeactivateIndEvt     ;
extern InEvtDsc_t snd_smModifyIndEvt         ;
extern InEvtDsc_t snd_smStopAssignIndEvt     ;

    // Itf with LLC
//DefExtern InEvtDsc_t snd_llcStatusIndEvt        ;
extern InEvtDsc_t snd_llcResetIndEvt         ;
extern InEvtDsc_t snd_llcEstabIndEvt         ;
extern InEvtDsc_t snd_llcEstabCnfEvt         ;
extern InEvtDsc_t snd_llcReleaseIndEvt       ;
extern InEvtDsc_t snd_llcReleaseCnfEvt       ;
extern InEvtDsc_t snd_llcXidIndEvt           ;
extern InEvtDsc_t snd_llcXidCnfEvt           ;
extern InEvtDsc_t snd_llcDataIndEvt          ;
extern InEvtDsc_t snd_llcUnitDataIndEvt      ;
extern InEvtDsc_t snd_llcQueueCreateIndEvt   ;
extern InEvtDsc_t snd_llcDataCnfEvt          ;

    // Itf with MMP
extern InEvtDsc_t snd_mmpSequenceIndEvt      ;

    // Derived events
extern InEvtDsc_t  snd_sndUnitDataReqEvt     ;
extern InEvtDsc_t  snd_sndDataReqEvt         ;

/*
** General table of incoming events for SNDCP
*/
extern InEvtDsc_t  * const snd_EvtList[]            ;

/*
==========================================
             Outgoing Events
==========================================
*/
/*
**  Outgoing SMC --> MM Events
*/
// SNDCP => LLC
extern OutEvtDsc_t snd_llcEstabReqEvt        ;
extern OutEvtDsc_t snd_llcEstabRspEvt        ;
extern OutEvtDsc_t snd_llcReleaseReqEvt      ;
extern OutEvtDsc_t snd_llcXidReqEvt          ;
extern OutEvtDsc_t snd_llcXidRspEvt          ;
extern OutEvtDsc_t snd_llcDataReqEvt         ;
extern OutEvtDsc_t snd_llcUnitDataReqEvt     ;
extern OutEvtDsc_t snd_llcChangeLlcParReqEvt ;

// SNDCP => SM
extern OutEvtDsc_t snd_smActivateRspEvt      ;
extern OutEvtDsc_t snd_smDeactivateRspEvt    ;
extern OutEvtDsc_t snd_smModifyRspEvt        ;
extern OutEvtDsc_t snd_smStatusReqEvt        ;

// SNDCP => API
extern OutEvtDsc_t snd_apiDataIndEvt         ;
extern OutEvtDsc_t snd_apiQueueCreateIndEvt  ;

// SNDCP => MMP
extern OutEvtDsc_t snd_mmpSequenceRspEvt     ;

/*
** Prototypes
*/
extern s16 snd_GiveEvtdx (u32 EvtId )        ;

#endif  // __SND_EVT_H__
