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
  File       : RRI_DEFS.H
--------------------------------------------------------------------------------

  Scope      : define utilities for RRI

  History    :
--------------------------------------------------------------------------------
  May 11 06  |  MCE   | suppression of NewSched from PsiInst_t
  Apr 13 06  |  MCE   | suppression of RRI_PCO_IDX 
  Aug 09 05  |  MCE   | suppression of RRI_NONDRX                          
  Mar 02 05  |  MCE   | RRI_NONDRX returns FALSE if RR_NONDRX_TIMER = 0
  Jun 22 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RRI_DEFS_H__
#define __RRI_DEFS_H__
#include "rr_defs.h"
#include "rr_freq.h"
#include "rr_capa.h"

/*
** RR internal coding for [P]ccch mode 
*/
#define RRI_NORMAL_PAGING   0
#define RRI_EXTENDED_PAGING 1
#define RRI_REORG_PAGING    2
#define RRI_SAB_PAGING      3
#define RRI_FULL_CH         4 
#define RRI_NONDRX_PAGING   5

/*
** Mutli Instances of (P) Sysinfos                       
*/
#define RRI_MAX_COUNT           16  // Maximum # of instances 
typedef struct {

   u8   ChgeMark                ;   // Change Mark
   u8   Count                   ;   // Number of required instances
   u16  FlagRec                 ;   // BitField of received Sysinfos Indexes
   u8   NextDecodIdx            ;   // Next Expected Idx for decoding
   u8 * RawData [RRI_MAX_COUNT] ;   // Raw Data ( when needed ) 
   void (* RstFunc)(void *)     ;   
                                    
 
} rri_PsiInst_t;

/*
** Indexes of (P) Sysinfos  Multi Instances for save                      
*/
#ifdef __GPRS__
enum { RRI_SI2Q_IDX, RRI_PSI2_IDX, RRI_PSI3B_IDX, RRI_PSI5_IDX, RRI_LAST_PSIIDX };
#else
enum { RRI_SI2Q_IDX, RRI_LAST_PSIIDX };
#endif

/*
** Possible indexes of PSI1 in HrBitmap in l1_DataInd_t
*/
#define RRI_L1_PSI1_IDX0    (1<<0)
#define RRI_L1_PSI1_IDX1    (1<<2)

/*
** Gprs access parameters structure
*/
typedef struct {                    
  u8          Ac[2]        ; // Access Class
  u8          S            ; // S parameter 
  u8          TxInteger    ; // For Rach Computation
  u8          MaxRetrans[4]; // Nb of RTrans after 1st Rach for each Prio
  u8          PersLev[4]   ; // Persistence Level for each Prio
} rr_GprsAcc_t ;




#define RRI_PMO_MAX_COUNT 8   
#define RRI_MAX_SCAN_CELL   8

typedef enum { RRI_DYN_NC_PAR_PMO, RRI_DYN_NC_PAR_PCCO } rri_DynNcMeasParType_t  ;

#endif // __RRI_DEFS_H__

