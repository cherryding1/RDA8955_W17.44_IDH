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
 File          l1s_pnbp.h
--------------------------------------------------------------------------------

  Scope      : L1 synchronous Packet Normal Block management Package header file.

  History    :
--------------------------------------------------------------------------------
  Jan 15 03  |  ADA  | Creation
================================================================================
*/

#ifndef __L1S_PNBP_H__
#define __L1S_PNBP_H__

#include "sxs_type.h"

#ifndef __NO_GPRS__
#ifdef __L1S_PNBP_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

#define L1S_MULTISLOT_CAP_TTA_BIT_MAP   0x03L
#define L1S_MULTISLOT_CAP_TTA           2
#define L1S_MULTISLOT_CAP_TTB_BIT_MAP   0x01L
#define L1S_MULTISLOT_CAP_TTB           1
#define L1S_MULTISLOT_CAP_TRA_BIT_MAP   0x03L
#define L1S_MULTISLOT_CAP_TRA           2
#define L1S_MULTISLOT_CAP_TRB_BIT_MAP   0x01L
#define L1S_MULTISLOT_CAP_TRB           1

#define L1S_MULTISLOT_CAP_RX_BIT_MAP    0x0FL
#define L1S_MULTISLOT_CAP_RX            4
#define L1S_MULTISLOT_CAP_TX_BIT_MAP    0x0FL
#define L1S_MULTISLOT_CAP_TX            4

#define L1S_MULTISLOT_CAP_SUM   5


#define L1_GET_BST_IDX(i)  (i & 3)
#define L1_RX_TX_BST(i)    ((1 << 2) | i)
#define L1_EC(A)    ((1 << (3 + A))) // Time to encode for defined anticipation.
#define L1_DC       (1 << 6)         // Time to decode.
#define L1_BLK_END  (1 << 7)         // Last burst of current block being received.



#ifdef __L1S_PNBP_VAR__

/* The table below gives the distance in frames from the first burst of  */
/* the next Pdch block.                                                  */
const u8 l1s_PdchDist [13] = {0, 3, 2, 1, 0, 3, 2, 1, 0, 4, 3, 2, 1};

/* Ordered block list for PBcch and PAgch blocks. */
const u8 l1s_POrderedBlkList [12] = {0, 6, 3, 9, 1, 7, 4, 10, 2, 5, 8, 11};

const u8 l1s_PdchFSchd [26] =
{
/*  0 */  L1_RX_TX_BST(1),
/*  1 */  L1_RX_TX_BST(2) | L1_EC(2),
/*  2 */  L1_RX_TX_BST(3) | L1_EC(1),
/*  3 */  L1_RX_TX_BST(0) | L1_BLK_END | L1_EC(0),
/*  4 */  L1_RX_TX_BST(1) | L1_DC,
/*  5 */  L1_RX_TX_BST(2) | L1_EC(2),
/*  6 */  L1_RX_TX_BST(3) | L1_EC(1),
/*  7 */  L1_RX_TX_BST(0) | L1_BLK_END | L1_EC(0),
/*  8 */  L1_RX_TX_BST(1) | L1_DC,
/*  9 */  L1_RX_TX_BST(2),
/* 10 */  L1_RX_TX_BST(3) | L1_EC(2),
/* 11 */  0               | L1_BLK_END | L1_EC(1),
/* 12 */  L1_RX_TX_BST(0) | L1_DC | L1_EC(0),
/* 13 */  L1_RX_TX_BST(1),
/* 14 */  L1_RX_TX_BST(2) | L1_EC(2),
/* 15 */  L1_RX_TX_BST(3) | L1_EC(1),
/* 16 */  L1_RX_TX_BST(0) | L1_BLK_END | L1_EC(0),
/* 17 */  L1_RX_TX_BST(1) | L1_DC,
/* 18 */  L1_RX_TX_BST(2) | L1_EC(2),
/* 19 */  L1_RX_TX_BST(3) | L1_EC(1),
/* 20 */  L1_RX_TX_BST(0) | L1_BLK_END | L1_EC(0),
/* 21 */  L1_RX_TX_BST(1) | L1_DC,
/* 22 */  L1_RX_TX_BST(2),
/* 23 */  L1_RX_TX_BST(3) | L1_EC(2),
/* 24 */  0               | L1_BLK_END | L1_EC(1),
/* 25 */  L1_RX_TX_BST(0) | L1_DC | L1_EC(0),
};

#else

extern const u8 l1s_PdchDist [13];
extern const u8 l1s_POrderedBlkList [12];
extern const u8 l1s_PdchFSchd [26];

#endif
#endif

#undef DefExtern

#endif
