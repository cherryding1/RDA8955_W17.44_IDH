/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003 - 2006

================================================================================
*/

/*
================================================================================
  File       : rr_dec.h 
--------------------------------------------------------------------------------

  Scope      : Defintion of types, constants and macro for decoding
               common to RRD and RRI

  History    :
--------------------------------------------------------------------------------
  Jun 12 03  |  PFA   | Creation            
  Jul 09 04  |  MCE   | Addition of EdCtx   
================================================================================
*/
#ifndef __RR_DEC_H__
#define __RR_DEC_H__
#include "cmn_dec.h"

// EnDec includes
#include "rri_ed_c.h"
#include "ed_rr.h"

/*
** RR Skip Indicator
*/
#define RR_GET_SKIP_IND(byte)  CMN_EXTRACTU8(byte, 4, CMN_MASKL4)

/*
** RR internal structure for the decoding of frequency lists
*/
typedef struct
{
 u8    FreqNb ;
 u16 * FreqL  ;
} rr_FreqList_t ;

/* 
** Cell Options (Sys 3 and Sys 6)
*/
#define RR_GET_RLT(val)        CMN_EXTRACTU8(val, 0, CMN_MASKL4)   // Rlt  
#define RR_GET_DTX(val)        CMN_EXTRACTU8(val, 4, CMN_MASKL2)   // Dtx  
#define RR_GET_HDTX_SACCH(val) ( (val & 0x80) >> 5 )               // Dtx bit 8
#define RR_GET_PWRC(val)       CMN_EXTRACTU8(val, 6, CMN_MASKL1)   // Pwrc 

#define RR_PUT_RLT(val)        CMN_PUT(val, 0, CMN_MASKL4)
#define RR_PUT_DTX(val)        CMN_PUT(val, 4, CMN_MASKL2)
#define RR_PUT_PWRC(val)       CMN_PUT(val, 6, CMN_MASKL1)

/*
** Ms Classmark
*/
#define RR_GET_CM3(val)        CMN_GET(val, 7, CMN_MASKL1)
#define RR_GET_ESIND(val)      CMN_GET(val, 4, CMN_MASKL1)
#define RR_PWRCLASS_RESET      0xf8

#define RR_PUT_REVL(val)       CMN_PUT(val, 5, CMN_MASKL2)
#define RR_PUT_ESIND(val)      CMN_PUT(val, 4, CMN_MASKL1)
#define RR_PUT_PWRCLASS(val)   CMN_PUT(val, 0, CMN_MASKL3)

#define RR_PUT_PS(val)         CMN_PUT(val, 6, CMN_MASKL1)
#define RR_PUT_SSSCR_IND(val)  CMN_PUT(val, 4, CMN_MASKL2)
#define RR_PUT_SM(val)         CMN_PUT(val, 3, CMN_MASKL1)
#define RR_FC_RESET            0xfe
#define RR_PUT_FC(val)         CMN_PUT(val, 0, CMN_MASKL1)

#define RR_PUT_CM3(val)        CMN_PUT(val, 7, CMN_MASKL1)
#define RR_PUT_UCS2(val)       CMN_PUT(val, 4, CMN_MASKL1) 
#define RR_PUT_A53(val)        CMN_PUT(val, 1, CMN_MASKL1)
#define RR_PUT_A52(val)        CMN_PUT(val, 0, CMN_MASKL1)




#define RR_GET_STARTMODE(val)  CMN_GET(val, 0, CMN_MASKL2)
#define RR_GET_ICMI(val)       CMN_GET(val, 3, CMN_MASKL1)
#define RR_GET_NSCB(val)       CMN_GET(val, 4, CMN_MASKL1)
#define RR_GET_MRVER(val)      CMN_GET(val, 5, CMN_MASKL3)

#ifndef __RR_DEC_C__
#define DefExtern extern
#else 
#define DefExtern
#endif

/*
** Rach Control Parameters : conversion tables
*/
DefExtern const u8 rr_TxInteger [] ;
DefExtern const u8 rr_SComb     [] ;
DefExtern const u8 rr_SNComb    [] ;
DefExtern const u8 rr_MaxRetrans[] ;
#ifdef __EGPRS__
DefExtern const u16 rr_EgprsWs [] ;
#endif
#undef DefExtern 

/*
** Decoder Types redefinitions
*/
typedef       c_Si13Ro_GprsMa         ed_GprsMa_t ;




#define IGNORE_MEAS_MAPPING_STRUCT              \
  if ( SHIFT_INT( 1 ) )                         \
  {                                             \
   SHIFT_INT( 13 );                             \
  }                                             \
  else                                          \
  {                                             \
   SHIFT_INT( 16 );                             \
  }                                             \
  SHIFT_INT( 13 );


#endif	

