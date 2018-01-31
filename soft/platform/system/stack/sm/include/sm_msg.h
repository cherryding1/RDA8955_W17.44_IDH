//------------------------------------------------------------------------------
//  $Log: sm_msg.h,v $
//  Revision 1.4  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.3  2006/01/24 11:55:26  aba
//  minimum quality of service
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
  File       : SM_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for SM Task

  History    :
--------------------------------------------------------------------------------
  Jun 22 06  |  PCA   | Creation
================================================================================
*/
#ifndef __SM_MSG_H__
#define __SM_MSG_H__

#include "itf_mm.h"    // Interface with MMP
#include "itf_sm.h"    // Interface with SNDCP
#include "itf_api.h"    // Interface with API


//============================================================================
// SM interlayer headers
//============================================================================



/*
** SM interlayer headers
*/
typedef union
{
  /*
  ** With MMP
  */
  mmp_DataReq_t             mmpDataReq  ;
} IlHdr_t ;

#include "itf_il.h" // interlayer body definition


//============================================================================
// SM messages body
//============================================================================
typedef union
{
  /*
  ** With MMP
  */
  mmp_AbortInd_t            mmpAbortInd;
  mmp_PdpCtxStatusInd_t     mmpPdpCtxStatusInd;
  mmp_DelSavaDataReq_t      mmp_DelSavaDataReq;      // DMX add 20161031.  MMP_DEL_SAVE_DATA_REQ
  mmp_RauAccPdpStatusInd_t mmp_RauAccPdpStatusInd;  // DMX add 20161120, __RAUACC_PDP_STATUS_FTR__

  /*
  ** With SNDCP
  */
  sm_ActivateInd_t          smActivateInd;
  sm_DeactivateInd_t        smDeactivateInd;
  sm_ModifyInd_t            smModifyInd;
  sm_ActivateRsp_t          smActivateRsp;
  sm_DeactivateRsp_t        smDeactivateRsp;
  sm_ModifyRsp_t            smModifyRsp;
  sm_StatusReq_t            smStatusReq;

  /*
  ** With API
  */
  sm_PdpCtxActivateReq_t    smPdpCtxActivateReq;
  sm_PdpCtxActivateCnf_t    smPdpCtxActivateCnf;
  sm_PdpCtxSmIndexInd_t     smPdpCtxSmIndexInd;
  sm_PdpCtxActivateRej_t    smPdpCtxActivateRej;
  sm_PdpCtxActivateInd_t    smPdpCtxActivateInd;
  sm_PdpCtxModifyReq_t      smPdpCtxModifyReq;
  sm_PdpCtxModifyCnf_t      smPdpCtxModifyCnf;
  sm_PdpCtxModifyRej_t      smPdpCtxModifyRej;
  sm_PdpCtxModifyInd_t      smPdpCtxModifyInd;
  sm_PdpCtxModifyAcc_t      smPdpCtxModifyAcc;
  sm_PdpCtxDeactivateReq_t  smPdpCtxDeactivateReq;
  sm_PdpCtxDeactivateCnf_t  smPdpCtxDeactivateCnf;
  sm_PdpCtxDeactivateInd_t  smPdpCtxDeactivateInd;
#ifdef __MINQOS__
  sm_PdpSetMinQoSReq_t      smPdpSetMinQoSReq;
#endif
  /*
  ** InterLayer interfaces
  */
  IlBody_t                  IL;


} MsgBody_t;


#include "itf_msg.h"    // Message definition

#endif // endif __SM_MSG_H__
