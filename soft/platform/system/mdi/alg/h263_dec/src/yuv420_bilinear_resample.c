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
#include "yuv420_bilinear_resample.h"



#define PIXEL_VALUE 256 //0~255
//#define PIXEL_SCALE_BIT 4 // adjust max 8 for short
#define PIXEL_SCALE_BIT 20 // adjust max 8 for short
#define Mask_SCALE_BIT (unsigned int)0x000fffff //(((unsigned int)-1)>>(32-PIXEL_SCALE_BIT)) // adjust max 16 for short

#define BYPASS0 0//no bypass row.
#define BYPASS1 1//bypass first row.
#define BYPASS2 2//bypass both first and second row.


/*
    internal SRAM
    0x80c00000 - 0x80c0ffff  total 64kB SRAM
    0x80c00000- 0x80c068b0 be used 26kB
    0x80c0fdb0 -0x80c0ffff be used 592 bytes
    0x80c068b0-0x80c0fdaf can be use 38143 bytes
*/
//#define YUV2RGB_START 0x80c068b0 //size 2048*3 bytes
//#define RESAMPLE_START 0x80c080b0 //size uncertain (128x96 image=1408 bytes)



/**************
for yuv 2 rgb565
***************/

//#define YUV2RGBLOOKUPTABLE

#ifdef YUV2RGBLOOKUPTABLE
static int*L_tab, *Cr_r_tab, *Cr_g_tab, *Cb_g_tab, *Cb_b_tab;
#endif

short *r_2_pix;
short *g_2_pix;
short *b_2_pix;
static short *r_2_pix_alloc;
static short *g_2_pix_alloc;
static short *b_2_pix_alloc;
///yuv 2 rgb565 end

extern char * mmc_MemMalloc(int32 nsize);


#ifdef LOOKUPTABLE
static uint8 (*PixelTab)[(1<<PIXEL_SCALE_BIT)+1]=NULL;//256*(2^PIXEL_SCALE_BIT)
static uint8 *LimitTab=NULL;//0~255

//uint8* tempRotateBuf=NULL;

int init_pixeltab(unsigned int ImgInw, unsigned int ImgInh, unsigned int ImgOutw,unsigned int ImgOuth)
{
    int i,j;
    unsigned short maxScale;
    maxScale=(1<<PIXEL_SCALE_BIT)+1;
    //PixelTab=malloc(PIXEL_VALUE*maxScale*sizeof(unsigned short));

    PixelTab=mmc_MemMalloc(PIXEL_VALUE*maxScale);
    LimitTab=mmc_MemMalloc(PIXEL_VALUE*2);

    if((PixelTab!=NULL) ||(LimitTab!=NULL))
    {
        memset(PixelTab,0,PIXEL_VALUE*maxScale);
        memset(LimitTab,0,PIXEL_VALUE*2);
    }
    else
        return -1;

    for(i=0; i<PIXEL_VALUE; i++)
    {
        for(j=0; j<maxScale; j++)
            PixelTab[i][j]=(i*(maxScale-1-j)+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;
    }

    for(i=0; i<(PIXEL_VALUE*2); i++)
    {
        if(i<256)
            LimitTab[i]=i;
        else
            LimitTab[i]=255;
    }

    return 0;
}

int close_pixeltab()
{
    //if(PixelTab!=NULL)
    //{
    //  free(PixelTab);
    //  PixelTab=NULL;
    //}
    return 0;
}
#endif

#if 1 //fix point, any size resample

#ifdef LOOKUPTABLE
//table resample
int Img_Resample(uint8 *InBuf, unsigned short ImgInw, unsigned short ImgInh, uint8 *outBuf,unsigned short ImgOutw,unsigned short ImgOuth)
{

    uint8 *piY;//Y in buf address
    uint8 *piU;
    uint8 *piV;
    uint8 *pitY;//temp to piY
    uint8 *pitU;
    uint8 *pitV;

    uint8 *poY;//Y out buf address
    uint8 *poU;
    uint8 *poV;

    unsigned short xScale, yScale;// ImgInw/ImgOutw and ImgOutw/ImgOuth scale

    unsigned short sX, sY;  //virtual position x y in source location
    unsigned short dX,dY;  //destination position x y

    unsigned short xi;// real position x y in source location
    unsigned short yi;

    uint8  *pxptr;
    unsigned short wt1,wt2;
    unsigned short wa,wb,wc;
    unsigned short maxScale;

    if(InBuf==NULL ||outBuf==NULL)
        return -1;
    if((!ImgInw) ||(!ImgInh)||(!ImgOutw)||(!ImgOuth))
        return -1;
    else if((ImgInw-((ImgInw>>2)<<2)) ||(ImgInh-((ImgInh>>2)<<2))||(ImgOutw-((ImgOutw>>2)<<2))||(ImgOuth-((ImgOuth>>2)<<2)))
        return -1;

    xScale =((((ImgInw<<PIXEL_SCALE_BIT)<<3)/ImgOutw)+7)>>3;
    yScale =((((ImgInh<<PIXEL_SCALE_BIT)<<3)/ImgOuth)+7)>>3;
    //xScale =(ImgInw<<PIXEL_SCALE_BIT)/ImgOutw;
    //yScale =(ImgInh<<PIXEL_SCALE_BIT)/ImgOuth;

    poY=outBuf;
    poU=outBuf+ImgOutw*ImgOuth;
    poV=outBuf+((ImgOutw*ImgOuth*5)>>2);

    piY=InBuf;
    piU=InBuf+ImgInw*ImgInh;
    piV=InBuf+((ImgInw*ImgInh*5)>>2);

    maxScale=(1<<PIXEL_SCALE_BIT);

    for(dY=0; dY<ImgOuth; dY++)
    {

        sY = dY * yScale ;
        yi=(unsigned short )(sY>>PIXEL_SCALE_BIT);
        wt2=(unsigned short)(sY-(yi<<PIXEL_SCALE_BIT));

        pitY=piY+yi*ImgInw;
        if((yi+1)<(ImgInh>>1))
        {
            pitU=piU+yi*(ImgInw>>1);
            pitV=piV+yi*(ImgInw>>1);
        }

        for(dX=0; dX<ImgOutw; dX++)
        {
            sX = dX  * xScale ;

            xi=(unsigned short )(sX>>PIXEL_SCALE_BIT);
            wt1=(unsigned short)(sX-(xi<<PIXEL_SCALE_BIT));

            if ((xi+1)<ImgInw  && (yi+1)<ImgInh )
            {

                //Y
                pxptr=pitY+xi;
                /*
                wa=PixelTab[*pxptr++][wt1];
                wa+=PixelTab[*pxptr][maxScale-wt1];

                pxptr+=ImgInw-1;//next row
                wb=PixelTab[*pxptr++][wt1];
                wb+=PixelTab[*pxptr][maxScale-wt1];
                */

                (*poY++)=LimitTab[PixelTab[LimitTab[PixelTab[*pxptr][wt1]+PixelTab[*(pxptr+1)][maxScale-wt1]]][wt2]+
                                  PixelTab[LimitTab[PixelTab[*(pxptr+ImgInw)][wt1]+PixelTab[*(pxptr+ImgInw+1)][maxScale-wt1]]][maxScale-wt2]];

                //(*poY++)=LimitTab[wc];

                if(dX<(ImgOutw>>1) && dY<(ImgOuth>>1))
                {
                    if ((xi+1)<(ImgInw>>1)  && (yi+1)<(ImgInh>>1) )
                    {
                        //U
                        pxptr=pitU+xi;
                        /*
                        wa=PixelTab[*pxptr++][wt1];
                        wa+=PixelTab[*pxptr][maxScale-wt1];

                        pxptr+=(ImgInw>>1)-1;//next row
                        wb=PixelTab[*pxptr++][wt1];
                        wb+=PixelTab[*pxptr][maxScale-wt1];

                        wc=PixelTab[LimitTab[wa]][wt2]+PixelTab[LimitTab[wb]][maxScale-wt2];
                        (*poU++)=LimitTab[wc];
                        */

                        (*poU++)=LimitTab[PixelTab[LimitTab[PixelTab[*pxptr][wt1]+PixelTab[*(pxptr+1)][maxScale-wt1]]][wt2]+
                                          PixelTab[LimitTab[PixelTab[*(pxptr+(ImgInw>>1))][wt1]+PixelTab[*(pxptr+(ImgInw>>1)+1)][maxScale-wt1]]][maxScale-wt2]];

                        //V
                        pxptr=pitV+xi;
                        /*
                        wa=PixelTab[*pxptr++][wt1];
                        wa+=PixelTab[*pxptr][maxScale-wt1];

                        pxptr+=(ImgInw>>1)-1;//next row
                        wb=PixelTab[*pxptr++][wt1];
                        wb+=PixelTab[*pxptr][maxScale-wt1];

                        wc=PixelTab[LimitTab[wa]][wt2]+PixelTab[LimitTab[wb]][maxScale-wt2];
                        (*poV++)=LimitTab[wc];
                        */

                        (*poV++)=LimitTab[PixelTab[LimitTab[PixelTab[*pxptr][wt1]+PixelTab[*(pxptr+1)][maxScale-wt1]]][wt2]+
                                          PixelTab[LimitTab[PixelTab[*(pxptr+(ImgInw>>1))][wt1]+PixelTab[*(pxptr+(ImgInw>>1)+1)][maxScale-wt1]]][maxScale-wt2]];

                    }
#if 1
                    else
                    {
                        //U
                        (*poU++)=128;
                        //V
                        (*poV++)=128;
                    }
#else
                    else if((xi+1)>=(ImgInw>>1) && (yi+1)>=(ImgInh>>1))
                    {
                        //U
                        pxptr=poU+(dY-1)*(ImgOutw>>1)+dX;
                        wa=(*pxptr);
                        //if(wa>255)wa=255;
                        pxptr=poU+dY*(ImgOutw>>1)+(dX-1);//next row
                        wb=(*pxptr);
                        //if(wb>255)wb=255;

                        pxptr=poU+dY*(ImgOutw>>1)+dX;//out put
                        (*pxptr)=(PixelTab[wa][wt2]+PixelTab[wb][maxScale-wt2]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;

                        //V
                        pxptr=poV+(dY-1)*(ImgOutw>>1)+dX;
                        wa=(*pxptr);
                        //if(wa>255)wa=255;
                        pxptr=poV+dY*(ImgOutw>>1)+(dX-1);//next row
                        wb=(*pxptr);
                        //if(wb>255)wb=255;

                        pxptr=poV+dY*(ImgOutw>>1)+dX;//out put
                        (*pxptr)=(PixelTab[wa][wt2]+PixelTab[wb][maxScale-wt2]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;

                    }
                    else if((xi+1)>=(ImgInw>>1))
                    {
                        //U
                        pxptr=piU+yi*(ImgInw>>1)+((ImgInw>>1)-1);
                        wa=(*pxptr);
                        //if(wa>255)wa=255;
                        pxptr=piU+(yi+1)*(ImgInw>>1)+((ImgInw>>1)-1);//next row
                        wb=(*pxptr);
                        //if(wb>255)wb=255;

                        pxptr=poU+dY*(ImgOutw>>1)+dX;//out put
                        (*pxptr)=(PixelTab[wa][wt2]+PixelTab[wb][maxScale-wt2]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;

                        //V
                        pxptr=piV+yi*(ImgInw>>1)+((ImgInw>>1)-1);
                        wa=(*pxptr);
                        //if(wa>255)wa=255;
                        pxptr=piV+(yi+1)*(ImgInw>>1)+((ImgInw>>1)-1);//next row
                        wb=(*pxptr);
                        //if(wb>255)wb=255;

                        pxptr=poV+dY*(ImgOutw>>1)+dX;//out put
                        (*pxptr)=(PixelTab[wa][wt2]+PixelTab[wb][maxScale-wt2]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;

                    }
                    else
                    {
                        //U
                        pxptr=piU+((ImgInh>>1)-1)*(ImgInw>>1)+xi;
                        wa=(PixelTab[*pxptr++][wt1]+PixelTab[*pxptr++][maxScale-wt1]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;
                        //if(wa>255)wa=255;

                        pxptr=poU+dY*(ImgOutw>>1)+dX;//out put
                        (*pxptr)=wa;

                        //V
                        pxptr=piV+((ImgInh>>1)-1)*(ImgInw>>1)+xi;
                        wa=(PixelTab[*pxptr++][wt1]+PixelTab[*pxptr++][maxScale-wt1]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;
                        //if(wa>255)wa=255;

                        pxptr=poV+dY*(ImgOutw>>1)+dX;//out put
                        (*pxptr)=wa;

                    }
#endif
                }
            }
#if 1
            else
            {
                (*poY++)=0;
            }
#else
            else if((xi+1)>=ImgInw && (yi+1)>=ImgInh)
            {
                //黑边不可以yuv都为0，Y U V 都为0时是绿色即 r=0 g=255 b=0
                wt1=(unsigned short)(sX-(xi<<PIXEL_SCALE_BIT));
                wt2=(unsigned short)(sY-(yi<<PIXEL_SCALE_BIT));

                //Y
                pxptr=poY+(dY-1)*ImgOutw+dX;
                wa=(*pxptr);
                //if(wa>255)wa=255;
                pxptr=poY+dY*ImgOutw+(dX-1);//next row
                wb=(*pxptr);
                //if(wb>255)wb=255;

                pxptr=poY+dY*ImgOutw+dX;//out put
                (*pxptr)=(PixelTab[wa][wt2]+PixelTab[wb][maxScale-wt2]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;
            }
            else if((xi+1)>=ImgInw)
            {
                //黑边不可以yuv都为0，Y U V 都为0时是绿色即 r=0 g=255 b=0
                wt1=(unsigned short)(sX-(xi<<PIXEL_SCALE_BIT));
                wt2=(unsigned short)(sY-(yi<<PIXEL_SCALE_BIT));

                //Y
                pxptr=piY+yi*ImgInw+(ImgInw-1);
                wa=(*pxptr);
                //if(wa>255)wa=255;
                pxptr=piY+(yi+1)*ImgInw+(ImgInw-1);//next row
                wb=(*pxptr);
                //if(wb>255)wb=255;

                pxptr=poY+dY*ImgOutw+dX;//out put
                (*pxptr)=(PixelTab[wa][wt2]+PixelTab[wb][maxScale-wt2]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;
            }
            else
            {
                //黑边不可以yuv都为0，Y U V 都为0时是绿色即 r=0 g=255 b=0
                wt1=(unsigned short)(sX-(xi<<PIXEL_SCALE_BIT));
                wt2=(unsigned short)(sY-(yi<<PIXEL_SCALE_BIT));

                //Y
                pxptr=piY+(ImgInh-1)*ImgInw+xi;
                wa=(PixelTab[*pxptr++][wt1]+PixelTab[*pxptr++][maxScale-wt1]+(1<<(PIXEL_SCALE_BIT-1)))>>PIXEL_SCALE_BIT;
                //if(wa>255)wa=255;

                pxptr=poY+dY*ImgOutw+dX;//out put
                (*pxptr)=wa;
            }
#endif

        }
    }

    return 0;
}
#endif

//calculate resample
int Img_Resample2(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, uint8 *outBuf, uint8* tempRotateBuf, unsigned int ImgOutw,unsigned int ImgOuth)
{


    unsigned int ImgInw_UV=ImgInw>>1;
    unsigned int ImgInh_UV=ImgInh>>1;
    unsigned int ImgOutw_UV=ImgOutw>>1;
    unsigned int ImgOuth_UV=ImgOuth>>1;


    uint8 *piY;//Y in buf address
    uint8 *piU;
    uint8 *piV;



    uint8 *poY;//Y out buf address
    uint8 *poU;
    uint8 *poV;

    uint8 *ptempY;//Y out buf address
    uint8 *ptempU;
    uint8*ptempV;

    uint8 *ptempY_Begin;//Y out buf address
    uint8 *ptempU_Begin;
    uint8 *ptempV_Begin;

    unsigned int xScale, yScale;// ImgInw/ImgOutw and ImgOutw/ImgOuth scale
    unsigned int dX,dY;  //destination position x y

    uint8  *pxptrY;
    uint8 *pxptrU;
    uint8  *pxptrV;

    uint8  *pxptrY_A;
    uint8  *pxptrY_B;
    uint8  *pxptrU_A;
    uint8 *pxptrU_B;
    uint8  *pxptrV_A;
    uint8  *pxptrV_B;

    unsigned int pixelY_A;
    unsigned int pixelY_B;
    unsigned int pixelU_A;
    unsigned int pixelU_B;
    unsigned int pixelV_A;
    unsigned int pixelV_B;

    unsigned int wa;
    unsigned int maxScale;

    unsigned int input_p=0;
    unsigned int weight_A=0;
    unsigned int weight_B=0;
    uint8  *temp_p=tempRotateBuf;

    if(image==NULL ||outBuf==NULL)
        return -1;
    if(image->y==NULL ||image->u==NULL ||image->v==NULL)
        return -1;
    if((!ImgInw) ||(!ImgInh)||(!ImgOutw)||(!ImgOuth))
        return -1;
    else if((ImgInw-((ImgInw>>2)<<2)) ||(ImgInh-((ImgInh>>2)<<2))||(ImgOutw-((ImgOutw>>2)<<2))||(ImgOuth-((ImgOuth>>2)<<2)))
        return -1;

    xScale =((ImgInw-1)<<(PIXEL_SCALE_BIT))/ImgOutw;
    yScale =((ImgInh-1)<<(PIXEL_SCALE_BIT))/ImgOuth;


    poY=outBuf;
    poU=outBuf+ImgOutw*ImgOuth;
    poV=outBuf+((ImgOutw*ImgOuth*5)>>2);

    piY=image->y;
    piU=image->u;
    piV=image->v;

    /************************************************************************/
    /*                              Y  ZOOM in ROW                        */
    /************************************************************************/

    ptempY=temp_p;

    maxScale=(1<<PIXEL_SCALE_BIT);

    for(dY=0; dY<ImgInh; dY++)
    {
        input_p=0;
        pxptrY=piY;

        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;

            pxptrY=piY+(input_p>>(PIXEL_SCALE_BIT));
            //asm  ("multu  %0,%1"       :  : "r" (*pxptrY++), "r" (weight_A));
            //asm ("maddu   %0,%1"   : : "r" (*pxptrY), "r" (weight_B));
            //asm  ("mflo %0" :  "=r"(wa));

            wa=(*pxptrY++)*(weight_A);
            wa+=(*pxptrY)*weight_B;
            (*ptempY++)=wa>>PIXEL_SCALE_BIT;

            input_p=input_p+xScale;

            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;

            pxptrY=piY+(input_p>>(PIXEL_SCALE_BIT));
            //asm  ("multu  %0,%1"       :  : "r" (*pxptrY++), "r" (weight_A));
            //asm ("maddu   %0,%1"   : : "r" (*pxptrY), "r" (weight_B));
            //asm  ("mflo %0" :  "=r"(wa));

            wa=(*pxptrY++)*(weight_A);
            wa+=(*pxptrY)*weight_B;
            (*ptempY++)=wa>>PIXEL_SCALE_BIT;

            input_p=input_p+xScale;



        }
        piY=piY+ImgInw +extr_width;
    }

    /************************************************************************/
    /*                              Y  ZOOM in colume                        */
    /************************************************************************/


    ptempY_Begin=temp_p;
    input_p=0;

    for(dY=0; dY<ImgOuth; dY++)
    {
        ptempY=ptempY_Begin+ImgOutw*(input_p>>PIXEL_SCALE_BIT);
        pxptrY_A=ptempY;
        pxptrY_B=ptempY+ImgOutw;
        weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
        weight_A=maxScale-weight_B;
        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            pixelY_A=(unsigned int)*pxptrY_A;
            pixelY_B=(unsigned int)*pxptrY_B;

            wa=pixelY_A*weight_A+pixelY_B*weight_B;

            //asm  volatile ("mult  %0,%1"       :  : "r" (pixelY_A), "r" (weight_A));
            pxptrY_A++;
            //asm volatile ("madd   %0,%1"   : : "r" (pixelY_B), "r" (weight_B));

            pxptrY_B++;
            //asm volatile ("mflo %0" :  "=r"(wa));

            *poY=(wa>>PIXEL_SCALE_BIT);

            poY++;
            pixelY_A=(unsigned int)*pxptrY_A;

            pixelY_B=(unsigned int)*pxptrY_B;

            wa=pixelY_A*weight_A+pixelY_B*weight_B;

            //asm  volatile ("mult  %0,%1"       :  : "r" (pixelY_A), "r" (weight_A));
            pxptrY_A++;
            //asm volatile ("madd   %0,%1"   : : "r" (pixelY_B), "r" (weight_B));
            pxptrY_B++;
            //asm volatile ("mflo %0" :  "=r"(wa));

            *poY=(wa>>PIXEL_SCALE_BIT);
            poY++;


        }

        input_p=input_p+yScale;
    }


    /************************************************************************/
    /*                              U V  ZOOM in ROW                        */
    /************************************************************************/
    ptempU=temp_p;
    ptempV=temp_p+((ImgOutw*ImgInh)>>2);

    for(dY=0; dY<ImgInh_UV; dY++)
    {
        input_p=0;
        pxptrU=piU;
        pxptrV=piV;

        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;
            wa=input_p>>(PIXEL_SCALE_BIT);
            pxptrU=piU+wa;
            pixelU_A=(unsigned int)*pxptrU;
            pxptrU++;
            pixelU_B=(unsigned int)*pxptrU;

            pxptrV=piV+wa;
            pixelV_A=(unsigned int)*pxptrV;
            pxptrV++;

            wa=pixelU_A*weight_A+pixelU_B*weight_B;

            //asm  volatile ("mult  %0,%1"       : : "r" (pixelU_A), "r" (weight_A));
            pixelV_B=(unsigned int)*pxptrV;
            //asm  volatile ("madd  %0,%1"   : : "r" (pixelU_B), "r" (weight_B));
            //asm  volatile ("mflo %0" :  "=r"(wa));

            *ptempU=wa>>PIXEL_SCALE_BIT;

            //asm  volatile  ("mult %0,%1"       : : "r" (pixelV_A), "r" (weight_A));
            ptempU++;
            //asm  volatile ("madd  %0,%1"   : : "r" (pixelV_B), "r" (weight_B));
            //asm  volatile ("mflo %0" :  "=r"(wa));

            wa=pixelV_A*weight_A+pixelV_B*weight_B;
            (*ptempV++)=wa>>PIXEL_SCALE_BIT;

            input_p=input_p+xScale;
        }
        piU=piU+(ImgInw_UV)+(extr_width>>1);
        piV=piV+(ImgInw_UV)+(extr_width>>1);
    }


    /************************************************************************/
    /*                             U V  ZOOM in colume                        */
    /************************************************************************/
    ptempU_Begin=temp_p;
    ptempV_Begin=temp_p+((ImgOutw*ImgInh)>>2);

    input_p=0;
    for(dY=0; dY<ImgOuth_UV; dY++)
    {
        ptempU=ptempU_Begin+(ImgOutw_UV)*(input_p>>PIXEL_SCALE_BIT);
        ptempV=ptempV_Begin+(ImgOutw_UV)*(input_p>>PIXEL_SCALE_BIT);

        pxptrU_A=ptempU;
        pxptrU_B=ptempU+(ImgOutw_UV);

        pxptrV_A=ptempV;
        pxptrV_B=ptempV+(ImgOutw_UV);

        weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
        weight_A=maxScale-weight_B;

        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            pixelU_A=(unsigned int)*pxptrU_A;
            pxptrU_A++;
            pixelU_B=(unsigned int)*pxptrU_B;
            pxptrU_B++;

            pixelV_A=(unsigned int)*pxptrV_A;
            pxptrV_A++;
            pixelV_B=(unsigned int)*pxptrV_B;

            //asm  volatile ("mult  %0,%1"       :  : "r" (pixelU_A), "r" (weight_A));
            pxptrV_B++;
            //asm  volatile ("madd  %0,%1"   : : "r" (pixelU_B), "r" (weight_B));
            //asm  volatile ("mflo %0" :  "=r"(wa));
            wa=pixelU_A*weight_A+pixelU_B*weight_B;

            *poU=wa>>PIXEL_SCALE_BIT;
            //asm  volatile  ("mult %0,%1"       :  : "r" (pixelV_A), "r" (weight_A));
            poU++;
            //asm  volatile ("madd  %0,%1"   : : "r" (pixelV_B), "r" (weight_B));
            //asm  volatile ("mflo %0" :  "=r"(wa));
            wa=pixelV_A*weight_A+pixelV_B*weight_B;
            (*poV++)=wa>>PIXEL_SCALE_BIT;
        }

        input_p=input_p+yScale;
    }
    return 0;
}

#if 1
static uint8 *pResampleM;
void initResample(short lcd_width)
{
#ifdef USE_INTERNAL_SRAM
    pResampleM=(uint8*)RESAMPLE_START;
#else
    pResampleM=(uint8*)mmc_MemMalloc(lcd_width*5);
#endif
}
/*****************
calculate resample with yuv to rgb565 and embed to lcd buffer
shenh.9.1
*****************/
//extern short *r_2_pix;
//extern short *g_2_pix;
//extern short *b_2_pix;

int Img_Resample_Yuv2Rgb_Embed(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, char* LcdBuf, short LcdBufWidth, short StartX, short StartY,  unsigned int ImgOutw,unsigned int ImgOuth)
{

    int32 res;
    unsigned int ImgOutw_UV=ImgOutw>>1;
    unsigned int ImgOuth_UV=ImgOuth>>1;
    unsigned int ExtrW_Y=ImgInw+extr_width;
    unsigned int ExtrW_UV=ExtrW_Y>>1;

    uint8 *piY;//Y in buf address
    uint8 *piU;
    uint8 *piV;

    unsigned int xScale, yScale;// ImgInw/ImgOutw and ImgOutw/ImgOuth scale
    unsigned int maxScale;

    unsigned int dX,dY;  //destination position x y
    unsigned int i;

    uint8  *pxptr;

    unsigned int inputRow_p;
    unsigned int inputColumnY_p;
    unsigned int inputColumnUV_p;
    unsigned int weight_A;//row
    unsigned int weight_B;
    unsigned int weight_C;//column
    unsigned int weight_D;
    unsigned int w;

    uint8 *UVrow=(uint8*)pResampleM;//temp save for row interpolate value [U1, U2, V1, V2]. ImgOutw_UV*4 bytes
    uint8 *pUVrow1, *pUVrow2;

    uint8* UVrowInter=UVrow+ImgOutw_UV*4;//temp save for first with second row UV interpolate value [U, V]. ImgOutw_UV*2 bytes
    uint8 *pUVrowInter;

    uint8 *Yrow=UVrowInter+ImgOutw_UV*2;//same as up [Y1, Y2], but need not y_rowBufInter,will directly output. ImgOutw*2 bytes
    uint8 *pYrow1, *pYrow2;

    uint8* pUVLastrow1=NULL;
    uint8* pUVLastrow2=NULL;
    uint8* pYLastrow1=NULL;
    uint8* pYLastrow2=NULL;

    uint8 UVBypassRow;
    uint8 YBypassRow;

    uint8 UVRowExchange;
    uint8 YRowExchange;

    uint16 *pRGB=(uint16*)(LcdBuf +(LcdBufWidth*StartY+StartX)*2);
    int R, G, B;
    int  CR, CB;
    int32 *CrCbrCbRow=(uint32*)(Yrow+ImgOutw*2);// temp row buf for [cr, cbr, cb].4*ImgOutw_UV*3 bytes
    int32 *pCrCbrCb;
    int32 lum, cr, cbr, cb;
    uint16 lcdExtraW=LcdBufWidth-ImgOutw;

    if(image==NULL ||LcdBuf==NULL)
        return -1;
    if(image->y==NULL ||image->u==NULL ||image->v==NULL)
        return -1;
    if((!ImgInw) ||(!ImgInh)||(!ImgOutw)||(!ImgOuth) ||(!LcdBufWidth))
        return -1;
    else if((ImgInw-((ImgInw>>2)<<2)) ||(ImgInh-((ImgInh>>2)<<2))||(ImgOutw-((ImgOutw>>2)<<2))||(ImgOuth-((ImgOuth>>2)<<2)))
        return -1;

    xScale =(ImgInw<<PIXEL_SCALE_BIT)/ImgOutw;
    yScale =(ImgInh<<PIXEL_SCALE_BIT)/ImgOuth;

    piY=image->y;
    piU=image->u;
    piV=image->v;

    maxScale=(1<<PIXEL_SCALE_BIT);

    inputColumnUV_p=0;
    inputColumnY_p=0;
    UVBypassRow=BYPASS0;
    YBypassRow=BYPASS0;
    UVRowExchange=0;
    YRowExchange=0;

    for(dY=0; dY<ImgOuth_UV; dY++)
    {
        /**UV column weight**/
        weight_D=inputColumnUV_p&Mask_SCALE_BIT;
        weight_C=maxScale-weight_D;
        /**************/

        /**UV temp row buf back to start**/
        pUVrowInter=UVrowInter;
        /***************/

        if(UVBypassRow==BYPASS1)//UV bypass first row.
        {
            /**U**/
            piU+=ExtrW_UV;
            inputRow_p=0;
            pUVLastrow2=piU;

            pUVrow2=UVrow + (1-UVRowExchange);

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                *pUVrow2=w>>PIXEL_SCALE_BIT;
                pUVrow2+=4;

                inputRow_p+=xScale;
            }

            /***** V******/
            piV+=ExtrW_UV;
            inputRow_p=0;
            pUVrow2=UVrow + (1-UVRowExchange)+2;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**V row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                *pUVrow2=w>>PIXEL_SCALE_BIT;
                pUVrow2+=4;

                inputRow_p+=xScale;
            }

        }
        else if(UVBypassRow==BYPASS0)//no bypass row.
        {
            /***** U row1 ******/
            inputRow_p=0;
            pUVLastrow1=piU;

            pUVrow1=UVrow+UVRowExchange;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                (*pUVrow1)=w>>PIXEL_SCALE_BIT;
                pUVrow1+=4;

                inputRow_p+=xScale;
            }

            piU+=ExtrW_UV;
            inputRow_p=0;
            pUVLastrow2=piU;

            pUVrow2=UVrow +(1-UVRowExchange);

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                *pUVrow2=w>>PIXEL_SCALE_BIT;
                pUVrow2+=4;

                inputRow_p+=xScale;

            }


            /***** V row1 ******/
            inputRow_p=0;
            pUVrow1=UVrow+UVRowExchange+2;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                (*pUVrow1)=w>>PIXEL_SCALE_BIT;
                pUVrow1+=4;

                inputRow_p+=xScale;
            }

            piV+=ExtrW_UV;
            inputRow_p=0;

            pUVrow2=UVrow+(1-UVRowExchange)+2;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**V row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                *pUVrow2=w>>PIXEL_SCALE_BIT;
                pUVrow2+=4;

                inputRow_p+=xScale;
            }

        }

        pUVrow1=UVrow + UVRowExchange;
        pUVrow2=UVrow + (1-UVRowExchange);
        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            /***** U row inter ******/
            w=(*pUVrow1)*weight_C;
            pUVrow1+=2;
            w+=(*pUVrow2)*weight_D;
            pUVrow2+=2;
            (*pUVrowInter++)=w>>PIXEL_SCALE_BIT;

            /***** V row inter ******/
            w=(*pUVrow1)*weight_C;
            pUVrow1+=2;
            w+=(*pUVrow2)*weight_D;
            pUVrow2+=2;
            (*pUVrowInter++)=w>>PIXEL_SCALE_BIT;
        }

        /***UV next process***/
        inputColumnUV_p+=yScale;
        res=ExtrW_UV*(inputColumnUV_p>>PIXEL_SCALE_BIT);
        piU=image->u+res;
        piV=image->v+res;

        /***************************
        bypass up neighbor row calculate
        use U judge, V the same.
        ***************************/
        if(piU==pUVLastrow2)
        {
            pUVLastrow1=piU;

            UVRowExchange=1-UVRowExchange;// row1=row2 row2=row1

            UVBypassRow=BYPASS1;//UV bypass first row.
        }
        else if(piU==pUVLastrow1)
        {
            //row1 row2 hold last state
            UVBypassRow=BYPASS2;//UV bypass both first and second row.
        }
        else
        {
            UVRowExchange=0;//row1=1 row2=2
            UVBypassRow=BYPASS0;//no bypass row.
        }
        /*****UV end******/


        /** get cr cbr cb first into temp buffer for yuv2rgb**/
        pUVrowInter=UVrowInter;
        pCrCbrCb=CrCbrCbRow;
        for(i=0; i<ImgOutw_UV; i++)
        {
            CB=(int)*pUVrowInter++;//u
            CR=(int)*pUVrowInter++;//v
            *pCrCbrCb++=409 * CR - 56992;//cr
            *pCrCbrCb++=34784 -100 * CB -208 * CR ;//cbr
            *pCrCbrCb++=516 * CB - 70688;//cb
        }
        /**************/

        /** interpolate Y and output rgb565 **/
        for(i=0; i<2; i++) // calculate 2 rows Y
        {
            /***Y column weight***/
            weight_D=inputColumnY_p&Mask_SCALE_BIT;
            weight_C=maxScale-weight_D;
            /***************/

            if(YBypassRow==BYPASS1)//Y bypass first row.
            {
                /***** Y row2 and inter ******/
                piY+=ExtrW_Y;
                inputRow_p=0;
                pYLastrow2=piY;
                pYrow2=Yrow + (1-YRowExchange);

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    /**Y1 row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    *pYrow2=w>>PIXEL_SCALE_BIT;
                    pYrow2+=2;

                    inputRow_p+=xScale;

                    /**Y2 row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    *pYrow2=w>>PIXEL_SCALE_BIT;
                    pYrow2+=2;

                    inputRow_p+=xScale;
                }
            }
            else if(YBypassRow==BYPASS0)//no bypass row.
            {
                /***** Y row1 ******/
                inputRow_p=0;
                pYLastrow1=piY;
                pYrow1=Yrow + YRowExchange;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    // Y1
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    (*pYrow1)=w>>PIXEL_SCALE_BIT;
                    pYrow1+=2;

                    inputRow_p+=xScale;

                    // Y2
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    (*pYrow1)=w>>PIXEL_SCALE_BIT;
                    pYrow1+=2;

                    inputRow_p+=xScale;
                }

                /***** Y row2 and inter ******/
                piY+=ExtrW_Y;
                inputRow_p=0;
                pYLastrow2=piY;

                pYrow2=Yrow + (1-YRowExchange);

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    /**Y1 row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    *pYrow2=w>>PIXEL_SCALE_BIT;
                    pYrow2+=2;

                    inputRow_p+=xScale;

                    /**Y2 row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    *pYrow2=w>>PIXEL_SCALE_BIT;
                    pYrow2+=2;

                    inputRow_p+=xScale;
                }
            }


            pYrow1=Yrow + YRowExchange;
            pYrow2=Yrow + (1-YRowExchange);
            pCrCbrCb=CrCbrCbRow;

            /***** Y row inter ******/
            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                cr=*pCrCbrCb++;
                cbr=*pCrCbrCb++;
                cb=*pCrCbrCb++;

                // Y1
                w=(*pYrow1)*weight_C;
                pYrow1+=2;
                w+=(*pYrow2)*weight_D;
                pYrow2+=2;
                lum=298*(w>>PIXEL_SCALE_BIT);

                R=( lum + cr)>> 8;
                G=( lum+cbr)>> 8;
                B=( lum + cb)>> 8;
                (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);


                // Y2
                w=(*pYrow1)*weight_C;
                pYrow1+=2;
                w+=(*pYrow2)*weight_D;
                pYrow2+=2;
                lum=298*(w>>PIXEL_SCALE_BIT);

                R=( lum + cr)>> 8;
                G=( lum+cbr)>> 8;
                B=( lum + cb)>> 8;
                (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
            }


            /***Y next process***/
            inputColumnY_p+=yScale;
            piY=image->y+ExtrW_Y*(inputColumnY_p>>PIXEL_SCALE_BIT);

            /***************************
            bypass up neighbor row calculate
            ***************************/
            if(piY==pYLastrow2)
            {
                pYLastrow1=piY;

                YRowExchange=1-YRowExchange;

                YBypassRow=BYPASS1;//Y bypass first row.
            }
            else if(piY==pYLastrow1)
            {
                YBypassRow=BYPASS2;//Y bypass both first and second row.
            }
            else
            {
                YRowExchange=0;
                YBypassRow=BYPASS0;//no bypass row.
            }
            /*****Y end******/

            pRGB+=lcdExtraW;
        }

    }

    return 0;
}
#endif


#if 0
/*****************
calculate resample
different from Img_Resample2 and higher efficiency
use temp buf or sram size= (out width row bytes)*5
shenh.8.29
*****************/

extern short *r_2_pix;
extern short *g_2_pix;
extern short *b_2_pix;

int Img_Resample_Yuv2Rgb_Embed(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, char* LcdBuf, short LcdBufWidth, short StartX, short StartY,  unsigned int ImgOutw,unsigned int ImgOuth)
{

    int32 res;
    unsigned int ImgOutw_UV=ImgOutw>>1;
    unsigned int ImgOuth_UV=ImgOuth>>1;
    unsigned int ExtrW_Y=ImgInw+extr_width;
    unsigned int ExtrW_UV=ExtrW_Y>>1;

    uint8 *piY;//Y in buf address
    uint8 *piU;
    uint8 *piV;

    unsigned int xScale, yScale;// ImgInw/ImgOutw and ImgOutw/ImgOuth scale
    unsigned int dX,dY;  //destination position x y
    unsigned int i;

    uint8  *pxptr;

    unsigned int w;
    unsigned int maxScale;

    unsigned int inputRow_p;
    unsigned int inputColumnY_p;
    unsigned int inputColumnUV_p;
    unsigned int weight_A;//row
    unsigned int weight_B;
    unsigned int weight_C;//column
    unsigned int weight_D;

    //uint8  *temp_p=tempRotateBuf;
    uint8 *Urow1=(uint8*)0x80c080b0;//0x80c068b0;//temp save for first row interpolate value. ImgOutw_UV bytes
    uint8 *pUrow1;
    uint8* Urow2=Urow1+ImgOutw_UV;//temp save for second row interpolate value. ImgOutw_UV bytes
    uint8 *pUrow2;
    uint8* UrowInter=Urow2+ImgOutw_UV;//temp save for first with second row interpolate value. ImgOutw_UV bytes
    uint8 *pUrowInter;

    uint8 *Vrow1=UrowInter+ImgOutw_UV;//same as up. ImgOutw_UV bytes
    uint8 *pVrow1;
    uint8* Vrow2=Vrow1+ImgOutw_UV;//ImgOutw_UV bytes
    uint8 *pVrow2;
    uint8* VrowInter=Vrow2+ImgOutw_UV;//ImgOutw_UV bytes
    uint8* pVrowInter;

    uint8 *Yrow1=VrowInter+ImgOutw_UV;//same as up, but need not y_rowBufInter,will directly output. ImgOutw bytes
    uint8 *pYrow1;
    uint8* Yrow2=Yrow1+ImgOutw;//ImgOutw bytes
    uint8 *pYrow2;

    uint8* pUVLastrow1;
    uint8* pUVLastrow2;
    uint8* pYLastrow1;
    uint8* pYLastrow2;
    uint8* pTemp;

    uint8 UVBypassRow;
    uint8 YBypassRow;

    uint16 *pRGB=(uint16*)(LcdBuf +(LcdBufWidth*StartY+StartX)*2);
    int R, G, B;
    int  CR, CB;
    int lum,cr,cbr,cb;
    uint16 lcdExtraW=LcdBufWidth-ImgOutw;

    diag_printf( " LcdBufWidth=%d  StartX=%d   StartY=%d", LcdBufWidth,  StartX,  StartY);

    if(image==NULL ||LcdBuf==NULL)
        return -1;
    if(image->y==NULL ||image->u==NULL ||image->v==NULL)
        return -1;
    if((!ImgInw) ||(!ImgInh)||(!ImgOutw)||(!ImgOuth))
        return -1;
    else if((ImgInw-((ImgInw>>2)<<2)) ||(ImgInh-((ImgInh>>2)<<2))||(ImgOutw-((ImgOutw>>2)<<2))||(ImgOuth-((ImgOuth>>2)<<2)))
        return -1;

    xScale =(ImgInw<<PIXEL_SCALE_BIT)/ImgOutw;
    yScale =(ImgInh<<PIXEL_SCALE_BIT)/ImgOuth;

    piY=image->y;
    piU=image->u;
    piV=image->v;

    maxScale=(1<<PIXEL_SCALE_BIT);

    inputColumnUV_p=0;
    inputColumnY_p=0;
    UVBypassRow=BYPASS0;

    for(dY=0; dY<ImgOuth_UV; dY++)
    {
        /**UV column weight**/
        weight_D=inputColumnUV_p&Mask_SCALE_BIT;
        weight_C=maxScale-weight_D;
        /**************/

        if(UVBypassRow==BYPASS2)//UV bypass both first and second row.
        {
            /***** U row inter ******/
            pUrow1=Urow1;
            pUrow2=Urow2;
            pUrowInter=UrowInter;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row inter**/
                w=(*pUrow1++)*weight_C;
                w+=(*pUrow2++)*weight_D;
                (*pUrowInter++)=w>>PIXEL_SCALE_BIT;
            }

            /***** V row inter ******/
            pVrow1=Vrow1;
            pVrow2=Vrow2;
            pVrowInter=VrowInter;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row inter**/
                w=(*pVrow1++)*weight_C;
                w+=(*pVrow2++)*weight_D;
                (*pVrowInter++)=w>>PIXEL_SCALE_BIT;
            }
        }
        else if(UVBypassRow==BYPASS1)//UV bypass first row.
        {
            /***** U row2 and inter ******/
            piU+=ExtrW_UV;
            inputRow_p=0;
            pUVLastrow2=piU;

            pUrow1=Urow1;
            pUrow2=Urow2;
            pUrowInter=UrowInter;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pUrow2++)=w;

                /**U row inter**/
                w*=weight_D;
                w+=(*pUrow1++)*weight_C;
                (*pUrowInter++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;

            }

            /***** V row2 and inter ******/
            piV+=ExtrW_UV;
            inputRow_p=0;

            pVrow1=Vrow1;
            pVrow2=Vrow2;
            pVrowInter=VrowInter;


            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**V row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pVrow2++)=w;

                /**V row inter**/
                w*=weight_D;
                w+=(*pVrow1++)*weight_C;
                (*pVrowInter++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;

            }

        }
        else//no bypass row.
        {
            /***** U row1 ******/
            inputRow_p=0;
            pUVLastrow1=piU;

            pUrow1=Urow1;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                (*pUrow1++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;
            }

            /***** U row2 and inter ******/
            piU+=ExtrW_UV;
            inputRow_p=0;
            pUVLastrow2=piU;

            pUrow1=Urow1;
            pUrow2=Urow2;
            pUrowInter=UrowInter;


            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pUrow2++)=w;

                /**U row inter**/
                w*=weight_D;
                w+=(*pUrow1++)*weight_C;
                (*pUrowInter++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;

            }


            /***** V row1 ******/
            inputRow_p=0;

            pVrow1=Vrow1;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                (*pVrow1++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;
            }

            /***** V row2 and inter ******/
            piV+=ExtrW_UV;
            inputRow_p=0;

            pVrow1=Vrow1;
            pVrow2=Vrow2;
            pVrowInter=VrowInter;


            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**V row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pVrow2++)=w;

                /**V row inter**/
                w*=weight_D;
                w+=(*pVrow1++)*weight_C;
                (*pVrowInter++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;
            }

        }

        /***UV next process***/
        inputColumnUV_p+=yScale;
        res=ExtrW_UV*(inputColumnUV_p>>PIXEL_SCALE_BIT);
        piU=image->u+res;
        piV=image->v+res;

        /***************************
        bypass up neighbor row calculate
        use U judge, V the same.
        ***************************/
        if(piU==pUVLastrow2)
        {
            pUVLastrow1=piU;

            /**Urow1<-->Urow2**/
            pTemp=Urow1;
            Urow1=Urow2;
            Urow2=pTemp;
            /**Vrow1<-->Vrow2**/
            pTemp=Vrow1;
            Vrow1=Vrow2;
            Vrow2=pTemp;

            UVBypassRow=BYPASS1;//UV bypass first row.
        }
        else if(piU==pUVLastrow1)
        {
            UVBypassRow=BYPASS2;//UV bypass both first and second row.
        }
        else
        {
            UVBypassRow=BYPASS0;//no bypass row.
        }
        /*****UV end******/


        for(i=0; i<2; i++) // calculate 2 rows Y
        {
            /***Y column weight***/
            weight_D=inputColumnY_p&Mask_SCALE_BIT;
            weight_C=maxScale-weight_D;
            /***************/
            pUrowInter=UrowInter;
            pVrowInter=VrowInter;

            if(YBypassRow==BYPASS2)//Y bypass both first and second row.
            {
                /***** Y row inter ******/
                pYrow1=Yrow1;
                pYrow2=Yrow2;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    CB=(int)*pUrowInter++;
                    CR=(int)*pVrowInter++;
                    cr=409 * CR - 56992;
                    cbr=34784 -100 * CB -208 * CR ;
                    cb=516 * CB - 70688;

                    // 1
                    w=(*pYrow1++)*weight_C;
                    w+=(*pYrow2++)*weight_D;
                    lum=298*(w>>PIXEL_SCALE_BIT);

                    R=( lum + cr)>> 8;
                    G=( lum+cbr)>> 8;
                    B=( lum + cb)>> 8;
                    (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);


                    // 2
                    w=(*pYrow1++)*weight_C;
                    w+=(*pYrow2++)*weight_D;
                    lum=298*(w>>PIXEL_SCALE_BIT);

                    R=( lum + cr)>> 8;
                    G=( lum+cbr)>> 8;
                    B=( lum + cb)>> 8;
                    (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);
                }
            }
            else if(YBypassRow==BYPASS1)//Y bypass first row.
            {
                /***** Y row2 and inter ******/
                piY+=ExtrW_Y;
                inputRow_p=0;
                pYLastrow2=piY;

                pYrow1=Yrow1;
                pYrow2=Yrow2;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    CB=(int)*pUrowInter++;
                    CR=(int)*pVrowInter++;
                    cr=409 * CR - 56992;
                    cbr=34784 -100 * CB -208 * CR ;
                    cb=516 * CB - 70688;

                    // 1
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    lum=298*(w>>PIXEL_SCALE_BIT);

                    R=( lum + cr)>> 8;
                    G=( lum+cbr)>> 8;
                    B=( lum + cb)>> 8;
                    (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

                    inputRow_p+=xScale;

                    // 2
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    lum=298*(w>>PIXEL_SCALE_BIT);

                    R=( lum + cr)>> 8;
                    G=( lum+cbr)>> 8;
                    B=( lum + cb)>> 8;
                    (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

                    inputRow_p+=xScale;
                }
            }
            else//no bypass row.
            {
                /***** Y row1 ******/
                inputRow_p=0;
                pYLastrow1=piY;

                pYrow1=Yrow1;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    // 1
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    (*pYrow1++)=w>>PIXEL_SCALE_BIT;

                    inputRow_p+=xScale;

                    // 2
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    (*pYrow1++)=w>>PIXEL_SCALE_BIT;

                    inputRow_p+=xScale;
                }

                /***** Y row2 and inter ******/
                piY+=ExtrW_Y;
                inputRow_p=0;
                pYLastrow2=piY;

                pYrow1=Yrow1;
                pYrow2=Yrow2;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    CB=(int)*pUrowInter++;
                    CR=(int)*pVrowInter++;
                    cr=409 * CR - 56992;
                    cbr=34784 -100 * CB -208 * CR ;
                    cb=516 * CB - 70688;

                    // 1
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    lum=298*(w>>PIXEL_SCALE_BIT);

                    R=( lum + cr)>> 8;
                    G=( lum+cbr)>> 8;
                    B=( lum + cb)>> 8;
                    (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

                    inputRow_p+=xScale;

                    // 2
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    lum=298*(w>>PIXEL_SCALE_BIT);

                    R=( lum + cr)>> 8;
                    G=( lum+cbr)>> 8;
                    B=( lum + cb)>> 8;
                    (*pRGB++) =(r_2_pix[R] | g_2_pix[G] | b_2_pix[B]);

                    inputRow_p+=xScale;
                }
            }

            /***Y next process***/
            inputColumnY_p+=yScale;
            piY=image->y+ExtrW_Y*(inputColumnY_p>>PIXEL_SCALE_BIT);

            /***************************
            bypass up neighbor row calculate
            ***************************/
            if(piY==pYLastrow2)
            {
                pYLastrow1=piY;

                /**Urow1<-->Urow2**/
                pTemp=Yrow1;
                Yrow1=Yrow2;
                Yrow2=pTemp;

                YBypassRow=BYPASS1;//Y bypass first row.
            }
            else if(piY==pYLastrow1)
            {
                YBypassRow=BYPASS2;//Y bypass both first and second row.
            }
            else
            {
                YBypassRow=BYPASS0;//no bypass row.
            }
            /*****Y end******/

            pRGB+=lcdExtraW;
        }

    }

    return 0;
}
#endif

#if 0
/*****************
calculate resample
different from Img_Resample2 and higher efficiency
use temp buf or sram size= (out width row bytes)*5
shenh.8.29
*****************/
int Img_Resample3(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, char* LcdBuf, short LcdBufWidth, short StartX, short StartY,  unsigned int ImgOutw,unsigned int ImgOuth)
{

    int32 res;
    //unsigned int ImgInw_UV=ImgInw>>1;
    //unsigned int ImgInh_UV=ImgInh>>1;
    unsigned int ImgOutw_UV=ImgOutw>>1;
    unsigned int ImgOuth_UV=ImgOuth>>1;
    unsigned int ExtrW_Y=ImgInw+extr_width;
    unsigned int ExtrW_UV=ExtrW_Y>>1;

    uint8 *piY;//Y in buf address
    uint8 *piU;
    uint8 *piV;

    unsigned int xScale, yScale;// ImgInw/ImgOutw and ImgOutw/ImgOuth scale
    unsigned int dX,dY;  //destination position x y
    unsigned int i;

    uint8  *pxptr;

    unsigned int w;
    unsigned int maxScale;

    unsigned int inputRow_p;
    unsigned int inputColumnY_p;
    unsigned int inputColumnUV_p;
    unsigned int weight_A;//row
    unsigned int weight_B;
    unsigned int weight_C;//column
    unsigned int weight_D;

    //uint8  *temp_p=tempRotateBuf;
    uint8 *Urow1=(uint8*)0x80c068b0;//temp save for first row interpolate value. ImgOutw_UV bytes
    uint8 *pUrow1;
    uint8* Urow2=Urow1+ImgOutw_UV;//temp save for second row interpolate value. ImgOutw_UV bytes
    uint8 *pUrow2;
    uint8* UrowInter=Urow2+ImgOutw_UV;//temp save for first with second row interpolate value. ImgOutw_UV bytes
    uint8 *pUrowInter;

    uint8 *Vrow1=UrowInter+ImgOutw_UV;//same as up. ImgOutw_UV bytes
    uint8 *pVrow1;
    uint8* Vrow2=Vrow1+ImgOutw_UV;//ImgOutw_UV bytes
    uint8 *pVrow2;
    uint8* VrowInter=Vrow2+ImgOutw_UV;//ImgOutw_UV bytes
    uint8* pVrowInter;

    uint8 *Yrow1=VrowInter+ImgOutw_UV;//same as up, but need not y_rowBufInter,will directly output. ImgOutw bytes
    uint8 *pYrow1;
    uint8* Yrow2=Yrow1+ImgOutw;//ImgOutw bytes
    uint8 *pYrow2;

    uint8* pUVLastrow1;
    uint8* pUVLastrow2;
    uint8* pYLastrow1;
    uint8* pYLastrow2;
    uint8* pTemp;

    uint8 UVBypassRow;
    uint8 YBypassRow;

    //test out yuv
    char *YOutTest=LcdBuf;
    char *UOutTest=YOutTest+ImgOutw*ImgOuth;
    char *VOutTest=UOutTest+ImgOutw_UV*ImgOuth_UV;
    //


    if(image==NULL ||LcdBuf==NULL)
        return -1;
    if(image->y==NULL ||image->u==NULL ||image->v==NULL)
        return -1;
    if((!ImgInw) ||(!ImgInh)||(!ImgOutw)||(!ImgOuth))
        return -1;
    else if((ImgInw-((ImgInw>>2)<<2)) ||(ImgInh-((ImgInh>>2)<<2))||(ImgOutw-((ImgOutw>>2)<<2))||(ImgOuth-((ImgOuth>>2)<<2)))
        return -1;

    xScale =(ImgInw<<PIXEL_SCALE_BIT)/ImgOutw;
    yScale =(ImgInh<<PIXEL_SCALE_BIT)/ImgOuth;

    piY=image->y;
    piU=image->u;
    piV=image->v;

    maxScale=(1<<PIXEL_SCALE_BIT);

    inputColumnUV_p=0;
    inputColumnY_p=0;
    UVBypassRow=BYPASS0;

    for(dY=0; dY<ImgOuth_UV; dY++)
    {
        /**UV column weight**/
        weight_D=inputColumnUV_p&Mask_SCALE_BIT;
        weight_C=maxScale-weight_D;
        /**************/

        if(UVBypassRow==BYPASS2)//UV bypass both first and second row.
        {
            /***** U row inter ******/
            pUrow1=Urow1;
            pUrow2=Urow2;
            pUrowInter=UrowInter;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row inter**/
                w=(*pUrow1++)*weight_C;
                w+=(*pUrow2++)*weight_D;
                //(*pUrowInter++)=w>>PIXEL_SCALE_BIT;
                (*UOutTest++)=w>>PIXEL_SCALE_BIT;//test
            }

            /***** V row inter ******/
            pVrow1=Vrow1;
            pVrow2=Vrow2;
            pVrowInter=VrowInter;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row inter**/
                w=(*pVrow1++)*weight_C;
                w+=(*pVrow2++)*weight_D;
                //(*pUrowInter++)=w>>PIXEL_SCALE_BIT;
                (*VOutTest++)=w>>PIXEL_SCALE_BIT;//test
            }
        }
        else if(UVBypassRow==BYPASS1)//UV bypass first row.
        {
            /***** U row2 and inter ******/
            piU+=ExtrW_UV;
            inputRow_p=0;
            pUVLastrow2=piU;

            pUrow1=Urow1;
            pUrow2=Urow2;
            pUrowInter=UrowInter;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pUrow2++)=w;

                /**U row inter**/
                w*=weight_D;
                w+=(*pUrow1++)*weight_C;
                //(*pUrowInter++)=w>>PIXEL_SCALE_BIT;
                (*UOutTest++)=w>>PIXEL_SCALE_BIT;//test

                inputRow_p+=xScale;

            }

            /***** V row2 and inter ******/
            piV+=ExtrW_UV;
            inputRow_p=0;

            pVrow1=Vrow1;
            pVrow2=Vrow2;
            pVrowInter=VrowInter;


            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**V row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pVrow2++)=w;

                /**V row inter**/
                w*=weight_D;
                w+=(*pVrow1++)*weight_C;
                //(*pVrowInter++)=w>>PIXEL_SCALE_BIT;
                (*VOutTest++)=w>>PIXEL_SCALE_BIT;//test

                inputRow_p+=xScale;

            }

        }
        else//no bypass row.
        {
            /***** U row1 ******/
            inputRow_p=0;
            pUVLastrow1=piU;

            pUrow1=Urow1;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                (*pUrow1++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;
            }

            /***** U row2 and inter ******/
            piU+=ExtrW_UV;
            inputRow_p=0;
            pUVLastrow2=piU;

            pUrow1=Urow1;
            pUrow2=Urow2;
            pUrowInter=UrowInter;


            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**U row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piU+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pUrow2++)=w;

                /**U row inter**/
                w*=weight_D;
                w+=(*pUrow1++)*weight_C;
                //(*pUrowInter++)=w>>PIXEL_SCALE_BIT;
                (*UOutTest++)=w>>PIXEL_SCALE_BIT;//test

                inputRow_p+=xScale;

            }


            /***** V row1 ******/
            inputRow_p=0;

            pVrow1=Vrow1;

            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                (*pVrow1++)=w>>PIXEL_SCALE_BIT;

                inputRow_p+=xScale;
            }

            /***** V row2 and inter ******/
            piV+=ExtrW_UV;
            inputRow_p=0;

            pVrow1=Vrow1;
            pVrow2=Vrow2;
            pVrowInter=VrowInter;


            for(dX=0; dX<ImgOutw_UV; dX++)
            {
                /**V row2**/
                weight_B=inputRow_p&Mask_SCALE_BIT;
                weight_A=maxScale-weight_B;

                pxptr=piV+(inputRow_p>>PIXEL_SCALE_BIT);

                w=(*pxptr++)*(weight_A);
                w+=(*pxptr)*weight_B;
                w>>=PIXEL_SCALE_BIT;
                (*pVrow2++)=w;

                /**V row inter**/
                w*=weight_D;
                w+=(*pVrow1++)*weight_C;
                //(*pVrowInter++)=w>>PIXEL_SCALE_BIT;
                (*VOutTest++)=w>>PIXEL_SCALE_BIT;//test

                inputRow_p+=xScale;
            }

        }

        /***UV next process***/
        inputColumnUV_p+=yScale;
        res=ExtrW_UV*(inputColumnUV_p>>PIXEL_SCALE_BIT);
        piU=image->u+res;
        piV=image->v+res;

        /***************************
        bypass up neighbor row calculate
        use U judge, V the same.
        ***************************/
        if(piU==pUVLastrow2)
        {
            pUVLastrow1=piU;

            /**Urow1<-->Urow2**/
            pTemp=Urow1;
            Urow1=Urow2;
            Urow2=pTemp;
            /**Vrow1<-->Vrow2**/
            pTemp=Vrow1;
            Vrow1=Vrow2;
            Vrow2=pTemp;

            UVBypassRow=BYPASS1;//UV bypass first row.
        }
        else if(piU==pUVLastrow1)
        {
            UVBypassRow=BYPASS2;//UV bypass both first and second row.
        }
        else
        {
            UVBypassRow=BYPASS0;//no bypass row.
        }
        /*****UV end******/


        for(i=0; i<2; i++) // calculate 2 rows Y
        {
            /***Y column weight***/
            weight_D=inputColumnY_p&Mask_SCALE_BIT;
            weight_C=maxScale-weight_D;
            /***************/

            if(YBypassRow==BYPASS2)//Y bypass both first and second row.
            {
                /***** Y row inter ******/
                pYrow1=Yrow1;
                pYrow2=Yrow2;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    // 1
                    w=(*pYrow1++)*weight_C;
                    w+=(*pYrow2++)*weight_D;
                    (*YOutTest++)=w>>PIXEL_SCALE_BIT;//test

                    // 2
                    w=(*pYrow1++)*weight_C;
                    w+=(*pYrow2++)*weight_D;
                    (*YOutTest++)=w>>PIXEL_SCALE_BIT;//test
                }
            }
            else if(YBypassRow==BYPASS1)//Y bypass first row.
            {
                /***** Y row2 and inter ******/
                piY+=ExtrW_Y;
                inputRow_p=0;
                pYLastrow2=piY;

                pYrow1=Yrow1;
                pYrow2=Yrow2;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    // 1
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    (*YOutTest++)=w>>PIXEL_SCALE_BIT;//test

                    inputRow_p+=xScale;

                    // 2
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    (*YOutTest++)=w>>PIXEL_SCALE_BIT;//test

                    inputRow_p+=xScale;
                }
            }
            else//no bypass row.
            {
                /***** Y row1 ******/
                inputRow_p=0;
                pYLastrow1=piY;

                pYrow1=Yrow1;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    // 1
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    (*pYrow1++)=w>>PIXEL_SCALE_BIT;

                    inputRow_p+=xScale;

                    // 2
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    (*pYrow1++)=w>>PIXEL_SCALE_BIT;

                    inputRow_p+=xScale;
                }

                /***** Y row2 and inter ******/
                piY+=ExtrW_Y;
                inputRow_p=0;
                pYLastrow2=piY;

                pYrow1=Yrow1;
                pYrow2=Yrow2;

                for(dX=0; dX<ImgOutw_UV; dX++)
                {
                    // 1
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    (*YOutTest++)=w>>PIXEL_SCALE_BIT;//test

                    inputRow_p+=xScale;

                    // 2
                    /**Y row2**/
                    weight_B=inputRow_p&Mask_SCALE_BIT;
                    weight_A=maxScale-weight_B;

                    pxptr=piY+(inputRow_p>>PIXEL_SCALE_BIT);

                    w=(*pxptr++)*(weight_A);
                    w+=(*pxptr)*weight_B;
                    w=w>>PIXEL_SCALE_BIT;
                    (*pYrow2++)=w;

                    /**Y row inter**/
                    w*=weight_D;
                    w+=(*pYrow1++)*weight_C;
                    (*YOutTest++)=w>>PIXEL_SCALE_BIT;//test

                    inputRow_p+=xScale;
                }
            }

            /***Y next process***/
            inputColumnY_p+=yScale;
            piY=image->y+ExtrW_Y*(inputColumnY_p>>PIXEL_SCALE_BIT);

            /***************************
            bypass up neighbor row calculate
            ***************************/
            if(piY==pYLastrow2)
            {
                pYLastrow1=piY;

                /**Urow1<-->Urow2**/
                pTemp=Yrow1;
                Yrow1=Yrow2;
                Yrow2=pTemp;

                YBypassRow=BYPASS1;//Y bypass first row.
            }
            else if(piY==pYLastrow1)
            {
                YBypassRow=BYPASS2;//Y bypass both first and second row.
            }
            else
            {
                YBypassRow=BYPASS0;//no bypass row.
            }
            /*****Y end******/
        }

    }

    return 0;
}
#endif


//resample and rotate 90.
int Img_Resample2_rotate_closewise(yuvImage * image, unsigned int ImgInw, unsigned int ImgInh, unsigned int extr_width, uint8 *outBuf, uint8* tempRotateBuf, unsigned int ImgOutw,unsigned int ImgOuth)
{
    //unsigned int ImgInw_UV=(ImgInw>>1);
    unsigned int ImgInh_UV=(ImgInh>>1);
    unsigned int ImgOutw_UV=(ImgOutw>>1);
    unsigned int ImgOuth_UV=(ImgOuth>>1);
    unsigned int edged_width=ImgInw+extr_width;
    unsigned int edged_width2=(edged_width>>1);

    uint8 *piY;//Y in buf address
    uint8 *piU;
    uint8 *piV;

    uint8 *poY;//Y out buf address
    uint8 *poU;
    uint8 *poV;

    uint8 *ptempY;//Y out buf address
    uint8 *ptempU;
    uint8 *ptempV;

    unsigned int xScale, yScale;// ImgInw/ImgOutw and ImgOutw/ImgOuth scale
    unsigned int dX,dY;  //destination position x y

    uint8  *pxptrY;
    uint8  *pxptrU;
    uint8  *pxptrV;

    unsigned int wa;
    unsigned int maxScale;

    unsigned int input_p=0;
    unsigned int weight_A=0;
    unsigned int weight_B=0;
    uint8  *temp_p1;//=tempRotateBuf;
    uint8  *temp_p2;

    if(image==NULL ||outBuf==NULL || tempRotateBuf==NULL)
        return -1;
    if(image->y==NULL ||image->u==NULL ||image->v==NULL)
        return -1;
    if((!ImgInw) ||(!ImgInh)||(!ImgOutw)||(!ImgOuth))
        return -1;
    else if((ImgInw-((ImgInw>>1)<<1)) ||(ImgInh-((ImgInh>>1)<<1))||(ImgOutw-((ImgOutw>>1)<<1))||(ImgOuth-((ImgOuth>>1)<<1)))
        return -1;

    xScale =(ImgInw<<PIXEL_SCALE_BIT)/ImgOutw;
    yScale =(ImgInh<<PIXEL_SCALE_BIT)/ImgOuth;
    maxScale=(1<<PIXEL_SCALE_BIT);

    /************************************************************************/
    /*                              Y  ZOOM in ROW  and Rotate                      */
    /************************************************************************/
    piY=image->y;
    temp_p1=tempRotateBuf+ImgInh-1;

    for(dY=0; dY<ImgInh; dY++)
    {
        input_p=0;
        ptempY=temp_p1-dY;

        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            //one
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;
            pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrY++)*weight_A;
            wa+=(*pxptrY)*weight_B;
            (*ptempY)=(wa>>PIXEL_SCALE_BIT);
            ptempY+=ImgInh;
            input_p+=xScale;

            //two
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;
            pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrY++)*weight_A;
            wa+=(*pxptrY)*weight_B;
            (*ptempY)=(wa>>PIXEL_SCALE_BIT);
            ptempY+=ImgInh;
            input_p+=xScale;
        }
        piY+=edged_width;
    }

    /************************************************************************/
    /*                              Y  ZOOM in colume                        */
    /************************************************************************/
    poY=outBuf;
    piY=tempRotateBuf;

    for(dY=0; dY<ImgOutw; dY++)
    {
        input_p=0;

        for(dX=0; dX<ImgOuth_UV; dX++)
        {
            //one
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;
            pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrY++)*weight_A;
            wa+=(*pxptrY)*weight_B;
            (*poY++)=(wa>>PIXEL_SCALE_BIT);
            input_p+=yScale;

            //two
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;
            pxptrY=piY+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrY++)*weight_A;
            wa+=(*pxptrY)*weight_B;
            (*poY++)=(wa>>PIXEL_SCALE_BIT);
            input_p+=yScale;

        }
        piY+=ImgInh;
    }

    /************************************************************************/
    /*                              U V  ZOOM in ROW and Rotate                       */
    /************************************************************************/

    piU=image->u;
    piV=image->v;
    temp_p1=tempRotateBuf+ImgInh_UV-1;
    temp_p2=tempRotateBuf+(ImgOutw_UV*ImgInh_UV)+ImgInh_UV-1;

    for(dY=0; dY<ImgInh_UV; dY++)
    {
        input_p=0;
        ptempU=temp_p1-dY;
        ptempV=temp_p2-dY;

        for(dX=0; dX<ImgOutw_UV; dX++)
        {
            //one U
            weight_B=input_p&Mask_SCALE_BIT;
            weight_A=maxScale-weight_B;
            pxptrU=piU+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrU++)*weight_A;
            wa+=(*pxptrU)*weight_B;
            (*ptempU)=(wa>>PIXEL_SCALE_BIT);
            ptempU+=ImgInh_UV;

            //one V
            pxptrV=piV+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrV++)*weight_A;
            wa+=(*pxptrV)*weight_B;
            (*ptempV)=(wa>>PIXEL_SCALE_BIT);
            ptempV+=ImgInh_UV;
            input_p+=xScale;
        }
        piU+=edged_width2;
        piV+=edged_width2;
    }


    /************************************************************************/
    /*                             U V  ZOOM in colume                        */
    /************************************************************************/

    piU=tempRotateBuf;
    piV=tempRotateBuf+(ImgOutw_UV*ImgInh_UV);
    poU=outBuf+ImgOutw*ImgOuth;
    poV=outBuf+((ImgOutw*ImgOuth*5)>>2);

    for(dY=0; dY<ImgOutw_UV; dY++)
    {
        input_p=0;

        for(dX=0; dX<ImgOuth_UV; dX++)
        {
            //one U
            weight_B=input_p&Mask_SCALE_BIT; //PIXEL_SCALE_BIT
            weight_A=maxScale-weight_B;
            pxptrU=piU+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrU++)*weight_A;
            wa+=(*pxptrU)*weight_B;
            (*poU++)=(wa>>PIXEL_SCALE_BIT);

            //one V
            pxptrV=piV+(input_p>>PIXEL_SCALE_BIT);
            wa=(*pxptrV++)*weight_A;
            wa+=(*pxptrV)*weight_B;
            (*poV++)=(wa>>PIXEL_SCALE_BIT);
            input_p+=yScale;
        }
        piU+=ImgInh_UV;
        piV+=ImgInh_UV;
    }

    return 0;
}


//only double size
int  zoom_twice(unsigned char* inbuf, unsigned short width, unsigned short extr_width,unsigned short height,uint8* outbuf,unsigned short des_w,unsigned short des_h)
{
    unsigned short i,j;
    unsigned short len=width+extr_width;

    uint8* pin;
    uint8* pout;

    unsigned short s_w;
    unsigned short s_h;

    unsigned short temp1,temp2;

    //176  32  144  320  240
    unsigned short off_w = (width - (des_w>>1))>>1;//16/2=8
    unsigned short off_h = (height- (des_h>>1))>>1;//22/2=11

    if(!des_w || !des_h)
        return -1;

    pin = inbuf+off_h*len+off_w;
    pout=outbuf;

    //column insert
    s_h=(des_h>>1)-1;
    s_w=(des_w>>1)-1;

    //first row
    temp1=(*pin++);
    for(j=0; j<s_w; j++)
    {
        (*pout++)=temp2=temp1;
        temp1=(*pin++);
        temp2+=temp1;
        (*pout++)=temp2>>1;
    }
    (*pout++)=temp1;
    (*pout++)=temp1;
    pout+=des_w;
    pin+=len-s_w-1;

    //middle row
    for(i=0; i<s_h; i++)
    {
        temp1=(*pin++);
        for(j=0; j<s_w; j++)
        {
            (*(pout-des_w))=(*(pout-(des_w<<1))+temp1)>>1;
            (*pout++)=temp2=temp1;
            temp1=(*pin++);
            temp2+=temp1;
            (*(pout-des_w))=(*(pout-(des_w<<1))+(temp2>>1))>>1;
            (*pout++)=temp2>>1;
        }
        (*(pout-des_w))=(*(pout-(des_w<<1))+temp1)>>1;
        (*pout++)=temp1;
        (*(pout-des_w))=(*(pout-(des_w<<1))+temp1)>>1;
        (*pout++)=temp1;
        pout+=des_w;
        pin+=len-s_w-1;
    }

    //last row
    pout-=des_w;
    s_w=des_w;
    for(j=0; j<s_w; j++)
    {
        (*pout)=*(pout-des_w);
        pout++;
    }

    return 0;
}


//only double size and rotate clockwise 90 degree
int  zoom_twice_rotate_clockwise(uint8* inbuf, unsigned short width, unsigned short extr_width,unsigned short height,uint8* outbuf, uint8* tempRotateBuf,unsigned short des_w,unsigned short des_h)
{
    unsigned short i,j;
    unsigned short len=width+extr_width;

    uint8*  pin;
    uint8*  pout;

    unsigned short s_w;
    unsigned short s_h;

    unsigned short temp1,temp2;

    //176  32  144  320  240
    unsigned short off_w = (width - (des_w>>1))>>1;//16/2=8
    unsigned short off_h = (height- (des_h>>1))>>1;//22/2=11

    //0xbD000000;16k
    uint8* tempBuf=tempRotateBuf;

    if(!des_w || !des_h)
        return -1;

    pin = inbuf+off_h*len+off_w;
    pout=outbuf;

    //column insert
    s_h=(des_h>>1)-1;
    s_w=(des_w>>1)-1;

    for(i=0; i<(s_h+1); i++)
    {
        tempBuf=tempRotateBuf+s_h-i;
        for(j=0; j<(s_w+1); j++)
        {
            *tempBuf=*pin;
            pin++;
            tempBuf+=(s_h+1);
        }
        pin+=len-(s_w+1);
    }
    tempBuf=tempRotateBuf;

    //first row
    temp1=(*tempBuf++);
    for(j=0; j<s_h; j++)
    {
        (*pout++)=temp2=temp1;
        temp1=(*tempBuf++);
        temp2+=temp1;
        (*pout++)=temp2>>1;
    }
    (*pout++)=temp1;
    (*pout++)=temp1;
    pout+=des_h;

    //middle row
    for(i=0; i<s_w; i++)
    {
        temp1=(*tempBuf++);
        for(j=0; j<s_h; j++)
        {
            (*(pout-des_h))=(*(pout-(des_h<<1))+temp1)>>1;
            (*pout++)=temp2=temp1;
            temp1=(*tempBuf++);
            temp2+=temp1;
            (*(pout-des_h))=(*(pout-(des_h<<1))+(temp2>>1))>>1;
            (*pout++)=temp2>>1;
        }
        (*(pout-des_h))=(*(pout-(des_h<<1))+temp1)>>1;
        (*pout++)=temp1;
        (*(pout-des_h))=(*(pout-(des_h<<1))+temp1)>>1;
        (*pout++)=temp1;
        pout+=des_h;
    }

    //last row
    pout-=des_h;
    s_h=des_h;
    for(j=0; j<s_h; j++)
    {
        (*pout)=*(pout-des_h);
        pout++;
    }

#if 0

    if(!des_w || !des_h)
        return -1;
    //inbuf=(int)inbuf|0xa0000000;
    //outbuf=(int)outbuf|0xa0000000;

    pin = inbuf+off_h*len+off_w;
    rotate_offset=des_h-16;
    pout=outbuf+rotate_offset;
    rotate_offset-=16;

    //column insert
    s_h=(des_h>>1)-1;
    s_w=(des_w>>1)-1;

    tempBuf_offset=19;
    tempBuf+=tempBuf_offset;
    tempBuf_offset-=2;

    //first row
    temp1=(*pin++);
    for(j=0; j<s_w; j++)
    {
        (*tempBuf)=temp2=temp1;
        tempBuf+=20;
        temp1=(*pin++);
        temp2+=temp1;
        (*tempBuf)=temp2>>1;
        tempBuf+=20;
    }
    (*tempBuf)=temp1;
    tempBuf+=20;
    (*tempBuf)=temp1;
    tempBuf=0x1D000000+tempBuf_offset;
    tempBuf_offset-=2;
    pin+=len-s_w-1;

    //middle row
    for(i=0; i<s_h; i++)
    {
        temp1=(*pin++);
        for(j=0; j<s_w; j++)
        {
            (*(tempBuf+1))=(*(tempBuf+2)+temp1)>>1;
            (*tempBuf)=temp2=temp1;
            tempBuf+=20;
            temp1=(*pin++);
            temp2+=temp1;
            (*(tempBuf+1))=(*(tempBuf+2)+(temp2>>1))>>1;
            (*tempBuf)=temp2>>1;
            tempBuf+=20;
        }
        (*(tempBuf+1))=(*(tempBuf+2)+temp1)>>1;
        (*tempBuf)=temp1;
        tempBuf+=20;
        (*(tempBuf+1))=(*(tempBuf+2)+temp1)>>1;
        (*tempBuf)=temp1;
        tempBuf+=20;

        if(tempBuf_offset==3)
        {
            tempBuf=0x1D000000+4;
            for(n=0; n<des_w; n++)
            {
//              memcpy32(pout, tempBuf, 4);
                ((unsigned int *)pout)[0] = ((unsigned int *)tempBuf)[0];
                ((unsigned int *)pout)[1] = ((unsigned int *)tempBuf)[1];
                ((unsigned int *)pout)[2] = ((unsigned int *)tempBuf)[2];
                ((unsigned int *)pout)[3] = ((unsigned int *)tempBuf)[3];

                *(tempBuf+19)=*(tempBuf-1);
                tempBuf+=20;
                pout+=des_h;
            }
            tempBuf_offset=19;
            pout=outbuf+rotate_offset;
            rotate_offset-=16;
        }
        tempBuf=0x1D000000+tempBuf_offset;
        tempBuf_offset-=2;

        pin+=len-s_w-1;
    }

    //last row
    s_w=des_w;
    for(j=0; j<s_w; j++)
    {
        *(tempBuf+1)=*(tempBuf+2);
        tempBuf+=20;
    }

    if(!tempBuf_offset)
    {
        tempBuf=0x1D000000+4;

        for(n=0; n<des_w; n++)
        {
            memcpy32(pout, tempBuf, 4);
            tempBuf+=20;
            pout+=des_h;
        }
    }
#endif

    return 0;
}

//only rotate clockwise 90 degree
int  rotate_clockwise(uint8* inbuf, unsigned short width, unsigned short extr_width,unsigned short height,uint8* outbuf)
{
    unsigned short i,j;
    uint8*  pin;
    uint8*  pout;
    unsigned short outOffset;

    if(!inbuf || !width || !height ||!outbuf)return -1;

    pin = inbuf;
    outOffset=height-1;

    for(i=0; i<height; i++)
    {
        pout=outbuf+(outOffset--);
        for(j=0; j<width; j++)
        {
            *pout=(*pin++);
            pout+=height;
        }
        pin+=extr_width;
    }

    return 0;
}



#else //float point

RGBQUAD GetPixelColorWithOverflow(
    unsigned char  *buf,
    long Width, long Height,
    long x,long y)
{
    RGBQUAD rgb;
    unsigned char * iDst;

    if (!((0<=y && y<Height && 0<=x && x<Width)))
    {
        x=max(x,0); x=min(x, Width-1);
        y=max(y,0); y=min(y, Height-1);
    }

    iDst  = buf + y*Width*3 + x*3;
    rgb.rgbBlue = *iDst++;
    rgb.rgbGreen= *iDst++;
    rgb.rgbRed  = *iDst;

    return rgb;
}

RGBQUAD GetPixelColorInterpolated(
    unsigned char  *buf,
    long biWidth, long biHeight,
    float x,float y)
{
    int xi;//源图像中虚拟位置最接近的真实位置
    int yi;
    RGBQUAD color;                    //calculated colour
    //calculate nearest pixel
    xi=(int)(x); //these replace (incredibly slow) floor (Visual c++ 2003, AMD Athlon)
    if (x<0) xi--;
    yi=(int)(y);
    if (y<0) yi--;


    if (xi<-1 || xi>=biWidth || yi<-1 || yi>=biHeight)  //一般不会进这里
    {
        if(0<=y && y<biHeight && 0<=x && x<biWidth)
        {
        }
        else
        {
            x=max(x,0); x=min(x, biWidth-1); // limite to 0~(biWidth-1) shenh
            y=max(y,0); y=min(y, biHeight-1);
        }
        xi=(int)(x); if (x<0) xi--;   //x and/or y have changed ... recalculate xi and yi
        yi=(int)(y); if (y<0) yi--;
    }
    if ((xi+1)<biWidth && xi>=0 && (yi+1)<biHeight && yi>=0 )
    {
        //all pixels are inside RGB24 image... optimize reading (and use fixed point arithmetic)
        unsigned short wt1=(unsigned short)((x-xi)*256.0f), wt2=(unsigned short)((y-yi)*256.0f);
        unsigned short wd=wt1*wt2>>8;
        unsigned short wb=wt1-wd;
        unsigned short wc=wt2-wd;
        unsigned short wa=256-wt1-wc;
        unsigned short wrr,wgg,wbb;
        unsigned char  *pxptr=(unsigned char *)buf+yi*biWidth*3+xi*3;
        wbb=wa*(*pxptr++); wgg=wa*(*pxptr++); wrr=wa*(*pxptr++);
        wbb+=wb*(*pxptr++); wgg+=wb*(*pxptr++); wrr+=wb*(*pxptr);
        pxptr+=(biWidth*3-5); //move to next row
        wbb+=wc*(*pxptr++); wgg+=wc*(*pxptr++); wrr+=wc*(*pxptr++);
        wbb+=wd*(*pxptr++); wgg+=wd*(*pxptr++); wrr+=wd*(*pxptr);
        color.rgbRed=(unsigned char ) (wrr>>8); color.rgbGreen=(unsigned char ) (wgg>>8); color.rgbBlue=(unsigned char ) (wbb>>8);

        return color;
    }
    else
    {
        /*
        shenh 2008.4.21
        a,b,c,d系数的推导

        设rgb11 和rgb21之间的点temp1
        则temp1=rgb11*((xi+1)-x)+rgb21*(x-xi)

        设rgb12 和rgb22之间的点temp2
        则temp2=rgb12*((xi+1)-x)+rgb22*(x-xi)

        设temp1 和temp2之间的点temp3
        则temp3=temp1*((yi+1)-y)+temp2*(y-yi)
        最后得temp3后rgb11,rgb21,rgb12,rgb22前面的系数就是下面的a,b,c,d
        上面的wa,wb,wc,wd同理只是用定点计算。
          */
        float t1=x-xi, t2=y-yi;
        float d=t1*t2;
        float b=t1-d;
        float c=t2-d;
        float a=1-t1-c;
        RGBQUAD rgb11,rgb21,rgb12,rgb22;
        rgb11=GetPixelColorWithOverflow(buf, biWidth, biHeight, xi, yi);//得到源图像中临近的真实像素shenh
        rgb21=GetPixelColorWithOverflow(buf, biWidth, biHeight, xi+1, yi);
        rgb12=GetPixelColorWithOverflow(buf, biWidth, biHeight, xi, yi+1);
        rgb22=GetPixelColorWithOverflow(buf, biWidth, biHeight, xi+1, yi+1);
        //calculate linear interpolation
        color.rgbRed=(unsigned char ) (a*rgb11.rgbRed+b*rgb21.rgbRed+c*rgb12.rgbRed+d*rgb22.rgbRed);
        color.rgbGreen=(unsigned char ) (a*rgb11.rgbGreen+b*rgb21.rgbGreen+c*rgb12.rgbGreen+d*rgb22.rgbGreen);
        color.rgbBlue=(unsigned char ) (a*rgb11.rgbBlue+b*rgb21.rgbBlue+c*rgb12.rgbBlue+d*rgb22.rgbBlue);

        return color;
    }

}

int Img_Resample(char *InBuf, unsigned long ImgInw, unsigned long ImgInh, char *outBuf,unsigned long ImgOutw,unsigned long ImgOuth)
{
    char *pxptr;
    float xScale, yScale;

    float sX, sY;         //source location 影射到源图像中的虚拟位置shenh
    long dX,dY;           //destination pixel (int value) 缩放后目的图像的真实位置shenh

    RGBQUAD q;              //pixel colour

    xScale =(float)ImgInw / (float)ImgOutw;//every xScale get one sample in ImgInw
    yScale =(float)ImgInh / (float)ImgOuth;
    /*
    shenh 2008.4.21
    例如
    ImgInw=3
    ImgOutw=5
    xScale=0.6
    放大后实际位置dX  0 1 2 3 4 五个点
    源数据中实际位置0 1 2 三个点
    影射到源图像的虚拟 位置sX -0.2 0.4 1.0 1.6 2.2 五个点
    计算出这五个虚拟位置的值就是所需要的。
    */

    for(dY=0; dY<ImgOuth; dY++)
    {

        sY = (dY + 0.5f) * yScale - 0.5f;///0.5 使得缩放后的图像四个边界像素位置，与原图像斯边界距离相等
        pxptr=outBuf+ dY*ImgOutw*3;

        for(dX=0; dX<ImgOutw; dX++)
        {
            sX = (dX + 0.5f) * xScale - 0.5f;
            q=GetPixelColorInterpolated(InBuf, ImgInw, ImgInh, sX, sY);
            *pxptr++=q.rgbBlue;
            *pxptr++=q.rgbGreen;
            *pxptr++=q.rgbRed;
        }
    }
    return 0;
}
#endif

/**************************************************
YUV 2RGB565
*************************************************/

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

#ifdef YUV2RGBLOOKUPTABLE
    L_tab = (int *) mmc_MemMalloc(256 * sizeof (int));
    Cr_r_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
    Cr_g_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
    Cb_g_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
    Cb_b_tab = (int *) mmc_MemMalloc (256 * sizeof (int));
#endif

#ifdef USE_INTERNAL_SRAM
    r_2_pix_alloc = (short *) YUV2RGB_START;//mmc_MemMalloc (1024 * sizeof (short));
    g_2_pix_alloc = (short *) (YUV2RGB_START+2048);//mmc_MemMalloc (1024 * sizeof (short));
    b_2_pix_alloc = (short *) (YUV2RGB_START+4096);//mmc_MemMalloc (1024 * sizeof (short));
#else
    r_2_pix_alloc = (short *) mmc_MemMalloc (1024 * sizeof (short));
    g_2_pix_alloc = (short *) mmc_MemMalloc (1024 * sizeof (short));
    b_2_pix_alloc = (short *) mmc_MemMalloc (1024 * sizeof (short));
#endif

    if (
#ifdef YUV2RGBLOOKUPTABLE
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

#ifdef YUV2RGBLOOKUPTABLE
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


#endif// MEDIA_VOCVID_SUPPORT
#endif

