//------------------------------------------------------------------------------
//  $Log: cb_evt.h,v $
//  Revision 1.3  2006/05/23 10:36:37  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:40  pca
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

  File       : CB_EVT.H
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of CB Task

  History    :
--------------------------------------------------------------------------------
  Dec 07 04  |  FCH   | Creation
================================================================================
*/
#ifndef __CB_EVT_H__
#define __CB_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition


      //-------------------------------------------------------
      //  Flags for the events
      //-------------------------------------------------------
#define CB_NO_FLAG     0
#define CB_IL_MSG      0x0001    // For interlayer messages




//==========================================
//             Incoming Events
//==========================================

/*
** Incomming events from API
*/
extern InEvtDsc_t cb_ApiActivateReq;
extern InEvtDsc_t cb_ApiDeactivateReq;


/*
** Incomming events from RR
*/
extern InEvtDsc_t cb_RrCellInd;


/*
** Incomming events from L1
*/
extern InEvtDsc_t cb_L1DataInd;




//==========================================
//             Outgoing Events
//==========================================

/*
** Outgoing events to API
*/
extern OutEvtDsc_t cb_CbMsgRecInd;
extern OutEvtDsc_t cb_CbPageRecInd;


/*
** Outgoing events to RR
*/
extern OutEvtDsc_t cb_RrCbchReq;


/*
** Outgoing events to L1
*/
extern OutEvtDsc_t cb_L1CbchSkipReq;




#endif  // __CB_EVT_H__
