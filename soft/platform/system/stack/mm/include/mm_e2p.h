//------------------------------------------------------------------------------
//  $Log: mm_e2p.h,v $
//  Revision 1.3  2006/05/23 10:51:30  fch
//  Update Copyright
//
//  Revision 1.2  2005/12/29 17:38:47  pca
//  Log keyword added to all files
//
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of Coolsand. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright Coolsand (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : mm_e2p.h
--------------------------------------------------------------------------------

  Scope      : E2P stuff for MM

  History    :
--------------------------------------------------------------------------------
  Aug 11 04  |  FCH   | Creation            
================================================================================
*/

#ifndef __MM_E2P_H__
#define __MM_E2P_H__

// System includes
#include "sxs_type.h"


// Structure to map on the global GsimUnaware variable
typedef struct
{
  u8 IMSI[9];

    // !! Do not separate the 2 following items
  u8 KcGprs[8];
  u8 KcGprsCksn;

    // !! Do not separate the 4 following items
  u8 PTmsi[4];
  u8 PTmsiSign[3];
  u8 RAI[6];
  u8 RAUStatus;
}mmp_SIM_t;

extern u8 GsimUnaware[32];

#define API_MSNWKCAP_SIZE 9
#define API_MSRACAP_SIZE  52

#if !(STK_MAX_SIM==1)
extern u8  api_MsNwkCap[STK_MAX_SIM][API_MSNWKCAP_SIZE];   // 24.008
extern u8  api_MsRACap[STK_MAX_SIM][API_MSRACAP_SIZE];     // 24.008
extern u16 api_LenMsRACap[STK_MAX_SIM];
#else
extern u8  api_MsNwkCap[];   // 24.008
extern u8  api_MsRACap[];     // 24.008
extern u16 api_LenMsRACap;
#endif

#endif // __MM_E2P_H__
