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
  File       : rr_freq.h
--------------------------------------------------------------------------------

  Scope      : Definition of macros for frequency Arfcns computation / handling

  History    :
--------------------------------------------------------------------------------
  Mar 28 05  |  MCE   | Addition of GSMR, and GSM480 bands
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RR_FREQ_H__
#define __RR_FREQ_H__

/*
** Band List Definition


         { 1  , 124 },      // 0-  Primary GSM Band    1 ..    124
         { 975, 50  },      // 1-  Extended Band     975 .. 1023,0
         { 512, 374 },      // 2-  DCS 1800          512 ..    885
         { 512, 299 }       // 3-  PCS 1900          512 ..    810
         { 259,  35 }       // 4-       400          259 ..    293
         { 128, 124 }       // 5-       850          128 ..    251

   Index in the ArfcnList table
*/
#define RR_PGSM_LIST 0
#define RR_EGSM_LIST 1
#define RR_RGSM_LIST 2
#define RR_DCS_LIST  3
#define RR_PCS_LIST  4
#define RR_450_LIST  5
#define RR_480_LIST  6
#define RR_850_LIST  7


/*
** Band Definition:  used for sameband() function
** Also used as Indexes for Tables in Measrt Report
** (not List Table !!!) 
*/
typedef enum  {
                RR_GSM_BAND,         // Include PGSM + EGSM
                RR_DCS_BAND,         // Band 1800         
                RR_PCS_BAND,         // Band 1900            
                RR_850_BAND,         // Band  850
                RR_400_BAND,         // Band  400
       
                RR_LAST_BAND         // Last fo Index Num
       
              } rr_FrqBand_t ;

#define RR_UKN_BAND     0xff    // Band Unknown       

/*
** Detailled Band Definition:  used for rr_GiveBand() and 
** rr_IsFrqBandSupported() functions
*/
typedef enum  {
                RR_D_450_BAND ,       
                RR_D_480_BAND ,       
                RR_D_900P_BAND,
                RR_D_900E_BAND,
                RR_D_900R_BAND,
                RR_D_850_BAND ,
                RR_D_1800_BAND,
                RR_D_1900_BAND,
                RR_D_LAST_BAND,    
                RR_D_UKN_BAND
              } rr_DetailedFrqBand_t ;

#endif

