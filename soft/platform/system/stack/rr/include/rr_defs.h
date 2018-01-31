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
  File       : RR_DEFS.H
--------------------------------------------------------------------------------

  Scope      : defines and typedefs for several RR subentities

  History    :
--------------------------------------------------------------------------------
  Sep 23 05  |  MCE   | Moved rr_SubBaL_t ant rr_CaL_t from rr_cell.h
  Jun 22 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RR_DEFS_H__
#define __RR_DEFS_H__
#include "itf_l1.h"

#define RR_MA_SIZE L1_MA_SIZE

typedef struct {
   u8             Nb        ;       // Arfcn Nb in a Sub list  
   u8             Raw[16]   ;       // Save of Sub list as broadcasted
   u16          * ArfcnL    ;       // Expanded Arfcn list
} rr_SubBaL_t ;

typedef rr_SubBaL_t rr_CaL_t ;

#endif // __RR_DEFS_H__

