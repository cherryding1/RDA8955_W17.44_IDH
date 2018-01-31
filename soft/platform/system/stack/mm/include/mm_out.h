//================================================================================
//  File       : mm_out.h
//--------------------------------------------------------------------------------
//  Scope      : Header file for mm_out.c
//
//  History    :
//--------------------------------------------------------------------------------
//  Aug 29 03  |  OTH   | Creation            
//================================================================================

#ifndef __MM_OUT_H__
#define __MM_OUT_H__

// System includes
#include "sxs_type.h"
#include "fsm.h"

void  mmc_PrepMsg (OutEvtDsc_t * Evt, bool IsIL);
void  mmp_PrepMsg (OutEvtDsc_t * Evt, u16  MsgLen);
void  mm_SendMsg (u8 Mbx);
void  mm_SendOtherSimMsg (u8 Mbx,u8 SimId);

#endif // __MM_OUT_H__
