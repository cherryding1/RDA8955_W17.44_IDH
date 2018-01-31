//------------------------------------------------------------------------------
//  $Log: sim_msg.h,v $
//  Revision 1.4  2006/05/22 20:51:18  aba
//  Update of the copyright
//
//  Revision 1.3  2006/01/27 21:21:44  aba
//  Add sim_EnvelopeCnf message
//
//  Revision 1.2  2005/12/29 17:39:16  pca
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

/*================================================================================
  File       : SIM_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for sim Task

  History    :
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __SIM_MSG_H__
#define __SIM_MSG_H__

#include "itf_sim.h"

typedef union
{
  sim_ResetReq_t               sim_ResetReq;
  sim_ResetCnf_t               sim_ResetCnf;
  sim_RunGsmAlgoReq_t          sim_RunGsmAlgoReq;
  usim_RunUmtsAlgoReq_t          usim_RunUmtsAlgoReq;
  sim_ReadBinaryReq_t          sim_ReadBinaryReq;
  sim_UpdateBinaryReq_t        sim_UpdateBinaryReq;
  sim_ReadRecordReq_t          sim_ReadRecordReq;
  sim_UpdateRecordReq_t        sim_UpdateRecordReq;
  sim_InvalidateReq_t          sim_InvalidateReq;
  sim_RehabilitateReq_t        sim_RehabilitateReq;
  sim_SeekReq_t                sim_SeekReq;
  sim_IncreaseReq_t            sim_IncreaseReq;
  sim_VerifyCHVReq_t           sim_VerifyCHVReq;
  sim_ChangeCHVReq_t           sim_ChangeCHVReq;
  sim_DisableCHVReq_t          sim_DisableCHVReq;
  sim_EnableCHVReq_t           sim_EnableCHVReq;
  sim_UnblockCHVReq_t          sim_UnblockCHVReq;
  sim_DedicFileStatusReq_t     sim_DedicFileStatusReq;
  sim_ElemFileStatusReq_t      sim_ElemFileStatusReq;
  sim_TerminalProfileReq_t     sim_TerminalProfileReq;
  sim_ToolKitReq_t             sim_ToolKitReq;
  sim_EnvelopeReq_t            sim_EnvelopeReq;
  sim_TerminalResponseReq_t    sim_TerminalResponseReq;
  sim_PollReq_t                sim_PollReq; 
  sim_FetchInd_t               sim_FetchInd;
  sim_StatusErrorInd_t         sim_StatusErrorInd;
  sim_Cnf_t                    sim_Cnf;
  sim_CnfWithFile_t            sim_CnfWithFile;
  sim_EnvelopeCnf_t            sim_EnvelopeCnf;
  sim_SelectApplicationReq_t      sim_SelectApplicationReq;
  sim_SelectApplicationCnf_t       sim_SelectApplicationCnf;
  sim_SetUsimPbkReq_t              sim_SetUsimPbkReq;
  sim_SeekAllReq_t                sim_SeekAllReq;
}MsgBody_t;

#include "itf_msg.h"  // message definition

#endif
