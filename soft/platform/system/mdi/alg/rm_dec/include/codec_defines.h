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



#ifndef CODEC_DEFINES_H
#define CODEC_DEFINES_H

/* Audio codec defines */
#define RA_FORMAT_ID        0x2E7261FD  /* RealAudio Stream */
#define RA_LOSSLESS_ID      0x4C53443A  /* RealAudio Lossless Identifier */
#define RA_NO_INTERLEAVER   0x496E7430  /* 'Int0' (no interleaver) */
#define RA_INTERLEAVER_SIPR 0x73697072  /* interleaver used for SIPRO codec ("sipr") */
#define RA_INTERLEAVER_GENR 0x67656E72  /* interleaver used for ra8lbr and ra8hbr codecs ("genr") */
#define RA_INTERLEAVER_VBRS 0x76627273  /* Simple VBR interleaver ("vbrs") */
#define RA_INTERLEAVER_VBRF 0x76627266  /* Simple VBR interleaver (with possibly fragmenting) ("vbrf") */

//ra codec 4cc. add sheen
#define RA_CODEC_SIPR   0x73697072  /*realaudio voice*/
#define RA_CODEC_COOK   0x636F6F6B  /*realaudio 8 low bit rate*/
#define RA_CODEC_ATRC   0x61747263  /*realaudio 8 high bit rate*/
#define RA_CODEC_RAAC   0x72616163  /*AAC*/
#define RA_CODEC_RACP   0x72636170  /*AAC*/
//add.end

/* Video codec defines */
#define HX_MEDIA_AUDIO    0x4155444FL /* 'AUDO' */
#define HX_MEDIA_VIDEO    0x5649444FL /* 'VIDO' */

#define HX_RVTRVIDEO_ID   0x52565452  /* 'RVTR' (for rv20 codec) */
#define HX_RVTR_RV30_ID   0x52565432  /* 'RVT2' (for rv30 codec) */
#define HX_RV20VIDEO_ID   0x52563230  /* 'RV20' */
#define HX_RV30VIDEO_ID   0x52563330  /* 'RV30' */
#define HX_RV40VIDEO_ID   0x52563430  /* 'RV40' */
#define HX_RVG2VIDEO_ID   0x52564732  /* 'RVG2' (raw TCK format) */
#define HX_RV89COMBO_ID   0x54524F4D  /* 'TROM' (raw TCK format) */

#define RV20_MAJOR_BITSTREAM_VERSION  2

#define RV30_MAJOR_BITSTREAM_VERSION  3
#define RV30_BITSTREAM_VERSION        2
#define RV30_PRODUCT_RELEASE          0
#define RV30_FRONTEND_VERSION         2

#define RV40_MAJOR_BITSTREAM_VERSION  4
#define RV40_BITSTREAM_VERSION        0
#define RV40_PRODUCT_RELEASE          2
#define RV40_FRONTEND_VERSION         0

#define RAW_BITSTREAM_MINOR_VERSION   128

#define RAW_RVG2_MAJOR_VERSION        2
#define RAW_RVG2_BITSTREAM_VERSION    RAW_BITSTREAM_MINOR_VERSION
#define RAW_RVG2_PRODUCT_RELEASE      0
#define RAW_RVG2_FRONTEND_VERSION     0

#define RAW_RV8_MAJOR_VERSION         3
#define RAW_RV8_BITSTREAM_VERSION     RAW_BITSTREAM_MINOR_VERSION
#define RAW_RV8_PRODUCT_RELEASE       0
#define RAW_RV8_FRONTEND_VERSION      0

#define RAW_RV9_MAJOR_VERSION         4
#define RAW_RV9_BITSTREAM_VERSION     RAW_BITSTREAM_MINOR_VERSION
#define RAW_RV9_PRODUCT_RELEASE       0
#define RAW_RV9_FRONTEND_VERSION      0

#define WIDTHBYTES(i) ((unsigned long)((i+31)&(~31))/8)  /* ULONG aligned ! */
#define T_YUV420   10

#endif /* #ifndef CODEC_DEFINES_H */
