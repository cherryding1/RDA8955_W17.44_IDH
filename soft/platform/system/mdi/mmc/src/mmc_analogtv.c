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


#include "cs_types.h"
#include "cos.h"

#include "mcip_debug.h"
#include "vpp_analogtv.h"
#include "mmc.h"
#include "mmc_analogtv.h"

#include "hal_error.h"
#include "hal_timers.h"


#include "lcdd_m.h"

#include "sxr_tls.h"

#include "aud_fm.h"
#include "global.h"

#include "hal_overlay.h"


extern uint32 pcmbuf_overlay[4608];

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  STREAM GLOBAL VARIABLES
// =============================================================================



// =============================================================================
//  CONFIG GLOBAL VARIABLES
// =============================================================================

/// Interface used to play the current stream.
//PRIVATE AUD_ITF_T g_apbsItf= 0;

// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// mmc_TvSetup
// -----------------------------------------------------------------------------
// =============================================================================
UINT32 mmc_AnalogTvAudioSetup(UINT8 volume)
{
    AUD_ERR_T audError = AUD_ERR_NO;
    AUD_ITF_T itf=AUD_ITF_TV;   //xiaoyifeng for atv
    AUD_LEVEL_T cfg;

    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_ATV]mmc_AnalogTvAudioSetup %d",volume);

    if(volume>MAX_VOL_LEVEL)volume=MAX_VOL_LEVEL;

    cfg.spkLevel=volume;
    cfg.micLevel=0;
    cfg.sideLevel=0;
    cfg.toneLevel=0;

    audError=aud_Setup(itf,  & cfg);

    return audError;
}


// =============================================================================
// mmc_TvPlay
// -----------------------------------------------------------------------------
// =============================================================================
UINT32 mmc_AnalogTvAudioOpen(VOID)
{
    AUD_ERR_T audError;
    HAL_AIF_STREAM_T stream;
    AUD_LEVEL_T level;
    AUD_ITF_T itf=AUD_ITF_TV;   //xiaoyifeng for atv

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_AnalogTvAudioOpen!");

#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
    level.spkLevel=AUD_SPK_VOL_12;
#else
    level.spkLevel=AUD_SPK_VOL_4;
#endif
    level.sideLevel=0;
    level.micLevel=0;
    level.toneLevel=0;
    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

    // Set audio interface
    stream.startAddress = (UINT32*)pcmbuf_overlay;
    stream.length = 6400;
    stream.sampleRate = HAL_AIF_FREQ_48000HZ;
    stream.channelNb = HAL_AIF_STEREO;
    stream.voiceQuality = FALSE;
    stream.playSyncWithRecord = FALSE;

    // initialize the interrupt function.
    stream.halfHandler = NULL;
    stream.endHandler = NULL;



    ATVD_ERR_T anlogtvdError = ATVD_ERR_NO;


    ATVD_I2S_CFG_T  anlogtvdI2sCfg= {0};


    switch (stream.sampleRate)
    {
        case HAL_AIF_FREQ_8000HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_8000HZ;
            break;

        case HAL_AIF_FREQ_11025HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_11025HZ;
            break;

        case HAL_AIF_FREQ_12000HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_12000HZ;
            break;

        case HAL_AIF_FREQ_16000HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_16000HZ;
            break;

        case HAL_AIF_FREQ_22050HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_22050HZ;
            break;

        case HAL_AIF_FREQ_24000HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_24000HZ;
            break;

        case HAL_AIF_FREQ_32000HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_32000HZ;
            break;

        case HAL_AIF_FREQ_44100HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_44100HZ;
            break;

        case HAL_AIF_FREQ_48000HZ:
            anlogtvdI2sCfg.freq=ATVD_FREQ_48000HZ;
            break;
        default:
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,     "[MCI]Sample rate erro:%d",stream.sampleRate);
            break;
    }

    anlogtvdI2sCfg.slave=TRUE;

    anlogtvdError= atvd_I2sOpen(anlogtvdI2sCfg);

    if (anlogtvdError != ATVD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,     "[MCI]atvd_I2sOpen FAILED");
        return AUD_ERR_RESOURCE_BUSY;
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,     "[MCI]atvd_I2sOpen SUCCESS");
    }

    audError= aud_StreamStart(itf, &stream, &level);

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStart FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStart SUCCESS");
    }

    return audError;


}

// =============================================================================
// MCI_TvStop
// -----------------------------------------------------------------------------
// =============================================================================
UINT32 mmc_AnalogTvAudioClose(VOID)
{
    AUD_ERR_T audError = AUD_ERR_NO;
    AUD_ITF_T itf=AUD_ITF_TV;

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_AnalogTvAudioClose!");

    atvd_I2sClose();
    audError=aud_StreamStop(itf);

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStop FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStop SUCCESS");
    }
    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    return audError;
}

//test for WT xiaoyifeng
// =============================================================================
// mmc_WalkieTalkieAudioPlay
// -----------------------------------------------------------------------------
// =============================================================================
#if 0
UINT32 mmc_BtWalkieTalkieSendAudioTxEnanble(UINT32 streamStartAddress , UINT16 streamLength , HAL_AIF_HANDLER_T streamXferhalfHandler)
{
    AUD_ERR_T audError;
    HAL_AIF_STREAM_T stream;
    AUD_LEVEL_T level;
    AUD_ITF_T itf=AUD_ITF_TV;   //xiaoyifeng for atv

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_BtWalkieTalkieAudioOpen!");
#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
    level.spkLevel=SND_SPK_VOL_15;
#else
    level.spkLevel=SND_SPK_VOL_7;
#endif
    level.sideLevel=0;
    level.micLevel=AUD_MIC_ENABLE;
    level.toneLevel=0;
    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

    // Set audio interface
    stream.startAddress = streamStartAddress;
    stream.length = streamLength;
    stream.sampleRate = HAL_AIF_FREQ_8000HZ;
    stream.channelNb = HAL_AIF_MONO;
    stream.voiceQuality = TRUE;
    stream.playSyncWithRecord = FALSE;
    // initialize the interrupt function.
    stream.halfHandler = streamXferhalfHandler;
    stream.endHandler = NULL;

    audError= aud_StreamStart(itf, &stream, &level);
    //aud_StreamRecord

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStart FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStart SUCCESS");
    }

    return audError;

}

#else
uint32 pcmbuf_tmp[64];//18*1024 bytes

UINT32 mmc_BtWalkieTalkieAudioOpen(VOID)
{
    AUD_ERR_T audError;
    HAL_AIF_STREAM_T stream;
    AUD_LEVEL_T level;
    AUD_ITF_T itf=AUD_ITF_TV;   //xiaoyifeng for atv

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_BtWalkieTalkieAudioOpen!");

#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
    level.spkLevel=SND_SPK_VOL_15;
#else
    level.spkLevel=SND_SPK_VOL_7;
#endif
    level.sideLevel=0;
    level.micLevel=AUD_MIC_ENABLE;
    level.toneLevel=0;
    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

    // Set audio interface
    stream.startAddress = (UINT32*)pcmbuf_tmp;
    stream.length = sizeof(pcmbuf_tmp);
    stream.sampleRate = HAL_AIF_FREQ_8000HZ;
    stream.channelNb = HAL_AIF_MONO;
    stream.voiceQuality = TRUE;
    stream.playSyncWithRecord = FALSE;
    // initialize the interrupt function.
    stream.halfHandler = NULL;
    stream.endHandler = NULL;

    audError= aud_StreamStart(itf, &stream, &level);
    //aud_StreamRecord

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStart FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStart SUCCESS");
    }

    return audError;


}
#endif
// =============================================================================
// MCI_TvStop
// -----------------------------------------------------------------------------
// =============================================================================
UINT32 mmc_BtWalkieTalkieSendAudioTxDisable(VOID)
{
    AUD_ERR_T audError = AUD_ERR_NO;
    AUD_ITF_T itf=AUD_ITF_TV;

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_BtWalkieTalkieAudioClose!");

    audError=aud_StreamStop(itf);

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStop FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStop SUCCESS");
    }
    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    return audError;
}

#if 1
UINT32 mmc_BtWalkieTalkieSendAudioRxEnanble(UINT32 streamStartAddress , UINT16 streamLength , HAL_AIF_HANDLER_T streamXferhalfHandler)
{
    AUD_ERR_T audError;
    HAL_AIF_STREAM_T stream;
    AUD_LEVEL_T level;
    AUD_ITF_T itf=AUD_ITF_TV;   //xiaoyifeng for atv

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_BtWalkieTalkieAudioOpen!");

#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
    level.spkLevel=SND_SPK_VOL_15;
#else
    level.spkLevel=SND_SPK_VOL_7;
#endif
    level.sideLevel=0;
    level.micLevel=AUD_MIC_ENABLE;
    level.toneLevel=0;
    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

    // Set audio interface
    stream.startAddress = streamStartAddress;
    stream.length = streamLength;
    stream.sampleRate = HAL_AIF_FREQ_8000HZ;
    stream.channelNb = HAL_AIF_MONO;
    stream.voiceQuality = TRUE;
    stream.playSyncWithRecord = FALSE;
    // initialize the interrupt function.
    stream.halfHandler = streamXferhalfHandler;
    stream.endHandler = NULL;

    audError= aud_StreamStart(itf, &stream, &level);
    //aud_StreamRecord

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStart FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStart SUCCESS");
    }

    return audError;

}

#else
uint32 pcmbuf_tmp[64];//18*1024 bytes

UINT32 mmc_BtWalkieTalkieAudioOpen(VOID)
{
    AUD_ERR_T audError;
    HAL_AIF_STREAM_T stream;
    AUD_LEVEL_T level;
    AUD_ITF_T itf=AUD_ITF_TV;   //xiaoyifeng for atv

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_BtWalkieTalkieAudioOpen!");

#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
    level.spkLevel=SND_SPK_VOL_15;
#else
    level.spkLevel=SND_SPK_VOL_7;
#endif
    level.sideLevel=0;
    level.micLevel=AUD_MIC_ENABLE;
    level.toneLevel=0;
    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

    // Set audio interface
    stream.startAddress = (UINT32*)pcmbuf_tmp;
    stream.length = sizeof(pcmbuf_tmp);
    stream.sampleRate = HAL_AIF_FREQ_8000HZ;
    stream.channelNb = HAL_AIF_MONO;
    stream.voiceQuality = TRUE;
    stream.playSyncWithRecord = FALSE;
    // initialize the interrupt function.
    stream.halfHandler = NULL;
    stream.endHandler = NULL;

    audError= aud_StreamStart(itf, &stream, &level);
    //aud_StreamRecord

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStart FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStart SUCCESS");
    }

    return audError;


}
#endif
// =============================================================================
// MCI_TvStop
// -----------------------------------------------------------------------------
// =============================================================================
UINT32 mmc_BtWalkieTalkieSendAudioRxDisable(VOID)
{
    AUD_ERR_T audError = AUD_ERR_NO;
    AUD_ITF_T itf=AUD_ITF_TV;

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_ATV]mmc_BtWalkieTalkieAudioClose!");

    audError=aud_StreamStop(itf);

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,   "[MCI]aud_StreamStop FAILED");
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_StreamStop SUCCESS");
    }
    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    return audError;
}

//test for WT end
PUBLIC VOID mmc_AnalogTvRealTimeProcess(COS_EVENT *pnMsg)
{
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MMC_ANALOG_TV_REALTIME_PROCESS);
}

// =============================================================================
// mmc_AnalogTVUserMsg
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOID mmc_AnalogTvUserMsgHandle(COS_EVENT *pnMsg)
{
    MCI_TRACE(MCI_ANALOGTV_TRC, 0,"[MMC_ATV]mmc_AnalogTVUserMsg");

    switch (pnMsg->nEventId)
    {
        case  MMC_ANALOG_TV_REALTIME_PROCESS:
            MCI_TRACE(MCI_ANALOGTV_TRC, 0,"[MMC_ATV]REALTIME_PROCESS");
            RDAAPP_TimerHandler();
            break;

        default:
            //MCI_TRACE(MCI_ANALOGTV_TRC|TSTDOUT, 0,"[MMC_ATV]ERROR MESSAGE ID: %d",pnMsg->nEventId);
            break;
    }

    return;
}

