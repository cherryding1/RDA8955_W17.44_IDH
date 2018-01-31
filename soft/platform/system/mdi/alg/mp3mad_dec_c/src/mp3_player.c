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



# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <string.h>

# include "player.h"
# include "decoder.h"
# include "mmf_mp3_api.h"
#include "mp3_global.h"

//API for cool profile
#if 0
#include <cswtype.h>
//#include <fs.h>
#include "ts.h"

#define u16 UINT16
#define u8   UINT8
extern void hal_PXTS_SendProfilingCode(u16 code);
#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
#define CP_ENTRY(id) hal_PXTS_SendProfilingCode((id) & CPMASK)
#define CP_EXIT(id) hal_PXTS_SendProfilingCode(((id) & CPMASK) | CPEXITFLAG)
#endif


# define MPEG_BUFSZ 2000    /* 2.5 s at 128 kbps; 1 s at 320 kbps */
# define FREQ_TOLERANCE 6   /* percent sampling frequency tolerance */

//unsigned char input_buffer[MPEG_BUFSZ]; sheen
unsigned char * pSourceMp3;
unsigned long SMp3Len;

struct mad_stream astream;

struct mad_decoder adecoder;
struct player aplayer;

mad_fixed_t  *xr;//(*xr)[576]; sheen

mad_fixed_t *dtmp=NULL;
char * LAY3_BUFFER=NULL;
char * LAY3_OVERLAP=NULL;
struct mad_frame *pFrame=NULL;
struct mad_synth *pSynth=NULL;

extern enum mad_flow error_default(void *data, struct mad_stream *stream, struct mad_frame *frame);
extern char * mmc_MemMalloc(int32 nsize);
extern void mmc_MemFreeAll();

unsigned char *pOutPcm;
unsigned long pcmLen;

/*
 * NAME:    player_init()
 * DESCRIPTION: initialize player structure
 */
void player_init(struct player *player)
{
    player->verbosity = 0;

    player->options  = 0;
    player->repeat   = 1;

    player->control = PLAYER_CONTROL_DEFAULT;

    player->playlist.entries = 0;
    player->playlist.length  = 0;
    player->playlist.current = 0;

    player->global_start = mad_timer_zero;
    player->global_stop  = mad_timer_zero;

    player->fade_in      = mad_timer_zero;
    player->fade_out     = mad_timer_zero;
    player->gap          = mad_timer_zero;

    player->input.path        = 0;
//  player->input.fd          = -1;

    player->input.data        = 0;//input_buffer;
    player->input.length      = 0;
    player->input.eof         = 0;


    player->output.mode          = AUDIO_MODE_DITHER;
    player->output.attenuation   = MAD_F_ONE;
    player->output.filters       = 0;
    player->output.channels_in   = 0;
    player->output.channels_out  = 0;
    player->output.select        = PLAYER_CHANNEL_DEFAULT;
    player->output.speed_in      = 0;
    player->output.speed_out     = 0;
    player->output.precision_in  = 0;
    player->output.precision_out = 0;
    player->output.path          = 0;
//  player->output.command       = 0;
    /* player->output.resample */
//  player->output.resampled     = 0;

    player->stats.show                  = STATS_SHOW_OVERALL;
    player->stats.label                 = 0;
    player->stats.total_bytes           = 0;
    player->stats.total_time            = mad_timer_zero;
    player->stats.global_timer          = mad_timer_zero;
    player->stats.absolute_timer        = mad_timer_zero;
    player->stats.play_timer            = mad_timer_zero;
    player->stats.global_framecount     = 0;
    player->stats.absolute_framecount   = 0;
    player->stats.play_framecount       = 0;
    player->stats.error_frame           = -1;
    player->stats.mute_frame            = 0;
    player->stats.vbr                   = 0;
    player->stats.bitrate               = 0;
    player->stats.vbr_frames            = 0;
    player->stats.vbr_rate              = 0;
    player->stats.nsecs                 = 0;
    player->stats.audio.clipped_samples = 0;
    player->stats.audio.peak_clipping   = 0;
    player->stats.audio.peak_sample     = 0;
}


static
enum mad_flow decode_input_read(void *data, struct mad_stream *stream)
{
    struct player *player = data;
    struct input *input = &player->input;
    int len;

    if (input->eof)
        return MAD_FLOW_STOP;
    /*
      if (stream->next_frame) {
        memmove(input->data, stream->next_frame,
            input->length = &input->data[input->length] - stream->next_frame);
      }

      memcpy(input->data + input->length, pSourceMp3, SMp3Len);
      */
    input->data= pSourceMp3;

    len=SMp3Len;

    input->eof = 1;

    mad_stream_buffer(stream, input->data, input->length += len);

    return MAD_FLOW_CONTINUE;
}



/*
 * NAME:    decode->header()
 * DESCRIPTION: decide whether to continue decoding based on header
 */
static
enum mad_flow decode_header(void *data, struct mad_header const *header)
{
    struct player *player = data;

    if ((player->options & PLAYER_OPTION_TIMED) &&
            mad_timer_compare(player->stats.global_timer, player->global_stop) > 0)
        return MAD_FLOW_STOP;

    if (player->stats.absolute_framecount)
    {
        /* delay counting first frame */

        ++player->stats.absolute_framecount;
        mad_timer_add(&player->stats.absolute_timer, header->duration);

        ++player->stats.global_framecount;
        mad_timer_add(&player->stats.global_timer, header->duration);

        if ((player->options & PLAYER_OPTION_SKIP) &&
                mad_timer_compare(player->stats.global_timer,
                                  player->global_start) < 0)
            return MAD_FLOW_IGNORE;
    }

    return MAD_FLOW_CONTINUE;
}





/*
 * NAME:    decode->error()
 * DESCRIPTION: handle a decoding error
 */
static
enum mad_flow decode_error(void *data, struct mad_stream *stream,
                           struct mad_frame *frame)
{
    struct player *player = data;

//  TS_OutputText(MMI_TS_ID, " decode_error(player)************************* 0x%x\n",stream->error);
//  printf(" decode_error(player)************************* 0x%x\n",stream->error);

    switch (stream->error)
    {
        case MAD_ERROR_BADDATAPTR:
            return MAD_FLOW_CONTINUE;

        case MAD_ERROR_LOSTSYNC:

        default:
            if (player->verbosity >= -1 &&
                    ((stream->error == MAD_ERROR_LOSTSYNC && !player->input.eof)
                     || stream->sync) &&
                    player->stats.global_framecount != player->stats.error_frame)
            {

                player->stats.error_frame = player->stats.global_framecount;
            }
    }

    if (stream->error == MAD_ERROR_BADCRC)
    {
        if (player->stats.global_framecount == player->stats.mute_frame)
            mad_frame_mute(frame);

        player->stats.mute_frame = player->stats.global_framecount + 1;

        return MAD_FLOW_IGNORE;
    }

    return MAD_FLOW_CONTINUE;
}



int Mp3_DecodeFrame(struct player *player)
{
    enum mad_flow (*error_func)(void *, struct mad_stream *, struct mad_frame *);
    void *error_data;
    int bad_last_frame = 0;
    struct mad_stream *stream;
    struct mad_frame *frame;
    struct mad_synth *synth;
    int result = 0;

    struct mad_decoder *decoder=&adecoder;

    if (decoder->input_func == 0)
        return 0;

    if (decoder->error_func)
    {
        error_func = decoder->error_func;
        error_data = decoder->cb_data;
    }
    else
    {
        error_func = error_default;
        error_data = &bad_last_frame;
    }

    stream=&astream;
    frame  =pFrame;
    synth  =pSynth;


    do
    {
        switch (decoder->input_func(decoder->cb_data, stream))
        {
            case MAD_FLOW_STOP:
                goto done;
            case MAD_FLOW_BREAK:
                goto fail;
            case MAD_FLOW_IGNORE:
                continue;
            case MAD_FLOW_CONTINUE:
                break;
        }

        while (1)
        {

            if (decoder->header_func)
            {
                if (mad_header_decode(&frame->header, stream) == -1)
                {
                    if (!MAD_RECOVERABLE(stream->error))
                        break;

                    switch (error_func(error_data, stream, frame))
                    {
                        case MAD_FLOW_STOP:
                            goto done;
                        case MAD_FLOW_BREAK:
                            goto fail;
                        case MAD_FLOW_IGNORE:
                        case MAD_FLOW_CONTINUE:
                        default:
                            continue;
                    }
                }

                switch (decoder->header_func(decoder->cb_data, &frame->header))
                {
                    case MAD_FLOW_STOP:
                        goto done;
                    case MAD_FLOW_BREAK:
                        goto fail;
                    case MAD_FLOW_IGNORE:
                        continue;
                    case MAD_FLOW_CONTINUE:
                        break;
                }
            }

            if (mad_frame_decode(frame, stream) == -1)
            {
                if (!MAD_RECOVERABLE(stream->error))
                    break;

                switch (error_func(error_data, stream, frame))
                {
                    case MAD_FLOW_STOP:
                        goto done;
                    case MAD_FLOW_BREAK:
                        goto fail;
                    case MAD_FLOW_IGNORE:
                        break;
                    case MAD_FLOW_CONTINUE:
                    default:
                        continue;
                }
            }
            else
                bad_last_frame = 0;

            mad_synth_frame(synth, frame);
            goto done;//sheen
        }
    }
    while (stream->error == MAD_ERROR_BUFLEN);

fail:
    result = -1;

done:

    return result;
}



int MMF_Mp3_OpenDecoder()
{
    int options;

    struct mad_stream *stream;
    struct mad_frame *frame;
    struct mad_synth *synth;

    struct player *player=&aplayer;

    struct mad_decoder *decoder=&adecoder;

    //malloc memory at 20071204
    LAY3_BUFFER=(char *)mmc_MemMalloc(MAD_BUFFER_MDLEN*4);
    if(!LAY3_BUFFER)
        return -1;
    memset32(LAY3_BUFFER, 0, MAD_BUFFER_MDLEN);

    LAY3_OVERLAP=(char *)mmc_MemMalloc(1280*4);
    if(!LAY3_OVERLAP)
        return -1;
    memset32(LAY3_OVERLAP, 0, 1280);

    pFrame=(struct mad_frame *)mmc_MemMalloc(sizeof(struct mad_frame));
    if(!pFrame)
        return -1;
    memset(pFrame, 0, sizeof(struct mad_frame));

    pSynth=(struct mad_synth *)mmc_MemMalloc(sizeof(struct mad_synth));
    if(!pSynth)
        return -1;
    memset(pSynth, 0, sizeof(struct mad_synth));

    xr=(mad_fixed_t*)mmc_MemMalloc(576*8) ;
    if(!xr)
        return -1;
    memset32(xr, 0, 576*2);

    dtmp=(mad_fixed_t *)mmc_MemMalloc(33*18*4) ;
    if(!dtmp)
        return -1;
    memset32(dtmp, 0, 33*18);

    //if(!LAY3_BUFFER || !LAY3_OVERLAP || !pFrame ||!pSynth ||!dtmp)
    //  return -1;

    player_init(player);

    mad_decoder_init(&adecoder, player,
                     decode_input_read,
                     decode_header, decode_error, 0);

    options = 0;
    if (player->options & PLAYER_OPTION_DOWNSAMPLE)
        options |= MAD_OPTION_HALFSAMPLERATE;
    if (player->options & PLAYER_OPTION_IGNORECRC)
        options |= MAD_OPTION_IGNORECRC;

    mad_decoder_options(&adecoder, options);

    stream=&astream;
    frame  =pFrame;
    synth  =pSynth;


    if (decoder->error_func)
    {
        decoder->error_data = decoder->cb_data;
    }
    else
    {
        decoder->error_func = error_default;
        decoder->error_data = &decoder->bad_last_frame;
    }


    mad_stream_init(stream);
    mad_frame_init(frame);
    mad_synth_init(synth);

    mad_stream_options(stream, decoder->options);

    return 0;
}

int MMF_Mp3_CloseDecoder()
{
    int result = 0;

//  mad_frame_finish(&aframe);
    if(pFrame)
    {
        mad_frame_finish(pFrame);
        pFrame=NULL;
    }
    mad_stream_finish(&astream);

   // mmc_MemFreeAll();


    return result;

}

int MMF_Mp3_DecodeFrame(unsigned char ** inputBuf, unsigned int *bytesleft, unsigned char *outputBuf, unsigned int initFlag)
{
    pOutPcm=outputBuf;
    pcmLen=0;

    pSourceMp3=*inputBuf;
    SMp3Len=*bytesleft;

    //here, need to check whether frame length > mpeg_bufsize/2

    Mp3_DecodeFrame(&aplayer);

    aplayer.input.eof=0;
    /*
        if(initFlag){
            pOutPcm=outputBuf;
            pcmLen=0;

            pSourceMp3=*inputBuf;
            SMp3Len=inputLen;

            Mp3_DecodeFrame(&aplayer);

            aplayer.input.eof=0;
        }
    */
    (*bytesleft) -= (unsigned int)(astream.next_frame- astream.this_frame);
    (*inputBuf) += (unsigned int)(astream.next_frame- astream.this_frame);

    return pcmLen;
}

void MMF_Mp3_GetFrmInfo(Mp3FrmInfo *pInfo)
{
    pInfo->bitrate= pFrame->header.bitrate;
    pInfo->nChans= pSynth->pcm.channels;
    pInfo->samprate= pSynth->pcm.samplerate;
    pInfo->bitsPerSample= 16;
    pInfo->outputSamps=pSynth->pcm.channels*pSynth->pcm.length;
}

#ifdef SHEEN_VC_DEBUG
#include <stdio.h>
#include <stdlib.h>

void main(void)
{
    FILE *fin,*fout;
    char *inBuf;
    short *outBuf;
    char *p;
    int byteleft;
    int res,i;
    Mp3FrmInfo info;

    //fin= fopen("F:\\测试文件\\mp3\\波斯王子mjpg_mp3.mp3","rb");
    fin= fopen("F:\\测试文件\\mp3\\mp2_0.mp3","rb");
    fout= fopen("F:\\测试文件\\mp3\\test.pcm","wb");
    inBuf= malloc(2048);
    outBuf= malloc(1152*20);

    res=MMF_Mp3_OpenDecoder();

    byteleft=0;
    i=0;
    while(1)
    {
        res= fread(inBuf+ byteleft,1,2048-byteleft, fin);
        if(res>0)
            byteleft +=res;

        if(byteleft<4)break;

        p= inBuf;
        res=MMF_Mp3_DecodeFrame(&p, &byteleft,outBuf,0);
        MMF_Mp3_GetFrmInfo(&info);

        printf("i=%d,res=%d\n",i,res);
        printf("bitr=%d ch=%d smp=%d \n",info.bitrate,info.nChans,info.samprate);

        if(res<=0)break;
        fwrite(outBuf,1,res, fout);
        memmove(inBuf,p,byteleft);
        i++;
    }

    MMF_Mp3_CloseDecoder();

    fclose(fin);
    fclose(fout);
    free(inBuf);
    free(outBuf);

}
#endif


