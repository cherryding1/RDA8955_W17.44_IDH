////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2002-2006, Coolsand Technologies, Inc.
//                       All Rights Reserved
// 
// This source code is property of Coolsand. The information contained in this
// file is confidential. Distribution, reproduction, as well as exploitation,
// or transmisison of any content of this file is not allowed except if
// expressly permitted.Infringements result in damage claims!
//
// FILENAME: SS_MSG.H
// 
// DESCRIPTION:
//   Defintion of Msg body for SS Task
// 
// REVISION HISTORY:
//   NAME            DATE            REMAKS
//   W F |20070529 |add ss_ActionInd to msg body, reviewed by DMX
////////////////////////////////////////////////////////////////////////////////

#ifndef __SS_MSG_H__
#define __SS_MSG_H__

#include "itf_mm.h"
#include "itf_ss.h"

typedef union
{
 // Interlayer header with MM
  u8 toto;
} IlHdr_t;

#include "itf_il.h"

typedef union 
{
    IlBody_t IL;

  // Interface with MM
  mm_EstabReq_t    mmEstabReq;
  mm_PDTIInd_t     mmPDTIInd;    //used for the ServReqAck and EstabInd

  // Interface with API
  ss_ActionReq_t  ss_ActionReq;  //API requests SS to start a transaction
  ss_TIInd_t      ss_TIInd;      //SS indicates to API that transaction has started and provides TI
  ss_ActionCnf_t  ss_ActionCnf;  //SS provides success indication to API for a given transaction
  ss_ErrorInd_t   ss_ErrorInd;   //SS provides error indication to API for a given transaction
  ss_ActionInd_t  ss_ActionInd;  //when nwk initiate USSD,SS provide results obtained to API. add by wangf 20070529

} MsgBody_t;

#include "itf_msg.h"

#endif
