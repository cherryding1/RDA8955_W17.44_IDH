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
  File       : rri_psel.h
--------------------------------------------------------------------------------

  Scope      : Defintion of strcutures for Plmn Selection    

  History    :
--------------------------------------------------------------------------------
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RRI_PSEL_H__
#define __RRI_PSEL_H__

#include "itf_l1.h"       // ArfRssi_t definition

/*
** Available Plmn 
*/

#define RR_PLMN_ARFCN  20

typedef struct
{
   u8           PlmnId [3]              ;   // Plmn Code
   u16          ARssiNb	                ;   // Number of Elem in ARssiL
   ArfRssi_t    ARssiL[ RR_PLMN_ARFCN ] ;   // List of (Arfcn, Measure)

} rr_APlmn_t; 


#endif

