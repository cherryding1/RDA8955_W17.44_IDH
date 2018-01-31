//------------------------------------------------------------------------------
//  $Log: itf_llc.h,v $
//  Revision 1.2  2005/12/29 17:38:44  pca
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
  File       : ITF_LLC.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by the LLC entity
  History    :
--------------------------------------------------------------------------------
  Sep 29 04  |  FCH   | Remove Mode from llc_DataReq_t, add NPDUNb and
             |        | change order to optimize size 
  Sep 08 04  |  FCH   | Add llc_TriggerReq_t
  Sep 07 04  |  FCH   | Add the llc_ChangeLlcParReq
  Jul 29 04  |  FCH   | Add the interface by Queue for each SAPI for the DataReq
             |        | and the UnitDataRaq. Creation of the messages
             |        | LLC_QUEUECREATE_IND and LLC_QUEUEDATA_REQ.
  Mar 16 04  |  FCH   | Creation            
================================================================================
*/

#ifndef __ITF_LLC_H__
#define __ITF_LLC_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_msgc.h"



//============================================================================
// LLC Internal msg Start at INTRABASE + 0x8000
//============================================================================
#define LLC_INTRA_BASE        ( HVY_LLC + 0x8000 )



#define L3_TO_LLC       0
#define LLC_TO_L3       128

//============================================================================
// Primitive ID definitions for L3 => LLC direction
//============================================================================
#define LLC_TLLI_IND          ( HVY_LLC + L3_TO_LLC + 0  )
#define LLC_CIPHPARAM_IND     ( HVY_LLC + L3_TO_LLC + 1  )
#define LLC_TRIGGER_REQ       ( HVY_LLC + L3_TO_LLC + 2  )
#define LLC_SUSPEND_REQ       ( HVY_LLC + L3_TO_LLC + 3  )
#define LLC_RESUME_REQ        ( HVY_LLC + L3_TO_LLC + 4  )
#define LLC_ESTAB_REQ         ( HVY_LLC + L3_TO_LLC + 5  )
#define LLC_ESTAB_RSP         ( HVY_LLC + L3_TO_LLC + 6  )
#define LLC_RELEASE_REQ       ( HVY_LLC + L3_TO_LLC + 7  )
#define LLC_XID_REQ           ( HVY_LLC + L3_TO_LLC + 8  )
#define LLC_XID_RSP           ( HVY_LLC + L3_TO_LLC + 9  )
#define LLC_DATA_REQ          ( HVY_LLC + L3_TO_LLC + 10 )
#define LLC_UNITDATA_REQ      ( HVY_LLC + L3_TO_LLC + 11 )
#define LLC_QUEUEDATA_REQ     ( HVY_LLC + L3_TO_LLC + 12 + SXS_TIMER_EVT_ID )
#define LLC_CHANGELLCPAR_REQ  ( HVY_LLC + L3_TO_LLC + 13 )

//============================================================================
// Primitive ID definitions for LLC => L3 direction
//============================================================================
#define LLC_STATUS_IND        ( HVY_LLC + LLC_TO_L3 + 0  )
#define LLC_RESET_IND         ( HVY_LLC + LLC_TO_L3 + 1  )
#define LLC_ESTAB_IND         ( HVY_LLC + LLC_TO_L3 + 2  )
#define LLC_ESTAB_CNF         ( HVY_LLC + LLC_TO_L3 + 3  )
#define LLC_RELEASE_IND       ( HVY_LLC + LLC_TO_L3 + 4  )
#define LLC_RELEASE_CNF       ( HVY_LLC + LLC_TO_L3 + 5  )
#define LLC_XID_IND           ( HVY_LLC + LLC_TO_L3 + 6  )
#define LLC_XID_CNF           ( HVY_LLC + LLC_TO_L3 + 7  )
#define LLC_DATA_IND          ( HVY_LLC + LLC_TO_L3 + 8  )
#define LLC_UNITDATA_IND      ( HVY_LLC + LLC_TO_L3 + 9  )
#define LLC_DATA_CNF          ( HVY_LLC + LLC_TO_L3 + 10  )
#define LLC_NEW_TLLI_IND      ( HVY_LLC + LLC_TO_L3 + 11 )
#define LLC_QUEUECREATE_IND   ( HVY_LLC + LLC_TO_L3 + 12 )




#define LLC_SAPI_MMP    1
#define LLC_SAPI_DATA3  3
#define LLC_SAPI_DATA5  5
#define LLC_SAPI_SMS    7
#define LLC_SAPI_DATA9  9
#define LLC_SAPI_DATA11 11

#define LLC_SAPI_NB     6


/*
================================================================================
  Structure  : llc_TlliInd_t
--------------------------------------------------------------------------------
  Direction  : MMP -> LLC
  Message    : LLC_TLLI_IND
  Scope      : Assign, change, unassign TLLI
================================================================================
*/
typedef struct
{
  u32   NewTlli;    // New TLLI
  u32   OldTlli;    // Old TLLI
} llc_TlliInd_t;

// Possible values for NewTlli and OldTlli
// Valid values range from 0x70000000 to 0xFFFFFFFF
// Reserved values from 0x00000000 to 0x6FFFFFFF
// See 23.003 sec 2.4
#define LLC_TLLI_UNASSIGNED   0   // It is a reserved value


/*
================================================================================
  Structure  : llc_CiphParamInd_t
--------------------------------------------------------------------------------
  Direction  : MMP -> LLC
  Message    : LLC_CIPHPARAM_IND
  Scope      : Assign, change ciphering Key and algorithm
================================================================================
*/
#define LLC_KC_LENGTH   8   // 64 bits

typedef struct
{
  u8    Flags;
  u8    Kc[LLC_KC_LENGTH];  // Ciphering key
  u8    CiphAlgo;   // Ciphering algorithm or indication of ciphering off
} llc_CiphParamInd_t;


// Possible values for Flags
#define LLC_CIPH_NO_CHANGE  0
#define LLC_KC_CHANGE       ( 1 << 0 )
#define LLC_ALGO_CHANGE     ( 1 << 1 )


// Possible values for CiphAlgo
// Valid values range from 1 to 7 
// See 24.008 sec 10.5.5.3
#define LLC_CIPH_OFF          0
#define LLC_CIPH_ALGO_MAX     7


/*
================================================================================
  Structure  : llc_UnitDataReq_t
--------------------------------------------------------------------------------
  Direction  : L3 -> LLC
  Message    : LLC_UNITDATA_REQ
  Scope      : asks for the transmission of an unacknowledged L3 PDU
  Comment    : InterLayer message
================================================================================
*/
typedef struct 
{
  u8   Cause;         // Purpose of the L3 PDU
  u8   RadioPrio;     // Radio priority, value in [1..4].
                      // For MMP, the highest radio priority (1) shall be used (4.60 sec 7.1)
  u8   Throughput;    // From QoS IE (24.008).
                      // Peak troughput class, value in [1..9]
  u8   Pfi;           // Packet Flow Identifier (for RLC)
  u8   Reliability;   // From QoS IE (24.008).
                      // Define RLC mode and LLC protected/unprotected mode
                      // if Reliability == 1 to 3 => RLC acknowledge mode
                      // if Reliability == 4 or 5 => RLC unacknowledge mode
                      // if Reliability == 5      => LLC unprotected mode
  u8   Action;        // Cipher the frame, Start READY timer on frame sending, ...
  u8   Ref;           // Reference RLU must use with MMP to identify a message
                      // for the requested action

} llc_UnitDataReq_t;

// Possible values for Cause
// DO NOT CHANGE
// These values MUST MATCH the itf_rlc.h cause values 
// for the rlu_DataReq_t struct 
#define LLC_USER_DATA    0
#define LLC_PAGE_RSP     1
#define LLC_CELL_UPDATE  2
#define LLC_MM_PROCEDURE 3


// Possible values for Pfi
// DO NOT CHANGE
// must match the itf_rlc.h Pfi values
// other valid values range from 0 to 127
#define LLC_BEST_EFFORT_PFI 0x00
#define LLC_SIG_PFI         0x01
#define LLC_SMS_PFI         0x02
#define LLC_NO_PFI          0x80

// Possible values for Reliability
#define LLC_RELIABILITY_CLASS1    1   // RLC ack, LLC ack, protected mode
#define LLC_RELIABILITY_CLASS2    2   // RLC ack, LLC ack, protected mode
#define LLC_RELIABILITY_CLASS3    3   // RLC ack, LLC unack, protected mode
#define LLC_RELIABILITY_CLASS4    4   // RLC unack, LLC unack, protected mode
#define LLC_RELIABILITY_CLASS5    5   // RLC unack, LLC unack, unprotected mode

// Possible values for Radio priority
#define LLC_RADIO_LEV0    1   // Highest
#define LLC_RADIO_LEV1    2
#define LLC_RADIO_LEV2    3
#define LLC_RADIO_LEV3    4   // Lowest

// Possible values for Action (nearly aligned with PAction in rlu_dataReq_t)
#define LLC_CIPHER        ( 1 << 0 )
#define LLC_START_TREADY  ( 1 << 1 )
#define LLC_INFORM_ON_TX  ( 1 << 2 )
#define LLC_INFORM_ON_ACK ( 1 << 3 )



/*
================================================================================
  Structure  : llc_DataReq_t
--------------------------------------------------------------------------------
  Direction  : L3 -> LLC
  Message    : LLC_DATA_REQ
  Scope      : asks for the transmission of an acknowledged L3 PDU
  Comment    : InterLayer message
================================================================================
*/
typedef struct
{
  void  *UsrPduAdr;   // Address of the UserPdu the SNDCP Pdu is derived from.

  u16   NPDUNb;             // Reserved for SNDCP usage, used during routing area update inter sgsn
  u8    NSapi;              // Reserved for SNDCP usage,
  u8    NbSeg;              // Reserved for SNDCP usage, used to count the segment acknoledgement
  u8    RetransDataOffset;  // Reserved for SNDCP usage, used to restore the DataOffset during retransmission
  
  // Information about the I frame associated with the L3Pdu
  // Reserved for LLC usage
  u8    FrmStatus;
  u8    Retrans;
  u8    Pad;
  u16   Ns;
  u16   L3PduLen;
} llc_DataReq_t;


/*
================================================================================
  Structure  : llc_DataCnf_t
--------------------------------------------------------------------------------
  Direction  : LLC -> L3
  Message    : LLC_DATA_CNF
  Scope      : Confirm the delivery, to the peer, of all the SNDCP PDU belonging
               to a UserPdu
  Comment    : 
================================================================================
*/

typedef struct
{
  u8   Sapi;          // LLC SAPI where the PDU was transfered (keep at this place)
  void *UsrPduAdr;    // Address of the UserPdu for which all the corresponding
                      // SNDCP Pdu has reached the peer
} llc_DataCnf_t;



/*
================================================================================
  Structure  : llc_EstabReq_t
--------------------------------------------------------------------------------
  Direction  : L3 -> LLC
  Message    : LLC_ESTAB_REQ
  Scope      : Asks for the establishment of the ABM operation for the
               indicated LLC entity (Sapi) and give the parameters of the PDP
               context
  Comment    : InterLayer message
               May contain L3 Xid parameters in the radio part (if Offset != Len)
================================================================================
*/
typedef struct
{
  u8   Sapi;          // LLC SAPI where must be transfered (keep at this place)
  u8   RadioPrio;     // Radio priority, value in [0..3].
                      // For MMP, the highest radio priority (0) shall be used (4.60 sec 7.1)
  u8   Throughput;    // From QoS IE (24.008).
                      // Peak troughput class, value in [1..9]
  u8   Reliability;   // From QoS IE (24.008).
                      // Define RLC mode and LLC protected/unprotected mode
                      // if Reliability == 1 to 3 => RLC acknowledge mode
                      // if Reliability == 4 or 5 => RLC unacknowledge mode
                      // if Reliability == 5      => LLC unprotected mode
  u8   Pfi;           // Packet Flow Identifier (for RLC)
} llc_EstabReq_t;


/*
================================================================================
  Structure  : llc_EstabInd_t, llc_EstabCnf_t, llc_XidInd_t, llc_XidCnf_t
--------------------------------------------------------------------------------
  Direction  : LLC -> SNDCP
  Message    : LLC_ESTAB_IND, LLC_ESTAB_CNF, LLC_XID_IND, LLC_XID_CNF
  Scope      : Indicates establisment of the ABM operation
               Confirm establisment of the ABM operation
               Indicates new XID parameters
               Confirm XID parameters

  Comment    : InterLayer message
               May contain L3 Xid parameters in the radio part (if Offset != Len)
================================================================================
*/
typedef struct
{
  u8   Sapi;          // LLC SAPI concerned (keep at this place)
  u16  N201_U;        // Max number of bytes in an information field (frame U & UI): 140 to 1520
  u16  N201_I;        // Max number of bytes in an information field (frame I): 140 to 1520

} llc_EstabInd_t, llc_EstabCnf_t, llc_XidInd_t, llc_XidCnf_t;


/*
================================================================================
  Structure  : llc_EstabRsp_t, llc_XidReq_t, llc_XidRsp_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> LLC
  Message    : LLC_ESTAB_RSP, LLC_XID_REQ, LLC_XID_RSP
  Scope      : XID request from SNDCP
               Establisment response from SNDCP for ABM operation
               XID response from SNDCP to an XID command that include L3 param

  Comment    : InterLayer message
               May contain L3 Xid parameters in the radio part (if Offset != Len)
               Must contain L3 Xid parameters for LLC_XID_REQ
================================================================================
*/
typedef struct
{
  u8   Sapi;          // LLC SAPI concerned (keep at this place)
} llc_EstabRsp_t, llc_XidReq_t, llc_XidRsp_t;


/*
================================================================================
  Structure  : llc_DataInd_t
--------------------------------------------------------------------------------
  Direction  : LLC -> L3
  Message    : LLC_DATA_IND
  Scope      : Delivery of a L3-PDU received in an acknowledge or an
               unacknwoledge LLC frame 
  Comment    : InterLayer message
================================================================================
*/
typedef struct 
{
  u8   Sapi;           // LLC SAPI where data was received (usefull for SNDCP)
                       // (keep at this place)

} llc_DataInd_t, llc_UnitDataInd_t;


/*
================================================================================
  Structure  : llc_NewTlliInd_t
--------------------------------------------------------------------------------
  Direction  : LLC -> MMP
  Message    : LLC_NEW_TLLI_IND
  Scope      : Inform MMP that a frame with the new TLLI has been received
  Comment    : 
================================================================================
*/
typedef struct 
{
  u32   Tlli;    // TLLI

} llc_NewTlliInd_t;


/*
================================================================================
  Structure  : llc_StatusInd_t, llc_ReleaseInd_t
--------------------------------------------------------------------------------
  Direction  : LLC -> L3
  Message    : LLC_STATUS_IND or LLC_RELEASE_IND
  Scope      : Send a status to L3 (MMP or SNDCP) in case of failure, or a
               release indication to SNDCP
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8   Sapi;
  u8   Cause;
} llc_StatusInd_t, llc_ReleaseInd_t;


  // Status and Release causes                    Status    Release
#define LLC_NO_PEER_RSP               0x01    //    X         X
#define LLC_INVALID_XID_RSP           0x02    //    X         X
#define LLC_NORMAL_RELEASE            0x03    //              X
#define LLC_DM_RECEIVED               0X04    //              X
#define LLC_N200_RETRANS              0x05    //    X
#define LLC_POSSIBLE_MULTI_TLLI_ASS   0x06    //    X
#define LLC_REESTABLISHMENT_INITIATED 0x07    //    X               04.64 section 8.6.4
#define LLC_REESTABLISHMENT_DONE      0x08    //    X               04.64 section 8.7.2


/*
================================================================================
  Structure  : llc_ReleaseCnf_t
--------------------------------------------------------------------------------
  Direction  : LLC -> SNDCP
  Message    : LLC_RELEASE_CNF
  Scope      : Send a release confirmation to SNDCP
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8   Sapi;
} llc_ReleaseCnf_t;

/*
================================================================================
  Structure  : llc_ReleaseReq_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> LLC
  Message    : LLC_RELEASE_REQ
  Scope      : SNDCP sends a release request to LLC
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8    Sapi;
  bool  Local;    // TRUE if the release is local
} llc_ReleaseReq_t;


/*
================================================================================
  Structure  : llc_ResetInd_t
--------------------------------------------------------------------------------
  Direction  : LLC -> SNDCP
  Message    : LLC_RESET_IND
  Scope      : Inform SNDCP that a reset, from SGSN, has occured
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8   Pad;        // empty message

} llc_ResetInd_t;


/*
================================================================================
  Structure  : llc_QueueCreateInd_t
--------------------------------------------------------------------------------
  Direction  : LLC -> SNDCP
  Message    : LLC_QUEUECREATE_IND
  Scope      : LLC gives a Queue Identifier for a given SAPI to SNDCP.
               The queue is used to pass DataReq or UnitDataReq from SNDCP to
               LLC
               
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8    Sapi;
  u8    QueueId;
} llc_QueueCreateInd_t;


/*
================================================================================
  Structure  : llc_SuspendReq_t, llc_ResumeReq_t
--------------------------------------------------------------------------------
  Direction  : MMP -> LLC
  Message    : LLC_SUSPEND_REQ, LLC_RESUME_REQ
  Scope      : MMP order LLC to suspend/resume operation
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8    Pad;        // empty message
} llc_SuspendReq_t, llc_ResumeReq_t;



/*
================================================================================
  Structure  : llc_ChangeLlcParReq_t
--------------------------------------------------------------------------------
  Direction  : SNDCP -> LLC
  Message    : LLC_CHANGELLCPAR_REQ
  Scope      : MMP order LLC to suspend/resume operation
  Comment    : SNDCP to provide LLC with the new LLC parameters after pdp
               context modification initiated by the NW
               Not an interlayer message.
================================================================================
*/
typedef llc_EstabReq_t llc_ChangeLlcParReq_t;


/*
================================================================================
  Structure  : llc_TriggerReq_t
--------------------------------------------------------------------------------
  Direction  : MMP -> LLC
  Message    : LLC_TRIGGER_REQ
  Scope      : MMP order LLC to transmit any single frame
  Comment    : 
================================================================================
*/
typedef struct 
{
  u8    Cause;
  bool  NullFrmAllowed;
} llc_TriggerReq_t;

// Possible values for Cause : see llc_UnitDataReq_t
// DO NOT CHANGE
// These values MUST MATCH the itf_rlc.h cause values 
// for the rlu_DataReq_t struct 
//
// Only LLC_PAGE_RSP or LLC_CELL_UPDATE are used for TriggerReq

// Possible values for NullFrmAllowed
// TRUE if NULL frame shall be used (cell notification)


#endif  // __ITF_LLC_H__
