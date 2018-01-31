//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_msg.h,v 1.2 2006/03/08 00:18:10 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : RLU_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for rr Task

  History    :
--------------------------------------------------------------------------------
  Apr 05 06  |  MCE   | Addition of rrpCellOptInd 
  Mar 30 06  |  MCE   | Addition of UlCongestInd
  Jan 31 06  |  MCE   | Addition of l1TxDiscartInd 
  Jan 03 06  |  MCE   | Addition of l1TxDiscardReq     
  May 10 05  |  MCE   | Addition of TestModeReq        
  Feb 06 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLU_MSG_H__
#define __RLU_MSG_H__

// interface files
#include "itf_rr.h"
#include "itf_rlc.h"
#include "itf_l1.h"

// RLU files
#include "rlu_iitf.h"

/*
** Shortcut to access fields in Interlayer messages
*/
#define RLU_ILS  B.IL.Hdr.Specific  // specific header 
#define RLU_ILD  B.IL.Data          // Radio Data

/*
** RLU interlayer headers
*/
typedef union
{
 /*
 ** With LLC
 */
 rlu_DataReq_t             rluDataReq ;

 /*
 ** With MAC(RR)
 */
 mac_UlDataReq_t           macDataReq    ;
 mac_TlliInd_t             macTlliInd    ;
 //mac_AckNackInd_t          macAckNackInd ;

  //with rld
  rld_TestDataReq_t TestDataReq;

} IlHdr_t ;                


#include "itf_il.h" // interlayer body definition

/*
** RLU messages body
*/

typedef union 
{
 /*
 ** InterLayer interfaces
 */
 IlBody_t                IL                 ;

 /*
 ** Interface with LLC
 */
 rlu_QueueCreateInd_t    QCreateInd         ;
 rlu_TestModeReq_t       TestModeReq        ;

 /*
 ** Internal Interface with RLU
 */
 rlu_RepeatReq_t         rluRepeatReq       ;
 rlu_TxFillReq_t         rluTxFillReq       ;
      
 /*
 ** Interface with RR
 */
 rrp_ConnectReq_t        rrpConnectReq      ;
 rrp_UlConnectCnf_t      rrpConnectCnf      ;
 rrp_UlReleaseReq_t      rrpReleaseReq      ;
 rr_ReleaseInd_t         rrpReleaseInd      ;
 rrp_CellOptInd_t        rrpCellOptInd      ;

 /*
 ** Interface with MAC(RR)
 */
 mac_AckNackInd_t        macAckNackInd      ;
 mac_TlliInd_t           macTlliInd         ;

 /*
 ** With L1
 */
 l1_TxInd_t              l1TxInd            ;
 l1_PDataPurgeInd_t      l1PurgeDQInd       ;
 l1_TxDiscardReq_t       l1TxDiscardReq     ;
 l1_TxDiscardInd_t       l1TxDiscardInd     ;

 /*
 ** With MMP
 */
 rlu_TReadyReq_t         TReadyReq          ;
 rlu_TReadyInd_t         TReadyInd          ;
 rlu_SynchInd_t          SynchInd           ;
#ifdef __EGPRS__
 //added by fengwei 070319
 rlu_EgprsLoopBackMode_t LoopBackModeReq;
 //added end
#endif


 rlu_UlCongestInd_t      UlCongestInd       ;
 rld_TestModeReq_t      Rld_TestModeReq; 

 rrp_TestModeReq_t        Rrp_TestModeReq;
 l1_GprsTestModeReq_t   l1_GprsTestModeReq;

} MsgBody_t;

#include "itf_msg.h"

#endif
