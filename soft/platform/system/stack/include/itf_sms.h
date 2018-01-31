//------------------------------------------------------------------------------
//  $Log: itf_sms.h,v $
//  Revision 1.3  2006/06/08 16:29:31  aba
//  Abort of the MMA procedure
//
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
  File       : ITF_SMS.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages provided by SMS
  History    :
--------------------------------------------------------------------------------
  Feb 09 04  |  ABA   | Creation
================================================================================
*/

#ifndef __ITF_SMS_H__
#define __ITF_SMS_H__

#include "sxs_type.h"
#include "cmn_defs.h"

/*
** SMS Internal msg Start at INTRABASE + 0
*/
#define SMS_INTRA_BASE ( HVY_SMS + 0x8000 )

/*
** Primitive ID definitions
*/
#define SMSPP_SEND_REQ                  ( HVY_SMS + 1  )
#define SMSPP_SEND_MMA_REQ              ( HVY_SMS + 2  )
#define SMSPP_REPORT_REQ                ( HVY_SMS + 3  )
#define SMSCB_ACTIVATE_REQ              ( HVY_SMS + 4  )
#define SMSCB_DEACTIVATE_REQ            ( HVY_SMS + 5  )
#define SMSPP_ACK_IND                   ( HVY_SMS + 6  )
#define SMSPP_ERROR_IND                 ( HVY_SMS + 7  )
#define SMSPP_RECEIVE_IND               ( HVY_SMS + 8  )
#define SMSCB_RECEIVE_IND               ( HVY_SMS + 9  )
#define SMSPP_ACK_REQ                   ( HVY_SMS + 10 )
#define SMSPP_ERROR_REQ                 ( HVY_SMS + 11 )
#define SMSPP_ABORT_MMA_REQ             ( HVY_SMS + 12 )
#define SMSPP_ABORT_REQ                 ( HVY_SMS + 13 )


// Possible values for Path
#define SMS_PATH_GSM  0
#define SMS_PATH_GPRS 1

/*
================================================================================
  Structure  : smspp_SendReq_t
--------------------------------
  Direction  : API -> SMS
  Scope      : API asks for the sending of a SMS
  Comment    : Inter layer message
================================================================================
*/
typedef struct
{
  u8 Path;        // 0:GSM  1:GPRS
  bool Concat;    // Used to keep the RR connection
  u8 AddressLen;  // Number of bytes of the address field
  u8 Address[11]; // Address used to build the RP header (SC address)
}smspp_SendReq_t;

/*
================================================================================
  Structure  : smspp_SendMMAReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SMS
  Scope      : API sends a notification indicating that the mobile has memory
               to receive one or more short message.
  Comment    : no parameter
================================================================================
*/
typedef struct
{
  u8 Path;         // 0:GSM  1:GPRS
}smspp_SendMMAReq_t;

/*
================================================================================
  Structure  : smspp_AckReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SMS
  Scope      : API sends an acknowledge report for the received sms
  Comment    : Inter layer message
================================================================================
*/
typedef struct
{
  u8 Path;         // 0:GSM  1:GPRS
}smspp_AckReq_t;

/*
================================================================================
  Structure  : smspp_ErrorReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SMS
  Scope      : API sends an error report for the received sms
  Comment    : Inter layer message
================================================================================
*/
typedef struct
{
  u8 Path;         // 0:GSM  1:GPRS
  u8 CauseLen;     // 1 (no diag) or 2 (diag is present)
  u8 Cause[2];
}smspp_ErrorReq_t;

/*
================================================================================
  Structure  : smspp_AbortMMAReq_t
--------------------------------------------------------------------------------
  Direction  : API -> SMS
  Scope      : Abort the MMA procedure
  Comment    : Inter layer message
================================================================================
*/
// No parameter



/*
================================================================================
  Structure  : smspp_AckInd_t
--------------------------------------------------------------------------------
  Direction  : SMS -> API
  Scope      : Receiving of a RP ACK from the network
  Comment    : Inter layer message
================================================================================
*/

/*
================================================================================
  Structure  : smspp_ErrorInd_t
--------------------------------------------------------------------------------
  Direction  : SMS -> API
  Scope      : Receiving of a RP error from the network
  Comment    : Inter layer message
================================================================================
*/

//    Error type
#define LOCAL_SMS_ERROR 0
#define PEER_SMR_ERROR  1
#define PEER_SMC_ERROR  2
//    Local error cause
#define MAX_CPDATA_RETRANS_CAUSE 0
#define MAX_RPDATA_RETRANS_CAUSE 1
#define MM_FAILURE_CAUSE         2
#define SMS_REQ_PARA_CAUSE 96

#define SMS_MM_OTHER_FALIURE     0
#define SMS_MM_COLLISION    1
#define SMS_MM_LOWLAYER_FAILURE 2
typedef struct
{
  u8  ErrorType;   // 0: Local error, 1: Peer Smr error, 2: Peer Smc error
  u8  CauseLen;    // Number of bytes of the cause field
  u8  Cause[2];    // Cause
} smspp_ErrorInd_t;

/*
================================================================================
  Structure  : smspp_ReceiveInd_t
--------------------------------------------------------------------------------
  Direction  : SMS -> API
  Scope      : Indication of a SMS PP receiving
  Comment    : Inter layer message
================================================================================
*/
typedef struct
{
  u8  AddressLen;  // Number of bytes of the address field
  u8  Address[11];
} smspp_ReceiveInd_t;



typedef struct
{
  u8 Path;         // 0:GSM  1:GPRS
}smspp_AbortReq_t;


/*
================================================================================
  Structure  : smscb_ReceiveInd_t
--------------------------------------------------------------------------------
  Direction  : SMS -> API
  Scope      : Indication of a SMS CB receiving
  Comment    : Inter layer message, no specific parameter
================================================================================
*/

#endif  // __ITF_SMS_H__
