//------------------------------------------------------------------------------
//  $Log: sim_evt.h,v $
//  Revision 1.3  2006/05/22 20:49:15  aba
//  Memory optimization
//  Update of the copyright
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
  File       : sim_evt.h
--------------------------------------------------------------------------------

  Scope      : Declaration of all Events of sim Task

  History    :
--------------------------------------------------------------------------------
  Jun 06 05  |  ABA   | ISSUE 19: poll interval for simtoolkit
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __SIM_EVT_H__
#define __SIM_EVT_H__

#include "fsm.h"         // EvtDsc_t Type definition

#ifdef SIM_EVT
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
** Interface with L3/MMI
*/
DefExtern InEvtDsc_t  sim_ResetReq_Evt;
DefExtern InEvtDsc_t  sim_RunGsmAlgoReq_Evt;
DefExtern InEvtDsc_t  sim_ReadBinaryReq_Evt;
DefExtern InEvtDsc_t  sim_UpdateBinaryReq_Evt;
DefExtern InEvtDsc_t  sim_ReadRecordReq_Evt;
DefExtern InEvtDsc_t  sim_UpdateRecordReq_Evt;
DefExtern InEvtDsc_t  sim_InvalidateReq_Evt;
DefExtern InEvtDsc_t  sim_RehabilitateReq_Evt;
DefExtern InEvtDsc_t  sim_VerifyCHVReq_Evt;
DefExtern InEvtDsc_t  sim_ChangeCHVReq_Evt;
DefExtern InEvtDsc_t  sim_DisableCHVReq_Evt;
DefExtern InEvtDsc_t  sim_EnableCHVReq_Evt;
DefExtern InEvtDsc_t  sim_UnblockCHVReq_Evt;
DefExtern InEvtDsc_t  sim_SeekReq_Evt;
DefExtern InEvtDsc_t  sim_IncreaseReq_Evt;
DefExtern InEvtDsc_t  sim_DedicFileStatusReq_Evt;
DefExtern InEvtDsc_t  sim_ElemFileStatusReq_Evt;
DefExtern InEvtDsc_t  sim_TerminalProfileReq_Evt;
DefExtern InEvtDsc_t  sim_EnvelopeReq_Evt;
DefExtern InEvtDsc_t  sim_TerminalResponseReq_Evt;
DefExtern InEvtDsc_t  sim_StatusTimer_Evt;
DefExtern InEvtDsc_t  sim_StopReq_Evt;
DefExtern InEvtDsc_t  sim_PollReq_Evt;
DefExtern InEvtDsc_t  sim_PollOffReq_Evt;
DefExtern InEvtDsc_t  sim_SelectApplicationReq_Evt;
DefExtern InEvtDsc_t  sim_RunUmtsAlgoReq_Evt;
DefExtern InEvtDsc_t  sim_SetUsimPbkReq_Evt;
DefExtern InEvtDsc_t  sim_PowerOffReq_Evt;
DefExtern InEvtDsc_t  sim_RetryReq_Evt;
DefExtern InEvtDsc_t  sim_SeekAllReq_Evt;

/*
** General table of incoming events for SIM
*/
DefExtern    InEvtDsc_t  * const sim_EvtList[];

/*
==========================================
             Outgoing Events
==========================================
*/
/*
**  Outgoing SIM --> L3/MMI Events
*/


/*
** Prototypes
*/
DefExtern s16 sim_GiveEvtdx (u32 EvtId ) ;

#undef DefExtern
#endif  // __SIM_EVT_H__
