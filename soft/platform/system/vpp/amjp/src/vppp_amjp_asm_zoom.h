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


#define   Q15      8192//16384//32768
#define   Q14      4096//8192//16384
#define   S15      13//14// 15
#define   MASK     0x00001fff//Q15-1

//#define   NUMIN_LINE       22  //x
//#define   NUMIN_LINE_1     21  //x - 1
//#define   NUMIN_LINE_2     20  //x - 2
//#define   NUMIN_LINE_UV    11  //x/2
//#define   NUMIN_LINE_UV_1  10  //x/2 -1

#define  INTPUT_COUNT      4000//X*4/3
#define  OUTPUT_COUNT      2560
#define  ZOOM_CONST_Y_SIZE   69

void vpp_AmjpZoom(void);
void CoolSand_SignBits2(void);
void CoolSand_Div2(void);
void Coolsand_UpdateP3_addr(void);
void CoolSand_Updatep3p4(void);
void Coolsand_UpdateP4_addr(void);
void Coolsand_zoomline_yuv411(void);
void COOLSAND_DMA_READ(void);
void Coolsand_bilinear_yuv4112rgb(void);

void vpp_AmjpDecompressVssAdpcm16Table(void);
void vpp_AmjpZoomGenerateYuv2RgbTable(void);
