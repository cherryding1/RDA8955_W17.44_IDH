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















#ifndef VPP_PRIVATE_H
#define VPP_PRIVATE_H

#include "cs_types.h"

#define REG0                          0x0
#define REG1                          0x1
#define REG2                          0x2
#define REG3                          0x3
#define REG4                          0x4
#define REG5                          0x5
#define REG6                          0x6
#define REG7                          0x7

#define REG01                         0x0
#define REG23                         0x1
#define REG45                         0x2
#define REG67                         0x3
#define ACC0                          0x4
#define ACC1                          0x5
#define RL6                           0x6
#define RL7                              0x7

/*****************************
 **    MP3 decoder buffers      **
 *****************************/
#define RAM_X_BEGIN_ADDR                            0x0000
#define MAD_GLOBAL_VARS_X_START_ADDR                            (0   +    RAM_X_BEGIN_ADDR)
#define GLOBAL_INPUT0_DATA_ADDR                     (MAD_GLOBAL_VARS_X_START_ADDR  +VOC_RAM_X_BASE)     //1024     end in1023 wrap buffer 
#define OUTPUT_PCM_BUFFER1_ADDR                         (2220+1024+GLOBAL_INPUT0_DATA_ADDR )          //2304
#define OUTPUT_PCM_BUFFER2_ADDR                         ( 2304+OUTPUT_PCM_BUFFER1_ADDR )          //2304   [2][576]*2  = 1152

#define TABLE_RAM_X_BEGINE_ADDR       ((1024   +    RAM_X_BEGIN_ADDR)<< 1 +VOC_SRAM_BASE)  //   0 + RAM_Z_BEGIN_ADDR 
#define MAX_MP3_FRAME_SIZE                   512
#define FIFO_SIZE                                      1152*2

/*****************************
 **    JPEG decoder buffers      **
 *****************************/
#define RAM_Y_BEGIN_ADDR                             0x4000
#define GLOBAL_JPEG_Y                           (0 + RAM_Y_BEGIN_ADDR)
#define TABLE_qt_table_ADDR                                 ((0 + GLOBAL_JPEG_Y)<<1 + VOC_SRAM_BASE) //[192]shorts

/*************************************************
                Vpp struct
 *************************************************/
#define VOC_AHB_DMA_EADDR_MASK      (0x7FFFFF<<2)

///  @defgroup vpp_struct
///  @{
//
/*
/// Structure used to configure VOC frame encode
typedef struct
{
    /// Mode of the voice coded output frame
    Uint16 encMode;
    /// Type of the coded output frame (AMR only)
    Uint16 encFrameType;
    /// Voice Activity Detection flag (all except AMR)
    Uint16 vad;
    /// Speech flag (all except AMR)
    Uint16 sp;
    /// Encoder output buffer (coded frame in enc_mode)
    Uint16 encOutBuf[MAX_SPECCH_FRAME_SIZE];
}HAL_ENC_OUT_T;
*/
// for mp3 data
#define  MAD_FLOW_CONTINUE   0x0000     /* continue normally */
#define  MAD_FLOW_STOP       0x0010     /* stop decoding normally */
#define  MAD_FLOW_BREAK      0x0011     /* stop decoding and signal an error */
#define  MAD_FLOW_IGNORE     0x0020 /* ignore the current frame */


//for voc status
#define VOC_STOP                  0x4
#define VOC_STALL                0x0


/// Structure used to configure VOC frame decode
typedef struct
{
    /// Decoder input buffer (coded frame in dec_mode)
    int32 decInBuf[MAX_MP3_FRAME_SIZE];
} HAL_DEC_IN_T;

/// Structure used for vocoder reset
typedef struct
{
    /// vocoder reset
    uint16 reset;
} HAL_RESET_T;




///  @} <- End of the vpp_struct group
typedef const enum hal_CodecMode
{
    MP3 = 0,
    AMR

} CODEC;

typedef const enum iEQ
{
    NORMAL = 0,
    BASS,
    ROCK,
    POP,
    JAZZ,
    CLASSIC
} IEQ;

enum Error_type
{
    mode_error = 0,     //mode err0or
    need_moredata,      //need more data
    others            //others
};

typedef struct DataInfo
{
    int32 Consume_data_length;           //data length which have been used for one frame
    int32 Output_data_length;               //length for decoded data
    int32 Seek_length;                               //length we should seek,sometimes we need seek for next good frame
    enum Error_type ErrorType;                 //to show why these is no data for output buffer

} DSP_STATE_INFO;

enum usedMode
{
    mp3_enc = 0,
    mp3_dec,
    amr_dec,
    amr_enc,
    wma_dec,
    mjpeg_enc,
    mjpeg_dec
};
#endif  // VPP_PRIVATE_H

