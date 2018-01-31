//==============================================================================
//  File       : sms_out.h
//------------------------------------------------------------------------------
//  Scope      : Prototypes for the sms_out.c utilities
//  History    :
//------------------------------------------------------------------------------
//  20051229  | PCA   | Log keyword added to all files
//  Sep 02 03  | ABA    | Creation
//==============================================================================

#ifndef __SMS_OUT_H__
#define __SMS_OUT_H__

// SMS includes
#include "sms_msg.h"

#define SMS_HEAD_MAX_SIZE 20

void sms_SendMsg ( u8 Mbx ) ;
void sms_PrepMsg (OutEvtDsc_t * Evt, bool IsIL ) ;

// dmx add to support dual sim
void sms_StartTimer(u32 Period, u32 Id, void *Data, u8 ReStart, u8 Mbx);
u8 sms_StopTimer(u32 Id, void *Data, u8 Mbx);


#endif // __SMS_OUT_H__
