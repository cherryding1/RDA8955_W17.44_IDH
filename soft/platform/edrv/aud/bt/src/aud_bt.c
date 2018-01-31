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

#include "aud_m.h"
#include "audp_debug.h"
#include "hal_error.h"
#include "aud_codec_common.h"
#include "calib_m.h"


// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

PRIVATE HAL_AIF_SERIAL_CFG_T g_audBtAifSerialCfg;
PRIVATE HAL_AIF_CONFIG_T     g_audBtAifCfg;

#ifdef __BT_PCM_OVER_UART__

// =============================================================================
//  TYPES
// =============================================================================
// =============================================================================
// AUD_BT_STATE_T
// -----------------------------------------------------------------------------
/// This type describes the actions currently being operated on the AUD driver.
// =============================================================================
typedef struct
{

    SND_ITF_T   itf;
    HAL_AIF_STREAM_T  *play;
    HAL_AIF_STREAM_T  *record;
    UINT32*   pos;
    UINT32*   start;
    UINT32*   half;
    UINT32*   end;


} AUD_BT_STATE_T;


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
/// Opened status - All operation are described here.
PRIVATE AUD_BT_STATE_T  g_audBtState
    = {SND_ITF_NONE,NULL,NULL ,0,0,0,0 };

#endif

// =============================================================================
//  FUNCTIONS
// =============================================================================

// =============================================================================
// aud_BtPopulateAifCfg
// -----------------------------------------------------------------------------
/// Populate the AIF configuration structure.
/// @param stream Stream to play or record.
/// @return
// =============================================================================
PRIVATE VOID aud_BtPopulateAifCfg(CONST HAL_AIF_STREAM_T* stream)
{
    if (stream->voiceQuality)
    {
        AUD_ASSERT(stream->sampleRate == HAL_AIF_FREQ_8000HZ,
                   "AUD: BT: Voice mode only supports 8kHz frequency.");
        AUD_ASSERT(stream->channelNb == HAL_AIF_MONO,
                   "AUD: BT: Voice mode is mono only.");
        // TODO add a bunch of assert/check

        g_audBtAifSerialCfg.mode = HAL_SERIAL_MODE_VOICE;//HAL_SERIAL_MODE_I2S;
        g_audBtAifSerialCfg.aifIsMaster = TRUE;
        g_audBtAifSerialCfg.lsbFirst  = FALSE;
        g_audBtAifSerialCfg.polarity  = FALSE;
        g_audBtAifSerialCfg.rxDelay   = HAL_AIF_RX_DELAY_2; // <-- 2 !!!
        g_audBtAifSerialCfg.txDelay   = HAL_AIF_TX_DELAY_1;//HAL_AIF_TX_DELAY_ALIGN;
        g_audBtAifSerialCfg.rxMode    = HAL_AIF_RX_MODE_STEREO_MONO_FROM_L;
        g_audBtAifSerialCfg.txMode    = HAL_AIF_TX_MODE_MONO_STEREO_CHAN_L;

        g_audBtAifSerialCfg.fs = 8000;
        g_audBtAifSerialCfg.bckLrckRatio = 31;
        g_audBtAifSerialCfg.invertBck = FALSE;
        g_audBtAifSerialCfg.outputHalfCycleDelay = FALSE;
        g_audBtAifSerialCfg.inputHalfCycleDelay = FALSE;
        g_audBtAifSerialCfg.enableBckOutGating = FALSE;

        g_audBtAifCfg.interface = HAL_AIF_IF_SERIAL; // FIXME ?
        g_audBtAifCfg.serialCfg = &g_audBtAifSerialCfg;
        g_audBtAifCfg.channelNb = HAL_AIF_MONO;
        g_audBtAifCfg.sampleRate = HAL_AIF_FREQ_8000HZ;
    }
    else
    {
        g_audBtAifCfg.interface    = HAL_AIF_IF_PARALLEL_STEREO;
        g_audBtAifCfg.serialCfg    = NULL;
        g_audBtAifCfg.sampleRate   = stream->sampleRate;
        g_audBtAifCfg.channelNb    = stream->channelNb;
    }
}


// =============================================================================
// aud_BtSetup
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
PUBLIC AUD_ERR_T aud_BtSetup(SND_ITF_T itf, CONST AUD_DEVICE_CFG_T* cfg)
{
    INT8 AlgGain;
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    UINT8 calibItf = itf;
    if((itf == AUD_ITF_BLUETOOTH) && (aud_CodecCommonGetBtNRECFlag()))
    {
        calibItf = CALIB_ITF_BLUETOOTH_NREC;
    }

    CALIB_AUDIO_OUT_GAINS_T *pCalibOutGains =
        &calibPtr->bb->audio[calibItf].audioGains.outGains[cfg->level->spkLevel];

    if (aud_CodecCommonGetVoiceQuality())
        AlgGain = pCalibOutGains->voiceOrEpAlg;
    else
        AlgGain = pCalibOutGains->musicOrLsAlg;
    aud_CodecCommonOutAlgGainDb2Val( AlgGain);

    return AUD_ERR_NO;
}

#ifdef __BT_PCM_OVER_UART__

extern UINT16 connect_id;
extern void mmi_bth_hfg_send_data_req_hdler( UINT32 len, UINT8 *data);

void aud_BtSendSCOData(UINT8 half )
{

    if (( g_audBtState.itf  != SND_ITF_BLUETOOTH)||( g_audBtState.play ==NULL))
        return;

    HCI_SendSCOData(connect_id,  g_audBtState.play ->length/2,
                    g_audBtState.play->startAddress +  half*g_audBtState.play ->length/8 );

    return;

}


void aud_BtGetSCOData(UINT8 type, void *pdu, UINT32 len)
{

    UINT8 SCO_Buf[120];
    UINT32 SCO_length;
    UINT32 length;


    if (( g_audBtState.itf  != SND_ITF_BLUETOOTH)||( g_audBtState.record ==NULL )||  (type != 0x03) )
        return;

    HCI_GetSCOData(&SCO_length, SCO_Buf, pdu);

    //  AUD_TRACE(AUD_INFO_TRC, 0,   " aud_BtGetSCOData    g_audBtState.pos = 0x%x,SCO_length = %d ", g_audBtState.pos,SCO_length);

    if( SCO_length ==0 )   return;


    if( g_audBtState.pos  < g_audBtState.half)
    {

        memcpy((UINT8*)g_audBtState.pos, SCO_Buf,SCO_length);

        g_audBtState.pos+=SCO_length/4;//UINT32

        if(( g_audBtState.pos>= g_audBtState.half)&&( g_audBtState.record->halfHandler))
        {
            //           AUD_TRACE(AUD_INFO_TRC, 0,   " aud_BtGetSCOData    11111111");
            g_audBtState.record->halfHandler();
        }
    }
    else
    {
        length =   (g_audBtState.end -g_audBtState.pos)*4;

        if(SCO_length < length)
        {
            memcpy((UINT8*)g_audBtState.pos, SCO_Buf,SCO_length);
            g_audBtState.pos+=SCO_length/4;
        }
        else
        {
            memcpy((UINT8*)g_audBtState.pos, SCO_Buf,length);

            if(g_audBtState.record->endHandler )
            {
                //                AUD_TRACE(AUD_INFO_TRC, 0,   " aud_BtGetSCOData    44444  length= %d  ",length);
                g_audBtState.record->endHandler();
            }
            memcpy((UINT8*)g_audBtState.start, (SCO_Buf+length),SCO_length-length);
            g_audBtState.pos =g_audBtState.start + (SCO_length-length)/4;

        }

    }

    return;

}

extern int HCI_RegisterRxHandler(void (*callbackFunc)(UINT8 type, void *pdu, UINT32 len));
extern int  HCI_DeregisterRxHandler(void (*callbackFunc)(UINT8 type, void *pdu, UINT32 len));

#endif


// =============================================================================
// aud_BtStreamStart
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
PUBLIC AUD_ERR_T aud_BtStreamStart(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{

#ifdef __BT_PCM_OVER_UART__

    g_audBtState.itf    =  itf;
    g_audBtState.play = stream;

    //    return AUD_ERR_NO;
#endif

    aud_BtPopulateAifCfg(stream);
    if (!aud_CodecCommonSetConfig(FALSE, HAL_AUD_USER_AIF_ONLY, &g_audBtAifCfg))
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    return aud_CodecCommonStreamStart(itf, stream, cfg);


}



// =============================================================================
// aud_BtStreamStop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback or/and record.
/// But it doesn't free the audio out resource. If the function returns
/// #AUD_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #AUD_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AUD_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AUD_ERR_T aud_BtStreamStop(SND_ITF_T itf)
{
#ifdef __BT_PCM_OVER_UART__
    g_audBtState.itf        =  SND_ITF_NONE;
    g_audBtState.play     =  NULL;
    g_audBtState.record  =  NULL;

    g_audBtState.pos    = 0;
    g_audBtState.start  = 0;
    g_audBtState.half   =0;
    g_audBtState.end   =0;

    HCI_DeregisterRxHandler(aud_BtGetSCOData );
// return  AUD_ERR_NO;
#endif
    return aud_CodecCommonStreamStop(itf);


}

// =============================================================================
// aud_BtStreamPause
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
PUBLIC AUD_ERR_T aud_BtStreamPause(SND_ITF_T itf, BOOL pause)
{
    return aud_CodecCommonStreamPause(itf, pause);
}


// =============================================================================
// aud_BtStreamRecord
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
PUBLIC AUD_ERR_T aud_BtStreamRecord(
    SND_ITF_T itf,
    CONST HAL_AIF_STREAM_T* stream,
    CONST AUD_DEVICE_CFG_T *       cfg)
{

#ifdef __BT_PCM_OVER_UART__

    g_audBtState.itf    =  itf;
    g_audBtState.record= stream;
    g_audBtState.start =(UINT32*)  stream->startAddress;
    g_audBtState.pos = (UINT32*)  stream->startAddress;

    g_audBtState.end  = (UINT32*)  ( stream->startAddress +stream->length/4); //UINT 32
    g_audBtState.half  = (UINT32*)  ( stream->startAddress +stream->length/8); //UINT 32

    HCI_RegisterRxHandler(aud_BtGetSCOData );

    return AUD_ERR_NO;

#else

    aud_BtPopulateAifCfg(stream);
    if (!aud_CodecCommonSetConfig(TRUE, HAL_AUD_USER_AIF_ONLY, &g_audBtAifCfg))
    {
        AUD_TRACE(AUD_INFO_TRC, 0,
                  "AUD: " __FILE__ ", line:%d, Resource Busy",__LINE__);
        return AUD_ERR_RESOURCE_BUSY;
    }

    return   aud_CodecCommonStreamRecord(itf, stream, cfg);

#endif

}



// =============================================================================
// aud_BtTone
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
PUBLIC AUD_ERR_T aud_BtTone(
    SND_ITF_T itf,
    CONST SND_TONE_TYPE_T        tone,
    CONST AUD_DEVICE_CFG_T*             cfg,
    CONST BOOL                   start)
{
    return aud_CodecCommonTone(itf, tone, cfg, start);
}



// =============================================================================
// aud_BtTonePause
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
PUBLIC AUD_ERR_T aud_BtTonePause(SND_ITF_T itf, BOOL pause)
{
    return aud_CodecCommonTonePause(itf, pause);
}


// =============================================================================
// aud_BtCalibUpdateValues
// -----------------------------------------------------------------------------
/// Update values depending on calibration parameters.
///
/// @param itf AUD interface on which to stop the tone.
/// @return #AUD_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #AUD_ERR_NO otherwise
// =============================================================================
PUBLIC AUD_ERR_T aud_BtCalibUpdateValues(SND_ITF_T itf)
{
    return AUD_ERR_NO;
}




