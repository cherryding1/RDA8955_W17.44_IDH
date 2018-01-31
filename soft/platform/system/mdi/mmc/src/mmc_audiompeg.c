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



#include "cswtype.h"
#include "mcip_debug.h"
#include "fs.h"
#include "mmc_audiompeg.h"
#include "hal_error.h"



#include "cpu_share.h"
#include "assert.h"
#include "hal_voc.h"

#include "global.h"
#include "audio_api.h"

#include "aud_m.h"

#include "fs_asyn.h"

#include "mci.h"
#include "cos.h"

#include "sxs_type.h"
#include "sxr_sbx.h"
#include "sxr_mem.h"

#include "string.h"
#include "hal_overlay.h"

#include "sxr_tls.h"

#include "resample.h"
#ifdef AUD_3_IN_1_SPK
#include "mmc_audiovibrator.h"

#endif
#include "mpegheader_parser.h"


#define AudioJpeg_INPUT_BUFSIZE     (80*1024)//20k byte
//#define MinFileLen                  (8*1024)
#define  MIN_INPUT_REMAIN_SIZE   (2*1024)
#ifdef PCM_OUT_24BIT_PER_SAMPLE
#define AUDIO_MAX_OUTPUT_BUFSIZE (1152*4)
#define AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE (1152*2*4*23)


#define AACDEC_OUTPUT_BUFSIZE (1024*4)
#define AACDEC_PCM_OUTPUT_BUFFER_SIZE (1024*2*4*22)

#define MP3DEC_OUTPUT_BUFSIZE (1152*4)
#define MP3DEC_PCM_OUTPUT_BUFFER_SIZE (1152*2*4*22)
#else//16bit depth
#define AUDIO_MAX_OUTPUT_BUFSIZE (1152*4)
//#define AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE (1152*4*23)
#define AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE (1152*4*4)


#define AACDEC_OUTPUT_BUFSIZE (1024*4)
#define AACDEC_PCM_OUTPUT_BUFFER_SIZE (1024*4*4)

#define MP3DEC_OUTPUT_BUFSIZE (1152*4)
#define MP3DEC_PCM_OUTPUT_BUFFER_SIZE (1152*4*4)
#endif
#define AUDIO_STRAM_CHECKLENGTH (1152*4)

#define AUDIO_VOCDEC_STANDBY    (0)   
#define AUDIO_OUTBUF_ISFULL   (0)  

#define MMC_AUDIODEC_LOCAL_VARS(pAudioPlayer) \
            MPEG_INPUT *input = &pAudioPlayer->MpegInput; \
            MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput; \
            vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus = &pAudioPlayer->Voc_AudioDecStatus; \
            vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;\
            SSHdl *my_ss_handle = pAudioPlayer->MpegInput.ss_handle; \
            bt_a2dp_audio_cap_struct * my_audio_config_handle=   pAudioPlayer->MpegInput.audio_config_handle; \
            bt_a2dp_sbc_codec_cap_struct *sbc_config_handle =  &my_audio_config_handle->codec_cap.sbc;

#define MMC_DrainInputBuffer(pVoc_Voc_AudioDecIN) \
        vpp_AudioJpegDecScheduleOneFrame(pVoc_Voc_AudioDecIN);

extern AUD_LEVEL_T audio_cfg;
extern HAL_AIF_STREAM_T audioStream;
extern AUD_ITF_T audioItf;

MPEG_PLAY MpegPlayer;

volatile static INT32 Mmc_Audio_Pcm_Half;
 
volatile static UINT32 Mmc_Audio_PcmDataCopyCount =0;
volatile static UINT32 ErrorFrameCount=0;

extern UINT32 pcmbuf_overlay[4608];

volatile INT32 g_ConsumedLength=0;
volatile UINT32 g_AudioMode=0;
volatile UINT32 g_AudioSeekFlag=0;

volatile INT32 g_MPEGFILESIZE=0;

extern uint8 g_MmcMciMutex;
#define AUD_MPG_STAT_CLOSED 0
#define AUD_MPG_STAT_OPENED 1
volatile static uint8 g_audioMpegState = AUD_MPG_STAT_CLOSED;
volatile static int32 g_audioMpegVocMsgCount;
volatile static int32 g_audioMpegPcmMsgCount;

#ifdef __BT_AUDIO_VIA_SCO__

static UINT16 g_Nout;
static UINT32 g_counterframe;

static INT16 g_NumOfOutSample=0;

static INT16 g_TotalFrame=0;
static UINT8 g_ResampleInitFlag=0;
static INT16 *g_BT8kPcmBuffer=NULL;

#endif

static HAL_SYS_VOC_FREQ_T VOC_FREQ_STATE = 0;


#ifdef AUD_3_IN_1_SPK


 
static  INT32  g_VibratorSample=0;
static  INT32  g_VibratorCh=0;


#endif

static void MMC_FillinputBuffer( MPEG_INPUT *input,INT16 consumedlen);
static void MMC_FillinputBufDone(void *pParam);

static int MMC_DecErrorHandle(vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus,
                            MPEG_INPUT *input,MPEG_OUTPUT *output);

static MCI_ERR_T MMC_DrainInputBufDone (MPEG_PLAY *pAudioPlayer);
static MCI_ERR_T MMC_DrainOutputBufDone(MPEG_PLAY *pAudioPlayer);
static MCI_ERR_T MMC_AudioDecOpen (HANDLE fhd, MPEG_PLAY *pAudioPlayer);
static void MMC_MpegPcmOutStart(MPEG_PLAY *pAudioPlayer);
static void MMC_UpdateVOCSetting(MPEG_PLAY *pAudioPlayer);
static void MMC_AudioPostProcess(vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus,
                                            int16 *data,bool sco_enable,int pcmsize);

static void MMC_DrainOutputBuffer(MPEG_PLAY *pAudioPlayer);

//#define MP3_PLAYER_AGC

#ifdef MP3_PLAYER_AGC

#define MULTS( x, y)  (INT64)(((INT64)x *(INT64)y)>>16)



#define ABS_AGC(X)  ((X<0)?-X:X)


static int AGC_Coeff_l = 0x7fff*2*8;
static int AGC_serror_l = 0;
static int  AGC_acc_error_l = 0;


static  short  *g_AgcDataAddress=0;
static  int g_AgcDataLength=0;

//this value is what u want to control audio gain level;
//it should be set by user;
static int  AGC_MAXVAL = 30000; // 16384; // 32767;


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




//HANDLE g_FileHandle;
/*
* NAME: MMC_AUDIODecClose()
* DESCRIPTION: Close aac decoder.
*/
//extern uint32 *DRV_PCMOutStop(void);
MCI_ERR_T MMC_AUDIODecClose (MPEG_PLAY *pAudioPlayer)
{
    MPEG_FILE_OPTION *mpegstream;
    MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;
    mpegstream = MpegPlayer.MpegInput.MpegStreamRead;
    
    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AUDIODecClose!");

    g_audioMpegState = AUD_MPG_STAT_CLOSED;

    if (output->StartAif == 1) {
        aud_StreamStop(audioItf);
    }

    vpp_AudioJpegDecClose();

    while(mpegstream->ReadFileFlag==1)COS_Sleep(5); //wait for finishi reading file.
    mmc_MemFreeAll();

    if (pAudioPlayer->mode == MMC_MP3_DECODE && VOC_FREQ_STATE)
    {
        hal_SysSetVocClock(VOC_FREQ_STATE);
        VOC_FREQ_STATE = 0;
    }

    hal_OverlayUnload(HAL_OVERLAY_INT_SRAM_ID_2);

    return  MCI_ERR_NO;

}

/*
* NAME: MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_FillinputBufDone(void *pParam)
{
    FS_ASYN_READ_RESULT *pRsp = ( FS_ASYN_READ_RESULT*)pParam;
    MPEG_FILE_OPTION *mpegstream;
    mpegstream = MpegPlayer.MpegInput.MpegStreamRead;

    mpegstream->ReadFileCounter--;
    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]Read File Finish!");
    if(pRsp->iResult!=0)
    {
        MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC_ERROR] Reading File Error!");
        mpegstream->EndofFileFlag=1;
        mpegstream->ReadFileSize=0;
        mpegstream->ReadFileError=1;
        mpegstream->ReadFileFlag=0;
        return;
    }

    if(pRsp->uSize<((AudioJpeg_INPUT_BUFSIZE-MIN_INPUT_REMAIN_SIZE)>>1))
    {
        MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]End of File!");
        mpegstream->EndofFileFlag=1;
        mpegstream->ReadFileSize=pRsp->uSize;
    }

    mpegstream->ReadFileFlag=0;
    return;
}



/*
* NAME: MMC_AudioVocISR()
* DESCRIPTION: ISR for VOC decode a frame.
*/
void MMC_AudioVocISR(void)
{
//  MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]VoC INT!");
    if(g_audioMpegState == AUD_MPG_STAT_OPENED){
	    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_VOC);
	    g_audioMpegVocMsgCount++;
    }
}



/*
* NAME: MMC_AudioPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/


void AudioHalfPcmISR(void)
{
//  MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]PCM Half!");
    if(g_audioMpegState == AUD_MPG_STAT_OPENED){
	    Mmc_Audio_Pcm_Half = 0;
	    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_PCM);
	    g_audioMpegPcmMsgCount++;
    }
}

/*
* NAME: MMC_AudioPcmISR()
* DESCRIPTION: ISR for pcm half buffer exhausted.
*/
void AudioEndPcmISR(void)
{
    //MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]PCM End!");
    if(g_audioMpegState == AUD_MPG_STAT_OPENED){
	    Mmc_Audio_Pcm_Half = 1;
	    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_PCM);
	    g_audioMpegPcmMsgCount++;
    }

}
/// Audio interface used in the test

void MMC_MpegPcmOutStart(MPEG_PLAY *pAudioPlayer)
{



    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_MpegPcmOutStart begin!");
    // Stream out

#ifdef __BT_AUDIO_VIA_SCO__

    if( pAudioPlayer->MpegInput.bt_sco)  //voice over bt sco
    {
        audioStream.startAddress  = (UINT32*) g_BT8kPcmBuffer;//pAudioPlayer->MpegOutput.pcm.Buffer;
        audioStream.length        = g_TotalFrame*g_NumOfOutSample*2;

        //   streamOut.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
        audioStream.channelNb     =HAL_AIF_MONO; //pAudioPlayer->Voc_AudioDecStatus.nbChannel;//HAL_AIF_STEREO;//HAL_AIF_MONO;

        pAudioPlayer->Voc_AudioDecStatus.SampleRate=HAL_AIF_FREQ_8000HZ;

        aud_StreamStop(audioItf);

        audioItf = AUD_ITF_BLUETOOTH;
        audioStream.voiceQuality         = TRUE;
    }
    else
#endif 
    {
        audioStream.startAddress  = (UINT32*) pAudioPlayer->MpegOutput.pcm.Buffer;
        audioStream.length        = pAudioPlayer->MpegOutput.pcm.Size*4;//MP3DEC_PCM_OUTPUT_BUFFER_SIZE;//AIF_FIFO_LEN * 4;
        //   streamOut.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
        audioStream.channelNb     = pAudioPlayer->Voc_AudioDecStatus.nbChannel;//HAL_AIF_STEREO;//HAL_AIF_MONO;

        audioStream.voiceQuality         = !TRUE;    // To adjust with a key

        aud_StreamStop(audioItf);

        if(pAudioPlayer->MpegInput.pcmflag)
            audioItf = AUD_ITF_LOUD_SPEAKER;

    }

    //audioStream.voiceQuality         = !TRUE;    // To adjust with a key
    audioStream.playSyncWithRecord = FALSE;
    audioStream.halfHandler   = AudioHalfPcmISR;
    audioStream.endHandler    = AudioEndPcmISR;


    if(audioItf!=AUD_ITF_EAR_PIECE&&audioItf!=AUD_ITF_LOUD_SPEAKER&&audioItf!=AUD_ITF_BLUETOOTH)
    {
        // Initial cfg
        audioItf = AUD_ITF_EAR_PIECE;
    }


    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]pAudioPlayer->Voc_AudioDecStatus.SampleRate:%d",pAudioPlayer->Voc_AudioDecStatus.SampleRate);

    switch (pAudioPlayer->Voc_AudioDecStatus.SampleRate)
    {
        case 48000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_48K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_48000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 44100:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_44_1K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 32000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_32K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_32000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 24000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_24K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_24000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 22050:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_22_05K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_22050HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;

        case 16000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_16K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_16000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 12000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_12K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_12000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 11025:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_11_025K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_11025HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        case 8000:
            diag_printf("[MMC_AUDIO]PCM_SAMPLE_RATE_8K\n");
            audioStream.sampleRate    = HAL_AIF_FREQ_8000HZ;//HAL_AIF_FREQ_44100HZ;//HAL_AIF_FREQ_8000HZ;
            break;
        default:
            diag_printf("[MMC_AUDIO]##WARNING## Sample rate error:%d\n",pAudioPlayer->Voc_AudioDecStatus.SampleRate);
            break;
    }

#ifdef AUD_3_IN_1_SPK


    g_VibratorSample=audioStream.sampleRate;
    g_VibratorCh=audioStream.channelNb;


#endif
    UINT32 errStatus =  aud_StreamStart(audioItf, &audioStream, &audio_cfg);

    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_MpegPcmOutStart end; errStatus = %d", errStatus);

    vpp_AudioMP3DecSetReloadFlag();
}

/*
* NAME: MMC_AudioDecOpen()
* DESCRIPTION: Initialize AUDIO decoder.
*/
INT32 *G_VppSBCConstY=NULL;

MCI_ERR_T MMC_AudioDecOpen (HANDLE fhd, MPEG_PLAY *pAudioPlayer)
{

    MPEG_INPUT *input = &pAudioPlayer->MpegInput; 
    MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput; 
        
    MPEG_FILE_OPTION *MpegStreamRead ;
    HAL_ERR_T voc_ret = 0;
    int32 readlen = 0;
    uint32 tagsize=0;
    int32 result;


    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AudioDecOpen begin!");

    if(g_audioMpegState != AUD_MPG_STAT_CLOSED){
	    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]invalid state %d !",g_audioMpegState);
	return MCI_ERR_BUSY;
    }
    g_audioMpegState = AUD_MPG_STAT_OPENED;
    g_audioMpegVocMsgCount=0;
    g_audioMpegPcmMsgCount=0;
    //check file handle
    input->fileHandle=-1;
  
    input->fileHandle =fhd;

    if (input->fileHandle < 0)
    {
        MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AudioDecOpen file open error!");

        return MCI_ERR_CANNOT_OPEN_FILE;
    }

    /*initialize I/O data struct*/
    input->MpegStreamRead = mmc_MemMalloc(sizeof(MPEG_FILE_OPTION));
   
    G_VppSBCConstY = (INT32*)mmc_MemMalloc(468*4);
    input->bof = (uint8 *)mmc_MemMalloc(AudioJpeg_INPUT_BUFSIZE);

    if(input->bof==NULL || G_VppSBCConstY==NULL ||input->MpegStreamRead == NULL)
    {
        mmc_MemFreeAll();
        return MCI_ERR_OUT_OF_MEMORY;
    }

     
    memset(input->MpegStreamRead,0,sizeof(MPEG_FILE_OPTION));
    MpegStreamRead = input->MpegStreamRead;
    
    memset(input->bof, 0, AudioJpeg_INPUT_BUFSIZE);
    input->length = AudioJpeg_INPUT_BUFSIZE;
    input->data = input->bof;
    input->eof = input->bof + input->length;
    input->fileOffset = 0;
    input->readOffset = 0;
    input->is_first_frame=1;
    input->pcmflag = FALSE;
    
    output->length = (AUDIO_MAX_OUTPUT_BUFSIZE) >> 2;//80 int
    output->rPingpFlag = 0;
    output->wPingpFlag = 0;
    output->StartAif = 0;


    hal_OverlayLoad(HAL_OVERLAY_INT_SRAM_ID_2);
    output->pcm.Buffer = (uint32 *)pcmbuf_overlay;
    output->data =  (int16 *)pcmbuf_overlay;
    memset(output->pcm.Buffer,0,AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
    output->pcm.Buffer_head=output->pcm.Buffer;

#ifdef __BT_AUDIO_VIA_SCO__

    g_counterframe=0;
    g_ResampleInitFlag=1;
    
    g_BT8kPcmBuffer=(INT16 *)mmc_MemMalloc(AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);

    if(g_BT8kPcmBuffer==NULL)
    {
        mmc_MemFreeAll();
        return MCI_ERR_OUT_OF_MEMORY;
    }

    memset(g_BT8kPcmBuffer, 0, AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
#endif

    switch(pAudioPlayer->mode)
    {
        case MMC_AAC_DECODE:
            output->pcm.Size = AACDEC_PCM_OUTPUT_BUFFER_SIZE>>2;
            pAudioPlayer->Voc_AudioDecStatus.consumedLen = 0;
            pAudioPlayer->Voc_AudioDecStatus.nbChannel = 0;
            pAudioPlayer->Voc_AudioDecStatus.output_len = 0;
            pAudioPlayer->Voc_AudioDecStatus.streamStatus= 0;

            pAudioPlayer->Voc_AudioDecIN.EQ_Type=-1;
            pAudioPlayer->Voc_AudioDecIN.reset=1;
            break;

        case MMC_MP3_DECODE:
            output->pcm.Size = MP3DEC_PCM_OUTPUT_BUFFER_SIZE>>2;
            pAudioPlayer->Voc_AudioDecStatus.consumedLen = 0;
            pAudioPlayer->Voc_AudioDecStatus.nbChannel = 0;
            pAudioPlayer->Voc_AudioDecStatus.output_len = 0;
            pAudioPlayer->Voc_AudioDecStatus.streamStatus= 0;

            pAudioPlayer->Voc_AudioDecIN.EQ_Type=-1;
            pAudioPlayer->Voc_AudioDecIN.reset=1;
            break;

        default:
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_MP3AAC_ERROR]Error Mode!");
            break;
    }
    Mmc_Audio_PcmDataCopyCount=0;
    Mmc_Audio_Pcm_Half=0;
    ErrorFrameCount=0;
    
    /*prepare input stream*/

    result = mpeg_stream_error_check(fhd,pAudioPlayer->mode,input->bof,AUDIO_STRAM_CHECKLENGTH);

    if (result < 0){

        return MCI_ERR_ERROR;
    }
    if (g_ConsumedLength>0){
        
        if ((result=(int32)FS_Seek(fhd,(INT64)g_ConsumedLength,FS_SEEK_SET))< 0)
        {
            diag_printf("[MMC_AUDIO_ERROR]can not seek file!:%d",result);
            return MCI_ERR_CANNOT_OPEN_FILE;
        }
        g_AudioSeekFlag=1;
    }else {
    
        g_AudioSeekFlag=0;
    }
 
    readlen = FS_Read(input->fileHandle, input->bof, input->length);
    diag_printf("[MMC_AUDIO] after ID3first read len=%d ;need length=%d,header=%x\n", readlen,input->length,*(int*)input->bof);
    if(readlen<0)
    {
        return MCI_ERR_CANNOT_OPEN_FILE;
    }
    if(readlen<input->length)
    {
        input->eof = input->bof + readlen;
        MpegStreamRead->MemMoveFlag=1;
        MpegStreamRead->EndofFileFlag=1;
        if(readlen<4096)
        {
            memset(input->bof +  readlen,0,4096-readlen);
            input->eof = input->bof + 4096; //4096 is two times than Voc inbuffer.

        }
    }

    MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AMR]g_AMRConsumedLength: %d",g_ConsumedLength);

    if (pAudioPlayer->mode == MMC_MP3_DECODE)
    {
        //get current voc frequency and set to 104M.
        VOC_FREQ_STATE = hal_SysSetVocClock(HAL_SYS_VOC_FREQ_104M);
        if(VOC_FREQ_STATE >= HAL_SYS_VOC_FREQ_104M)
        {
            diag_printf("[MCI_MP3AAC]VOC freq = %d",VOC_FREQ_STATE);
            hal_SysSetVocClock(VOC_FREQ_STATE);
            VOC_FREQ_STATE =0;
        }
        else
        {
            diag_printf("[MCI_MP3AAC]VOC old freq = %d and set to 104M!",VOC_FREQ_STATE);
        }
    }

    input->fileOffset += readlen;
    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AudioDecOpen vpp_AudioJpegDecOpen!");
    /*open voc decoder*/
    voc_ret = vpp_AudioJpegDecOpen((HAL_VOC_IRQ_HANDLER_T)MMC_AudioVocISR);

    switch (voc_ret)
    {
        case HAL_ERR_NO:
            //diag_printf("[MMC_AUDIO] success to call vpp_AudioJpegDecOpen(), result: %d \n", voc_ret);
            break;

        case HAL_ERR_RESOURCE_RESET:
        case HAL_ERR_RESOURCE_BUSY:
        case HAL_ERR_RESOURCE_TIMEOUT:
        case HAL_ERR_RESOURCE_NOT_ENABLED:
        case HAL_ERR_BAD_PARAMETER:
        case HAL_ERR_UART_RX_OVERFLOW:
        case HAL_ERR_UART_TX_OVERFLOW:
        case HAL_ERR_UART_PARITY:
        case HAL_ERR_UART_FRAMING:
        case HAL_ERR_UART_BREAK_INT:
        case HAL_ERR_TIM_RTC_NOT_VALID:
        case HAL_ERR_TIM_RTC_ALARM_NOT_ENABLED:
        case HAL_ERR_TIM_RTC_ALARM_NOT_DISABLED:
        case HAL_ERR_COMMUNICATION_FAILED:
        case HAL_ERR_QTY:
            MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_MP3AAC_ERROR]Fail to Call vpp_AudioJpegDecOpen()t: %d \n", voc_ret);
            return MCI_ERR_ERROR;
        default:
            break;
    }
    MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]MMC_AudioDecOpen end!");
    return MCI_ERR_NO;

}

/*
* NAME: MMC_DrainInputBufDone()
* DESCRIPTION: Decode AAC stream ......
*/

extern const INT32 G_VppSBCConstX[];

MCI_ERR_T MMC_DrainInputBufDone (MPEG_PLAY *pAudioPlayer)
{

    MMC_AUDIODEC_LOCAL_VARS(pAudioPlayer);

    int result = 0;
    
    if (input->is_first_frame==0) {
        
        vpp_AudioJpegDecStatus(pVoc_AudioDecStatus);

        // First error check .
        if (input->MpegStreamRead->ReadFileError != MCI_ERR_NO){
            
            return MCI_ERR_ERROR;
            
        }else if (pVoc_AudioDecStatus->ErrorStatus != MCI_ERR_NO
                || pVoc_AudioDecStatus->output_len <= 0
                || pVoc_AudioDecStatus->output_len > MP3DEC_OUTPUT_BUFSIZE) {
            
            result = MMC_DecErrorHandle(pVoc_AudioDecStatus,input,output);
           
            if (result != MCI_ERR_NO) {

                return result; 
            } 
        } else {

            ErrorFrameCount=0;
        }

        //Second audio post processing.
#if defined(MP3_PLAYER_AGC)||defined(AUD_3_IN_1_SPK)||defined(__BT_AUDIO_VIA_SCO__)
    
        MMC_AudioPostProcess(pVoc_AudioDecStatus,output->data,
                             input->bt_sco,output->pcm.Size);
#endif

        //Third read mpeg stream .
        MMC_FillinputBuffer(input,pVoc_AudioDecStatus->consumedLen);

        g_ConsumedLength += pVoc_AudioDecStatus->consumedLen;
    }
    
    // Fourth update voc config.
    MMC_UpdateVOCSetting(pAudioPlayer);

    //fifth send sbc if support a2dp,otherwise output pcm.
    MMC_DrainOutputBuffer(pAudioPlayer);

    input->is_first_frame=0;
    //sixth start decode next frame.
    if(Mmc_Audio_PcmDataCopyCount != AUDIO_OUTBUF_ISFULL || output->StartAif == 0) {
        
        MMC_DrainInputBuffer(pVoc_Voc_AudioDecIN);
       
    }

}

MCI_ERR_T MMC_DrainOutputBufDone(MPEG_PLAY *pAudioPlayer)
{

    MPEG_INPUT *input = &pAudioPlayer->MpegInput;
    MPEG_OUTPUT *output = &pAudioPlayer->MpegOutput;
        
    vpp_AudioJpeg_DEC_IN_T *pVoc_Voc_AudioDecIN = &pAudioPlayer->Voc_AudioDecIN;

    if(Mmc_Audio_PcmDataCopyCount == AUDIO_VOCDEC_STANDBY)
    {

       MMC_DrainInputBuffer(pVoc_Voc_AudioDecIN);

    }

    return MCI_ERR_NO;
}


int32 Audio_MpegPlay(int32 OutputPath, HANDLE fhd,int16 filetype,int32 PlayProgress)
{
    int32 result;
    uint8 mutexUsrId;
    MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MpegPlay,fhd is %d",fhd);

    mutexUsrId= sxr_TakeMutex(g_MmcMciMutex);

    //g_FileHandle=fhd;
    if (fhd< 0)
    {
        diag_printf("[MMC_AUDIO_ERROR]ERROR file hander: %d \n", fhd);
	sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }

    if ((g_MPEGFILESIZE=(INT32)FS_GetFileSize(fhd))<= 0)
    {
        diag_printf("[MMC_AUDIO_ERROR]can not get file size!:%d",g_MPEGFILESIZE);
	sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
        return MCI_ERR_CANNOT_OPEN_FILE;
    }

    MCI_TRACE (MCI_AUDIO_TRC,0,"[MMC_AMR]File Size: %d",g_MPEGFILESIZE);

    g_ConsumedLength=(INT32)(((INT64)PlayProgress*(INT64)g_MPEGFILESIZE)/10000);


    switch(filetype)
    {
        case MCI_TYPE_DAF:
            MpegPlayer.mode = MMC_MP3_DECODE;
            break;

        case MCI_TYPE_AAC:
            MpegPlayer.mode = MMC_AAC_DECODE;
            break;

        default:
            diag_printf("Audio_MpegPlay()->unsupport record media type");
            break;
    }

    MpegPlayer.MpegOutput.OutputPath = OutputPath;

    if((result= MMC_AudioDecOpen(fhd, &MpegPlayer))!=MCI_ERR_NO)
    {
        diag_printf("[MMC_AUDIO]##WARNING##MMC_AudioDecOpen() return false!\n");
        MMC_AUDIODecClose(&MpegPlayer);
	sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
        return result;
    }

    sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
    return  MCI_ERR_NO;
}

int32 Audio_MpegStop (void)
{
    //clear the flag used to play left data in the input buffer and pcm buffer
    uint8 mutexUsrId;
    ErrorFrameCount=0;

    MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MpegStop");
    mutexUsrId= sxr_TakeMutex(g_MmcMciMutex);

    MMC_AUDIODecClose(&MpegPlayer);
    sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);

    while((g_audioMpegVocMsgCount>0) ||(g_audioMpegPcmMsgCount>0) ){
        sxr_Sleep(160);
    }
    return  MCI_ERR_NO;
}


int32 Audio_MpegPause (void)
{
    //MPEG_INPUT *input = &MpegPlayer.MpegInput;
    //Drv_PcmOutStop();
    uint8 mutexUsrId;
    g_AudioMode=MpegPlayer.mode;
    MCI_TRACE (MCI_AUDIO_TRC,0, "******************* Audio_MpegPause**********************");
    //aud_StreamPause(audioItf,TRUE);
    //clear the flag used to play left data in the input buffer and pcm buffer
    ErrorFrameCount=0;

    mutexUsrId= sxr_TakeMutex(g_MmcMciMutex);

    MMC_AUDIODecClose(&MpegPlayer);

    sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
    //memset(MpegPlayer.MpegOutput.pcm.Buffer,0,AUDIO_MAX_PCM_OUTPUT_BUFFER_SIZE);
    while((g_audioMpegVocMsgCount>0) ||(g_audioMpegPcmMsgCount>0) ){
        sxr_Sleep(160);
    }
    return  MCI_ERR_NO;
}


int32 Audio_MpegResume ( HANDLE fhd)
{
    int32 result;
    uint8 mutexUsrId;
//  Drv_PcmOutStart();
    MpegPlayer.mode=g_AudioMode;
//  aud_StreamPause(audioItf,FALSE);

    g_ConsumedLength-=2048;//VoC buffer length
    MCI_TRACE (MCI_AUDIO_TRC,0,"Audio_MpegResume");

    mutexUsrId= sxr_TakeMutex(g_MmcMciMutex);

    if(g_ConsumedLength<0)g_ConsumedLength=0;

    result=(INT32)FS_Seek(fhd,(INT64)g_ConsumedLength,FS_SEEK_SET);

    MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek pos:%d",result);

    if(result<0 )
    {
        MCI_TRACE (MCI_AUDIO_TRC,0,"FS_seek error!");
	sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
        return -1;
    }

    if((result= MMC_AudioDecOpen(fhd, &MpegPlayer))!=MCI_ERR_NO)
    {
        diag_printf("[MMC_AUDIO]##WARNING##MMC_AudioDecOpen() return false!\n");
        MMC_AUDIODecClose(&MpegPlayer);
	sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
        return result;
    }


    sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
    return  MCI_ERR_NO;
}


int32 Audio_MpegGetID3 (char * pFileName)
{
    return MCI_ERR_NO;
}

int32 Audio_MpegGetPlayInformation (MCI_PlayInf * MCI_PlayInfMPEG)
{

    if(g_MPEGFILESIZE>0)
    {
        MCI_PlayInfMPEG->PlayProgress=(INT32)((((INT64)g_ConsumedLength)*10000)/g_MPEGFILESIZE);
    }
    else
    {
        MCI_PlayInfMPEG->PlayProgress=0;
    }


    MCI_TRACE (MCI_AUDIO_TRC,0,"[MCI_AUDIO_MPEG]PlayProgress:%d",MCI_PlayInfMPEG->PlayProgress);


    return MCI_ERR_NO;
}


int32 Audio_MpegUserMsg(int32 nMsg)
{
    uint8 mutexUsrId;
    mutexUsrId= sxr_TakeMutex(g_MmcMciMutex);

    switch (nMsg)
    {
        case MSG_MMC_AUDIODEC_VOC:

		if(g_audioMpegState != AUD_MPG_STAT_OPENED){
			g_audioMpegVocMsgCount--;
			MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]UserMsg invalid state %d !",g_audioMpegState);
			break;
		}

            switch (MMC_DrainInputBufDone(&MpegPlayer))
            {
                case MCI_ERR_END_OF_FILE:
                    diag_printf("[MMC_AUDIO]MMC_AACDecRun() return FINISH!\n");

                    MMC_AUDIODecClose(&MpegPlayer);
                    MCI_AudioFinished(MCI_ERR_END_OF_FILE);

                    mmc_SetCurrMode(MMC_MODE_IDLE);
                    break;
                case MCI_ERR_ERROR:
                    diag_printf("[MMC_AUDIO]##WARNING##MMC_AACDecRun() return FALSE!\n");
                    MMC_AUDIODecClose(&MpegPlayer);
                    if(g_AudioSeekFlag==1)
                    {
                        MCI_AudioFinished(MCI_ERR_END_OF_FILE);
                    }
                    else
                    {
                        MCI_AudioFinished(MCI_ERR_INVALID_FORMAT);
                    }
                    mmc_SetCurrMode(MMC_MODE_IDLE);//jiashuo add
                    break;
                default:
                    break;
            }

	    g_audioMpegVocMsgCount--;

            break;

        case MSG_MMC_AUDIODEC_PCM:
//    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, "*******************MMCTask MSG_MMC_AUDIODEC_PCM**********************");
	
		if(g_audioMpegState != AUD_MPG_STAT_OPENED){
			g_audioMpegPcmMsgCount--;
			MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]UserMsg invalid state %d !",g_audioMpegState);
			break;
		}

            MMC_DrainOutputBufDone(&MpegPlayer);
#ifdef __BT_PCM_OVER_UART__
            aud_BtSendSCOData((Mmc_Audio_Pcm_Half+1)%2);
#endif
	    g_audioMpegPcmMsgCount--;
            break;
        default:
            diag_printf("[MMC_AUDIO]##WARNING##**************Error message******************!\n");
            break;
    }

    sxr_ReleaseMutex(g_MmcMciMutex,mutexUsrId);
    return 0;
}

int MMC_DecErrorHandle(vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus,
                            MPEG_INPUT *input,MPEG_OUTPUT *output) {

    MPEG_FILE_OPTION *mpegstream;
    mpegstream = input->MpegStreamRead;

    if (MpegPlayer.mode == MMC_MP3_DECODE ) {

        pVoc_AudioDecStatus->output_len = 1152*4;

    }else if (MpegPlayer.mode == MMC_AAC_DECODE) {

        pVoc_AudioDecStatus->output_len = 1024*4;
    }
    
    memset((char *)((UINT32)(output->data)|0x20000000),
            0, pVoc_AudioDecStatus->output_len);//mute current frame
                           
    if (mpegstream->EndErrorFlag)
    {
        MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_MP3AAC_ERROR]Error Frame!EndErrorFlag=1");
        mpegstream->EndErrorFlag = 0;

        if(output->StartAif == 1)
        {
            return MCI_ERR_END_OF_FILE;
        }
        else
        {
            return MCI_ERR_ERROR;
        }
    }else {

        ErrorFrameCount++;
        sxr_Sleep(5);
        if(mpegstream->ReadFileCounter>1)sxr_Sleep(80);

        MCI_TRACE (MCI_AUDIO_TRC,0, "DecoderError Count %d",ErrorFrameCount);
         
        if(ErrorFrameCount>200||mpegstream->ReadFileCounter>2)
        {
            MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_MP3AAC_ERROR]ErrorFrameCount:%d g_ReadFileCounter:%d",ErrorFrameCount,mpegstream->ReadFileCounter);

            return MCI_ERR_ERROR;
        }
    }

    return MCI_ERR_NO;
    
}

void MMC_FillinputBuffer( MPEG_INPUT *input,INT16 consumedlen){    

    MPEG_FILE_OPTION *mpegstream;
    mpegstream = input->MpegStreamRead;
    input->data += consumedlen;
    if (mpegstream->MemMoveFlag==0) {
        
        if ((input->data-input->bof) >
            (((AudioJpeg_INPUT_BUFSIZE-MIN_INPUT_REMAIN_SIZE)>>1)+MIN_INPUT_REMAIN_SIZE)
            && mpegstream->buffer_flag==0 
            && mpegstream->EndofFileFlag==0) {
        
            FS_AsynReadReq (input->fileHandle,
                            input->bof+MIN_INPUT_REMAIN_SIZE,
                            ((AudioJpeg_INPUT_BUFSIZE-MIN_INPUT_REMAIN_SIZE)>>1),
                            MMC_FillinputBufDone);
            
            mpegstream->ReadFileFlag=1;
            mpegstream->buffer_flag=1;
            mpegstream->ReadFileCounter++;
            MCI_TRACE (MCI_AUDIO_TRC,0,"FILL FIRST HALF OF BUFFER");
            
        }
        else if(mpegstream->EndofFileFlag != 0 && mpegstream->buffer_flag!=1)
        {
            input->eof =input->eof
                        -((AudioJpeg_INPUT_BUFSIZE-MIN_INPUT_REMAIN_SIZE)>>1)
                        + mpegstream->ReadFileSize;
            
           mpegstream->MemMoveFlag=1;
        }

        if ((input->eof - input->data)<MIN_INPUT_REMAIN_SIZE)
        {
            
            memcpy(input->bof+MIN_INPUT_REMAIN_SIZE-(input->eof - input->data), 
                    input->data, (input->eof - input->data));
            
            input->data=input->bof+MIN_INPUT_REMAIN_SIZE-(input->eof - input->data);

            if(mpegstream->EndofFileFlag==0)
            {
             
                FS_AsynReadReq (input->fileHandle,
                                (input->bof+MIN_INPUT_REMAIN_SIZE +
                                ((AudioJpeg_INPUT_BUFSIZE-MIN_INPUT_REMAIN_SIZE)>>1)),
                                ((AudioJpeg_INPUT_BUFSIZE-MIN_INPUT_REMAIN_SIZE)>>1),
                                MMC_FillinputBufDone);

                mpegstream->ReadFileFlag=1;
                mpegstream->buffer_flag=0;
                mpegstream->ReadFileCounter++;
                MCI_TRACE (MCI_AUDIO_TRC,0,"FILL SECOND HALF OF BUFFER");
            }
            else
            {
                input->eof =input->bof+MIN_INPUT_REMAIN_SIZE+mpegstream->ReadFileSize;
                mpegstream->MemMoveFlag=1;
            }

        }

    }
    else
    {
        if(input->data > input->eof) {
            
            mpegstream->EndErrorFlag = 1;
        }
        if((input->eof - input->data)<MIN_INPUT_REMAIN_SIZE&&mpegstream->EndErrorFlag==0)
        {
            MCI_TRACE (MCI_AUDIO_TRC,0, "[MCI_MP3AAC]input->eof - input->data: %d",(input->eof - input->data));
            memcpy(input->bof, input->data, (input->eof - input->data));
            memset(input->bof + (input->eof - input->data),0,4096);
            input->data=input->bof;
            input->eof=input->bof+(input->eof - input->data)+4096;
            mpegstream->EndErrorFlag = 1;
        }
    }

}

void MMC_DrainOutputBuffer(MPEG_PLAY *pAudioPlayer){

    
    MMC_AUDIODEC_LOCAL_VARS(pAudioPlayer);

       //Fourth output sbc if support a2dp.
    if (is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type ==1)) {

        if(input->is_first_frame == 0){
            my_ss_handle->PutData(my_ss_handle, 
                                  (UINT8*) hal_VocGetPointer(SBC_Out_put_addr),
                                   pVoc_AudioDecStatus->ImgWidth);
        }
    }
    
    if (output->StartAif == 0){

        if (Mmc_Audio_PcmDataCopyCount>=(output->pcm.Size))
        {
            if(!pAudioPlayer->Voc_AudioDecStatus.ErrorStatus)
            {
                MMC_MpegPcmOutStart(pAudioPlayer);

                output->StartAif  = 1;
                input->pcmflag = FALSE;
            }
            Mmc_Audio_PcmDataCopyCount = AUDIO_OUTBUF_ISFULL;
        }
    }else {
        
        if ((input->pcmflag)&&(!pAudioPlayer->Voc_AudioDecStatus.ErrorStatus))
        {
            MMC_MpegPcmOutStart(pAudioPlayer);
            input->pcmflag = FALSE;
        }

        if(Mmc_Audio_PcmDataCopyCount >= (output->pcm.Size>>1))
        {

            Mmc_Audio_PcmDataCopyCount = AUDIO_OUTBUF_ISFULL;

        }

    }

}


void MMC_AudioPostProcess(vpp_AudioJpeg_DEC_OUT_T *pVoc_AudioDecStatus,
                                    int16 *data,bool sco_enable,int pcmsize){

// processing  current pcm stream if need.
#ifdef MP3_PLAYER_AGC

//      diag_printf("voc status, ErrorStatus : %d, audioItf: %d,  audio_cfg.spkLevel: %d\n", pVoc_AudioDecStatus->ErrorStatus, audioItf, audio_cfg.spkLevel );
#if defined(VOLUME_WITH_15_LEVEL) || defined(VOLUME_WITH_7_LEVEL)
        if(pVoc_AudioDecStatus->ErrorStatus==0&&audioItf==AUD_ITF_LOUD_SPEAKER&&audio_cfg.spkLevel== AUD_SPK_VOL_15)
#else
        if(pVoc_AudioDecStatus->ErrorStatus==0&&audioItf==AUD_ITF_LOUD_SPEAKER&&audio_cfg.spkLevel== AUD_SPK_VOL_7)
#endif
        {
            UINT32 AgcDataLength=pVoc_AudioDecStatus->output_len/2;
            INT16 * AgcDataAddress=(INT16 *)((UINT32)(data)|0x20000000);
            int32 timebegin=hal_TimGetUpTime();

            AGainCtr_S(AgcDataAddress,AgcDataAddress,AgcDataLength);

            int32 timeend=hal_TimGetUpTime();
            MCI_TRACE (TSTDOUT,0,"AGC time = %dms",((timeend-timebegin)*1000)/16384);

        }

#endif

#ifdef AUD_3_IN_1_SPK

        if(pVoc_AudioDecStatus->ErrorStatus==0&&audioItf==AUD_ITF_LOUD_SPEAKER
                &&(pVoc_AudioDecStatus->nbChannel==1||pVoc_AudioDecStatus->nbChannel==2))
        {

            INT16 *VibratorDataAddr=(INT16 *)((UINT32)(data)|0x20000000);
            INT32 VibratorDataLength=pVoc_AudioDecStatus->output_len/2;
            INT32 VibratorDataSample=g_VibratorSample;
            INT32 VibratorDataCh=pVoc_AudioDecStatus->nbChannel;

            //INT32 timebegin=hal_TimGetUpTime();
            mmc_Vibrator(VibratorDataAddr,VibratorDataLength,VibratorDataAddr ,g_MCIAudioVibrator, VibratorDataSample,VibratorDataCh);
            //INT32 timeend_xuml=hal_TimGetUpTime();
            //MCI_TRACE (TSTDOUT,0,"Vibrator time = %dms",((timeend_xuml-timebegin)*1000)/16384);

        }
#endif
        //always close because of bad voice quality,TODO:Resample alg is ugly,need improve.
#ifdef __BT_AUDIO_VIA_SCO__

        if( sco_enable == TRUE)
        {
            INT32 i;

            int32 timebegin=hal_TimGetUpTime();
            INT16 *BTDataAddress=(INT16 *)((UINT32)(data)|0x20000000);

            if(pVoc_AudioDecStatus->nbChannel==2)
            {
                INT16 *PCM_p=(INT16 *)BTDataAddress;

                for(i=0; i<pVoc_AudioDecStatus->output_len/2; i=i+2)
                {
                    PCM_p[i/2]=(PCM_p[i]>>1)+(PCM_p[i+1]>>1);
                }
            }


            if(g_ResampleInitFlag)
            {
                g_TotalFrame=pcmsize/(pVoc_AudioDecStatus->output_len>>2);

                g_NumOfOutSample=ResampleInit(pVoc_AudioDecStatus->SampleRate, 8000, pVoc_AudioDecStatus->output_len, pVoc_AudioDecStatus->nbChannel);

                g_ResampleInitFlag=0;
            }

            /* Resample stuff in input buffer */

            g_Nout=ResampleOneFrame(BTDataAddress,g_BT8kPcmBuffer+g_counterframe*g_NumOfOutSample);

            g_counterframe++;

            if(g_counterframe>=g_TotalFrame)
            {
                g_counterframe=0;
            }

            MCI_TRACE (TSTDOUT,0,"g_Nout = %d",g_Nout);


            int32 timeend=hal_TimGetUpTime();

            MCI_TRACE (TSTDOUT,0,"resample time = %dus",((timeend-timebegin)*1000000)/16384);

        }
 #endif 


}
void MMC_UpdateVOCSetting(MPEG_PLAY *pAudioPlayer){


    
    MMC_AUDIODEC_LOCAL_VARS(pAudioPlayer);
    
    //update io point
    if(!pAudioPlayer->Voc_AudioDecStatus.ErrorStatus)
    {
        if(Mmc_Audio_Pcm_Half==0)
        {
            output->pcm.Buffer_head=output->pcm.Buffer;
        }
        else if(Mmc_Audio_Pcm_Half==1)
        {
            output->pcm.Buffer_head=output->pcm.Buffer+(output->pcm.Size>>1);
        }
        output->data=(int16 *)&output->pcm.Buffer_head[Mmc_Audio_PcmDataCopyCount];

        if (input->is_first_frame == 0 ) {
            Mmc_Audio_PcmDataCopyCount+=(pVoc_AudioDecStatus->output_len>>2);
        }
    }
    pVoc_Voc_AudioDecIN->inStreamBufAddr=(INT32 *)input->data;
    pVoc_Voc_AudioDecIN->outStreamBufAddr=(INT32 *)output->data;

    pVoc_Voc_AudioDecIN->mode=pAudioPlayer->mode;
    pVoc_Voc_AudioDecIN->BsbcEnable = 0;
    pVoc_Voc_AudioDecIN->audioItf=audioItf;

    //update sbc encoder config.
    if( is_SSHdl_valid(my_ss_handle)&& (my_audio_config_handle->codec_type == 1))     
    {

        //  { 0x40, "-24dB"},  { 0x5a, "-21dB"},  { 0x80, "-18dB"},  { 0xb5, "-15dB"},
        //  { 0x100, "-12dB"},{ 0x16a, "-9dB"},{ 0x200, "-6dB"},{ 0x2d4, "-3dB"},
        //  { 0x400, "0dB"}

        switch(audio_cfg.spkLevel)
        {
            case SND_SPK_MUTE:
                pVoc_Voc_AudioDecIN->zoom_mode=0;//mute
                break;
            case SND_SPK_VOL_1:
                pVoc_Voc_AudioDecIN->zoom_mode=0x80;//-18db
                break;
            case SND_SPK_VOL_2:
                pVoc_Voc_AudioDecIN->zoom_mode=0xb5;//-15db
                break;
            case SND_SPK_VOL_3:
                pVoc_Voc_AudioDecIN->zoom_mode=0x100;//-12db
                break;
            case SND_SPK_VOL_4:
                pVoc_Voc_AudioDecIN->zoom_mode=0x16a;//-9db
                break;
            case SND_SPK_VOL_5:
                pVoc_Voc_AudioDecIN->zoom_mode=0x200;//-6db
                break;
            case SND_SPK_VOL_6:
                pVoc_Voc_AudioDecIN->zoom_mode=0x2d4;//-3db
                break;
            case SND_SPK_VOL_7:
                pVoc_Voc_AudioDecIN->zoom_mode=0x400;//0db
                break;
            default:
                pVoc_Voc_AudioDecIN->zoom_mode=0x400;//0db
                break;

        }

        int mode =SBC_MODE_JOINT_STEREO;
        pVoc_Voc_AudioDecIN->BsbcEnable = 1;


        if( sbc_config_handle->channel_mode == 0x01 )
            mode = SBC_MODE_JOINT_STEREO; // joint stereo
        else if( sbc_config_handle->channel_mode == 0x02 )
            mode = SBC_MODE_STEREO; // stereo
        else if( sbc_config_handle->channel_mode == 0x04 )
            mode = SBC_MODE_DUAL_CHANNEL; // dual
        else if( sbc_config_handle->channel_mode == 0x08 )
            mode= SBC_MODE_MONO; // mono
        else
            MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_AUDIO]##WARNING##ErrorSBC channel mode!\n");

        pVoc_Voc_AudioDecIN->Source_width = mode;

        if( sbc_config_handle->alloc_method  == 0x01 )
            pVoc_Voc_AudioDecIN->Source_height= SBC_AM_LOUDNESS;//
        else if( sbc_config_handle->alloc_method  == 0x02 )
            pVoc_Voc_AudioDecIN->Source_height= SBC_AM_SNR;//SBC_AM_LOUDNESS;//
        else
            MCI_TRACE (MCI_AUDIO_TRC,0, "[MMC_AUDIO]##WARNING##ErrorSBC alloc_method!\n");
#ifdef __BT_PROFILE_BQB__
        UINT8 bitpool=0;
        if(sbc_config_handle->min_bit_pool ==sbc_config_handle->max_bit_pool)
            bitpool = sbc_config_handle->max_bit_pool;
        else if(SBC_BIT_POOL >sbc_config_handle->min_bit_pool && SBC_BIT_POOL<sbc_config_handle->max_bit_pool)
            bitpool = SBC_BIT_POOL;
        else
            bitpool = sbc_config_handle->max_bit_pool;

        pVoc_Voc_AudioDecIN->Cut_width =    bitpool;
        MCI_TRACE (MCI_AUDIO_TRC,0, "MMC_AUDIODecVoc begin bitpool=%d", bitpool);
#else
        pVoc_Voc_AudioDecIN->Cut_width =    SBC_BIT_POOL;
#endif
        pVoc_Voc_AudioDecIN->inStreamUBufAddr = (INT32*)G_VppSBCConstX;
        pVoc_Voc_AudioDecIN->inStreamVBufAddr = G_VppSBCConstY;
    }

}


