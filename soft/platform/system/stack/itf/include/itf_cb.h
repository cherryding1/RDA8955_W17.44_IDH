//------------------------------------------------------------------------------
//  $Log: itf_cb.h,v $
//  Revision 1.2  2005/12/29 17:38:44  pca
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
  File       : ITF_CB.H
--------------------------------------------------------------------------------

  Scope      : Interface Mesages provided by CB                       

  History    :
--------------------------------------------------------------------------------
  Dec 12 08  |  FCH   | Creation            
================================================================================
*/

#ifndef __ITF_CB_H__
#define __ITF_CB_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "cmn_dec.h"

#include "itf_api.h"


/*
** Primitive ID definitions
*/
#define CB_ACTIVATE_REQ     ( HVY_CB + 1  )
#define CB_DEACTIVATE_REQ   ( HVY_CB + 2  )
#define CB_MSGREC_IND       ( HVY_CB + 3  )
#define CB_PAGREC_IND       ( HVY_CB + 4  )


/*
================================================================================
  Structure  : cb_ActivateReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CB
  Scope      : API activate Cell Broadcast Service
================================================================================
*/
typedef api_SmsCBActivateReq_t cb_ActivateReq_t;

/*
================================================================================
  Structure  : cb_DeactivateReq_t
--------------------------------------------------------------------------------
  Direction  : API -> CB
  Scope      : API deactivate Cell Broadcast Service
================================================================================
*/
typedef api_SmsCBDeActivateReq_t cb_DeactivateReq_t;

/*
================================================================================
  Structure  : cb_MsgRecInd_t
--------------------------------------------------------------------------------
  Direction  : CB -> API
  Scope      : CB forwards to API a received SMSCB message
================================================================================
*/
typedef api_SmsCBReceiveInd_t cb_MsgRecInd_t;

/*
================================================================================
  Structure  : cb_PageRecInd_t
--------------------------------------------------------------------------------
  Direction  : CB -> API
  Scope      : CB forwards to API a received CBS page
================================================================================
*/
typedef api_SmsCBPageInd_t cb_PageRecInd_t;



#endif // __ITF_CB_H__
