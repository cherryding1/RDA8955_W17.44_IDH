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

#include "string.h"

#include "hal_aif.h"
#include "hal_sys.h"

#include "hal_host.h"
#include "hal_debug.h"

#include "sxr_ops.h"
#include "sxs_io.h"
#include "sxr_tls.h"

#include "aud_m.h"

#include "avrs_m.h"
#include "avrsp.h"
#include "avrsp_debug.h"

#include "avrsp_amjr.h"

#include "avrsp_video_handler.h"


#include "vpp_speech.h"
#include "vpp_amjr.h"

#include "imgs_m.h"




// svn propset svn:keywords "HeadURL Author Date Revision" dummy.h

/// TODO Does this part of the doc belong to the API ?
/// Workflow description
///   Init:
///     Init VPP, and load the first sample in its decoding buffer.
///     Mute mode is set.
///     Lauch play on the output decoded buffer of VPP.
///
///   Data flow:
///     Data are copied on VPP interrupts, meaning the previous set has been
///     decoded. Decoding is triggered by the IFC 'interrupt' directly telling
///     VPP what to do.
///     On IRQ from VPP, we copy the next buffer to be decoded.

// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================



// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

/// Start address of the stream buffer
PROTECTED UINT32* g_avrsBufferStart = NULL;

/// End address of the stream buffer
PROTECTED UINT32 g_avrsBufferLength = 0;

/// End address of the stream buffer
PROTECTED UINT32* g_avrsBufferTop = NULL;

/// Current position in the buffer. Next read data will be
/// read from here.
PROTECTED UINT32* g_avrsBufferCurPos = NULL;

/// Number of bytes of data to read from the buffer
PROTECTED UINT32 g_avrsBufferAvailBytes = 0;

/// Pointer to the ouput (encoded) buffer of VPP part of VPP decoding buffer.
PROTECTED UINT32* g_avrsEncOutput = NULL;

/// Number of played trames
PROTECTED UINT32 g_avrsTramePlayedNb = 0;

/// Number of trames dropped
PROTECTED UINT32 g_avrsTrameDroppedNb = 0;

/// User handler to call with play status is stored here.
PROTECTED UINT8 g_avrsAudioMbx = 0xFF;


/// Audio configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PROTECTED AUD_LEVEL_T g_avrsAudioCfg;

/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PROTECTED VPP_SPEECH_AUDIO_CFG_T g_avrsVppCfg;

/// Interface used to play the current stream.
PROTECTED AUD_ITF_T g_avrsItf = 0;

/// Index in the swap buffer (for PCM) which is the one where to copy
/// the new data
PROTECTED UINT32 g_avrsSwIdx      = 0;

/// Local buffer (TODO Use a malloc to avoid taking unneeded
/// memory when we are not recording)
PROTECTED UINT32 g_avrsPcmBuf[2*576*2]
__attribute__((section (".ucbss")));

/// Number of bytes composing one trame for the codec currently
/// used
PROTECTED UINT32 g_avrsFrameSize  = 0;

/// Handshake to prevent the same message to be sent several times
/// When a message is sent, this variable is set to FALSE.
/// When data are added, it is set to TRUE.
PROTECTED BOOL g_avrsBufferingMsgReceived = TRUE;


/// Video preview configuration structure.
/// TODO: replace with a mallocated structure.
PROTECTED AVRS_DECODED_PICTURE_T g_avrsPreviewCfg =
{
    .fbw        = NULL,
    .rotation   = 0,
    .callback   = NULL
};


/// Video preview configuration structure.
/// TODO: replace with a mallocated structure.
PROTECTED AVRS_ENCODED_PICTURE_T g_avrsVideoFrameCfg =
{
    .buffer     = NULL,
    .rotation   = 0,
    .encCfg     =
    {
        .format = IMGS_IMG_FORMAT_JPG,
        {
            .jpg    =
            {
                .quality    = IMGS_JPG_QUALITY_HIGH
            },
        }
    },
    .callback   = NULL
};


/// Video stream being produced by AVRS
PROTECTED AVRS_ENC_VIDEO_STREAM_T g_avrsVideoStream  =
{
    .startAddress       = 0,
    .length             = 0,
    .mode               = AVRS_REC_VIDEO_MODE_NONE,
    .resolution         = AVRS_REC_VIDEO_RESOLUTION_QQVGA,
    .frameRate          = 0,
    .lowQuality         = FALSE,
    .mbx                = 0xFF,
    .previewFbw         = NULL,
    .previewCallback    = NULL
};

/// Audio stream being produced by AVRS
PROTECTED AVRS_ENC_AUDIO_STREAM_T g_avrsAudioStream  =
{
    .startAddress       = 0,
    .length             = 0,
    .mode               = AVRS_REC_AUDIO_MODE_NONE,
    .sampleRate         = 0,
    .channelNb          = HAL_AIF_MONO,
    .voiceQuality       = FALSE,
    .mbx                = 0xFF,
};

/// Are we recording video ?
PROTECTED BOOL g_avrsVideoEnabled  = FALSE;

/// Are we recording audio ?
PROTECTED BOOL g_avrsAudioEnabled  = FALSE;


/// Latest encoded frame pointer.
PROTECTED UINT8* g_avrsLatestEncodedFrame  = NULL;

/// Latest encoded frame length.
PROTECTED UINT32 g_avrsLatestEncodedFrameLength = 0;


//  video streams descriptors.
PROTECTED AVRS_FBW_STREAM_T           g_avrsFbwStream;

PROTECTED AVRS_ENC_BUFFER_STREAM_T    g_avrsEncBufferStream;

// Pointer to the first half of the PCM buffer
PROTECTED UINT32* g_avrsPcmBuf_0;

// Pointer to the second half of the PCM buffer
PROTECTED UINT32* g_avrsPcmBuf_1;

// Pointer to the first half of the encoded buffer, when using an encoded swap buffer.
PROTECTED UINT32* g_avrsEncBuf_0;

// Pointer to the second half of the encoded buffer, when using an encoded swap buffer.
PROTECTED UINT32* g_avrsEncBuf_1;

/// Last set video configuration.
PROTECTED AVRS_VIDEO_CFG_T  g_avrsVideoCfg;


// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// avrs_Setup
// -----------------------------------------------------------------------------
/// Configure the avrs service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param itf Interface number of the interface to setup.
/// @param audioCfg The configuration set applied to the audio interface. See
/// #AVRS_AUDIO_CFG_T for more details.
/// @param videoCfg The configuration set applied to the video recorder. See
/// #AVRS_VIDEO_CFG_T for more details.
/// @return #AVRS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Setup(CONST AVRS_AUDIO_CFG_T* audioCfg,
                             CONST AVRS_VIDEO_CFG_T* videoCfg)
{
    // TODO Add the video configuration.
    AUD_ERR_T audErr;

    // Save the configuration
    // FIXME Some processing may depend on the previous
    // config.
    g_avrsVideoCfg    = *videoCfg;

    // Set decoder config
    // ...

    // Set audio interface
    g_avrsAudioCfg.spkLevel = AUD_SPK_MUTE;
    g_avrsAudioCfg.micLevel = audioCfg->micLevel;
    g_avrsAudioCfg.sideLevel = 0;
    g_avrsAudioCfg.toneLevel = 0;

    audErr = aud_Setup(audioCfg->itf, &g_avrsAudioCfg);
    switch (audErr)
    {
        // TODO Add error as they comes

        case AUD_ERR_NO:
            return AVRS_ERR_NO;
            break;

        default:
            return AVRS_ERR_QTY; // ...unknown error ...
            break;
    }
}


// =============================================================================
// avrs_Record
// -----------------------------------------------------------------------------
/// Start the recording of an audio and/or video stream buffer : When the
/// record reach the end of the buffer, it carries on from the beginning.
///
/// Events are sent to the mailbox recorded in the stream configuration, to
/// give accounts on how the recording behaves.
///
/// This function records an audio and/or a video stream. Only one stream (video
/// or audio) can be recorded. When one stream is not to be recorded, leave
/// the corresponding parameter (audioStream or videoStream) at the \c NULL
/// value. One of the stream must not be NULL !
///
/// Configuration of the streams are passed as the \c audioCfg or
/// \c videoCfg parameter. In the case where one of the stream is not
/// recorded, its configuration is ignored, and thus its configuration
/// parameter can be (should be) \c NULL.
///
/// @param audioStream Audio stream to record. If the pointer is NULL, only
/// a video stream will be record.
/// @param videoStream Video stream to record. If the pointer is NULL, only
/// an audio stream will be record.
/// @param audioCfg The configuration set applied to the audio recording. Can
/// only be NULL if audioStream is NULL too.
/// @param videoCfg The configuration set applied to the video recording. Can
/// only be NULL if videoStream is NULL too.
/// @return #AVRS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #AVRS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Record(CONST AVRS_ENC_AUDIO_STREAM_T*  audioStream,
                              CONST AVRS_ENC_VIDEO_STREAM_T*  videoStream,
                              CONST AVRS_AUDIO_CFG_T*         audioCfg,
                              CONST AVRS_VIDEO_CFG_T*         videoCfg)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_Record);

    // If a stream is currently being processed,
    // return the resource is busy.
    if (g_avrsAudioEnabled || g_avrsVideoEnabled)
    {
        AVRS_TRACE(AVRS_WARN_TRC, 0, "AVRS: avrs_Record failed for a stream is being recorded, "
                   "audio:%d, video:%d", g_avrsAudioEnabled, g_avrsVideoEnabled);
        AVRS_PROFILE_FUNCTION_EXIT(avrs_Record);
        return AVRS_ERR_RESOURCE_BUSY;
    }

    AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS Start");
    if (audioStream)
    {
        AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS audioStream - buffer %#x  %d codec %#x",
                   audioStream->startAddress, audioStream->length,
                   audioStream->mode);
    }

    // TODO
    // BIG switch for PCM and/or other codec
    // Initial audio configuration
    AUD_LEVEL_T audioLevel;
    AUD_ERR_T   audioErr = AUD_ERR_NO;
    AVRS_ERR_T  videoErr = AVRS_ERR_NO;
    HAL_AIF_STREAM_T pcmStream;

    // VPP Speech configuration.
    VPP_SPEECH_AUDIO_CFG_T vppSpeechAudioCfg;
    HAL_SPEECH_DEC_IN_T vppSpeechCodecCfg;

    // Video variables
    AVRS_ENCODED_PICTURE_T* encodedFramePtr     = NULL;
    AVRS_DECODED_PICTURE_T* previewPicturePtr   = NULL;

    // MJPEG codec configuration structure.
    VPP_AMJR_VIDEO_CFG_T amjrVideoCfg;

    UINT32 i;
    UINT16 scaleFactorX =0;
    UINT16 scaleFactorY =0;


    // Initializations
    g_avrsLatestEncodedFrame        = NULL;
    g_avrsLatestEncodedFrameLength  = 0;


    // Audio is configured first if there is an audio stream.
    // Video is then configured.
    // Depending on the combination of audio and video codec,
    // an encoder is chosen (For example, vpp_speech or
    // vpp_amjr) and then lauched.

    if (audioStream)
    {
        g_avrsAudioEnabled = TRUE;

        // Fill global variables
        g_avrsAudioStream = *audioStream;

        // Register global var
        g_avrsItf = audioCfg->itf;
        g_avrsBufferStart = (UINT32*) audioStream->startAddress;
        g_avrsBufferTop = (UINT32*)(audioStream->startAddress + audioStream->length);
        g_avrsBufferCurPos =(UINT32*) audioStream->startAddress;
        g_avrsBufferAvailBytes = 0;
        g_avrsBufferLength = audioStream->length;

        g_avrsTramePlayedNb = 0;
        g_avrsTrameDroppedNb = 0;
        g_avrsAudioMbx = audioStream->mbx;
        g_avrsBufferingMsgReceived = TRUE;

        g_avrsAudioCfg.spkLevel = AUD_SPK_MUTE;
        g_avrsAudioCfg.micLevel = audioCfg->micLevel;
        g_avrsAudioCfg.sideLevel = 0;
        g_avrsAudioCfg.toneLevel = 0;

        // TODO Improve ...
        g_avrsVppCfg.echoEsOn = 0;
        g_avrsVppCfg.echoEsVad = 0;
        g_avrsVppCfg.echoEsDtd = 0;
        g_avrsVppCfg.echoExpRel = 0;
        g_avrsVppCfg.echoExpMu = 0;
        g_avrsVppCfg.echoExpMin = 0;
        g_avrsVppCfg.encMute = VPP_SPEECH_UNMUTE;
        g_avrsVppCfg.decMute = VPP_SPEECH_MUTE;
        g_avrsVppCfg.sdf = NULL;
        g_avrsVppCfg.mdf = NULL;
        g_avrsVppCfg.if1 = 0;

        // Set audio interface
        audioLevel.spkLevel = AUD_SPK_MUTE;
        audioLevel.micLevel = audioCfg->micLevel;
        audioLevel.sideLevel = 0;
        audioLevel.toneLevel = 0;


        switch (audioStream->mode)
        {
            case AVRS_REC_AUDIO_MODE_MP3:
            case AVRS_REC_AUDIO_MODE_AMR_RING:
                g_avrsBufferStart  = (UINT32*)(audioStream->startAddress);
                g_avrsBufferTop    = (UINT32*)(audioStream->startAddress
                                               + audioStream->length);
                g_avrsBufferCurPos = (UINT32*)(g_avrsBufferCurPos);

                // Encoded buffers
                // FIXME use a size dependent on the channel number, bit rate,
                g_avrsEncBuf_0 = sxr_Malloc(1152*4);
                g_avrsEncBuf_1 = sxr_Malloc(1152*4);


                // HW event used to wakeup VoC
                pcmStream.halfHandler = NULL;
                pcmStream.endHandler  = NULL;

                // Other PCM configuration.
                pcmStream.sampleRate = audioStream->sampleRate;
                pcmStream.channelNb = audioStream->channelNb;
                pcmStream.voiceQuality = audioStream->voiceQuality;
                pcmStream.playSyncWithRecord = FALSE;

                // Audio Vpp cfg
                if (audioStream->mode == AVRS_REC_AUDIO_MODE_MP3)
                {
                    // PCM buffer size
                    pcmStream.length = 2*2*576*audioStream->channelNb;
                    if (audioStream->sampleRate > HAL_AIF_FREQ_16000HZ)
                    {
                        pcmStream.length *= 2;
                    }

                    // swap buffer start addresses
                    g_avrsPcmBuf_0 = &g_avrsPcmBuf[0];
                    g_avrsPcmBuf_1 = &g_avrsPcmBuf[(pcmStream.length/2)/sizeof(UINT32)];


                    g_avrsVppAmjrAudioCfg.mode = VPP_AMJR_AUDIO_MODE_MP3;

                    g_avrsVppAmjrAudioCfg.reset         = 1;
                    // Irrelevant for now:  g_avrsVppAmjrAudioCfg.eqType = cfg->equalizer;
                    g_avrsVppAmjrAudioCfg.inBufAddr = g_avrsPcmBuf_0;
                    g_avrsVppAmjrAudioCfg.outBufAddr     = g_avrsEncBuf_0;

                    g_avrsVppAmjrAudioCfg.sampleRate    = audioStream->sampleRate;

                    // Bit rate in k.
                    g_avrsVppAmjrAudioCfg.bitRate       = 128;
                    g_avrsVppAmjrAudioCfg.channelNum    = audioStream->channelNb;

                    // FIXME Discover what this is usable for:
                    g_avrsVppAmjrAudioCfg.ifcStatusPtr  = NULL;

                }
                else
                {
                    // PCM buffer size
                    pcmStream.length = 2*2*160; //2*160 samples (16 bits)
                    // swap buffer start addresses
                    g_avrsPcmBuf_0 = &g_avrsPcmBuf[0];
                    g_avrsPcmBuf_1 = &g_avrsPcmBuf[(pcmStream.length/2)/sizeof(UINT32)];

                    g_avrsVppAmjrAudioCfg.mode = VPP_AMJR_AUDIO_MODE_AMR;

                    g_avrsVppAmjrAudioCfg.reset         = 1;
                    // Irrelevant for now:  g_avrsVppAmjrAudioCfg.eqType = cfg->equalizer;
                    g_avrsVppAmjrAudioCfg.inBufAddr     = g_avrsPcmBuf_0;
                    g_avrsVppAmjrAudioCfg.outBufAddr    = g_avrsEncBuf_0;

                    g_avrsVppAmjrAudioCfg.sampleRate    = audioStream->sampleRate;

                    // Bit rate in k.
                    g_avrsVppAmjrAudioCfg.bitRate       = 12;
                    g_avrsVppAmjrAudioCfg.channelNum    = audioStream->channelNb;

                    // FIXME Discover what this is usable for:
                    g_avrsVppAmjrAudioCfg.ifcStatusPtr  = NULL;
                }

                // TODO Mv pcm stream cfg here
                pcmStream.startAddress  = g_avrsPcmBuf;
                break;


            case AVRS_REC_AUDIO_MODE_PCM:
                // Typical global vars
                g_avrsEncOutput = g_avrsPcmBuf;
// FIXME Do that properly:
// The copy is done nimportenawak dans le handler
// a refaire. Checker le encod buffer aussi
                g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_PCM_BUF;//sizeof(g_avrsPcmBuf)/2;

                // PCM Audio stream, output of VPP
                pcmStream.startAddress = g_avrsPcmBuf; // == g_avrsEncOutput
                // Swap buffer of HAL_SPEECH_FRAME_SIZE_PCM_BUF
                pcmStream.length = 2 * HAL_SPEECH_FRAME_SIZE_PCM_BUF; // sizeof(g_avrsPcmBuf);
                pcmStream.sampleRate = audioStream->sampleRate;
                pcmStream.channelNb = audioStream->channelNb;
                pcmStream.voiceQuality = audioStream->voiceQuality;
                pcmStream.playSyncWithRecord = FALSE;
                pcmStream.halfHandler = avrs_XXXPcmHandler;
                pcmStream.endHandler =  avrs_XXXPcmHandler;
                break;

            default:
                // AMR mode

                // VPP Speech audio config
                vppSpeechAudioCfg.echoEsOn = 0;
                vppSpeechAudioCfg.echoEsVad = 0;
                vppSpeechAudioCfg.echoEsDtd = 0;
                vppSpeechAudioCfg.echoExpRel = 0;
                vppSpeechAudioCfg.echoExpMu = 0;
                vppSpeechAudioCfg.echoExpMin = 0;
                vppSpeechAudioCfg.encMute = VPP_SPEECH_MUTE;
                vppSpeechAudioCfg.decMute = VPP_SPEECH_MUTE;
                vppSpeechAudioCfg.sdf = NULL;
                vppSpeechAudioCfg.mdf = NULL;
                vppSpeechAudioCfg.if1 = 0;

                // VPP Speech codec structure config
                vppSpeechCodecCfg.dtxOn = 0;
                vppSpeechCodecCfg.decFrameType = 0;
                vppSpeechCodecCfg.bfi = 0;
                vppSpeechCodecCfg.sid = 0;
                vppSpeechCodecCfg.taf = 0;
                vppSpeechCodecCfg.ufi = 0;

                // Specific config
                switch (audioStream->mode)
                {
                    case AVRS_REC_AUDIO_MODE_AMR475:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR475;
                        vppSpeechCodecCfg.codecMode = HAL_AMR475_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR515:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR515;
                        vppSpeechCodecCfg.codecMode = HAL_AMR515_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR59:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR59;
                        vppSpeechCodecCfg.codecMode = HAL_AMR59_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR67:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR67 ;
                        vppSpeechCodecCfg.codecMode = HAL_AMR67_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR74:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR74 ;
                        vppSpeechCodecCfg.codecMode = HAL_AMR74_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR795:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR795;
                        vppSpeechCodecCfg.codecMode = HAL_AMR795_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR102:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR102;
                        vppSpeechCodecCfg.codecMode = HAL_AMR102_ENC;
                        break;

                    case AVRS_REC_AUDIO_MODE_AMR122:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR122;
                        vppSpeechCodecCfg.codecMode = HAL_AMR122_ENC & HAL_AMR122_DEC;
                        break;

//                    case AVRS_REC_AUDIO_MODE_AMR_RING:
//                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR_RING;
//                        vppSpeechCodecCfg.codecMode = HAL_AMR122_ENC & HAL_AMR122_DEC;
//                        // Set the AMR_RING flag in both temporarily
//                        // muted configuration and in cruise mode
//                        // configuration.
//                        vppSpeechAudioCfg.if1 = 1;
//                        g_avrsVppCfg.if1 = 1;
//                        break;

                    case AVRS_REC_AUDIO_MODE_EFR:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_EFR  ;
                        vppSpeechCodecCfg.codecMode = HAL_EFR;
                        break;

                    case AVRS_REC_AUDIO_MODE_HR:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_HR ;
                        vppSpeechCodecCfg.codecMode = HAL_HR;
                        break;

                    case AVRS_REC_AUDIO_MODE_FR:
                        g_avrsFrameSize = HAL_SPEECH_FRAME_SIZE_FR;
                        vppSpeechCodecCfg.codecMode = HAL_FR;
                        break;

                    default:
                        AVRS_ASSERT(FALSE, "Unsupported mode in AVRS");
                }

                // Pointer from which to get the encoded data.
                g_avrsEncOutput = (UINT32*) ((HAL_SPEECH_ENC_OUT_T*)vpp_SpeechGetTxCodBuffer()->encOutBuf);

                // PCM Audio stream, input of VPP
                pcmStream.startAddress = (UINT32*)vpp_SpeechGetTxPcmBuffer();
                pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
                pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
                pcmStream.channelNb = HAL_AIF_MONO;
                pcmStream.voiceQuality = TRUE;
                pcmStream.playSyncWithRecord = FALSE;
                pcmStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
                pcmStream.endHandler = NULL; // Mechanical interaction with VPP's VOC
                break;
        }
    }

    // Video management
    if (videoStream != NULL)
    {
        AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video Stream On");

        g_avrsVideoEnabled = TRUE;

        // Fill global variables
        g_avrsVideoStream = *videoStream;

        // Save the configuration
        g_avrsVideoCfg    = *videoCfg;

        // Buffer configuration (for the handler)
        for (i=0 ; i< AVRS_PICTURE_STREAM_DEPTH ; i++)
        {
            // Frame buffer
            // 16 bits per pixel
            g_avrsFbwStream[i].fb.buffer = sxr_Malloc(videoStream->previewFbw->fb.width*
                                           videoStream->previewFbw->fb.height*
                                           2);
            g_avrsFbwStream[i].fb.width  = videoStream->previewFbw->fb.width ;
            g_avrsFbwStream[i].fb.height = videoStream->previewFbw->fb.height;
            g_avrsFbwStream[i].fb.colorFormat = videoStream->previewFbw->fb.colorFormat;

            g_avrsFbwStream[i].roi.x = videoStream->previewFbw->roi.x;
            g_avrsFbwStream[i].roi.y = videoStream->previewFbw->roi.y;
            g_avrsFbwStream[i].roi.width = videoStream->previewFbw->roi.width;
            g_avrsFbwStream[i].roi.height = videoStream->previewFbw->roi.height;

            // Encoded buffer
            g_avrsEncBufferStream[i] = ((UINT8*)videoStream->startAddress)
                                       + i*(videoStream->length/AVRS_PICTURE_STREAM_DEPTH);
            AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video Encoded Buffer[%d]: %#x.",
                       i, g_avrsEncBufferStream[i]);
        }

        // Preview
        g_avrsPreviewCfg.fbw        = videoStream->previewFbw;
        g_avrsPreviewCfg.rotation   = videoCfg->rotation;
        g_avrsPreviewCfg.callback   = videoStream->previewCallback;

        if (g_avrsPreviewCfg.callback && g_avrsPreviewCfg.fbw)
        {
            // We display a preview
            previewPicturePtr = &g_avrsPreviewCfg;
            AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video preview enabled.");
        }
        else
        {
            previewPicturePtr = NULL;
            AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video preview disabled.");
        }

        // Encoder
        g_avrsVideoFrameCfg.buffer     = (UINT8*) videoStream->startAddress;
        g_avrsVideoFrameCfg.rotation   = videoCfg->rotation;

        switch (videoStream->mode)
        {
            case AVRS_REC_VIDEO_MODE_MJPEG:
                g_avrsVideoFrameCfg.encCfg.format = IMGS_IMG_FORMAT_JPG;
                g_avrsVideoFrameCfg.encCfg.jpg.quality = IMGS_JPG_QUALITY_HIGH;

                // Configure here the codec
                amjrVideoCfg.mode           = VPP_AMJR_VIDEO_MODE_JPEG;
                amjrVideoCfg.format         = VPP_AMJR_BMP_RGB; // FIXME Get that from the camera driver ?
                amjrVideoCfg.quality        = (videoStream->lowQuality?VPP_AMJR_JPEG_QUALITY_NORMAL:VPP_AMJR_JPEG_QUALITY_SUPERIOR);
                amjrVideoCfg.pxlSwap        = VPP_AMJR_PXL_ORDER_SWAP;

                // Configure the preview
                if (previewPicturePtr)
                {
                    // Parameter set to the real value
                    // on decoding scheduling.
                    amjrVideoCfg.previewBufAddr = NULL; // (UINT32*)videoStream->previewFbw->fb.buffer;
                    amjrVideoCfg.previewWidth   = g_avrsPreviewCfg.fbw->roi.width;
                    amjrVideoCfg.previewHeight  = g_avrsPreviewCfg.fbw->roi.height;


                }
                else
                {
                    amjrVideoCfg.previewBufAddr = NULL;
                    amjrVideoCfg.previewWidth   = 0;
                    amjrVideoCfg.previewHeight  = 0;
                }

                // Video stream configuration
                amjrVideoCfg.inBufAddr      = NULL;
                amjrVideoCfg.outBufAddr     = NULL;

                // FIXME Handle rotation
                switch(g_avrsVideoStream.resolution)
                {
                    case AVRS_REC_VIDEO_RESOLUTION_QQVGA:
                        amjrVideoCfg.width          = 160;
                        amjrVideoCfg.height         = 120;
                        break;

                    case AVRS_REC_VIDEO_RESOLUTION_QVGA:
                        amjrVideoCfg.width          = 320;
                        amjrVideoCfg.height         = 240;
                        break;
                }

                // Calculate Preview Scale Factor
                scaleFactorX = ((videoCfg->zoom * amjrVideoCfg.previewWidth) / amjrVideoCfg.width) ;
                scaleFactorY = ((videoCfg->zoom * amjrVideoCfg.previewHeight)/ amjrVideoCfg.height) ;
                amjrVideoCfg.previewScaleFactor =  (scaleFactorX > scaleFactorY)? scaleFactorX: scaleFactorY;


                // Handler called when a frame has been encoded.
                g_avrsVideoFrameCfg.callback = avrs_VideoHandler;


                break;

            default:
                AVRS_ASSERT(FALSE, "Unsupported video mode %d", videoStream->mode);
        }

        if (g_avrsVideoFrameCfg.buffer && g_avrsVideoFrameCfg.callback)
        {
            // We want to encode the video stream.
            AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video encoding enabled.");
            encodedFramePtr = &g_avrsVideoFrameCfg;
        }
        else
        {
            // we don't want to encode the video stream,
            // the preview is (probably) all that we need.
            AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video encoding disabled.");
            encodedFramePtr = NULL;
        }
    }
    else
    {
        // No video
        AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS: Video Stream Off");
        g_avrsVideoEnabled = FALSE;
    }


    // Open (and configure) the proper encoder
    // depending on the requirement about audio
    // and video.

    // Use the speech codec when recording any
    // speech mode with no video.
    if (g_avrsVideoStream.mode == AVRS_REC_VIDEO_MODE_NONE
            &&
            (  g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR475
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR515
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR59
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR67
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR74
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR795
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR102
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR122
//        || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR_RING
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_EFR
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_HR
               || g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_FR))
    {
        // set vpp
        if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(avrs_VppSpeechHandler,
                VPP_SPEECH_WAKEUP_HW_ENC))
        {
            return AVRS_ERR_RESOURCE_BUSY;
        }

        vpp_SpeechAudioCfg(&vppSpeechAudioCfg);
        vpp_SpeechCodecCfg(&vppSpeechCodecCfg);

        // Set all the preprocessing modules
        vpp_SpeechSetEncDecPocessingParams(g_avrsItf, g_avrsAudioCfg.spkLevel, 0);
    }

    else
        // MJPEG recording and/or (MP3 or AMR RING audio)
        if ((g_avrsVideoStream.mode == AVRS_REC_VIDEO_MODE_NONE && g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR_RING)
                || (g_avrsVideoStream.mode == AVRS_REC_VIDEO_MODE_MJPEG && g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_AMR_RING)
                || (g_avrsVideoStream.mode == AVRS_REC_VIDEO_MODE_NONE && g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_MP3)
                || (g_avrsVideoStream.mode == AVRS_REC_VIDEO_MODE_MJPEG && g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_MP3)
                || (g_avrsVideoStream.mode == AVRS_REC_VIDEO_MODE_MJPEG && g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_NONE))
        {
            // Open Amjr
            // Only wake-up on the audio hardware interrupt: scheduling the encode of
            // half a buffer worth of data at the mid-period time seems
            // a good compromise.
            if (HAL_ERR_RESOURCE_BUSY == vpp_AmjrOpen(avrs_VppAmjrHandler, VPP_AMJR_WAKEUP_HW_AUDIO_SW_VIDEO))
            {
                return AVRS_ERR_RESOURCE_BUSY;
            }

            // Configure picture stream
            vpp_AmjrVideoCfg(&amjrVideoCfg);
        }
        else
            // RAW PCM
            if (g_avrsAudioStream.mode == AVRS_REC_AUDIO_MODE_PCM)
            {
                // Nothing to start
            }
            else
            {
                // Unsupported combination
                AVRS_ASSERT(FALSE, "Unsupported audio %d / video %d combination:",
                            g_avrsAudioStream.mode, g_avrsVideoStream.mode);
            }
    // Otherwise, unsupported combination ...



    // Start the video stream
    if (g_avrsVideoEnabled)
    {
        videoErr = avrs_StartPictureStream(previewPicturePtr, encodedFramePtr,
                                           &g_avrsFbwStream, &g_avrsEncBufferStream);
    }

    // Start the audio stream, as video started properly
    // (If there is no video stream, videoErr is at its
    // initialization value, ie AVRS_ERR_NO.
    if (g_avrsAudioEnabled && (videoErr == AVRS_ERR_NO))
    {
        audioErr = aud_StreamRecord(audioCfg->itf, &pcmStream, &audioLevel);

        // profit
        switch (audioErr)
        {
            case AUD_ERR_NO:
                AVRS_PROFILE_FUNCTION_EXIT(avrs_Record);
                return AVRS_ERR_NO;
                break; // :)

            default:
                AVRS_TRACE(AVRS_WARN_TRC, 0, "AVRS Start failed !!!");
                AVRS_PROFILE_FUNCTION_EXIT(avrs_Record);
                return AVRS_ERR_UNKNOWN;
                break; // :)
        }
    }
    else
    {
        // Starting the video stream triggered an error.
        AVRS_PROFILE_FUNCTION_EXIT(avrs_Record);
        return videoErr;
    }
}



// =============================================================================
// avrs_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio and video record.
/// If the function returns
/// #AVRS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @todo Implement a way to recover the content of the buffer when stop is
/// requested, in cooperation with MRS.
///
/// @return #AVRS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVRS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_Stop(VOID)
{
    /// Stop the audio stream if any, then send the stop
    /// message to the task handling the video. When the
    /// video management is over, the task will call
    /// the #avrs_FreeStreams() function to release and
    /// reset the global variables describing the streams.
    AVRS_PROFILE_FUNCTION_ENTER(avrs_Stop);

    // Clean the audio stream.
    if (g_avrsAudioEnabled)
    {
        // stop stream ...
        aud_StreamStop(g_avrsItf);
        // FIXME Check we need that.
        vpp_SpeechClose();
    }

    // Stop the video streaming (In AVRS task)
    // Once stopped, avrs_FreeStreams() will finish
    // the cleaning.
    avrs_StopPictureStream();

    AVRS_PROFILE_FUNCTION_EXIT(avrs_Stop);
    return AVRS_ERR_NO;
}


// =============================================================================
// avrs_FreeStreams
// -----------------------------------------------------------------------------
/// This function frees the global variables used in the streams (video and
/// audio) management.
/// Call it only after both video and audio related processes are finished,
/// and none of the involved variables are still in use.
///
/// @return #AVRS_ERR_NO if it can execute the command.
// =============================================================================
PROTECTED AVRS_ERR_T avrs_FreeStreams(VOID)
{
    UINT32 i;
    AVRS_PROFILE_FUNCTION_ENTER(avrs_FreeStreams);

    // Clean the audio stream.
    if (g_avrsAudioEnabled)
    {
        // and reset global state
        g_avrsAudioEnabled = FALSE;

        g_avrsItf = 0;
        g_avrsBufferStart = NULL;
        g_avrsBufferTop =  NULL;
        g_avrsBufferCurPos = NULL;
        g_avrsBufferAvailBytes = 0;
        g_avrsBufferLength = 0;

        g_avrsTramePlayedNb = 0;
        g_avrsTrameDroppedNb = 0;
        g_avrsAudioMbx = 0xFF;
        g_avrsBufferingMsgReceived = TRUE;

        g_avrsAudioCfg.spkLevel = AUD_SPK_MUTE;
        g_avrsAudioCfg.micLevel = AUD_MIC_MUTE;
        g_avrsAudioCfg.sideLevel = 0;
        g_avrsAudioCfg.toneLevel = 0;

        // TODO Improve ...
        g_avrsVppCfg.echoEsOn = 0;
        g_avrsVppCfg.echoEsVad = 0;
        g_avrsVppCfg.echoEsDtd = 0;
        g_avrsVppCfg.echoExpRel = 0;
        g_avrsVppCfg.echoExpMu = 0;
        g_avrsVppCfg.echoExpMin = 0;
        g_avrsVppCfg.encMute = VPP_SPEECH_MUTE;
        g_avrsVppCfg.decMute = VPP_SPEECH_MUTE;
        g_avrsVppCfg.sdf = NULL;
        g_avrsVppCfg.mdf = NULL;
        g_avrsVppCfg.if1 = 0;

        g_avrsFrameSize = 0;

        g_avrsAudioStream.startAddress       = 0;
        g_avrsAudioStream.length             = 0;
        g_avrsAudioStream.mode               = 0;
        g_avrsAudioStream.sampleRate         = 0;
        g_avrsAudioStream.channelNb          = HAL_AIF_MONO;
        g_avrsAudioStream.voiceQuality       = FALSE;
        g_avrsAudioStream.mbx                = 0xFF;

        if (g_avrsEncBuf_0)
        {
            sxr_Free(g_avrsEncBuf_0);
            g_avrsEncBuf_0 = NULL;
        }

        if (g_avrsEncBuf_1)
        {
            sxr_Free(g_avrsEncBuf_1);
            g_avrsEncBuf_1 = NULL;
        }
    }

    // Clean the video stream
    if (g_avrsVideoEnabled)
    {
        // FIXME Check the mode to know what to close
        vpp_AmjrClose();
        g_avrsVideoEnabled = FALSE;
        g_avrsVideoStream.startAddress      = 0;
        g_avrsVideoStream.length            = 0;
        g_avrsVideoStream.mode              = AVRS_REC_VIDEO_MODE_NONE;
        g_avrsVideoStream.frameRate         = 0;
        g_avrsVideoStream.lowQuality        = FALSE;
        g_avrsVideoStream.mbx               = 0xFF;
        g_avrsVideoStream.previewFbw        = NULL;
        g_avrsVideoStream.previewCallback   = NULL;

        g_avrsLatestEncodedFrame            = NULL;
        g_avrsLatestEncodedFrameLength      = 0;

        for (i=0 ; i< AVRS_PICTURE_STREAM_DEPTH ; i++)
        {
            // Frame buffer
            // 16 bits per pixel
            if (g_avrsFbwStream[i].fb.buffer != NULL)
            {
                sxr_Free(g_avrsFbwStream[i].fb.buffer);
                g_avrsFbwStream[i].fb.buffer = NULL;
            }
            g_avrsFbwStream[i].fb.width  = 0;
            g_avrsFbwStream[i].fb.height = 0;
            g_avrsFbwStream[i].fb.colorFormat = 0;

            g_avrsFbwStream[i].roi.x = 0;
            g_avrsFbwStream[i].roi.y = 0;
            g_avrsFbwStream[i].roi.width = 0;
            g_avrsFbwStream[i].roi.height = 0;

            // Encoded buffer
            g_avrsEncBufferStream[i] = NULL;
        }
    }

    AVRS_PROFILE_FUNCTION_EXIT(avrs_FreeStreams);
    return AVRS_ERR_NO;
}


// =============================================================================
// avrs_ReadData
// -----------------------------------------------------------------------------
/// When a stream buffer is recorded, already recorded data can be replaced by new
/// ones to record a long song seamlessly. The AVRS service needs to be informed
/// about the number of new bytes available in the buffer. This is the role of the
/// #avrs_ReadData function.
///
/// If the function returns
/// #AVRS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param readDataBytes Number of bytes read from the buffer.
/// @return #AVRS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #AVRS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC AVRS_ERR_T avrs_ReadData(UINT32 readDataBytes)
{
    AVRS_PROFILE_FUNCTION_ENTER(avrs_ReadData);
    UINT32 status = hal_SysEnterCriticalSection();
    // nota : g_avrsBufferAvailBytes is the number of bytes in the
    // buffer available for reading
    g_avrsBufferAvailBytes -= readDataBytes;
    AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS, read data: %d less", readDataBytes);
    AVRS_TRACE(AVRS_INFO_TRC, 0, "AVRS, available data: %d", g_avrsBufferAvailBytes);
    g_avrsBufferingMsgReceived = TRUE;
    hal_SysExitCriticalSection(status);
    AVRS_PROFILE_FUNCTION_EXIT(avrs_ReadData);
    return AVRS_ERR_NO;
}



