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

#include <stdio.h>

#include "voc2_library.h"

#include "vppp_analogtv_common.h"
#include "vpp_analogtv.h"
#include "vppp_analogtv_asm_map.h"
#include "vppp_analogtv_asm.h"


void Rotate_90_degree(unsigned short* dec,unsigned short* src,unsigned short width,unsigned short height)
{
    int i,j;

    for (i=0; i<height; i++)
    {
        for (j=0; j<width; j++)
        {
            *(dec + j*height + (height-1-i)) = *src++;
        }
    }
}

void vpp_audiojpeg_encDecScheduleOneFrame(VPP_ANALOGTV_INPUT_CFG_T *dec_in_ptr)
{
    VPP_ANALOGTV_INPUT_CFG_T *dec_in_ptr_VoC = (VPP_ANALOGTV_INPUT_CFG_T*)&RAM_Y[(GLOBAL_INPUT_STRUCT-RAM_Y_BEGIN_ADDR)/2];

    *dec_in_ptr_VoC=*dec_in_ptr;
    dec_in_ptr_VoC->inStreamBufAddr=dec_in_ptr ->inStreamBufAddr;
    dec_in_ptr_VoC->outStreamBufAddr=(UINT32 *)((UINT32)dec_in_ptr ->outStreamBufAddr |0x40000000L);
    //dec_in_ptr_VoC->cambufAddr=(dec_in_ptr ->cambufAddr |0x40000000L);
    VoC_set_event(CFG_WAKEUP_SOF0);

}

void VoC_XCPU_IRQ_handler()
{
    static int framecount=0;
    VPP_ANALOGTV_INPUT_CFG_T  cutzoom;
    unsigned short w, h;
    FILE*  fp = fopen("test.dat","wb");
    unsigned short cutwidth,cutheight;
    unsigned short previewZoom = 0;//1 2 3 4 5
    //static short counth = 240;
    //static int  countInc;

    w = 320;
    h = 240;



#if 0
    cutwidth = 320 ;
    cutheight = 240;


    if (framecount==1) //if(counth<=0)//
    {
        VoC_counter_print(framecount);
        fwrite(&RAM_EXTERN[STREAM_OUT_ADDR],2,w*h,fp);
        fclose(fp);

    }
    else
    {
        if (w == 320)
        {
            //cutzoom.reset = 1;
            //countInc = 0;
            cutzoom.rotate = 1;

        }
        else
        {
            //cutzoom.reset = 0;
            //countInc+= 320*YUV2RGBLINE*2;
            cutzoom.rotate = 0;
        }
        cutzoom.inStreamBufAddr = (STREAM_IN_ADDR<<2);
        cutzoom.outStreamBufAddr= (STREAM_OUT_ADDR<<2);
        cutzoom.blendbufADDR = (STREAM_OUT_ADDR<<2);
        //cutzoom.cambufAddr =(CMD_DATA_STAR<<2);
        cutzoom.src_w = 640;
        cutzoom.src_h = 80;
        cutzoom.cut_w = 640;//cutwidth;
        cutzoom.cut_h = 80;//cutheight;
        cutzoom.dec_w = w;
        cutzoom.dec_h = h;
        //cutzoom.rotate = 1;



        //cutzoom.blit_mode = Zoom_blit_default;//Zoom_blit_sb33f;//
        framecount++;
        //for (counth=h;counth>0;counth-=YUV2RGBLINE)
        //if(counth > 0)
        //{
        vpp_audiojpeg_encDecScheduleOneFrame(&cutzoom);



        //counth-=YUV2RGBLINE;
        //counth=0;
        //}
#else

    if (h==160 || h==220 || h==320)
    {
        cutwidth  = 180*10/(10+2*previewZoom);
    }
    else
    {
        cutwidth  = 320*10/(10+2*previewZoom);
    }
    cutheight =240*10/(10+2*previewZoom);


    if (framecount==1)
    {
        VoC_counter_print(framecount);
        fwrite(&RAM_EXTERN[STREAM_OUT_ADDR],2,w*h,fp);
        fclose(fp);

    }
    else
    {

        cutzoom.reset = 1;
        cutzoom.inStreamBufAddr = (UINT32 *)(STREAM_IN_ADDR<<2);
        cutzoom.outStreamBufAddr= (UINT32 *)(STREAM_OUT_ADDR<<2);
        cutzoom.blendBufAddr = (UINT32 *)(STREAM_OUT_ADDR<<2);
        //cutzoom.cambufAddr =(CMD_DATA_STAR<<2);
        cutzoom.src_w = 320;
        cutzoom.src_h = 240;
        cutzoom.cut_w = cutwidth;
        cutzoom.cut_h = cutheight;
        cutzoom.dec_w = w;
        cutzoom.dec_h = h;



        //cutzoom.blit_mode = Zoom_blit_default;//Zoom_blit_sb33f;//
        framecount++;
        //for (counth=h;counth>0;counth-=YUV2RGBLINE)
        vpp_audiojpeg_encDecScheduleOneFrame(&cutzoom);

#endif
    }


}

void VoC_BCPU_IRQ_handler()
{
    ;
}
void  randram()
{
    int i;

    for (i=0; i<8; ++i)
    {
        REGS[i].reg = rand();
        REGS[i].inc = rand();
    }
    REGL[0] = rand();
    REGL[1] = rand();
    REGL[2] = rand();
    REGL[3] = rand();

    for (i = 0; i < 10240; ++i) RAM_X[i] =  (rand()<<16) | rand();
    for (i = 0; i < 10240; ++i) RAM_Y[i] = (rand()<<16) | rand();
}

//#define BINARY_SIM  0
//#undef BINARY_SIM

void main(int argc, char* argv[])
{

    FILE*  fp = fopen(argv[1],"rb");
    //FILE*  frotate = fopen("rotate.dat","wb");
    //unsigned short  rotate_buf[320*240];

    randram();

    //memcpy(RAM_X,constdata,512*4+32);
    memcpy(RAM_X,g_vppAnalogTVConstX,(1536+104)*2);
    memcpy(&RAM_Y[(CONST_TabU2G_ADDR-RAM_Y_BEGIN_ADDR)/2],g_vppAnalogTVConstY,1024*2);

    fread(&RAM_EXTERN[STREAM_IN_ADDR],2,320*240,fp);
#ifdef BINARY_SIM
    printf("Simulation based on binary code\n");

    printf("input is yuv format\n");
    VoC_load_code(g_vppAnalogTVMainCode,   COMMON_CODE_SIZE);

    VoC_execute(0);
#else
    printf("Simulation based on VoC library\n");
    voc_simu_init();
    Coolsand_MAIN();
#endif

    //Rotate_90_degree(rotate_buf,(unsigned short*)&RAM_EXTERN[STREAM_OUT_ADDR],320,240);
    //fwrite(rotate_buf,2,320*240,frotate);
    //fclose(frotate);

    fclose(fp);


}


