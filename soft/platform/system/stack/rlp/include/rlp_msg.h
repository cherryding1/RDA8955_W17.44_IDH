//------------------------------------------------------------------------------
//  $Log: rlp_msg.h,v $
//  Revision 1.5  2006/05/23 10:37:37  fch
//  Update Copyright
//
//  Revision 1.4  2006/01/27 15:13:34  fch
//  Added interrogation from API of the current RLP system parameters settings
//
//  Revision 1.3  2006/01/27 10:14:45  fch
//  Handling of RLP system parameters provides by user and XID command sending
//
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

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : RLP_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for RLP Task

  History    :
--------------------------------------------------------------------------------
  Mar 03 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __RLP_MSG_H__
#define __RLP_MSG_H__

#include "itf_l1.h"     
#include "itf_rlp.h"    





#define RLP_ILS   B.IL.Hdr.Specific  
#define RLP_ILD   B.IL.Data          





//============================================================================
// RLP interlayer headers
//============================================================================

typedef union
{
  


  l1_DataReq_t              l1DataReq;

  


  rlp_DReqCtrl_t            rlpDataReq;
  rlp_DReqCtrl_t            rlpUDataReq;

} IlHdr_t ;                


#include "itf_il.h"   



//============================================================================
// RLP messages body
//============================================================================

typedef union 
{
  


  rlp_CrlpReq_t         rlpCrlpReq;
  rlp_CrlpRdInd_t       rlpCrlpRdInd;

  


  rlp_ConfReq_t         rlpConfReq;
  rlp_FlowCtrlReq_t     rlpFlowCtrlReq;

  rlp_ConnectCnf_t      rlpConnectCnf;
  rlp_FlowCtrlInd_t     rlpFlowCtrlInd;
  

  


  IlBody_t              IL;


} MsgBody_t;


#include "itf_msg.h"    


#endif // endif __RLP_MSG_H__
