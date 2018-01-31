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

#include <stdlib.h>
#include <string.h>             /* memcpy, memset */
//#include <math.h>
#include "portab.h"
#include "mpeg4_global.h"           /* XVID_CSP_XXX's */
#include "xvid.h"           /* XVID_CSP_XXX's */
#include "image.h"
//#include "colorspace.h"
#include "interpolate8x8.h"
#include "mem_align.h"

/*
unsigned short zoom_width;//shenh for zoom
unsigned short zoom_height;//shenh for zoom
unsigned short zoom_mode;//0=no zoom 1=zoom in 2=zoom out
unsigned short rotate_mode;//0=no rotate 1=image rotate 2=LCD rotate
*/
extern uint8_t mpeg4_lowres;
int32_t
image_create(IMAGE * image,
             int16_t edged_width,
             int16_t edged_height)
{
    const int16_t edged_width2 = edged_width >> 1;
    const int16_t edged_height2 = edged_height >> 1;
    uint32_t temp_size;

    temp_size = edged_width * edged_height;
    image->y =
        xvid_malloc(temp_size+8, CACHE_LINE);//+8 for gouda y addr 8byte align.
    if (image->y == NULL)
    {
        return -1;
    }
    memset(image->y, 0, temp_size+8);
    //addr 8bytes align
    image->y+=8;
    image->y=(uint8_t*)((uint32_t)image->y>>3);
    image->y=(uint8_t*)((uint32_t)image->y<<3);

    temp_size = edged_width2 * edged_height2;
    image->u = xvid_malloc(temp_size, CACHE_LINE);
    if (image->u == NULL)
    {
        xvid_free(image->y);
        image->y = NULL;
        return -1;
    }
    memset(image->u, 0, temp_size);

    image->v = xvid_malloc(temp_size, CACHE_LINE);
    if (image->v == NULL)
    {
        xvid_free(image->u);
        image->u = NULL;
        xvid_free(image->y);
        image->y = NULL;
        return -1;
    }
    memset(image->v, 0, temp_size);

    if(mpeg4_lowres ==0)

    {
        image->y += EDGE_SIZE * edged_width + EDGE_SIZE;
        image->u += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;
        image->v += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;
    }
    else if(mpeg4_lowres == 1)
    {
        image->y += EDGE_SIZE_LOWRES_1* edged_width + EDGE_SIZE_LOWRES_1;
        image->u += EDGE_SIZE2_LOWRES_1* edged_width2 + EDGE_SIZE2_LOWRES_1;
        image->v += EDGE_SIZE2_LOWRES_1* edged_width2 + EDGE_SIZE2_LOWRES_1;

    }
    return 0;
}



void
image_destroy(IMAGE * image,
              int16_t edged_width,
              int16_t edged_height)
{
    const int16_t edged_width2 = edged_width >> 1;
    if(mpeg4_lowres == 0)
    {

        if (image->y)
        {
            xvid_free(image->y - (EDGE_SIZE * edged_width + EDGE_SIZE));
            image->y = NULL;
        }
        if (image->u)
        {
            xvid_free(image->u - (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2));
            image->u = NULL;
        }
        if (image->v)
        {
            xvid_free(image->v - (EDGE_SIZE2 * edged_width2 + EDGE_SIZE2));
            image->v = NULL;
        }
    }
    else if(mpeg4_lowres == 1)
    {

        if (image->y)
        {
            xvid_free(image->y - (EDGE_SIZE_LOWRES_1* edged_width + EDGE_SIZE_LOWRES_1));
            image->y = NULL;
        }
        if (image->u)
        {
            xvid_free(image->u - (EDGE_SIZE2_LOWRES_1* edged_width2 + EDGE_SIZE2_LOWRES_1));
            image->u = NULL;
        }
        if (image->v)
        {
            xvid_free(image->v - (EDGE_SIZE2_LOWRES_1* edged_width2 + EDGE_SIZE2_LOWRES_1));
            image->v = NULL;
        }
    }
}


void
image_swap(IMAGE * image1,
           IMAGE * image2)
{
    SWAP(uint8_t*, image1->y, image2->y);
    SWAP(uint8_t*, image1->u, image2->u);
    SWAP(uint8_t*, image1->v, image2->v);
}


void
image_copy(IMAGE * image1,
           IMAGE * image2,
           uint32_t edged_width,
           uint32_t height)
{
    int size = edged_width * height;

    memcpy(image1->y, image2->y, size);
    memcpy(image1->u, image2->u, size >> 2);
    memcpy(image1->v, image2->v, size >> 2);
}

/* setedges bug was fixed in this BS version */
void
image_setedges(IMAGE * image,
               uint32_t edged_width,
               uint32_t edged_height,
               uint32_t width,
               uint32_t height,
               int bs_version,
               uint8 low_resolution)
{
    const uint32_t edged_width2 = edged_width >> 1;
    //const uint32_t edged_height2 = edged_height >> 1;
    const uint32_t width2 = width >> 1;
    const uint32_t height2 = height >> 1;
    uint32_t edge_size2;
    uint32_t i;
    uint8_t *p_top_y_s,*p_top_y_d,*p_bottom_y_s,*p_bottom_y_d;
    uint8_t *p_top_u_s,*p_top_u_d,*p_bottom_u_s,*p_bottom_u_d;
    uint8_t *p_top_v_s,*p_top_v_d,*p_bottom_v_s,*p_bottom_v_d;
    uint8_t *p_left_y_d, *p_right_y_d;
    uint8_t *p_left_u_d, *p_right_u_d;
    uint8_t *p_left_v_d, *p_right_v_d;
    int32_t left_y_s, right_y_s;
    int32_t left_u_s, right_u_s;
    int32_t left_v_s, right_v_s;

    int8_t p_edge_size;
    int8_t p_edge_size2;
    switch(low_resolution)
    {
        case 0 : p_edge_size = EDGE_SIZE;
            p_edge_size2 = EDGE_SIZE2;
            break;
        case 1 : p_edge_size = EDGE_SIZE_LOWRES_1;
            p_edge_size2 = EDGE_SIZE2_LOWRES_1;
            break;


        default : p_edge_size = EDGE_SIZE;
            p_edge_size2 = EDGE_SIZE2;
            break;
    }

    edge_size2 = p_edge_size2;
    //left, right
    p_left_y_d  = image->y - p_edge_size;
    p_right_y_d = image->y + width;

    p_left_u_d  = image->u - edge_size2;
    p_right_u_d = image->u + width2;

    p_left_v_d  = image->v - edge_size2;
    p_right_v_d = image->v + width2;

    for(i = height2 ; i > 0; i--)
    {
        left_y_s = (int32_t)(*(p_left_y_d + p_edge_size));
        right_y_s = (int32_t)(*(p_right_y_d - 1));
        memset(p_left_y_d,  (int32_t)left_y_s  , p_edge_size);
        memset(p_right_y_d, (int32_t)right_y_s , p_edge_size);
        p_left_y_d  +=  edged_width;
        p_right_y_d +=  edged_width;

        left_y_s = (int32_t)(*(p_left_y_d + p_edge_size));
        right_y_s = (int32_t)(*(p_right_y_d - 1));
        memset(p_left_y_d,  (int32_t)left_y_s  , p_edge_size);
        memset(p_right_y_d, (int32_t)right_y_s , p_edge_size);
        p_left_y_d  +=  edged_width;
        p_right_y_d +=  edged_width;

        left_u_s = (int32_t)(*(p_left_u_d + edge_size2));
        right_u_s = (int32_t)(*(p_right_u_d - 1));
        memset(p_left_u_d,  (int32_t)left_u_s  , edge_size2);
        memset(p_right_u_d, (int32_t)right_u_s , edge_size2);
        p_left_u_d  +=  edged_width2;
        p_right_u_d +=  edged_width2;

        left_v_s = (int32_t)(*(p_left_v_d + edge_size2));
        right_v_s = (int32_t)(*(p_right_v_d - 1));
        memset(p_left_v_d,  (int32_t)left_v_s  , edge_size2);
        memset(p_right_v_d, (int32_t)right_v_s , edge_size2);
        p_left_v_d  +=  edged_width2;
        p_right_v_d +=  edged_width2;
    }
    //top, bottom
    p_top_y_s = image->y - p_edge_size;
    p_top_y_d = p_top_y_s - p_edge_size * edged_width;
    p_bottom_y_s = image->y + edged_width * (height - 1) - p_edge_size;
    p_bottom_y_d = p_bottom_y_s + edged_width;

    p_top_u_s = image->u - edge_size2;
    p_top_u_d = p_top_u_s - edge_size2 * edged_width2;
    p_bottom_u_s = image->u + edged_width2 * (height2 - 1) - edge_size2;
    p_bottom_u_d = p_bottom_u_s + edged_width2;

    p_top_v_s = image->v - edge_size2;
    p_top_v_d = p_top_v_s - edge_size2 * edged_width2;
    p_bottom_v_s = image->v + edged_width2 * (height2 - 1) - edge_size2;
    p_bottom_v_d = p_bottom_v_s + edged_width2;

    for( i = edge_size2 ; i > 0; i--)
    {
        memcpy(p_top_y_d, p_top_y_s, edged_width);
        memcpy(p_bottom_y_d, p_bottom_y_s, edged_width);
        p_top_y_d += edged_width;
        p_bottom_y_d += edged_width;

        memcpy(p_top_y_d, p_top_y_s, edged_width);
        memcpy(p_bottom_y_d, p_bottom_y_s, edged_width);
        p_top_y_d += edged_width;
        p_bottom_y_d += edged_width;

        memcpy(p_top_u_d, p_top_u_s, edged_width2);
        memcpy(p_bottom_u_d, p_bottom_u_s, edged_width2);
        p_top_u_d += edged_width2;
        p_bottom_u_d += edged_width2;

        memcpy(p_top_v_d, p_top_v_s, edged_width2);
        memcpy(p_bottom_v_d, p_bottom_v_s, edged_width2);
        p_top_v_d += edged_width2;
        p_bottom_v_d += edged_width2;
    }
}
int
image_output(IMAGE * image,
             uint32_t width,
             uint32_t height,
             uint32_t edged_width,
             uint8_t * dst[5],
             zoomPar *pZoom)
{
    *((unsigned int*)dst[0])=(unsigned int)image->y;
    *((unsigned int*)(dst[0]+4))=(unsigned int)image->u;
    *((unsigned int*)(dst[0]+8))=(unsigned int)image->v;
    *((unsigned int*)(dst[0]+12))=(unsigned int)edged_width;
    *((unsigned int*)(dst[0]+16))=(unsigned int)height;

#if 0
    int edged_width2 = edged_width >> 1;
    int width2 = width >> 1;
    int height2 = height >> 1;
    uint8_t *y_src,*y_dst,*u_src,*u_dst,*v_src,*v_dst;
    int y;
    yuvImage *yuvIn;
    yuvIn=(yuvImage*)image;
    y_src = image->y;
    u_src = image->u;
    v_src = image->v;
    y_dst = dst[0];

    //u_dst = y_dst + width*height;
    //v_dst = u_dst + width2*height2;
    u_dst = y_dst +pZoom->zoom_width*pZoom->zoom_height;//test
    v_dst = u_dst +(pZoom->zoom_width>>1)*(pZoom->zoom_height>>1);//test
#if 0
    for (y = height; y; y--)
    {
        memcpy(y_dst, y_src, width);
        y_src += edged_width;
        y_dst += width;
    }

    for (y = height2; y; y--)
    {
        memcpy(u_dst, u_src, width2);
        u_src += edged_width2;
        u_dst += width2;
    }

    for (y = height2; y; y--)
    {
        memcpy(v_dst, v_src, width2);
        v_src += edged_width2;
        v_dst += width2;
    }
#else
//int t;
//time_measure_start();
    if(pZoom->zoom_mode==1 && pZoom->rotate_mode==1)
    {
        //zoom and rotate

        zoom_twice_rotate_clockwise(y_src,  width,  edged_width-width,height,y_dst, pZoom->tempRotateBuf, pZoom->zoom_width,pZoom->zoom_height);
        zoom_twice_rotate_clockwise(u_src,  width2,  edged_width2-width2,height>>1,u_dst, pZoom->tempRotateBuf, pZoom->zoom_width>>1,pZoom->zoom_height>>1);
        zoom_twice_rotate_clockwise(v_src,  width2,  edged_width2-width2,height>>1,v_dst, pZoom->tempRotateBuf, pZoom->zoom_width>>1,pZoom->zoom_height>>1);

        //Img_Resample2_rotate_closewise( image, width, height, edged_width-width, dst[0], zoom_width, zoom_height);
    }
    else if(pZoom->zoom_mode==1 && pZoom->rotate_mode==0)
    {
        //zoom
        /*
        zoom_twice(y_src,  width,  edged_width-width,height,y_dst,zoom_width,zoom_height);
        zoom_twice(u_src,  width2,  edged_width2-width2,height>>1,u_dst,zoom_width>>1,zoom_height>>1);
        zoom_twice(v_src,  width2,  edged_width2-width2,height>>1,v_dst,zoom_width>>1,zoom_height>>1);
        */
        Img_Resample2( yuvIn, width, height, edged_width-width, dst[0], pZoom->tempRotateBuf, pZoom->zoom_width, pZoom->zoom_height);
    }
    else if(pZoom->zoom_mode==0 && pZoom->rotate_mode==1)
    {
        rotate_clockwise(y_src,  width,  edged_width-width, height, y_dst);
        rotate_clockwise(u_src,  width2,  edged_width2-width2, height>>1, u_dst);
        rotate_clockwise(v_src,  width2,  edged_width2-width2, height>>1, v_dst);
    }
    else if(pZoom->zoom_mode==2 && pZoom->rotate_mode==0)
    {
        Img_Resample2( yuvIn, width, height, edged_width-width, dst[0], pZoom->tempRotateBuf, pZoom->zoom_width, pZoom->zoom_height);
    }
    else
    {
        /*
        for(y = height2;y;y--)
        {
            memcpy(y_dst, y_src, width);
            y_src += edged_width;
            y_dst += width;
            memcpy(y_dst, y_src, width);
            y_src += edged_width;
            y_dst += width;
            memcpy(u_dst, u_src, width2);
            u_src += edged_width2;
            u_dst += width2;
            memcpy(v_dst, v_src, width2);
            v_src += edged_width2;
            v_dst += width2;
        }
        */

        Color16DitherImageIntoLcdBuf(image->y,image->u, image->v, (short)(edged_width-width),
                                     (short*)dst[0], 128, 0, 10,
                                     (short)width, (short)height);
    }

//t=time_measure_end();
//diag_printf(" t=%d\n",t/78);
#endif

#endif
    return 0;
}

void
image_clear(IMAGE * img, int width, int height, int edged_width,
            int y, int u, int v)
{
    uint8_t * p;
    int i;

    p = img->y;
    for (i = 0; i < height; i++)
    {
        memset(p, y, width);
        p += edged_width;
    }

    p = img->u;
    for (i = 0; i < height/2; i++)
    {
        memset(p, u, width/2);
        p += edged_width/2;
    }

    p = img->v;
    for (i = 0; i < height/2; i++)
    {
        memset(p, v, width/2);
        p += edged_width/2;
    }
}
#endif

