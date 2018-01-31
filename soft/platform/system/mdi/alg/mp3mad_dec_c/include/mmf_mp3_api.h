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











#ifndef _MMF_MP3_API_H_
#define _MMF_MP3_API_H_
//#include "cswtype.h"

typedef struct _Mp3FrmInfo
{
    int bitrate;
    int nChans;
    int samprate;
    int bitsPerSample;
    int outputSamps;
} Mp3FrmInfo;

int MMF_Mp3_OpenDecoder();
int MMF_Mp3_CloseDecoder();
int MMF_Mp3_DecodeFrame(unsigned char ** inputBuf, unsigned int *bytesleft, unsigned char *outputBuf, unsigned int initFlag);
void MMF_Mp3_GetFrmInfo(Mp3FrmInfo *pInfo);

#endif
