//------------------------------------------------------------------------------
//  $Log: lap_iitf.h,v $
//  Revision 1.3  2006/05/22 12:46:12  aba
//  Copyrigth is updated
//
//  Revision 1.2  2005/12/29 17:38:45  pca
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
  File       : LAP_IITF.H
--------------------------------------------------------------------------------

  Scope      : Defintion of LAPDm internal events

  History    :
--------------------------------------------------------------------------------
  Sep 27 03  |  ABA   | Creation
================================================================================
*/
#ifndef __LAP_IITF_H__
#define __LAP_IITF_H__

#include "itf_lap.h"

#define LAP_RX_UA_FRAME              ( LAP_INTRA_BASE + 0 )
#define LAP_RX_DM_FRAME              ( LAP_INTRA_BASE + 1 )
#define LAP_RX_DISC_FRAME            ( LAP_INTRA_BASE + 2 )
#define LAP_RX_RR_FRAME              ( LAP_INTRA_BASE + 3 )
#define LAP_RX_REJ_FRAME             ( LAP_INTRA_BASE + 4 )
#define LAP_RX_SABM_FRAME            ( LAP_INTRA_BASE + 5 )
#define LAP_RX_I_FRAME               ( LAP_INTRA_BASE + 6 )
#define LAP_RX_UI_FRAME              ( LAP_INTRA_BASE + 7 )
#define LAP_SAPI0_T200_EXP          (( LAP_INTRA_BASE + 8 ) | SXS_TIMER_EVT_ID )
#define LAP_SAPI0_LAST_T200_EXP     (( LAP_INTRA_BASE + 9 ) | SXS_TIMER_EVT_ID )
#define LAP_SAPI3_T200_EXP          (( LAP_INTRA_BASE + 10) | SXS_TIMER_EVT_ID )
#define LAP_SAPI3_LAST_T200_EXP     (( LAP_INTRA_BASE + 11) | SXS_TIMER_EVT_ID )

#endif
