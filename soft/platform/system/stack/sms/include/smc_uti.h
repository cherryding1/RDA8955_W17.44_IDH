// ++===========================================================================
//  File       : SMC_UTI.H
// -----------------------------------------------------------------------------
//  Scope      :
//  History    :
// -----------------------------------------------------------------------------
//  20051229 pca| Log keyword added to all files
//  Feb 17 04  |  ABA   | Creation
// =============================================================================

#ifndef __SMC_UTI_H__
#define __SMC_UTI_H__

#include "sxr_ops.h"
#include "cmn_defs.h"
#include "itf_mm.h"

// Macro used to fill the transaction identifier and the protocol descriminator
#define M_SET_PDTI(_PtSmcHead)                                               \
{                                                                            \
  if (gp_smsCurData->SmsDirection == SMS_MO)                                       \
  {                                                                          \
    *(_PtSmcHead) = ((gp_smsCurData->MOSmc.TI) << 4) | MM_SMS_PD;                  \
  }                                                                          \
  else if (gp_smsCurData->SmsDirection == SMS_MT)                                  \
  {                                                                          \
    *(_PtSmcHead) = 0x80 | ((gp_smsCurData->MTSmc.TI) << 4) | MM_SMS_PD;           \
  }                                                                          \
  else                                                                       \
    SXS_TRACE(SMS_OUT_TRC,"Sms direction not initialized");                   \
}

#define M_SET_CP_ERROR_CAUSE(_PtSmcHead,_Cause)                              \
{                                                                            \
  *(_PtSmcHead+2) = _Cause;                                                  \
}

// Macro used to fill the message type
#define TYPE_CP_DATA   0x01
#define TYPE_CP_ACK    0x04
#define TYPE_CP_ERROR  0x10
#define M_SET_MSG_TYPE(_PtSmcHead, _Type)                                    \
{                                                                            \
  *(_PtSmcHead+1) = _Type;                                                   \
}

#define M_SET_CP_DATA_LENGTH(_PtSmcHead, _Length)                            \
{                                                                            \
  *(_PtSmcHead+2) = _Length;                                                 \
}

#define M_GET_TI(_PtSmcHead) ( ((u8)(((*(_PtSmcHead))&0x70)))>>4)

#define M_GET_CAUSE_CP_ERROR(_PtSmcHead) (*(_PtSmcHead+2))


void smc_FillCpDataHeader(Msg_t* PtMsg);
void smc_SendCpAck(void);
void smc_SendCpError(u8 Cause);

#endif  // __SMC_UTI_H__
