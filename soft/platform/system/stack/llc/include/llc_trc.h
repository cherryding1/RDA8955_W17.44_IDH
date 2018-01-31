//------------------------------------------------------------------------------
//  $Log: llc_trc.h,v $
//  Revision 1.4  2006/05/23 10:36:46  fch
//  Update Copyright
//
//  Revision 1.3  2006/02/28 10:37:02  fch
//  Traces cleaning
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
  File       : LLC_TRC.C
--------------------------------------------------------------------------------

  Scope      : Defintion of LLC Trace Levels  

  History    :
--------------------------------------------------------------------------------
  Mar 03 04  |  FCH   | Creation            
================================================================================
*/
#ifndef __LLC_TRCE_H__
#define __LLC_TRCE_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define  LLC_IN_TRC     (_LLC | CMN_IN_TRC)     // Incoming messages
#define  LLC_DIN_TRC    (_LLC | CMN_DIN_TRC)    // Dump incoming messages
#define  LLC_OUT_TRC    (_LLC | CMN_OUT_TRC)    // Outgoing messages
#define  LLC_DOUT_TRC   (_LLC | CMN_DOUT_TRC)   // Dump outcoming messages
#define  LLC_FSM_TRC    (_LLC | CMN_FSM_TRC)    // Finite State Machine
#define  LLC_DRMIN_TRC  (_LLC | CMN_DRMIN_TRC)  // Dump Incomming Radio Msg 
#define  LLC_DRMOUT_TRC (_LLC | CMN_DRMOUT_TRC) // Dump Outgoing Radio Msg 

#define  LLC_MMP_TRC    (_LLC | TLEVEL(8))      // MMP stuff
#define  LLC_CTX_TRC    (_LLC | TLEVEL(9))      // LLE context managment
#define  LLC_FRM_TRC    (_LLC | TLEVEL(10))     // LLC Frame
#define  LLC_XID_TRC    (_LLC | TLEVEL(11))     // XID parameters
#define  LLC_CIPH_TRC   (_LLC | TLEVEL(12))     // Ciphering
#define  LLC_TIM_TRC    (_LLC | TLEVEL(13))     // Timers
#define  LLC_SND_TRC    (_LLC | TLEVEL(14))     // SNDCP stuff

#define  LLC_SP15_TRC   (_LLC | TLEVEL(15))     // Level 15

#define  LLC_DBG_TRC    (_LLC | TLEVEL(16))     // LLC debug



#endif /* endif __LLC_TRCE_H__ */
