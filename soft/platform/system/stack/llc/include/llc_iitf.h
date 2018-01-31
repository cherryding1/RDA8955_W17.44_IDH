//------------------------------------------------------------------------------
//  $Log: llc_iitf.h,v $
//  Revision 1.3  2006/05/23 10:36:46  fch
//  Update Copyright
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
  File       : LLC_IITF.H
--------------------------------------------------------------------------------

  Scope      : Defintion of LLC internal events

  History    :
--------------------------------------------------------------------------------
  Jul 01 04  |  FCH   | Creation
================================================================================
*/
#ifndef __LLC_IITF_H__
#define __LLC_IITF_H__

#include "itf_llc.h"


      //-------------------------------------------------------
      //  Primitive Id for derived event
      //-------------------------------------------------------
#define LLC_RX_I                    ( LLC_INTRA_BASE + 0  )
#define LLC_RX_S                    ( LLC_INTRA_BASE + 1  )
#define LLC_RX_UI                   ( LLC_INTRA_BASE + 2  )
#define LLC_RX_U_DM                 ( LLC_INTRA_BASE + 3  )
#define LLC_RX_U_DISC               ( LLC_INTRA_BASE + 4  )
#define LLC_RX_U_UA                 ( LLC_INTRA_BASE + 5  )
#define LLC_RX_U_SABM               ( LLC_INTRA_BASE + 6  )
#define LLC_RX_U_FRMR               ( LLC_INTRA_BASE + 7  )
#define LLC_RX_U_XID                ( LLC_INTRA_BASE + 8  )
#define LLC_RX_U_UNDEF              ( LLC_INTRA_BASE + 9  )

#define LLC_UNASSIGN_REQ            ( LLC_INTRA_BASE + 10 )
#define LLC_XID_RESET_SLAVE         ( LLC_INTRA_BASE + 11 )
#define LLC_NULL_FRM_TO_SEND        ( LLC_INTRA_BASE + 12 )

  /*
  ========================================================================
  =   T200 definition (U frame retransmission timer)
  =   There is one T200 per LLE, but not for Sapi 1 & 7
  =   A T200 occurence is identified by : LLC_T200_BASE + (Sapi-1)/2
  ========================================================================
  */
#define LLC_T200_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 32)
//#define LLC_T200_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 33)
//#define LLC_T200_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 34)
//#define LLC_T200_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 35)
//#define LLC_T200_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 36)
#define LLC_T200_LAST               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 37)

#define LLC_T200_TIM                LLC_T200_BASE


  /*
  ========================================================================
  =   T201 definition (I frame retransmission timer)
  =   There is one T201 per LLE, but not for Sapi 1 & 7
  =   A T201 occurence is identified by : LLC_T201_BASE + (Sapi-1)/2
  ========================================================================
  */
#define LLC_T201_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 64)
//#define LLC_T201_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 65)
//#define LLC_T201_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 66)
//#define LLC_T201_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 67)
//#define LLC_T201_BASE               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 68)
#define LLC_T201_LAST               ( SXS_TIMER_EVT_ID + LLC_INTRA_BASE + 69)

#define LLC_T201_TIM                LLC_T201_BASE

#endif // __LLC_IITF_H__
