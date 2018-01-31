//------------------------------------------------------------------------------
//  $Log: itf_msgc.h,v $
//  Revision 1.4  2006/05/09 17:46:48  fch
//  Update with Bob REF v2.22
//
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
  File       : itf_msgc.h
--------------------------------------------------------------------------------

  Scope      : Definition of Common Messages parts

  History    :
--------------------------------------------------------------------------------
  Mar 09 06  |  ADA   | added HVY_PAL
  Nov 27 05  |  PFA   | added HVY_MMI
  Nov 23 05  |  FCH   | added HVY_RLP
  Sep 22 05  |  FCH   | added HVY_M2A
  Apr 21 05  |  MCE   | added HVY_MSK
  Apr 06 04  |  MCE   | moved SXS_TIMER_EVT_ID from sxs_cfg.h
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __ITF_MSGC_H__
#define __ITF_MSGC_H__

#include "cmn_defs.h"

/*
** Base for Timer Event Ids
*/
#define SXS_TIMER_EVT_ID  0x40000000

/*
** Heavy Part for Msg ID (heavy byte of u16) 
*/
#define HVY_STT ( CMN_STT << 16 )
#define HVY_PAL ( CMN_PAL << 16 )
#define HVY_L1  ( CMN_L1  << 16 )
#define HVY_RR  ( CMN_RR  << 16 )
#define HVY_LAP ( CMN_LAP << 16 )
#define HVY_MM  ( CMN_MM  << 16 )
#define HVY_CC  ( CMN_CC  << 16 )
#define HVY_SS  ( CMN_SS  << 16 )
#define HVY_SMS ( CMN_SMS << 16 )
#define HVY_CM  ( CMN_CM  << 16 )
#define HVY_SIM ( CMN_SIM << 16 )
#define HVY_API ( CMN_API << 16 )
#define HVY_RLU ( CMN_RLU << 16 )
#define HVY_RLD ( CMN_RLD << 16 )
#define HVY_LLC ( CMN_LLC << 16 )
#define HVY_SND ( CMN_SND << 16 )
#define HVY_SM  ( CMN_SM  << 16 )
#define HVY_CB  ( CMN_CB  << 16 )
#define HVY_M2A ( CMN_M2A << 16 )
#define HVY_RLP ( CMN_RLP << 16 )
#define HVY_SMT ( CMN_SMT << 16 )
#ifdef LTE_NBIOT_SUPPORT
#define HVY_OSITIMER  ( CMN_OSITIMER << 16 )
#endif

/*
** MMI and Application Identifdiers must use bases >= HVY_MMI
*/
#define HVY_MMI 0x80000000





#define HVY_MSK 0xFFFF0000


/*
** All of these parts willbe combined is each task
** To create the complete message once they have defined their own body
*/

typedef struct 
{
   u32    Id ;        // Identifier Unique for whole stack

/* # Added by LIUBO 2008.10.20 begin */   
#if (STK_MAX_SIM != 1)
/* Used to identify which instance the message belongs to */
/* 0, instance 0; 1, instance 1 */
   u32    SimId;
#endif
/* # Added by LIUBO 2008.10.20 end */    
   
}MsgHead_t;

#endif

