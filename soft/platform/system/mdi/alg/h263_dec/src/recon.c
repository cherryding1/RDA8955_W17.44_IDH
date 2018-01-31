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










#ifndef MEDIA_VOCVID_SUPPORT
//#include <stdio.h>
//#include <stdlib.h>

#include "h263_config.h"
#include "tmndec.h"
#include "h263_global.h"
#ifdef MP4_3GP_SUPPORT

/* private prototypes */
static void recon_comp _ANSI_ARGS_ ((unsigned char *src, unsigned char *dst,
                                     int lx, int lx2, int w, int h, int x, int y, int dx, int dy, int flag));
//static void recon_comp_obmc _ANSI_ARGS_ ((unsigned char *src, unsigned char *dst, int lx, int lx2, int comp, int w, int h, int x, int y, int newgob1));
static void rec _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
//static void recon_bidir_average _ANSI_ARGS_ ((int bx, int by));//change shenh
static void recc _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
//static void reco _ANSI_ARGS_ ((unsigned char *s, int *d, int lx, int lx2, int addflag, int c, int xa, int xb, int ya, int yb));
static void rech _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
static void rechc _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
//static void recho _ANSI_ARGS_ ((unsigned char *s, int *d, int lx, int lx2, int addflag, int c, int xa, int xb, int ya, int yb));
static void recv _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
static void recvc _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
//static void recvo _ANSI_ARGS_ ((unsigned char *s, int *d, int lx, int lx2, int addflag, int c, int xa, int xb, int ya, int yb));
static void rec4 _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
static void rec4c _ANSI_ARGS_ ((unsigned char *s, unsigned char *d, int lx, int lx2, int h));
//static void rec4o _ANSI_ARGS_ ((unsigned char *s, int *d, int lx, int lx2, int addflag, int c, int xa, int xb, int ya, int yb));
extern int h263_lowres;

void reconstruct (int bx, int by, int P, int bdx, int bdy, int MODB, int newgob1)
{
    int w, h, lx, lx2, dx, dy;//, xp, yp, comp, sum;
    int x, y, mode;//, xvec, yvec;
    //unsigned char *src[3];
    //int store_rtype;
    //int tmp;
    uint8 *pY_Cur, *pU_Cur, *pV_Cur;//add by 20130412
    int x_pos,y_pos;
    errorstate=0;
    //add by 2013041
    if(h263_lowres==1)
    {
        x = (bx >>3) + 1;
        y = (by >>3) + 1;
//      lx = coded_picture_width>>1;
//      lx2 = coded_picture_width>>1;
    }
    else
    {
        x = (bx >>4) + 1;
        y = (by >>4) + 1;
        lx = coded_picture_width;

        /* change shenh
        if (mv_outside_frame)
        {

            lx2 = coded_picture_width + 64;
            src[0] = edgeframe[0];
            src[1] = edgeframe[1];
            src[2] = edgeframe[2];
        } else
        {
            */
        lx2 = coded_picture_width;
    }/////////////////////////////add by 20130412
    /*
    src[0] = prev_I_P_frame[0];
    src[1] = prev_I_P_frame[1];
    src[2] = prev_I_P_frame[2];
    */
    //  }

    // mode = modemap[y][x];
#if 0//sheen
    tmp=y*(MBC+2)+x;
    mode = *(modemap+tmp);
#else
    mode = *(modemap_curline+ x);
#endif
////////////////////////////////////////////
//add lowres   20130412

    //  if (P)
    //  {
    /* P prediction */
    if (!use_4mv)
    {
        /* normal prediction mode */

////////////add h263_lowres////////////////////
//add by 20130412
        if(h263_lowres==1)
        {
            w=8;
            h=8;
            x_pos=bx >>3;
            y_pos=by >>3;
            //dx = MV[0][0][y][x];
            dx=(*(MV_curline+ x));
            //dy = MV[1][0][y][x];
            dy=(*(MV_curline +6*2*(MBC+2)+ x));
            pY_Cur = current_frame[0]+(coded_picture_width>>1)*by+bx;
            pU_Cur = current_frame[1]+(chrom_width>>1)*(by>>1)+(bx>>1);
            pV_Cur = current_frame[2]+(chrom_width>>1)*(by>>1)+(bx>>1);

            h263_motion_lowres(pY_Cur,pU_Cur,pV_Cur,dx, dy, h,x_pos,y_pos);

        }

/////////////////////////////////////
        else
        {
            /* P prediction */
            w = 16;
            h = 16;
#if 0//sheen
            //dx = MV[0][0][y][x];
            dx=*(MV+tmp);
            //dy = MV[1][0][y][x];
            dy=*(MV+6*(MBR+1)*(MBC+2)+tmp);
#else
            //dx = MV[0][0][y][x];
            dx=*(MV_curline+ x);
            //dy = MV[1][0][y][x];
            dy=*(MV_curline +6*2*(MBC+2)+ x);
#endif
            /* Y */
            recon_comp (prev_I_P_frame[0], current_frame[0], lx, lx2, w, h, bx, by, dx, dy, 0);

            lx >>= 1;
            w >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            h >>= 1;
            by >>= 1;
            /* chroma rounding */
            dx = (dx % 4 == 0 ? dx >> 1 : (dx >> 1) | 1);
            dy = (dy % 4 == 0 ? dy >> 1 : (dy >> 1) | 1);

            /* Chroma */
            recon_comp (prev_I_P_frame[1], current_frame[1], lx, lx2, w, h, bx, by, dx, dy, 1);
            recon_comp (prev_I_P_frame[2], current_frame[2], lx, lx2, w, h, bx, by, dx, dy, 2);
        }
    }
    else
    {
#if 0
        w = 8;
        h = 8;
        /* Y */
        /* 4 MV can be used without OBMC in        * deblocking filter mode                  */
        if (overlapping_MC)
        {
            for (comp = 0; comp < 4; comp++)
            {
                xp = bx + ((comp & 1) << 3);
                yp = by + ((comp & 2) << 2);
                recon_comp_obmc (src[0], current_frame[0], lx, lx2, comp, w, h, xp, yp, newgob1);
                if(errorstate)return;
            }
        }
        else
        {
            if (mode == MODE_INTER4V || mode == MODE_INTER4V_Q)
            {
                for (comp = 0; comp < 4; comp++)
                {
                    dx = MV[0][comp + 1][y][x];
                    dy = MV[1][comp + 1][y][x];

                    xp = bx + ((comp & 1) << 3);
                    yp = by + ((comp & 2) << 2);
                    recon_comp (src[0], current_frame[0], lx, lx2, w, h, xp, yp, dx, dy, 0);
                }
            }
            else
            {
                dx = MV[0][0][y][x];
                dy = MV[1][0][y][x];
                recon_comp (src[0], current_frame[0], lx, lx2, w << 1, h << 1, bx, by, dx, dy, 0);
            }
        }
        /* Chroma */
        if (mode == MODE_INTER4V || mode == MODE_INTER4V_Q)
        {
            sum = MV[0][1][y][x] + MV[0][2][y][x] + MV[0][3][y][x] + MV[0][4][y][x];
            dx = sign (sum) * (h263_roundtab[ABS (sum) % 16] + (ABS (sum) / 16) * 2);

            sum = MV[1][1][y][x] + MV[1][2][y][x] + MV[1][3][y][x] + MV[1][4][y][x];
            dy = sign (sum) * (h263_roundtab[ABS (sum) % 16] + (ABS (sum) / 16) * 2);

        }
        else
        {
            dx = MV[0][0][y][x];
            dy = MV[1][0][y][x];
            /* chroma rounding */
            dx = (dx % 4 == 0 ? dx >> 1 : (dx >> 1) | 1);
            dy = (dy % 4 == 0 ? dy >> 1 : (dy >> 1) | 1);
        }
        lx >>= 1;
        bx >>= 1;
        lx2 >>= 1;
        by >>= 1;
        /* Chroma */
        recon_comp (src[1], current_frame[1], lx, lx2, w, h, bx, by, dx, dy, 1);
        recon_comp (src[2], current_frame[2], lx, lx2, w, h, bx, by, dx, dy, 2);
#else
        errorstate=-1;
        return;
#endif
    }

#if 0//shenh     
} else
{
#if 0//change shenh
    store_rtype = rtype;
    rtype = 0;
    /* B forward prediction */
    if (pb_frame == IM_PB_FRAMES && (MODB == PBMODE_CBPB_FRW_PRED || MODB == PBMODE_FRW_PRED))
    {
#if 0//change shenh
        w = 16;
        h = 16;
        dx = bdx;
        dy = bdy;

        /* Y */
        recon_comp (src[0], bframe[0], lx, lx2, w, h, bx, by, dx, dy, 0);

        lx >>= 1;
        w >>= 1;
        bx >>= 1;
        lx2 >>= 1;
        h >>= 1;
        by >>= 1;
        /* chroma rounding */
        dx = (dx % 4 == 0 ? dx >> 1 : (dx >> 1) | 1);
        dy = (dy % 4 == 0 ? dy >> 1 : (dy >> 1) | 1);

        /* Chroma */
        recon_comp (src[1], bframe[1], lx, lx2, w, h, bx, by, dx, dy, 1);
        recon_comp (src[2], bframe[2], lx, lx2, w, h, bx, by, dx, dy, 2);
#endif
        errorstate=-1;
        return;
    }
    else if (pb_frame == IM_PB_FRAMES && (MODB == PBMODE_CBPB_BCKW_PRED || MODB == PBMODE_BCKW_PRED))
    {
#if 0//change shenh
        lx2 = coded_picture_width;
        src[0] = prev_I_P_frame[0];
        src[1] = prev_I_P_frame[1];
        src[2] = prev_I_P_frame[2];

        w = 16;
        h = 16;

        /* Y */
        recon_comp (current_frame[0], bframe[0], lx, lx2, w, h, bx, by, 0, 0, 0);

        lx >>= 1;
        w >>= 1;
        bx >>= 1;
        lx2 >>= 1;
        h >>= 1;
        by >>= 1;

        /* Chroma */
        recon_comp (current_frame[1], bframe[1], lx, lx2, w, h, bx, by, 0, 0, 1);
        recon_comp (current_frame[2], bframe[2], lx, lx2, w, h, bx, by, 0, 0, 2);
#endif
        errorstate=-1;
        return;

    }
    else
    {
        /* B bidir prediction */
        if (pb_frame == IM_PB_FRAMES)
        {
            /*//change shenh
            bdx = 0;
            bdy = 0;
            */
            errorstate=-1;
            return;

        }
        if (use_4mv && (mode == MODE_INTER4V || mode == MODE_INTER4V_Q))
        {
            w = 8;
            h = 8;
            /* Y */
            xvec = yvec = 0;
            for (comp = 0; comp < 4; comp++)
            {
                xvec += dx = (trb) * MV[0][comp + 1][y][x] / trd + bdx;
                yvec += dy = (trb) * MV[1][comp + 1][y][x] / trd + bdy;
                xp = bx + ((comp & 1) << 3);
                yp = by + ((comp & 2) << 2);
                recon_comp (src[0], bframe[0], lx, lx2, w, h, xp, yp, dx, dy, 0);
            }

            /* chroma rounding (table 16/H.263) */
            dx = sign (xvec) * (h263_roundtab[ABS (xvec) % 16] + (ABS (xvec) / 16) * 2);
            dy = sign (yvec) * (h263_roundtab[ABS (yvec) % 16] + (ABS (yvec) / 16) * 2);

            lx >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            by >>= 1;
            /* Chroma */
            recon_comp (src[1], bframe[1], lx, lx2, w, h, bx, by, dx, dy, 1);
            recon_comp (src[2], bframe[2], lx, lx2, w, h, bx, by, dx, dy, 2);
        }
        else
        {
            /* B bidir prediction with 16x16 blocks */
            w = 16;
            h = 16;

            dx = (trb) * MV[0][0][y][x] / trd + bdx;
            dy = (trb) * MV[1][0][y][x] / trd + bdy;
            /* Y */
            recon_comp (src[0], bframe[0], lx, lx2, w, h, bx, by, dx, dy, 0);

            lx >>= 1;
            w >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            h >>= 1;
            by >>= 1;

            xvec = 4 * dx;
            yvec = 4 * dy;

            /* chroma rounding (table 16/H.263) */
            dx = sign (xvec) * (h263_roundtab[ABS (xvec) % 16] + (ABS (xvec) / 16) * 2);
            dy = sign (yvec) * (h263_roundtab[ABS (yvec) % 16] + (ABS (yvec) / 16) * 2);

            /* Chroma */
            recon_comp (src[1], bframe[1], lx, lx2, w, h, bx, by, dx, dy, 1);
            recon_comp (src[2], bframe[2], lx, lx2, w, h, bx, by, dx, dy, 2);
        }
    }
    rtype = store_rtype;
#endif
    errorstate=-1;
    return;
}
#endif

}

/**********************************************************************
 *
 *  Name:        reconstruct_true_B
 *  Description: Reconstructs true B MB
 *
 *  Input:       position, mb prediction type
 *  Returns:
 *  Side effects:
 *
 *  Date: 970831 Author: Michael Gallant --- mikeg@ee.ubc.ca
 *
 ***********************************************************************/
#if 0//change shenh
void reconstruct_true_B (int bx, int by, int true_B_prediction_type)
{
    int w, h, lx, lx2, dxf, dyf, dxb, dyb;
    int x, y, mode, anchorframemode;
    int bx_chroma, by_chroma;
    unsigned char *src_f[3], *src_b[3];
    int sumx = 0, sumy = 0, comp, dx, dy, xp, yp;

    errorstate=0;

    x = bx / 16 + 1;
    y = by / 16 + 1;
    lx = coded_picture_width;
    w = 16;
    h = 16;

    lx2 = coded_picture_width + 64;
    src_f[0] = edgeframe[0];
    src_f[1] = edgeframe[1];
    src_f[2] = edgeframe[2];
    src_b[0] = nextedgeframe[0];
    src_b[1] = nextedgeframe[1];
    src_b[2] = nextedgeframe[2];

    mode = modemap[y][x];
    anchorframemode = anchorframemodemap[y][x];

    switch (true_B_prediction_type)
    {

        case B_DIRECT_PREDICTION:

            if (MODE_INTER4V == anchorframemode || MODE_INTER4V_Q == anchorframemode)
            {
                w = 8;
                h = 8;

                for (comp = 0; comp < 4; comp++)
                {
                    dx = true_B_direct_mode_MV[0][comp + 1][y][x];
                    dy = true_B_direct_mode_MV[1][comp + 1][y][x];

                    dxf = (true_b_trb) * dx / trd;
                    dyf = (true_b_trb) * dy / trd;
                    dxb = (true_b_trb - trd) * dx / trd;
                    dyb = (true_b_trb - trd) * dy / trd;

                    xp = bx + ((comp & 1) << 3);
                    yp = by + ((comp & 2) << 2);

                    /* Y */
                    recon_comp (src_f[0], tmp_f[0], lx, lx2, w, h, xp, yp, dxf, dyf, 0);
                    recon_comp (src_b[0], tmp_b[0], lx, lx2, w, h, xp, yp, dxb, dyb, 0);
                }

                dxf = dyf = dxb = dyb = 0;

                for (comp=0; comp<4; comp++)
                {
                    dx = true_B_direct_mode_MV[0][comp + 1][y][x];
                    dy = true_B_direct_mode_MV[1][comp + 1][y][x];

                    dxf += (true_b_trb) * dx / trd;
                    dyf += (true_b_trb) * dy / trd;
                    dxb += (true_b_trb - trd) * dx / trd;
                    dyb += (true_b_trb - trd) * dy / trd;
                }

                dxf = sign (dxf) * (h263_roundtab[ABS (dxf) % 16] + (ABS (dxf) / 16) * 2);
                dyf = sign (dyf) * (h263_roundtab[ABS (dyf) % 16] + (ABS (dyf) / 16) * 2);
                dxb = sign (dxb) * (h263_roundtab[ABS (dxb) % 16] + (ABS (dxb) / 16) * 2);
                dyb = sign (dyb) * (h263_roundtab[ABS (dyb) % 16] + (ABS (dyb) / 16) * 2);

                lx >>= 1;
                bx >>= 1;
                lx2 >>= 1;
                by >>= 1;

                /* Chroma */
                recon_comp (src_f[1], tmp_f[1], lx, lx2, w, h, bx, by, dxf, dyf, 1);
                recon_comp (src_f[2], tmp_f[2], lx, lx2, w, h, bx, by, dxf, dyf, 2);
                recon_comp (src_b[1], tmp_b[1], lx, lx2, w, h, bx, by, dxb, dyb, 1);
                recon_comp (src_b[2], tmp_b[2], lx, lx2, w, h, bx, by, dxb, dyb, 2);

                bx <<= 1;
                by <<= 1;
            }
            else
            {
                dxf = (true_b_trb) * true_B_direct_mode_MV[0][0][y][x] / trd;
                dyf = (true_b_trb) * true_B_direct_mode_MV[1][0][y][x] / trd;
                dxb = (true_b_trb - trd) * true_B_direct_mode_MV[0][0][y][x] / trd;
                dyb = (true_b_trb - trd) * true_B_direct_mode_MV[1][0][y][x] / trd;

                /* Y */
                recon_comp (src_f[0], tmp_f[0], lx, lx2, w, h, bx, by, dxf, dyf, 0);
                recon_comp (src_b[0], tmp_b[0], lx, lx2, w, h, bx, by, dxb, dyb, 0);

                lx >>= 1;
                w >>= 1;
                bx >>= 1;
                lx2 >>= 1;
                h >>= 1;
                by >>= 1;

                /* Chroma rounding (table 16/H.263) */
                dxf = (dxf % 4 == 0 ? dxf >> 1 : (dxf >> 1) | 1);
                dyf = (dyf % 4 == 0 ? dyf >> 1 : (dyf >> 1) | 1);
                dxb = (dxb % 4 == 0 ? dxb >> 1 : (dxb >> 1) | 1);
                dyb = (dyb % 4 == 0 ? dyb >> 1 : (dyb >> 1) | 1);

                /* Chroma */
                recon_comp (src_f[1], tmp_f[1], lx, lx2, w, h, bx, by, dxf, dyf, 1);
                recon_comp (src_f[2], tmp_f[2], lx, lx2, w, h, bx, by, dxf, dyf, 2);
                recon_comp (src_b[1], tmp_b[1], lx, lx2, w, h, bx, by, dxb, dyb, 1);
                recon_comp (src_b[2], tmp_b[2], lx, lx2, w, h, bx, by, dxb, dyb, 2);

                bx <<= 1;
                by <<= 1;
            }

            /* Average forward and backward prediction. */
            recon_bidir_average (bx, by);

            break;

        case B_FORWARD_PREDICTION:

            w = 16;
            h = 16;
            dxf = MV[0][0][y][x];
            dyf = MV[1][0][y][x];

            /* Y */
            recon_comp (src_f[0], bframe[0], lx, lx2, w, h, bx, by, dxf, dyf, 0);

            lx >>= 1;
            w >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            h >>= 1;
            by >>= 1;

            /* chroma rounding */
            dxf = (dxf % 4 == 0 ? dxf >> 1 : (dxf >> 1) | 1);
            dyf = (dyf % 4 == 0 ? dyf >> 1 : (dyf >> 1) | 1);

            /* Chroma */
            recon_comp (src_f[1], bframe[1], lx, lx2, w, h, bx, by, dxf, dyf, 1);
            recon_comp (src_f[2], bframe[2], lx, lx2, w, h, bx, by, dxf, dyf, 2);

            break;

        case B_BACKWARD_PREDICTION:

            w = 16;
            h = 16;
            dxb = MV[0][5][y][x];
            dyb = MV[1][5][y][x];

            /* Y */
            recon_comp (src_b[0], bframe[0], lx, lx2, w, h, bx, by, dxb, dyb, 0);

            lx >>= 1;
            w >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            h >>= 1;
            by >>= 1;
            /* chroma rounding */
            dxb = (dxb % 4 == 0 ? dxb >> 1 : (dxb >> 1) | 1);
            dyb = (dyb % 4 == 0 ? dyb >> 1 : (dyb >> 1) | 1);

            /* Chroma */
            recon_comp (src_b[1], bframe[1], lx, lx2, w, h, bx, by, dxb, dyb, 1);
            recon_comp (src_b[2], bframe[2], lx, lx2, w, h, bx, by, dxb, dyb, 2);

            break;

        case B_BIDIRECTIONAL_PREDICTION:

            w = 16;
            h = 16;

            dxf = MV[0][0][y][x];
            dyf = MV[1][0][y][x];
            dxb = MV[0][5][y][x];
            dyb = MV[1][5][y][x];

            /* Y */
            recon_comp (src_f[0], tmp_f[0], lx, lx2, w, h, bx, by, dxf, dyf, 0);
            recon_comp (src_b[0], tmp_b[0], lx, lx2, w, h, bx, by, dxb, dyb, 0);

            lx >>= 1;
            w >>= 1;
            lx2 >>= 1;
            h >>= 1;

            bx_chroma = bx >> 1;
            by_chroma = by >> 1;


            dxf = (dxf % 4 == 0 ? dxf >> 1 : (dxf >> 1) | 1);
            dyf = (dyf % 4 == 0 ? dyf >> 1 : (dyf >> 1) | 1);
            dxb = (dxb % 4 == 0 ? dxb >> 1 : (dxb >> 1) | 1);
            dyb = (dyb % 4 == 0 ? dyb >> 1 : (dyb >> 1) | 1);

            /* Chroma */
            recon_comp (src_f[1], tmp_f[1], lx, lx2, w, h, bx_chroma, by_chroma, dxf, dyf, 1);
            recon_comp (src_f[2], tmp_f[2], lx, lx2, w, h, bx_chroma, by_chroma, dxf, dyf, 2);
            recon_comp (src_b[1], tmp_b[1], lx, lx2, w, h, bx_chroma, by_chroma, dxb, dyb, 1);
            recon_comp (src_b[2], tmp_b[2], lx, lx2, w, h, bx_chroma, by_chroma, dxb, dyb, 2);

            /* Average forward and backward prediction. */
            recon_bidir_average (bx, by);

            break;

        default:

            //fprintf (stderr, "Illegal scalable prediction type in reconstruct_true_B (recon.c)\n");
            diag_printf ("Illegal scalable prediction type in reconstruct_true_B (recon.c)\n");
            //exit (-1);
            errorstate=-1;
            return;
            break;

    }
}
#endif

/**********************************************************************
  *
 *  Name:        reconstruct_ei_ep
 *  Description: Reconstructs ei and ep MBs
 *
 *  Input:       position, mb prediction type
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102 Author: Michael Gallant --- mikeg@ee.ubc.ca
 *
 ***********************************************************************/
#if 0//change shenh
void reconstruct_ei_ep (int bx, int by, int ei_ep_prediction_type)
{
    int w, h, lx, lx2, dxf, dyf;
    int x, y, mode;
    int bx_chroma, by_chroma;
    unsigned char *src_fwd[3], *src_up[3], *tmp_fwd[3], *tmp_up[3];

    errorstate=0;

    /* current_refererence frame should be upsampled already if spatial scalbility is in use */

    x = bx / 16 + 1;
    y = by / 16 + 1;
    lx = coded_picture_width;
    w = 16;
    h = 16;

    lx2 = coded_picture_width + 64;
    src_fwd[0] = enhance_edgeframe[enhancement_layer_num-2][0];
    src_fwd[1] = enhance_edgeframe[enhancement_layer_num-2][1];
    src_fwd[2] = enhance_edgeframe[enhancement_layer_num-2][2];
    src_up[0] = curr_reference_frame[0];
    src_up[1] = curr_reference_frame[1];
    src_up[2] = curr_reference_frame[2];

    tmp_fwd[0] = tmp_enhance_fwd[enhancement_layer_num-2][0];
    tmp_fwd[1] = tmp_enhance_fwd[enhancement_layer_num-2][1];
    tmp_fwd[2] = tmp_enhance_fwd[enhancement_layer_num-2][2];
    tmp_up[0] = tmp_enhance_up[enhancement_layer_num-2][0];
    tmp_up[1] = tmp_enhance_up[enhancement_layer_num-2][1];
    tmp_up[2] = tmp_enhance_up[enhancement_layer_num-2][2];

    mode = modemap[y][x];

    switch (ei_ep_prediction_type)
    {

        case EP_FORWARD_PREDICTION:

            w = 16;
            h = 16;
            dxf = MV[0][0][y][x];
            dyf = MV[1][0][y][x];

            /* Y */
            recon_comp (src_fwd[0], current_enhancement_frame[enhancement_layer_num-2][0], lx, lx2, w, h, bx, by, dxf, dyf, 0);

            lx >>= 1;
            w >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            h >>= 1;
            by >>= 1;

            /* chroma rounding */
            dxf = (dxf % 4 == 0 ? dxf >> 1 : (dxf >> 1) | 1);
            dyf = (dyf % 4 == 0 ? dyf >> 1 : (dyf >> 1) | 1);

            /* Chroma */
            recon_comp (src_fwd[1], current_enhancement_frame[enhancement_layer_num-2][1], lx, lx2, w, h, bx, by, dxf, dyf, 1);
            recon_comp (src_fwd[2], current_enhancement_frame[enhancement_layer_num-2][2], lx, lx2, w, h, bx, by, dxf, dyf, 2);

            break;

        case EI_EP_UPWARD_PREDICTION:

            /* Inter 16x16 */
            w = 16;
            h = 16;

            /* Y */
            recon_comp (src_up[0], current_enhancement_frame[enhancement_layer_num-2][0], lx, lx, w, h, bx, by, 0, 0, 0);

            lx >>= 1;
            w >>= 1;
            bx >>= 1;
            lx2 >>= 1;
            h >>= 1;
            by >>= 1;

            /* Chroma */
            recon_comp (src_up[1], current_enhancement_frame[enhancement_layer_num-2][1], lx, lx, w, h, bx, by, 0, 0, 1);
            recon_comp (src_up[2], current_enhancement_frame[enhancement_layer_num-2][2], lx, lx, w, h, bx, by, 0, 0, 2);

            break;

        case EP_BIDIRECTIONAL_PREDICTION:

            w = 16;
            h = 16;

            dxf = MV[0][0][y][x];
            dyf = MV[1][0][y][x];

            /* Y */
            recon_comp (src_fwd[0], tmp_fwd[0], lx, lx2, w, h, bx, by, dxf, dyf, 0);
            recon_comp (src_up[0], tmp_up[0], lx, lx, w, h, bx, by, 0, 0, 0);

            lx >>= 1;
            w >>= 1;
            lx2 >>= 1;
            h >>= 1;

            bx_chroma = bx >> 1;
            by_chroma = by >> 1;


            dxf = (dxf % 4 == 0 ? dxf >> 1 : (dxf >> 1) | 1);
            dyf = (dyf % 4 == 0 ? dyf >> 1 : (dyf >> 1) | 1);

            /* Chroma */
            recon_comp (src_fwd[1], tmp_fwd[1], lx, lx2, w, h, bx_chroma, by_chroma, dxf, dyf, 1);
            recon_comp (src_fwd[2], tmp_fwd[2], lx, lx2, w, h, bx_chroma, by_chroma, dxf, dyf, 2);
            recon_comp (src_up[1], tmp_up[1], lx, lx, w, h, bx_chroma, by_chroma, 0, 0, 1);
            recon_comp (src_up[2], tmp_up[2], lx, lx, w, h, bx_chroma, by_chroma, 0, 0, 2);

            /* Average forward and backward prediction. */
            recon_bidir_average (bx, by);

            break;

        default:

            //fprintf (stderr, "Illegal scalable prediction type in reconstruct_ei_ep (recon.c)\n");
            diag_printf ( "Illegal scalable prediction type in reconstruct_ei_ep (recon.c)\n");
            //exit (-1);
            errorstate=-1;
            return;

            break;

    }
}
#endif

static void recon_comp (unsigned char *src, unsigned char *dst,
                        int lx, int lx2, int w, int h, int x,
                        int y, int dx, int dy, int chroma)
{
    int xint, xh, yint, yh;
    unsigned char *s, *d;
    int store_rtype;

    store_rtype = rtype;

    xint = dx >> 1;
    xh = dx & 1;
    yint = dy >> 1;
    yh = dy & 1;

    /* origins */
    s = src + lx2 * (y + yint) + x + xint;
    d = dst + lx * y + x;

    if (!xh && !yh)//x，y方向整像素位置补偿
        if (w != 8)
            rec (s, d, lx, lx2, h);// 16x16
        else
            recc (s, d, lx, lx2, h);// 8x8
    else if (!xh && yh)//x方向整像素补偿，y方向半像素位置
        if (w != 8)
            recv (s, d, lx, lx2, h);//内插出y方向补偿像素16x16
        else
            recvc (s, d, lx, lx2, h);// 8x8
    else if (xh && !yh)//x方向半像素补偿，y方向整像素位置
        if (w != 8)
            rech (s, d, lx, lx2, h);
        else
            rechc (s, d, lx, lx2, h);
    else//xy方向都在半像素位置
        /* if (xh && yh) */ if (w != 8)
            rec4 (s, d, lx, lx2, h);
        else
            rec4c (s, d, lx, lx2, h);

    rtype = store_rtype;
}

static void rec (unsigned char *s, unsigned char *d,
                 int lx, int lx2, int h)
{
    int j;

    if((((unsigned int)s &3) ==0) && (((unsigned int)d&3) ==0))
    {
        unsigned int *pt0, *pt1;

        for (j = 0; j < h; j++)
        {
            pt0 = (unsigned int *)s;
            pt1 = (unsigned int *)d;

            pt1[0] = pt0[0];
            pt1[1] = pt0[1];
            pt1[2] = pt0[2];
            pt1[3] = pt0[3];

            s += lx2;
            d += lx;
        }
    }
    else
    {
        for (j = 0; j < h; j++)
        {

            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
            d[3] = s[3];
            d[4] = s[4];
            d[5] = s[5];
            d[6] = s[6];
            d[7] = s[7];
            d[8] = s[8];
            d[9] = s[9];
            d[10] = s[10];
            d[11] = s[11];
            d[12] = s[12];
            d[13] = s[13];
            d[14] = s[14];
            d[15] = s[15];

            //memcpy(d, s, 16);
            s += lx2;
            d += lx;
        }
    }
}

/**********************************************************************
 *
 *  Name:        recon_bidir_average
 *  Description: move average of bidir predicted blocks into current frame
 *
 *  Input:       absolute position of current MB
 *
 *  Returns:
 *  Side effects:
 *
 *  Date: 971102 Author: Michael Gallant --- mikeg@ee.ubc.ca
 *
 ***********************************************************************/
#if 0//change shenh
static void recon_bidir_average (int bx, int by)
{
    int i, iincr, cc, comp;
    unsigned char *avg, *forw, *backw;
    unsigned char *prev_frame[3], *next_frame[3], *curr[3];

    /* B picture */
    if (PCT_B == pict_type)
    {
        prev_frame[0] = tmp_f[0];
        prev_frame[1] = tmp_f[1];
        prev_frame[2] = tmp_f[2];
        next_frame[0] = tmp_b[0];
        next_frame[1] = tmp_b[1];
        next_frame[2] = tmp_b[2];
        curr[0] = current_frame[0];
        curr[1] = current_frame[1];
        curr[2] = current_frame[2];
    }
    /* EP picture */
    else
    {
        prev_frame[0] = tmp_enhance_fwd[enhancement_layer_num-2][0];
        prev_frame[1] = tmp_enhance_fwd[enhancement_layer_num-2][1];
        prev_frame[2] = tmp_enhance_fwd[enhancement_layer_num-2][2];
        next_frame[0] = tmp_enhance_up[enhancement_layer_num-2][0];
        next_frame[1] = tmp_enhance_up[enhancement_layer_num-2][1];
        next_frame[2] = tmp_enhance_up[enhancement_layer_num-2][2];
        curr[0] = current_enhancement_frame[enhancement_layer_num-2][0];
        curr[1] = current_enhancement_frame[enhancement_layer_num-2][1];
        curr[2] = current_enhancement_frame[enhancement_layer_num-2][2];
    }

    for (comp = 0; comp < 6; ++comp)
    {
        cc = (comp < 4) ? 0 : (comp & 1) + 1; /* color component index */

        /* luminance */
        if (cc == 0)
        {
            avg = curr[cc] + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
            forw = prev_frame[cc] + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
            backw = next_frame[cc] + coded_picture_width * (by + ((comp & 2) << 2)) + bx + ((comp & 1) << 3);
            iincr = coded_picture_width;
        }
        /* chrominance */
        else
        {
            /* scale coordinates */
            if (4 == comp)
            {
                bx >>= 1;
                by >>= 1;
            }
            avg = curr[cc] + chrom_width * by + bx;
            forw = prev_frame[cc] + chrom_width * by + bx;
            backw = next_frame[cc] + chrom_width * by + bx;
            iincr = chrom_width;
        }

        for (i = 0; i < 8; i++)
        {
            avg[0] = (forw[0] + backw[0]) / 2;
            avg[1] = (forw[1] + backw[1]) / 2;
            avg[2] = (forw[2] + backw[2]) / 2;
            avg[3] = (forw[3] + backw[3]) / 2;
            avg[4] = (forw[4] + backw[4]) / 2;
            avg[5] = (forw[5] + backw[5]) / 2;
            avg[6] = (forw[6] + backw[6]) / 2;
            avg[7] = (forw[7] + backw[7]) / 2;
            avg += iincr;
            forw += iincr;
            backw += iincr;
        }
    }
}
#endif


static void recc (unsigned char *s, unsigned char *d,
                  int lx, int lx2, int h)
{
    int j;

    if((((unsigned int)s&3) ==0) && (((unsigned int)d&3) ==0))
    {
        unsigned int *pt0, *pt1;

        for (j = 0; j < h; j++)
        {
            pt0 = (unsigned int *)s;
            pt1 = (unsigned int *)d;

            pt1[0] = pt0[0];
            pt1[1] = pt0[1];

            s += lx2;
            d += lx;
        }
    }
    else
    {
        for (j = 0; j < h; j++)
        {
            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
            d[3] = s[3];
            d[4] = s[4];
            d[5] = s[5];
            d[6] = s[6];
            d[7] = s[7];

            //memcpy(d, s, 8);
            s += lx2;
            d += lx;
        }
    }
}

#if 0
static void reco (unsigned char *s, int *d, int lx, int lx2,
                  int addflag, int c, int xa, int xb,
                  int ya, int yb)
{
    int i, j;
    int *om;

    om = &OM[c][ya][0];

    if (!addflag)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                d[i] = s[i] * om[i];
            }
            s += lx2;
            d += lx;
            om += 8;
        }
    }
    else
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                d[i] += s[i] * om[i];
            }
            s += lx2;
            d += lx;
            om += 8;
        }
    }
}
#endif

static void rech (unsigned char *s, unsigned char *d,
                  int lx, int lx2, int h)
{
    unsigned char *dp, *sp;
    int j;
    unsigned int s1, s2;
    int tmp;

    sp = s;
    dp = d;
    tmp=1 - rtype;
    for (j = 0; j < h; j++)
    {
        s1 = sp[0];
        dp[0] = (unsigned int) (s1 + (s2 = sp[1]) + tmp) >> 1;
        dp[1] = (unsigned int) (s2 + (s1 = sp[2]) + tmp) >> 1;
        dp[2] = (unsigned int) (s1 + (s2 = sp[3]) + tmp) >> 1;
        dp[3] = (unsigned int) (s2 + (s1 = sp[4]) + tmp) >> 1;
        dp[4] = (unsigned int) (s1 + (s2 = sp[5]) + tmp) >> 1;
        dp[5] = (unsigned int) (s2 + (s1 = sp[6]) + tmp) >> 1;
        dp[6] = (unsigned int) (s1 + (s2 = sp[7]) + tmp) >> 1;
        dp[7] = (unsigned int) (s2 + (s1 = sp[8]) + tmp) >> 1;
        dp[8] = (unsigned int) (s1 + (s2 = sp[9]) + tmp) >> 1;
        dp[9] = (unsigned int) (s2 + (s1 = sp[10]) + tmp) >> 1;
        dp[10] = (unsigned int) (s1 + (s2 = sp[11]) + tmp) >> 1;
        dp[11] = (unsigned int) (s2 + (s1 = sp[12]) + tmp) >> 1;
        dp[12] = (unsigned int) (s1 + (s2 = sp[13]) + tmp) >> 1;
        dp[13] = (unsigned int) (s2 + (s1 = sp[14]) + tmp) >> 1;
        dp[14] = (unsigned int) (s1 + (s2 = sp[15]) + tmp) >> 1;
        dp[15] = (unsigned int) (s2 + sp[16] + tmp) >> 1;
        sp += lx2;
        dp += lx;
    }
}

static void rechc (unsigned char *s, unsigned char *d,
                   int lx, int lx2, int h)
{
    unsigned char *dp, *sp;
    int j;
    unsigned int s1, s2;
    int tmp;

    sp = s;
    dp = d;
    tmp=1 - rtype;
    for (j = 0; j < h; j++)
    {
        s1 = sp[0];
        dp[0] = (unsigned int) (s1 + (s2 = sp[1]) + tmp) >> 1;
        dp[1] = (unsigned int) (s2 + (s1 = sp[2]) + tmp) >> 1;
        dp[2] = (unsigned int) (s1 + (s2 = sp[3]) + tmp) >> 1;
        dp[3] = (unsigned int) (s2 + (s1 = sp[4]) + tmp) >> 1;
        dp[4] = (unsigned int) (s1 + (s2 = sp[5]) + tmp) >> 1;
        dp[5] = (unsigned int) (s2 + (s1 = sp[6]) + tmp) >> 1;
        dp[6] = (unsigned int) (s1 + (s2 = sp[7]) + tmp) >> 1;
        dp[7] = (unsigned int) (s2 + sp[8] + tmp) >> 1;
        sp += lx2;
        dp += lx;
    }
}

#if 0
static void recho (unsigned char *s, int *d, int lx, int lx2,
                   int addflag, int c, int xa, int xb, int ya, int yb)
{
    int *dp, *om;
    unsigned char *sp;
    int i, j;

    sp = s;
    dp = d;
    om = &OM[c][ya][0];

    if (!addflag)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                dp[i] = (((unsigned int) (sp[i] + sp[i + 1] + 1 - rtype)) >> 1) * om[i];
            }
            sp += lx2;
            dp += lx;
            om += 8;
        }
    }
    else
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                dp[i] += (((unsigned int) (sp[i] + sp[i + 1] + 1 - rtype)) >> 1) * OM[c][j][i];
            }
            sp += lx2;
            dp += lx;
            om += 8;
        }
    }
}
#endif

static void recv (unsigned char *s, unsigned char *d,
                  int lx, int lx2, int h)
{
    unsigned char *dp, *sp, *sp2;
    int j;
    int tmp;

    sp = s;
    sp2 = s + lx2;
    dp = d;
    tmp=1-rtype;
    for (j = 0; j < h; j++)
    {
        dp[0] = (unsigned int) (sp[0] + sp2[0] + tmp) >> 1;
        dp[1] = (unsigned int) (sp[1] + sp2[1] + tmp) >> 1;
        dp[2] = (unsigned int) (sp[2] + sp2[2] + tmp) >> 1;
        dp[3] = (unsigned int) (sp[3] + sp2[3] + tmp) >> 1;
        dp[4] = (unsigned int) (sp[4] + sp2[4] + tmp) >> 1;
        dp[5] = (unsigned int) (sp[5] + sp2[5] + tmp) >> 1;
        dp[6] = (unsigned int) (sp[6] + sp2[6] + tmp) >> 1;
        dp[7] = (unsigned int) (sp[7] + sp2[7] + tmp) >> 1;
        dp[8] = (unsigned int) (sp[8] + sp2[8] + tmp) >> 1;
        dp[9] = (unsigned int) (sp[9] + sp2[9] + tmp) >> 1;
        dp[10] = (unsigned int) (sp[10] + sp2[10] + tmp) >> 1;
        dp[11] = (unsigned int) (sp[11] + sp2[11] + tmp) >> 1;
        dp[12] = (unsigned int) (sp[12] + sp2[12] + tmp) >> 1;
        dp[13] = (unsigned int) (sp[13] + sp2[13] + tmp) >> 1;
        dp[14] = (unsigned int) (sp[14] + sp2[14] + tmp) >> 1;
        dp[15] = (unsigned int) (sp[15] + sp2[15] + tmp) >> 1;
        sp += lx2;
        sp2 += lx2;
        dp += lx;
    }
}

static void recvc (unsigned char *s, unsigned char *d,
                   int lx, int lx2, int h)
{
    unsigned char *dp, *sp, *sp2;
    int j;
    int tmp;

    sp = s;
    sp2 = s + lx2;
    dp = d;
    tmp=1 - rtype;
    for (j = 0; j < h; j++)
    {
        dp[0] = (unsigned int) (sp[0] + sp2[0] + tmp) >> 1;
        dp[1] = (unsigned int) (sp[1] + sp2[1] + tmp) >> 1;
        dp[2] = (unsigned int) (sp[2] + sp2[2] + tmp) >> 1;
        dp[3] = (unsigned int) (sp[3] + sp2[3] + tmp) >> 1;
        dp[4] = (unsigned int) (sp[4] + sp2[4] + tmp) >> 1;
        dp[5] = (unsigned int) (sp[5] + sp2[5] + tmp) >> 1;
        dp[6] = (unsigned int) (sp[6] + sp2[6] + tmp) >> 1;
        dp[7] = (unsigned int) (sp[7] + sp2[7] + tmp) >> 1;
        sp += lx2;
        sp2 += lx2;
        dp += lx;
    }
}

#if 0
static void recvo (unsigned char *s, int *d, int lx, int lx2,
                   int addflag, int c, int xa, int xb, int ya,
                   int yb)
{
    int *dp, *om;
    unsigned char *sp, *sp2;
    int i, j;

    sp = s;
    sp2 = s + lx2;
    dp = d;
    om = &OM[c][ya][0];

    if (!addflag)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                dp[i] = (((unsigned int) (sp[i] + sp2[i] + 1 - rtype)) >> 1) * om[i];
            }
            sp += lx2;
            sp2 += lx2;
            dp += lx;
            om += 8;
        }
    }
    else
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                dp[i] += (((unsigned int) (sp[i] + sp2[i] + 1 - rtype)) >> 1) * om[i];
            }
            sp += lx2;
            sp2 += lx2;
            dp += lx;
            om += 8;
        }
    }
}
#endif

static void rec4 (unsigned char *s, unsigned char *d,
                  int lx, int lx2, int h)
{
    unsigned char *dp, *sp, *sp2;
    int j;
    unsigned int s1, s2, s3, s4;
    int tmp;

    sp = s;
    sp2 = s + lx2;
    dp = d;
    tmp=2 - rtype;
    for (j = 0; j < h; j++)
    {
        s1 = sp[0];
        s3 = sp2[0];
        dp[0] = (unsigned int) (s1 + (s2 = sp[1]) + s3 + (s4 = sp2[1]) + tmp) >> 2;
        dp[1] = (unsigned int) (s2 + (s1 = sp[2]) + s4 + (s3 = sp2[2]) + tmp) >> 2;
        dp[2] = (unsigned int) (s1 + (s2 = sp[3]) + s3 + (s4 = sp2[3]) + tmp) >> 2;
        dp[3] = (unsigned int) (s2 + (s1 = sp[4]) + s4 + (s3 = sp2[4]) + tmp) >> 2;
        dp[4] = (unsigned int) (s1 + (s2 = sp[5]) + s3 + (s4 = sp2[5]) + tmp) >> 2;
        dp[5] = (unsigned int) (s2 + (s1 = sp[6]) + s4 + (s3 = sp2[6]) + tmp) >> 2;
        dp[6] = (unsigned int) (s1 + (s2 = sp[7]) + s3 + (s4 = sp2[7]) + tmp) >> 2;
        dp[7] = (unsigned int) (s2 + (s1 = sp[8]) + s4 + (s3 = sp2[8]) + tmp) >> 2;
        dp[8] = (unsigned int) (s1 + (s2 = sp[9]) + s3 + (s4 = sp2[9]) + tmp) >> 2;
        dp[9] = (unsigned int) (s2 + (s1 = sp[10]) + s4 + (s3 = sp2[10]) + tmp) >> 2;
        dp[10] = (unsigned int) (s1 + (s2 = sp[11]) + s3 + (s4 = sp2[11]) + tmp) >> 2;
        dp[11] = (unsigned int) (s2 + (s1 = sp[12]) + s4 + (s3 = sp2[12]) + tmp) >> 2;
        dp[12] = (unsigned int) (s1 + (s2 = sp[13]) + s3 + (s4 = sp2[13]) + tmp) >> 2;
        dp[13] = (unsigned int) (s2 + (s1 = sp[14]) + s4 + (s3 = sp2[14]) + tmp) >> 2;
        dp[14] = (unsigned int) (s1 + (s2 = sp[15]) + s3 + (s4 = sp2[15]) + tmp) >> 2;
        dp[15] = (unsigned int) (s2 + sp[16] + s4 + sp2[16] + tmp) >> 2;
        sp += lx2;
        sp2 += lx2;
        dp += lx;
    }
}

static void rec4c (unsigned char *s, unsigned char *d,
                   int lx, int lx2, int h)
{
    unsigned char *dp, *sp, *sp2;
    int j;
    unsigned int s1, s2, s3, s4;
    int tmp;

    sp = s;
    sp2 = s + lx2;
    dp = d;
    tmp=2 - rtype;
    for (j = 0; j < h; j++)
    {
        s1 = sp[0];
        s3 = sp2[0];
        dp[0] = (unsigned int) (s1 + (s2 = sp[1]) + s3 + (s4 = sp2[1]) + tmp) >> 2;
        dp[1] = (unsigned int) (s2 + (s1 = sp[2]) + s4 + (s3 = sp2[2]) + tmp) >> 2;
        dp[2] = (unsigned int) (s1 + (s2 = sp[3]) + s3 + (s4 = sp2[3]) + tmp) >> 2;
        dp[3] = (unsigned int) (s2 + (s1 = sp[4]) + s4 + (s3 = sp2[4]) + tmp) >> 2;
        dp[4] = (unsigned int) (s1 + (s2 = sp[5]) + s3 + (s4 = sp2[5]) + tmp) >> 2;
        dp[5] = (unsigned int) (s2 + (s1 = sp[6]) + s4 + (s3 = sp2[6]) + tmp) >> 2;
        dp[6] = (unsigned int) (s1 + (s2 = sp[7]) + s3 + (s4 = sp2[7]) + tmp) >> 2;
        dp[7] = (unsigned int) (s2 + sp[8] + s4 + sp2[8] + tmp) >> 2;
        sp += lx2;
        sp2 += lx2;
        dp += lx;
    }
}

#if 0
static void rec4o (unsigned char *s, int *d, int lx, int lx2,
                   int addflag, int c, int xa, int xb, int ya,
                   int yb)
{
    int *dp, *om;
    unsigned char *sp, *sp2;
    int i, j;

    sp = s;
    sp2 = s + lx2;
    dp = d;
    om = &OM[c][ya][0];

    if (!addflag)
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                dp[i] = (((unsigned int) (sp[i] + sp2[i] + sp[i + 1] + sp2[i + 1] + 2 - rtype)) >> 2) * om[i];
            }
            sp += lx2;
            sp2 += lx2;
            dp += lx;
            om += 8;
        }
    }
    else
    {
        for (j = ya; j < yb; j++)
        {
            for (i = xa; i < xb; i++)
            {
                dp[i] += (((unsigned int) (sp[i] + sp2[i] + sp[i + 1] + sp2[i + 1] + 2 - rtype)) >> 2) * om[i];
            }
            sp += lx2;
            sp2 += lx2;
            dp += lx;
            om += 8;
        }
    }
}
#endif

#if 0
static void recon_comp_obmc (unsigned char *src, unsigned char *dst,
                             int lx, int lx2, int comp, int w, int h,
                             int x, int y, int newgob1)
{
    int j, k;
    int xmb, ymb;
    int c8, t8, l8, r8;
    int ti8, li8, ri8;
    int xit, xib, xir, xil;
    int yit, yib, yir, yil;
    int vect, vecb, vecr, vecl;
    int nx[5], ny[5], xint[5], yint[5], xh[5], yh[5];
    int p[64], *pd;
    unsigned char *d, *s[5];

    xmb = (x >> 4) + 1;
    ymb = (y >> 4) + 1;

    c8 = (modemap[ymb][xmb] == MODE_INTER4V ? 1 : 0);
    c8 = (modemap[ymb][xmb] == MODE_INTER4V_Q ? 1 : c8);

    t8 = (modemap[ymb - 1][xmb] == MODE_INTER4V ? 1 : 0);
    t8 = (modemap[ymb - 1][xmb] == MODE_INTER4V_Q ? 1 : t8);
    ti8 = (modemap[ymb - 1][xmb] == MODE_INTRA ? 1 : 0);
    ti8 = (modemap[ymb - 1][xmb] == MODE_INTRA_Q ? 1 : ti8);

    l8 = (modemap[ymb][xmb - 1] == MODE_INTER4V ? 1 : 0);
    l8 = (modemap[ymb][xmb - 1] == MODE_INTER4V_Q ? 1 : l8);
    li8 = (modemap[ymb][xmb - 1] == MODE_INTRA ? 1 : 0);
    li8 = (modemap[ymb][xmb - 1] == MODE_INTRA_Q ? 1 : li8);

    r8 = (modemap[ymb][xmb + 1] == MODE_INTER4V ? 1 : 0);
    r8 = (modemap[ymb][xmb + 1] == MODE_INTER4V_Q ? 1 : r8);
    ri8 = (modemap[ymb][xmb + 1] == MODE_INTRA ? 1 : 0);
    ri8 = (modemap[ymb][xmb + 1] == MODE_INTRA_Q ? 1 : ri8);

    /*
      if (pb_frame)
      {
       // ti8 = li8 = ri8 = 0;//change shenh
        errorstate=-1;
        return;

      }
      */

    switch (comp + 1)
    {

        case 1:
            vect = (ti8 ? (c8 ? 1 : 0) : (t8 ? 3 : 0));
            yit = (ti8 ? ymb : ymb - 1);
            xit = xmb;

            vecb = (c8 ? 3 : 0);
            yib = ymb;
            xib = xmb;

            vecl = (li8 ? (c8 ? 1 : 0) : (l8 ? 2 : 0));
            yil = ymb;
            xil = (li8 ? xmb : xmb - 1);

            vecr = (c8 ? 2 : 0);
            yir = ymb;
            xir = xmb;

            /* edge handling */
            if (ymb == 1 || newgob1)
            {
                yit = ymb;
                vect = (c8 ? 1 : 0);
            }
            if (xmb == 1)
            {
                xil = xmb;
                vecl = (c8 ? 1 : 0);
            }
            break;

        case 2:
            vect = (ti8 ? (c8 ? 2 : 0) : (t8 ? 4 : 0));
            yit = (ti8 ? ymb : ymb - 1);
            xit = xmb;

            vecb = (c8 ? 4 : 0);
            yib = ymb;
            xib = xmb;
            vecl = (c8 ? 1 : 0);
            yil = ymb;
            xil = xmb;

            vecr = (ri8 ? (c8 ? 2 : 0) : (r8 ? 1 : 0));
            yir = ymb;
            xir = (ri8 ? xmb : xmb + 1);

            /* edge handling */
            if (ymb == 1 || newgob1)
            {
                yit = ymb;
                vect = (c8 ? 2 : 0);
            }
            if (xmb == mb_width)
            {
                xir = xmb;
                vecr = (c8 ? 2 : 0);
            }
            break;

        case 3:
            vect = (c8 ? 1 : 0);
            yit = ymb;
            xit = xmb;
            vecb = (c8 ? 3 : 0);
            yib = ymb;
            xib = xmb;

            vecl = (li8 ? (c8 ? 3 : 0) : (l8 ? 4 : 0));
            yil = ymb;
            xil = (li8 ? xmb : xmb - 1);

            vecr = (c8 ? 4 : 0);
            yir = ymb;
            xir = xmb;

            /* edge handling */
            if (xmb == 1)
            {
                xil = xmb;
                vecl = (c8 ? 3 : 0);
            }
            break;

        case 4:
            vect = (c8 ? 2 : 0);
            yit = ymb;
            xit = xmb;
            vecb = (c8 ? 4 : 0);
            yib = ymb;
            xib = xmb;
            vecl = (c8 ? 3 : 0);
            yil = ymb;
            xil = xmb;

            vecr = (ri8 ? (c8 ? 4 : 0) : (r8 ? 3 : 0));
            yir = ymb;
            xir = (ri8 ? xmb : xmb + 1);

            /* edge handling */
            if (xmb == mb_width)
            {
                xir = xmb;
                vecr = (c8 ? 4 : 0);
            }
            break;

        default:
            //fprintf (stderr, "Illegal block number in recon_comp_obmc (recon.c)\n");
            diag_printf ( "Illegal block number in recon_comp_obmc (recon.c)\n");
            //exit (-1);
            errorstate=-1;
            return;
            break;
    }

    nx[0] = MV[0][c8 ? comp + 1 : 0][ymb][xmb];
    ny[0] = MV[1][c8 ? comp + 1 : 0][ymb][xmb];

    nx[1] = MV[0][vect][yit][xit];
    ny[1] = MV[1][vect][yit][xit];
    nx[2] = MV[0][vecb][yib][xib];
    ny[2] = MV[1][vecb][yib][xib];
    nx[3] = MV[0][vecr][yir][xir];
    ny[3] = MV[1][vecr][yir][xir];
    nx[4] = MV[0][vecl][yil][xil];
    ny[4] = MV[1][vecl][yil][xil];

    for (k = 0; k < 5; k++)
    {
        xint[k] = nx[k] >> 1;
        xh[k] = nx[k] & 1;
        yint[k] = ny[k] >> 1;
        yh[k] = ny[k] & 1;
        s[k] = src + lx2 * (y + yint[k]) + x + xint[k];
    }

    d = dst + lx * y + x;

    if (!xh[0] && !yh[0])
        reco (s[0], &p[0], 8, lx2, 0, 0, 0, 8, 0, 8);
    else if (!xh[0] && yh[0])
        recvo (s[0], &p[0], 8, lx2, 0, 0, 0, 8, 0, 8);
    else if (xh[0] && !yh[0])
        recho (s[0], &p[0], 8, lx2, 0, 0, 0, 8, 0, 8);
    else                          /* if (xh[] && yh[]) */
        rec4o (s[0], &p[0], 8, lx2, 0, 0, 0, 8, 0, 8);

    if (!xh[1] && !yh[1])
        reco (s[1], &p[0], 8, lx2, 1, 1, 0, 8, 0, 4);
    else if (!xh[1] && yh[1])
        recvo (s[1], &p[0], 8, lx2, 1, 1, 0, 8, 0, 4);
    else if (xh[1] && !yh[1])
        recho (s[1], &p[0], 8, lx2, 1, 1, 0, 8, 0, 4);
    else                          /* if (xh[] && yh[]) */
        rec4o (s[1], &p[0], 8, lx2, 1, 1, 0, 8, 0, 4);

    if (!xh[2] && !yh[2])
        reco (s[2] + (lx2 << 2), &p[32], 8, lx2, 1, 2, 0, 8, 4, 8);
    else if (!xh[2] && yh[2])
        recvo (s[2] + (lx2 << 2), &p[32], 8, lx2, 1, 2, 0, 8, 4, 8);
    else if (xh[2] && !yh[2])
        recho (s[2] + (lx2 << 2), &p[32], 8, lx2, 1, 2, 0, 8, 4, 8);
    else                          /* if (xh[] && yh[]) */
        rec4o (s[2] + (lx2 << 2), &p[32], 8, lx2, 1, 2, 0, 8, 4, 8);

    if (!xh[3] && !yh[3])
        reco (s[3], &p[0], 8, lx2, 1, 3, 4, 8, 0, 8);
    else if (!xh[3] && yh[3])
        recvo (s[3], &p[0], 8, lx2, 1, 3, 4, 8, 0, 8);
    else if (xh[3] && !yh[3])
        recho (s[3], &p[0], 8, lx2, 1, 3, 4, 8, 0, 8);
    else                          /* if (xh[] && yh[]) */
        rec4o (s[3], &p[0], 8, lx2, 1, 3, 4, 8, 0, 8);

    if (!xh[4] && !yh[4])
        reco (s[4], &p[0], 8, lx2, 1, 4, 0, 4, 0, 8);
    else if (!xh[4] && yh[4])
        recvo (s[4], &p[0], 8, lx2, 1, 4, 0, 4, 0, 8);
    else if (xh[4] && !yh[4])
        recho (s[4], &p[0], 8, lx2, 1, 4, 0, 4, 0, 8);
    else                          /* if (xh[] && yh[]) */
        rec4o (s[4], &p[0], 8, lx2, 1, 4, 0, 4, 0, 8);

    pd = &p[0];
    for (j = 0; j < 8; j++)
    {
        d[0] = (pd[0] + 4) >> 3;
        d[1] = (pd[1] + 4) >> 3;
        d[2] = (pd[2] + 4) >> 3;
        d[3] = (pd[3] + 4) >> 3;
        d[4] = (pd[4] + 4) >> 3;
        d[5] = (pd[5] + 4) >> 3;
        d[6] = (pd[6] + 4) >> 3;
        d[7] = (pd[7] + 4) >> 3;
        d += lx;
        pd += 8;
    }

}
#endif
#endif

#endif// MEDIA_VOCVID_SUPPORT
