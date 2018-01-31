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

#define AGC1_ConstY_size 96


void Agc3_Main(void);
void Agc_ProcessDigital(void);

void Agc1_Main(void);
void Agc_Init(void);
void Agc_Set_Config(void);
void CII_div_mode32(void);
void Coolsand_CLZU(void);
void Coolsand_CLZS(void);
void Div_SignedInt(void);


void Agc_ProcessVad(void);
void VOC_Agc_DownsampleBy2(void);
void MUL_ACCUM_FUN(void);

void VOC_Agc_WebRtcSpl_Sqrt(void);
void VOC_WebRtcSpl_NormW32(void);
void VOC_WebRtcSpl_SqrtLocal(void);
void VOC_Agc_MATH_CLZ(void);

void VOC_WebRtcSpl_DivW32W16(void);
void VOC_WebRtcSpl_DivW32W16ResW16(void);
void VOC_MATH_POSDIV32_S(void);

void VOC_MUL_W32W16ResW32(void);
