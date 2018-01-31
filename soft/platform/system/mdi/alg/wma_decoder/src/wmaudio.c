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


#include "map_addr.h"
#include "wmaudio.h"
//#include "macros.h"
#include "wmaudio_parse.h"
#include "hal_voc.h"
//extern unsigned short WMA_Decoder[684];


int32  LOG2_VOC(uint32 i)
{
    // returns n where n = log2(2^n) = log2(2^(n+1)-1)
    int32 iLog2 = 0;
    while ((i >> iLog2) > 1)
        iLog2++;

    return iLog2;
}







int32 msaudioGetSamplePerFrame (int32   cSamplePerSec,
                                uint32   dwBitPerSec,
                                int32   cNumChannels,
                                int32   iVersion)
{

    int32 cSamplePerFrame;
    uint32 dwBytesPerFrame;

    if (dwBitPerSec == 0 || iVersion > 2)
        return 0;
    if (cSamplePerSec <= 8000)
        cSamplePerFrame = 512;
    else if (cSamplePerSec <= 11025)
        cSamplePerFrame = 512;
    else if (cSamplePerSec <= 16000)
    {
        cSamplePerFrame = 512;
    }
    else if (cSamplePerSec <= 22050)
    {
        cSamplePerFrame = 1024;
    }
    else if (cSamplePerSec <= 32000)
    {
        if(iVersion == 1)
            cSamplePerFrame = 1024;
        else
            cSamplePerFrame = 2048;
    }
    else if (cSamplePerSec <= 44100)
        cSamplePerFrame = 2048;
    else if (cSamplePerSec <= 48000)
        cSamplePerFrame = 2048;
    else
        return 0;
    dwBytesPerFrame = (uint32) (((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
    if ( dwBytesPerFrame==0 && (cSamplePerFrame*dwBitPerSec) == 0 )
    {


        dwBitPerSec = cSamplePerSec;
        dwBytesPerFrame = (uint32) (((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
    }
    if (dwBytesPerFrame <= 1)
    {
        while (dwBytesPerFrame == 0)
        {
            cSamplePerFrame *= 2;
            dwBytesPerFrame = (uint32) (((cSamplePerFrame*dwBitPerSec + cSamplePerSec/2)/cSamplePerSec + 7)/8);
        }
    }
    return cSamplePerFrame;
}


const uint16 g_rgiBarkFreq[26] =
{
    100,    200,    300,    400,    510,
    630,    770,    920,   1080,   1270,
    1480,   1720,   2000,   2320,   2700,
    3150,   3700,   4400,   5300,   6400,
    7700,   9500,  12000,  15500,  24500
};

WMARESULT WMADecoderInit (

    int32 iVersionNumber,
    int32 cSubband,
    int32 cSamplePerSec,
    uint16 cChannel,
    int32 cBytePerSec,
    int32 cbPacketLength,
    uint16 iEncodeOpt,
    uint16 iPlayerOpt)

{

    uint16 iNonSupportedPlayerOpt;

    static const int8 fOKOptions[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0 };
    short m_fAllowSuperFrame,m_fAllowSubFrame,m_iMaxSubFrameDiv,m_iEntropyMode,m_cPossibleWinSize;
//    float m_fltBitsPerSample,m_fltWeightedBitsPerSample;
    int32 m_intBitsPerSample,m_intWeightedBitsPerSample;

    if(iVersionNumber == 1)
        return WMA_E_NOTSUPPORTED;
    if ( cChannel>2 )
        return WMA_E_NOTSUPPORTED;

    cSubband = msaudioGetSamplePerFrame (cSamplePerSec, cBytePerSec * 8, cChannel, iVersionNumber);

    *(short *)hal_VocGetPointer(GLOBAL_m_iVersion_ADDR)= (unsigned short)iVersionNumber;
    *(short *)hal_VocGetPointer(GLOBAL_m_iWeightingMode_ADDR)= (iEncodeOpt & 0x0001) ? BARK_MODE : LPC_MODE;
    *(short *)hal_VocGetPointer(GLOBAL_m_fV5Lpc_ADDR)= (iEncodeOpt & 0x0020);
    *(short *)hal_VocGetPointer(GLOBAL_m_fAllowSuperFrame_ADDR)=  m_fAllowSuperFrame = !!(iEncodeOpt & 0x0002);
    *(short *)hal_VocGetPointer(GLOBAL_m_fAllowSubFrame_ADDR)= m_fAllowSubFrame =!!(iEncodeOpt & 0x0002) && !!(iEncodeOpt & 0x0004);
    if (m_fAllowSubFrame)
    {
        m_iMaxSubFrameDiv = ((iEncodeOpt & 0x0018) >>
                             3);
        if (cBytePerSec / cChannel >= 4000)
            m_iMaxSubFrameDiv = (8 << m_iMaxSubFrameDiv);
        else
            m_iMaxSubFrameDiv = (2 << m_iMaxSubFrameDiv);
    }
    else
        m_iMaxSubFrameDiv = 1;
    if (m_iMaxSubFrameDiv > cSubband / (128))
        m_iMaxSubFrameDiv = cSubband / (128);
    *(short *)hal_VocGetPointer(GLOBAL_m_iMaxSubFrameDiv_ADDR)= m_iMaxSubFrameDiv;

    iNonSupportedPlayerOpt = ~0;
    if ( (iNonSupportedPlayerOpt & iPlayerOpt) || !fOKOptions[iPlayerOpt&0xF] )
        return WMA_E_NOTSUPPORTED;

    if ( (iPlayerOpt&(0x0002|0x0008))==(0x0002|0x0008) )
    {

        iPlayerOpt &= ~(0x0002|0x0008);
    }
    *(int *)hal_VocGetPointer(GLOBAL_m_iSamplingRate_ADDR)= cSamplePerSec;
    *(short *)hal_VocGetPointer(GLOBAL_m_cChannel_ADDR)= cChannel;
    *(short *)hal_VocGetPointer(GLOBAL_m_cSubband_ADDR)= (unsigned short)cSubband;



//    m_fltWeightedBitsPerSample = m_fltBitsPerSample  = (Float)(cBytePerSec*8.0f/(cSamplePerSec*cChannel));
    m_intWeightedBitsPerSample = m_intBitsPerSample  = cBytePerSec*8000/(cSamplePerSec*cChannel);
    if (cChannel > 1)
//        m_fltWeightedBitsPerSample *= (Float) 1.6;
        m_intWeightedBitsPerSample =  m_intWeightedBitsPerSample*16/10;


    *(short *)hal_VocGetPointer(GLOBAL_m_cFrameSample_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_cSubFrameSample_ADDR)=
            *(short *)hal_VocGetPointer(GLOBAL_m_iCoefRecurQ4_ADDR)= (unsigned short)(2 * cSubband);

    *(short *)hal_VocGetPointer(GLOBAL_m_cFrameSampleHalf_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_cSubFrameSampleHalf_ADDR)=
            *(short *)hal_VocGetPointer(GLOBAL_m_iCoefRecurQ2_ADDR)=
                *(short *)hal_VocGetPointer(GLOBAL_m_iCoefRecurQ3_ADDR)= (unsigned short)cSubband;

    *(short *)hal_VocGetPointer(GLOBAL_m_cSubFrameSampleQuad_ADDR)= (unsigned short)(unsigned short)(cSubband / 2);

//    pau->m_iCoefRecurQ1 = 0;
    *(short *)hal_VocGetPointer(GLOBAL_m_cMinSubFrameSampleHalf_ADDR)= cSubband / m_iMaxSubFrameDiv;
    *(short *)hal_VocGetPointer(GLOBAL_m_cPossibleWinSize_ADDR)= m_cPossibleWinSize= LOG2_VOC((uint32)m_iMaxSubFrameDiv) + 1;
    *(short *)hal_VocGetPointer(GLOBAL_m_cBitsSubbandMax_ADDR)= (unsigned short)LOG2_VOC((uint32)cSubband);


    *(short *)hal_VocGetPointer(GLOBAL_m_cLowCutOffLong_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_cLowCutOff_ADDR)=  0;

    *(short *)hal_VocGetPointer(GLOBAL_m_cHighCutOffLong_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_cHighCutOff_ADDR)= cSubband - 9 * cSubband / 100;

    m_iEntropyMode = 3;
//    if (m_fltWeightedBitsPerSample < 0.72f) {
    if (m_intWeightedBitsPerSample < 720)
    {
        if (cSamplePerSec >= 32000)
            m_iEntropyMode = 1;
    }
//    else if (m_fltWeightedBitsPerSample < 1.16f) {
    else if (m_intWeightedBitsPerSample < 1160)
    {
        if (cSamplePerSec >= 32000)
        {
            m_iEntropyMode = 2;
        }
    }
    *(short *)hal_VocGetPointer(GLOBAL_m_iEntropyMode_ADDR)=  m_iEntropyMode;

    //prvAllocate
    {
        {
            int32 i, iWin;
//          Float fltSamplingPeriod;
            int32 cFrameSample;
            boolean bCombined;
            short * m_rgcValidBarkBand;
            short * piBarkIndex;

//          fltSamplingPeriod = 1.0F / cSamplePerSec;
            m_rgcValidBarkBand = (short *)hal_VocGetPointer(GLOBAL_m_rgcValidBarkBand_ADDR); //where the data is put on  ram_x or the data arryay?
            piBarkIndex = (short *)hal_VocGetPointer(GLOBAL_m_rgiBarkIndexOrig_ADDR);
            {
                for (iWin = 0; iWin < m_cPossibleWinSize; iWin++)
                {
                    piBarkIndex  [0] = 0;
                    cFrameSample = 2 * cSubband / (1 << iWin);
                    bCombined = 0;
                    if (cSamplePerSec >= 44100)
                    {
                        if(cFrameSample == 1024)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand[iWin] = 17;
                            piBarkIndex[1]  = 5;    piBarkIndex[2]  = 12;
                            piBarkIndex[3]  = 18;   piBarkIndex[4]  = 25;
                            piBarkIndex[5]  = 34;   piBarkIndex[6]  = 46;
                            piBarkIndex[7]  = 54;   piBarkIndex[8]  = 63;
                            piBarkIndex[9]  = 86;   piBarkIndex[10] = 102;
                            piBarkIndex[11] = 123;  piBarkIndex[12] = 149;
                            piBarkIndex[13] = 179;  piBarkIndex[14] = 221;
                            piBarkIndex[15] = 279;  piBarkIndex[16] = 360;
                            piBarkIndex[17] = 512;
                        }
                        else if(cFrameSample == 512)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand[iWin] = 15;
                            piBarkIndex[1]  = 5;    piBarkIndex[2]  = 11;
                            piBarkIndex[3]  = 17;   piBarkIndex[4]  = 23;
                            piBarkIndex[5]  = 31;   piBarkIndex[6]  = 37;
                            piBarkIndex[7]  = 43;   piBarkIndex[8]  = 51;
                            piBarkIndex[9]  = 62;   piBarkIndex[10] = 74;
                            piBarkIndex[11] = 89;   piBarkIndex[12] = 110;
                            piBarkIndex[13] = 139;  piBarkIndex[14] = 180;
                            piBarkIndex[15] = 256;
                        }
                        else if(cFrameSample == 256)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand [iWin] = 12;
                            piBarkIndex[1]  = 4;   piBarkIndex[2]  = 9;
                            piBarkIndex[3]  = 12;  piBarkIndex[4]  = 16;
                            piBarkIndex[5]  = 21;  piBarkIndex[6]  = 26;
                            piBarkIndex[7]  = 37;  piBarkIndex[8]  = 45;
                            piBarkIndex[9]  = 55;  piBarkIndex[10] = 70;
                            piBarkIndex[11] = 90;  piBarkIndex[12] = 128;
                        }
                    }
                    else if(cSamplePerSec >= 32000)
                    {
                        if(cFrameSample == 1024)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand[iWin] = 16;
                            piBarkIndex[1]  = 6;   piBarkIndex[2]  = 13;
                            piBarkIndex[3]  = 20;  piBarkIndex[4]  = 29;
                            piBarkIndex[5]  = 41;  piBarkIndex[6]  = 55;
                            piBarkIndex[7]  = 74;  piBarkIndex[8]  = 101;
                            piBarkIndex[9]  = 141; piBarkIndex[10] = 170;
                            piBarkIndex[11] = 205; piBarkIndex[12] = 246;
                            piBarkIndex[13] = 304; piBarkIndex[14] = 384;
                            piBarkIndex[15] = 496; piBarkIndex[16] = 512;
                        }
                        else if(cFrameSample == 512)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand[iWin] = 15;
                            piBarkIndex[1]  = 5;   piBarkIndex[2]  = 10;
                            piBarkIndex[3]  = 15;  piBarkIndex[4]  = 20;
                            piBarkIndex[5]  = 28;  piBarkIndex[6]  = 37;
                            piBarkIndex[7]  = 50;  piBarkIndex[8]  = 70;
                            piBarkIndex[9]  = 85;  piBarkIndex[10] = 102;
                            piBarkIndex[11] = 123; piBarkIndex[12] = 152;
                            piBarkIndex[13] = 192; piBarkIndex[14] = 248;
                            piBarkIndex[15] = 256;

                        }
                        else if(cFrameSample == 256)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand [iWin] = 11;
                            piBarkIndex[1]  = 4;   piBarkIndex[2]  = 9;
                            piBarkIndex[3]  = 14;  piBarkIndex[4]  = 19;
                            piBarkIndex[5]  = 25;  piBarkIndex[6]  = 35;
                            piBarkIndex[7]  = 51;  piBarkIndex[8]  = 76;
                            piBarkIndex[9]  = 96;  piBarkIndex[10] = 124;
                            piBarkIndex[11] = 128;

                        }
                    }
                    else if(cSamplePerSec >= 22050)
                    {
                        if(cFrameSample == 512)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand [iWin] = 14;
                            piBarkIndex[1]  = 5;   piBarkIndex[2]  = 12;
                            piBarkIndex[3]  = 18;  piBarkIndex[4]  = 25;
                            piBarkIndex[5]  = 34;  piBarkIndex[6]  = 46;
                            piBarkIndex[7]  = 63;  piBarkIndex[8]  = 86;
                            piBarkIndex[9]  = 102; piBarkIndex[10] = 123;
                            piBarkIndex[11] = 149; piBarkIndex[12] = 179;
                            piBarkIndex[13] = 221; piBarkIndex[14] = 256;

                        }
                        else if(cFrameSample == 256)
                        {
                            bCombined = 1;
                            m_rgcValidBarkBand [iWin] = 10;
                            piBarkIndex[1]  = 5;   piBarkIndex[2]  = 11;
                            piBarkIndex[3]  = 17;  piBarkIndex[4]  = 23;
                            piBarkIndex[5]  = 31;  piBarkIndex[6]  = 43;
                            piBarkIndex[7]  = 62;  piBarkIndex[8]  = 89;
                            piBarkIndex[9]  = 110; piBarkIndex[10] = 128;

                        }
                    }
                    if(!bCombined)
                    {
//                      Float fltTemp = cFrameSample*fltSamplingPeriod;


                        int32 iIndex;
                        int32 iFreqCurr = 0;
                        int32 iCurr = 1;
                        while(1)
                        {
//                          iIndex = ((int32) ((g_rgiBarkFreq [iFreqCurr++]*fltTemp + 2.0f)/4.0f))*4;
                            iIndex =  (((g_rgiBarkFreq [iFreqCurr++]*cFrameSample/cSamplePerSec) + 2)>>2)<<2;
                            if(iIndex > piBarkIndex[iCurr - 1])
                                piBarkIndex[iCurr++] = (unsigned short)iIndex;
                            if(iFreqCurr >= 25 || piBarkIndex[iCurr - 1] > cFrameSample/2)
                            {
                                piBarkIndex[iCurr - 1] = cFrameSample/2;
                                m_rgcValidBarkBand[iWin] = iCurr - 1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (i = 0; i < m_rgcValidBarkBand [iWin]; i++)
                        {
                            piBarkIndex [i + 1] = ((piBarkIndex [i + 1] + 2) / 4) * 4;

                        }
                    }
                    piBarkIndex +=  25 + 1;
                }
            }

            *(short *)hal_VocGetPointer(GLOBAL_m_rgiBarkIndex_ADDR)=  GLOBAL_m_rgiBarkIndexOrig_ADDR;
            *(short *)hal_VocGetPointer(GLOBAL_m_cValidBarkBand_ADDR)=  m_rgcValidBarkBand[0];
        }
        {
            int32 iWin, iBand;

            int32 cFrameSample;
            int32 iNoiseIndex;

//          float fltFirstNoiseFreq;
            int32 intFirstNoiseFreq;

            short    m_fNoiseSub;
            short * m_rgcValidBarkBand;
            short * m_rgiFirstNoiseBand;
            short * piBarkIndex;

            m_rgcValidBarkBand = (short *)hal_VocGetPointer(GLOBAL_m_rgcValidBarkBand_ADDR);
            m_rgiFirstNoiseBand = (short *)hal_VocGetPointer(GLOBAL_m_rgiFirstNoiseBand_ADDR);
            piBarkIndex = (short *)hal_VocGetPointer(GLOBAL_m_rgiBarkIndexOrig_ADDR);

            m_fNoiseSub = WMAB_TRUE;
//          fltFirstNoiseFreq = (Float)(0.5f);
            intFirstNoiseFreq = 1;

            // HongCho: This is related to Bark bands (re-adjust when Bark bands change)
            // for newer versions...  more correct using inequality...

            if (cSamplePerSec >= 44100)
            {
//              if (m_fltWeightedBitsPerSample >= 0.61f)
                if (m_intWeightedBitsPerSample >= 610)
                    m_fNoiseSub = WMAB_FALSE;
                else
//                  fltFirstNoiseFreq *= (Float) 0.4;
                    intFirstNoiseFreq *= 8;
            }
            else if(cSamplePerSec >= 22050)
            {
                // somewhat different parameters...
//              if (m_fltWeightedBitsPerSample >= 1.16f)
                if (m_intWeightedBitsPerSample >= 1160)
                    m_fNoiseSub = WMAB_FALSE;
//              else if(m_fltWeightedBitsPerSample >= 0.72f)
                else if(m_intWeightedBitsPerSample >= 720)
//                  fltFirstNoiseFreq *= (Float)0.70;
                    intFirstNoiseFreq *= 14;
                else
//                  fltFirstNoiseFreq *= (Float)0.60;
                    intFirstNoiseFreq *= 12;
            }
            else if (cSamplePerSec >= 16000)
            {
//              if (m_fltBitsPerSample <= 0.5f)
                if (m_intBitsPerSample <= 500)
//                  fltFirstNoiseFreq *= (Float) 0.30;
                    intFirstNoiseFreq *= 6;
                else
//                  fltFirstNoiseFreq *= (Float) 0.50;
                    intFirstNoiseFreq *= 10;
            }
            else if (cSamplePerSec >= 11025)
            {
//              fltFirstNoiseFreq *= (Float) 0.70;
                intFirstNoiseFreq *= 14;
            }
            else if (cSamplePerSec >= 8000)
            {
//              if (m_fltBitsPerSample <=0.625f)
                if (m_intBitsPerSample <=625)
//                  fltFirstNoiseFreq *= (Float) 0.50;
                    intFirstNoiseFreq *= 10;
                else if (m_intBitsPerSample <= 750)
//                  fltFirstNoiseFreq *= (Float) 0.65;
                    intFirstNoiseFreq *= 13;
                else
                    m_fNoiseSub = WMAB_FALSE;
            }
            else
            {
//              if(m_fltBitsPerSample >= 0.8f)
                if(m_intBitsPerSample >= 800)
//                  fltFirstNoiseFreq *= (Float)0.75;
                    intFirstNoiseFreq *= 15;
//              else if(m_fltBitsPerSample >= 0.6f)
                else if(m_intBitsPerSample >= 600)
//                  fltFirstNoiseFreq *= (Float)0.60;
                    intFirstNoiseFreq *= 12;
                else
//                  fltFirstNoiseFreq *= (Float)0.5;
                    intFirstNoiseFreq *= 10;
            }

//          WMA_Decoder[GLOBAL_m_fNoiseSub_ADDR - (GLOBAL_tWMAFileStateInternal_ADDR)]= m_fNoiseSub;
            *(short *)hal_VocGetPointer(GLOBAL_m_fNoiseSub_ADDR)=   m_fNoiseSub;
            if (!m_fNoiseSub)
                goto InitNoiseSub_Over;


            //calculate index of each bark freq
            // wchen: we need to think what to do with the cut off frequencies: not include at all or include zeros.
            //for long window

            for (iWin = 0; iWin < m_cPossibleWinSize; iWin++)
            {
                // precalculate the first noise bands
                m_rgiFirstNoiseBand[iWin] = m_rgcValidBarkBand[iWin] - 1;// init to max...
                cFrameSample = 2 * cSubband / (1 << iWin);
//              iNoiseIndex = (int32)(fltFirstNoiseFreq*cFrameSample + 0.5f);
                iNoiseIndex = (intFirstNoiseFreq*cFrameSample + 20)/40;

                for(iBand = 1; iBand < m_rgcValidBarkBand[iWin]; iBand++)
                {
                    if(piBarkIndex[iBand] > iNoiseIndex)
                    {
                        m_rgiFirstNoiseBand[iWin] = iBand - 1;
                        break;
                    }
                }
                piBarkIndex +=  NUM_BARK_BAND + 1;
            }

            *(short *)hal_VocGetPointer(GLOBAL_m_iFirstNoiseBand_ADDR)=   m_rgiFirstNoiseBand[0];
            *(short *)hal_VocGetPointer(GLOBAL_m_fltFirstNoiseFreq_ADDR)=  (short)(intFirstNoiseFreq*0x7fff/40);
        }
InitNoiseSub_Over:
        ;
    }

    *(short *)hal_VocGetPointer(GLOBAL_m_cBitPackedFrameSize_ADDR)= (short)(LOG2_VOC((uint32 ) (m_intBitsPerSample * cSubband / 8 + 500)/1000) + 2);
    *(short *)hal_VocGetPointer(GLOBAL_m_iFrameNumber_ADDR)= 0;


    *(short *)hal_VocGetPointer(GLOBAL_m_iMaxEscSize_ADDR)= 9;
    *(short *)hal_VocGetPointer(GLOBAL_m_fHeaderReset_ADDR)=  1;
    *(short *)hal_VocGetPointer(GLOBAL_m_iQuantStepSize_ADDR)=   (1 + 129 - 1) / 2;
    *(short *)hal_VocGetPointer(GLOBAL_m_tRandState_ADDR)=   0;
    *(short *)hal_VocGetPointer(GLOBAL_m_tRandState_ADDR+1)=   0;
    *(short *)hal_VocGetPointer(GLOBAL_m_tRandState_ADDR+2)=   0;
    *(short *)hal_VocGetPointer(GLOBAL_m_tRandState_ADDR+3)=   1;
    *(short *)hal_VocGetPointer(GLOBAL_m_tRandState_ADDR+4)=   2;
//channel info
    // can be del after replace lw_d addr by lwi buffer
    *(short *)hal_VocGetPointer(GLOBAL_Lm_rgiMaskQ_ADDR)= GLOBAL_Lm_rgiMaskQ_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rgiMaskQ_ADDR)= GLOBAL_Rm_rgiMaskQ_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Lm_rgffltSqrtBWRatio_ADDR)= GLOBAL_Lm_rgffltSqrtBWRatio_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rgffltSqrtBWRatio_ADDR)= GLOBAL_Rm_rgffltSqrtBWRatio_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Lm_rgbBandNotCoded_ADDR) =  GLOBAL_Lm_rgbBandNotCoded_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rgbBandNotCoded_ADDR)= GLOBAL_Rm_rgbBandNotCoded_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Lm_rgiNoisePower_ADDR)= GLOBAL_Lm_rgiNoisePower_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rgiNoisePower_ADDR)= GLOBAL_Rm_rgiNoisePower_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Lm_rguiWeightFactor_ADDR)= GLOBAL_Lm_rguiWeightFactor_BUFFER;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rguiWeightFactor_ADDR)= GLOBAL_Rm_rguiWeightFactor_BUFFER;

//        ppcinfo->m_cSubbandActual = pau->m_cHighCutOff - pau->m_cLowCutOff;
    *(short *)hal_VocGetPointer(GLOBAL_Lm_cSubbandActual_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_Rm_cSubbandActual_ADDR)=
            *(short *)hal_VocGetPointer(GLOBAL_m_cHighCutOff_ADDR)-(*(short *)hal_VocGetPointer(GLOBAL_m_cLowCutOff_ADDR));

    *(short *)hal_VocGetPointer(GLOBAL_Lm_rgiCoefRecon_ADDR)=  GLOBAL_m_rgiCoefReconOrig_ADDR;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rgiCoefRecon_ADDR)= GLOBAL_m_rgiCoefReconOrig_ADDR + cSubband*2;

    *(short *)hal_VocGetPointer(GLOBAL_Lm_rgiCoefReconCurr_ADDR)= GLOBAL_m_rgiCoefReconOrig_ADDR + (unsigned short)cSubband;
    *(short *)hal_VocGetPointer(GLOBAL_Rm_rgiCoefReconCurr_ADDR)= GLOBAL_m_rgiCoefReconOrig_ADDR + (unsigned short)cSubband*3;

    *(short *)hal_VocGetPointer(GLOBAL_Lm_iCurrGetPCM_SubFrame_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_Rm_iCurrGetPCM_SubFrame_ADDR)= (unsigned short)cSubband;

//strm
    *(short *)hal_VocGetPointer(GLOBAL_m_pBufferBegin_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_pBuffer_ADDR)=  GLOBAL_pCallBackBuffer_ADDR;

    *(short *)hal_VocGetPointer(GLOBAL_m_iPrevPacketNum_ADDR)=  (1 << NBITS_PACKET_CNT) - 1;
    *(short *)hal_VocGetPointer(GLOBAL_m_fAllowPackets_ADDR)= m_fAllowSuperFrame;

    *(short *)hal_VocGetPointer(GLOBAL_m_subfrmconfigPrev_cSubFrame_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_subfrmconfigCurr_cSubFrame_ADDR)=
            *(short *)hal_VocGetPointer(GLOBAL_m_subfrmconfigNext_cSubFrame_ADDR)=  1;

    *(short *)hal_VocGetPointer(GLOBAL_m_subfrmconfigPrev_rgiSubFrameSize_ADDR)=
        *(short *)hal_VocGetPointer(GLOBAL_m_subfrmconfigCurr_rgiSubFrameSize_ADDR)=
            *(short *)hal_VocGetPointer(GLOBAL_m_subfrmconfigNext_rgiSubFrameSize_ADDR)= (unsigned short)cSubband;

    *(short *)hal_VocGetPointer(GLOBAL_m_codecStatus_ADDR)= CODEC_BEGIN;
    *(int *)hal_VocGetPointer(GLOBAL_m_iPacketCurr_ADDR)= -2;


    return WMA_OK;
}




tWMAFileStatus WMAFileDecodeInit (tHWMAFileState hstate)
{
    tWMAFileStateInternal *pInt;
    WMAERR wmarc;
    WMARESULT   wmaResult;

//    extern unsigned int header_test[10];
    int32 frame_start;

    pInt = (tWMAFileStateInternal*) hstate;

    wmarc = WMA_ParseAsfHeader(&pInt->hdr_parse, 0);
    if(wmarc != WMAERR_OK)
        return cWMA_BadAsfHeader;

    wmaResult = WMADecoderInit (
                    (uint16) pInt->hdr_parse.nVersion,
                    (uint16) pInt->hdr_parse.nSamplesPerBlock,
                    (uint16) pInt->hdr_parse.nSamplesPerSec,
                    (uint16) pInt->hdr_parse.nChannels,
                    (uint16) pInt->hdr_parse.nAvgBytesPerSec,
                    (uint16) pInt->hdr_parse.nBlockAlign,
                    (uint16) pInt->hdr_parse.nEncodeOpt,
                    0
                );



    if (wmaResult == WMA_E_NOTSUPPORTED)
        return cWMA_Failed;

    if(pInt->hdr_parse.cbSecretData > 0)// not supported for DRM
        return cWMA_DRMUnsupported;

    pInt->hdr_parse.cbLastPacketOffset = pInt->hdr_parse.cbFirstPacketOffset;
    if (pInt->hdr_parse.cPackets > 0)
        pInt->hdr_parse.cbLastPacketOffset += (pInt->hdr_parse.cPackets - 1)*pInt->hdr_parse.cbPacketSize;
    //for ram update

//  *(int *)hal_VocGetPointer(GLOBAL_currPacketOffset_ADDR)=
//  *(int *)hal_VocGetPointer(GLOBAL_nextPacketOffset_ADDR)= pInt->hdr_parse.cbHeader;

    frame_start = (INT32)(((INT64)pInt->playprogress *(INT64)pInt->hdr_parse.cPackets)/10000);
    hal_HstSendEvent(0x77777001);
    hal_HstSendEvent(frame_start);
    pInt->hdr_parse.currPacketOffset  =pInt->hdr_parse.cbHeader+pInt->hdr_parse.cbPacketSize *frame_start;
    hal_HstSendEvent(pInt->hdr_parse.cbPacketSize);
    hal_HstSendEvent(pInt->hdr_parse.cbHeader);
    *(int *)hal_VocGetPointer(GLOBAL_currPacketOffset_ADDR)=
        *(int *)hal_VocGetPointer(GLOBAL_nextPacketOffset_ADDR)= pInt->hdr_parse.currPacketOffset;//pInt->hdr_parse.cbHeader+pInt->hdr_parse.cbPacketSize *frame_start;

    hal_HstSendEvent(*(int *)hal_VocGetPointer(GLOBAL_currPacketOffset_ADDR));

    *(int *)hal_VocGetPointer(GLOBAL_cbPacketSize_ADDR)= pInt->hdr_parse.cbPacketSize;
    *(int *)hal_VocGetPointer(GLOBAL_cbLastPacketOffset_ADDR)= pInt->hdr_parse.cbLastPacketOffset;
    *(short *)hal_VocGetPointer(GLOBAL_nBlockAlign_ADDR)=  pInt->hdr_parse.nBlockAlign ;
    *(int *)hal_VocGetPointer(GLOBAL_wAudioStreamId_ADDR)= pInt->hdr_parse.wAudioStreamId;

    *(short *)hal_VocGetPointer(GLOBAL_parse_state_ADDR)= csWMA_NewAsfPacket ;
    *(short *)hal_VocGetPointer(GLOBAL_m_fLastSubFrame_ADDR)= WMAB_TRUE ;

    *(short *)hal_VocGetPointer(GLOBAL_bHasDRM_ADDR)= 0;
    *(short *)hal_VocGetPointer(GLOBAL_bFirst_ADDR)= 0;
    *(int *)hal_VocGetPointer(GLOBAL_m_dwHeaderBuf_ADDR)= 0;

    hal_HstSendEvent(*(int *)hal_VocGetPointer(GLOBAL_cbPacketSize_ADDR));

    hal_HstSendEvent(*(int *)hal_VocGetPointer(GLOBAL_m_dwHeaderBuf_ADDR));

    return cWMA_NoErr;
}


tWMAFileStatus WMAFileGetInfo (tHWMAFileState hstate)
{
    tWMAFileStateInternal *pInt;
    WMAERR wmarc;
    pInt = (tWMAFileStateInternal*) hstate;


    wmarc = WMA_ParseAsfHeader(&pInt->hdr_parse, 0);
    if(wmarc != WMAERR_OK)
        return cWMA_BadAsfHeader;

    return cWMA_NoErr;
}

