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
 File          l1s_spy.h
--------------------------------------------------------------------------------

  Scope      : L1 Spy window definition.

  History    :
--------------------------------------------------------------------------------
  Jan 15 03  |  ADA  | Creation
================================================================================
*/

//#ifndef __TARGET__
//#define __L1S_SPY__
//#endif

#ifdef __L1S_SPY__

#ifndef __L1S_SPY_H__
#define __L1S_SPY_H__

#include "sxs_type.h"
#include "sxs_spy.h"

#ifdef __L1S_SPY_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

#define L1S_NB_SERVING_SPY 31
#define L1S_NB_NEAR_SPY    5*6
#define L1S_EXTRA_NEAR_SPY 1

DefExtern u16 l1s_ServingSpyData [L1S_NB_SERVING_SPY];
DefExtern u16 l1s_NearSpyData    [L1S_NB_NEAR_SPY + 5*L1S_EXTRA_NEAR_SPY];

#define l1s_SetServingSpyArfcn(a)   l1s_ServingSpyData [0] = a
#define l1s_SetServingSpyPwr(a)     l1s_ServingSpyData [1] = a
#define l1s_SetServingSpyRla(a)     l1s_ServingSpyData [2] = a
#define l1s_SetServingSpyAgc(a)     l1s_ServingSpyData [3] = a
#define l1s_SetServingSpyPAgc(a)    l1s_ServingSpyData [4] = a
#define l1s_SetServingSpyFOf(a)     l1s_ServingSpyData [5] = a
#define l1s_SetServingSpyTOf(a)     l1s_ServingSpyData [6] = a
#define l1s_SetServingSpySnr(a)     l1s_ServingSpyData [7] = a
#define l1s_SetServingSpyTxP(a)     l1s_ServingSpyData [8] = a
#define l1s_SetServingSpyPTxP(a,n)  l1s_ServingSpyData [8+(n & 3)] = a
#define l1s_SetServingSpyC(a)       l1s_ServingSpyData [12] = a
#define l1s_SetServingSpyI(a,n)     l1s_ServingSpyData [13+(n & 3)] = a
#define l1s_SetServingSpyChn(a,Chn) l1s_ServingSpyData [17 + Chn - L1_BCCH_ID] = a
#define l1s_IncServingSpyChn(Chn)   l1s_ServingSpyData [17 + Chn - L1_BCCH_ID]++
#define l1s_DecServingSpyChn(Chn)   l1s_ServingSpyData [17 + Chn - L1_BCCH_ID]--

#define l1s_SetNearSpyArfcn(a,n)  l1s_NearSpyData [0 + 5*n] = a
#define l1s_SetNearSpyRla(a,n)    l1s_NearSpyData [1 + 5*n] = a
#define l1s_SetNearSpyBsic(a,n)   l1s_NearSpyData [2 + 5*n] = a
#define l1s_SetNearSpySch(a,n)    l1s_NearSpyData [3 + 5*n] = a
#define l1s_IncNearSpySch(n)      l1s_NearSpyData [3 + 5*n]++
#define l1s_DecNearSpySch(n)      l1s_NearSpyData [3 + 5*n]--
#define l1s_SetNearSpyBcch(a,n)   l1s_NearSpyData [4 + 5*n] = a
#define l1s_IncNearSpyBcch(n)     l1s_NearSpyData [4 + 5*n]++
#define l1s_DecNearSpyBcch(n)     l1s_NearSpyData [4 + 5*n]--



typedef struct
{
 u8 Idx [6 + 1];
 u8 Free;
} l1s_NearSpy_t;


#ifdef __L1S_SPY_VAR__
const ascii *const l1s_ServingSpyStr [L1S_NB_SERVING_SPY] =
{
 "Arfcn 0x%03x",
 "Pwr %3i",
 "Rla %3i",
 "Agc %3i",
 "PAgc %3i",
 "FOf %3i",
 "TOf %3i",
 "Snr %3i",
 "TxP %3i",
 "TxP %3i",
 "TxP %3i",
 "TxP %3i",
 " C %3i",
 "I %3i",
 "I %3i",
 "I %3i",
 "I %3i",
 "Bcch %i",
 "Cch %i",
 "Pch %i",
 "PBcch %i",
 "PCcch %i",
 "PPch %i",
 "PDch %i",
 "PTcch %i",
 "Sdcch %i",
 "TchF %i",
 "TchH %i",
 "Facch %i",
 "Sacch %i",
 "Cbcch %i"
};


const ascii *const l1s_NearSpyStr [L1S_NB_NEAR_SPY] =
{
 "ARFCN 0x%03x",
 "Rla %3i",
 "Bsic %i",
 "Sch %02i",
 "Bcch %02i",
 "ARFCN 0x%03x",
 "Rla %3i",
 "Bsic %i",
 "Sch %02i",
 "Bcch %02i",
 "ARFCN 0x%03x",
 "Rla %3i",
 "Bsic %i",
 "Sch %02i",
 "Bcch %02i",
 "ARFCN 0x%03x",
 "Rla %3i",
 "Bsic %i",
 "Sch %02i",
 "Bcch %02i",
 "ARFCN 0x%03x",
 "Rla %3i",
 "Bsic %i",
 "Sch %02i",
 "Bcch %02i",
 "ARFCN 0x%03x",
 "Rla %3i",
 "Bsic %i",
 "Sch %02i",
 "Bcch %02i"
};

l1s_NearSpy_t l1s_NearSpy = {{1, 2, 3, 4, 5, 6, 0xFF}, 0};

u8 l1s_NewNearSpy (void)
{
 u8 Idx = l1s_NearSpy.Free;

 if (Idx != 0xFF)
  l1s_NearSpy.Free = l1s_NearSpy.Idx [l1s_NearSpy.Free];
 else
  Idx = 6 + L1S_EXTRA_NEAR_SPY -1;

 return Idx;
}

void l1s_FreeNearSpy (u8 Idx)
{
 if (Idx != (6 + L1S_EXTRA_NEAR_SPY - 1))
 {
  l1s_SetNearSpyArfcn (0, Idx);
  l1s_SetNearSpyBsic (0, Idx);
  l1s_SetNearSpySch  (0, Idx);
  l1s_SetNearSpyBcch (0, Idx);
  l1s_NearSpy.Idx [Idx] = l1s_NearSpy.Free;
  l1s_NearSpy.Free = Idx;
 }
}

#else

extern const ascii *const l1s_ServingSpyStr [L1S_NB_SERVING_SPY];
extern const ascii *const l1s_NearSpyStr [L1S_NB_NEAR_SPY];
extern l1s_NearSpy_t l1s_NearSpy;

u8   l1s_NewNearSpy (void);
void l1s_FreeNearSpy (u8 Idx);

#endif




/*
==============================================================================
   Function   :
 ----------------------------------------------------------------------------
   Scope      :
   Parameters : none
   Return     : none
==============================================================================
*/


#undef DefExtern

#endif
#endif

