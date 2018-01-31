//------------------------------------------------------------------------------
//  $Log: mm_func.h,v $
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
  File       : mm_func.h
--------------------------------------------------------------------------------

  Scope      : Header file for the event MM functions

  History    :
--------------------------------------------------------------------------------
  Apr 12 04  |  OTH   | Creation            
================================================================================
*/

#ifndef __MM_FUNC_H__
#define __MM_FUNC_H__

// System includes
#include "sxs_type.h"

void mm_SimOpenInd   ( void );
void mm_SimClosedInd ( void );
void mm_CellInd      ( void );
void mm_SimUpdBinCnf ( void );
void mm_CmkInd       ( void );
void mm_PagingInd    ( void );
bool mm_PagingCheck (void);
void mm_StkStartReq(void) ;    // DMX MOD, 20140721, the STK_START_REQ to RR will forward by MM


//==========
// MM macros
//==========
#define MM_PAG_REQ1_MT 0x21
#define MM_PAG_REQ2_MT 0x22
#define MM_PAG_REQ3_MT 0x24

#define MM_MOBILE_ID_IEI 0x17
#define MM_TMSI_IE_TAG ( 1 << 7 )
#define MM_ID_GPRS     ( 1 << 3 )


#endif // __MM_FUNC_H__
