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
#include "mci.h"

#include "mcip_debug.h"

#include "video_api.h"
//#include "med_main.h"
#include "fs_asyn.h"

#include "audio_api.h"
#include "mmc_camera.h"

#include "mmc_audiomp3stream.h"
#include "vidrec_api.h"

#include "cos.h"
#include "dm.h"
#include "mmc_sndrec.h"
#include "apfs.h"
#include "mmc_dafrtpss.h"

#include "apbs_m.h"
#include "mmc_midi.h"
#include "aud_fm.h"
#include "fmd_m.h"
#include "mcip_debug.h"

#include "hal_overlay.h"

#include "mmc_analogtv.h"
#include "cam_motion_detect.h"


static HANDLE g_ServerTaskHandle = HNULL;

static HANDLE g_AppTaskHandle = HNULL;

static HANDLE g_CodecTaskHandle = HNULL;

static HANDLE g_AgentTaskHandle = HNULL;

static HANDLE g_FsappTaskHandle = HNULL;


static HANDLE g_MMCTaskHandle = HNULL;

static HANDLE g_DITaskHandle = HNULL;

static HANDLE g_ImageTaskHandle = HNULL;

static MCI_PLAY_MODE_T  g_audio_format;

#define COS_MMC_TASKS_PRIORITY_BASE 180

#define MMC_TASK_PRIORITY       (COS_MMC_TASKS_PRIORITY_BASE+0)
#define IMAGE_TASK_PRIORITY (COS_MMC_TASKS_PRIORITY_BASE+1)
#define USB_TASK_PRIORITY       (COS_MMC_TASKS_PRIORITY_BASE+2)

// for asyn fs task.
#define ASYNFS_TASK_PRIORITY (COS_MMC_TASKS_PRIORITY_BASE+3)

#ifdef MEDIA_VOCVID_SUPPORT //define in target.def
#define MMC_TASK_STACK_SIZE     2048*2      // 4k
#else
#define MMC_TASK_STACK_SIZE     2048      // 2k
#endif
#define IMAGE_TASK_STACK_SIZE       2048    // 2k
#define USB_TASK_STACK_SIZE     2048      // 2k

// for asyn fs task.
#define ASYNFS_TASK_STACK_SIZE 2048   // 2k





/**************************************************************

    FUNCTION NAME       : GetMCITaskHandle

    PURPOSE             : Get    Task   Handle

    INPUT PARAMETERS    : module_type mod

    OUTPUT PARAMETERS   : VOID

    RETURNS             : HANDLE

    REMARKS             :

**************************************************************/
HANDLE GetMCITaskHandle(COSMBOXID mod)
{
    switch (mod)
    {
        case MBOX_ID_SERVER:
            return g_ServerTaskHandle;
        case MBOX_ID_APP:
            return g_AppTaskHandle;
        case MBOX_ID_COCEC:
            return g_CodecTaskHandle;
        case MBOX_ID_AGENT:
            return g_AgentTaskHandle;
        case MBOX_ID_FSAPP:
            return g_FsappTaskHandle;
        case MBOX_ID_MMC:
            return g_MMCTaskHandle;
        case MBOX_ID_DI:
            return g_DITaskHandle;
        case MBOX_ID_IMAGE:
            return g_ImageTaskHandle;
        default:
            //ASSERT(0);
            return g_ServerTaskHandle;
    }
}
/**************************************************************

    FUNCTION NAME       : SetMCITaskHandle

    PURPOSE             : Set  MmiTask  Handle

    INPUT PARAMETERS    : module_type mod, HANDLE hTask

    OUTPUT PARAMETERS   : VOID

    RETURNS             : BOOL

    REMARKS             :

**************************************************************/
BOOL SetMCITaskHandle(COSMBOXID mod, HANDLE hTask)
{
    switch (mod)
    {
        case MBOX_ID_SERVER:
            g_ServerTaskHandle = hTask;
            return TRUE;
        case MBOX_ID_APP:
            g_AppTaskHandle = hTask;
            return TRUE;
        case MBOX_ID_COCEC:
            g_CodecTaskHandle = hTask;
            return TRUE;
        case MBOX_ID_AGENT:
            g_AgentTaskHandle = hTask;
            return TRUE;
        case MBOX_ID_FSAPP:
            g_FsappTaskHandle = hTask;
            return TRUE;
        case MBOX_ID_MMC:
            g_MMCTaskHandle = hTask;
            return TRUE;
        case MBOX_ID_DI:
            g_DITaskHandle = hTask;
            return TRUE;
        case MBOX_ID_IMAGE:
            g_ImageTaskHandle = hTask;
            return TRUE;
        default:
            //ASSERT(0);
            return FALSE;
    }
}


extern VOID MMCTask(UINT32 data);
extern VOID ImageTask(UINT32 data);

extern void mmi_ebook_usb_mode_off(void);
extern void mmi_ebook_usb_mode_on(void);

/*
static INT32 MMC_GetLcdWidth()
{
     return LCD_WIDTH;
}

static INT32 MMC_GetLcdHeight()
{
    return LCD_HEIGHT;
}
*/

extern HANDLE g_hAsynFsTask;

INT32 MMC_LcdWidth = 0;
INT32 MMC_LcdHeight = 0;
#ifdef DUAL_LCD
INT32 MMC_SubLcdWidth = 0;
INT32 MMC_SubLcdHeight = 0;
#endif

/*
init lcd size for mci.
*/
BOOL MCI_LcdSizeSet(INT32 LcdWidth, INT32 LcdHeight)
{
    if(LcdWidth<=0 || LcdHeight<=0)
        return FALSE;
    MMC_LcdWidth = LcdWidth;
    MMC_LcdHeight = LcdHeight;
    return TRUE;
}

#ifdef DUAL_LCD
BOOL MCI_SubLcdSizeSet(INT32 LcdWidth, INT32 LcdHeight)
{
    if(LcdWidth <= 0 || LcdHeight <= 0)
        return FALSE;
    MMC_SubLcdWidth = LcdWidth;
    MMC_SubLcdHeight = LcdHeight;
    return TRUE;
}
#endif

INT32  MMC_GetBVGA(INT32 bvga)
{
    return  bvga;
}

BOOL MCI_TaskInit(VOID)  //  MCI_MEDIA_PLAY_REQ,
{

    diag_printf( "*******************MCI_TaskInit**********************");
    #ifndef __TTS_FEATURES__
    mmc_MemInit();
    #endif
    //MMC_LcdWidth=MMC_GetLcdWidth();
    //MMC_LcdHeight=MMC_GetLcdHeight();
    HANDLE hTask;
    hTask = COS_CreateTask((PTASK_ENTRY)MMCTask,
                           NULL, NULL,
                           MMC_TASK_STACK_SIZE,
                           MMC_TASK_PRIORITY,
                           COS_CREATE_DEFAULT, 0, "MMC Task");
    //ASSERT(hTask != HNULL);
    SetMCITaskHandle(MBOX_ID_MMC, hTask);

// creat asyn fs task.
    g_hAsynFsTask = COS_CreateTask(BAL_AsynFsTask,
                                   NULL, NULL,
                                   ASYNFS_TASK_STACK_SIZE,
                                   ASYNFS_TASK_PRIORITY,
                                   COS_CREATE_DEFAULT, 0, "ASYNFS Task");



    /*
    hTask = COS_CreateTask(MMCTask,
                    NULL, NULL,
                    USB_TASK_STACK_SIZE,
                    USB_TASK_PRIORITY,
                    COS_CREATE_DEFAULT, 0, "USB Task");
        ASSERT(hTask != HNULL);
        SetMCITaskHandle(MBOX_ID_MMC, hTask);*/

    return TRUE;

}






UINT32 MCI_Speach(boolean on) //chenhe for jasperII,for  can't speach on when calling
{
    return 0;
}

PRIVATE MCI_AUDIO_PLAY_CALLBACK_T g_mciAudioFinishedCallback = NULL;
PRIVATE MCI_AUDIO_BUFFER_PLAY_CALLBACK_T g_mciRingFinishedCallback = NULL;
PRIVATE MCI_AUDIO_FILE_RECORD_CALLBACK_T g_mciAudioRecordFinishedCallback = NULL;


VOID MCI_AudioFinished(MCI_ERR_T result)
{
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    if (g_mciAudioFinishedCallback)
    {
        (*g_mciAudioFinishedCallback)(result);
    }
}

VOID MCI_RingFinished(MCI_ERR_T result)
{
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    if (g_mciRingFinishedCallback)
    {
        (*g_mciRingFinishedCallback)(result);
        g_mciRingFinishedCallback = NULL;
    }
}

VOID MCI_AudioRecordFinished(MCI_ERR_T result)
{
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    if (g_mciAudioRecordFinishedCallback)
    {
        (*g_mciAudioRecordFinishedCallback)(result);
    }
}

#if (CSW_EXTENDED_API_AUDIO_VIDEO == 0)

#ifdef AUD_3_IN_1_SPK


volatile INT16 g_MCIAudioVibrator=FALSE;


VOID MCI_AudioVibratorStart(VOID)
{

    diag_printf( "*START*");


    g_MCIAudioVibrator=TRUE;
    aud_BypassNotchFilterReq(TRUE);

    return;
}


VOID MCI_AudioVibratorStop(VOID)
{
    diag_printf( "*STOP*");

    g_MCIAudioVibrator=FALSE;
    aud_BypassNotchFilterReq(FALSE);

    return;
}
#endif

mci_type_enum audio_mode;
void MCI_vid_play_finish_ind(int32 result)
{
    if (g_mciAudioFinishedCallback)
    {
        (*g_mciAudioFinishedCallback)(MCI_ERR_END_OF_FILE);
    }
}

void MCI_AudioSetFinishCallbask(MCI_AUDIO_PLAY_CALLBACK_T callback)
{
    g_mciAudioFinishedCallback=callback;
}

UINT32 MCI_AudioPlay (INT32 OutputPath,HANDLE fileHandle, mci_type_enum fielType,MCI_AUDIO_PLAY_CALLBACK_T callback,INT32 PlayProgress)   //  MCI_MEDIA_PLAY_REQ,
{
    INT32 result=MCI_ERR_NO;
    diag_printf( "***********OutputPath:%d*****file_name_p:%d, PlayProgress :%d ,fielType :%d ",OutputPath,fileHandle,PlayProgress,fielType);
    g_mciAudioFinishedCallback=callback;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_78M);

    audio_mode = fielType;

    if((audio_mode >=MCI_TYPE_3GP)&&(audio_mode <=MCI_TYPE_MJPG))
    {

        if(PlayProgress ==0)
        {
            result=  MCI_VideoOpenFile(2,2, fileHandle, 0, 0, fielType, MCI_vid_play_finish_ind,NULL);

            if (result !=ISOM_ERR_OK)
            {
                result = MCI_VideoClose();
                return MCI_ERR_ERROR;
            }
            MCI_VideoPlay (0,0)  ;
        }
        else
        {
            INT16 img_width = 0;
            INT16 img_height = 0;
            INT32 totaltime = 0;

            UINT16  aud_channel;
            UINT16  aud_sample_rate;
            UINT16  track;
            UINT16  audio_type;
            INT64  current_Progress;

            result=MCI_VideoOpenFile(2,2, fileHandle, 0, 0, fielType, MCI_vid_play_finish_ind,NULL);
            audio_type = MCI_VideoGetInfo(&img_width,&img_height, &totaltime,&aud_channel,&aud_sample_rate,& track);

            current_Progress= (((INT64)PlayProgress*(INT64)totaltime)/10000);

            MCI_VideoSeek (current_Progress, SEEK_TIME_MODE_ABSOLUTE, 0, 0) ;
            MCI_VideoPlay (0,0)  ;
        }

    }
    else
        result = LILY_AudioPlay(OutputPath, fileHandle,fielType, PlayProgress);
    if (result != MCI_ERR_NO)
    {
        csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    }

    return result;
}


UINT32 MCI_AudioPause(VOID)
{
    INT32 result;
    result = LILY_AudioPause();
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    return result;
}

extern INT MMI_FS_Seek(HANDLE FileHandle, INT32 Offset, INT32 Whence);

UINT32 MCI_AudioGetDurationTime(HANDLE fileHandle, mci_type_enum fielType,INT32 BeginPlayProgress,INT32 OffsetPlayProgress,MCI_ProgressInf* PlayInformation)
{
    INT32 result =MCI_ERR_NO;

    if((audio_mode >=MCI_TYPE_3GP)&&(audio_mode <=MCI_TYPE_MJPG))
    {

        INT16 img_width = 0;
        INT16 img_height = 0;
        INT32 totaltime = 0;

        UINT16  aud_channel;
        UINT16  aud_sample_rate;
        UINT16  track;
        UINT16  audio_type;
        INT64  current_Progress;

        MMI_FS_Seek(fileHandle, 0, 0);
        result=MCI_VideoOpenFile(2,2, fileHandle, 0, 0, fielType, MCI_vid_play_finish_ind,NULL);

        MCI_VideoGetInfo(&img_width,&img_height, &totaltime,&aud_channel,&aud_sample_rate,& track);

        current_Progress= (((INT64)OffsetPlayProgress*(INT64)totaltime)/10000);

        MCI_VideoSeek (current_Progress, SEEK_TIME_MODE_ABSOLUTE, 0, 0) ;

        PlayInformation->DurationTime= MCI_VideoGetPlayTime();//*10000/ totaltime;

        result = MCI_VideoStop();
        result = MCI_VideoClose();

    }
    else
        result = apfs_GetDurationTime( fileHandle,  fielType, BeginPlayProgress, OffsetPlayProgress, PlayInformation);

    return  result;
}


UINT32 MCI_AudioResume(HANDLE fileHandle)
{
    INT32 result;
    diag_printf( "##################MCI_AudioResume############################");
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_78M);
    result = LILY_AudioResume( fileHandle );
    return result;
}


UINT32 MCI_AudioStop(VOID)
{
    INT32 result;
    if((audio_mode >=MCI_TYPE_3GP)&&(audio_mode <=MCI_TYPE_MJPG))
    {
        result = MCI_VideoStop();
        result = MCI_VideoClose();
    }
    else
        result = LILY_AudioStop();

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    return result;
}


UINT32 MCI_AudioGetID3(char *pfilename)     //  MCI_MEDIA_PLAY_REQ,
{
    return 0;
}

UINT32 MCI_AudioGetPlayInformation(MCI_PlayInf* PlayInformation)     //  MCI_MEDIA_PLAY_REQ,
{

    INT32 result =MCI_ERR_NO;
    if((audio_mode >=MCI_TYPE_3GP)&&(audio_mode <=MCI_TYPE_MJPG))
    {

        INT16 img_width = 0;
        INT16 img_height = 0;
        INT32 totaltime = 0;

        UINT16  aud_channel;
        UINT16  aud_sample_rate;
        UINT16  track;

        MCI_VideoGetInfo(&img_width,&img_height, &totaltime,&aud_channel,&aud_sample_rate,& track);
        if(totaltime>0)
            PlayInformation->PlayProgress= (INT32)((INT64)MCI_VideoGetPlayTime()*10000  / totaltime);
        else
            PlayInformation->PlayProgress=0;

    }
    else
        result = LILY_AudioGetPlayInformation(PlayInformation);

    return result;
}

UINT32 MCI_AudioSetVolume(UINT16 volume)
{
    return 0;
}


UINT32 MCI_AudioSetOutputPath(UINT16 OutputPath,UINT16 Onoff)
{
    return 0;
}


UINT32 MCI_AudioSetEQ(AUDIO_EQ EQMode)
{
    INT32 result=0;

    result = LILY_AudioSetEQ(EQMode);

    return result;
}


MCI_ERR_T MCI_AudioGetFileInformation (CONST HANDLE FileHander,
                                       AudDesInfoStruct  * CONST DecInfo,
                                       CONST mci_type_enum FileType   )
{
    INT32 result=0;

    if((FileType >=MCI_TYPE_3GP)&&(FileType <=MCI_TYPE_MJPG))
    {

        INT16 img_width = 0;
        INT16 img_height = 0;
        INT32 totaltime = 0;

        UINT16  aud_channel;
        UINT16  aud_sample_rate;
        UINT16  track;
        UINT16  audio_type;

        result=  MCI_VideoOpenFile(2,2, FileHander, 0, 0, FileType, MCI_vid_play_finish_ind,NULL);
        audio_type= MCI_VideoGetInfo(&img_width,&img_height, &totaltime,&aud_channel,&aud_sample_rate,& track);

        if(audio_type ==MCI_TYPE_AMR)
        {
            DecInfo->sampleRate=0;
            DecInfo->stereo    = 0;
        }
        else
        {
            DecInfo->sampleRate=aud_sample_rate;
            DecInfo->stereo    = aud_channel;
        }

        result = MCI_VideoClose();
        return MCI_ERR_NO;
    }
    else
        return apfs_GetFileInformation(FileHander, DecInfo, FileType);
}


#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */

extern MPEG_PLAY MpegPlayer;

MPEG_INPUT *mpeg_input_p = &(MpegPlayer.MpegInput);



void mmc_audio_bt_sco(bool flag)
{
#ifdef __BT_AUDIO_VIA_SCO__
    MpegPlayer.MpegInput.bt_sco = flag;
    MpegPlayer.MpegInput.pcmflag = TRUE;
#else
    MpegPlayer.MpegInput.bt_sco = FALSE;
    MpegPlayer.MpegInput.pcmflag = FALSE;

#endif

}


#ifdef __BT_A2DP_PROFILE__
A2DP_codec_struct *MCI_A2dpDafOpen( A2DP_Callback pHandler,
                                    bt_a2dp_audio_cap_struct *daf_config_data,
                                    UINT8 *buf, UINT32 buf_len)
{


    mpeg_input_p->audio_config_handle = daf_config_data;

    return A2DP_DAF_Open( &(mpeg_input_p->ss_handle),  pHandler, &(mpeg_input_p->audio_config_handle)->codec_cap.mp3, buf,  buf_len);
}



void MCI_A2dpDafClose(void)
{
    A2DP_DAF_Close(&(mpeg_input_p->ss_handle));
    return;
}

extern A2DP_codec_struct *SBC_Open( SSHdl **ss_handle,  A2DP_Callback pHandler,
                                    bt_a2dp_sbc_codec_cap_struct *sbc_config_data,
                                    UINT8 *buf,   UINT32 buf_len );

extern VOID SBC_Close( SSHdl **ss_handle );

A2DP_codec_struct *MCI_SBCOpen(    A2DP_Callback pHandler,
                                   bt_a2dp_audio_cap_struct *daf_config_data,
                                   UINT8 *buf, UINT32 buf_len)
{

    mpeg_input_p->audio_config_handle = daf_config_data;
    return SBC_Open( &(mpeg_input_p->ss_handle),  pHandler, &(mpeg_input_p->audio_config_handle)->codec_cap.sbc, buf,  buf_len);

    return 0;
}


void MCI_SBCClose(void )
{
    SBC_Close(&(mpeg_input_p->ss_handle));
    return;
}




#endif
extern AUD_LEVEL_T audio_cfg;
extern AUD_ITF_T audioItf;

//
//UINT32 MCI_AudioPlayBuffer(UINT32 *pBuffer, UINT32 len, UINT8 loop,MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callbackenum formatINT32 startPosition)
//?òλü?
//UINT32  MCI_AudioBufferplay_getinfo(struct* info),
//?
//UINT32 MCI_AudioPauseBufferplay(),
//?
//UINT32 MCI_AudioResumeBufferplay(),
//??
//UINT32MCI_AudioStopBuffer

INT32  MCI_AudioPlayBuffer(INT32 *pBuffer, UINT32 len, UINT8 loop,MCI_AUDIO_BUFFER_PLAY_CALLBACK_T callback,MCI_PLAY_MODE_T  format,INT32 startPosition)
{
    APBS_ENC_STREAM_T stream;
    APBS_AUDIO_CFG_T apbsAudioCfg =
    {
        .spkLevel = audio_cfg.spkLevel,
        .filter = NULL
    };

    if(format == MCI_PLAY_MODE_AMR_RING)
    {
        stream.startAddress = (UINT32)pBuffer+6;
        stream.length = len*4-6;
    }
    else
    {
        stream.startAddress = (UINT32)pBuffer;
        stream.length = len*4;
    }
    stream.mode = format;
    stream.handler = NULL;

    g_mciRingFinishedCallback=callback;
    g_audio_format = format;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);

    INT32 result;
    if(format==MCI_PLAY_MODE_MID)
    {
#ifdef MEDIA_MIDI_SUPPORT
        result = MMC_Midi2RawBuffer(pBuffer,  len*4+4, loop);
#else
        result = MCI_ERR_UNKNOWN_FORMAT;
#endif
    }
    else
    {
        result = apbs_Play(audioItf, &stream, &apbsAudioCfg, loop);
    }
    //result = AudioMp3StartStream((uint32 *)pBuffer,len,loop);

    if (result != MCI_ERR_NO)
    {
        csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    }

    return result;
}

INT32  MCI_AUD_StreamPlayPCM( INT32 *pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback,UINT32 sampleRate, UINT32  bitPerSample)

{

    APBS_ENC_STREAM_T stream;
    APBS_AUDIO_CFG_T apbsAudioCfg =
    {
        .spkLevel = audio_cfg.spkLevel,
        .filter = NULL
    };


    stream.startAddress = (UINT32)pBuffer;
    stream.length = len;
    stream.mode = mciFormat;
    stream.channelNb = 1;
    stream.sampleRate = sampleRate;//HAL_AIF_FREQ_16000HZ
    stream.bitPerSample = bitPerSample;

    stream.handler =(APBS_USER_HANDLER_T) callback;

    g_mciRingFinishedCallback=NULL;
    g_audio_format = mciFormat;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);

    INT32 result;

    if(mciFormat == MCI_PLAY_MODE_MID)
    {
        result = APBS_ERR_BAD_PARAMETER;
    }
    else
    {
        result =  apbs_Play(audioItf, &stream, &apbsAudioCfg, TRUE);
    }

    if (result != MCI_ERR_NO)
    {
        csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    }

    return result;
}


INT32  MCI_AUD_StreamPlay( INT32 *pBuffer, UINT32 len, MCI_PLAY_MODE_T mciFormat, APBS_STREAM_USER_HANDLER_T callback)
{
    APBS_ENC_STREAM_T stream;
    APBS_AUDIO_CFG_T apbsAudioCfg =
    {
        .spkLevel = audio_cfg.spkLevel,
        .filter = NULL
    };


    stream.startAddress = (UINT32)pBuffer;
    stream.length = len;
    stream.mode = mciFormat;
    stream.handler =(APBS_USER_HANDLER_T) callback;

    g_mciRingFinishedCallback=NULL;
    g_audio_format = mciFormat;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);

    INT32 result;
    if(mciFormat == MCI_PLAY_MODE_MID)
    {
        result = APBS_ERR_BAD_PARAMETER;
    }
    else
    {
        result = apbs_Play(audioItf, &stream, &apbsAudioCfg, TRUE);
    }

    if (result != MCI_ERR_NO)
    {
        csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    }

    return result;
}


INT32  MCI_AudioStopBuffer(void)
{
    INT32 result;
    if(g_audio_format==MCI_PLAY_MODE_MID)
    {
#ifdef MEDIA_MIDI_SUPPORT
        result = Audio_MidiStop();
#else
        result = APBS_ERR_NO;
#endif
    }
    else
    {
        result = apbs_Stop();
    }
    //result = AudioMp3StopStream();

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);

    if (g_mciRingFinishedCallback)
    {
        (*g_mciRingFinishedCallback)(result);
        g_mciRingFinishedCallback = NULL;
    }

    return result;
}

UINT32 MCI_AudioPlayPause(VOID)
{
    INT32 result;
    if(g_audio_format==MCI_PLAY_MODE_MID)
    {
        result = APBS_ERR_BAD_PARAMETER;
    }
    else
    {
        result = apbs_Pause(TRUE);
    }

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);

    return result;
}

UINT32 MCI_AudioPlayResume(VOID)
{
    INT32 result;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);

    if(g_audio_format==MCI_PLAY_MODE_MID)
    {
        result = APBS_ERR_BAD_PARAMETER;
    }
    else
    {
        result = apbs_Pause(FALSE);
    }

    return result;
}

UINT32* MCI_GetBufPosition(VOID)
{
    return apbs_GetBufPosition();
}

void MCI_GetWriteBuffer( UINT32 **buffer, UINT32 *buf_len )
{
    apbs_GetWriteBuffer(buffer, buf_len);
}


UINT32 MCI_GetRemain(VOID)
{
    return apbs_GetRemain();
}
void MCI_DataFinished( void )
{
    apbs_DataFinished();
}
INT32 MCI_AddedData(UINT32 addedDataBytes)
{
    return apbs_AddedData(addedDataBytes);
}

void  MCI_SetBuffer(UINT32 *buffer, uint32 buf_len)
{
    apbs_SetBuffer(buffer, buf_len);
}

#ifdef SOUND_RECORDER_SUPPORT
UINT32 MCI_AudioRecordStart (HANDLE fhd,mci_type_enum format,U8 quality, MCI_AUDIO_FILE_RECORD_CALLBACK_T callback,  MCI_AUDIO_RECORD_BUFFER_PLAY_CALLBACK_T usercallback)
{
    g_mciAudioRecordFinishedCallback=callback;

    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);

    INT32 result = Mmc_sndRecStart(fhd,quality, format, callback, usercallback);
    if (result != MCI_ERR_NO)
    {
        csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    }

    return result;
}

UINT32 MCI_AudioRecordPause(VOID)
{
    INT32 result = Mmc_sndRecPause();
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    return result;
}
UINT32 MCI_AudioRecordResume(VOID)
{
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_104M);
    INT32 result = Mmc_sndRecResume();
    return result;
}
UINT32 MCI_AudioRecordStop(VOID)
{
    INT32 result = Mmc_sndRecStop();
    csw_SetResourceActivity(CSW_LP_RESOURCE_AUDIO_PLAYER, CSW_SYS_FREQ_32K);
    return result;
}
#endif

//FMRADIO START
UINT32 MCI_FM_RDA5800_init(uint8 scl,uint8 sda)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_close(VOID)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_open(UINT32 bUseLastFreq)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_tune(UINT32 iFrequency,UINT32 isStereo)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_mute(UINT32 isMute)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_setVolume(UINT32 iVol)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_seek(UINT32 isSeekUp)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_I2S(VOID)
{
    return 0;
}

UINT32 MCI_FM_RDA5800_STOPI2S(VOID)
{
    return 0;
}


VOID MCI_FmSetOutputDevice(MCI_AUDIO_PATH_T device)
{
    AUD_SPK_T out = AUD_SPK_EAR_PIECE;
    if (device == MCI_PATH_HP || device == MCI_PATH_FM_HP)
    {
        out = AUD_SPK_EAR_PIECE;
    }
    else if (device == MCI_PATH_LSP || device == MCI_PATH_FM_LSP_HP)
    {
        out = AUD_SPK_LOUD_SPEAKER;
    }

    aud_SetCurOutputDevice(out);
}


extern uint32 pcmbuf_overlay[4608];

static BOOL g_FmBT8KEnbleFlag=0;

UINT32 MCI_FmPlay(void)
{
    AUD_ERR_T audError;
    HAL_AIF_STREAM_T stream;
    AUD_LEVEL_T level;
    AUD_ITF_T itf;

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_FM]MCI_FmPlay!");

    level.spkLevel=AUD_SPK_MUTE; // MMI will set the volumn later
    level.sideLevel=0;
    level.micLevel=0;
    level.toneLevel=0;
    // Set audio interface
    stream.startAddress = NULL;
    stream.length = 6400;
    stream.playSyncWithRecord = FALSE;

    // initialize the interrupt function.
    stream.halfHandler = NULL;
    stream.endHandler = NULL;

    if(g_FmBT8KEnbleFlag==1)
    {
        stream.sampleRate = HAL_AIF_FREQ_8000HZ;
        stream.channelNb = HAL_AIF_MONO;
        stream.voiceQuality = TRUE;

        //hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

        FMD_ERR_T fmdError = FMD_ERR_NO;
        FMD_I2S_CFG_T fmdI2sCfg= {0};

        fmdI2sCfg.freq=FMD_FREQ_8000HZ;

        fmdError= fmd_I2sOpen( fmdI2sCfg);

        if (fmdError != FMD_ERR_NO)
        {
            MCI_TRACE(MCI_AUDIO_TRC, 0,     "[MCI]fmd_I2sOpen FAILED");
            return AUD_ERR_RESOURCE_BUSY;
        }
        else
        {
            MCI_TRACE(MCI_AUDIO_TRC, 0,     "[MCI]fmd_I2sOpen SUCCESS");
        }
    }
    else
    {
#if	(defined (CHIP_DIE_8809E) | defined (CHIP_DIE_8955))
        stream.sampleRate = HAL_AIF_FREQ_48000HZ;
#else
        stream.sampleRate = HAL_AIF_FREQ_32000HZ;
#endif
        stream.channelNb = HAL_AIF_STEREO;
        stream.voiceQuality = FALSE;

        //hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);

        FMD_ERR_T fmdError = FMD_ERR_NO;
        FMD_I2S_CFG_T fmdI2sCfg= {0};

        switch (stream.sampleRate)
        {
            case HAL_AIF_FREQ_8000HZ:
                fmdI2sCfg.freq=FMD_FREQ_8000HZ;
                break;

            case HAL_AIF_FREQ_11025HZ:
                fmdI2sCfg.freq=FMD_FREQ_11025HZ;
                break;

            case HAL_AIF_FREQ_12000HZ:
                fmdI2sCfg.freq=FMD_FREQ_12000HZ;
                break;

            case HAL_AIF_FREQ_16000HZ:
                fmdI2sCfg.freq=FMD_FREQ_16000HZ;
                break;

            case HAL_AIF_FREQ_22050HZ:
                fmdI2sCfg.freq=FMD_FREQ_22050HZ;
                break;

            case HAL_AIF_FREQ_24000HZ:
                fmdI2sCfg.freq=FMD_FREQ_24000HZ;
                break;

            case HAL_AIF_FREQ_32000HZ:
                fmdI2sCfg.freq=FMD_FREQ_32000HZ;
                break;

            case HAL_AIF_FREQ_44100HZ:
                fmdI2sCfg.freq=FMD_FREQ_44100HZ;
                break;

            case HAL_AIF_FREQ_48000HZ:
                fmdI2sCfg.freq=FMD_FREQ_48000HZ;
                break;
            default:
                MCI_TRACE(MCI_AUDIO_TRC, 0,     "[MCI]Sample rate erro:%d",stream.sampleRate);
                break;
        }

        fmdI2sCfg.slave=TRUE;
#ifdef FM_RADIO_ENABLE
        fmdError= fmd_I2sOpen( fmdI2sCfg);
#endif
        if (fmdError != FMD_ERR_NO)
        {
            MCI_TRACE(MCI_AUDIO_TRC, 0,     "[MCI]fmd_I2sOpen FAILED");
            return AUD_ERR_RESOURCE_BUSY;
        }
        else
        {
            MCI_TRACE(MCI_AUDIO_TRC, 0,     "[MCI]fmd_I2sOpen SUCCESS");
        }


    }

    itf = AUD_ITF_FM;

    audError= aud_StreamStart(itf, &stream, &level);

    if (audError != AUD_ERR_NO)
    {
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_FmStreamStart FAILED");
    }
    else
    {
        audioItf = AUD_ITF_FM;
        MCI_TRACE(MCI_AUDIO_TRC, 0,   "[MCI]aud_FmStreamStart SUCCESS");
    }

    return audError;
}

extern U8 GetHandsetInPhone(VOID);

UINT32 MCI_FmStop(VOID)
{
    AUD_ERR_T audError = AUD_ERR_NO;

    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_FM]MCI_FmStop!audioItf = %d",audioItf);


    while((audError=aud_StreamStop(audioItf)) != AUD_ERR_NO)
    {
        COS_Sleep(100);
    }

    // Directly call MMI API GetHandsetInPhone() here.
    // In the future audioItf will be managed only in CSW.
    if (1 == GetHandsetInPhone())
    {
        audioItf = AUD_ITF_EAR_PIECE;
    }
    else
    {
        audioItf = AUD_ITF_RECEIVER;
    }
#ifdef FM_RADIO_ENABLE
    fmd_I2sClose();
#endif
    //hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    return audError;
}


UINT32 MCI_FmSetup(UINT8 volume )
{
    AUD_ERR_T audError = AUD_ERR_NO;
    AUD_LEVEL_T cfg;

    if(volume>7)volume=7;



    cfg.spkLevel=volume;
    cfg.micLevel=0;
    cfg.sideLevel=0;
    cfg.toneLevel=0;



    audError=aud_Setup(audioItf,  & cfg);

    return audError;
}


//fmradio record
UINT32 MCI_FmrRecordStart (char *file_name_p)   //  MCI_MEDIA_PLAY_REQ,
{
    return 0;
}
UINT32 MCI_FmrRecordPause(VOID)
{
    return 0;
}
UINT32 MCI_FmrRecordResume(VOID)
{
    return 0;
}
UINT32 MCI_FmrRecordStop(VOID)
{
    return 0;
}

/**************************************************************************/
/**************************************************************************/
/******************Video function API   ***************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*
    MCI_VID_CLOSE_REQ,          // 7
        MCI_VID_OPEN_FILE_REQ,      // 8
        MCI_VID_FILE_READY_IND,     // 9
        MCI_VID_PLAY_REQ,               // 10
        MCI_VID_PLAY_FINISH_IND,        // 11
        MCI_VID_STOP_REQ,               // 12
        MCI_VID_PAUSE_REQ,          // 13
        MCI_VID_RESUME_REQ,         // 14
        MCI_VID_SEEK_REQ,               // 15
        MCI_VID_SEEK_DONE_IND,
*/


UINT32  MCI_DisplayVideoInterface (VOID)
{
#ifdef VIDEO_PLAYER_SUPPORT
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_VID_DISPLAY_INTERFACE_REQ);
#endif
    return 0;
}

#if (CSW_EXTENDED_API_AUDIO_VIDEO == 0)

UINT32  MCI_VideoClose (VOID)   //MCI_VID_CLOSE_REQ
{
    INT32 result=0;
#ifdef VIDEO_PLAYER_SUPPORT
    result = LILY_VideoClose();
#endif
    return result;

}

UINT32 MCI_VideoOpenFile (INT32 OutputPath, uint8 open_audio, HANDLE fhd, UINT8 file_mode, UINT32 data_Len, mci_type_enum type, void(*vid_play_finish_callback)(int32), void (*vid_draw_panel_callback)(uint32))   //MCI_VID_OPEN_FILE_REQ
{

    INT32 result=0;
    //INT32 i;

#ifdef VIDEO_PLAYER_SUPPORT
    result     = LILY_VideoOpenFile( fhd, file_mode, data_Len, OutputPath,open_audio, type, vid_play_finish_callback, vid_draw_panel_callback);
#endif
#if 0
    if (result == 0)
    {
        mci_vid_file_ready_ind_struct msg;
        media_vid_file_ready_ind_struct *ind_p=NULL ;
        ilm_struct       *local_ilm_ptr = NULL;
        LILY_VideoGetInfo(&msg);
        ind_p = (media_vid_file_ready_ind_struct*)
                construct_local_para(sizeof(media_vid_file_ready_ind_struct), TD_CTRL);
        if(!ind_p)return 1;
        ind_p->result = (int16)result;
        ind_p->image_width=msg.image_width;
        ind_p->image_height=msg.image_height;
        ind_p->total_frame_num=msg.total_frame_num;
        ind_p->total_time=msg.total_time;
        ind_p->seq_num=msg.seq_num;
        ind_p->media_type =msg.media_type;

        local_ilm_ptr = allocate_ilm(MOD_MED);
        if(!local_ilm_ptr)return 1;
        local_ilm_ptr->src_mod_id = MOD_MED;
        local_ilm_ptr->dest_mod_id = MOD_MMI;
        local_ilm_ptr->sap_id = MED_SAP;

        local_ilm_ptr->msg_id = (msg_type_t)MSG_ID_MEDIA_VID_FILE_READY_IND;
        local_ilm_ptr->local_para_ptr = (local_para_struct*)ind_p;
        local_ilm_ptr->peer_buff_ptr = NULL;

        i=0;
        while(!mmc_SendMsg(GetMmiTaskHandle(local_ilm_ptr->dest_mod_id),local_ilm_ptr->msg_id, (uint32)local_ilm_ptr, 0, 0))
        {
            i++;
            diag_printf( "COS_SendEvent 2 MMI fail i=%d",i);
            COS_Sleep(20);//ms
            if(i>=3)
            {
                result=1;
                break;
            }
        }

    }
#endif

    return result;
}

UINT32 MCI_VideoPlayFinishInd (mci_vid_play_finish_ind_struct *plyFinish)
{
    return 0;
}


UINT32  MCI_VideoGetPlayTime (VOID)
{
#ifdef VIDEO_PLAYER_SUPPORT
    mci_vid_file_ready_ind_struct fileInfo;
    LILY_VideoGetInfo(&fileInfo);
    return fileInfo.current_time;
#else
    return 0;
#endif
}

UINT16  MCI_VideoGetInfo ( UINT16  *image_width, UINT16  *image_height, UINT32  *total_time, UINT16  *aud_channel, UINT16  *aud_sample_rate,UINT16  *track)
{
#ifdef VIDEO_PLAYER_SUPPORT
    mci_vid_file_ready_ind_struct fileInfo;
    LILY_VideoGetInfo(&fileInfo);
    *image_width=fileInfo.image_width;
    *image_height=fileInfo.image_height;
    *total_time=fileInfo.total_time;
    *aud_channel = fileInfo.aud_channel;
    *aud_sample_rate = fileInfo.aud_sample_rate;
    *track = fileInfo.track;
    return fileInfo.audio_type;
#else
    return -1;
#endif

}


UINT32 MCI_VideoRecordFinishInd (INT32 finishCause)
{
    return 0;
}

UINT32 MCI_VideoSeekDoneInd (mci_vid_seek_done_ind_struct *pSeekDone)
{
    return 0;
}

VOID MCI_VideoOpenFileInd (mci_vid_file_ready_ind_struct *vdoOpen)    //MCI_VID_FILE_READY_IND
{
    return;
}

UINT32 MCI_VideoPlay (UINT16 startX, UINT16 startY)   //MCI_VID_PLAY_REQ
{
    INT32 result=0;
#ifdef VIDEO_PLAYER_SUPPORT
    result = LILY_VideoPlay(startX,startY);
#endif
    return result;
}

VOID  MCI_VideoPlayInd (VOID)   // MCI_VID_PLAY_FINISH_IND
{
    return;
}


UINT32 MCI_VideoStop (VOID)    // MCI_VID_STOP_REQ
{
    INT32 result=0;
#ifdef VIDEO_PLAYER_SUPPORT
    result = LILY_VideoStop();
#endif
    return result;
}

UINT32  MCI_VideoPause (VOID)  //  MCI_VID_PAUSE_REQ
{
    INT32 result=0;
#ifdef VIDEO_PLAYER_SUPPORT
    result = LILY_VideoPause();
#endif
    return result;
}

UINT32  MCI_VideoResume (VOID)  // MCI_VID_RESUME_REQ
{
    INT32 result=0;
#ifdef VIDEO_PLAYER_SUPPORT
    result = LILY_VideoResume();
#endif
    return result;

}

UINT32 MCI_VideoSeek (long long playtime, INT32 time_mode, INT16 startX, INT16 startY)   // MCI_VID_SEEK_REQ
{
    INT32 result=0;
    //media_vid_seek_done_ind_struct *ind_p=NULL;
    //ilm_struct       *local_ilm_ptr = NULL;
    //INT32 i;
#ifdef VIDEO_PLAYER_SUPPORT
    result    = LILY_VideoSeek(playtime, time_mode, startX, startY);
#endif
#if 0
    ind_p = (media_vid_seek_done_ind_struct*)
            construct_local_para(sizeof(media_vid_seek_done_ind_struct), TD_CTRL);
    if(!ind_p)return 1;
    ind_p->result = (int16)result;
    local_ilm_ptr = allocate_ilm(MOD_MED);
    if(!local_ilm_ptr)return 1;
    local_ilm_ptr->src_mod_id = MOD_MED;
    local_ilm_ptr->dest_mod_id = MOD_MMI;
    ind_p->seq_num = 0;
    local_ilm_ptr->sap_id = MED_SAP;

    local_ilm_ptr->msg_id = (msg_type_t)MSG_ID_MEDIA_VID_SEEK_DONE_IND;
    local_ilm_ptr->local_para_ptr = (local_para_struct*)ind_p;
    local_ilm_ptr->peer_buff_ptr = NULL;

    i=0;
    while(!mmc_SendMsg(GetMmiTaskHandle(local_ilm_ptr->dest_mod_id),local_ilm_ptr->msg_id, (uint32)local_ilm_ptr, 0, 0))
    {
        i++;
        diag_printf( "COS_SendEvent 2 MMI fail i=%d",i);
        COS_Sleep(20);//ms
        if(i>=3)
        {
            result=1;
            break;
        }
    }
#endif
    return result;
}

VOID  MCI_VideoSeekInd (VOID)  // MCI_VID_SEEK_DONE_IND
{
    return;
}

UINT32 MCI_VideoSet (UINT16 ZoomWidth, UINT16 ZoomHeight, INT16 startX, INT16 startY, INT16 cutX, INT16 cutY, INT16 cutW, INT16 cutH, UINT16 Rotate)  // MCI_VID_SET_MODE_REQ
{
    UINT32 ret=0;
#ifdef VIDEO_PLAYER_SUPPORT
    mci_vid_set_mode_ind_struct SetMode;

    SetMode.lcd_start_x = startX;
    SetMode.lcd_start_y = startY;
    SetMode.zoom_height = ZoomHeight;
    SetMode.zoom_width = ZoomWidth;
    SetMode.cutX= cutX;
    SetMode.cutY= cutY;
    SetMode.cutW= cutW;
    SetMode.cutH= cutH;
    SetMode.rotate=Rotate;
    ret = LILY_VideoSetInfo(&SetMode);
#endif
    return ret;
}

#endif /* CSW_EXTENDED_API_AUDIO_VIDEO */


/**************************************************************************/
/**************************************************************************/
/******************Camera function API***************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*
    MCI_CAM_POWER_UP_REQ,       // 17
        MCI_CAM_POWER_DOWN_REQ, // 18
        MCI_CAM_PREVIEW_REQ,            // 19
        MCI_CAM_STOP_REQ,               // 20
        MCI_CAM_CAPTURE_REQ,            // 21
        MCI_CAM_SET_PARAM_REQ,      // 22
*/

#if (CSW_EXTENDED_API_CAMERA == 0)

#if defined(CAM_MD_SUPPORT)
INT32 cam_md_state=0;//0=close,1=open,2=need reopen
#endif

UINT32 MCI_CamPowerUp (INT32  vediomode,void (*cb)(int32))   // MCI_CAM_POWER_UP_REQ
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamPowerUp.vediomode%d",vediomode);

#if defined(CAM_MD_SUPPORT)
    if(cam_md_state)
    {
        MCI_CamMdClose();
        //only close and not reopen again.
        cam_md_state=0;//reopen need keep 2 parameters.
    }
#endif

    return Lily_Camera_PowerOn(vediomode,cb);
}

UINT32 MCI_CamPowerDown (VOID)  //MCI_CAM_POWER_DOWN_REQ
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamPowerDown");
    return Lily_Camera_PowerOff();
}

UINT32 MCI_CamPlayBack ()  //MCI_CAM_PLAY_BACK
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamPlayBack");

    return Lily_Capture_Preview();
}

INT32  MCI_CamSavePhoto (INT32  filehandle)  //MCI_CAM_SAVE_PHOTO
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamSavePhoto.filehandle:%d",filehandle);
    return Lily_camera_savePhoto(filehandle);
}

UINT32 MCI_CamPreviewOpen(CAM_PREVIEW_STRUCT* data)  // MCI_CAM_PREVIEW_REQ
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamPreviewOpen");
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]start_x:%d",data->start_x);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]start_y:%d",data->start_y);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]end_x:%d",data->end_x);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]end_y:%d",data->end_y);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]image_width:%d",data->image_width);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]image_height:%d",data->image_height);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]nightmode:%d",data->nightmode);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]imageQuality:%d",data->imageQuality);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]factor:%d",data->factor);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]contrast:%d",data->contrast);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]specialEffect:%d",data->specialEffect);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]brightNess:%d",data->brightNess);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]whiteBlance:%d",data->whiteBlance);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]exposure:%d",data->exposure);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]addFrame:%d",data->addFrame);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]===banding:%d",data->banding);

    return Lily_Camera_Preview(data);
}

UINT32 MCI_CamPreviewClose(VOID)  // MCI_CAM_STOP_REQ
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamPreviewClose");
    return Lily_Camera_PreviewOff();
}

UINT32 MCI_CamCapture (CAM_CAPTURE_STRUCT *data)    //MCI_CAM_CAPTURE_REQ
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamCapture");
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]image_width:%d",data->image_width);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]image_height:%d",data->image_height);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]media_mode:%d",data->media_mode);

    return Lily_Camera_Capture(data);
}

UINT32 MCI_CamSetPara(INT32 effectCode,INT32 value)  // MCI_CAM_SET_PARAM_REQ
{
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]MCI_CamSetPara ");
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]effectCode:%d",effectCode);
    MCI_TRACE(MCI_CAMERA_TRC|TSTDOUT, 0,"[MCI_CAMERA]value:%d",value);

    return  Lily_Camera_SetEffect(effectCode,value);
}

UINT32 MCI_CamCancelLastSave(VOID)  // MCI_CAM_PREVIEW_REQ
{
    return 0;
}

#if defined(CAM_MD_SUPPORT)
//open camera motion detect
INT32 MCI_CamMdOpen(INT32 sensLevel, VOID(*md_callback)(int32))
{
    if( motion_detect_open( sensLevel, md_callback)==0)
    {
        cam_md_state=1;
        return 0;
    }
    else
    {
        cam_md_state=0;
        return -1;
    }
}
//close camera motion detect
INT32 MCI_CamMdClose( VOID)
{
    cam_md_state=0;
    return motion_detect_close();
}

VOID MCI_CamDoMd( VOID)
{
    do_motion_detect();
}
#endif // CAM_MD_SUPPORT


#endif /* CSW_EXTENDED_API_CAMERA */

/**************************************************************************/
/**************************************************************************/
/******************Image  function API***************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*
    MCI_IMG_DECODE_REQ,         // 23
    MCI_IMG_DECODE_EVENT_IND,   // 24
    MCI_IMG_DECODE_FINISH_IND,  // 25
    MCI_IMG_RESIZE_REQ,         // 26
    MCI_IMG_STOP_REQ,               // 27
*/


UINT32 MCI_ImgCheckDecodeState(VOID)
{
    return 0;
}

UINT32 MCI_ImgResize (img_decode_req_struct *imgPara)  // MCI_IMG_RESIZE_REQ
{
    return 0;
}
UINT32 MCI_ImgStop (VOID)  //MCI_IMG_STOP_REQ
{
    return 0;
}


INT32 MCI_ImgDisplay(uint8 startx,uint8 starty, uint8 width,uint8 height,char * filename)  // MCI_DI_IMAGE_DISPLAYSTILL_IND
{
    return 0;
}


UINT32 MCI_getLilyStatus(UINT32 cmd_arg)
{
    return 0;
}

#ifdef VIDEO_RECORDER_SUPPORT
UINT32 MCI_VideoRecordPreviewStart (MMC_VDOREC_SETTING_STRUCT *previewPara, VOID(*vid_rec_finish_ind)(uint16 msg_result))
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordPreviewStart!");
    return LILY_VidrecPreviewStartReq(previewPara,vid_rec_finish_ind);
}

UINT32 MCI_VideoRecordAdjustSetting (INT32 adjustItem, INT32 value)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordAdjustSetting!");
    return LILY_VidrecAdjustSettingReq(adjustItem, value);
}

UINT32 MCI_VideoRecordPreviewStop(VOID)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordPreviewStop!");
    return LILY_VidrecPreviewStopReq();
}

UINT32 MCI_VideoRecordStart(HANDLE filehandle)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordStart!");
    return LILY_VidrecRecordStartReq(filehandle);
}

UINT32 MCI_VideoRecordStop( VOID)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordStop!");
    return LILY_VidrecRecordStopReq();
}

UINT32 MCI_VideoRecordPause(VOID)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordPause!");
    return LILY_VidrecRecordPauseReq();
}

UINT32 MCI_VideoRecordResume( VOID)
{
    MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordResume!");
    return LILY_VidrecRecordResumeReq();
}
UINT32 MCI_VideoRecordGetTime(VOID)
{
    //MCI_TRACE(MCI_VIDREC_TRC|TSTDOUT , 0,"[MCI_VIDREC]MCI_VideoRecordGetTime!");
    return LILY_VidrecRecordGetTimeReq();
}
UINT32 MCI_VideoRecordSound(INT32 *PCMdata, UINT32 length)
{
    return 0;
}
#endif




#ifdef __PNG_DECODE_WITH_VOC__




#include "vpp_pngdec.h"




PRIVATE PNGDEC_USER_HANDLER_T g_PngDecUserHandler = NULL;

void PngDecVocISR(INT32 value)
{

    if(g_PngDecUserHandler!=NULL)
    {
        g_PngDecUserHandler(value);
    }


    return;
}




INT32 MCI_PngdecStart(UINT32 *FileContent,
                      UINT32 FileSize,
                      UINT32 *PNGOutBuffer,
                      UINT32 OutBufferSize,
                      UINT32 *AlphaOutBuffer,
                      UINT32 AlphaBufferSize,
                      UINT32 *BkgrdBuffer,
                      UINT16 BkgrdClipX1,
                      UINT16 BkgrdClipX2,
                      UINT16 BkgrdClipY1,
                      UINT16 BkgrdClipY2,
                      INT16 BkgrdOffsetX,
                      INT16 BkgrdOffsetY,
                      UINT16 BkgrdWidth,
                      UINT16 BkgrdHeight,
                      UINT16 ZoomWidth,
                      UINT16 ZoomHeight,
                      UINT16 DecMode,
                      UINT16 alpha_blending_to_transparentColor,
                      UINT32 *ImgWidth,
                      UINT32 *ImgHeigh ,
                      UINT32*WindowBuf,
                      UINT32 *rgb_temp_ptr,
                      UINT32 PngFileHandle,
                      PNGDEC_USER_HANDLER_T handle

                     )
{




    VPP_PNGDEC_CFG_T g_vppPngDecCfg;



    hal_HstSendEvent(0xabcd1233);
    hal_HstSendEvent(FileContent);

    /*      hal_HstSendEvent(FileSize);
            hal_HstSendEvent(PNGOutBuffer);
            hal_HstSendEvent(OutBufferSize);
            hal_HstSendEvent(AlphaOutBuffer);
            hal_HstSendEvent(AlphaBufferSize);
        hal_HstSendEvent(0xabcd1240);
            hal_HstSendEvent(BkgrdBuffer);
            hal_HstSendEvent(BkgrdClipX1);
            hal_HstSendEvent(BkgrdClipX2);
            hal_HstSendEvent(BkgrdClipY1);
            hal_HstSendEvent(BkgrdClipY2);
        hal_HstSendEvent(0xabcd1241);
            hal_HstSendEvent(BkgrdOffsetX);
            hal_HstSendEvent(BkgrdOffsetY);
            hal_HstSendEvent(BkgrdWidth);
            hal_HstSendEvent(BkgrdHeight);
            hal_HstSendEvent(ZoomWidth);
            hal_HstSendEvent(ZoomHeight);
        hal_HstSendEvent(0xabcd1242);
            hal_HstSendEvent(DecMode);
            hal_HstSendEvent(ImgWidth);
            hal_HstSendEvent(ImgHeigh);

        hal_HstSendEvent(0xabcd1243);
    */



    g_PngDecUserHandler=handle;

    HAL_ERR_T re_value=vpp_PngDecOpen((HAL_VOC_IRQ_HANDLER_T )PngDecVocISR,VPP_PNGDEC_WAKEUP_SW0);

    if (re_value != HAL_ERR_RESOURCE_BUSY)
    {

        // ----------------
        // config
        // ----------------
        g_vppPngDecCfg.mode         =  0;
        g_vppPngDecCfg.reset        =  1;

        g_vppPngDecCfg.FileContent=FileContent;
        g_vppPngDecCfg.FileSize=FileSize;
        g_vppPngDecCfg.PNGOutBuffer=PNGOutBuffer;
        g_vppPngDecCfg.OutBufferSize=OutBufferSize;
        g_vppPngDecCfg.AlphaOutBuffer=AlphaOutBuffer;
        g_vppPngDecCfg.AlphaBufferSize=AlphaBufferSize;
        g_vppPngDecCfg.BkgrdBuffer=BkgrdBuffer;
        g_vppPngDecCfg.BkgrdClipX1=BkgrdClipX1;
        g_vppPngDecCfg.BkgrdClipX2=BkgrdClipX2;
        g_vppPngDecCfg.BkgrdClipY1=BkgrdClipY1;
        g_vppPngDecCfg.BkgrdClipY2=BkgrdClipY2;
        g_vppPngDecCfg.BkgrdOffsetX=BkgrdOffsetX;
        g_vppPngDecCfg.BkgrdOffsetY=BkgrdOffsetY;
        g_vppPngDecCfg.BkgrdWidth=BkgrdWidth;
        g_vppPngDecCfg.BkgrdHeight=BkgrdHeight;
        g_vppPngDecCfg.ZoomWidth=ZoomWidth;
        g_vppPngDecCfg.ZoomHeight=ZoomHeight;
        g_vppPngDecCfg.DecMode=DecMode;
        g_vppPngDecCfg.alpha_blending_to_transparentColor=alpha_blending_to_transparentColor;

        g_vppPngDecCfg.ImgWidth=ImgWidth;
        g_vppPngDecCfg.ImgHeigh=ImgHeigh;

        g_vppPngDecCfg.Window=WindowBuf;

        g_vppPngDecCfg.PngFileHandle=PngFileHandle;

        vpp_PngDecCfg(&g_vppPngDecCfg);

        vpp_PngDecSchedule();

        hal_GdbFlushCacheData();

        return HAL_ERR_NO;

    }

    return re_value;

}


//0 is decoding .1 is decoded
BOOL MCI_PngdecGetDecodingStatus(MCI_PNGDEC_STATUS_T *status_png)
{

    VPP_PNGDEC_STATUS_T  vpp_Status;

    vpp_PngDecStatus(&vpp_Status);


    status_png->mode=vpp_Status.mode;
    status_png->errorStatus=vpp_Status.errorStatus;
    status_png->PngdecFinished=vpp_Status.PngdecFinished;


    if(status_png->PngdecFinished==1)
    {
        vpp_PngDecClose();

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

#endif


