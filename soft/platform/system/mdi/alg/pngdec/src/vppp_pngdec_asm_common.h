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


#define DEBUG_STEP


void vpp_PngDecMain(void);
void vpp_PngDecRunDma(void);
void vpp_PngDecCodeConstReload(void);




void vpp_PngDecBye(void);
void vpp_PngDecByeTest(void);

#define GLOBAL_CONST_SIZE 319

#define GLOBAL_CONST_LENFIX_DISTFIX_SIZE (((512+32)*3)/2)


#define GLOBAL_WRAP_BIT 10


void Coolsand_PngDec(void);
void Coolsand_PNG_Init(void);
void Coolsand_IHDRHandle(void);
void Coolsand_crc32(void);


void Coolsand_cHRMHandle(void);
void Coolsand_gAMAHandle(void);
void Coolsand_iCCPHandle(void);
void Coolsand_sBITHandle(void);
void Coolsand_sRGBHandle(void);
void Coolsand_PLTEHandle(void);
void Coolsand_bKGDHandle(void);
void Coolsand_hISTHandle(void);
void Coolsand_tRNSHandle(void);
void Coolsand_pHYsHandle(void);
void Coolsand_sPLTHandle(void);
void Coolsand_IDATHandle(void);
void Coolsand_tIMEHandle(void);
void Coolsand_iTXtHandle(void);
void Coolsand_tEXtHandle(void);
void Coolsand_zTXtHandle(void);
void Coolsand_IENDHandle(void);
void Coolsand_UnknownHandle(void);


void Coolsand_UpdateInfo(void);
void Coolsand_PngRowInit(void);
void Coolsand_UpdateTransformInfo(void);
void Coolsand_InitTransform(void);
void Coolsand_ProcessOneRow(void);
void Coolsand_ReadFinishRow(void);
void Coolsand_inflate(void);
void Coolsand_inflateReset(void);
void Coolsand_inflateInit(void);
void Coolsand_inflate_table(void);
void Coolsand_inflate_fast(void);
void Coolsand_CopyBytesFromOutBuf(void);
void Coolsand_CopyBytesFromWindow(void);

void Coolsand_DmaInData(void);
void Coolsand_CRC2(void);

void Coolsand_NEEDBITS(void);
void Coolsand_DROPBITS(void);
void Coolsand_BITS(void);
void Coolsand_BYTEBITS(void);
void Coolsand_INITBITS(void);
void Coolsand_PULLBYTE(void);

void Coolsand_updatewindow(void);
void Coolsand_zmemcpyFromVoC(void);
void Coolsand_FilterRow(void);

void Coolsand_CombineRow(void);
void Coolsand_ImageTransform(void);
void Coolsand_ExpandImage(void);
void Coolsand_ExpandPalette(void);
void Coolsand_ExpandGrey2RGB(void);
void Coolsand_BkgdProcess(void);
void Coolsand_Chop16to8(void);
void Coolsand_zmemcpyToVoC(void);

void Coolsand_fixedtables(void);


// 错误码定义
#define PNG_NO_ERROR 0
// 关键块
#define IHDRERR 1
#define IHDRCRCERR 2
#define PLTEERR 3
#define PLTECRCERR 4
#define IDATERR 5
#define IDATCRCERR 6
#define IENDERR 7
#define IENDCRCERR 8

// 辅助块
#define gAMAERR 9
#define gAMACRCERR 10
#define tRNSERR 11
#define tRNSCRCERR 12
#define sBITERR 13
#define sBITCRCERR 14
#define bKGDERR 15
#define bKGDCRCERR 16
#define cHRMERR 17
#define cHRMCRCERR 18
#define hISTERR 19
#define hISTCRCERR 20
#define iCCPERR 21
#define iCCPCRCERR 22
#define iTXtERR 23
#define iTXtCRCERR 24
#define pHYsERR 25
#define pHYsCRCERR 26
#define sPLTERR 27
#define sPLTCRCERR 28
#define sRGBERR 29
#define sRGBCRCERR 30
#define tEXtERR 31
#define tEXtCRCERR 32
#define tIMEERR 33
#define tIMECRCERR 34
#define zTXtERR 35
#define zTXtCRCERR 36
#define UnknownERR 37
#define UnknownCRCERR 38
#define ChunkExtra 39

#define INFLATEERR 40
#define RESIZEERR 41
#define INPUTERR 42
#define MEMERR 43
#define SUPPORTERR 44
#define CLIPOVER 45 //clip area decode over,not an err.sheen

#define CRCERR 46 //CRC error




// 色彩类型
#define GreyScale 0
#define TrueColor 2
#define IndexColor 3
#define GreyScaleAlpha 4
#define TrueColorAlpha 6

// 图像数据变换
#define EXPAND 0x01
#define DEPTH16to8 0x02
#define tRNSEXPAND 0x04
#define GREY2RGB 0x08
#define bKGDEXPAND 0x10
#define gAMAEXPAND 0x20
#define INTERLACE 0x40

#define DEFAULTbKGD 0x80

//in coolsand plarform, we support back ground image
//that would replace the default back ground colour  added by elisa 2009-10-9
#define BKGDImage   0x100




#define Z_DEFLATED   8
/* The deflate compression method (the only one supported in this version) */



#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define MAXBITS 15

#define ENOUGH 2048
#define MAXD 592



// Filter Types
#define FilterTypeNone 0
#define FilterTypeSub 1
#define FilterTypeUp 2
#define FilterTypeAverage 3
#define FilterTypePaeth 4


// sRGB
#define sRGBPerceptual 0
#define sRGBRelative 1
#define sRGBSaturation 2
#define sRGBAbsolute 3
#define sRGBRenderingLast 4


// VoC_directive: PARSER_ON


