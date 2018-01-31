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
























#include "h263_config.h"
#include "h263_global.h"
#ifdef MP4_3GP_SUPPORT

#ifndef MEDIA_VOCVID_SUPPORT
//add  4.07
////////////////////////////////lowres_idct16///////////////////////
/*
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
*/
typedef short DCTELEM;
#define EIGHT_BIT_SAMPLES

#define DCTSIZE 8
#define DCTSIZE2 64

#define GLOBAL

#define RIGHT_SHIFT(x, n) ((x) >> (n))

typedef DCTELEM DCTBLOCK[DCTSIZE2];

#define CONST_BITS 13


#if DCTSIZE != 8
Sorry, this code only copes with 8x8 DCTs. /* deliberate syntax err */
#endif

#ifdef EIGHT_BIT_SAMPLES
#define PASS1_BITS  2
#else
#define PASS1_BITS  1   /* lose a little precision to avoid overflow */
#endif

#define ONE         ((int32_t) 1)

#define CONST_SCALE (ONE << CONST_BITS)

#define FIX(x)  ((int32_t) ((x) * CONST_SCALE + 0.5))
#define DESCALE(x,n)  RIGHT_SHIFT((x) + (ONE << ((n)-1)), n)
#ifdef EIGHT_BIT_SAMPLES
#ifdef SHORTxSHORT_32           /* may work if 'int' is 32 bits */
#define MULTIPLY(var,const)  (((int16_t) (var)) * ((int16_t) (const)))
#endif
#ifdef SHORTxLCONST_32          /* known to work with Microsoft C 6.0 */
#define MULTIPLY(var,const)  (((int16_t) (var)) * ((int32_t) (const)))
#endif
#endif

#ifndef MULTIPLY                /* default definition */
#define MULTIPLY(var,const)  ((var) * (const))
#endif

#define FIX_0_211164243 1730
#define FIX_0_275899380 2260
#define FIX_0_298631336 2446
#define FIX_0_390180644 3196
#define FIX_0_509795579 4176
#define FIX_0_541196100 4433
#define FIX_0_601344887 4926
#define FIX_0_765366865 6270
#define FIX_0_785694958 6436
#define FIX_0_899976223 7373
#define FIX_1_061594337 8697
#define FIX_1_111140466 9102
#define FIX_1_175875602 9633
#define FIX_1_306562965 10703
#define FIX_1_387039845 11363
#define FIX_1_451774981 11893
#define FIX_1_501321110 12299
#define FIX_1_662939225 13623
#define FIX_1_847759065 15137
#define FIX_1_961570560 16069
#define FIX_2_053119869 16819
#define FIX_2_172734803 17799
#define FIX_2_562915447 20995
#define FIX_3_072711026 25172

#undef DCTSIZE
#define DCTSIZE 4
#define DCTSTRIDE 8

//////////////////////////////////lowres_idct16/////////////////////////////

//////////////////////////////////lowres_MV///////////////////////////////
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

extern int h263_lowres;
//////////////////////////////////lowres_MV///////////////////////////////


#define W1 2841                 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676                 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408                 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609                 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108                 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565                  /* 2048*sqrt(2)*cos(7*pi/16) */

/* global declarations */
int init_idct _ANSI_ARGS_ ((void));
void idct _ANSI_ARGS_ ((short *block));
//////////////////////h263_lowres/////////////
void h263_idct16(DCTELEM *data);  //add 2013 4 10
void put_h263_pixels_clamped4_c(uint8_t *pixels,const short *block,
                                int line_size); //add 20130410
void h263_add_pixels_clamped4_c(uint8_t *pixels,const short *block,
                                int line_size) ;

////////////////////////////////////////////////

static short *iclip;       /* clipping table */
/* private data */
//static short iclip[1024];       /* clipping table */ //change shenh
static short *iclp;

/* private prototypes */
static void idctrow _ANSI_ARGS_ ((short *blk));
static void idctcol _ANSI_ARGS_ ((short *blk));

#if 1
/* row (horizontal) IDCT
 *
 * 7                       pi         1 dst[k] = sum c[l] * src[l] * cos( -- *
 * ( k + - ) * l ) l=0                      8          2
 *
 * where: c[0]    = 128 c[1..7] = 128*sqrt(2) */


static void idctrow (short *blk)
{
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;

    /* shortcut */
    if (!((x1 = blk[4] << 11) | (x2 = blk[6]) | (x3 = blk[2]) |
            (x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
    {
        blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
        return;
    }
    x0 = (blk[0] << 11) + 128;    /* for proper rounding in the fourth stage */

    /* first stage */
    x8 = W7 * (x4 + x5);
    x4 = x8 + (W1 - W7) * x4;
    x5 = x8 - (W1 + W7) * x5;
    x8 = W3 * (x6 + x7);
    x6 = x8 - (W3 - W5) * x6;
    x7 = x8 - (W3 + W5) * x7;

    /* second stage */
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6 * (x3 + x2);
    x2 = x1 - (W2 + W6) * x2;
    x3 = x1 + (W2 - W6) * x3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;

    /* third stage */
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181 * (x4 + x5) + 128) >> 8;
    x4 = (181 * (x4 - x5) + 128) >> 8;

    /* fourth stage */
    blk[0] = (x7 + x1) >> 8;
    blk[1] = (x3 + x2) >> 8;
    blk[2] = (x0 + x4) >> 8;
    blk[3] = (x8 + x6) >> 8;
    blk[4] = (x8 - x6) >> 8;
    blk[5] = (x0 - x4) >> 8;
    blk[6] = (x3 - x2) >> 8;
    blk[7] = (x7 - x1) >> 8;
}

/* column (vertical) IDCT
 *
 * 7                         pi         1 dst[8*k] = sum c[l] * src[8*l] *
 * cos( -- * ( k + - ) * l ) l=0                        8          2
 *
 * where: c[0]    = 1/1024 c[1..7] = (1/1024)*sqrt(2) */
static void idctcol (short *blk)
{
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;

    /* shortcut */
    if (!((x1 = (blk[32] << 8)) | (x2 = blk[48]) | (x3 = blk[16]) |
            (x4 = blk[8 ]) | (x5 = blk[56]) | (x6 = blk[40]) | (x7 = blk[24])))
    {
        blk[ 0] = blk[8 ] = blk[16] = blk[24] = blk[32] = blk[40] = blk[48] = blk[56] =
                iclp[(blk[ 0] + 32) >> 6];
        return;
    }
    x0 = (blk[ 0] << 8) + 8192;

    /* first stage */
    x8 = W7 * (x4 + x5) + 4;
    x4 = (x8 + (W1 - W7) * x4) >> 3;
    x5 = (x8 - (W1 + W7) * x5) >> 3;
    x8 = W3 * (x6 + x7) + 4;
    x6 = (x8 - (W3 - W5) * x6) >> 3;
    x7 = (x8 - (W3 + W5) * x7) >> 3;

    /* second stage */
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6 * (x3 + x2) + 4;
    x2 = (x1 - (W2 + W6) * x2) >> 3;
    x3 = (x1 + (W2 - W6) * x3) >> 3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;

    /* third stage */
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181 * (x4 + x5) + 128) >> 8;
    x4 = (181 * (x4 - x5) + 128) >> 8;

    /* fourth stage */
    blk[ 0] = iclp[(x7 + x1) >> 14];
    blk[8 ] = iclp[(x3 + x2) >> 14];
    blk[16] = iclp[(x0 + x4) >> 14];
    blk[24] = iclp[(x8 + x6) >> 14];
    blk[32] = iclp[(x8 - x6) >> 14];
    blk[40] = iclp[(x0 - x4) >> 14];
    blk[48] = iclp[(x3 - x2) >> 14];
    blk[56] = iclp[(x7 - x1) >> 14];
}

/* two dimensional inverse discrete cosine transform */
void idct (short *block)
{
    int i;

    for (i = 0; i < 8; i++)
        idctrow (block + 8 * i);

    for (i = 0; i < 8; i++)
        idctcol (block + i);
    /*
        idctrow (block );
        idctrow (block + 8);
        idctrow (block + 16);
        idctrow (block + 24);
        idctrow (block + 32);
        idctrow (block + 40);
        idctrow (block + 48 );
        idctrow (block + 56);

        idctcol (block );
        idctcol (block + 1);
        idctcol (block + 2);
        idctcol (block + 3);
        idctcol (block + 4);
        idctcol (block + 5);
        idctcol (block + 6);
        idctcol (block + 7);
    */
}
#else

/* two dimensional inverse discrete cosine transform */
void idct (short *block)
{
    /*
      int i;

      for (i = 0; i < 8; i++)
        idctrow (block + 8 * i);

      for (i = 0; i < 8; i++)
        idctcol (block + i);
     */
    int i;
    short *blkIn, *blkOut;
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;
    short *blk;
#if 1
    /* row (horizontal) IDCT
     *
     * 7                       pi         1 dst[k] = sum c[l] * src[l] * cos( -- *
     * ( k + - ) * l ) l=0                      8          2
     *
     * where: c[0]    = 128 c[1..7] = 128*sqrt(2) */
    blkIn=block;
    blkOut=ld->block_temp;
    for(i=0; i<8; i++)
    {
        /* shortcut */
        if (!((x1 = blkIn[4] << 11) | (x2 = blkIn[6]) | (x3 = blkIn[2]) |
                (x4 = blkIn[1]) | (x5 = blkIn[7]) | (x6 = blkIn[5]) | (x7 = blkIn[3])))
        {
            blkOut[0] =blkOut[8] = blkOut[16] = blkOut[24] =blkOut[32] = blkOut[40] = blkOut[48] = blkOut[56] = blkIn[0] << 3;

            blkIn+=8;
            blkOut++;
            continue;
        }
        x0 = (blkIn[0] << 11) + 128;    /* for proper rounding in the fourth stage */

        /* first stage */
        x8 = W7 * (x4 + x5);
        x4 = x8 + (W1 - W7) * x4;
        x5 = x8 - (W1 + W7) * x5;
        x8 = W3 * (x6 + x7);
        x6 = x8 - (W3 - W5) * x6;
        x7 = x8 - (W3 + W5) * x7;

        /* second stage */
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2);
        x2 = x1 - (W2 + W6) * x2;
        x3 = x1 + (W2 - W6) * x3;
        x1 = x4 + x6;
        x4 -= x6;
        x6 = x5 + x7;
        x5 -= x7;

        /* third stage */
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        x2 = (181 * (x4 + x5) + 128) >> 8;
        x4 = (181 * (x4 - x5) + 128) >> 8;

        /* fourth stage */
        blkOut[0] = (x7 + x1) >> 8;
        blkOut[8] = (x3 + x2) >> 8;
        blkOut[16] = (x0 + x4) >> 8;
        blkOut[24] = (x8 + x6) >> 8;
        blkOut[32] = (x8 - x6) >> 8;
        blkOut[40] = (x0 - x4) >> 8;
        blkOut[48] = (x3 - x2) >> 8;
        blkOut[56] = (x7 - x1) >> 8;

        blkIn+=8;
        blkOut++;
    }

    /* column (vertical) IDCT
     *
     * 7                         pi         1 dst[8*k] = sum c[l] * src[8*l] *
     * cos( -- * ( k + - ) * l ) l=0                        8          2
     *
     * where: c[0]    = 1/1024 c[1..7] = (1/1024)*sqrt(2) */
    blkIn=ld->block_temp;
    blkOut=block;
    for(i=0; i<8; i++)
    {
        /* shortcut */
        if (!((x1 = (blkIn[ 4] << 8)) | (x2 = blkIn[ 6]) | (x3 = blkIn[ 2]) |
                (x4 = blkIn[ 1]) | (x5 = blkIn[ 7]) | (x6 = blkIn[ 5]) | (x7 = blkIn[ 3])))
        {
            blkOut[ 0] = blkOut[ 8] = blkOut[ 16] = blkOut[ 24] = blkOut[ 32] = blkOut[ 40] = blkOut[ 48] = blkOut[ 56] =
                    iclp[(blkIn[0] + 32) >> 6];

            blkIn+=8;
            blkOut++;
            continue;
        }
        x0 = (blkIn[ 0] << 8) + 8192;

        /* first stage */
        x8 = W7 * (x4 + x5) + 4;
        x4 = (x8 + (W1 - W7) * x4) >> 3;
        x5 = (x8 - (W1 + W7) * x5) >> 3;
        x8 = W3 * (x6 + x7) + 4;
        x6 = (x8 - (W3 - W5) * x6) >> 3;
        x7 = (x8 - (W3 + W5) * x7) >> 3;

        /* second stage */
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2) + 4;
        x2 = (x1 - (W2 + W6) * x2) >> 3;
        x3 = (x1 + (W2 - W6) * x3) >> 3;
        x1 = x4 + x6;
        x4 -= x6;
        x6 = x5 + x7;
        x5 -= x7;

        /* third stage */
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        x2 = (181 * (x4 + x5) + 128) >> 8;
        x4 = (181 * (x4 - x5) + 128) >> 8;

        /* fourth stage */
        blkOut[ 0] = iclp[(x7 + x1) >> 14];
        blkOut[ 8] = iclp[(x3 + x2) >> 14];
        blkOut[ 16] = iclp[(x0 + x4) >> 14];
        blkOut[ 24] = iclp[(x8 + x6) >> 14];
        blkOut[ 32] = iclp[(x8 - x6) >> 14];
        blkOut[ 40] = iclp[(x0 - x4) >> 14];
        blkOut[ 48] = iclp[(x3 - x2) >> 14];
        blkOut[ 56] = iclp[(x7 - x1) >> 14];

        blkIn+=8;
        blkOut++;
    }
#else
    /* row (horizontal) IDCT
     *
     * 7                       pi         1 dst[k] = sum c[l] * src[l] * cos( -- *
     * ( k + - ) * l ) l=0                      8          2
     *
     * where: c[0]    = 128 c[1..7] = 128*sqrt(2) */
    for(i=0; i<8; i++)
    {
        blk=block+(i<<3);
        /* shortcut */
        if (!((x1 = blk[4] << 11) | (x2 = blk[6]) | (x3 = blk[2]) |
                (x4 = blk[1]) | (x5 = blk[7]) | (x6 = blk[5]) | (x7 = blk[3])))
        {
            blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
            continue;
        }
        x0 = (blk[0] << 11) + 128;    /* for proper rounding in the fourth stage */

        /* first stage */
        x8 = W7 * (x4 + x5);
        x4 = x8 + (W1 - W7) * x4;
        x5 = x8 - (W1 + W7) * x5;
        x8 = W3 * (x6 + x7);
        x6 = x8 - (W3 - W5) * x6;
        x7 = x8 - (W3 + W5) * x7;

        /* second stage */
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2);
        x2 = x1 - (W2 + W6) * x2;
        x3 = x1 + (W2 - W6) * x3;
        x1 = x4 + x6;
        x4 -= x6;
        x6 = x5 + x7;
        x5 -= x7;

        /* third stage */
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        x2 = (181 * (x4 + x5) + 128) >> 8;
        x4 = (181 * (x4 - x5) + 128) >> 8;

        /* fourth stage */
        blk[0] = (x7 + x1) >> 8;
        blk[1] = (x3 + x2) >> 8;
        blk[2] = (x0 + x4) >> 8;
        blk[3] = (x8 + x6) >> 8;
        blk[4] = (x8 - x6) >> 8;
        blk[5] = (x0 - x4) >> 8;
        blk[6] = (x3 - x2) >> 8;
        blk[7] = (x7 - x1) >> 8;
    }

    /* column (vertical) IDCT
     *
     * 7                         pi         1 dst[8*k] = sum c[l] * src[8*l] *
     * cos( -- * ( k + - ) * l ) l=0                        8          2
     *
     * where: c[0]    = 1/1024 c[1..7] = (1/1024)*sqrt(2) */
    for(i=0; i<8; i++)
    {
        blk=block+i;
        /* shortcut */
        if (!((x1 = (blk[32] << 8)) | (x2 = blk[48]) | (x3 = blk[16]) |
                (x4 = blk[8 ]) | (x5 = blk[56]) | (x6 = blk[40]) | (x7 = blk[24])))
        {
            blk[ 0] = blk[8 ] = blk[16] = blk[24] = blk[32] = blk[40] = blk[48] = blk[56] =
                    iclp[(blk[ 0] + 32) >> 6];
            continue;
        }
        x0 = (blk[ 0] << 8) + 8192;

        /* first stage */
        x8 = W7 * (x4 + x5) + 4;
        x4 = (x8 + (W1 - W7) * x4) >> 3;
        x5 = (x8 - (W1 + W7) * x5) >> 3;
        x8 = W3 * (x6 + x7) + 4;
        x6 = (x8 - (W3 - W5) * x6) >> 3;
        x7 = (x8 - (W3 + W5) * x7) >> 3;

        /* second stage */
        x8 = x0 + x1;
        x0 -= x1;
        x1 = W6 * (x3 + x2) + 4;
        x2 = (x1 - (W2 + W6) * x2) >> 3;
        x3 = (x1 + (W2 - W6) * x3) >> 3;
        x1 = x4 + x6;
        x4 -= x6;
        x6 = x5 + x7;
        x5 -= x7;

        /* third stage */
        x7 = x8 + x3;
        x8 -= x3;
        x3 = x0 + x2;
        x0 -= x2;
        x2 = (181 * (x4 + x5) + 128) >> 8;
        x4 = (181 * (x4 - x5) + 128) >> 8;

        /* fourth stage */
        blk[ 0] = iclp[(x7 + x1) >> 14];
        blk[8 ] = iclp[(x3 + x2) >> 14];
        blk[16] = iclp[(x0 + x4) >> 14];
        blk[24] = iclp[(x8 + x6) >> 14];
        blk[32] = iclp[(x8 - x6) >> 14];
        blk[40] = iclp[(x0 - x4) >> 14];
        blk[48] = iclp[(x3 - x2) >> 14];
        blk[56] = iclp[(x7 - x1) >> 14];
    }

#endif
}
#endif

int init_idct ()
{
    int i;
#ifdef USE_INTERNAL_SRAM
    iclip=(short*)IDCTCLIP_START;//mmc_MemMalloc(sizeof(short)*1024);//add shenh
#else
    if((sizeof(short)*1024)>mmc_MemGetFree()-32)
    {

        return -1;
    }
    iclip=(short*)mmc_MemMalloc(sizeof(short)*1024);//add shenh
#endif
    if(!iclip)
    {
        //exit (-1);
        return -1;
    }
    iclp = iclip + 512;
    for (i = -512; i < 512; i++)
        iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
    return 0;
}


////////////////////////lowres////////////////////
void h263_idct16(DCTELEM *data)
{
    int32_t tmp0, tmp1, tmp2, tmp3;
    int32_t tmp10, tmp11, tmp12, tmp13;
    int32_t z1;
    int32_t d0, d2, d4, d6;
    register DCTELEM *dataptr;
    int rowctr;

    /* Pass 1: process rows. */
    /* Note results are scaled up by sqrt(8) compared to a true IDCT; */
    /* furthermore, we scale the results by 2**PASS1_BITS. */

    data[0] += 4;

    dataptr = data;

    for (rowctr = DCTSIZE-1; rowctr >= 0; rowctr--)
    {
        /* Due to quantization, we will usually find that many of the input
         * coefficients are zero, especially the AC terms.  We can exploit this
         * by short-circuiting the IDCT calculation for any row in which all
         * the AC terms are zero.  In that case each output is equal to the
         * DC coefficient (with scale factor as needed).
         * With typical images and quantization tables, half or more of the
         * row DCT calculations can be simplified this way.
         */

        register int *idataptr = (int*)dataptr;

        d0 = dataptr[0];
        d2 = dataptr[1];
        d4 = dataptr[2];
        d6 = dataptr[3];

        if ((d2 | d4 | d6) == 0)
        {
            /* AC terms all zero */
            if (d0)
            {
                /* Compute a 32 bit value to assign. */
                DCTELEM dcval = (DCTELEM) (d0 << PASS1_BITS);
                register int v = (dcval & 0xffff) | ((dcval << 16) & 0xffff0000);

                idataptr[0] = v;
                idataptr[1] = v;
            }

            dataptr += DCTSTRIDE;     /* advance pointer to next row */
            continue;
        }

        /* Even part: reverse the even part of the forward DCT. */
        /* The rotator is sqrt(2)*c(-6). */
        if (d6)
        {
            if (d2)
            {
                /* d0 != 0, d2 != 0, d4 != 0, d6 != 0 */
                z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
                tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
                tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

                tmp0 = (d0 + d4) << CONST_BITS;
                tmp1 = (d0 - d4) << CONST_BITS;

                tmp10 = tmp0 + tmp3;
                tmp13 = tmp0 - tmp3;
                tmp11 = tmp1 + tmp2;
                tmp12 = tmp1 - tmp2;
            }
            else
            {
                /* d0 != 0, d2 == 0, d4 != 0, d6 != 0 */
                tmp2 = MULTIPLY(-d6, FIX_1_306562965);
                tmp3 = MULTIPLY(d6, FIX_0_541196100);

                tmp0 = (d0 + d4) << CONST_BITS;
                tmp1 = (d0 - d4) << CONST_BITS;

                tmp10 = tmp0 + tmp3;
                tmp13 = tmp0 - tmp3;
                tmp11 = tmp1 + tmp2;
                tmp12 = tmp1 - tmp2;
            }
        }
        else
        {
            if (d2)
            {
                /* d0 != 0, d2 != 0, d4 != 0, d6 == 0 */
                tmp2 = MULTIPLY(d2, FIX_0_541196100);
                tmp3 = MULTIPLY(d2, FIX_1_306562965);

                tmp0 = (d0 + d4) << CONST_BITS;
                tmp1 = (d0 - d4) << CONST_BITS;

                tmp10 = tmp0 + tmp3;
                tmp13 = tmp0 - tmp3;
                tmp11 = tmp1 + tmp2;
                tmp12 = tmp1 - tmp2;
            }
            else
            {
                /* d0 != 0, d2 == 0, d4 != 0, d6 == 0 */
                tmp10 = tmp13 = (d0 + d4) << CONST_BITS;
                tmp11 = tmp12 = (d0 - d4) << CONST_BITS;
            }
        }

        /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

        dataptr[0] = (DCTELEM) DESCALE(tmp10, CONST_BITS-PASS1_BITS);
        dataptr[1] = (DCTELEM) DESCALE(tmp11, CONST_BITS-PASS1_BITS);
        dataptr[2] = (DCTELEM) DESCALE(tmp12, CONST_BITS-PASS1_BITS);
        dataptr[3] = (DCTELEM) DESCALE(tmp13, CONST_BITS-PASS1_BITS);

        dataptr += DCTSTRIDE;       /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    /* Note that we must descale the results by a factor of 8 == 2**3, */
    /* and also undo the PASS1_BITS scaling. */

    dataptr = data;
    for (rowctr = DCTSIZE-1; rowctr >= 0; rowctr--)
    {
        /* Columns of zeroes can be exploited in the same way as we did with rows.
         * However, the row calculation has created many nonzero AC terms, so the
         * simplification applies less often (typically 5% to 10% of the time).
         * On machines with very fast multiplication, it's possible that the
         * test takes more time than it's worth.  In that case this section
         * may be commented out.
         */

        d0 = dataptr[DCTSTRIDE*0];
        d2 = dataptr[DCTSTRIDE*1];
        d4 = dataptr[DCTSTRIDE*2];
        d6 = dataptr[DCTSTRIDE*3];

        /* Even part: reverse the even part of the forward DCT. */
        /* The rotator is sqrt(2)*c(-6). */
        if (d6)
        {
            if (d2)
            {
                /* d0 != 0, d2 != 0, d4 != 0, d6 != 0 */
                z1 = MULTIPLY(d2 + d6, FIX_0_541196100);
                tmp2 = z1 + MULTIPLY(-d6, FIX_1_847759065);
                tmp3 = z1 + MULTIPLY(d2, FIX_0_765366865);

                tmp0 = (d0 + d4) << CONST_BITS;
                tmp1 = (d0 - d4) << CONST_BITS;

                tmp10 = tmp0 + tmp3;
                tmp13 = tmp0 - tmp3;
                tmp11 = tmp1 + tmp2;
                tmp12 = tmp1 - tmp2;
            }
            else
            {
                /* d0 != 0, d2 == 0, d4 != 0, d6 != 0 */
                tmp2 = MULTIPLY(-d6, FIX_1_306562965);
                tmp3 = MULTIPLY(d6, FIX_0_541196100);

                tmp0 = (d0 + d4) << CONST_BITS;
                tmp1 = (d0 - d4) << CONST_BITS;

                tmp10 = tmp0 + tmp3;
                tmp13 = tmp0 - tmp3;
                tmp11 = tmp1 + tmp2;
                tmp12 = tmp1 - tmp2;
            }
        }
        else
        {
            if (d2)
            {
                /* d0 != 0, d2 != 0, d4 != 0, d6 == 0 */
                tmp2 = MULTIPLY(d2, FIX_0_541196100);
                tmp3 = MULTIPLY(d2, FIX_1_306562965);

                tmp0 = (d0 + d4) << CONST_BITS;
                tmp1 = (d0 - d4) << CONST_BITS;

                tmp10 = tmp0 + tmp3;
                tmp13 = tmp0 - tmp3;
                tmp11 = tmp1 + tmp2;
                tmp12 = tmp1 - tmp2;
            }
            else
            {
                /* d0 != 0, d2 == 0, d4 != 0, d6 == 0 */
                tmp10 = tmp13 = (d0 + d4) << CONST_BITS;
                tmp11 = tmp12 = (d0 - d4) << CONST_BITS;
            }
        }

        /* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

        dataptr[DCTSTRIDE*0] = tmp10 >> (CONST_BITS+PASS1_BITS+3);
        dataptr[DCTSTRIDE*1] = tmp11 >> (CONST_BITS+PASS1_BITS+3);
        dataptr[DCTSTRIDE*2] = tmp12 >> (CONST_BITS+PASS1_BITS+3);
        dataptr[DCTSTRIDE*3] = tmp13 >> (CONST_BITS+PASS1_BITS+3);

        dataptr++;                  /* advance pointer to next column */
    }
}

///////////////////////////////////////////////////

static  uint8_t h263_av_clip_uint8_c(int a)
{
    if (a&(~0xFF)) return (-a)>>31;
    else           return a;
}

#ifndef VOC_DO
void put_h263_pixels_clamped4_c(uint8_t *pixels,const short *block,
                                int line_size)
{
    int i;

    /* read the pixels */
    for(i=0; i<4; i++)
    {
        pixels[0] = h263_av_clip_uint8_c(block[0]);
        pixels[1] = h263_av_clip_uint8_c(block[1]);
        pixels[2] = h263_av_clip_uint8_c(block[2]);
        pixels[3] = h263_av_clip_uint8_c(block[3]);

        pixels += line_size;
        block += 8;
    }
}


void h263_add_pixels_clamped4_c(uint8_t *pixels,const short *block,
                                int line_size)
{
    int i;

    /* read the pixels */
    for(i=0; i<4; i++)
    {
        pixels[0] = h263_av_clip_uint8_c(pixels[0] + block[0]);
        pixels[1] = h263_av_clip_uint8_c(pixels[1] + block[1]);
        pixels[2] = h263_av_clip_uint8_c(pixels[2] + block[2]);
        pixels[3] = h263_av_clip_uint8_c(pixels[3] + block[3]);
        pixels += line_size;
        block += 8;
    }
}


//////////////////////////////////lowres_MV///////////////////////////////
void put_h263_chroma_mc4_8_c(uint8_t *p_dst , uint8_t *p_src , int stride, int h, int x, int y)
{
    uint8_t *dst = (uint8_t*)p_dst;
    uint8_t *src = (uint8_t*)p_src;
    const int A=(8-x)*(8-y);
    const int B=( x)*(8-y);
    const int C=(8-x)*( y);
    const int D=( x)*( y);
    int i;
    stride >>= sizeof(uint8_t)-1;
    if(D)
    {
        for(i=0; i<h; i++)
        {
            dst[0] = ((((A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1])) + 32)>>6);
            dst[1] = ((((A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2])) + 32)>>6);
            dst[2] = ((((A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3])) + 32)>>6);
            dst[3] = ((((A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4])) + 32)>>6);
            dst+= stride; src+= stride;
        }
    }
    else
    {
        const int E= B+C;
        const int step= C ? stride : 1;
        for(i=0; i<h; i++)
        {
            dst[0] = ((((A*src[0] + E*src[step+0])) + 32)>>6);
            dst[1] = ((((A*src[1] + E*src[step+1])) + 32)>>6);
            dst[2] = ((((A*src[2] + E*src[step+2])) + 32)>>6);
            dst[3] = ((((A*src[3] + E*src[step+3])) + 32)>>6);
            dst+= stride;
            src+= stride;
        }
    }
}


void put_h263_chroma_mc8_8_c(uint8_t *p_dst , uint8_t *p_src , int stride, int h, int x, int y)
{
    uint8_t *dst = (uint8_t*)p_dst;
    uint8_t *src = (uint8_t*)p_src;
    const int A=(8-x)*(8-y);
    const int B=( x)*(8-y);
    const int C=(8-x)*( y);
    const int D=( x)*( y);
    int i;
    stride >>= sizeof(uint8_t)-1;
    if(D)
    {
        for(i=0; i<h; i++)
        {
            dst[0] = ((((A*src[0] + B*src[1] + C*src[stride+0] + D*src[stride+1])) + 32)>>6);
            dst[1] = ((((A*src[1] + B*src[2] + C*src[stride+1] + D*src[stride+2])) + 32)>>6);
            dst[2] = ((((A*src[2] + B*src[3] + C*src[stride+2] + D*src[stride+3])) + 32)>>6);
            dst[3] = ((((A*src[3] + B*src[4] + C*src[stride+3] + D*src[stride+4])) + 32)>>6);
            dst[4] = ((((A*src[4] + B*src[5] + C*src[stride+4] + D*src[stride+5])) + 32)>>6);
            dst[5] = ((((A*src[5] + B*src[6] + C*src[stride+5] + D*src[stride+6])) + 32)>>6);
            dst[6] = ((((A*src[6] + B*src[7] + C*src[stride+6] + D*src[stride+7])) + 32)>>6);
            dst[7] = ((((A*src[7] + B*src[8] + C*src[stride+7] + D*src[stride+8])) + 32)>>6);
            dst+= stride;
            src+= stride;
        }
    }
    else
    {
        const int E= B+C;
        const int step= C ? stride : 1;
        for(i=0; i<h; i++)
        {
            dst[0] = ((((A*src[0] + E*src[step+0])) + 32)>>6);
            dst[1] = ((((A*src[1] + E*src[step+1])) + 32)>>6);
            dst[2] = ((((A*src[2] + E*src[step+2])) + 32)>>6);
            dst[3] = ((((A*src[3] + E*src[step+3])) + 32)>>6);
            dst[4] = ((((A*src[4] + E*src[step+4])) + 32)>>6);
            dst[5] = ((((A*src[5] + E*src[step+5])) + 32)>>6);
            dst[6] = ((((A*src[6] + E*src[step+6])) + 32)>>6);
            dst[7] = ((((A*src[7] + E*src[step+7])) + 32)>>6);
            dst+= stride;
            src+= stride;
        }
    }
}


/* apply one mpeg motion vector to the three components */
void h263_motion_lowres(uint8_t *dest_y,uint8_t *dest_cb,uint8_t *dest_cr,
                        int motion_x, int motion_y,
                        int h,
                        int mb_x,int mb_y)
{

    uint8_t *ptr_y, *ptr_cb, *ptr_cr;
    int mx, my, src_x, src_y, uvsrc_x, uvsrc_y, uvlinesize, linesize, sx, sy,
        uvsx, uvsy;
    int field_based=0;
    int bottom_field=0;
    int field_select=0;
    const int op_index   = 1;//FFMIN(lowres-1+s->chroma_x_shift, 2);
    const int block_s    = 4;//8>>lowres;
    const int s_mask     = 3;//(2 << lowres) - 1;
    //  const int h_edge_pos =dec->mb_width*8; //s->h_edge_pos >> lowres;
    //  const int v_edge_pos = dec->mb_height*8; //s->v_edge_pos >> lowres;
    linesize   = coded_picture_width>>1;//  s->current_picture.f.linesize[0] << field_based;
    uvlinesize = chrom_width>>1;       //s->current_picture.f.linesize[1] << field_based;


    sx = motion_x & s_mask;
    sy = motion_y & s_mask;
    src_x = mb_x * 2 * block_s + (motion_x >>( h263_lowres + 1));
    src_y = (mb_y * 2 * block_s >> field_based) + (motion_y >>( h263_lowres + 1));

    {
        uvsx    = ((motion_x >> 1) & s_mask) | (sx & 1);
        uvsy    = ((motion_y >> 1) & s_mask) | (sy & 1);
        uvsrc_x = src_x >> 1;
        uvsrc_y = src_y >> 1;
    }

    ptr_y  = prev_I_P_frame[0] + src_y   * linesize   + src_x;
    ptr_cb = prev_I_P_frame[1] + uvsrc_y * uvlinesize + uvsrc_x;
    ptr_cr = prev_I_P_frame[2] + uvsrc_y * uvlinesize + uvsrc_x;

    sx = (sx << 2) >> h263_lowres;
    sy = (sy << 2) >> h263_lowres;
    put_h263_chroma_mc8_8_c(dest_y, ptr_y, linesize, h, sx, sy);
    {
        uvsx = (uvsx << 2) >> h263_lowres;
        uvsy = (uvsy << 2) >> h263_lowres;
        if (h >>1)
        {
            put_h263_chroma_mc4_8_c(dest_cb, ptr_cb, uvlinesize, h >>1, uvsx, uvsy);
            put_h263_chroma_mc4_8_c(dest_cr, ptr_cr, uvlinesize, h >> 1, uvsx, uvsy);
        }
    }
}



#endif
#undef FIX
#undef CONST_BITS

#endif// MEDIA_VOCVID_SUPPORT
#endif

