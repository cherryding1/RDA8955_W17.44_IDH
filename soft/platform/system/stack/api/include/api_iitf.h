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
  File       : api_iitf.h
--------------------------------------------------------------------------------

  Scope      : Internal Interface messages to API

  History    :
--------------------------------------------------------------------------------
  May 23 06  |  MCE   | Creation            
================================================================================
*/

#ifndef __API_IITF_H__
#define __API_IITF_H__

#include "sxs_type.h"     // Common definitions for standards types
#include "itf_msgc.h"     // Definition of Bases

#define API_INTRA_BASE      HVY_API_RESERVED


#define API_MEM_ALERT_EXP ( SXS_TIMER_EVT_ID + API_INTRA_BASE + 0x00 )

#endif
