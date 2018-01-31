//------------------------------------------------------------------------------
//  $Log: llc_msg.h,v $
//  Revision 1.3  2006/05/23 10:36:46  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:46  pca
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
  File       : LLC_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for LLC Task

  History    :
--------------------------------------------------------------------------------
  Mar 03 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __LLC_MSG_H__
#define __LLC_MSG_H__

#include "itf_rlc.h"    // Interface with RLU
#include "itf_rr.h"     // Interface with RR
#include "itf_llc.h"    // Interface with L3


/*
** Shortcut to access fields in Interlayer messages
*/
#define LLC_ILS   B.IL.Hdr.Specific  // Specific header 
#define LLC_ILD   B.IL.Data          // Radio Data





//============================================================================
// LLC interlayer headers
//============================================================================

typedef union
{
  /*
  ** With RLU
  */
  rlu_DataReq_t             rluDataReq;

  /*
  ** With RLD
  */
  rld_DataInd_t             rldDataInd;

  /*
  ** With SNDCP
  */
  llc_EstabReq_t            llcEstabReq;
  llc_EstabInd_t            llcEstabInd;
  llc_EstabCnf_t            llcEstabCnf;
  llc_EstabRsp_t            llcEstabRsp;
  llc_XidReq_t              llcXidReq;
  llc_XidCnf_t              llcXidCnf;
  
  /*
  ** With L3
  */
  llc_UnitDataReq_t         llcUnitDataReq;

  llc_DataReq_t             llcDataReq;

  llc_DataInd_t             llcDataInd;
  llc_XidRsp_t              llcXidRsp;
  llc_XidInd_t              llcXidInd;

} IlHdr_t ;                


#include "itf_il.h"   // interlayer body definition

//#if (sizeof(IlHdr_t) > ILHDR_SPECIFIC_SIZE)
//#error (IlHdr_t) exceeds ILHDR_SPECIFIC_SIZE
//#endif



//============================================================================
// LLC messages body
//============================================================================

typedef union 
{
  /*
  ** Internal Interface with MMP
  */
  llc_TlliInd_t         llcTlliInd;
  llc_CiphParamInd_t    llcCiphParamInd;
  llc_NewTlliInd_t      llcNewTlliInd;
  llc_TriggerReq_t      llcTriggerReq;

  #ifdef __TEMP_FLOW_CONTROL__
  api_UlCongestInd_t    llcFlowCtrlInd;
  #endif
  /*
  ** Internal Interface with L3 (MMP & SNDCP)
  */
  llc_StatusInd_t       llcStatusInd;
  llc_DataCnf_t         llcDataCnf;

  /*
  ** Internal Interface with SNDCP
  */
  llc_ReleaseInd_t      llcReleaseInd;
  llc_ReleaseCnf_t      llcReleaseCnf;
  llc_ReleaseReq_t      llcReleaseReq;
  llc_ResetInd_t        llcResetInd;
  llc_QueueCreateInd_t  llcQueueCreateInd;

  /*
  ** Internal Interface with RLU
  */
  rlu_QueueCreateInd_t  rluQueueCreateInd;

  /*
  ** Internal Interface with MAC
  */
  mac_NewTlliCnf_t      macNewTlliCnf;


  /*
  ** InterLayer interfaces
  */
  IlBody_t              IL;


} MsgBody_t;


#include "itf_msg.h"    // Message definition

#define llc_TxL3Pdu_t   Msg_t


#endif // endif __LLC_MSG_H__
