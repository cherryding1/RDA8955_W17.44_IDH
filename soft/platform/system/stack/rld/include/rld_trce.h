//------------------------------------------------------------------------------
//  $Log: rld_trce.h,v $
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
  File       : rld_trce.h
--------------------------------------------------------------------------------

  Scope      : Defintion of Trace Level for rld  

  History    :
--------------------------------------------------------------------------------
  Mar 04 04  |  OTH   | Creation            
================================================================================
*/
#ifndef __RLD_TRCE_H__
#define __RLD_TRCE_H__

#include "sxs_io.h"
#include "cmn_defs.h"

#define RLD_IN_TRC    (_RLD | CMN_IN_TRC)   // Incoming messages
#define RLD_DIN_TRC   (_RLD | CMN_DIN_TRC)  // Dump incoming messages
#define RLD_OUT_TRC   (_RLD | CMN_OUT_TRC)  // Outgoing messages
#define RLD_DOUT_TRC  (_RLD | CMN_DOUT_TRC) // Dump outcoming messages
#define RLD_FSM_TRC   (_RLD | CMN_FSM_TRC)  // Finite state machine
#define RLD_DBG_TRC   (_RLD | TLEVEL(6))    // Debug trace level
#define RLD_PDU_TRC   (_RLD | TLEVEL(7))    // Handling of the PDU
#define RLD_SDU_TRC   (_RLD | TLEVEL(8))    // Handling of the SDU
#define RLD_CTRL_TRC  (_RLD | TLEVEL(9))    // Control traces

#endif
