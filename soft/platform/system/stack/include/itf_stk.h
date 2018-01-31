//------------------------------------------------------------------------------
//  $Log: itf_stk.h,v $
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
  File       : ITF_STK.H
--------------------------------------------------------------------------------

  Scope      : Interface Messages broadcast to several layers

  History    :
--------------------------------------------------------------------------------
  Aug 06 03  |  OTH   | Creation            
================================================================================
*/

#ifndef __ITF_STK_H__
#define __ITF_STK_H__
 
#include "sxs_type.h"
#include "cmn_defs.h"
#include "itf_api.h"

#define HVY_STCK ( HVY_API + 0xFF00 )

/*
** Primitive ID definitions
*/
// API -> MM , RR
#define STK_SIM_OPEN_IND      ( HVY_STCK + 0  )
#define STK_SIM_CLOSED_IND    ( HVY_STCK + 1  )

#define STK_START_REQ         ( HVY_STCK + 2  )
#define STK_STOP_REQ          ( HVY_STCK + 3  )
#define STK_STOP_CNF          ( HVY_STCK + 4  )

#define STK_SIM_CLOSE_CNF          ( HVY_STCK + 5  )



/*
================================================================================
  Structure  : stk_StartReq_t
--------------------------------------------------------------------------------
  Direction  : API -> RR 
  Scope      : Starts the protocol stack
  Comment    : This body is only used for RR. 
================================================================================
*/
typedef api_StartReq_t stk_StartReq_t ;  

/*
================================================================================
  Structure  : stk_StopCnf_t
--------------------------------
  Direction  : RR -> API or MM -> API
  Scope      : The different layers confirm the STK_STOP_REQ
  Comment    : 
================================================================================
*/
typedef struct 
{
 u8 CnfFsm;
} stk_StopCnf_t ;

#define STK_CNFFSM_MMC ( 1 << 0 )
#define STK_CNFFSM_MMP ( 1 << 1 )
#define STK_CNFFSM_RR ( 1 << 2 )

#endif
