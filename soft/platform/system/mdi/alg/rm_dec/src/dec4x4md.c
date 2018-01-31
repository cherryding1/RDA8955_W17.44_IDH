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





















#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* try to avoid conflicts with our own prototypes: */
#include "dec4x4m.h"
#include "rvdebug.h"
#include "dec4x4t.h"

/* trace macro: */
#if 0
#define TRACE(s,p)      fprintf(stdout,s,p);
#define TRACE2(s,p1,p2) fprintf(stdout,s,p1,p2);
#else
#define TRACE(s,p)      ((void) 0)
#define TRACE2(s,p1,p2) ((void) 0)
#endif

/*#define PRINT_DECRYPT_TABLE */
/*#define USE_DECRYPT_TABLE */

#ifdef PRINT_DECRYPT_TABLE
#ifdef USE_DECRYPT_TABLE
#error "Can't Use and create the tables at the same time"
#endif
#undef USE_DECRYPT_TABLE
FILE_HDL decrypt;
#endif

#if 0//move to rv_overlay_set.c sheen
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

/* terminates the use of local bitstream variables: */
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

/* sends a given bitvector to the bitstream: */
#define BS_PUTBITS(bits,len)                            \
    do {                                                \
        register int shift = 32 - (len);                \
        /* we can't let bitOffset be equal 32... */     \
        if ((shift -= bitOffset) <= 0) {                \
            register int bitsFlushed;                   \
            /* flush bitBuffer: */                      \
            pbs[0] = (unsigned char) (bitBuffer >> 24); \
            pbs[1] = (unsigned char) (bitBuffer >> 16); \
            pbs[2] = (unsigned char) (bitBuffer >> 8);  \
            pbs[3] = (unsigned char) bitBuffer;         \
            /* update local bitstream variables: */     \
            bitsFlushed = bitOffset & ~7;               \
            pbs += bitOffset >> 3; bitOffset &= 7;      \
            bitBuffer <<= bitsFlushed;                  \
            shift += bitsFlushed;                       \
        }                                               \
        /* add bits into a bitBuffer: */                \
        bitBuffer |= (bits) << shift;                   \
        bitOffset += (len);                             \
    } while (0)

#if 0 //move to dec4x4m.h sheen
/* max. # of bits that can be retrieved at no extra cost: */
#define BS_MAXTRYBITS   16
#endif

/* retrieves a bit vector from the bitstream. */
#define BS_TRYBITS(len)    ((unsigned int)bitBuffer >> (32-(len)))

/* shifts the current position in the bitstream: */
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

/* Reads a bit vector from the bitstream. */
#define BS_GETBITS(bits,len)                            \
    do {                                                \
        bits = BS_TRYBITS(len);                         \
        BS_SKIPBITS(len);                               \
    } while (0)

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

/* encode a symbol using Huffman code/len tables: */
#define HUF_ENCODE(val,code,len) BS_PUTBITS(code[val],len[val])

/* decode a symbol using Moffat-Turpin/Brodnik-Carlsson algorithm: */
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

#if 0// move to dec4x4m.h sheen

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

#endif

#if 1
/* descriptor to level conversion tables: */
unsigned char *dsc_to_l0;// [MAX_2x2_DSC]; /* Flawfinder: ignore */ sheen
unsigned char *dsc_to_l1;// [MAX_2x2_DSC]; /* Flawfinder: ignore */
unsigned char *dsc_to_l2;// [MAX_2x2_DSC]; /* Flawfinder: ignore */
unsigned char *dsc_to_l3;// [MAX_2x2_DSC]; /* Flawfinder: ignore */
#endif

#if 0// move to dec4x4m.h sheen

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

#endif

#if 0//move to dec4x4m.h sheen

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
#endif


/* luma descriptor to cbp4x4 conversion tables: */
unsigned int *y_to_cbp4x4;// [MAX_8x8_DSC]; sheen

/* 8x8 cbp to context-predictor conversion table: */
unsigned int *cbp_to_cx;// [16]; sheen

#if 0// move to dec4x4m.h sheen

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

#endif

#if 0// move to dec4x4t.c sheen

const static int intra_qp_to_idx [MAX_QP+1] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   /* 0-9   */
    1, 1, 1, 1, 1, 1,               /* 10-15 */
    2, 2, 2, 2,                     /* 16-20 */
    3, 3, 3, 3, 3,                  /* 21-25 */
    4, 4, 4, 4, 4                   /* 26-30 */
};

#endif

#if 0// move to dec4x4m.h sheen

#define MAX_INTER_QP_REGIONS 7

#endif

#if 0// move to dec4x4t.c sheen

const static int inter_qp_to_idx [MAX_QP+1] =
{
    0, 0, 0, 0, 0, 0, 0,            /* 0-6   */
    1, 1, 1, 1,                     /* 7-10  */
    2, 2, 2, 2,                     /* 11-14 */
    3, 3, 3, 3,                     /* 15-18 */
    4, 4, 4, 4,                     /* 19-22 */
    5, 5, 5, 5,                     /* 23-26 */
    6, 6, 6, 6                      /* 27-30 */
};
#endif


#if 0// move to dec4x4m.h sheen
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
 * In a more compact implementation, we should use a cache containing
 * 2-4 most frequently used tables (depending on intra/inter and qp),
 * and regenerate other ones as needed.
 *
 * Such implementation will use 5+7/2..4 = 3..6 times less memory.
 *
 */

/* full set of decode tables: */
INTRA_DECODE_TABLES *intra_decode_tables;//[MAX_INTRA_QP_REGIONS]; sheen
INTER_DECODE_TABLES *inter_decode_tables;//[MAX_INTER_QP_REGIONS]; sheen

/* compressed length tables: */
//#include "dec4x4t.h"

/*
* Generates data structure for fast Huffman decoding.
* Use:
*  int make_decode_tables(int n, unsigned char *len,
*     unsigned int *next, unsigned int *base, unsigned int *offset, void *symbol);
* Input:
*  n - the number of symbols in the tree
*  len - code lengths
*  next - the list of the subsequent non-empty levels
*  base - left-justified smallest canonical codewords of given length
*  offset - a table to contain indices of base canonical codewords
*  symbol - maps canonical codeword back to the original alphabet
* Returns:
 *  !0, if success
 *  0, if error
 */
static int make_decode_tables (unsigned int n, unsigned char *len,
                               unsigned int *next, unsigned int *base, unsigned int *offset, void *symbol)
{
    unsigned int count[MAX_DEPTH+1], start[MAX_DEPTH+2];
    register unsigned int i, j;

    /* clear arrays: */
    memset (count, 0, sizeof (count));
    memset (start, 0, sizeof (start));

    /* count the number of leaves on each level: */
    for (i = 0; i <= MAX_DEPTH; i++) count [i] = 0;     /* reset */
    for (i = 0; i < n; i++) count [len [i]] ++;         /* count */

    /* calculate start codewords for each level: */
    start [1] = 0;
    for (i = 1; i <= MAX_DEPTH; i++)
        start [i + 1] = (start [i] + count [i]) << 1;   /* start[i+1] = xxxxx0; */

    /*
     * Generate tables for Moffat-Turpin/Carlsson-Brodnik decoding:
     */

    /* find maximum code length: */
    for (j = MAX_DEPTH; j > 0; j--)
        if (count[j] != 0) break;

    /* generate next[] table: */
    for (i = 0; i < j; i++)
    {
        /* find nearest non-zero entry: */
        register unsigned int k = i+1;
        while (count[k] == 0 && k <= j) k++;
        /* store it: */
        next [i] = k;
    }
    for (; i<=MAX_DEPTH+1; i++)
        next [i] = j+1;

    /* generate base values: */
    base [0] = 0;
    for (i = 1; i <= j; i++)
        base [i] = start [i] << (32 - i);   /* just left-aligned start codes */
    /* fill the rest of the base table: */
    for (; i<=MAX_DEPTH+1; i++)
        base [i] = 0xFFFFFFFF;              /* to terminate code-length search loop */

#ifdef PRINT_DECRYPT_TABLE
    /* Base Map */
    fprintf(decrypt, "\n", base [i]);
    for (i=1; i<=MAX_DEPTH; i++)
        fprintf(decrypt, "/* Length %2d Base %4x Leaves %d \n", i, (base[i])>>16, count[i]); */
#endif

    /* count linear offsets of base codewords: */
    offset [0] = 0;
    offset [1] = 0;
    for (i = 1; i < MAX_DEPTH; i++)
        offset [i+1] = offset [i] + count [i];

    /* generate canonical->original symbol map: */
    for (i = 0; i <= MAX_DEPTH; i++)
        start [i] = offset [i];             /* reset start table */
    if (n < 256)
    {
        /* 8-bit symbol map: */
        for (i = 0; i < (unsigned int) n; i++)
            ((unsigned char *) symbol) [start [len [i]] ++] = i; /* Flawfinder: ignore */
    }
    else
    {
        /* 16-bit symbol map: */
        for (i = 0; i < (unsigned int) n; i++)
            ((unsigned short *) symbol) [start [len [i]] ++] = i;
    }

    return 1;
}

#if 0// move to dec4x4t.c sheen
/* gamma sequence (we use it to descramble the tables): */
const static unsigned int _y[8] =
{
    0x69727559, 0x65522079, 0x6b696e7a, 0x72793c20,
    0x696e7a65, 0x6572406b, 0x632e6c61, 0x003e6d6f
};
#endif

static unsigned char *len_tbl ;//[MAX_CBP]; /* use largest table size */ /* Flawfinder: ignore */ sheen
/* decompresses length table & generates canonic Huffman codes: */
static void _yy (unsigned int n,
                 int start, const unsigned long *packed_len_tbl,
                 unsigned int *next, unsigned int *base, unsigned int *offset, void *symbol)
{
    register int i, j, k, l;
    //static unsigned char len_tbl [MAX_CBP]; /* use largest table size */ /* Flawfinder: ignore */ sheen
    /* clear len table: */
    //memset (len_tbl, 0, sizeof (len_tbl));
    memset (len_tbl, 0, MAX_CBP);

    /* decompress length table: */
    len_tbl[0] = 0; /* it is skipped if start=1 */
    for (i=start; i<(signed int)n; i++)
    {
        j = i & 7; k = i >> 3;
        l = (packed_len_tbl[k] ^ _y[k&7]) >> (7-j)*4;
        len_tbl[i] = (unsigned char) (l & 0x0F) + 1;

    }
#ifdef PRINT_DECRYPT_TABLE
    fprintf(decrypt, "={\n/* %d\n",n);   */
    fprintf(decrypt, " %d, %d, %d, %d", len_tbl[i], len_tbl[i+1], len_tbl[i+2], len_tbl[i+3]);
    for (i=4; i<n; i+=4)
    {
        fprintf(decrypt, ",\n");
        fprintf(decrypt, " %d, %d, %d, %d", len_tbl[i], len_tbl[i+1], len_tbl[i+2], len_tbl[i+3]);
    }
    fprintf(decrypt, "\n};");
#endif
    /* make decode tables: */
    make_decode_tables (n, len_tbl, next, base, offset, symbol);
}

#ifdef USE_DECRYPT_TABLE
#include "dec4x4help.c"
#else /* USE_DECRYPT_TABLE */
/*
 * Initializes 4x4 decoding tables.
 * Use:
 *  void decode_4x4_init();
 */
int decode_4x4_init()
{
    register int i, j, qp;
#ifdef PRINT_DECRYPT_TABLE
    decrypt=fopen("dec4x4tables.c","w");
#endif

    //add sheen. need free
    dsc_to_l0=(unsigned char*)RV_Allocate(MAX_2x2_DSC, 0);
    if(!dsc_to_l0) return -1;
    dsc_to_l1=(unsigned char*)RV_Allocate(MAX_2x2_DSC, 0);
    if(!dsc_to_l1) return -1;
    dsc_to_l2=(unsigned char*)RV_Allocate(MAX_2x2_DSC, 0);
    if(!dsc_to_l2) return -1;
    dsc_to_l3=(unsigned char*)RV_Allocate(MAX_2x2_DSC, 0);
    if(!dsc_to_l3) return -1;

    intra_decode_tables=(INTRA_DECODE_TABLES*)RV_Allocate(MAX_INTRA_QP_REGIONS*sizeof(INTRA_DECODE_TABLES), 0);
    if(!intra_decode_tables) return -1;
    inter_decode_tables=(INTER_DECODE_TABLES*)RV_Allocate(MAX_INTER_QP_REGIONS*sizeof(INTER_DECODE_TABLES), 0);
    if(!inter_decode_tables) return -1;

    len_tbl=(unsigned char*)RV_Allocate(MAX_CBP, 0);
    if(!len_tbl) return -1;

    y_to_cbp4x4=(unsigned int *)RV_Allocate(MAX_8x8_DSC*4, 0);
    if(!y_to_cbp4x4) return -1;

    cbp_to_cx=(unsigned int *)RV_Allocate(16*4, 0);
    if(!cbp_to_cx) return -1;

    //end.sheen

    /* initialize descriptor to level conversion tables: */
    for (i=0; i<MAX_2x2_DSC; i++)
    {
        j = (unsigned int)i;
        dsc_to_l3 [i] = j % MAX_123_LEVEL; j /= MAX_123_LEVEL;
        dsc_to_l2 [i] = j % MAX_123_LEVEL; j /= MAX_123_LEVEL;
        dsc_to_l1 [i] = j % MAX_123_LEVEL; j /= MAX_123_LEVEL;
        dsc_to_l0 [i] = j;
    }

    /* initialize luma to cbp4x4 conversion table: */
    for (i=0; i<MAX_8x8_DSC; i++)
        y_to_cbp4x4 [i] = ((i&8)>>3) | ((i&4)>>1) | ((i&2)<<3) | ((i&1)<<5);

    /* 8x8 cbp to context predictor: */
    for (i=0; i<16; i++)
    {
        j = 0; if (i&1) j++; if (i&2) j++; if (i&4) j++; if (i&8) j++;
        cbp_to_cx [i] = j? j-1: 0;
    }

    /* generate intra decode tables: */
    for (qp=0; qp<MAX_INTRA_QP_REGIONS; qp++)
    {

        /* intra cbp tables: */
        for (i=0; i<2; i++)
        {

#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char intra%d_cbp_desc_len_QP%d[/*MAX_CBP*/%d]", i, qp, MAX_CBP);
#endif
            /* cbp descriptor: */
            _yy (MAX_CBP, 0, _x_rv.intra_cbp[qp][i],
                 intra_decode_tables[qp].cbp[i].cbp.next,
                 intra_decode_tables[qp].cbp[i].cbp.base,
                 intra_decode_tables[qp].cbp[i].cbp.offset,
                 (void *) (intra_decode_tables[qp].cbp[i].cbp.symbol));

            /* 8x8 descriptors: */
            for (j=0; j<4; j++)
            {

#ifdef PRINT_DECRYPT_TABLE
                fprintf(decrypt, "\nstatic unsigned char intra%d_8x8_desc%d_len_QP%d[MAX_8x8DSC]",i, j, qp);
#endif

                _yy (MAX_8x8_DSC, 1, _x_rv.intra_8x8_dsc[qp][i][j],
                     intra_decode_tables[qp].cbp[i]._8x8_dsc[j].next,
                     intra_decode_tables[qp].cbp[i]._8x8_dsc[j].base,
                     intra_decode_tables[qp].cbp[i]._8x8_dsc[j].offset,
                     (void *) (intra_decode_tables[qp].cbp[i]._8x8_dsc[j].symbol));
            }
        }

        /* intra luma tables: */
        for (i=0; i<3; i++)
        {
#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char intra_luma_4x4_desc%d_len_QP%d[MAX_4x4DSC]", i, qp);
#endif
            _yy (MAX_4x4_DSC, i<2, _x_rv.intra_luma_4x4_dsc[qp][i],
                 intra_decode_tables[qp].luma._4x4_dsc[i].next,
                 intra_decode_tables[qp].luma._4x4_dsc[i].base,
                 intra_decode_tables[qp].luma._4x4_dsc[i].offset,
                 (void *) (intra_decode_tables[qp].luma._4x4_dsc[i].symbol));
        }
        for (i=0; i<2; i++)
        {
#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char intra_luma_2x2_desc%d_len_QP%d[MAX_2x2DSC]", i, qp);
#endif
            _yy (MAX_2x2_DSC, 1, _x_rv.intra_luma_2x2_dsc[qp][i],
                 intra_decode_tables[qp].luma._2x2_dsc[i].next,
                 intra_decode_tables[qp].luma._2x2_dsc[i].base,
                 intra_decode_tables[qp].luma._2x2_dsc[i].offset,
                 (void *) (intra_decode_tables[qp].luma._2x2_dsc[i].symbol));
        }

#ifdef PRINT_DECRYPT_TABLE
        fprintf(decrypt, "\nstatic unsigned char intra_chroma_4x4_desc_len_QP%d[MAX_4x4DSC]", qp);
#endif
        /* intra chroma tables: */
        _yy (MAX_4x4_DSC, 1, _x_rv.intra_chroma_4x4_dsc[qp],
             intra_decode_tables[qp].chroma._4x4_dsc.next,
             intra_decode_tables[qp].chroma._4x4_dsc.base,
             intra_decode_tables[qp].chroma._4x4_dsc.offset,
             (void *) (intra_decode_tables[qp].chroma._4x4_dsc.symbol));
        for (i=0; i<2; i++)
        {
#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char intra_chroma_2x2_desc%d_len_QP%d[MAX_2x2DSC]", i, qp);
#endif
            _yy (MAX_2x2_DSC, 1, _x_rv.intra_chroma_2x2_dsc[qp][i],
                 intra_decode_tables[qp].chroma._2x2_dsc[i].next,
                 intra_decode_tables[qp].chroma._2x2_dsc[i].base,
                 intra_decode_tables[qp].chroma._2x2_dsc[i].offset,
                 (void *) (intra_decode_tables[qp].chroma._2x2_dsc[i].symbol));
        }

#ifdef PRINT_DECRYPT_TABLE
        fprintf(decrypt, "\nstatic unsigned char intra_level_desc_len_QP%d[MAX_LEVEL_DSC]", qp);
#endif
        /* intra extra levels: */
        _yy (MAX_LEVEL_DSC, 0, _x_rv.intra_level_dsc[qp],
             intra_decode_tables[qp].level_dsc.next,
             intra_decode_tables[qp].level_dsc.base,
             intra_decode_tables[qp].level_dsc.offset,
             (void *) (intra_decode_tables[qp].level_dsc.symbol));
    }

    /* generate inter decode tables: */
    for (qp=0; qp<MAX_INTER_QP_REGIONS; qp++)
    {

        /* inter cbp table: */
#ifdef PRINT_DECRYPT_TABLE
        fprintf(decrypt, "\nstatic unsigned char inter_cbp_desc_len_QP%d[MAX_CBP]",qp);
#endif
        /* inter cbp descriptor: */
        _yy (MAX_CBP, 0, _x_rv.inter_cbp[qp],
             inter_decode_tables[qp].cbp.cbp.next,
             inter_decode_tables[qp].cbp.cbp.base,
             inter_decode_tables[qp].cbp.cbp.offset,
             (void *) (inter_decode_tables[qp].cbp.cbp.symbol));

        /* 8x8 descriptors: */
        for (j=0; j<4; j++)
        {

#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char inter_8x8_desc%d_len_QP%d[MAX_8x8DSC]", j,qp);
#endif
            _yy (MAX_8x8_DSC, 1, _x_rv.inter_8x8_dsc[qp][j],
                 inter_decode_tables[qp].cbp._8x8_dsc[j].next,
                 inter_decode_tables[qp].cbp._8x8_dsc[j].base,
                 inter_decode_tables[qp].cbp._8x8_dsc[j].offset,
                 (void *) (inter_decode_tables[qp].cbp._8x8_dsc[j].symbol));
        }

#ifdef PRINT_DECRYPT_TABLE
        fprintf(decrypt, "\nstatic unsigned char inter_luma_4x4_desc_len_QP%d[MAX_4x4DSC]",qp);
#endif
        /* inter luma tables: */
        _yy (MAX_4x4_DSC, 1, _x_rv.inter_luma_4x4_dsc[qp],
             inter_decode_tables[qp].luma._4x4_dsc.next,
             inter_decode_tables[qp].luma._4x4_dsc.base,
             inter_decode_tables[qp].luma._4x4_dsc.offset,
             (void *) (inter_decode_tables[qp].luma._4x4_dsc.symbol));

        for (i=0; i<2; i++)
        {

#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char inter_luma_2x2_desc%d_len_QP%d[MAX_2x2DSC]", i,qp);
#endif

            _yy (MAX_2x2_DSC, 1, _x_rv.inter_luma_2x2_dsc[qp][i],
                 inter_decode_tables[qp].luma._2x2_dsc[i].next,
                 inter_decode_tables[qp].luma._2x2_dsc[i].base,
                 inter_decode_tables[qp].luma._2x2_dsc[i].offset,
                 (void *) (inter_decode_tables[qp].luma._2x2_dsc[i].symbol));
        }

#ifdef PRINT_DECRYPT_TABLE
        fprintf(decrypt, "\nstatic unsigned char inter_chroma_4x4_desc_len_QP%d[MAX_4x4DSC]",qp);
#endif

        /* inter chroma tables: */
        _yy (MAX_4x4_DSC, 1, _x_rv.inter_chroma_4x4_dsc[qp],
             inter_decode_tables[qp].chroma._4x4_dsc.next,
             inter_decode_tables[qp].chroma._4x4_dsc.base,
             inter_decode_tables[qp].chroma._4x4_dsc.offset,
             (void *) (inter_decode_tables[qp].chroma._4x4_dsc.symbol));

        for (i=0; i<2; i++)
        {
#ifdef PRINT_DECRYPT_TABLE
            fprintf(decrypt, "\nstatic unsigned char inter_chroma_2x2_desc%d_len_QP%d[MAX_2x2DSC]", i,qp);
#endif

            _yy (MAX_2x2_DSC, 1, _x_rv.inter_chroma_2x2_dsc[qp][i],
                 inter_decode_tables[qp].chroma._2x2_dsc[i].next,
                 inter_decode_tables[qp].chroma._2x2_dsc[i].base,
                 inter_decode_tables[qp].chroma._2x2_dsc[i].offset,
                 (void *) (inter_decode_tables[qp].chroma._2x2_dsc[i].symbol));
        }
#ifdef PRINT_DECRYPT_TABLE
        fprintf(decrypt, "\nstatic unsigned char inter_level_desc_len_QP%d[MAX_LEVEL_DSC]",qp);
#endif
        /* inter extra levels: */
        _yy (MAX_LEVEL_DSC, 0, _x_rv.inter_level_dsc[qp],
             inter_decode_tables[qp].level_dsc.next,
             inter_decode_tables[qp].level_dsc.base,
             inter_decode_tables[qp].level_dsc.offset,
             (void *) (inter_decode_tables[qp].level_dsc.symbol));
    }
#ifdef PRINT_DECRYPT_TABLE
    fclose(decrypt);
#endif

#if 0// print table
    {
        FILE *pTab;
        int i,j,k,l,m,n;
        pTab=fopen("H://tab.txt","w");

        fprintf(pTab, "\n {//dsc_to_l0 %d bytes\n",MAX_2x2_DSC);
        for(i=0; i<MAX_2x2_DSC-1; i++)
            fprintf(pTab, "0x%x, ",dsc_to_l0[i]);
        fprintf(pTab, "0x%x \n };\n",dsc_to_l0[MAX_2x2_DSC-1]);

        fprintf(pTab, "\n {//dsc_to_l1 %d bytes\n",MAX_2x2_DSC);
        for(i=0; i<MAX_2x2_DSC-1; i++)
            fprintf(pTab, "0x%x, ",dsc_to_l1[i]);
        fprintf(pTab, "0x%x \n };\n",dsc_to_l1[MAX_2x2_DSC-1]);

        fprintf(pTab, "\n {//dsc_to_l2 %d bytes\n",MAX_2x2_DSC);
        for(i=0; i<MAX_2x2_DSC-1; i++)
            fprintf(pTab, "0x%x, ",dsc_to_l2[i]);
        fprintf(pTab, "0x%x \n };\n",dsc_to_l2[MAX_2x2_DSC-1]);

        fprintf(pTab, "\n {//dsc_to_l3 %d bytes\n",MAX_2x2_DSC);
        for(i=0; i<MAX_2x2_DSC-1; i++)
            fprintf(pTab, "0x%x, ",dsc_to_l3[i]);
        fprintf(pTab, "0x%x \n };\n",dsc_to_l3[MAX_2x2_DSC-1]);


        fprintf(pTab, "\n {//y_to_cbp4x4 %d bytes\n",MAX_8x8_DSC);
        for(i=0; i<MAX_8x8_DSC-1; i++)
            fprintf(pTab, "0x%x, ",y_to_cbp4x4[i]);
        fprintf(pTab, "0x%x \n };\n",y_to_cbp4x4[MAX_8x8_DSC-1]);

        fprintf(pTab, "\n {//cbp_to_cx 16 bytes\n");
        for(i=0; i<16-1; i++)
            fprintf(pTab, "0x%x, ",cbp_to_cx[i]);
        fprintf(pTab, "0x%x \n };\n",cbp_to_cx[16-1]);

        intra_decode_tables=(INTRA_DECODE_TABLES*)RV_Allocate(MAX_INTRA_QP_REGIONS*sizeof(INTRA_DECODE_TABLES), 0);
        inter_decode_tables=(INTER_DECODE_TABLES*)RV_Allocate(MAX_INTER_QP_REGIONS*sizeof(INTER_DECODE_TABLES), 0);

        fprintf(pTab, "\n {//intra_decode_tables %d bytes\n",MAX_INTRA_QP_REGIONS*sizeof(INTRA_DECODE_TABLES));
        for(i=0; i<MAX_INTRA_QP_REGIONS; i++)
        {
            fprintf(pTab, "{//cbp \n");
            for(j=0; j<2; j++)
            {

                fprintf(pTab, "{//cbp \n");
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j].cbp.next[k]);
                }
                fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].cbp[j].cbp.next[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j].cbp.base[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].cbp[j].cbp.base[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j].cbp.offset[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].cbp[j].cbp.offset[MAX_DEPTH]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_CBP-1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j].cbp.symbol[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].cbp[j].cbp.symbol[MAX_CBP-1]);
                fprintf(pTab, "\n }, \n");


                fprintf(pTab, "{//_8x8_dsc \n");
                for(k=0; k<4; k++)
                {
                    fprintf(pTab, "{ \n");
                    fprintf(pTab, "{ \n");
                    for(l=0; l<MAX_DEPTH+1; l++)
                    {
                        fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j]._8x8_dsc[k].next[l]);
                    }
                    fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].cbp[j]._8x8_dsc[k].next[MAX_DEPTH+1]);
                    fprintf(pTab, "{ \n");
                    for(l=0; l<MAX_DEPTH+1; l++)
                    {
                        fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j]._8x8_dsc[k].base[l]);
                    }
                    fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].cbp[j]._8x8_dsc[k].base[MAX_DEPTH+1]);
                    fprintf(pTab, "{ \n");
                    for(l=0; l<MAX_DEPTH; l++)
                    {
                        fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j]._8x8_dsc[k].offset[l]);
                    }
                    fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].cbp[j]._8x8_dsc[k].offset[MAX_DEPTH]);
                    fprintf(pTab, "{ \n");
                    for(l=0; l<MAX_8x8_DSC-1; l++)
                    {
                        fprintf(pTab, "0x%x, ",intra_decode_tables[i].cbp[j]._8x8_dsc[k].symbol[l]);
                    }
                    fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].cbp[j]._8x8_dsc[k].symbol[MAX_8x8_DSC-1]);
                    fprintf(pTab, "\n }, \n");

                }
                fprintf(pTab, "\n }, \n");
            }
            fprintf(pTab, "\n }, \n");

            fprintf(pTab, "{//luma \n");
            fprintf(pTab, "{ \n");
            for(j=0; j<3; j++)
            {
                fprintf(pTab, "{ \n");
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._4x4_dsc[j].next[k]);
                }
                fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].luma._4x4_dsc[j].next[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._4x4_dsc[j].base[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].luma._4x4_dsc[j].base[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._4x4_dsc[j].offset[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].luma._4x4_dsc[j].offset[MAX_DEPTH]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_4x4_DSC-1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._4x4_dsc[j].symbol[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].luma._4x4_dsc[j].symbol[MAX_4x4_DSC-1]);
                fprintf(pTab, "\n }, \n");
            }
            fprintf(pTab, "\n }, \n");

            fprintf(pTab, "{ \n");
            for(j=0; j<2; j++)
            {
                fprintf(pTab, "{ \n");
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._2x2_dsc[j].next[k]);
                }
                fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].luma._2x2_dsc[j].next[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._2x2_dsc[j].base[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].luma._2x2_dsc[j].base[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._2x2_dsc[j].offset[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].luma._2x2_dsc[j].offset[MAX_DEPTH]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_2x2_DSC-1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].luma._2x2_dsc[j].symbol[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].luma._2x2_dsc[j].symbol[MAX_2x2_DSC-1]);
                fprintf(pTab, "\n }, \n");
            }
            fprintf(pTab, "\n } \n");
            fprintf(pTab, "\n }, \n");


            fprintf(pTab, "{//chroma \n");
            fprintf(pTab, "{ \n");

            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_DEPTH+1; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._4x4_dsc.next[k]);
            }
            fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].chroma._4x4_dsc.next[MAX_DEPTH+1]);
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_DEPTH+1; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._4x4_dsc.base[k]);
            }
            fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].chroma._4x4_dsc.base[MAX_DEPTH+1]);
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_DEPTH; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._4x4_dsc.offset[k]);
            }
            fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].chroma._4x4_dsc.offset[MAX_DEPTH]);
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_4x4_DSC-1; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._4x4_dsc.symbol[k]);
            }
            fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].chroma._4x4_dsc.symbol[MAX_4x4_DSC-1]);

            fprintf(pTab, "\n }, \n");

            fprintf(pTab, "{ \n");
            for(j=0; j<2; j++)
            {
                fprintf(pTab, "{ \n");
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._2x2_dsc[j].next[k]);
                }
                fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].chroma._2x2_dsc[j].next[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH+1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._2x2_dsc[j].base[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].chroma._2x2_dsc[j].base[MAX_DEPTH+1]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_DEPTH; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._2x2_dsc[j].offset[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].chroma._2x2_dsc[j].offset[MAX_DEPTH]);
                fprintf(pTab, "{ \n");
                for(k=0; k<MAX_2x2_DSC-1; k++)
                {
                    fprintf(pTab, "0x%x, ",intra_decode_tables[i].chroma._2x2_dsc[j].symbol[k]);
                }
                fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].chroma._2x2_dsc[j].symbol[MAX_2x2_DSC-1]);
                fprintf(pTab, "\n }, \n");
            }
            fprintf(pTab, "\n } \n");
            fprintf(pTab, "\n }, \n");


            fprintf(pTab, "{//level_dsc \n");
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_DEPTH+1; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].level_dsc.next[k]);
            }
            fprintf(pTab, "0x%x \n}, \n",intra_decode_tables[i].level_dsc.next[MAX_DEPTH+1]);
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_DEPTH+1; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].level_dsc.base[k]);
            }
            fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].level_dsc.base[MAX_DEPTH+1]);
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_DEPTH; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].level_dsc.offset[k]);
            }
            fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].level_dsc.offset[MAX_DEPTH]);
            fprintf(pTab, "{ \n");
            for(k=0; k<MAX_LEVEL_DSC-1; k++)
            {
                fprintf(pTab, "0x%x, ",intra_decode_tables[i].level_dsc.symbol[k]);
            }
            fprintf(pTab, "0x%x }, \n",intra_decode_tables[i].level_dsc.symbol[MAX_LEVEL_DSC-1]);
            fprintf(pTab, "\n }, \n");
        }
        fprintf(pTab, "\n };\n");

        fclose(pTab);
    }
#endif

    return 0;//sheen
}
#endif /*USE_DECRYPT_TABLE */

#if 0 //move to rv_voerlay_set.c sheen
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

    TRACE2("decoding 4x4 block type %d,%d\n", intra, chroma);

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
                TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);   \
                if (c == max_level-1) {         \
                    HUF_DECODE(s,MAX_LEVEL_DSC, \
                        p_level_dsc->next,p_level_dsc->base,  \
                        p_level_dsc->offset,p_level_dsc->symbol);\
                    /* check if escape code: */ \
                    if (s >= MAX_EXTRA_LEVEL-1) { \
                        register int l;         \
                        l = s - (MAX_EXTRA_LEVEL-1); \
                        TRACE("class: %d ", l); \
                        if (l > 1) {            \
                            BS_GETBITS(s, l-1); \
                            TRACE("bits: %d ", s); \
                            s += (1U << (l-1)) + MAX_EXTRA_LEVEL-1; \
                        }                       \
                    }                           \
                    c += s;                     \
                }                               \
                BS_GETBITS(s,1);                \
                if (s) c =- c;                  \
                coef = c; /* send to the output */ \
                TRACE("decoded coefficient: %d", c);  \
                TRACE2(" (%08x,%02d)\n", bitBuffer, bitOffset); \
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
    TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
    HUF_DECODE(d,MAX_4x4_DSC,p_4x4_dsc->next,p_4x4_dsc->base,p_4x4_dsc->offset,p_4x4_dsc->symbol);
    TRACE("decoded 4x4 descriptor: 0x%x\n", d);
    d2 = d >> 3;
    if (d2) DECODE_2x2_COEFFS(d2, block[0], block[1], block[4], block[5]);

    /* decode the remaining 2x2 blocks: */
    if (d & 4)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(d2,MAX_2x2_DSC,p_2x2_dsc[0].next,p_2x2_dsc[0].base,p_2x2_dsc[0].offset,p_2x2_dsc[0].symbol);
        TRACE("decoded 2x2 descriptor: 0x%x\n", d2);
        DECODE_2x2_COEFFS(d2, block[2], block[3], block[6], block[7]);
    }
    if (d & 2)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(d2,MAX_2x2_DSC,p_2x2_dsc[0].next,p_2x2_dsc[0].base,p_2x2_dsc[0].offset,p_2x2_dsc[0].symbol);
        TRACE("decoded 2x2 descriptor: 0x%x\n", d2);
        DECODE_2x2_COEFFS(d2, block[8], block[12],block[9], block[13]);
    }
    if (d & 1)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(d2,MAX_2x2_DSC,p_2x2_dsc[1].next,p_2x2_dsc[1].base,p_2x2_dsc[1].offset,p_2x2_dsc[1].symbol);
        TRACE("decoded 2x2 descriptor: 0x%x\n", d2);
        DECODE_2x2_COEFFS(d2, block[10],block[11],block[14],block[15]);
    }

    /* update bit-stream vars & exit: */
    BS_CLOSE(p_pbs, p_bitOffset, 0);
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

    TRACE("decoding CBP type %d\n", intra);

    /* decode the master CBP descriptor: */
    TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
    HUF_DECODE(d,MAX_CBP,ct->cbp.next,ct->cbp.base,ct->cbp.offset,ct->cbp.symbol);
    TRACE("decoded cbp descriptor: 0x%x\n", d);

    /* decode 8x8 descriptors: */
    cx = cbp_to_cx [d & 0x0F];
    if (d & 8)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        TRACE("decoded 8x8 descriptor: 0x%x\n", c);
        cbp |= y_to_cbp4x4 [c];
    }
    if (d & 4)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        TRACE("decoded 8x8 descriptor: 0x%x\n", c);
        cbp |= y_to_cbp4x4 [c] << 2;
    }
    if (d & 2)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        TRACE("decoded 8x8 descriptor: 0x%x\n", c);
        cbp |= y_to_cbp4x4 [c] << 8;
    }
    if (d & 1)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        HUF_DECODE(c,MAX_8x8_DSC,ct->_8x8_dsc[cx].next,ct->_8x8_dsc[cx].base,ct->_8x8_dsc[cx].offset,ct->_8x8_dsc[cx].symbol);
        TRACE("decoded 8x8 descriptor: 0x%x\n", c);
        cbp |= y_to_cbp4x4 [c] << 10;
    }

    /* decode chroma bits: */
    d >>= 4;
    c = ccbp_to_c0 [d];
    if (c == 2) cbp |= 0x11 << 16;
    else if (c == 1)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        BS_GETBITS(c, 1);
        TRACE("decoded chroma descriptor: 0x%x\n", c);
        cbp |= c << 16;
        cbp |= (c^1) << 20;
    }
    c = ccbp_to_c1 [d];
    if (c == 2) cbp |= 0x11 << 17;
    else if (c == 1)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        BS_GETBITS(c, 1);
        TRACE("decoded chroma descriptor: 0x%x\n", c);
        cbp |= c << 17;
        cbp |= (c^1) << 21;
    }
    c = ccbp_to_c2 [d];
    if (c == 2) cbp |= 0x11 << 18;
    else if (c == 1)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        BS_GETBITS(c, 1);
        TRACE("decoded chroma descriptor: 0x%x\n", c);
        cbp |= c << 18;
        cbp |= (c^1) << 22;
    }
    c = ccbp_to_c3 [d];
    if (c == 2) cbp |= 0x11 << 19;
    else if (c == 1)
    {
        TRACE2("(%08x,%02d) ", bitBuffer, bitOffset);
        BS_GETBITS(c, 1);
        TRACE("decoded chroma descriptor: 0x%x\n", c);
        cbp |= c << 19;
        cbp |= (c^1) << 23;
    }

    /* update bit-stream vars: */
    BS_CLOSE(p_pbs, p_bitOffset, 0);

    /* return the descriptor: */
    return cbp;
}

/* dec4x4md.c - end of file */
#endif

