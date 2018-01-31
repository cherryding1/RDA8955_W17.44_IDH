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

#include "hal_aif.h"
#include "hal_sys.h"
#include "hal_error.h"
#include "hal_overlay.h"
#include "hal_debug.h"

#include "aud_m.h"
#include "apbs_m.h"
#include "apbsp_cfg.h"
#include "vpp_speech.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"
#include "mcip_debug.h"
#include "mci.h"

#include "string.h"
#include "sxr_tls.h"
#include "mmc_audiowav.h"
#ifdef AUD_3_IN_1_SPK

#include "mmc_audiovibrator.h"
#endif

extern AUD_LEVEL_T audio_cfg;

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
extern uint32 pcmbuf_overlay[4608];

extern AUD_ITF_T audioItf;
// =============================================================================
//  MACROS
// =============================================================================

/// Number of trames before unmuting AUD
#define APBS_AUD_UNMUTE_TRAME_NB 6

/// Number of trames before unmuting VPP
#define APBS_VPP_UNMUTE_TRAME_NB 3

/// TODO : should be properly dfefined in the header
#define APBS_ERR_NB_MAX 20

// the length of PCM buffer for stereo.(Bytes)
#define  MP3PCMBUFSIZE  (1152*4*4)

// the length of PCM buffer for stereo.(Bytes)
#define  AACPCMBUFSIZE  (1024*4*4)


// the length of PCM buffer for stereo.(Bytes)
#define  WAVPCMBUFSIZE  (1024*2)

// the length of PCM buffer for mono.(Bytes)
#define  g_apbsPcmMono8kBufSize (2*576*2)

// the length of VoC internal buffer.(Bytes)
#define  g_apbsVoCIntBufSize  (2048)

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  STREAM GLOBAL VARIABLES
// =============================================================================

PRIVATE BOOL g_apbsFinishData = FALSE;
/// End address of the stream buffer
PRIVATE UINT32 g_apbsBufferLength = 0;

/// Start address of the stream buffer
PRIVATE UINT32* g_apbsBufferStart= NULL;

/// End address of the stream buffer
PRIVATE UINT32* g_apbsBufferTop = NULL;

/// Number of bytes of data to read in the buffer
PRIVATE INT32 g_apbsBufferRemainingBytes = 0;

/// Current position in the buffer. Next read data will be read from here.
PRIVATE UINT32* g_apbsBufferCurPos= NULL;

/// Current position in the PCM buffer. Next write data will be written from here.
PRIVATE UINT32* g_apbsOutBufferCurPos= NULL;

/// Output Count Bytes.
PRIVATE UINT32 g_apbsOutCountBytes= 0;

VOLATILE UINT32 g_apbsPcmStereoBufSize=0;

PRIVATE MCI_PLAY_MODE_T g_apbsaudiomode=0;
// =============================================================================
//  CONFIG GLOBAL VARIABLES
// =============================================================================

/// Interface used to play the current stream.
PRIVATE AUD_ITF_T g_apbsItf= 0;

/// Number of bytes composing one trame for the codec currently used
PRIVATE UINT32 g_apbsFrameSize= 0;

/// To stop at the end of the buffer if not in loop mode
PRIVATE BOOL g_apbsLoopMode = FALSE;

/// For openning the  audio device only once.
VOLATILE BOOL g_apbsAudioDeviceOpened = FALSE;

/// The flag of buffer end.
VOLATILE BOOL g_apbsBufferEndFlag = FALSE;

/// For VoC Run.
VOLATILE BOOL g_apbsVoCRunFlag = FALSE;

/// User handler to call with play status is stored here.
PRIVATE APBS_USER_HANDLER_T g_apbsUserHandler = NULL;

/// Number of played Frames
PRIVATE UINT32 g_apbsFramesPlayedNb = 0;

/// Pointer to the stream input buffer of VPP decoding buffer
PRIVATE UINT32* g_apbsDecInput= NULL;

/// To only pulse the mid buffer
PRIVATE BOOL g_apbsMidBufDetected = FALSE;

/// Index in the swap buffer (for PCM) which is the one where to copy
/// the new data
PRIVATE UINT32 g_apbsSwIdx = 0;

/// Audio configuration used to play the stream in normal state.
/// (ie not when started or stopped.
PRIVATE AUD_LEVEL_T g_apbsAudioCfg;

/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped).
PRIVATE VPP_SPEECH_AUDIO_CFG_T g_apbsVppCfg;

/// VPP configuration used to play multimedai stream in normal state.
/// (ie not when started or stopped.
PRIVATE vpp_AudioJpeg_DEC_IN_T g_apbsVppAmjpAudioCfg;

/// counter for the errors
VOLATILE UINT32 g_apbsErrNb;

/// Audio PCM buffer
/// TODO :  Durty patch to play MP3. This bufer should be allocated by MPS!
PRIVATE UINT32 *g_apbsPcmBuf= NULL;

/// TODO: move that in the speech handler specific file.
/// Enable amr ring
PROTECTED BOOL g_apbsAmrRing;

/// Are we playing a speech stream ?
PRIVATE BOOL g_apbsSpeechUsed = FALSE;

/// Are we playing a audio stream ?
PRIVATE BOOL g_apbsAudioUsed = FALSE;

/// Are we playing a audio stream in Temp Buffer?
PRIVATE BOOL g_apbsDataInTempBuffer= FALSE;


/// Bit per sample.
PRIVATE UINT32  g_apbsBitsPerSample= 0;

/// Bit per sample.
VOLATILE UINT32  g_apbsSampleRate= 0;

/// Bit per sample.
VOLATILE UINT32  g_apbsChannel= 0;

#ifndef SMALL_BSS_RAM_SIZE

//temp buffer for loop mode
PRIVATE UINT32 g_apbsBufferForLoopmode [g_apbsVoCIntBufSize/2] ; /// 2*g_apbsVoCIntBufSize in bytes.
#else

PRIVATE UINT8* g_apbsBufferForLoopmode_alloc= NULL;
PRIVATE UINT32* g_apbsBufferForLoopmode= NULL;
#endif

WAV_INPUT g_wavPlayer;
PRIVATE INT16 *g_apbsAdpcmBuf= NULL;
PRIVATE INT16 *g_apbsAdpcmBufRead= NULL;
VOLATILE INT32  g_apbsAdpcmRemainSize= 0;

// TODO : This is durty, half/full buff values should be given by MPS
#define  g_apbsPcmMono8kBuf_0    (g_apbsPcmBuf)
//#define  g_apbsPcmMono8kBuf_1    (g_apbsPcmBuf+(g_apbsPcmMono8kBufSize/8))  // half size in UINT32

#define  g_apbsPcmStereoBuf_0    (g_apbsPcmBuf)
#define  g_apbsPcmStereoBuf_1   (g_apbsPcmBuf+(g_apbsPcmStereoBufSize/8))  // half size in UINT32



typedef struct
{
    UINT8   szRiff[4];          /* "RIFF" */
    UINT32  dwFileSize;         /* file size */
    UINT8   szWaveFmt[8];       /* "WAVEfmt " */
    UINT32  dwFmtSize;          /* 16 */
    UINT16  wFormatTag;     /* format type */
    UINT16  nChannels;          /* number of channels (i.e. mono, stereo...) */
    UINT32  nSamplesPerSec; /* sample rate */
    UINT32  nAvgBytesPerSec;    /* for buffer estimation */
    UINT16  nBlockAlign;            /* block size of data */
    UINT16  wBitsPerSample;     /* number of bits per sample of mono data */
} WAVHeader;


typedef struct
{
    UINT8       szData[4];      /* "data" */
    UINT32      dwDataSize;     /*pcm data size*/
} WAVDataHeader;


//#define MP3_PLAYERBUFFER_AGC



#ifdef MP3_PLAYERBUFFER_AGC

#define MULTS( x, y)  (INT64)(((INT64)x *(INT64)y)>>16)
#define ABS_AGC(X)  ((X<0)?-X:X)

static int AGC_Coeff_l = 0x7fff*2*4;
static int AGC_serror_l = 0;
static int  AGC_acc_error_l = 0;

//this value is what u want to control audio gain level;
//it should be set by user;
static int  AGC_MAXVAL = 32000; // 16384; // 32767;


static void AGainCtr_S(short *in,short *out,int nLen)
{

    int temp;
    int index;

    INT64 temp_xuml;

    for(index=0; index<nLen; index++)
    {
        temp = ABS_AGC(in[index]);

        temp_xuml=MULTS(temp,AGC_Coeff_l);


        if (temp_xuml>32767)
        {
            temp_xuml=32767;
        }

        AGC_serror_l = temp_xuml - AGC_MAXVAL;
        if(AGC_serror_l <0)
        {

            if (AGC_serror_l<-32767/2)
            {
                AGC_serror_l=-1;
            }
            else
            {
                AGC_serror_l =0;
            }
        }

        AGC_acc_error_l = (AGC_acc_error_l >>17) + (AGC_serror_l <<1);



        AGC_Coeff_l = AGC_Coeff_l - AGC_acc_error_l ;

        if (AGC_Coeff_l> 0x7fff*32)
        {
            AGC_Coeff_l=0x7fff*32;
        }

        temp_xuml=MULTS(in[index] ,AGC_Coeff_l);

        if (temp_xuml<-32768)
        {
            temp_xuml=-32768;
        }

        if (temp_xuml>32767)
        {
            temp_xuml=32767;
        }


        out[index] =temp_xuml; //MULTS(in[index] ,AGC_Coeff_l);

    }

}


#endif










PRIVATE CONST UINT16 WAVSamplesRate[]= {48000,44100,32000,24000,22050,16000,12000,11025,8000};


// ============================================================================
// APBS_STATE_T
// ----------------------------------------------------------------------------
/// Describes the APBS opening/closing state.
// ============================================================================
typedef enum
{
    APBS_NULL,
    APBS_BEING_OPENED,
    APBS_BEING_CLOSED,
} APBS_CONFIG_STATE_T;

// Global variable telling if the open/close procedure is finished
PRIVATE VOLATILE APBS_CONFIG_STATE_T g_apbsConfigState  = APBS_NULL;


// =============================================================================
// apbs_AmjpPcmHalfHandler
// -----------------------------------------------------------------------------
/// Handler for the Amjp module, to play PCM  streams.
/// Wake up the VoC to decode the frames to the First half PCM buffer.
// =============================================================================
PRIVATE VOID apbs_AmjpPcmHalfHandler(VOID)
{
    if (g_apbsVoCRunFlag !=TRUE)
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] Half");
        // set write pointer to beging
        g_apbsOutBufferCurPos = g_apbsPcmBuf;  // half size in UINT32;
        // updata the output buffer address.
        g_apbsVppAmjpAudioCfg.outStreamBufAddr = g_apbsOutBufferCurPos;
        // configure next frame and wake up VoC
        vpp_AudioJpegDecScheduleOneFrame(&g_apbsVppAmjpAudioCfg);
    }
}


// =============================================================================
// apbs_AmjpPcmEndHandler
// -----------------------------------------------------------------------------
/// Handler for the Amjp module, to play PCM  streams.
/// Wake up the VoC to decode the  frames to the Second half PCM buffer.
// =============================================================================
PRIVATE VOID apbs_AmjpPcmEndHandler(VOID)
{
    if (g_apbsVoCRunFlag !=TRUE)
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] END");
        // set write pointer to half
        g_apbsOutBufferCurPos = (g_apbsPcmBuf+(g_apbsPcmStereoBufSize/8));  // half size in UINT32;
        // updata the output buffer address.
        g_apbsVppAmjpAudioCfg.outStreamBufAddr = g_apbsOutBufferCurPos;
        // configure next frame and wake up VoC
        vpp_AudioJpegDecScheduleOneFrame(&g_apbsVppAmjpAudioCfg);
    }
}

// =============================================================================
//  FUNCTIONS
// =============================================================================
// =============================================================================
// apbs_VppAmjpHandler
// -----------------------------------------------------------------------------
// =============================================================================
PRIVATE VOID apbs_VppAmjpHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    //    HAL_AIF_STREAM_T pcmStream;
    vpp_AudioJpeg_DEC_OUT_T vppStatus;
    INT32 readBytes;
    APBS_PROFILE_FUNCTION_ENTER(apbs_VppAmjpHandler);
    g_apbsVoCRunFlag = FALSE;
    MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] VoC");

    if (g_apbsFramesPlayedNb == APBS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        MCI_TRACE(MCI_AUDIO_TRC,0, "AUD unmuted");
        g_apbsAudioCfg.spkLevel  = audio_cfg.spkLevel;
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_SETUP);
        //   aud_Setup(g_apbsItf, &g_apbsAudioCfg);
    }

    // do not read status during the first callback
    if (g_apbsFramesPlayedNb++)
    {
        // get status
        vpp_AudioJpegDecStatus(&vppStatus);
        //MCI_TRACE (MCI_AUDIO_TRC,0, "APBS consumedLen:%d",vppStatus.consumedLen);
        // compute the new remaining size
        readBytes =vppStatus.consumedLen;
        APBS_ASSERT(!(readBytes < 0), "consumedLen must be >=0");
        // update the stream buffer status
        g_apbsBufferRemainingBytes -= readBytes;
        // update read pointer
        g_apbsBufferCurPos += readBytes/4;  //  in UINT32
        // check for errors
        if (vppStatus.ErrorStatus)
        {
            g_apbsFramesPlayedNb--;
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]APBS ERROR!");

            if (g_apbsBufferRemainingBytes < 0)
            {
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]APBS NO DATA!");
                g_apbsErrNb+=APBS_ERR_NB_MAX;
            }

            if (g_apbsErrNb++ > APBS_ERR_NB_MAX)
            {
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]APBS ERROR STOP!");

                //send message to caller.
                if (MCI_RingFinished!=NULL)
                {
                    MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
                }

                // Stop the play
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
                //                    apbs_Stop();
                // Leave
                return;
            }
        }
        else
        {
            /// reset counter for the errors
            g_apbsErrNb=0;
            // update the Counter status
            g_apbsOutCountBytes += vppStatus.output_len;

#ifdef MP3_PLAYERBUFFER_AGC

            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]AGC_Coeff_l:0x%x",AGC_Coeff_l);

            if(audioItf==AUD_ITF_LOUD_SPEAKER)
            {
                AGainCtr_S((INT16 *)((UINT32)(g_apbsOutBufferCurPos)|0x20000000),(INT16 *)((UINT32)(g_apbsOutBufferCurPos)|0x20000000),vppStatus.output_len/2);

            }
#endif



#ifdef AUD_3_IN_1_SPK


            if(audioItf==AUD_ITF_LOUD_SPEAKER&&(vppStatus.nbChannel==1||vppStatus.nbChannel==2))
            {

                INT16 * VibratorDataAddr=(INT16 *)((UINT32)(g_apbsOutBufferCurPos)|0x20000000);;
                INT32 VibratorDataLength=vppStatus.output_len/2;
                INT32 VibratorDataSample=vppStatus.SampleRate;
                INT32 VibratorDataCh=vppStatus.nbChannel;

                //INT32 timebegin=hal_TimGetUpTime();
                mmc_Vibrator(VibratorDataAddr,VibratorDataLength,VibratorDataAddr ,g_MCIAudioVibrator, VibratorDataSample,VibratorDataCh);
                //INT32 timeend_xuml=hal_TimGetUpTime();
                //MCI_TRACE (TSTDOUT,0,"Vibrator time = %dms",((timeend_xuml-timebegin)*1000)/16384);
            }
#endif

            // update write pointer
            g_apbsOutBufferCurPos += vppStatus.output_len/4;  //in UINT32



        }
    }

    if (g_apbsBufferRemainingBytes < g_apbsVoCIntBufSize)
    {
        if (g_apbsUserHandler)
        {
            g_apbsUserHandler(g_apbsBufferRemainingBytes);
        }

        if (g_apbsLoopMode)
        {
            if (g_apbsDataInTempBuffer)
            {
                MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS]APBS USE THE EXTERN BUFFER!");
                //reset the current poioter.
                g_apbsBufferCurPos=g_apbsBufferStart+(g_apbsVoCIntBufSize-g_apbsBufferRemainingBytes)/4;//in UINT32
                //reset the remaining bytes
                g_apbsBufferRemainingBytes =g_apbsBufferLength-g_apbsVoCIntBufSize+g_apbsBufferRemainingBytes;
                //swiche to play the audio stream in extern buffer.
                g_apbsDataInTempBuffer=FALSE;
            }
            else
            {
                MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS]APBS USE THE TEMP BUFFER!");
                //copy the final data to the temp buffer.
                memcpy(g_apbsBufferForLoopmode,g_apbsBufferCurPos,g_apbsBufferRemainingBytes);
                //copy the begining dada to the temp buffer.
                memcpy(g_apbsBufferForLoopmode+g_apbsBufferRemainingBytes/4,g_apbsBufferStart,g_apbsVoCIntBufSize);
                //reset the remaining bytes
                g_apbsBufferRemainingBytes +=g_apbsVoCIntBufSize;
                //reset the current poioter.
                g_apbsBufferCurPos=g_apbsBufferForLoopmode;
                //reset the Frames
                g_apbsFramesPlayedNb=1;
                //in loop mode, last we will paly audio stream in the internal temp buffer for VoC DMA.
                //we must make sure there is enough(g_apbsVoCIntBufSize) data in input buffer.
                g_apbsDataInTempBuffer=TRUE;
            }
        }
        else
        {

            if( g_apbsBufferRemainingBytes <= 0)
            {
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]NO LOOP! END OF BUFFER !");
                g_apbsBufferEndFlag = TRUE;

                if (g_apbsAudioDeviceOpened==TRUE)
                {
                    //send message to caller.
                    if (MCI_RingFinished!=NULL)
                    {
                        MCI_RingFinished(MCI_ERR_END_OF_FILE);
                    }

                    // Stop the play
                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
                    //                  apbs_Stop();
                    // Leave
                    return;
                }
                else
                {
                    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] AUDIO DEVICE HAS BEEN NOT OPENED!");
                    return;
                }
            }
        }

    }

    // call the user handler if new data is necessary
    if (g_apbsBufferRemainingBytes < (g_apbsBufferLength/2))
    {
        if (g_apbsUserHandler)
        {
            g_apbsUserHandler(g_apbsBufferRemainingBytes);
        }
    }

    if((UINT32)(&g_apbsBufferCurPos[0]) - (UINT32)(&g_apbsBufferForLoopmode[0]) < 0x1000)
    {
        // updata the input buffer address.
        g_apbsVppAmjpAudioCfg.inStreamBufAddr = g_apbsBufferCurPos;
    }
    else
    {
        memcpy(g_apbsBufferForLoopmode,g_apbsBufferCurPos,g_apbsVoCIntBufSize);
        // updata the input buffer address.
        g_apbsVppAmjpAudioCfg.inStreamBufAddr = g_apbsBufferForLoopmode;
    }

    // uadata the the audio interface.
    g_apbsVppAmjpAudioCfg.audioItf   = audioItf;
    // updata the output buffer address.
    g_apbsVppAmjpAudioCfg.outStreamBufAddr = g_apbsOutBufferCurPos;

    if(g_apbsAudioDeviceOpened==TRUE)
    {
        if (g_apbsOutCountBytes<g_apbsPcmStereoBufSize/2)
        {
            // configure next frame and wake up VoC
            vpp_AudioJpegDecScheduleOneFrame(&g_apbsVppAmjpAudioCfg);
            g_apbsVoCRunFlag = TRUE;
        }
        else
        {
            //have filled in  the half buffer. and will wait the next PCM interrupt.
            g_apbsOutCountBytes=0;

        }
    }
    else
    {

        // if(g_apbsOutCountBytes<g_apbsPcmStereoBufSize/2)
        if(g_apbsFramesPlayedNb<3)
        {
            // configure next frame and wake up VoC
            vpp_AudioJpegDecScheduleOneFrame(&g_apbsVppAmjpAudioCfg);
            g_apbsVoCRunFlag = TRUE;
        }
        else
        {
            //have filled in  the half buffer. and will wait the next PCM interrupt.
            g_apbsOutCountBytes=0;

            g_apbsPcmStereoBufSize=vppStatus.output_len*2;

            g_apbsSampleRate=vppStatus.SampleRate;
            g_apbsChannel=vppStatus.nbChannel;
            //have opened the audio device.
            g_apbsAudioDeviceOpened=TRUE;


        }
    }

    // profile
    APBS_PROFILE_FUNCTION_EXIT(apbs_VppAmjpHandler);
}



PRIVATE VOID apbs_StreamVppAmjpHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    //    HAL_AIF_STREAM_T pcmStream;
    vpp_AudioJpeg_DEC_OUT_T vppStatus;
    INT32 readBytes;
    INT32 nRemainingLength;
    APBS_PROFILE_FUNCTION_ENTER(apbs_VppAmjpHandler);
    g_apbsVoCRunFlag = FALSE;
    MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] apbs_StreamVppAmjpHandler");

    if (g_apbsFramesPlayedNb == APBS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        MCI_TRACE(MCI_AUDIO_TRC,0, "AUD unmuted");
        g_apbsAudioCfg.spkLevel  = audio_cfg.spkLevel;
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_SETUP);
        //   aud_Setup(g_apbsItf, &g_apbsAudioCfg);
    }

    // do not read status during the first callback
    if (g_apbsFramesPlayedNb++)
    {
        // get status
        vpp_AudioJpegDecStatus(&vppStatus);
        //MCI_TRACE (MCI_AUDIO_TRC,0, "APBS consumedLen:%d",vppStatus.consumedLen);
        // compute the new remaining size
        readBytes =vppStatus.consumedLen;
        APBS_ASSERT(!(readBytes < 0), "consumedLen must be >=0");
        UINT32 csStatus = hal_SysEnterCriticalSection();
        // update the stream buffer status
        g_apbsBufferRemainingBytes -= readBytes;
        hal_SysExitCriticalSection(csStatus);
        MCI_TRACE(MCI_AUDIO_TRC,0, "readBytes=%d", readBytes);

        if (((UINT32)g_apbsBufferCurPos + readBytes) >= (UINT32)g_apbsBufferTop)
        {
            // else wrap ...
            MCI_TRACE(MCI_AUDIO_TRC,0, "Entry Loop::g_apbsBufferCurPos= %d  g_apbsBufferLength=%d", (g_apbsBufferCurPos-g_apbsBufferStart)*4, g_apbsBufferLength);
            g_apbsBufferCurPos = (UINT32*)((UINT32)g_apbsBufferStart+readBytes-
                                           ((UINT32)g_apbsBufferTop -(UINT32) g_apbsBufferCurPos));
        }
        else
        {
            g_apbsBufferCurPos += readBytes/4;  //  in UINT32
        }

        MCI_TRACE(MCI_AUDIO_TRC,0, "g_apbsBufferRemainingBytes= %d  g_apbsBufferCurPos= %d", g_apbsBufferRemainingBytes, (g_apbsBufferCurPos-g_apbsBufferStart)*4);
        // update read pointer

        // check for errors
        if (vppStatus.ErrorStatus)
        {
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]APBS ERROR!");

            if (g_apbsErrNb++ > APBS_ERR_NB_MAX)
            {
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]APBS ERROR STOP!");
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_ERR);
                // Stop the play
                //                    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
                //                    apbs_Stop();
                // Leave
                return;
            }
        }
        else
        {
            /// reset counter for the errors
            g_apbsErrNb=0;
            // update the Counter status
            g_apbsOutCountBytes += vppStatus.output_len;
            // update write pointer
            g_apbsOutBufferCurPos += vppStatus.output_len/4;  //in UINT32
        }
    }

    if (g_apbsBufferRemainingBytes < g_apbsVoCIntBufSize)
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "Not  Enough Data::g_apbsBufferRemainingBytes=%d",g_apbsBufferRemainingBytes);

        if (g_apbsAudioDeviceOpened==FALSE)
        {
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] AUDIO DEVICE HAS BEEN NOT OPENED!");
            return;
        }

        if (g_apbsFinishData)
        {
            MCI_TRACE(MCI_AUDIO_TRC,0, "Send APBS_STOP message!!");
            g_apbsBufferEndFlag = TRUE;

            if (MCI_RingFinished!=NULL)
            {
                MCI_RingFinished(MCI_ERR_END_OF_FILE);
            }

            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
        }
        else
        {
            MCI_TRACE(MCI_AUDIO_TRC,0, "VPP muted");
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_NO_MORE_DATA);
        }

        return;
    }

    nRemainingLength = (UINT32)g_apbsBufferTop -(UINT32) g_apbsBufferCurPos;

    if (nRemainingLength  < g_apbsVoCIntBufSize)
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "entry g_apbsBufferRemainingBytes=%d",g_apbsBufferRemainingBytes);
        MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS]APBS USE THE TEMP BUFFER!");
        //copy the final data to the temp buffer.
        memcpy(g_apbsBufferForLoopmode,g_apbsBufferCurPos,nRemainingLength);
        //copy the begining dada to the temp buffer.
        memcpy(g_apbsBufferForLoopmode+nRemainingLength/4,g_apbsBufferStart,2*g_apbsVoCIntBufSize-nRemainingLength);
        //reset the current poioter.
        g_apbsVppAmjpAudioCfg.inStreamBufAddr = g_apbsBufferForLoopmode;
    }
    else
    {
        g_apbsVppAmjpAudioCfg.inStreamBufAddr = g_apbsBufferCurPos;
    }

    // call the user handler if new data is necessary
    if (g_apbsBufferRemainingBytes < (g_apbsBufferLength/2))
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "Entry Data request callback::g_apbsBufferRemainingBytes=%d, g_apbsUserHandler=%x", g_apbsBufferRemainingBytes, g_apbsUserHandler);
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_REQUEST);
        //          if (g_apbsUserHandler)
        //          {
        //              g_apbsUserHandler(STREAM_STATUS_REQUEST_DATA);
        //          }
    }

    // uadata the the audio interface.
    g_apbsVppAmjpAudioCfg.audioItf   = audioItf;
    // updata the input buffer address.
    //  g_apbsVppAmjpAudioCfg.inStreamBufAddr = g_apbsBufferCurPos;
    // updata the output buffer address.
    g_apbsVppAmjpAudioCfg.outStreamBufAddr = g_apbsOutBufferCurPos;

    if (g_apbsOutCountBytes<g_apbsPcmStereoBufSize/2)
    {
        // configure next frame and wake up VoC
        vpp_AudioJpegDecScheduleOneFrame(&g_apbsVppAmjpAudioCfg);
        g_apbsVoCRunFlag = TRUE;
    }
    else
    {
        //have filled in  the half buffer. and will wait the next PCM interrupt.
        g_apbsOutCountBytes=0;

        if (g_apbsAudioDeviceOpened==FALSE)
        {
            g_apbsSampleRate=vppStatus.SampleRate;
            g_apbsChannel=vppStatus.nbChannel;
            //have opened the audio device.
            g_apbsAudioDeviceOpened=TRUE;
        }
    }

    // profile
    APBS_PROFILE_FUNCTION_EXIT(apbs_VppAmjpHandler);
}


// =============================================================================
// apbs_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID apbs_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    APBS_PROFILE_FUNCTION_ENTER(apbs_VppSpeechHandler);
    // Number of bytes to copy;
    UINT32 cpSz;
    // Position in the decoding structure where to store
    // the data from the stream.
    UINT32* decInBufPos;
    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;
    HAL_SPEECH_DEC_IN_T* decStruct;
    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    MCI_TRACE(MCI_AUDIO_TRC,0, "entry apbs_VppSpeechHandler");

    if (g_apbsFramesPlayedNb == APBS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        MCI_TRACE(MCI_AUDIO_TRC,0, "AUD unmuted");
        g_apbsAudioCfg.spkLevel  = audio_cfg.spkLevel;
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_SETUP);
        //   aud_Setup(g_apbsItf, &g_apbsAudioCfg);
    }

    if (g_apbsFramesPlayedNb == APBS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        MCI_TRACE(MCI_AUDIO_TRC,0, "VPP unmuted");
        vpp_SpeechAudioCfg(&g_apbsVppCfg);
    }

    decStruct = vpp_SpeechGetRxCodBuffer();

    if (g_apbsAmrRing == TRUE)
    {
        g_apbsFrameSize = 1;
        MCI_TRACE(MCI_AUDIO_TRC,0, "g_apbsBufferRemainingBytes=%d", g_apbsBufferRemainingBytes);

        if (g_apbsBufferRemainingBytes >= 1)
        {
            switch ((*((UINT8*)g_apbsBufferCurPos) >> 3) & 0xF)
            {
                // Real frame size, as there is no alignment
                case 0:
                    g_apbsFrameSize      = 13;
                    decStruct->codecMode = HAL_AMR475_DEC;
                    break;
                case 1:
                    g_apbsFrameSize      = 14;
                    decStruct->codecMode = HAL_AMR515_DEC;
                    break;
                case 2:
                    g_apbsFrameSize      = 16;
                    decStruct->codecMode = HAL_AMR59_DEC;
                    break;
                case 3:
                    g_apbsFrameSize      = 18;
                    decStruct->codecMode = HAL_AMR67_DEC;
                    break;
                case 4:
                    g_apbsFrameSize      = 20;
                    decStruct->codecMode = HAL_AMR74_DEC;
                    break;
                case 5:
                    g_apbsFrameSize      = 21;
                    decStruct->codecMode = HAL_AMR795_DEC;
                    break;
                case 6:
                    g_apbsFrameSize      = 27;
                    decStruct->codecMode = HAL_AMR102_DEC;
                    break;
                case 7:
                    g_apbsFrameSize      = 32;
                    decStruct->codecMode = HAL_AMR122_DEC;
                    break;
                default:
                    break;
            }
        }

        MCI_TRACE(MCI_AUDIO_TRC,0, "entry g_apbsFrameSize=%d,cod=%d ",g_apbsFrameSize,decStruct->codecMode);
    }

    if (g_apbsBufferRemainingBytes < g_apbsFrameSize)
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "entry <g_apbsFrameSize");

        // FIXME Let's assume something: If the buffer
        // is played in no loop, we don't care about the
        // alignment and guess that there won't be any refill
        if (!g_apbsLoopMode)
        {
            // As stop destroy the global vars ...
            APBS_USER_HANDLER_T handler = g_apbsUserHandler;
            // Stop the play
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
            //  apbs_Stop();

            //send message to caller.
            if (MCI_RingFinished!=NULL)
            {
                MCI_RingFinished(MCI_ERR_END_OF_FILE);
            }

            // End of buffer reached
            if (handler)
            {
                handler(APBS_STATUS_END_BUFFER_REACHED);
            }

            // Leave
            return;
        }
        else
        {
            if (g_apbsUserHandler)
                // Data underflow ...
                // Stops here
            {
                // We need and ask for refill
                MCI_TRACE(MCI_AUDIO_TRC,0,"APBS famine\n");
                AUD_LEVEL_T audioCfg = g_apbsAudioCfg;
                VPP_SPEECH_AUDIO_CFG_T vppCfg = g_apbsVppCfg;
                g_apbsUserHandler(APBS_STATUS_NO_MORE_DATA);
                /// Mute ...
                audioCfg.spkLevel = 0;
                vppCfg.decMute = VPP_SPEECH_MUTE;
                //      aud_Setup(g_apbsItf, &audioCfg);
                vpp_SpeechAudioCfg(&vppCfg);
                /// Set state as for starting again from there ?
                g_apbsFramesPlayedNb = 0;
            }
            else
            {
                // Simply wrap, we want to play that buffer again
                // and again
                g_apbsBufferCurPos = g_apbsBufferStart;
                g_apbsBufferRemainingBytes = g_apbsBufferLength;
            }
        }
    }
    else
    {
        MCI_TRACE(MCI_AUDIO_TRC,0, "g_apbsBufferRemainingBytes >g_apbsFrameSize");
        UINT32 csStatus = hal_SysEnterCriticalSection();
        g_apbsBufferRemainingBytes -= g_apbsFrameSize;
        hal_SysExitCriticalSection(csStatus);
        decInBufPos = (UINT32*)decStruct->decInBuf;

        ///
        if (((UINT32)g_apbsBufferCurPos + g_apbsFrameSize) > (UINT32)g_apbsBufferTop)
        {
            if (!g_apbsLoopMode)
            {
                // As stop destroy the global vars ...
                APBS_USER_HANDLER_T handler = g_apbsUserHandler;
                // Stop the play
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
                //                apbs_Stop();

                //send message to caller.
                if (MCI_RingFinished!=NULL)
                {
                    MCI_RingFinished(MCI_ERR_END_OF_FILE);
                }

                // End of buffer reached
                if (handler)
                {
                    handler(APBS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }

            // else wrap ...
            // Copy first part
            cpSz = (UINT32)(g_apbsBufferTop - g_apbsBufferCurPos);
            memcpy(decInBufPos, g_apbsBufferCurPos, cpSz);
            decInBufPos        = (UINT32*)((UINT32)decInBufPos+cpSz);
            g_apbsBufferCurPos = g_apbsBufferStart;
            cpSz = g_apbsFrameSize - cpSz;
            // Buffer End Reached
            endBufferReached = TRUE;
            g_apbsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_apbsFrameSize;
        }

        // Finish or full copy
        memcpy(decInBufPos, g_apbsBufferCurPos, cpSz);
        g_apbsBufferCurPos = (UINT32*)((UINT32)g_apbsBufferCurPos+cpSz);
        g_apbsFramesPlayedNb++;

        // Half buffer reached
        if (((UINT32)g_apbsBufferCurPos >= (UINT32)g_apbsBufferStart + (g_apbsBufferLength/2))
                && !g_apbsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_apbsMidBufDetected = TRUE;
        }
    }

    if (halfBufferReached)
    {
        if (g_apbsUserHandler)
        {
            g_apbsUserHandler(APBS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_apbsUserHandler)
        {
            g_apbsUserHandler(APBS_STATUS_END_BUFFER_REACHED);
        }
    }

    APBS_PROFILE_FUNCTION_EXIT(apbs_VppSpeechHandler);
}

// =============================================================================
// apbs_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID apbs_StreamSpeechHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    // Number of bytes to copy;
    UINT32 cpSz;
    // Position in the decoding structure where to store
    // the data from the stream.
    UINT32* decInBufPos;
    HAL_SPEECH_DEC_IN_T* decStruct;
    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    MCI_TRACE(MCI_AUDIO_TRC,0, "entry apbs_StreamSpeechHandler");

    if (!g_apbsUserHandler)
    {
        return;
    }

    if (g_apbsFramesPlayedNb == APBS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        MCI_TRACE(MCI_AUDIO_TRC,0, "AUD unmuted");
        g_apbsAudioCfg.spkLevel  = audio_cfg.spkLevel;
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_SETUP);
        //   aud_Setup(g_apbsItf, &g_apbsAudioCfg);
    }

    if (g_apbsFramesPlayedNb == APBS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        MCI_TRACE(MCI_AUDIO_TRC,0, "VPP unmuted");
        vpp_SpeechAudioCfg(&g_apbsVppCfg);
    }

    decStruct = vpp_SpeechGetRxCodBuffer();

    if (g_apbsAmrRing == TRUE)
    {
        g_apbsFrameSize = 1;

        if (g_apbsBufferRemainingBytes >= 1)
        {
            MCI_TRACE(MCI_AUDIO_TRC,0, "g_apbsBufferCurPos Value=0x%x ",*((UINT8*)g_apbsBufferCurPos));

            switch ((*((UINT8*)g_apbsBufferCurPos) >> 3) & 0xF)
            {
                // Real frame size, as there is no alignment
                case 0:
                    g_apbsFrameSize      = 13;
                    decStruct->codecMode = HAL_AMR475_DEC;
                    break;
                case 1:
                    g_apbsFrameSize      = 14;
                    decStruct->codecMode = HAL_AMR515_DEC;
                    break;
                case 2:
                    g_apbsFrameSize      = 16;
                    decStruct->codecMode = HAL_AMR59_DEC;
                    break;
                case 3:
                    g_apbsFrameSize      = 18;
                    decStruct->codecMode = HAL_AMR67_DEC;
                    break;
                case 4:
                    g_apbsFrameSize      = 20;
                    decStruct->codecMode = HAL_AMR74_DEC;
                    break;
                case 5:
                    g_apbsFrameSize      = 21;
                    decStruct->codecMode = HAL_AMR795_DEC;
                    break;
                case 6:
                    g_apbsFrameSize      = 27;
                    decStruct->codecMode = HAL_AMR102_DEC;
                    break;
                case 7:
                    g_apbsFrameSize      = 32;
                    decStruct->codecMode = HAL_AMR122_DEC;
                    break;
                default:
                    //           MCI_ASSERT(1, "error Frame type");
                    break;
            }

            MCI_TRACE(MCI_AUDIO_TRC,0, "entry g_apbsFrameSize=%d,cod=%d ",g_apbsFrameSize,decStruct->codecMode);
        }
    }

    MCI_TRACE(MCI_AUDIO_TRC,0, "entry g_apbsBufferCurPos=%d,g_apbsBufferRemainingBytes=%d ", (g_apbsBufferCurPos-g_apbsBufferStart)*4, g_apbsBufferRemainingBytes);

    if (g_apbsBufferRemainingBytes < g_apbsFrameSize)
    {
        // FIXME Let's assume something: If the buffer
        // is played in no loop, we don't care about the
        // alignment and guess that there won't be any refill
        MCI_TRACE(MCI_AUDIO_TRC,0, "g_apbsBufferRemainingBytes <g_apbsFrameSize");

        // Stop the play
        if (g_apbsFinishData)
        {
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
            MCI_TRACE(MCI_AUDIO_TRC,0,"APBS famine\n");
            AUD_LEVEL_T audioCfg = g_apbsAudioCfg;
            VPP_SPEECH_AUDIO_CFG_T vppCfg = g_apbsVppCfg;
            /// Mute ...
            audioCfg.spkLevel = 0;
            vppCfg.decMute = VPP_SPEECH_MUTE;
            //      aud_Setup(g_apbsItf, &audioCfg);
            vpp_SpeechAudioCfg(&vppCfg);
            /// Set state as for starting again from there ?
            g_apbsFramesPlayedNb = 0;

            if (MCI_RingFinished!=NULL)
            {
                MCI_RingFinished(MCI_ERR_END_OF_FILE);
            }
        }
        else
        {
            // Data underflow ...
            // Stops here
            // We need and ask for refill
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_NO_MORE_DATA);
            MCI_TRACE(MCI_AUDIO_TRC,0,"APBS famine\n");
            AUD_LEVEL_T audioCfg = g_apbsAudioCfg;
            VPP_SPEECH_AUDIO_CFG_T vppCfg = g_apbsVppCfg;
            /// Mute ...
            audioCfg.spkLevel = 0;
            vppCfg.decMute = VPP_SPEECH_MUTE;
            //      aud_Setup(g_apbsItf, &audioCfg);
            vpp_SpeechAudioCfg(&vppCfg);
            /// Set state as for starting again from there ?
            g_apbsFramesPlayedNb = 0;
        }
    }
    else
    {
        //                  MCI_TRACE (MCI_AUDIO_TRC,0, "g_apbsBufferRemainingBytes >g_apbsFrameSize");
        decInBufPos = (UINT32*)decStruct->decInBuf;
        UINT32 csStatus = hal_SysEnterCriticalSection();
        g_apbsBufferRemainingBytes -= g_apbsFrameSize;
        hal_SysExitCriticalSection(csStatus);

        ///
        if (((UINT32)g_apbsBufferCurPos + g_apbsFrameSize) > (UINT32)g_apbsBufferTop)
        {
            // else wrap ...
            // Copy first part
            cpSz = (UINT32)g_apbsBufferTop -(UINT32) g_apbsBufferCurPos;
            memcpy(decInBufPos, g_apbsBufferCurPos, cpSz);
            decInBufPos        = (UINT32*)((UINT32)decInBufPos+cpSz);
            MCI_TRACE(MCI_AUDIO_TRC,0, "g_apbsBufferTop= %d >cpSz=%d", g_apbsBufferTop-g_apbsBufferStart, cpSz);
            g_apbsBufferCurPos = g_apbsBufferStart;
            cpSz = g_apbsFrameSize - cpSz;
        }
        else
        {
            cpSz = g_apbsFrameSize;
        }

        // Finish or full copy
        memcpy(decInBufPos, g_apbsBufferCurPos, cpSz);
        g_apbsBufferCurPos = (UINT32*)((UINT32)g_apbsBufferCurPos+cpSz);

        if ((UINT32)g_apbsBufferCurPos == (UINT32)g_apbsBufferTop)
        {
            g_apbsBufferCurPos = g_apbsBufferStart;
        }

        if (g_apbsBufferRemainingBytes <g_apbsFrameSize*16)
        {
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_REQUEST);
        }

        if (g_apbsBufferRemainingBytes <g_apbsBufferLength/2)
        {
            if (g_apbsFramesPlayedNb%10 == 1)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_REQUEST);
            }
        }

        g_apbsFramesPlayedNb++;
    }
}


// =============================================================================
// apbs_RawPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the RAW module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID apbs_RawPcmHandler(VOID)
{
    // Number of bytes to copy;
    UINT32 cpSz;
    UINT32 i;
    MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] RAW PCM");
    // Position in the copy buffer, where data from the stream
    // are going to be copied
    UINT32* cpBufPos;
    BOOL    endBufferReached = FALSE;
    BOOL    halfBufferReached = FALSE;
    INT16 *PCM_d;
    UINT8 *PCM_s;
    WAV_INPUT *input = &g_wavPlayer;
    APBS_PROFILE_FUNCTION_ENTER(apbs_XXXSpeechHandler);

    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    if (g_apbsFramesPlayedNb == APBS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_SETUP);
        //  aud_Setup(audioItf, &g_apbsAudioCfg);
    }


    {
        UINT32 status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_apbsBufferRemainingBytes -= g_apbsFrameSize;
        hal_SysExitCriticalSection(status);

        if (g_apbsSwIdx == 0)
        {
            cpBufPos = g_apbsDecInput;
        }
        else
        {
            // == 1
            cpBufPos =  g_apbsDecInput + WAVPCMBUFSIZE/8; // half size in UINT32
        }

        ///
        if ((((UINT32)g_apbsBufferCurPos) + (g_apbsFrameSize)) > (UINT32)g_apbsBufferTop)
        {
            if (!g_apbsLoopMode)
            {
                // As stop destroy the global vars ...
                APBS_USER_HANDLER_T handler = g_apbsUserHandler;
                // Stop the play
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
                //                apbs_Stop();

                //send message to caller.
                if (MCI_RingFinished!=NULL)
                {
                    MCI_RingFinished(MCI_ERR_END_OF_FILE);
                }

                // End of buffer reached
                if (handler)
                {
                    handler(APBS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }

            // else wrap ...
            // Copy first part
            cpSz = (UINT32)(g_apbsBufferTop - g_apbsBufferCurPos);

            if (g_apbsBitsPerSample==16)
            {
                if(input->codec == 1  ) //PCM
                {
                    for (i = 0 ; i<(cpSz/4) ; i++)
                    {
                        *cpBufPos = *g_apbsBufferCurPos;
                        g_apbsBufferCurPos++;
                        cpBufPos++;
                    }

                    cpSz = g_apbsFrameSize - (cpSz/4)*4;
                }
            }
            else
            {
                PCM_d=(INT16 *)cpBufPos;
                PCM_s=(UINT8 *)g_apbsBufferCurPos;

                for (i = 0 ; i<(cpSz/2) ; i++)
                {
                    *PCM_d= ((INT16)((*PCM_s)-0x80)) <<8;
                    PCM_d++;
                    PCM_s++;
                    *PCM_d= ((INT16)((*PCM_s)-0x80)) <<8;
                    PCM_d++;
                    PCM_s++;
                }

                cpBufPos=cpBufPos+(cpSz/2);
                g_apbsBufferCurPos=g_apbsBufferCurPos+(cpSz/4);
                cpSz = g_apbsFrameSize - (cpSz/2)*2;
            }

            g_apbsBufferCurPos = g_apbsBufferStart;
            // Buffer End Reached
            endBufferReached = TRUE;
            g_apbsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_apbsFrameSize;
        }

        // Finish or full copy
        if (g_apbsBitsPerSample==16)
        {
            if(input->codec == 1  ) //PCM
            {
                for (i = 0 ; i<(cpSz/4) ; i++)
                {
                    *cpBufPos= *g_apbsBufferCurPos;
                    g_apbsBufferCurPos++;
                    cpBufPos++;
                }

            }
            else  //ADPCM
            {
                INT32  Remain_pcm = WAVPCMBUFSIZE/2 -g_apbsAdpcmRemainSize;
                UINT32   data_size;
                INT16  length;

                PCM_d=(INT16 *)cpBufPos;
                if((g_apbsAdpcmBufRead!=NULL )&&(g_apbsAdpcmRemainSize>0))
                {
                    for (i = 0 ; i<(g_apbsAdpcmRemainSize/2) ; i++)
                    {
                        *PCM_d=*g_apbsAdpcmBufRead;
                        g_apbsAdpcmBufRead++;
                        PCM_d++;
                    }

                    g_apbsAdpcmBufRead= NULL;
                    g_apbsAdpcmRemainSize =0;
                }

                while(Remain_pcm>0)
                {

                    data_size = WAVPCMBUFSIZE;
                    length = adpcm_decode_frame(input, (void*)g_apbsAdpcmBuf,&data_size, (UINT8 *)g_apbsBufferCurPos ,input->block_align);

                    if(length<0)
                    {
                        length=0;
                        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS]apbs_Play::  ERROR ADPCM buffer over!! \n");
                    }

                    g_apbsBufferCurPos +=(length/4);
                    g_apbsAdpcmBufRead =  g_apbsAdpcmBuf ;
                    if(Remain_pcm <= data_size )
                    {
                        cpSz = Remain_pcm;
                        g_apbsAdpcmRemainSize = data_size -Remain_pcm;
                        Remain_pcm =0;
                    }
                    else
                    {
                        cpSz = data_size;
                        Remain_pcm =  Remain_pcm - data_size;
                        g_apbsAdpcmRemainSize=0;
                    }


                    for (i = 0 ; i<(cpSz/2) ; i++)
                    {
                        *PCM_d=*g_apbsAdpcmBufRead;
                        g_apbsAdpcmBufRead++;
                        PCM_d++;
                    }

                }

            }

        }
        else
        {
            PCM_d=(INT16 *)cpBufPos;
            PCM_s=(UINT8 *)g_apbsBufferCurPos;

            for (i = 0 ; i<(cpSz/2) ; i++)
            {
                *PCM_d= ((INT16)((INT16)(*PCM_s)-0x80)) <<8;
                PCM_d++;
                PCM_s++;
                *PCM_d= ((INT16)((INT16)(*PCM_s)-0x80)) <<8;
                PCM_d++;
                PCM_s++;
            }

            cpBufPos=cpBufPos+(cpSz/2);
            g_apbsBufferCurPos=g_apbsBufferCurPos+(cpSz/4);
        }

        if (g_apbsSwIdx == 0)
        {
            PCM_d = (INT16 *)g_apbsDecInput;
        }
        else
        {
            PCM_d = (INT16 *)(g_apbsDecInput + WAVPCMBUFSIZE/8); // half size in UINT32
        }
        INT16 alg_gain = aud_GetOutAlgGainDb2Val();

        for(i = 0; i < (cpSz>>1); i++)
        {
            PCM_d[i] = ((INT32)PCM_d[i] * alg_gain) >>14;
        }
        if (g_apbsSwIdx == 0)
            g_apbsSwIdx = 1;
        else
            g_apbsSwIdx = 0;
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
	if (g_apbsSwIdx == 1) //in this function begin,  swap it
	{
             hal_DcacheFlushAddrRange(g_apbsDecInput, g_apbsDecInput + WAVPCMBUFSIZE/8, FALSE);
	}
	else
	{
             hal_DcacheFlushAddrRange( g_apbsDecInput + WAVPCMBUFSIZE/8, g_apbsDecInput + WAVPCMBUFSIZE/4, FALSE);
	}

#endif

        // TODO Call VPP's function that does processing
        // on VPP in case of processing to be made ...
        g_apbsFramesPlayedNb++;

        // Half buffer reached
        if (((UINT32)g_apbsBufferCurPos >= (UINT32)g_apbsBufferStart + (g_apbsBufferLength/2))
                && !g_apbsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_apbsMidBufDetected = TRUE;
        }
    }
    // Both of them shouldn't happen during the same iteration
    // of this function call.
    APBS_ASSERT(!(halfBufferReached & endBufferReached), "Half buffer and End of buffer reached in the same run");

    if (halfBufferReached)
    {
        if (g_apbsUserHandler)
        {
            g_apbsUserHandler(APBS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_apbsUserHandler)
        {
            g_apbsUserHandler(APBS_STATUS_END_BUFFER_REACHED);
        }
    }

    APBS_PROFILE_FUNCTION_EXIT(apbs_XXXSpeechHandler);
}


// =============================================================================
// apbs_RawPcmHandler
// -----------------------------------------------------------------------------
/// Handler for the RAW module, to play PCM  streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID apbs_Stream_RawPcmHandler(VOID)
{
    // Number of bytes to copy;
    UINT32 cpSz;
    UINT32 i;
//    MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] STREAM RAW PCM, g_apbsBufferRemainingBytes=%d", g_apbsBufferRemainingBytes);
    // Position in the copy buffer, where data from the stream
    // are going to be copied
    UINT32* cpBufPos;
    INT16 *PCM_d;
    UINT8 *PCM_s;
    UINT32 status;

    APBS_PROFILE_FUNCTION_ENTER(apbs_XXXSpeechHandler);

    /// Special operation according to state
    ///
    /// if (sampleNumber == ...)
    /// ...
    /// ...
    if (g_apbsFramesPlayedNb == APBS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_SETUP);
        //  aud_Setup(audioItf, &g_apbsAudioCfg);
    }


    {
        if(g_apbsBufferRemainingBytes <g_apbsFrameSize)
        {
            if (g_apbsFinishData)
            {
                // Stop the play
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,"STOP:: Remaining < FrameSize");

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_STOP);
                //                apbs_Stop();

                //send message to caller.
                if (MCI_RingFinished!=NULL)
                {
                    MCI_RingFinished(MCI_ERR_END_OF_FILE);
                }

            }
            else
            {

                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_REQUEST);

            }
            // Leave
            return;

        }

        status = hal_SysEnterCriticalSection();
        // Decrease size in bytes
        g_apbsBufferRemainingBytes -= g_apbsFrameSize;
        hal_SysExitCriticalSection(status);

        if (g_apbsSwIdx == 0)
        {
            cpBufPos = g_apbsDecInput;
            g_apbsSwIdx = 1;
        }
        else
        {
            // == 1
            cpBufPos =  g_apbsDecInput + WAVPCMBUFSIZE/8; // half size in UINT32
            g_apbsSwIdx = 0;
        }

//   MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS]apbs_Stream_RawPcmHandler:: g_apbsBufferCurPos: %x g_apbsBufferRemainingBytes=%d\n", g_apbsBufferCurPos, g_apbsBufferRemainingBytes);

        ///
        if ((((UINT32)g_apbsBufferCurPos) + (g_apbsFrameSize)) > (UINT32)g_apbsBufferTop)
        {


            // else wrap ...
            // Copy first part
            cpSz = (UINT32)(g_apbsBufferTop - g_apbsBufferCurPos);

            if (g_apbsBitsPerSample==16)
            {
                for (i = 0 ; i<(cpSz/4) ; i++)
                {
                    *cpBufPos = *g_apbsBufferCurPos;
                    g_apbsBufferCurPos++;
                    cpBufPos++;
                }

                cpSz = g_apbsFrameSize - (cpSz/4)*4;
            }
            else
            {
                PCM_d=(INT16 *)cpBufPos;
                PCM_s=(UINT8 *)g_apbsBufferCurPos;

                for (i = 0 ; i<(cpSz/2) ; i++)
                {
                    *PCM_d= ((INT16)((*PCM_s)-0x80)) <<8;
                    PCM_d++;
                    PCM_s++;
                    *PCM_d= ((INT16)((*PCM_s)-0x80)) <<8;
                    PCM_d++;
                    PCM_s++;
                }

                cpBufPos=cpBufPos+(cpSz/2);
                g_apbsBufferCurPos=g_apbsBufferCurPos+(cpSz/4);
                cpSz = g_apbsFrameSize - (cpSz/2)*2;
            }

            g_apbsBufferCurPos = g_apbsBufferStart;
        }
        else
        {
            cpSz = g_apbsFrameSize;
        }

        // Finish or full copy
        if (g_apbsBitsPerSample==16)
        {
            for (i = 0 ; i<(cpSz/4) ; i++)
            {
                *cpBufPos= *g_apbsBufferCurPos;
                g_apbsBufferCurPos++;
                cpBufPos++;
            }
        }
        else
        {
            PCM_d=(INT16 *)cpBufPos;
            PCM_s=(UINT8 *)g_apbsBufferCurPos;

            for (i = 0 ; i<(cpSz/2) ; i++)
            {
                *PCM_d= ((INT16)((INT16)(*PCM_s)-0x80)) <<8;
                PCM_d++;
                PCM_s++;
                *PCM_d= ((INT16)((INT16)(*PCM_s)-0x80)) <<8;
                PCM_d++;
                PCM_s++;
            }

            cpBufPos=cpBufPos+(cpSz/2);
            g_apbsBufferCurPos=g_apbsBufferCurPos+(cpSz/4);
        }

        // TODO Call VPP's function that does processing
        // on VPP in case of processing to be made ...
        g_apbsFramesPlayedNb++;

//        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] apbs_Stream_RawPcmHandle late: g_apbsBufferCurPos= %x  g_apbsBufferRemainingBytes= %d\n", g_apbsBufferCurPos, g_apbsBufferRemainingBytes);

#if(XCPU_CACHE_MODE_WRITE_BACK==1)
	if (g_apbsSwIdx == 1) //in this function begin,  swap it
	{
             hal_DcacheFlushAddrRange(g_apbsDecInput, g_apbsDecInput + WAVPCMBUFSIZE/8, FALSE);
	}
	else
	{
             hal_DcacheFlushAddrRange( g_apbsDecInput + WAVPCMBUFSIZE/8, g_apbsDecInput + WAVPCMBUFSIZE/4, FALSE);
	}

#endif



        if ((UINT32)g_apbsBufferCurPos == (UINT32)g_apbsBufferTop)
        {
            g_apbsBufferCurPos = g_apbsBufferStart;
        }

        if (g_apbsBufferRemainingBytes <g_apbsFrameSize*16)
        {
            mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_REQUEST);
        }

        if (g_apbsBufferRemainingBytes <g_apbsBufferLength/2)
        {
            if (g_apbsFramesPlayedNb%20 == 1)
            {
                mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), APBS_REQUEST);
            }
        }

    }
    // Both of them shouldn't happen during the same iteration
    // of this function call.


    APBS_PROFILE_FUNCTION_EXIT(apbs_XXXSpeechHandler);
}

// =============================================================================
// apbs_Setup
// -----------------------------------------------------------------------------
/// Configure the apbs service..
///
/// This functions configures the playing of a stream on a given audio interface:
/// gain for the side tone and
/// the speaker, input selection for the microphone and output selection for
/// the speaker... (TODO complete)
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #AUD_LEVEL_T for more details.
/// @return #APBS_ERR_NO it can execute the configuration.
// =============================================================================
PUBLIC APBS_ERR_T apbs_Setup(AUD_ITF_T itf, CONST APBS_AUDIO_CFG_T* cfg)
{
    AUD_ERR_T audErr;
    // Set decoder config
    // ...
    // Set audio interface
    g_apbsAudioCfg.spkLevel = cfg->spkLevel;
    g_apbsAudioCfg.micLevel = AUD_MIC_MUTE;
    g_apbsAudioCfg.sideLevel = 0;
    g_apbsAudioCfg.toneLevel = 0;
    audErr = aud_Setup(itf, &g_apbsAudioCfg);

    switch (audErr)
    {
        // TODO Add error as they comes
        case AUD_ERR_NO:
            return APBS_ERR_NO;
            break;
        default:
            return APBS_ERR_QTY; // ...unknown error ...
            break;
    }
}


// =============================================================================
// apbs_Play
// -----------------------------------------------------------------------------
/// Start the playing of a stream.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #apbs_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. It can
/// loop according to the caller choice. If a handler is defined, it is expected
/// that the handler will refeed data and call #apbs_AddedData to tell APBS about
/// that. If there is no handler, in loop mode, the buffer will be played in loop
/// without expecting new data.
/// APBS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop \c TRUE if the buffer is played in loop mode.
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #APBS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC APBS_ERR_T apbs_Play(
    CONST AUD_ITF_T      itf,
    CONST APBS_ENC_STREAM_T* stream,
    CONST APBS_AUDIO_CFG_T*  cfg,
    BOOL loop)
{
    // TODO
    // BIG switch for PCM and/or other codec
    // Initial audio confguration
    //   AUD_LEVEL_T audioCfg;
    HAL_AIF_STREAM_T pcmStream;
    AUD_ERR_T audErr;
    UINT32 i;
    WAV_INPUT *input = &g_wavPlayer;


    UINT32 status = hal_SysEnterCriticalSection();
    while (g_apbsConfigState != APBS_NULL)
    {
        sxr_Sleep(32);
    }
    g_apbsConfigState = APBS_BEING_OPENED;
    hal_SysExitCriticalSection(status);

    APBS_PROFILE_FUNCTION_ENTER(apbs_Play);

    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]apbs_Play::g_apbsBufferRemainingBytes= %d",g_apbsBufferRemainingBytes);
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]  Stream - buffer %#x  %d codec %#x", stream->startAddress, stream->length,stream->mode);

#ifdef MP3_PLAYERBUFFER_AGC

    AGC_Coeff_l = 0x7fff*2*4;

#endif


    // Register global var
    g_apbsItf = itf;

    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] g_apbsItf=%d audioItf=%d",g_apbsItf,audioItf);

    if ((stream->handler == NULL) || (loop== FALSE))
    {
        g_apbsBufferStart                     = (UINT32*) stream->startAddress;
        g_apbsBufferTop                       = (UINT32*)(stream->startAddress + stream->length);
        g_apbsBufferCurPos                    = (UINT32*) stream->startAddress;
        g_apbsBufferRemainingBytes            = stream->length;
        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]apbs_Play entry set bytes");
    }

    g_apbsBufferLength                    = stream->length;
    g_apbsMidBufDetected                  = FALSE;
    g_apbsFramesPlayedNb                   = 0;
    g_apbsUserHandler                     = stream->handler;
    g_apbsAudioCfg.spkLevel               = cfg->spkLevel;
    g_apbsAudioCfg.micLevel               = AUD_MIC_MUTE;
    g_apbsAudioCfg.sideLevel              = 0;
    g_apbsAudioCfg.toneLevel              = 0;
    g_apbsLoopMode                        = loop;
    g_apbsSpeechUsed                      = FALSE;
    g_apbsAudioUsed = FALSE;
    g_apbsFinishData  = FALSE;

    if (mmc_GetCurrMode()!=MMC_MODE_CAMERA)
    {
        // set mode to analog TV.
        mmc_SetCurrMode(MMC_MODE_APBS);
    }

    // put the PCM buffer in the internSRAM.
    // hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);
    //initialize the pointer of PCM buffer.
    g_apbsPcmBuf=(UINT32 *)pcmbuf_overlay;
    g_apbsaudiomode=stream->mode;
    g_apbsSwIdx = 0;
    g_apbsVoCRunFlag = FALSE;

#ifdef SMALL_BSS_RAM_SIZE

    g_apbsBufferForLoopmode_alloc = (UINT8*)mmc_MemMalloc_BssRam((g_apbsVoCIntBufSize<<1)+3);

    g_apbsBufferForLoopmode = (UINT32*)((UINT32)(g_apbsBufferForLoopmode_alloc +3) & ~0x3);
    hal_HstSendEvent(0xaccc2222);
    hal_HstSendEvent(g_apbsBufferForLoopmode_alloc);
    hal_HstSendEvent(g_apbsBufferForLoopmode);
#endif

    if (audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER&&audioItf!=AUD_ITF_BLUETOOTH)
    {
        // Initial cfg
        g_apbsItf= audioItf = AUD_ITF_LOUD_SPEAKER;
    }

    switch (stream->mode)
    {
            HAL_SPEECH_DEC_IN_T decStruct;
            VPP_SPEECH_AUDIO_CFG_T vppCfg;
        case MCI_PLAY_MODE_AAC:
        case MCI_PLAY_MODE_MP3:
            memset(g_apbsPcmBuf,0,MP3PCMBUFSIZE);//set max
            //   g_apbsOutBufferCurPos=g_apbsPcmBuf+(g_apbsPcmStereoBufSize/8);  // half size in UINT32;
            //   initialize the Counters.
            g_apbsOutCountBytes=0;
            g_apbsErrNb=0;
            g_apbsBufferEndFlag = FALSE;
            // Audio is used
            g_apbsAudioUsed = TRUE;
            //First we will paly audio stream in the extern buffer.
            g_apbsDataInTempBuffer= FALSE;
            //  In mp3 Mode,we must open audio device after decoding some frame for samplerate,etc.
            g_apbsAudioDeviceOpened=FALSE;

            // config the input parometers that can not change in playing except the reset;
            if (stream->mode==MCI_PLAY_MODE_AAC)
            {
                g_apbsVppAmjpAudioCfg.mode  = MMC_AAC_DECODE;
                g_apbsPcmStereoBufSize=AACPCMBUFSIZE;
            }
            else
            {
                g_apbsVppAmjpAudioCfg.mode  = MMC_MP3_DECODE;
                g_apbsPcmStereoBufSize=MP3PCMBUFSIZE;
            }

            g_apbsVppAmjpAudioCfg.reset     = 1;
            g_apbsVppAmjpAudioCfg.BsbcEnable     = 0;
            g_apbsVppAmjpAudioCfg.EQ_Type    = -1;
            g_apbsVppAmjpAudioCfg.audioItf   = audioItf;


            g_apbsOutBufferCurPos=g_apbsPcmBuf;  // half size in UINT32;
            if ((g_apbsUserHandler != NULL)&&g_apbsLoopMode)
            {
                // Open Amjp
                if (HAL_ERR_RESOURCE_BUSY == vpp_AudioJpegDecOpen(apbs_StreamVppAmjpHandler))
                {
                    MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS]APBS_ERR_RESOURCE_BUSY");
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return APBS_ERR_RESOURCE_BUSY;
                }
            }
            else
            {
                // Open Amjp
                if (HAL_ERR_RESOURCE_BUSY == vpp_AudioJpegDecOpen(apbs_VppAmjpHandler))
                {
                    MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS]APBS_ERR_RESOURCE_BUSY");
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return APBS_ERR_RESOURCE_BUSY;
                }
            }

            //waiting......
            while ((g_apbsAudioDeviceOpened==FALSE)&&(g_apbsErrNb< APBS_ERR_NB_MAX)&&(g_apbsBufferEndFlag==FALSE))
            {
                MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS]Sleep!");
                sxr_Sleep(50);
            }

            vpp_AudioMP3DecSetReloadFlag();

            if (g_apbsAudioDeviceOpened==TRUE)
            {
                // Set audio interface
                pcmStream.startAddress = g_apbsPcmBuf;
                pcmStream.length = g_apbsPcmStereoBufSize;
                pcmStream.sampleRate =g_apbsSampleRate;
                pcmStream.channelNb = g_apbsChannel;
                pcmStream.voiceQuality = FALSE;
                pcmStream.playSyncWithRecord = FALSE;
                // initialize the interrupt function.
                pcmStream.halfHandler = apbs_AmjpPcmHalfHandler;
                pcmStream.endHandler = apbs_AmjpPcmEndHandler;
            }
            else
            {
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS]ERROR FORMAT!");
                vpp_AudioJpegDecClose();
                APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                g_apbsConfigState = APBS_NULL;
                return MCI_ERR_INVALID_FORMAT;
            }

            break;
        case MCI_PLAY_MODE_PCM:

            input->codec        =   1;
            memset(g_apbsPcmBuf,0,MP3PCMBUFSIZE);//set max
            g_apbsBitsPerSample=16;
            g_apbsFrameSize = WAVPCMBUFSIZE/2;
            // Typical global vars
            g_apbsDecInput = g_apbsPcmBuf; //(UINT32*)vpp_SpeechGetRxPcmBuffer();
            // PCM Audio stream, output of VPP
            pcmStream.startAddress = g_apbsDecInput;
            pcmStream.length = WAVPCMBUFSIZE;
            pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            pcmStream.channelNb = HAL_AIF_MONO;
            pcmStream.voiceQuality = TRUE;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = apbs_RawPcmHandler;
            pcmStream.endHandler =  apbs_RawPcmHandler;
            break;

        case MCI_PLAY_MODE_STREAM_PCM:
        {

            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] SampleRate: %d\n", stream->sampleRate);
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] nbChannel: %d\n", stream->channelNb);
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] wBitsPerSample: %d\n", stream->bitPerSample);
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] g_apbsBufferRemainingBytes: %d\n",g_apbsBufferRemainingBytes);
            memset(g_apbsPcmBuf,0,MP3PCMBUFSIZE);//set max

            if (stream->bitPerSample == 8)
            {
                g_apbsFrameSize = WAVPCMBUFSIZE/4;
                g_apbsBitsPerSample= 8;
            }
            else if (stream->bitPerSample==16)
            {
                g_apbsBitsPerSample= 16;
                g_apbsFrameSize = WAVPCMBUFSIZE/2;
            }
            else
            {
                diag_printf("[APBS]BitsPerSample error!:%d",stream->bitPerSample);
                APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                g_apbsConfigState = APBS_NULL;
                return MCI_ERR_BAD_FORMAT;
            }


            // Typical global vars
            g_apbsDecInput = g_apbsPcmBuf; //(UINT32*)vpp_SpeechGetRxPcmBuffer();

            // PCM Audio stream, output of VPP
            pcmStream.startAddress = g_apbsDecInput;
            pcmStream.length = WAVPCMBUFSIZE;
            pcmStream.sampleRate = stream->sampleRate;
            pcmStream.channelNb = stream->channelNb;
            pcmStream.voiceQuality = FALSE;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = apbs_Stream_RawPcmHandler;
            pcmStream.endHandler =  apbs_Stream_RawPcmHandler;

        }
        break;

        case MCI_PLAY_MODE_WAV:
        {
            WAVHeader WAVHeaderInfo;
            WAVDataHeader WAVDataHeaderInfo;
            UINT8 OptionalInfo[2];
            UINT8 ChunkInfoID[4];
            UINT32 ChunkInfoSize;
            UINT32 ChunkInfoData;
            UINT32 g_BufferWAVHeaderLength=0;


            memset(g_apbsPcmBuf,0,MP3PCMBUFSIZE);//set max
            UINT8 *WAVbufferStart=(UINT8 *)g_apbsBufferStart;
            memcpy((UINT8 *)&WAVHeaderInfo, WAVbufferStart, sizeof(WAVHeader));
            WAVbufferStart=WAVbufferStart+sizeof(WAVHeader);
            g_BufferWAVHeaderLength+=36;
            /* Memory allocation map for ADPCM decoder
            *
            *                                                           pcmbuf_overlay [4608]*4 bytes
            *|-----------------------------------------------------------------------------------------------------|
            *      g_apbsPcmBuf[ ]                            g_apbsAdpcmBuf[]               priv_data sizeof    extradata
            *|WAVPCMBUFSIZE=2*1024bytes |WAVPCMBUFSIZE=2*1024byte   |(ADPCMContext)  | < 20bytes
            *|---------------------------|---------------------------|---------------|----------|
            *
            *
            */
            if(WAVHeaderInfo.wFormatTag == 1  )   //PCM format wav
            {
                if ((memcmp(WAVHeaderInfo.szRiff,"RIFF",4)!=0)
                        ||(memcmp(WAVHeaderInfo.szWaveFmt,"WAVEfmt ",8) != 0)
                        ||((WAVHeaderInfo.nChannels != 1)&&(WAVHeaderInfo.nChannels != 2))
                        ||((WAVHeaderInfo.wBitsPerSample != 8)&&(WAVHeaderInfo.wBitsPerSample != 16))
                        ||(WAVHeaderInfo.nAvgBytesPerSec != WAVHeaderInfo.nChannels*WAVHeaderInfo.wBitsPerSample*WAVHeaderInfo.nSamplesPerSec/8)
                        ||(WAVHeaderInfo.nBlockAlign != WAVHeaderInfo.nChannels*WAVHeaderInfo.wBitsPerSample/8))
                {
                    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_APBS_WAV]parse header error\n");
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return MCI_ERR_BAD_FORMAT;
                }
            }
            else  //ADPCM format
            {
                if ((memcmp(WAVHeaderInfo.szRiff,"RIFF",4)!=0)
                        ||(memcmp(WAVHeaderInfo.szWaveFmt,"WAVEfmt ",8) != 0)
                        ||((WAVHeaderInfo.nChannels != 1)&&(WAVHeaderInfo.nChannels != 2)))
                {
                    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_APBS_WAV]parse ADPCM header error\n");
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return MCI_ERR_BAD_FORMAT;
                }
            }


            if (WAVHeaderInfo.dwFmtSize>=18)  /* We're obviously dealing with WAVEFORMATEX */
            {
                UINT32 cbSize,size;
                size =WAVHeaderInfo.dwFmtSize -18;

                memcpy((UINT8 *)&cbSize, WAVbufferStart, 2);
                cbSize = MIN(size, cbSize);
                WAVbufferStart+=2;
                g_BufferWAVHeaderLength+=2;

                if(cbSize > 0)
                {
                    if( (cbSize >=22)&&( WAVHeaderInfo.wFormatTag==0xfffe ))  /* WAVEFORMATEXTENSIBLE */
                    {
                        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_APBS_WAV] ERROR!!!:  Don't support  WAVEFORMATEXTENSIBLE mode!!\n");
                        return MCI_ERR_BAD_FORMAT;
                    }
                    input->extradata_size = cbSize;
                    input->extradata =  (uint8 *)g_apbsPcmBuf + 2*WAVPCMBUFSIZE+ sizeof(ADPCMContext);//refer to adpcm memory allocation map


                    memcpy(input->extradata, WAVbufferStart, input->extradata_size);
                    WAVbufferStart+=input->extradata_size;
                    g_BufferWAVHeaderLength+=input->extradata_size;

                }

                size -= cbSize;
                /* It is possible for the chunk to contain garbage at the end */
                if (size > 0)
                {

                    WAVbufferStart+=size;
                    g_BufferWAVHeaderLength+=size;

                }

            }


            //check samplerate
            for (i=0; i<sizeof(WAVSamplesRate)/sizeof(WAVSamplesRate[0]); i++)
            {
                if (WAVHeaderInfo.nSamplesPerSec==WAVSamplesRate[i])
                {
                    break;
                }
            }

            if (i>=sizeof(WAVSamplesRate)/sizeof(WAVSamplesRate[0]))
            {
                APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                g_apbsConfigState = APBS_NULL;
                return MCI_ERR_INVALID_FORMAT;
            }

            memcpy((UINT8 *)ChunkInfoID, WAVbufferStart, 4);
            WAVbufferStart+=4;
            g_BufferWAVHeaderLength+=4;

            if (memcmp(ChunkInfoID,"fact",4)==0)
            {
                memcpy((UINT8 *)&ChunkInfoSize, WAVbufferStart, 4);
                WAVbufferStart+=4;
                g_BufferWAVHeaderLength+=4;

                if (ChunkInfoSize!=4)
                {
                    diag_printf("[APBS]Fact Chunk Size Read Error!:%d\n",ChunkInfoSize);
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return MCI_ERR_ERROR;
                }

                memcpy((UINT8 *)&ChunkInfoData, WAVbufferStart, 4);
                WAVbufferStart+=4;
                g_BufferWAVHeaderLength+=4;
                memcpy((UINT8 *)&WAVDataHeaderInfo, WAVbufferStart, sizeof(WAVDataHeader));
                WAVbufferStart+=sizeof(WAVDataHeader);
                g_BufferWAVHeaderLength+=sizeof(WAVDataHeader);
            }
            else if (memcmp(ChunkInfoID,"data",4)==0)
            {
                WAVDataHeaderInfo.szData[0]=ChunkInfoID[0];
                WAVDataHeaderInfo.szData[1]=ChunkInfoID[1];
                WAVDataHeaderInfo.szData[2]=ChunkInfoID[2];
                WAVDataHeaderInfo.szData[3]=ChunkInfoID[3];
                memcpy((UINT8 *)&WAVDataHeaderInfo.dwDataSize, WAVbufferStart, 4);
                WAVbufferStart+=4;
                g_BufferWAVHeaderLength+=4;
            }
            else
            {
                diag_printf("[APBS]data header error!\n");
                APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                g_apbsConfigState = APBS_NULL;
                return MCI_ERR_BAD_FORMAT;
            }

            if (WAVDataHeaderInfo.dwDataSize == 0)
            {
                APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                g_apbsConfigState = APBS_NULL;
                return MCI_ERR_BAD_FORMAT;
            }

            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] SampleRate: %d\n", WAVHeaderInfo.nSamplesPerSec);
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] nbChannel: %d\n", WAVHeaderInfo.nChannels);
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] wBitsPerSample: %d\n", WAVHeaderInfo.wBitsPerSample);
            memset(g_apbsPcmBuf,0,MP3PCMBUFSIZE);//set max


            input->codec        =   WAVHeaderInfo.wFormatTag;
            input->codec_id    =  wav_codec_get_id(WAVHeaderInfo.wFormatTag, WAVHeaderInfo.wBitsPerSample);
            input->block_align = WAVHeaderInfo.nBlockAlign;
            input->channels = (uint8)WAVHeaderInfo.nChannels;

            if(input->codec != 1  ) //ADPCM
            {
                /*refer to adpcm memory allocation map*/
                g_apbsAdpcmBuf = (INT16 *)g_apbsPcmBuf + WAVPCMBUFSIZE/2;

                g_apbsAdpcmBufRead= NULL;
                g_apbsAdpcmRemainSize= 0;
                input->priv_data   =  (INT16 *)g_apbsPcmBuf + 2*WAVPCMBUFSIZE/2;

                adpcm_decode_init(input );

                diag_printf("[MMC_ADPCM]  Finished adpcm_decode_init:  codec_id is  0x%x    \n", input->codec_id);
            }

            if ((WAVHeaderInfo.wBitsPerSample==4) &&(WAVHeaderInfo.wFormatTag != 1  ))
            {
                g_apbsBitsPerSample= 16;
                g_apbsFrameSize = WAVHeaderInfo.nBlockAlign;
            }
            else if (WAVHeaderInfo.wBitsPerSample == 8)
            {
                g_apbsFrameSize = WAVPCMBUFSIZE/4;
                g_apbsBitsPerSample= 8;
            }
            else if (WAVHeaderInfo.wBitsPerSample==16)
            {
                g_apbsBitsPerSample= 16;
                g_apbsFrameSize = WAVPCMBUFSIZE/2;
            }
            else
            {
                diag_printf("[APBS]BitsPerSample error!:%d",WAVHeaderInfo.wBitsPerSample);
                APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                g_apbsConfigState = APBS_NULL;
                return MCI_ERR_BAD_FORMAT;
            }

            MCI_TRACE(MCI_AUDIO_TRC,0,"[APBS] g_BufferWAVHeaderLength: %d", g_BufferWAVHeaderLength);

            if ((g_BufferWAVHeaderLength&0x3)!=0)
            {
                g_BufferWAVHeaderLength=(g_BufferWAVHeaderLength+4)&0xfffffffc;
            }

            g_apbsBufferStart                     = (UINT32*)(stream->startAddress+g_BufferWAVHeaderLength);
            g_apbsBufferTop                       = (UINT32*)(stream->startAddress + stream->length-g_BufferWAVHeaderLength);
            g_apbsBufferCurPos                    = (UINT32*)(stream->startAddress+g_BufferWAVHeaderLength);
            g_apbsBufferRemainingBytes            = stream->length-g_BufferWAVHeaderLength;
            g_apbsBufferLength                    = stream->length-g_BufferWAVHeaderLength;
            // Typical global vars
            g_apbsDecInput = g_apbsPcmBuf; //(UINT32*)vpp_SpeechGetRxPcmBuffer();
            // PCM Audio stream, output of VPP
            pcmStream.startAddress = g_apbsDecInput;
            pcmStream.length = WAVPCMBUFSIZE;//sizeof(HAL_SPEECH_PCM_BUF_T);
            pcmStream.sampleRate = WAVHeaderInfo.nSamplesPerSec;
            pcmStream.channelNb = WAVHeaderInfo.nChannels;
            pcmStream.voiceQuality = FALSE;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = apbs_RawPcmHandler;
            pcmStream.endHandler =  apbs_RawPcmHandler;
            break;
        }
        default:
            // Speech is used
            g_apbsSpeechUsed                      = TRUE;
            // vpp speech congiguration structure
            g_apbsVppCfg.echoEsOn                 = 0;
            g_apbsVppCfg.echoEsVad                = 0;
            g_apbsVppCfg.echoEsDtd                = 0;
            g_apbsVppCfg.echoExpRel               = 0;
            g_apbsVppCfg.echoExpMu                = 0;
            g_apbsVppCfg.echoExpMin               = 0;
            g_apbsVppCfg.encMute                  = VPP_SPEECH_MUTE;
            g_apbsVppCfg.decMute                  = VPP_SPEECH_UNMUTE;
            g_apbsVppCfg.sdf                      = NULL;
            g_apbsVppCfg.mdf                      = NULL;
            g_apbsVppCfg.if1                      = 0;
            decStruct.dtxOn = 0;
            // Codec mode depends on the codec used by the stream
            // and will be set in the switch below
            decStruct.decFrameType = 0;
            decStruct.bfi = 0;
            decStruct.sid = 0;
            decStruct.taf = 0;
            decStruct.ufi = 0;

            // Size is a multiple of 4.
            for (i=0; i<HAL_SPEECH_FRAME_SIZE_COD_BUF/4; i++)
            {
                *((UINT32*)decStruct.decInBuf + i) = 0;
            }

            // AMR mode
            g_apbsAmrRing = FALSE;

            // Specific config
            switch (stream->mode)
            {
                case MCI_PLAY_MODE_AMR475:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR475;
                    decStruct.codecMode = HAL_AMR475_DEC;
                    break;
                case MCI_PLAY_MODE_AMR515:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR515;
                    decStruct.codecMode = HAL_AMR515_DEC;
                    break;
                case MCI_PLAY_MODE_AMR59:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR59;
                    decStruct.codecMode = HAL_AMR59_DEC;
                    break;
                case MCI_PLAY_MODE_AMR67:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR67 ;
                    decStruct.codecMode = HAL_AMR67_DEC;
                    break;
                case MCI_PLAY_MODE_AMR74:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR74 ;
                    decStruct.codecMode = HAL_AMR74_DEC;
                    break;
                case MCI_PLAY_MODE_AMR795:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR795;
                    decStruct.codecMode = HAL_AMR795_DEC;
                    break;
                case MCI_PLAY_MODE_AMR102:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR102;
                    decStruct.codecMode = HAL_AMR102_DEC;
                    break;
                case MCI_PLAY_MODE_AMR122:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR122;
                    decStruct.codecMode = HAL_AMR122_DEC;
                    break;
                case MCI_PLAY_MODE_AMR_RING:
                    //          g_apbsBufferStart                     = (UINT32*) (stream->startAddress);
                    //          g_apbsBufferTop                       = (UINT32*)(stream->startAddress + stream->length);
                    //          g_apbsBufferCurPos                    = (UINT32*) (stream->startAddress);
                    //          g_apbsBufferRemainingBytes            = stream->length;
                    //          g_apbsBufferLength                    = stream->length;
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR_RING;
                    decStruct.codecMode = HAL_AMR122_DEC;
                    g_apbsVppCfg.if1    = 1;
                    g_apbsAmrRing = TRUE;
                    break;
                case MCI_PLAY_MODE_EFR:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_EFR  ;
                    decStruct.codecMode = HAL_EFR;
                    break;
                case MCI_PLAY_MODE_HR:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_HR ;
                    decStruct.codecMode = HAL_HR;
                    break;
                case MCI_PLAY_MODE_FR:
                    g_apbsFrameSize = HAL_SPEECH_FRAME_SIZE_FR;
                    decStruct.codecMode = HAL_FR;
                    break;
                default:
                    APBS_ASSERT(FALSE, "[APBS]Unsupported mode in APBS:%d", stream->mode);
            }

            // PCM Audio stream, output of VPP
            pcmStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
            pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
            pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
            pcmStream.channelNb = HAL_AIF_MONO;
            pcmStream.voiceQuality = FALSE;
            pcmStream.playSyncWithRecord = FALSE;
            pcmStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
            pcmStream.endHandler = NULL; // Mechanical interaction with VPP's VOC
            // VPP audio config
            vppCfg.echoEsOn = 0;
            vppCfg.echoEsVad = 0;
            vppCfg.echoEsDtd = 0;
            vppCfg.echoExpRel = 0;
            vppCfg.echoExpMu = 0;
            vppCfg.echoExpMin = 0;
            vppCfg.encMute = VPP_SPEECH_MUTE;
            vppCfg.decMute = VPP_SPEECH_MUTE;
            vppCfg.sdf = NULL;
            vppCfg.mdf = NULL;
            vppCfg.if1 = g_apbsVppCfg.if1;
            // Clear the PCM buffer
            memset(pcmStream.startAddress, 0, pcmStream.length);
            // configure AUD and VPP buffers
            // set vpp

            if ((g_apbsUserHandler != NULL)&&g_apbsLoopMode)
            {
                if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(apbs_StreamSpeechHandler, VPP_SPEECH_WAKEUP_HW_DEC))
                {
                    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] VoC busy!");
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return APBS_ERR_RESOURCE_BUSY;
                }

                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "ser callback == apbs_StreamSpeechHandler");
            }
            else
            {
                if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(apbs_VppSpeechHandler, VPP_SPEECH_WAKEUP_HW_DEC))
                {
                    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] VoC busy!");
                    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
                    g_apbsConfigState = APBS_NULL;
                    return APBS_ERR_RESOURCE_BUSY;
                }
            }

            MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] VoC Opened!");
            vpp_SpeechAudioCfg(&vppCfg);
            vpp_SpeechSetOutAlgGain(0);
            // Set all the preprocessing modules
            vpp_SpeechSetEncDecPocessingParams(g_apbsItf, g_apbsAudioCfg.spkLevel, 0);
            // Init the Rx buffer
            *(vpp_SpeechGetRxCodBuffer()) = decStruct;
            // Pointer to the stream buffer in VPP decoding buffer
            g_apbsDecInput = (UINT32*) decStruct.decInBuf;
            break;
    }

    MCI_TRACE(MCI_AUDIO_TRC,0,"[APBS] SampleRate: %d\n", pcmStream.sampleRate);
    MCI_TRACE(MCI_AUDIO_TRC,0,"[APBS] nbChannel: %d\n", pcmStream.channelNb);
    MCI_TRACE(MCI_AUDIO_TRC,0,"[APBS] PcmBufSize: %d\n", pcmStream.length);
    // Set audio interface
    //   audioCfg.spkLevel = AUD_SPK_MUTE;
    //   audioCfg.micLevel = AUD_MIC_MUTE;
    //   audioCfg.sideLevel = 0;
    //   audioCfg.toneLevel = 0;

    // ...
    g_apbsAudioCfg.spkLevel               = AUD_SPK_MUTE;
    audErr = aud_StreamStart(audioItf, &pcmStream, &g_apbsAudioCfg);
    g_apbsAudioCfg.spkLevel               = cfg->spkLevel;

    // profit
    switch (audErr)
    {
        case AUD_ERR_NO:
            APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
            g_apbsConfigState = APBS_NULL;
            return APBS_ERR_NO;
            break; // :)
        default:
            MCI_TRACE(MCI_AUDIO_TRC,0, "[APBS] Start failed ::audErr= %d!!!", audErr);
            APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
            g_apbsConfigState = APBS_NULL;
            return APBS_ERR_UNKNOWN;
            break; // :)
    }

    // the code is already loaded
    APBS_PROFILE_FUNCTION_EXIT(apbs_Play);
    g_apbsConfigState = APBS_NULL;
    return APBS_ERR_NO;
}



// =============================================================================
// apbs_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #APBS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APBS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC APBS_ERR_T apbs_Stop(VOID)
{
    APBS_PROFILE_FUNCTION_ENTER(apbs_Stop);
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] apbs_Stop!");
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] g_apbsItf=%d audioItf=%d",g_apbsItf,audioItf);


    UINT32 status = hal_SysEnterCriticalSection();
    while (g_apbsConfigState != APBS_NULL)
    {
        sxr_Sleep(32);
    }
    g_apbsConfigState = APBS_BEING_CLOSED;
    hal_SysExitCriticalSection(status);

    if (AUD_ITF_NONE == g_apbsItf)
    {
        APBS_PROFILE_FUNCTION_EXIT(apbs_Stop);
        g_apbsConfigState = APBS_NULL;
        return APBS_ERR_NO;
    }

    aud_Setup(g_apbsItf, &g_apbsAudioCfg);
    // stop stream ...
    aud_StreamStop(g_apbsItf);

    // VPP Speech is used ?
    if (g_apbsSpeechUsed)
    {
        vpp_SpeechClose();
        g_apbsSpeechUsed = FALSE;
    }

    // AMJP is used ?
    if (g_apbsAudioUsed)
    {
        vpp_AudioJpegDecClose();
        g_apbsAudioUsed = FALSE;
    }

    // overlay is over.
    // hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    if (mmc_GetCurrMode()==MMC_MODE_APBS)
    {
        // set mode to analog TV.
        mmc_SetCurrMode(MMC_MODE_IDLE);
    }

    // and reset global state
    g_apbsItf = AUD_ITF_NONE;
    g_apbsBufferStart = NULL;
    g_apbsBufferTop =  NULL;
    g_apbsBufferCurPos = NULL;
    g_apbsBufferRemainingBytes = 0;
    g_apbsBufferLength = 0;
    g_apbsFramesPlayedNb = 0;
    g_apbsUserHandler = NULL;
    g_apbsAudioCfg.spkLevel = AUD_SPK_MUTE;
    g_apbsAudioCfg.micLevel = AUD_MIC_MUTE;
    g_apbsAudioCfg.sideLevel = 0;
    g_apbsAudioCfg.toneLevel = 0;
    g_apbsLoopMode = FALSE;
    // TODO : mode dependent switch ...
    g_apbsVppCfg.echoEsOn                 = 0;
    g_apbsVppCfg.echoEsVad                = 0;
    g_apbsVppCfg.echoEsDtd                = 0;
    g_apbsVppCfg.echoExpRel               = 0;
    g_apbsVppCfg.echoExpMu                = 0;
    g_apbsVppCfg.echoExpMin               = 0;
    g_apbsVppCfg.encMute = VPP_SPEECH_MUTE;
    g_apbsVppCfg.decMute = VPP_SPEECH_MUTE;
    g_apbsVppCfg.sdf = NULL;
    g_apbsVppCfg.mdf = NULL;
    g_apbsVppCfg.if1 = 0;
    g_apbsFrameSize = 0;
    g_apbsMidBufDetected = FALSE;
    g_apbsDecInput = NULL;
    g_apbsFinishData = FALSE;

    APBS_PROFILE_FUNCTION_EXIT(apbs_Stop);
#ifdef SMALL_BSS_RAM_SIZE
    if(g_apbsBufferForLoopmode_alloc)
    {
        mmc_MemFreeAll_BssRam(g_apbsBufferForLoopmode_alloc);
        g_apbsBufferForLoopmode_alloc=NULL;
    }
#endif
    g_apbsConfigState = APBS_NULL;
    return APBS_ERR_NO;
}




// =============================================================================
// apbs_Pause
// -----------------------------------------------------------------------------
/// This function pauses the audio stream playback.
/// If the function returns
/// #APBS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param pause If \c TRUE, pauses a playing stream. If \c FALSE (and a
/// stream is paused), resumes a paused stream.
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APBS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC APBS_ERR_T apbs_Pause(BOOL pause)
{
    APBS_PROFILE_FUNCTION_ENTER(apbs_Pause);
    aud_StreamPause(g_apbsItf, pause);
    APBS_PROFILE_FUNCTION_EXIT(apbs_Pause);
    return APBS_ERR_NO;
}

// =============================================================================
// apbs_GetBufPosition
// -----------------------------------------------------------------------------
/// This function returns the current position in the stream buffer.
/// This position points the next bytes to be played.
/// The bytes before this position might not yet have been played at the
/// time this position is read, but are in the process pipe to be played.
///
/// @return The pointer to the next sample to be fetched from the stream buffer.
// =============================================================================
PUBLIC void apbs_SetBuffer(UINT32 *buffer, UINT32 buf_len)
{
    g_apbsBufferStart                     = buffer;
    g_apbsBufferTop                       = buffer + buf_len/4;
    g_apbsBufferCurPos                  = buffer;
    g_apbsBufferRemainingBytes    = 0;
    g_apbsBufferLength                   = buf_len;

    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] g_apbsBufferStart: %x\n", g_apbsBufferStart);
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] g_apbsBufferTop: %x\n", g_apbsBufferTop);
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] g_apbsBufferLength: %d\n", g_apbsBufferLength);

}


PUBLIC UINT32* apbs_GetBufPosition(VOID)
{
    return (g_apbsBufferCurPos);
}


PUBLIC void apbs_GetWriteBuffer( UINT32 **buffer, UINT32 *buf_len )
{
    UINT32 RemaingBytes = apbs_GetRemain();
    UINT32 *CurPos =apbs_GetBufPosition();

    if((UINT32)CurPos+RemaingBytes >= (UINT32)g_apbsBufferTop )
    {
//          MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] apbs_GetWriteBuffer: g_apbsBufferCurPos = %x g_apbsBufferRemainingBytes=%d\n", CurPos, RemaingBytes);
        *buffer =(UINT8* ) CurPos+RemaingBytes-g_apbsBufferLength;
    }
    else
    {
        *buffer= (UINT8 *)CurPos+RemaingBytes;
    }


    if(CurPos>= *buffer )
        *buf_len = (UINT32)CurPos-(UINT32)(*buffer);
    else
        *buf_len = (UINT32) g_apbsBufferTop-(UINT32)(*buffer);

//        hal_HstSendEvent(RemaingBytes);//(UINT32)(*buffer)
//        MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] apbs_GetWriteBuffer: *buffer = %x buf_len=%d\n", *buffer, *buf_len);

}

PUBLIC UINT32 apbs_GetRemain(VOID)
{
    return (g_apbsBufferRemainingBytes);
}

PUBLIC void apbs_DataFinished(void)
{
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,"apbs_DataFinished");

    g_apbsFinishData = TRUE;
}

// =============================================================================
// apbs_AddedData
// -----------------------------------------------------------------------------
/// When a stream buffer is played in loop mode, already played data can be
/// replaced by new ones to play a long song seamlessly. The APBS service needs
/// to be informed about the number of new bytes to play in the buffer. This
/// is the role of the #apbs_AddedData function.
///
/// It is mandatory to call this function when a stream is played in loop mode
/// with handler defined.
///
/// If the function returns
/// #APBS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @param addedDataBytes Number of bytes added to the buffer.
/// @return #APBS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APBS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC APBS_ERR_T apbs_AddedData(UINT32 addedDataBytes)
{
    APBS_PROFILE_FUNCTION_ENTER(apbs_AddedData);

    UINT32 status = hal_SysEnterCriticalSection();
    g_apbsBufferRemainingBytes += addedDataBytes;
    hal_SysExitCriticalSection(status);
    APBS_PROFILE_FUNCTION_EXIT(apbs_AddedData);
    MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0,"[APBS] apbs_AddedData:addedDataBytes=%dg_apbsBufferRemainingBytes = %d\n", addedDataBytes, g_apbsBufferRemainingBytes);
    return APBS_ERR_NO;
}

// =============================================================================
// apbs_UserMsgHandle
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOID apbs_UserMsgHandle(COS_EVENT *pnMsg)
{
    MCI_TRACE(MCI_ANALOGTV_TRC, 0,"[APBS]apbs_UserMsgHandle");

    switch (pnMsg->nEventId)
    {
        case  APBS_STOP:
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,"[[APBS]]STOP");

            if (g_apbsUserHandler)
            {
                g_apbsUserHandler(STREAM_STATUS_END);
            }

            //STOP
            apbs_Stop();
            break;
        case  APBS_SETUP:
            MCI_TRACE(MCI_ANALOGTV_TRC|TSTDOUT, 0,"[[APBS]]SETUP");
            UINT32 status = hal_SysEnterCriticalSection();
            UINT32 setup=1;
            if(g_apbsConfigState != APBS_NULL)
            {
                setup=0;
                MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] skip aud_Setup",g_apbsItf,audioItf);
            }
            hal_SysExitCriticalSection(status);

            //STOP
            if(setup)
                aud_Setup(audioItf, &g_apbsAudioCfg);
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT,0, "[APBS] g_apbsItf=%d audioItf=%d",g_apbsItf,audioItf);
            break;
        case APBS_REQUEST:
            MCI_TRACE(MCI_ANALOGTV_TRC|TSTDOUT, 0,"[[APBS]]APBS_REQUEST");

            if (g_apbsUserHandler)
            {
                g_apbsUserHandler(STREAM_STATUS_REQUEST_DATA);
            }

            break;
        case APBS_ERR:
            MCI_TRACE(MCI_ANALOGTV_TRC|TSTDOUT, 0,"[[APBS]]APBS_ERR");

            if (g_apbsUserHandler)
            {
                g_apbsUserHandler(STREAM_STATUS_ERR);
            }

            //send message to caller.
            if (MCI_RingFinished!=NULL)
            {
                MCI_RingFinished(MCI_ERR_INVALID_FORMAT);
            }

            apbs_Stop();
            break;
        case APBS_NO_MORE_DATA:
            MCI_TRACE(MCI_AUDIO_TRC|TSTDOUT, 0,"[[APBS]]APBS_NO_MORE_DATA");

            if (g_apbsUserHandler)
            {
                g_apbsUserHandler(STREAM_STATUS_NO_MORE_DATA);
            }

            apbs_Stop();
            break;
        default:
            MCI_TRACE(MCI_ANALOGTV_TRC|TSTDOUT, 0,"[APBS] MESSAGE ID: %d",pnMsg->nEventId);
            break;
    }

    return;
}


