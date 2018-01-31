//------------------------------------------------------------------------------
//  $Log: llc_evt.h,v $
//  Revision 1.5  2006/06/26 12:05:34  fch
//  Now LLC suspend and resume RLC when MMP orders. If LLC receives a XID frame with reset and/or IOV_UI, it sends a RLU_PURGE_REQ to RLU to request purge of PDU (with cause USER_DATA) that are in RLU queue. (Mantis #449)
//
//  Revision 1.4  2006/05/23 10:36:46  fch
//  Update Copyright
//
//  Revision 1.3  2006/05/18 18:20:01  fch
//  Change llc_EvtList and llc_DataIndEvtList definition to set them as const
//
//  Revision 1.2  2005/12/29 17:38:46  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : LLC_EVT.H
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of LLC Task

  History    :
--------------------------------------------------------------------------------
  Mar 22 04  |  FCH   | Creation
================================================================================
*/
#ifndef __LLC_EVT_H__
#define __LLC_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition
#include "llc_frm.h"


extern InEvtDsc_t *const llc_DataIndEvtList [LLC_NB_FRM_TYP];


      //-------------------------------------------------------
      //  Flags for the events
      //-------------------------------------------------------
#define LLC_NO_FLAG     0
#define LLC_IL_MSG      0x0001    // For interlayer messages
#define LLC_USR_SAPI    0x0002    // To check that the Sapi belongs to Usr Sapi


//==========================================
//             Incoming Events
//==========================================

/*
** Incomming events from RLC
*/
extern InEvtDsc_t  llc_RldDataIndEvt;
extern InEvtDsc_t  llc_RluQueueCreateIndEvt;


/*
** Incomming events from MMP
*/
extern InEvtDsc_t  llc_MmpTlliIndEvt;
extern InEvtDsc_t  llc_MmpCiphParamIndEvt;
extern InEvtDsc_t  llc_MmpSuspendReqEvt;
extern InEvtDsc_t  llc_MmpResumeReqEvt;
extern InEvtDsc_t  llc_MmpTriggerReqEvt;

/*
** Incomming events from SNDCP
*/
extern InEvtDsc_t  llc_SndcpEstReqEvt;
extern InEvtDsc_t  llc_SndcpEstRspEvt;
extern InEvtDsc_t  llc_SndcpXidReqEvt;
extern InEvtDsc_t  llc_SndcpXidRspEvt;
extern InEvtDsc_t  llc_SndcpRelReqEvt;
extern InEvtDsc_t  llc_SndcpChangeLlcParReqEvt;

/*
** Incomming events from L3 (MMP, SNDCP, SMS)
*/
extern InEvtDsc_t  llc_L3QueueDataReqEvt;

/*
** TIMERS
*/
extern InEvtDsc_t  llc_llcT200ExpEvt;
extern InEvtDsc_t  llc_llcT201ExpEvt;


/*
** DERIVED EVENTS :
*/
extern InEvtDsc_t  llc_MmpUnassignReqEvt;
extern InEvtDsc_t  llc_XidResetSlaveEvt;
extern InEvtDsc_t llc_NullFrameEvt;

extern InEvtDsc_t  llc_RxIFrameEvt;

extern InEvtDsc_t  llc_RxSFrameEvt;

extern InEvtDsc_t  llc_RxUIFrameEvt;

extern InEvtDsc_t  llc_RxU_DMFrameEvt;
extern InEvtDsc_t  llc_RxU_DISCFrameEvt;
extern InEvtDsc_t  llc_RxU_UAFrameEvt;
extern InEvtDsc_t  llc_RxU_SABMFrameEvt;
extern InEvtDsc_t  llc_RxU_FRMRFrameEvt;
extern InEvtDsc_t  llc_RxU_XIDFrameEvt;
extern InEvtDsc_t  llc_RxU_UNDEFFrameEvt;



//==========================================
//             Outgoing Events
//==========================================

/*
** Outgoing events to RLU
*/
extern OutEvtDsc_t  llc_rluDataReqEvt;
extern OutEvtDsc_t  llc_rluSuspendReqEvt;
extern OutEvtDsc_t  llc_rluResumeReqEvt;
extern OutEvtDsc_t  llc_rluPurgeReqEvt;


/*
** Outgoing events to MAC
*/
extern OutEvtDsc_t  llc_macNewTlliCnfEvt;


/*
** Outgoing events to L3 (MMP & SNDCP)
*/
extern OutEvtDsc_t  llc_llcDataIndEvt;
extern OutEvtDsc_t  llc_llcUnitDataIndEvt;
extern OutEvtDsc_t  llc_llcStatusIndEvt;



/*
** Outgoing events to MMP
*/
extern OutEvtDsc_t  llc_llcNewTlliIndEvt;

#ifdef __TEMP_FLOW_CONTROL__
extern OutEvtDsc_t    llc_FlowCtrlEvt;
#endif
/*
** Outgoing events to SNDCP
*/
extern OutEvtDsc_t  llc_llcReleaseIndEvt;
extern OutEvtDsc_t  llc_llcReleaseCnfEvt;
extern OutEvtDsc_t  llc_llcEstabIndEvt;
extern OutEvtDsc_t  llc_llcEstabCnfEvt;
extern OutEvtDsc_t  llc_llcXidIndEvt;
extern OutEvtDsc_t  llc_llcXidCnfEvt;
extern OutEvtDsc_t  llc_llcResetIndEvt;
extern OutEvtDsc_t  llc_llcDataCnfEvt;
extern OutEvtDsc_t  llc_llcQueueCreateIndEvt;

#endif  // __LLC_EVT_H__
