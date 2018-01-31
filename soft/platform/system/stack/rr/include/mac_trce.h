//------------------------------------------------------------------------------
//  $Log: mac_trce.h,v $
//  Revision 1.5  2006/05/22 14:36:19  oth
//  Update the copyright
//
//  Revision 1.4  2006/04/04 07:18:54  oth
//  Add a MAC_BZD_TRC trace level
//
//  Revision 1.3  2005/12/30 17:45:40  oth
//  Remove the MAC trace level and merge MAC traces with RRD
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

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : mac_trce.h
--------------------------------------------------------------------------------

  Scope      : Defintion of Trace Level for mac  

  History    :
--------------------------------------------------------------------------------
  Oct 14 03  |  OTH   | Creation            
================================================================================
*/
#ifndef __MAC_TRCE_H__
#define __MAC_TRCE_H__

#include "sxs_io.h"
#include "cmn_defs.h"

#include "rr_trce.h"

#define  MAC_IN_TRC      (_RRD | CMN_IN_TRC)   // Incoming messages
#define  MAC_DIN_TRC     (_RRD | CMN_DIN_TRC)  // Dump incoming messages
#define  MAC_OUT_TRC     (_RRD | CMN_OUT_TRC)  // Outgoing messages
#define  MAC_DOUT_TRC    (_RRD | CMN_DOUT_TRC) // Dump outcoming messages
#define  MAC_FSM_TRC     (_RRD | CMN_FSM_TRC)  // Not used by MAC (no FSM)
#define  MAC_DRMNIN_TRC  (_RRD | CMN_DRMIN_TRC)
#define  MAC_DRMNOUT_TRC (_RRD | CMN_DRMOUT_TRC)
#define  MAC_ENDEC_TRC   RR_CODEC_TRC
#define  MAC_RM_TRC      (_RRD | TLEVEL(8 ))
#define  MAC_RLC_TRC     (_RRD | TLEVEL(9 ))
#define  MAC_L1_TRC      (_RRD | TLEVEL(10))
#define  MAC_ID_TRC      RRD_COND_TRC
#define  MAC_BZD_TRC     (_RRD | TLEVEL(16))

#endif
