//------------------------------------------------------------------------------
//  $Log: rlp_trc.h,v $
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
  File       : RLP_TRC.C
--------------------------------------------------------------------------------

  Scope      : Defintion of RLP Trace Levels  

  History    :
--------------------------------------------------------------------------------
  Mar 31 05  |  FCH   | Creation            
================================================================================
*/
#ifndef __RLP_TRC_H__
#define __RLP_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define  RLP_IN_TRC     (_RLP | CMN_IN_TRC)     // Incoming messages
#define  RLP_DIN_TRC    (_RLP | CMN_DIN_TRC)    // Dump incoming messages
#define  RLP_OUT_TRC    (_RLP | CMN_OUT_TRC)    // Outgoing messages
#define  RLP_DOUT_TRC   (_RLP | CMN_DOUT_TRC)   // Dump outcoming messages
#define  RLP_FSM_TRC    (_RLP | CMN_FSM_TRC)    // Finite State Machine
#define  RLP_DRMIN_TRC  (_RLP | CMN_DRMIN_TRC)  // Incomming Radio Msg dump
#define  RLP_DRMOUT_TRC (_RLP | CMN_DRMOUT_TRC) // Outgoing Radio Msg dump

#define  RLP_OTHER_TRC  (_RLP | TLEVEL(8))      // Various traces
#define  RLP_FRM_TRC    (_RLP | TLEVEL(9))      // Frame
#define  RLP_TIM_TRC    (_RLP | TLEVEL(10))     // RLP timers
#define  RLP_XID_TRC    (_RLP | TLEVEL(11))     // XID parameters
#define  RLP_ABM_TRC    (_RLP | TLEVEL(12))     // ABM purpose
#define  RLP_FLOW_TRC   (_RLP | TLEVEL(13))     // Flow control

#define  RLP_SP14_TRC   (_RLP | TLEVEL(14))     // Level 14
#define  RLP_SP15_TRC   (_RLP | TLEVEL(15))     // Level 15
#define  RLP_SP16_TRC   (_RLP | TLEVEL(16))     // Level 16

#endif /* __RLP_TRC_H__ */
