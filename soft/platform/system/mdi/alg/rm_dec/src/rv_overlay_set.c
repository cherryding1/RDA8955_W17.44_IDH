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



#include "dec4x4m.h"
#include "dec4x4t.h"
#include "bsx.h"
#include "rvdebug.h"
#include "rvdecoder.h"
#include "tables.h"

#ifdef SHEEN_VC_DEBUG

//added by huangx
#include "vpp_rmvb.h"
#include "vppp_rmvb_asm_map.h"
#include "voc2_define.h"
#include "hal_voc.h"

#else

#include "vpp_rm_dec_asm.h"

#endif

/***************move from dec4x4md.c *************************/

#if 1

typedef struct
{
    unsigned char *pbs;
    unsigned int bitBuffer;
    unsigned int bitOffset;
} BS_STRUCT;

BS_STRUCT layer_bs,*bs;


/*
 * Bitstream processing macros.
 *
 * From ILVC CBitstream we are being given two basic parameters:
 *  pbs - pointer to current byte position in the bitstream
 *  bitOffset - offset of the next available bit in a byte
 *    pointed by pbs;
 * Since for some odd reason CBitstream uses big-endian bitpacking,
 * bitOffset actually shows the offset from the rightmost bit in a byte;
 * also, there is no alignment in pbs...
 *
 * We will use both of these variables to get initial information
 * about the bitstream, and, once we are done with our own processing,
 * we will update them accordingly.
 *
 * Internally we will use one more variable:
 *  bitBuffer - a 32 bit integer, containing a copy of the last
 *    incomplete byte in the bitstream plus up to 3 bytes of new
 *    information to be added to the bitstream.
 */
#if 0//hx chg.
/* initialize local bitstream variables: */
#define BS_OPEN(p_pbs, p_bitOffset, read)               \
    register unsigned char *pbs = *p_pbs;               \
    register unsigned int bitOffset = *p_bitOffset;     \
    register unsigned int bitBuffer = (unsigned int)pbs[0] << 24;   \
    /* check if we need to load the rest of bytes: */   \
    if (read) {                                         \
        bitBuffer |= (unsigned int)pbs[1] << 16;        \
        bitBuffer |= (unsigned int)pbs[2] << 8;         \
        bitBuffer |= (unsigned int)pbs[3];              \
        bitBuffer <<= bitOffset;                        \
    }
#endif

/* initialize local bitstream variables: */
#define  BS_OPEN(p_pbs,p_bitOffset, read)           \
    bs = &layer_bs;                             \
    bs->pbs = *p_pbs;                               \
    bs->bitOffset = *p_bitOffset;                   \
    bs->bitBuffer = (unsigned int)bs->pbs[0] << 24;     \
    if (read) {                                                         \
        bs->bitBuffer |= (unsigned int)bs->pbs[1] << 16;    \
        bs->bitBuffer |= (unsigned int)bs->pbs[2] << 8;     \
        bs->bitBuffer |= (unsigned int)bs->pbs[3];              \
        bs->bitBuffer<<= bs->bitOffset;                         \
    }



/* terminates the use of local bitstream variables: */
#if 0//hx chg.
#define BS_CLOSE(p_pbs, p_bitOffset, write)             \
    /* check if we need to flush bitBuffer: */          \
    if (write) {                                        \
        pbs[0] = (unsigned char) (bitBuffer >> 24);     \
        pbs[1] = (unsigned char) (bitBuffer >> 16);     \
        pbs[2] = (unsigned char) (bitBuffer >> 8);      \
        pbs[3] = (unsigned char) bitBuffer;             \
    }                                                   \
    /* update ILVC CBitstream variables: */             \
    *p_pbs = pbs + (bitOffset >> 3);                    \
    *p_bitOffset = (bitOffset & 7);
#endif

/* terminates the use of local bitstream variables: */
#define BS_CLOSE(p_pbs, p_bitOffset)                \
    *p_pbs = bs->pbs + (bs->bitOffset >> 3);                \
    *p_bitOffset = (bs->bitOffset & 7);


/* retrieves a bit vector from the bitstream. */
#if 0//hx chg.
#define BS_TRYBITS(len)    ((unsigned int)bitBuffer >> (32-(len)))
#endif
#define BS_TRYBITS(len)    ((unsigned int)bs->bitBuffer >> (32-(len)))

/* shifts the current position in the bitstream: */
#if 0//hx chg
#define BS_SKIPBITS(len)                                \
    do {                                                \
        bitOffset += (len);                             \
        bitBuffer <<= (len);                            \
        if (bitOffset > 32-BS_MAXTRYBITS) {             \
            /* normalize pbs/bitOffset: */              \
            pbs += bitOffset >> 3; bitOffset &= 7;      \
            /* reload bitBuffer: */                     \
            bitBuffer =  (unsigned int)pbs[0] << 24;    \
            bitBuffer |= (unsigned int)pbs[1] << 16;    \
            bitBuffer |= (unsigned int)pbs[2] << 8;     \
            bitBuffer |= (unsigned int)pbs[3];          \
            bitBuffer <<= bitOffset;                    \
        }                                               \
    } while (0)
#endif

/* shifts the current position in the bitstream: */
#define BS_SKIPBITS(len)                                \
    do {                                                \
        bs->bitOffset += (len);                             \
        bs->bitBuffer<<= (len);                            \
        if (bs->bitOffset > 16) {             \
            bs->pbs += bs->bitOffset >> 3; bs->bitOffset &= 7;      \
            /* reload bitBuffer: */                     \
            bs->bitBuffer =  (unsigned int)bs->pbs[0] << 24;    \
            bs->bitBuffer |= (unsigned int)bs->pbs[1] << 16;    \
            bs->bitBuffer |= (unsigned int)bs->pbs[2] << 8;     \
            bs->bitBuffer |= (unsigned int)bs->pbs[3];          \
            bs->bitBuffer <<= bs->bitOffset;                    \
        }                                               \
    } while (0)


#if 0//hx chg
/* Reads a bit vector from the bitstream. */
#define BS_GETBITS(bits,len)                            \
    do {                                                \
        bits = BS_TRYBITS(len);                         \
        BS_SKIPBITS(len);                               \
    } while (0)
#endif

/* Reads a bit vector from the bitstream. */
unsigned int BS_GETBITS(unsigned int len)
{
    unsigned int bits;
    bits = BS_TRYBITS(len);
    BS_SKIPBITS(len);
    return bits;
}

/*
 * Macros for fast Huffman decoding.
 *
 * We use a linear-search based variant of Moffat-Turpin/Brodnik-Carlsson
 * algorithm for decoding canonic Huffman codes.
 *
 * The decoding data structure consists of the following items:
 *  next [MAX_DEPTH] - list of the sub-sequent non-empty levels
 *  base [MAX_DEPTH] - left-justified smallest canonical codewords of given length
 *  offset [MAX_DEPTH] - indices of base canonical codewords
 *  symbol [MAX_SYMBOLS] - maps canonical codeword back to the original alphabet
 */

/* decode a symbol using Moffat-Turpin/Brodnik-Carlsson algorithm: */
#if 0//hx chg.
#define HUF_DECODE(val,n,next,base,offset,symbol)       \
     do {                                                \
         /* get start level: */                          \
         register unsigned int l = next[0];              \
         register unsigned int bitBuffer2 = bitBuffer & 0xffff0000;              \
         /* find code length: */                         \
         while (/* next[l] != l && */                    \
             (bitBuffer2 >= base [next[l]]) ) \
             l = next[l];   \
        /* decode canonic Huffman code: */              \
        val = symbol[offset[l] + ((unsigned int)        \
                    (bitBuffer - base[l]) >> (32-l))];  \
        /* skip l bits: */                              \
        BS_SKIPBITS(l);                                 \
    } while (0)
#endif

#endif

unsigned int RM_HufDecodeAsm(unsigned int *next,unsigned int *base,unsigned int *offset, BS_STRUCT *loc_bs)
{
    unsigned int val;
    unsigned int l = next[0];
    unsigned int bitBuffer2 = loc_bs->bitBuffer & 0xffff0000;
    while ((bitBuffer2 >= base [next[l]]) )
        l = next[l];

    val = offset[l] + ((unsigned int)(loc_bs->bitBuffer - base[l]) >> (32-l));

    loc_bs->bitOffset += l;
    loc_bs->bitBuffer<<= l;
    if (loc_bs->bitOffset > 16)
    {
        loc_bs->pbs += loc_bs->bitOffset >> 3; loc_bs->bitOffset &= 7;
        loc_bs->bitBuffer =  (unsigned int)loc_bs->pbs[0] << 24;
        loc_bs->bitBuffer |= (unsigned int)loc_bs->pbs[1] << 16;
        loc_bs->bitBuffer |= (unsigned int)loc_bs->pbs[2] << 8;
        loc_bs->bitBuffer |= (unsigned int)loc_bs->pbs[3];
        loc_bs->bitBuffer <<= loc_bs->bitOffset;
    }

    return val;
}


extern INTRA_DECODE_TABLES *intra_decode_tables;//[MAX_INTRA_QP_REGIONS]; sheen
extern INTER_DECODE_TABLES *inter_decode_tables;//[MAX_INTER_QP_REGIONS]; sheen

/* descriptor to level conversion tables: */
extern unsigned char *dsc_to_l0;// [MAX_2x2_DSC]; /* Flawfinder: ignore */ sheen
extern unsigned char *dsc_to_l1;// [MAX_2x2_DSC]; /* Flawfinder: ignore */
extern unsigned char *dsc_to_l2;// [MAX_2x2_DSC]; /* Flawfinder: ignore */
extern unsigned char *dsc_to_l3;// [MAX_2x2_DSC]; /* Flawfinder: ignore */

/*
 * Conversion between progressively scanned 4x4 block bits
 * and CBP descriptor:
 *  Y:            Cr:     Cb
 *    0  1  2  3   16 17    20 21
 *    4  5  6  7   18 19    22 23
 *    8  9 10 11
 *   12 13 14 15
 *
 *  cbp4x4 := [...|23..16|15..8|7..0]
 */

/* chroma cbp to level conversion tables: */
#define ccbp_to_c0  dsc_to_l0
#define ccbp_to_c1  dsc_to_l1   /* just share the tables */
#define ccbp_to_c2  dsc_to_l2
#define ccbp_to_c3  dsc_to_l3

/* luma descriptor to cbp4x4 conversion tables: */
extern unsigned int *y_to_cbp4x4;// [MAX_8x8_DSC]; sheen

/* 8x8 cbp to context-predictor conversion table: */
extern unsigned int *cbp_to_cx;// [16]; sheen

//hx chg.
_4x4_DSC_DECODE_TABLE *p_4x4_dsc;
_2x2_DSC_DECODE_TABLE *p_2x2_dsc;
LEVEL_DSC_DECODE_TABLE *p_level_dsc;

#define DECODE_COEFF(coef,p_level_dsc1, dsc_level, max_level) \
{                                                       \
   /*register*/ int s, c, l;                                                \
   c = dsc_level;                                           \
   if (c == max_level-1)                                    \
   {                                                            \
      s = RM_HufDecodeAsm(p_level_dsc1->next,p_level_dsc1->base,p_level_dsc1->offset, bs); \
      s = p_level_dsc1->symbol[s];                          \
    l = s - 23;                                             \
      if (l > 0)                     \
    {      s = BS_TRYBITS(l);   \
        BS_SKIPBITS(l);         \
            s += (1U << l) + 22;     \
       }                           \
      c += s;                   \
    }                              \
    s = BS_TRYBITS(1);    \
    BS_SKIPBITS(1);    \
    if (s) c =- c;               \
    coef= c; \
}


void decode_4x4_gettab(int intra, int chroma, int qp)
{
    if (intra)
    {
        /*register*/ INTRA_DECODE_TABLES *ct = intra_decode_tables + intra_qp_to_idx [qp];
        if (!chroma)
        {
            p_4x4_dsc = &(ct->luma._4x4_dsc [intra-1]); /* !!! */
            p_2x2_dsc = &(ct->luma._2x2_dsc [0]);
        }
        else
        {
            p_4x4_dsc = &(ct->chroma._4x4_dsc);
            p_2x2_dsc = &(ct->chroma._2x2_dsc [0]);
        }
        p_level_dsc = &(ct->level_dsc);
    }
    else     /* inter */
    {
        /*register*/ INTER_DECODE_TABLES *ct = inter_decode_tables + inter_qp_to_idx [qp];
        if (!chroma)
        {
            p_4x4_dsc = &(ct->luma._4x4_dsc);
            p_2x2_dsc = &(ct->luma._2x2_dsc [0]);
        }
        else
        {
            p_4x4_dsc = &(ct->chroma._4x4_dsc);
            p_2x2_dsc = &(ct->chroma._2x2_dsc [0]);
        }
        p_level_dsc = &(ct->level_dsc);
    }

}

//hx end.


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
#if 0//hx chg
void decode_4x4_block (short *block, int intra, int chroma, int qp,
                       unsigned char **p_pbs, unsigned int *p_bitOffset)
{
    /* decode tables: */
    _4x4_DSC_DECODE_TABLE *p_4x4_dsc;
    _2x2_DSC_DECODE_TABLE *p_2x2_dsc;
    LEVEL_DSC_DECODE_TABLE *p_level_dsc;
    register unsigned int d, d2;

    /* init bit-stream vars: */
    BS_OPEN(p_pbs,p_bitOffset,1);


    /* load code tables: */
    if (intra)
    {
        register INTRA_DECODE_TABLES *ct = intra_decode_tables + intra_qp_to_idx [qp];
        if (!chroma)
        {
            p_4x4_dsc = &(ct->luma._4x4_dsc [intra-1]); /* !!! */
            p_2x2_dsc = &(ct->luma._2x2_dsc [0]);
        }
        else
        {
            p_4x4_dsc = &(ct->chroma._4x4_dsc);
            p_2x2_dsc = &(ct->chroma._2x2_dsc [0]);
        }
        p_level_dsc = &(ct->level_dsc);
    }
    else     /* inter */
    {
        register INTER_DECODE_TABLES *ct = inter_decode_tables + inter_qp_to_idx [qp];
        if (!chroma)
        {
            p_4x4_dsc = &(ct->luma._4x4_dsc);
            p_2x2_dsc = &(ct->luma._2x2_dsc [0]);
        }
        else
        {
            p_4x4_dsc = &(ct->chroma._4x4_dsc);
            p_2x2_dsc = &(ct->chroma._2x2_dsc [0]);
        }
        p_level_dsc = &(ct->level_dsc);
    }

    /* some local macros: */
#   define DECODE_COEFF(coef,dsc_level,max_level) \
        do {                                    \
            register int s, c;                  \
            if ((c = dsc_level) != 0) {         \
                if (c == max_level-1) {         \
                    HUF_DECODE(s,MAX_LEVEL_DSC, \
                        p_level_dsc->next,p_level_dsc->base,  \
                        p_level_dsc->offset,p_level_dsc->symbol);\
                    /* check if escape code: */ \
                    if (s >= MAX_EXTRA_LEVEL-1) { \
                        register int l;         \
                        l = s - (MAX_EXTRA_LEVEL-1); \
                        if (l > 1) {            \
                            BS_GETBITS(s, l-1); \
                            s += (1U << (l-1)) + MAX_EXTRA_LEVEL-1; \
                        }                       \
                    }                           \
                    c += s;                     \
                }                               \
                BS_GETBITS(s,1);                \
                if (s) c =- c;                  \
                coef = c; /* send to the output */ \
            }                                   \
        } while (0)

#   define DECODE_2x2_COEFFS(dsc,c0,c1,c2,c3)   \
        do {                                    \
            DECODE_COEFF(c0,dsc_to_l0[dsc],MAX_0_LEVEL);    \
            DECODE_COEFF(c1,dsc_to_l1[dsc],MAX_123_LEVEL);  \
            DECODE_COEFF(c2,dsc_to_l2[dsc],MAX_123_LEVEL);  \
            DECODE_COEFF(c3,dsc_to_l3[dsc],MAX_123_LEVEL);  \
        } while (0)

    /* step #1: set all coeffs to 0: done by VOC*/
    //memset(block, 0, 16 * sizeof (block[0]));   /* !!! */

    /* decode 4x4 descriptor & first 2x2 coefficients: */
    HUF_DECODE(d,MAX_4x4_DSC,p_4x4_dsc->next,p_4x4_dsc->base,p_4x4_dsc->offset,p_4x4_dsc->symbol);
    d2 = d >> 3;
    if (d2) DECODE_2x2_COEFFS(d2, block[0], block[1], block[4], block[5]);

    /* decode the remaining 2x2 blocks: */
    if (d & 4)
    {
        HUF_DECODE(d2,MAX_2x2_DSC,p_2x2_dsc[0].next,p_2x2_dsc[0].base,p_2x2_dsc[0].offset,p_2x2_dsc[0].symbol);
        DECODE_2x2_COEFFS(d2, block[2], block[3], block[6], block[7]);
    }
    if (d & 2)
    {
        HUF_DECODE(d2,MAX_2x2_DSC,p_2x2_dsc[0].next,p_2x2_dsc[0].base,p_2x2_dsc[0].offset,p_2x2_dsc[0].symbol);
        DECODE_2x2_COEFFS(d2, block[8], block[12],block[9], block[13]);
    }
    if (d & 1)
    {
        HUF_DECODE(d2,MAX_2x2_DSC,p_2x2_dsc[1].next,p_2x2_dsc[1].base,p_2x2_dsc[1].offset,p_2x2_dsc[1].symbol);
        DECODE_2x2_COEFFS(d2, block[10],block[11],block[14],block[15]);
    }

    /* update bit-stream vars & exit: */
    BS_CLOSE(p_pbs, p_bitOffset, 0);
}
#endif

void decode_4x4_block (short *block, unsigned char **p_pbs, unsigned int *p_bitOffset)
{
    /* decode tables: */
    /*register*/ unsigned int d, d2;

    /* init bit-stream vars: */
    BS_OPEN(p_pbs,p_bitOffset,1);

    /* load code tables: */

#define DECODE_2x2_COEFFS(dsc,c0,c1,c2,c3)   \
        do {                                    \
            if(dsc_to_l0[dsc]!=0)               \
                DECODE_COEFF(c0,p_level_dsc,dsc_to_l0[dsc],MAX_0_LEVEL);    \
            if(dsc_to_l1[dsc]!=0)               \
                DECODE_COEFF(c1,p_level_dsc,dsc_to_l1[dsc],MAX_123_LEVEL);  \
            if(dsc_to_l2[dsc]!=0)               \
                DECODE_COEFF(c2,p_level_dsc,dsc_to_l2[dsc],MAX_123_LEVEL);  \
            if(dsc_to_l3[dsc]!=0)               \
                DECODE_COEFF(c3,p_level_dsc,dsc_to_l3[dsc],MAX_123_LEVEL);  \
        } while (0)

    /* step #1: set all coeffs to 0: done by VOC*/
    //memset(block, 0, 16 * sizeof (block[0]));   /* !!! */

    /* decode 4x4 descriptor & first 2x2 coefficients: */
    d=RM_HufDecodeAsm(p_4x4_dsc->next,p_4x4_dsc->base,p_4x4_dsc->offset, bs);
    d= p_4x4_dsc->symbol[d];
    d2 = d >> 3;
    if (d2) DECODE_2x2_COEFFS(d2, block[0], block[1], block[4], block[5]);

    /* decode the remaining 2x2 blocks: */
    if (d & 4)
    {
        d2=RM_HufDecodeAsm(p_2x2_dsc[0].next,p_2x2_dsc[0].base,p_2x2_dsc[0].offset, bs);
        d2= p_2x2_dsc[0].symbol[d2];
        DECODE_2x2_COEFFS(d2, block[2], block[3], block[6], block[7]);
    }
    if (d & 2)
    {
        d2=RM_HufDecodeAsm(p_2x2_dsc[0].next,p_2x2_dsc[0].base,p_2x2_dsc[0].offset, bs);
        d2= p_2x2_dsc[0].symbol[d2];
        DECODE_2x2_COEFFS(d2, block[8], block[12],block[9], block[13]);
    }
    if (d & 1)
    {
        d2=RM_HufDecodeAsm(p_2x2_dsc[1].next,p_2x2_dsc[1].base,p_2x2_dsc[1].offset, bs);
        d2= p_2x2_dsc[1].symbol[d2];
        DECODE_2x2_COEFFS(d2, block[10],block[11],block[14],block[15]);
    }

    /* update bit-stream vars & exit: */
    BS_CLOSE(p_pbs, p_bitOffset);
}


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

#if 0//hx chg
unsigned int decode_cbp (int intra, int qp,
                         unsigned char **p_pbs, unsigned int *p_bitOffset)
{
    /* load code tables: */
    CBP_DECODE_TABLE *ct = intra?
                           &(intra_decode_tables [intra_qp_to_idx [qp]].cbp [intra-1]):
                           &(inter_decode_tables [inter_qp_to_idx [qp]].cbp);
    register unsigned int d, c, cx, cbp = 0;

    /* init bit-stream vars: */
    BS_OPEN(p_pbs,p_bitOffset,1);


    /* decode the master CBP descriptor: */
    HUF_DECODE(d,MAX_CBP,ct->cbp.next,ct->cbp.base,ct->cbp.offset,ct->cbp.symbol);

    /* decode 8x8 descriptors: */
    cx = cbp_to_cx [d & 0x0F];
    if (d & 8)
    {
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        cbp |= y_to_cbp4x4 [c];
    }
    if (d & 4)
    {
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        cbp |= y_to_cbp4x4 [c] << 2;
    }
    if (d & 2)
    {
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        cbp |= y_to_cbp4x4 [c] << 8;
    }
    if (d & 1)
    {
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        cbp |= y_to_cbp4x4 [c] << 10;
    }

    /* decode chroma bits: */
    d >>= 4;
    c = ccbp_to_c0 [d];
    if (c == 2) cbp |= 0x11 << 16;
    else if (c == 1)
    {
        BS_GETBITS(c, 1);
        cbp |= c << 16;
        cbp |= (c^1) << 20;
    }
    c = ccbp_to_c1 [d];
    if (c == 2) cbp |= 0x11 << 17;
    else if (c == 1)
    {
        BS_GETBITS(c, 1);
        cbp |= c << 17;
        cbp |= (c^1) << 21;
    }
    c = ccbp_to_c2 [d];
    if (c == 2) cbp |= 0x11 << 18;
    else if (c == 1)
    {
        BS_GETBITS(c, 1);
        cbp |= c << 18;
        cbp |= (c^1) << 22;
    }
    c = ccbp_to_c3 [d];
    if (c == 2) cbp |= 0x11 << 19;
    else if (c == 1)
    {
        BS_GETBITS(c, 1);
        cbp |= c << 19;
        cbp |= (c^1) << 23;
    }

    /* update bit-stream vars: */
    BS_CLOSE(p_pbs, p_bitOffset, 0);

    /* return the descriptor: */
    return cbp;
}
#endif
unsigned int decode_cbp (int intra, int qp,
                         unsigned char **p_pbs, unsigned int *p_bitOffset)
{
    /* load code tables: */
    CBP_DECODE_TABLE *ct = intra?
                           &(intra_decode_tables [intra_qp_to_idx [qp]].cbp [intra-1]):
                           &(inter_decode_tables [inter_qp_to_idx [qp]].cbp);
    /*register*/ unsigned int d, c, cx, cbp = 0;

    /* init bit-stream vars: */
    BS_OPEN(p_pbs,p_bitOffset,1);

    /* decode the master CBP descriptor: */
    d = RM_HufDecodeAsm(ct->cbp.next,ct->cbp.base,ct->cbp.offset, bs);
    d = ct->cbp.symbol[d];

    /* decode 8x8 descriptors: */
    cx = cbp_to_cx [d & 0x0F];
    if (d & 8)
    {
        c = RM_HufDecodeAsm(ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset, bs);
        c = ct->_8x8_dsc[cx].symbol[c];
        cbp |= y_to_cbp4x4 [c];
    }
    if (d & 4)
    {
        c = RM_HufDecodeAsm(ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset, bs);
        c = ct->_8x8_dsc[cx].symbol[c];
        cbp |= y_to_cbp4x4 [c] << 2;
    }
    if (d & 2)
    {
        c = RM_HufDecodeAsm(ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset, bs);
        c = ct->_8x8_dsc[cx].symbol[c];
        cbp |= y_to_cbp4x4 [c] << 8;
    }
    if (d & 1)
    {
        c = RM_HufDecodeAsm(ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset, bs);
        c = ct->_8x8_dsc[cx].symbol[c];
        cbp |= y_to_cbp4x4 [c] << 10;
    }

    /* decode chroma bits: */
    d >>= 4;
    c = ccbp_to_c0 [d];
    if (c == 2) cbp |= 0x11 << 16;
    else if (c == 1)
    {
        c = BS_GETBITS(1);
        cbp |= c << 16;
        cbp |= (c^1) << 20;
    }
    c = ccbp_to_c1 [d];
    if (c == 2) cbp |= 0x11 << 17;
    else if (c == 1)
    {
        c = BS_GETBITS(1);
        cbp |= c << 17;
        cbp |= (c^1) << 21;
    }
    c = ccbp_to_c2 [d];
    if (c == 2) cbp |= 0x11 << 18;
    else if (c == 1)
    {
        c = BS_GETBITS(1);
        cbp |= c << 18;
        cbp |= (c^1) << 22;
    }
    c = ccbp_to_c3 [d];
    if (c == 2) cbp |= 0x11 << 19;
    else if (c == 1)
    {
        c = BS_GETBITS(1);
        cbp |= c << 19;
        cbp |= (c^1) << 23;
    }

    /* update bit-stream vars: */
    BS_CLOSE(p_pbs, p_bitOffset);

    /* return the descriptor: */
    return cbp;
}



/*********************end*****************************/



/*****************move from bsx.c*************************************/


/* lookup table used for chroma blocks when modified quantization is in effect */
/* Annex T tables used in encoder too  */
extern const I32 gNewTAB_DQUANT_MQ_RV89[32][2] ;


/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetDQUANT() */
/* --------------------------------------------------------------------------- */
void CB_GetDQUANT
(
    struct CRealVideoBs *t,
    U8 * quant,
    const U8 quant_prev,
    const RV_Boolean mq
)
{
    I32 dquant;
    I32 qp=0;

    if (mq)
    {
        if (CB_Get1Bit(t))
        {
            dquant = gNewTAB_DQUANT_MQ_RV89[quant_prev][CB_Get1Bit(t)];
            qp = quant_prev + dquant;
        }
        else
        {
            qp = CB_GetBits(t,5);
        }
    }
    else
    {
        /*      dquant  = gNewTAB_DQUANT[GetBits(2)]; */
        /*      qp = quant_prev + dquant */
    }

    if (qp < 0)
    {
        qp = 0;
    }
    else if (qp > 31)
    {
        qp = 31;
    }

    *quant = (U8)qp;


} /* CRealVideoBs::GetDQUANT() */


/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetPicSize() */
/*  Gets the picture size from the bitstream. */
/* --------------------------------------------------------------------------- */

void CB_GetPicSize(struct CRealVideoBs *t, U32 *width, U32 *height)
{
    U32 code;
    U32 w, h;

    const U32 code_width[8] = {160, 176, 240, 320, 352, 640, 704, 0};
    const U32 code_height1[8] = {120, 132, 144, 240, 288, 480, 0, 0};
    const U32 code_height2[4] = {180, 360, 576, 0};

    /* If we're decoding a P or B frame, get 1 bit that signals */
    /* difference in size from previous reference frame. */
    if (t->m_rv.ptype == RV_INTERPIC || t->m_rv.ptype == RV_TRUEBPIC)
    {
        /* no diff in size */
        if (CB_Get1Bit(t))
        {
            *width = t->m_rv.pwidth_prev;
            *height = t->m_rv.pheight_prev;
            return;
        }
    }


    /* width */
    code = CB_GetBits(t,3);
    w = code_width[code];
    if (w == 0)
    {
        do
        {
            code = CB_GetBits(t,8);
            w += (code << 2);
        }
        while (code == 255);
    }

    /* height */
    code = CB_GetBits(t,3);
    h = code_height1[code];
    if (h == 0)
    {
        code <<= 1;
        code |= CB_Get1Bit(t);
        code &= 3;
        h = code_height2[code];
        if (h == 0)
        {
            do
            {
                code = CB_GetBits(t,8);
                h += (code << 2);
            }
            while (code == 255);
        }
    }

    *width = w;
    *height = h;
}


/* --------------------------------------------------------------------------- */
/*  CB_GetPictureHeader() */
/* --------------------------------------------------------------------------- */

RV_Status CB_GetPictureHeader(struct CRealVideoBs *t, struct PictureHeader* hdr)
{
    RV_Status Status = RV_S_OK;
    I32 gfid, mba, squant;
    I32 tmp;


    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        /* note: header should be initialized earlier by SetFrameProperties() */

        /* find the first valid slice */
        if (!CB_IsSegmentValid(t) && CB_LossRecovery(t,TRUE) != BSLR_STATUS_OK)
            return RV_S_ERROR;

        /* get the first slice header */
        /* set m_rv.mba to -1 to indicate this is a call to GetSliceHeader from  */
        /* GetPicHeader */
        t->m_rv.mba = -1;
        if (CB_GetSliceHeader(t, FALSE, &mba, &gfid, &squant,
                              hdr))
        {
            switch(t->m_rv.ptype)
            {
                case RV_FORCED_INTRAPIC:
                    hdr->PicCodType = INTRAPIC;
                    t->m_isForcedKey    = TRUE;
                    //rm_printf("Force I frame\n");
                    break;
                case RV_INTRAPIC:
                    hdr->PicCodType = INTRAPIC;
                    t->m_isForcedKey    = FALSE;
                    //rm_printf("I frame\n");
                    break;
                case RV_INTERPIC:
                    hdr->PicCodType = INTERPIC;
                    //rm_printf("P frame\n");
                    break;
                case RV_TRUEBPIC:
                    hdr->PicCodType = TRUEBPIC;
                    //rm_printf("B frame\n");
                    break;
                default:
                    RVAssert(0);
                    break;
            }
            hdr->Deblocking_Filter_Passthrough  = t->m_rv.deblocking_filter_passthrough;
            hdr->PQUANT     = t->m_rv.pquant;
            hdr->TR         = t->m_rv.tr;
            hdr->TRB        = t->m_rv.trb;
            hdr->DBQUANT    = t->m_rv.dbq;
#ifdef INTERLACED_CODE
            hdr->x10        = t->m_rv.x10;
            hdr->Interlaced = t->m_rv.interlaced;
            hdr->InterlacedTopFieldFirst = t->m_rv.interlacedTopFieldFirst;
            hdr->DeblockingQpAdjust      = t->m_rv.DeblockingQpAdjust;
            hdr->RepeatField             = t->m_rv.RepeatField;
#endif

            /* initialize m_nGOBS, m_nMBs, and m_mbaSize */
            CB_SetDimensions(t,hdr);
        }
        else
            Status = RV_S_ERROR;
    }
    else
    {
        U32 length, info;
        const U32 PSC_LEN = 31;

        (void) memset(hdr, 0, sizeof(*hdr));

        /* SUPER_VLC needs longer start code to avoid emulation */
        tmp = CB_GetBits(t,16);
        if (tmp != 21845)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }
        tmp = CB_GetBits(t,16);
        if (tmp != 21845)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }

        length = CB_GetVLCBits(t,&info);

        if (length < PSC_LEN)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }

        if (info & 1)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR GetPictureHeader -- "
                     "Found sequence end code"));
            return RV_S_OUT_OF_RANGE;
            /* This is probably not the right error code to use, */
            /* but it will have to do for now. */
        }


        if (!((info>>1) & 1))
        {
            /* QCIF; */
            hdr->dimensions.width = 176;
            hdr->dimensions.height = 144;
        }
        else
        {
            hdr->dimensions.width = 0;
            hdr->dimensions.height = 0;
            /* Read dimensions from the bitstream, below */
        }

        hdr->PQUANT = (U8)((info>>2) & 31);
        hdr->TR = (info>>(2+5)) & 255;

        /* Picture type */
        length = CB_GetVLCBits(t,&info);

        if (length == 1)
        {
            hdr->PicCodType = INTERPIC;
            rm_printf("P frame\n");
        }
        else if (length == 3 && info == 1)
        {
            hdr->PicCodType = INTRAPIC;
            rm_printf("I frame\n");
        }
        else if (length == 5 && info == 0)
        {
            hdr->PicCodType = TRUEBPIC;
            rm_printf("B frame\n");
        }
        else
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Unsupported picture coding type"));
            return RV_S_UNSUPPORTED;
        }

        /* Non-QCIF frame size */
        if (hdr->dimensions.width == 0)
        {
            CB_GetCPFMT(t, hdr);
        }

        CB_SetDimensions(t,hdr);


#if defined(INTERLACED_CODE) && defined(RV10i_RAW_BITSTREAM)

        /* x10 Header */
        if(CB_Get1Bit(t))
        {
            hdr->x10 = TRUE;
            /* Interlaced */
            hdr->Interlaced = (CB_Get1Bit(t))?(TRUE):(FALSE);
            hdr->InterlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            /* Repeat Field */
            hdr->RepeatField = (CB_Get1Bit(t))?(TRUE):(FALSE);
            /* Bitstream Version */
            CB_Get1Bit(t);
            /* Deblocking filter Adjust */
            if(CB_Get1Bit(t))
            {
                /* qp */
                hdr->DeblockingQpAdjust = CB_GetBits(t,2);
                hdr->DeblockingQpAdjust = hdr->DeblockingQpAdjust + 2;
            }
            else
            {
                hdr->DeblockingQpAdjust = 0;
            }
        }
        else
        {
            hdr->x10 = FALSE;
            /* Interlaced */
            hdr->Interlaced = FALSE;
            hdr->InterlacedTopFieldFirst = TRUE;
            /* Repeat Field */
            hdr->RepeatField = FALSE;
            hdr->DeblockingQpAdjust = 0;
        }
#endif

        /* OSV Quant */
        hdr->OSVQUANT = (U8) CB_GetBits(t,2);
    }
    return Status;
}



U32
CB_GetSuperCBP(struct CRealVideoBs *t, U32 mbType, U32 QP)
{
    /* TBD, the ::decode_cbp prototype currently uses "int" instead */
    /* of U32.  Thus we jump through some hoops to guarantee appropriate */
    /* type conversions. */

    int cbp;
    unsigned int bitOffset = t->m_bitOffset;

    cbp = decode_cbp((int)(mbType), (int)(QP), &t->m_pbs, &bitOffset);

    t->m_bitOffset = (U32)(bitOffset);

    return (U32)(cbp);
}

#if 0//hx chg
void CB_DecodeSuperSubBlockCoefficients
(
    struct CRealVideoBs *t,
    I16    *pDst,
    U32     mbType,
    RV_Boolean    isChroma,
    U32     QP
)
{
    unsigned int bitOffset = t->m_bitOffset;

    decode_4x4_block((short *)pDst, (int)mbType, (int)isChroma, (int)(QP),
                     &t->m_pbs, &bitOffset);

    t->m_bitOffset = (U32)(bitOffset);
}
#endif


extern const U32 FIELDLEN_CSFMT_PARC;
extern const U32 FIELDLEN_CSFMT_FWI;
extern const U32 FIELDLEN_CSFMT_FHI;
extern const U32 FIELDVAL_SSC;
extern const U32 FIELDLEN_SQUANT;
extern const U32 FIELDLEN_TR_RV;
extern const U32 FIELDLEN_MPPTYPE_ROUNDING;
extern const U32 FIELDLEN_SQUANT;
extern const U32 FIELDLEN_GFID;


/* --------------------------------------------------------------------------- */
/*  CB_GetCPFMT() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetCPFMT(struct CRealVideoBs *t, struct PictureHeader *hdr)
{
    hdr->pixel_aspect_ratio = CB_GetBits(t,FIELDLEN_CSFMT_PARC);

    /* Frame width indication */
    /* The number of pixels per line is given by (FWI+1)*4 */
    hdr->dimensions.width = (CB_GetBits(t,FIELDLEN_CSFMT_FWI) + 1) << 2;

    /* Bit 14 must be "1" to prevent start code emulation */
    if (CB_Get1Bit(t) != 1)
    {
        RVDebug((RV_TL_ALWAYS, "ERROR GetCSFMT - CSFMT bit 14 != 1"));
        return(FALSE);
    }

    /* Frame height indication */
    /* The number of lines is given by FHI*4 */
    hdr->dimensions.height = CB_GetBits(t,FIELDLEN_CSFMT_FHI) << 2;

    return(TRUE);

} /* CB_GetCPFMT() */


/* --------------------------------------------------------------------------- */
/*  CB_GetSSC() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetSSC(struct CRealVideoBs *t)
{
    U32 offset, i, deltaofs;
    const U32 MAX_SSC_LOOKAHEAD_NUMBER = 7;


    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        offset = CB_GetBsOffset(t);
#ifdef BITSTREAM_ERROR_RESILIENT
        if (offset < SliceBitOffset(0) ||
                offset >= (CB_GetMaxBsSize(t) << 3))
            return FALSE;
#else
        RVAssert(offset >= SliceBitOffset(0) &&
                 offset < (CB_GetMaxBsSize(t) << 3));
#endif

        /* locate current slice, i - 1 */
        for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++) {};

        /* current slice must be valid at this point */
        RVAssert(SliceFlag(i - 1));

        /* check if just beginning to decode the current slice */
        if (offset == SliceBitOffset(i - 1))
            return TRUE;

        /* there is no more slice header in the bitstream */
        if (i == t->m_nSlices)
            return FALSE;

        /* skip stuffing if any */
        deltaofs = SliceBitOffset(i) - offset;
        if (deltaofs < 8 && CB_SearchBits(t,deltaofs, 0, 0))
            return TRUE;
    }
    else
    {

        if (CB_SearchBits_IgnoreOnly0( t,FIELDLEN_SSC, FIELDVAL_SSC,
                                       MAX_SSC_LOOKAHEAD_NUMBER))
            return TRUE;

        RVDebug((RV_TL_SUBPICT, "GetSSC - SSC not found"));
    }

    return FALSE;

} /* CB_GetSSC() */



/* --------------------------------------------------------------------------- */
/*  CB_GetSliceHeader() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetSliceHeader(
    struct CRealVideoBs *t,
    RV_Boolean  bFirst,
    I32*        iSliceMBA,
    I32*        iSliceGFID,
    I32*        iSQUANT,
    struct PictureHeader* hdr)
{
    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        /* skip if this is the first slice called from IAPass1ProcessFrame */
        /* otherwise read the slice header */
        if (!bFirst)
        {
            U32 fieldlen;

            /* this was called from GetPicHeader */
            if (t->m_rv.mba == -1)
                bFirst = TRUE;

#ifdef USE_INTEL_VLC
            /* Bitstream version */
            U32 versionEncoding = CB_GetBits(t,3 /*FIELDLEN_RV_BITSTREAM_VERSION*/);
            if (versionEncoding > 0)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unrecognized RV bitstream version %ld",
                         versionEncoding));
                return FALSE;
            }

            /* PTYPE */
            if (bFirst)
                t->m_rv.ptype = (EnumRVPicCodType) CB_GetBits(t,2);
            else if ((EnumRVPicCodType) CB_GetBits(t,2) != t->m_rv.ptype)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unmatched PTYPE"));
                return FALSE;
            }

            /* ECC */
            CB_Get1Bit(t);

#ifdef INTERLACED_CODE
#ifdef COMPATIBILITY_BITSTREAM
            t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
            if(t->m_rv.interlaced) t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            t->m_rv.x10 = FALSE;
            /* Repeat Field */
            t->m_rv.RepeatField = FALSE;
            t->m_rv.DeblockingQpAdjust = 0;
#else
            /* x10 Header */
            if((CB_Get1Bit(t) ? TRUE : FALSE))
            {
                t->m_rv.x10 = TRUE;
                t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
                t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
                /* repeat field */
                t->m_rv.RepeatField = CB_Get1Bit(t);
                /* x10 bitstream version */
                CB_Get1Bit(t);
                /* Deblocking Qp Adjust */
                if(CB_Get1Bit(t))
                {
                    t->m_rv.DeblockingQpAdjust = CB_GetBits(t,2);
                    t->m_rv.DeblockingQpAdjust = t->m_rv.DeblockingQpAdjust +2;
                }
                else
                {
                    t->m_rv.DeblockingQpAdjust = 0;
                }
            }
            else
            {
                t->m_rv.x10 = FALSE;
                /* Interlaced */
                t->m_rv.interlaced = FALSE;
                t->m_rv.interlacedTopFieldFirst = TRUE;
                /* Repeat Field */
                t->m_rv.RepeatField = FALSE;
                t->m_rv.DeblockingQpAdjust = 0;
            }
#endif
#endif
            /* PQUANT/SQUANT */
            t->m_rv.pquant = (U8) CB_GetBits(t,FIELDLEN_SQUANT);

            /* 0-31 are legitimate QP values for RV89Combo */
            if (t->m_rv.pquant > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "SQUANT %d > 31", t->m_rv.pquant));
                return FALSE;
            }

            /* OSV Quant */
            hdr->OSVQUANT = (U8) CB_GetBits(t,2);

            /* In-loop deblocking */
            t->m_rv.deblocking_filter_passthrough = CB_GetBits(t,1)?TRUE:FALSE;

            fieldlen = FIELDLEN_TR_RV;

            if (bFirst)
                t->m_rv.tr = CB_GetBits(t,fieldlen);
            else if (CB_GetBits(t,fieldlen) != t->m_rv.tr)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unmatched TR"));
                return FALSE;
            }

            /* PCTSZ (RealVideo RPR picture size) */
            {
                U32 width, height;

                if (bFirst)
                {
                    switch (t->m_rv.ptype)
                    {
                        case RV_INTRAPIC:
                        case RV_FORCED_INTRAPIC:
                            t->m_rv.pwidth_prev  = 0;
                            t->m_rv.pheight_prev = 0;
                            break;
                        case RV_INTERPIC:
                            t->m_rv.pwidth_prev  = t->m_rv.pwidth;
                            t->m_rv.pheight_prev = t->m_rv.pheight;
                            break;
                    }
                }

                CB_GetPicSize(t, &width, &height);

                if (bFirst)
                {
                    t->m_rv.pwidth = width;
                    t->m_rv.pheight = height;
                    hdr->pixel_aspect_ratio = PARC_SQUARE;
                    /* TBD, why is parc set here? */
                    hdr->dimensions.width = width;
                    hdr->dimensions.height = height;
                    SetDimensions(*hdr);
                }
                else if (t->m_rv.pwidth != width || t->m_rv.pheight != height)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                             "Unmatched PCTSZ"));
                    return FALSE;
                }
            }

            /* MBA */
            t->m_rv.mba = CB_GetBits(t,t->m_mbaSize);

            /* RTYPE */
            /* RTYPE not used anymore.  This code is hear to provide */
            /* compatibility with version sent to Intel. */
            CB_GetBits(this,FIELDLEN_MPPTYPE_ROUNDING);

#else /* USE_INTEL_VLC */

            /* RealNetworks Slice Header */

            /* ECC */
            if (CB_Get1Bit(t) != 0)
            {
                RVDebug((RV_TL_SUBPICT, "WARNING GetSliceHeader_RV9 - "
                         "ECC packet"));
                return FALSE;
            }

            /* PTYPE */
            if (bFirst)
                t->m_rv.ptype = (EnumRVPicCodType) CB_GetBits(t,2);
            else if ((EnumRVPicCodType) CB_GetBits(t,2) != t->m_rv.ptype)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unmatched PTYPE"));
                return FALSE;
            }

            /* PQUANT/SQUANT */
            t->m_rv.pquant = (U8) CB_GetBits(t,FIELDLEN_SQUANT);
            /* 0-31 are legitimate QP values for RV89Combo */
            if (t->m_rv.pquant > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "SQUANT %d > 31", t->m_rv.pquant));
                return FALSE;
            }

            /* Bitstream version */
            if (CB_Get1Bit(t) != 0)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unrecognized RV bitstream version"));
                return FALSE;
            }

#ifdef INTERLACED_CODE
#ifdef COMPATIBILITY_BITSTREAM
            t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
            if(t->m_rv.interlaced) t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            t->m_rv.x10 = FALSE;
            /* Repeat Field */
            t->m_rv.RepeatField = FALSE;
            t->m_rv.DeblockingQpAdjust = 0;
#else
            /* x10 Header */
            if((CB_Get1Bit(t) ? TRUE : FALSE))
            {
                t->m_rv.x10 = TRUE;
                t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
                t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
                /* repeat field */
                t->m_rv.RepeatField = (CB_Get1Bit(t)? TRUE : FALSE);
                /* x10 bitstream version */
                CB_Get1Bit(t);
                /* Deblocking Qp Adjust */
                if(CB_Get1Bit(t))
                {
                    t->m_rv.DeblockingQpAdjust = CB_GetBits(t,2);
                    t->m_rv.DeblockingQpAdjust = t->m_rv.DeblockingQpAdjust +2;
                }
                else
                {
                    t->m_rv.DeblockingQpAdjust = 0;
                }
            }
            else
            {
                t->m_rv.x10 = FALSE;
                /* Interlaced */
                t->m_rv.interlaced = FALSE;
                t->m_rv.interlacedTopFieldFirst = TRUE;
                /* Repeat Field */
                t->m_rv.RepeatField = FALSE;
                t->m_rv.DeblockingQpAdjust = 0;
            }
#endif
#else
            if (CB_Get1Bit(t))
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Interlaced bit set."));
                return FALSE;
            }
#endif

            /* OSV Quant */
            hdr->OSVQUANT = (U8) CB_GetBits(t,2);

            /* In-loop deblocking */
            t->m_rv.deblocking_filter_passthrough = CB_GetBits(t,1)?TRUE:FALSE;

            fieldlen = FIELDLEN_TR_RV;

            if (bFirst)
                t->m_rv.tr = CB_GetBits(t,fieldlen);
            else if (CB_GetBits(t,fieldlen) != t->m_rv.tr)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV8 - "
                         "Unmatched TR"));
                return FALSE;
            }

            /* PCTSZ (RealVideo RPR picture size) */
            {
                U32 width, height;

                if (bFirst)
                {
                    switch (t->m_rv.ptype)
                    {
                        case RV_INTRAPIC:
                        case RV_FORCED_INTRAPIC:
                            t->m_rv.pwidth_prev  = 0;
                            t->m_rv.pheight_prev = 0;
                            break;
                        case RV_INTERPIC:
                            t->m_rv.pwidth_prev  = t->m_rv.pwidth;
                            t->m_rv.pheight_prev = t->m_rv.pheight;
                            break;
                        case RV_TRUEBPIC: //sheen
                            break;
                    }
                }

                CB_GetPicSize(t, &width, &height);

                if (bFirst)
                {
                    t->m_rv.pwidth = width;
                    t->m_rv.pheight = height;
                    /*                  hdr->pixel_aspect_ratio = PARC_SQUARE; */
                    /* TBD, why is parc set here? */
                    hdr->dimensions.width = width;
                    hdr->dimensions.height = height;
                    CB_SetDimensions(t,hdr);
                }
                else if (t->m_rv.pwidth != width || t->m_rv.pheight != height)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                             "Unmatched PCTSZ"));
                    return FALSE;
                }
            }

            /* MBA */
            t->m_rv.mba = CB_GetBits(t,t->m_mbaSize);

#endif /* USE_INTEL_VLC */
        }

        *iSliceMBA  = t->m_rv.mba;
        *iSQUANT    = t->m_rv.pquant;
        *iSliceGFID = 0;
    }
    else
    {
        /* The SSC either does not exist because this is the first slice */
        /* after the picture start code, or the SSC has already been read */
        /* in IAPass1. */

        if (!bFirst)
        {

            /* Must be 1 to prevent start code emulation (SEPB1) */
            if (CB_Get1Bit(t) != 1)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
                return FALSE;
            }

            /* No SSBI since CPM can not be "1". */

#ifdef INTERLACED_CODE
#ifdef COMPATIBILITY_BITSTREAM
            t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
            if(t->m_rv.interlaced) t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            t->m_rv.x10 = FALSE;
            /* Repeat Field */
            t->m_rv.RepeatField = FALSE;
            t->m_rv.DeblockingQpAdjust = 0;
#else


            if((CB_Get1Bit(t) ? TRUE : FALSE))
            {
                hdr->x10        = TRUE;
                hdr->Interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
                hdr->InterlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
                /* repeat field */
                hdr->RepeatField = (CB_Get1Bit(t)? TRUE : FALSE);
                /* x10 bitstream version */
                CB_Get1Bit(t);
                /* Deblocking Qp Adjust */
                if(CB_Get1Bit(t))
                {
                    hdr->DeblockingQpAdjust = CB_GetBits(t,2);
                    hdr->DeblockingQpAdjust = hdr->DeblockingQpAdjust +2;
                }
                else
                {
                    hdr->DeblockingQpAdjust = 0;
                }
            }
            else
            {
                hdr->x10        = FALSE;
                /* Interlaced */
                hdr->Interlaced = FALSE;
                hdr->InterlacedTopFieldFirst = TRUE;
                /* Repeat Field */
                hdr->RepeatField = FALSE;
                hdr->DeblockingQpAdjust = 0;
            }
#endif
#endif

            /* Get macro-block address */
            *iSliceMBA = CB_GetBits(t,t->m_mbaSize);

            if (t->m_mbaSize > 11)
            {
                /* Must be 1 to prevent start code emulation (SEPB2) */
                if (CB_Get1Bit(t) != 1)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
                    return FALSE;
                }
            }

            /* Get SQUANT */
            *iSQUANT = CB_GetBits(t,FIELDLEN_SQUANT);

            /* 0-31 are legitimate QP values for RV89Combo */
            if (*iSQUANT > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "SQUANT %d > 31", *iSQUANT));
                return FALSE;
            }

            /* OSV Quant */
            hdr->OSVQUANT = (U8) CB_GetBits(t,2);

            /* No SWI field. */

            /* Must be 1 to prevent start code emulation (SEPB3) */
            if (CB_Get1Bit(t) != 1)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
                return FALSE;
            }

            /* Get GOB frame ID. */
            *iSliceGFID = CB_GetBits(t,FIELDLEN_GFID);

            RVDebug((RV_TL_SUBPICT,
                     "CB_GetSliceHeader_RV8 - MBA=%ld SQUANT=%ld GFID=%ld",
                     *iSliceMBA, *iSQUANT, *iSliceGFID));

        }
        else
        {
            *iSliceMBA = 0;
            *iSQUANT = -1;
        }

    }

    return TRUE;

} /* CB_GetSliceHeader() */


/* --------------------------------------------------------------------------- */
/*  CB_SetDimensions() */
/*      use in the decoder to initialize size, m_nGOBs, m_nMBs, and m_mbaSize */
/* --------------------------------------------------------------------------- */
extern const U32 MBA_NumMBs[];
extern const U32 MBA_FieldWidth[] ;
extern const U32 MBA_Widths;

void CB_SetDimensions(struct CRealVideoBs *t, struct PictureHeader *hdr)
{
    U32 i,j;
    /* Set m_nMBs and m_nGOBs based on the image dimensions. */
    /* TBD, there's probably a formula that can be used here instead */
    /* of this nested if-then-else sequence. */

    if (hdr->dimensions.width == 128 && hdr->dimensions.height == 96)
    {
        t->m_nMBs   = 8;
        t->m_nGOBs  = 6;
    }
    else if (hdr->dimensions.width == 176 && hdr->dimensions.height == 144)
    {
        t->m_nMBs   = 11;
        t->m_nGOBs  = 9;
    }
    else if (hdr->dimensions.width == 352 && hdr->dimensions.height == 288)
    {
        t->m_nMBs   = 22;
        t->m_nGOBs  = 18;
    }
    else if (hdr->dimensions.width == 704 && hdr->dimensions.height == 576)
    {
        t->m_nMBs   = 44;
        t->m_nGOBs  = 36;
    }
    else if (hdr->dimensions.width == 1408 && hdr->dimensions.height == 1152)
    {
        t->m_nMBs   = 88;
        t->m_nGOBs  = 72;
    }
    else
    {
        t->m_nMBs  = (hdr->dimensions.width + 15) >> 4;
        t->m_nGOBs = (hdr->dimensions.height + 15) >> 4;
    }

    /* initialize m_mbaSize */

    j = t->m_nGOBs * t->m_nMBs - 1;
    for (i = 0; i < (MBA_Widths - 1) && MBA_NumMBs[i] <  j; i++)
        ;

    t->m_mbaSize = MBA_FieldWidth[i];

} /* CB_SetDimensions() */

/* --------------------------------------------------------------------------- */
/*  CB_LossRecovery() */
/*  used by the recv side to recover from a packet loss */
/*  Returns: */
/*      BSLR_STATUS_OK      Packet loss recovered from */
/*      BSLR_STATUS_ERROR   Packet loss not recognized */
/* --------------------------------------------------------------------------- */

BSLR_Status CB_LossRecovery(struct CRealVideoBs *t, RV_Boolean bUpdateState)
{
    U32 offset1, offset2, i, maxBitLimit;
    BSLR_Status status = BSLR_STATUS_OK;

    if (t->m_nSlices)
    {
        maxBitLimit = CB_GetMaxBsSize(t) << 3;
        offset1 = CB_GetBsOffset(t);
        offset2 = maxBitLimit;

        for (i = 1; i < t->m_nSlices; i ++)
        {
            if (SliceFlag(i) && offset1 <= SliceBitOffset(i))
            {
                offset2 = SliceBitOffset(i);
                break;
            }
        }

        if (offset2 == maxBitLimit)
            status = BSLR_STATUS_EOS;

        if (bUpdateState)
            if (offset2 > offset1)
                CB_UpdateState(t,offset2 - offset1);
    }
    else
        status = BSLR_STATUS_ERROR;

    return status;

} /* CB_LossRecovery() */



/* --------------------------------------------------------------------------- */
/*  [DEC] CB_IsSegmentValid() */
/*  This should be called before decoding each MB to determine if the current */
/*  bitstream pointer is pointing inside a valid segment/packet.  If the  */
/*  pointer has reached the end of a segment, forward the pointer to the next  */
/*  segment to be decoded and check if it is valid. */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_IsSegmentValid(struct CRealVideoBs *t)
{
    U32 offset,maxBitLimit,delta_offset,i;
    /* in the case where segment information is not available, assume each */
    /* segment given to the is properly formatted (with header and decodable */
    /* data, or 4 bytes of 0's which will be caught by the decoder later) */
    if (!t->m_nSlices)
        return TRUE;

    offset = CB_GetBsOffset(t);
    maxBitLimit = CB_GetMaxBsSize(t) << 3;
    if (offset < SliceBitOffset(0) || offset >= maxBitLimit)
        return FALSE;

    /* locate current slice, i - 1 */
    for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++) {};

    //delta_offset; //sheen
    if (i == t->m_nSlices)
    {
        /* this is the last slice */
        /* skip stuffing if any */
        delta_offset = maxBitLimit - offset;
        if (delta_offset < 8 && CB_SearchBits(t,delta_offset, 0, 0))
            return FALSE;
    }
    else
    {
        /* this is not the last slice */
        /* skip stuffing if any */
        delta_offset = SliceBitOffset(i) - offset;
        if (delta_offset < 8 && CB_SearchBits(t,delta_offset, 0, 0))
            return((RV_Boolean) SliceFlag(i));
    }
    return((RV_Boolean) SliceFlag(i - 1));

} /* CB_IsSegmentValid() */



/******************end********************/


/*****************move from mc.c************************/


extern VPP_RMVB_DATA_T *vppDataIn;

/*//////////////////////////////////////////////////////////////// */
/* Based on luma motion vectors, perform frame reconstruction for */
/* one 16x16 luma macroblock. */
RV_Status
Decoder_reconstructLumaMacroblock
(
    struct Decoder *t,
    U32                     mbType,
    /* We must not be called for intra and skipped macroblocks */
    struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlane,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    RV_Status   status = RV_S_OK;
    U16         *pt0;
    U32         *pt1;

    const U8    *pRef;
    U32         subblock, pitch, subBlockPitch;
    I32         xint, yint, xh, yh;

    pitch = t->m_pCurrentFrame->m_pitch;
    subBlockPitch = t->m_pCurrentFrame->m_subBlockSize.width;

    pt0 = vppDataIn->motion_vectors;
    pt1 = (U32*)(vppDataIn->interplate_yblk_addr);

    switch (mbType)
    {
#ifdef SKIPPED_RV10i
        case MBTYPE_SKIPPED:
#endif
        case MBTYPE_INTER:
        case MBTYPE_INTER_16x16:
            *pt0++ = pMV->mvx;
            *pt0++ = pMV->mvy;

            MV_Sub2FullPels(pMV->mvx, &xint, &xh);
            MV_Sub2FullPels(pMV->mvy, &yint, &yh);

            pRef = pRefYPlane + mbXOffset + xint + ((mbYOffset + yint) * pitch);

            if(yh==0 || (xh==3 && yh==3))
                pRef = pRef -2;
            else
                pRef = pRef -2*pitch -2;

            *pt1 = (INT32)(pRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
            /* interpolate 16x16 block */
            //(*t->m_InterpFunc)[yh][xh](pRef, pt1, pitch, 16);
            break;

        case MBTYPE_FORWARD:
        case MBTYPE_BACKWARD:
            /* 1 vector for the MB */
            *pt0++ = pMV->mvx;
            *pt0++ = pMV->mvy;

            MV_Sub2FullPels_B(pMV->mvx, &xint, &xh);
            MV_Sub2FullPels_B(pMV->mvy, &yint, &yh);

            pRef = pRefYPlane + mbXOffset + xint + ((mbYOffset + yint) * pitch);

            if(yh==0 || (xh==3 && yh==3))
                pRef = pRef -2;
            else
                pRef = pRef -2*pitch -2;

            *pt1 = (INT32)(pRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
            //(*t->m_InterpFunc_B)[yh][xh](pRef, pt1, pitch, 16);
            break;

        case MBTYPE_INTER_4V:
        case MBTYPE_INTER_16x8V:
        case MBTYPE_INTER_8x16V:
            /* 4 8x8 blocks */
            for (subblock=0; subblock<4; subblock++)
            {
                *pt0++ = pMV->mvx;
                *pt0++ = pMV->mvy;

                MV_Sub2FullPels(pMV->mvx, &xint, &xh);
                MV_Sub2FullPels(pMV->mvy, &yint, &yh);

                pRef = pRefYPlane + mbXOffset + xoff[chroma_block_address[subblock]] + xint
                       + (mbYOffset + yoff[chroma_block_address[subblock]] + yint) * pitch;

                if(yh==0 || (xh==3 && yh==3))
                    pRef = pRef -2;
                else
                    pRef = pRef -2*pitch -2;

                *pt1++ = (INT32)(pRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
                /* interpolate 8x8 block */
                //(*t->m_InterpFunc)[yh][xh](pRef, pt1, pitch, 8);

                /* advance MV pointer and source ptr to next 8x8 block */
                if (1 == subblock)
                {
                    /* next block is lower left */
                    pMV += subBlockPitch*2 - 2;
                }
                else
                {
                    pMV += 2;
                }
                //pt1 += VPP_LUMABLK_SIZE;
            }   /* for subblock */
            break;

        default:
            /* not supposed to be here */
            RVAssert(0);
            break;

    }
    return status;
}



RV_Status Decoder_reconstructLumaMacroblockDirect
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    /* We must not be called for intra and skipped macroblocks */
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch;
    const I32 width = t->m_pCurrentFrame->m_lumaSize.width;
    const I32 height = t->m_pCurrentFrame->m_lumaSize.height;

    I32 mvxP, mvyP, mvxF, mvyF;
    I32 posx, posy;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    I32 lo, hi;     /* temp vars for TRUNCATE macro */
    U16 *pt0;
    U32 *pt1,*pt2;
    I32 i;

    const U8* pRefYFuture;
    const U8* pRefYPrevious;

    pt0   = vppDataIn->motion_vectors;
    for(i=0; i<4; i++)
    {
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
    }

    pt1   = (U32*)(vppDataIn->interplate_yblk_addr);
    pt2   = (U32*)(vppDataIn->interplate_yblk_b_addr);

    pRefYPrevious   = pRefYPlanePrevious + mbXOffset + mbYOffset * pitch;
    pRefYFuture     = pRefYPlaneFuture + mbXOffset + mbYOffset * pitch;


    if (IS_INTRA_MBTYPE(mbTypeRef) || (mbTypeRef == MBTYPE_SKIPPED))
    {
        *pt1 = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
        *pt2 = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
        /* interpolate copy the 16x16 previous block */
        //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);

        /* interpolate copy the 16x16 future block */
        //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
    }
    else if (mbTypeRef == MBTYPE_INTER || mbTypeRef == MBTYPE_INTER_16x16)
    {
        if (!pMV->mvx && !pMV->mvy)
        {
            *pt1 = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2 = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 16x16 previous block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);

            /* interpolate copy the 16x16 future block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
        }
        else
        {
            posx = mbXOffset;
            posy = mbYOffset;

            if (pMV->mvx)
            {
                mvxP = (iRatio0 * pMV->mvx + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - pMV->mvx;

                TRUNCATE_LO(mvxF, (width - 3 - posx) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (-14 - posx) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefYPrevious += xintp;
                pRefYFuture += xintf;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (pMV->mvy)
            {
                mvyP = (iRatio0 * pMV->mvy + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - pMV->mvy;

                TRUNCATE_LO(mvyF, (height - 6 - posy) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (-11 - posy) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefYPrevious += yintp * pitch;
                pRefYFuture += yintf * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt1 = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2 = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 16x16 previous block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 16);

            /* interpolate copy the 16x16 future block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 16);
        }
    }
    else
    {
        vppDataIn->inter16      = 0;
        /* process four 8x8 blocks */
        for (i = 0; i < 16; i += 4)
        {
            posx = mbXOffset + xoff[block_subblock_mapping[i]];
            posy = mbYOffset + yoff[block_subblock_mapping[i]];

            pRefYPrevious = pRefYPlanePrevious + posx + posy * pitch;
            pRefYFuture = pRefYPlaneFuture + posx + posy * pitch;

            if (pMV->mvx)
            {
                mvxP = (iRatio0 * pMV->mvx + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - pMV->mvx;
                TRUNCATE_LO(mvxF, (width + 5 - posx) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (-14 - posx) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefYPrevious += xintp;
                pRefYFuture += xintf;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (pMV->mvy)
            {
                mvyP = (iRatio0 * pMV->mvy + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - pMV->mvy;

                TRUNCATE_LO(mvyF, (height + 2 - posy) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (-11 - posy) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefYPrevious += yintp * pitch;
                pRefYFuture += yintf * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt1++ = (INT32)(pRefYPrevious-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2++ = (INT32)(pRefYFuture-2) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 8x8 previous block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYPrevious, pt0, pitch, 8);
            /* interpolate copy the 8x8 future block */
            //(*t->m_InterpFunc_B)[0][0]((U8 *)pRefYFuture, pt1, pitch, 8);

            if (i & 4)
            {
                pMV += t->m_pCurrentFrame->m_subBlockSize.width * 2 - 2;
            }
            else
            {
                pMV += 2;
            }

        } /* for subblock */
    }
    return RV_S_OK;

} /* reconstructLumaMacroblockDirect */



RV_Status Decoder_reconstructLumaMacroblockBidir
(
    struct Decoder *t,
    const U32               mbType,
    struct DecoderPackedMotionVector *pMVp,
    struct DecoderPackedMotionVector *pMVf,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{

    const I32 pitch = t->m_pCurrentFrame->m_pitch;
    const U8* in1;
    const U8* in2;
    I32 mvxP, mvyP, mvxF, mvyF;
    I32 posx, posy;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    U16 *pt0;
    U32 *pt1;
    I32 i;

    posx = mbXOffset;
    posy = mbYOffset;

    in1 = pRefYPlanePrevious + posx + posy * pitch;
    in2 = pRefYPlaneFuture + posx + posy * pitch;

    mvxF = pMVf->mvx;
    mvxP = pMVp->mvx;

    xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
    xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

    in1 += xintp;
    in2 += xintf;

    mvyF = pMVf->mvy;
    mvyP = pMVp->mvy;

    yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
    yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

    in1 += yintp * pitch;
    in2 += yintf * pitch;

    pt0   = vppDataIn->motion_vectors;
    for(i=0; i<4; i++)
    {
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
        *pt0++ = 0;
    }

    pt1   = (U32*)(vppDataIn->interplate_yblk_addr);
    *pt1++ = (INT32)(in1-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
    /* interpolate copy the 16x16 previous block */
    //(*t->m_InterpFunc_B)[0][0]((U8 *)in1, pt0, pitch, 16);

    pt1   = (U32*)(vppDataIn->interplate_yblk_b_addr);
    *pt1++ = (INT32)(in2-2) & VOC_CFG_PRED_DMA_EADDR_MASK;
    /* interpolate copy the 16x16 future block */
    //(*t->m_InterpFunc_B)[0][0]((U8 *)in2, pt0, pitch, 16);

    return RV_S_OK;

} /* reconstructLumaMacroblockBidir */


#ifdef __cplusplus
extern "C"
#endif


void Decoder_reconstructChromaMacroBlockDirect
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen
    const I32 crWidth = t->m_pCurrentFrame->m_lumaSize.width >> 1;
    const I32 crHeight = t->m_pCurrentFrame->m_lumaSize.height >> 1;
    U32 *pt0,*pt1,*pt2,*pt3;

    I32 xRefPos = (mbXOffset >> 1);
    I32 yRefPos = (mbYOffset >> 1);
    I32 offset = xRefPos + (pitch * yRefPos);

    I32 mvX, mvY;
    I32 mvxP, mvyP, mvxF, mvyF;
    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;
    I32 lo, hi;         /* temp vars for TRUNCATE macro */

    const U8* pRefFuture;
    const U8* pRefPrevious;
    const U8* pRefFuture1;
    const U8* pRefPrevious1;

    pRefPlaneFuture   += offset;
    pRefPlanePrevious += offset;
    pRefPlaneFuture1  += offset;
    pRefPlanePrevious1+= offset;

    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_ublk_b_addr);
    pt2   = (U32*)(vppDataIn->interplate_vblk_addr);
    pt3   = (U32*)(vppDataIn->interplate_vblk_b_addr);

    if ((mbTypeRef == MBTYPE_INTER) || (mbTypeRef == MBTYPE_INTER_16x16) ||
            IS_INTRA_MBTYPE(mbTypeRef) || (mbTypeRef == MBTYPE_SKIPPED))
    {
        if(IS_INTRA_MBTYPE(mbTypeRef) || mbTypeRef == MBTYPE_SKIPPED || (!pMV->mvx && !pMV->mvy))
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;
        }
        else
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;

            mvX = pMV->mvx / 2;
            mvY = pMV->mvy / 2;

            if (mvX)
            {
                mvxP = (iRatio0 * mvX + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - mvX;
                TRUNCATE_LO(mvxF, (crWidth - xRefPos - 1) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (- 7 - xRefPos) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefFuture += xintf;
                pRefPrevious += xintp;
                pRefFuture1 += xintf;
                pRefPrevious1+= xintp;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (mvY)
            {
                mvyP = (iRatio0 * mvY + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - mvY;
                TRUNCATE_LO(mvyF, (crHeight - yRefPos - 1) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (- 7 - yRefPos) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefFuture += yintf * pitch;
                pRefPrevious += yintp * pitch;
                pRefFuture1 += yintf * pitch;
                pRefPrevious1 += yintp * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }
        }
        pRefPrevious = (U8 *)((INT32)(pRefPrevious) & VOC_CFG_PRED_DMA_EADDR_MASK);
        pRefFuture    = (U8 *)((INT32)(pRefFuture) & VOC_CFG_PRED_DMA_EADDR_MASK);
        pRefPrevious1= (U8 *)((INT32)(pRefPrevious1) & VOC_CFG_PRED_DMA_EADDR_MASK);
        pRefFuture1   = (U8 *)((INT32)(pRefFuture1) & VOC_CFG_PRED_DMA_EADDR_MASK);

        *pt0++ = (INT32)pRefPrevious;
        *pt1++ = (INT32)pRefFuture;
        *pt2++ = (INT32)pRefPrevious1;
        *pt3++ = (INT32)pRefFuture1;

        //(*t->m_InterpFuncChroma)[0][0](pRefPrevious, pt0, pitch);
        //(*t->m_InterpFuncChroma)[0][0](pRefFuture, pt1, pitch);
        //(*t->m_InterpFuncChroma)[0][0](pRefPrevious1, pt2, pitch);
        //(*t->m_InterpFuncChroma)[0][0](pRefFuture1, pt3, pitch);

        //pt0 +=VPP_CHROMABLK_SIZE;
        //pt1 +=VPP_CHROMABLK_SIZE;
        //pt2 +=VPP_CHROMABLK_SIZE;
        //pt3 +=VPP_CHROMABLK_SIZE;

        *pt0++ = (INT32)(pRefPrevious + 4);
        *pt1++ = (INT32)(pRefFuture + 4);
        *pt2++ = (INT32)(pRefPrevious1 + 4);
        *pt3++ = (INT32)(pRefFuture1 + 4);

        /*
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious + 4, pt0, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture + 4, pt1, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious1 + 4, pt2, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture1 + 4, pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
        *pt1++ = (INT32)(pRefFuture + (pitch << 2));
        *pt2++ = (INT32)(pRefPrevious1 + (pitch << 2));
        *pt3++ = (INT32)(pRefFuture1 + (pitch << 2));

        /*
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2), pt0, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2), pt1, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious1 + (pitch << 2), pt2, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture1 + (pitch << 2), pt3, pitch);

        pt0 +=VPP_CHROMABLK_SIZE;
        pt1 +=VPP_CHROMABLK_SIZE;
        pt2 +=VPP_CHROMABLK_SIZE;
        pt3 +=VPP_CHROMABLK_SIZE;
        */

        *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
        *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);
        *pt2++ = (INT32)(pRefPrevious1 + (pitch << 2) + 4);
        *pt3++ = (INT32)(pRefFuture1 + (pitch << 2) + 4);

        /*
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious+ (pitch << 2) + 4, pt0, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture+ (pitch << 2) + 4, pt1, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefPrevious1+ (pitch << 2) + 4, pt2, pitch);
        (*t->m_InterpFuncChroma)[0][0](pRefFuture1+ (pitch << 2) + 4, pt3, pitch);
        */
    }
    else /* MBTYPE_INTER_4V */
    {
        const I32 widthInBlks = t->m_pCurrentFrame->m_subBlockSize.width;
        U32 subblock;

        for (subblock = 0; subblock < 4; subblock ++)
        {
            pRefFuture   = pRefPlaneFuture;
            pRefPrevious = pRefPlanePrevious;
            pRefFuture1  = pRefPlaneFuture1;
            pRefPrevious1= pRefPlanePrevious1;

            mvX = pMV->mvx / 2;
            mvY = pMV->mvy / 2;

            if (mvX)
            {
                mvxP = (iRatio0 * mvX + TR_RND) >> TR_SHIFT;
                mvxF = mvxP - mvX;
                TRUNCATE_LO(mvxF, (crWidth - xRefPos - 3) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvxF, (- 7 - xRefPos) * INTERP_FACTOR + 1, hi)

                xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
                xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

                pRefFuture += xintf;
                pRefPrevious += xintp;
                pRefFuture1 += xintf;
                pRefPrevious1 += xintp;
            }
            else
            {
                xhf = xhp = 0;
            }

            if (mvY)
            {
                mvyP = (iRatio0 * mvY + TR_RND) >> TR_SHIFT;
                mvyF = mvyP - mvY;
                TRUNCATE_LO(mvyF, (crHeight - yRefPos - 3) * INTERP_FACTOR - 1, lo)
                TRUNCATE_HI(mvyF, (- 7 - yRefPos) * INTERP_FACTOR + 1, hi)

                yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
                yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

                pRefFuture += yintf * pitch;
                pRefPrevious += yintp * pitch;
                pRefFuture1 += yintf * pitch;
                pRefPrevious1 += yintp * pitch;
            }
            else
            {
                yhf = yhp = 0;
            }

            *pt0++ = (INT32)(pRefPrevious) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt1++ = (INT32)(pRefFuture) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt2++ = (INT32)(pRefPrevious1) & VOC_CFG_PRED_DMA_EADDR_MASK;
            *pt3++ = (INT32)(pRefFuture1) & VOC_CFG_PRED_DMA_EADDR_MASK;

            /* interpolate copy the 4x4 block from previous, then future */
            //(*t->m_InterpFuncChroma)[0][0](pRefPrevious, pt0, pitch);
            //(*t->m_InterpFuncChroma)[0][0](pRefFuture, pt1, pitch);
            //(*t->m_InterpFuncChroma)[0][0](pRefPrevious1, pt2, pitch);
            //(*t->m_InterpFuncChroma)[0][0](pRefFuture1, pt3, pitch);

            /* Advance xRefPos and yRefPos for next iteration */
            /* Move pointers to next subblock. */
            if (subblock != 1)
            {
                xRefPos += 4;
                pRefPlaneFuture += 4;
                pRefPlanePrevious += 4;
                pRefPlaneFuture1 += 4;
                pRefPlanePrevious1 += 4;
                pMV += 2;
            }
            else
            {
                xRefPos -= 4;
                yRefPos += 4;
                pRefPlaneFuture += (pitch << 2) - 4;
                pRefPlanePrevious += (pitch << 2) - 4;
                pRefPlaneFuture1 += (pitch << 2) - 4;
                pRefPlanePrevious1 += (pitch << 2) - 4;

                pMV += (widthInBlks << 1) - 2;
            }

            //pt0 +=VPP_CHROMABLK_SIZE;
            //pt1 +=VPP_CHROMABLK_SIZE;
            //pt2 +=VPP_CHROMABLK_SIZE;
            //pt3 +=VPP_CHROMABLK_SIZE;
        } /* for subblock */
    }

} /* reconstructChromaMacroBlockDirect */


void Decoder_reconstructChromaMacroBlockBidir
(
    struct Decoder *t,
    const struct DecoderPackedMotionVector *pMVp,
    const struct DecoderPackedMotionVector *pMVf,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    const I32 pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen

    I32 xRefPos = (mbXOffset >> 1);
    I32 yRefPos = (mbYOffset >> 1);
    I32 offset = xRefPos + (pitch * yRefPos);

    I32 mvxP, mvyP, mvxF, mvyF;

    I32 xintf, yintf, xhf, yhf;
    I32 xintp, yintp, xhp, yhp;

    U32 *pt0,*pt1;
    const U8* pRefFuture;
    const U8* pRefPrevious;

    pRefPlaneFuture += offset;
    pRefPlanePrevious += offset;
    pRefPlaneFuture1+= offset;
    pRefPlanePrevious1+= offset;

    mvxF = pMVf->mvx / 2;
    mvyF = pMVf->mvy / 2;

    mvxP = pMVp->mvx / 2;
    mvyP = pMVp->mvy / 2;

    pRefFuture = pRefPlaneFuture;
    pRefPrevious = pRefPlanePrevious;

    xintf = (mvxF + 1)/INTERP_FACTOR; xhf = 0;
    xintp = (mvxP + 1)/INTERP_FACTOR; xhp = 0;

    pRefFuture += xintf;
    pRefPrevious += xintp;

    yintf = (mvyF + 1)/INTERP_FACTOR; yhf = 0;
    yintp = (mvyP + 1)/INTERP_FACTOR; yhp = 0;

    pRefFuture += yintf * pitch;
    pRefPrevious += yintp * pitch;

    /* interpolate copy the 4 blocks from previous, then future */
    //U BLOCK
    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_ublk_b_addr);

    pRefPrevious = (U8*)((INT32)pRefPrevious & VOC_CFG_PRED_DMA_EADDR_MASK);
    pRefFuture    = (U8*)((INT32)pRefFuture & VOC_CFG_PRED_DMA_EADDR_MASK);

    *pt0++ = (INT32)pRefPrevious;
    *pt1++ = (INT32)pRefFuture;

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture,  pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious+4);
    *pt1++ = (INT32)(pRefFuture+4);

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture  +4,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
    *pt1++ = (INT32)(pRefFuture + (pitch << 2));
    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2),pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)  ,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
    *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);
    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2)+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)+4  ,pt1, pitch);

    //V BLOCK
    pRefFuture = pRefPlaneFuture1;
    pRefPrevious = pRefPlanePrevious1;

    pRefFuture += xintf;
    pRefPrevious += xintp;

    pRefFuture += yintf * pitch;
    pRefPrevious += yintp * pitch;

    pt0   = (U32*)(vppDataIn->interplate_vblk_addr);
    pt1   = (U32*)(vppDataIn->interplate_vblk_b_addr);

    pRefPrevious = (U8 *)((INT32)(pRefPrevious) & VOC_CFG_PRED_DMA_EADDR_MASK);
    pRefFuture    = (U8 *)((INT32)(pRefFuture) & VOC_CFG_PRED_DMA_EADDR_MASK);

    *pt0++ = (INT32)pRefPrevious;
    *pt1++ = (INT32)pRefFuture;

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture,  pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious+4);
    *pt1++ = (INT32)(pRefFuture+4);

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture  +4,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2));
    *pt1++ = (INT32)(pRefFuture + (pitch << 2));

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2),pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)  ,pt1, pitch);
    //pt0 +=VPP_CHROMABLK_SIZE;
    //pt1 +=VPP_CHROMABLK_SIZE;

    *pt0++ = (INT32)(pRefPrevious + (pitch << 2) + 4);
    *pt1++ = (INT32)(pRefFuture + (pitch << 2) + 4);

    //(*t->m_InterpFuncChroma)[0][0](pRefPrevious + (pitch << 2)+4,pt0, pitch);
    //(*t->m_InterpFuncChroma)[0][0](pRefFuture + (pitch << 2)+4  ,pt1, pitch);

} /* reconstructChromaMacroBlockBidir */


void Decoder_reconstructChromaBlock
(
    struct Decoder *t,
    const struct DecoderMBInfo    *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRef,
    const U8               *pRef1,
    const U32               mbXOffset,
    const U32               mbYOffset
)
{
    I32     pitch;
    U32     subblock;
    I32     mvX, mvY, mvXint, mvYint, mvXsubpel, mvYsubpel;
    U32     xRefPos;
    U32     yRefPos;
    U32     offset;
    U32     *pt0,*pt1;
    const U8 *pMCRef,*pMCRef1;

    pitch = t->m_pCurrentFrame->m_pitch>>1;//t->m_pCurrentFrame->m_pitch; sheen

    xRefPos = (mbXOffset >> 1);
    yRefPos = (mbYOffset >> 1);
    offset = xRefPos + (pitch * yRefPos);

    pRef += offset;
    pRef1+= offset;

    pt0   = (U32*)(vppDataIn->interplate_ublk_addr);
    pt1   = (U32*)(vppDataIn->interplate_vblk_addr);

    for (subblock = 0; subblock < 4; subblock++)
    {
        mvX = pMV->mvx / 2;
        mvY = pMV->mvy / 2;

        MV_Sub2FullPels(mvX, &mvXint, &mvXsubpel);
        MV_Sub2FullPels(mvY, &mvYint, &mvYsubpel);

        pMCRef = pRef + mvXint + pitch * mvYint;
        pMCRef1= pRef1+ mvXint + pitch * mvYint;
        /* pMCRef is the motion-compensated reference subblock */

        /* interpolate copy the 4x4 block */
        *pt0++ = (INT32)(pMCRef) & VOC_CFG_PRED_DMA_EADDR_MASK;
        *pt1++ = (INT32)(pMCRef1) & VOC_CFG_PRED_DMA_EADDR_MASK;

        //(*t->m_InterpFuncChroma)[mvYsubpel][mvXsubpel](pMCRef, pt0, pitch);
        //(*t->m_InterpFuncChroma)[mvYsubpel][mvXsubpel](pMCRef1,pt1, pitch);

        /* Move pointers to next subblock. */
        if (subblock != 1)
        {
            pRef += 4;
            pRef1+= 4;
            pMV  += 2;
        }
        else
        {
            /* Move pointers from subblock 1 to subblock 2. */
            I32 offsetToSubblock2 = (pitch << 2) - 4;

            pRef += offsetToSubblock2;
            pRef1+= offsetToSubblock2;
            pMV  += (t->m_pCurrentFrame->m_subBlockSize.width << 1) - 2;
        }
        //pt0 +=VPP_CHROMABLK_SIZE;
        //pt1 +=VPP_CHROMABLK_SIZE;
    }

}   /* reconstructChromaBlock */


/*******************end**********************/








