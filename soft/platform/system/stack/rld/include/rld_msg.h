//------------------------------------------------------------------------------
//  $Log: rld_msg.h,v $
//  Revision 1.2  2005/12/29 17:39:12  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
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
  File       : RLD_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for rld Task

  History    :
--------------------------------------------------------------------------------
  Mar 04 04  |  OTH   | Creation            
================================================================================
*/

#ifndef __RLD_MSG_H__
#define __RLD_MSG_H__

#include "itf_rr.h"
#include "itf_rlc.h"

/*
** Shortcut to access fields in Interlayer messages
*/
#define RLD_ILS  B.IL.Hdr.Specific  // specific header 
#define RLD_ILD  B.IL.Data          // Radio Data

/*
** RLD interlayer headers
*/
typedef union
{
 /*
 ** With RR
 */
 mac_DlDataInd_t macDlDataInd  ;

 /*
 ** With LLC
 */
 rld_DataInd_t rldDataInd ;

 //with RLU
 rld_TestDataReq_t TestDataReq;
} IlHdr_t ;


#include "itf_il.h" // interlayer body definition


/*
** RLD messages body
*/
typedef union 
{
 /*
 ** InterLayer interfaces
 */
 IlBody_t           IL    ;

 /*
 ** Interface with RR
 */
 mac_AckNackInd_t   macAckNackInd ;
 rrp_DlConnectInd_t rrpDlConInd   ;
 rld_TestModeReq_t      Rld_TestModeReq; 
 /*#Added by PENGQIANG 2007.3.19 for testmode begin.*/
#ifdef __EGPRS__
 rrp_Egprs_LoopBackCmd_t rrpEgprsLoopBackCmd;
#endif
/*#Added by PENGQIANG 2007.3.19 for testmode end.*/
} MsgBody_t;

#include "itf_msg.h"

#endif
