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
#include "cos.h"

#include "hal_timers.h"
#include "hal_aif.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_speech.h"
#include "hal_overlay.h"

#include "vois_m.h"
#include "vois_map.h"
#include "voisp_map.h"
#include "voisp_cfg.h"
#include "vois_profile_codes.h"

#include "aud_m.h"
#include "vpp_speech.h"
#include "calib_m.h"

#include "hal_map_engine.h"

#include "sxr_sbx.h"
#include "sxr_tls.h"

//#define SPEECH_NOISE_SUPPRESSER_MIC
//#define SPEECH_NOISE_SUPPRESSER_SPEAKER

//#define VOIS_DUMP_PCM

#ifdef VOIS_DUMP_PCM
#include "base_prv.h"
#include "fs.h"
#endif
#include "dtmf_decode.h"

//#define JT_MAGIC_SOUND_SUPPORT

//#define XF_MAGIC_SOUND_SUPPORT

extern INT32 vois_set_ae( INT32 nAeID,  INT32 iValue);



#ifdef XF_MAGIC_SOUND_SUPPORT

#include "ivMagicVoice.h"

#endif



#ifdef JT_MAGIC_SOUND_SUPPORT
#include "jtae.h"
//#include "mdi_audio.h"
#endif

#ifdef MAGIC_AE_SUPPORT
#include "morphvoice.h"
#endif
#include "dtmfgen.h"

#ifdef SPEECH_NOISE_SUPPRESSER_MIC

extern UINT32 pcmbuf_overlay[4608];
#include "speex/speex_preprocess.h"
SpeexPreprocessState *mic_den_p;
SpeexPreprocessState *speaker_den_p;

//unsigned char *  p_alloc=pcmbuf_overlay;

#endif


/// TODO Does this part of the doc belong to the API ?
/// Workflow description
///   Init:
///     Init VPP, and load the first encoded sample (Rx) in its decoding buffer,
///     and get the first (invalid) encoded sample (Tx) from the encoding buffer
///     thanks to the first interrupt that is generated after the opening of VPP
///     is complete and before any data is processed.
///     Mute mode is set.
///     Lauch play on the output decoded buffer of VPP.
///
///   Data flow:
///     Data are copied on VPP interrupts, meaning the previous set has been
///     decoded/encoded. Decoding and encoding are triggered by the IFC events
///     directly telling VPP what to do.
///     On IRQ from VPP, we copy the next buffer to be decoded and the freshly
///     encoded one to put in the Speech Fifo.



//#define SPEEH_RECORD_PCM_ENABLE


//#define SPEEH_RECORD_DEC_DATA_ENABLE


#ifdef SPEEH_RECORD_PCM_ENABLE
#include "cos.h"
#include "fs.h"

#endif



#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
#include "cos.h"
#include "fs.h"

#define TEST_BUFFER_LEN (500*1024)

#endif



// =============================================================================
//  MACROS
// =============================================================================

/// FIXME USE the definitive notation from
#define VPP_RX_BUFFER_SIZE  (sizeof(HAL_SPEECH_DEC_IN_T))
#define VPP_TX_BUFFER_SIZE  (sizeof(HAL_SPEECH_ENC_OUT_T))


/// Number of trames before unmuting AUD
#define VOIS_AUD_UNMUTE_TRAME_NB 2

/// Number of trames before unmuting VPP
#define VOIS_VPP_UNMUTE_TRAME_NB 2

#define VOC_INTR_STEP


int NoiseSuppressDefault = -30;
int MinRange[4] = {30, 30, 30, 30};
int NonClipThreshold[2] = {1000, 700};

CALIB_AUDIO_VOC_SPEEX_T speex_config =
{
    .rx =
    {
        .flag = 0,
        .noise_suppress_default = -30,
        .noise_probe_range = {30, 30, 30, 30},
    },
    .tx =
    {
        .flag = 0,
        .noise_suppress_default = -30,
        .noise_probe_range = {30, 30, 30, 30},
    },
};

CALIB_AUDIO_VOC_AGC_T agc_config =
{
    .rx =
    {
        .flag = 0,
        .smooth_threshold = 700,
        .noise_threshold = 1000,
        .gain = 0x40,
    },
    .tx =
    {
        .flag = 0,
        .smooth_threshold = 700,
        .noise_threshold = 1000,
        .gain = 0x40,
    },
};

PROTECTED BOOL g_msdfReload = 0;

// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================



// =============================================================================
// g_voisCtx
// -----------------------------------------------------------------------------
/// VoiS context, holding global variables, to be watched through CoolWatcher.
// =============================================================================
PROTECTED VOIS_CONTEXT_T g_voisCtx =
{
    .voisStarted = FALSE,
    .voisEncOutput = NULL,
    .voisDecInput = NULL,
    .voisTramePlayedNb = 0,
    .voisAudioCfg = {0,0,0,0},
    .voisVppCfg = {0,0,0,0,0,0,0,0,NULL, NULL,0},
    .voisItf = AUD_ITF_NONE,
    .voisLatestCfg =
    {
        .spkSel     = AUD_SPK_RECEIVER,
        .micSel     = AUD_MIC_RECEIVER,
        .spkLevel   = AUD_SPK_MUTE,
        .micLevel   = AUD_MIC_MUTE,
        .sideLevel  = AUD_SIDE_MUTE,
        .toneLevel  = AUD_TONE_0dB,
        .encMute    = VPP_SPEECH_MUTE,
        .decMute    = VPP_SPEECH_MUTE
    },
    .mode = HAL_AMR475_ENC & HAL_AMR475_DEC,
    .bufferSize = HAL_SPEECH_FRAME_SIZE_AMR475
};

/// Stream to encode.
PRIVATE HAL_AIF_STREAM_T g_voisPcmTxStream;

/// Decoded stream.
PRIVATE HAL_AIF_STREAM_T g_voisPcmRxStream;

/// User handler to call with play status is stored here.
PRIVATE VOIS_USER_HANDLER_T g_voisUserHandler= NULL;

/// SRAM overlay usage flag
PRIVATE BOOL g_voisOverlayLoaded = FALSE;

/// Record flag
PRIVATE VOLATILE BOOL g_voisRecordFlag = FALSE;

/// Start address of the stream buffer
PRIVATE UINT32* g_voisBufferStart = NULL;

/// End address of the stream buffer
PRIVATE UINT32 g_voisBufferLength= 0;

/// End address of the stream buffer
PRIVATE UINT32* g_voisBufferTop = NULL;

/// Current position in the buffer. Next read data will be
/// read from here.
PRIVATE UINT32* g_voisBufferCurPos= NULL;

/// PCM buffer address
PRIVATE INT16* g_MicPcmBuffer = NULL;

/// PCM buffer address
PRIVATE INT16 *g_SpeakerPcmBuffer=NULL;

// delay buffer 400ms(20 frames)
#define kLoopTestDelayNum 20
PRIVATE HAL_SPEECH_PCM_HALF_BUF_T *g_LoopTestDelayBuf = NULL;
PRIVATE BOOL g_VoisLoopTestFlag = FALSE;
PRIVATE AUD_TEST_T g_voisRunningTest        = AUD_TEST_NO;

INT32 time_handle;

PRIVATE BOOL g_VoisRxToTXLoopFlag = FALSE;


/*
VOID Vois_test(VOIS_STATUS_T test)
{

   VOIS_TRACE(TSTDOUT, 0, "HANDLE:%d",test);


    return;
 }


*/



// =============================================================================
// sample
// -----------------------------------------------------------------------------
/// A dirty test variable!
// =============================================================================
#ifdef VOIS_LOOP

UINT32 sample []=
{
#include "amr_ring_122_10sec.dat"
};

volatile BOOL g_voisUseSample = FALSE;
volatile UINT32 g_voisIndex = 0;
#endif // VOIS_LOOP

// =============================================================================
// sample
// -----------------------------------------------------------------------------
/// A dirty PCM dump variable!
// =============================================================================
#ifdef VOIS_DUMP_PCM

#define NB_DUMP_PCM_FRAMES 300
#define SECONDS        * HAL_TICK1S
#define FS_DUMP_TIMER_CNT (1 SECONDS)

// to avoid the buffer corruption during the encode the dump for Tx is done in a external buffer
UINT32 g_voisPcmTxBuffer[HAL_SPEECH_FRAME_SIZE_PCM_BUF];
PRIVATE UINT32 *g_voisSpkPcmBuffer = NULL;
PRIVATE UINT32 *g_voisMicPcmBuffer = NULL;
PRIVATE UINT32 *g_voisResPcmBuffer = NULL;
PRIVATE UINT32 g_voisPcmCount = 0;
PRIVATE INT32 g_voisResfileHandle = 0;
PRIVATE INT32 g_voisSpkfileHandle = 0;
PRIVATE INT32 g_voisMicfileHandle = 0;
PRIVATE INT32 g_voisPcmDumpBufferSwapFlag = FALSE;
PRIVATE INT32 g_voisPcmDumpFlag = FALSE;///featurehone data dump flag
PRIVATE INT32 g_voisSmartPhonePcmDumpFlag = FALSE;///smartphone data dump flag
extern UINT32 ML_LocalLanguage2UnicodeBigEnding(const UINT8 *in, UINT32 in_len, UINT8 **out, UINT32 *out_len, UINT8 nCharset[12]);

// memory from ddr
/*
volatile UINT32 *g_voisMicPcmBuffer = (volatile UINT32 *)0x9fa00000;
volatile UINT32 *g_voisSpkPcmBuffer = (volatile UINT32 *)0x9fc00000;
volatile UINT32 *g_voisResPcmBuffer = (volatile UINT32 *)0x9fe00000;
*/

#endif //VOIS_DUMP_PCM

PRIVATE INT32 g_voisITFSwapFlag = FALSE;///ITF swap flag

#ifdef XF_MAGIC_SOUND_SUPPORT
/* 实例句柄 */
void *pMVHANDLE = NULL;

ivTMVUserSys tUserSys;

ivTMVResPackDesc pBg[2];

ivStatus iVcstatus;

char *pHeal=NULL;

BOOL XF_magic_enble=FALSE;

INT32 MagicSoundBufLen=0;

const UINT8 ivMVRes []=
{
#include "ivMVRes.tab"
};

UINT8 xf_magic_mode[7]= {ivMV_CHANGE_NONE,
                         ivMV_ROLE_NONE,
                         ivMV_VE_NONE,
                         ivMV_EQID_NONE,
                         ivMV_BGID_NONE,
                         0,
                         0
                        };



ivInt32 ivCall ReadResCB(
    ivPointer        pParameter,        /* [in] user callback parameter */
    ivPointer        pBuffer,        /* [out] read resource buffer */
    ivResAddress    iPos,            /* [in] read start position */
    ivResSize        nSize )            /* [in] read size */
{
    //FILE* pFile = (FILE*)pParameter;
    //fseek(pFile, iPos, SEEK_SET);
    //fread(pBuffer, 1, nSize, pFile);

    UINT8 *ivMVRes_p=(UINT8*)pParameter;
    memcpy(pBuffer,ivMVRes_p+iPos,nSize);

    return 0;
}


#endif



#ifdef JT_MAGIC_SOUND_SUPPORT
//for jtAE.sheen
//#define AE_WORK_IN_TASK
static volatile int8 OPEN_JTAE=0;
static volatile int16 aeParam[4];//for jtAEParam.
static volatile unsigned long  pJtAEHandle = 0;
static volatile int8* pJtAEInitBuf=NULL;
static volatile int8* pPCMTempBuf=NULL;
static volatile int8 AE_PRO_OVER=1;

//test
//static volatile int32 timestart;
//static volatile int32 timeend;

extern char * mmc_MemMalloc(int32 nsize);
extern void mmc_MemFreeLast(char * pbuf);
extern void mdi_audio_ae_process(UINT32  *pHandle,  INT16 *pData, INT32 dataLen, void (*ae_callback)(void));
//end.sheen
#endif

#ifdef MAGIC_AE_SUPPORT
//for jtAE.sheen
//#define AE_WORK_IN_TASK
static volatile BOOL bMorphVoiceFlag=FALSE;
static volatile UINT8 MorphVoiceParam= RDA_CHANGE_NONE ;//for jtAEParam.
#endif

PRIVATE BOOL g_VoisMixFinishData = FALSE;
/// End address of the stream buffer
PRIVATE UINT32 g_VoisMixBufferLength = 0;

/// Start address of the stream buffer
PRIVATE UINT32* g_VoisMixBufferStart= NULL;

/// End address of the stream buffer
PRIVATE UINT32* g_VoisMixBufferTop = NULL;

/// Number of bytes of data to read in the buffer
PRIVATE INT32 g_VoisMixBufferRemainingBytes = 0;

/// Current position in the buffer. Next read data will be read from here.
PRIVATE UINT32* g_VoisMixBufferCurPos= NULL;

MIXVOISE_STATE g_MIXVOICE_TYPE = MIXVOISE_STATE_IDLE;
VOIS_USER_HANDLER_T g_MIXVOICE_Handler = NULL;
// =============================================================================
// Aec Delay Unit
// -----------------------------------------------------------------------------
/// Delay Mic and Speaker for Aec
// =============================================================================
//#define SPK_DELAY_SAMPLE_LEN 0
//#define MIC_DELAY_SAMPLE_LEN 0
INT32 spk_delay_sample_len = 35;

#define MIC_DELAY_SAMPLE_MAX_NUM 160
#define SPK_DELAY_SAMPLE_MAX_NUM 160

//INT16 gMicDelayHistoryBuffer[MIC_DELAY_SAMPLE_MAX_NUM+160] = {0};
//INT16 gSpkDelayHistoryBuffer[SPK_DELAY_SAMPLE_MAX_NUM+160] = {0};
// =============================================================================
//  FUNCTIONS
// =============================================================================

#ifdef SPEEH_RECORD_PCM_ENABLE

UINT8 * g_pmallocHeadReceiver=NULL;
UINT8 * g_pmallocPReceiver=NULL;

UINT8 * g_pmallocHeadMic=NULL;
UINT8 * g_pmallocPMic=NULL;

UINT8 *PcmBufferReceiver=NULL;
UINT8 *PcmBufferMic=NULL;

UINT32 sample_test [2]=
{
    0//#include "speech02.tab"
};

UINT32 *g_testbutterP=NULL;

#endif





#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
UINT8 * g_pmallocHeaddec=NULL;
UINT8 * g_pmallocPdec=NULL;

#endif




#define MULTS( x, y)  (INT64)(((INT64)x *(INT64)y)>>16)



#define ABS_AGC(X)  ((X<0)?-X:X)


static int AGC_Coeff_l = 0x7fff;
static int AGC_serror_l = 0;
static int    AGC_acc_error_l = 0;

VOLATILE BOOL g_AGCEnbleFlag = FALSE;
VOLATILE AUD_SPK_LEVEL_T g_SpkLevel = 0;
VOLATILE AUD_ITF_T g_AudInterface = 0;


VOLATILE INT16 g_VoCRunFlag = 0;



//this value is what u want to control audio gain level;
//it should be set by user;
static int  AGC_MAXVAL = 30000; // 16384; // 32767;

static void AGainCtr_S(short *in,short *out,int nLen)
{

    int temp;
    int index;

    INT64 temp_xuml;

    //VOIS_TRACE(TSTDOUT, 0, "AGC");

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

VOID vois_SetAGC( BOOL EnbleFlag)
{
    VOIS_TRACE(TSTDOUT, 0, "AGC EnbleFlag:%d",EnbleFlag);
    if(EnbleFlag==TRUE)
    {
        g_AGCEnbleFlag = TRUE;
    }
    else
    {
        g_AGCEnbleFlag = FALSE;
    }

    return;

}


// =============================================================================
// vois_Open
// -----------------------------------------------------------------------------
/// Open the VOIS service. Must be called before any other VOIS function.
/// This function registers the VoiS context in HAL map engine.
// =============================================================================
PUBLIC VOID vois_Open(VOID)
{
    //  Init the pointer used by HST to say talk with the Platform's
    //  calibration code.
    //  This will allow HST to read the calib buffer.
    hal_MapEngineRegisterModule(HAL_MAP_ID_VOIS, &g_voisMapVersion, &g_voisCtx);
}



#ifdef VOIS_LOOP
// =============================================================================
// hal_VoisVppLoop
// -----------------------------------------------------------------------------
/// Loop between Tx and Rx shortcutting the Speech Fifo.
// =============================================================================
PRIVATE VOID hal_VoisVppLoop(HAL_SPEECH_DEC_IN_T* popedRx,
                             CONST HAL_SPEECH_ENC_OUT_T* pushedTx)
{
    UINT32 i;

    popedRx->dtxOn = 0;
    popedRx->codecMode = g_voisCtx.mode;
    popedRx->decFrameType = 0;
    popedRx->bfi = 0;
    popedRx->sid = 0;
    popedRx->taf = 0;
    popedRx->ufi = 0;

    for (i=0; i< g_voisCtx.bufferSize/4 ; i++)
    {
        if (g_voisUseSample)
        {
            *((UINT32*)popedRx->decInBuf + i) = sample[g_voisIndex++];
            if (g_voisIndex > 3999) // 4000 values
            {
                g_voisIndex = 0;
            }
        }
        else
        {
            *((UINT32*)popedRx->decInBuf + i) = *((UINT32*)pushedTx->encOutBuf + i);
        }
    }
}
#endif // VOIS_LOOP




#ifdef XF_MAGIC_SOUND_SUPPORT




VOID Xf_MagicSoundRun(INT16 *pcm_buf)
{
    //int time_begin,time_end;

    //time_begin=hal_TimGetUpTime();

    if(XF_magic_enble==TRUE)
    {
        MagicSoundBufLen=(int)(g_voisPcmTxStream.length>>2);

        /* 向魔音中存入语音数据 */
        ivMV_AppendData(pMVHANDLE,(short*)pcm_buf, MagicSoundBufLen);
        /*  处理魔音中的语音数据：
            本来应该采用
            while(ivMV_RunStep(pMVHANDLE) == ivMV_OK);
            将append进来的所有数据处理完。
            当每次append的采样点数为ivMV_ProSize时，
            1次ivMV_RunStep就可以处理完数据。
        */
        do
        {

            iVcstatus = ivMV_RunStep(pMVHANDLE);

        }
        while (iVcstatus == ivMV_OK);

        /* 从魔音中取出语音数据 */

        ivMV_GetData(pMVHANDLE,(short*)pcm_buf,&MagicSoundBufLen);
    }

    //time_end=hal_TimGetUpTime();
    //VOIS_TRACE(_PAL| TLEVEL (1), 0, "VoC profile: Xf_MagicSoundRun takes %d ms\n", 1000 * (time_end - time_begin) / 16384);

    return;

}


#endif


#ifdef JT_MAGIC_SOUND_SUPPORT

VOID ae_Callback(VOID)
{
    AE_PRO_OVER=1;

    //timeend=hal_TimGetUpTime();
    //VOIS_TRACE(_PAL| TLEVEL (1), 0, " time = %d ms",((timeend-timestart)*1000)/16384);

}

VOID Jt_MagicSoundRun(INT16 *pcm_buf)
{
//    int time_begin,time_end;

//    time_begin=hal_TimGetUpTime();

#ifndef AE_WORK_IN_TASK
    if(OPEN_JTAE)
    {
        jtAEErr jtRes;
        jtRes=jtAE_Progress(pJtAEHandle, (short*)pcm_buf,160);
        //VOIS_TRACE(_PAL| TLEVEL (1), 0, "jtRes=%d length=%d", jtRes, g_voisPcmTxStream.length);
    }
#else
    if(OPEN_JTAE /*&& AE_PRO_OVER*/)
    {
        //AE over or not, all pcm will dump out.

        //timestart=hal_TimGetUpTime();

        AE_PRO_OVER=0;
        memcpy((void*)pPCMTempBuf, (void*)pcm_buf, (g_voisPcmTxStream.length>>1));//out
        memcpy((void*)pcm_buf, (void*)(pPCMTempBuf+(g_voisPcmTxStream.length>>1)), (g_voisPcmTxStream.length>>1));//in
        //send msg
        mdi_audio_ae_process((UINT32*)&pJtAEHandle, (INT16*)pPCMTempBuf, (INT32)(g_voisPcmTxStream.length>>2), ae_Callback);
        //mdi_audio_ae_process(&pJtAEHandle, (INT16*)g_voisPcmTxStream.startAddress, (INT32)(g_voisPcmTxStream.length>>2), ae_callback);
    }
#endif

//    time_end=hal_TimGetUpTime();
//    hal_HstSendEvent(0xAA000000+((time_end - time_begin)));


}
#endif

// =============================================================================
// vois_SetCalibratedVppCfg
// -----------------------------------------------------------------------------
/// Using a VOIS configuration structure, this function gets the calibrated data
/// from the calibration structure to configure properly a VPP config
/// structure.
/// @param itf Audio Interface used.
/// @param vppCfg VPP configuration structure to set with calibrated data.
// =============================================================================
PRIVATE VOID vois_SetCalibratedVppCfg(UINT8 itf,
                                      VPP_SPEECH_AUDIO_CFG_T* vppCfg)
{
    // FIXME : All calibration to be checked
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    // Set the configuration (in the global variables)
    // Get VPP config From Calib
    vppCfg->echoEsOn     = calibPtr->bb->audio[itf].vocEc.esOn;
    vppCfg->echoEsVad    = calibPtr->bb->audio[itf].vocEc.esVad;
    vppCfg->echoEsDtd    = calibPtr->bb->audio[itf].vocEc.esDtd;
    vppCfg->echoExpRel   = calibPtr->bb->audio[itf].vocEc.ecRel;
    vppCfg->echoExpMu    = ((calibPtr->bb->audio[itf].vocEc.ecMu == 0) || (g_voisCtx.voisAudioCfg.spkLevel < calibPtr->bb->audio[itf].vocEc.ecMu)) ? 0 : 1;
    vppCfg->echoExpMin   = calibPtr->bb->audio[itf].vocEc.ecMin;
    // use fir config from audio_voc struct
    vppCfg->sdf          = (INT32*)calibPtr->audio_voc->voc[itf].fir.sdfFilter;
    vppCfg->mdf          = (INT32*)calibPtr->audio_voc->voc[itf].fir.mdfFilter;
}


#ifdef __BT_PCM_OVER_UART__
extern void aud_BtSendSCOData(UINT8 half );
#endif

PRIVATE BOOL vois_GetMagicSoundFlag(VOID)
{
#if defined( XF_MAGIC_SOUND_SUPPORT)
    return XF_magic_enble;
#elif defined(JT_MAGIC_SOUND_SUPPORT)
    return OPEN_JTAE;
#elif defined(MAGIC_AE_SUPPORT)
	return bMorphVoiceFlag;
#else
    return 0;
#endif
}

#ifdef VOIS_DUMP_PCM
PRIVATE INT32 vois_DumpDataFileOpen(CONST CHAR *name,  UINT32 iFlag, UINT32 iMode)
{
    INT32 result;
    INT32 fd = -1;
    UINT32 uni_len = 0;
    UINT8 *uni_name = NULL;
    if((result = ML_LocalLanguage2UnicodeBigEnding(name,  strlen(name), &uni_name, &uni_len, NULL) ) != 0)
    {
        if(uni_name)
        {
            COS_FREE(uni_name);
            uni_name = NULL;
        }
        return -1;
    }

    fd = FS_Open(uni_name, iFlag, iMode);

    if(uni_name)
    {
        COS_FREE(uni_name);
        uni_name = NULL;
    }

    return fd;

}

PUBLIC VOID vois_DumpPcmDataToTFlashProcess(VOID)
{
    INT32 i, datalen;
    INT32 result = 0;

    if (g_voisSpkPcmBuffer == NULL || g_voisMicPcmBuffer == NULL || g_voisResPcmBuffer == NULL)
    {
        return;
    }

    datalen = HAL_SPEECH_FRAME_SIZE_PCM_BUF * NB_DUMP_PCM_FRAMES / 2;
    i = g_voisPcmDumpBufferSwapFlag * NB_DUMP_PCM_FRAMES * HAL_SPEECH_FRAME_SIZE_PCM_BUF / 8;
    result = FS_Write(g_voisResfileHandle, (UINT8 *)&g_voisResPcmBuffer[i], datalen);
    if(result == ERR_FS_DISK_FULL)
    {
        FS_Truncate(g_voisResfileHandle, 0);
    }
    else if(result < 0)
    {
        vois_DumpPcmDataToTFlashStop();
        return;
    }

    result = FS_Write(g_voisSpkfileHandle, (UINT8 *)&g_voisSpkPcmBuffer[i], datalen);
    if(result == ERR_FS_DISK_FULL)
    {
        FS_Truncate(g_voisSpkfileHandle, 0);
    }
    else if(result < 0)
    {
        vois_DumpPcmDataToTFlashStop();
        return;
    }

    result = FS_Write(g_voisMicfileHandle, (UINT8 *)&g_voisMicPcmBuffer[i], datalen);
    if(result == ERR_FS_DISK_FULL)
    {
        FS_Truncate(g_voisMicfileHandle, 0);
    }
    else if(result < 0)
    {
        vois_DumpPcmDataToTFlashStop();
        return;
    }

    g_voisPcmDumpBufferSwapFlag = 1 - g_voisPcmDumpBufferSwapFlag;

    COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), COS_VOIS_DUMP_PCMDATA_TO_TFLASH_TIMER_ID);

}


PUBLIC VOID vois_DumpPcmDataToTFlashStart(VOID)
{
    g_voisPcmCount = 0;
    g_voisPcmDumpBufferSwapFlag = FALSE;
    if (g_voisSpkPcmBuffer == NULL)
        g_voisSpkPcmBuffer = COS_MALLOC(NB_DUMP_PCM_FRAMES*HAL_SPEECH_FRAME_SIZE_PCM_BUF);
    if (g_voisMicPcmBuffer == NULL)
        g_voisMicPcmBuffer = COS_MALLOC(NB_DUMP_PCM_FRAMES*HAL_SPEECH_FRAME_SIZE_PCM_BUF);
    if (g_voisResPcmBuffer == NULL)
        g_voisResPcmBuffer = COS_MALLOC(NB_DUMP_PCM_FRAMES*HAL_SPEECH_FRAME_SIZE_PCM_BUF);

    if (g_voisSpkPcmBuffer == NULL || g_voisMicPcmBuffer== NULL || g_voisResPcmBuffer == NULL )
    {
        VOIS_ASSERT(0, "vois_DumpPcmDataToTFlashStart: no enough memory for dump");
        return;
    }
    VOIS_TRACE(TSTDOUT, 0, "Vois Dump, g_voisSpkPcmBuffer: 0x%x, g_voisMicPcmBuffer: 0x%x, g_voisResPcmBuffer: 0x%x",
               g_voisSpkPcmBuffer, g_voisMicPcmBuffer, g_voisResPcmBuffer);
    COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , COS_VOIS_OPEN_FILE_TIMER_ID, COS_TIMER_MODE_SINGLE, FS_DUMP_TIMER_CNT);

}

PUBLIC VOID  vois_DumpPcmDataToTFlashStop(VOID)
{
    g_voisPcmDumpFlag = FALSE;
    COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), COS_VOIS_DUMP_PCMDATA_TO_TFLASH_TIMER_ID);

    FS_Close(g_voisResfileHandle);
    FS_Close(g_voisSpkfileHandle);
    FS_Close(g_voisMicfileHandle);

    if (g_voisSpkPcmBuffer)
    {
        COS_FREE(g_voisSpkPcmBuffer);
        g_voisSpkPcmBuffer = NULL;
    }
    if (g_voisMicPcmBuffer)
    {
        COS_FREE(g_voisMicPcmBuffer);
        g_voisMicPcmBuffer = NULL;
    }
    if (g_voisResPcmBuffer)
    {
        COS_FREE(g_voisResPcmBuffer);
        g_voisResPcmBuffer = NULL;
    }

}

PUBLIC VOID vois_DumpPcmDataToTFlashFileOpen(VOID)
{
    UINT8 resDataName[20] = "/t/resData.pcm";
    UINT8 spkDataName[20] = "/t/spkData.pcm";
    UINT8 micDataName[20] = "/t/micData.pcm";

    if((g_voisResfileHandle = vois_DumpDataFileOpen(resDataName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0)) < 0)
    {
        VOIS_TRACE(TSTDOUT, 0, "vois_DumpPcmDataToTFlashFileOpen:resData.pcm file open fail");
        return;
    }
    if((g_voisSpkfileHandle = vois_DumpDataFileOpen(spkDataName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0)) < 0)
    {
        VOIS_TRACE(TSTDOUT, 0, "vois_DumpPcmDataToTFlashFileOpen:spkData.pcm file open fail");
        return;
    }
    if((g_voisMicfileHandle = vois_DumpDataFileOpen(micDataName, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0)) < 0)
    {
        VOIS_TRACE(TSTDOUT, 0, "vois_DumpPcmDataToTFlashFileOpen:micData.pcm file open fail");
        return;
    }
    g_voisPcmDumpFlag = TRUE;

    COS_KillTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY), COS_VOIS_OPEN_FILE_TIMER_ID);

}

#endif

PRIVATE VOID vois_DumpDataHandler( VOID)
{
        if ((UINT32)g_voisBufferCurPos == (UINT32)g_voisBufferTop)
        {
            g_voisBufferCurPos=g_voisBufferStart;

            if (g_voisUserHandler)
            {
                g_voisUserHandler(VOIS_STATUS_END_BUFFER_REACHED);
            }
        }
        else if((UINT32)g_voisBufferCurPos == (UINT32)g_voisBufferStart + (g_voisBufferLength/2))
        {
            if (g_voisUserHandler)
            {
                g_voisUserHandler(VOIS_STATUS_MID_BUFFER_REACHED);
            }
        }
return;
}


// =============================================================================
// vois_VppSpeechHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID vois_VppSpeechHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    //INT32 time0,time1;

    //INT32 time0 = hal_TimGetUpTime();

    //VOIS_TRACE(_PAL| TLEVEL (1), 0, "VoC profile: voc process takes %d ms\n", 1000 * (time0 - time_handle) / 16384);

    if(vpp_SpeechGetType()==1)     //For processing the mic signal.(for example "noise suppressing","magic sound" etc.)
    {

#ifdef SPEECH_NOISE_SUPPRESSER_MIC
        //time0=hal_TimGetUpTime();

#ifdef XF_MAGIC_SOUND_SUPPORT

        if(XF_magic_enble!=TRUE)
        {

#else

#ifdef JT_MAGIC_SOUND_SUPPORT
        if(!OPEN_JTAE)
        {
#else
        if(1)
        {
#endif

#endif

            //time0=hal_TimGetUpTime();
            if(g_voisCtx.voisTramePlayedNb%2==0)
            {
                speex_preprocess_run(mic_den_p, (INT16 *)(g_MicPcmBuffer)+160);
                speex_preprocess_run(mic_den_p, (INT16 *)(g_MicPcmBuffer)+160+80);
            }
            else
            {
                speex_preprocess_run(mic_den_p, (INT16 *)(g_MicPcmBuffer));
                speex_preprocess_run(mic_den_p, (INT16 *)(g_MicPcmBuffer)+80);
            }
        }
        //time1=hal_TimGetUpTime();
        //hal_HstSendEvent(0xAA000000+((time1 - time0)));
#endif


#ifdef XF_MAGIC_SOUND_SUPPORT

        if(g_voisCtx.voisTramePlayedNb%2==0)
        {
            Xf_MagicSoundRun( (INT16 *)(g_MicPcmBuffer)+160);
        }
        else
        {
            Xf_MagicSoundRun( (INT16 *)(g_MicPcmBuffer));
        }
#else



#ifdef JT_MAGIC_SOUND_SUPPORT


        if(g_voisCtx.voisTramePlayedNb%2==0)
        {
            Jt_MagicSoundRun( (INT16 *)(g_MicPcmBuffer)+160);
        }
        else
        {
            Jt_MagicSoundRun( (INT16 *)(g_MicPcmBuffer));
        }

#endif

#endif

#ifdef MAGIC_AE_SUPPORT

        if(g_voisCtx.voisTramePlayedNb%2==0)
        {
             if(bMorphVoiceFlag)
                 MorphVoice_run( (INT16 *)(g_MicPcmBuffer)+160,160 );  
			 
        }
        else
        {
             if(bMorphVoiceFlag)
                 MorphVoice_run( (INT16 *)(g_MicPcmBuffer),160);
        }
#endif
      if(isDTMFGEN_Run())
      	{
	
    	    if(g_voisCtx.voisTramePlayedNb%2==0)
		    	DTMFGen_Process( (INT16 *)(g_MicPcmBuffer)+160,160 );  		
            else
       	  	    DTMFGen_Process( (INT16 *)(g_MicPcmBuffer),160);
      	}
#ifdef VOIS_DUMP_PCM
#ifndef CHIP_HAS_AP

    if(g_voisPcmDumpFlag)
    {
        if ((g_voisPcmCount < NB_DUMP_PCM_FRAMES)&&
            (g_voisCtx.voisTramePlayedNb > VOIS_VPP_UNMUTE_TRAME_NB))
        {
            UINT32 i;
            UINT32 *pTx;

                if (g_voisCtx.voisTramePlayedNb % 2 == 0)
                    pTx = (UINT32 *)g_MicPcmBuffer + HAL_SPEECH_FRAME_SIZE_PCM_BUF / 4;
                else
                    pTx = (UINT32 *)g_MicPcmBuffer;

                //VOIS_TRACE(TSTDOUT, 0, "Dump res data, frame: %d, save in buffer %d", g_voisPcmCount, g_voisPcmTxSwap);

			// copy data
			for (i = g_voisPcmCount*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i < (g_voisPcmCount+1)*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i++)
			{
				g_voisResPcmBuffer[i] = *pTx++;
			}
		}
	}

#else///CHIP_HAS_AP

    if( g_voisSmartPhonePcmDumpFlag == TRUE && g_voisBufferStart != NULL)
    {
        ///add the res flag
        memset( g_voisBufferCurPos, 0xab , 4);
        g_voisBufferCurPos+=(4/4);

        /// get the current pcm pointers
        UINT32 *pTx;
        if (g_voisCtx.voisTramePlayedNb % 2 == 0)
            pTx = (UINT32 *)g_MicPcmBuffer + HAL_SPEECH_FRAME_SIZE_PCM_BUF / 4;
        else
            pTx = (UINT32 *)g_MicPcmBuffer;

        ///copy the res data
        memcpy(g_voisBufferCurPos, pTx, 320); 
        g_voisBufferCurPos+=(320/4);
        vois_DumpDataHandler();
    }

#endif///CHIP_HAS_AP
#endif///VOIS_DUMP_PCM


       if((g_MIXVOICE_TYPE == MIXVOISE_STATE_MIXLOCAL) ||(g_MIXVOICE_TYPE == MIXVOISE_STATE_MIXALL))
        {
      
 	     if(g_voisCtx.voisTramePlayedNb%2==0)
	        vois_MixBuffer( (INT16 *)(g_MicPcmBuffer)+HAL_SPEECH_FRAME_SIZE_PCM_BUF/2,HAL_SPEECH_FRAME_SIZE_PCM_BUF/2 , (g_MIXVOICE_TYPE == MIXVOISE_STATE_MIXALL));  		
            else
               vois_MixBuffer( (INT16 *)(g_MicPcmBuffer),HAL_SPEECH_FRAME_SIZE_PCM_BUF/2, (g_MIXVOICE_TYPE == MIXVOISE_STATE_MIXALL));
       
        }
		
        vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_ENC);

        return;
    }



    // FIXME : All calibration to be checked
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    UINT8 calibItf = g_AudInterface;
    if((g_AudInterface == AUD_ITF_BLUETOOTH) && (aud_GetBtNRECFlag()))
    {
        calibItf = CALIB_ITF_BLUETOOTH_NREC;
    }
    if((g_AudInterface == AUD_ITF_EAR_PIECE) && (aud_GetForceReceiverMicSelectionFlag()))
    {
        calibItf = CALIB_ITF_EAR_PIECE_THREE;
    }


    // VOIS_TRACE(TSTDOUT, 0, "VoC");

    g_VoCRunFlag=0; //It must be after "mic processing".Beacause the VoC must be stopped in right status.


    if (g_voisCtx.voisTramePlayedNb == VOIS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        vpp_SpeechAudioCfg((VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);
    }

    // FIXME: trick to only copy data after a encode finished interrupt
    {
        // Copy the buffer to decode (Rx) and fetch from VPP's memory
        // the freshly encoded buffer (Tx).
#ifdef VOIS_LOOP
        hal_VoisVppLoop(g_voisCtx.voisDecInput, g_voisCtx.voisEncOutput);
#else
        HAL_SPEECH_DEC_IN_T* in = g_voisCtx.voisDecInput;
        HAL_SPEECH_ENC_OUT_T* out = g_voisCtx.voisEncOutput;
        hal_SpeechPopRxPushTx(g_voisCtx.voisDecInput, g_voisCtx.voisEncOutput);

#ifdef SPEEH_RECORD_DEC_DATA_ENABLE

        if(g_pmallocPdec<g_pmallocHeaddec+TEST_BUFFER_LEN-100)
        {
            memcpy(g_pmallocPdec, g_voisCtx.voisDecInput, sizeof(HAL_SPEECH_DEC_IN_T));
            g_pmallocPdec=g_pmallocPdec+sizeof(HAL_SPEECH_DEC_IN_T);
        }
        else
        {
            VOIS_TRACE(TSTDOUT, 0, "DEC DATA OVER!");
        }

#endif
            if(g_voisCtx.voisTramePlayedNb%2==0)
            {
               DTMF_DetectPushVoiceData((g_SpeakerPcmBuffer)+160, 160);
            }
            else
            {
                DTMF_DetectPushVoiceData((g_SpeakerPcmBuffer),160);
            }

        if((g_MIXVOICE_TYPE == MIXVOISE_STATE_MIXREMOTE) ||(g_MIXVOICE_TYPE == MIXVOISE_STATE_MIXALL))
        {
 	     if(g_voisCtx.voisTramePlayedNb%2==0)
	        vois_MixBuffer( (INT16 *)(g_SpeakerPcmBuffer)+HAL_SPEECH_FRAME_SIZE_PCM_BUF/2,HAL_SPEECH_FRAME_SIZE_PCM_BUF/2 ,FALSE);  		
            else
                {
               vois_MixBuffer( (INT16 *)(g_SpeakerPcmBuffer),HAL_SPEECH_FRAME_SIZE_PCM_BUF/2,FALSE);
                }
       
        }
        if(g_AGCEnbleFlag==TRUE&&g_SpkLevel==AUD_SPK_VOL_7&&g_AudInterface==AUD_ITF_RECEIVER)
        {
            if(g_voisCtx.voisTramePlayedNb%2==0)
            {
                AGainCtr_S((INT16 *)(g_SpeakerPcmBuffer)+160,(INT16 *)(g_SpeakerPcmBuffer)+160,160);
            }
            else
            {
                AGainCtr_S((INT16 *)(g_SpeakerPcmBuffer),(INT16 *)(g_SpeakerPcmBuffer),160);
            }
        }


#ifdef SPEECH_NOISE_SUPPRESSER_SPEAKER

        /*

#ifdef XF_MAGIC_SOUND_SUPPORT

            if(XF_magic_enble!=TRUE){

#else

    #ifdef JT_MAGIC_SOUND_SUPPORT
            if(!OPEN_JTAE)    {
    #else
            if(1)    {
    #endif

#endif
        */

//    time0=hal_TimGetUpTime();


        if(g_voisCtx.voisTramePlayedNb%2==0)
        {
            speex_preprocess_run(speaker_den_p, (INT16 *)(g_SpeakerPcmBuffer)+160);
            speex_preprocess_run(speaker_den_p, (INT16 *)(g_SpeakerPcmBuffer)+160+80);
        }
        else
        {
            speex_preprocess_run(speaker_den_p, (INT16 *)(g_SpeakerPcmBuffer));
            speex_preprocess_run(speaker_den_p, (INT16 *)(g_SpeakerPcmBuffer)+80);
        }

        //}
//    time1=hal_TimGetUpTime();
//    hal_HstSendEvent(0xBB000000+((time1 - time0)));

#endif


#if defined( XF_MAGIC_SOUND_SUPPORT)

        if(XF_magic_enble==TRUE)

#elif defined(JT_MAGIC_SOUND_SUPPORT)

        if(OPEN_JTAE)

#else

        if(0)

#endif
        {
            vpp_SpeechSetProcessMicEnableFlag(TRUE);
        }
        else
        {
            vpp_SpeechSetProcessMicEnableFlag(TRUE);
        }


#ifdef SPEEH_RECORD_PCM_ENABLE

        if(g_voisCtx.voisTramePlayedNb%2==0)
        {


            if(g_pmallocPReceiver<g_pmallocHeadReceiver+500*1024-320)
            {

                memcpy(g_pmallocPReceiver,PcmBufferReceiver+320, 320);
                g_pmallocPReceiver=g_pmallocPReceiver+320;

            }
            else
            {
                VOIS_TRACE(TSTDOUT| TLEVEL (1), 0, "RE OVER!");
            }


            if(g_pmallocPMic<g_pmallocHeadMic+500*1024-320)
            {
                memcpy(g_pmallocPMic,PcmBufferMic+320, 320);
                g_pmallocPMic=g_pmallocPMic+320;
            }
            else
            {
                VOIS_TRACE(TSTDOUT| TLEVEL (1), 0, "ME OVER!");
            }


        }
        else
        {



            if(g_pmallocPReceiver<g_pmallocHeadReceiver+500*1024-320)
            {

                memcpy(g_pmallocPReceiver,PcmBufferReceiver, 320);
                g_pmallocPReceiver=g_pmallocPReceiver+320;

                //memcpy(g_pmallocPReceiver,PcmBufferReceiver, 640); //for gallite
                //g_pmallocPReceiver=g_pmallocPReceiver+640;
            }
            else
            {
                VOIS_TRACE(TSTDOUT| TLEVEL (1), 0, "RH OVER!");
            }


            if(g_pmallocPMic<g_pmallocHeadMic+500*1024-320)
            {
                memcpy(g_pmallocPMic,PcmBufferMic, 320);
                g_pmallocPMic=g_pmallocPMic+320;
            }
            else
            {
                VOIS_TRACE(TSTDOUT| TLEVEL (1), 0, "MH OVER!");
            }



        }


#endif

            vpp_SpeechSetAecEnableFlag(calibPtr->bb->audio[calibItf].audioParams.AecEnbleFlag,
                                       calibPtr->bb->audio[calibItf].audioParams.AgcEnbleFlag);




        vpp_SpeechSetFilterPara(calibPtr->bb->audio[calibItf].audioParams.HighPassFilterFlag,
                                calibPtr->bb->audio[calibItf].audioParams.NotchFilterFlag,
                                calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserFlag,
                                calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserWithoutSpeechFlag);


#ifdef SPEECH_NOISE_SUPPRESSER_MIC


#ifdef XF_MAGIC_SOUND_SUPPORT

        if(XF_magic_enble!=TRUE)
        {

#else

#ifdef JT_MAGIC_SOUND_SUPPORT
        if(!OPEN_JTAE)
        {

#else

        if(1)
        {

#endif

#endif

            vpp_SpeechSetAecPara(127,
                                 calibPtr->bb->audio[calibItf].audioParams.NoiseMin,
                                 calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimitStep,
                                 calibPtr->bb->audio[calibItf].audioParams.AmpThr,
                                 calibPtr->bb->audio[calibItf].audioParams.StrongEchoFlag);
        }
        else
        {
            vpp_SpeechSetAecPara(calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimit,
                                 calibPtr->bb->audio[calibItf].audioParams.NoiseMin,
                                 calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimitStep,
                                 calibPtr->bb->audio[calibItf].audioParams.AmpThr,
                                 calibPtr->bb->audio[calibItf].audioParams.StrongEchoFlag);

        }

#else
        vpp_SpeechSetAecPara(calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimit,
                             calibPtr->bb->audio[calibItf].audioParams.NoiseMin,
                             calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimitStep,
                             calibPtr->bb->audio[calibItf].audioParams.AmpThr,
                             calibPtr->bb->audio[calibItf].audioParams.StrongEchoFlag);

#endif

        // speex_config.tx.flag = calibPtr->bb->audio[itf].audioParams.NoiseSuppresserFlag;
        // speex_config.rx.flag = calibPtr->bb->audio[itf].audioParams.NoiseSuppresserWithoutSpeechFlag;
        vpp_SpeechSetNoiseSuppressParaAll(&(calibPtr->audio_voc->voc[calibItf].speex));

        // agc_config.tx.flag = TRUE;
        // agc_config.rx.flag = FALSE;
        vpp_SpeechSetNonClipParaAll(&(calibPtr->audio_voc->voc[calibItf].agc));

        vpp_SpeechSetAecAllPara(&(calibPtr->audio_voc->voc[calibItf].aec));

        vpp_SpeechSetScaleDigitalGainPara(&(calibPtr->audio_voc->voc[calibItf].gain), g_voisCtx.voisAudioCfg.spkLevel);

        vpp_SpeechSetHpfPara(&(calibPtr->audio_voc->voc[calibItf].hpf));

        vpp_SpeechSetEQPara(&(calibPtr->audio_voc->voc[calibItf].eq));

        vpp_SpeechSetMSDFPara(&(calibPtr->audio_voc->voc[calibItf].fir));

        vpp_SpeechSetWebRtcAgcParaAll(&(calibPtr->audio_voc->voc[calibItf].webrtcagc));

        if (g_msdfReload)
        {
            vpp_SpeechMSDFReload();
            g_msdfReload = 0;
        }

        if (vois_GetMagicSoundFlag())
        {
            vpp_SpeechSetEncDecPocessingParams(calibItf, g_voisCtx.voisAudioCfg.spkLevel, 0);
        }

#ifdef VOIS_DUMP_PCM
#ifndef CHIP_HAS_AP

    if(g_voisPcmDumpFlag)
    {
        if ((g_voisPcmCount < NB_DUMP_PCM_FRAMES)&&
            (g_voisCtx.voisTramePlayedNb > VOIS_VPP_UNMUTE_TRAME_NB))
        {
            UINT32 i;
            UINT32 *pRx;

                if (g_voisCtx.voisTramePlayedNb % 2 == 0)
                    pRx = (UINT32*)g_SpeakerPcmBuffer + HAL_SPEECH_FRAME_SIZE_PCM_BUF / 4;
                else
                    pRx = (UINT32*)g_SpeakerPcmBuffer;

                //VOIS_TRACE(TSTDOUT, 0, "Dump spk data, frame: %d, save in buffer %d", g_voisPcmCount, g_voisPcmRxSwap);

                // copy data
                for (i = g_voisPcmCount*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i < (g_voisPcmCount+1)*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i++)
                {
                    g_voisSpkPcmBuffer[i] = *pRx++;
                }

                if ((g_voisPcmCount == NB_DUMP_PCM_FRAMES - 1) || (g_voisPcmCount == (NB_DUMP_PCM_FRAMES / 2 - 1)))
                {
                    COS_SetTimerEX(BAL_TH(BAL_DEV_MONITOR_TASK_PRIORITY) , COS_VOIS_DUMP_PCMDATA_TO_TFLASH_TIMER_ID, COS_TIMER_MODE_SINGLE, FS_DUMP_TIMER_CNT);
                }

        if (g_voisPcmCount == NB_DUMP_PCM_FRAMES - 1) 
            g_voisPcmCount = 0;
        else
            g_voisPcmCount++;
        }
    }

#else///CHIP_HAS_AP

    if( g_voisSmartPhonePcmDumpFlag == TRUE && g_voisBufferStart != NULL)
    {
        ///add the SPK flag
        memset( g_voisBufferCurPos, 0xac, 4);
        g_voisBufferCurPos+=(4/4);

        /// get the current pcm pointers
        UINT32 *pRx;

        if (g_voisCtx.voisTramePlayedNb % 2 == 0)
            pRx = (UINT32*)g_SpeakerPcmBuffer + HAL_SPEECH_FRAME_SIZE_PCM_BUF / 4;
        else
            pRx = (UINT32*)g_SpeakerPcmBuffer;

        ///copy the SPK data
        memcpy(g_voisBufferCurPos, pRx, 320);
        g_voisBufferCurPos+=(320/4);
        vois_DumpDataHandler();
    }

#endif///CHIP_HAS_AP
#endif // VOIS_DUMP_PCM

#endif

#ifdef __BT_PCM_OVER_UART__

        if(g_voisCtx.voisTramePlayedNb > VOIS_VPP_UNMUTE_TRAME_NB)
        {
            if( g_voisCtx.voisTramePlayedNb%2==0)
            {
                aud_BtSendSCOData(1);
            }
            else
            {
                aud_BtSendSCOData(0);
            }
        }
#endif

#if (defined(CHIP_HAS_AP) && !defined(VOIS_DUMP_PCM)) || !defined(CHIP_HAS_AP)
if(g_voisRecordFlag == TRUE)vpp_SpeechSetReocrdSpeech(TRUE);

        if(g_voisRecordFlag == TRUE&&g_voisBufferStart!=NULL)
        {



            if(g_voisCtx.voisTramePlayedNb%2==0)
            {
                memcpy(g_voisBufferCurPos,g_MicPcmBuffer+160, 320);
            }
            else
            {
                memcpy(g_voisBufferCurPos,g_MicPcmBuffer, 320);
            }


            g_voisBufferCurPos+=(320/4);

            if ((UINT32)g_voisBufferCurPos == (UINT32)g_voisBufferTop)
            {
                g_voisBufferCurPos=g_voisBufferStart;

                if (g_voisUserHandler)
                {
                    g_voisUserHandler(VOIS_STATUS_END_BUFFER_REACHED);
                }

            }
            else if((UINT32)g_voisBufferCurPos == (UINT32)g_voisBufferStart + (g_voisBufferLength/2))
            {

                if (g_voisUserHandler)
                {
                    g_voisUserHandler(VOIS_STATUS_MID_BUFFER_REACHED);
                }
            }

    }
#endif

        g_voisCtx.voisTramePlayedNb++;
    }
    return;
}






// =============================================================================
// vois_Setup
// -----------------------------------------------------------------------------
/// Configure the vois service..
///
/// This functions configures the audio interface for the speech stream:
/// gain for the side tone and the speaker, input selection for the microphone
/// and output selection for the speaker... It also configures the decoding
/// parameters like audio cancellation. (cf #VOIS_AUDIO_CFG_T for exhaustive
/// details).
///
/// DO NOT CALL THAT FUNCTION BEFORE #vois_start.
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #VOIS_AUDIO_CFG_T for more details.
/// @return #VOIS_ERR_NO it can execute the configuration.
// =============================================================================

PUBLIC VOIS_ERR_T vois_Setup(AUD_ITF_T itf, CONST VOIS_AUDIO_CFG_T* cfg)
{
    AUD_ERR_T audErr = AUD_ERR_NO;
    AUD_LEVEL_T audioCfg;
    VPP_SPEECH_AUDIO_CFG_T vppCfg;
    UINT8 calibItf = itf;

    // FIXME : All calibration to be checked
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    if((itf == AUD_ITF_BLUETOOTH) && (aud_GetBtNRECFlag()))
    {
        calibItf = CALIB_ITF_BLUETOOTH_NREC;
    }
    if((itf == AUD_ITF_EAR_PIECE) && (aud_GetForceReceiverMicSelectionFlag()))
    {
        calibItf = CALIB_ITF_EAR_PIECE_THREE;
    }

    VOIS_TRACE(TSTDOUT, 0, "vois_Setup");

    // Check that VOIS is running
    if (g_voisCtx.voisItf == AUD_ITF_NONE || !g_voisCtx.voisStarted)
    {
        // Ignore this call
        return VOIS_ERR_NO;
    }

#ifndef VOC_INTR_STEP
    UINT32 Wakeup_Mask_Status =  hal_VocIntrClose();
    if(Wakeup_Mask_Status==0xffffffff)
        return VOIS_ERR_NO;
#else
#ifdef __BT_PCM_OVER_UART__
    //do null
#else
    UINT32 now,voc_time;
    now = hal_TimGetUpTime();

    while(! g_VoCRunFlag)
    {
        voc_time = hal_TimGetUpTime();
        if (voc_time - now >(16384>>2))
        {
            g_VoCRunFlag=1;
        }
    }
    now = hal_TimGetUpTime();

    while( (g_VoCRunFlag==1)&&(g_voisCtx.voisStarted==TRUE))
    {
        // VOIS_TRACE(TSTDOUT, 0, "vois_Setup waiting");
        voc_time = hal_TimGetUpTime();
        if (voc_time - now > 16384)
        {
            g_VoCRunFlag=0;
            hal_HstSendEvent(0xaaaa9999);
        }
    }
#endif
#endif
    g_SpkLevel=cfg->spkLevel;

    g_AudInterface=itf;

    // Record the last applied configuration
    g_voisCtx.voisLatestCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisLatestCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisLatestCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisLatestCfg.toneLevel = cfg->toneLevel;
    g_voisCtx.voisLatestCfg.encMute = cfg->encMute;
    g_voisCtx.voisLatestCfg.decMute = cfg->decMute;

    // Those parameters are not calibration dependent.
    g_voisCtx.voisVppCfg.encMute = cfg->encMute;
    g_voisCtx.voisVppCfg.decMute = cfg->decMute;

    // disable push-to-talk
    g_voisCtx.voisVppCfg.if1 = 0;

    // Set audio interface
    g_voisCtx.voisAudioCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisAudioCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisAudioCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisAudioCfg.toneLevel = cfg->toneLevel;

    // get calibration params for Vpp Speech. Need to be called after setting g_voisCtx
    vois_SetCalibratedVppCfg(calibItf, (VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);

    // If the interface is different, we stop and resume the streams
    // to apply the interface change.
    if (itf != g_voisCtx.voisItf)
    {
#if defined(CHIP_HAS_AP) && defined(VOIS_DUMP_PCM)
    if((g_voisSmartPhonePcmDumpFlag == TRUE) && (g_voisITFSwapFlag == FALSE))
#else
    if((g_voisRecordFlag == TRUE) && (g_voisITFSwapFlag == FALSE))
#endif
    {
        g_voisITFSwapFlag = TRUE;
    }
        // Stop the stream.
        audErr = vois_Stop();
        g_voisCtx.voisItf = itf;


        // Set the DAC to use
        g_voisPcmRxStream.voiceQuality =
#ifdef VOIS_USE_STEREO_DAC_FOR_LOUDSPEAKER
            // use stereo DAC if the mode is loudspeaker
            (itf == AUD_ITF_LOUD_SPEAKER) ? FALSE :
#endif
#ifdef VOIS_USE_STEREO_DAC_FOR_EARPIECE
            // use stereo DAC if the mode is earpiece
            (itf == AUD_ITF_EAR_PIECE) ?   FALSE :
#endif
            // default
            TRUE;

        // Enforce temporary muting and progressive level resume by the
        // speech handler.
        g_voisCtx.voisTramePlayedNb = 1;

        // Set audio interface, initially muted.
        *(AUD_LEVEL_T*)&audioCfg = *(AUD_LEVEL_T*)&g_voisCtx.voisAudioCfg;
        audioCfg.spkLevel   = cfg->spkLevel;
        audioCfg.micLevel   = cfg->micLevel;
        audioCfg.sideLevel  = cfg->sideLevel;
        audioCfg.toneLevel  = cfg->toneLevel;

        // Set the VPP config, initially muted.
        *(VPP_SPEECH_AUDIO_CFG_T*)&vppCfg = *(VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg;
        vppCfg.encMute = VPP_SPEECH_MUTE;
        vppCfg.decMute = VPP_SPEECH_MUTE;

        vpp_SpeechAudioCfg(&vppCfg);

        vpp_SpeechReset();


        if (audErr == AUD_ERR_NO)
        {
            // configure AUD and VPP buffers
            audErr = vois_Start(itf,  cfg);
        }

        if (audErr == AUD_ERR_NO)
        {
            // Start the other flux only if the Rx one is started
            audErr = aud_StreamRecord(itf, &g_voisPcmTxStream, &audioCfg);
        }
    }
    else
    {
        // Just apply the configuration change on the same
        // interface.
        g_voisITFSwapFlag = FALSE;
        vpp_SpeechAudioCfg((VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);
        audErr = aud_Setup(itf, (AUD_LEVEL_T*)&g_voisCtx.voisAudioCfg);
    }
    //Set Mic Gain in VoC

    if (cfg->encMute == VPP_SPEECH_MUTE || cfg->micLevel == SND_MIC_MUTE)
    {
        vpp_SpeechSetInAlgGain(CALIB_AUDIO_GAIN_VALUE_MUTE);
    }
    else
    {
        CALIB_AUDIO_IN_GAINS_T inGains;
        INT32 algVol;

        if (cfg->micLevel == SND_MIC_VOL_CALL)
        {
            inGains = calibPtr->bb->audio[calibItf].audioGains.inGainsCall;
        }
        else
        {
            inGains = calibPtr->bb->audio[calibItf].audioGains.inGainsRecord;
        }
        algVol = inGains.alg;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 && !defined(CHIP_DIE_8810E))
        if (hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID) < HAL_SYS_CHIP_METAL_ID_U09 &&
                inGains.adc > -8)
        {
            // Compensate the ADC gain which is lowered in aud_CodecCommonSetMicCfg()
            algVol += inGains.adc + 8 + 1;
        }
#endif

        vpp_SpeechSetInAlgGain(algVol);
    }

    vpp_SpeechSetAecEnableFlag(calibPtr->bb->audio[calibItf].audioParams.AecEnbleFlag,
                               calibPtr->bb->audio[calibItf].audioParams.AgcEnbleFlag);

    vpp_SpeechSetAecPara(calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimit,
                         calibPtr->bb->audio[calibItf].audioParams.NoiseMin,
                         calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimitStep,
                         calibPtr->bb->audio[calibItf].audioParams.AmpThr,
                         calibPtr->bb->audio[calibItf].audioParams.StrongEchoFlag);

    vpp_SpeechSetFilterPara(calibPtr->bb->audio[calibItf].audioParams.HighPassFilterFlag,
                            calibPtr->bb->audio[calibItf].audioParams.NotchFilterFlag,
                            calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserFlag,
                            calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserWithoutSpeechFlag);

    vpp_SpeechSetProcessMicEnableFlag(TRUE);

    // speex_config.tx.flag = calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserFlag;
    // speex_config.rx.flag = calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserWithoutSpeechFlag;
    vpp_SpeechSetNoiseSuppressParaAll(&(calibPtr->audio_voc->voc[calibItf].speex));

    // agc_config.tx.flag = TRUE;
    // agc_config.rx.flag = FALSE;
    vpp_SpeechSetNonClipParaAll(&(calibPtr->audio_voc->voc[calibItf].agc));

    vpp_SpeechSetAecAllPara(&(calibPtr->audio_voc->voc[calibItf].aec));

    vpp_SpeechSetScaleDigitalGainPara(&(calibPtr->audio_voc->voc[calibItf].gain), g_SpkLevel);

    vpp_SpeechSetHpfPara(&(calibPtr->audio_voc->voc[calibItf].hpf));

    vpp_SpeechSetEQPara(&(calibPtr->audio_voc->voc[calibItf].eq));

    vpp_SpeechSetMSDFPara(&(calibPtr->audio_voc->voc[calibItf].fir));

    vpp_SpeechSetWebRtcAgcParaAll(&(calibPtr->audio_voc->voc[calibItf].webrtcagc));

    vpp_SpeechSetInAlgGain(calibPtr->audio_voc->voc[calibItf].agc.tx.gain);
    vpp_SpeechSetOutAlgGain(calibPtr->audio_voc->voc[calibItf].agc.rx.gain);

    VOIS_TRACE(TSTDOUT, 0, "SETUP RxGain = %d, TxGain = %d",
               calibPtr->audio_voc->voc[calibItf].agc.tx.gain,
               calibPtr->audio_voc->voc[calibItf].agc.rx.gain);

//    VOIS_TRACE(TSTDOUT, 0, "g_VoCRunFlag=%d", g_VoCRunFlag);

#ifndef VOC_INTR_STEP
    hal_VocIntrOpen(Wakeup_Mask_Status);
#endif

    switch (audErr)
    {
        // TODO Add error as they come

        case AUD_ERR_NO:

            return VOIS_ERR_NO;
            break;

        default:

            return audErr; // ...unknown error ...
            break;
    }
}


VOID txhandle_half(VOID)
{
    HAL_SPEECH_DEC_IN_T* popedRx;
    HAL_SPEECH_ENC_OUT_T* pushedTx;
	
    g_VoCRunFlag=1;

    if(g_VoisRxToTXLoopFlag)
    {
        popedRx = 	g_voisCtx.voisDecInput; 
        pushedTx = g_voisCtx.voisEncOutput;

        pushedTx->encFrameType = popedRx->decFrameType;
        pushedTx->encMode= popedRx->codecMode;
        memcpy(pushedTx->encOutBuf,popedRx->decInBuf,HAL_SPEECH_FRAME_SIZE_COD_BUF );
	 
        hal_SpeechPopRxPushTx(g_voisCtx.voisDecInput, g_voisCtx.voisEncOutput);
        return;
    }	

    //time_handle=hal_TimGetUpTime();
    if(g_AudInterface == AUD_ITF_BLUETOOTH)
    {

#ifndef CHIP_HAS_AP
        if(aud_GetMicMuteFlag())
        {
            memset(&g_MicPcmBuffer[0], 0 , 320);
        }
        else
#endif
        {
            int i;
            for(i = 0; i<160; i++)
            {
                g_MicPcmBuffer[i] >>= 1;
            }
        }
    }

#ifdef VOIS_DUMP_PCM
#ifndef CHIP_HAS_AP
	if(g_voisPcmDumpFlag)
	{
		if ((g_voisPcmCount < NB_DUMP_PCM_FRAMES)&&
		(g_voisCtx.voisTramePlayedNb > VOIS_VPP_UNMUTE_TRAME_NB))
		{
			UINT32 i;

            // get the current pcm pointers
            UINT32 *pTx = (UINT32*)g_MicPcmBuffer;

            //VOIS_TRACE(TSTDOUT, 0, "Dump res data, frame: %d, save in buffer 0", g_voisPcmCount);

			// copy data
			for (i = g_voisPcmCount*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i < (g_voisPcmCount+1)*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i++)
			{
				g_voisMicPcmBuffer[i] = *pTx++;
			}
		}
	}

#else
    if( g_voisSmartPhonePcmDumpFlag == TRUE && g_voisBufferStart != NULL)
    {
        ///add the mic flag
        memset( g_voisBufferCurPos, 0xad , 4);
        g_voisBufferCurPos+=(4/4);
        ///copy the mic data
        memcpy(g_voisBufferCurPos, g_MicPcmBuffer, 320); 
        VOIS_TRACE(TSTDOUT, 0, "half MIC DATA");
        g_voisBufferCurPos+=(320/4);
        vois_DumpDataHandler();
    }
#endif///CHIP_HAS_AP
#endif///VOIS_DUMP_PCM


    vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC);
    vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_ENC);

    return;
}

VOID txhandle_end(VOID)
{

    HAL_SPEECH_DEC_IN_T* popedRx;
    HAL_SPEECH_ENC_OUT_T* pushedTx;
	
    g_VoCRunFlag=1;

    if(g_VoisRxToTXLoopFlag)
    {
        popedRx = 	g_voisCtx.voisDecInput; 
        pushedTx = g_voisCtx.voisEncOutput;

        pushedTx->encFrameType = popedRx->decFrameType;
        pushedTx->encMode= popedRx->codecMode;
        memcpy(pushedTx->encOutBuf,popedRx->decInBuf,HAL_SPEECH_FRAME_SIZE_COD_BUF );
	 
        hal_SpeechPopRxPushTx(g_voisCtx.voisDecInput, g_voisCtx.voisEncOutput);
	 return;
    }

    //time_handle=hal_TimGetUpTime();
    if(g_AudInterface == AUD_ITF_BLUETOOTH)
    {

#ifndef CHIP_HAS_AP
        if(aud_GetMicMuteFlag())
        {
            memset(&g_MicPcmBuffer[160], 0 , 320);
        }
        else
#endif
        {
            int i;
            for(i = 160; i<320; i++)
            {
                g_MicPcmBuffer[i] >>= 1;
            }
        }
    }

#ifdef VOIS_DUMP_PCM
#ifndef CHIP_HAS_AP
	if(g_voisPcmDumpFlag)
	{
		if ((g_voisPcmCount < NB_DUMP_PCM_FRAMES)&&
		(g_voisCtx.voisTramePlayedNb > VOIS_VPP_UNMUTE_TRAME_NB))
		{
			UINT32 i;

            // get the current pcm pointers
            UINT32 *pTx = (UINT32*)g_MicPcmBuffer + HAL_SPEECH_FRAME_SIZE_PCM_BUF/4;

            //VOIS_TRACE(TSTDOUT, 0, "Dump res data, frame: %d, save in buffer 1", g_voisPcmCount);

			// copy data
			for (i = g_voisPcmCount*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i < (g_voisPcmCount+1)*HAL_SPEECH_FRAME_SIZE_PCM_BUF/4; i++)
			{
				g_voisMicPcmBuffer[i] = *pTx++;
			}
		}
	}

#else
    if( g_voisSmartPhonePcmDumpFlag == TRUE && g_voisBufferStart != NULL)
    {
    ///add the mic flag
        memset( g_voisBufferCurPos, 0xad , 4);
        g_voisBufferCurPos+=(4/4);
        ///copy the mic data
        memcpy(g_voisBufferCurPos,g_MicPcmBuffer+160, 320); 
        VOIS_TRACE(TSTDOUT, 0, "end MIC DATA");
        g_voisBufferCurPos+=(320/4);   
        vois_DumpDataHandler();
    }
#endif///CHIP_HAS_AP
#endif///VOIS_DUMP_PCM

    vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_DEC);
    vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_ENC);

    return;
}



// =============================================================================
// vois_Start
// -----------------------------------------------------------------------------
/// Start the VOIS service.
///
/// This function records from the mic and outputs sound on the speaker
/// on the audio interface specified as a parameter, using the input and output
/// set by the \c parameter. \n
///
/// @param itf Interface providing the audion input and output.
/// @param cfg The configuration set applied to the audio interface
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #VOIS_ERR_NO it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Start(
    CONST AUD_ITF_T      itf,
    CONST VOIS_AUDIO_CFG_T* cfg)
{
    VOIS_PROFILE_FUNCTION_ENTRY(vois_Start);
    VOIS_TRACE(_PAL| TLEVEL (1), 0, "VOIS Start");

    // Initial audio confguration
    AUD_LEVEL_T audioCfg;
    VPP_SPEECH_AUDIO_CFG_T vppCfg;
    AUD_ERR_T audErr;
    UINT8 calibItf = itf;

    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();
    if((itf == AUD_ITF_BLUETOOTH) && (aud_GetBtNRECFlag()))
    {
        calibItf = CALIB_ITF_BLUETOOTH_NREC;
    }
    if((itf == AUD_ITF_EAR_PIECE) && (aud_GetForceReceiverMicSelectionFlag()))
    {
        calibItf = CALIB_ITF_EAR_PIECE_THREE;
    }

    // if (calibPtr->bb->audio[calibItf].audioParams.AecEnbleFlag)
    {
        if (hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_5) != HAL_ERR_NO)
        {
            VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
            return VOIS_ERR_RESOURCE_BUSY;
        }
        g_voisOverlayLoaded = TRUE;
    }


    hal_SwRequestClk(FOURCC_VOIS, HAL_CLK_RATE_104M);

#ifndef CHIP_HAS_AP
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_178M);
#endif

#ifdef SPEECH_NOISE_SUPPRESSER_MIC
#ifdef USE_STATIC_ARRAY
    mic_den_p= speex_preprocess_state_init(FRAME_N, 8000,0);
#else
    mic_den_p = speex_preprocess_state_init(FRAME_N, 8000);
#endif


    hal_SwRequestClk(FOURCC_VOIS, HAL_CLK_RATE_250M);

#ifndef CHIP_HAS_AP
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_178M);
#endif
#endif


#ifdef SPEECH_NOISE_SUPPRESSER_SPEAKER
#ifdef USE_STATIC_ARRAY
    speaker_den_p= speex_preprocess_state_init(FRAME_N, 8000,1);

#else
    speaker_den_p = speex_preprocess_state_init(FRAME_N, 8000);

#endif

    hal_SwRequestClk(FOURCC_VOIS, HAL_CLK_RATE_250M);
#ifndef CHIP_HAS_AP
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_178M);
#endif

#endif

    //  g_AGCEnbleFlag = FALSE;
    g_SpkLevel=cfg->spkLevel;
    g_AudInterface=itf;

    // Record the last applied configuration
    g_voisCtx.voisLatestCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisLatestCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisLatestCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisLatestCfg.toneLevel = cfg->toneLevel;
    g_voisCtx.voisLatestCfg.encMute = cfg->encMute;
    g_voisCtx.voisLatestCfg.decMute = cfg->decMute;

    // Configure the Speech FIFO
    hal_SpeechFifoReset();

    g_voisCtx.voisVppCfg.encMute = cfg->encMute;
    g_voisCtx.voisVppCfg.decMute = cfg->decMute;

    // disable push-to-talk
    g_voisCtx.voisVppCfg.if1 = 0;

    // Set audio interface
    g_voisCtx.voisAudioCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisAudioCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisAudioCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisAudioCfg.toneLevel = cfg->toneLevel;

    // get calibration params for Vpp Speech. Need to be called after setting g_voisCtx
    vois_SetCalibratedVppCfg(calibItf, (VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);
    // Register global var
    g_voisCtx.voisItf = itf;

    g_voisCtx.voisTramePlayedNb = 0;

    // Set audio interface, initially muted
    *(AUD_LEVEL_T*)&audioCfg = *(AUD_LEVEL_T*)&g_voisCtx.voisAudioCfg;
    audioCfg.spkLevel   =  cfg->spkLevel;
    audioCfg.micLevel   =  cfg->micLevel;
    audioCfg.sideLevel  =  cfg->sideLevel;
    audioCfg.toneLevel  =  cfg->toneLevel;

    // Set the VPP config
    *(VPP_SPEECH_AUDIO_CFG_T*)&vppCfg = *(VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg;
    vppCfg.encMute = VPP_SPEECH_MUTE;
    vppCfg.decMute = VPP_SPEECH_MUTE;

    g_SpeakerPcmBuffer=(INT16 *)((UINT32)( vpp_SpeechGetRxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechGetRxPcmBuffer();
    g_MicPcmBuffer=(INT16 *)((UINT32)(vpp_SpeechGetTxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechGetTxPcmBuffer();

#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
    {
        g_pmallocPdec=g_pmallocHeaddec=(UINT8 *)COS_SHMEMMALLOC(TEST_BUFFER_LEN);

        if(g_pmallocHeaddec == NULL)
        {
            if (g_voisOverlayLoaded)
            {
                hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
                g_voisOverlayLoaded = FALSE;
            }
            VOIS_TRACE (TSTDOUT,0,"[vois_Start]*****g_pmallocHeaddec*******no memory to malloc!!!  ************\n");
            VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
            return VOIS_ERR_RESOURCE_BUSY;
        }
        else
        {
            VOIS_TRACE(TSTDOUT, 0, "g_pmallocHeaddec  = %X", g_pmallocHeaddec);
            memset(g_pmallocHeaddec,1,TEST_BUFFER_LEN);
        }
    }
#endif // SPEEH_RECORD_DEC_DATA_ENABLE

#ifdef SPEEH_RECORD_PCM_ENABLE
    {
        g_testbutterP=sample_test;

        g_pmallocPReceiver=g_pmallocHeadReceiver=(UINT8 *)COS_SHMEMMALLOC(500*1024);

        if(g_pmallocHeadReceiver == NULL)
        {
#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
            COS_SHMEMFREE(g_pmallocHeaddec);
#endif
            if (g_voisOverlayLoaded)
            {
                hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
                g_voisOverlayLoaded = FALSE;
            }
            VOIS_TRACE (TSTDOUT,0,"[vois_Start]*****g_pmallocHeadReceiver*******no memory to malloc!!!  ************\n");
            VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
            return VOIS_ERR_RESOURCE_BUSY;
        }
        else
        {
            VOIS_TRACE(TSTDOUT, 0, "g_pmallocHeadReceiver  = %X", g_pmallocHeadReceiver);
            memset(g_pmallocHeadReceiver,1,500*1024);
        }

        g_pmallocPMic=g_pmallocHeadMic=(UINT8 *)COS_SHMEMMALLOC(500*1024);

        if(g_pmallocHeadMic == NULL)
        {
            COS_SHMEMFREE(g_pmallocHeadReceiver);
#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
            COS_SHMEMFREE(g_pmallocHeaddec);
#endif
            if (g_voisOverlayLoaded)
            {
                hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
                g_voisOverlayLoaded = FALSE;
            }
            VOIS_TRACE (TSTDOUT,0,"[vois_Start]******g_pmallocHeadMic******no memory to malloc!!!  ************\n");
            VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
            return VOIS_ERR_RESOURCE_BUSY;
        }
        else
        {
            VOIS_TRACE(TSTDOUT, 0, "g_pmallocHeadMic  = %X", g_pmallocHeadMic);
            memset(g_pmallocHeadMic,1,500*1024);
        }

        //PcmBufferReceiver=(UINT8 *)((UINT32)(vpp_SpeechGetRxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechGetRxPcmBuffer();

        PcmBufferReceiver=(UINT8 *)((UINT32)( vpp_SpeechGetRxPcmBuffer())|0x20000000);
        PcmBufferMic=(UINT8 *)((UINT32)(vpp_SpeechGetTxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechGetTxPcmBuffer();
    }
#endif // SPEEH_RECORD_PCM_ENABLE

    //g_voisBufferStart=(UINT32 *)COS_SHMEMMALLOC(50*160*2);
    //vois_RecordStart(g_voisBufferStart,50*160*2,Vois_test);

    // Typical global vars
    // Those buffer are written by VoC, thus the need to access
    // them the uncached way.
    g_voisCtx.voisEncOutput = (HAL_SPEECH_ENC_OUT_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)vpp_SpeechGetTxCodBuffer());
    g_voisCtx.voisDecInput  = (HAL_SPEECH_DEC_IN_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)vpp_SpeechGetRxCodBuffer());




    DTMF_DetectRun(DTMF_DETECT_TYPE_VOISE);


#ifdef XF_MAGIC_SOUND_SUPPORT


    /* 初始化 */

    if(XF_magic_enble==TRUE    )
    {
        VOIS_TRACE(_PAL| TLEVEL (1), 0, "Vois XF magic sound init.\n");

        /* 多资源包设置 */
        //fidBG = fopen("ivMVRes.irf", "rb");
        //    pBg[0].pCBParam = (void *)fidBG;
        //    pBg[0].pfnRead = ReadResCB;
        /* 多资源包设置 */
        //    fidBG = fopen("ivMVRes.irf", "rb");
        pBg[0].pCBParam = (void *)ivMVRes;
        pBg[0].pfnRead = ReadResCB;;


        /* 用户系统信息设置 */

        pHeal=(char *)mmc_MemMalloc(20*1024 );

        if(pHeal==NULL)
        {
            VOIS_TRACE(_PAL| TLEVEL (1), 0, "Vois XF magic sound init failed: not enough memory, just disable it.\n");
            XF_magic_enble=FALSE;
        }

        tUserSys.lpHeap = (ivPointer)pHeal;
        tUserSys.nHeapSize = ivMV_SUGGEST_HEAPSIZE;
        tUserSys.pLogCBParam = 0;
        tUserSys.pLogWrite = 0;
        tUserSys.pMVResPackDesc = pBg;
        tUserSys.nMVResPackDesc = 1;


        if (ivMV_OK != ivMV_Init(&pMVHANDLE,&tUserSys))
        {
            XF_magic_enble=FALSE;
        }
        else
        {
            /* 参数设置 */
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_CHANGE, xf_magic_mode[ivMV_PARAM_CHANGE]);
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_ROLE, xf_magic_mode[ivMV_PARAM_ROLE]);
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_EFFECT, xf_magic_mode[ivMV_PARAM_EFFECT]);
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_EQID, xf_magic_mode[ivMV_PARAM_EQID]);
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_BGID, xf_magic_mode[ivMV_PARAM_BGID]);
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_BGVolume,xf_magic_mode[ivMV_PARAM_BGVolume]);
            ivMV_SetParam(pMVHANDLE,ivMV_PARAM_MVolume, xf_magic_mode[ivMV_PARAM_MVolume]);

            //set cpu freqence. use HAL_SYS_FREQ_AVPS temporary

            hal_SwRequestClk(FOURCC_VOIS, HAL_CLK_RATE_250M);

#ifndef CHIP_HAS_AP
            hal_SysSetVocClock(HAL_SYS_VOC_FREQ_250M);
#endif

        }

        if(XF_magic_enble==TRUE)
        {
            vpp_SpeechSetProcessMicEnableFlag(TRUE);
        }



    }


#endif





#ifdef JT_MAGIC_SOUND_SUPPORT
    //for JT AE.sheen
    /*
    testID=1-testID;
    //OPEN_JTAE=0;
    if(testID)
        {
        vois_set_ae(jtAE_PARAM_TIMBRE, jtAE_TIMBRE_FEMALE);//jtAE_TIMBRE_FEMALE
        VOIS_TRACE(_PAL| TLEVEL (1), 0, "test AE CHILD!");
        }
    else
        {
        //vois_set_ae(jtAE_PARAM_AUDIOEFFECT, jtAE_AUDIOEFFECT_ROBOT);
        vois_set_ae(jtAE_PARAM_TIMBRE, jtAE_TIMBRE_MALE );
        VOIS_TRACE(_PAL| TLEVEL (1), 0, "test AE ROBOT!");
        }*/

    while(OPEN_JTAE)//use while for break.sheen
    {
        jtAEErr jtRes;
        int i;
        AE_PRO_OVER=1;
        //NEED_DEC_ENC=0;
        //CANCEL_CALLBACK=0;
        pJtAEHandle=0;
        pJtAEInitBuf=NULL;
        //malloc pJtAEInitBuf and pPCMTempBuf
#ifdef AE_WORK_IN_TASK
        pJtAEInitBuf=(int8 *)mmc_MemMalloc(jtAE_MIN_BUF_LEN +sizeof(HAL_SPEECH_PCM_BUF_T));
#else
        pJtAEInitBuf=(int8 *)mmc_MemMalloc(jtAE_MIN_BUF_LEN );
#endif
        //VOIS_TRACE(_PAL| TLEVEL (1), 0, "pJtAEInitBuf=%x %x",pJtAEInitBuf, pJtAEInitBuf+jtAE_MIN_BUF_LEN);
        if(pJtAEInitBuf==NULL)
        {
            VOIS_TRACE(_PAL| TLEVEL (1), 0, "pJtAEInitBuf malloc error!Stop jtAE!");
            OPEN_JTAE=0;
            break;
        }
#ifdef AE_WORK_IN_TASK
        pPCMTempBuf=pJtAEInitBuf+jtAE_MIN_BUF_LEN;
        memset((void*)pPCMTempBuf, 0, sizeof(HAL_SPEECH_PCM_BUF_T));
#endif

        jtRes = jtAE_Init((unsigned long*)&pJtAEHandle, (void*)pJtAEInitBuf);
        if(jtRes!=jtAE_ERR_NONE)
        {
            VOIS_TRACE(_PAL| TLEVEL (1), 0, "jtAE_Init error!Stop jtAE!");
            OPEN_JTAE=0;
            break;
        }

        i=0;
        while(aeParam[i]==0)
        {
            i++;
            if(i>3)
            {
                OPEN_JTAE=0;
                break;
            }
        }

        if(OPEN_JTAE)
        {
            //jtRes = jtAE_SetParam(pJtAEHandle, jtAE_PARAM_SAMPLE, jtAE_SAMPLE_8K);//test
            //jtRes = jtAE_SetParam(pJtAEHandle, jtAE_PARAM_VOLUME, 28000);//test
            jtRes = jtAE_SetParam(pJtAEHandle, (jtAEParam)i, (long)aeParam[i]);
            if(jtRes!=jtAE_ERR_NONE)
            {
                VOIS_TRACE(_PAL| TLEVEL (1), 0, "jtAE_SetParam error!Stop jtAE!");
                OPEN_JTAE=0;
                break;
            }
            VOIS_TRACE(_PAL| TLEVEL (1), 0, "sheen: AE OPEN! jtAEParam=%d iValue=%d", i, aeParam[i]);
        }
        if(OPEN_JTAE)
        {
            vpp_SpeechSetProcessMicEnableFlag(TRUE);
        }

        //set cpu freqence. use HAL_SYS_FREQ_AVPS temporary

        hal_SwRequestClk(FOURCC_VOIS, HAL_CLK_RATE_250M);
#ifndef CHIP_HAS_AP
        hal_SysSetVocClock(HAL_SYS_VOC_FREQ_250M);
#endif

        break;
    }
    //sheen.end
#endif
#ifdef MAGIC_AE_SUPPORT
     if(bMorphVoiceFlag)
     {
         MorphVoice_Init(1);
         // Setup the 'SoundTouch' object for processing the sound
         MorphVoice_setup(HAL_AIF_FREQ_8000HZ,MorphVoiceParam,TRUE,0);

         hal_SwRequestClk(FOURCC_VOIS, HAL_SYS_FREQ_250M);
     }
#endif

    // PCM Audio Tx stream, output of VPP
#ifndef SMALL_BSS_RAM_SIZE
    g_voisPcmRxStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
#endif
    g_voisPcmRxStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
    g_voisPcmRxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    g_voisPcmRxStream.channelNb = HAL_AIF_MONO;
    g_voisPcmRxStream.voiceQuality = TRUE;
    g_voisPcmRxStream.playSyncWithRecord = TRUE;
    g_voisPcmRxStream.halfHandler =  NULL ; // Mechanical interaction with VPP's VOC
    g_voisPcmRxStream.endHandler =  NULL ; // Mechanical interaction with VPP's VOC

#ifndef SMALL_BSS_RAM_SIZE
    // PCM Audio Rx stream, input of VPP
    g_voisPcmTxStream.startAddress = (UINT32*)vpp_SpeechGetTxPcmBuffer();

#endif // SMALL_BSS_RAM_SIZE


    g_voisPcmTxStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
    g_voisPcmTxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    g_voisPcmTxStream.channelNb = HAL_AIF_MONO;
    g_voisPcmTxStream.voiceQuality = TRUE;
    g_voisPcmTxStream.playSyncWithRecord = TRUE;
    g_voisPcmTxStream.halfHandler =  txhandle_half ; // Mechanical interaction with VPP's VOC
    g_voisPcmTxStream.endHandler =  txhandle_end ; // Mechanical interaction with VPP's VOC_AHB_BCPU_DEBUG_IRQ_CAUSE

    UINT32 spStatus =    vpp_SpeechOpen(vois_VppSpeechHandler, VPP_SPEECH_WAKEUP_SW_DECENC);



#ifdef SMALL_BSS_RAM_SIZE
    // PCM Audio Rx stream, input of VPP
    g_voisPcmRxStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
    g_voisPcmTxStream.startAddress = (UINT32*)vpp_SpeechGetTxPcmBuffer();

#endif // SMALL_BSS_RAM_SIZE

    VOIS_TRACE(TSTDOUT, 0, "VPP_SPEECH_OPEN STATUS = %08X", spStatus);
    if (spStatus == HAL_ERR_RESOURCE_BUSY)
    {
#ifdef SPEEH_RECORD_PCM_ENABLE
        COS_SHMEMFREE(g_pmallocHeadReceiver);
        COS_SHMEMFREE(g_pmallocHeadMic);
#endif
#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
        COS_SHMEMFREE(g_pmallocHeaddec);
#endif
        if (g_voisOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_voisOverlayLoaded = FALSE;
        }
        VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
        return VOIS_ERR_RESOURCE_BUSY;
    }
    vpp_SpeechAudioCfg(&vppCfg);

    memset(g_voisPcmRxStream.startAddress,0,g_voisPcmRxStream.length);

    CALIB_AUDIO_IN_GAINS_T inGains;
    INT32 algVol;

    if (cfg->micLevel == SND_MIC_VOL_CALL)
    {
        inGains = calibPtr->bb->audio[calibItf].audioGains.inGainsCall;
    }
    else
    {
        inGains = calibPtr->bb->audio[calibItf].audioGains.inGainsRecord;
    }
    algVol = inGains.alg;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 && !defined(CHIP_DIE_8810E))
    if (hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID) < HAL_SYS_CHIP_METAL_ID_U09 &&
            inGains.adc > -8)
    {
        // Compensate the ADC gain which is lowered in aud_CodecCommonSetMicCfg()
        algVol += inGains.adc + 8 + 1;
    }
#endif

    vpp_SpeechSetInAlgGain(algVol);

    vpp_SpeechSetAecEnableFlag(calibPtr->bb->audio[calibItf].audioParams.AecEnbleFlag,
                               calibPtr->bb->audio[calibItf].audioParams.AgcEnbleFlag);

    vpp_SpeechSetAecPara(calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimit,
                         calibPtr->bb->audio[calibItf].audioParams.NoiseMin,
                         calibPtr->bb->audio[calibItf].audioParams.NoiseGainLimitStep,
                         calibPtr->bb->audio[calibItf].audioParams.AmpThr,
                         calibPtr->bb->audio[calibItf].audioParams.StrongEchoFlag);

    vpp_SpeechSetFilterPara(calibPtr->bb->audio[calibItf].audioParams.HighPassFilterFlag,
                            calibPtr->bb->audio[calibItf].audioParams.NotchFilterFlag,
                            calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserFlag,
                            calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserWithoutSpeechFlag);

    vpp_SpeechSetProcessMicEnableFlag(TRUE);

    // speex_config.tx.flag = calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserFlag;
    // speex_config.rx.flag = calibPtr->bb->audio[calibItf].audioParams.NoiseSuppresserWithoutSpeechFlag;
    vpp_SpeechSetNoiseSuppressParaAll(&(calibPtr->audio_voc->voc[calibItf].speex));

    // agc_config.tx.flag = TRUE;
    // agc_config.rx.flag = FALSE;
    vpp_SpeechSetNonClipParaAll(&(calibPtr->audio_voc->voc[calibItf].agc));

    vpp_SpeechSetAecAllPara(&(calibPtr->audio_voc->voc[calibItf].aec));

    vpp_SpeechSetScaleDigitalGainPara(&(calibPtr->audio_voc->voc[calibItf].gain), audioCfg.spkLevel);

    vpp_SpeechSetHpfPara(&(calibPtr->audio_voc->voc[calibItf].hpf));

    vpp_SpeechSetEQPara(&(calibPtr->audio_voc->voc[calibItf].eq));

    vpp_SpeechSetMSDFPara(&(calibPtr->audio_voc->voc[calibItf].fir));

    vpp_SpeechSetWebRtcAgcParaAll(&(calibPtr->audio_voc->voc[calibItf].webrtcagc));

    vpp_SpeechSetInAlgGain(calibPtr->audio_voc->voc[calibItf].agc.tx.gain);
    vpp_SpeechSetOutAlgGain(calibPtr->audio_voc->voc[calibItf].agc.rx.gain);

    VOIS_TRACE(TSTDOUT, 0, "SETUP RxGain = %d, TxGain = %d",
               calibPtr->audio_voc->voc[calibItf].agc.tx.gain,
               calibPtr->audio_voc->voc[calibItf].agc.rx.gain);

#ifdef MAGIC_AE_SUPPORT
    vpp_SpeechSetMorphVoice(g_MorphVoiceEnableFlag,g_MorphVoicePara);
#endif

    // configure AUD and VPP buffers
    audErr = aud_StreamStart(itf, &g_voisPcmRxStream, &audioCfg);

    if (audErr == AUD_ERR_NO)
    {
        // Start the other flux only if the Rx one is started
        audErr = aud_StreamRecord(itf, &g_voisPcmTxStream, &audioCfg);
    }

    switch (audErr)
    {
        case AUD_ERR_NO:
	     #ifdef MODEM_WITHOUT_VOICE_FORGCF
            hal_VocClose();
            g_VoisRxToTXLoopFlag = TRUE;
	     #endif
            break;

        default:
            vpp_SpeechClose();
#ifdef SPEEH_RECORD_PCM_ENABLE
            COS_SHMEMFREE(g_pmallocHeadReceiver);
            COS_SHMEMFREE(g_pmallocHeadMic);
#endif
#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
            COS_SHMEMFREE(g_pmallocHeaddec);
#endif
            if (g_voisOverlayLoaded)
            {
                hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
                g_voisOverlayLoaded = FALSE;
            }
            VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
            VOIS_TRACE(TSTDOUT, 0, "VOIS Start Failed !!!");
            return VOIS_ERR_UNKNOWN;
            break; // :)
    }

    VOIS_PROFILE_FUNCTION_EXIT(vois_Start);
    g_voisCtx.voisStarted = TRUE;
    return VOIS_ERR_NO;
}



#ifdef SPEEH_RECORD_PCM_ENABLE
extern UINT32 ML_LocalLanguage2UnicodeBigEnding(const UINT8* in, UINT32 in_len, UINT8** out, UINT32* out_len,UINT8 nCharset[12]);
BOOL CSW_Free (  PVOID pMemBlock);
#endif

// =============================================================================
// vois_Stop
// -----------------------------------------------------------------------------
/// This function stops the VOIS service.
/// If the function returns
/// #VOIS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #VOIS_ERR_NO if it can execute the command.
// =============================================================================
PUBLIC VOIS_ERR_T vois_Stop(VOID)
{
    VOIS_PROFILE_FUNCTION_ENTRY(vois_Stop);
    VOIS_TRACE(_PAL| TLEVEL (1), 0, "VOIS Stop");

#ifdef SPEECH_NOISE_SUPPRESSER_MIC
    speex_preprocess_state_destroy(mic_den_p);
#endif


#ifdef SPEECH_NOISE_SUPPRESSER_SPEAKER
    speex_preprocess_state_destroy(speaker_den_p);

#endif


    DTMF_DetectStop();
    
    if (g_voisCtx.voisStarted)
    {
        // stop stream ...
        aud_StreamStop(g_voisCtx.voisItf);
        vpp_SpeechClose();
#ifndef CHIP_HAS_AP
        hal_SysSetVocClock(HAL_SYS_VOC_FREQ_78M);
#endif
        if (g_voisOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_voisOverlayLoaded = FALSE;
        }
        g_VoCRunFlag=0;

        g_VoisRxToTXLoopFlag = FALSE;

#ifdef SPEEH_RECORD_PCM_ENABLE
        {
            UINT8 SpkFileName[50] = "/t/speaker.pcm";
            UINT8 MicFileName[50] = "/t/mic.pcm";
            UINT8 *Spkuniname=NULL;
            UINT8 *Micuniname=NULL;
            UINT32 Spknamelen=0;
            UINT32 Micnamelen=0;

            ML_LocalLanguage2UnicodeBigEnding(SpkFileName, strlen(SpkFileName), &Spkuniname, &Spknamelen, NULL);
            ML_LocalLanguage2UnicodeBigEnding(MicFileName, strlen(MicFileName), &Micuniname, &Micnamelen, NULL);

            INT32 SpkFileHandle=FS_Open(Spkuniname, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);
            INT32 MicFileHandle=FS_Open(Micuniname, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);

            CSW_Free(Spkuniname);
            CSW_Free(Micuniname);

            VOIS_TRACE (TSTDOUT,0,"[vois_Stop]Spk File Handle:0x%x",SpkFileHandle);
            VOIS_TRACE (TSTDOUT,0,"[vois_Stop]Mic File Handle:0x%x",MicFileHandle);

            INT32 Spkresult=FS_Write(SpkFileHandle,g_pmallocHeadReceiver,450*1024);
            INT32 Micresult=FS_Write(MicFileHandle,g_pmallocHeadMic+(320)*2,450*1024);

            if(Spkresult != 450*1024)
            {
                VOIS_TRACE (TSTDOUT,0,"[vois_Stop_ERROR]spk write file error!\n");
            }
            if(Micresult != 450*1024)
            {
                VOIS_TRACE (TSTDOUT,0,"[vois_Stop_ERROR]mic write file error!\n");
            }
            FS_Close(SpkFileHandle);
            FS_Close(MicFileHandle);
            COS_SHMEMFREE(g_pmallocHeadReceiver);
            COS_SHMEMFREE(g_pmallocHeadMic);
        }
#endif // SPEEH_RECORD_PCM_ENABLE

#ifdef SPEEH_RECORD_DEC_DATA_ENABLE
        {
            UINT8 SpkFileName[50] = "/t/speaker.dat";
            UINT8 *Spkuniname=NULL;
            UINT32 Spknamelen=0;

            ML_LocalLanguage2UnicodeBigEnding(SpkFileName, strlen(SpkFileName), &Spkuniname, &Spknamelen, NULL);

            INT32 SpkFileHandle=FS_Open(Spkuniname, FS_O_RDWR | FS_O_CREAT | FS_O_TRUNC, 0);

            CSW_Free(Spkuniname);

            VOIS_TRACE (TSTDOUT,0,"[vois_Stop]Spk File Handle:0x%x",SpkFileHandle);

            FS_Write(SpkFileHandle,(UINT8 *)&g_voisCtx.voisTramePlayedNb,4);
            INT32 Spkresult=FS_Write(SpkFileHandle,g_pmallocHeaddec,TEST_BUFFER_LEN);

            if(Spkresult != TEST_BUFFER_LEN)
            {
                VOIS_TRACE (TSTDOUT,0,"[vois_Stop_ERROR]spk write file error!\n");
            }
            FS_Close(SpkFileHandle);
            COS_SHMEMFREE(g_pmallocHeaddec);
        }
#endif // SPEEH_RECORD_DEC_DATA_ENABLE

        // and reset global state and cl
        g_voisCtx.voisItf = AUD_ITF_NONE;

        g_voisCtx.voisTramePlayedNb = 0;

        g_voisCtx.voisAudioCfg.spkLevel = AUD_SPK_MUTE;
        g_voisCtx.voisAudioCfg.micLevel = AUD_MIC_MUTE;
        g_voisCtx.voisAudioCfg.sideLevel = 0;
        g_voisCtx.voisAudioCfg.toneLevel = 0;

        g_voisCtx.voisVppCfg.echoEsOn = 0;
        g_voisCtx.voisVppCfg.echoEsVad = 0;
        g_voisCtx.voisVppCfg.echoEsDtd = 0;
        g_voisCtx.voisVppCfg.echoExpRel = 0;
        g_voisCtx.voisVppCfg.echoExpMu = 0;
        g_voisCtx.voisVppCfg.echoExpMin = 0;
        g_voisCtx.voisVppCfg.encMute = VPP_SPEECH_MUTE;
        g_voisCtx.voisVppCfg.decMute = VPP_SPEECH_MUTE;
        g_voisCtx.voisVppCfg.sdf = NULL;
        g_voisCtx.voisVppCfg.mdf = NULL;
        g_voisCtx.voisVppCfg.if1 = 0;

        hal_SpeechFifoReset();
        g_voisCtx.voisStarted = FALSE;

        // PCM Audio Tx stream, output of VPP
        g_voisPcmRxStream.startAddress = NULL;
        g_voisPcmRxStream.length = 0;
        g_voisPcmRxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
        g_voisPcmRxStream.channelNb = HAL_AIF_MONO;
        g_voisPcmRxStream.voiceQuality = TRUE;
        g_voisPcmRxStream.playSyncWithRecord = TRUE;
        g_voisPcmRxStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
        g_voisPcmRxStream.endHandler = NULL; // Mechanical interaction with VPP's VOC

        // PCM Audio Rx stream, input of VPP
        g_voisPcmTxStream.startAddress = NULL;
        g_voisPcmTxStream.length = 0;
        g_voisPcmTxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
        g_voisPcmTxStream.channelNb = HAL_AIF_MONO;
        g_voisPcmTxStream.voiceQuality = TRUE;
        g_voisPcmTxStream.playSyncWithRecord = TRUE;
        g_voisPcmTxStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
        g_voisPcmTxStream.endHandler = NULL; // Mechanical interaction with VPP's VOC
    }


#if defined(CHIP_HAS_AP) && defined(VOIS_DUMP_PCM)
    if((g_voisSmartPhonePcmDumpFlag == TRUE) && (g_voisITFSwapFlag == FALSE))
    {
        if (g_voisUserHandler)
        {
            g_voisUserHandler(VOIS_STATUS_NO_MORE_DATA);
        }
    }
#else
    if((g_voisRecordFlag == TRUE) && (g_voisITFSwapFlag == FALSE))
    {
        if (g_voisUserHandler)
        {
            g_voisUserHandler(VOIS_STATUS_NO_MORE_DATA);
        }
    }
#endif

    /* this cause magic sound disabled when switch itf, this is a bug
       introduced in SVN 32279,
       just ignore this in vois_Stop ,do it in vois_clear_xf or vois_clear_ae
     */
#ifdef XF_MAGIC_SOUND_SUPPORT

    //XF_magic_enble=FALSE;

    if(pHeal)
    {
        mmc_MemFreeLast((char*)pHeal);
        pHeal=NULL;
    }

    /*
     xf_magic_mode[0]=ivMV_CHANGE_NONE;
      xf_magic_mode[1]=ivMV_ROLE_NONE;
      xf_magic_mode[2]=ivMV_VE_NONE;
      xf_magic_mode[3]=ivMV_EQID_NONE;
      xf_magic_mode[4]=ivMV_BGID_NONE;
     xf_magic_mode[5]=0;
      xf_magic_mode[6]=0;
    */

#endif


#ifdef JT_MAGIC_SOUND_SUPPORT
    //for jtAE.sheen
    //OPEN_JTAE=0;
    pJtAEHandle = 0;
    pPCMTempBuf=NULL;
    if(pJtAEInitBuf)
    {
        mmc_MemFreeLast((char*)pJtAEInitBuf);
        pJtAEInitBuf=NULL;
    }
    /*
    aeParam[0]=0;
    aeParam[1]=0;
    aeParam[2]=0;
    aeParam[3]=0;
    */
#endif

#ifdef MAGIC_AE_SUPPORT
  MorphVoice_Close();
#endif

   vois_MixDataStop();

   hal_SwReleaseClk(FOURCC_VOIS);
  //  COS_SHMEMFREE(g_voisRecordBufferStart);
  //  vois_RecordStop();

    VOIS_PROFILE_FUNCTION_EXIT(vois_Stop);
    return VOIS_ERR_NO;
}



// =============================================================================
// vois_CalibUpdateValues
// -----------------------------------------------------------------------------
/// Update Vois related values depending on calibration parameters.
///
/// @return #VOIS_ERR_RESOURCE_BUSY, if the driver is busy with another audio command,
///         #VOIS_ERR_NO otherwise
// =============================================================================
PUBLIC VOIS_ERR_T vois_CalibUpdateValues(VOID)
{
    VOIS_AUDIO_CFG_T cfg;
    cfg.spkLevel = g_voisCtx.voisLatestCfg.spkLevel;
    cfg.micLevel = g_voisCtx.voisLatestCfg.micLevel;
    cfg.sideLevel = g_voisCtx.voisLatestCfg.sideLevel;
    cfg.toneLevel = g_voisCtx.voisLatestCfg.toneLevel;
    cfg.encMute = g_voisCtx.voisLatestCfg.encMute;
    cfg.decMute = g_voisCtx.voisLatestCfg.decMute;

    // Only relevant would VoiS be started
    if (g_voisCtx.voisStarted)
    {
        return vois_Setup(g_voisCtx.voisItf, &cfg);
        // As vois_Setup does a aud_Setup, all settings
        // will be updated from the possibly new calibration
        // values.
    }
    else
    {
        return VOIS_ERR_NO;
    }
}


#ifdef XF_MAGIC_SOUND_SUPPORT


INT32 vois_set_xf( INT32 nAeID,  UINT8 iValue)
{
    //VOIS_TRACE(_PAL| TLEVEL (1), 0, "Vois set XF magic sound.\n");
    XF_magic_enble=TRUE;

    if(nAeID<=6)
    {
        xf_magic_mode[nAeID]=iValue;
    }

    return 0;
}


VOID vois_clear_xf(VOID)
{
    //VOIS_TRACE(_PAL| TLEVEL (1), 0, "Vois clear XF magic sound.\n");
    XF_magic_enble=FALSE;

    pMVHANDLE = NULL;

    xf_magic_mode[0]=ivMV_CHANGE_NONE;
    xf_magic_mode[1]=ivMV_ROLE_NONE;
    xf_magic_mode[2]=ivMV_VE_NONE;
    xf_magic_mode[3]=ivMV_EQID_NONE;
    xf_magic_mode[4]=ivMV_BGID_NONE;
    xf_magic_mode[5]=0;
    xf_magic_mode[6]=0;
    return;

}

#endif

#ifdef MAGIC_AE_SUPPORT

void vois_morph_set(  UINT8 iValue)
{
    //VOIS_TRACE(_PAL| TLEVEL (1), 0, "Vois set XF magic sound.\n");
    if(iValue == 0)
    {
        bMorphVoiceFlag=FALSE;
    }
    else
    {
  	  bMorphVoiceFlag=TRUE;
    
        if(iValue <= 6)
  	     MorphVoiceParam = iValue;
    }
	
 } 
 
VOID vois_morph_clear(VOID)
{
	//VOIS_TRACE(_PAL| TLEVEL (1), 0, "Vois clear XF magic sound.\n");
    bMorphVoiceFlag=FALSE;

    MorphVoiceParam = RDA_CHANGE_NONE;
}

#endif


#ifdef JT_MAGIC_SOUND_SUPPORT
//open audio effect and set param.sheen
INT32 vois_set_ae( INT32 nAeID,  INT32 iValue)
{
    OPEN_JTAE=1;
    aeParam[jtAE_PARAM_SAMPLE]=jtAE_SAMPLE_8K;
    aeParam[jtAE_PARAM_AUDIOEFFECT]=jtAE_AUDIOEFFECT_BASE;
    aeParam[jtAE_PARAM_TIMBRE]=jtAE_TIMBRE_BASE;
    aeParam[jtAE_PARAM_VOLUME]=0;//-32768 ~32767

    switch(nAeID)
    {
        case jtAE_PARAM_SAMPLE:
            if(iValue!=jtAE_SAMPLE_8K || iValue!=jtAE_SAMPLE_16K)
            {
                VOIS_TRACE(_PAL| TLEVEL (1), 0, "vois_set_ae invalid jtAE_PARAM_SAMPLE iValue=%d",iValue);
                return -1;
            }
            aeParam[jtAE_PARAM_SAMPLE]=jtAE_SAMPLE_8K;//only 8k
            break;
        case jtAE_PARAM_AUDIOEFFECT:
            if(iValue < jtAE_AUDIOEFFECT_BASE || iValue > jtAE_AUDIOEFFECT_ROBOT)
            {
                VOIS_TRACE(_PAL| TLEVEL (1), 0, "vois_set_ae invalid jtAE_PARAM_AUDIOEFFECT iValue=%d",iValue);
                return -1;
            }
            aeParam[jtAE_PARAM_AUDIOEFFECT]=iValue;
            break;
        case jtAE_PARAM_TIMBRE:
            if(iValue < jtAE_TIMBRE_BASE || iValue > jtAE_TIMBRE_CHILD)
            {
                VOIS_TRACE(_PAL| TLEVEL (1), 0, "vois_set_ae invalid jtAE_PARAM_TIMBRE iValue=%d",iValue);
                return -1;
            }
            aeParam[jtAE_PARAM_TIMBRE]=iValue;
            break;
        case jtAE_PARAM_VOLUME:
            if(iValue < -32768 || iValue > 32767)
            {
                VOIS_TRACE(_PAL| TLEVEL (1), 0, "vois_set_ae invalid jtAE_PARAM_VOLUME iValue=%d",iValue);
                return -1;
            }
            aeParam[jtAE_PARAM_VOLUME]=iValue;
            break;
        default:
            VOIS_TRACE(_PAL| TLEVEL (1), 0, "vois_set_ae invalid nAeID=%d",nAeID);
            return -1;
    }

    if(iValue==0)//no need AE.
        OPEN_JTAE=0;

    VOIS_TRACE(_PAL| TLEVEL (1), 0, "sheen: vois_set_ae iValue=%d  nAeID=%d",iValue, nAeID);
    return 0;
}

VOID vois_clear_ae(VOID)
{
    OPEN_JTAE=0;

    pJtAEHandle = 0;
    pPCMTempBuf=NULL;

    aeParam[0]=0;
    aeParam[1]=0;
    aeParam[2]=0;
    aeParam[3]=0;

    VOIS_TRACE(_PAL| TLEVEL (1), 0, "sheen: vois_clear_ae ");
}
#endif

// =============================================================================
// vois_RecordStart
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOIS_ERR_T vois_RecordStart( INT32 *startAddress,INT32 length,VOIS_USER_HANDLER_T handler)
{

    VOIS_TRACE(TSTDOUT, 0, "vois_RecordStart,startAddress:0x%x,length:%d,handler:0x%x",startAddress,length,handler);

#if defined(CHIP_HAS_AP) && defined(VOIS_DUMP_PCM)
    if(length%1944!=0||startAddress==NULL||length<1944)//must be the multples of two speech frames:3*160*2(frame+4)*2Byte/sample=1944Bytes
#else
    if(length%640!=0||startAddress==NULL||length<640)//must be the multples of two speech frames:160*2frame*2Byte/sample=640Bytes
#endif
    {
        VOIS_TRACE(TSTDOUT, 0, "[VOIS]Parameter error!");
        return VOIS_ERR_BAD_PARAMETER;
    }

    g_voisUserHandler=handler;

    g_voisBufferLength=length;

    g_voisBufferStart = startAddress;
    g_voisBufferTop = startAddress+(length/4);
    g_voisBufferCurPos =startAddress;

#if defined(CHIP_HAS_AP) && defined(VOIS_DUMP_PCM)
    g_voisSmartPhonePcmDumpFlag = TRUE;
#else
    g_voisRecordFlag = TRUE;
#endif


    return VOIS_ERR_NO;
}

// =============================================================================
// vois_RecordStop
// -----------------------------------------------------------------------------
// =============================================================================
PUBLIC VOIS_ERR_T vois_RecordStop( VOID)
{

    VOIS_TRACE(TSTDOUT, 0, "vois_RecordStop");
#if defined(CHIP_HAS_AP) && defined(VOIS_DUMP_PCM)
    g_voisSmartPhonePcmDumpFlag = FALSE;
#else
    g_voisRecordFlag = FALSE;
#endif
    g_voisITFSwapFlag = FALSE;

    g_voisBufferLength=0;

    g_voisUserHandler=NULL;

    g_voisBufferStart = NULL;
    g_voisBufferTop = NULL;
    g_voisBufferCurPos =NULL;

    return VOIS_ERR_NO;
}

PUBLIC VOID vois_SetRxToTxLoopFlag( BOOL flag)
{

    g_VoisRxToTXLoopFlag = flag;
}


// =============================================================================
// vois_GetLatestCfg
// -----------------------------------------------------------------------------
/// get the vois Latest config.
// =============================================================================
PUBLIC VOIS_AUDIO_CFG_T* vois_GetAudioCfg(VOID)
{
    return ( (VOIS_AUDIO_CFG_T*)&g_voisCtx.voisAudioCfg);
}

PUBLIC VOID vois_SetMsdfReloadFlag(VOID)
{
    g_msdfReload = 1;
}

// delay buffer 400ms(20 frames)
PRIVATE VOID vois_LoopTestCopyDataInit(VOID)
{
    if (g_LoopTestDelayBuf == NULL)
    {
        g_LoopTestDelayBuf = COS_MALLOC(kLoopTestDelayNum * sizeof(HAL_SPEECH_PCM_HALF_BUF_T));
    }
    if (g_LoopTestDelayBuf == NULL)
    {
        g_VoisLoopTestFlag = FALSE;
        VOIS_TRACE(TSTDOUT, 0, "hal_VoisLoopTest Not enough memory for delay buffer");
    }
    else
    {
        g_VoisLoopTestFlag = TRUE;
        memset(g_LoopTestDelayBuf, 0, kLoopTestDelayNum * sizeof(HAL_SPEECH_PCM_HALF_BUF_T));
        VOIS_TRACE(TSTDOUT, 0, "hal_VoisLoopTest g_LoopTestDelayBuf = 0x%x", g_LoopTestDelayBuf);
    }
}

PRIVATE VOID vois_LoopTestCopyDataDestory(VOID)
{
    if (g_LoopTestDelayBuf)
    {
        COS_FREE(g_LoopTestDelayBuf);
        g_LoopTestDelayBuf = NULL;
    }
}
// =============================================================================
// vois_LoopTestCopyData
// -----------------------------------------------------------------------------
/// Loop between Tx and Rx shortcutting the Speech Fifo.
// =============================================================================
PRIVATE VOID vois_LoopTestCopyData(HAL_SPEECH_DEC_IN_T* popedRx,
                                   CONST HAL_SPEECH_ENC_OUT_T* pushedTx)
{
    UINT32 i;

    popedRx->dtxOn = 0;
    popedRx->codecMode = g_voisCtx.mode;
    popedRx->decFrameType = 0;
    popedRx->bfi = 0;
    popedRx->sid = 0;
    popedRx->taf = 0;
    popedRx->ufi = 0;

    for (i=0; i< g_voisCtx.bufferSize/4 ; i++)
    {
        *((UINT32*)popedRx->decInBuf + i) = *((UINT32*)(g_LoopTestDelayBuf + kLoopTestDelayNum - 1) + i);
    }

    // move 0-18 to 1-19
    memmove(g_LoopTestDelayBuf + 1, g_LoopTestDelayBuf,
            (kLoopTestDelayNum - 1) * sizeof(HAL_SPEECH_PCM_HALF_BUF_T));

    // save new encOutBuf
    for (i=0; i< g_voisCtx.bufferSize/4 ; i++)
    {
        *((UINT32*)g_LoopTestDelayBuf+ i) = *((UINT32*)pushedTx->encOutBuf + i);
    }
}

// =============================================================================
// vois_LoopTestHandler
// -----------------------------------------------------------------------------
/// Handler for the vpp module, for speech-kind encoded streams.
/// In charge of copying new datas, dealing with
/// underflow (no more samples), pause and resume etc ... It is called
/// every time a trame has been decoded by VPP.
// =============================================================================
PRIVATE VOID vois_LoopTestHandler(HAL_VOC_IRQ_STATUS_T* status)
{
    if(vpp_SpeechGetType()==1)     //For processing the mic signal.(for example "noise suppressing","magic sound" etc.)
    {
        vpp_SpeechScheduleOneFrame(VPP_SPEECH_WAKEUP_SW_ENC);
        return;
    }

    g_VoCRunFlag=0; //It must be after "mic processing".Beacause the VoC must be stopped in right status.

    if (g_voisCtx.voisTramePlayedNb == VOIS_VPP_UNMUTE_TRAME_NB)
    {
        // Unmute VPP
        vpp_SpeechAudioCfg((VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);
    }

    // Copy the buffer to decode (Rx) and fetch from VPP's memory
    // the freshly encoded buffer (Tx).
    if(g_VoisLoopTestFlag)
        vois_LoopTestCopyData(g_voisCtx.voisDecInput, g_voisCtx.voisEncOutput);
    g_voisCtx.voisTramePlayedNb++;

    return;
}

// =============================================================================
// vois_LoopTestSetup
// -----------------------------------------------------------------------------
/// Configure the LoopTest..
///
/// @param itf Interface number of the interface to setup.
/// @param cfg The configuration set applied to the audio interface. See
/// #VOIS_AUDIO_CFG_T for more details.
/// @return #VOIS_ERR_NO it can execute the configuration.
// =============================================================================

PRIVATE VOIS_ERR_T vois_LoopTestSetup(AUD_ITF_T itf, CONST VOIS_AUDIO_CFG_T* cfg)
{

    AUD_ERR_T audErr = AUD_ERR_NO;

    // FIXME : All calibration to be checked
    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    VOIS_TRACE(TSTDOUT, 0, "vois_LoopTestSetup");

    // Check that VOIS is running
    if (g_voisCtx.voisItf == AUD_ITF_NONE || !g_voisCtx.voisStarted)
    {
        // Ignore this call
        return VOIS_ERR_NO;
    }

#ifndef VOC_INTR_STEP
    UINT32 Wakeup_Mask_Status =  hal_VocIntrClose();
    if(Wakeup_Mask_Status==0xffffffff)
        return VOIS_ERR_NO;
#else
#ifdef __BT_PCM_OVER_UART__
    //do null
#else
    UINT32 now,voc_time;
    now = hal_TimGetUpTime();

    while(! g_VoCRunFlag)
    {
        voc_time = hal_TimGetUpTime();
        if (voc_time - now >(16384>>2))
        {
            g_VoCRunFlag=1;
        }
    }
    now = hal_TimGetUpTime();

    while( (g_VoCRunFlag==1)&&(g_voisCtx.voisStarted==TRUE))
    {
        voc_time = hal_TimGetUpTime();
        if (voc_time - now > 16384)
        {
            g_VoCRunFlag=0;
            hal_HstSendEvent(0xaaaa9999);
        }
    }
#endif
#endif
    g_SpkLevel=cfg->spkLevel;

    g_AudInterface=itf;

    // Record the last applied configuration
    g_voisCtx.voisLatestCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisLatestCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisLatestCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisLatestCfg.toneLevel = cfg->toneLevel;
    g_voisCtx.voisLatestCfg.encMute = cfg->encMute;
    g_voisCtx.voisLatestCfg.decMute = cfg->decMute;

    // Those parameters are not calibration dependent.
    g_voisCtx.voisVppCfg.encMute = cfg->encMute;
    g_voisCtx.voisVppCfg.decMute = cfg->decMute;

    // disable push-to-talk
    g_voisCtx.voisVppCfg.if1 = 0;

    // Set audio interface
    g_voisCtx.voisAudioCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisAudioCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisAudioCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisAudioCfg.toneLevel = cfg->toneLevel;

    // get calibration params for Vpp Speech. Need to be called after setting g_voisCtx
    vois_SetCalibratedVppCfg(itf, (VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);

    // Just apply the configuration change on the same
    // interface.
    vpp_SpeechAudioCfg((VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);

    audErr = aud_Setup(itf, (AUD_LEVEL_T*)&g_voisCtx.voisAudioCfg);

    //Set Mic Gain in VoC

    if (cfg->encMute == VPP_SPEECH_MUTE || cfg->micLevel == SND_MIC_MUTE)
    {
        vpp_SpeechSetInAlgGain(CALIB_AUDIO_GAIN_VALUE_MUTE);
    }
    else
    {
        CALIB_AUDIO_IN_GAINS_T inGains;
        INT32 algVol;

        if (cfg->micLevel == SND_MIC_VOL_CALL)
        {
            inGains = calibPtr->bb->audio[itf].audioGains.inGainsCall;
        }
        else
        {
            inGains = calibPtr->bb->audio[itf].audioGains.inGainsRecord;
        }
        algVol = inGains.alg;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 && !defined(CHIP_DIE_8810E))
        if (hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID) < HAL_SYS_CHIP_METAL_ID_U09 &&
                inGains.adc > -8)
        {
            // Compensate the ADC gain which is lowered in aud_CodecCommonSetMicCfg()
            algVol += inGains.adc + 8 + 1;
        }
#endif

        vpp_SpeechSetInAlgGain(algVol);
    }

    vpp_SpeechSetAecEnableFlag(calibPtr->bb->audio[itf].audioParams.AecEnbleFlag,
                               calibPtr->bb->audio[itf].audioParams.AgcEnbleFlag);

    vpp_SpeechSetAecPara(calibPtr->bb->audio[itf].audioParams.NoiseGainLimit,
                         calibPtr->bb->audio[itf].audioParams.NoiseMin,
                         calibPtr->bb->audio[itf].audioParams.NoiseGainLimitStep,
                         calibPtr->bb->audio[itf].audioParams.AmpThr,
                         calibPtr->bb->audio[itf].audioParams.StrongEchoFlag);

    vpp_SpeechSetFilterPara(calibPtr->bb->audio[itf].audioParams.HighPassFilterFlag,
                            calibPtr->bb->audio[itf].audioParams.NotchFilterFlag,
                            calibPtr->bb->audio[itf].audioParams.NoiseSuppresserFlag,
                            calibPtr->bb->audio[itf].audioParams.NoiseSuppresserWithoutSpeechFlag);

    vpp_SpeechSetProcessMicEnableFlag(TRUE);

    vpp_SpeechSetNoiseSuppressParaAll(&(calibPtr->audio_voc->voc[itf].speex));

    vpp_SpeechSetNonClipParaAll(&(calibPtr->audio_voc->voc[itf].agc));

    vpp_SpeechSetAecAllPara(&(calibPtr->audio_voc->voc[itf].aec));

    vpp_SpeechSetScaleDigitalGainPara(&(calibPtr->audio_voc->voc[itf].gain), g_SpkLevel);

    vpp_SpeechSetHpfPara(&(calibPtr->audio_voc->voc[itf].hpf));

    vpp_SpeechSetEQPara(&(calibPtr->audio_voc->voc[itf].eq));

    vpp_SpeechSetMSDFPara(&(calibPtr->audio_voc->voc[itf].fir));

    vpp_SpeechSetWebRtcAgcParaAll(&(calibPtr->audio_voc->voc[itf].webrtcagc));

    vpp_SpeechSetInAlgGain(calibPtr->audio_voc->voc[itf].agc.tx.gain);
    vpp_SpeechSetOutAlgGain(calibPtr->audio_voc->voc[itf].agc.rx.gain);

    VOIS_TRACE(TSTDOUT, 0, "SETUP RxGain = %d, TxGain = %d",
               calibPtr->audio_voc->voc[itf].agc.tx.gain,
               calibPtr->audio_voc->voc[itf].agc.rx.gain);

#ifndef VOC_INTR_STEP
    hal_VocIntrOpen(Wakeup_Mask_Status);
#endif

    switch (audErr)
    {
        case AUD_ERR_NO:

            return VOIS_ERR_NO;
            break;

        default:

            return audErr; // ...unknown error ...
            break;
    }
}

// =============================================================================
// vois_LoopTestStart
// -----------------------------------------------------------------------------
/// Start the loop test
///
/// @param itf Interface providing the audion input and output.
/// @param cfg The configuration set applied to the audio interface
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio
/// command, or if VPP is unavailable.
///         #VOIS_ERR_NO it can execute the command.
// =============================================================================
PRIVATE VOIS_ERR_T vois_LoopTestStart(
    CONST AUD_ITF_T      itf,
    CONST VOIS_AUDIO_CFG_T* cfg)
{
    VOIS_TRACE(_PAL| TLEVEL (1), 0, "VOIS LoopTestStart");

    // Initial audio confguration
    AUD_LEVEL_T audioCfg;
    VPP_SPEECH_AUDIO_CFG_T vppCfg;
    AUD_ERR_T audErr;

    CALIB_CALIBRATION_T* calibPtr = calib_GetPointers();

    // if (calibPtr->bb->audio[itf].audioParams.AecEnbleFlag)
    {
        if (hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_5) != HAL_ERR_NO)
        {
            return VOIS_ERR_RESOURCE_BUSY;
        }
        g_voisOverlayLoaded = TRUE;
    }


    hal_SwRequestClk(FOURCC_VOIS, HAL_CLK_RATE_104M);

#ifndef CHIP_HAS_AP
    hal_SysSetVocClock(HAL_SYS_VOC_FREQ_178M);
#endif
    //  g_AGCEnbleFlag = FALSE;
    g_SpkLevel=cfg->spkLevel;
    g_AudInterface=itf;

    // Record the last applied configuration
    g_voisCtx.voisLatestCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisLatestCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisLatestCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisLatestCfg.toneLevel = cfg->toneLevel;
    g_voisCtx.voisLatestCfg.encMute = cfg->encMute;
    g_voisCtx.voisLatestCfg.decMute = cfg->decMute;

    // Configure the Speech FIFO
    hal_SpeechFifoReset();

    g_voisCtx.voisVppCfg.encMute = cfg->encMute;
    g_voisCtx.voisVppCfg.decMute = cfg->decMute;

    // disable push-to-talk
    g_voisCtx.voisVppCfg.if1 = 0;

    // Set audio interface
    g_voisCtx.voisAudioCfg.spkLevel = cfg->spkLevel;
    g_voisCtx.voisAudioCfg.micLevel = cfg->micLevel;
    g_voisCtx.voisAudioCfg.sideLevel = cfg->sideLevel;
    g_voisCtx.voisAudioCfg.toneLevel = cfg->toneLevel;

    // get calibration params for Vpp Speech. Need to be called after setting g_voisCtx
    vois_SetCalibratedVppCfg(itf, (VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg);
    // Register global var
    g_voisCtx.voisItf = itf;

    g_voisCtx.voisTramePlayedNb = 0;

    // Set audio interface, initially muted
    *(AUD_LEVEL_T*)&audioCfg = *(AUD_LEVEL_T*)&g_voisCtx.voisAudioCfg;
    audioCfg.spkLevel   =  cfg->spkLevel;
    audioCfg.micLevel   =  cfg->micLevel;
    audioCfg.sideLevel  =  cfg->sideLevel;
    audioCfg.toneLevel  =  cfg->toneLevel;

    // Set the VPP config
    *(VPP_SPEECH_AUDIO_CFG_T*)&vppCfg = *(VPP_SPEECH_AUDIO_CFG_T*)&g_voisCtx.voisVppCfg;
    vppCfg.encMute = VPP_SPEECH_MUTE;
    vppCfg.decMute = VPP_SPEECH_MUTE;

    g_SpeakerPcmBuffer=(INT16 *)((UINT32)( vpp_SpeechGetRxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechGetRxPcmBuffer();
    g_MicPcmBuffer=(INT16 *)((UINT32)(vpp_SpeechGetTxPcmBuffer())|0x20000000);//(UINT8 *)vpp_SpeechGetTxPcmBuffer();

    // Those buffer are written by VoC, thus the need to access
    // them the uncached way.
    g_voisCtx.voisEncOutput = (HAL_SPEECH_ENC_OUT_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)vpp_SpeechGetTxCodBuffer());
    g_voisCtx.voisDecInput  = (HAL_SPEECH_DEC_IN_T*) HAL_SYS_GET_UNCACHED_ADDR((UINT32)vpp_SpeechGetRxCodBuffer());

    vois_LoopTestCopyDataInit();

    // PCM Audio Tx stream, output of VPP
#ifndef SMALL_BSS_RAM_SIZE
    g_voisPcmRxStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
#endif
    g_voisPcmRxStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
    g_voisPcmRxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    g_voisPcmRxStream.channelNb = HAL_AIF_MONO;
    g_voisPcmRxStream.voiceQuality = TRUE;
    g_voisPcmRxStream.playSyncWithRecord = TRUE;
    g_voisPcmRxStream.halfHandler =  NULL ; // Mechanical interaction with VPP's VOC
    g_voisPcmRxStream.endHandler =  NULL ; // Mechanical interaction with VPP's VOC

#ifndef SMALL_BSS_RAM_SIZE
    // PCM Audio Rx stream, input of VPP
    g_voisPcmTxStream.startAddress = (UINT32*)vpp_SpeechGetTxPcmBuffer();

#endif // SMALL_BSS_RAM_SIZE

    g_voisPcmTxStream.length = sizeof(HAL_SPEECH_PCM_BUF_T);
    g_voisPcmTxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
    g_voisPcmTxStream.channelNb = HAL_AIF_MONO;
    g_voisPcmTxStream.voiceQuality = TRUE;
    g_voisPcmTxStream.playSyncWithRecord = TRUE;
    g_voisPcmTxStream.halfHandler = txhandle_half ; // Mechanical interaction with VPP's VOC
    g_voisPcmTxStream.endHandler = txhandle_end ; // Mechanical interaction with VPP's VOC_AHB_BCPU_DEBUG_IRQ_CAUSE

    UINT32 spStatus =    vpp_SpeechOpen(vois_LoopTestHandler, VPP_SPEECH_WAKEUP_SW_DECENC);

#ifdef SMALL_BSS_RAM_SIZE
    // PCM Audio Rx stream, input of VPP
    g_voisPcmRxStream.startAddress = (UINT32*)vpp_SpeechGetRxPcmBuffer();
    g_voisPcmTxStream.startAddress = (UINT32*)vpp_SpeechGetTxPcmBuffer();

#endif // SMALL_BSS_RAM_SIZE

    VOIS_TRACE(TSTDOUT, 0, "VPP_SPEECH_OPEN STATUS = %08X", spStatus);
    if (spStatus == HAL_ERR_RESOURCE_BUSY)
    {
        if (g_voisOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_voisOverlayLoaded = FALSE;
        }

        return VOIS_ERR_RESOURCE_BUSY;
    }
    vpp_SpeechAudioCfg(&vppCfg);

    memset(g_voisPcmRxStream.startAddress,0,g_voisPcmRxStream.length);

    CALIB_AUDIO_IN_GAINS_T inGains;
    INT32 algVol;

    if (cfg->micLevel == SND_MIC_VOL_CALL)
    {
        inGains = calibPtr->bb->audio[itf].audioGains.inGainsCall;
    }
    else
    {
        inGains = calibPtr->bb->audio[itf].audioGains.inGainsRecord;
    }
    algVol = inGains.alg;

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810 && !defined(CHIP_DIE_8810E))
    if (hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID) < HAL_SYS_CHIP_METAL_ID_U09 &&
            inGains.adc > -8)
    {
        // Compensate the ADC gain which is lowered in aud_CodecCommonSetMicCfg()
        algVol += inGains.adc + 8 + 1;
    }
#endif

    vpp_SpeechSetInAlgGain(algVol);

    vpp_SpeechSetAecEnableFlag(calibPtr->bb->audio[itf].audioParams.AecEnbleFlag,
                               calibPtr->bb->audio[itf].audioParams.AgcEnbleFlag);

    vpp_SpeechSetAecPara(calibPtr->bb->audio[itf].audioParams.NoiseGainLimit,
                         calibPtr->bb->audio[itf].audioParams.NoiseMin,
                         calibPtr->bb->audio[itf].audioParams.NoiseGainLimitStep,
                         calibPtr->bb->audio[itf].audioParams.AmpThr,
                         calibPtr->bb->audio[itf].audioParams.StrongEchoFlag);

    vpp_SpeechSetFilterPara(calibPtr->bb->audio[itf].audioParams.HighPassFilterFlag,
                            calibPtr->bb->audio[itf].audioParams.NotchFilterFlag,
                            calibPtr->bb->audio[itf].audioParams.NoiseSuppresserFlag,
                            calibPtr->bb->audio[itf].audioParams.NoiseSuppresserWithoutSpeechFlag);

    vpp_SpeechSetProcessMicEnableFlag(TRUE);

    vpp_SpeechSetNoiseSuppressParaAll(&(calibPtr->audio_voc->voc[itf].speex));

    vpp_SpeechSetNonClipParaAll(&(calibPtr->audio_voc->voc[itf].agc));

    vpp_SpeechSetAecAllPara(&(calibPtr->audio_voc->voc[itf].aec));

    vpp_SpeechSetScaleDigitalGainPara(&(calibPtr->audio_voc->voc[itf].gain), audioCfg.spkLevel);

    vpp_SpeechSetHpfPara(&(calibPtr->audio_voc->voc[itf].hpf));

    vpp_SpeechSetEQPara(&(calibPtr->audio_voc->voc[itf].eq));

    vpp_SpeechSetMSDFPara(&(calibPtr->audio_voc->voc[itf].fir));

    vpp_SpeechSetWebRtcAgcParaAll(&(calibPtr->audio_voc->voc[itf].webrtcagc));

    vpp_SpeechSetInAlgGain(calibPtr->audio_voc->voc[itf].agc.tx.gain);
    vpp_SpeechSetOutAlgGain(calibPtr->audio_voc->voc[itf].agc.rx.gain);

    VOIS_TRACE(TSTDOUT, 0, "SETUP RxGain = %d, TxGain = %d",
               calibPtr->audio_voc->voc[itf].agc.tx.gain,
               calibPtr->audio_voc->voc[itf].agc.rx.gain);

#ifdef MAGIC_AE_SUPPORT
    vpp_SpeechSetMorphVoice(g_MorphVoiceEnableFlag,g_MorphVoicePara);
#endif

    // configure AUD and VPP buffers

    audErr = aud_StreamStart(itf, &g_voisPcmRxStream, &audioCfg);

    if (audErr == AUD_ERR_NO)
    {
        // Start the other flux only if the Rx one is started
        audErr = aud_StreamRecord(itf, &g_voisPcmTxStream, &audioCfg);
    }

    switch (audErr)
    {
        case AUD_ERR_NO:
            break;

        default:
            vpp_SpeechClose();
            if (g_voisOverlayLoaded)
            {
                hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
                g_voisOverlayLoaded = FALSE;
            }

            VOIS_TRACE(TSTDOUT, 0, "VOIS LoopTestStart Failed !!!");
            return VOIS_ERR_UNKNOWN;
            break; // :)
    }

    //
    g_voisCtx.voisStarted = TRUE;
    return VOIS_ERR_NO;

}

// =============================================================================
// vois_LoopTestStop
// -----------------------------------------------------------------------------
/// This function stops the VOIS looptest.
/// If the function returns
/// #VOIS_ERR_RESOURCE_BUSY, it means that the driver is busy with another
/// audio command.
///
/// @return #VOIS_ERR_RESOURCE_BUSY when the driver is busy with another audio command,
///         #VOIS_ERR_NO if it can execute the command.
// =============================================================================
PRIVATE VOIS_ERR_T vois_LoopTestStop(VOID)
{
    VOIS_TRACE(_PAL| TLEVEL (1), 0, "VOIS LoopTestStop");

    if (g_voisCtx.voisStarted)
    {
        // stop stream ...
        aud_StreamStop(g_voisCtx.voisItf);
        vpp_SpeechClose();
#ifndef CHIP_HAS_AP
        hal_SysSetVocClock(HAL_SYS_VOC_FREQ_78M);
#endif
        if (g_voisOverlayLoaded)
        {
            hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_5);
            g_voisOverlayLoaded = FALSE;
        }
        g_VoCRunFlag=0;

        // and reset global state and cl
        g_voisCtx.voisItf = AUD_ITF_NONE;

        g_voisCtx.voisTramePlayedNb = 0;

        g_voisCtx.voisAudioCfg.spkLevel = AUD_SPK_MUTE;
        g_voisCtx.voisAudioCfg.micLevel = AUD_MIC_MUTE;
        g_voisCtx.voisAudioCfg.sideLevel = 0;
        g_voisCtx.voisAudioCfg.toneLevel = 0;

        g_voisCtx.voisVppCfg.echoEsOn = 0;
        g_voisCtx.voisVppCfg.echoEsVad = 0;
        g_voisCtx.voisVppCfg.echoEsDtd = 0;
        g_voisCtx.voisVppCfg.echoExpRel = 0;
        g_voisCtx.voisVppCfg.echoExpMu = 0;
        g_voisCtx.voisVppCfg.echoExpMin = 0;
        g_voisCtx.voisVppCfg.encMute = VPP_SPEECH_MUTE;
        g_voisCtx.voisVppCfg.decMute = VPP_SPEECH_MUTE;
        g_voisCtx.voisVppCfg.sdf = NULL;
        g_voisCtx.voisVppCfg.mdf = NULL;
        g_voisCtx.voisVppCfg.if1 = 0;

        hal_SpeechFifoReset();
        g_voisCtx.voisStarted = FALSE;

        // PCM Audio Tx stream, output of VPP
        g_voisPcmRxStream.startAddress = NULL;
        g_voisPcmRxStream.length = 0;
        g_voisPcmRxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
        g_voisPcmRxStream.channelNb = HAL_AIF_MONO;
        g_voisPcmRxStream.voiceQuality = TRUE;
        g_voisPcmRxStream.playSyncWithRecord = TRUE;
        g_voisPcmRxStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
        g_voisPcmRxStream.endHandler = NULL; // Mechanical interaction with VPP's VOC

        // PCM Audio Rx stream, input of VPP
        g_voisPcmTxStream.startAddress = NULL;
        g_voisPcmTxStream.length = 0;
        g_voisPcmTxStream.sampleRate = HAL_AIF_FREQ_8000HZ;
        g_voisPcmTxStream.channelNb = HAL_AIF_MONO;
        g_voisPcmTxStream.voiceQuality = TRUE;
        g_voisPcmTxStream.playSyncWithRecord = TRUE;
        g_voisPcmTxStream.halfHandler = NULL; // Mechanical interaction with VPP's VOC
        g_voisPcmTxStream.endHandler = NULL; // Mechanical interaction with VPP's VOC
    }

    vois_LoopTestCopyDataDestory();

    hal_SwReleaseClk(FOURCC_VOIS);

    return VOIS_ERR_NO;
}

// =============================================================================
// vois_TestModeSetup
// -----------------------------------------------------------------------------
/// Enable a test mode.
/// This function enables a test mode, enabling for example the side test
/// or the DAIs.
/// The audio interface must be enabled when this function is called.
/// This function disables the AIF (audio interface), sets the audio
/// interface in the mode specified, and re-enables the AIF.
/// See the <B> Application Note 0018 </B> for more details on the DAI
/// functionality and see 3GPP TS 44014-500 page 40 for the description of the
/// DAI test interface.
///
/// @param itf AUD interface to test.
/// @param stream Depending on the test, this parameter can be ignored, or
/// used to configure it. (This is the case for the side test). This
/// sets things relative to the streaming (sampling rate, etc)
/// @param cfg Depending on the test, this parameter can be ignored, or
/// used to configure it. (This is the case for the side test). Configure
/// the audio interface.
/// @param mode Test mode to set.
/// @return #VOIS_ERR_NO if it succeeds.
// =============================================================================
PUBLIC AUD_ERR_T vois_TestModeSetup(CONST SND_ITF_T         itf,
                                    CONST HAL_AIF_STREAM_T* stream,
                                    CONST AUD_LEVEL_T *     cfg,
                                    AUD_TEST_T              mode)
{
    VOIS_AUDIO_CFG_T voisCfg;
    switch (mode)
    {
        case AUD_TEST_SIDE_TEST:
        case AUD_TEST_RECVMIC_IN_EARPIECE_OUT:
        {
            if (g_voisRunningTest != AUD_TEST_NO)
            {
                return VOIS_ERR_RESOURCE_BUSY;
            }

            if (mode == AUD_TEST_RECVMIC_IN_EARPIECE_OUT &&
                    itf != SND_ITF_EAR_PIECE)
            {
                return VOIS_ERR_BAD_PARAMETER;
            }

            voisCfg.spkLevel = cfg->spkLevel;
            voisCfg.micLevel = cfg->micLevel;
            voisCfg.sideLevel = AUD_SIDE_MUTE;
            voisCfg.toneLevel = cfg->toneLevel;
            voisCfg.encMute= VPP_SPEECH_UNMUTE;
            voisCfg.decMute= VPP_SPEECH_UNMUTE;

            while (vois_LoopTestSetup(itf, &voisCfg) != VOIS_ERR_NO)
            {
                sxr_Sleep(1 MS_WAITING);
            }

            while (vois_LoopTestStart(itf, &voisCfg) != VOIS_ERR_NO)
            {
                sxr_Sleep(1 MS_WAITING);
            }

            while (vois_LoopTestSetup(itf, &voisCfg) != VOIS_ERR_NO)
            {
                sxr_Sleep(1 MS_WAITING);
            }

            g_voisRunningTest = mode;
            break;
        }

        case AUD_TEST_NO:
            switch (g_voisRunningTest)
            {
                case AUD_TEST_SIDE_TEST:
                case AUD_TEST_RECVMIC_IN_EARPIECE_OUT:
                    while( vois_LoopTestStop() != AUD_ERR_NO)
                    {
                        sxr_Sleep(1 MS_WAITING);
                    }

                    g_voisRunningTest = AUD_TEST_NO;
                    break;

                default:
                    break;
            }

        default:
            break;
    }

    return VOIS_ERR_NO;
}

PUBLIC void vois_SetMixBuffer(UINT32 *buffer, UINT32 buf_len, VOIS_USER_HANDLER_T handler)
{
    g_VoisMixBufferStart                     = buffer;
    g_VoisMixBufferTop                       = buffer + buf_len/4;
    g_VoisMixBufferCurPos                  = buffer;
    g_VoisMixBufferRemainingBytes    = 0;
    g_VoisMixBufferLength                   = buf_len;
    g_VoisMixFinishData = FALSE;
    g_MIXVOICE_Handler = handler;

}


PUBLIC UINT32* vois_GetMixBufPosition(VOID)
{
    return (g_VoisMixBufferCurPos);
}


PRIVATE void vois_MixSpeechLine(INT16 *voisbuffer,  INT16 *mixbuff, UINT32 buf_len)
{
    int i;
    UINT32 count = buf_len/sizeof(INT16);
    INT32 buffsum;

     for(i=0; i<count; i++)
     {
       buffsum =(INT32) (voisbuffer[i]) +(INT32)(mixbuff[i]);//sxr_Random (1000)
	   voisbuffer[i] =  buffsum/2;
     }
}

PRIVATE void vois_MixSpeechNormal(INT16 *voisbuffer,  INT16 *mixbuff, UINT32 buf_len)
{
     int i;
     UINT32 count = buf_len/sizeof(INT16);
     INT32 buffsum;
     static  UINT32 parame  = 4096;

     for(i=0; i<count; i++)
     {
          buffsum =(INT32) (voisbuffer[i]) + (INT32)(mixbuff[i]);
	   buffsum = (buffsum*parame)/4096;
	   if(buffsum > 32767)
	   {
	       parame = 32767*4096/buffsum;
		buffsum = 32767;
	   }

	   if(buffsum < -32768)
	   {
	       parame = -32768*4096/buffsum;
		buffsum = -32768;
	   }

	   if(parame < 4096)
	   	parame += (4096-parame) /32;

	   voisbuffer[i] =  buffsum;
     }
}


PRIVATE void vois_MixSpeechNewlc(INT16 *voisbuffer,  INT16 *mixbuff, UINT32 buf_len)
{
    int i;
    UINT32 count = buf_len/sizeof(INT16);
    INT32 ValueMix,ValueMix2;
    INT32 ValueSrc, ValueDes;

     for(i=0; i<count; i++)
     {
          ValueDes = voisbuffer[i];
	   ValueSrc = 	 mixbuff[i];

	   if((ValueDes < 0) && (ValueSrc < 0))
	   {
	   	ValueMix = (ValueDes+ValueSrc )+ (ValueDes*ValueSrc)/65535;
	   }
	   else
	   {
	         ValueMix2 = (ValueDes*ValueSrc);
		  ValueMix = (ValueDes+ValueSrc) - ( ValueMix2)/65535;

	   }
	   voisbuffer[i] =  (INT16)ValueMix;
     }
}


#define vois_MixSPeech vois_MixSpeechLine

PUBLIC UINT32 vois_GetMixRemain(VOID)
{
    return (g_VoisMixBufferRemainingBytes);
}

PUBLIC void vois_MixDataStop(void)
{

    g_VoisMixFinishData = TRUE;

    if(g_MIXVOICE_Handler != NULL)
        g_MIXVOICE_Handler(VOIS_STATUS_NO_MORE_DATA);

    g_MIXVOICE_TYPE = MIXVOISE_STATE_IDLE;
    g_VoisMixBufferStart                     = NULL;
    g_VoisMixBufferTop                       = NULL;
    g_VoisMixBufferCurPos                  = NULL;
    g_VoisMixBufferRemainingBytes    = 0;
    g_VoisMixBufferLength                   = 0;
}


PUBLIC void vois_MixDataFinished(void)
{
    g_VoisMixFinishData = TRUE;
}


PUBLIC void vois_MixBuffer( void *framebuffer, UINT32 len, BOOL OnlyMixflag )
{
       UINT32 framelen = len * sizeof(INT16);
         UINT32 RemaingBytes = vois_GetMixRemain();
         UINT32 *CurPos =vois_GetMixBufPosition();

         if( g_MIXVOICE_TYPE == MIXVOISE_STATE_IDLE) return;

         if(RemaingBytes > framelen)
         {

           if((UINT32)CurPos+framelen >= (UINT32)g_VoisMixBufferTop )
           {
               vois_MixSPeech(framebuffer, CurPos,  (UINT32) g_VoisMixBufferTop-(UINT32)CurPos);
               vois_MixSPeech(framebuffer+(UINT32) g_VoisMixBufferTop-(UINT32)CurPos, g_VoisMixBufferStart, framelen -((UINT32) g_VoisMixBufferTop-(UINT32)CurPos) );

               CurPos =(UINT8* ) CurPos+framelen-g_VoisMixBufferLength;
           }
           else
           {
               vois_MixSPeech(framebuffer, CurPos, framelen);
               CurPos= (UINT8 *)CurPos+framelen;
           }

             if(!OnlyMixflag)
             {
                 g_VoisMixBufferCurPos = CurPos;
               g_VoisMixBufferRemainingBytes -= framelen;
             }
         }
         else
         {
             if(g_VoisMixFinishData)
             {
                  vois_MixDataStop();
             }
             //ignore  this frame mix
         }

}

PUBLIC void vois_GetWriteMixBuffer( UINT32 **buffer, UINT32 *buf_len )
{
	UINT32 RemaingBytes = vois_GetMixRemain();
	UINT32 *CurPos =vois_GetMixBufPosition();

	if((UINT32)CurPos+RemaingBytes >= (UINT32)g_VoisMixBufferTop )
	{
             *buffer =(UINT8* ) CurPos+RemaingBytes-g_VoisMixBufferLength;
	}
	else
	{
		*buffer= (UINT8 *)CurPos+RemaingBytes;
	}


	  if(CurPos>= *buffer )
	  	*buf_len = (UINT32)CurPos-(UINT32)(*buffer);
	  else
	  	 *buf_len = (UINT32) g_VoisMixBufferTop-(UINT32)(*buffer);


}

PUBLIC VOIS_ERR_T vois_AddedMixData(UINT32 addedDataBytes)
{
    UINT32 status = hal_SysEnterCriticalSection();
    g_VoisMixBufferRemainingBytes += addedDataBytes;
    hal_SysExitCriticalSection(status);
    return VOIS_ERR_NO;
}


PUBLIC VOID  vois_SetMixType(MIXVOISE_STATE state)
{
    g_MIXVOICE_TYPE = state;
}
