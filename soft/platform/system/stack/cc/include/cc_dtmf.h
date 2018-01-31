////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: cc_dtmf.c
// 
// DESCRIPTION:
//   Header file for dtmf functions 
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   TODO... 
//
////////////////////////////////////////////////////////////////////////////////


#ifndef __CC_DTMF_H__
#define __CC_DTMF_H__

// System includes
#include "sxr_ops.h"
#include "ed_c.h"
#include "edre.h"

void   cc_DtmfStartReq   ( u8 Key, cc_DtmfProc_t * DtmfProc) ;

void   cc_DtmfProcInit   ( cc_DtmfProc_t * DtmfProc   ) ;
void   cc_DtmfStopReq    ( cc_DtmfProc_t * DtmfProc   ) ;
void   cc_DtmfStartAck   ( cc_DtmfProc_t * DtmfProc   ) ;
void   cc_DtmfEndProc    ( cc_DtmfProc_t * DtmfProc   ) ;
void 	cc_DiscardOtherDtmf( void);
bool cc_OnDtmf();
void cc_RetrieveApiMsgSavedforDtmf();


#endif
