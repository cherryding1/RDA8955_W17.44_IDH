//------------------------------------------------------------------------------
//  $Log: rlp_iitf.h,v $
//  Revision 1.3  2006/05/23 10:37:37  fch
//  Update Copyright
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
  File       : RLP_IITF.H
--------------------------------------------------------------------------------

  Scope      : Defintion of RLP internal events

  History    :
--------------------------------------------------------------------------------
  Mar 31 05  |  FCH   | Creation
================================================================================
*/
#ifndef __RLP_IITF_H__
#define __RLP_IITF_H__

#include "itf_rlp.h"
#include "rlp_defs.h"

    //-------------------------------------------------------
    //  Primitive Id for derived event
    //-------------------------------------------------------
enum
{
  RLP_RX_S      = ( RLP_INTRA_BASE + 0 ),
  RLP_RX_I,
  RLP_RX_UI,
  RLP_RX_DM,
  RLP_RX_SABM,
  RLP_RX_DISC,
  RLP_RX_UA,
  RLP_RX_XID,
  RLP_RX_TEST,
  
  RLP_SND_OPP,

    
  RLP_LAST_DERIVED_MSG
};


    //-------------------------------------------------------
    //  Timer Ids
    //-------------------------------------------------------
enum
{
  RLP_FIRST_TIM         = (SXS_TIMER_EVT_ID | RLP_LAST_DERIVED_MSG),
  RLP_T1_SABM_DISC_TIM  = RLP_FIRST_TIM,
  RLP_T1_XID_TIM,
  RLP_T1_IFRM_TIM,
  RLP_TRCVR_TIM,

    
  RLP_TRCVS_TIM,
  RLP_TRCVS_LAST_TIM    = RLP_TRCVS_TIM + RLP_MAX_WINDOW_LEN - 1,
  
  RLP_LAST_TIM
};


#endif // __RLP_IITF_H__
