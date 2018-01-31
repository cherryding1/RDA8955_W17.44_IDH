//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_iitf.h,v 1.2 2006/03/08 00:18:10 mce Exp $
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
  File       : rlu_iitf.h
--------------------------------------------------------------------------------

  Scope      : Internal Interface messages from RLU to RLU

  History    :
--------------------------------------------------------------------------------
  Jun 17 06  |  MCE   | Added RLU_TPURGE 
  Feb 10 04  |  MCE   | Creation            
================================================================================
*/

#ifndef __RLU_IITF_H__
#define __RLU_IITF_H__

#include "sxs_type.h"     // Comon definitions for standards types
#include "itf_msgc.h"     // Definition of Bases

/*
** internal messages
*/
#define RLU_STARTTBF_REQ         ( RLU_INTRA_BASE + 0 )
#define RLU_CONTINUE_TX          ( RLU_INTRA_BASE + 1 )
#define RLU_REPEAT_REQ           ( RLU_INTRA_BASE + 2 )
#define RLU_TXFILL_REQ           ( RLU_INTRA_BASE + 3 )


/*
** timers
*/
#define RLU_T3182                ( SXS_TIMER_EVT_ID + RLU_INTRA_BASE + 0x80 )
#define RLU_TREADY               ( SXS_TIMER_EVT_ID + RLU_INTRA_BASE + 0x81 )
#define RLU_TPURGE               ( SXS_TIMER_EVT_ID + RLU_INTRA_BASE + 0x82 )

/*
================================================================================
  Primitive  : rlu_RepeatReq_t                                  
--------------------------------------------------------------------------------

  Scope      : RLU wants to repeat the NACKED PDUs with 
               block nb >= Bsn and < Vs

================================================================================
*/
/*
================================================================================
  Primitive  : rlu_TxFillReq_t                                  
--------------------------------------------------------------------------------

  Scope      : RLU wants to repeat the ACK_PENDING PDUs with 
               block nb >= Bsn and < Vs

================================================================================
*/
typedef struct
{
 u32 Bsn      ; // Bsn of the block we want to repeat
} rlu_RepeatReq_t,
  rlu_TxFillReq_t;

#endif

