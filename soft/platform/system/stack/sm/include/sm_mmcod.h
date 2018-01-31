//------------------------------------------------------------------------------
//  $Log: sm_mmcod.h,v $
//  Revision 1.3  2006/05/22 19:35:41  aba
//  Update of the copyright
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
  File       : sm_mmcod.h
--------------------------------------------------------------------------------

  Scope      : Header File for sm_mmcod
  History    :
--------------------------------------------------------------------------------
  Oct 06 04  |  ABA   | Creation
================================================================================
*/

#ifndef __SM_MMCOD_H__
#define __SM_MMCOD_H__

#include "sxr_ops.h"
#include "sm_data.h"

//============================================================================
// SM L3 Messages Type
//============================================================================
#define SM_PD           0x0A
#define SM_MSG_BASE     0x40
#define ACT_PDP_REQ     (SM_MSG_BASE  + 1)
#define ACT_PDP_ACC     (SM_MSG_BASE  + 2)
#define ACT_PDP_REJ     (SM_MSG_BASE  + 3)

#define PDP_REQ_ACT     (SM_MSG_BASE  + 4)
#define PDP_REQ_ACT_REJ (SM_MSG_BASE  + 5)

#define DEACT_PDP_REQ   (SM_MSG_BASE  + 6)
#define DEACT_PDP_ACC   (SM_MSG_BASE  + 7)
#define NW_MOD_PDP_REQ  (SM_MSG_BASE  + 8)
#define MS_MOD_PDP_ACC  (SM_MSG_BASE  + 9)
#define MS_MOD_PDP_REQ  (SM_MSG_BASE  + 10)
#define NW_MOD_PDP_ACC  (SM_MSG_BASE  + 11)
#define MOD_PDP_REJ     (SM_MSG_BASE  + 12)

#define ACT2_PDP_REQ    (SM_MSG_BASE  + 13)
#define ACT2_PDP_ACC    (SM_MSG_BASE  + 14)
#define ACT2_PDP_REJ    (SM_MSG_BASE  + 15)
#define SM_STATUS       (SM_MSG_BASE  + 21)




//============================================================================
// SM IEIs for optional elements
//============================================================================
#define ACCESS_POINT_NAME_IEI       0x28
#define PROT_CONF_OPT_IEI           0x27
#define PDP_ADDRESS_IEI             0x2B
#define PACKET_FLOW_ID_IEI          0x34
#define PACKET_FLOW_TEMP_IEI        0x36
#define REQUEST_LLC_SAPI_IEI        0x32
#define NEGOCIATED_LLC_SAPI_IEI     0x32
#define REQUEST_NEW_QOS_IEI         0x30
#define NEGOCIATED_NEW_QOS_IEI      0x30
#define NEW_TFT_IEI                 0x31
#define NEW_RADIO_PRIORITY_IEI      0x08
#define NO_MORE_IEI                 0xFF

void sm_BuildActivatePdpCtxReqMsg(void);
void sm_BuildActivatePdpCtxRejMsg(u8 SmIndex, u8 Cause);
void sm_BuildDeactivatePdpCtxReqMsg(u8 Cause);
void sm_BuildDeactivatePdpCtxAccMsg(void);
void sm_BuildStatusMsg(u8 Cause);
void sm_BuildModifyPdpCtxReqMsg(void);
void sm_BuildModifyPdpCtxAccMsg(void);

#endif    // __SM_MMCOD_H__
