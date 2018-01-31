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



#include "at.h"
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special.h"
#include "at_cfg.h"
#include "dm_audio.h"
#include "mci.h"
#include "at_trace.h"
#include "at_cmd_special_audio.h"
#include "aud_m.h"

#ifdef MODEM_CALL_RECORDER


//#include "soundrecorderdef.h"
#include "vois_m.h"
#include "mmc_audiowav.h"
#include "at_cmd_special_call_rec.h"
//#include "callmanagementstruct.h"
//#include "callsstruct.h"
/**************************************************************************************************
 *
 **************************************************************************************************/
#define ENABLE_ADPCOM
#define OslMalloc COS_MALLOC
#define OslMfree COS_FREE
//#define g_output_path_default DM_AUDIO_MODE_EARPIECE
U8 g_output_path_default = DM_AUDIO_MODE_LOUDSPEAKER;
#define ENABLE_ADPCM
#if defined(__MMI_CALL_RECORDER_FILE_SAVE_PATH__)
#define FMGR_DEFAULT_FOLDER_CALL_REC        "Auto Call Recorder/"
#else
#define FMGR_DEFAULT_FOLDER_CALL_REC        "CallRec/"
#endif

#ifdef ENABLE_ADPCM
#define MMI_CALL_REC_BUFFER_SIZE        (320*10*2) //20 frame
#else
#define MMI_CALL_REC_BUFFER_SIZE        16000
#endif //ENABLE_ADPCM


typedef enum CALL_REC_STATE
{
    CALL_REC_NOT_REC,
    CALL_REC_RECDING,
    CALL_REC_PAUSE
} CALL_REC_STATE;


BOOL g_setAutoCallRecordFlag = TRUE;
BOOL g_isAutoCallRecordStoped = FALSE;

PU8 g_curCallNum;
PU8 g_curCallName;
U8 g_curCallIndex = 0;

#define CALL_RECORD_FILENAME "/t/call_rec.wav"
CALL_REC_STATE g_call_recording = CALL_REC_NOT_REC;
U8 *g_p_call_rec_buffer = NULL;
U8 g_rec_filename[60];
INT32   g_call_rec_file_handle = -1;
U32 g_call_recTime_remain = 0;

extern HANDLE g_hCosATTask;
extern AUD_ITF_T  audioItf;


#ifdef ENABLE_ADPCM

#define NOISE_SHAPING_OFF       0   // flat noise (no shaping)
#define NOISE_SHAPING_STATIC    1   // first-order highpass shaping
#define NOISE_SHAPING_DYNAMIC   2   // dynamically tilted noise based on signal

#define ADPCM_BLOCK_SAMPLES 505 //for 8k wav block.sheen
#define ADPCM_BLOCK_SIZE 256 //encode block size.sheen

/* This module encodes and decodes 4-bit ADPCM (DVI/IMA varient). ADPCM data is divided
 * into independently decodable blocks that can be relatively small. The most common
 * configuration is to store 505 samples into a 256 byte block, although other sizes are
 * permitted as long as the number of samples is one greater than a multiple of 8. When
 * multiple channels are present, they are interleaved in the data with an 8-sample
 * interval.
 */

/********************************* 4-bit ADPCM encoder ********************************/

#define CLIP(data, min, max) \
if ((data) > (max)) data = max; \
else if ((data) < (min)) data = min;

/* step table */
static const uint16_t step_table[89] = {
    7, 8, 9, 10, 11, 12, 13, 14,
    16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411,
    1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
    7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767
};

/* step index tables */
static const int index_table[] = {
    /* adpcm data size is 4 */
    -1, -1, -1, -1, 2, 4, 6, 8
};

struct adpcm_channel {
    int32_t pcmdata;                        // current PCM value
    int32_t error, weight, history [2];     // for noise shaping
    int16_t index;                          // current index into step size table
};

struct adpcm_context {
    struct adpcm_channel channels [2];
    int num_channels, lookahead, noise_shaping;
    int block_samples;                      //record encode samples count for block.total 505
    int tmp_count;                          //remain sample count.
    int16_t tmp_buf[2];                     //for 2 samples align. 1 ch.

};

struct adpcm_context adpcm_cntx;
uint8_t *pbuf_adpcm= NULL;                  //256 bytes

static double minimum_error (const struct adpcm_channel *pchan, int nch, int32_t csample, const int16_t *sample, int depth, int *best_nibble)
{
    int32_t delta = csample - pchan->pcmdata;
    struct adpcm_channel chan = *pchan;
    int step = (int)step_table[chan.index];
    int trial_delta = (step >> 3);
    int nibble, nibble2;
    double min_error;

    if (delta < 0) {
        int mag = (-delta << 2) / step;
        nibble = 0x8 | (mag > 7 ? 7 : mag);
    }
    else {
        int mag = (delta << 2) / step;
        nibble = mag > 7 ? 7 : mag;
    }

    if (nibble & 1) trial_delta += (step >> 2);
    if (nibble & 2) trial_delta += (step >> 1);
    if (nibble & 4) trial_delta += step;
    if (nibble & 8) trial_delta = -trial_delta;

    chan.pcmdata += trial_delta;
    CLIP(chan.pcmdata, -32768, 32767);
    if (best_nibble) *best_nibble = nibble;
    min_error = (double) (chan.pcmdata - csample) * (chan.pcmdata - csample);

    if (depth) {
        chan.index += index_table[nibble & 0x07];
        CLIP(chan.index, 0, 88);
        min_error += minimum_error (&chan, nch, sample [nch], sample + nch, depth - 1, NULL);
    }
    else
        return min_error;

    for (nibble2 = 0; nibble2 <= 0xF; ++nibble2) {
        double error;

        if (nibble2 == nibble)
            continue;

        chan = *pchan;
        trial_delta = (step >> 3);

        if (nibble2 & 1) trial_delta += (step >> 2);
        if (nibble2 & 2) trial_delta += (step >> 1);
        if (nibble2 & 4) trial_delta += step;
        if (nibble2 & 8) trial_delta = -trial_delta;

        chan.pcmdata += trial_delta;
        CLIP(chan.pcmdata, -32768, 32767);

        error = (double) (chan.pcmdata - csample) * (chan.pcmdata - csample);

        if (error < min_error) {
            chan.index += index_table[nibble2 & 0x07];
            CLIP(chan.index, 0, 88);
            error += minimum_error (&chan, nch, sample [nch], sample + nch, depth - 1, NULL);

            if (error < min_error) {
                if (best_nibble) *best_nibble = nibble2;
                min_error = error;
            }
        }
    }

    return min_error;
}

static uint8_t adpcm_encode_sample (struct adpcm_context *pcnxt, int ch, const int16_t *sample, int num_samples)
{
    struct adpcm_channel *pchan = pcnxt->channels + ch;
    int32_t csample = *sample;
    int depth = num_samples - 1, nibble;
    int step = step_table[pchan->index];
    int trial_delta = (step >> 3);

    if (pcnxt->noise_shaping == NOISE_SHAPING_DYNAMIC) {
        int32_t sam = (3 * pchan->history [0] - pchan->history [1]) >> 1;
        int32_t temp = csample - (((pchan->weight * sam) + 512) >> 10);
        int32_t shaping_weight;

        if (sam && temp) pchan->weight -= (((sam ^ temp) >> 29) & 4) - 2;
        pchan->history [1] = pchan->history [0];
        pchan->history [0] = csample;

        shaping_weight = (pchan->weight < 256) ? 1024 : 1536 - (pchan->weight * 2);
        temp = -((shaping_weight * pchan->error + 512) >> 10);

        if (shaping_weight < 0 && temp) {
            if (temp == pchan->error)
                temp = (temp < 0) ? temp + 1 : temp - 1;

            pchan->error = -csample;
            csample += temp;
        }
        else
            pchan->error = -(csample += temp);
    }
    else if (pcnxt->noise_shaping == NOISE_SHAPING_STATIC)
        pchan->error = -(csample -= pchan->error);

    if (depth > pcnxt->lookahead)
        depth = pcnxt->lookahead;

    minimum_error (pchan, pcnxt->num_channels, csample, sample, depth, &nibble);

    if (nibble & 1) trial_delta += (step >> 2);
    if (nibble & 2) trial_delta += (step >> 1);
    if (nibble & 4) trial_delta += step;
    if (nibble & 8) trial_delta = -trial_delta;

    pchan->pcmdata += trial_delta;
    pchan->index += index_table[nibble & 0x07];
    CLIP(pchan->index, 0, 88);
    CLIP(pchan->pcmdata, -32768, 32767);

    if (pcnxt->noise_shaping)
        pchan->error += pchan->pcmdata;

    return nibble;
}
/*
multiple of 2 samples
*/
static void adpcm_encode_chunks (struct adpcm_context *pcnxt, uint8_t *outbuf, uint32_t *outbufsize, int16_t *inbuf, uint32_t *inbufcount)
{
    const int16_t *pcmbuf;
    int chunks, ch;
    int16_t *pIn= inbuf;
    uint8_t *pOut= outbuf;

    if(pcnxt->tmp_count==1){
        //only for 1 ch
        *pOut = adpcm_encode_sample (pcnxt, 0, pcnxt->tmp_buf, 1);
        *pOut |= adpcm_encode_sample (pcnxt, 0, pIn,1) << 4;

        (pIn)++;
        (pOut)++;
        *outbufsize += 1;
        *inbufcount -= 1;
        pcnxt->block_samples +=2;
    }else if(pcnxt->tmp_count==2){
        //only for 1 ch
        *pOut = adpcm_encode_sample (pcnxt, 0, pcnxt->tmp_buf, 1);
        *pOut |= adpcm_encode_sample (pcnxt, 0, pcnxt->tmp_buf+1,1) << 4;

        (pOut)++;
        *outbufsize += 1;
        pcnxt->block_samples +=2;
    }

    pcnxt->tmp_count =0;

    if(pcnxt->block_samples==505)                   //one block
        return;

    if((*inbufcount) <= 505- pcnxt->block_samples ){
        chunks = (*inbufcount)>>1;
        *outbufsize += chunks * pcnxt->num_channels;
        *inbufcount = (*inbufcount) - (chunks*2);
    }else{
        chunks = (505- pcnxt->block_samples)>>1;
        *outbufsize += chunks * pcnxt->num_channels;
        *inbufcount = (*inbufcount) - (chunks*2);
    }
    pcnxt->block_samples +=chunks*2;

    while (chunks-- >0)
    {
        for (ch = 0; ch < pcnxt->num_channels; ch++)
        {
            pcmbuf = pIn + ch;

            *pOut = adpcm_encode_sample (pcnxt, ch, pcmbuf, chunks *2 +2);
            pcmbuf += pcnxt->num_channels;
            *pOut |= adpcm_encode_sample (pcnxt, ch, pcmbuf, chunks * 2 + 1) << 4;
            pcmbuf += pcnxt->num_channels;
            (pOut)++;
        }
        pIn += 2 * pcnxt->num_channels;
    }

    if((*inbufcount) ==1){
        pcnxt->tmp_buf[0] = pIn[0];
        pcnxt->tmp_count =1;
        *inbufcount -= 1;
    }

}

static void adpcm_encode_block_header(struct adpcm_context *pcnxt, uint8_t *outbuf, uint32_t *outbufsize, const int16_t *inbuf, uint32_t *inbufcount)
{
    if(pcnxt->tmp_count>0){
        //2 byte for first pcm value.
        *((int16_t*)outbuf)= pcnxt->tmp_buf[0];
        pcnxt->tmp_count--;
        if(pcnxt->tmp_count>0)//move data
            pcnxt->tmp_buf[0]=pcnxt->tmp_buf[1];
    }else{
        //2 byte for first pcm value.
        *((int16_t*)outbuf)= *((int16_t*)inbuf);
        //update input buffer count.
        *inbufcount -=1;
    }
    outbuf[2]= pcnxt->channels[0].index;
    outbuf[3]= 0;
    //only for 1 ch
    pcnxt->channels[0].pcmdata= *((int16_t*)outbuf);
    //update
    *outbufsize +=4;
    pcnxt->block_samples =1;//total 505
    return;
}
#endif //ENABLE_ADPCM


/**************************************************************************************************
 *                                      module call record API
 **************************************************************************************************/


static void call_rec_cb_end()
{
    mmi_call_rec_write_file(VOIS_STATUS_END_BUFFER_REACHED);
}


static CALL_REC_STATE get_call_recording_state(void)
{
    return g_call_recording;
}

static void set_call_recording_state(CALL_REC_STATE state)
{
    g_call_recording = state;
}



static INT32 callRec_writeWaveHeader(HANDLE fhd)
{
    UINT32 fileSize = 0;
    INT32 result=-1;
#ifdef ENABLE_ADPCM
    UINT32 tmp;
    WAVE_HEADER_EX waveFormatHeader_ex;
#else
    WAVE_HEADER waveFormatHeader;
#endif
    WAVE_DATA_HEADER waveDataHeader;
    INT32 size=0;

    FS_Seek(fhd, 0, FS_SEEK_END);
//    FS_GetFileSize(fhd, &fileSize);
    fileSize = FS_GetFileSize(fhd);
    result=FS_Seek( fhd, 0,FS_SEEK_SET );

    AT_TC(g_sw_SPEC, " func :%s fhd =%d, fileSize = %d, result = %d", __FUNCTION__, fhd,fileSize, result);
    if(ERR_SUCCESS != result || fileSize < 0)
    {
        return result;
    }

    //wav header

#ifdef ENABLE_ADPCM
    memcpy(waveFormatHeader_ex.szRiff,"RIFF",4);
    waveFormatHeader_ex.dwFileSize = fileSize- 8;
    memcpy(waveFormatHeader_ex.szWaveFmt,"WAVEfmt ",8);
    waveFormatHeader_ex.dwFmtSize = 20;
    waveFormatHeader_ex.wFormatTag = 0x11;//IMA ADPCM
    waveFormatHeader_ex.nChannels = HAL_AIF_MONO;
    waveFormatHeader_ex.nSamplesPerSec = 8000;
    waveFormatHeader_ex.wBitsPerSample = 4;
    waveFormatHeader_ex.nAvgBytesPerSec = 8000*256/505;//mono
    waveFormatHeader_ex.nBlockAlign = 256;
    waveFormatHeader_ex.cbSize = 2;
    waveFormatHeader_ex.samplesPerBlock = 505;
    memcpy(waveFormatHeader_ex.fact,"fact",4);
    waveFormatHeader_ex.factsize_low = 4;
    waveFormatHeader_ex.factsize_hi = 0;
    tmp= fileSize -sizeof(WAVE_HEADER_EX) -sizeof(WAVE_DATA_HEADER);
    tmp= (tmp*505/256) + (tmp%256)*2;
    waveFormatHeader_ex.sample_low = tmp&0xffff;
    waveFormatHeader_ex.sample_hi = (tmp>>16)&0xff;

    //write wav header
    result=FS_Write(fhd,(UINT8 *)&waveFormatHeader_ex,sizeof(WAVE_HEADER_EX));
    AT_TC(g_sw_SPEC, " 710func :%s  result = %d, size = %d", __FUNCTION__,result, sizeof(WAVE_HEADER_EX));
    if(result!=sizeof(WAVE_HEADER_EX))
    {
        return result;
    }
    //wav data header
    memcpy(waveDataHeader.szData,"data",4);
    waveDataHeader.dwDataSize = fileSize - 60;

#else
	//pcm
    memcpy(waveFormatHeader.szRiff,"RIFF",4);
    waveFormatHeader.dwFileSize = fileSize- 8;
    memcpy(waveFormatHeader.szWaveFmt,"WAVEfmt ",8);
    waveFormatHeader.dwFmtSize = 16;
    waveFormatHeader.wFormatTag = 1; //PCM
    waveFormatHeader.nChannels = HAL_AIF_MONO;
    waveFormatHeader.nSamplesPerSec = 8000;
    waveFormatHeader.wBitsPerSample = 16;
    waveFormatHeader.nAvgBytesPerSec = waveFormatHeader.nChannels*waveFormatHeader.wBitsPerSample*waveFormatHeader.nSamplesPerSec/8;
    waveFormatHeader.nBlockAlign = waveFormatHeader.nChannels*waveFormatHeader.wBitsPerSample/8;
    //write wav header
    result=FS_Write(fhd,(UINT8 *)&waveFormatHeader,sizeof(WAVE_HEADER));

    AT_TC(g_sw_SPEC, " 734func :%s  result = %d, size = %d", __FUNCTION__,result, sizeof(WAVE_HEADER_EX));
    if(result!=sizeof(WAVE_HEADER))
    {
        return result;
    }

    //wav data header
    memcpy(waveDataHeader.szData,"data",4);
    waveDataHeader.dwDataSize = fileSize - 44;
#endif //ENABLE_ADPCM
    //write wav data header
    result=FS_Write(fhd,(UINT8 *)&waveDataHeader,sizeof(WAVE_DATA_HEADER));

    AT_TC(g_sw_SPEC, " func :%s  result = %d, size = %d", __FUNCTION__,result, sizeof(WAVE_DATA_HEADER));
    if(result!=sizeof(WAVE_DATA_HEADER))
    {
        return result;
    }

    return 0;
}

static void mod_call_rec_write_file(VOIS_STATUS_T state)
{
    INT32 size = 0;
    INT32 ret = -1;
#if defined(__AUTO_CALL_RECORDER__)
    if ((state == VOIS_STATUS_MID_BUFFER_REACHED) || (state == VOIS_STATUS_END_BUFFER_REACHED))
        g_isAutoCallRecordStoped = FALSE;
#endif
    if(get_call_recording_state() == CALL_REC_NOT_REC)
        return;
/*
    if(get_call_recording_state() == CALL_REC_PAUSE
            && state != VOIS_STATUS_NO_MORE_DATA)
        return;
    */
#if defined(__AUTO_CALL_RECORDER__)
    if ((state == VOIS_STATUS_NO_MORE_DATA) && !g_isAutoCallRecordStoped)
        return;
#endif
    switch(state)
    {
        case VOIS_STATUS_MID_BUFFER_REACHED:
            {
#ifdef ENABLE_ADPCM
                UINT32 blk_size=0;
                UINT32 inbufcnt=MMI_CALL_REC_BUFFER_SIZE/4;
                while(inbufcnt>0)
                {
                    blk_size =0;
                    if(adpcm_cntx.block_samples==0){
                        adpcm_encode_block_header(&adpcm_cntx, pbuf_adpcm,(uint32_t*)&blk_size, (int16_t*)(g_p_call_rec_buffer +((MMI_CALL_REC_BUFFER_SIZE/2) -inbufcnt*2)),(uint32_t*)&inbufcnt);
                    }
                    if(inbufcnt>0)
                        adpcm_encode_chunks (&adpcm_cntx, pbuf_adpcm+blk_size,
                                (uint32_t*)&blk_size,(int16_t*)(g_p_call_rec_buffer+((MMI_CALL_REC_BUFFER_SIZE/2) -inbufcnt*2)), (uint32_t*)&inbufcnt);

                    if(adpcm_cntx.block_samples==505)
                        adpcm_cntx.block_samples= 0;
                    if(blk_size>0)
                        ret = FS_Write(g_call_rec_file_handle, pbuf_adpcm, blk_size);
                    else
                        ret=0;
                }
#else
                ret = FS_Write(g_call_rec_file_handle, g_p_call_rec_buffer, MMI_CALL_REC_BUFFER_SIZE/2);
#endif //ENABLE_ADPCM
                hal_HstSendEvent(0xcccc0829);
                AT_TC(g_sw_SPEC, " func :%s ret = %d,", __FUNCTION__, ret);

                break;
            }
        case VOIS_STATUS_END_BUFFER_REACHED:
            {//hal_HstSendEvent(0xcccc0835);
#ifdef ENABLE_ADPCM
                UINT32 blk_size=0;
                UINT32 inbufcnt=MMI_CALL_REC_BUFFER_SIZE/4;
                while(inbufcnt>0){
                    blk_size =0;
                    if(adpcm_cntx.block_samples==0){
                        adpcm_encode_block_header(&adpcm_cntx, pbuf_adpcm,(uint32_t*)&blk_size, (int16_t*)(g_p_call_rec_buffer +(MMI_CALL_REC_BUFFER_SIZE -inbufcnt*2)),(uint32_t*)&inbufcnt);
                    }
                    //multiple of 2 samples for each chunk
                    if(inbufcnt>0)
                        adpcm_encode_chunks (&adpcm_cntx, pbuf_adpcm+blk_size,
                                (uint32_t*)&blk_size,(int16_t*)(g_p_call_rec_buffer+(MMI_CALL_REC_BUFFER_SIZE -inbufcnt*2)),  (uint32_t*)&inbufcnt);
                    if(adpcm_cntx.block_samples==505)
                        adpcm_cntx.block_samples= 0;
                    if(blk_size>0)
                        ret = FS_Write(g_call_rec_file_handle, pbuf_adpcm, blk_size);
                    else
                        ret=0;
                }

#else
                ret= FS_Write(g_call_rec_file_handle,( g_p_call_rec_buffer + MMI_CALL_REC_BUFFER_SIZE/2), MMI_CALL_REC_BUFFER_SIZE/2);
#endif //ENABLE_ADPCM
                AT_TC(g_sw_SPEC, " func :%s ret = %d,", __FUNCTION__, ret);
                break;
            }
        case VOIS_STATUS_NO_MORE_DATA:
            ret = callRec_writeWaveHeader(g_call_rec_file_handle);
            vois_RecordStop();
            break;
        default:
            break;
    }
    if(ret < 0 && (state == VOIS_STATUS_MID_BUFFER_REACHED || state == VOIS_STATUS_END_BUFFER_REACHED) )
    {
        vois_RecordStop();
    }
    else if(ret >= 0 && (state == VOIS_STATUS_MID_BUFFER_REACHED || state == VOIS_STATUS_END_BUFFER_REACHED) )
    {
        return;
    }

    if(g_call_rec_file_handle >= 0)
    {
        FS_Close(g_call_rec_file_handle);
        g_call_rec_file_handle = -1;
    }
    if(g_p_call_rec_buffer != NULL)
    {
        OslMfree(g_p_call_rec_buffer);
        g_p_call_rec_buffer = NULL;
    }
    #ifdef ENABLE_ADPCM
    if(pbuf_adpcm != NULL)
    {
        OslMfree(pbuf_adpcm );
        pbuf_adpcm = NULL;
    }
#endif
    set_call_recording_state(CALL_REC_NOT_REC);
}

static void mod_callrec_stop_record(void)
{
#if defined(__MMI_AUTO_CALL_RECORDER__)
    hal_HstSendEvent(0xca2ab3ef);
    g_isAutoCallRecordStoped = TRUE;
#endif
    mod_call_rec_write_file(VOIS_STATUS_NO_MORE_DATA);
    set_call_recording_state(CALL_REC_NOT_REC);
}
#define TIMER_REAL_TIME_CLOCK 101

VOID AT_CALLREC_AsyncEventProcess(COS_EVENT *pEvent)
{
    CFW_EVENT ev;
    CFW_EVENT *pEv = &ev;
    UINT8 OutStr[50];
    memset(OutStr, 0, 50);
    AT_CosEvent2CfwEvent(pEvent, pEv);
    UINT8 nSim = pEv->nFlag;
    AT_TC(g_sw_SPEC, "AT_CALLREC_AsyncEventProcess :%d ", pEvent->nParam2);
    switch (pEvent->nEventId)
    {
        case EV_CFW_CALL_REC_REQ:
            mod_call_rec_write_file(pEvent->nParam2);
            break;
        default :
            break;
    }
}

void call_rec_cb(VOIS_STATUS_T state)
{
    AT_TC(g_sw_SPEC, "func :%s state=%d", __FUNCTION__,state);
    do
    {
        COS_EVENT ev;
        AT_TC(g_sw_SPEC, " send event continue");
        ev.nEventId = EV_CFW_CALL_REC_REQ;
        ev.nParam1  = NULL;
        ev.nParam2  = (UINT32)state;
        ev.nParam3 =  0;
        COS_SendEvent(g_hCosATTask, &ev, COS_WAIT_FOREVER, COS_EVENT_PRI_NORMAL);
    }while(0);

    return;
}

/**************************************************************************************************
 *                                       FUNCTIONS - API
 **************************************************************************************************/

VOID AT_CALL_StartRecord()
{
    int ret;
    //  if(get_call_recording_state() == CALL_REC_NOT_REC)
    g_call_recTime_remain = 0;

    if(g_p_call_rec_buffer == NULL)
    {
        g_p_call_rec_buffer = OslMalloc(MMI_CALL_REC_BUFFER_SIZE);
        AT_TC(g_sw_SPEC, "====== func :%s rec_buffer= %p,%d", __FUNCTION__, g_p_call_rec_buffer,g_p_call_rec_buffer);
        ASSERT(g_p_call_rec_buffer != NULL);
    }
    memset(g_p_call_rec_buffer, 0x00, MMI_CALL_REC_BUFFER_SIZE);

    AnsiiToUnicodeString(g_rec_filename, CALL_RECORD_FILENAME);

    FS_Delete(g_rec_filename);

    if(g_call_rec_file_handle >= 0 )
        return;
    g_call_rec_file_handle = FS_Open(g_rec_filename, (FS_O_RDWR | FS_O_CREAT ),0);
    if(g_call_rec_file_handle < 0)
    {
        AT_TC(g_sw_SPEC, " func :%s  ERROR !===FS_Open ret =%d", __FUNCTION__, g_call_rec_file_handle);
        return;
    }
    ret = callRec_writeWaveHeader(g_call_rec_file_handle);
    if(ret != 0)
    {
        AT_TC(g_sw_SPEC, " func :%s  ERROR !===callRec_writeWaveHeader ret =%d", __FUNCTION__, ret);
        return;
    }
#ifdef ENABLE_ADPCM
    memset(&adpcm_cntx,0,sizeof(struct adpcm_context));
    adpcm_cntx.num_channels= 1;//mono
    if(pbuf_adpcm== NULL)
    {
        pbuf_adpcm = OslMalloc(ADPCM_BLOCK_SIZE);
    }

#endif //ENABLE_ADPCM

    ret = vois_RecordStart((INT32 *)g_p_call_rec_buffer,MMI_CALL_REC_BUFFER_SIZE,call_rec_cb);

    if(ret != VOIS_ERR_NO)
    {
        AT_TC(g_sw_SPEC, " func :%s  ERROR !===vois_RecordStart ret =%d", __FUNCTION__, ret);
        return;
    }
    set_call_recording_state(CALL_REC_RECDING);
    //SetCMScrnFlag(FALSE);
}
VOID AT_CALL_StopRecord()
{
    if(g_call_rec_file_handle != -1)
    {
        mod_callrec_stop_record();
        //FS_Close(g_call_rec_file_handle);
    }
}
VOID AT_CALL_Play_CallBack(AT_RECORD_ERR_T result)
{
    AT_TC(g_sw_SPEC, "==AT_CALL_Play_CallBack result %x", result);
    MCI_AudioStop();
    FS_Close(g_call_rec_file_handle);
    g_call_rec_file_handle = -1;
    set_call_recording_state(CALL_REC_NOT_REC);
    hal_HstSendEvent(0x1180001);
    hal_HstSendEvent(result);
}
VOID AT_CALL_PlayRecord()
{
    AnsiiToUnicodeString(g_rec_filename, CALL_RECORD_FILENAME);
    g_call_rec_file_handle = FS_Open(g_rec_filename, (FS_O_RDWR), 0);
    if (g_call_rec_file_handle < 0)
    {
        AT_TC(g_sw_GC, "open failing.h=%d", g_call_rec_file_handle);
        FS_Close(g_call_rec_file_handle);
       // g_call_rec_file_handle = FS_Create(g_rec_filename, 0);
    }
    AT_TC(g_sw_GC, "open read file  OK.h=%d", g_call_rec_file_handle);
    
    UINT32 ret = MCI_AudioPlay(0,g_call_rec_file_handle,MCI_TYPE_WAV,AT_CALL_Play_CallBack,0);

    
/*
    if(pmd_GetEarModeStatus())
    {
        DM_SetAudioMode(DM_AUDIO_MODE_HANDSET);
        MCI_AudioSetOutputPath(MCI_PATH_HP, 0);
    }
    else
    {
    */
        if(DM_AUDIO_MODE_LOUDSPEAKER == g_output_path_default)
        {
            DM_SetAudioMode(DM_AUDIO_MODE_LOUDSPEAKER);
            //MCI_AudioSetOutputPath(MCI_PATH_LSP, 0);
            //pmd_EnablePower(4, 1);
        }
        else
        {
            DM_SetAudioMode(DM_AUDIO_MODE_EARPIECE);
            //MCI_AudioSetOutputPath(MCI_PATH_NORMAL, 0);
        }
  //  }


    
}

VOID AT_AUDIO_CmdFunc_Call_Record(AT_CMD_PARA *pParam)
{
    UINT16 nParamLen;
    UINT8 arrRes[120];

    INT32 iRet       = ERR_SUCCESS;
    UINT8 paraCount;
    UINT8 nCallMode;
    FS_INFO psFSInfo;


    UINT8 nSim = AT_SIM_ID(pParam->nDLCI);

    if (pParam == NULL)
    {
        AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

    if (pParam->iType == AT_CMD_TEST)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_StrCpy(arrRes, "+CALLREC: file record(1-0-2)");
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_SET)
    {
        iRet = AT_Util_GetParaCount(pParam->pPara, &paraCount);
        if (iRet != ERR_SUCCESS || paraCount < 1)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }

        nParamLen = 1;
        iRet = AT_Util_GetParaWithRule(pParam->pPara, 0, AT_UTIL_PARA_TYPE_UINT8, &nCallMode, &nParamLen);
        if (iRet != ERR_SUCCESS)
        {
            AT_Result_Err(ERR_AT_CME_PARAM_INVALID, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        if (1 == nCallMode)//start
        {
            if(CALL_REC_NOT_REC != get_call_recording_state())
            {
                if(CALL_REC_RECDING == get_call_recording_state())
                    AT_Sprintf(arrRes, "+CALLREC: %d, reacording now,please stop recorder",nCallMode);
                else if(CALL_REC_PAUSE == get_call_recording_state())
                    AT_Sprintf(arrRes,"CALLREC: %d, recording noe,please stop recorder",nCallMode);
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                return;
            }
            VOIS_AUDIO_CFG_T *pVoisAudioCfg = vois_GetAudioCfg();
            vois_Setup(audioItf, pVoisAudioCfg);
            AT_TC(g_sw_SPEC, "+CALLREC :state--%d\n", get_call_recording_state());
            AT_CALL_StartRecord();
        }
        else if(0 == nCallMode)//stop
        {
           // set_call_recording_state(CALL_REC_NOT_REC);
            AT_TC(g_sw_SPEC, "+CALLREC :state--%d\n", get_call_recording_state());
            AT_CALL_StopRecord();
        }
        else if(2 == nCallMode)//play
        {
         UINT32 uStat = CFW_CcGetCallStatus(nSim);
  
         if(uStat != 0x00)  //CC_STATE_NULL
            {
                AT_Sprintf(arrRes, "+CALLREC:in  calling %d. )",uStat);
                AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                return;
            }
            if(CALL_REC_NOT_REC != get_call_recording_state())
            {
                 AT_Sprintf(arrRes, "+CALLREC: %d, reacording now,please stop recorder",nCallMode);
                 AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
                 return;
            }
            set_call_recording_state(CALL_REC_PAUSE);
            AT_TC(g_sw_SPEC, "+CALLREC :state--%d\n", get_call_recording_state());
            AT_CALL_PlayRecord();
        }/*
        else if(3 == nCallMode)//setup
        {
            VOIS_AUDIO_CFG_T *pVoisAudioCfg = vois_GetAudioCfg();
            vois_Setup(1 , pVoisAudioCfg);
        }*/
          else
        {
            AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
            return;
        }
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_Sprintf(arrRes, "+CALLREC: %d",nCallMode);
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else if (pParam->iType == AT_CMD_READ)
    {
        AT_MemZero(arrRes, sizeof(arrRes));
        AT_Sprintf(arrRes, "+CALLREC: %d", get_call_recording_state());
        AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, arrRes, AT_StrLen(arrRes), pParam->nDLCI, nSim);
        return;
    }
    else
    {
        AT_Result_Err(ERR_AT_CME_EXE_NOT_SURPORT, CMD_ERROR_CODE_TYPE_CME, pParam->nDLCI, nSim);
        return;
    }

}
#endif

