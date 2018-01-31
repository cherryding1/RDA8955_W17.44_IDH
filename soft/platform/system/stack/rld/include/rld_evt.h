//------------------------------------------------------------------------------
//  $Log: rld_evt.h,v $
//  Revision 1.2  2005/12/29 17:39:12  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : rld_evt.h  
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events od rld Task               

  History    :
--------------------------------------------------------------------------------
  Mar 04 04  |  OTH   | Creation            
================================================================================
*/

#ifndef __RLD_EVT_H__
#define __RLD_EVT_H__


/*
==========================================
             Incoming Events
==========================================
*/

/*
**  Incoming RRP -> RLD events
*/
extern InEvtDsc_t rld_ConIndEvt  ;
extern InEvtDsc_t rld_DataIndEvt ;
extern InEvtDsc_t rld_RelIndEvt  ;
#ifdef __EGPRS__
/*#Added by PENGQIANG 2007.3.19 for testmode begin.*/
extern InEvtDsc_t rld_LoopBackCmdEvt ;
/*#Added by PENGQIANG 2007.3.19 for testmode end.*/
#endif
/*
==========================================
             Outgoing Events
==========================================
*/

/*
**  Outgoing  RRI --> RRI Events
*/
extern OutEvtDsc_t rld_ODataIndEvt ;

/*
**  Outgoing  RLD --> RRP Events
*/
extern OutEvtDsc_t rld_OAckReqEvt ;
extern OutEvtDsc_t 	rld_OTestDatareqEvt;

#endif
