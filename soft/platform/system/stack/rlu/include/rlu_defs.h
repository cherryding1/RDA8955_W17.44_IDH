//------------------------------------------------------------------------------
//  $Header: /usr/local/cvsroot/rlu/inc/rlu_defs.h,v 1.2 2006/03/08 00:18:10 mce Exp $
//------------------------------------------------------------------------------
/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or 
  transmission of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (C) 2003-2006

================================================================================
*/

/*
================================================================================
  File       : rlu_defs.h 
--------------------------------------------------------------------------------

  Scope      : Definition of Constants and Macros for RLU

  History    :
--------------------------------------------------------------------------------
  Jun 17 06  |  MCE   | Added RLU_TPURGE_DURATION 
  Mar 30 06  |  MCE   | Added RLU_ULSDU_THR_HIGH and RLU_ULSDU_THR_LOW
  Mar 30 06  |  MCE   | Added RLU_ULCON_THR_HIGH and RLU_ULCON_THR_LOW
  Mar 28 06  |  MCE   | Added RLU_TXBP_THR_HIGH and RLU_TXBP_THR_LOW
  Nov 21 05  |  MCE   | Added RLU_PFI_SUPP
  Feb 09 04  |  MCE   | Creation            
================================================================================
*/
#ifndef __RLU_DEFS_H__
#define __RLU_DEFS_H__

#include "rlc_defs.h"

/*
** Utilities
** RLC data block size as in 4.60 (does not include MAC header : 1 byte)
** Includes 1 byte for spar bits
*/
#define RLU_CS1_ALLOC  22
#define RLU_CS2_ALLOC  33
#define RLU_CS3_ALLOC  39
#define RLU_CS4_ALLOC  53




#define RLU_MIN_PDU_SDU_BYTES ( RLU_CS1_ALLOC - 6 ) 
                                                    

/*
** macros using RLU data
*/
#ifdef __EGPRS__
#define RLU_MOD_SNS(_val)  ((_val) & pRLUCtx->Sns)
#define RLU_STALLED  (RLU_MOD_SNS(pRLUCtx->Vs - pRLUCtx->Va) >= pRLUCtx->WS)
#else
#define RLU_STALLED  ((pRLUCtx->Vs - pRLUCtx->Va) == RLC_WS)
#endif
#define RLU_SDUDONE  ( pRLUCtx->CurSduOff == pRLUCtx->CurSdu->B.IL.DataLen ) 
#define RLU_CNTDOWN  ( pRLUCtx->Cv != 15 )          // CntDwn proc. started

/*
** T3182 duration
*/
#define RLU_T3182_DURATION (5 SECONDS)




#define RLU_UACK_REP_MAX    4




#define RLU_MAXPDU_PERSDU   ( (1500/16) + 1)  
#endif	




/* # Modified by LIUBO 2008.11.27 begin */
/* Just use sap_StackCfg[pLRUCtx->SimIndex] instead of sap_StackCfg. When dual SIM is supported in RLU module */
/* Such change happens in file rlu_void.c and rlu_tbf.c which refer to sap_StackCfg */
//#define RLU_PFI_SUPP 	( sap_StackCfg.PfcSupport && pRLUCtx->PfcNwkSupport )
#ifndef STK_SINGLE_SIM
#define RLU_PFI_SUPP 	( sap_StackCfg[pRLUCtx->SimIndex].PfcSupport && pRLUCtx->PfcNwkSupport )
#else
#define RLU_PFI_SUPP 	( sap_StackCfg.PfcSupport && pRLUCtx->PfcNwkSupport )
#endif
/* # Modified by LIUBO 2008.11.27 end */





#define RLU_TXBP_THR_HIGH   3






#define RLU_TXBP_THR_LOW    RLU_TXBP_THR_HIGH





#define RLU_ULCONG_THR_HIGH   200



#define RLU_ULCONG_THR_LOW    100






#define RLU_ULSDU_THR_HIGH   20





#define RLU_ULSDU_THR_LOW    10




#define RLU_TPURGE_DURATION (15 SECONDS)

