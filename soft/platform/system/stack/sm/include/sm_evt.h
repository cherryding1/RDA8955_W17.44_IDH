//------------------------------------------------------------------------------
//  $Log: sm_evt.h,v $
//  Revision 1.5  2006/05/23 14:50:57  aba
//  *** empty log message ***
//
//  Revision 1.4  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.3  2006/01/24 11:55:07  aba
//  minimum quality of service
//
//  Revision 1.2  2005/12/29 17:39:16  pca
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
  File       : sm_evt.h
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of sm

  History    :
--------------------------------------------------------------------------------
  Jun 24 03  |  PCA   | Creation
================================================================================
*/
#ifndef __SM_EVT_H__
#define __SM_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition

#ifdef SM_EVT
#define DefExtern
#else
#define DefExtern extern
#endif




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
** Incoming SM <-- API Events
*/
extern InEvtDsc_t  sm_apiCtxActivateReqEvt;
extern InEvtDsc_t  sm_apiInCtxActivateRejEvt;
extern InEvtDsc_t  sm_apiCtxDeactivateReqEvt;
extern InEvtDsc_t  sm_apiCtxModifyReqEvt;
extern InEvtDsc_t  sm_apiCtxModifyAccEvt;
extern InEvtDsc_t  sm_apiCtxModifyRejEvt;

#ifdef __MINQOS__
extern InEvtDsc_t  sm_apiSetMinQoSReqEvt;
#endif

/*
** Incoming SM <-- SNDCP  Events
*/
extern InEvtDsc_t sm_sndActivateRspEvt;
extern InEvtDsc_t sm_sndDeactivateRspEvt;
extern InEvtDsc_t sm_sndModifyRspEvt;
extern InEvtDsc_t sm_sndStatusReqEvt;

/*
** Incoming SM <-- MMP  Events
*/
extern InEvtDsc_t sm_mmpAbortIndEvt;
extern InEvtDsc_t sm_mmpDataIndEvt;
extern InEvtDsc_t sm_mmpRauAccPdpStatusIndEvt ; // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__

/*
** Internal event
*/
extern InEvtDsc_t sm_T3380Evt;
extern InEvtDsc_t sm_T3381Evt;
extern InEvtDsc_t sm_T3390Evt;

/*
** DERIVED EVENTS :
** These events are called by the routing functions of SM,
** hence they don't appear in sm_EvtList
*/
extern InEvtDsc_t sm_mmpReqCtxActivateEvt;
extern InEvtDsc_t sm_mmpCtxActivateAccEvt;
extern InEvtDsc_t sm_mmpCtxActivateRejEvt;
// InEvtDsc_t sm_mmpCtxActivateSecPdpAccept             = IEVT_DESCR(SM_ACTIVATESEC_ACC   , 0, 0);
// InEvtDsc_t sm_mmpCtxActivateSecPdpReject             = IEVT_DESCR(SM_ACTIVATESEC_REJ   , 0, 0);
extern InEvtDsc_t sm_mmpCtxModifyReqEvt;
extern InEvtDsc_t sm_mmpCtxModifyAccEvt;
extern InEvtDsc_t sm_mmpCtxModifyRejEvt;
extern InEvtDsc_t sm_mmpCtxDeactivateReqEvt;
extern InEvtDsc_t sm_mmpCtxDeactivateAccEvt;
extern InEvtDsc_t sm_mmpSmStatusIndEvt;

/*
** General table of incoming events for SM
*/
extern InEvtDsc_t  * const sm_EvtList[]             ;

/*
==========================================
             Outgoing Events
==========================================
*/

// SM => API
//DefExtern OutEvtDsc_t sm_apiPdpCtxRejEvt;
extern OutEvtDsc_t sm_apiCtxActivateCnfEvt;
extern OutEvtDsc_t sm_apiCtxSmIndexIndEvt;
extern OutEvtDsc_t sm_apiCtxActivateIndEvt;
extern OutEvtDsc_t sm_apiCtxDeactivateCnfEvt;
extern OutEvtDsc_t sm_apiCtxDeactivateIndEvt;
extern OutEvtDsc_t sm_apiOutCtxActivateRejEvt;
extern OutEvtDsc_t sm_apiCtxModifyCnfEvt;
extern OutEvtDsc_t sm_apiCtxModifyIndEvt;
extern OutEvtDsc_t sm_apiOutCtxModifyRejEvt;

// SM => SNDCP
extern OutEvtDsc_t sm_sndActivateIndEvt;

extern OutEvtDsc_t sm_sndDeactivateIndEvt;

extern OutEvtDsc_t sm_sndModifyIndEvt;
//extern OutEvtDsc_t sm_sndSequenceIndEvt;
//extern OutEvtDsc_t sm_sndStopAssignIndEvt;


// SM => GMM

extern OutEvtDsc_t sm_mmpDataReqEvt;
extern OutEvtDsc_t sm_mmpPdpCtxStatusIndEvt;
extern OutEvtDsc_t sm_mmpNoNetrspIndEvt;  // DMX add 20140625, PDP active req no respond, may need  MMP do RAU
extern OutEvtDsc_t sm_mmpDeleteSaveDataReqEvt ;  // DMX add 20161031.  MMP_DEL_SAVE_DATA_REQ

// Prototypes

DefExtern InEvtDsc_t * sm_GiveDsc ( u32 EvtId );
#undef DefExtern
#endif  // __SM_EVT_H__
