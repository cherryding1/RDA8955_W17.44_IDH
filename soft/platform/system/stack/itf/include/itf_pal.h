//------------------------------------------------------------------------------
//  $Log: itf_pal.h,v $
//  Revision 1.3  2006/03/09 09:34:32  fch
//  Update with bob for v2.20
//
//  Revision 1.2  2005/12/29 17:38:44  pca
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
  File       : ITF_PAL.H
--------------------------------------------------------------------------------

  Scope      : Interface provided by PAL to L2 and L3
  History    :
--------------------------------------------------------------------------------
  Apr 06 04  |  FCH   | Creation
  Mar 03 06  |  ADA   | Keypad handling
================================================================================
*/

#ifndef __ITF_PAL_H__
#define __ITF_PAL_H__
#include "pal_gsm.h"

#define PAL_KC_SIZE       8


#define PAL_MS_TO_SGSN    0
#define PAL_SGSN_TO_MS    1


#define HVY_PAL_IN              (HVY_PAL + 0   )
#define HVY_PAL_OUT             (HVY_PAL + 0x80)

#define PAL_KEYPAD_IND          (HVY_PAL_IN + 0)



#define PAL_KEY_PRESSED  0
#define PAL_KEY_RELEASED 1

typedef struct
{
 u8 KeyCode;
 u8 Event;       // PAL_KEY_PRESSED or PAL_KEY_RELEASED
 u8 Pad [2];
} pal_KeypadInd_t;




#endif  // __ITF_PAL_H__
