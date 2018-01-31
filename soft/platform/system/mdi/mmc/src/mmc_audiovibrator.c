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



#ifdef AUD_3_IN_1_SPK

#include "cs_types.h"
#include "mcip_debug.h"

//  1~128
// 128:0db
// 64:-6db
// 32:-12db
// 16:-18db
// 8:-24db
// 4:-30db
// 2:-36db
// 1:-42db

#define VIBRATORGAIN  16


PROTECTED CONST INT16 g_MmcAudioVibrator8k[] =
{
#include "175hz_8000Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator11p025k[] =
{
#include "175hz_11025Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator12k[] =
{
#include "175hz_12000Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator16k[] =
{
#include "175hz_16000Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator22p05k[] =
{
#include "175hz_22050Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator24k[] =
{
#include "175hz_24000Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator32k[] =
{
#include "175hz_32000Hz_mono.tab"
};
PROTECTED CONST INT16 g_MmcAudioVibrator44p1k[] =
{
#include "175hz_44100Hz_mono.tab"
};

PROTECTED CONST INT16 g_MmcAudioVibrator48k[] =
{
#include "175hz_48000Hz_mono.tab"
};


static INT32 g_VibratorSourcePointer=0;
static INT32 g_VibratorLastSampleRate=0;
static INT32 g_VibratorLastEnbleFlag=FALSE;


VOID mmc_Vibrator(INT16 *inputbuf,INT32 length,INT16 *outputbuf ,INT16 EnbleFlag,INT32 samplerate,INT32 ch)
{
    CONST INT16 *VibratorSource;
    INT32 VibratorSourceLength;
    INT32 i;
    INT16 VibratorLastValue;

    MCI_TRACE (TSTDOUT,0, "[MCI_VIBRATOR]sample:%d,length:%d,ch:%d,EnbleFlag:%d",samplerate,length,ch,EnbleFlag);

    switch(samplerate)
    {

        case 48000:

            VibratorSource=g_MmcAudioVibrator48k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator48k)/2;

            break;

        case 44100:
            VibratorSource=g_MmcAudioVibrator44p1k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator44p1k)/2;
            break;

        case 32000:
            VibratorSource=g_MmcAudioVibrator32k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator32k)/2;

            break;

        case 24000:
            VibratorSource=g_MmcAudioVibrator24k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator24k)/2;

            break;

        case 22050:
            VibratorSource=g_MmcAudioVibrator22p05k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator22p05k)/2;

            break;

        case 16000:
            VibratorSource=g_MmcAudioVibrator16k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator16k)/2;

            break;

        case 12000:
            VibratorSource=g_MmcAudioVibrator12k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator12k)/2;

            break;

        case 11025:
            VibratorSource=g_MmcAudioVibrator11p025k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator11p025k)/2;

            break;

        case 8000:
            VibratorSource=g_MmcAudioVibrator8k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator8k)/2;

            break;

        default:
            VibratorSource=g_MmcAudioVibrator48k;
            VibratorSourceLength=sizeof(g_MmcAudioVibrator48k)/2;

            EnbleFlag=FALSE;

            break;
    }


//          MCI_TRACE (TSTDOUT,0, "[MCI_VIBRATOR]VibratorSourceLength:%d",VibratorSourceLength);
    //     MCI_TRACE (TSTDOUT,0, "[MCI_VIBRATOR]g_VibratorSourcePointer:%d",g_VibratorSourcePointer);



    if(g_VibratorLastSampleRate!=samplerate)
    {
        g_VibratorSourcePointer=0;
    }

//  MCI_TRACE (TSTDOUT,0, "g_VibratorLastEnbleFlag:%d,EnbleFlag:%d",g_VibratorLastEnbleFlag,EnbleFlag);


    if(g_VibratorLastEnbleFlag==FALSE&&EnbleFlag==TRUE)
    {
        g_VibratorSourcePointer=0;

        for(i=0; i<length; i=i+ch)
        {
            outputbuf[i]=((inputbuf[i]*(INT32)(128-VIBRATORGAIN))>>7)
                         +((VibratorSource[g_VibratorSourcePointer++]*(INT32)VIBRATORGAIN)>>7);


            if(ch==2)
            {
                outputbuf[i+1]=outputbuf[i];
            }


            if(g_VibratorSourcePointer>=VibratorSourceLength)
                g_VibratorSourcePointer=0;
        }


    }
    else if(g_VibratorLastEnbleFlag==FALSE&&EnbleFlag==FALSE)
    {

        for(i=0; i<length; i=i+ch)
        {
            outputbuf[i]=(inputbuf[i]*(INT32)(128-VIBRATORGAIN))>>7;

            if(ch==2)
            {
                outputbuf[i+1]=outputbuf[i];
            }

        }


        g_VibratorLastSampleRate=0;
        g_VibratorLastEnbleFlag=EnbleFlag;
        g_VibratorSourcePointer=0;
        return;
    }
    else if(g_VibratorLastEnbleFlag==TRUE&&EnbleFlag==FALSE)
    {

        for(i=0; i<length; i=i+ch)
        {
            VibratorLastValue=VibratorSource[g_VibratorSourcePointer];

            outputbuf[i]=((inputbuf[i]*(INT32)(128-VIBRATORGAIN))>>7)
                         +((VibratorSource[g_VibratorSourcePointer++]*(INT32)VIBRATORGAIN)>>7);


            if(ch==2)
            {
                outputbuf[i+1]=outputbuf[i];
            }


            if(g_VibratorSourcePointer>=VibratorSourceLength)
                g_VibratorSourcePointer=0;

            if((VibratorLastValue&0x8000)!=(VibratorSource[g_VibratorSourcePointer]&0x8000))
            {
                MCI_TRACE (TSTDOUT,0, "break,last:%d,end:%d",VibratorLastValue,VibratorSource[g_VibratorSourcePointer]);

                g_VibratorSourcePointer=0;
                i=i+ch;

                break;
            }
        }


        for(; i<length; i=i+ch)
        {
            outputbuf[i]=(inputbuf[i]*(INT32)(128-VIBRATORGAIN))>>7;

            if(ch==2)
            {
                outputbuf[i+1]=outputbuf[i];
            }


        }


    }
    else
    {
        for(i=0; i<length; i=i+ch)
        {
            outputbuf[i]=((inputbuf[i]*(INT32)(128-VIBRATORGAIN))>>7)
                         +((VibratorSource[g_VibratorSourcePointer++]*(INT32)VIBRATORGAIN)>>7);


            if(ch==2)
            {
                outputbuf[i+1]=outputbuf[i];
            }


            if(g_VibratorSourcePointer>=VibratorSourceLength)
            {

                g_VibratorSourcePointer=0;
            }
        }
    }

    g_VibratorLastSampleRate=samplerate;
    g_VibratorLastEnbleFlag=EnbleFlag;

    return;
}



#endif




