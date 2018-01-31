#include "string.h"
#include "stdio.h"
#include "cos.h"
#include "tts.h"
//#include "mdi_audio.h"
#include "fs.h"
//#include "mmi_trace.h"
//#include "oslmemory.h"
//#include "nvram_user_defs.h"
//#include "app_ltlcom.h"
#include "ttsplayer.h"
//#include "mdi_datatype.h"
//#include "mdi_audio.h"
#include "mci.h"
#include "ttsfile.h"

#include "dsm_cf.h"
#ifdef __TTS_FEATURES__
//#include "DebugInitDef.h"
typedef INT32 FS_HANDLE;
#define DEV_TASK_PRIORITY (COS_MMI_TASKS_PRIORITY_BASE + 1)
#define TTS_BUFF_SIZE 1024 * 64
//语音数据达到该大小时开始播放
#define VOICE_BUF_SIZE_TO_PLAY 1024 * 1

#define AUD_MODE_NORMAL 0  /* Normal Mode */
#define AUD_MODE_HEADSET 1 /* HeadSet (Earphone) Mode */
#define AUD_MODE_LOUDSPK 2 /* Loudspeaker Mode */
#define AUD_MODE_BLUETOOTH 3
#define AUD_VOLUME_DEFAULE 13
#define MAX_AUD_MODE_NUM 4
typedef unsigned char kal_uint8;
typedef signed char kal_int8;
typedef unsigned short int kal_uint16;
typedef unsigned int kal_uint32;
#define OslMalloc COS_MALLOC
#define OslMfree COS_FREE
#define DM_AUDIO_MODE_HANDSET 0
#define DM_AUDIO_MODE_EARPIECE 1
#define DM_AUDIO_MODE_LOUDSPEAKER 2
#define DM_AUDIO_MODE_BLUETOOTH 3
#define ST_SUCCESS 1
#define ST_FAILURE 0

#ifdef AT_TTS_MIX
#define jtCNP_Max_Len 804720
#else
#define jtCNP_Max_Len 1445584
#endif

#if defined(COMPRESS_USE_LZMA)

#ifdef AT_TTS_MIX
#define jtCNP_lzmaMax_Len 523662
#else
#define jtCNP_lzmaMax_Len 828388
#endif

//extern  unsigned char* CNPackage;
extern unsigned char CNPackage[jtCNP_Max_Len];
extern const unsigned char CNPackageLzma[jtCNP_lzmaMax_Len];
extern int decompress(unsigned char *buf, int in_len, unsigned char *output);

#else
extern const unsigned char CNPackage[jtCNP_Max_Len];
#endif
typedef struct
{
    //  kal_uint8 gttsRingKnownNumberCallCustom[(MAX_TTS_CODE + 1) * ENCODING_LENGTH];
    //  kal_uint8 gttsRingUnKnownNumberCallCustom[(MAX_TTS_CODE + 1) * ENCODING_LENGTH];
    //   kal_uint8 gttsRingKnownNumberMsgCustom[(MAX_TTS_CODE + 1) * ENCODING_LENGTH];
    //  kal_uint8 gttsRingUnKnownNumberMsgCustom[(MAX_TTS_CODE + 1) * ENCODING_LENGTH];
    /*这里是语音合成*/
    //  kal_uint8 curTTSAlertMissCallStatus;
    //  kal_uint8 curTTSAlertMissMsgStatus;

    //  kal_uint8 curTTSAlertToDoListStatus;

    kal_uint8 curTTSAlertAlarmAutoStatus;
    kal_uint8 curTTSAlertHourStatus;
    kal_uint8 curTTSAlertWeekStatus;
    kal_uint8 curTTSAlertDateStatus;
    /*语音播报*/
    kal_uint8 curTTSRingIncommingCallStatus;
    //  kal_uint8 curTTSRingKnownNumberCallStatus;
    //kal_uint8 curTTSRingUnKnownNumberCallStatus;

    //  kal_uint8 curTTSRingIncommingMsgStatus;
    //  kal_uint8 curTTSRingKnownNumberMsgStatus;
    //  kal_uint8 curTTSRingUnKnownNumberMsgStatus;

    /*菜单朗读*/
    kal_uint8 curTTSMenuReadStatus;

    /*短信朗读*/
    kal_uint8 curTTSMsgReadStatus;

    kal_uint8 curTTSVolumeStatus;
    kal_uint8 curTTSSpeedStatus;
    kal_uint8 curTTSEffectStatus;
    kal_uint8 curTTSStyleStatus;
    kal_uint8 curTTSTuneStatus;

    /*合成*/
    //  kal_uint8 curASRStatus;
    //  kal_uint8 curCallSim;
    //  kal_uint8 curAutoDibblerMusicStatus;

} nvram_tts_setting_struct;

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    MSG_ID_TTS_PLAY_ASYN = 88,
    MSG_ID_TTS_PLAY_EBOOK = 89,
    MSG_ID_TTS_END
};

// WAV文件头格式
typedef struct _WAVE_FILE_HEADER
{
    char chRIFF[4];
    DWORD dwRIFFLen;
    char chWAVE[4];
    char chFMT[4];
    DWORD dwFMTLen;
    WORD wFormatTag;       /* format type */
    WORD nChannels;        /* number of channels (i.e. mono, stereo, etc.) */
    DWORD nSamplesPerSec;  /* sample rate */
    DWORD nAvgBytesPerSec; /* for buffer estimation */
    WORD nBlockAlign;      /* block size of data */
    WORD wBitsPerSample;
    char chDATA[4];
    DWORD dwDATALen;
} WAVE_FILE_HEADER;

//#pragma pack (4)
typedef struct strParameter
{
    unsigned long hTTS;
    //mdi_handle* pHandle;
    FS_HANDLE fHandle;
    long offset;
} jtParameter;

typedef struct tagUserData
{
    FS_HANDLE pInputFIle;
    FS_HANDLE pOutputFIle;
    unsigned long hTTS;
} jtUserData;

#define LOCAL_PARA_HDR \
    U8 ref_count;      \
    U16 msg_len;
typedef struct _tts_param_struct
{
    LOCAL_PARA_HDR
    jtTTS_InputTextProc inputCall;
    jtTTS_ProgressProc progressCall;
    char *string; /* string */
    unsigned long dwUserData;
} tts_param_struct;

typedef struct _tts_Asyn_param_struct
{
    LOCAL_PARA_HDR
    char *string; /* string */
    JTTSCALLBACKPROCEX lpfnCallback;
    unsigned long dwUserData;
} tts_Asyn_param_struct;

jtTTSPlayerStatus g_tts_status_flag = TTS_PLAYER_NOT_INIT;
static BOOL g_pause_to_stop = FALSE;

static long g_buffer_len_written = 0;
static BOOL g_status_stable = FALSE;

U8 prevMode = AUD_MODE_NORMAL;

HANDLE g_hTTSPlayTask;
unsigned long hTTS = NULL;
unsigned char *pHeap;
char *gTtsData;
U8 bStopTTS;
int g_TTS_Mix;
int g_TTS_MixState;
//PCM播放接口回调
//U16 AnsiiToUnicodeString(S8 *pOutBuffer, S8 *pInBuffer );
#define OSM_MAlloc(obj) CSW_MALLOC(BASE_DSM_TS_ID, (obj))
const char UTF8_HEADER[] = {-17, -69, -65};
const char UNICODE_HEADER[] = {-1, -2};
const char UNICODE_BE_HEADER[] = {-2, -1};

mdi_callback g_finish_callback;
extern void MusicEndAudioMode(); //Added by Jinzh:20070616

U8 g_TTS_output_path_default = DM_AUDIO_MODE_EARPIECE; //DM_AUDIO_MODE_LOUDSPEAKER;

VOID TTSOutputPath()
{
    //hal_HstSendEvent(GetHandsetInPhone());
    if (pmd_GetEarModeStatus())
    {
        DM_SetAudioMode(DM_AUDIO_MODE_HANDSET);
        MCI_AudioSetOutputPath(MCI_PATH_HP, 0);
        prevMode = DM_AUDIO_MODE_HANDSET;
    }
    else
    {
        if (DM_AUDIO_MODE_LOUDSPEAKER == g_TTS_output_path_default)
        {
            DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
            MCI_AudioSetOutputPath(MCI_PATH_LSP, 0);
        }
        else
        {
            DM_SetAudioMode(DM_AUDIO_MODE_EARPIECE);
            MCI_AudioSetOutputPath(MCI_PATH_NORMAL, 0);
        }
    }
}

enum
{
    MDI_AUDIO_SUCCESS = 0,         /* 0 */
    MDI_AUDIO_FAIL,                /* 1 */
    MDI_AUDIO_BUSY,                /* 2 */
    MDI_AUDIO_DISC_FULL,           /* 3 */
    MDI_AUDIO_OPEN_FILE_FAIL,      /* 4 */
    MDI_AUDIO_END_OF_FILE,         /* 5 */
    MDI_AUDIO_TERMINATED,          /* 6 */
    MDI_AUDIO_BAD_FORMAT,          /* 7 */
    MDI_AUDIO_INVALID_FORMAT,      /* 8 */
    MDI_AUDIO_ERROR,               /* 9 */
    MDI_AUDIO_NO_DISC,             /* 10 */
    MDI_AUDIO_NO_SPACE,            /* 11 */
    MDI_AUDIO_INVALID_HANDLE,      /* 12 */
    MDI_AUDIO_NO_HANDLE,           /* 13 */
    MDI_AUDIO_RESUME,              /* 14 */
    MDI_AUDIO_BLOCKED,             /* 15 */
    MDI_AUDIO_MEM_INSUFFICIENT,    /* 16 */
    MDI_AUDIO_BUFFER_INSUFFICIENT, /* 17 */
    MDI_AUDIO_FILE_EXIST,          /* 18 */
    MDI_AUDIO_WRITE_PROTECTION,    /* 19 */
    MDI_AUDIO_PARAM_ERROR,         /* 20 */
    /* MP4,AAC */
    MDI_AUDIO_UNSUPPORTED_CHANNEL, /* 21 */
    MDI_AUDIO_UNSUPPORTED_FREQ,
    MDI_AUDIO_UNSUPPORTED_TYPE,
    MDI_AUDIO_UNSUPPORTED_OPERATION,
    MDI_AUDIO_PARSER_ERROR,
    MDI_AUDIO_VIDEO_ERROR,
    MDI_AUDIO_AUDIO_ERROR,
    MDI_AUDIO_FSAL_ERROR,
    MDI_AUDIO_MP4_PRODUCER_ERROR,
    MDI_AUDIO_MP4_SAMPLEDATA_ERROR,
    MDI_AUDIO_MP4_NO_VIDEO_TRACK,
    MDI_AUDIO_MP4_NO_AUDIO_TRACK,
    /* VR */
    MDI_AUDIO_ID_MISMATCH, /* 33 */
    MDI_AUDIO_ID_EXIST,
    MDI_AUDIO_TRAINING_CONTINUE,
    MDI_AUDIO_NO_SOUND,
    MDI_AUDIO_TOO_SIMILAR,
    MDI_AUDIO_TOO_DIFFERENT,
    MDI_AUDIO_NO_MATCH,
    MDI_AUDIO_SPEAK_TOO_SHORT,
    MDI_AUDIO_SPEAK_TOO_LONG,
    /* FM Record */
    MDI_AUDIO_STOP_FM_RECORD,

    MDI_NO_OF_EVENT
};

typedef enum {
    MIXVOISE_STATE_IDLE,
    MIXVOISE_STATE_MIXLOCAL,
    MIXVOISE_STATE_MIXREMOTE,
    MIXVOISE_STATE_MIXALL,
} MIXVOISE_STATE;

S32 UCS2Strlen(const S8 *arrOut)
{

    S32 nCount = 0;
    S32 nLength = 0;
    /*Check for NULL character only at the odd no. of bytes
      assuming forst byte start from zero */
    if (arrOut)
    {
        while (arrOut[nCount] != 0 || arrOut[nCount + 1] != 0)
        {
            ++nLength;
            nCount += 2;
        }
    }
    return nLength; /*One is added to count 0th byte*/
}

void pcm_play_callback(APBS_STREAM_STATUS_T status)
{

    APBS_STREAM_STATUS_T stream_status = (APBS_STREAM_STATUS_T)status;
    //mmi_trace(g_sw_DEBUG, " pcm_play_callback:;stream_status=%d, g_tts_status_flag= %d ",  stream_status, g_tts_status_flag);

    switch (stream_status)
    {
    case STREAM_STATUS_REQUEST_DATA:
        break;
    case STREAM_STATUS_NO_MORE_DATA:
    case STREAM_STATUS_ERR:
        jtTTS_SynthStop(hTTS);
        break;
    case STREAM_STATUS_END:
        if (g_tts_status_flag != TTS_PLAYER_NOT_INIT)
        {
            if (g_finish_callback != NULL)
            {
                g_finish_callback(MDI_AUDIO_END_OF_FILE);
                g_finish_callback = NULL;
            }

            g_tts_status_flag = TTS_PLAYER_IDLE;
        }
        // MusicEndAudioMode();
        prevMode = AUD_MODE_NORMAL;
        //mdi_start_background_timer();

        break;

    default:
        break;
    }
}

BOOL get_wav_format(unsigned long handle, kal_uint16 *sample_rate, kal_int8 *bit_rate)
{
    long wav_format = -1;
    jtErrCode nErr = jtTTS_ERR_NONE;

    nErr = jtTTS_GetParam(handle, jtTTS_PARAM_WAV_FORMAT, &wav_format);
    //mmi_trace(1, "tts test, get wav format the code is %d", nErr);
    //kal_prompt_trace(MOD_EJTTS, "jtTTS_PARAM_WAV_FORMAT = %d %d", wav_format, nErr);

    if (jtTTS_ERR_NONE != nErr || -1 == wav_format)
    {
        //mmi_trace(1, "tts test, get wav format error!");
        return FALSE;
    }

    switch (wav_format)
    {
    case jtTTS_FORMAT_PCM_NORMAL:
        //此处依音库而定，用户需要手动修改
        *bit_rate = 16;
        *sample_rate = 16000;
        break;
    case jtTTS_FORMAT_PCM_8K8B:
        *bit_rate = 8;
        *sample_rate = 8000;
        break;
    case jtTTS_FORMAT_PCM_8K16B:
        *bit_rate = 16;
        *sample_rate = 8000;
        break;
    case jtTTS_FORMAT_PCM_16K8B:
        *bit_rate = 8;
        *sample_rate = 16000;
        break;
    case jtTTS_FORMAT_PCM_16K16B:
        *bit_rate = 16;
        *sample_rate = 16000;
        break;
    case jtTTS_FORMAT_PCM_11K8B:
        *bit_rate = 8;
        *sample_rate = 11025;
        break;
    case jtTTS_FORMAT_PCM_11K16B:
        *bit_rate = 16;
        *sample_rate = 11025;
        break;
    case jtTTS_FORMAT_PCM_22K8B:
        *bit_rate = 8;
        *sample_rate = 22050;
        break;
    case jtTTS_FORMAT_PCM_22K16B:
        *bit_rate = 16;
        *sample_rate = 22050;
        break;
    case jtTTS_FORMAT_PCM_44K8B:
        *bit_rate = 8;
        *sample_rate = 44100;
        break;
    case jtTTS_FORMAT_PCM_44K16B:
        *bit_rate = 16;
        *sample_rate = 44100;
        break;
    default:
        *bit_rate = 16;
        *sample_rate = 16000;
        break;
    }

    return TRUE;
}

/*
void TTS_PlayTask(PVOID pData)
{
    //add the code to play
    const char * pFileName = "d:/test.wav";
    mdi_audio_play_file(pFileName, MDI_AUDIO_PLAY_ONCE, NULL, NULL);

    COS_DeleteTask(g_hTTSPlayTask);
}
//*/

jtErrCode jt_OutputVoiceProc(void *pParameter,
                             long iOutputFormat, void *pData, long iSize)
{
    INT32 write;

    jtParameter *parameter;

    if (pParameter == NULL)
    {
        //mmi_trace(1, "test tts the output proc user data uis not exist.");
        return jtTTS_ERR_NONE;
    }

    parameter = (jtParameter *)pParameter;

    //mmi_trace(1, "test tts the output proc tts handle is %x, ", parameter->hTTS);
    //mmi_trace(1, "test tts the output proc file handle is %x, ", parameter->fHandle);
    //mmi_trace(1, "test tts the output data size is :%d", iSize);

    if (iSize <= 0)
    {
        jtTTS_SynthStop(parameter->hTTS);

        return jtTTS_ERR_NONE;
    }

    FS_Seek(parameter->fHandle, 0, FS_FILE_END);
    write = FS_Write(parameter->fHandle, pData, iSize);

    return jtTTS_ERR_NONE;
}

jtErrCode jt_OutputVoicePCMProc(void *pParameter,
                                long iOutputFormat, void *pData, long iSize)
{
    INT32 result;
    jtParameter *parameter;

    parameter = (jtParameter *)pParameter;
    if (pParameter == NULL)
    {
        //mmi_trace(1, "test tts the output proc user data uis not exist.");
        return jtTTS_ERR_NONE;
    }

    //mmi_trace(1, "tjt_OutputVoicePCMProc::pData = %x  iSize= %d.", pData, iSize);
    if (iSize <= 0)
    {
        //mmi_trace(1, "tjt_OutputVoicePCMProc::iSize<0, Finished Data");
        MCI_DataFinished();
        jtTTS_SynthStop(hTTS);
        g_tts_status_flag = TTS_PLAYER_IDLE;
    }

    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_SYNTHESIZING:
        //第一次
        memcpy((char *)gTtsData + g_buffer_len_written, pData, iSize);
        g_buffer_len_written += iSize;

        //数据填充到指定大小再播放,防止播放声音卡
        if (VOICE_BUF_SIZE_TO_PLAY <= g_buffer_len_written)
        {
            MCI_SetBuffer((UINT32 *)gTtsData, TTS_BUFF_SIZE);

            MCI_AddedData(g_buffer_len_written);
#ifdef AT_TTS_MIX
            result = MCI_AUD_StreamPlayPCM(gTtsData, TTS_BUFF_SIZE, MCI_PLAY_MODE_STREAM_PCM, pcm_play_callback, 8000, 16);
#else
            result = MCI_AUD_StreamPlayPCM(gTtsData, TTS_BUFF_SIZE, MCI_PLAY_MODE_STREAM_PCM, pcm_play_callback, 16000, 16);
#endif

            if (0 != result)
            {
                //mmi_trace(1, "Play Failed = %d", result);
                jtTTS_SynthStop(hTTS);
                g_tts_status_flag = TTS_PLAYER_IDLE;

                //              mmi_tts_send_notify(TTS_PLAYER_NOTIFY_ERROR);
            }
            else
            {
                UINT8 volumelevel;

                if (!DM_GetSpeakerVolume(&volumelevel))
                {

                    DM_SetAudioVolume(AUD_VOLUME_DEFAULE);
                }
                TTSOutputPath();
                g_tts_status_flag = TTS_PLAYER_PLAYING;
            }
        }
        break;

    case TTS_PLAYER_PLAYING:
    {
        unsigned char *buf_pcm, *buf_src;
        UINT32 buf_len;
        UINT32 pcm_len;

        buf_src = (unsigned char *)pData;
        pcm_len = iSize;

        /* 获取写入空间 */
        MCI_GetWriteBuffer(&buf_pcm, (kal_uint32 *)&buf_len);

        //如果播放buffer后部不够整段合成buffer放入，先将部分放入buffer末尾，
        //待播放buffer前部空出后，剩下的放入前部
        while (buf_len < pcm_len)
        {
            if (buf_len > 0)
            {
                memcpy(buf_pcm, buf_src, buf_len);
                pcm_len -= buf_len;
                buf_src += buf_len;
                MCI_AddedData(buf_len);
            }

            COS_Sleep(1);

            MCI_GetWriteBuffer(&buf_pcm, &buf_len);
        }

        memcpy(buf_pcm, buf_src, pcm_len);

        MCI_AddedData(pcm_len);
    }
        g_buffer_len_written += iSize;

        break;

    case TTS_PLAYER_PAUSE:

        MCI_AudioPlayPause();
        g_pause_to_stop = FALSE;

        while (TTS_PLAYER_PAUSE == g_tts_status_flag)
        {
            if (g_pause_to_stop)
            {
                return jtTTS_ERR_NONE;
            }
            COS_Sleep(50);
        }
        MCI_AudioPlayResume();
        break;

    case TTS_PLAYER_IDLE:
        jtTTS_SynthStop(hTTS);
        //  mmi_tts_send_notify(TTS_PLAYER_NOTIFY_ERROR);
        break;

    default:
        jtTTS_SynthStop(hTTS);
        break;
    }

    return jtTTS_ERR_NONE;
}

void AT_TTS_MIX_Stop()
{
    vois_MixDataFinished();
    jtTTS_SynthStop(hTTS);
    g_tts_status_flag = TTS_PLAYER_IDLE;
}
void AT_TTS_VoisTypeSet()
{
    switch (g_TTS_MixState)
    {
    case 0:
        vois_SetMixType(MIXVOISE_STATE_MIXREMOTE);
        break;
    case 1:
        vois_SetMixType(MIXVOISE_STATE_MIXLOCAL);
        break;
    case 2:
        vois_SetMixType(MIXVOISE_STATE_MIXALL);
        break;
    default:
        vois_SetMixType(MIXVOISE_STATE_IDLE);
    }
}
jtErrCode jt_OutputVoiceMixSpeechProc(void *pParameter,
                                      long iOutputFormat, void *pData, long iSize)
{
    INT32 result;
    jtParameter *parameter;

    parameter = (jtParameter *)pParameter;
    if (pParameter == NULL)
    {
        //		mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("test tts the output proc user data uis not exist.",0x09100391));
        return jtTTS_ERR_NONE;
    }

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(2), TSTR("tjt_OutputVoicePCMProc::pData = %x  iSize= %d.",0x09100392), pData, iSize);
    if (iSize <= 0)
    {
        // 	mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("tjt_OutputVoicePCMProc::iSize<0, Finished Data",0x09100393));
        vois_MixDataFinished();
        jtTTS_SynthStop(hTTS);
        g_tts_status_flag = TTS_PLAYER_IDLE;
    }

    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_SYNTHESIZING:
        //第一次
        memcpy((char *)gTtsData + g_buffer_len_written, pData, iSize);
        g_buffer_len_written += iSize;

        //数据填充到指定大小再播放,防止播放声音卡
        if (VOICE_BUF_SIZE_TO_PLAY <= g_buffer_len_written)
        {
            vois_SetMixBuffer((UINT32 *)gTtsData, TTS_BUFF_SIZE, AT_TTS_MIX_Stop);

            vois_AddedMixData(g_buffer_len_written);

            AT_TTS_VoisTypeSet();
            g_tts_status_flag = TTS_PLAYER_PLAYING;
        }
        break;

    case TTS_PLAYER_PLAYING:
    {
        unsigned char *buf_pcm, *buf_src;
        UINT32 buf_len;
        UINT32 pcm_len;

        buf_src = (unsigned char *)pData;
        pcm_len = iSize;

        /* 获取写入空间 */
        vois_GetWriteMixBuffer(&buf_pcm, (kal_uint32 *)&buf_len);

        //如果播放buffer后部不够整段合成buffer放入，先将部分放入buffer末尾，
        //待播放buffer前部空出后，剩下的放入前部
        while (buf_len < pcm_len)
        {
            if (buf_len > 0)
            {
                memcpy(buf_pcm, buf_src, buf_len);
                pcm_len -= buf_len;
                buf_src += buf_len;
                vois_AddedMixData(buf_len);
            }

            COS_Sleep(5);

            vois_GetWriteMixBuffer(&buf_pcm, &buf_len);
        }

        memcpy(buf_pcm, buf_src, pcm_len);

        vois_AddedMixData(pcm_len);
    }
        g_buffer_len_written += iSize;

        break;

    case TTS_PLAYER_PAUSE:

        g_pause_to_stop = FALSE;
        vois_SetMixType(MIXVOISE_STATE_IDLE);

        while (TTS_PLAYER_PAUSE == g_tts_status_flag)
        {
            if (g_pause_to_stop)
            {
                return jtTTS_ERR_NONE;
            }
            COS_Sleep(50);
        }

        AT_TTS_VoisTypeSet();

        break;

    case TTS_PLAYER_IDLE:

        jtTTS_SynthStop(hTTS);
        //vois_SetMixType(MIXVOISE_STATE_IDLE);

        //	mmi_tts_send_notify(TTS_PLAYER_NOTIFY_ERROR);
        break;

    default:

        jtTTS_SynthStop(hTTS);
        //vois_SetMixType(MIXVOISE_STATE_IDLE);

        break;
    }

    return jtTTS_ERR_NONE;
}
jtErrCode jt_InputTextProc(void *pParameter, void *pText, long *pSize)
{
    jtUserData *pUserData = (jtUserData *)pParameter;

    UINT32 read;
    char *p = (char *)pText;
    //  int count = (128-nReadCount)> &pSize? &pSize:(128-nReadCount);
    //如果读取的数据长度为0，并将0返回给引擎，
    //引擎将认为没有文本需要合成了，对于JtTTS_Synthesize
    //方式，合成函数将会退出，如果是jtTTS_synthStart,
    //引擎将会不断地轮询该回调函数，以获得为本

    read = FS_Read(pUserData->pInputFIle, pText, *pSize);

    *pSize = read;

    return jtTTS_ERR_NONE;
}
jtErrCode jt_ProgressProc(void *pParameter, long iProcBegin, long iProcLen)
{
    mmi_trace(1, "jt_ProgressProc:: Begin=%d, cLen=%d", iProcBegin, iProcLen);
    return jtTTS_ERR_NONE;
}

#if defined(TTS_LOAD_CONST_MODE)

//extern const unsigned char CNPackage16k[];
//extern const unsigned char ENPackage[] ;
#endif

int ejTTS_Init()
{
    ERRCODE error = TTS_PLAYER_ERR_NONE;
    long nSize = 0;
    if (TTS_PLAYER_NOT_INIT != g_tts_status_flag)
    {
        return TTS_PLAYER_ERR_ALREADYINIT;
    }

    if (NULL != hTTS)
    {
        return TTS_PLAYER_ERR_ALREADYINIT;
    }

#ifdef TTS_LOAD_FILE_MODE

    error = jtTTS_GetExtBufSize((signed char *)L"D:/CNPackage.dat", NULL, NULL, &nSize);
    //mmi_trace(1, "tts test, the getbufsize error is: %d, and request heap size is: %d", error, nSize);
    if (error != jtTTS_ERR_NONE)
    {
        //mmi_trace(1, "tts test, get buf size error!");
        hTTS = 0;
        //      MMI_ASSERT(0);
        return;
    }

    pHeap = OslMalloc(nSize);
    if (NULL == pHeap)
    {
        return TTS_PLAYER_ERR_MEMORY;
    }

    memset(pHeap, 0, nSize);
    //mmi_trace(1, "tts test, the heap address is %x!", pHeap);

    error = jtTTS_Init((signed char *)L"D:/CNPackage.dat", NULL, NULL, &hTTS, pHeap);

#elif defined(TTS_LOAD_CONST_MODE)
#if defined(COMPRESS_USE_LZMA)
    error = decompress((unsigned char *)CNPackageLzma, (int)jtCNP_lzmaMax_Len, CNPackage);
#endif
    error = jtTTS_GetExtBufSize(CNPackage, NULL, NULL, &nSize);

    //error = jtTTS_GetExtBufSize(CNPackage, NULL, NULL, &nSize);
    //mmi_trace(1, "tts test, the getbufsize error is: %d, and request heap size is: %d", error, nSize);
    if (error != jtTTS_ERR_NONE)
    {
        //mmi_trace(1, "tts test, get buf size error!");
        hTTS = 0;
        ASSERT(0);
        return error;
    }
    pHeap = OslMalloc(nSize);
    if (NULL == pHeap)
    {
        return TTS_PLAYER_ERR_MEMORY;
    }
    memset(pHeap, 0, nSize);
    //mmi_trace(1, "tts test, the heap address is %x!", pHeap);

    error = jtTTS_Init(CNPackage, NULL, NULL, &hTTS, pHeap);

#else
#error "TTS_LOAD_MODE define error!!"
#endif
    //error = jtTTS_Init(CNPackage, NULL, NULL, &hTTS, pHeap);
    //mmi_trace(1, "tts test, the init error is: %d", error);

    if (error != jtTTS_ERR_NONE)
    {
        //mmi_trace(1, "tts test, init tts error!");
        if (pHeap != NULL)
        {
            OslMfree(pHeap);
            pHeap = NULL;
        }
        hTTS = 0;
        return error;
    }

    if (gTtsData == NULL)
    {
        gTtsData = OslMalloc(TTS_BUFF_SIZE);
    }

    if (NULL == gTtsData)
    {
        OslMfree(pHeap);
        pHeap = NULL;
        return TTS_PLAYER_ERR_MEMORY;
    }
    g_tts_status_flag = TTS_PLAYER_IDLE;
    return jtTTS_ERR_NONE;
}

ERRCODE ejTTSPlayer_End()
{
    ERRCODE err_code = TTS_PLAYER_ERR_NONE;

    //mmi_trace(1, "ejTTSPlayer_End_Stat = %d", g_tts_status_flag);

    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_PAUSE:
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_PLAYING:
        //      ejTTSPlayer_Stop(TTS_PLAYER_STOP_BLOCK);
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
    case TTS_PLAYER_IDLE:
        break;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    err_code = jtTTS_End(hTTS);

    if (TTS_PLAYER_ERR_NONE != err_code)
    {
        return err_code;
    }

    g_tts_status_flag = TTS_PLAYER_NOT_INIT;

    //  release_pcm_device();

    if (NULL != pHeap)
    {
        OslMfree(pHeap);
        pHeap = NULL;
    }

    if (NULL != gTtsData)
    {
        OslMfree(gTtsData);
        gTtsData = NULL;
    }

    hTTS = NULL;

    return TTS_PLAYER_ERR_NONE;
}

void ejTTS_PlayToFile(char *pText, size_t nSize)
{

    //long nSize = 0;

    //unsigned char *pHeap;

    jtErrCode error;

    //mdi_handle handle;

    jtParameter parameter;

    FS_HANDLE fHandle;
    WAVE_FILE_HEADER wfh;
    INT32 fSize;
    UINT32 write;

    nvram_tts_setting_struct ttsSetting;
    long volumes[] =
        {
            jtTTS_VOLUME_MIN,
            jtTTS_VOLUME_MIN >> 1,
            jtTTS_VOLUME_NORMAL,
            jtTTS_VOLUME_MAX >> 1,
            jtTTS_VOLUME_MAX

        };

    long speeds[] =
        {
            jtTTS_SPEED_MAX,
            jtTTS_SPEED_NORMAL,
            jtTTS_SPEED_MIN};

    long styles[] =
        {
            jtTTS_SPEAK_STYLE_CLEAR,
            jtTTS_SPEAK_STYLE_NORMAL,
            jtTTS_SPEAK_STYLE_PLAIN,
            jtTTS_SPEAK_STYLE_VIVID};

    long effects[] =
        {
            jtTTS_SOUNDEFFECT_BASE,
            jtTTS_SOUNDEFFECT_CHORUS,
            jtTTS_SOUNDEFFECT_ECHO,
            jtTTS_SOUNDEFFECT_NEARFAR,
            jtTTS_SOUNDEFFECT_REVERB,
            jtTTS_SOUNDEFFECT_ROBOT};

    long tones[] =
        {
            jtTTS_PITCH_MIN,
            jtTTS_PITCH_NORMAL,
            jtTTS_PITCH_MAX};

    //mmi_trace(1, "tts test, the task begin!");

    //如果TTS 没有初始化，则进行初始化。
    if (hTTS == 0)
    {
        ejTTS_Init();
    }

    //mmi_trace(1, "test tts the input tts handle is %x, ", parameter.hTTS);

    //wav 头文件
    fHandle = FS_Open((const UINT8 *)L"D:/test.wav", FS_O_CREAT, 0);

    //mmi_trace(1, "tts test, open the file the handle is:%x", fHandle);

    parameter.hTTS = hTTS;
    parameter.offset = 0;
    parameter.fHandle = fHandle;

    // WAV 文件写文件头
    memset(&wfh, 0, sizeof(wfh));
    //mmi_trace(1, "tts test, the wave header size is:%d", sizeof(WAVE_FILE_HEADER));
    get_wav_format(hTTS, (kal_uint16 *)&wfh.nSamplesPerSec, (kal_int8 *)&wfh.wBitsPerSample);

    //mmi_trace(1, "tts test, get the wave format nSamplesPerSec is:%d, the wBitsPerSample is: %d", wfh.nSamplesPerSec, wfh.wBitsPerSample);

    memcpy(wfh.chRIFF, "RIFF", 4);
    memcpy(wfh.chWAVE, "WAVE", 4);
    memcpy(wfh.chFMT, "fmt ", 4);
    memcpy(wfh.chDATA, "data", 4);
    wfh.dwFMTLen = 0x10;
    wfh.wFormatTag = 1; // WAVE_FORMAT_PCM
    wfh.nChannels = 1;  // mono
    wfh.nAvgBytesPerSec = wfh.wBitsPerSample * wfh.nSamplesPerSec / 8;
    wfh.nBlockAlign = (WORD)(wfh.wBitsPerSample / 8);

    FS_Seek(fHandle, 0, FS_FILE_BEGIN);
    write = FS_Write(fHandle, (void *)&wfh, (UINT32)sizeof(WAVE_FILE_HEADER));
    //mmi_trace(1, "tts test, write the wave header size is:%d", write);

    // 设置输入文本编码
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_UNICODE);
    //mmi_trace(1, "tts test, the set text code error is: %d", error);

    //设置为文本直接输入的方式
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUTTXT_MODE, jtTTS_INPUT_TEXT_DIRECT);
    //mmi_trace(1, "tts test, the set mode error is: %d", error);

    //error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUT_CALLBACK, (long)jt_InputTextProc);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_OUTPUT_CALLBACK, (long)jt_OutputVoiceProc);
    //mmi_trace(1, "tts test, the set output callback error is: %d", error);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CALLBACK_USERDATA, (long)(&parameter));

    //error = jtTTS_Synthesize(hTTS);
    //mmi_trace(1, "tts test, the set user data error is: %d", error);

    //获取tts的配置信息
    mmi_settings_tts_read_from_nvram(&ttsSetting);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_VOLUME, volumes[ttsSetting.curTTSVolumeStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEAK_STYLE, styles[ttsSetting.curTTSStyleStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEED, speeds[ttsSetting.curTTSSpeedStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SOUNDEFFECT, effects[ttsSetting.curTTSEffectStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_PITCH, tones[ttsSetting.curTTSTuneStatus]);

    //return;

    //mmi_trace(1, "tts test, the text size is %d", nSize);
    //mmi_trace_hex(1, nSize, pText);

    //初始化全局变量
    bStopTTS = 0;

    error = jtTTS_SynthesizeText(hTTS, pText, nSize);

    fSize = FS_GetFileSize(fHandle);
    wfh.dwDATALen = fSize - sizeof(WAVE_FILE_HEADER);
    wfh.dwRIFFLen = fSize - 8;
    FS_Seek(fHandle, 0, FS_FILE_BEGIN);
    write = FS_Write(fHandle, (void *)&wfh, (UINT32)sizeof(WAVE_FILE_HEADER));
    //mmi_trace(1, "tts test, in the end write the wave header size is:%d", write);
    FS_Close(fHandle);

    if (bStopTTS == 0)
    {
        //mdi_audio_suspend_background_play();
        //mdi_audio_play_file((const UINT8 *)L"D:/test.wav", MDI_AUDIO_PLAY_ONCE, NULL, NULL);
    }

    //if(pHeap != NULL)
    //{
    //  OslMfree(pHeap);
    //}

    //COS_DeleteTask(g_hTTSPlayTask);

    //mdi_audio_init(void)
    //mdi_audio_init_event_hdlrs(void)
    //mdi_audio_play_file(void * file_name, U8 play_style, mdi_handle * handle_p, mdi_callback handler)
}

UINT32 ejTTS_PlayToPCM_withCall(char *pText, size_t nSize, mdi_callback callback)
{
    g_finish_callback = callback;
    ejTTS_PlayToPCM(pText, nSize, 0);
}

void mmi_settings_tts_read_from_nvram(nvram_tts_setting_struct *pTtsInfo)
{
    //S16 error;
    //ReadRecord( NVRAM_SETTING_TTS_LID, 1, (void*)pTtsInfo, NVRAM_SETTING_TTS_SIZE, &error );
    /*
    if(error == NVRAM_READ_SUCCESS)
    {
    }
    else
    {
    */
    pTtsInfo->curTTSMsgReadStatus = 1;
    pTtsInfo->curTTSMenuReadStatus = 1;
    pTtsInfo->curTTSRingIncommingCallStatus = 1;
    pTtsInfo->curTTSVolumeStatus = AUD_VOLUME_DEFAULE;
    pTtsInfo->curTTSSpeedStatus = 1;
    pTtsInfo->curTTSEffectStatus = 1;
    pTtsInfo->curTTSStyleStatus = 0;
    pTtsInfo->curTTSTuneStatus = 1;
    pTtsInfo->curTTSAlertAlarmAutoStatus = 1;
    pTtsInfo->curTTSAlertDateStatus = 1;
    pTtsInfo->curTTSAlertWeekStatus = 1;
    //memset( pReadMsgInfo->magnitude, MAX_EQUALIZER_NUM*8, 0 );

    //mmi_settings_tts_write_to_nvram(pTtsInfo);
}

UINT32 ejTTS_PlayToPCMWithMix(char *pText, size_t nSize, BOOL UCS2)
{

    //long nSize = 0;

    //unsigned char *pHeap;

    jtErrCode error;
    INT32 result;

    //mdi_handle handle;

    jtParameter parameter;

    kal_uint16 sample_rate;
    kal_int8 bit_rate;

    nvram_tts_setting_struct ttsSetting;
    long volumes[] =
        {
            jtTTS_VOLUME_MIN,
            jtTTS_VOLUME_MIN >> 1,
            jtTTS_VOLUME_NORMAL,
            jtTTS_VOLUME_MAX >> 1,
            jtTTS_VOLUME_MAX

        };

    long speeds[] =
        {
            jtTTS_SPEED_MAX,
            jtTTS_SPEED_NORMAL,
            jtTTS_SPEED_MIN};

    long styles[] =
        {
            jtTTS_SPEAK_STYLE_CLEAR,
            jtTTS_SPEAK_STYLE_NORMAL,
            jtTTS_SPEAK_STYLE_PLAIN,
            jtTTS_SPEAK_STYLE_VIVID};

    long effects[] =
        {
            jtTTS_SOUNDEFFECT_BASE,
            jtTTS_SOUNDEFFECT_CHORUS,
            jtTTS_SOUNDEFFECT_ECHO,
            jtTTS_SOUNDEFFECT_NEARFAR,
            jtTTS_SOUNDEFFECT_REVERB,
            jtTTS_SOUNDEFFECT_ROBOT};

    long tones[] =
        {
            jtTTS_PITCH_MIN,
            jtTTS_PITCH_NORMAL,
            jtTTS_PITCH_MAX};

    if (g_tts_status_flag == TTS_PLAYER_NOT_INIT)
    {
        if (ejTTS_Init() != jtTTS_ERR_NONE)
            return jtTTS_ERR_NOT_INIT;
    }

    if (hTTS == 0)
    {
        return jtTTS_ERR_LICENCE;
    }

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(0), TSTR("tts test, the task begin!",0x091003ab));
    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_PAUSE:
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_PLAYING:
        return TTS_PLAYER_ERR_PLAYING;
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
    case TTS_PLAYER_IDLE:
        break;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    //如果TTS 没有初始化，则进行初始化。
    if (NULL == pText)
    {
        return TTS_PLAYER_ERR_PARAM;
    }
    else if (TTS_PLAYER_MAX_TEXTLEN - 2 < UCS2Strlen(pText) * sizeof(short))
    {
        return TTS_PLAYER_ERR_TOO_MORE_TEXT;
    }

    memset(gTtsData, 0, TTS_BUFF_SIZE);

    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("test tts the input tts handle is %x, ",0x091003ac), parameter.hTTS);

    parameter.hTTS = hTTS;
    parameter.offset = 0;
    //parameter.fHandle = fHandle;

    get_wav_format(hTTS, &sample_rate, &bit_rate);
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(2), TSTR("tts test, get the wave format nSamplesPerSec is:%d, the wBitsPerSample is: %d",0x091003ad), sample_rate, bit_rate);

    // 设置输入文本编码
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_UNICODE);
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("tts test, the set text code error is: %d",0x091003ae), error);

    //设置为文本直接输入的方式
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUTTXT_MODE, jtTTS_INPUT_TEXT_DIRECT);
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("tts test, the set mode error is: %d",0x091003af), error);

    //error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUT_CALLBACK, (long)jt_InputTextProc);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_OUTPUT_CALLBACK, (long)jt_OutputVoiceMixSpeechProc);
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("tts test, the set output callback error is: %d",0x091003b0), error);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CALLBACK_USERDATA, (long)(&parameter));

    //error = jtTTS_Synthesize(hTTS);
    //mmi_trace_b(_MMI|TLEVEL(1)|TDB|TNB_ARG(1), TSTR("tts test, the set user data error is: %d",0x091003b1), error);

    //获取tts的配置信息
    mmi_settings_tts_read_from_nvram(&ttsSetting);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_VOLUME, volumes[ttsSetting.curTTSVolumeStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEAK_STYLE, styles[ttsSetting.curTTSStyleStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEED, speeds[ttsSetting.curTTSSpeedStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SOUNDEFFECT, effects[ttsSetting.curTTSEffectStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_PITCH, tones[ttsSetting.curTTSTuneStatus]);

    //初始化全局变量
    g_buffer_len_written = 0;
    g_tts_status_flag = TTS_PLAYER_SYNTHESIZING;

    error = jtTTS_SynthesizeText(hTTS, pText, nSize);

    // 如果输出的数据总量小于VOICE_BUF_SIZE_TO_PLAY，在声音输出回调函数内不会启动播放
    // 此现象发生时，TTSPlayer状态仍为TTS_PLAYER_SYNTHESIZING，此时要在此启动播放
    if (TTS_PLAYER_SYNTHESIZING == g_tts_status_flag)
    {

        vois_SetMixBuffer((UINT32 *)gTtsData, TTS_BUFF_SIZE);
        vois_AddedMixData(g_buffer_len_written);
        AT_TTS_VoisTypeSet();
        // g_tts_status_flag = TTS_PLAYER_PLAYING;
        COS_Sleep(50);

        //vois_MixDataFinished();
        jtTTS_SynthStop(hTTS);
    }
    else
    {
        //		  g_tts_status_flag = TTS_PLAYER_IDLE;
    }
    return jtTTS_ERR_NONE;

    //		  ejTTSPlayer_End();
}
UINT32 ejTTS_PlayToPCM(char *pText, size_t nSize, BOOL UCS2)
{

    jtErrCode error;
    INT32 result;

    jtParameter parameter;

    kal_uint16 sample_rate;
    kal_int8 bit_rate;

    nvram_tts_setting_struct ttsSetting;
    long volumes[] =
        {
            jtTTS_VOLUME_MIN,
            jtTTS_VOLUME_MIN >> 1,
            jtTTS_VOLUME_NORMAL,
            jtTTS_VOLUME_MAX >> 1,
            jtTTS_VOLUME_MAX

        };

    long speeds[] =
        {
            jtTTS_SPEED_MAX,
            jtTTS_SPEED_NORMAL,
            jtTTS_SPEED_MIN};

    long styles[] =
        {
            jtTTS_SPEAK_STYLE_CLEAR,
            jtTTS_SPEAK_STYLE_NORMAL,
            jtTTS_SPEAK_STYLE_PLAIN,
            jtTTS_SPEAK_STYLE_VIVID};

    long effects[] =
        {
            jtTTS_SOUNDEFFECT_BASE,
            jtTTS_SOUNDEFFECT_CHORUS,
            jtTTS_SOUNDEFFECT_ECHO,
            jtTTS_SOUNDEFFECT_NEARFAR,
            jtTTS_SOUNDEFFECT_REVERB,
            jtTTS_SOUNDEFFECT_ROBOT};

    long tones[] =
        {
            jtTTS_PITCH_MIN,
            jtTTS_PITCH_NORMAL,
            jtTTS_PITCH_MAX};

    if (g_tts_status_flag == TTS_PLAYER_NOT_INIT)
    {
        if (ejTTS_Init() != jtTTS_ERR_NONE)
            return jtTTS_ERR_NOT_INIT;
    }

    if (hTTS == 0)
    {
        return jtTTS_ERR_LICENCE;
    }

    //mmi_trace(1, "tts test, the task begin!");
    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_PAUSE:
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_PLAYING:
        return TTS_PLAYER_ERR_PLAYING;
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
    case TTS_PLAYER_IDLE:
        break;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    //如果TTS 没有初始化，则进行初始化。
    if (NULL == pText)
    {
        return TTS_PLAYER_ERR_PARAM;
    }
    else if (TTS_PLAYER_MAX_TEXTLEN - 2 < UCS2Strlen(pText) * sizeof(short))
    {
        return TTS_PLAYER_ERR_TOO_MORE_TEXT;
    }

    memset(gTtsData, 0, TTS_BUFF_SIZE);

    //mmi_trace(1, "test tts the input tts handle is %x, ", parameter.hTTS);

    parameter.hTTS = hTTS;
    parameter.offset = 0;
    //parameter.fHandle = fHandle;

    get_wav_format(hTTS, &sample_rate, &bit_rate);
    //mmi_trace(1, "tts test, get the wave format nSamplesPerSec is:%d, the wBitsPerSample is: %d", sample_rate, bit_rate);

    // 设置输入文本编码

    if (UCS2)
        error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_UNICODE); //jtTTS_CODEPAGE_ASCII,jtTTS_CODEPAGE_UNICODE
    else
        error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_ASCII); //jtTTS_CODEPAGE_ASCII,jtTTS_CODEPAGE_UNICODE

    //设置为文本直接输入的方式
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUTTXT_MODE, jtTTS_INPUT_TEXT_DIRECT);

    //mmi_trace(1, "tts test, the set mode error is: %d", error);

    //error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUT_CALLBACK, (long)jt_InputTextProc);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_OUTPUT_CALLBACK, (long)jt_OutputVoicePCMProc);

    //mmi_trace(1, "tts test, the set output callback error is: %d", error);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CALLBACK_USERDATA, (long)(&parameter));

    //error = jtTTS_Synthesize(hTTS);
    //mmi_trace(1, "tts test, the set user data error is: %d", error);

    //获取tts的配置信息
    mmi_settings_tts_read_from_nvram(&ttsSetting);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_VOLUME, volumes[ttsSetting.curTTSVolumeStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEAK_STYLE, styles[ttsSetting.curTTSStyleStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEED, speeds[ttsSetting.curTTSSpeedStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SOUNDEFFECT, effects[ttsSetting.curTTSEffectStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_PITCH, tones[ttsSetting.curTTSTuneStatus]);

    //初始化全局变量
    g_buffer_len_written = 0;
    g_tts_status_flag = TTS_PLAYER_SYNTHESIZING;

    MCI_AudioStopBuffer();

    error = jtTTS_SynthesizeText(hTTS, pText, nSize);

    // 如果输出的数据总量小于VOICE_BUF_SIZE_TO_PLAY，在声音输出回调函数内不会启动播放
    // 此现象发生时，TTSPlayer状态仍为TTS_PLAYER_SYNTHESIZING，此时要在此启动播放
    if (TTS_PLAYER_SYNTHESIZING == g_tts_status_flag)

    {
        MCI_SetBuffer((UINT32 *)gTtsData, TTS_BUFF_SIZE);
        MCI_AddedData(g_buffer_len_written);

#ifdef AT_TTS_MIX
        result = MCI_AUD_StreamPlayPCM(gTtsData, TTS_BUFF_SIZE, MCI_PLAY_MODE_STREAM_PCM, pcm_play_callback, 8000, 16);
#else
        result = MCI_AUD_StreamPlayPCM(gTtsData, TTS_BUFF_SIZE, MCI_PLAY_MODE_STREAM_PCM, pcm_play_callback, 16000, 16);
#endif

        if (0 != result)
        {

            jtTTS_SynthStop(hTTS);
            g_tts_status_flag = TTS_PLAYER_IDLE;
        }
        else
        {
            UINT8 volumelevel;

            if (!DM_GetSpeakerVolume(&volumelevel))
            {

                DM_SetAudioVolume(AUD_VOLUME_DEFAULE);
            }

            //DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
            //MCI_AudioSetOutputPath(MCI_PATH_LSP, 0);
            //pmd_EnablePower(4, 1);
            //prevMode=AUD_MODE_LOUDSPK;
            TTSOutputPath();
            g_tts_status_flag = TTS_PLAYER_PLAYING;
        }
        MCI_DataFinished();
        jtTTS_SynthStop(hTTS);
    }
    else
    {
        g_tts_status_flag = TTS_PLAYER_IDLE;
    }

    return jtTTS_ERR_NONE;

    //        ejTTSPlayer_End();
}

void ejTTS_Play_Synthesize_main()
{
    // ejTTS_Play_Synthesize(jt_InputTextProc, jt_ProgressProc);
    mmi_tts_play_Ebook(jt_InputTextProc, jt_ProgressProc);
}

void ejTTS_Play_Synthesize(jtTTS_InputTextProc inputCall, jtTTS_ProgressProc progressCall)
{

    jtErrCode error;
    INT32 result;

    jtUserData parameter;

    kal_uint16 sample_rate;
    kal_int8 bit_rate;

    FS_HANDLE fpInput;
    char strInput[] = "d:/cuiyongyuan.txt";
    S8 uniinput[256];

    nvram_tts_setting_struct ttsSetting;
    long volumes[] =
        {
            jtTTS_VOLUME_MIN,
            jtTTS_VOLUME_MIN >> 1,
            jtTTS_VOLUME_NORMAL,
            jtTTS_VOLUME_MAX >> 1,
            jtTTS_VOLUME_MAX

        };

    long speeds[] =
        {
            jtTTS_SPEED_MAX,
            jtTTS_SPEED_NORMAL,
            jtTTS_SPEED_MIN};

    long styles[] =
        {
            jtTTS_SPEAK_STYLE_CLEAR,
            jtTTS_SPEAK_STYLE_NORMAL,
            jtTTS_SPEAK_STYLE_PLAIN,
            jtTTS_SPEAK_STYLE_VIVID};

    long effects[] =
        {
            jtTTS_SOUNDEFFECT_BASE,
            jtTTS_SOUNDEFFECT_CHORUS,
            jtTTS_SOUNDEFFECT_ECHO,
            jtTTS_SOUNDEFFECT_NEARFAR,
            jtTTS_SOUNDEFFECT_REVERB,
            jtTTS_SOUNDEFFECT_ROBOT};

    long tones[] =
        {
            jtTTS_PITCH_MIN,
            jtTTS_PITCH_NORMAL,
            jtTTS_PITCH_MAX};

    //mmi_trace(1, "tts test, the task begin!");
    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_PAUSE:
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_PLAYING:
        return TTS_PLAYER_ERR_PLAYING;
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
    case TTS_PLAYER_IDLE:
        break;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    if (hTTS == 0)
    {
        return jtTTS_ERR_LICENCE;
    }

    AnsiiToUnicodeString(uniinput, strInput);
    fpInput = FS_Open(uniinput, FS_O_RDONLY, 0);

    //mmi_trace(1, "ejTTS_Play_Synthesize:fpInput= %d ", fpInput);

    if (fpInput < 0)
        return jtTTS_ERR_NO_INPUT;

    parameter.hTTS = hTTS;
    parameter.pInputFIle = fpInput;

    memset(gTtsData, 0, TTS_BUFF_SIZE);

    //parameter.fHandle = fHandle;

    get_wav_format(hTTS, &sample_rate, &bit_rate);
    //mmi_trace(1, "tts test, get the wave format nSamplesPerSec is:%d, the wBitsPerSample is: %d", sample_rate, bit_rate);

    // 设置输入文本编码
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_GBK);
    //mmi_trace(1, "tts test, the set text code error is: %d", error);

    //设置为文本直接输入的方式
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUTTXT_MODE, jtTTS_INPUT_TEXT_CALLBACK);
    //mmi_trace(1, "tts test, the set mode error is: %d", error);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_INPUT_CALLBACK, (long)inputCall);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_OUTPUT_CALLBACK, (long)jt_OutputVoicePCMProc);
    //mmi_trace(1, "tts test, the set output callback error is: %d", error);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_PROGRESS_CALLBACK, (long)progressCall);
    //mmi_trace(1, "tts test, the set progress callback error is: %d", error);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_CALLBACK_USERDATA, (long)(&parameter));

    //error = jtTTS_Synthesize(hTTS);
    //mmi_trace(1, "tts test, the set user data error is: %d", error);

    //获取tts的配置信息
    mmi_settings_tts_read_from_nvram(&ttsSetting);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_VOLUME, volumes[ttsSetting.curTTSVolumeStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEAK_STYLE, styles[ttsSetting.curTTSStyleStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SPEED, speeds[ttsSetting.curTTSSpeedStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_SOUNDEFFECT, effects[ttsSetting.curTTSEffectStatus]);

    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_PITCH, tones[ttsSetting.curTTSTuneStatus]);

#if 0
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_BACKAUDIO_PATH, L"d:/16khz_mono.wav");

    mmi_trace(1, "tts test, the set backaudio path error is: %d", error);
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_BACKAUDIO_VOLUME, jtTTS_VOLUME_MIN);
    error = jtTTS_SetParam(hTTS, jtTTS_PARAM_BACKAUDIO_REPEAT, jtTTS_BACKAUDIO_REPEAT);
    {
        FS_HANDLE FileHandle;
        INT32 Offset;
        char strInputwav[] = "d:/16khz_mono.wav";
        char filename[256];
        AnsiiToUnicodeString(filename,strInputwav );
        FileHandle = MMI_FS_Open(filename,FS_READ_ONLY);
        Offset = MMI_FS_Seek(FileHandle, 0, FS_FILE_END);
        MMI_FS_Close( FileHandle);
        mmi_trace(1, "tts test, the set backaudio Offset is: %d", Offset);

        error = jtTTS_SetParam(hTTS, jtTTS_PARAM_BACKAUDIO_FILESIZE, Offset);
    }
#endif

    //初始化全局变量
    g_buffer_len_written = 0;
    g_tts_status_flag = TTS_PLAYER_SYNTHESIZING;
    MCI_AudioStopBuffer();

    error = jtTTS_Synthesize(hTTS);
    FS_Close(fpInput);

    // 如果输出的数据总量小于VOICE_BUF_SIZE_TO_PLAY，在声音输出回调函数内不会启动播放
    // 此现象发生时，TTSPlayer状态仍为TTS_PLAYER_SYNTHESIZING，此时要在此启动播放
    if (TTS_PLAYER_SYNTHESIZING == g_tts_status_flag)
    {
        MCI_SetBuffer((UINT32 *)gTtsData, TTS_BUFF_SIZE);
        MCI_AddedData(g_buffer_len_written);
#ifdef AT_TTS_MIX
        result = MCI_AUD_StreamPlayPCM(gTtsData, TTS_BUFF_SIZE, MCI_PLAY_MODE_STREAM_PCM, pcm_play_callback, 8000, 16);
#else
        result = MCI_AUD_StreamPlayPCM(gTtsData, TTS_BUFF_SIZE, MCI_PLAY_MODE_STREAM_PCM, pcm_play_callback, 16000, 16);
#endif
        if (0 != result)
        {
            //mmi_trace(1, "Play Failed = %d", result);
            jtTTS_SynthStop(hTTS);
            g_tts_status_flag = TTS_PLAYER_IDLE;
        }
        else
        {
            UINT8 volumelevel;

            if (!DM_GetSpeakerVolume(&volumelevel))
            {

                DM_SetAudioVolume(AUD_VOLUME_DEFAULE);
            }
            //DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
            //prevMode=AUD_MODE_LOUDSPK;
            TTSOutputPath();
            g_tts_status_flag = TTS_PLAYER_PLAYING;
        }
        MCI_DataFinished();
        jtTTS_SynthStop(hTTS);
    }
    else
    {
        g_tts_status_flag = TTS_PLAYER_IDLE;
    }
}
#if 0
VOID AT_TTS_AsyncEventProcess(COS_EVENT *pEvent)
    {
        CFW_EVENT ev;
        CFW_EVENT *pEv = &ev;
        UINT8 OutStr[50];
        memset(OutStr, 0, 50);
        AT_CosEvent2CfwEvent(pEvent, pEv);
        UINT8 nSim = pEv->nFlag;
        //AT_TC(g_sw_SPEC, "AT_TTS_AsyncEventProcess :%d ", pEvent->nParam2);
        switch (pEvent->nEventId)
        {
            case EV_CFW_TTS_PLAY:

                //ejTTS_PlayToPCMWithMix(pEvent->nParam1,pEvent->nParam2,pEvent->nParam3);
                break;
                /*
                   case EV_CFW_TTS_STOP:
                   ejTTSPlayerStop(pEvent->nParam2);
                   break;
                   case EV_CFW_TTS_PAUSE:
                   ejTTSPlayer_Pause(pEvent->nParam2);
                   break;
                   case EV_CFW_TTS_RESUME:
                   ejTTSPlayer_Resume(pEvent->nParam2);
                   break;
                   */
            default :
                break;
        }
    }
#endif

UINT32 AT_TTS_Play(char *pText, size_t nSize, BOOL UCS2)
{
    // AT_TC(g_sw_SPEC, "func :%s state=%d", __FUNCTION__,state);
    do
    {
        COS_EVENT ev;
        // AT_TC(g_sw_SPEC, " send event continue");
        ev.nEventId = EV_CFW_TTS_PLAY;
        ev.nParam1 = (UINT32)pText;
        ev.nParam2 = (UINT32)nSize;
        ev.nParam3 = (UINT32)UCS2;
        COS_SendEvent(g_hTTSTask, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    } while (0);

    return ERR_SUCCESS;
}

/*==================================================*/
BOOL bInMsgBox;
U16 tempMsgLen;

void ejTTSPlayer_Task_Main(void *pData)
{
    COS_EVENT ev;
    memset(&ev, 0, sizeof(ev));
    //ilm_struct *ilm_ptr;
    tts_param_struct *pContent = NULL;
    tts_Asyn_param_struct *pAsynContent = NULL;
    /* 创建Media Task的消息循环 */

    while (1)
    {
        //MMI_FreeEvent(&ev);
        COS_WaitEvent(g_hTTSTask, &ev, COS_WAIT_FOREVER);

        switch (ev.nEventId)
        {
        case EV_CFW_TTS_PLAY:
            if (g_TTS_Mix)
                ejTTS_PlayToPCMWithMix(ev.nParam1, ev.nParam2, ev.nParam3);
            else
                ejTTS_PlayToPCM(ev.nParam1, ev.nParam2, ev.nParam3);
            break;
        case MSG_ID_TTS_PLAY_ASYN:
        {
            pAsynContent = (tts_Asyn_param_struct *)ev.nParam1;
            //pAsynContent = (tts_Asyn_param_struct*)(ilm_ptr->local_para_ptr);

            if (bInMsgBox == TRUE)
            {
                ejTTS_PlayToPCM(pAsynContent->string, tempMsgLen, 0);
                bInMsgBox = FALSE;
                tempMsgLen = 0;
            }
            else
                ejTTS_PlayToPCM(pAsynContent->string, UCS2Strlen(pAsynContent->string) * sizeof(short), 0);
        }

        case MSG_ID_TTS_PLAY_EBOOK:
        {
            //ilm_ptr  = (ilm_struct *)ev.nParam1;
            pContent = (tts_param_struct *)ev.nParam1;
            ejTTS_Play_Synthesize(pContent->inputCall, pContent->progressCall);
        }
        break;
        default:
            break;
        }
    }
}
void mmi_tts_play_Async(char *string, JTTSCALLBACKPROCEX lpfnCallback, unsigned long dwUserData)
{
    HANDLE hTask;
    //ilm_struct *ilm_ptr;
    tts_Asyn_param_struct *msg_param = (tts_Asyn_param_struct *)COS_MALLOC(sizeof(tts_Asyn_param_struct));
    COS_EVENT event = {0};

    msg_param->string = string;
    msg_param->lpfnCallback = lpfnCallback;
    msg_param->dwUserData = dwUserData;
    event.nEventId = MSG_ID_TTS_PLAY_ASYN;
    event.nParam1 = (UINT32)(msg_param);
    event.nParam2 = 0;
    event.nParam3 = 0;
    /*noted by makai
    ilm_ptr = allocate_ilm(MOD_MMI);
    ilm_ptr->src_mod_id = MOD_MMI;
    ilm_ptr->dest_mod_id = MOD_TTS;
    ilm_ptr->msg_id = MSG_ID_TTS_PLAY_ASYN;
    ilm_ptr->local_para_ptr = (local_para_struct *)msg_param;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct *)NULL;
    */

    // msg_send_ext_queue(ilm_ptr);
    COS_SendEvent(g_hTTSTask, &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

void mmi_tts_play_Ebook(jtTTS_InputTextProc inputCall, jtTTS_ProgressProc progressCall)
{
    HANDLE hTask;
    // ilm_struct *ilm_ptr;
    tts_param_struct *msg_param = (tts_param_struct *)COS_MALLOC(sizeof(tts_param_struct));
    COS_EVENT event = {0};

    msg_param->string = NULL;
    msg_param->inputCall = inputCall;
    msg_param->progressCall = progressCall;
    /* noted by makai
    ilm_ptr = allocate_ilm(MOD_MMI);
    ilm_ptr->src_mod_id = MOD_MMI;
    ilm_ptr->dest_mod_id = MOD_TTS;
    ilm_ptr->msg_id = MSG_ID_TTS_PLAY_EBOOK;
    ilm_ptr->local_para_ptr = (local_para_struct *)msg_param;
    ilm_ptr->peer_buff_ptr = (peer_buff_struct *)NULL;
     */
    event.nEventId = MSG_ID_TTS_PLAY_EBOOK;
    event.nParam1 = (UINT32)(msg_param);
    event.nParam2 = 0;
    event.nParam3 = 0;
    COS_SendEvent(g_hTTSTask, &event, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
}

ERRCODE mmi_tts_stop_Async()
{

    ERRCODE err_code = jtTTS_ERR_NONE;

    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_PLAYING:
    case TTS_PLAYER_PAUSE:
        err_code = jtTTS_SynthStop(hTTS);
        if (jtTTS_ERR_NONE != err_code)
        {
            return err_code;
        }
        while (TTS_PLAYER_SYNTHESIZING == g_tts_status_flag ||
               TTS_PLAYER_PLAYING == g_tts_status_flag ||
               TTS_PLAYER_PAUSE == g_tts_status_flag)
        {
            COS_Sleep(5);
        }
        break;
    case TTS_PLAYER_IDLE:
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
        break;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    MCI_AudioStopBuffer();
    //mdi_start_background_timer();
}

ERRCODE ejTTSPlayer_Pause(void)
{
    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_PLAYING:
        g_tts_status_flag = TTS_PLAYER_PAUSE;
        break;
    case TTS_PLAYER_IDLE:
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_PAUSE:
        return TTS_PLAYER_ERR_DONOTHING;
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
        break;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    return TTS_PLAYER_ERR_NONE;
}

ERRCODE ejTTSPlayer_Resume(void)
{
    //mmi_trace(1,"entry ejTTSPlayer_Resume");

    switch (g_tts_status_flag)
    {
    case TTS_PLAYER_PAUSE:
        g_tts_status_flag = TTS_PLAYER_PLAYING;
        break;
    case TTS_PLAYER_PLAYING:
    case TTS_PLAYER_SYNTHESIZING:
    case TTS_PLAYER_IDLE:
        return TTS_PLAYER_ERR_DONOTHING;
        break;
    case TTS_PLAYER_NOT_INIT:
        return TTS_PLAYER_ERR_NOTINIT;
    default:
        return TTS_PLAYER_ERR_PARAM;
    }

    return TTS_PLAYER_ERR_NONE;
}
BOOL IsTTSRun()
{
    if ((g_tts_status_flag == TTS_PLAYER_NOT_INIT) || (g_tts_status_flag == TTS_PLAYER_IDLE))
        return FALSE;
    else
        return TRUE;
}

BOOL IsTTSInit()
{
    if ((g_tts_status_flag == TTS_PLAYER_NOT_INIT))
        return FALSE;
    else
        return TRUE;
}
#define DM_AUDIO_MODE_HANDSET 0
#define DM_AUDIO_MODE_EARPIECE 1
#define DM_AUDIO_MODE_LOUDSPEAKER 2
#define DM_AUDIO_MODE_BLUETOOTH 3

void MusicEndAudioMode() //Added by Jinzh:20070616
{

    if (1 == GetHandsetInPhone()) //Added by jinzh:20070710
    {
        DM_SetAudioMode(DM_AUDIO_MODE_HANDSET);
#ifdef MMI_ON_HARDWARE_P
        MCI_AudioSetOutputPath(MCI_PATH_HP, 0);
#endif
    }
    else
    {
        DM_SetAudioMode(DM_AUDIO_MODE_EARPIECE);
#ifdef MMI_ON_HARDWARE_P
        MCI_AudioSetOutputPath(MCI_PATH_NORMAL, 0);
#endif
    }
}

int ejTTSPlayerStop(void)
{
    bStopTTS = 1;
    MCI_AudioStopBuffer();
    MusicEndAudioMode();
    //mdi_audio_stop_file();
    //  mdi_audio_stop_string();
    //  mdi_audio_resume_background_play();
}

#ifdef __cplusplus
}
#endif
#endif
