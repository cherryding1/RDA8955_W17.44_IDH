//------------------------------------------------------------------------------
//  $Log: sm_iitf.h,v $
//  Revision 1.3  2006/05/22 19:35:41  aba
//  Update of the copyright
//
//  Revision 1.2  2005/12/29 17:39:17  pca
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
  File       : SM_IITF.H
--------------------------------------------------------------------------------

  Scope      : Internal Interfaces for SM

  History    :
--------------------------------------------------------------------------------
  Jun 22 06  |  PCA   | Creation            
================================================================================
*/
#ifndef __SM_IITF_H__
#define __SM_IITF_H__

#include "itf_sm.h"

/*
** Internal Event definitions
*/

// Timer definition : One Timer per NSAPI => 11 Timers
#define T3380_VALUE     30
#define T3381_VALUE     8
#define T3390_VALUE     8

//SM_INTRA_BASE   0xF8000
#define SM_T3380_BASE (SM_INTRA_BASE + 0x80)
#define SM_T3381_BASE (SM_INTRA_BASE + 0x90)
#define SM_T3390_BASE (SM_INTRA_BASE + 0xA0)

#define SM_T3380 ((SM_T3380_BASE + 0) | SXS_TIMER_EVT_ID)
#define SM_T3381 ((SM_T3381_BASE + 0) | SXS_TIMER_EVT_ID)
#define SM_T3390 ((SM_T3390_BASE + 0) | SXS_TIMER_EVT_ID)

/*
   ===========================
   Derived Event definitions
   ===========================
*/
#define MMP_CTXACTIVATE_ACC       (SM_INTRA_BASE + 1)
#define MMP_CTXACTIVATE_REJ       (SM_INTRA_BASE + 2)
#define MMP_CTXACTIVATESEC_ACC    (SM_INTRA_BASE + 3)
#define MMP_CTXACTIVATESEC_REJ    (SM_INTRA_BASE + 4)
#define MMP_REQCTXACTIVATE        (SM_INTRA_BASE + 5)
#define MMP_CTXMODIFY_REQ         (SM_INTRA_BASE + 6)
#define MMP_CTXMODIFY_ACC         (SM_INTRA_BASE + 7)
#define MMP_CTXMODIFY_REJ         (SM_INTRA_BASE + 8)
#define MMP_CTXDEACTIVATE_REQ     (SM_INTRA_BASE + 9)
#define MMP_CTXDEACTIVATE_ACC     (SM_INTRA_BASE + 10)
#define MMP_SMSTATUS_IND          (SM_INTRA_BASE + 11)

#endif
