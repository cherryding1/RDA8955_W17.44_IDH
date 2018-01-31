////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_out.h
// 
// DESCRIPTION:
//   Header file for cc_out.c
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __CC_OUT_H__
#define __CC_OUT_H__

// System includes
#include "sxs_type.h"
#include "fsm.h"

void  cc_PrepMsg (OutEvtDsc_t * Evt, bool IsIL);
void  cc_SendMsg (u8 Mbx);


#endif // __CC_OUT_H__
