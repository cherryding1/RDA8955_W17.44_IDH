//------------------------------------------------------------------------------
//  $Log: lap_evt.h,v $
//  Revision 1.4  2006/05/22 12:45:21  aba
//  Memory optimization
//
//  Revision 1.3  2006/01/24 18:18:36  aba
//  Changing of DefExtern into extern for the events definition
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
  File       : lap_evt.h
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of lap Task

  History    :
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __LAP_EVT_H__
#define __LAP_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition

/*
==========================================
                   Flags
==========================================
*/


/*
==========================================
             Incoming Events
==========================================
*/
/*
**  Incoming RR <-- RR Events
*/

/*
** Interface with RR  (LAP <- RR)
*/
extern InEvtDsc_t  lap_rrEstablishReqEvt;
extern InEvtDsc_t  lap_rrSuspendReqEvt;
extern InEvtDsc_t  lap_rrResumeReqEvt;
extern InEvtDsc_t  lap_rrReconnectReqEvt;
extern InEvtDsc_t  lap_rrNormalReleaseReqEvt;
extern InEvtDsc_t  lap_rrLocalReleaseReqEvt;
extern InEvtDsc_t  lap_rrDataReqEvt;
extern InEvtDsc_t  lap_rrUnitDataReqEvt;

/*
** Interface with L1  (LAP <- L1)
*/
extern InEvtDsc_t  lap_l1DataIndEvt;
extern InEvtDsc_t  lap_l1TxIndEvt;

/*
** Internal event
*/
extern InEvtDsc_t  lap_lapSapi0T200Evt;
extern InEvtDsc_t  lap_lapSapi3T200Evt;
extern InEvtDsc_t  lap_lapSapi0LastT200Evt;
extern InEvtDsc_t  lap_lapSapi3LastT200Evt;

/*
** DERIVED EVENTS :
** These events are called by the routing functions of LAP,
** hence they don't appear in lap_EvtList
*/
extern InEvtDsc_t  lap_RxUaFrameEvt     ;
extern InEvtDsc_t  lap_RxDmFrameEvt     ;
extern InEvtDsc_t  lap_RxDiscFrameEvt   ;
extern InEvtDsc_t  lap_RxRrFrameEvt     ;
extern InEvtDsc_t  lap_RxRejFrameEvt    ;
extern InEvtDsc_t  lap_RxSabmFrameEvt   ;
extern InEvtDsc_t  lap_RxIFrameEvt      ;
extern InEvtDsc_t  lap_RxUiFrameEvt     ;

/*
** General table of incoming events for LAP
*/
extern    InEvtDsc_t  * const lap_EvtList[];

/*
==========================================
             Outgoing Events
==========================================
*/
/*
**  Outgoing LAP --> RR Events
*/
extern OutEvtDsc_t  lap_rrEstablishCnfEvt                ;
extern OutEvtDsc_t  lap_rrEstablishFailureIndEvt         ;
extern OutEvtDsc_t  lap_rrEstablishIndEvt                ;
extern OutEvtDsc_t  lap_rrResumeCnfEvt                   ;
extern OutEvtDsc_t  lap_rrResumeFailureIndEvt            ;
extern OutEvtDsc_t  lap_rrReconnectCnfEvt                ;
extern OutEvtDsc_t  lap_rrReconnectFailureIndEvt         ;
extern OutEvtDsc_t  lap_rrReleaseIndEvt                  ;
extern OutEvtDsc_t  lap_rrDataIndEvt                     ;
extern OutEvtDsc_t  lap_rrUnitDataIndEvt                 ;
extern OutEvtDsc_t  lap_l1DataReqEvt                     ;
extern OutEvtDsc_t  lap_rrNormalReleaseCnfEvt            ;
extern OutEvtDsc_t  lap_rrNormalReleaseFailureIndEvt     ;

/*
** Prototypes
*/
extern s16 lap_GiveEvtdx (u32 EvtId ) ;

#endif  // __LAP_EVT_H__
