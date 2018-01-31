/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/




































#ifndef TTABLES_H__
#define TTABLES_H__

#include "beginhdr.h"
#include "rvtypes.h"
#include "deblock.h"


#ifdef __cplusplus
extern "C" {
#endif

/*/////////////////////////////////////////////////////// */
/* offsets for 4x4 blocks */
extern const U8 xoff[16];
extern const U8 yoff[16];
extern const U8 yoffi[16];

/* Offset for 8x8 blocks */
extern const U8 xoff8[4];
extern const U8 yoff8[4];

/* Offset for 16x16 block */
extern const U8 yoff16[1];

/*//////////////////////////////////////////////////////// */
/* offset for ipred blocks (a pair of 4x4 block) */
extern const U8 xoffipred[8];
extern const U8 yoffipred[8];

/*//////////////////////////////////////////////////////// */
/* scan matrices */
extern const U8 single_scan[16];
extern const U8 double_scan[16];
extern const U8 dec_single_scan[16];
extern const U8 dec_double_scan[16];

/*//////////////////////////////////////////////////////// */
/* Mapping from luma QP to chroma QP */
extern const U8 chroma_quant[32];
extern const U8 chroma_quant_DC[32];

/* Mapping for QP applied to luma DC coefs */
extern const U8 luma_intra_quant_DC[32];
extern const U8 luma_quant_DC_RV8[32];
extern const U8 luma_inter_quant_DC[32];

extern const U8 enc_aic_prob[2];

/* Legacy Intra prediction probabilities */
extern const U8 dec_iprob_rv8[162];
extern const U8 dec_aic_prob_rv8[10/*A*/][10/*B*/][9/*prob*/];


/*///////////////////////////////////// */
/* Tables for decoding CBP */
extern const U8 dec_cbp_intra[64];
extern const U8 dec_cbp_inter[64];
extern const U8 dec_cbp_intra16x16[64];
extern const U32 CBP4x4Mask[24];

/*////////////////////////////////////////////////////// */
/* Mappings from block number in loop to 8x8 block number */
extern const U8 subblock_block_ss[24];
extern const U8 subblock_block_ds[32];

/* Mapping from 8x8 block number to 4x4 block number */
extern const U8 block_subblock_mapping[16];

/* Mapping from block number in loop to 8x8 block number */
extern const U8 subblock_block_mapping[16];

/* chroma block re-mapping */
extern const U8 subblock_block_chroma[8];

/* Mapping from chroma block number to luma block number where the vector */
/* to be used for chroma can be found */
extern const U8 chroma_block_address[4];
#ifdef INTERLACED_CODE
extern const U8 chroma_block_address_i[4];
#endif


/*/////////////////////////////////////////////////////// */
/* Tables for decoding inter coefficients */
extern const U8 ntab_inter[4][8][2];
extern const U8 levrun_inter_tab[16];

/*//////////////////////////////////////////////////////// */
/* Tables for decoding intra coefficients */
extern const U8 ntab_intra[4][8][2];
extern const U8 levrun_intra_tab[8];

/*///////////////////////////////////////////////////////// */
/* Tables for decoding chroma DC coefficients */
extern const U8 ntab_dc[2][2][2];
extern const U8 levrun_dc_tab[4];


/* Encoder tables for coefficient encoding */
extern const U8 enc_levrun_inter[16];
extern const U8 enc_levrun_intra[8];
extern const U8 enc_ntab_inter[10][4];
extern const U8 enc_ntab_intra[5][9];
extern const U8 enc_ntab_cDC[2][2];
extern const U8 enc_levrun_cDC[4];


/*////////////////////////////////////////////////////////////////// */
/* Quantization tables */
extern const I32 BquantTable[32];
#ifdef APK
extern const I32 B[32];
#endif

/*////////////////////////////////////////////////////////////////// */
/* Translation from block number in chroma loop to */
/* actual chroma block number */
extern const U8 block_trans[8];


/*/////////////////////////////////////////////////////////////////// */
/* RD multiplier (Lagrange factor) */
extern const U8 rd_quant[32];
extern const U8 rd_quant2[32];
extern const int QP2QUANT[32];


#ifdef DEBUG
/* R-D Lagrange parameter to use for MB coding evaluation with SSD for distortion */
extern const double RDLPSSD[32];
/* R-D Lagrange parameter to use for MB coding evaluation with SAD for distortion */
extern const double RDLPSAD[32];
#endif

/* Table used to prevent single or 'expensive' coefficients are coded */
extern const U8 coeff_importance[16];
extern const I32 COEFF_BLOCK_THRESHOLD;
extern const I32 COEFF_MB_THRESHOLD ;

extern const I32 empty_block[16];

/* Tables used for finding MV prediction */
extern const U8 not_available16[1];
extern const U8 not_available8[4] ;
extern const U8 not_available4[16];

/* Tables used for finding if a block is on the edge */
/* of a macroblock */
extern const U8 left_edge_tab4[4];
extern const U8 top_edge_tab4[4];
extern const U8 left_edge_tab16[16];
extern const U8 top_edge_tab16[16];


/* chroma vector mapping */
extern const U8 c2x2m[4][4];


/* empty threshold tables */
extern const U32 EmptyThreshold_1V[32];
extern const U32 EmptyThreshold_4V[32];
extern const U32 EmptyThreshold_16V[32];

extern const U32 EmptyThreshold_2V[32];
extern const U32 EmptyThreshold_8V[32];


#ifdef COMPLIANCE_CLIP
#define CLAMP_BIAS  (128+128+32) /* Bias in clamping table */
#else
#define CLAMP_BIAS  128 /* Bias in clamping table */
#endif
#define CLIP_RANGE  (CLAMP_BIAS + 256 + CLAMP_BIAS)

extern const U8  ClampTbl[CLIP_RANGE];

#define ClampVal(x)  (ClampTbl[CLAMP_BIAS + (x)])

#define ClampTblLookup(x, y) ClampVal((x) + (y))



/* gc_Zeroes is a constant array of zeroes.  It is useful for the decoder's */
/* reconstruction functions, to quickly emulate a non-coded block or subblock. */
/* The "gc_" prefix indicates this is a global, constant table. */
/* Because it's constant, you can use it in contexts expecting other */
/* than an I32, via an appropriate cast. */

extern const I32 gc_Zeroes[8*8];

/* lookup table indexed by block number, return 0xff if upper right predictors */
/* (EFGH) are valid for the block, based upon its position in the macroblock. */
extern const U8 uBlockURPredOK[];

/* lookup table indexed by block number, return 0xff if lower left predictors */
/* (MNOP) are valid for the block, based upon its position in the macroblock. */
extern const U8 uBlockLLPredOK[];


#ifdef INTERLACED_CODE
extern const U8 uBlockURPredOKi[];
extern const U8 uBlockLLPredOKi[];
extern const U8 top_edge_tab16i[16];
extern const U32 uIBlockEdgeAdj[16];
extern const U32 uModeAboveFactI[16];
extern const I32 iModeLeftFactI[16];
#endif

extern const unsigned char  aic_top_vlc[16]; /* Flawfinder: ignore */
extern const unsigned short aic_table_index[20];
//extern const unsigned short aic_2mode_vlc[20][81]; hx chg
extern const unsigned int aic_2mode_vlc[20][81];
extern const unsigned char  aic_1mode_vlc[100][9]; /* Flawfinder: ignore */

#if defined(_WIN32) && defined(_M_IX86)

#define GET_VLC_LENGTH_INFO_SHORT(info,length)\
    _asm mov eax, (info)                    \
    _asm bsr edx, eax                       \
    _asm btr eax, edx                       \
    _asm mov (info), eax                    \
    _asm mov (length), edx

#define GET_VLC_LENGTH_INFO_CHAR(info,length)\
    _asm mov eax, (info)                    \
    _asm bsr edx, eax                       \
    _asm btr eax, edx                       \
    _asm mov (info), eax                    \
    _asm mov (length), edx

#else

#define GET_VLC_LENGTH_INFO_SHORT(info,length)      \
    (length) =                      \
    (info < 2)?(0):                 \
    (info < 4)?(1):                 \
    (info < 8)?(2):                 \
    (info < 16)?(3):                \
    (info < 32)?(4):                \
    (info < 64)?(5):                \
    (info < 128)?(6):               \
    (info < 256)?(7):               \
    (info < 512)?(8):               \
    (info < 1024)?(9):              \
    (info < 2048)?(10):             \
    (info < 4096)?(11):             \
    (info < 8192)?(12):             \
    (info < 16384)?(13):            \
    (info < 32768)?(14):(15);       \
    info <<= (32 - length);         \
    info >>= (32 - length);

#define GET_VLC_LENGTH_INFO_CHAR(info,length)       \
    (length) =                      \
    (info < 2)?(0):                 \
    (info < 4)?(1):                 \
    (info < 8)?(2):                 \
    (info < 16)?(3):                \
    (info < 32)?(4):                \
    (info < 64)?(5):                \
    (info < 128)?(6):(7);           \
    info <<= (32 - length);         \
    info >>= (32 - length);

#endif  /*  defined(_WIN32) && defined(_M_IX86) */

extern const unsigned char mbtype_p_dec_tab[7]; /* Flawfinder: ignore */
extern const unsigned char mbtype_p_enc_tab[12]; /* Flawfinder: ignore */
extern const unsigned char mbtype_b_dec_tab[6]; /* Flawfinder: ignore */
extern const unsigned char mbtype_b_enc_tab[12]; /* Flawfinder: ignore */
extern const unsigned char mbtype_pframe_vlc [7][8][2]; /* Flawfinder: ignore */
extern const unsigned char mbtype_bframe_vlc [6][7][2]; /* Flawfinder: ignore */

#ifdef INTERLACED_CODE
extern const unsigned char mbtype_pi_dec_tab[10]; /* Flawfinder: ignore */
extern const unsigned char mbtype_bi_dec_tab[10]; /* Flawfinder: ignore */

extern const unsigned char  mbtype_pi_enc_tab[15];
extern const unsigned char  mbtype_bi_enc_tab[19]; /* Flawfinder: ignore */
extern const unsigned char  mbtype_pframei_vlc [10][11][2]; /* Flawfinder: ignore */
extern const unsigned char  mbtype_bframei_vlc [10][11][2]; /* Flawfinder: ignore */
#endif

extern const short ditherL[16];
extern const short ditherR[16];
extern const I32 alpha_tab[32] ;
extern const U8 beta_tab[32];

/* mapping of DeblockingQPAdjust */
extern const U8 DBQPA[6];

extern const I8 clip_table[3][QP_LEVELS] ;


#ifdef __cplusplus
}
#endif



#endif /* TTABLES_H__ */
