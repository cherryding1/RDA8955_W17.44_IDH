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




#include "avpsp_decode_amjp.h"
#include "avpsp.h"

#include "aud_m.h"

#include "sxr_ops.h"
#include "sxs_io.h"

#include "hal_debug.h"

#include "vpp_amjp.h"
#include "hal_voc.h"


#include <string.h>



// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// GLOBAL VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE MPS_DECODE_TEST_RESULT_T avps_decodeAmjpTest(CONST AVPS_ENC_STREAM_T* stream);

PRIVATE VOID avps_decodeAmjpStop(VOID);

PRIVATE AVPS_ERR_T avps_decodeAmjpPlay(CONST AVPS_ENC_STREAM_T*       stream,
                                       CONST AVPS_CFG_T*              cfg,
                                       CONST LCDD_FBW_T*              frameWindows,
                                       HAL_AIF_STREAM_T*              pcmStream);

PRIVATE AVPS_ERR_T avps_decodeAmjpSetup(CONST AVPS_CFG_T*              cfg);

// =============================================================================
// PRIVATE VARIABLES
// =============================================================================

PROTECTED AVPS_DECODE_T g_avpsAmjpDecode =
{
    .test  = avps_decodeAmjpTest,
    .play  = avps_decodeAmjpPlay,
    .stop  = avps_decodeAmjpStop,
    .setup = avps_decodeAmjpSetup
};

/// VPP audio configuration used to play multimedia stream in normal state.
/// (ie not when started or stopped)
PRIVATE VPP_AMJP_AUDIO_CFG_T g_avpsVppAmjpAudioCfg;

/// VPP video configuration used to play multimedia stream in normal state.
/// (ie not when started or stopped)
PRIVATE VPP_AMJP_VIDEO_CFG_T g_avpsVppAmjpVideoCfg;

// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// avps_VppAmjpHandler
// -----------------------------------------------------------------------------
// =============================================================================

PROTECTED VOID avps_VppAmjpHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    UINT32              addr;
    VPP_AMJP_STATUS_T   vppStatus;
    INT32               readBytes;
    BOOL                decodeVideo = FALSE;
    Msg_t               msg;

    AVPS_PROFILE_FUNCTION_ENTER(avps_Handler);

    // get status
    vpp_AmjpStatus(&vppStatus);

    AVPS_TRACE(TSTDOUT, 0, "VPP STREAM STATUS %#x %#x", vppStatus.strmStatus, vppStatus.outStreamIfcStatus);
    // schedule everything on audio
    if ((vppStatus.strmStatus == VPP_AMJP_STRM_ID_AUDIO) ||
            (vppStatus.strmStatus == VPP_AMJP_STRM_ID_INIT))
    {
        AVPS_TRACE(TSTDOUT, 0, "!!!!!!!! handler %i %i %#x", g_avpsFramesPlayedNb,
                   g_avpsAudioBufferRemainingBytes, g_avpsAudioBufferCurPos);

        memcpy(g_avpsAudioBars, vppStatus.displayBars, sizeof(vppStatus.displayBars));

        if (g_avpsFramesPlayedNb == AVPS_AUD_UNMUTE_TRAME_NB)
        {
            // Unmute AUD
            AVPS_TRACE(_PAL| TLEVEL (1), 0, "AUD unmuted");
            aud_Setup(g_avpsItf, &g_avpsAudioCfg);
        }

        if (g_avpsAudioBufferRemainingBytes < 0)
        {
            AVPS_TRACE(TSTDOUT, 0, "!!!!!!!! remainingbyte=%i", g_avpsAudioBufferRemainingBytes);

            // Ask for more data if:
            // - there is a mailbox to post the request in
            // - the same message has not already been sent
            // - this is not the End of the File to play (in which
            // case there is no more data to expect !);
            if (g_avpsAudioBufferingMsgReceived)
            {
                addr             = (UINT32)g_avpsAudioBufferCurPos + g_avpsAudioBufferRemainingBytes;
                if((UINT32)addr < (UINT32)g_avpsAudioBufferStart)
                {
                    addr += (UINT32)g_avpsAudioBufferTop - (UINT32)g_avpsAudioBufferStart;
                }
                if((UINT32)addr >= (UINT32)g_avpsAudioBufferTop)
                {
                    addr -= (UINT32)g_avpsAudioBufferTop - (UINT32)g_avpsAudioBufferStart;
                }
                // We NEED data, urgently !
                msg.H.Id        = AVPS_HEADER_ID_AUDIO;
                msg.B.bufferPos = (VOID*)(addr | 0x80000000);
                g_avpsAudioBufferingMsgReceived = FALSE;
                sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
                AVPS_TRACE(TSTDOUT, 0, "!!!!@@@ MSG SEND remainingbyte=%i EOF", g_avpsAudioBufferRemainingBytes);
            }


            // Stop the play
            avps_Stop(0);

            // Leave
            return;
        }

        if (g_avpsFramesPlayedNb++)
        {
            // check for errors
            if (vppStatus.errorStatus)
            {
                AVPS_TRACE(TSTDOUT, 0, "AVPS: error status %i", vppStatus.errorStatus);

                /*                 if (g_avpsErrNb++ > AVPS_ERR_NB_MAX) */
                /*                 { */
                /*                     // Stop the play */
                /*                     avps_Stop(0); */

                /*                     AVPS_TRACE(TSTDOUT, 0, "AVPS: too many errors"); */
                /*                    // Leave */
                /*                     return; */
                /*                 } */
            }

            // compute the new remaining size
            readBytes = (INT32)vppStatus.inStreamBufAddr - (INT32)g_avpsAudioBufferCurPos;
            if (readBytes < 0)
            {
                readBytes += g_avpsAudioBufferLength;
            }

            // update the stream buffer status
            g_avpsAudioBufferRemainingBytes -= readBytes;

            // update read pointer
            g_avpsAudioBufferCurPos = vppStatus.inStreamBufAddr;
        }

        // call the user handler if new data is necessary
        if (g_avpsAudioBufferRemainingBytes <= (g_avpsAudioBufferLength/2))
        {
            if (g_avpsAudioBufferingMsgReceived)
            {
                msg.H.Id        = AVPS_HEADER_ID_AUDIO;
                msg.B.bufferPos = (VOID*)((UINT32)g_avpsAudioBufferCurPos | 0x80000000);
                g_avpsAudioBufferingMsgReceived = FALSE;
                sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);

                AVPS_PROFILE_PULSE(avps_fetchAudio);
                AVPS_TRACE(TSTDOUT, 0, "!!!!@@@ MSG SEND remainingbyte=%i", g_avpsAudioBufferRemainingBytes);
            }
        }

        // Schedule frame decoding on some frame number conditions
        if ((g_avpsFramesPlayedNb >= (g_avpsNextVideoFramePlayed>>8)) &&
                (g_avpsVideoEncodedBuffer != NULL))
        {
            g_avpsNextVideoFramePlayed += g_avpsAudioVideoRatio;

            if(g_avpsVideoBufferingMsgReceived == FALSE)
            {
                AVPS_PROFILE_PULSE(avps_skipVideo);
                // No data receive skip one frame
                msg.H.Id        = AVPS_HEADER_ID_VIDEO_SKIP;
                msg.B.bufferPos = (UINT32*)(g_avpsVideoEncodedBuffer
                                            +  g_avpsVideoEncodedIndex * (g_avpsVideoEncodedBufferLength/2));
                sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
                AVPS_TRACE(TSTDOUT, 0, "avps: Skip video frame");
            }
            else
            {
                g_avpsVideoEncodedIndex = (g_avpsVideoEncodedIndex) ? 0 : 1;
                // Need a full buffer at the beginning !
                // Configure decoding for this side of the frame buffer
                g_avpsVppAmjpVideoCfg.inVidBufAddrStart =
                    hal_VocGetDmaiPointer((UINT32*)(g_avpsVideoEncodedBuffer
                                                    + g_avpsVideoEncodedIndex * (g_avpsVideoEncodedBufferLength/2)),
                                          HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
                g_avpsVppAmjpVideoCfg.inVidBufAddrEnd =
                    hal_VocGetDmaiPointer((UINT32*)(g_avpsVideoEncodedBuffer
                                                    + g_avpsVideoEncodedBufferLength),
                                          HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

                g_avpsVppAmjpVideoCfg.outVidBufAddr =
                    hal_VocGetDmaiPointer((UINT32*)g_avpsDecodedVideoFbw[g_avpsVideoDecodedIndex].fb.buffer,
                                          HAL_VOC_DMA_WRITE,HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
                decodeVideo = TRUE;
                // Ask for data for the other half of the buffer

                AVPS_PROFILE_WINDOW_ENTER(avps_videoDecode0+g_avpsVideoDecodedIndex);
                g_avpsVideoDecodedIndex = (g_avpsVideoDecodedIndex) ? 0 : 1;

                msg.H.Id        = AVPS_HEADER_ID_VIDEO;
                msg.B.bufferPos = (UINT32*)(g_avpsVideoEncodedBuffer
                                            +  g_avpsVideoEncodedIndex * (g_avpsVideoEncodedBufferLength/2));
                g_avpsVideoBufferingMsgReceived = FALSE;
                sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
                AVPS_PROFILE_PULSE(avps_fetchVideo0+g_avpsVideoEncodedIndex);
                AVPS_TRACE(TSTDOUT, 0, "======> MSG SEND Refill video buffer %i", g_avpsVideoEncodedIndex);
            }
        }


        // use swap buffer
        if((((UINT32)vppStatus.outStreamIfcStatus)&0xFFFFFF) >= (((UINT32)g_avpsPcmBuf0)&0xFFFFFF) &&
                (((UINT32)vppStatus.outStreamIfcStatus)&0xFFFFFF) <  (((UINT32)g_avpsPcmBuf1)&0xFFFFFF))
        {
            g_avpsVppAmjpAudioCfg.outStreamBufAddr = g_avpsPcmBuf0;
        }
        else
        {
            g_avpsVppAmjpAudioCfg.outStreamBufAddr = g_avpsPcmBuf1;
        }

        // configure next frame
        AVPS_TRACE(TSTDOUT, 0, "Configure next frame, decode video :%d", decodeVideo);

        // TODO : these cfg need to be executed only when cfg parameters other than the inout buffers are updated
        vpp_AmjpAudioCfg(&g_avpsVppAmjpAudioCfg);
        vpp_AmjpVideoCfg(&g_avpsVppAmjpVideoCfg);

        vpp_AmjpScheduleOneAudioFrame(g_avpsVppAmjpAudioCfg.outStreamBufAddr);

        if (decodeVideo)
        {
            vpp_AmjpScheduleOneVideoFrame(g_avpsVppAmjpVideoCfg.inVidBufAddrStart, g_avpsVppAmjpVideoCfg.outVidBufAddr);
        }
    }


    // If Video Interrupt: request display
    else if (vppStatus.strmStatus == VPP_AMJP_STRM_ID_VIDEO)
    {
        AVPS_TRACE(TSTDOUT, 0, "Let's ROCKKKKKKKK, mbx = %d, encodedbuf?%#x", g_avpsUserMbx, g_avpsVideoEncodedBuffer);
        AVPS_PROFILE_WINDOW_EXIT(avps_videoDecode0+g_avpsVideoDecodedDisplayIndex);

        if (g_avpsVideoEncodedBuffer != NULL)
        {
            msg.H.Id        = AVPS_HEADER_ID_VIDEO_DISPLAY;
            msg.B.bufferPos = (VOID*)&g_avpsDecodedVideoFbw[g_avpsVideoDecodedDisplayIndex];
            sxr_Send(&msg, g_avpsUserMbx, SXR_SEND_EVT);
            AVPS_TRACE(TSTDOUT, 0, "======> MSG SEND DISPLAY  video %i", g_avpsVideoDecodedDisplayIndex);
            // Swap Frame buffer window.
            g_avpsVideoDecodedDisplayIndex = (g_avpsVideoDecodedDisplayIndex) ? 0 : 1;
        }

    } // vppStatus.strmStatus



    // profile
    AVPS_PROFILE_FUNCTION_EXIT(avps_Handler);
}


PRIVATE MPS_DECODE_TEST_RESULT_T avps_decodeAmjpTest(CONST AVPS_ENC_STREAM_T* stream)
{
    MPS_DECODE_TEST_RESULT_T result =  MPS_DECODE_TEST_RESULT_NO;

    switch(stream->audioMode)
    {
        case AVPS_PLAY_AUDIO_MODE_AAC:
        case AVPS_PLAY_AUDIO_MODE_MP3:
        case AVPS_PLAY_AUDIO_MODE_AMR_RING:
            result |= MPS_DECODE_TEST_RESULT_AUDIO;
            break;
        default:
            return result;
    }

    switch(stream->videoMode)
    {
        case AVPS_PLAY_VIDEO_MODE_MJPEG:
        case AVPS_PLAY_VIDEO_MODE_H263:
        case AVPS_PLAY_VIDEO_MODE_MPEG4:
            result |= MPS_DECODE_TEST_RESULT_VIDEO;
            break;
        default:
            break;
    }


    return result;
}

PRIVATE VOID avps_decodeAmjpStop(VOID)
{
    vpp_AmjpClose();

    // Clear video stuff
    if (g_avpsVideoEncodedBuffer != NULL)
    {
        g_avpsVideoEncodedBuffer = NULL;
        g_avpsVideoEncodedBufferLength = 0;
        g_avpsVideoEncodedIndex = 0;
        g_avpsVideoDecodedIndex = 0;
        if (g_avpsDecodedVideoFbw[0].fb.buffer != NULL)
        {
            sxr_Free(g_avpsDecodedVideoFbw[0].fb.buffer);
            g_avpsDecodedVideoFbw[0].fb.buffer = NULL;
        }

        if (g_avpsDecodedVideoFbw[1].fb.buffer != NULL)
        {
            sxr_Free(g_avpsDecodedVideoFbw[1].fb.buffer);
            g_avpsDecodedVideoFbw[1].fb.buffer = NULL;
        }
    }
}

PRIVATE AVPS_ERR_T avps_decodeAmjpSetup(CONST AVPS_CFG_T*              cfg)
{
    // Set default audio config
    g_avpsVppAmjpAudioCfg.eqType           = cfg->equalizer;

    // set default video config
    g_avpsVppAmjpVideoCfg.rotateMode       = VPP_AMJP_ROTATE_NO;
    if(cfg->videoRotation == AVPS_VIDEO_ROTATION_LANDSCAPE)
    {
        g_avpsVppAmjpVideoCfg.rotateMode       = VPP_AMJP_ROTATE_90;
    }
    g_avpsVppAmjpVideoCfg.resizeToWidth        = 0;
    if(g_avpsVppAmjpVideoCfg.rotateMode == VPP_AMJP_ROTATE_NO ||
            g_avpsVppAmjpVideoCfg.rotateMode == VPP_AMJP_ROTATE_180)
    {
        g_avpsVppAmjpVideoCfg.resizeToWidth    = g_avpsDecodedVideoFbw[0].fb.width;
    }

    return AVPS_ERR_NO;
}

PRIVATE AVPS_ERR_T avps_decodeAmjpPlay(CONST AVPS_ENC_STREAM_T*       stream,
                                       CONST AVPS_CFG_T*              cfg,
                                       CONST LCDD_FBW_T*              frameWindows,
                                       HAL_AIF_STREAM_T*              pcmStream)
{
    LCDD_SCREEN_INFO_T     screenInfo;

    if(stream->videoMode != AVPS_PLAY_VIDEO_MODE_NO)
    {
        lcdd_GetScreenInfo(&screenInfo);

        // Initialize the video variables
        g_avpsVideoEncodedBuffer       = (VOID*)stream->videoAddress;
        g_avpsVideoEncodedBufferLength = stream->videoLength;
        g_avpsVideoEncodedIndex        = 0;
        g_avpsVideoDecodedIndex        = 0;
        g_avpsVideoDecodedDisplayIndex = 0;

        AVPS_TRACE(TSTDOUT, 0, "======> g_avpsVideoEncodedBuffer = %#x", g_avpsVideoEncodedBuffer);
        AVPS_TRACE(TSTDOUT, 0, "======> g_avpsVideoEncodedBufferLength = %#d", g_avpsVideoEncodedBufferLength);

        AVPS_ASSERT(frameWindows, "AVPS play: video and frameWindows = 0");
        AVPS_ASSERT(frameWindows->fb.width,   "AVPS play: framewindows width == 0");
        AVPS_ASSERT(frameWindows->fb.height,  "AVPS play: framewindows height == 0");
        AVPS_ASSERT(frameWindows->roi.width,  "AVPS play: roi width == 0");
        AVPS_ASSERT(frameWindows->roi.height, "AVPS play: roi height == 0");

        // Ping of FBW decoded video swap buffer.
        g_avpsDecodedVideoFbw[0].fb.width        = frameWindows->fb.width;
        g_avpsDecodedVideoFbw[0].fb.height       = frameWindows->fb.height;
        g_avpsDecodedVideoFbw[0].fb.colorFormat  = screenInfo.bitdepth;
        // 16 bits per pixel
        g_avpsDecodedVideoFbw[0].fb.buffer       = sxr_Malloc(frameWindows->fb.width *
                frameWindows->fb.height * 2);

        g_avpsDecodedVideoFbw[0].roi.width       = frameWindows->roi.width;
        g_avpsDecodedVideoFbw[0].roi.height      = frameWindows->roi.height;
        g_avpsDecodedVideoFbw[0].roi.x           = frameWindows->roi.x;
        g_avpsDecodedVideoFbw[0].roi.y           = frameWindows->roi.y;

        memset(g_avpsDecodedVideoFbw[0].fb.buffer, 0,
               frameWindows->fb.width * frameWindows->fb.height * 2);

        // Pong of FBW decoded video swap buffer.
        g_avpsDecodedVideoFbw[1].fb.width        = frameWindows->fb.width;
        g_avpsDecodedVideoFbw[1].fb.height       = frameWindows->fb.height;
        g_avpsDecodedVideoFbw[1].fb.colorFormat  = screenInfo.bitdepth;
        // 16 bits per pixel
        g_avpsDecodedVideoFbw[1].fb.buffer       = sxr_Malloc(frameWindows->fb.width  *
                frameWindows->fb.height * 2);

        g_avpsDecodedVideoFbw[1].roi.width       = frameWindows->roi.width;
        g_avpsDecodedVideoFbw[1].roi.height      = frameWindows->roi.height;
        g_avpsDecodedVideoFbw[1].roi.x           = frameWindows->roi.x;
        g_avpsDecodedVideoFbw[1].roi.y           = frameWindows->roi.y;

        memset(g_avpsDecodedVideoFbw[1].fb.buffer, 0,
               frameWindows->fb.width * frameWindows->fb.height * 2);
    }

    // PCM buffer size + audio vpp cfg
    switch(stream->audioMode)
    {
        case AVPS_PLAY_AUDIO_MODE_AMR_RING:
            pcmStream->length          = 160;
            g_avpsVppAmjpAudioCfg.mode = VPP_AMJP_AUDIO_MODE_AMR;
            break;
        case AVPS_PLAY_AUDIO_MODE_AAC:
            pcmStream->length          = 512;
            g_avpsVppAmjpAudioCfg.mode = VPP_AMJP_AUDIO_MODE_AAC;
            break;
        case AVPS_PLAY_AUDIO_MODE_MP3:
            pcmStream->length          = 576;
            g_avpsVppAmjpAudioCfg.mode = VPP_AMJP_AUDIO_MODE_MP3;
            break;
        default:
            pcmStream->length          = 0;
            g_avpsVppAmjpAudioCfg.mode = VPP_AMJP_AUDIO_MODE_NO;
            break;
    }

    pcmStream->length *= 2*2*stream->channelNb;
    if (stream->audioSampleRate > HAL_AIF_FREQ_24000HZ)
    {
        pcmStream->length *= 2;
    }

    g_avpsAudioBufferStart  = hal_VocGetDmaiPointer((UINT32*)(stream->audioAddress),
                              HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    g_avpsAudioBufferTop    = hal_VocGetDmaiPointer((UINT32*)(stream->audioAddress + stream->audioLength),
                              HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    g_avpsAudioBufferCurPos = hal_VocGetDmaiPointer((UINT32*)(g_avpsAudioBufferCurPos),
                              HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);


    g_avpsVppAmjpAudioCfg.reset                = 0;
    g_avpsVppAmjpAudioCfg.eqType               = cfg->equalizer;
    g_avpsVppAmjpAudioCfg.inStreamBufAddrStart = g_avpsAudioBufferStart;
    g_avpsVppAmjpAudioCfg.inStreamBufAddrEnd   = g_avpsAudioBufferTop;
    g_avpsVppAmjpAudioCfg.outStreamBufAddr     = hal_VocGetDmaiPointer((UINT32*)g_avpsPcmBuf,
            HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    // video Vpp cfg
    switch(stream->videoMode)
    {
        case AVPS_PLAY_VIDEO_MODE_MJPEG:
            g_avpsVppAmjpVideoCfg.mode = VPP_AMJP_VIDEO_MODE_JPEG;
            break;
        case AVPS_PLAY_VIDEO_MODE_H263:
            g_avpsVppAmjpVideoCfg.mode = VPP_AMJP_VIDEO_MODE_H263;
            break;
        case AVPS_PLAY_VIDEO_MODE_MPEG4:
            g_avpsVppAmjpVideoCfg.mode = VPP_AMJP_VIDEO_MODE_MPEG4;
            break;
        default:
            g_avpsVppAmjpVideoCfg.mode = VPP_AMJP_VIDEO_MODE_NO;
    }


    g_avpsVppAmjpVideoCfg.zoomMode             = VPP_AMJP_ZOOM_NO;
    g_avpsVppAmjpVideoCfg.rotateMode           = VPP_AMJP_ROTATE_NO;
    g_avpsVppAmjpVideoCfg.vidBufMode           = VPP_AMJP_BUF_MODE_SWAP;
    if(cfg->videoRotation == AVPS_VIDEO_ROTATION_LANDSCAPE)
    {
        g_avpsVppAmjpVideoCfg.rotateMode       = VPP_AMJP_ROTATE_90;
    }
    g_avpsVppAmjpVideoCfg.resizeToWidth        = 0;
    if(g_avpsVppAmjpVideoCfg.rotateMode == VPP_AMJP_ROTATE_NO ||
            g_avpsVppAmjpVideoCfg.rotateMode == VPP_AMJP_ROTATE_180)
    {
        g_avpsVppAmjpVideoCfg.resizeToWidth    = g_avpsDecodedVideoFbw[0].fb.width;
    }
    g_avpsVppAmjpVideoCfg.inVidBufAddrStart    = hal_VocGetDmaiPointer((UINT32*)(g_avpsVideoEncodedBuffer),
            HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);
    g_avpsVppAmjpVideoCfg.inVidBufAddrEnd      = hal_VocGetDmaiPointer((UINT32*)(g_avpsVideoEncodedBuffer +
            g_avpsVideoEncodedBufferLength),
            HAL_VOC_DMA_READ, HAL_VOC_DMA_BURST,HAL_VOC_DMA_B2S_NO);

    pcmStream->voiceQuality                = FALSE;
    pcmStream->playSyncWithRecord          = FALSE;

    pcmStream->startAddress                = g_avpsPcmBuf;


    // swap buffer start addresses
    g_avpsPcmBuf0 = &g_avpsPcmBuf[0];
    g_avpsPcmBuf1 = &g_avpsPcmBuf[(pcmStream->length/2)/sizeof(UINT32)];

    // HW event used to wakeup VoC
    pcmStream->halfHandler = NULL;
    pcmStream->endHandler  = NULL;

    // Open Amjp
    if (HAL_ERR_RESOURCE_BUSY == vpp_AmjpOpen(avps_VppAmjpHandler, VPP_AMJP_WAKEUP_HW_AUDIO_ONLY))
    {
        return AVPS_ERR_RESOURCE_BUSY;
    }
    return AVPS_ERR_NO;
}
