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
















#ifndef __DEC4X4_H__
#define __DEC4X4_H__    1

#ifdef __cplusplus
extern "C" {  /* be nice to our friends in C++ */
#endif

#if 1// move from dec4x4md.c sheen

/* max. # of bits that can be retrieved at no extra cost: */
#define BS_MAXTRYBITS   16

/* maximum code length: */
#define MAX_DEPTH   16
#if MAX_DEPTH > BS_MAXTRYBITS
#error "BS_TRYBITS() must be able to process the longest codeword!!!"
#endif

/*
 * 4x4 transform coefficients encoding.
 *
 * We attempt to group most frequently used combinations of events
 * (values of coefficients) into units called descriptors.
 *
 * Thus, for each block we will use a 4x4 descriptor
 * that combines most probable values of 4 lowest-frequency
 * coefficients with indicators of non-zero elements in
 * the remaining (high-frequency) 2x2 sub-blocks.
 *
 * In turn, if any of these 2x2 sub-blocks is coded, it is being
 * represented by another descriptor, combining the most probable
 * values of each of the coefficients.
 *
 * If any of the coefficints is larger than levels included
 * in the descriptor, it is being encoded separately as
 * <level + sign> pair, where level is the (relative) absolute
 * value of the coefficient, and sign is a bit indicating whether
 * the coefficient is positive or negative.
 */

/* extra level descriptor: */
#define MAX_EXTRA_LEVEL 23
#define MAX_EXTRA_BITS  8

/* abs. value (level) of quantized transform coefficients: */
#define MAX_LEVEL       (MAX_EXTRA_LEVEL + (1 << (MAX_EXTRA_BITS+1)) )

/* the number of level_dsc codewords: */
#define MAX_LEVEL_DSC   (MAX_EXTRA_LEVEL+MAX_EXTRA_BITS+1)

/*
 * Block descriptors and level thresholds:
 *
 * 2x2 block descriptor:     l0 values:
 *                             0 -> l0 = 0
 *  l0 | l1                    1 -> abs(l0) = 1
 *  ---+---                    2 -> abs(l0) = 2
 *  l2 | l3                    3 -> abs(l0) >= 3
 *
 * 4x4 block descriptor:     l1..3 values:
 *                             0 -> lx = 0
 *  l0 | l1 |                  1 -> abs(lx) = 1
 *  ---+----|   l4             2 -> abs(lx) >= 2
 *  l2 | l3 |
 *  --------+---------       l4..6 values:
 *          |                  0 -> lx = 0
 *    l5    |   l6             1 -> lx != 0
 *          |                      -> 2x2 descriptor to follow
 */
#define MAX_0_LEVEL     4
#define MAX_123_LEVEL   3
#define MAX_456_LEVEL   2

/* the number of 2x2 descriptor codewords: */
#define MAX_2x2_DSC             \
        (MAX_0_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL)   /* 108 */

/* 4x4 descriptor: */
#define MAX_4x4_DSC             \
        (MAX_0_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL * MAX_123_LEVEL \
            * MAX_456_LEVEL * MAX_456_LEVEL * MAX_456_LEVEL)            /* 864 */

/*
 * 8x8 block descriptors.
 *
 * These are the additional codewords that indicate which of
 * the 4x4 blocks in the 8x8 region are actually coded.
 */
#define MAX_8x8_DSC 16

/*
 * CBP macroblock descriptor:
 *
 *    y0  | y1    yi values:
 *        |         0 -> the corresponding 8x8 luma block not coded
 *  ------+------   1 -> 8x8 descriptor to follow
 *    y2  | y3
 *        |
 *  +             ci values:
 *     c0 | c1      0 -> cr[i] = 0 and cb[i] = 0
 *     ---+---      1 -> (cr[i] = 0 & cb[i] != 0) or (cr[i] != 0 & cb[i] = 0)
 *     c2 | c3         -> 1 bit to follow
 *                  2 -> cr[i]!=0 and cb[i]!=0
 */
#define MAX_CBP     (16*3*3*3*3)    /* 1296 */

/*
 * QP range partition
 *
 * In order to reduce # of tables, we partition
 * the range of all possible qp values into a set of
 * regions (which, in turn will be associated with the
 * corresponding set of tables).
 */
#define MAX_QP  30
#define MAX_INTRA_QP_REGIONS 5

#define MAX_INTER_QP_REGIONS 7


/*
 * A set of compression/decomresssion tables for each
 * quantization range and frame type (intra/inter) modes:
 */

/* 4x4 descriptor tables: */
typedef struct
{
    unsigned int next [MAX_DEPTH+2];
    unsigned int base [MAX_DEPTH+2];
    unsigned int offset [MAX_DEPTH+1];
    unsigned short symbol [MAX_4x4_DSC];
} _4x4_DSC_DECODE_TABLE;

/* 2x2 descriptor table: */
typedef struct
{
    unsigned int next [MAX_DEPTH+2];
    unsigned int base [MAX_DEPTH+2];
    unsigned int offset [MAX_DEPTH+1];
    unsigned char symbol [MAX_2x2_DSC]; /* Flawfinder: ignore */
} _2x2_DSC_DECODE_TABLE;

/* extra large levels table: */
typedef struct
{
    unsigned int next [MAX_DEPTH+2];
    unsigned int base [MAX_DEPTH+2];
    unsigned int offset [MAX_DEPTH+1];
    unsigned char symbol [MAX_LEVEL_DSC]; /* Flawfinder: ignore */
} LEVEL_DSC_DECODE_TABLE;

/* CBP encoding/decoding tables: */
typedef struct
{
    /* CBP main descriptor: */
    struct
    {
        unsigned int next [MAX_DEPTH+2];
        unsigned int base [MAX_DEPTH+2];
        unsigned int offset [MAX_DEPTH+1];
        unsigned short symbol [MAX_CBP];
    } cbp;

    /* 8x8 descriptors: */
    struct
    {
        unsigned int next [MAX_DEPTH+2];
        unsigned int base [MAX_DEPTH+2];
        unsigned int offset [MAX_DEPTH+1];
        unsigned char symbol [MAX_8x8_DSC]; /* Flawfinder: ignore */
    } _8x8_dsc [4]; /* context = # of encoded 8x8 blocks - 1 */

} CBP_DECODE_TABLE;

/* combined encoding/decoding tables: */
typedef struct
{
    /* cbp tables: */
    CBP_DECODE_TABLE cbp [2];         /* 2 types of intra CBPs   */
    /* 1, if normal intra macroblock 2, if DC-removed intra macroblock */
    /* luma descriptors: */
    struct
    {
        _4x4_DSC_DECODE_TABLE _4x4_dsc[3]; /* 3 types of intra luma blocks!!! */
        _2x2_DSC_DECODE_TABLE _2x2_dsc[2];
    } luma;
    /* chroma descriptors: */
    struct
    {
        _4x4_DSC_DECODE_TABLE _4x4_dsc;
        _2x2_DSC_DECODE_TABLE _2x2_dsc[2];
    } chroma;
    /* extra levels: */
    LEVEL_DSC_DECODE_TABLE level_dsc;

} INTRA_DECODE_TABLES;

typedef struct
{
    /* cbp table: */
    CBP_DECODE_TABLE cbp;
    /* luma descriptors: */
    struct
    {
        _4x4_DSC_DECODE_TABLE _4x4_dsc;
        _2x2_DSC_DECODE_TABLE _2x2_dsc[2];
    } luma;
    /* chroma descriptors: */
    struct
    {
        _4x4_DSC_DECODE_TABLE _4x4_dsc;
        _2x2_DSC_DECODE_TABLE _2x2_dsc[2];
    } chroma;
    /* extra levels: */
    LEVEL_DSC_DECODE_TABLE level_dsc;

} INTER_DECODE_TABLES;


#endif

/*
 * Initializes 4x4 decoding engine.
 * Use:
 *  void decode_4x4_init ();
 * Returns:
 *  none.
 */
int decode_4x4_init ();

/*
 * Decodes a 4x4 block.
 * Use:
 *  void decode_4x4_block (short *block, int intra, int chroma, int qp,
 *      unsigned char **p_pbs, unsigned int *p_bitOffset);
 * Input:
 *  block - a pointer to 4x4 progressively scanned coefficients
 *  intra - 0, if inter coded block
 *          1, if normal intra block
 *          2, if DC-removed intra block
 *          3, if double-transformed intra DC block
 *  chroma - 1, if chroma block, 0 if luma
 *  qp    - quantization parameter (1..31)
 *  p_pbs - pointer to pointer to the current position in the bitstream
 *  p_bitOffset - pointer to the bit position in the byte pointed by *p_pbs
 * Returns:
 *  none.
 */
#if 0 //hx chg
void decode_4x4_block (short *block, int intra, int chroma, int qp,
                       unsigned char **p_pbs, unsigned int *p_bitOffset);
#endif
void decode_4x4_block (short *block, unsigned char **p_pbs, unsigned int *p_bitOffset);
void decode_4x4_gettab(int intra, int chroma, int qp);

/*
 * Decodes a cbp descriptor.
 * Use:
 *  unsigned int decode_cbp (int intra, int qp,
 *      unsigned char **p_pbs, unsigned int *p_bitOffset);
 * Input:
 *  intra - 0, if inter coded macroblock
 *          1, if normal intra macroblock
 *          2, if DC-removed intra macroblock
 *  qp    - quantization parameter (1..31)
 *  p_pbs - pointer to pointer to the current position in the bitstream
 *  p_bitOffset - pointer to the bit position in the byte pointed by *p_pbs
 * Returns:
 *  decoded cbp descriptor
 */
unsigned int decode_cbp (int intra, int qp,
                         unsigned char **p_pbs, unsigned int *p_bitOffset);

#ifdef __cplusplus
}
#endif

#endif /* __DEC4X4_H__ */

