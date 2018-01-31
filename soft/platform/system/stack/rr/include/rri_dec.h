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
  File       : rri_dec.h 
--------------------------------------------------------------------------------

  Scope      : Defintion of Constants and Macro for Sysinfo decoding

  History    :
--------------------------------------------------------------------------------
  Jun 12 03  |  PFA   | Creation            
================================================================================
*/
#ifndef __RRI_DEC_H__
#define __RRI_DEC_H__
#include "cmn_dec.h"
#include "rr_dec.h"
#include "ed_rr.h"


/*
** Information Elements used in Sysinfos
*/

/* 
** Rach Control Parameters (Sys 1,2,2B,3,4)
*/
#define RR_GET_MAX_RETR(val)   CMN_EXTRACTU8(val, 6, CMN_MASKL2)   // Max_Retrans 
#define RR_GET_TX_INT(val)     CMN_EXTRACTU8(val, 2, CMN_MASKL4)   // Tx_Integer 
#define RR_GET_CB(val)         CMN_EXTRACTU8(val, 1, CMN_MASKL1)   // Cell Bar Access 
#define RR_GET_EC(val)         CMN_EXTRACTU8(val, 2, CMN_MASKL1)   // Emergency Call      
#define RR_GET_RE(val)         CMN_EXTRACTU8(val, 0, CMN_MASKL1)   // ReestabCall      

/* 
** Control Channel Description (Sys 3)
*/
#define RR_GET_CC_CNF(val)     CMN_EXTRACTU8(val, 0, CMN_MASKL3)   // Ccch_Conf      
#define RR_GET_BS_AG(val)      CMN_EXTRACTU8(val, 3, CMN_MASKL3)   // Bs_Ag_Blks_Res 
#define RR_GET_ATT(val)        CMN_EXTRACTU8(val, 6, CMN_MASKL1)   // Att Flag       
#define RR_GET_MSCR(val)       CMN_EXTRACTU8(val, 7, CMN_MASKL1)   // Att Flag       
#define RR_GET_BS_PA(val)      CMN_EXTRACTU8(val, 0, CMN_MASKL3)   // Bs_Pa_Mfrms    


/* 
** Cell selection Parameters (Sys 3,4)
*/
#define RR_GET_MAXTXPW(val)    CMN_EXTRACTU8(val, 0, CMN_MASKL5)   // Ms_TxPwr_Max
#define RR_GET_CRH(val)        CMN_EXTRACTU8(val, 5, CMN_MASKL3)   // Crh        
#define RR_GET_RXLACMIN(val)   CMN_EXTRACTU8(val, 0, CMN_MASKL6)   // RxLev Acc Min 
#define RR_GET_NECI(val)       CMN_EXTRACTU8(val, 6, CMN_MASKL1)   // Neci         
#define RR_GET_ACS(val)        CMN_EXTRACTU8(val, 7, CMN_MASKL1)   // Acs         

/* 
** Optional Cell selection Parameters (Sys 3Rest)
*/
#define RR_GET_CBQ(val)        CMN_EXTRACTU8(val, 6, CMN_MASKL1)    // Cell Barr Qualify
#define RR_GET_CRO(val)        CMN_EXTRACTU8(val, 0, CMN_MASKL6)    // Cell Resel Offset
#define RR_GET_TPO(val)        CMN_EXTRACTU8(val, 5, CMN_MASKL3)    // Temporary Offset 
#define RR_GET_PTY(val)        CMN_EXTRACTU8(val, 0, CMN_MASKL5)    // Penalty Time     

/* 
** Neighbour cell descriptions                    
*/
#define RR_GET_SEQCOD(val)     CMN_EXTRACTU8(val, 4, CMN_MASKL1)    // Seq Code Ext Meas
#define RR_GET_BAIND(val)      CMN_EXTRACTU8(val, 4, CMN_MASKL1)    // BA indicator     
#define RR_GET_EXTIND(val)     CMN_EXTRACTU8(val, 5, CMN_MASKL1)    // SI2B Needed       
#define RR_GET_MBREP(val)      CMN_EXTRACTU8(val, 5, CMN_MASKL2)    // MultiBand Report 

/* 
** SI2 Q specific                                 
*/
#define RR_GET_2QBAIND(val)    CMN_EXTRACTU8(val, 7, CMN_MASKL1)    // BA indicator     
#define RR_GET_2QCHGMRK(val)   CMN_EXTRACTU8(val, 5, CMN_MASKL1)    // Chage Mark       
#define RR_GET_2QIDX(val)      CMN_EXTRACTU8(val, 1, CMN_MASKL4)    // Index            
#define RR_GET_2QCOUNTH(val)   CMN_EXTRACTU8(val, 0, CMN_MASKL1)    // Count H          
#define RR_GET_2QCOUNTL(val)   CMN_EXTRACTU8(val, 5, CMN_MASKL3)    // Count L          


/* 
** Measurt Report                                 
*/
#define RR_MEA_NO_NCELLH(val)       (val >> 2)
#define RR_MEA_NO_NCELLL(val)       (val << 6)
#define RR_MEA_RXLEV_NCELL1(val)    (val << 0)
#define RR_MEA_BFREQ_NCELL1(val)    (val << 3)
#define RR_MEA_BSIC_NCELL1H(val)    (val >> 3)
#define RR_MEA_BSIC_NCELL1L(val)    (val << 5)
#define RR_MEA_RXLEV_NCELL2H(val)   (val >> 1)
#define RR_MEA_RXLEV_NCELL2L(val)   (val << 7)
#define RR_MEA_BFREQ_NCELL2(val)    (val << 2)
#define RR_MEA_BSIC_NCELL2H(val)    (val >> 4)
#define RR_MEA_BSIC_NCELL2L(val)    (val << 4)
#define RR_MEA_RXLEV_NCELL3H(val)   (val >> 2)
#define RR_MEA_RXLEV_NCELL3L(val)   (val << 6)
#define RR_MEA_BFREQ_NCELL3(val)    (val << 1)
#define RR_MEA_BSIC_NCELL3H(val)    (val >> 5)
#define RR_MEA_BSIC_NCELL3L(val)    (val << 3)
#define RR_MEA_RXLEV_NCELL4H(val)   (val >> 3)
#define RR_MEA_RXLEV_NCELL4L(val)   (val << 5)
#define RR_MEA_BFREQ_NCELL4(val)    (val << 0)
#define RR_MEA_BSIC_NCELL4(val)     (val << 2)
#define RR_MEA_RXLEV_NCELL5H(val)   (val >> 4)
#define RR_MEA_RXLEV_NCELL5L(val)   (val << 4)
#define RR_MEA_BFREQ_NCELL5H(val)   (val >> 1)
#define RR_MEA_BFREQ_NCELL5L(val)   (val << 7)
#define RR_MEA_BSIC_NCELL5(val)     (val << 1)
#define RR_MEA_RXLEV_NCELL6H(val)   (val >> 5)
#define RR_MEA_RXLEV_NCELL6L(val)   (val << 3)
#define RR_MEA_BFREQ_NCELL6H(val)   (val >> 2)
#define RR_MEA_BFREQ_NCELL6L(val)   (val << 6)
#define RR_MEA_BSIC_NCELL6(val)     (val << 0)



















#define OFSINBCCH (CurrOfs < 168)
#define OFSINMSG  (CurrOfs < Len)

#endif	

