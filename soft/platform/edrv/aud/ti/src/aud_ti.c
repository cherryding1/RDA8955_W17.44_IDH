////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//            Copyright (C) 2003-2007, Coolsand Technologies, Inc.            //
//                            All Rights Reserved                             //
//                                                                            //
//      This source code is the property of Coolsand Technologies and is      //
//      confidential.  Any  modification, distribution,  reproduction or      //
//      exploitation  of  any content of this file is totally forbidden,      //
//      except  with the  written permission  of  Coolsand Technologies.      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  $HeadURL: http://svn.rdamicro.com/svn/developing1/Sources/edrv/trunk/aud/ti/src/aud_ti.c $ //
//    $Author: jingyuli $                                                        //
//    $Date: 2016-06-07 16:37:31 +0800 (Tue, 07 Jun 2016) $                     //
//    $Revision: 32438 $                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file aud_ti.c                                                        //
/// That file provides the implementation of the AUD interface for the ti //
//  chip.                                                                     //
////////////////////////////////////////////////////////////////////////////////


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================

#include "cs_types.h"

#include "aud_ti.h"
#include "audp_ti.h"
#include "audp_debug.h"

#include "hal_i2c.h"
#include "hal_aif.h"
#include "hal_timers.h"
#include "hal_error.h"
#include "hal_sys.h"

#include "tgt_aud_cfg.h"

// =============================================================================
//  MACROS
// =============================================================================
/// Helper to write a register
#define TI_WR(reg,val)   \
   halErr = HAL_ERR_RESOURCE_BUSY; \
    while (halErr == HAL_ERR_RESOURCE_BUSY) \
    { \
        halErr = hal_I2cSendByte(HAL_I2C_BUS_ID_1, TI_ADDR, ((reg)<<1)|(((val)&0x100)>>8), (val)&0xff); \
    } \
    if (halErr != HAL_ERR_NO) \
    { \
        AUD_ASSERT(FALSE, "%s line %d", __FILE__, __LINE__); \
            return AUD_ERR_RESOURCE_BUSY; \
    }

// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Playing status
PRIVATE BOOL  g_audTiPlaying __attribute__((section (".aud.globalvars.c"))) = FALSE;

/// Recording status
PRIVATE BOOL  g_audTiRecording __attribute__((section (".aud.globalvars.c"))) = FALSE;

/// Openness status
PRIVATE BOOL  g_audTiOpened __attribute__((section (".aud.globalvars.c"))) = FALSE;

/// Being open status
PRIVATE BOOL g_audTiOpening __attribute__((section (".aud.globalvars.c"))) = FALSE;

/// Remember the last settings applied
/// They will be set again to take into account any potential
/// calibration change.
PROTECTED AUD_LEVEL_T g_audTiLatestLevel __attribute__((section (".aud.globalvars.c"))) =
{
    .spkLevel   = AUD_SPK_MUTE,
    .micLevel   = AUD_MIC_MUTE,
    .sideLevel  = AUD_SIDE_MUTE,
    .toneLevel  = AUD_TONE_0dB,
};

PROTECTED AUD_DEVICE_CFG_T g_audTiLatestCfg  __attribute__((section (".aud.globalvars.c"))) =
{
    .spkSel = AUD_SPK_RECEIVER,
    .micSel = AUD_MIC_RECEIVER,
    .level  = &g_audTiLatestLevel
};


// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// aud_TieGetRateStg
// -----------------------------------------------------------------------------
/// @return value to write in the correct ti register to set the sample rate
/// using HAL types.
// =============================================================================
INLINE UINT32 aud_TieGetRateStg(HAL_AIF_FREQ_T sampleRate)
{
    // Based on the USB-Mode sampling rates table for TI TLV320AIC23B
    UINT32 setting;
    switch (sampleRate)
    {
        case HAL_AIF_FREQ_8000HZ :
            setting = TI_SR_VALUE(3) | TI_BOSR_SLOW;
            break;
        case HAL_AIF_FREQ_44100HZ:
            setting = TI_SR_VALUE(8) | TI_BOSR_FAST;
            break;
        case HAL_AIF_FREQ_48000HZ:
            setting = TI_SR_VALUE(0) | TI_BOSR_SLOW;
            break;
        case HAL_AIF_FREQ_32000HZ:
            setting = TI_SR_VALUE(6) | TI_BOSR_SLOW;
            break;
        case HAL_AIF_FREQ_11025HZ:
        case HAL_AIF_FREQ_12000HZ:
        case HAL_AIF_FREQ_16000HZ:
        case HAL_AIF_FREQ_22050HZ:
        case HAL_AIF_FREQ_24000HZ:
        default:
            // Use the 32K instead of assert
            setting = TI_SR_VALUE(6) | TI_BOSR_SLOW;
            AUD_TRACE(TSTDOUT, 0,
                      "AUD: " __FILE__ ", line:%d, Unsupported sample rate: %d", __LINE__, sampleRate);
            //AUD_ASSERT(FALSE, "Unsupported sample rate in AUD: %s, %d", __FILE__, __LINE__);
    }

    return setting;
}

// =============================================================================
// aud_ChipConfig
// -----------------------------------------------------------------------------
/// Configure the ti chip, through I2C.
/// The stream parameter is needed to know the format of the PCM
/// thing to play. The cfg one is to know which mic, spk etc use
/// Interfaces matching table:
/// -
/// -
/// .
///
///
/// @param sampleRate Sample Rate of the audio to play
/// @param voice \c TRUE if in voice mode.
/// @param master \c TRUE if the ti is the I2S master.
/// @return
// =============================================================================
PRIVATE AUD_ERR_T aud_ChipConfig(HAL_AIF_FREQ_T sampleRate, BOOL voice, BOOL mono, BOOL master)
{
    AUD_PROFILE_FUNCTION_ENTRY(aud_ChipConfig);
    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    if (voice)
    {
        AUD_ASSERT(sampleRate == HAL_AIF_FREQ_8000HZ,
                   "AUD: ti: Voice mode only supports 8kHz frequency.");
    }

    TI_WR(REG_TI_RESET, TI_RESET);

    TI_WR(REG_TI_LINE_VOL_L, TI_LRS_ON|TI_LIM_ON);
    // Avoid explosive voice pulse when turning on audio chip
    TI_WR(REG_TI_HEAD_VOL_L, TI_LRS_ON|TI_LHV_MUTE);

    TI_WR(REG_TI_ANA_AUD_CTRL, TI_SIDETONE_0DB|TI_ANA_DAC_ON|TI_ANA_BYP_OFF|TI_INSEL_MIC|TI_MICM_ON|TI_MICB_OFF);

    TI_WR(REG_TI_DIG_AUD_CTRL, TI_DACM_OFF|TI_DEEMP_OFF|TI_ADCHP_OFF);

    UINT32 clkMode, lrpAndDataFormat;
    if (master)
    {
        clkMode = TI_MODE_MASTER;
    }
    else
    {
        clkMode = TI_MODE_SLAVE;
    }
    if (voice)
    {
        lrpAndDataFormat = TI_LRP_R_LRCIN_LOW|TI_FOR_L_ALIGN;
    }
    else
    {
        lrpAndDataFormat = TI_LRP_R_LRCIN_HIGH|TI_FOR_I2S;
    }
    TI_WR(REG_TI_DIG_FORMAT, TI_LRSWAP_OFF|TI_IWL_16BIT|clkMode|lrpAndDataFormat);

    TI_WR(REG_TI_SAMPLE_RATE, TI_CLKIN_DIV_OFF|TI_CLKOUT_DIV_OFF|aud_TieGetRateStg(sampleRate)|TI_CLK_MODE_USB);

    TI_WR(REG_TI_DIG_ACT, TI_ACT_ON);

    TI_WR(REG_TI_POWER_CTRL, TI_POWER_ON|TI_CLK_ON|TI_OSC_ON|TI_OUT_ON|TI_DAC_ON|TI_ADC_ON|TI_MIC_ON|TI_LINE_OFF);

    AUD_PROFILE_FUNCTION_EXIT(aud_ChipConfig);
    return AUD_ERR_NO;
}


// =============================================================================
// aud_TiAifConfig
// -----------------------------------------------------------------------------
/// Configure the AIF interface for use with the ti chip.
/// @param sampleRate Sample Rate of the audio to play
/// @param voice \c TRUE if in voice mode.
/// @param mono  \c TRUE if the stream played/recorded is in Mono
/// @param master Is the ti master ?
/// @return AUD_ERR_NO
// =============================================================================
INLINE AUD_ERR_T aud_TiAifConfig(HAL_AIF_FREQ_T sampleRate, BOOL voice,
                                 BOOL mono, BOOL master,
                                 HAL_AIF_IF_T aifItf)
{
    HAL_AIF_SERIAL_CFG_T serialCfg;
    HAL_AIF_CONFIG_T     aifCfg;

    if (voice)
    {
        serialCfg.mode = HAL_SERIAL_MODE_VOICE;
    }
    else
    {
        serialCfg.mode = HAL_SERIAL_MODE_I2S;
    }
    // TODO add the DAI case !

    serialCfg.aifIsMaster = !master; // If the ti is master, the AIF ain't.
    serialCfg.lsbFirst  = FALSE;
    serialCfg.polarity  = !voice; // must be LEFT_H_RIGHT_L in voice mode;
    serialCfg.rxDelay   = HAL_AIF_RX_DELAY_2;
    serialCfg.txDelay   = HAL_AIF_TX_DELAY_1;
    if (mono)
    {
        serialCfg.rxMode    = HAL_AIF_RX_MODE_STEREO_MONO_FROM_L;
        serialCfg.txMode    = HAL_AIF_TX_MODE_MONO_STEREO_DUPLI;
    }
    else
    {
        serialCfg.rxMode    = HAL_AIF_RX_MODE_STEREO_STEREO;
        serialCfg.txMode    = HAL_AIF_TX_MODE_STEREO_STEREO;
    }

// LCK/BCK
    switch (sampleRate)
    {
        case HAL_AIF_FREQ_8000HZ:
            serialCfg.fs = 8000;
            if (voice)
            {
                serialCfg.bckLrckRatio = 25;
            }
            else
            {
                serialCfg.bckLrckRatio = 50;
            }
            break;

        case HAL_AIF_FREQ_11025HZ:
            serialCfg.fs = 11025;
            serialCfg.bckLrckRatio = 36;
            break;

        case HAL_AIF_FREQ_12000HZ:
            serialCfg.fs = 12000;
            serialCfg.bckLrckRatio = 38;
            break;

        case HAL_AIF_FREQ_16000HZ:
            serialCfg.fs = 16000;
            serialCfg.bckLrckRatio = 50;
            break;

        case HAL_AIF_FREQ_22050HZ:
            serialCfg.fs = 22050;
            serialCfg.bckLrckRatio = 40;
            break;

        case HAL_AIF_FREQ_24000HZ:
            serialCfg.fs = 24000;
            serialCfg.bckLrckRatio = 38;
            break;

        case HAL_AIF_FREQ_32000HZ:
            serialCfg.fs = 32000;
            serialCfg.bckLrckRatio = 56;
            break;

        case HAL_AIF_FREQ_44100HZ:
            serialCfg.fs = 44100;
            serialCfg.bckLrckRatio = 62;
            break;

        case HAL_AIF_FREQ_48000HZ:
            serialCfg.fs = 48000;
            serialCfg.bckLrckRatio = 36;
            break;

        default:
            AUD_ASSERT(FALSE, "Improper stream frequency.");
            // Die
            break;
    }


// -----------------------
    serialCfg.invertBck = master?TRUE:FALSE;
    serialCfg.outputHalfCycleDelay = master?FALSE:TRUE;
    serialCfg.inputHalfCycleDelay = FALSE;
    serialCfg.enableBckOutGating = FALSE;

#if (CHIP_HAS_I2S_DI_1 == 1)
    // We need to choose the I2S 'interface' to use
    aifCfg.interface = aifItf;
#else
    aifCfg.interface = HAL_AIF_IF_SERIAL;
#endif
    aifCfg.sampleRate = sampleRate;
    aifCfg.channelNb = mono ? HAL_AIF_MONO : HAL_AIF_STEREO;
    aifCfg.serialCfg = &serialCfg;

    hal_AifOpen(&aifCfg);

    return AUD_ERR_NO;
}


// =============================================================================
// aud_TiOpen
// -----------------------------------------------------------------------------
/// Open the driver and configure the ti and AIF for use with the ti chip.
/// @param sampleRate Sample Rate of the audio to play
/// @param voice \c TRUE if in voice mode.
/// @param master Is the ti master ?
/// @return AUD_ERR_NO or ...
// =============================================================================
PRIVATE AUD_ERR_T aud_TiOpen(HAL_AIF_FREQ_T sampleRate, BOOL voice, BOOL mono, BOOL master,
                             HAL_AIF_IF_T aifItf)
{
    // TODO Use a global var not to open it again
    UINT32 status = hal_SysEnterCriticalSection();
    if (g_audTiOpening)
    {
        hal_SysExitCriticalSection(status);
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }
    else
    {
        g_audTiOpening = TRUE;
        hal_SysExitCriticalSection(status);

        hal_I2cOpen(HAL_I2C_BUS_ID_1);

        AUD_ERR_T errStatus;
        errStatus = aud_ChipConfig(sampleRate, voice, mono, master);
        if (errStatus == AUD_ERR_NO)
        {
            errStatus = aud_TiAifConfig(sampleRate, voice, mono, master, aifItf);
            if (errStatus == AUD_ERR_NO)
            {
                g_audTiOpened = TRUE;
            }
        }
        g_audTiOpening = FALSE;
        return errStatus;
    }
}


// =============================================================================
// aud_TiClose
// -----------------------------------------------------------------------------
/// Close the driver and configure the ti and AIF for use with the ti chip.
/// @return AUD_ERR_NO or ...
// =============================================================================
PRIVATE AUD_ERR_T aud_TiClose(VOID)
{
    hal_AifClose();

    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    TI_WR(REG_TI_POWER_CTRL, TI_POWER_ALL_OFF);

    hal_I2cClose(HAL_I2C_BUS_ID_1);

    g_audTiOpened = FALSE;

    return AUD_ERR_NO;
}



// =============================================================================
// aud_TiSetup
// -----------------------------------------------------------------------------
/// Configure the audio.
///
/// This functions configures an audio interface: gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker.
///
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_DEVICE_CFG_T for more details.
/// @return #AUD_ERR_NO if it can execute the configuration.
// =============================================================================
PUBLIC AUD_ERR_T aud_TiSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg)
{

    HAL_ERR_T halErr = HAL_ERR_RESOURCE_BUSY;

    // regSpkLvl:
    // 1111111 = +6 dB, 79 steps between +6 dB and -73 dB (mute), 0110000 = -73 dB (mute)
#define SPEAKER_LEVEL_STEP (79 / (AUD_SPK_VOL_QTY-1))
    UINT8 regSpkLvl = cfg->level->spkLevel * SPEAKER_LEVEL_STEP + 0x30;

    // Record last config
    g_audTiLatestCfg.spkSel = cfg->spkSel;
    g_audTiLatestCfg.spkType = cfg->spkType;
    g_audTiLatestCfg.micSel = cfg->micSel;
    g_audTiLatestLevel = *(cfg->level);

    if (cfg->level->spkLevel == AUD_SPK_MUTE)
    {
        TI_WR(REG_TI_HEAD_VOL_L, TI_LRS_ON|TI_LHV_MUTE);
    }
    else
    {
        TI_WR(REG_TI_HEAD_VOL_L, TI_LRS_ON|TI_LZC_ON|TI_LHV_VALUE(regSpkLvl));
        //TI_WR(REG_TI_HEAD_VOL_L, TI_LRS_OFF|TI_LZC_ON|TI_LHV_VALUE(regSpkLvl));
        //TI_WR(REG_TI_HEAD_VOL_R, TI_RLS_OFF|TI_RZC_ON|TI_RHV_VALUE(regSpkLvl));
    }

    UINT32 micMute;
    if (cfg->level->micLevel == AUD_MIC_MUTE)
    {
        micMute = TI_MICM_ON;
    }
    else
    {
        micMute = TI_MICM_OFF;
    }
    TI_WR(REG_TI_ANA_AUD_CTRL, TI_SIDETONE_0DB|TI_ANA_DAC_ON|TI_ANA_BYP_OFF|
          TI_INSEL_MIC|TI_MICB_OFF|micMute);

    // Side volume controlled in the AIF
    hal_AifSetSideTone(cfg->level->sideLevel);

    return AUD_ERR_NO;
}



// =============================================================================
// aud_TiStreamStart
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #aud_Setup function. \n
/// In normal operation, when
/// the buffer runs out, the playing will wrap at the beginning. Here, there are two ways
/// you can handle your buffer: HAL can call a user function at the middle or
/// at the end of the playback or, depending
/// on your application, you can simply poll the playing state using the "reached half"
/// and "reached end functions" TODO Ask if needed, and then implement !
///
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command,
///         #AUD_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TiStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_ERR_T errStatus = AUD_ERR_NO;
    HAL_AIF_IF_T aifItf;

    // Get the I2S interface this driver instance is plugged onto:
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    aifItf = audioCfg[itf].parameter;


    if (g_audTiPlaying)
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (! g_audTiOpened)
        {
            errStatus = aud_TiOpen(stream->sampleRate, stream->voiceQuality,
                                   (stream->channelNb == HAL_AIF_MONO), FALSE, aifItf); // The AIF module is the master.
        }

        if (errStatus == AUD_ERR_NO)
        {
            errStatus = aud_TiSetup(itf, cfg);
            if (errStatus == AUD_ERR_NO)
            {
                // Send the stream through the IFC
                if (hal_AifPlayStream(stream) != HAL_ERR_NO)
                {
                    AUD_TRACE(AUD_INFO_TRC, 0,
                              "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
                    errStatus = AUD_ERR_RESOURCE_BUSY;
                }
                else
                {
                    g_audTiPlaying = TRUE;
                }
            }
        }
        return errStatus;
    }
}



// =============================================================================
// aud_StreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TiStreamStop(SND_ITF_T itf)
{
    if (g_audTiPlaying)
    {
        hal_AifStopPlay();
        g_audTiPlaying = FALSE;
    }

    if (g_audTiRecording)
    {
        hal_AifStopRecord();
        g_audTiRecording = FALSE;
    }

    if (g_audTiOpened)
    {
        return aud_TiClose();
    }
    else
    {
        return AUD_ERR_NO;
    }
}

// =============================================================================
// aud_StreamPause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses the stream. If \c FALSE, resumes a paused
/// stream.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TiStreamPause(SND_ITF_T itf, BOOL pause)
{
    switch (hal_AifPause(pause))
    {
        case HAL_ERR_NO:
        default:
            return AUD_ERR_NO;
            break;
    }
}


// =============================================================================
// aud_StreamRecord
// -----------------------------------------------------------------------------
/// Manage the recording of a stream.
///
///
/// The #aud_StreamRecord send a message to the driver which begins the
/// dataflow from the audio and the compression. If the function returns \c
/// FALSE, it means that the driver cannot handle the record. The value \c len
/// should be the maximum size of the recording buffer and when the buffer is
/// full, the recording will automatically wrap. A
/// user function can be called, either when the middle or the end of the
/// buffer is reached. (TODO add iatus about polling if available)
///
/// @param stream Stream recorded (describe the buffer and the possible interrupt
/// of middle-end buffer handlers.
/// @param cfg The configuration set applied to the audio interface.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver cannot handle the stream,
///         #AUD_ERR_NO if the stream ca be recorded.
// =============================================================================
PUBLIC AUD_ERR_T aud_TiStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{
    AUD_ERR_T errStatus = AUD_ERR_NO;
    HAL_AIF_IF_T aifItf;
    // Get the I2S interface this driver instance is plugged onto:
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    aifItf = audioCfg[itf].parameter;


    if (g_audTiRecording)
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }
    else
    {
        if (!g_audTiOpened)
        {
            errStatus = aud_TiOpen(stream->sampleRate, stream->voiceQuality,
                                   (stream->channelNb == HAL_AIF_MONO),FALSE, aifItf); // The AIF module is the master.
        }

        if (errStatus == AUD_ERR_NO)
        {
            errStatus = aud_TiSetup(itf, cfg);
            if (errStatus == AUD_ERR_NO)
            {
                // Send the stream through the IFC
                if (hal_AifRecordStream(stream) != HAL_ERR_NO)
                {
                    AUD_TRACE(AUD_INFO_TRC, 0,
                              "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
                    errStatus = AUD_ERR_RESOURCE_BUSY;
                }
                else
                {
                    g_audTiRecording = TRUE;
                }
            }
        }
        return errStatus;
    }
    return AUD_ERR_NO;
}



// =============================================================================
// aud_TiTone
// -----------------------------------------------------------------------------
//  Manage the playing of a tone: DTMF or Comfort Tone.
///
/// Outputs a DTMF or comfort tone
///
/// When the audio output is enabled, a DTMF or a comfort tones can be output
/// as well. This function starts the output of a tone generated in the audio
/// module. \n
/// You can call this function several times without calling the
/// #aud_ToneStop function or the #aud_TonePause function in
/// between, if you just need to change the attenuation or the tone type. \n
/// If the function returns #AUD_ERR_RESOURCE_BUSY, it means that the driver is
/// busy with an other audio command.
///
/// @param tone The tone to generate
/// @param attenuation The attenuation level of the tone generator
/// @param cfg The configuration set applied to the audio interface
/// @param start If \c TRUE, starts the playing of the tone.
///              If \c FALSE, stops the tone.
///
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_TiTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T        tone,
    CONST AUD_DEVICE_CFG_T*             cfg,
    CONST BOOL                   start)
{
    AUD_ERR_T errStatus = AUD_ERR_NO;
    HAL_AIF_IF_T aifItf;
    // Get the I2S interface this driver instance is plugged onto:
    CONST AUD_ITF_CFG_T* audioCfg = tgt_GetAudConfig();
    aifItf = audioCfg[itf].parameter;


    if (!g_audTiOpened && start == TRUE)
    {
        // Open the ti
        // FIXME Use the stream phantom !
        errStatus = aud_TiOpen(HAL_AIF_FREQ_8000HZ, FALSE, TRUE, FALSE, aifItf); // The AIF module is the master.
    }

    if (start == FALSE)
    {
        hal_AifTone(0, 0, FALSE);
        if( !g_audTiPlaying && !g_audTiRecording)
        {
            aud_TiClose();
        }
    }
    else
    {
        if (errStatus == AUD_ERR_NO)
        {
            errStatus = aud_TiSetup(itf, cfg);
            if (errStatus == AUD_ERR_NO)
            {
                hal_AifTone(tone, cfg->level->toneLevel, start);
            }
        }
    }

    return AUD_ERR_NO;

}



// =============================================================================
// aud_TiTonePause
// -----------------------------------------------------------------------------
/// This function pauses the audio tone.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses the tone. If \c FALSE, resumes a paused
/// tone.
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_TiTonePause(SND_ITF_T itf, BOOL pause)
{
    switch (hal_AifTonePause(pause))
    {
        case HAL_ERR_NO:
        default:
            return AUD_ERR_NO;
            break;
    }
}



// =============================================================================
// aud_TiCalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_TiCalibUpdateValues(SND_ITF_T itf)
{
    // FIXME --> Open the i2c before ...
//    return aud_TiSetup(itf, &g_audTiLatestCfg);
    return HAL_ERR_NO;
}





