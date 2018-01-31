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


#if 0
#include "cs_types.h"

//#define LOOKUPTABLE

#ifdef LOOKUPTABLE
static int*L_tab, *Cr_r_tab, *Cr_g_tab, *Cb_g_tab, *Cb_b_tab;
#endif

short *r_2_pix;
short *g_2_pix;
short *b_2_pix;
static short *r_2_pix_alloc;
static short *g_2_pix_alloc;
static short *b_2_pix_alloc;

/*
公式参考
R = 1.164 * (Y - 16) + (V - 128) * 1.596798
G = 1.164 * (Y - 16) - (U - 128) * 0.391186 - (V - 128) * 0.813360
B = 1.164 * (Y - 16) + (U - 128) * 2.014265

R = (298 * (Y - 16) + (V - 128) * 409)>>8
G = (298 * (Y - 16) - (U - 128) *100 - (V - 128) * 208)>>8
B = (298 * (Y - 16) + (U - 128) * 516)>>8

*/

/* --------------------------------------------------------------
 *
 * InitColor16Dither --
 *
 * To get rid of the multiply and other conversions in color dither, we use a
 * lookup table.
 *
 * Results: None.
 *
 * Side effects: The lookup tables are initialized.
 *
 * -------------------------------------------------------------- */

int InitColorDither ()
{
    /*
        0x80c00000 - 0x80c0ffff  total 64kB SRAM
        0x80c00000- 0x80c068b0 be used 26kB
        0x80c0fdb0 -0x80c0ffff be used 592 bytes
        0x80c068b0-0x80c0fdaf can be use 38143 bytes
    */

    int  i;

#ifdef LOOKUPTABLE
    L_tab = (int *) mmc_MemMalloc(256 * sizeof (int));
    Cr_r_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
    Cr_g_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
    Cb_g_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
    Cb_b_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
#endif

    r_2_pix_alloc = (short *) 0x80c068b0;//mmc_MemMalloc (1024 * sizeof (short));
    g_2_pix_alloc = (short *) 0x80c070b0;//mmc_MemMalloc (1024 * sizeof (short));
    b_2_pix_alloc = (short *) 0x80c078b0;//mmc_MemMalloc (1024 * sizeof (short));

    if (
#ifdef LOOKUPTABLE
        L_tab == NULL ||
        Cr_r_tab == NULL ||
        Cr_g_tab == NULL ||
        Cb_g_tab == NULL ||
        Cb_b_tab == NULL ||
#endif
        r_2_pix_alloc == NULL ||
        g_2_pix_alloc == NULL ||
        b_2_pix_alloc == NULL
    )
    {
        return -1;
    }

#ifdef LOOKUPTABLE
    for (i = 0; i < 256; i++)
    {
        L_tab[i] = (i-16)*298;
        CB =(i-128);
        CR =(i-128);

        Cr_r_tab[i] = 409  * CR;
        Cr_g_tab[i] = -208* CR;
        Cb_g_tab[i] = -100 * CB;
        Cb_b_tab[i] = 516 * CB;

    }
#endif

    /* clip 0-255  rgb to 0-31 0-63 0-31 rgb565 */
    for (i = 0; i < 384; i++)
    {
        r_2_pix_alloc[i + 384] = i >> 3;// 5
        r_2_pix_alloc[i + 384] <<= 11;
        g_2_pix_alloc[i + 384] = i >> 2;// 6
        g_2_pix_alloc[i + 384] <<= 5;
        b_2_pix_alloc[i + 384] = i >> 3;// 5
    }

    /* Spread out the values we have to the rest of the array so that we do
     * not need to check for overflow. */
    for (i = 0; i < 384; i++)
    {
        r_2_pix_alloc[i] = r_2_pix_alloc[384];
        r_2_pix_alloc[i + 640] = r_2_pix_alloc[639];
        g_2_pix_alloc[i] = g_2_pix_alloc[384];
        g_2_pix_alloc[i + 640] = g_2_pix_alloc[639];
        b_2_pix_alloc[i] = b_2_pix_alloc[384];
        b_2_pix_alloc[i + 640] = b_2_pix_alloc[639];
    }

    r_2_pix = r_2_pix_alloc + 384;
    g_2_pix = g_2_pix_alloc + 384;
    b_2_pix = b_2_pix_alloc + 384;

    return 0;

}




/* --------------------------------------------------------------
 *
 * Color16DitherImage --
 *
 * Converts image into 16 bit color.
 *
 * Results: None.
 *
 * Side effects: None.
 *
 * -------------------------------------------------------------- */
#if 1//calculate and part lookup table
void Color16DitherImage (char *InY, char *InU, char *InV, char* Out, short width, short height)
{
    unsigned char *lum = InY;
    unsigned char *lum2;
    unsigned char *cb = InU;
    unsigned char *cr = InV;

    int  CR, CB;

    unsigned short *row1, *row2;
    int R, G, B;

    int x, y;
    int temp1,temp2,temp3,temp4;

    row1 = (unsigned short *) Out;
    row2 = row1 + width ;
    lum2 = lum + width;

    for (y = 0; y < height ; y +=2 )
    {
        for (x = 0; x <width ; x+=2)
        {

            CR = (int)*cr++;
            CB = (int)*cb++;

            temp2=409 * CR - 56992;
            temp3=34784 -100 * CB -208 * CR ;
            temp4=516 * CB - 70688;

            temp1=298*((int)*lum++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row1++ =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            temp1=298*((int)*lum++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row1++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            //second row
            temp1=298*((int)*lum2++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            temp1=298*((int)*lum2++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row2++ =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);


        }
        lum   += width;
        lum2 += width;
        row1 += width;
        row2 += width;
    }
}
#endif


#if 1//calculate and part lookup table. yuv2rgb565 and embed lcd buffer together.
void Color16DitherImageIntoLcdBuf (char *InY, char *InU, char *InV, short InExtrWidth, short* LcdBuf, short LcdBufWidth, short StartX, short StartY, short width, short height)
{
    unsigned char *lum = InY;
    unsigned char *lum2;
    unsigned char *cb = InU;
    unsigned char *cr = InV;
    short rowOffset=(LcdBufWidth<<1)-width;
    short lumOffset=width+(InExtrWidth<<1);
    short uvInExtrWidth=InExtrWidth>>1;

    int  CR, CB;

    unsigned short *row1, *row2;
    int R, G, B;

    int x, y;
    int temp1,temp2,temp3,temp4;

    row1 = LcdBuf +LcdBufWidth*StartY+StartX;
    row2 = row1 + LcdBufWidth ;
    lum2 = lum + width+InExtrWidth;

    for (y = 0; y < height ; y +=2 )
    {
        for (x = 0; x <width ; x+=2)
        {

            CR = (int)*cr++;
            CB = (int)*cb++;

            temp2=409 * CR - 56992;
            temp3=34784 -100 * CB -208 * CR ;
            temp4=516 * CB - 70688;

            temp1=298*((int)*lum++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row1++ =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            temp1=298*((int)*lum++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row1++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            //second row
            temp1=298*((int)*lum2++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            temp1=298*((int)*lum2++);
            R=( temp1 + temp2)>> 8;
            G=( temp1+temp3)>> 8;
            B=( temp1 + temp4)>> 8;
            *row2++ =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);


        }
        lum   += lumOffset;
        lum2 += lumOffset;
        cr+=uvInExtrWidth;
        cb+=uvInExtrWidth;
        row1 += rowOffset;
        row2 += rowOffset;
    }

}
#endif

#if 0// 4if calculate and part lookup table
void Color16DitherImage (char *InY, char *InU, char *InV, char* Out, short width, short height)
{
    unsigned char *lum = InY;
    unsigned char *lum2;
    unsigned char *cb = InU;
    unsigned char *cr = InV;

    int L, CR, CB;
    int cr_r;
    int cr_g;
    int cb_g;
    int cb_b;

    unsigned short *row1, *row2;
    int R, G, B;

    int x, y;
    int temp1,temp2,temp3,temp4;
    int lastL1, lastL2, lastRes1, lastRes2;

    row1 = (unsigned short *) Out;
    row2 = row1 + width ;
    lum2 = lum + width;

    lastL2=-1;
    for (y = 0; y < height ; y +=2 )
    {
        for (x = 0; x <width ; x+=2)
        {

            L=(int)*lum++;
            CR = (int)*cr++;
            CB = (int)*cb++;
#if 1
            temp2=409 * CR - 56992;
            temp3=34784 -100 * CB -208 * CR ;
            temp4=516 * CB - 70688;
#else
#endif
#if 1// if 判断临近值相等省略计算
            if(L==lastL2)
                *row1++=lastRes1=lastRes2;
            else
            {
                temp1=298*L;
                R=( temp1 + temp2)>> 8;
                G=( temp1+temp3)>> 8;
                B=( temp1 + temp4)>> 8;
                *row1++ =lastRes1=(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
            }

            L=lastL2=(int)*lum++;
            if(L==lastL1)
                *row1++=lastRes2=lastRes1;
            else
            {
                temp1=298*L;
                R=( temp1 + temp2)>> 8;
                G=( temp1+temp3)>> 8;
                B=( temp1 + temp4)>> 8;
                *row1++ =lastRes2= (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
            }

            L=(int)*lum2++;
            if(L==lastL1)
                *row2++=lastRes1;
            else
            {
                temp1=298*L;
                R=( temp1 + temp2)>> 8;
                G=( temp1+temp3)>> 8;
                B=( temp1 + temp4)>> 8;
                *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
            }

            L=(int)*lum2++;
            if(L==lastL2)
                *row2++=lastRes2;
            else
            {
                temp1=298*L;
                R=( temp1 + temp2)>> 8;
                G=( temp1+temp3)>> 8;
                B=( temp1 + temp4)>> 8;
                *row2++ =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
            }

#endif


        }
        /* These values are at the start of the next line, (due to the ++'s
         * above),but they need to be at the start of the line after that. */
        lum += width;
        lum2 += width;
        row1 += width;
        row2 += width;
        //cr-=width>>1;
        //cb-=width>>1;
    }
}
#endif

#if 0//full lookup table
void Color16DitherImage (char *InY, char *InU, char *InV, char* Out, short width, short height)
{
    unsigned char *lum = InY;
    unsigned char *lum2;
    unsigned char *cb = InU;
    unsigned char *cr = InV;

    int L, CR, CB;
    int cr_r;
    int cr_g;
    int cb_g;
    int cb_b;

    unsigned short *row1, *row2;
    int R, G, B;

    int x, y;

    row1 = (unsigned short *) Out;
    row2 = row1 + width ;
    lum2 = lum + width;

    for (y = 0; y < height ; y += 2)
    {
        for (x = 0; x <width ; x+=2)
        {

            CR = (int)*cr++;
            CB = (int)*cb++;
            cr_r = Cr_r_tab[CR];
            cr_g = Cr_g_tab[CR];
            cb_g = Cb_g_tab[CB];
            cb_b = Cb_b_tab[CB];

            L = L_tab[ *lum++];

            R = (L + cr_r)>>8;
            G = (L - cb_g - cr_g)>>8;
            B = (L + cb_b)>>8;

            *row1++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);


            L = L_tab[ *lum++];

            R = (L + cr_r)>>8;
            G = (L - cb_g - cr_g)>>8;
            B = (L + cb_b)>>8;

            *row1++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            /* Now, do second row. */

            L = L_tab[ *lum2++];
            R = (L + cr_r)>>8;
            G = (L - cb_g - cr_g)>>8;
            B = (L + cb_b)>>8;

            *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

            L = L_tab[(int) *lum2++];
            R = (L + cr_r)>>8;
            G = (L - cb_g - cr_g)>>8;
            B = (L + cb_b)>>8;

            *row2++ = (r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
        }
        /* These values are at the start of the next line, (due to the ++'s
         * above),but they need to be at the start of the line after that. */
        lum += width;
        lum2 += width;
        row1 += width;
        row2 += width;
    }
}
#endif

#if 0
uint16 yuv420_to_rgb565(int32 y, int32 u, int32 v)
{
    uint16 res;
    int32 r, g , b;
    r=( 298 * y + 409 * v - 56992)>> 8;
    if(r<0) r=0; else if(r>255) r=255;
    g=( 298 * y -100 * u -208 * v + 34784)>> 8;
    if(g<0) g=0; else if(g>255) g=255;
    b=( 298 * y + 516 * u - 70688)>> 8;
    if(b<0) b=0; else if(b>255) b=255;
    res=((( (r) & 0xf8) << 8) | (( (g) & 0xfc) << 3) | (( (b) & 0xf8) >> 3));
    return res;
}
#endif


#endif


