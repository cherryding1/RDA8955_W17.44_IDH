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

#ifndef VPPP_AUDIOJPEGENC_AMRJPEGENC_H
#define VPPP_AUDIOJPEGENC_AMRJPEGENC_H



#define AMRJPEG_ConstX_size 172
#define AMRJPEG_ConstY_size 3060

#define PIXEL_SCALE_BIT 14
#define Mask_SCALE_BIT 0x3fff


/*****************************
 **    Common functions     **
 *****************************/

/////////////////////////////////////////
#define MAX_32                                               0x7fffffffL
#define MIN_32                                               0x80000000L
#define MAX_16                                               0x7fff
#define MIN_16                                               0x8000
#define _1_2                                                 (32768L/2)
#define _1_4                                                 (32768L/4)
#define _1_8                                                 (32768L/8)
#define _1_16                                                (32768L/16)
#define _1_32                                                (32768L/32)
#define _1_64                                                (32768L/64)
#define _1_128                                               (32768L/128)
#define PIT_MIN                                              20        /* Minimum pitch lag (all other modes)      */
#define PIT_MAX                                              143       /* Maximum pitch lag                        */
#define MAX_PRM_SIZE                                 57        /* max. num. of params                      */
#define MAX_SERIAL_SIZE                              244       /* max. num. of serial bits                 */
#define L_WINDOW                                         240       /* Window size in LP analysis               */
#define L_FRAME                                              160       /* Frame size                               */
#define L_FRAME_BY2                                      80        /* Frame size divided by 2                  */
#define L_SUBFR                                              40        /* Subframe size                            */
#define L_CODE                                               40        /* codevector length                        */
#define NB_TRACK                                         5         /* number of tracks                         */
#define STEP                                                 5         /* codebook step size                       */
#define NB_TRACK_MR102                               4         /* number of tracks mode mr102              */
#define STEP_MR102                                       4         /* codebook step size mode mr102            */
#define M                                                        10        /* Order of LP filter                       */
#define MP1                                                      (M+1)     /* Order of LP filter + 1                   */
#define LSF_GAP                                              205       /* Minimum distance between LSF after quan-tization; 50 Hz = 205                    */
#define NC                                                       M/2


/* Constants of VAD hangover addition */
#define HANGCONST                                        10
#define BURSTCONST                                       3

/* Constant of spectral comparison */
#define STAT_THRESH                                      3670L       /* 0.056 */

/* Constants of periodicity detection */
#define LTHRESH                                              2
#define NTHRESH                                              4

/* Pseudo floating point representations of constants for threshold adaptation */
#define M_PTH                                                32500          /*** 130000.0 ***/
#define E_PTH                                                17
#define M_PLEV                                               21667          /*** 346666.7 ***/
#define E_PLEV                                               19
#define M_MARGIN                                         16927          /*** 69333340 ***/
#define E_MARGIN                                         27

#define FAC                                                      17203                       /* 2.1 */
#define FREQTH                                               3189
#define PREDTH                                               1464
#define PN_INITIAL_SEED                              0x70816958L   /* Pseudo noise generator seed value  */

/* Comfort noise interpolation period (nbr of frames between successive SID updates in the decoder) */
#define CN_INT_PERIOD                                24

/* Period when SP=1 although VAD=0.Used for comfort noise averaging */
#define DTX_HANGOVER                                 7
#define INV_DTX_HANGOVER                         (0x7fff / DTX_HANGOVER)
#define INV_DTX_HANGOVER_P1                      (0x7fff / (DTX_HANGOVER+1))

/* Number of pulses in fixed codebook excitation */
#define NB_PULSE                                         10

/* SID frame classification thresholds */
#define VALID_SID_THRESH                         2
#define INVALID_SID_THRESH                       16

/* Constant DTX_ELAPSED_THRESHOLD is used as threshold for allowing
   SID frame updating without hangover period in case when elapsed
   time measured from previous SID update is below 24 */
#define DTX_ELAPSED_THRESHOLD                (24 + DTX_HANGOVER - 1)
#define DTX_HANGOVER_1                               (4  * DTX_HANGOVER - 1)
#define DTX_ELAPSED_THRESHOLD                (24 + DTX_HANGOVER - 1)
#define DTX_HANGOVER_1_MPY10                 (DTX_HANGOVER-1)*M

/* Frame classification constants */
#define VALID_SID_FRAME                              1
#define INVALID_SID_FRAME                        2
#define GOOD_SPEECH_FRAME                        3
#define UNUSABLE_FRAME                               4

/* Encoder DTX control flags */
#define TX_SP_FLAG                                       0x0001
#define TX_VAD_FLAG                                      0x0002
#define TX_HANGOVER_ACTIVE                       0x0004
#define TX_PREV_HANGOVER_ACTIVE              0x0008
#define TX_SID_UPDATE                                0x0010
#define TX_USE_OLD_SID                               0x0020

#define RX_SP_FLAG                                       0x0001
#define RX_UPD_SID_QUANT_MEM                 0x0002
#define RX_FIRST_SID_UPDATE                      0x0004
#define RX_CONT_SID_UPDATE                       0x0008
#define RX_LOST_SID_FRAME                        0x0010
#define RX_INVALID_SID_FRAME                 0x0020
#define RX_NO_TRANSMISSION                       0x0040
#define RX_DTX_MUTING                                0x0080
#define RX_PREV_DTX_MUTING                       0x0100
#define RX_CNI_BFI                                       0x0200
#define RX_FIRST_SP_FLAG                         0x0400

#define PITCH_OL_M475_LOOP_NUM               (L_FRAME+PIT_MAX)
#define PITCH_OL_M795_LOOP_NUM               (L_FRAME_BY2+PIT_MAX)
#define PITCH_OL_WGH_LOOP_NUM                (L_FRAME_BY2+PIT_MAX)
#define PITCH_OL_M122_LOOP_NUM               (L_FRAME_BY2+PIT_MAX)

#define DICO1_SIZE                                       128
#define DICO2_SIZE                                       256
#define DICO3_SIZE                                       256
#define DICO4_SIZE                                       256
#define DICO5_SIZE                                       64

#define INLINE_NUM                                         9
#define INLINE_NUM_1                                       8

#define   Q15       32767
#define   Q14       16384
#define   S15       15
#define   MASK      (Q15-1)
// VoC_directive: PARSER_OFF
//jpeg enc data///////////////////////////////////////
static unsigned char head1[20]=
{
    0xff,0xd8,0xff,0xe0,0x0,0x10,0x4a,0x46,0x49,0x46,0x0,0x1,0x1,
    0x0,0x0,0x1,0x0,0x1,0x0,0x0
};

//static unsigned char head_q_l[136]={
//   0xff,0xdb,0x0,0x84,0x0,

//   0x10,0xb,0xc,0xe,0xc,0xa,0x10,0xe,0xd,0xe,
//   0x12,0x11,0x10,0x13,0x18,0x28,0x1a,0x18,0x16,0x16,0x18,0x31,0x23,
//   0x25,0x1d,0x28,0x3a,0x33,0x3d,0x3c,0x39,0x33,0x38,0x37,0x40,0x48,
//   0x5c,0x4e,0x40,0x44,0x57,0x45,0x37,0x38,0x50,0x6d,0x51,0x57,0x5f,
//   0x62,0x67,0x68,0x67,0x3e,0x4d,0x71,0x79,0x70,0x64,0x78,0x5c,0x65,
//   0x67,0x63,0x1,0x11,0x12,0x12,0x18,0x15,0x18,0x2f,0x1a,0x1a,0x2f,
//   0x63,0x42,0x38,0x42,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
//   0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
//   0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
//   0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,0x63,
//   0x63,0x63,

//   0xff,0xc0
//};

static unsigned char head_q_l[136]=
{
    0xff,0xdb,0x0,0x84,

    0x0,

    0x6  ,0x4  ,0x5  ,0x8  ,0x6  ,0x4  ,0x4  ,0x9  ,
    0x6  ,0xb  ,0xc  ,0xa  ,0x6  ,0x6  ,0x5  ,0x4  ,
    0x5  ,0x8  ,0xa  ,0xc  ,0xf  ,0x13  ,0xe  ,0xc  ,
    0xc  ,0xb  ,0x6  ,0x6  ,0x5  ,0x9  ,0xc  ,0x10  ,
    0x10  ,0xc  ,0x1c  ,0x1f  ,0x1f  ,0x1b  ,0x13  ,0xc  ,
    0xc  ,0xa  ,0xb  ,0xa  ,0xc  ,0xe  ,0x13  ,0x1b  ,
    0x1f  ,0x1f  ,0x1b  ,0x14  ,0xf  ,0xc  ,0x10  ,0x14  ,
    0x1c  ,0x1f  ,0x1f  ,0x1f  ,0x13  ,0x1f  ,0x1f  ,0x1f  ,

    0x1,
    0x7  ,0x7  ,0x10  ,0x1f  ,0x18  ,0x7  ,0xd  ,0x1a  ,
    0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x15  ,0xc  ,0xd  ,
    0x11  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,
    0x1f  ,0x1f  ,0x15  ,0x18  ,0x10  ,0x1a  ,0x1f  ,0x1f  ,
    0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,
    0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,
    0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,
    0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,0x1f  ,

    0xff,0xc0
};

static  unsigned char head_q_h[136]=
{
    0xff,0xdb,0x0,0x84,0x0,0x2,0x1,0x1,0x1,0x1,0x1,0x2,0x1,0x1,0x1,
    0x2,0x2,0x2,0x2,0x2,0x4,0x3,0x2,0x2,0x2,0x2,0x5,0x4,0x4,0x3,
    0x4,0x6,0x5,0x6,0x6,0x6,0x5,0x6,0x6,0x6,0x7,0x9,0x8,0x6,0x7,
    0x9,0x7,0x6,0x6,0x8,0xb,0x8,0x9,0xa,0xa,0xa,0xa,0xa,0x6,0x8,
    0xb,0xc,0xb,0xa,0xc,0x9,0xa,0xa,0xa,0x1,0x2,0x2,0x2,0x2,0x2,
    0x2,0x5,0x3,0x3,0x5,0xa,0x7,0x6,0x7,0xa,0xa,0xa,0xa,0xa,0xa,
    0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,
    0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,
    0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xa,0xff,0xc0
};

static unsigned char head_size_128_411[10]=
{
    0x0,0x11,0x8,0x0,0x60,0x0,0x80,0x3,0x1,0x22,
};

static unsigned char head_size_196_411[10]=
{
    0x0,0x11,0x8,0x0,0x90,0x0,0xB0,0x3,0x1,0x22,
};
static unsigned char head_4[441]=
{
    0x0,0x2,0x11,0x1,0x3,0x11,0x1,
    0xff,0xc4,0x1,0xa2,0x0,0x0,0x1,0x5,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,
    0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0x10,0x0,0x2,0x1,0x3,0x3,0x2,0x4,
    0x3,0x5,0x5,0x4,0x4,0x0,0x0,0x1,0x7d,0x1,0x2,0x3,0x0,0x4,0x11,0x5,0x12,0x21,0x31,0x41,
    0x6,0x13,0x51,0x61,0x7,0x22,0x71,0x14,0x32,0x81,0x91,0xa1,0x8,0x23,0x42,0xb1,0xc1,0x15,
    0x52,0xd1,0xf0,0x24,0x33,0x62,0x72,0x82,0x9,0xa,0x16,0x17,0x18,0x19,0x1a,0x25,0x26,0x27,
    0x28,0x29,0x2a,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,
    0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,
    0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,
    0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,
    0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
    0xe8,0xe9,0xea,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0x1,0x0,0x3,0x1,0x1,
    0x1,0x1,0x1,0x1,0x1,0x1,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,
    0x9,0xa,0xb,0x11,0x0,0x2,0x1,0x2,0x4,0x4,0x3,0x4,0x7,0x5,0x4,0x4,0x0,0x1,0x2,0x77,0x0,
    0x1,0x2,0x3,0x11,0x4,0x5,0x21,0x31,0x6,0x12,0x41,0x51,0x7,0x61,0x71,0x13,0x22,0x32,0x81,
    0x8,0x14,0x42,0x91,0xa1,0xb1,0xc1,0x9,0x23,0x33,0x52,0xf0,0x15,0x62,0x72,0xd1,0xa,0x16,
    0x24,0x34,0xe1,0x25,0xf1,0x17,0x18,0x19,0x1a,0x26,0x27,0x28,0x29,0x2a,0x35,0x36,0x37,
    0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,
    0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,
    0x7a,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,
    0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,
    0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,
    0xd7,0xd8,0xd9,0xda,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf2,0xf3,0xf4,0xf5,
    0xf6,0xf7,0xf8,0xf9,0xfa,0xff,0xda,0x0,0xc,0x3,0x1,0x0,0x2,0x11,0x3,0x11,0x0,0x3f,0x0
};
#define INT16                                            short
#define INT32                                            int

typedef struct
{
    INT16  mode;
    INT16  reset;
    INT16  imag_width;
    INT16  imag_height;
    INT32  imag_quality;
    INT32  inStreamBufAddr;
    INT32  outStreamBufAddr;
} VPP_JPEG_AMRRING_ENC_IN_T;


//struct of output parameters
typedef struct
{
    short  Mode;
    short  nbChannel;
    int    output_len;
    short  ImgWidth;
    short  ImgHeight;
    short  consumedLen;
    short  streamStatus;
    short  reserv1;
    short  reserv2;
} VPP_VPP_CODEC_OUT_T;


///////////////////////////////////////////////////
void  analyze_bandwidth(int* array , int length);
extern int  VPP_ENCODE_AMR(int inputaddr, int outputaddr);
extern int  VPP_ENCODE_JPEG(int inputaddr, int outputaddr,short width, short height , short quailty);
extern void GetStatus(VPP_VPP_CODEC_OUT_T*  pstatus,short voc_addr);
extern void VPP_MP4_ENC_CLOSE(void);
extern void VPP_MP4_ENC_INIT(void);
extern void COOLSAND_ENCODE_JPEG(void);
extern void  DMAE_READDATA(void);
//extern void  DMAI_MOVEDATA(void);
extern void  DMAI_WRITE(void);
extern void COOLSAND_encode_block(void);
extern void apply_fdct(void);
extern void format_number(void);
extern void COOLSAND_put_bits(void);
extern void COOLSNAD_flush_bits(void);
extern void  COOLSNAD_DIV(void);
/*****************************
 **    Common functions     **
 *****************************/

void COOLSAND_ENCODE_AMRRING(void);
void  DMAI_WRITE(void);
void COOLSAND_encode_block(void);
void apply_fdct(void);
void format_number(void);
void COOLSAND_put_bits(void);
void COOLSNAD_flush_bits(void);
void COOLSAND_ENCODE_AMRRING(void);
void  MCU_QUA_RGB2YUV411(void);
void  MCU_QUA_RGB2YUV422(void);
void  MCU_QUA_RGB2YUV444(void);
void  COOLSAND_YUVJPEG422(void);
void Convert_YUV422_Block(void);
void Convert_YUV411_Block(void);
void COOLSAND_Double2SingleLastRow_YUV(void);
void COOLSAND_Double2Single_YUV(void);
void COOLSAND_Double2SingleLastRow_YUV(void);
void COOLSAND_Double2Row_YUV(void);
void COOLSAND_1280BY1024_YUV422_JPEG(void);
void COOLSAND_MEMCopy_YUV(void);

void  COOLSAND_YUV2JPEG411(void);
void COOLSAND_RGB2JPEG(void);
void  COOLSAND_RGB2JPEG411(void);
void  COOLSAND_RGB2JPEG422(void);
void  COOLSAND_RGB2JPEG444(void);
void DEMAI_READ_RGBDATA(void);
void COOLSAND_1280BY1024_JPEG(void);
void COOLSAND_DMAIRead_1280BY1024Odd(void);
void COOLSAND_DMAIRead_1280BY1024Even(void);
void COOLSAN_Encode422MCUBlock(void);
void COOLSAND_Double2Row(void);
void COOLSAND_Double2SingleLastRow(void);
void COOLSAND_Double2Single(void);
void COOLSAND_MEMCopy(void);

void CII_RunTxAudioFilter(void);
void CII_RunAntiDistorsionFilter (void);
void CII_LoadSpFormatFilter (void);
void CII_encoder_homing_frame_test(void);
void clear_stall_pipe (void);
void CII_NORM_L_ACC0(void);
void CII_NORM_L_ACC0_HR(void);
void CII_reload_sideinfo(void);
void CII_reload_mode(void);
void CII_prepare_reload(void);
void CII_DigitalGain (void);
//void CII_DigitalRamp(void);
void CII_reload_section(void);
void COOLSAND_JpegEncodeWithoutZoom(void);
void COOLSAND_JpegDmaMcu(void);
void COOLSAND_JpegBlock(void);
void COOLSAND_JpegEncodeWithZoom(void);
void COOLSAND_JpegDmaMcuWithZoom(void);
void COOLSAND_JpegBlockWithZoom(void);
void COOLSAND_RGBToYUV422(void);


/*****************************
 **   AMR/EFR functions     **
 *****************************/

void CII_AMR_Decode(void);
void CII_AMR_Encode(void);
void CII_energy_new(void);
void CII_agc(void);
void CII_decode_2i40_11bits(void);
void CII_decode_3i40_14bits(void);
void CII_Reorder_lsf(void);
void CII_D_plsf_3(void);
void CII_D_plsf_5_sub(void);
void CII_Int_lsf(void);
void CII_ec_gain_code (void);
void CII_ec_gain_code_update (void);
void CII_ec_gain_pitch (void);
void CII_ec_gain_pitch_update (void);
void CII_dtx_com(void);
void CII_dtx_com_s2(void);
void CII_bits2prm_amr_efr (void);
void CII_Decoder_amr (void);
void CII_Speech_Decode_Frame(void);
void CII_dtx_dec(void);
void CII_reset_rx_dtx(void);
void CII_rx_dtx(void);
void CII_dec_reset(void);
void CII_Post_Filter_reset (void);
void CII_R02R3_p_R6(void);
void CII_Inv_sqrt(void);
void CII_Lsp_lsf (void);
void CII_Lsf_lsp(void);
void CII_Log2_norm(void);
void CII_Pred_lt_3or6 (void);
void CII_Lsp_Az(void);
void CII_build_code_com2(void);
void CII_lpc_M122(void);
void CII_Int_lpc_1and3(void);
void CII_Int_lpc_1to3(void);
void CII_dtx_buffer(void);
void CII_dtx_enc(void);
void CII_lsp_M122(void);
void CII_Q_plsf_3(void);
void CII_check_lsp(void);
void CII_Pitch_ol_M475_515(void);
void CII_Pitch_ol_M122(void);
void CII_Lag_max_M122(void);
void CII_Pitch_ol_wgh(void);
void CII_gmed_n(void);
void CII_Lag_max_M102(void);
void CII_SEARCH_COPY(void);
void CII_subframePreProc(void);
void CII_subframePostProc(void);
void CII_copy(void);
void CII_copy_xy(void);
void CII_scale(void);
void CII_scale_xy(void);
void CII_Autocorr(void);
void CII_Lag_window(void);
void CII_Levinson (void);
void CII_Az_lsp(void);
void CII_Q_plsf_5(void);
void CII_Lsf_wt (void);
void CII_Vq_subvec (void);
void CII_MR475_gain_quant_subr2_475(void);
void CII_MR475_gain_quant_subr2_102(void);
void CII_MR475_gain_quant_subr2_515(void);
void CII_MR475_gain_quant_subr2_59(void);
void CII_MR475_gain_quant_subr2_74(void);
void CII_Pre_Process(void);
void CII_Weight_Ai(void);
void CII_Residu_new(void);
void CII_Syn_filt(void);
void CII_Convolve(void);
void CII_G_pitch(void);
void CII_q_gain_code(void);
void CII_CN_encoding(void);
void CII_sid_codeword_encoding(void);
void CII_EFR_LEVINSON_PART (void);
void CII_Chebps (void);
void CII_cor_h(void);
void CII_cor_h_x (void);
void CII_set_sign_475 (void);
void CII_set_sign_515 (void);
void CII_set_sign_59 (void);
void CII_set_sign_67 (void);
void CII_set_sign_74 (void);
void CII_pseudonoise (void);
void CII_Int_lpc_only(void);
void CII_Int_lpc_only_2(void);
void CII_DIV_32(void);
void CII_update_gcode0_CN (void);
void CII_Pow2 (void);
void CII_Log2 (void);
void CII_code_2i40_9bits_475(void);
void CII_code_2i40_9bits_515(void);
void CII_sqrt_l_exp (void);
void CII_calc_target_energy(void);
void CII_MR795_gain_quant(void);
void CII_cbseach_subr1(void);
void CII_search_4i40(void);
void CII_COMPUTE_ALP1_M74(void);
void CII_COMPUTE_ALP1_M74_795(void);
void CII_cbseach_cala_track_475(void);



void CII_search_M475_475(void);
void CII_search_M475_515(void);
void CII_build_code_M475_475(void);
void CII_build_code_M475_515(void);
void CII_code_2i40_11bits(void);
void CII_search_M59(void);
void CII_build_code_MR59(void);
void CII_code_3i40_14bits(void);
void CII_search_3i40(void);
void CII_build_code_MR67(void);
void CII_code_8i40_31bits (void);
void CII_set_sign12k2_102 (void);
void CII_set_sign12k2_122 (void);
void CII_COMPUTE_RRV_M102(void);
void CII_COMPUTE_RRV_M122(void);
void CII_COMPUTE_ALP1_M102(void);
void CII_COMPUTE_ALP1_M122(void);
void CII_SEARCH_LOOP_M102(void);
void CII_SEARCH_LOOP_M122(void);
void CII_build_code_M102 (void);
void CII_code_10i40_35bits (void);
void CII_build_code_M122 (void);
void CII_search_M122(void);
void CII_gc_pred_update(void);
void CII_gc_pred(void);
void CII_gainQuant_M475(void);
void CII_gainQuant_M122(void);
void CII_calc_filt_energies_102(void);
void CII_calc_filt_energies_475(void);
void CII_calc_filt_energies_515(void);
void CII_calc_filt_energies_59(void);
void CII_calc_filt_energies_67(void);
void CII_calc_filt_energies_74(void);
void CII_L_shr_r(void);
void CII_Copy_M(void);
void CII_gainQuant_102(void);
void CII_gainQuant_515(void);
void CII_gainQuant_59(void);
void CII_gainQuant_67(void);
void CII_gainQuant_74(void);
void CII_gainQuant_795(void);
void CII_calc_filt_energies_subr_102(void);
void CII_calc_filt_energies_subr_475(void);
void CII_calc_filt_energies_subr_515(void);
void CII_calc_filt_energies_subr_59(void);
void CII_calc_filt_energies_subr_67(void);
void CII_calc_filt_energies_subr_74(void);
void CII_calc_filt_energies_subr_795(void);
void CII_Int_lpc_1and3_dec(void);
void CII_mr475_quant_sub(void);
void CII_MR475_quant_store_results(void);
void CII_code_4i40_17bits_74(void);
void CII_code_4i40_17bits_795(void);
void CII_cbseach_cala_cod_475(void);
void CII_cbseach_cala_cod_515(void);
void CII_cbseach_cala_cod_59(void);
void CII_cbseach_cala_cod_74(void);
void CII_cbseach_clear_CODE_475(void);
void CII_cbseach_clear_CODE_515(void);
void CII_cbseach_clear_CODE_59(void);
void CII_cbseach_clear_CODE_74(void);
void CII_cbseach_clear_CODE_102(void);
void CII_Pitch_ol_M795_59(void);
void CII_Pitch_ol_M795_67(void);
void CII_Pitch_ol_M795_74(void);
void CII_Pitch_ol_M475_515(void);
void CII_Pitch_ol_M475_475(void);
void CII_Lag_max_M475_475(void);
void CII_Lag_max_M475_515(void);
void CII_Lag_max_M475_59(void);
void CII_Lag_max_M475_67(void);
void CII_Lag_max_M475_74(void);
void CII_Lag_max_M475_795(void);
void CII_COMPUTE_ALP1_M67_475(void);
void CII_COMPUTE_ALP1_M67_515(void);
void CII_COMPUTE_ALP1_M67_59(void);
void CII_MR475_gain_quant_subr_475(void);
void CII_MR475_gain_quant_subr_515(void);
void CII_MR475_gain_quant_subr_59(void);
void CII_MR475_gain_quant_subr_67(void);
void CII_MR475_gain_quant_subr_74(void);
void CII_MR475_gain_quant_subr_102(void);
void CII_search_M102(void);
void CII_COD_AMR_475(void);
void CII_COD_AMR_515(void);
void CII_COD_AMR_59(void);
void CII_COD_AMR_67(void);
void CII_COD_AMR_74(void);
void CII_COD_AMR_795(void);
void CII_COD_AMR_102(void);
void CII_COD_AMR_122(void);
void CII_tx_dtx_handler(void);
void CII_encoder_reset (void);
void CII_vad1(void);
void CII_reset_rx_dtx(void);
void CII_update_lsf_p_CN(void);
void CII_interpolate_CN_lsf(void);
void CII_update_lsf_history(void);
void CII_build_CN_code(void);
void CII_interpolate_CN_param (void);
void CII_update_gain_code_history_rx (void);
void CII_rx_dtx_handler(void);
void CII_efr_dtx_func1(void);
void CII_efr_dtx_func2(void);
void CII_efr_dtx_func3(void);
void CII_efr_dtx_func4(void);
void CII_efr_dtx_func5(void);
void CII_efr_dtx_func6(void);
void CII_efr_dtx_func7(void);
void CII_efr_dtx_func8(void);
void CII_efr_dtx_func9(void);
void CII_efr_dtx_func11(void);
void CII_amr_dec_com_sub1(void);
void CII_amr_dec_com_sub3(void);
void CII_amr_dec_com_sub4(void);
void CII_amr_dec_com_sub5(void);
void CII_amr_dec_475(void);
void CII_amr_dec_515(void);
void CII_amr_dec_59(void);
void CII_amr_dec_67(void);
void CII_amr_dec_74(void);
void CII_amr_dec_795(void);
void CII_amr_dec_102(void);
void CII_amr_dec_122(void);
void CII_schur_recursion (void);
void CII_step_up (void);
void CII_compute_rav1 (void);
void CII_vad_computation (void);
void CII_tx_dtx(void);
void CII_aver_lsf_history(void);
void CII_update_lsf_p_CN(void);
void CII_periodicity_update(void);
void CII_compute_CN_excitation_gain(void);
void CII_update_gain_code_history_tx(void);
void CII_vad_pitch_detection(void);
void CII_PRM2_BITS_POST_PROCESS(void);
void CII_PRM2_BITS_POST_PROCESS_EFR(void);
void CII_EFR_DTX_PART1(void);
void CII_plsf5_dtx1(void);
void CII_prm2bits_amr_efr(void);
void CII_EFR_DTX_ENC_RESET(void);
void CII_AMR_subfunc0(void);
void CII_Dec_lag3_67(void);
void CII_Dec_lag3_59(void);
void CII_Dec_lag3_515(void) ;
void CII_gc_pred_M122_dec(void);
void CII_plsf5_dtx2(void);
void CII_plsf5_dtx3(void);
void CII_plsf5_dtx4(void);
void CII_decode_2i40_9bits_515(void);
void CII_lsp_795(void);
void CII_decode_4i40_17bits_795(void);
void CII_Enc_lag3_74(void);
void CII_lsp_74(void);
void CII_decode_4i40_17bits_74(void);
void CII_Enc_lag3_67(void);
void CII_Enc_lag3_59(void);
void CII_lsp_59(void);
void CII_Enc_lag3_515(void);
void CII_Enc_lag3_475(void);
void CII_lsp_475(void);
void CII_lsp_122(void);
void CII_lsp_67(void);
void CII_lsp_515(void);
void CII_Enc_lag3_102(void);
void CII_lsp_102(void);
void CII_dtx_efr_dec_reset(void);
void CII_SUBFRAME_EFR_PART(void);
void CII_RESET_AMR_CONSTANT(void);
void CII_Post_Process(void);


void  COOLSAND_ZOOM_JpgeEnc(void);
void CoolSand_Div2(void);
void CoolSand_SignBits2(void);
void  COOLSAND_AverageTwoLine(void);
void Coolsand_zoomline_yuv(void);
void  COOLSAND_DMA_READ_ORG(void);
void  TEMP_DMA_WRITE(void);
void COOLSAND_ENCODE_YUVBLOCK(void);


// VoC_directive: PARSER_ON

#endif  // VPPP_AUDIOJPEGENC_AMRJPEGENC_H