//------------------------------------------------------------------------------
//  $Log: snd_data.h,v $
//  Revision 1.4  2006/05/23 14:37:18  aba
//  Update of the copyright
//
//  Revision 1.3  2006/02/23 10:59:20  aba
//  Additional commentaries
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
  File       : snd_data.h
--------------------------------------------------------------------------------

  Scope      : Declaration of SNDCP context variables

  History    :
--------------------------------------------------------------------------------
  Jul 14 04  |  ABA   | Creation
================================================================================
*/
#ifndef __SND_DATA_H__
#define __SND_DATA_H__

#include "fsm.h"
#include "snd_msg.h"

// State of AckReassembAuto and UnackReassembAuto :
#define NEXT_SEG_FIRST    0
#define NEXT_SEG_SUBS     1
#define NEXT_SEG_DISCARD  2

// Values of ProcType
#define MODIFY_ACK_To_UNACK         0x80
#define MODIFY_UNACK_TO_ACK         0x81
#define MODIFY_ACK_TO_ACK_NEW_SAPI  0x82
#define MODIFY_ACK_TO_ACK_SAME_SAPI 0x83
#define MODIFY_IND                  0x80
#define DEACTIVATE_IND              0x04
#define ACTIVATE_IND                0x05

#define __SND_UNACK_ENHANCE__
// with __SND_UNACK_ENHANCE__, the reveived segment will not change to remove the snd head,
// untill to Reassemble, this is different with oridin design
// we can buffer one more SND pdu ,and the segmeng of same PDU will not with fixed order
// NSAPI context (all global of the NSAPI)
// not using UnackReassembAuto to limit the segment order,

#define MAX_SND_PDU_SEG 16
#define MAX_SND_UACK_PDU_NUM 0xFFF
typedef struct
{
  u8     AckTxNPDUNb          ;  // Send NPDU Number for acknowledged LLC
  u8     CurrentAckRxNPDUNb   ;  // Receive NPDU Number for acknowledged LLC
                                 // used for:
                                 // - exception case in function snd_Connected_DataInd
                                 // - routing area update inter sgsn (sent in llc_SequenceRsp)
                                 // - recovery state
  u16    UnackTxNPDUNb        ;  // Send NPDU Number for unacknowledged LLC (12 bits)
  u16    UnackFirstSegRxNPDUNb;  // RX NPDU Number for unacknowledged LLC (12 bits)
                                 // (only used to discard bad segments)
  u16    SegNbBitmap          ;  // Acknowledged segment (15 segments max)
  u8     UnackComp            ;  // (DCOMP<<4) | PCOMP
                                 // DCOMP : Data compression coding (0 if no compression)
                                 // PCOMP : Protocol information comprssion coding (0 if no compression)
  u8     AckComp              ;
  u8     SapiIdx              ;  // Associated sapi 0 => Sapi3, 1 => Sapi5, 2 => Sapi9, 3 => Sapi11
  #ifndef __SND_UNACK_ENHANCE__  
  u8     UnackReassembAuto    ;  // Automaton for the receiving of unacknowledged segments
  #endif
  u8     AckReassembAuto      ;  // Automaton for the receiving of acknowledged segments
  Msg_t* PtRxUnackSeg[MAX_SND_PDU_SEG]     ;  // Storage of the RX segments in unack mode
  #ifdef __SND_UNACK_ENHANCE__  
  Msg_t* PtRxUnackSeg_NextSndPdu[MAX_SND_PDU_SEG]     ;  // Storage of the RX segments in unack mode
  #endif
  Msg_t* PtRxAckSeg[16]       ;  // Storage of the RX segments in ack mode
  u8     AckSegIdx            ;  // Index for the storage of RX segments in ack mode
  u8     DataReqQueueId       ;  // Identifier of the queue used to store the
                                 // SND_DATA_REQ message waiting for acknowledgment
  bool   RecoveryState        ;  // TRUE => recovery state

  u8     RadioPrio            ;  // Radio priority, value in [0..3].
                                 // For MMP, the highest radio priority (0) shall be used (4.60 sec 7.1)
  u8     Throughput           ;  // From QoS IE (24.008).
                                 // Peak troughput class, value in [1..9]
  u8     Pfi                  ;  // Packet Flow Identifier (for RLC)
  u8     Reliability          ;  // From QoS IE (24.008).
                                 // Define RLC mode and LLC protected/unprotected mode
                                 // if Reliability == 1 to 3 => RLC acknowledge mode
                                 // if Reliability == 4 or 5 => RLC unacknowledge mode
                                 // if Reliability == 5      => LLC unprotected mode
  u8     ProcType             ;  // Procedure type
  u8     UnitDataReqQueueId   ;  // Queue used to store the SND_UNITDATA_REQ message when
                                 // the transmission is suspended on the associated sapi
  u8     RAUInter             ;  // Indicates that the routing area update procedure is running, used
                                 // to stop the transmission
  bool   RAUInterRxSequenceInd;  // Indicates if a sm_sequence_ind has been received during a routing area
                                 // update inter sgsn
  bool   RAUInterRxEstabInd;     // indicates if a llc_estab_ind has been received during a routing area
                                 // update inter sgsn
  u8     RAUSequenceIndNDPUNb ;  // Memorization of the npdu of the sm_sequence_ind if llc_estab_ind has
                                 // not been received

} snd_NSAPI_t;

// SAPI context (all global of the SAPI)
#define SAPI_IDLE_STATE            0
#define SAPI_ESTABLISHED_STATE     1
#define SAPI_ESTABLISHING_STATE    2
#define SAPI_RELEASING_STATE       3

#define MAX_NSAPI_NB                     11

typedef struct
{
  u8     State               ;  // State of the SAPI (see values above)
  u16    N201_U              ;  // Max number of bytes in an unacknowledged PDU: 140 to 1520
  u16    N201_I              ;  // Max number of bytes in an acknowledged PDU: 140 to 1520
  u8     LlcQueueId          ;  // Queue for LLC_DATA_REQ and LLC_UNITDATA_REQ
} snd_SAPI_t;

// SNDCP context (all global of SNDCP)
typedef struct
{

#if !(STK_MAX_SIM==1)
  u32		   nCurSimId           ;	//Current used SimId;
#endif

  Msg_t        * RecMsg      ; // Received message
  Msg_t        * OutMsg      ; // Message to send

  Fsm_t        * Fsm         ; // Current selected Fsm
  InEvtDsc_t   * IEvtDsc     ; // Incoming Event
  OutEvtDsc_t  * OEvtDsc     ; // Outgoing Event

  bool           KeepMsg     ; // TRUE if RecMsg must not be deleted

  snd_NSAPI_t   *NSAPI[MAX_NSAPI_NB]   ; // NSAPI context
  snd_SAPI_t     SAPI[4]     ; // All globals of SAPI
  u8             NSapiIdx    ; // 0 => NSAPI5, 1 => NSAPI6
  u8             SndQueueId  ; // Identifier of the queue used  by api to send
                               // SND_SEND_REQ message
} snd_Data_t;

#ifdef __SND_TRS_C__
#define DefExtern
#else
#define DefExtern extern
#endif


 // Added by Akui 20090312 for 221 begin.

#if !(STK_MAX_SIM==1)
DefExtern snd_Data_t  snd_Data[STK_MAX_SIM];
#else
 // Added by Akui 20090312 for 221 end.
 
DefExtern snd_Data_t  snd_Data  ;
#endif

//Added by Akui 20090312 for 221
DefExtern snd_Data_t *pSndCtx;

/*
** SNDCP Finite State Machines
*/
#if !(STK_MAX_SIM==1)
extern Fsm_t snd_Fsm[STK_MAX_SIM][MAX_NSAPI_NB];
#else
extern Fsm_t snd_Fsm[MAX_NSAPI_NB] ; // One per NSAPI
#endif

#undef DefExtern
#endif // __SND_DATA_H__
