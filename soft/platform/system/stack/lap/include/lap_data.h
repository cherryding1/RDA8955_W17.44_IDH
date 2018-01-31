//------------------------------------------------------------------------------
//  $Log: lap_data.h,v $
//  Revision 1.3  2006/05/22 12:46:12  aba
//  Copyrigth is updated
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
  File       : lap_data.h
--------------------------------------------------------------------------------

  Scope      : Declaration of LAP context variables

  *--------------------------------------------------------------------------*
  |    Lap(multi-instance)  =      1 task                                    |
  |                             +  2 FSM                                     |
  |                             +  1 mailbox                                 |
  |                             +  2 Data contexts                           |
  |                             +  1 global variable to select the instance  |
  *--------------------------------------------------------------------------*

  History    :
--------------------------------------------------------------------------------
  Sep 28 03  |  ABA   | Creation
================================================================================
*/
#ifndef __LAP_DATA_H__
#define __LAP_DATA_H__

#include "fsm.h"
#include "sxr_cnf.h"


#include "lap_msg.h"

#ifndef STK_SINGLE_SIM
#include "cmn_dec.h"
#endif
#ifdef __LAP_DATA__
#define DefExtern
#else
#define DefExtern extern
#endif

// Check L3 message
#define __LAP_DBG_RR_MSG__
// Debug internal lap processing
#define __LAP_DBG__

// Lap context (all global of the lap)
typedef struct
{
#ifndef STK_SINGLE_SIM
  u32           SimIndex;     // new added for dual SIM dupport        
#endif
  Msg_t       * RecMsg               ;   // ptr to the current received message; NIL if evt
  Msg_t       * OutMsg               ;
  InEvtDsc_t  * IEvtDsc              ;   // Incoming event
  OutEvtDsc_t * OEvtDsc              ;   // Outgoing event
  bool          KeepMsg              ;   // TRUE if RecMsg must not be deleted after fsm_Run
  u8            DataReqQueue         ;
  u8            RetransmissionCounter;
  u8            EstabData[20]        ;   // Used for contention resolution
                                         // on pourrait économiser cette mémoire (en plus ca sert pas ?sapi3
  u8            EstabDataLength      ;   // Used for contention resolution
  u8            Chan                 ;   // Channel type : L3 indicates the current channel
                                         // by the message lap_EstablisReq, lap_ResumeReq (and
                                         // implicitly by lap_ReconnectReq)
  u8            VR                   ;
  u8            VS                   ;
  u8            VA                   ;
  Msg_t         *SegmentedLapDataReqInProcess ;
                                         // NULL => no segmented Lap_DataReq in process
                                         // set to null after the acknowledge of the last segment
  u16           LapDataReqNbTxByte   ;   // Counter for segmentation
  bool          LapDataReqTxComplete ;   // Indicate that all segments are sent
                                         // but don't indicate that last segment is acknowledged
  Msg_t         *ConcatMsg           ;   // Pointer on the message used to concatenate
                                         // the received I frame.
  u8            ReconnectChan        ;   // Storage of the channel at the resume
  bool          LastT200             ;   // Indicate the last retransmission
  Msg_t         *PtT200Frame         ;   // Frame pointer used for retransmission
  bool          SendRRMsg            ;   // Used to delay the RR message sending on L1_TX_IND event
  bool          BadFrame             ;
  bool          LastRxIFrameAck      ;   // Used for the sending of reject
  Msg_t         *DelayedMsg          ;   // Used to delay the RR message sending on L1_TX_IND event
} lap_Data_t ;


#ifndef STK_SINGLE_SIM
DefExtern lap_Data_t lap_Data[STK_MAX_SIM][2];        
#else
DefExtern lap_Data_t lap_Data[2];        // for Sapi0 and Sapi3
#endif

DefExtern lap_Data_t* pLapCtx;        // for Sapi0 and Sapi3

// Selection of the lap, this variable is is changed at each event or message
// receiving to select the fsm and lap context
DefExtern u8 lap_SapiIdx;   // 0 => Sapi0, 1 => Sapi3

/*
** LAP Finite State Machines
*/
#ifndef STK_SINGLE_SIM
extern Fsm_t lap_Fsm[STK_MAX_SIM][2];        
#else
extern Fsm_t lap_Fsm[2] ;
#endif
#undef DefExtern

#endif // __LAP_DATA_H__
