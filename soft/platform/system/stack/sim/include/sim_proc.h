//------------------------------------------------------------------------------
//  $Log: sim_proc.h,v $
//  Revision 1.3  2006/05/22 20:51:18  aba
//  Update of the copyright
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

/*
================================================================================
  File       : sim_proc.h
--------------------------------------------------------------------------------

  Scope      : Declaration of SIM context variables

  History    :
--------------------------------------------------------------------------------
  Jun 06 05  |  ABA   | ISSUE 19: poll interval for simtoolkit
--------------------------------------------------------------------------------
  Sep 28 03  |  ABA   | Creation
================================================================================
*/
#ifndef __SIM_PROC_H__
#define __SIM_PROC_H__
void sim_ResetReq(void);
void sim_RunGsmAlgoReq(void);
void sim_ReadBinaryReq(void);
void sim_UpdateBinaryReq(void);
void sim_ReadRecordReq(void);
void sim_UpdateRecordReq(void);
void sim_VerifyCHVReq(void);
void sim_ChangeCHVReq(void);
void sim_DisableCHVReq(void);
void sim_EnableCHVReq(void);
void sim_UnblockCHVReq(void);
void sim_InvalidateReq(void);
void sim_RehabilitateReq(void);
void sim_SeekReq(void);
void sim_SeekAllReq(void);
void sim_IncreaseReq(void);
void sim_DedicFileStatusReq(void);
void sim_ElemFileStatusReq(void);
void sim_StatusTimer(void);
void sim_ToolKitReq(void);
void sim_TerminalProfileReq(void);
void sim_EnvelopeReq(void);
void sim_TerminalResponseReq(void);
void sim_StopReq(void);
void sim_PollReq(void);
void sim_PollOffReq(void);
void sim_SelectApplicationReq(void);
void sim_RunUmtsAlgoReq(void);
void sim_SetUsimPbkReq(void);
void sim_PowerOffReq(void);
void sim_RetryReq(void);

#ifdef LTE_NBIOT_SUPPORT
void nbiot_sim_ReadBinaryReq(void);
void nbiot_sim_UpdateBinaryReq(void);
void nbiot_sim_ReadRecordReq(void);
void nbiot_sim_UpdateRecordReq(void);
void nbiot_sim_DedicFileStatusReq(void);
void nbiot_sim_ElemFileStatusReq(void);
void nbiot_sim_RunUmtsAlgoReq(void);
#endif
bool sim_CheckResetState(void);
#endif // __SIM_PROC_H__
