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


#include "vppp_audiojpegenc_asm_map.h"
#include "vppp_audiojpegenc_amrjpegenc.h"
#include "vppp_audiojpegenc_mp3enc.h"
#include "yuvrotate.h"

#include "vppp_audiojpegenc_asm.h"

#include "vppp_audiojpegenc_sections.h"


typedef enum
{
    MP3_ENCODE = 0,
    MP3_DECODE,
    AMR_ENCODE,
    AMR_DECODE,
    MJPEG_ENCODE,
    MJPEG_DECODE,
    WMA_ENCODE,
    WMA_DECODE,
    AAC_ENCODE,
    AAC_DECODE,
    YUV_ROTATE,
    RGB565_ROTATE
} Codecmode;

void Coolsand_AudioEncode_MAIN(void);
void Coolsand_VoC_code_reload(void);

// struct of input parameters
typedef struct
{
    short mode;
    short reset;
    int inStreamBufAddr;         //Input stream buffer address
    int outStreamBufAddr;        //Output stream buffer address

    int SampleRate;
    short BitRate;
    short imag_quality;
    short imag_width;
    short imag_height;
    short channel_num;
    short reseverd;
    //int  inDataBufAddr;

} VPP_audiojpeg_enc_DEC_IN_T;

//struct of output parameters
typedef struct
{
    short mode;                 //used by AAC encode
    short streamStatus;         //Stream Status
    int   output_len;           //Size of the output PCM frame(bytes).
    int consumedLen;
    short ErrorStatus;
    short reserve0;

} VPP_audiojpeg_enc_DEC_OUT_T;

#define Input_buffer_size 384*40 //6frame

// enum of stream status
typedef enum
{
    VPP_AudioJpeg_Enc_STREAM_CONTINUE = 0x0000,
    VPP_AudioJpeg_Enc_STREAM_STOP = 0x0010,
    VPP_AudioJpeg_Enc_STREAM_BREAK = 0x0011,
    VPP_AudioJpeg_Enc_STREAM_IGNORE = 0x0020
} VPP_AudioJpeg_Enc_STREAM_STATUS;

void VoC_load_code(int * source_code, int word_size);
void VoC_execute(short start_address);


#define Input_buffer_START 0
#define MP3_ENC_Code_START Input_buffer_size+Input_buffer_START
#define MP3_ENC_ConstX_START SECTION_MP3_ENC_SIZE+MP3_ENC_Code_START
#define MP3_ENC_ConstY_START MP3_ENC_ConstX_size+MP3_ENC_ConstX_START
#define MP3_ENC_Const_rqmy_START MP3_ENC_ConstY_size+MP3_ENC_ConstY_START
#define MP3_ENC_Const_Zig_START MP3_ENC_Const_rqmy_size+MP3_ENC_Const_rqmy_START

#define AMRJPEG_Code_START MP3_ENC_Const_Zig_size+MP3_ENC_Const_Zig_START
#define AMRJPEG_ConstX_START SECTION_AMR_ENC_SIZE+AMRJPEG_Code_START
#define AMRJPEG_ConstY_START AMRJPEG_ConstX_size+AMRJPEG_ConstX_START
//#define Output_buffer_START AMRJPEG_ConstY_size+AMRJPEG_ConstY_START

#define RGBYUV_ROTATE_Code_START AMRJPEG_ConstY_size+AMRJPEG_ConstY_START
#define Output_buffer_START RGBYUV_ROTATE_Code_START+SECTION_RGBYUV_ROTATE_SIZE
