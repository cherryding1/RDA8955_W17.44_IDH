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




#include "cs_types.h"
#include "mcip_debug.h"
#include "mmf_isp.h"
#include "mmc_err.h"
#include "mmc_draw_lcd.h"

const long a_cosa[23]= {63302,  56755,  46340,  32768,  16961,  0,
                        -16961, -32767, -46340, -56755, -63302  -65536,
                        -63302, -56755, -46340, -32767, -16961, 0,
                        16961,  32768,  46340,  56755,  63302
                       };

const long b_sina[23]= {16961,  32768,  46340,  56755,  63302,  65536,
                        63302,  56755,  46340,  32768,  16961,  0,
                        -16961, -32767, -46340, -56755, -63302  -65536,
                        -63302, -56755, -46340, -32767, -16961
                       };


int MMF_Isp_Specialeffect1 (unsigned short H,  unsigned short W,  unsigned short* pInBuf, enum MMF_effect  Effect, unsigned short* pOutBuf )
{
    if( pInBuf==NULL)  return ISP_DATA_ERROR;

    unsigned short  i, j;
    short       G1, B1;
    unsigned short  piex_value;
    unsigned short  piex_R, piex_G, piex_B;
    unsigned long   Ru, Gu, Bu;

    switch( Effect )
    {
        case SPECIA:
            for( i=0; i<H; i++ )
            {
                for( j=0; j<W; j++ )
                {
                    piex_value = *(pInBuf+i*W+j);

                    piex_R  =     (unsigned short)(piex_value>>11);
                    piex_G  =     (unsigned short)((piex_value & 0x07ff)>>5);
                    piex_B  =     (unsigned short)(piex_value & 0x001f);

                    Ru  =     ((((unsigned long)(piex_R)*25756) + ((unsigned long)(piex_G)*25198) + ((unsigned long)(piex_B)*12386))>>16);
                    Gu  =     ((((unsigned long)(piex_R)*45744) + ((unsigned long)(piex_G)*44958) + ((unsigned long)(piex_B)*22020))>>16);
                    Bu  =     ((((unsigned long)(piex_R)*17826) + ((unsigned long)(piex_G)*17498) + ((unsigned long)(piex_B)*8585  ))>>17);


                    if( Ru>31 )
                    {
                        Ru = 31;
                    }
                    if( Gu>63 )
                    {
                        Gu = 63;
                    }
                    if( Bu>31 )
                    {
                        Bu= 31;
                    }
                    pOutBuf[i*W+j]  =   ((((unsigned short)Ru) & 0x001f)<<11) | ((((unsigned short)Gu) & 0x003f)<<5 ) | (((unsigned short)Bu)  & 0x001f);
                }
            }
            return ISP_FUN_OK;
            break;

        case GRAY:
            for( i=0; i<H; i++ )
            {
                for( j=0; j<W; j++ )
                {
                    piex_value  =  *(pInBuf+i*W+j);

                    piex_R  =     (unsigned short)(piex_value>>11);
                    piex_G  =     (unsigned short)((piex_value & 0x07ff)>>5);
                    piex_B  =     (unsigned short)(piex_value & 0x001f);

                    Ru  =   (((unsigned long)piex_R)*19595 + ((unsigned long)piex_G)*19235 + ((unsigned long)piex_B)*7472)>>16;
                    Gu  =   (((unsigned long)piex_R)*39190 + ((unsigned long)piex_G)*38469 + ((unsigned long)piex_B)*14944)>>16;
                    Bu  =   (((unsigned long)piex_R)*19595 + ((unsigned long)piex_G)*19235 + ((unsigned long)piex_B)*7472)>>16;


                    if( Ru>31 )
                    {
                        Ru = 31;
                    }
                    if( Gu>63 )
                    {
                        Gu = 63;
                    }
                    if( Bu>31 )
                    {
                        Bu= 31;
                    }
                    pOutBuf[i*W+j]  =   ((((unsigned short)Ru) & 0x001f)<<11) | ((((unsigned short)Gu) & 0x003f)<<5 ) | (((unsigned short)Bu)  & 0x001f);
                }
            }

            return ISP_FUN_OK;
            break;

        case NEGATIVE:
            for( i=0; i<H; i++ )
            {
                for( j=0; j<W; j++ )
                {
                    piex_value  =   *(pInBuf+i*W+j);
                    pOutBuf[i*W+j]   = 65535 - piex_value ;
                }
            }


            return ISP_FUN_OK;
            break;

        case RED_STANDOUT:
            for( i=0; i<H; i++ )
            {
                for( j=0; j<W; j++ )
                {
                    piex_value   =    *(pInBuf+i*W+j);

                    piex_R  =     (unsigned short)(piex_value>>11);
                    piex_G  =     (unsigned short)((piex_value & 0x07ff)>>5);
                    piex_B  =     (unsigned short)(piex_value & 0x001f);

                    Ru  =     (unsigned long)(piex_R + 15 ) ;
                    G1  =     piex_G;
                    B1  =     piex_B;

                    if( Ru>31 )
                    {
                        Ru = 31;
                    }
                    if( Ru<0 )
                    {
                        Ru =0;
                    }
                    if( G1>63 )
                    {
                        G1 = 63;
                    }
                    if( G1<0 )
                    {
                        G1 = 0;
                    }
                    if( B1>31 )
                    {
                        B1= 31;
                    }
                    if( B1<0 )
                    {
                        B1 =0 ;
                    }

                    pOutBuf[i*W+j]  = (((unsigned short)(Ru))<<11) | (((unsigned short)(G1) )<<5 ) | ((unsigned short)(B1) );
                }
            }


            return ISP_FUN_OK;
            break;


        default :
            return ISP_STATE_ERROR;
            break;

    }

}

//2007/10/13
int MMF_ISP_ZoomIn(unsigned short *pH,unsigned short *pW, unsigned short *pInBuf, unsigned short *pOutBuf, enum MMF_ZoomInWay ZoomInWay, int display_mode )
{

    if( pInBuf==NULL )  return ISP_DATA_ERROR;

    uint32      i, j,k;
    unsigned short      piex_value;
    uint32          H, W;
    uint32          return_hei, return_wid;
    uint32          Zoom;
    unsigned short      u, v;
    uint32 lineZoom;


    H = *pH;
    W = *pW;
    switch ( display_mode )
    {
        case 2:             //没旋转，缩放到buffer
            switch(  ZoomInWay )
            {
                case TWENTY_FIVE_PERCENT:
                    Zoom = 4;
                    return_hei = H>>2;
                    return_wid = W>>2;
                    lineZoom = (Zoom-1)*W;
                    for( i=0,k=0; i<return_hei; i++ )
                    {
                        for ( j=0; j<return_wid; j++ )
                        {
                            pOutBuf[k++] = *pInBuf;
                            pInBuf += Zoom;
                        }
                        pInBuf += lineZoom;
                    }

                    *pH = return_hei;
                    *pW = return_wid;
                    return ISP_FUN_OK;
                    break;

                case FIFTY_PERCENT:
                    Zoom = 2;
                    return_hei = H>>1;
                    return_wid = W>>1;
                    lineZoom = (Zoom-1)*W;
                    for( i=0,k=0; i<return_hei; i++ )
                    {
                        for ( j=0; j<return_wid; j++ )
                        {
                            pOutBuf[k++] = *pInBuf;
                            pInBuf += 2;
                        }
                        pInBuf += lineZoom;
                    }


                    *pH = return_hei;
                    *pW = return_wid;
                    return ISP_FUN_OK;
                    break;

                case SEVENTY_FIVE_PERCENT:
                    Zoom = 4;
                    u=0;
                    for( i=0; i<(H-(H-1)/4-1); i++ )
                    {
                        if( i%3==0 )
                        {
                            u++;
                        }
                        v = 0;
                        for( j=0; j<(W-(W-1)/4-1); j++ )
                        {
                            if( j%3==0 )
                            {
                                v++;
                            }
                            piex_value  =   *(pInBuf+(i+u)*W+(j+v));
                            pOutBuf[i*(W-(W-1)/4-1)+j]  =   piex_value;


                        }
                    }

                    return_hei = H-(H-1)/4-1;
                    return_wid = W-(W-1)/4-1;

                    *pH = return_hei;
                    *pW = return_wid;

                    return ISP_FUN_OK;
                    break;
                case EIGHTY_PERCENTY:
                    Zoom = 5;
                    u=0;
                    for( i=0; i<(H-(H-1)/5-1); i++ )
                    {
                        if( i%4==0 )
                        {
                            u++;
                        }
                        v = 0;
                        for( j=0; j<(W-(W-1)/5-1); j++ )
                        {
                            if( j%4==0 )
                            {
                                v++;
                            }
                            piex_value  =   *(pInBuf+(i+u)*W+(j+v));
                            pOutBuf[i*(W-(W-1)/5-1)+j]  =   piex_value;

                        }
                    }

                    return_hei = H-(H-1)/5-1;
                    return_wid = W-(W-1)/5-1;

                    *pH = return_hei;
                    *pW = return_wid;

                    return ISP_FUN_OK;
                    break;

                default :
                    return ISP_STATE_ERROR;
                    break;
            }
            break;
        case 1: //没旋转,直接写道LCD
            switch(  ZoomInWay )
            {
                case TWENTY_FIVE_PERCENT:
                    Zoom = 4;
                    return_hei = H>>2;
                    return_wid = W>>2;
                    lineZoom = (Zoom-1)*W;
                    DRAWPOINT_START(0,return_hei-1,0,return_wid-1);
                    for( i=0,k=0; i<return_hei; i++ )
                    {
                        for ( j=0; j<return_wid; j++ )
                        {
                            DRAWPOINT_LCD(*pInBuf);
                            pInBuf += Zoom;
                        }
                        pInBuf += lineZoom;
                    }
                    DRAWPOINT_DONE();

                    *pH = return_hei;
                    *pW = return_wid;
                    return ISP_FUN_OK;
                    break;

                case FIFTY_PERCENT:
                    Zoom = 2;
                    return_hei = H>>1;
                    return_wid = W>>1;
                    lineZoom = (Zoom-1)*W;
                    DRAWPOINT_START(0,return_hei-1,0,return_wid-1);
                    for( i=0,k=0; i<return_hei; i++ )
                    {
                        for ( j=0; j<return_wid; j++ )
                        {
                            DRAWPOINT_LCD(*pInBuf);
                            pInBuf += Zoom;
                        }
                        pInBuf += lineZoom;
                    }
                    DRAWPOINT_DONE();

                    *pH = return_hei;
                    *pW = return_wid;
                    return ISP_FUN_OK;
                    break;

                case SEVENTY_FIVE_PERCENT:
                    Zoom = 4;
                    u=0;
                    return_hei = H-(H-1)/Zoom-1;
                    return_wid = W-(W-1)/Zoom-1;
                    DRAWPOINT_START(0,return_hei-1,0,return_wid-1);
                    for( i=0; i<(H-(H-1)/4-1); i++ )
                    {
                        if( i%3==0 )
                        {
                            u++;
                        }
                        v = 0;
                        for( j=0; j<(W-(W-1)/4-1); j++ )
                        {
                            if( j%3==0 )
                            {
                                v++;
                            }
                            piex_value  =   *(pInBuf+(i+u)*W+(j+v));
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
                    DRAWPOINT_DONE();
                    *pH = return_hei;
                    *pW = return_wid;

                    return ISP_FUN_OK;
                    break;
                case EIGHTY_PERCENTY:
                    Zoom = 5;
                    u=0;
                    return_hei = H-(H-1)/Zoom-1;
                    return_wid = W-(W-1)/Zoom-1;
                    DRAWPOINT_START(0,return_hei-1,0,return_wid-1);
                    for( i=0; i<(H-(H-1)/5-1); i++ )
                    {
                        if( i%4==0 )
                        {
                            u++;
                        }
                        v = 0;
                        for( j=0; j<(W-(W-1)/5-1); j++ )
                        {
                            if( j%4==0 )
                            {
                                v++;
                            }
                            piex_value  =   *(pInBuf+(i+u)*W+(j+v));
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
                    DRAWPOINT_DONE();
                    *pH = return_hei;
                    *pW = return_wid;

                    return ISP_FUN_OK;
                    break;

                default :
                    return ISP_STATE_ERROR;
                    break;
            }
            break;
        case 0:               //有旋转
            switch(  ZoomInWay )
            {
                case TWENTY_FIVE_PERCENT:
                    Zoom = 4;
                    DRAWPOINT_START(0,(H/Zoom)-1,0,W/Zoom-1);
#if 0
                    for( i=0; i<(H/Zoom); i++ )
                    {
                        for ( j=0; j<(W/Zoom); j++ )
                        {
                            piex_value      =   *(pInBuf+i*W*Zoom+j*Zoom);
                            //pOutBuf[j*(H/Zoom)+i]     =   piex_value;
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
#else

                    for ( j=0; j<(W/Zoom); j++ )
                    {
                        for( i=0; i<(H/Zoom); i++ )
                        {
                            piex_value      =   *(pInBuf+i*W*Zoom+j*Zoom);
                            //pOutBuf[j*(H/Zoom)+i]     =   piex_value;
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
#endif
                    DRAWPOINT_DONE();
                    return_hei = W/Zoom;
                    return_wid = H/Zoom;

                    *pH = return_hei;
                    *pW = return_wid;
                    return ISP_FUN_OK;
                    break;

                case FIFTY_PERCENT:
                    Zoom = 2;
                    DRAWPOINT_START(0,(H/Zoom)-1,0,W/Zoom-1);
                    for( i=0; i<(H/Zoom); i++ )
                    {
                        for ( j=0; j<(W/Zoom); j++ )
                        {
                            piex_value      =   *(pInBuf+i*W*Zoom+j*Zoom);
                            //pOutBuf[j*(H/Zoom)+i]     =   piex_value;
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
                    DRAWPOINT_DONE();
                    return_hei = W/Zoom;
                    return_wid = H/Zoom;

                    *pH = return_hei;
                    *pW = return_wid;
                    return ISP_FUN_OK;
                    break;

                case SEVENTY_FIVE_PERCENT:
                    Zoom = 4;
                    u=0;
                    return_hei = H-(H-1)/Zoom-1;
                    return_wid = W-(W-1)/Zoom-1;
                    DRAWPOINT_START(0,return_hei-1,0,return_wid-1);
                    for( i=0; i<(H-(H-1)/4-1); i++ )
                    {
                        if( i%3==0 )
                        {
                            u++;
                        }
                        v = 0;
                        for( j=0; j<(W-(W-1)/4-1); j++ )
                        {
                            if( j%3==0 )
                            {
                                v++;
                            }
                            piex_value  =   *(pInBuf+(i+u)*W+(j+v));
                            //pOutBuf[j*(H-(H-1)/4-1)+i]    =   piex_value;
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
                    DRAWPOINT_DONE();
                    *pH = return_hei;
                    *pW = return_wid;

                    return ISP_FUN_OK;
                    break;
                case EIGHTY_PERCENTY:
                    Zoom = 5;
                    u=0;
                    return_hei = H-(H-1)/Zoom-1;
                    return_wid = W-(W-1)/Zoom-1;
                    DRAWPOINT_START(0,return_hei-1,0,return_wid-1);
                    for( i=0; i<(H-(H-1)/5-1); i++ )
                    {
                        if( i%4==0 )
                        {
                            u++;
                        }
                        v = 0;
                        for( j=0; j<(W-(W-1)/5-1); j++ )
                        {
                            if( j%4==0 )
                            {
                                v++;
                            }
                            piex_value  =   *(pInBuf+(i+u)*W+(j+v));
                            //pOutBuf[j*(H-(H-1)/5-1)+i]    =   piex_value;
                            DRAWPOINT_LCD(piex_value);

                        }
                    }
                    DRAWPOINT_DONE();
                    *pH = return_hei;
                    *pW = return_wid;

                    return ISP_FUN_OK;
                    break;

                default :
                    return ISP_STATE_ERROR;
                    break;
            }
            break;
    }

}
int MMF_Isp_Blend(unsigned short H1,unsigned short W1,unsigned short* pBuf1,unsigned short H2,unsigned short W2,unsigned short* pBuf2,unsigned short OffsetH,unsigned short OffsetW,unsigned short TransColor,unsigned short* pBufRtn)
{
    if( pBuf1==NULL )  return ISP_DATA_ERROR;
    if( pBuf2==NULL )  return ISP_DATA_ERROR;

    unsigned short  i, j;
    unsigned short  piex_a, piex_b;
    unsigned short  row_buf2;
    unsigned short  col_buf2;
    unsigned short  row_buf1;
    unsigned short  max_row;
    unsigned short  max_col;

    max_row = OffsetH+H2;
    max_col = OffsetW+W2;

    for( i=0; i<H1; i++ )
    {
        row_buf1   = i*W1;
        row_buf2   = (i-OffsetH)*W2;

        for( j=0; j<W1; j++ )
        {
            col_buf2 = j-OffsetW;
            piex_a  = *(pBuf1+row_buf1+j);
            if( (i>=OffsetH && i<max_row) && (j>=OffsetW && j <max_col) )
            {
                piex_b  = *(pBuf2+row_buf2+col_buf2);
                if( piex_b==TransColor )
                {
                    pBufRtn[row_buf1+j] = piex_a;
                }
                else
                {
                    pBufRtn[row_buf1+j] = piex_b;
                }
            }
            else
            {
                pBufRtn[row_buf1+j] = piex_a;
            }
        }
    }


    return 1;
}

int     MMF_Isp_ZoomOut(unsigned short* pH,  unsigned short* pW,  unsigned short* rgb_buf, enum MMF_ZoomOutWay ZoomOutWay, unsigned short* output_buf )
{
    if( rgb_buf==NULL)  return ISP_DATA_ERROR;

    unsigned short      i, j;
    short           uu, vv;
    unsigned short      m_min, m_max, n_min, n_max;
    unsigned short      R,G,B;
    unsigned short      hei1, wid1;
    unsigned short      zoom;
    unsigned short      thirdx, thirdy;
    unsigned short      piex_min_min;
    unsigned short      piex_min_max;
    unsigned short      piex_max_min;
    unsigned short      piex_max_max;

    unsigned short      piex_ori_ori;
    unsigned short      piex_ori_min;
    unsigned short      piex_ori_max;
    unsigned short      piex_min_ori;
    unsigned short      piex_max_ori;

    unsigned short      R_max_max, G_max_max, B_max_max;
    unsigned short      R_max_min, G_max_min, B_max_min;
    unsigned short      R_min_max, G_min_max, B_min_max;
    unsigned short      R_min_min, G_min_min, B_min_min;

    unsigned short      R_ori_ori, G_ori_ori, B_ori_ori;
    unsigned short      R_ori_min, G_ori_min, B_ori_min;
    unsigned short      R_ori_max, G_ori_max, B_ori_max;
    unsigned short      R_min_ori, G_min_ori, B_min_ori;
    unsigned short      R_max_ori, G_max_ori, B_max_ori;
    unsigned short          hei, wid;
    unsigned short          return_hei, return_wid;

    hei = *pH;
    wid = *pW;

    switch(  ZoomOutWay )
    {
        case INTERPOLATION1:


            for( i=0; i<(hei<<1); i++ )
            {
                if( i%2==0 )
                {
                    m_min=(i>>1);
                    m_max=(i>>1);
                }
                else
                {
                    m_min=((i-1)>>1);
                    m_max=((i+1)>>1);
                }

                for ( j=0; j<(wid<<1); j++ )
                {
                    if( j%2==0 )
                    {
                        n_min=(j>>1);
                        n_max=(j>>1);
                    }
                    else
                    {
                        n_min=((j-1)>>1);
                        n_max=((j+1)>>1);
                    }

                    piex_min_min    =   *(rgb_buf +m_min*wid+n_min);
                    piex_min_max    =   *(rgb_buf +m_min*wid+n_max);
                    piex_max_min    =   *(rgb_buf +m_max*wid+n_min);
                    piex_max_max    =   *(rgb_buf +m_max*wid+n_max);

                    R_min_min   =   (unsigned short)(piex_min_min>>11);
                    R_min_max   =   (unsigned short)(piex_min_max>>11);
                    R_max_min   =   (unsigned short)(piex_max_min>>11);
                    R_max_max   =   (unsigned short)(piex_max_max>>11);

                    G_min_min   =   (unsigned short)((piex_min_min & 0x07ff)>>5);
                    G_min_max   =   (unsigned short)((piex_min_max & 0x07ff)>>5);
                    G_max_min   =   (unsigned short)((piex_max_min & 0x07ff)>>5);
                    G_max_max   =   (unsigned short)((piex_max_max & 0x07ff)>>5);

                    B_min_min   =   (unsigned short)(piex_min_min & 0x001f);
                    B_min_max   =   (unsigned short)(piex_min_max & 0x001f);
                    B_max_min   =   (unsigned short)(piex_max_min & 0x001f);
                    B_max_max   =   (unsigned short)(piex_max_max & 0x001f);

                    if( (i%2) == 0 )
                    {
                        R   =   ( R_min_min+ R_min_max ) >>1;
                        G   =   ( G_min_min + G_min_max) >>1;
                        B   =   ( B_min_min + B_min_max ) >>1;
                    }
                    else if( (j%2) == 0 )
                    {
                        R   =   ( R_min_min + R_max_min ) >>1;
                        G   =   ( G_min_min+ G_max_min) >>1;
                        B   =   ( B_min_min+ B_max_min) >>1;
                    }
                    else
                    {
                        R   =   ( R_min_min + R_min_max+ R_max_min + R_max_max) >>2;
                        G   =   ( G_min_min +G_min_max + G_max_min+ G_max_max ) >>2;
                        B   =   (B_min_min + B_min_max + B_max_min + B_max_max ) >>2;
                    }

                    output_buf[i*wid*2+j]   =   (R<<11) |((G<<5) & 0x07ff) | (B & 0x001f);

                }
            }

            return_hei = hei<<1;
            return_wid = wid<<1;

            *pH = return_hei;
            *pW = return_wid;

            return ISP_FUN_OK;
            break;

        case INTERPOLATION2:

            zoom=2;
            hei1        =   ((hei/zoom)*(zoom+1)+(hei%zoom));
            wid1        =   ((wid/zoom)*(zoom+1)+(wid%zoom));

            uu          =   -1;
            thirdy      =   0;


            for( i=0; i<hei1; i++ )
            {
                uu=uu+1;
                vv=-1;
                if( uu==zoom )
                {
                    m_min   =   (i+1)/(zoom+1);
                    m_max   =   (i+1+zoom+1)/(zoom+1);
                    thirdy  =   thirdy + 1;
                    uu      =   -1;
                }
                else
                {
                    m_min  = i - thirdy;
                    m_max = i - thirdy;
                }

                thirdx = 0;
                for ( j=0; j<wid1; j++ )
                {
                    vv  =vv+1;
                    if( vv==zoom )
                    {
                        n_min   =   (j+1)/(zoom+1);
                        n_max   =   (j+1+zoom+1)/(zoom+1);
                        thirdx  =   thirdx + 1;
                        vv      =   -1;
                    }
                    else
                    {
                        n_min  = j - thirdx;
                        n_max = j - thirdx;
                    }

                    piex_min_min    =   *(rgb_buf +m_min*wid+n_min);
                    piex_min_max    =   *(rgb_buf +m_min*wid+n_max);
                    piex_max_min    =   *(rgb_buf +m_max*wid+n_min);
                    piex_max_max    =   *(rgb_buf +m_max*wid+n_max);

                    piex_ori_ori    =   *(rgb_buf +(i-thirdy)*wid+(j-thirdx));
                    piex_min_ori    =   *(rgb_buf +m_min*wid+(j-thirdx));
                    piex_max_ori    =   *(rgb_buf +m_max*wid+(j-thirdx));
                    piex_ori_min    =   *(rgb_buf +(i-thirdy)*wid+n_min);
                    piex_ori_max    =   *(rgb_buf +(i-thirdy)*wid+n_max);

                    R_min_min   =   (unsigned short)(piex_min_min>>11);
                    R_min_max   =   (unsigned short)(piex_min_max>>11);
                    R_max_min   =   (unsigned short)(piex_max_min>>11);
                    R_max_max   =   (unsigned short)(piex_max_max>>11);

                    G_min_min   =   (unsigned short)((piex_min_min & 0x07ff)>>5);
                    G_min_max   =   (unsigned short)((piex_min_max & 0x07ff)>>5);
                    G_max_min   =   (unsigned short)((piex_max_min & 0x07ff)>>5);
                    G_max_max   =   (unsigned short)((piex_max_max & 0x07ff)>>5);

                    B_min_min   =   (unsigned short)(piex_min_min & 0x001f);
                    B_min_max   =   (unsigned short)(piex_min_max & 0x001f);
                    B_max_min   =   (unsigned short)(piex_max_min & 0x001f);
                    B_max_max   =   (unsigned short)(piex_max_max & 0x001f);



                    R_ori_ori   =   (unsigned short)(piex_ori_ori>>11);
                    R_min_ori   =   (unsigned short)(piex_min_ori>>11);
                    R_max_ori   =   (unsigned short)(piex_max_ori>>11);
                    R_ori_min   =   (unsigned short)(piex_ori_min>>11);
                    R_ori_max   =   (unsigned short)(piex_ori_max>>11);

                    G_ori_ori   =   (unsigned short)((piex_ori_ori & 0x07ff)>>5) ;
                    G_min_ori   =   (unsigned short)((piex_min_ori & 0x07ff)>>5);
                    G_max_ori   =   (unsigned short)((piex_max_ori & 0x07ff)>>5);
                    G_ori_min   =   (unsigned short)((piex_ori_min & 0x07ff)>>5);
                    G_ori_max   =   (unsigned short)((piex_ori_max & 0x07ff)>>5);

                    B_ori_ori   =   (unsigned short)(piex_ori_ori & 0x001f);
                    B_min_ori   =   (unsigned short)(piex_min_ori & 0x001f);
                    B_max_ori   =   (unsigned short)(piex_max_ori & 0x001f);
                    B_ori_min   =   (unsigned short)(piex_ori_min & 0x001f);
                    B_ori_max   =   (unsigned short)(piex_ori_max & 0x001f);

                    if(uu!=zoom && vv!=zoom )
                    {
                        R =  R_ori_ori ;
                        G =  G_ori_ori ;
                        B =  B_ori_ori ;
                    }
                    else if(uu!=zoom && vv==zoom )
                    {
                        R       = (R_ori_min+ R_ori_max)>>1;
                        G   = (G_ori_min +  G_ori_max)>>1;
                        B   = (B_ori_min + B_ori_max )>>1;
                    }
                    else if(uu==zoom && vv!=zoom )
                    {
                        R       = (R_min_ori + R_max_ori)>>1;
                        G   = (G_min_ori  + G_max_ori )>>1;
                        B   = (B_min_ori  + B_max_ori )>>1;
                    }
                    else
                    {
                        R       = (R_min_min + R_max_max + R_min_max + R_max_min)>>2;
                        G   = (G_min_min + G_max_max + G_min_max  + G_max_min )>>2;
                        B   = (B_min_min  + B_max_max + B_min_max  + B_max_min )>>2;
                    }

                    output_buf[i*wid1+j]    =   (R<<11) |((G<<5) & 0x07ff) | (B & 0x001f);

                }
            }

            return_hei = (hei/2)*3+(hei%2);
            return_wid = (wid/2)*3+(wid%2);

            *pH = return_hei;
            *pW = return_wid;

            return ISP_FUN_OK;
            break;

        default :
            return ISP_STATE_ERROR;
            break;

    }

}
////////////////////////////////////////////////////////////////////////////////
//2007/09/04
int MMF_ISP_Rotate( int* pH, int* pW, unsigned short* rgb_buf, int angle, unsigned short* output_buf )
{
    if( rgb_buf==NULL)  return ISP_DATA_ERROR;

    int     wid, hei;
    long        new_wid, new_hei;
    long        j_Src, i_Src;
    long        j, i;
    long        xa, xb;
    long        ya, yb;
    long        f1;
    long        f2;
    long        fSina, fCosa;
    int         max_wid;
    int         max_hei;
    long        max_new_wid;
    long        max_new_hei;
    int         i_fun;

    hei = *pH;
    wid = *pW;

    i_fun = angle/15-1;


    fSina  = b_sina[i_fun];      //15度
    fCosa  = a_cosa[i_fun];

    max_wid = wid-1;
    max_hei = hei-1;

    xa =  fCosa*max_wid - fSina*max_hei;
    xb = -fCosa*max_wid - fSina*max_hei;
    ya =  fSina*max_wid + fCosa*max_hei;
    yb =  fSina*max_wid - fCosa*max_hei;

    new_wid   = (long) ( fun_max( int_abs(xa), int_abs(xb) ) + 65536 )>>16;
    new_hei   = (long) ( fun_max( int_abs(ya), int_abs(yb) ) + 65536 )>>16;

    max_new_wid  = new_wid-1;
    max_new_hei  = new_hei-1;

    f1 =  (-max_new_wid*fCosa - max_new_hei*fSina + (max_wid<<16) )>>1;
    f2 =  ( max_new_wid*fSina - max_new_hei*fCosa + (max_hei<<16) )>>1;


    for( i=0; i<new_hei; i++ )
    {
        for( j=0; j<new_wid; j++ )
        {
            i_Src = (long) (-j * fSina +  i * fCosa + f2 + 65536 )>>16;
            j_Src = (long) ( j * fCosa +  i * fSina + f1 + 65536 )>>16;

            if( ( j_Src>=0 ) && ( j_Src<wid ) && ( i_Src>=0 ) && ( i_Src<hei ))
            {
                output_buf[i*new_wid+j] = *(rgb_buf+i_Src*wid+j_Src);
            }
            else
            {
                output_buf[i*new_wid+j] = 65535;
            }
        }
    }

    *pH = new_hei;
    *pW = new_wid;
    return ISP_FUN_OK;
}

int MMF_ISP_VGAto160_128( int* pH, int* pW, unsigned short* rgb_buf, unsigned short* output_buf )
{
    if( rgb_buf==NULL)  return ISP_DATA_ERROR;

    int     x,y;
    int     m_min;
    int     R_max_value, G_max_value, B_max_value;
    int     R_min_value, G_min_value, B_min_value;
    int     piex_min_value;
    int     piex_max_value;
    int         hei, wid;
    int     hei_stretch, wid_stretch;
    int         line_min_count, line_max_count;
    int     output_row;
    int         x_ori;
    int     R, G, B;

    if( (*pH!=480)  || (*pW!=640) )
    {
        return ISP_DATA_ERROR;
    }

    hei = *pH;
    wid = *pW;

    hei_stretch = (hei<<2);
    wid_stretch = (wid>>2);

    for( y=0; y<hei_stretch; y=y+15 )
    {
        output_row = y/15;
        m_min      = (y>>2);

        for ( x=0; x<wid_stretch; x++ )
        {
            x_ori = (x<<2);
            if( (y%5)==0 )
            {
                line_min_count = m_min*wid;
                output_buf[output_row*wid_stretch+x] = *(rgb_buf+line_min_count+x_ori);
            }
            else
            {
                line_min_count = m_min*wid;
                line_max_count = line_min_count+wid;

                piex_min_value  =  (int)(*(rgb_buf+line_min_count+x_ori));
                piex_max_value  =  (int)(*(rgb_buf+line_max_count+x_ori));

                R_min_value  =  (int)(piex_min_value>>11);
                R_max_value  =  (int)(piex_max_value>>11);

                G_min_value  =  (int)((piex_min_value & 0x07ff)>>5);
                G_max_value  =  (int)((piex_max_value & 0x07ff)>>5);

                B_min_value  =  (int)(piex_min_value & 0x001f);
                B_max_value  =  (int)(piex_max_value & 0x001f);

                R = ( R_min_value + R_max_value )>>1;
                G = ( G_min_value + G_max_value )>>1;
                B = ( B_min_value + B_max_value )>>1;

                output_buf[output_row*wid_stretch+x]    =   ((unsigned short)R<<11) |(((unsigned short)G<<5) & 0x07ff) | ((unsigned short)B & 0x001f);
            }
        }
    }

    *pH = 128;
    *pW = 160;
    return ISP_FUN_OK;
}
////////////////////////////////////////////////////////////
////2007/09/05
int MMF_Isp_Blend_Rotate(unsigned short* pH,unsigned short* pW,unsigned short* pBuf1,unsigned short H2,unsigned short W2,unsigned short* pBuf2,unsigned short OffsetH,unsigned short OffsetW,unsigned short TransColor,unsigned short* pBufRtn, unsigned short Mode )
{
    if( pBuf1==NULL )  return ISP_DATA_ERROR;
    if( pBuf2==NULL )  return ISP_DATA_ERROR;

    unsigned short  i, j;
    unsigned short  piex_a, piex_b;
    unsigned short  H1, W1;
    unsigned short  row_buf2;
    unsigned short  col_buf2;
    unsigned short  row_buf1;
    unsigned short  rotate_row;
    unsigned short  rotate_col;
    unsigned short  max_row;
    unsigned short  max_col;

    H1 = *pH;
    W1 = *pW;

    max_row = OffsetH+H2;
    max_col = OffsetW+W2;

    if( Mode == 0 )
    {
        for( i=0; i<H1; i++ )
        {
            row_buf1   = i*W1;
            row_buf2   = (i-OffsetH)*W2;

            for( j=0; j<W1; j++ )
            {
                col_buf2 = j-OffsetW;
                piex_a  = *(pBuf1+row_buf1+j);
                if( (i>=OffsetH && i<max_row) && (j>=OffsetW && j <max_col) )
                {
                    piex_b  = *(pBuf2+row_buf2+col_buf2);
                    if( piex_a==TransColor )
                    {
                        pBufRtn[row_buf1+j] = piex_b;
                    }
                    else
                    {
                        pBufRtn[row_buf1+j] = piex_a;
                    }
                }
                else
                {
                    pBufRtn[row_buf1+j] = piex_a;
                }
            }
        }
        *pH = H1;
        *pW = W1;
    }
    else
    {
        for( i=0; i<H1; i++ )
        {
            row_buf1   = i*W1;
            row_buf2   = (i-OffsetH)*W2;
            rotate_col = H1-i;

            for( j=0; j<W1; j++ )
            {
                rotate_row = j*H1;
                col_buf2   = j-OffsetW;
                piex_a  = *(pBuf1+row_buf1+j);
                if( (i>=OffsetH && i<max_row) && (j>=OffsetW && j <max_col) )
                {
                    piex_b  = *(pBuf2+row_buf2+col_buf2);
                    if( piex_a==TransColor )
                    {
                        pBufRtn[rotate_row+rotate_col] = piex_b;
                    }
                    else
                    {
                        pBufRtn[rotate_row+rotate_col] = piex_a;
                    }
                }
                else
                {
                    pBufRtn[rotate_row+rotate_col] = piex_a;
                }
            }
        }

        *pH = W1;
        *pW = H1;
    }

    return ISP_FUN_OK;
}


/////////////////////////////////////////////////////////////////////
////////////////////
//2007/10/13
int MMF_ISP_VGAto1280_1024( unsigned short hei, unsigned short wid, unsigned short* rgb_buf, unsigned short* output_buf )
{


    int x,y;
    int m_min,n_min,m_max,n_max;
    int R_max_max, G_max_max, B_max_max;
    int R_max_min, G_max_min, B_max_min;
    int R_min_max, G_min_max, B_min_max;
    int R_min_min, G_min_min, B_min_min;
    int piex_min_min;
    int piex_min_max;
    int piex_max_min;
    int piex_max_max;
    int     R, G, B;

    int     y_temp;
    static int  line_temp = 0;
    static int  a = 0;
    static int  u = 0;


    if( rgb_buf==NULL)  return ISP_DATA_ERROR;
    if( (hei!=480)  || (wid!=640) )
    {
        return ISP_DATA_ERROR;
    }
    for ( x=line_temp; x<line_temp+16; x++ )
    {
        if( (x & 0x0001)==0 )
        {
            m_min  =  (x>>1);
            m_max  =  m_min;
        }
        else
        {
            m_min  =  ((x-1)>>1);
            m_max =  m_min+1;
        }
        u=0;
        a=0;
        for (y=0; y<1024; y++)
        {
            if( y==a )
            {
                y_temp = y-u;
                u++;
                a = a+16;
            }
            else
            {
                y_temp = y-u;
            }

            if( (y_temp & 0x0001)==0 )
            {
                n_min  = (y_temp>>1);
                n_max  = n_min;
            }
            else
            {
                n_min  =  ((y_temp-1)>>1);
                n_max =  n_min+1;
            }

            piex_min_min    =   (int)(*(rgb_buf +n_min*wid+m_min));
            piex_min_max    =   (int)(*(rgb_buf +n_min*wid+m_max));
            piex_max_min    =   (int)(*(rgb_buf +n_max*wid+m_min));
            piex_max_max    =   (int)(*(rgb_buf +n_max*wid+m_max));


            R_min_min   =   (int)(piex_min_min>>11);
            R_min_max   =   (int)(piex_min_max>>11);
            R_max_min   =   (int)(piex_max_min>>11);
            R_max_max   =   (int)(piex_max_max>>11);

            G_min_min   =   (int)((piex_min_min & 0x07ff)>>5);
            G_min_max   =   (int)((piex_min_max & 0x07ff)>>5);
            G_max_min   =   (int)((piex_max_min & 0x07ff)>>5);
            G_max_max   =   (int)((piex_max_max & 0x07ff)>>5);

            B_min_min   =   (int)(piex_min_min & 0x001f);
            B_min_max   =   (int)(piex_min_max & 0x001f);
            B_max_min   =   (int)(piex_max_min & 0x001f);
            B_max_max   =   (int)(piex_max_max & 0x001f);

            if( (y_temp & 0x0001)==0 )
            {
                R   =   ( R_min_min + R_min_max ) >>1;
                G   =   ( G_min_min + G_min_max) >>1;
                B   =   ( B_min_min + B_min_max ) >>1;
            }

            else if( ( x & 0x0001)==0 )
            {
                R   =   ( R_min_min + R_max_min ) >>1;
                G   =   ( G_min_min + G_max_min ) >>1;
                B   =   ( B_min_min + B_max_min ) >>1;
            }
            else
            {
                R   =   ( R_min_min + R_min_max + R_max_min  + R_max_max) >>2;
                G   =   ( G_min_min + G_min_max + G_max_min + G_max_max ) >>2;
                B   =   ( B_min_min + B_min_max  + B_max_min + B_max_max) >>2;
            }
            output_buf[(x-line_temp)*1024+y]    =   ((unsigned short)R<<11) |(((unsigned short)G<<5) & 0x07ff) | ((unsigned short)B & 0x001f);
        }
    }
    line_temp = line_temp+16;



    return ISP_FUN_OK;
}
////////////////////////////
//11月2日
int MMF_ISP_ZoomIn_Blend_Rotate(unsigned short hei_b, unsigned short wid_b, unsigned short* rgb_buf_b,
                                unsigned short hei_a_ori, unsigned short wid_a_ori, unsigned short* rgb_buf_a,
                                unsigned short offsetH, unsigned short offsetW, enum MMF_ZoomInWay ZoomInWay )
{
    unsigned short hei_a, wid_a;
    unsigned short Zoom;
    unsigned short max_row, max_col;
    int u, v;
    unsigned short i, j;
    unsigned short i_a, j_a;
    unsigned short piex_a, piex_b;
    unsigned short row_buf_b;

    if( rgb_buf_b==NULL )  return ISP_DATA_ERROR;
    if( rgb_buf_a==NULL )  return ISP_DATA_ERROR;

    DRAWPOINT_START(0,127,0,159);                        //11月2日

    switch( ZoomInWay )
    {
        case SEVENTY_FIVE_PERCENT:
            Zoom = 4;
            hei_a = hei_a_ori-(hei_a_ori-1)/Zoom-1;
            wid_a = wid_a_ori-(wid_a_ori-1)/Zoom-1;
            max_row = offsetH+wid_a;
            max_col = offsetW+hei_a;

            u=0;
            for( i=0; i<hei_b; i++ )
            {
                row_buf_b   = i*wid_b;

                if( i>=offsetH && i<max_row )
                {
                    i_a = i - offsetH;

                    if( i_a%3==0 )
                    {
                        u++;
                    }
                }

                v = 0;
                for( j=0; j<wid_b; j++ )
                {
                    piex_b  = *(rgb_buf_b+row_buf_b+j);

                    if( (i>=offsetH && i<max_row) && (j>=offsetW && j <max_col) )
                    {
                        j_a = j - offsetW;
                        if( j_a%3==0 )
                        {
                            v++;
                        }

                        piex_a  = *(rgb_buf_a+(j_a+v)*wid_a_ori+(i_a+u));

                        if( piex_b==0x0000 )
                        {
                            //pBufRtn[row_buf_b+j] = piex_a;
                            DRAWPOINT_LCD(piex_a);
                        }
                        else
                        {
                            //pBufRtn[row_buf_b+j] = piex_b;
                            DRAWPOINT_LCD(piex_b);
                        }
                    }
                    else
                    {
                        //pBufRtn[row_buf_b+j] = piex_b;
                        DRAWPOINT_LCD(piex_b);
                    }
                }
            }
            break;

        case EIGHTY_PERCENTY:
            Zoom = 5;

            hei_a = hei_a_ori-(hei_a_ori-1)/Zoom-1;
            wid_a = wid_a_ori-(wid_a_ori-1)/Zoom-1;
            max_row = offsetH+wid_a;
            max_col = offsetW+hei_a;

            u=0;
            for( i=0; i<hei_b; i++ )
            {
                row_buf_b   = i*wid_b;

                if( i>=offsetH && i<max_row )
                {
                    i_a = i - offsetH;

                    if( i_a%3==0 )
                    {
                        u++;
                    }
                }

                v = 0;
                for( j=0; j<wid_b; j++ )
                {
                    piex_b  = *(rgb_buf_b+row_buf_b+j);

                    if( (i>=offsetH && i<max_row) && (j>=offsetW && j <max_col) )
                    {
                        j_a = j - offsetW;
                        if( j_a%3==0 )
                        {
                            v++;
                        }
                        piex_a  = *(rgb_buf_a+(j_a+v)*wid_a_ori+(i_a+u));

                        if( piex_b==0x0000 )
                        {
                            //pBufRtn[row_buf_b+j] = piex_a;
                            DRAWPOINT_LCD(piex_a);
                        }
                        else
                        {
                            //pBufRtn[row_buf_b+j] = piex_b;
                            DRAWPOINT_LCD(piex_b);
                        }
                    }
                    else
                    {
                        //pBufRtn[row_buf_b+j] = piex_b;
                        DRAWPOINT_LCD(piex_b);
                    }
                }
            }
            break;
        case TWENTY_FIVE_PERCENT:
            Zoom = 4;
            hei_a = hei_a_ori/Zoom;
            wid_a = wid_a_ori/Zoom;
            max_row = offsetH+wid_a;
            max_col = offsetW+hei_a;

            for( i=0; i<hei_b; i++ )
            {
                row_buf_b   = i*wid_b;

                if( i>=offsetH && i<max_row )
                {
                    i_a = i - offsetH;
                }

                for( j=0; j<wid_b; j++ )
                {
                    piex_b  = *(rgb_buf_b+row_buf_b+j);

                    if( (i>=offsetH && i<max_row) && (j>=offsetW && j <max_col) )
                    {
                        j_a = j - offsetW;

                        piex_a  = *(rgb_buf_a+j_a*wid_a_ori*Zoom+i_a*Zoom);    //枚举

                        if( piex_b==0x0000 )
                        {
                            //pBufRtn[row_buf_b+j] = piex_a;
                            DRAWPOINT_LCD(piex_a);
                        }
                        else
                        {
                            //pBufRtn[row_buf_b+j] = piex_b;
                            DRAWPOINT_LCD(piex_b);
                        }
                    }
                    else
                    {
                        //pBufRtn[row_buf_b+j] = piex_b;
                        DRAWPOINT_LCD(piex_b);
                    }
                }
            }
            break;

        case FIFTY_PERCENT:
            Zoom = 2;

            hei_a = hei_a_ori/Zoom;
            wid_a = wid_a_ori/Zoom;
            max_row = offsetH+wid_a;
            max_col = offsetW+hei_a;

            for( i=0; i<hei_b; i++ )
            {
                row_buf_b   = i*wid_b;

                if( i>=offsetH && i<max_row )
                {
                    i_a = i - offsetH;
                }

                for( j=0; j<wid_b; j++ )
                {
                    piex_b  = *(rgb_buf_b+row_buf_b+j);

                    if( (i>=offsetH && i<max_row) && (j>=offsetW && j <max_col) )
                    {
                        j_a = j - offsetW;

                        piex_a  = *(rgb_buf_a+j_a*wid_a_ori*Zoom+i_a*Zoom);    //枚举

                        if( piex_b==0x0000 )
                        {
                            //pBufRtn[row_buf_b+j] = piex_a;
                            DRAWPOINT_LCD(piex_a);
                        }
                        else
                        {
                            //pBufRtn[row_buf_b+j] = piex_b;
                            DRAWPOINT_LCD(piex_b);
                        }
                    }
                    else
                    {
                        //pBufRtn[row_buf_b+j] = piex_b;
                        DRAWPOINT_LCD(piex_b);
                    }
                }
            }
            break;
        default:
            break;
    }

    DRAWPOINT_DONE();                      //11月2日

    return ISP_FUN_OK;
}
