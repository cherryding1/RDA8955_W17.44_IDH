//------------------------------------------------------------------------------
//  $Log: smr_uti.h,v $
//  Revision 1.3  2006/05/22 07:54:20  aba
//  Copyrigth is updated
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------

/*
================================================================================
  File       : SMR_UTI.H
--------------------------------------------------------------------------------

  Scope      :

  History    :
--------------------------------------------------------------------------------
  Feb 17 04  |  ABA   | Creation
================================================================================
*/

#ifndef __SMR_UTI_H__
#define __SMR_UTI_H__

#include "sxr_ops.h"
#include "cmn_defs.h"

void smr_FillRpDataHeader(Msg_t* PtMsg);
void smr_FillRpSMMAHeader(Msg_t* PtMsg);
void smr_FillRpAckHeader(Msg_t* PtMsg);
void smr_FillRpErrorHeader(Msg_t* PtMsg, u8 CauseLen,  u8 Cause, u8 Diag, u8 MessageRef);
void smr_SendSmsPPAckInd(Msg_t* PtMsg);
void smr_SendSmsPPErrorInd(Msg_t *PtMsg);
void smr_SendRpError(u8 MessageRef,u8 Cause);
bool smr_TestTemporyFailureIndication(u8* PtSmrHead);
bool smr_CheckTPDU(Msg_t* PtMsg);


#define MTI_TX_RP_DATA    0x0
#define MTI_TX_RP_ACK     0x2
#define MTI_TX_RP_ERROR   0x4
#define MTI_TX_RP_SMMA    0x6
#define MTI_RX_RP_DATA    0x1
#define MTI_RX_RP_ACK     0x3
#define MTI_RX_RP_ERROR   0x5

#endif  // __SMR_UTI_H__
