//------------------------------------------------------------------------------
//  $Log: sm_mmdec.h,v $
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
  File       : sm_mmdec.h
--------------------------------------------------------------------------------

  Scope      : Header File for sm_mmdec
  History    :
--------------------------------------------------------------------------------
  Oct 06 04  |  ABA   | Creation
================================================================================
*/

#ifndef __SM_MMDEC_H__
#define __SM_MMDEC_H__

#include "sxr_ops.h"
#include "sm_data.h"

#define	SM_CONVERSATIONAL CLASS 1
#define	SM_STREAMING_CLASS      2
#define	SM_INTERACTIVE_CLASS    3
#define	SM_BACKGROUND_CLASS     4

u8 sm_ProcessActivatePdpCtxAccMsg(api_PdpCtxActivateCnf_t *PtApiPdpCtxActivateCnf, PdpCtx_t *PtPdpCtx, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessActivatePdpCtxRejMsg(api_PdpCtxActivateRej_t *PtApiPdpCtxActivateRej, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessRequestPdpCtxActivateIndMsg(api_PdpCtxActivateInd_t *PtApiPdpCtxActivateInd, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessModifyPdpCtxReqMsg(api_PdpCtxModifyInd_t *PtApiPdpCtxModifyInd, PdpCtx_t *PtPdpCtx, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessModifyPdpCtxAccMsg(api_PdpCtxModifyCnf_t *PtApiPdpCtxModifyCnf, PdpCtx_t *PtPdpCtx, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessModifyPdpCtxRejMsg(api_PdpCtxModifyRej_t *PtApiPdpCtxModifyRej, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessDeactivatePdpCtxReqMsg(api_PdpCtxDeactivateInd_t *PtApiPdpCtxDeactivateInd, u8 *PtMmpData, u8 *PtLastByte);
u8 sm_ProcessSmStatusMsg(u8 *Cause, u8 *PtMmpData, u8 *PtLastByte);

#endif    // __SM_MMDEC_H__
