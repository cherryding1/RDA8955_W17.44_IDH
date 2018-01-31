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


#include "aac_resample.h"


HWORD WordToHword(int v, int scl)
{
    HWORD out;
    int llsb = (1<<(scl-1));
    v += llsb;          /* round */
    v >>= scl;
    if (v>MAX_HWORD)
    {
        v = MAX_HWORD;
    }
    else if (v < MIN_HWORD)
    {
        v = MIN_HWORD;
    }
    out = (HWORD) v;
    return out;
}
int WordToHwordDou(int left_v, int right_v, int scl)
{
    int out;
    int llsb = (1<<(scl-1));
    left_v += llsb;          /* round */
    left_v >>= scl;
    right_v += llsb;          /* round */
    right_v >>= scl;

    if (left_v>MAX_HWORD)
    {
        left_v = MAX_HWORD;
    }
    else if (left_v < MIN_HWORD)
    {
        left_v = MIN_HWORD;
    }
    if (right_v>MAX_HWORD)
    {
        right_v = MAX_HWORD;
    }
    else if (right_v < MIN_HWORD)
    {
        right_v = MIN_HWORD;
    }

    out = (int) (((unsigned short)right_v) << 16 | (unsigned short)left_v);
    return out;
}



typedef struct RESAMPLE
{
    int nChans ;
    int insrate ;
    int newsrate ;
    int history;
    int dtb;
    int blocksize;
} RESAMPLE;
RESAMPLE sResample;

#if 0
static int ResampleLinear(int X[], int Y[], UHWORD Nx)
{
    HWORD iconst;
    HWORD *Xp;
    int *Ystart;
    int left_v,right_v,left_x1,left_x2, right_x1, right_x2;

    UWORD Time,endTime;              /* When Time reaches EndTime, return to user */
    Ystart = Y;
    Time = 0;
    endTime = 1<<Np;
    while (Time < endTime)
    {
        iconst = Time & Pmask;

        Xp = (HWORD *)(&sResample.history);
        left_x1 = *Xp++;
        right_x1 = *Xp++;
        Xp = (HWORD *)(&X[0]);
        left_x2 = *Xp++;
        right_x2 = *Xp;
        left_x1 *= ((1<<Np)-iconst);
        left_x2 *= iconst;
        right_x1 *= ((1<<Np)-iconst);
        right_x2 *= iconst;

        left_v = left_x1 + left_x2;
        right_v = right_x1 + right_x2;

        *Y++ = WordToHwordDou(left_v, right_v,Np);
        Time += sResample.dtb;
    }
    endTime = (1<<Np)*(int)(Nx) ;
    while (Time < endTime)
    {
        iconst = Time & Pmask;
        Xp = (HWORD *)(&X[(Time>>Np) - 1]);      /* Ptr to current input sample */

        left_x1 = *Xp++;
        right_x1 = *Xp++;
        left_x2 = *Xp++;
        right_x2 = *Xp;
        left_x1 *= ((1<<Np)-iconst);
        left_x2 *= iconst;
        right_x1 *= ((1<<Np)-iconst);
        right_x2 *= iconst;

        left_v = left_x1 + left_x2;
        right_v = right_x1 + right_x2;

        *Y++ = WordToHwordDou(left_v, right_v,Np);   /* Deposit output */
        Time += sResample.dtb;               /* Move to next sample by time increment */
    }
    sResample.history = X[Nx - 1];
    Time = Time - endTime;
    return (Y - Ystart);            /* Return number of output samples */
}
#endif

static int ResampleLinear_Mono(HWORD X[], HWORD Y[], UHWORD Nx)
{
    HWORD iconst;
    HWORD *Xp;
    HWORD *Ystart;
    int Mono_v,Mono_x1,Mono_x2;

    UWORD Time,endTime;              /* When Time reaches EndTime, return to user */
    Ystart = Y;
    Time = 0;
    endTime = 1<<Np;
    while (Time < endTime)
    {
        iconst = Time & Pmask;

        Mono_x1 = sResample.history;
        Xp = &X[0];
        Mono_x2 = *Xp;
        Mono_x1 *= ((1<<Np)-iconst);
        Mono_x2 *= iconst;

        Mono_v = Mono_x1 + Mono_x2;
        *Y++ = WordToHword(Mono_v, Np);
        Time += sResample.dtb;
    }
    endTime = (1<<Np)*(int)(Nx) ;
    while (Time < endTime)
    {
        iconst = Time & Pmask;
        Xp = &X[(Time>>Np) - 1];      /* Ptr to current input sample */

        Mono_x1 = *Xp++;
        Mono_x2 = *Xp;
        Mono_x1 *= ((1<<Np)-iconst);
        Mono_x2 *= iconst;

        Mono_v = Mono_x1 + Mono_x2;

        *Y++ = WordToHword(Mono_v,Np);   /* Deposit output */
        Time += sResample.dtb;               /* Move to next sample by time increment */
    }
    sResample.history = X[Nx - 1];
    Time = Time - endTime;
    return (Y - Ystart);            /* Return number of output samples */
}

/*
    初始化重采样
    inSample -- 输入数据的采样率
    inChan -- 输入数据的声道数
    layer -- 层号：1、2、3
    返回 -- 0 失败 1 成功
*/
int CII_InitResample(long inSample, long inChan, long layer)
{
    int nSampleNum;
    sResample.nChans = inChan;
    sResample.insrate = inSample;
    sResample.newsrate = 8000;
    sResample.history = 0;
    /*
    if ( layer >= 2)
    {
        switch (sResample.insrate)
        {
        case 8000:
            sResample.blocksize = 6352;
            break;
        case 11025:
            sResample.blocksize = 4608;
            break;
        case 12000:
            sResample.blocksize = 4232;
            break;
        case 16000:
            sResample.blocksize = 3176;
            break;
        case 22050:
            sResample.blocksize = 2304;
            break;
        case 24000:
            sResample.blocksize = 2116;
            break;
        case 32000:
            sResample.blocksize = 1588;
            break;
        case 44100:
            sResample.blocksize = 1152;
            break;
        case 48000:
            sResample.blocksize = 1060;
            break;
        default:
            sResample.dtb = 0;
            return 0;
        }
        nSampleNum = 1152;
    }
    else
    {
        switch (sResample.insrate)
        {
        case 8000:
            sResample.blocksize = 2116;
            break;
        case 11025:
            sResample.blocksize = 1536;
            break;
        case 12000:
            sResample.blocksize = 1410;
            break;
        case 16000:
            sResample.blocksize = 1060;
            break;
        case 22050:
            sResample.blocksize = 768;
            break;
        case 24000:
            sResample.blocksize = 704;
            break;
        case 32000:
            sResample.blocksize = 528;
            break;
        case 44100:
            sResample.blocksize = 384;
            break;
        case 48000:
            sResample.blocksize = 352;
            break;
        default:
            sResample.dtb = 0;
            return 0;
        }
            nSampleNum = 384;
    }*/

    if(inSample ==48000||inSample ==44100)
        nSampleNum = 256;
    else if(inSample ==24000||inSample ==22050)
        nSampleNum = 512;
    else
        nSampleNum = 1024;


    if(inSample ==48000||inSample ==24000||inSample ==12000)
        sResample.blocksize = 8000*nSampleNum/inSample;
    else if(inSample ==44100||inSample ==22050||inSample ==11025)
        sResample.blocksize = 8000*nSampleNum/inSample;
    else
        sResample.blocksize = 1;

    if(32768*nSampleNum % sResample.blocksize == 0)
        sResample.dtb = 32768*nSampleNum/sResample.blocksize;
    else
        sResample.dtb = 32768*nSampleNum/sResample.blocksize + 1;

    return 1;
}
/*
    重采样

    pSrc -- 输入数据的起始地址
    nSrcLen -- 输入数据的样本数
    pDec -- 输出数据的起始地址
    返回 -- 生成的新的样本的数量
    备注 -- 每首歌开始的时候必许要初始化
*/
long CII_Resample (unsigned int *pSrc, long nSrcLen, unsigned int *pDec)
{
    int Nout;
    if (nSrcLen <= 0)
        return 0;
    if (sResample.nChans == 1)
        Nout=ResampleLinear_Mono((HWORD *)pSrc, (HWORD *)pDec, nSrcLen );
    //else if (sResample.nChans == 2)
    //  Nout=ResampleLinear(pSrc, pDec, nSrcLen );
    else
        return 0;
    return Nout;
}

