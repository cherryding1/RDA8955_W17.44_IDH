////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_intra.h
// 
// DESCRIPTION:
//   Definition for the intra event of CC
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CC_INTRA_H__
#define __CC_INTRA_H__

// System includes
#include "sxs_type.h"

// Interface includes
#include "itf_msgc.h"

#define CC_INTRA_BASE      ( HVY_CC + 0x8000 )

//
// Primitive intra CC timers ID definitions
//
#define CC_T303_EXP        ( ( CC_INTRA_BASE + 1   ) | SXS_TIMER_EVT_ID )
#define CC_T305_EXP        ( ( CC_INTRA_BASE + 2   ) | SXS_TIMER_EVT_ID )
#define CC_T308_EXP        ( ( CC_INTRA_BASE + 3   ) | SXS_TIMER_EVT_ID )
#define CC_T310_EXP        ( ( CC_INTRA_BASE + 4   ) | SXS_TIMER_EVT_ID )
#define CC_T313_EXP        ( ( CC_INTRA_BASE + 5   ) | SXS_TIMER_EVT_ID )
#define CC_T323_EXP        ( ( CC_INTRA_BASE + 6   ) | SXS_TIMER_EVT_ID )
#define CC_T336_EXP        ( ( CC_INTRA_BASE + 7   ) | SXS_TIMER_EVT_ID )
#define CC_T337_EXP        ( ( CC_INTRA_BASE + 8   ) | SXS_TIMER_EVT_ID )
#define CC_TMPTY_EXP       ( ( CC_INTRA_BASE + 9   ) | SXS_TIMER_EVT_ID )
#define CC_TECT_EXP        ( ( CC_INTRA_BASE + 0xA ) | SXS_TIMER_EVT_ID )
#define CC_TS10_EXP        ( ( CC_INTRA_BASE + 0xB ) | SXS_TIMER_EVT_ID )
//[[ __CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX
#define CC_TRETRY_EXP      ( ( CC_INTRA_BASE + 0xC ) | SXS_TIMER_EVT_ID )
#define CC_TWAITLU_EXP     ( ( CC_INTRA_BASE + 0xD ) | SXS_TIMER_EVT_ID )
//]]__CC_TRY_FOR_MM_RELEASE_FTR__ 20160906 DMX

//
// Primitive intra CC Messages ID definitions
//
#define CC_SETUP           ( CC_INTRA_BASE + 0x21 )
#define CC_CALL_PROCEEDING ( CC_INTRA_BASE + 0x22 )
#define CC_PROGRESS        ( CC_INTRA_BASE + 0x23 )
#define CC_ALERTING        ( CC_INTRA_BASE + 0x24 )
#define CC_CONNECT         ( CC_INTRA_BASE + 0x25 )
#define CC_CONNECT_ACK     ( CC_INTRA_BASE + 0x26 )
#define CC_DISCONNECT      ( CC_INTRA_BASE + 0x27 )
#define CC_RELEASE         ( CC_INTRA_BASE + 0x28 )
#define CC_RELEASE_CMPL    ( CC_INTRA_BASE + 0x29 )
#define CC_STATUS_ENQUIRY  ( CC_INTRA_BASE + 0x2a )
#define CC_START_DTMF_ACK  ( CC_INTRA_BASE + 0x2b )
#define CC_START_DTMF_REJ  ( CC_INTRA_BASE + 0x2c )
#define CC_STOP_DTMF_ACK   ( CC_INTRA_BASE + 0x2d )
#define CC_HOLD_ACK        ( CC_INTRA_BASE + 0x2e )
#define CC_HOLD_REJ        ( CC_INTRA_BASE + 0x2f )
#define CC_RTRV_ACK        ( CC_INTRA_BASE + 0x30 )
#define CC_RTRV_REJ        ( CC_INTRA_BASE + 0x31 )
#define CC_FACILITY        ( CC_INTRA_BASE + 0x32 )
#define CC_MODIFY          ( CC_INTRA_BASE + 0x33 )
#define CC_MODIFYCMPLT     ( CC_INTRA_BASE + 0x34 )
#define CC_MODIFY_REJ      ( CC_INTRA_BASE + 0x35 )
#define CC_NOTIFY          ( CC_INTRA_BASE + 0x36 )
#define CC_USER_INFO       ( CC_INTRA_BASE + 0x37 )
#define CC_CONG_CTRL       ( CC_INTRA_BASE + 0x38 )
#define CC_STATUS          ( CC_INTRA_BASE + 0x39 )

#endif // __CC_INTRA_H__
