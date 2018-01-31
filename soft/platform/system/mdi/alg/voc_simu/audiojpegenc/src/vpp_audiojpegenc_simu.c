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

#include "voc2_library.h"

#include "vppp_audiojpegenc_common.h"

#ifndef MICRON_ALIGN

#define MICRON_ALIGN(a)

#endif

extern int Dec_inStreamBufAddr_Jpeg;
extern int Dec_inStreamBufAddr;
extern int Dec_inStreamBufStartAddr;
extern int Dec_inStreamBufEndAddr;


extern int *Test_Sequence;
extern int Test_Sequence_Size;


// MP3_Test_Sequence
const int MP3_ENC_Test_Sequence[] MICRON_ALIGN(32)=
{
//#include "..\work\m71_in.tab"//momo
#include "..\work\mp3in_chan3.tab"//stereo
};

// AMR
const int AMR_Test_Sequence[] MICRON_ALIGN(32)=
{
#include "..\work\buff_amr.tab"
    //#include "..\work\VIDREC0151A.tab"
};

static int     ref_intdata[]=
{
    // #include "test41.tab"//128
#include "..\work\bridgefar1.tab"
    // #include "bridgefar1.tab"//176
    //  #include "apple11.tab"//144*128
    //  #include "hagendashi_out.tab"
    //    #include "00421.tab"//320*240
};




//jpeg
const unsigned short JPEG_Test_Sequence[] MICRON_ALIGN(32)=
{
    //#include "..\work\240by176.tab"//411
    //#include "..\work\320by240dat.tab"//rgb565 format 320*240
    //#include "..\work\1.tab"//1280*1024  640*480
    //#include "..\work\640by480dat.tab"//1280*1024  640*480

    //#include "..\work\240by180_yuv.tab"//yuv422 format
    //#include "..\work\320by240.tab"//yuv422 format
    // #include "..\work\my7.tab"//yuv422 format 640*480
    //#include "..\work\176BY144YUV.tab"//yuv422 176*144

    //#include "..\work\160by120.tab"//yuv422 format
    //#include "..\work\IMG0008A.tab"//yuv422 format  320*240
//  #include "..\work\part2.tab"//yuv422 format  320*16
//#include "..\work\uxga_2.tab"//yuv422 format  1600*1200


#include "..\work\test.tab"//yuv422 format  1600*1200

    //#include "..\work\webcam_jpeg_2_VoC.tab" //640*480



};


#ifdef BINARY_SIM

#define CODE_SIZE sizeof(G_VppCommonEncCode)
#define PC_START_ADDR 0

#endif // BINARY_SIM



void vpp_audiojpeg_encDecOpen (Codecmode Multimedia_Mode)
{

    int* ptr32;
#ifndef  ROTATE_TEST
    VPP_audiojpeg_enc_DEC_IN_T *dec_in_ptr = (VPP_audiojpeg_enc_DEC_IN_T*)&RAM_Y[(Input_mode_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    VPP_audiojpeg_enc_DEC_OUT_T *pDecOut = (VPP_audiojpeg_enc_DEC_OUT_T*)&RAM_Y[(Output_mode_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];

    memset(pDecOut,0,sizeof(VPP_audiojpeg_enc_DEC_OUT_T));

    dec_in_ptr->reset=1;
    pDecOut->mode=-1;//not 0~9

    Dec_inStreamBufAddr=dec_in_ptr->inStreamBufAddr=(Input_buffer_START<<2)|0x00000000;
    Dec_inStreamBufEndAddr=((Input_buffer_START+Input_buffer_size)<<2)|0x00000000;
    dec_in_ptr->outStreamBufAddr=((Output_buffer_START)<<2)|0x40000000;//write output data to RAM_E

    Dec_inStreamBufStartAddr=(Input_buffer_START<<2)|0x00000000;



    ptr32=&RAM_Y[(MP3_Code_ExternalAddr_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *ptr32++=(MP3_ENC_Code_START<<2)|0x00000000;
    *ptr32++=(MP3_ENC_ConstX_START<<2)|0x00000000;
    *ptr32++=(MP3_ENC_ConstY_START<<2)|0x00000000;
    *ptr32++=(MP3_ENC_Const_rqmy_START<<2)|0x00000000;
    *ptr32++=(MP3_ENC_Const_Zig_START<<2)|0x00000000;


    ptr32=&RAM_Y[(AMRJPEG_Code_ExternalAddr_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *ptr32++=(AMRJPEG_Code_START<<2)|0x00000000;
    *ptr32++=(AMRJPEG_ConstX_START<<2)|0x00000000;
    *ptr32++=(AMRJPEG_ConstY_START<<2)|0x00000000;



    VoC_load_extern((int*)G_VppMp3EncCode,   SECTION_MP3_ENC_SIZE, MP3_ENC_Code_START);
    VoC_load_extern((int*)G_VppMp3EncConstX,   MP3_ENC_ConstX_size, MP3_ENC_ConstX_START);
    VoC_load_extern((int*)G_VppMp3EncConstY,   MP3_ENC_ConstY_size, MP3_ENC_ConstY_START);
    VoC_load_extern((int*)G_VppMp3EncConst_rqmy,   MP3_ENC_Const_rqmy_size, MP3_ENC_Const_rqmy_START);
    VoC_load_extern((int*)G_VppMp3EncConst_Zig,   MP3_ENC_Const_Zig_size, MP3_ENC_Const_Zig_START);

    VoC_load_extern((int*)G_VppAmrJpegEncCode,   SECTION_AMR_ENC_SIZE, AMRJPEG_Code_START);
    VoC_load_extern((int*)G_VppAmrJpegEncConstX,   AMRJPEG_ConstX_size, AMRJPEG_ConstX_START);
    VoC_load_extern((int*)G_VppAmrJpegEncConstY,   AMRJPEG_ConstY_size, AMRJPEG_ConstY_START);
#else

    INT16 *pMode;

    pMode=(short *)&RAM_Y[(Input_mode_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *pMode=Multimedia_Mode;

    pMode=(short*)&RAM_Y[(Output_mode_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *pMode=-1;
    pMode=(short*)&RAM_Y[(Output_ERR_Status_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *pMode=0;


    ptr32=&RAM_Y[(RGBYUV_ROTATE_Code_ExternalAddr_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *ptr32=(RGBYUV_ROTATE_Code_START<<2)|0x00000000;
    VoC_load_extern((int*)G_vocYUVRomateCode,   SECTION_RGBYUV_ROTATE_SIZE, RGBYUV_ROTATE_Code_START);

#endif

    switch(Multimedia_Mode)
    {
        case MP3_ENCODE:
            Test_Sequence=(int *)MP3_ENC_Test_Sequence;
            Test_Sequence_Size=sizeof(MP3_ENC_Test_Sequence)/4;

            break;
        case MP3_DECODE:
            break;
        case AMR_ENCODE:
            Test_Sequence=(int *)AMR_Test_Sequence;
            Test_Sequence_Size=sizeof(AMR_Test_Sequence)/4;
            break;
        case AMR_DECODE:

            break;
        case MJPEG_ENCODE:
            Test_Sequence=(int *)JPEG_Test_Sequence;
            Test_Sequence_Size=sizeof(JPEG_Test_Sequence)/4;
            break;
        case MJPEG_DECODE:

            break;
        case WMA_ENCODE:
            break;
        case WMA_DECODE:
            break;
        case AAC_ENCODE:
            break;
        case AAC_DECODE:
            break;
        case YUV_ROTATE:
            break;
        case RGB565_ROTATE:
            break;
        default:
            break;
    }


#ifdef BINARY_SIM
    printf("Simulation based on binary code\n");
    // load default code
    VoC_load_code(G_VppCommonEncCode, G_VppAudJpegMainCodeSize);
    VoC_execute(PC_START_ADDR);
#else
    printf("Simulation based on VoC library\n");
    voc_simu_init();
    Coolsand_AudioEncode_MAIN();
#endif

}

void vpp_audiojpeg_encDecClose(void)
{
    return;
}

void vpp_audiojpeg_encDecStatus(VPP_audiojpeg_enc_DEC_OUT_T * pStatus)
{
    VPP_audiojpeg_enc_DEC_OUT_T *pDecOut = (VPP_audiojpeg_enc_DEC_OUT_T*)&RAM_Y[(Output_mode_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];
    *pStatus = *pDecOut;
}


void vpp_audiojpeg_encDecScheduleOneFrame(VPP_audiojpeg_enc_DEC_IN_T *dec_in_ptr)
{
    VPP_audiojpeg_enc_DEC_IN_T *dec_in_ptr_VoC = (VPP_audiojpeg_enc_DEC_IN_T*)&RAM_Y[(Input_mode_addr-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];

    *dec_in_ptr_VoC=*dec_in_ptr;
    dec_in_ptr_VoC->inStreamBufAddr=dec_in_ptr ->inStreamBufAddr;
    dec_in_ptr_VoC->outStreamBufAddr=(dec_in_ptr ->outStreamBufAddr |0x40000000L);
    dec_in_ptr->reset=0;

    if (dec_in_ptr_VoC->mode==MJPEG_ENCODE)
    {

        int xScale =(((dec_in_ptr_VoC->SampleRate&0xffff)-1)<<(PIXEL_SCALE_BIT))/dec_in_ptr_VoC->imag_width;
        int yScale =(((dec_in_ptr_VoC->SampleRate>>16)-1)<<(PIXEL_SCALE_BIT))/dec_in_ptr_VoC->imag_height;

        int* ptr32=&RAM_Y[(GLOBAL_xScale-COMMON_GLOBAL_Y_BEGIN_ADDR)/2];

        *ptr32++=xScale;
        *ptr32++=yScale;

    }


    VoC_set_event(CFG_WAKEUP_SOF0);

}


