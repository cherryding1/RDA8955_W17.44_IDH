//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_cod.h,v 1.2 2006/03/08 00:18:10 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : rlu_cod.h 
--------------------------------------------------------------------------------

  Scope      : Defintion of Constants and Macro for RLU frame encoding

  History    :
--------------------------------------------------------------------------------
  Feb 06 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLU_COD_H__
#define __RLU_COD_H__
#include "cmn_dec.h"

/* 
** Packet Channel Request Description
*/
#define RLU_PUT_PTYP(val)     CMN_PUT(val, 0, CMN_MASKL1)   // Pdu type
#define RLU_PUT_MODE(val)     CMN_PUT(val, 1, CMN_MASKL1)   // Rlc mode
#define RLU_PUT_PRIO(val)     CMN_PUT(val, 2, CMN_MASKL2)   // Radio priority
#define RLU_PUT_CLAS(val)     CMN_PUT(val, 4, CMN_MASKL4)   // Thrugput class
#define RLU_PUT_BSN(val)      CMN_PUT(val, 1, CMN_MASKL7)   // BSN
#define RLU_PUT_PFI(val)      CMN_PUT(val, 1, CMN_MASKL7)   // PFI
#define RLU_PUT_E(val)        CMN_PUT(val, 0, CMN_MASKL1)   // E bit
#define RLU_PUT_M(val)        CMN_PUT(val, 1, CMN_MASKL1)   // M bit
#define RLU_PUT_LI(val)       CMN_PUT(val, 2, CMN_MASKL6)   // Length Indicator

/*
** Ack/Nack Description
*/
#define RLU_GET_FAI(val)      CMN_GET(val, 7, CMN_MASKL1)   // Final Ack Ind
#define RLU_GET_SSN(val)      CMN_GET(val, 0, CMN_MASKL7)   // Start Seq Nb
/*
** Values used to encode some of RLU's header fields
*/
#define RLU_EXT_PRESENT       0
#define RLU_EXT_ABSENT        1
#define RLU_MORE_OFF          0
#define RLU_MORE_ON           1

#endif	

