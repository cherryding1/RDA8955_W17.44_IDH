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



#ifndef _AACCOMMON_H
#define _AACCOMMON_H

#define READBUF_SIZE_divs_2  1024 // READBUF_SIZE/2
#define MAX_MS_MASK_BYTES_divs_2 8 //MAX_MS_MASK_BYTES/2



#define READBUF_SIZE    2048//(2 * AAC_MAINBUF_SIZE * AAC_MAX_NCHANS)   /* pick something big enough to hold a bunch of frames */

#define Tab_huffTabSpec_size 624
#define Tab_cos4sin4tab_size 1152
#define Tab_twidTabOdd_size 1152 //1008
#define Tab_sinWindow_size 1152
#define Tab_kbdWindow_size 1152

#define AAC_ConstX_size 0
#define AAC_ConstY_size (AAC_Dec_ConstY_end-AAC_Dec_ConstY_start+1)/2



/* according to spec (13818-7 section 8.2.2, 14496-3 section 4.5.3)
 * max size of input buffer =
 *    6144 bits =  768 bytes per SCE or CCE-I
 *   12288 bits = 1536 bytes per CPE
 *       0 bits =    0 bytes per CCE-D (uses bits from the SCE/CPE/CCE-I it is coupled to)
 */
#ifndef AAC_MAX_NCHANS              /* if max channels isn't set in makefile, */
#define AAC_MAX_NCHANS      2       /* set to default max number of channels  */
#endif
#define AAC_MAX_NSAMPS      1024
#define AAC_MAINBUF_SIZE    (768 * AAC_MAX_NCHANS)

#define AAC_NUM_PROFILES    3
#define AAC_PROFILE_MP      0
#define AAC_PROFILE_LC      1
#define AAC_PROFILE_SSR     2

// #define  HELIX_FEATURE_AUDIO_CODEC_AAC_SBR
/* define these to enable decoder features */
#if defined(HELIX_FEATURE_AUDIO_CODEC_AAC_SBR)
#define AAC_ENABLE_SBR
#endif //  HELIX_FEATURE_AUDIO_CODEC_AAC_SBR.
#define AAC_ENABLE_MPEG4


#define NWINDOWS_LONG           1
#define NWINDOWS_SHORT          8

#define DATA_BUF_SIZE           510             /* max count = 255 + 255 */
#define FILL_BUF_SIZE           269             /* max count = 15 + 255 - 1*/
#define ADIF_COPYID_SIZE        9
#define MAX_COMMENT_BYTES       255

#define MAX_NUM_FCE                     15
#define MAX_NUM_SCE                     15
#define MAX_NUM_BCE                     15
#define MAX_NUM_LCE                      3
#define MAX_NUM_ADE                      7
#define MAX_NUM_CCE                     15



#define MAX_HUFF_BITS                   20
#define HUFFTAB_SPEC_OFFSET             1


#define SF_DQ_OFFSET            15
#define FBITS_OUT_DQ            20
#define FBITS_OUT_DQ_OFF        (FBITS_OUT_DQ - SF_DQ_OFFSET)   /* number of fraction bits out of dequant, including 2^15 bias */

#define FBITS_IN_IMDCT          FBITS_OUT_DQ_OFF        /* number of fraction bits into IMDCT */
#define GBITS_IN_DCT4           4                                       /* min guard bits in for DCT4 */

#define FBITS_LOST_DCT4         1               /* number of fraction bits lost (>> out) in DCT-IV */
#define FBITS_LOST_WND          1               /* number of fraction bits lost (>> out) in synthesis window (neg = gain frac bits) */
#define FBITS_LOST_IMDCT        (FBITS_LOST_DCT4 + FBITS_LOST_WND)
#define FBITS_OUT_IMDCT         (FBITS_IN_IMDCT - FBITS_LOST_IMDCT)

#define NUM_IMDCT_SIZES         2


/* 12-bit syncword */
#define SYNCWORDH           0xff
#define SYNCWORDL           0xf0

#define MAX_NCHANS_ELEM     2   /* max number of channels in any single bitstream element (SCE,CPE,CCE,LFE) */

#define ADTS_HEADER_BYTES   7
#define NUM_SAMPLE_RATES    12
#define NUM_DEF_CHAN_MAPS   8
#define NUM_ELEMENTS        8
#define MAX_NUM_PCE_ADIF    16

#define MAX_WIN_GROUPS      8
#define MAX_SFB_SHORT       15
#define MAX_SF_BANDS        (MAX_SFB_SHORT*MAX_WIN_GROUPS)  /* worst case = 15 sfb's * 8 windows for short block */
#define MAX_MS_MASK_BYTES   ((MAX_SF_BANDS + 7) >> 3)
#define MAX_PRED_SFB        41
#define MAX_TNS_FILTERS     8
#define MAX_TNS_COEFS       60
#define MAX_TNS_ORDER       20
#define MAX_PULSES          4
#define MAX_GAIN_BANDS      3
#define MAX_GAIN_WIN        8
#define MAX_GAIN_ADJUST     7

#define NSAMPS_LONG         1024
#define NSAMPS_SHORT        128

#define NUM_SYN_ID_BITS     3
#define NUM_INST_TAG_BITS   4

#define EXT_SBR_DATA        0x0d
#define EXT_SBR_DATA_CRC    0x0e


void vpp_AmjpCircBuffDmaRd(void);
void vpp_AmjpCircBuffInit(void);
void vpp_AmjpAudioEqualizer(void);
void vpp_AmjpDecompressVssAdpcmTable(void);
void vpp_AmjpAacGenerateHuffTabSpecInfo(void);
void vpp_AmjpAacGenerateCos4Sin4(void);
void vpp_AmjpAacGenerateSinwKbdw(void);
void vpp_AmjpAacGenerateTwidOdd(void);
void vpp_AmjpAacGenerateTwidEven(void);
void vpp_AmjpAacGenerateCos1(void);
void vpp_AmjpAacGenerateBitRev(void);
void vpp_AmjpAacGenerateSfBandLong(void);

void Coolsand_UnpackADTSHeader(void);  //xuml
void Coolsand_DecodeNextElement(void); //xuml
void Coolsand_DecodeICSInfo(void); //xuml
void Coolsand_DecodeNoiselessData(void);//xuml
void Coolsand_DecodeHuffmanScalar(void);//xuml
void Coolsand_DecodeSpectrumShort(void);//xuml
void Coolsand_DecodeSpectrumLong(void);//xuml
void Coolsand_Dequantize(void);//xuml
void Coolsand_CLZ(void);//xuml
void Coolsand_MULSHIFT32(void);//xuml
void Coolsand_StereoProcess(void);//xuml
void Coolsand_TNSFilter(void);//xuml
void Coolsand_MADD64(void);//xuml
void Coolsand_IMDCT(void);//xuml
void Coolsand_R4FFT(void);//xuml
void Coolsand_DCT4(void);//xuml
void Coolsand_AACDecode(void);//xuml
void Coolsand_AACFindSyncWord(void);//xuml
void Coolsand_reload_data(void);

void Coolsand_PNS(void);
void Coolsand_CopyNoiseVector(void);
void Coolsand_GenerateNoiseVector(void);


#endif  /* _AACCOMMON_H */
