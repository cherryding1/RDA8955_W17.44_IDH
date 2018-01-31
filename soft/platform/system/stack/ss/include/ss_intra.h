////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: ss_intra.h
// 
// DESCRIPTION:
//   Definition for the intra event of SS
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __SS_INTRA_H__
#define __SS_INTRA_H__

// System includes
#include "sxs_type.h"

// Interface includes
#include "itf_msgc.h"

#define SS_INTRA_BASE      ( HVY_SS + 0x8000 )

#ifdef __SS_TIMER__
//
// Primitive intra SS timers ID definitions
//
#define SS_TIMER0_EXP        ( ( SS_INTRA_BASE + 1  ) | SXS_TIMER_EVT_ID )
#endif

//
// Primitive intra SS Messages ID definitions
//
#define SS_NETWORK_ACTION_REQ        ( SS_INTRA_BASE + 0x21 )
#define SS_COMP_RCVD              ( SS_INTRA_BASE + 0x22 )
#define SS_USSD_REL_COMP        ( SS_INTRA_BASE + 0x23 )
#endif // __SS_INTRA_H__
