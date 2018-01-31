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




#define JPEG_Dec_ConstX_size 0
#define JPEG_Dec_ConstY_size 770

// VoC_directive: PARSER_OFF
//JPEG Decode function
void CII_JPEG_Decode(void);
void CII_DecodeMCUBlock(void);
void CII_StoreBuffer(void);
void CII_StoreBuffer1(void);
void CII_storebuffer_intern(void);
void CII_HufBlock(void);
void CII_ReadByte(void);
void CII_DecodeLumDc(void);
void CII_DecodeChromDc(void);
void CII_DecodeLumAc(void);
void CII_DecodeChromAc(void);
void CII_DecodeElement(void);
void CII_IDCT_11_C(void);
void CII_IDCT_44_C(void);
void CII_IDCT_88_C(void);
void CII_idctrow(void);
void CII_idctcol(void);
void CII_idctrow4(void);
void CII_idctcol4(void);
void CII_L_mpy_sl_opt(void);
void CII_small_image(void);

void CII_DMAI_READDATA(void);
void CII_DMAI_WRITEDATA(void);
void CII_DMAI_SYNCDATA(void);
void CII_DMAI_WRITEISPDATA(void);

void CII_zoom(void);

// VoC_directive: PARSER_ON



