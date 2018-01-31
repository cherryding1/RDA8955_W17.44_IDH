//------------------------------------------------------------------------------
//  $Log: cb_msg.h,v $
//  Revision 1.3  2006/05/23 10:36:37  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:40  pca
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

  File       : CB_MSG.H
--------------------------------------------------------------------------------

  Scope      : Defintion of Msg body for CB Task (SMSCB)

  History    :
--------------------------------------------------------------------------------
  Dec 07 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __CB_MSG_H__
#define __CB_MSG_H__

#include "itf_cb.h"
#include "itf_l1.h"     // Interface with L1
#include "itf_rr.h"     // Interface with RR


/*
** Shortcut to access fields in Interlayer messages
*/
#define CB_ILS   B.IL.Hdr.Specific  // Specific header 
#define CB_ILD   B.IL.Data          // Radio Data





//============================================================================
// CB interlayer headers
//============================================================================

typedef union
{

  /*
  ** With L1
  */
  l1_DataInd_t          l1DataInd;


} IlHdr_t ;                


#include "itf_il.h"   // interlayer body definition



//============================================================================
// CB messages body
//============================================================================

typedef union 
{
  /*
  ** Internal Interface with RR
  */
  rr_CellInd_t          rrCellInd;
  rr_CbchReq_t          rrCbchReq;

  /*
  ** Internal Interface with API
  */
  cb_ActivateReq_t      cbActivateReq;
  cb_DeactivateReq_t    cbDeactivateReq;
  cb_MsgRecInd_t        cbMsgRecInd;
  cb_PageRecInd_t       cbPageRecInd;

  /*
  ** Internal Interface with L1
  */
  l1_CbchSkipReq_t      l1CbchSkipReq;

  /*
  ** InterLayer interfaces
  */
  IlBody_t              IL;


} MsgBody_t;


#include "itf_msg.h"    // Message definition


#endif // endif __CB_MSG_H__
