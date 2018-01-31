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




















#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif
#ifdef MP4_3GP_SUPPORT

#ifndef MEDIA_VOCVID_SUPPORT
#include "mpeg4_global.h"
#include "idct.h"
#include "mmc.h"
#include "decoder.h"


////////////////////////////////lowres_idct16///////////////////////
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

extern uint8_t mpeg4_lowres;
//////////////////////////////////lowres_MV///////////////////////////////


#define W1 2841                 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676                 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408                 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609                 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108                 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565                  /* 2048*sqrt(2)*cos(7*pi/16) */

/* private data
 * Initialized by idct_int32_init so it's mostly RO data,
 * doesn't hurt thread safety */
//#ifdef NOACC
static short *iclip;       /* clipping table */
//#else
//const short iclip[1024];      /* clipping table */
// #endif


static short *iclp;

/* function pointer */
//idctFuncPtr idct;

#ifndef VOC_DO

/* two dimensional inverse discrete cosine transform */
void
idct_int32(short *const block)
{
    short *blk;
    long i;
    long X0, X1, X2, X3, X4, X5, X6, X7, X8;

    for (i = 0; i < 8; i++)     /* idct rows */
    {
        blk = block + (i << 3);
        if (!((X1 = blk[4] << 11) | (X2 = blk[6]) | (X3 = blk[2]) | (X4 =blk[1]) |(X5 = blk[7]) | (X6 = blk[5]) | (X7 = blk[3])))
        {
            blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] =blk[7] = blk[0] << 3;
            continue;
        }

        X0 = (blk[0] << 11) + 128;  /* for proper rounding in the fourth stage  */

        /* first stage  */
        X8 = W7 * (X4 + X5);
        X4 = X8 + (W1 - W7) * X4;
        X5 = X8 - (W1 + W7) * X5;
        X8 = W3 * (X6 + X7);
        X6 = X8 - (W3 - W5) * X6;
        X7 = X8 - (W3 + W5) * X7;

        /* second stage  */
        X8 = X0 + X1;
        X0 -= X1;
        X1 = W6 * (X3 + X2);
        X2 = X1 - (W2 + W6) * X2;
        X3 = X1 + (W2 - W6) * X3;
        X1 = X4 + X6;
        X4 -= X6;
        X6 = X5 + X7;
        X5 -= X7;

        /* third stage  */
        X7 = X8 + X3;
        X8 -= X3;
        X3 = X0 + X2;
        X0 -= X2;
        X2 = (181 * (X4 + X5) + 128) >> 8; // mohongfee: here  add 128 means to add 0.5
        X4 = (181 * (X4 - X5) + 128) >> 8; // mohongfee: here  add 128 means to add 0.5

        /* fourth stage  */

        blk[0] = (short) ((X7 + X1) >> 8);
        blk[1] = (short) ((X3 + X2) >> 8);
        blk[2] = (short) ((X0 + X4) >> 8);
        blk[3] = (short) ((X8 + X6) >> 8);
        blk[4] = (short) ((X8 - X6) >> 8);
        blk[5] = (short) ((X0 - X4) >> 8);
        blk[6] = (short) ((X3 - X2) >> 8);
        blk[7] = (short) ((X7 - X1) >> 8);

    }                           /* end for ( i = 0; i < 8; ++i ) IDCT-rows */



    for (i = 0; i < 8; i++)     /* idct columns */
    {
        blk = block + i;
        /* shortcut  */
        if (!((X1 = (blk[8 * 4] << 8)) | (X2 = blk[8 * 6]) | (X3 =blk[8 *2]) | (X4 =blk[8 *1])| (X5 = blk[8 * 7]) | (X6 = blk[8 * 5]) | (X7 = blk[8 * 3])))
        {
            blk[8 * 0] = blk[8 * 1] = blk[8 * 2] = blk[8 * 3] = blk[8 * 4] =blk[8 * 5] = blk[8 * 6] = blk[8 * 7] =iclp[(blk[8 * 0] + 32) >> 6];
            continue;
        }

        X0 = (blk[8 * 0] << 8) + 8192; // mohongfee: here  add 8192 means to add 0.5

        /* first stage  */
        X8 = W7 * (X4 + X5) + 4;
        X4 = (X8 + (W1 - W7) * X4) >> 3;
        X5 = (X8 - (W1 + W7) * X5) >> 3;
        X8 = W3 * (X6 + X7) + 4;
        X6 = (X8 - (W3 - W5) * X6) >> 3;
        X7 = (X8 - (W3 + W5) * X7) >> 3;

        /* second stage  */
        X8 = X0 + X1;
        X0 -= X1;
        X1 = W6 * (X3 + X2) + 4;
        X2 = (X1 - (W2 + W6) * X2) >> 3;
        X3 = (X1 + (W2 - W6) * X3) >> 3;
        X1 = X4 + X6;
        X4 -= X6;
        X6 = X5 + X7;
        X5 -= X7;

        /* third stage  */
        X7 = X8 + X3;
        X8 -= X3;
        X3 = X0 + X2;
        X0 -= X2;
        X2 = (181 * (X4 + X5) + 128) >> 8; // mohongfee: here  add 128 means to add 0.5 ;  181 ==((2^0.5)/2) *256
        X4 = (181 * (X4 - X5) + 128) >> 8;// mohongfee: here  add 128 means to add 0.5 ; 181 ==((2^0.5)/2) *256

        /* fourth stage  */
        blk[8 * 0] = iclp[(X7 + X1) >> 14];
        blk[8 * 1] = iclp[(X3 + X2) >> 14];
        blk[8 * 2] = iclp[(X0 + X4) >> 14];
        blk[8 * 3] = iclp[(X8 + X6) >> 14];
        blk[8 * 4] = iclp[(X8 - X6) >> 14];
        blk[8 * 5] = iclp[(X0 - X4) >> 14];
        blk[8 * 6] = iclp[(X3 - X2) >> 14];
        blk[8 * 7] = iclp[(X7 - X1) >> 14];
    }

}                               /* end function idct_int32(block) */


//#ifdef NOACC

int
idct_int32_init(void)
{
    int i;
    iclip=(short*)mmc_MemMalloc(sizeof(short)*1024);//add shenh
    if(!iclip)
    {
        return -1;
    }

    iclp = (short *)(iclip + 512);
    for (i = -512; i < 512; i++)
        iclp[i] = (i < -256) ? -256 : ((i > 255) ? 255 : i);
    return 0;
}

////////////////////////lowres////////////////////
void mpeg4_idct16(DCTBLOCK data)
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

//////////////////////////////////lowres_MV///////////////////////////////
void put_mpeg4_chroma_mc4_8_c(uint8_t *p_dst , uint8_t *p_src , int stride, int h, int x, int y)
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


void put_mpeg4_chroma_mc8_8_c(uint8_t *p_dst , uint8_t *p_src , int stride, int h, int x, int y)
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


static  int mpeg4_hpel_motion_lowres(DECODER *dec,
                                     uint8_t *dest, uint8_t *src,
                                     int field_based, int field_select,
                                     int src_x, int src_y,
                                     int width, int height, int stride,
                                     int h_edge_pos, int v_edge_pos,
                                     int w, int h,
                                     int motion_x, int motion_y)
{
    const int op_index = 1;//FFMIN(lowres, 2);
    const int s_mask   = 3;//(2 << lowres) - 1;
    int emu = 0;
    int sx, sy;

    sx = motion_x & s_mask;
    sy = motion_y & s_mask;
    src_x += motion_x >> (mpeg4_lowres + 1);
    src_y += motion_y >>( mpeg4_lowres + 1);

    if (src_x>(h_edge_pos-(!!sx)-w))
    {
        src_x = (dec->width>>mpeg4_lowres)-1;
    }
    if (src_y >((v_edge_pos>>field_based)-(!!sy)-h))
    {
        src_y = (dec->height>>mpeg4_lowres)-1;
    }


    src   += src_y * stride + src_x;

    sx = (sx << 2) >> mpeg4_lowres;
    sy = (sy << 2) >> mpeg4_lowres;
    if (field_select)
        src += stride;
    put_mpeg4_chroma_mc4_8_c(dest,src,stride,h,sx,sy);
    return emu;

}

/* apply one mpeg motion vector to the three components */
static  void mpeg4_motion_lowres(DECODER *dec,
                                 uint8_t *dest_y,
                                 uint8_t *dest_cb,
                                 uint8_t *dest_cr,
                                 int field_based,
                                 int bottom_field,
                                 int field_select,
                                 IMAGE *ref_picture,
                                 int motion_x, int motion_y,
                                 int h,
                                 int mb_x,int mb_y)
{

    uint8_t *ptr_y, *ptr_cb, *ptr_cr;
    int mx, my, src_x, src_y, uvsrc_x, uvsrc_y, uvlinesize, linesize, sx, sy,
        uvsx, uvsy;
    const int op_index   = 1;//FFMIN(lowres-1+s->chroma_x_shift, 2);
    const int block_s    = 4;//8>>lowres;
    const int s_mask     = 3;//(2 << lowres) - 1;
    //const int h_edge_pos =dec->mb_width*8; //s->h_edge_pos >> lowres;
    //const int v_edge_pos = dec->mb_height*8; //s->v_edge_pos >> lowres;
    const int h_edge_pos = (dec->width>>mpeg4_lowres) + EDGE_SIZE_LOWRES_1;
    const int v_edge_pos = (dec->height>>mpeg4_lowres) + EDGE_SIZE_LOWRES_1;
    linesize   = dec->stride[0]<<field_based;//  s->current_picture.f.linesize[0] << field_based;
    uvlinesize = dec->stride[4]<<field_based;       //s->current_picture.f.linesize[1] << field_based;


    sx = motion_x & s_mask;
    sy = motion_y & s_mask;
    src_x = mb_x * 2 * block_s + (motion_x >>( mpeg4_lowres + 1));
    src_y = (mb_y * 2 * block_s >> field_based) + (motion_y >>( mpeg4_lowres + 1));

    if (src_x>(h_edge_pos-(!!sx)-2*block_s))
    {
        src_x = (dec->width>>mpeg4_lowres)-1;
    }
    if (src_y >((v_edge_pos>>field_based)-(!!sy)-h))
    {
        src_y = (dec->height>>mpeg4_lowres)-1;
    }


    {
        uvsx    = ((motion_x >> 1) & s_mask) | (sx & 1);
        uvsy    = ((motion_y >> 1) & s_mask) | (sy & 1);
        uvsrc_x = src_x >> 1;
        uvsrc_y = src_y >> 1;
    }

    ptr_y  = ref_picture->y + src_y   * linesize   + src_x;
    ptr_cb = ref_picture->u + uvsrc_y * uvlinesize + uvsrc_x;
    ptr_cr = ref_picture->v + uvsrc_y * uvlinesize + uvsrc_x;

    sx = (sx << 2) >> mpeg4_lowres;
    sy = (sy << 2) >> mpeg4_lowres;
    put_mpeg4_chroma_mc8_8_c(dest_y, ptr_y, linesize, h, sx, sy);
    {
        uvsx = (uvsx << 2) >> mpeg4_lowres;
        uvsy = (uvsy << 2) >> mpeg4_lowres;
        if (h >>1)
        {
            put_mpeg4_chroma_mc4_8_c(dest_cb, ptr_cb, uvlinesize, h >>1, uvsx, uvsy);
            put_mpeg4_chroma_mc4_8_c(dest_cr, ptr_cr, uvlinesize, h >> 1, uvsx, uvsy);
        }
    }
}

static  int ff_mpeg4_round_chroma(int x)
{
    static const uint8_t mpeg4_chroma_roundtab[16] =
    {
        //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
        0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1,
    };
    return mpeg4_chroma_roundtab[x & 0xf] + (x >> 3);
}


static  void mpeg4_chroma_4mv_motion_lowres(DECODER *dec,
        uint8_t *dest_cb, uint8_t *dest_cr,
        IMAGE *ref_picture,
        int16_t x_pos,int16_t y_pos,
        int mx, int my)
{

//    const int lowres     = s->avctx->mpeg4_lowres;
    const int op_index   = 1;//FFMIN(lowres, 2);
    const int block_s    = 4;//8 >> lowres;
    const int s_mask     = 3;//(2 << lowres) - 1;
    //const int h_edge_pos = dec->mb_width*4;//s->h_edge_pos >> lowres + 1;
    //const int v_edge_pos = dec->mb_height*4;//s->v_edge_pos >> lowres + 1;
    const int h_edge_pos = (dec->width>>(mpeg4_lowres+1))+EDGE_SIZE2_LOWRES_1;
    const int v_edge_pos = (dec->height>>(mpeg4_lowres+1))+EDGE_SIZE2_LOWRES_1;
    int emu = 0, src_x, src_y, offset, sx, sy;
    uint8_t *ptr;

    mx = ff_mpeg4_round_chroma(mx);
    my = ff_mpeg4_round_chroma(my);

    sx = mx & s_mask;
    sy = my & s_mask;
    src_x = x_pos * block_s + (mx >> mpeg4_lowres + 1);
    src_y = y_pos * block_s + (my >> mpeg4_lowres + 1);

    if (src_x>(h_edge_pos-(!!sx)-block_s))
    {
        src_x = (dec->width>>(mpeg4_lowres+1))-1;
    }

    if (src_y>(v_edge_pos-(!!sy)-block_s))
    {
        src_y = (dec->height>>(mpeg4_lowres+1))-1;
    }


    offset = src_y * dec->stride[4]+ src_x;
    ptr = ref_picture->u + offset;

    sx = (sx << 2) >> mpeg4_lowres;
    sy = (sy << 2) >> mpeg4_lowres;
    put_mpeg4_chroma_mc4_8_c(dest_cb, ptr, dec->stride[4], block_s, sx, sy);
    ptr = ref_picture->v+ offset;
    put_mpeg4_chroma_mc4_8_c(dest_cr, ptr, dec->stride[5], block_s, sx, sy);
}




void MPV_mpeg4_motion_lowres(DECODER *dec,uint8_t mv_type,
                             uint8_t *dest_y, uint8_t *dest_cb,
                             uint8_t *dest_cr,
                             int16_t x_pos,int16_t y_pos,
                             int dir, IMAGE *ref_picture)
{
    int mx, my;
    int mb_x, mb_y, i;
    const int block_s = 4;

    mb_x = x_pos;
    mb_y = y_pos;
    switch (mv_type)
    {
        case 0:
            mpeg4_motion_lowres(dec, dest_y, dest_cb, dest_cr,
                                0, 0, 0,
                                ref_picture,
                                dec->mvs[0].x, dec->mvs[0].y,
                                2 * block_s,x_pos,y_pos);
            break;

        case 1:
            mx = 0;
            my = 0;
            for (i = 0; i < 4; i++)
            {
                mpeg4_hpel_motion_lowres(dec, dest_y + ((i & 1) + (i >> 1) *
                                                        dec->stride[i]) * block_s,
                                         ref_picture->y, 0, 0,
                                         (2 * mb_x + (i & 1)) * block_s,
                                         (2 * mb_y + (i >> 1)) * block_s,
                                         dec->width, dec->height, dec->stride[i],
                                         (dec->width>>mpeg4_lowres)+EDGE_SIZE_LOWRES_1,(dec->height>>mpeg4_lowres)+EDGE_SIZE_LOWRES_1,
                                         block_s,block_s,
                                         dec->mvs[i].x, dec->mvs[i].y);

                mx += dec->mvs[i].x;
                my += dec->mvs[i].y;
            }

            mpeg4_chroma_4mv_motion_lowres(dec, dest_cb, dest_cr, ref_picture,x_pos,y_pos,
                                           mx, my);
            break;
        default:
            ;//        assert(0);
    }
}

#endif
//////////////////////////////////lowres_MV///////////////////////////////

#undef FIX
#undef CONST_BITS

#endif// MEDIA_VOCVID_SUPPORT
//#endif
#endif

