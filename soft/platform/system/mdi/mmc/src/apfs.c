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
#include "hal_timers.h"


//#include "vpp_speech.h"
#include "vpp_audiojpeg_dec.h"
#include "mmc.h"
#include "mcip_debug.h"
#include "fs.h"
#include "mci.h"


#include "string.h"
#include "mmc_midi.h"
#include "mmc_audiowav.h"
#include "mmc_audiowma.h"
#include "mpegheader_parser.h"

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


// =============================================================================
//  MACROS
// =============================================================================

/// Number of trames before unmuting AUD
#define APFS_AUD_UNMUTE_TRAME_NB 6

/// Number of trames before unmuting VPP
#define APFS_VPP_UNMUTE_TRAME_NB 3

/// TODO : should be properly dfefined in the header
#define APFS_ERR_NB_MAX 200

// the length of checking.(Bytes)
#define  APFS_CHECK_LENGTH (40*2048)

#define  APFS_MIN_DETEC_LENGTH (1*1024)

// the length of AAC PCM buffer.(Bytes)
#define  APFS_AAC_PCM_BUFFER_LENGTH (1024*4*4)

// the length of AAC PCM buffer.(Bytes)
#define  APFS_MP3_PCM_BUFFER_LENGTH (1152*4*4)



// the length of PCM buffer for mono.(Bytes)
#define  g_apfsPcmMono8kBufSize (2*576*2)

// the length of VoC internal buffer.(Bytes)
#define  g_apfsVoCIntBufSize  (2048)


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  STREAM GLOBAL VARIABLES
// =============================================================================

// the length of PCM buffer for stereo.(Bytes)
PRIVATE UINT32  g_apfsPcmStereoBufSize= 0;


/// the type of the file.
PRIVATE UINT32 g_apfsMode= 0;

/// the length of reading.
PRIVATE INT32 g_apfsReadLength= 0;


/// End address of the stream buffer
PRIVATE UINT32 g_apfsBufferLength = 0;

/// Start address of the stream buffer
PRIVATE UINT8* g_apfsBufferStart= NULL;

/// End address of the stream buffer
PRIVATE UINT8* g_apfsBufferTop = NULL;

/// Number of bytes of data to read in the buffer
PRIVATE INT32 g_apfsBufferRemainingBytes = 0;

/// Current position in the buffer. Next read data will be read from here.
PRIVATE UINT8* g_apfsBufferCurPos= NULL;

/// Current position in the PCM buffer. Next write data will be written from here.
PRIVATE UINT32* g_apfsOutBufferCurPos= NULL;

/// Output Count Bytes.
PRIVATE UINT32 g_apfsOutCountBytes= 0;

// =============================================================================
//  CONFIG GLOBAL VARIABLES
// =============================================================================

/// Number of bytes composing one trame for the codec currently used
//PRIVATE UINT32 g_apfsFrameSize= 0;

/// To stop at the end of the buffer if not in loop mode
//PRIVATE BOOL g_apfsLoopMode = FALSE;

/// the flag for decoding the infromation.
VOLATILE BOOL g_apfsDecFinishFlag = FALSE;

/// the flag for Error.
VOLATILE BOOL g_apfsErrorFlag = FALSE;

/// Number of played Frames
VOLATILE UINT32 g_apfsFramesPlayedNb = 0;

/// Number of played good Frames
VOLATILE INT32 g_apfsGoodFramesPlayedNb = 0;


/// Total good Frames Frame length.(for MP3)
VOLATILE INT32 g_apfsTotalFrameLength = 0;

/// Total good Frames Frame  bitrate.(for AAC)
VOLATILE INT32 g_apfsTotalFrameBitrate= 0;

/// Pointer to the stream input buffer of VPP decoding buffer
//PRIVATE UINT32* g_apfsDecInput= NULL;

/// To only pulse the mid buffer
PRIVATE BOOL g_apfsMidBufDetected = FALSE;

/// Index in the swap buffer (for PCM) which is the one where to copy
/// the new data
//PRIVATE UINT32 g_apfsSwIdx = 0;


/// VPP configuration used to play the stream in normal state.
/// (ie not when started or stopped).
//PRIVATE VPP_SPEECH_AUDIO_CFG_T g_apfsVppCfg;

/// VPP configuration used to play multimedai stream in normal state.
/// (ie not when started or stopped.
PRIVATE vpp_AudioJpeg_DEC_IN_T g_apfsVppAmjpAudioCfg;

/// counter for the errors
PRIVATE UINT32 g_apfsErrNb;

/// Audio PCM buffer
/// TODO :  Durty patch to play MP3. This bufer should be allocated by MPS!
PRIVATE UINT32 *g_apfsPcmBuf= NULL;

/// TODO: move that in the speech handler specific file.
/// Enable amr ring
//PROTECTED BOOL g_apfsAmrRing;

PRIVATE vpp_AudioJpeg_DEC_OUT_T g_apfsVppStatus;


/// GetFileInformation fuction;
PRIVATE BOOL g_GetFileInformation = FALSE;


/// Are we playing a speech stream ?
//PRIVATE BOOL g_apfsSpeechUsed = FALSE;

/// Are we playing a audio stream ?
//PRIVATE BOOL g_apfsAudioUsed = FALSE;

/// Are we playing a audio stream in Temp Buffer?
//PRIVATE BOOL g_apfsDataInTempBuffer= FALSE;

//temp buffer for loop mode
//PRIVATE UINT32 g_apfsBufferForLoopmode [g_apfsVoCIntBufSize/2] ; /// 2*g_apfsVoCIntBufSize in bytes.

// TODO : This is durty, half/full buff values should be given by MPS
//#define  g_apfsPcmMono8kBuf_0    (g_apfsPcmBuf)
//#define  g_apfsPcmMono8kBuf_1    (g_apfsPcmBuf+(g_apfsPcmMono8kBufSize/8))  // half size in UINT32



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


PRIVATE CONST UINT16 WAVSamplesRate[]= {48000,44100,32000,24000,22050,16000,12000,11025,8000};

// =============================================================================
//  FUNCTIONS
// =============================================================================
// =============================================================================
// apfs_VppAmjpHandler
// -----------------------------------------------------------------------------
// =============================================================================
PRIVATE VOID apfs_VppAmjpHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
//      vpp_AudioJpeg_DEC_OUT_T vppStatus;
    INT32 readBytes;

    //   MCI_TRACE (MCI_AUDIO_TRC,0, "APFS VoC");
    // do not read status during the first callback
    if (g_apfsFramesPlayedNb++)
    {

        // get status
        vpp_AudioJpegDecStatus(&g_apfsVppStatus);


        //MCI_TRACE (MCI_AUDIO_TRC,0, "APFS consumedLen:%d",vppStatus.consumedLen);

        // compute the new remaining size
        readBytes =g_apfsVppStatus.consumedLen;

        MCI_ASSERT((readBytes < 0), "consumedLen must be >=0");

        // update the stream buffer status
        g_apfsBufferRemainingBytes -= readBytes;

        // update read pointer
        g_apfsBufferCurPos += readBytes;  //  in UINT32

        // check for errors
        if (g_apfsVppStatus.ErrorStatus)
        {
            MCI_TRACE (MCI_AUDIO_TRC,0, "APFS ERROR!");
            if (g_apfsErrNb++ > APFS_ERR_NB_MAX||g_apfsBufferRemainingBytes < g_apfsVoCIntBufSize)
            {
                MCI_TRACE (MCI_AUDIO_TRC,0, "APFS ERROR STOP!");

                g_apfsErrorFlag=TRUE;

                //Stop decoding the infromation.
                g_apfsDecFinishFlag=TRUE;

                // Leave
                return;

            }
        }
        else
        {
            //count the good frame;
            g_apfsGoodFramesPlayedNb++;

            //count the total good frame length for AAC(g_apfsVppStatus.ImgWidth is used for AAC frame length).
            g_apfsTotalFrameLength+=g_apfsVppStatus.BitRate;

            //count the total good frame Bitrate for MP3.
            g_apfsTotalFrameBitrate+=g_apfsVppStatus.BitRate;

            /// reset counter for the errors
            g_apfsErrNb=0;



            // update the Counter status
            g_apfsOutCountBytes += g_apfsVppStatus.output_len;

            // update write pointer
            g_apfsOutBufferCurPos += g_apfsVppStatus.output_len/4;  //in UINT32
        }

    }


    if (g_apfsBufferRemainingBytes < g_apfsVoCIntBufSize)
    {
        g_apfsErrorFlag=FALSE;

        //Stop decoding the infromation.
        g_apfsDecFinishFlag=TRUE;
        return;
    }

    // updata the input buffer address.
    g_apfsVppAmjpAudioCfg.inStreamBufAddr = (UINT32*)g_apfsBufferCurPos;

    // updata the output buffer address.
    g_apfsVppAmjpAudioCfg.outStreamBufAddr = g_apfsPcmBuf;

    // configure next frame and wake up VoC
    vpp_AudioJpegDecScheduleOneFrame(&g_apfsVppAmjpAudioCfg);


    if(g_apfsOutCountBytes>=g_apfsPcmStereoBufSize/2&&g_GetFileInformation == TRUE)
    {

        //no errors.
        g_apfsErrorFlag=FALSE;

        //have finished decoding the infromation.
        g_apfsDecFinishFlag=TRUE;

        return;

    }

}

/*

// =============================================================================
// apfs_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID apfs_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* IRQStatus)
{
    APFS_PROFILE_FUNCTION_ENTER(apfs_VppSpeechHandler);
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


    if (g_apfsFramesPlayedNb == APFS_AUD_UNMUTE_TRAME_NB)
    {
        // Unmute AUD
        MCI_TRACE (MCI_AUDIO_TRC,0, "AUD unmuted");
        aud_Setup(g_apfsItf, &g_apfsAudioCfg);
    }

    if (g_apfsFramesPlayedNb == APFS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        MCI_TRACE (MCI_AUDIO_TRC,0, "VPP unmuted");
        vpp_SpeechAudioCfg(&g_apfsVppCfg);
    }

    decStruct = vpp_SpeechGetRxCodBuffer();

    if(g_apfsAmrRing == TRUE)
    {
        g_apfsFrameSize = 1;
        if(g_apfsBufferRemainingBytes >= 1)
        {
            switch((*((UINT8*)g_apfsBufferCurPos) >> 3) & 0xF)
            {
                // Real frame size, as there is no alignment
                case 0:
                    g_apfsFrameSize      = 13;
                    decStruct->codecMode = HAL_AMR475_DEC;
                    break;
                case 1:
                    g_apfsFrameSize      = 14;
                    decStruct->codecMode = HAL_AMR515_DEC;
                    break;
                case 2:
                    g_apfsFrameSize      = 16;
                    decStruct->codecMode = HAL_AMR59_DEC;
                    break;
                case 3:
                    g_apfsFrameSize      = 18;
                    decStruct->codecMode = HAL_AMR67_DEC;
                    break;
                case 4:
                    g_apfsFrameSize      = 20;
                    decStruct->codecMode = HAL_AMR74_DEC;
                    break;
                case 5:
                    g_apfsFrameSize      = 21;
                    decStruct->codecMode = HAL_AMR795_DEC;
                    break;
                case 6:
                    g_apfsFrameSize      = 27;
                    decStruct->codecMode = HAL_AMR102_DEC;
                    break;
                case 7:
                    g_apfsFrameSize      = 32;
                    decStruct->codecMode = HAL_AMR122_DEC;
                    break;
                default:
                    break;
            }
        }
    }


    if (g_apfsBufferRemainingBytes < g_apfsFrameSize)
    {
        // FIXME Let's assume something: If the buffer
        // is played in no loop, we don't care about the
        // alignment and guess that there won't be any refill
        if (!g_apfsLoopMode)
        {
            // As stop destroy the global vars ...
            APFS_USER_HANDLER_T handler = g_apfsUserHandler;

            // Stop the play
            apfs_Stop();

            // End of buffer reached
            if (handler)
            {
                handler(APFS_STATUS_END_BUFFER_REACHED);
            }

            // Leave
            return;
        }
        else
        {

            if (g_apfsUserHandler)
            // Data underflow ...
            // Stops here
            {
                // We need and ask for refill
                MCI_TRACE (MCI_AUDIO_TRC,0,"APFS famine\n");
                AUD_LEVEL_T audioCfg = g_apfsAudioCfg;
                VPP_SPEECH_AUDIO_CFG_T vppCfg = g_apfsVppCfg;
                g_apfsUserHandler(APFS_STATUS_NO_MORE_DATA);
                /// Mute ...
                audioCfg.spkLevel = 0;
                vppCfg.decMute = VPP_SPEECH_MUTE;
                aud_Setup(g_apfsItf, &audioCfg);
                vpp_SpeechAudioCfg(&vppCfg);

                /// Set state as for starting again from there ?
                g_apfsFramesPlayedNb = 0;
            }
            else
            {
                // Simply wrap, we want to play that buffer again
                // and again
                g_apfsBufferCurPos = g_apfsBufferStart;
                g_apfsBufferRemainingBytes = g_apfsBufferLength;
            }
        }

    }
    else
    {
        UINT32 csStatus = hal_SysEnterCriticalSection();
        g_apfsBufferRemainingBytes -= g_apfsFrameSize;
        hal_SysExitCriticalSection(csStatus);

        decInBufPos = (UINT32*)decStruct->decInBuf;

        ///
        if (((UINT32)g_apfsBufferCurPos + g_apfsFrameSize) > (UINT32)g_apfsBufferTop)
        {
            if (!g_apfsLoopMode)
            {
                // As stop destroy the global vars ...
                APFS_USER_HANDLER_T handler = g_apfsUserHandler;

                // Stop the play
                apfs_Stop();

                // End of buffer reached
                if (handler)
                {
                    handler(APFS_STATUS_END_BUFFER_REACHED);
                }

                // Leave
                return;
            }
            // else wrap ...

            // Copy first part
            cpSz = (UINT32)(g_apfsBufferTop - g_apfsBufferCurPos);
            memcpy(decInBufPos, g_apfsBufferCurPos, cpSz);
            decInBufPos        = (UINT32*)((UINT32)decInBufPos+cpSz);

            g_apfsBufferCurPos = g_apfsBufferStart;
            cpSz = g_apfsFrameSize - cpSz;

            // Buffer End Reached
            endBufferReached = TRUE;
            g_apfsMidBufDetected = FALSE;
        }
        else
        {
            cpSz = g_apfsFrameSize;
        }

        // Finish or full copy
        memcpy(decInBufPos, g_apfsBufferCurPos, cpSz);
        g_apfsBufferCurPos = (UINT32*)((UINT32)g_apfsBufferCurPos+cpSz);

        g_apfsFramesPlayedNb++;

        // Half buffer reached
        if (((UINT32)g_apfsBufferCurPos >= (UINT32)g_apfsBufferStart + (g_apfsBufferLength/2))
            && !g_apfsMidBufDetected)
        {
            halfBufferReached = TRUE;
            g_apfsMidBufDetected = TRUE;
        }

    }

    if (halfBufferReached)
    {
        if (g_apfsUserHandler)
        {
            g_apfsUserHandler(APFS_STATUS_MID_BUFFER_REACHED);
        }
    }

    if (endBufferReached)
    {
        if (g_apfsUserHandler)
        {
            g_apfsUserHandler(APFS_STATUS_END_BUFFER_REACHED);
        }
    }

    APFS_PROFILE_FUNCTION_EXIT(apfs_VppSpeechHandler);
}
 */

// =============================================================================
// apfs_GetFileInformation
// -----------------------------------------------------------------------------
/// Get decoding infromation from a file.
///
/// This function  outputs the
/// audio on the audio interface specified as a parameter, on the output selected in
/// the #apfs_Setup function. \n
/// In normal operation, when the buffer runs out, the playing will stop. It can
/// loop according to the caller choice. If a handler is defined, it is expected
/// that the handler will refeed data and call #apfs_AddedData to tell APFS about
/// that. If there is no handler, in loop mode, the buffer will be played in loop
/// without expecting new data.
/// APFS can a user function
/// at the middle or at the end of the playback.
///
/// @param itf Interface on which play that stream
/// @param stream Stream to play. Handler called at the middle and end of the buffer
/// are defined there.
/// @param cfg The configuration set applied to the audio interface
/// @param loop \c TRUE if the buffer is played in loop mode.
/// @return #APFS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #APFS_ERR_NO it can execute the command.
// =============================================================================



PUBLIC MCI_ERR_T apfs_GetFileInformation (
    CONST INT32 FileHander,
    AudDesInfoStruct  * CONST DecInfo,
    CONST mci_type_enum FileType   )
{
    INT32 tagsize;

    int32 timebegin=hal_TimGetUpTime();

    HAL_ERR_T VoC_RET;
    MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[APFS]apfs_GetFileInformation");

    g_GetFileInformation = TRUE;

//  MCI_ASSERT(FileHander<0, "[APFS]########File Hander Error!########\n");

    if(FileHander<0)
    {
        MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[APFS_ERROR]Error file handle:%d",FileHander);
        return MCI_ERR_UNKNOWN_FORMAT;
    }

    if(FileType == MCI_TYPE_WAV)   return  MMC_WavGetFileInformation (  FileHander,  DecInfo  );
#ifdef MEDIA_MIDI_SUPPORT
    if(FileType == MCI_TYPE_MIDI)   return  MMC_MidGetFileInformation (  FileHander,  DecInfo  );
#endif// MEDIA_MIDI_SUPPORT
    // Register global var
    g_apfsBufferStart                     = (UINT8*)mmc_MemMalloc(APFS_CHECK_LENGTH);
    g_apfsBufferTop                       = (UINT8*)(g_apfsBufferStart + APFS_CHECK_LENGTH);
    g_apfsBufferCurPos                  = g_apfsBufferStart;
    g_apfsBufferRemainingBytes   = APFS_CHECK_LENGTH;
    g_apfsBufferLength           = APFS_CHECK_LENGTH;
    g_apfsMidBufDetected              = FALSE;
    g_apfsFramesPlayedNb            = 0;
    g_apfsErrorFlag              = FALSE;
    //is it finished?
    g_apfsDecFinishFlag=FALSE;


    if(g_apfsBufferStart==NULL)
    {
        mmc_MemFreeAll();
        return MCI_ERR_OUT_OF_MEMORY;
    }

    //put the PCM buffer in the internSRAM.
//  hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

    //initialize the pointer of PCM buffer.
    g_apfsPcmBuf=(UINT32 *)pcmbuf_overlay;

    // read data from the file.
    g_apfsReadLength = FS_Read(FileHander, g_apfsBufferStart, APFS_CHECK_LENGTH);

    MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] first g_apfsReadLength=%d ;need length=%d\n", g_apfsReadLength,APFS_CHECK_LENGTH);


    if(g_apfsReadLength<0)
    {
        return MCI_ERR_UNKNOWN_FORMAT;
    }

    if(g_apfsReadLength<APFS_CHECK_LENGTH)
    {
        g_apfsBufferRemainingBytes= g_apfsReadLength;
    }


    if ((g_apfsBufferCurPos[0]=='I')&&(g_apfsBufferCurPos[1]=='D')&&(g_apfsBufferCurPos[2]=='3'))
    {
        // high bit is not used
        tagsize = ((g_apfsBufferCurPos[6] << 21) | (g_apfsBufferCurPos[7] << 14) |
                  (g_apfsBufferCurPos[8] <<  7) | (g_apfsBufferCurPos[9] <<  0));

        tagsize += 10;

        MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] tagsize: %d\n", tagsize);

        if(FS_Seek(FileHander, (UINT64)tagsize, FS_SEEK_SET)<0)return MCI_ERR_UNKNOWN_FORMAT;;

        g_apfsReadLength = FS_Read(FileHander, g_apfsBufferStart, APFS_CHECK_LENGTH);

        MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] after ID3 g_apfsReadLength=%d ;need length=%d\n", g_apfsReadLength,APFS_CHECK_LENGTH);

        if(g_apfsReadLength<0)
        {
            return MCI_ERR_UNKNOWN_FORMAT;
        }

        if(g_apfsReadLength<APFS_CHECK_LENGTH)
        {
            g_apfsBufferRemainingBytes= g_apfsReadLength;
        }
    }



    switch (FileType)
    {
//      HAL_SPEECH_DEC_IN_T decStruct;
//      VPP_SPEECH_AUDIO_CFG_T vppCfg;
        //    UINT32 i;

        case MCI_TYPE_AAC:
        case MCI_TYPE_DAF:
            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] mp3 and aac. ");

            //initialize the PCM buffer length.
            g_apfsPcmStereoBufSize=APFS_MP3_PCM_BUFFER_LENGTH;
            g_apfsMode=MMC_MP3_DECODE;

            if(FileType==MCI_TYPE_AAC)
            {
                MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS]  aac. ");
                g_apfsPcmStereoBufSize= APFS_AAC_PCM_BUFFER_LENGTH ;
                g_apfsMode=MMC_AAC_DECODE;
            }

            //initialize the PCM buffer.
            memset(g_apfsPcmBuf,0,g_apfsPcmStereoBufSize);

            //initialize the PCM buffer pointer.
            g_apfsOutBufferCurPos=g_apfsPcmBuf;  // half size in UINT32;

            //  initialize the Counters.
            g_apfsOutCountBytes=0;
            g_apfsErrNb=0;

            // config the input parometers that can not change in playing except the reset;
            g_apfsVppAmjpAudioCfg.mode  = g_apfsMode;
            g_apfsVppAmjpAudioCfg.reset     = 1;
            g_apfsVppAmjpAudioCfg.BsbcEnable     = 0;
            g_apfsVppAmjpAudioCfg.EQ_Type    = -1;

            // Open VoC.
            VoC_RET=vpp_AudioJpegDecOpen(apfs_VppAmjpHandler);
            if (HAL_ERR_NO  != VoC_RET)
            {
                if(HAL_ERR_RESOURCE_BUSY==VoC_RET)
                {
                    MCI_TRACE (MCI_AUDIO_TRC,0, "HAL_ERR_RESOURCE_BUSY and we will get the informaiton direct!");
                    vpp_AudioJpegDecStatus(&g_apfsVppStatus);
                    // get the infromation.
                    DecInfo->sampleRate             = g_apfsVppStatus.SampleRate;
                    DecInfo->stereo                     = g_apfsVppStatus.nbChannel;
                    DecInfo->bitRate                    = g_apfsVppStatus.BitRate;
                    DecInfo->details.DAF.CRC      = g_apfsVppStatus.ImgWidth;
                    DecInfo->details.DAF.channel_mode    = g_apfsVppStatus.ImgHeight;

                    MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] SampleRate: %d\n", g_apfsVppStatus.SampleRate);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] nbChannel: %d\n", g_apfsVppStatus.nbChannel);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] BitRate: %d\n", g_apfsVppStatus.BitRate);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] CRC: %d\n", g_apfsVppStatus.ImgWidth);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] channel_mode: %d\n", g_apfsVppStatus.ImgHeight);

                    // overlay is over.
                    //  hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
                    //mmc_MemFreeAll();
                    return MCI_ERR_NO;
                }
                else
                {
                    MCI_TRACE (MCI_AUDIO_TRC,0, "OTHER VoC ERROR!");
                    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);
                    mmc_MemFreeAll();
                    return MCI_ERR_ERROR;
                }
            }

            //waiting....
            while(g_apfsDecFinishFlag!=TRUE)
            {

            }

            // get the infromation.
            DecInfo->sampleRate             = g_apfsVppStatus.SampleRate;
            DecInfo->stereo                     = g_apfsVppStatus.nbChannel;
            DecInfo->bitRate                    = g_apfsVppStatus.BitRate;
            DecInfo->details.DAF.CRC      = g_apfsVppStatus.ImgWidth;
            DecInfo->details.DAF.channel_mode    = g_apfsVppStatus.ImgHeight;

            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] SampleRate: %d\n", g_apfsVppStatus.SampleRate);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] nbChannel: %d\n", g_apfsVppStatus.nbChannel);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] BitRate: %d\n", g_apfsVppStatus.BitRate);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] CRC: %d\n", g_apfsVppStatus.ImgWidth);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] channel_mode: %d\n", g_apfsVppStatus.ImgHeight);

            //resume the pointer location.
            if(FS_Seek(FileHander, 0, FS_SEEK_SET)<0)
            {
                // stop VoC.
                vpp_AudioJpegDecClose();

                //free all momery.
                mmc_MemFreeAll();

                return MCI_ERR_UNKNOWN_FORMAT;
            }

            // stop VoC.
            vpp_AudioJpegDecClose();

            // overlay is over.
            //hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] apfs_GetFileInformation end mp3 aac. ");

            mmc_MemFreeAll();

            int32 timeend=hal_TimGetUpTime();
            MCI_TRACE (MCI_AUDIO_TRC,0," end timeend = %d",timeend);
            MCI_TRACE (MCI_AUDIO_TRC,0,"end time = %dms",((timeend-timebegin)*1000)/16384);

            //are there errors?
            if(g_apfsErrorFlag==TRUE)
            {
                return MCI_ERR_UNKNOWN_FORMAT;
            }
            else
            {
                return MCI_ERR_NO;
            }

            break;

        case MCI_TYPE_WAV:


            /*        g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_PCM_BUF;

                    // Typical global vars
                    g_apfsDecInput = g_apfsPcmMono8kBuf_0; //(UINT32*)vpp_SpeechGetRxPcmBuffer();

                    // PCM Audio stream, output of VPP
                    pcmStream.startAddress = g_apfsDecInput;
                    pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
                    pcmStream.sampleRate = stream->sampleRate;
                    pcmStream.channelNb = stream->channelNb;
                    pcmStream.voiceQuality = stream->voiceQuality;
                    pcmStream.playSyncWithRecord = FALSE;
                    pcmStream.halfHandler = apfs_RawPcmHandler;
                    pcmStream.endHandler =  apfs_RawPcmHandler;*/
            break;

        case MCI_TYPE_AMR:
            /*          // Speech is used
                        g_apfsSpeechUsed                      = TRUE;

                        // vpp speech congiguration structure
                        g_apfsVppCfg.echoEsOn                 = 0;
                        g_apfsVppCfg.echoEsVad                = 0;
                        g_apfsVppCfg.echoEsDtd                = 0;
                        g_apfsVppCfg.echoExpRel               = 0;
                        g_apfsVppCfg.echoExpMu                = 0;
                        g_apfsVppCfg.echoExpMin               = 0;
                        g_apfsVppCfg.encMute                  = VPP_SPEECH_MUTE;
                        g_apfsVppCfg.decMute                  = VPP_SPEECH_UNMUTE;
                        g_apfsVppCfg.sdf                      = NULL;
                        g_apfsVppCfg.mdf                      = NULL;
                        g_apfsVppCfg.if1                      = 0;

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
                        g_apfsAmrRing = FALSE;

                        // Specific config
                        switch (stream->mode)
                        {
                            case APFS_PLAY_MODE_AMR475:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR475;
                                decStruct.codecMode = HAL_AMR475_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR515:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR515;
                                decStruct.codecMode = HAL_AMR515_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR59:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR59;
                                decStruct.codecMode = HAL_AMR59_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR67:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR67 ;
                                decStruct.codecMode = HAL_AMR67_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR74:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR74 ;
                                decStruct.codecMode = HAL_AMR74_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR795:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR795;
                                decStruct.codecMode = HAL_AMR795_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR102:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR102;
                                decStruct.codecMode = HAL_AMR102_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR122:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR122;
                                decStruct.codecMode = HAL_AMR122_DEC;
                                break;

                            case APFS_PLAY_MODE_AMR_RING:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_AMR_RING;
                                decStruct.codecMode = HAL_AMR122_DEC;
                                g_apfsVppCfg.if1    = 1;
                                g_apfsAmrRing = TRUE;
                                break;

                            case APFS_PLAY_MODE_EFR:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_EFR  ;
                                decStruct.codecMode = HAL_EFR;
                                break;

                            case APFS_PLAY_MODE_HR:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_HR ;
                                decStruct.codecMode = HAL_HR;
                                break;

                            case APFS_PLAY_MODE_FR:
                                g_apfsFrameSize = HAL_SPEECH_FRAME_SIZE_FR;
                                decStruct.codecMode = HAL_FR;
                                break;

                            default:
                                APFS_ASSERT(FALSE, "Unsupported mode in APFS:%d", stream->mode);
                        }

                        // PCM Audio stream, output of VPP
                        pcmStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
                        pcmStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
                        pcmStream.sampleRate = HAL_AIF_FREQ_8000HZ;
                        pcmStream.channelNb = HAL_AIF_MONO;
                        pcmStream.voiceQuality = TRUE;
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
                        vppCfg.if1 = g_apfsVppCfg.if1;

                        // Clear the PCM buffer
                        memset(pcmStream.startAddress, 0, pcmStream.length);

                        // configure AUD and VPP buffers
                        // set vpp
                        if (HAL_ERR_RESOURCE_BUSY == vpp_SpeechOpen(apfs_VppSpeechHandler, VPP_SPEECH_WAKEUP_HW_DEC))
                        {
                            return APFS_ERR_RESOURCE_BUSY;
                        }
                        vpp_SpeechAudioCfg(&vppCfg);

                        // Init the Rx buffer
                        *(vpp_SpeechGetRxCodBuffer()) = decStruct;

                        // Pointer to the stream buffer in VPP decoding buffer
                        g_apfsDecInput = (UINT32*) decStruct.decInBuf; */

            break;
        case MCI_TYPE_WMA:
#ifdef WMA_DECODE
            Audio_WMAGetFileInformation(FileHander,DecInfo);
#endif
            break;
        default:
            break;
    }



    return MCI_ERR_NO;
}


// =============================================================================
// apfs_GetDurationTime
// -----------------------------------------------------------------------------
/// Get duration time.
///
// =============================================================================



PUBLIC MCI_ERR_T apfs_GetDurationTime(
    INT32 FileHander,
    mci_type_enum FileType,
    INT32 BeginPlayProgress,
    INT32 OffsetPlayProgress,
    MCI_ProgressInf* PlayInformation)
{

    INT32 tagsize=0;

    INT32 timebegin=hal_TimGetUpTime();


    HAL_ERR_T VoC_RET;

    INT32 result;

    INT32 TotalFileSize,DurationLenth;

    MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[MCI_APFS]apfs_GetDurationTime BeginPlayProgress:%d,OffsetPlayProgress:%d",BeginPlayProgress,OffsetPlayProgress);

    //MCI_ASSERT(FileHander<0, "[MCI_APFS]########File Hander Error!########\n");

    g_GetFileInformation = FALSE;

    if(FileHander<0)
    {
        MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0, "[APFS_ERROR]apfs_GetDurationTimeError file handle:%d",FileHander);
        return MCI_ERR_UNKNOWN_FORMAT;
    }



    MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]OffsetPlayProgress:%d",OffsetPlayProgress);

    result=(INT32)FS_Seek(FileHander,0,FS_SEEK_SET);

    MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]FS_seek pos:%d",result);

    if(result<0 )
    {
        MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_APFS]FS_seek error!");
        return MCI_ERR_BAD_FORMAT;
    }


    if ((TotalFileSize=(INT32)FS_GetFileSize(FileHander))< 0)
    {
        MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MCI_APFS]can not get file size!:%d",TotalFileSize);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }

    DurationLenth=(INT32)(((INT64)(OffsetPlayProgress)*(INT64)TotalFileSize)/10000);

    MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]DurationLenth:%x",DurationLenth);

    PlayInformation->ID3Offset=0;

    switch (FileType)
    {
        case MCI_TYPE_AAC:
        case MCI_TYPE_DAF:
        {
            INT32 AverageFrameLength,AverageBitRate;
            //struct mpeg_stream  stream;
            mpeg_stream_play_info palyinfo;

            //stream.buflength = 300*1024;
            // Register global var
            //g_apfsBufferStart                     = (UINT8*)mmc_MemMalloc(stream.buflength);
            g_apfsBufferStart                     = (UINT8*)mmc_MemMalloc(APFS_CHECK_LENGTH);
            g_apfsBufferTop                       = (UINT8*)(g_apfsBufferStart + APFS_CHECK_LENGTH);
            g_apfsBufferCurPos                  = g_apfsBufferStart;
            g_apfsBufferRemainingBytes   = APFS_CHECK_LENGTH;
            g_apfsBufferLength           = APFS_CHECK_LENGTH;
            g_apfsMidBufDetected              = FALSE;
            g_apfsFramesPlayedNb            = 0;
            g_apfsErrorFlag              = FALSE;
            //is it finished?
            g_apfsDecFinishFlag=FALSE;


            if(g_apfsBufferStart==NULL)
            {
                mmc_MemFreeAll();
                return MCI_ERR_OUT_OF_MEMORY;
            }
#if 0
            if (FileType == MCI_TYPE_AAC) {

                stream.audiotype = MMC_AAC_DECODE;

            }else {

                stream.audiotype = MMC_MP3_DECODE;
            }

            //int timebegin1=hal_TimGetUpTime();
            result = mpeg_stream_init(FileHander,g_apfsBufferStart,APFS_MIN_DETEC_LENGTH,&stream);
            if (result == mpeg_ERROR_NONE) {

                int datalen = 0;

                datalen = TotalFileSize-stream.skiplen;

                DurationLenth =((UINT64)(OffsetPlayProgress* datalen))/10000;

                result = mpeg_stream_info_get(&palyinfo,stream);

              //  MCI_TRACE (MCI_AUDIO_TRC,0,"elapsed time = %dms",(((hal_TimGetUpTime())-timebegin1)*1000)/16384);
                if (result == mpeg_ERROR_NONE) {

                    PlayInformation->BitRate = palyinfo.bitRate;
                    PlayInformation->DurationTime = (DurationLenth *8 *1000)/palyinfo.bitRate;
                    mmc_MemFreeAll();
                    return MCI_ERR_NO;
                }

                MCI_TRACE (MCI_AUDIO_TRC,0,"xcpu get mpeg bitrate failed.");
            }
#endif
              //initialize the pointer of PCM buffer.
            g_apfsPcmBuf=(UINT32 *)mmc_MemMalloc(APFS_MP3_PCM_BUFFER_LENGTH);

            if(g_apfsPcmBuf==NULL)
            {
                mmc_MemFreeAll();
                return MCI_ERR_OUT_OF_MEMORY;
            }


            // read data from the file.

            g_apfsReadLength = FS_Read(FileHander, g_apfsBufferStart, APFS_CHECK_LENGTH);


            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] first g_apfsReadLength=%d ;need length=%d\n", g_apfsReadLength,APFS_CHECK_LENGTH);



            if(g_apfsReadLength<0)
            {
                mmc_MemFreeAll();
                return MCI_ERR_UNKNOWN_FORMAT;
            }


            if(g_apfsReadLength<APFS_CHECK_LENGTH)
            {
                g_apfsBufferRemainingBytes= g_apfsReadLength;
            }

            if ((g_apfsBufferCurPos[0]=='I')&&(g_apfsBufferCurPos[1]=='D')&&(g_apfsBufferCurPos[2]=='3'))
            {
                // high bit is not used
                tagsize = ((g_apfsBufferCurPos[6] << 21) | (g_apfsBufferCurPos[7] << 14) |
                          (g_apfsBufferCurPos[8] <<  7) | (g_apfsBufferCurPos[9] <<  0));

                tagsize += 10;


                MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] tagsize: %x\n", tagsize);

                PlayInformation->ID3Offset = tagsize;

                if(FS_Seek(FileHander, (INT64)tagsize, FS_SEEK_SET)<0)return MCI_ERR_UNKNOWN_FORMAT;;

                g_apfsReadLength = FS_Read(FileHander, g_apfsBufferStart, APFS_CHECK_LENGTH);


                MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] after ID3 g_apfsReadLength=%d ;need length=%d\n", g_apfsReadLength,APFS_CHECK_LENGTH);


                if(g_apfsReadLength<0)
                {
                    mmc_MemFreeAll();
                    return MCI_ERR_UNKNOWN_FORMAT;
                }

                if(g_apfsReadLength<APFS_CHECK_LENGTH)
                {
                    g_apfsBufferRemainingBytes= g_apfsReadLength;
                }
            }

            DurationLenth=(INT32)(((INT64)(OffsetPlayProgress)*(INT64)(TotalFileSize-tagsize))/10000);

            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] after ID3 DurationLenth: %d\n", DurationLenth);

            if(DurationLenth<0) DurationLenth=0;

            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] mp3 and aac. ");

            //initialize the PCM buffer length.
            g_apfsPcmStereoBufSize=APFS_MP3_PCM_BUFFER_LENGTH;
            g_apfsMode=MMC_MP3_DECODE;

            if(FileType==MCI_TYPE_AAC)
            {
                MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS]  aac. ");
                g_apfsPcmStereoBufSize= APFS_AAC_PCM_BUFFER_LENGTH ;
                g_apfsMode=MMC_AAC_DECODE;
            }

            //initialize the PCM buffer.
            memset(g_apfsPcmBuf,0,g_apfsPcmStereoBufSize);

            //initialize the PCM buffer pointer.
            g_apfsOutBufferCurPos=g_apfsPcmBuf;  // half size in UINT32;

            //  initialize the Counters.
            g_apfsOutCountBytes=0;
            g_apfsErrNb=0;

            // config the input parometers that can not change in playing except the reset;
            g_apfsVppAmjpAudioCfg.mode  = g_apfsMode;
            g_apfsVppAmjpAudioCfg.reset     = 1;
            g_apfsVppAmjpAudioCfg.BsbcEnable     = 0;
            g_apfsVppAmjpAudioCfg.EQ_Type    = -1;

            g_apfsGoodFramesPlayedNb=0;
            g_apfsTotalFrameBitrate=0;
            g_apfsTotalFrameLength=0;

            // Open VoC.
            VoC_RET=vpp_AudioJpegDecOpen(apfs_VppAmjpHandler);
            if (HAL_ERR_NO  != VoC_RET)
            {

                MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_APFS]OTHER VoC ERROR!");
                mmc_MemFreeAll();
                return MCI_ERR_ERROR;

            }

            //waiting....
            while(g_apfsDecFinishFlag!=TRUE)
            {

            }

            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] ===g_apfsTotalFrameLength: %d\n", g_apfsTotalFrameLength);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] g_apfsGoodFramesPlayedNb: %d\n", g_apfsGoodFramesPlayedNb);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] g_apfsTotalFrameBitrate: %d\n", g_apfsTotalFrameBitrate);

            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] SampleRate: %d\n", g_apfsVppStatus.SampleRate);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] nbChannel: %d\n", g_apfsVppStatus.nbChannel);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] BitRate: %d\n", g_apfsVppStatus.BitRate);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] CRC: %d\n", g_apfsVppStatus.ImgWidth);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] channel_mode: %d\n", g_apfsVppStatus.ImgHeight);

            if(FileType==MCI_TYPE_AAC)
            {

                if(g_apfsVppStatus.SampleRate!=0&&g_apfsGoodFramesPlayedNb!=0&&g_apfsVppStatus.nbChannel!=0)
                {
                    AverageFrameLength=g_apfsTotalFrameLength/g_apfsGoodFramesPlayedNb;
                    PlayInformation->DurationTime=(INT32)(((INT64)DurationLenth*1000*(g_apfsVppStatus.output_len/(2*g_apfsVppStatus.nbChannel)))/
                                                          (AverageFrameLength*g_apfsVppStatus.SampleRate));
                    if(PlayInformation->DurationTime>0)
                        PlayInformation->BitRate=((UINT64)DurationLenth*8*1000)/PlayInformation->DurationTime;
                    else
                        PlayInformation->BitRate=0;



                }
                else
                {
                    PlayInformation->DurationTime=0;
                    PlayInformation->BitRate=0;
                }
                MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]AAC Duration Time :%d ms",PlayInformation->DurationTime);
                MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]AAC BitRate :%d bps",PlayInformation->BitRate);

            }
            else
            {

                if(g_apfsGoodFramesPlayedNb!=0)
                {
                    AverageBitRate=g_apfsTotalFrameBitrate/g_apfsGoodFramesPlayedNb;

                    MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] AverageBitRate: %d\n", AverageBitRate);
                    MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS] DurationLenth: %d\n", DurationLenth);

                    PlayInformation->DurationTime=(INT32)(((INT64)DurationLenth*8)/AverageBitRate);//ms

                    PlayInformation->BitRate=(g_apfsTotalFrameBitrate*1000)/g_apfsGoodFramesPlayedNb;

                }
                else
                {
                    PlayInformation->DurationTime=0;
                }
                MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]MP3  Duration Time :%d ms",PlayInformation->DurationTime);
                MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]MP3  BitRate :%d bps",PlayInformation->BitRate);

            }





            // stop VoC.
            vpp_AudioJpegDecClose();

            MCI_TRACE (MCI_AUDIO_TRC,0,"[APFS] apfs_GetFileInformation end mp3 aac. ");

            mmc_MemFreeAll();

            int32 timeend=hal_TimGetUpTime();
            MCI_TRACE (MCI_AUDIO_TRC,0," end timeend = %d",timeend);
            MCI_TRACE (MCI_AUDIO_TRC,0,"end time = %dms",((timeend-timebegin)*1000)/16384);

            //are there errors?
            if(g_apfsErrorFlag==TRUE)
            {
                return MCI_ERR_UNKNOWN_FORMAT;
            }
            else
            {
                return MCI_ERR_NO;
            }

            break;
        }
        case MCI_TYPE_WAV:
        {
            INT32 readlen,i;
            WAVHeader WAVHeaderInfo;
            WAVDataHeader WAVDataHeaderInfo;
            UINT8 ChunkInfoID[4];
            UINT8 tempWavHeader[0xe8] = {0x00};

            readlen=FS_Read(FileHander,(UINT8 *)tempWavHeader, 0x0f);


            if(readlen!=0x0f)
            {
                return MCI_ERR_ERROR;
            }

            if(!(tempWavHeader[12] == 'f' && tempWavHeader[13] == 'm' && tempWavHeader[14] == 't'))
            {
                readlen=FS_Read(FileHander,(UINT8 *)tempWavHeader + 0x0f, 0xd7);

                if(readlen!=0xd7)
                {
                    return MCI_ERR_ERROR;
                }
                memcpy((UINT8 *)&WAVHeaderInfo, (UINT8 *)tempWavHeader,0x0c);
                memcpy((UINT8 *)(&WAVHeaderInfo)+0x0c, (UINT8 *)(tempWavHeader)+0xce,0x18);
            }
            else
            {
                readlen=FS_Read(FileHander,(UINT8 *)tempWavHeader+0x0f, 0x15);

                if(readlen!=0x15)
                {
                    return MCI_ERR_ERROR;
                }
                memcpy((UINT8 *)&WAVHeaderInfo, (UINT8 *)tempWavHeader,0x24);
            }


            if(WAVHeaderInfo.wFormatTag == 1)       //PCM
            {
                if((memcmp(WAVHeaderInfo.szRiff,"RIFF",4)!=0)
                        ||(memcmp(WAVHeaderInfo.szWaveFmt,"WAVEfmt ",8) != 0)
                        ||((WAVHeaderInfo.nChannels != 1)&&(WAVHeaderInfo.nChannels != 2))
                        ||((WAVHeaderInfo.wBitsPerSample != 8)&&(WAVHeaderInfo.wBitsPerSample != 16))
                        ||(WAVHeaderInfo.nAvgBytesPerSec != WAVHeaderInfo.nChannels*WAVHeaderInfo.wBitsPerSample*WAVHeaderInfo.nSamplesPerSec/8)
                        ||(WAVHeaderInfo.nBlockAlign != WAVHeaderInfo.nChannels*WAVHeaderInfo.wBitsPerSample/8))
                {
                    MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]parse header error\n");
                    return MCI_ERR_BAD_FORMAT;
                }
            }
            else
            {
                if((memcmp(WAVHeaderInfo.szRiff,"RIFF",4)!=0)
                        ||(memcmp(WAVHeaderInfo.szWaveFmt,"WAVEfmt ",8) != 0)
                        ||((WAVHeaderInfo.nChannels != 1)&&(WAVHeaderInfo.nChannels != 2)))
                {
                    MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]parse ADPCM header error\n");
                    return MCI_ERR_BAD_FORMAT;
                }

            }

            //check samplerate
            for(i=0; i<sizeof(WAVSamplesRate)/sizeof(WAVSamplesRate[0]); i++)
            {
                if(WAVHeaderInfo.nSamplesPerSec==WAVSamplesRate[i])
                {
                    break;
                }
            }

            if(i>=sizeof(WAVSamplesRate)/sizeof(WAVSamplesRate[0]))
            {
                return MCI_ERR_INVALID_FORMAT;
            }


            if(WAVHeaderInfo.dwFmtSize>=18)     /* We're obviously dealing with WAVEFORMATEX */
            {
                UINT32 cbSize,size;

                size = WAVHeaderInfo.dwFmtSize -18;

                readlen=FS_Read(FileHander,(UINT8*)&cbSize,2);

                if(readlen!=2)
                {
                    diag_printf("[MMC_AUDIO_WAV]Optional infomation read error!:%d\n",readlen);
                    return MCI_ERR_ERROR;
                }

                cbSize = MIN(size, cbSize);

                diag_printf("[MMC_AUDIO_WAV]cbSize:%d\n",cbSize);

                if(cbSize > 0)
                {
                    if( (cbSize >=22)&&( WAVHeaderInfo.wFormatTag==0xfffe ))  /* WAVEFORMATEXTENSIBLE */
                    {
                        diag_printf("[MMC_AUDIO_WAV] ERROR!!!:  Don't support  WAVEFORMATEXTENSIBLE mode!!\n");
                        return MCI_ERR_BAD_FORMAT;
                    }

                    result=(INT32)FS_Seek(FileHander,(INT64)cbSize,FS_SEEK_CUR);

                }

                size -= cbSize;

                /* It is possible for the chunk to contain garbage at the end */
                if (size > 0)
                {

                    INT64 result2;
                    result2=FS_Seek(FileHander,(INT64)size,FS_SEEK_CUR);

                    if(result2<0 )
                    {
                        MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_WAV_ERR]FS_seek ERROR!");
                        return MCI_ERR_BAD_FORMAT;
                    }
                }


            }
            readlen=FS_Read(FileHander,(UINT8 *)&WAVDataHeaderInfo,sizeof(WAVDataHeader));
            if(readlen!=sizeof(WAVDataHeader))
            {
                MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]data header read len error:%d\n",readlen);
                return MCI_ERR_ERROR;
            }
            if(memcmp(WAVDataHeaderInfo.szData, "fact", 4) == 0)
            {
                result=(INT32)FS_Seek(FileHander, (INT64)WAVDataHeaderInfo.dwDataSize,FS_SEEK_CUR);
                readlen=FS_Read(FileHander,(UINT8 *)&WAVDataHeaderInfo,sizeof(WAVDataHeader));
                if(readlen!=sizeof(WAVDataHeader))
                {
                    MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]data header read len error:%d\n",readlen);
                    return MCI_ERR_ERROR;
                }
            }
            if(memcmp(WAVDataHeaderInfo.szData, "data", 4) != 0)
            {

                MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]data header error [%d][%d][%d][%d]\n",
                           ChunkInfoID[0],ChunkInfoID[1],ChunkInfoID[2],ChunkInfoID[3]);
                return MCI_ERR_BAD_FORMAT;
            }

            if(WAVDataHeaderInfo.dwDataSize == 0)
            {
                return MCI_ERR_END_OF_FILE;
            }




            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS_WAV] SampleRate: %d\n", WAVHeaderInfo.nSamplesPerSec);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS_WAV] nbChannel: %d\n", WAVHeaderInfo.nChannels);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS_WAV] wBitsPerSample: %d\n", WAVHeaderInfo.wBitsPerSample);



            if(WAVHeaderInfo.wFormatTag !=1)
            {
                //ADPCM

                PlayInformation->DurationTime= (UINT32)(((UINT64)DurationLenth*8*1000)/WAVHeaderInfo.nAvgBytesPerSec);
                PlayInformation->BitRate=WAVHeaderInfo.nAvgBytesPerSec;

            }
            else
            {
                if(WAVHeaderInfo.nSamplesPerSec!=0&&WAVHeaderInfo.nChannels!=0&&WAVHeaderInfo.wBitsPerSample!=0)
                {
                    PlayInformation->DurationTime=(UINT32)(((UINT64)DurationLenth*8*1000)/(WAVHeaderInfo.nSamplesPerSec*WAVHeaderInfo.nChannels*WAVHeaderInfo.wBitsPerSample));//ms

                    if(PlayInformation->DurationTime>0)
                        PlayInformation->BitRate=(((UINT64)DurationLenth)*8*1000)/PlayInformation->DurationTime;
                    else
                        PlayInformation->BitRate=0;




                }
                else
                {
                    MCI_TRACE (MCI_AUDIO_TRC|TSTDOUT,0,"[MMC_AUDIO_WAV]error header:nbChannel: %d, wBitsPerSample: %d:wBitsPerSample: %d",
                               WAVHeaderInfo.nChannels, WAVHeaderInfo.wBitsPerSample,WAVHeaderInfo.wBitsPerSample);

                    return MCI_ERR_UNKNOWN_FORMAT;

                }
            }

            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]WAV Duration Time :%d ms",PlayInformation->DurationTime);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]WAV BitRate :%d bps",PlayInformation->BitRate);



        }
        break;

        case MCI_TYPE_AMR:
        {
            INT32  AMRMode;
            UINT8 AMEHeader[8]= {0};
            INT32 AMRSizePerFrame;
            INT32 readlen;

            readlen = FS_Read(FileHander, AMEHeader, 7);

            if(readlen<0)
            {
                return MCI_ERR_CANNOT_OPEN_FILE;
            }

            if (!((AMEHeader[0]==0x23)&&(AMEHeader[1]==0x21)&&(AMEHeader[2]==0x41)&&(AMEHeader[3]==0x4d)&&(AMEHeader[4]==0x52)&&(AMEHeader[5]==0x0a)))
                return MCI_ERR_INVALID_FORMAT;


            AMRMode=(AMEHeader[6]>>3)&0xf;

            while(AMRMode >= 8)
            {

                if(AMRMode == 8)
                    FS_Seek(FileHander, 5, FS_SEEK_CUR);
                else if(AMRMode < 0xf)
                    return MCI_ERR_INVALID_FORMAT;

                readlen = FS_Read(FileHander, AMEHeader, 1);

                if(readlen != 1)
                    return MCI_ERR_CANNOT_OPEN_FILE;

                AMRMode=(AMEHeader[0]>>3)&0xf;
            }


            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_AUDIO_AMR]AMR Mode :%d",AMRMode);

            switch (AMRMode)
            {
                case MCI_PLAY_MODE_AMR475:
                    AMRSizePerFrame=13;
                    break;
                case MCI_PLAY_MODE_AMR515:
                    AMRSizePerFrame=14;
                    break;
                case MCI_PLAY_MODE_AMR59:
                    AMRSizePerFrame=16;
                    break;
                case MCI_PLAY_MODE_AMR67:
                    AMRSizePerFrame=18;
                    break;
                case MCI_PLAY_MODE_AMR74:
                    AMRSizePerFrame=20;
                    break;
                case MCI_PLAY_MODE_AMR795:
                    AMRSizePerFrame=21;
                    break;
                case MCI_PLAY_MODE_AMR102:
                    AMRSizePerFrame=27;
                    break;
                case MCI_PLAY_MODE_AMR122:
                    AMRSizePerFrame=32;
                    break;
                default:
                    return MCI_ERR_INVALID_FORMAT;
                    break;
            }

            PlayInformation->DurationTime=(UINT32)((((UINT64)DurationLenth)/AMRSizePerFrame)*20);//ms

            if(PlayInformation->DurationTime>0)
                PlayInformation->BitRate=(((UINT64)DurationLenth)*8*1000)/(PlayInformation->DurationTime);
            else
                PlayInformation->BitRate=0;



            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]AMR Duration Time :%d ms",PlayInformation->DurationTime);
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_APFS]AMR BitRate :%d bps",PlayInformation->BitRate);
        }
        break;
#ifdef MEDIA_MIDI_SUPPORT
        case MCI_TYPE_MIDI:
            PlayInformation->DurationTime=midi_GetDurationTime( FileHander,  TotalFileSize, BeginPlayProgress, OffsetPlayProgress);

            if(PlayInformation->DurationTime>0)
                PlayInformation->BitRate=((UINT64)DurationLenth*8*1000)/PlayInformation->DurationTime;
            else
                PlayInformation->BitRate=0;


            break;
#endif

#ifdef WMA_DECODE
        case MCI_TYPE_WMA:
            PlayInformation->DurationTime=Audio_WMAGetDurationTime( FileHander,  TotalFileSize, BeginPlayProgress, OffsetPlayProgress);

            if(PlayInformation->DurationTime>0)
                PlayInformation->BitRate=((UINT64)DurationLenth*8*1000)/PlayInformation->DurationTime;
            else
                PlayInformation->BitRate=0;

            break;
#endif
        default:

            return MCI_ERR_UNKNOWN_FORMAT;
            break;
    }

    //resume the pointer location.
    if(FS_Seek(FileHander, 0, FS_SEEK_SET)<0)return MCI_ERR_UNKNOWN_FORMAT;

    return MCI_ERR_NO;
}

// =============================================================================
// apfs_Stop
// -----------------------------------------------------------------------------
/// This function stops the audio stream playback.
/// If the function returns
/// #APFS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #APFS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #APFS_ERR_NO if it can execute the command.
// =============================================================================

/*
PUBLIC APFS_ERR_T apfs_Stop(VOID)
{
    APFS_PROFILE_FUNCTION_ENTER(apfs_Stop);

    MCI_TRACE (MCI_AUDIO_TRC,0, "APFS Stop");

    aud_Setup(g_apfsItf, &g_apfsAudioCfg);

    // stop stream ...
    aud_StreamStop(g_apfsItf);

    // VPP Speech is used ?
    if (g_apfsSpeechUsed)
    {
        vpp_SpeechClose();
        g_apfsSpeechUsed = FALSE;
    }

    // AMJP is used ?
    if (g_apfsAudioUsed)
    {
        vpp_AudioJpegDecClose();
     g_apfsAudioUsed = FALSE;
    }
   // overlay is over.
   // hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    // and reset global state
    g_apfsItf = 0;
    g_apfsBufferStart = NULL;
    g_apfsBufferTop =  NULL;
    g_apfsBufferCurPos = NULL;
    g_apfsBufferRemainingBytes = 0;
    g_apfsBufferLength = 0;

    g_apfsFramesPlayedNb = 0;
    g_apfsUserHandler = NULL;

    g_apfsAudioCfg.spkLevel = AUD_SPK_MUTE;
    g_apfsAudioCfg.micLevel = AUD_MIC_MUTE;
    g_apfsAudioCfg.sideLevel = 0;
    g_apfsAudioCfg.toneLevel = 0;

    g_apfsLoopMode = FALSE;

    // TODO : mode dependent switch ...
    g_apfsVppCfg.echoEsOn                 = 0;
    g_apfsVppCfg.echoEsVad                = 0;
    g_apfsVppCfg.echoEsDtd                = 0;
    g_apfsVppCfg.echoExpRel               = 0;
    g_apfsVppCfg.echoExpMu                = 0;
    g_apfsVppCfg.echoExpMin               = 0;
    g_apfsVppCfg.encMute = VPP_SPEECH_MUTE;
    g_apfsVppCfg.decMute = VPP_SPEECH_MUTE;
    g_apfsVppCfg.sdf = NULL;
    g_apfsVppCfg.mdf = NULL;
    g_apfsVppCfg.if1 = 0;

    g_apfsFrameSize = 0;
    g_apfsMidBufDetected = FALSE;
    g_apfsDecInput = NULL;


    return APFS_ERR_NO;
}*/





