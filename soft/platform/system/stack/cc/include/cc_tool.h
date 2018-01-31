////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_tool.h
// 
// DESCRIPTION:
//   Header file for generic tools for CC
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////


// System includes
#include "sxr_ops.h"
#include "ed_c.h"
#include "edre.h"
#include "cc_msg.h"
#include "itf_msg.h"

// cc_tool.c
u8   cc_RelCon          ( bool );
u8   cc_CallClearing    ( Msg_t *, bool );
void cc_CreateCtx       ( u8 );
u8   cc_EncSdRelMO      ( Msg_t * );
u8   cc_EncCause        ( u8*, u8 );
void cc_EncSdStatus     ( u8 );
void cc_EncSdRelCmplt   ( u8, u8* );
void cc_StopAllTimers   ( void );
void cc_HdleProgressInd ( u8 );
u8   cc_AllocTI         ( void );
void cc_EncSdSimpleMsg  ( u8 );
void cc_SdApiErrorInd   ( u8 );
void cc_HdleAudioOn     ( void );
u8   cc_TICmp           ( void *, void * );
u8   cc_TimerCallClear  ( void );
void cc_EncSdAlert      ( cc_AlertReq_t * );
void cc_ReadSimSST      ( void );
void cc_EncSdConnect    ( cc_ConnectReq_t * );

// cc_crss.c
u8   cc_HdleFacilityIE  ( c_Facility_IE*, cc_CrssFacilityIndIE_t* );
u8   cc_HdleInvoke      ( c_Facility_IE_Invoke_data*, cc_CrssFacilityIndIE_t* );
void cc_EncFacilityIE   ( cc_CrssFacilityReqIE_t *, c_Facility_IE_MO * );
