/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : MMC_TRC.C
--------------------------------------------------------------------------------

  Scope      : Defintion of MMC Trace Levels  

  History    :
--------------------------------------------------------------------------------
  May 10 05  |  FCH   | Creation            
================================================================================
*/
#ifndef __MMC_TRC_H__
#define __MMC_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define MM_IN_TRC       ( _MM | CMN_IN_TRC)     // Incoming messages
#define MM_DIN_TRC      ( _MM | CMN_DIN_TRC)    // Dump incoming messages
#define MM_OUT_TRC      ( _MM | CMN_OUT_TRC)    // Outgoing messages
#define MM_DOUT_TRC     ( _MM | CMN_DOUT_TRC)   // Dump outcoming messages
#define MM_FSM_TRC      ( _MM | CMN_FSM_TRC)    // Finite State Machine
#define MM_DRMIN_TRC    ( _MM | CMN_DRMIN_TRC)  // Dump incomming Radio Msg
#define MM_DRMOUT_TRC   ( _MM | CMN_DRMOUT_TRC) // Dump outgoing Radio Msg

#define MM_SIM_TRC      ( _MM | TLEVEL(8)  )    // SIM
#define MM_CELL_TRC     ( _MM | TLEVEL(9)  )    // Cell
#define MM_RM_TRC       ( _MM | TLEVEL(10) )    // Radio Message
#define MM_OTHER_TRC    ( _MM | TLEVEL(11) )

#define MMC_LOCUP_TRC   ( _MM | TLEVEL(12) )
#define MMC_CALL_TRC    ( _MM | TLEVEL(13) )

#define MMP_TIM_TRC     ( _MM | TLEVEL(14) )    // MMP Timers
#define MMP_CMNPROC_TRC ( _MM | TLEVEL(15) )    // MMP Common MP proc
#define MMP_SPEPROC_TRC ( _MM | TLEVEL(16) )    // MMP Specific MMP proc


#endif /* endif __MMC_TRC_H__ */
