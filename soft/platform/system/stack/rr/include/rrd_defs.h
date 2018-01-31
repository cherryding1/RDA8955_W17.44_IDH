/*
================================================================================

  This source code is
								    property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : RRD_DEFS.H
--------------------------------------------------------------------------------

  Scope      : define utilities for RRD

  History    :
--------------------------------------------------------------------------------
  Aug 22 03  |  MCE   | Creation            
================================================================================
*/
#ifndef __RRD_DEFS_H__
#define __RRD_DEFS_H__
#include "rr_capa.h"
#include "rr_defs.h"

// RR internal defines for RR cause for release
// used by rrd_EndRrConnection
#define       RRG_L1_CON_END_CAUSE    1 // values for RrCause
#define       RRG_LAP_CON_END_CAUSE   2 
#define       RRG_ABORT_CAUSE         3
#endif // __RRD_DEFS_H__

// Macro to determine if received Channel mode is Data or not
#define       RRG_CHAN_MODE_DATA(_Mode) \
              ( (_Mode != L1_SIG_ONLY) && \
                (_Mode != L1_SPEECH_V1)&& \
                (_Mode != L1_SPEECH_V2)&& \
                (_Mode != L1_SPEECH_V3) )


