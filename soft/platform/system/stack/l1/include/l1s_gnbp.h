/*
================================================================================

  This source code is property of StackCom. The information contained in this
  file is confidential. Distribution, reproduction, as well as exploitation,or
  transmisison of any content of this file is not allowed except if expressly
  permitted. Infringements result in damage claims.

  Copyright StackCom (c) 2004

================================================================================
*/

/*
================================================================================
 File          l1s_gncp.h
--------------------------------------------------------------------------------

  Scope      : L1 synchronous GSM Normal Block management Package header file.

  History    :
--------------------------------------------------------------------------------
  Aug 25 03  |  ADA  | Creation
  May 10 05  |  ADA  | SdcchPos: position of the RP
================================================================================
*/
#ifndef __L1S_GNBP_H__
#define __L1S_GNBP_H__

#include "sxs_type.h"
#include "l1.h"

#ifdef __L1S_GNBP_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif


/* Connexion data context. */
typedef struct
{
 l1s_TACtx_t        TACtx;
 l1s_TxPwrCtx_t     TxPwrCtx;
 pal_TxWin_t        TxWin;
 pal_NBurstRxWin_t  NBurstRxWin [4];
 pal_NBlockDesc_t   NBlockDesc;
} l1s_CnxCtx_t;


#define L1_USF_FREE             0x07

#define L1_TX_ACCESS            (1 << 7)
#define L1_GET_NB_DIST(a)       ((a) & ~L1_TX_ACCESS)

#define L1_GET_PAGING_MODE(Pag) (((u8 *)Pag)[3] & 0x03)

#ifdef __L1S_GNBP_VAR__
/* Retreive block position in multi-frame thanks to block index. */
const u8 l1s_CcchPos  [9]  = {6, 12, 16, 22, 26, 32, 36, 42, 46};

#ifndef __NO_GPRS__
const u8 l1s_PccchPos [12] = {0, 4, 8, 13, 17, 21, 26, 30, 34, 39, 43, 47};
#endif

/* Retreive next normal block position and access burst position     */
/* thanks to Fn % 51 or Fn % 52 value.                               */
/* Table is built for an anticipation of one.                        */
const u8 l1s_BcchNBDist [51] =
{
 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 5 | L1_TX_ACCESS, 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS,
 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 5 | L1_TX_ACCESS, 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS,
 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 5 | L1_TX_ACCESS, 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS,
 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 5 | L1_TX_ACCESS, 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS,
 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 6 | L1_TX_ACCESS, 5 | L1_TX_ACCESS, 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS,
 2 | L1_TX_ACCESS
};

const u8 l1s_BcchSdCombNBDist [51] =
{
 1, 0,
 3, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0,
 5, 4, 3, 2,
 1, 0,
 3, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 36 | L1_TX_ACCESS, 35 | L1_TX_ACCESS, 34 | L1_TX_ACCESS, 33 | L1_TX_ACCESS,
 32 | L1_TX_ACCESS, 31 | L1_TX_ACCESS,
 30 | L1_TX_ACCESS, 29 | L1_TX_ACCESS, 28 | L1_TX_ACCESS, 27 | L1_TX_ACCESS,
 26 | L1_TX_ACCESS, 25 | L1_TX_ACCESS, 24 | L1_TX_ACCESS, 23 | L1_TX_ACCESS,
 22 | L1_TX_ACCESS, 21 | L1_TX_ACCESS,
 20 | L1_TX_ACCESS, 19 | L1_TX_ACCESS, 18 | L1_TX_ACCESS, 17 | L1_TX_ACCESS,
 16, 15, 14, 13,
 12, 11,
 10,  9,  8 | L1_TX_ACCESS, 7 | L1_TX_ACCESS,
 6, 5, 4, 3,
 2
};

#ifndef __NO_GPRS__
const u8 l1s_PccchNBDist [52] =
{
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS,
 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS,
 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS,
 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS, 0 | L1_TX_ACCESS,
 4 | L1_TX_ACCESS, 3 | L1_TX_ACCESS, 2 | L1_TX_ACCESS, 1 | L1_TX_ACCESS,
 0 | L1_TX_ACCESS
};
#endif

#define _L1_ARX  1  /* Anticipation for Rx. */
#define _L1_ATX  3  /* Anticipation for Tx. */

#define L1_RX    (1L << 8)    // Reception
#define L1_TX    (1L << 9)    // Transmission
#define L1_SA    (1L << 10)   // Sacch
#define L1_SD    (1L << 11)   // Sdcch
#define L1_RP_ST (1L << 12)   // Start reporting period.

#define L1_UL(t, c)  (t >= _L1_ATX ? ((t - _L1_ATX) | (L1_TX | c)) : ((t + 102 - _L1_ATX) | (L1_TX | c)))
#define L1_DL(t, c)  (t >= _L1_ARX ? ((t - _L1_ARX) | (L1_RX | c)) : ((t + 102 - _L1_ARX) | (L1_RX | c)))


/* Get position of Sdcch/u/d and Sacch/u/d in multiframe 102 for each Tdma offset. */
const u16 l1s_Sdcch4Pos [4][6] =
{
 {L1_DL(22,L1_SD), L1_UL(37,L1_SD) | L1_RP_ST, L1_DL(42,L1_SA), L1_UL(57,L1_SA), L1_DL(73,L1_SD), L1_UL (88, L1_SD)},
 {L1_DL(26,L1_SD), L1_UL(41,L1_SD) | L1_RP_ST, L1_DL(46,L1_SA), L1_UL(61,L1_SA), L1_DL(77,L1_SD), L1_UL (92, L1_SD)},
 {L1_UL( 6,L1_SA), L1_DL(32,L1_SD), L1_UL(47,L1_SD) | L1_RP_ST, L1_DL(83,L1_SD), L1_DL(93,L1_SA), L1_UL (98, L1_SD)},
 {L1_UL( 0,L1_SD), L1_UL(10,L1_SA), L1_DL(36,L1_SD) | L1_RP_ST, L1_UL(51,L1_SD), L1_DL(87,L1_SD), L1_DL (97, L1_SA)}
};

const u16 l1s_Sdcch8Pos [8][6] =
{
 {L1_DL( 0,L1_SD),            L1_UL(15,L1_SD) | L1_RP_ST, L1_DL(32,L1_SA), L1_UL(47,L1_SA), L1_DL(51,L1_SD), L1_UL (66, L1_SD)},
 {L1_DL( 4,L1_SD),            L1_UL(19,L1_SD) | L1_RP_ST, L1_DL(36,L1_SA), L1_UL(51,L1_SA), L1_DL(55,L1_SD), L1_UL (70, L1_SD)},
 {L1_DL( 8,L1_SD),            L1_UL(23,L1_SD) | L1_RP_ST, L1_DL(40,L1_SA), L1_UL(55,L1_SA), L1_DL(59,L1_SD), L1_UL (74, L1_SD)},
 {L1_DL(12,L1_SD) | L1_RP_ST, L1_UL(27,L1_SD),            L1_DL(44,L1_SA), L1_UL(59,L1_SA), L1_DL(63,L1_SD), L1_UL (78, L1_SD)},
 {L1_DL(16,L1_SD) | L1_RP_ST, L1_UL(31,L1_SD),            L1_DL(67,L1_SD), L1_UL(82,L1_SD), L1_DL(83,L1_SA), L1_UL (98, L1_SA)},
 {L1_UL( 0,L1_SA),            L1_DL(20,L1_SD) | L1_RP_ST, L1_UL(35,L1_SD), L1_DL(71,L1_SD), L1_UL(86,L1_SD), L1_DL (87, L1_SA)},
 {L1_UL( 4,L1_SA),            L1_DL(24,L1_SD) | L1_RP_ST, L1_UL(39,L1_SD), L1_DL(75,L1_SD), L1_UL(90,L1_SD), L1_DL (91, L1_SA)},
 {L1_UL( 8,L1_SA),            L1_DL(28,L1_SD) | L1_RP_ST, L1_UL(43,L1_SD), L1_DL(79,L1_SD), L1_UL(94,L1_SD), L1_DL (95, L1_SA)}
};


#define L1_GET_BST_IDX(i)  (i & 3)
#define L1_RX_TX_BST(i)   ((1 << 2) | i)
#define L1_EC_FA           (1 << 3)
#define L1_DC_FA           (1 << 4)
#define L1_DC_TC           (1 << 5)
#define L1_EC_SA           (1 << 6)
#define L1_DC_SA           (1 << 7)


const u8 l1s_TchFSchd [26] =
{
/*  0 */  L1_RX_TX_BST(1),
/*  1 */  L1_RX_TX_BST(2),
/*  2 */  L1_RX_TX_BST(3),
/*  3 */  L1_RX_TX_BST(0) | L1_EC_FA,
/*  4 */  L1_RX_TX_BST(1) | L1_DC_TC | L1_DC_FA,
/*  5 */  L1_RX_TX_BST(2),
/*  6 */  L1_RX_TX_BST(3),
/*  7 */  L1_RX_TX_BST(0) | L1_EC_FA,
/*  8 */  L1_RX_TX_BST(1) | L1_DC_TC | L1_DC_FA,
/*  9 */  L1_RX_TX_BST(2) | L1_EC_SA,
/* 10 */  L1_RX_TX_BST(3),
/* 11 */  L1_RX_TX_BST(0),
/* 12 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_DC_FA | L1_EC_FA,
/* 13 */  L1_RX_TX_BST(1) | L1_DC_SA,
/* 14 */  L1_RX_TX_BST(2),
/* 15 */  L1_RX_TX_BST(3),
/* 16 */  L1_RX_TX_BST(0) | L1_EC_FA,
/* 17 */  L1_RX_TX_BST(1) | L1_DC_TC | L1_DC_FA,
/* 18 */  L1_RX_TX_BST(2),
/* 19 */  L1_RX_TX_BST(3),
/* 20 */  L1_RX_TX_BST(0) | L1_EC_FA,
/* 21 */  L1_RX_TX_BST(1) | L1_DC_TC | L1_DC_FA,
/* 22 */  L1_RX_TX_BST(2) /* | L1_EC_FA */, // TBC
/* 23 */  L1_RX_TX_BST(3),
/* 24 */  0,
/* 25 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_DC_FA | L1_EC_FA,
};

const u8 l1s_TchHSchd [2][26] =
{
 {
/*  0 */  0,
/*  1 */  L1_RX_TX_BST(1),
/*  2 */  0,
/*  3 */  L1_RX_TX_BST(2) | L1_DC_TC,
/*  4 */  0,
/*  5 */  L1_RX_TX_BST(3),
/*  6 */  0,
/*  7 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_DC_FA  | L1_EC_FA,
/*  8 */  0,
/*  9 */  L1_RX_TX_BST(1),
/* 10 */  0,
/* 11 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_EC_SA,
/* 12 */  L1_RX_TX_BST(2),
/* 13 */  L1_DC_SA,
/* 14 */  L1_RX_TX_BST(3),
/* 15 */  0,
/* 16 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_DC_FA | L1_EC_FA,
/* 17 */  0,
/* 18 */  L1_RX_TX_BST(1),
/* 19 */  0,
/* 20 */  L1_RX_TX_BST(2) | L1_DC_TC,
/* 21 */  0,
/* 22 */  L1_RX_TX_BST(3),
/* 23 */  0,
/* 24 */  L1_DC_TC | L1_DC_FA,
/* 25 */  L1_RX_TX_BST(0) | L1_EC_FA
 },
 {
/* 13 */  L1_RX_TX_BST(2),
/* 14 */  0,
/* 15 */  L1_RX_TX_BST(3),
/* 16 */  0,
/* 17 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_DC_FA | L1_EC_FA,
/* 18 */  0,
/* 19 */  L1_RX_TX_BST(1),
/* 20 */  0,
/* 21 */  L1_RX_TX_BST(2) | L1_DC_TC,
/* 22 */  0,
/* 23 */  L1_RX_TX_BST(3),
/* 24 */  L1_RX_TX_BST(0) | L1_EC_SA,
/* 25 */  L1_DC_FA | L1_DC_TC,
/*  0 */  L1_RX_TX_BST(0) | L1_EC_FA | L1_DC_SA,
/*  1 */  0,
/*  2 */  L1_RX_TX_BST(1),
/*  3 */  0,
/*  4 */  L1_RX_TX_BST(2) | L1_DC_TC,
/*  5 */  0,
/*  6 */  L1_RX_TX_BST(3),
/*  7 */  0,
/*  8 */  L1_RX_TX_BST(0) | L1_DC_TC | L1_DC_FA | L1_EC_FA,
/*  9 */  0,
/* 10 */  L1_RX_TX_BST(1),
/* 11 */  0,
/* 12 */  L1_DC_TC,
 }
};

/* Position of the downlink SUB frames (Sacch + SID) for the different Tch */
/* configurations expressed in FN mod 104 with an anticipation of one.     */
/* Decoding positions of SID frames are indicated with define L1_SID_BLOCK */
/* with an anticipation of at least one.                                   */
/* Decoding position of Sacch which depends of allocated slot is not       */
/* indicated.                                                              */
#define L1_SID_BLOCK (1 << 7)
const u8 l1s_TchSubFrames [3][16] =
{ // Sa 11 37 63 89 - 24 50 76 102
 { 11, 24, 37, 50, 51, 52, 53, 54, 55, 56, 57, 58 | L1_SID_BLOCK, 63, 76, 89, 102},                  // Tch/F
 {103,  1,  3,  5 | L1_SID_BLOCK, 11, 24, 37, 50, 51, 53, 55, 57 | L1_SID_BLOCK, 63,  76, 89, 102},  // Tch/H0
 { 11, 13, 15, 17, 19 | L1_SID_BLOCK, 24, 37, 50, 63, 65, 67, 69, 71 | L1_SID_BLOCK, 76, 89, 102}   // Tch/H1
};

/* Position of the Dl SID frames for the half rate data configurations.    */
/* configurations expressed in FN mod 104 with an anticipation of one.     */
/* Decoding positions of SID frames are indicated with define L1_SID_BLOCK */
/* with an anticipation of at least one.                                   */
/* Decoding position of Sacch which depends of allocated slot is not       */
/* indicated.                                                              */
const u8 l1s_DlDataSubFrames [2][18] =
{
 /* Sa 11, 37, 63, 89. - 24 50 76 102 */
 {11, 24, 37, 50, 55, 57 | L1_SID_BLOCK, 59, 61, 63, 64, 66 | L1_SID_BLOCK, 68, 70, 72, 74 | L1_SID_BLOCK, 76, 89, 102},
 /* Sa 24, 50, 76, 102. - 11 37 63 89 */
 {11, 24, 37, 50, 63, 65, 67 | L1_SID_BLOCK, 69, 71, 73, 75 | L1_SID_BLOCK, 76, 78, 80, 82, 84 | L1_SID_BLOCK, 89, 102}
};

/* Identify all frames where transmission is mandatory in all modes full and */
/* half rate, SID + Sacch for sub channel 0 & 1 with an anticipation of one. */

#define L1_SUBF   (1 << 0)      // Blocks SUB for Tch/F
#define L1_SUBH0  (1 << 1)      // Blocks SUB for Tch/H 0
#define L1_SUBH1  (1 << 2)      // Blocks SUB for Tch/H 1
#define L1_SUBDH0 (1 << 3)      // Blocks SUB for Data Tch/H 0
#define L1_SUBDH1 (1 << 4)      // Blocks SUB for Data Tch/H 1

const u8 l1s_TchUlSubFrame [104] =
{
/* 0  */ /* L1_SUBH0, */ 0, L1_SUBH0, 0, L1_SUBH0, 0, L1_SUBH0, 0, 0, 0,
/* 1  */ 0, 0, L1_SUBF|L1_SUBH0|L1_SUBDH0, 0, L1_SUBH1, 0, L1_SUBH1, 0, L1_SUBH1, 0,
/* 2  */ L1_SUBH1, 0, 0, 0, 0, L1_SUBDH1, 0, 0, 0, 0,
/* 3  */ 0, 0, 0, 0, 0, 0, 0, 0, L1_SUBF|L1_SUBH0|L1_SUBDH0, 0,
/* 4  */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 5  */ 0, L1_SUBDH1, L1_SUBF|L1_SUBH0|L1_SUBDH0, L1_SUBF, L1_SUBF|L1_SUBH0|L1_SUBDH0, L1_SUBF, L1_SUBF|L1_SUBH0|L1_SUBDH0, L1_SUBF, L1_SUBF|L1_SUBH0|L1_SUBDH0, L1_SUBF,
/* 6  */ L1_SUBDH0, 0, L1_SUBDH0, 0, L1_SUBF|L1_SUBH0|L1_SUBDH0, L1_SUBDH0, L1_SUBH1, L1_SUBDH0, L1_SUBH1, L1_SUBDH0,
/* 7  */ L1_SUBH1|L1_SUBDH1, L1_SUBDH0, L1_SUBH1|L1_SUBDH1, 0, L1_SUBDH1, 0, L1_SUBDH1, L1_SUBDH1, 0, L1_SUBDH1,
/* 8  */ 0, L1_SUBDH1, 0, L1_SUBDH1, 0, L1_SUBDH1, 0, L1_SUBDH1, 0, L1_SUBDH1,
/* 9  */ L1_SUBF|L1_SUBH0|L1_SUBDH0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 10 */ 0, 0, 0, L1_SUBDH1, L1_SUBH0
};

// TCH/F                                   52, 53, 54, 55, 56, 57, 58, 59
// TCH/HS,subchannel 0                     0, 2, 4, 6, 52, 54, 56, 58
// TCH/HS,subchannel 1                     14, 16, 18, 20, 66, 68, 70, 72
//
// TCH/H,data,subchannel 0,uplink          52, 54, 56, 58, 60, 62, 65, 67, 69, 71
// TCH/H,data,subchannel 1,uplink          70, 72, 74, 76, 79, 81, 83, 85, 87, 89
// TCH/H,data,subchannel 0,downlink        56, 58, 60, 62, 65, 67, 69, 71, 73, 75
// TCH/H,data,subchannel 1,downlink        66, 68, 70, 72, 74, 76, 79, 81, 83, 85

#else
extern const u8  l1s_CcchPos  [9];
#ifndef __NO_GPRS__
extern const u8  l1s_PccchPos [12];
#endif
extern const u8  l1s_BcchNBDist [51];
extern const u8  l1s_BcchSdCombNBDist [51];
#ifndef __NO_GPRS__
extern const u8  l1s_PccchNBDist [52];
#endif
extern const u16 l1s_Sdcch4Pos [4][6];
extern const u16 l1s_Sdcch8Pos [8][6];
extern const u8  l1s_TchFSchd [26];
extern const u8  l1s_TchHSchd [2][26];
extern const u8  l1s_TchSubFrames [3][12];
extern const u8  l1s_DlDataSubFrames [2][14];
extern const u8  l1s_TchUlSubFrame [104];
#endif


#undef DefExtern
#endif
