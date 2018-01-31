//================================================================================
//  File       : SMS_TRC.H
//--------------------------------------------------------------------------------
//  Scope      : Defintion of SMS Trace Levels
//  History    :
//--------------------------------------------------------------------------------
//  2006/01/16   aba  Add of traces for debugging purpose
//  2005/12/29   pca  Log keyword added to all files
//  Sep 27 04  |  ABA   | Creation
//================================================================================

#ifndef __SMS_TRC_H__
#define __SMS_TRC_H__

#include "sxs_io.h"     // Trace constants
#include "cmn_defs.h"   // Common definitions



#define  SMS_IN_TRC     (_SMS | CMN_IN_TRC)     // Incoming messages
#define  SMS_DIN_TRC    (_SMS | CMN_DIN_TRC)    // Dump incoming messages
#define  SMS_OUT_TRC    (_SMS | CMN_OUT_TRC)    // Outgoing messages
#define  SMS_DOUT_TRC   (_SMS | CMN_DOUT_TRC)   // Dump outcoming messages
#define  SMS_FSM_TRC    (_SMS | CMN_FSM_TRC)    // Finite State Machine
#define  SMS_DRMIN_TRC  (_SMS | CMN_DRMIN_TRC)  // Dump incomming Radio Msg
#define  SMS_DRMOUT_TRC (_SMS | CMN_DRMOUT_TRC) // Dump outgoing Radio Msg

#define  SMS_API_TRC    (_SMS | TLEVEL(8))      // Trace api interface
#define  SMS_MO_TRC     (_SMS | TLEVEL(9))      // Mobile originated sms
#define  SMS_MT_TRC     (_SMS | TLEVEL(10))     // Mobile terminated sms
#define  SMS_MMA_TRC    (_SMS | TLEVEL(11))     // SMS Memory available
#define  SMS_ERROR_TRC  (_SMS | TLEVEL(12))     // Trace of abnormal processing (bad reeived sms ...)

///////////////////////////////////////////////////////////////////////////////////////
// !!!!! LEVELS 13 to 16 ARE RESERVED FOR SMS CB (file cb_trc.h in CB directory)
///////////////////////////////////////////////////////////////////////////////////////

#endif /* endif __SMS_TRC_H__ */
