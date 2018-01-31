//------------------------------------------------------------------------------
//  $Log: sim_trc.h,v $
//  Revision 1.3  2006/05/22 20:51:18  aba
//  Update of the copyright
//
//  Revision 1.2  2005/12/29 17:39:16  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 2004 2005 2006

================================================================================
*/

/*
================================================================================
  File       : SIM_TRC.H
--------------------------------------------------------------------------------

  Scope      : Defintion of SIM Trace Levels

  History    :
--------------------------------------------------------------------------------
  03 12 04   |  ABA   | Creation
================================================================================
*/
#ifndef __SIM_TRC_H__
#define __SIM_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define  SIM_IN_TRC           (_SIM | CMN_IN_TRC)   // Incoming messages
#define  SIM_DIN_TRC          (_SIM | CMN_DIN_TRC)  // Dump incoming messages
#define  SIM_OUT_TRC          (_SIM | CMN_OUT_TRC)  // Outgoing messages
#define  SIM_DOUT_TRC         (_SIM | CMN_DOUT_TRC) // Dump outcoming messages
#define  SIM_FSM_TRC          (_SIM | CMN_FSM_TRC)  // Finite State Machine

#define  SIM_RESET_TRC        (_SIM | TLEVEL(6))    // Sim reset
#define  SIM_ERROR_TRC        (_SIM | TLEVEL(7))    // Sim error management (analysis of SW1, SW2)
#define  SIM_TIMER_TRC        (_SIM | TLEVEL(8))    // Periodical status
#define  SIM_TOOLKIT_TRC      (_SIM | TLEVEL(9))    // Simtoolkit (pro-active sim)
#define  SIM_SUCCESS_TRC      (_SIM | TLEVEL(10))   // Successfull sim transaction
#define  SIM_DUMPRXDATA_TRC   (_SIM | TLEVEL(11))   // Dump received data from sim
#define  SIM_INSTRUCTION_TRC  (_SIM | TLEVEL(12))   // SIM Instruction
#define  SIM_SEMAPHORE_TRC    (_SIM | TLEVEL(13))   // SIM sùmephore
#endif /* endif __SIM_TRC_H__ */

