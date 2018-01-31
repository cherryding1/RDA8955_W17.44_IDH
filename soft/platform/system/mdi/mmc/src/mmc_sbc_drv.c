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
#include "mcip_debug.h"
#include "string.h"


#include "mci.h"

//#include "med_global.h"

#include "mmc_dafrtpss.h"
#include "mmc_sbc.h"

//#include "sbc.h"



//#define SBC_UNIT_TEST   1 // it shall be set in \make\l1audio\l1audio.def
//#define _SBC_DEBUG_NO_QOS_ // QOS indication wouldn't affect current bit rate
//#define SBC_DEBUG_QOS




#define _SBC_ENALBLE_DROP_ // enable SBC encoder drop frame when MCU busy

#define SBC_WAV_FIXED_START_RATE 165
#define SBC_WAV_FORMAT ((Media_Format)-1)
#define MCU_SB_BUFFER_LEN  8192 // it shall be a multiple of 128, recommended:8192 ; suggest it is a multiple of 384 for all combination
#define MCU_MIN_SB_BUFFER_SIZE 256

#define SBC_FRAME_SUBBAND_DATA_SIZE (sbc.frame_state.subbands * sbc.frame_state.blocks)  //  8 * 16
#define SBC_OVERFLOW_SIZE      1536



UINT16 BT_A2DP_MAX_BIT_RATE = 345;


// UINT8 overflow_buf[SBC_OVERFLOW_SIZE];

//static UINT16 max_sbc_bit_rate;


const bt_a2dp_sbc_codec_cap_struct sbc_cap[1] =
{
    {
        // joint-stereo, 44.1, 32, 16Ksps
#ifndef  __BT_PROFILE_BQB__
        19, // min_bit_pool

        36,// 75, // max_bit_pool hosoal change
#else
        2,  //min_bit_pool
        53, //max_bit_pool
#endif
        0x1, // block_len: 16 only
        0x1, // subband_num: 8 only
        SBC_ALLOCATION_SUPPORT,
        0xf, // sample_rate: all
        0xf, // channel_mode: all
    }
};

sbc_struct sbc;



a2dp_sbc_strm_struct a2dp_sbc_strm;



void  get_sbc_ss_handle(SSHdl  **handle  )
{

    *handle = (SSHdl  *)  a2dp_sbc_strm.sbc_ss_handle;

}



//============================ Fit the Interface of AudioPP Manager ================


//============================ A2DP Service ===============================
INT32 sbc_PutData( SSHdl *hdl, UINT8 *buf, UINT32 length )
{
    INT32 cnt;
    INT32 length_t = (INT32)length;

// MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, " Entry sbc_PutData!! length=%d  ", length);


    ass( hdl == NULL );
    if( !is_SSHdl_valid( hdl ) )
        return -1;


    cnt = SSHdl_GetFreeSpace( hdl );
    if( cnt<length )
    {
        // Buffer overflow
        MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, " Buffer overflow!!  length_t:%d,-cnt:%d,hdl->wait:%d",length_t, -cnt, hdl->wait);
        /*
            hdl->overflow        = TRUE;
            hdl->overflow_size = length;
            ass( length >= SBC_OVERFLOW_SIZE );
             memcpy( hdl->overflow_buf, buf, length );
           */
        return -2;
        // ass(0);
    }

    while( length>0 )
    {
        UINT8 *dest;
        SSHdl_GetWriteBuffer( hdl, &dest, &cnt );
        if( cnt > length )
            cnt = length;
        memcpy( dest, buf, cnt );
        SSHdl_ShiftWritePointer( hdl, cnt );
        buf += cnt;
        length -= cnt;
    }

    cnt = SSHdl_GetDataCount( hdl );
//   MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0, " Entry sbc_PutData!! length_t:%d,cnt:%d,hdl->wait:%d",length_t, cnt, hdl->wait);

    if( hdl->wait == FALSE && cnt >= hdl->rb_threshold )
    {

        hdl->wait = TRUE;
        hdl->callback( 0 );
    }

    return 0;
}

void sbc_Flush( SSHdl *hdl )
{
    INT32 cnt;
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"dafRTPSS_Flush ");
    ass( hdl == NULL );
    if( !is_SSHdl_valid( hdl ) )
        return;

    cnt = SSHdl_GetDataCount( hdl );

    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, "-cnt:%d,hdl->wait:%d", cnt, hdl->wait);


    if( cnt > 0 )
        hdl->callback( (void *)2 );
}

static void a2dp_sbc_callback( void *data )
{
    //kal_prompt_trace(0,"a2dp_callback data:%d",data);
    if(a2dp_sbc_strm.a2dp_codec_hdl != NULL)
    {
        a2dp_sbc_strm.a2dp_codec_hdl->state = (UINT32)data;
        a2dp_sbc_strm.callback( A2DP_DATA_NOTIFY, a2dp_sbc_strm.a2dp_codec_hdl );
    }
}
static UINT32 sbc_SetBitRate(UINT32 bit_rate)
{
    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_SetBitRate bit_rate=%d",bit_rate);


    return bit_rate ;

}

static UINT32 sbc_AdjustBitRateFromQos(UINT8 qos) // return adjusted bit rate
{
    UINT32 bit_rate_prev = sbc.current_bit_rate;

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_AdjustBitRateFromQos qos=%d, bit_rate_prev=%d",qos, bit_rate_prev);

    return   0;//( sbc.current_bit_rate );
}

static VOID sbc_QueryPayloadSize(UINT32 *min, UINT32 *total)
{



    if( a2dp_sbc_strm.sbc_ss_handle == NULL )
    {
        *min = 0;
        *total = 0;
    }
    else
    {
        UINT32 cnt = SSHdl_GetDataCount( (SSHdl *)a2dp_sbc_strm.sbc_ss_handle );
        /*    // TODO
              if( a2dp_sbc_strm.sbc_ss_handle->fragment_start != 0 )
                 *min = a2dp_sbc_strm.sbc_ss_handle->last_frame_size - a2dp_sbc_strm.sbc_ss_handle->fragment_start;
              else
                 *min = a2dp_sbc_strm.sbc_ss_handle->last_frame_size;
        */

        *min = a2dp_sbc_strm.sbc_ss_handle->current_frame_size;

        if(cnt == 0)
            *total = 0;
        else
            *total = cnt + 1;
    }



//   MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_QueryPayloadSize *min=%d, *total=%d",*min,*total);
}


UINT32 sbc_GetData( SSHdl *hdl, UINT8 *buf, UINT32 buf_len, UINT32 *sample_cnt )
{
    UINT8 uSbcHeaderWord[4];
    UINT32 headerstream,skip_num=0;
    sbcHeaderStruct stSbcHeader;
    UINT32 get_length=0,frame_length,offset =0;
    UINT8 media_head=0;
    UINT16 nframe=0;
    sbc_struct*ihdl = (sbc_struct *)hdl;
    ass( hdl == NULL );

    if( SSHdl_GetDataCount( hdl ) == 0 )
        return 0;
//   MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_GetData is called,ihdl->current_frame_size=%d",ihdl->current_frame_size);
    frame_length=ihdl->current_frame_size;
    nframe = (UINT16)   buf_len/frame_length;

    do
    {

        if( SSHdl_GetDataCount( hdl ) >= 4 )
        {
            SSHdl_SniffNBytes( hdl, 0, 4, uSbcHeaderWord );

            headerstream = GET_UINT32( uSbcHeaderWord );
            memcpy(&stSbcHeader, &headerstream, 4);


            if((stSbcHeader.Sync==ihdl->config_header.Sync)&&
                    (stSbcHeader.Subbands==ihdl->config_header.Subbands)&&
                    //  (stSbcHeader.Sample_rate==ihdl->config_header.Sample_rate)&&
                    //      (stSbcHeader.ChannelMode==ihdl->config_header.ChannelMode)&&
                    (stSbcHeader.Blocks==ihdl->config_header.Blocks)&&
                    (stSbcHeader.Bitpool==ihdl->config_header.Bitpool)&&
                    (stSbcHeader.Allocation_method==ihdl->config_header.Allocation_method)
              )
            {

                get_length=nframe*(frame_length);
                media_head=nframe;
                if(SSHdl_GetDataCount( hdl ) >(get_length+4))
                {
                    offset=get_length;
                }
                else
                {

                    get_length=(nframe-1)*(frame_length);
                    media_head=(nframe-1);


                    if(SSHdl_GetDataCount( hdl ) >(get_length+4))
                        offset=get_length;
                    else
                    {
                        MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_GetData rb no enough data");
                        break;
                    }
                }


                break;
            }
            else
            {
                MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"skip 1 byte");

                SSHdl_ShiftReadPointer( hdl, 1 );//skip 1 byte
                if((++skip_num)>frame_length)
                {
                    skip_num=0;
                    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"***skip_num>frame_length");
                    break;
                    //  ass(1);
                }


            }

        }
        else
        {
            MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"rb number <4bytes");
            break;
        }

    }
    while(1);

    skip_num=0;
    if(offset )
    {
        UINT8 flag=0;
        do
        {
            SSHdl_SniffNBytes( hdl, offset, 4, uSbcHeaderWord );

            headerstream = GET_UINT32( uSbcHeaderWord );
            memcpy(&stSbcHeader, &headerstream, 4);

            if((stSbcHeader.Sync==ihdl->config_header.Sync)&&
                    (stSbcHeader.Subbands==ihdl->config_header.Subbands)&&
                    //  (stSbcHeader.Sample_rate==ihdl->config_header.Sample_rate)&&
                    //      (stSbcHeader.ChannelMode==ihdl->config_header.ChannelMode)&&
                    (stSbcHeader.Blocks==ihdl->config_header.Blocks)&&
                    (stSbcHeader.Bitpool==ihdl->config_header.Bitpool)&&
                    (stSbcHeader.Allocation_method==ihdl->config_header.Allocation_method)
              )
            {
                flag=1;
                break;
            }
            else
            {
                offset++;
                get_length++;
                MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"skip 1 byte next packet");
                if((++skip_num)>frame_length)
                {
                    skip_num=0;
                    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"***skip 1 byte next packet>frame_length");
                    //ass(1);
                    break;
                }

            }

        }
        while(1);
        //  MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"flag=%d",flag);

        if(flag)
        {
            UINT32 cnt=get_length, cnt2=0;
            *buf=media_head;
            buf++;
            buf_len--;
            //if(buf_len<cnt)        ??????
            while( cnt > 0 )
            {
                UINT8 *src;
                SSHdl_GetReadBuffer( hdl, &src, &cnt2 );
                if( cnt2 > cnt )
                    cnt2 = cnt;
                memcpy( buf, src, cnt2 );
                SSHdl_ShiftReadPointer( hdl, cnt2 );
                buf += cnt2;
                cnt -= cnt2;
            }
            //      MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_GetData success ~!~!get_length=%d",get_length);
            return get_length+1;
        }
        else
        {

            MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_GetData flag =0!!");
            return 0;
        }


    }
    else
    {
        MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc rb is null!!");
        return 0;

    }



}


static UINT32 sbc_GetPayload(UINT8 *buf , UINT32 buf_len, UINT32 *sample_cnt)
{
    uint32 size;
//  MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_GetPayload is called a2dp_sbc_strm.sbc_ss_handle=0x%x ",a2dp_sbc_strm.sbc_ss_handle);
    if( a2dp_sbc_strm.sbc_ss_handle == NULL )
        return 0;
    size = sbc_GetData( (SSHdl *)a2dp_sbc_strm.sbc_ss_handle, buf, buf_len, sample_cnt );
//  MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"ssize=%d",size);

    return size;


}
/* SBC driver interface */

//extern int32 mmc_SendEvent(HANDLE hTask, uint32 nEventId);
void sbc_GetDataDone(SSHdl *hdl)
{
    hdl->wait = FALSE;
    /* if( hdl->overflow)
    mmc_SendEvent(GetMCITaskHandle(MBOX_ID_MMC), MSG_MMC_AUDIODEC_A2DP);
      */
}

static VOID sbc_GetPayloadDone()
{
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"sbc_GetPayloadDone\n");
    if( a2dp_sbc_strm.sbc_ss_handle == NULL )
        return ;

    sbc_GetDataDone( (SSHdl *)a2dp_sbc_strm.sbc_ss_handle );

}



static UINT32 sbc_calc_bitrate_from_bitpool( UINT32 bitpool, UINT8 nrof_subbands, UINT8 nrof_blocks, UINT8 channel_mode, UINT32 fs, UINT32 *frame_length )
{
    UINT8 nrof_channels, join;
    UINT32 frame_length_t, bitrate;

    nrof_channels = ( channel_mode==SBC_CM_MONO )? 1: 2;
    join = ( channel_mode==SBC_CM_JOINT_STEREO)? 1: 0;

    frame_length_t = 4 + ( 4 * nrof_subbands * nrof_channels ) / 8;
    if( channel_mode==SBC_CM_MONO || channel_mode==SBC_CM_DUAL_CHANNEL )
        frame_length_t += (nrof_blocks * nrof_channels * bitpool) / 8;
    else if( channel_mode==SBC_CM_STEREO || channel_mode==SBC_CM_JOINT_STEREO )
        frame_length_t += ( join * nrof_subbands + nrof_blocks * bitpool ) / 8;
    else
    {
        ass(1);
        //  EXT_ASSERT( 0, channel_mode, 0, 0 );
        return( 0 );
    }

    bitrate = ( 8 * frame_length_t * fs ) / ( nrof_subbands * nrof_blocks );
    bitrate = (bitrate + 999) / 1000; // in kb/s with rounding

    if( frame_length!=NULL )
        *frame_length = frame_length_t;
    return( bitrate );
}


static UINT32 sbc_config(bt_a2dp_sbc_codec_cap_struct *sbc_config_data)
{
    UINT8 bitpool_t;
    UINT32 bit_rate;
    UINT32 frame_length_t=0;
    UINT32 headerstream;;
    if( sbc_config_data!=NULL )
    {
        UINT8 header_data = 0;

        if( sbc_config_data->block_len==1 )
        {
            sbc.frame_state.blocks = 16;
            header_data = SBC_NB_16<<4;
        }
        else if( sbc_config_data->block_len==2 )
        {
            sbc.frame_state.blocks = 12;
            header_data = SBC_NB_12<<4;
        }
        else if( sbc_config_data->block_len==4 )
        {
            sbc.frame_state.blocks = 8;
            header_data = SBC_NB_8<<4;
        }
        else if( sbc_config_data->block_len==8 )
        {
            sbc.frame_state.blocks = 4;
            header_data = SBC_NB_4<<4;
        }
        else
        {
            ass(1);
            //     EXT_ASSERT(0, sbc_config_data->block_len, 0, 0);
        }

        if( sbc_config_data->subband_num==1 )
        {
            sbc.frame_state.subbands = 8;
            header_data |= SBC_SB_8;
        }
        else if( sbc_config_data->subband_num==2 )
        {
            sbc.frame_state.subbands = 4;
            header_data |= SBC_SB_4;
        }
        else
        {
            ass(1);
            //      EXT_ASSERT(0, sbc_config_data->subband_num, 0, 0);
        }

        if( sbc_config_data->alloc_method==1 )
        {
            sbc.frame_state.allocation_method = SBC_AM_LOUDNESS;
            header_data |= SBC_AM_LOUDNESS<<1;
        }
        else if( sbc_config_data->alloc_method==2 )
        {
            sbc.frame_state.allocation_method = SBC_AM_SNR;
            header_data |= SBC_AM_SNR<<1;
        }
        else
        {
            ass(1);
            //    EXT_ASSERT(0, sbc_config_data->alloc_method, 0, 0);
        }

        if( sbc_config_data->sample_rate==1 )
        {
            sbc.frame_state.sampling_frequency = 48000;
            header_data |= SBC_FS_48<<6;
        }
        else if( sbc_config_data->sample_rate==2 )
        {
            sbc.frame_state.sampling_frequency = 44100;
            header_data |= SBC_FS_44<<6;
        }
        else if( sbc_config_data->sample_rate==4 )
        {
            sbc.frame_state.sampling_frequency = 32000;
            header_data |= SBC_FS_32<<6;
        }
        else if( sbc_config_data->sample_rate==8 )
        {
            sbc.frame_state.sampling_frequency = 16000;
            header_data |= SBC_FS_16<<6;
        }
        else
        {
            ass(1);
            //      EXT_ASSERT(0, sbc_config_data->sample_rate, 0, 0);
        }

        if( sbc_config_data->channel_mode==1 )
        {
            sbc.frame_state.channel_mode = SBC_CM_JOINT_STEREO;
            header_data |= SBC_CM_JOINT_STEREO<<2;
        }
        else if( sbc_config_data->channel_mode==2 )
        {
            sbc.frame_state.channel_mode = SBC_CM_STEREO;
            header_data |= SBC_CM_STEREO<<2;
        }
        else if( sbc_config_data->channel_mode==4 )
        {
            sbc.frame_state.channel_mode = SBC_CM_DUAL_CHANNEL;
            header_data |= SBC_CM_DUAL_CHANNEL<<2;
        }
        else if( sbc_config_data->channel_mode==8 )
        {
            sbc.frame_state.channel_mode = SBC_CM_MONO;
            header_data |= SBC_CM_MONO<<2;
        }
        else
        {
            ass(1);
            //       EXT_ASSERT(0, sbc_config_data->channel_mode, 0, 0);
        }

        if( sbc.frame_state.channel_mode==SBC_CM_MONO )
            sbc.frame_state.channels = 1;
        else
            sbc.frame_state.channels = 2;

        sbc.frame_state.sbc_frame_header_data = header_data;

        sbc.max_bit_pool = sbc_config_data->max_bit_pool;
        sbc.min_bit_pool = sbc_config_data->min_bit_pool;
    }



// MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_config bit_rate=%d",bit_rate);

#ifndef __BT_PROFILE_BQB__

    headerstream =  (SBC_SYNCWORD<<24| sbc.frame_state.sbc_frame_header_data<<16|SBC_BIT_POOL<<8|0);

    memcpy(&sbc.config_header, &headerstream, 4);

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc.config_header=%x ,  sampling_frequency = %d  ",sbc.config_header,  sbc.frame_state.sampling_frequency  );

    bit_rate =sbc_calc_bitrate_from_bitpool( SBC_BIT_POOL, sbc.frame_state.subbands, sbc.frame_state.blocks, sbc.frame_state.channel_mode, sbc.frame_state.sampling_frequency, &frame_length_t );

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_config bit_rate=%d",bit_rate);

    sbc.frame_state.bitpool = SBC_BIT_POOL;
    sbc.current_frame_size = frame_length_t;
#else

    if(sbc.min_bit_pool ==sbc.max_bit_pool)
        bitpool_t = sbc.max_bit_pool;
    else if(SBC_BIT_POOL >sbc.min_bit_pool && SBC_BIT_POOL<sbc.max_bit_pool)
        bitpool_t = SBC_BIT_POOL;
    else
        bitpool_t = sbc.max_bit_pool;


    headerstream =  (SBC_SYNCWORD<<24| sbc.frame_state.sbc_frame_header_data<<16|bitpool_t<<8|0);

    memcpy(&sbc.config_header, &headerstream, 4);

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc.config_header=%x ,  sampling_frequency = %d  ",sbc.config_header,  sbc.frame_state.sampling_frequency  );

    bit_rate =sbc_calc_bitrate_from_bitpool( bitpool_t, sbc.frame_state.subbands, sbc.frame_state.blocks, sbc.frame_state.channel_mode, sbc.frame_state.sampling_frequency, &frame_length_t );

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"sbc_config bit_rate=%d,bitpool_t=%d",bit_rate,bitpool_t);

    sbc.frame_state.bitpool = bitpool_t;
    sbc.current_frame_size = frame_length_t;
#endif

    /*
       SBC_TRACE( TRACE_INFO, L1SBC_CONFIG_REPORT,
          sbc.frame_state.sampling_frequency, sbc.frame_state.channel_mode,
          sbc.min_bit_pool, sbc.frame_state.bitpool, sbc.max_bit_pool );
      */
//kal_prompt_trace(0, "sbc_config bit_rate=%d",bit_rate);

    return( bit_rate );
}




UINT32 SBC_GetMemReq(void)
{
    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0," L1SBC_ENTER_SBC_GETMEMREQ" );
    return 8*1024; // recommended minimum sbc frame buffer length , it is adjustable
}


A2DP_codec_struct a2dp_codec_handle_Sbc;

A2DP_codec_struct *SBC_Open(
    SSHdl **ss_handle,
    A2DP_Callback pHandler,
    bt_a2dp_sbc_codec_cap_struct *sbc_config_data,
    UINT8 *buf,
    UINT32 buf_len )
{
    A2DP_codec_struct *a2dp_codec_handle;

    //  ass( buf_len>SBC_GetMemReq() );

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0,"SBC_Open buf_len=%d",buf_len);


    {


        sbc_struct  *ihdl=&sbc;

        ihdl->ss_handle.PutData = sbc_PutData;
        ihdl->ss_handle.Flush = sbc_Flush;
        ihdl->ss_handle.rb_base = buf;
        ihdl->ss_handle.rb_size =     buf_len;
        ihdl->ss_handle.rb_read = 0;
        ihdl->ss_handle.rb_write = 0;
        ihdl->ss_handle.rb_threshold =   (2048<(buf_len/2))? 2048: (buf_len/2);
        ihdl->ss_handle.callback = a2dp_sbc_callback;
        ihdl->ss_handle.wait = FALSE;
        ihdl->ss_handle.rb_read_total = 0;
        *(UINT32 *)&(ihdl->config_header) = 0;
        /*   ihdl->ss_handle.overflow= FALSE;
          ihdl->ss_handle.overflow_size= 0;
           ihdl->ss_handle.overflow_buf= overflow_buf;
        */
        sbc.current_bit_rate = sbc_config( sbc_config_data ); // sbc.current_frame_size shall be set here

        a2dp_sbc_strm.sbc_ss_handle = ihdl;
        *ss_handle = (SSHdl *)&ihdl->ss_handle;

        MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0," a2dp_sbc_strm.sbc_ss_handle =0x%x,  config_header = 0x%x   ",a2dp_sbc_strm.sbc_ss_handle,ihdl->config_header);

        a2dp_codec_handle=&a2dp_codec_handle_Sbc;
        a2dp_codec_handle->GetPayload = sbc_GetPayload;
        a2dp_codec_handle->GetPayloadDone = sbc_GetPayloadDone;
        a2dp_codec_handle->QueryPayloadSize = sbc_QueryPayloadSize;
        a2dp_codec_handle->SetBitRate = sbc_SetBitRate;
        a2dp_codec_handle->AdjustBitRateFromQos = sbc_AdjustBitRateFromQos;
        a2dp_codec_handle->state = 0;

        a2dp_sbc_strm.a2dp_codec_hdl = a2dp_codec_handle;
        a2dp_sbc_strm.callback = pHandler;



    }

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0," sbc.aud_id=%d, sbc.current_bit_rate=%d",sbc.aud_id,sbc.current_bit_rate);



    return a2dp_codec_handle;
    // return &sbc.a2dp_codec_hdl;
}


void SBC_Close(SSHdl **ss_handle)
{


    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0," SBC_Close!!");

    *ss_handle = NULL;



}



//#endif // ...defined(__BT_A2DP_SUPPORT__)

