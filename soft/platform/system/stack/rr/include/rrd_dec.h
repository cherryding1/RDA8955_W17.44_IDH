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
  File       : RRD_DEC.H
--------------------------------------------------------------------------------

  Scope      : defines for RRD radio messages ( TS 4.18, 4.60 )

  History    :
--------------------------------------------------------------------------------
  Aug 22 03  |  MCE   | Creation            
================================================================================
*/
#ifndef __RRD_DEC_H__
#define __RRD_DEC_H__
#include "rr_dec.h"




#define RR_MSG_MAX_SIZE 23

  
// RR CAUSE IE values (see 3GPP TS 04.18, RR Cause IE)
#define   RR_NORMAL_EVENT          0x00
#define   RR_AB_REL_UNSPE          0x01
#define   RR_CHN_UNAC              0x02
#define   RR_TA_OOR                0x08
#define   RR_CHN_MOD_UNAC          0x09
#define   RR_FRQ_NOT_IMPL          0x0a
#define   RR_LOW_LAY_FAIL          0x0c
#define   RR_SEMANTIC_ERROR        0x5f
#define   RR_INV_MANDAT_INFO       0x60
#define   RR_MT_NOT_IMPLEMENTED    0x61
#define   RR_MT_NCOMPATIBLE_STATE  0x62
#define   RR_COND_IE_ERROR         0x64
#define   RR_NO_CA_AVAIL           0x65
#define   RR_PR_ERR_UNSPE          0x6f

// Ciphering Algorithm IE values
#define   RR_CIPH_A51              0
#define   RR_CIPH_A52              1
#define   RR_CIPH_A53              2
#define   RR_CIPH_A54              3
#define   RR_CIPH_A55              4
#define   RR_CIPH_A56              5
#define   RR_CIPH_A57              6

#ifdef __RRP__
#define RR_PCCO        0x01
#define RR_PDL_ASS     0x02
#define RR_PPWRCTRL_TA 0x05
#define RR_PQUE_NOTIF  0x06
#define RR_PTS_RECONF  0x07
#define RR_PTBF_REL    0x08
#define RR_PUL_ASS     0x0A
#define RR_PPDCH_REL   0x23

/*
** PRach estab cause
*/
#define RRP_TWO_PH_11 48
#define RRP_TWO_PH_8   8
#define RRP_SH_ACC_11  4
#define RRP_SH_ACC_8   0
#define RRP_ONE_PH_11  0
#define RRP_ONE_PH_8   1
#define RRP_SGL_8     12
#define RRP_SGL_11    52

/*
** Packet TBF Release cause
*/
#define RRP_REL_NORM 0
#define RRP_REL_ABN  2

#endif

#endif // __RRD_DEC_H__

