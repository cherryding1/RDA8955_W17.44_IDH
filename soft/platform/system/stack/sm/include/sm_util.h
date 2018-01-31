//------------------------------------------------------------------------------
//  $Log: sm_util.h,v $
//  Revision 1.4  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.3  2006/01/27 18:05:30  aba
//  *** empty log message ***
//
//  Revision 1.2  2005/12/29 17:39:17  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/

/*
================================================================================
  File       : sm_util.h
--------------------------------------------------------------------------------

  Scope      : Header File for sm_util.c
  History    :
--------------------------------------------------------------------------------
  Aug 02 04  |  PCA   | Creation
================================================================================
*/

#ifndef __SM_UTIL_H__
#define __SM_UTIL_H__

#include "sxr_ops.h"
#include "sm_data.h"


PdpCtx_t* sm_GetPdpCtx(void);
void  sm_FreeAllTi(void);
u8    sm_AllocTi(u8 NSapiIdx);
void  sm_FreeTi(u8 TI, u8 TxTIFlag);
void  sm_StoreNetworkOriginatedTI(u8 TI, u8 NSapiIdx);
u8    sm_GetNSapiIdx(u8 TI, u8 RxTIFlag);
u8    sm_GetNSapiIdxFromSmIndex(u8 SmIndex);
u8    sm_SelectSapi(u8 FirstByteQoS);
bool  sm_QoSCheck(u8 NegotiatedQoSLen, u8 NegociatedQoS[11], u8 RequestedQoSLen, u8 RequestedQoS[11]);
void  sm_ConvertR97R98QoSIntoR99QoS(u8* QoS);
void  sm_RetransMsg(void);

#endif    // __SM_UTIL_H__
