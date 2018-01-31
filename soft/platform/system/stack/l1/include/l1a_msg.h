/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2003

================================================================================
*/

/*
================================================================================
  File       : l1a_msg.h
--------------------------------------------------------------------------------

  Scope      : L1 asynchrone, message body definition.

  History    :
--------------------------------------------------------------------------------
  Aug 25 03  | ADA   | Creation
================================================================================
*/

#ifndef __L1A_MSG_H__
#define __L1A_MSG_H__

#include "sxs_type.h"
#include "itf_l1.h"

typedef union
{
 l1_DataReq_t        l1_DataReq;
 l1_DataInd_t        l1_DataInd;
 l1_PSDataReq_t      l1_PSDataReq;
} IlHdr_t;

#include "itf_il.h"

typedef union
{
 l1_InitReq_t        l1_InitReq;
 l1_HighBandChangeReq_t l1_HighBandChgReq;
 l1_Power_t          l1_Power;
 l1_SynchReq_t       l1_SynchReq;
 l1_SynchUpdateReq_t l1_SynchUpdateReq;
 l1_SynchRsp_t       l1_SynchRsp;
 l1_StopSynchReq_t   l1_StopSynchReq;
 l1_ErrorInd_t       l1_ErrorInd;
 l1_CchReq_t         l1_CchReq;
 l1_CbchReq_t        l1_CbchReq;
 l1_CbchSkipReq_t    l1_CbchSkipReq;
 l1_StopChReq_t      l1_StopChReq;
 l1_PCchReq_t        l1_PCchReq;
 l1_BcchReq_t        l1_BcchReq;  
 l1_BcchModeReq_t  l1_BcchModeReq; 
 l1_PBcchReq_t       l1_PBcchReq;
 l1_RachOnReq_t      l1_RachOnReq;
 l1_RachUpdtReq_t    l1_RachUpdtReq;
 l1_RachOffReq_t     l1_RachOffReq;
 l1_RachInd_t        l1_RachInd;
 l1_ConnectReq_t     l1_ConnectReq;
 IlBody_t            Il;
 l1_FreqRedefReq_t   l1_FreqRedefReq;
 l1_CellOptUpdtReq_t l1_CellOptUpdtReq;
 l1_ChanModeModifyReq_t l1_ChanModeModifyReq;
 l1_CiphReq_t        l1_CiphReq;
 l1_SetTestLoopReq_t l1_SetTestLoopReq;
 l1_ConnectCnf_t     l1_ConnectCnf;
 l1_ReleaseReq_t     l1_ReleaseReq;
 l1_PhyInfoReq_t     l1_PhyInfoReq;
// l1_DataReq_t        l1_DataReq;
 l1_TxInd_t          l1_TxInd;
 l1_TBFCfgReq_t      l1_TBFCfgReq;
 l1_TBFCfgCnf_t      l1_TBFCfgCnf;
 l1_PwrCtrlTAReq_t   l1_PwrCtrlTAReq;
 l1_PDCHRelReq_t     l1_PDCHRelReq;
 l1_TBFRelReq_t      l1_TBFRelReq;
 l1_MacFixedUpdtReq_t l1_MacFixedUpdtReq;
 l1_EndFixedPeriodInd_t l1_EndFixedPeriodInd;
// l1_PSDataReq_t      l1_PSDataReq;
 l1_GprsTestModeReq_t l1_GprsTestModeReq;
 l1_MonitorReq_t     l1_MonitorReq;
 l1_MonitorInd_t     l1_MonitorInd;
 l1_PMeasCtrlReq_t   l1_PMeasCtrlReq;
 l1_ExtIntReq_t      l1_ExtIntReq;
 l1_ExtIntInd_t      l1_ExtIntInd;
 l1_NcMonitorReq_t   l1_NcMonitorReq;
 l1_NcMonitorInd_t   l1_NcMonitorInd;
 l1_ChnQualityInd_t  l1_ChnQualityInd;
 l1_PSDataInd_t      l1_PSDataInd;
 l1_PDataPurgeInd_t  l1_PDataPurgeInd;
 l1_TxDiscardReq_t   l1_TxDiscardReq;
 l1_AdjCellReportReq_t l1_AdjCellReportReq;

 #ifdef __EGPRS__
 l1_IRBufferStatus_Ind_t  l1_IRBuffStatInd;
 l1_LoopbackSwiReq_t	   l1LoopbackSwiReq;  //added by zhanghy. 2007-09-06. for egprs loop back test mode.
 #endif
 u8                  l1_Data [16];
} MsgBody_t;

#include "itf_msg.h"

#endif

