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
//#include "mmi_features.h"
//#include "med_global.h"
#include "mmc_sbc.h"



//#define DAF_DECODE

#if defined(DAF_DECODE)




static const UINT16 DAF_BIT_RATE_TABLE_V1[15]= {0, 32, 40, 48, 56, 64, 80, 96, 112,
                                                128, 160, 192, 224, 256, 320
                                               };
static const UINT16 DAF_BIT_RATE_TABLE_V2[15]= {0, 8, 16, 24, 32, 40, 48, 56, 64, 80,
                                                96, 112, 128, 144, 160
                                               };





__inline void PUT_UINT32(UINT8 *ptr, UINT32 u32_t)
{
    ptr[0] = (UINT8)( (u32_t >> 24) & 0xff );
    ptr[1] = (UINT8)( (u32_t >> 16) & 0xff );
    ptr[2] = (UINT8)( (u32_t >> 8) & 0xff );
    ptr[3] = (UINT8)( (u32_t) & 0xff );
}

UINT32 GET_UINT32(UINT8 *ptr)
{
    UINT32 u32_t;
    u32_t = (UINT32)ptr[0] << 24;
    u32_t += (UINT32)ptr[1] << 16;
    u32_t += (UINT32)ptr[2] << 8;
    u32_t += (UINT32)ptr[3];
    return u32_t;
}

INT32 SSHdl_GetFreeSpace(SSHdl *hdl)
{
    INT32 cnt;
    cnt = hdl->rb_read - hdl->rb_write - 1;
    if( cnt < 0 )
        cnt += hdl->rb_size;
    return cnt;
}

INT32 SSHdl_GetDataCount(SSHdl *hdl)
{
    INT32 cnt;
    cnt = hdl->rb_write - hdl->rb_read;
    if( cnt < 0 )
        cnt += hdl->rb_size;
    return cnt;
}

void SSHdl_GetWriteBuffer(SSHdl *hdl, UINT8 **buf, INT32 *buf_len)
{
    INT32 cnt;

    if( hdl->rb_read > hdl->rb_write )
        cnt = hdl->rb_read - hdl->rb_write - 1;
    else if( hdl->rb_read == 0 )
        cnt = hdl->rb_size - hdl->rb_write - 1;
    else
        cnt = hdl->rb_size - hdl->rb_write;
    *buf = &hdl->rb_base[hdl->rb_write];
    *buf_len = cnt;
}

void SSHdl_GetReadBuffer(SSHdl *hdl, UINT8 **buf, INT32 *buf_len)
{
    INT32 cnt;

    if( hdl->rb_write > hdl->rb_read )
        cnt = hdl->rb_write - hdl->rb_read;
    else
        cnt = hdl->rb_size - hdl->rb_read;
    *buf = &hdl->rb_base[hdl->rb_read];
    *buf_len = cnt;
}

void SSHdl_ShiftWritePointer(SSHdl *hdl, INT32 shamt)
{
    hdl->rb_write += shamt;
    if( hdl->rb_write >= hdl->rb_size )
        hdl->rb_write -= hdl->rb_size;
}

void SSHdl_ShiftReadPointer(SSHdl *hdl, INT32 shamt)
{
    hdl->rb_read += shamt;
    if( hdl->rb_read >= hdl->rb_size )
        hdl->rb_read -= hdl->rb_size;
    hdl->rb_read_total += shamt;
}

void SSHdl_SniffNBytes(SSHdl *hdl, UINT32 offset, UINT32 N, UINT8 *dest)
{
    INT32 read = hdl->rb_read;
    read += offset;
    if(read >= hdl->rb_size )
    {
        read -= hdl->rb_size;
    }
    for( ; N>0; N-- )
    {
        *dest++ = hdl->rb_base[read];
        read += 1;
        if( read == hdl->rb_size )
            read = 0;
    }
}



void dafRTPSS_PutData( SSHdl *hdl, UINT8 *buf, UINT32 length )
{
    INT32 cnt;
    INT32 length_t = (INT32)length;


    ass( hdl == NULL );
    if( !is_SSHdl_valid( hdl ) )
        return;

    cnt = SSHdl_GetFreeSpace( hdl );
    if( cnt<length )
    {
        // Buffer overflow
        MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, " Buffer overflow!!  length_t:%d,-cnt:%d,hdl->wait:%d",length_t, -cnt, hdl->wait);

        return;
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
//  MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0, "length_t:%d,cnt:%d,hdl->wait:%d",length_t, cnt, hdl->wait);

    if( hdl->wait == FALSE && cnt >= hdl->rb_threshold )
    {
        hdl->wait = TRUE;
        hdl->callback( 0 );
    }


}

void dafRTPSS_Flush( SSHdl *hdl )
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

const UINT16 DAF_SAMPLE_RATE_TABLE[3] = { 44100, 48000, 32000 };
UINT32 dafGetFrameLen( dafHeaderStruct stDafHeader, UINT32 *uBitRate )
{
    // support MPEG-1,2,2.5 Layer III only
    UINT32 frameLen, sampleRate, bitRate, samplePerFrame = 576;

    // MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, "dafGetFrameLen VersionID=%d, BitRateIndex=%d", stDafHeader.VersionID,stDafHeader.BitRateIndex);

    sampleRate = (UINT32)DAF_SAMPLE_RATE_TABLE[stDafHeader.SampleRateIndex];
    if( stDafHeader.VersionID == 0x2 )
        sampleRate >>= 1;
    else if( stDafHeader.VersionID == 0x0 )
        sampleRate >>= 2;
    else
        samplePerFrame <<= 1;

    if( stDafHeader.VersionID == 0x3 )
        bitRate = DAF_BIT_RATE_TABLE_V1[stDafHeader.BitRateIndex];
    else
        bitRate = DAF_BIT_RATE_TABLE_V2[stDafHeader.BitRateIndex];

    *uBitRate = bitRate;
    frameLen = ( ( samplePerFrame * bitRate * 1000 / 8 ) / sampleRate );

//    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"dafGetFrameLen frameLen=%d,bitRate=%d,sampleRate=%d,samplePerFrame=%d \n",frameLen,bitRate,sampleRate,samplePerFrame);

    return frameLen;
}

UINT32 dafRTPSS_GetData( SSHdl *hdl, UINT8 *buf, UINT32 buf_len, UINT32 *sample_cnt )
{
    dafRTPSSHdlInternal *ihdl = (dafRTPSSHdlInternal *)hdl;
    INT32 cnt, cnt2, write_cnt = 0;
    UINT8 *payload_header = buf;
    UINT32 samples = 0;
    UINT32 buf_len_t = buf_len;

    ass( hdl == NULL );

    if( SSHdl_GetDataCount( hdl ) == 0 )
        return 0;

//    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"dafRTPSS_GetData ihdl->fragment_start=%d\n",ihdl->fragment_start);

    payload_header = buf;
    buf += 4;
    buf_len -= 4;
    ass( buf_len <= 0 );

    if( ihdl->fragment_start != 0 )
    {
        cnt = (INT32)ihdl->last_frame_size - (INT32)ihdl->fragment_start;
        ass( cnt <= 0 );
        if( cnt > buf_len )
            cnt = buf_len;
        cnt2 = SSHdl_GetDataCount( hdl );
        if( cnt > cnt2 )
            cnt = cnt2;

        PUT_UINT32( payload_header, ihdl->fragment_start );
        ihdl->fragment_start += cnt;
        buf_len -= cnt;
        write_cnt += cnt;
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

        ass( ihdl->fragment_start > ihdl->last_frame_size );
        if( ihdl->fragment_start == ihdl->last_frame_size )
            ihdl->fragment_start = 0;
    }
    else
    {
        UINT32 state = 0, break_code, skip_count = 0;
        UINT8 uDafHeaderWord[4];
        INT32 frameLen=0;
        UINT32 bitRate;

        UINT32 headerstream;

        dafHeaderStruct stDafHeader;
        //*(UINT32 *)&stDafHeader = 0;

        memset(&stDafHeader, 0, 4);

        do
        {
            break_code = 0;
            //MCI_TRACE (MCI_AUDIO_TRC,0,"dafRTPSS_GetData statet=%d\n",state);
            switch( state )
            {
                case 0: // search next frame header
                {
                    if( SSHdl_GetDataCount( hdl ) >= 4 )
                    {
                        SSHdl_SniffNBytes( hdl, 0, 4, uDafHeaderWord );
                        headerstream = GET_UINT32( uDafHeaderWord );
                        memcpy(&stDafHeader, &headerstream, 4);


                        // *(UINT32 *)&stDafHeader = GET_UINT32( uDafHeaderWord );

                        if( ( stDafHeader.Sync == ihdl->config_header.Sync ) &&
                                ( stDafHeader.VersionID == ihdl->config_header.VersionID ) &&
                                ( stDafHeader.Layer == ihdl->config_header.Layer ) &&
                                ( stDafHeader.BitRateIndex != 0x0 ) && ( stDafHeader.BitRateIndex != 0xf ) &&
                                ( stDafHeader.SampleRateIndex == ihdl->config_header.SampleRateIndex ) &&
                                ( stDafHeader.ChannelMode == ihdl->config_header.ChannelMode )
                          )
                        {
                            frameLen = dafGetFrameLen( stDafHeader, &bitRate );
                            state = 1;
                        }
                        else
                        {
                            state = 0;
                            SSHdl_ShiftReadPointer( hdl, 1 ); // skip one byte from bitstream
                            skip_count ++;
                        }
                    }
                    else
                    {
                        break_code = 1;
                        MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0, "skip_count:%d",skip_count);
                    }
                }
                break;
                case 1:
                {
                    dafHeaderStruct stNextDafHeader;
                    if( SSHdl_GetDataCount( hdl ) >= (4+frameLen+4) )
                    {
                        SSHdl_SniffNBytes( hdl, frameLen+1, 4, uDafHeaderWord );

                        headerstream = GET_UINT32( uDafHeaderWord );
                        memcpy(&stNextDafHeader, &headerstream, 4);

                        // *(UINT32 *)&stNextDafHeader = GET_UINT32( uDafHeaderWord );
                        if( ( stNextDafHeader.Sync == ihdl->config_header.Sync ) &&
                                ( stNextDafHeader.VersionID == ihdl->config_header.VersionID ) &&
                                ( stNextDafHeader.Layer == ihdl->config_header.Layer ) &&
                                ( stNextDafHeader.BitRateIndex != 0x0 ) && ( stDafHeader.BitRateIndex != 0xf ) &&
                                ( stNextDafHeader.SampleRateIndex == ihdl->config_header.SampleRateIndex ) &&
                                ( stNextDafHeader.ChannelMode == ihdl->config_header.ChannelMode )
                          )
                        {
                            stDafHeader.Padding = 1;
                            frameLen += 1;
                        }
                        else
                        {
                            stDafHeader.Padding = 0;
                        }
                        state = 2;
                    }
                    else
                    {
                        break_code = 10;
                    }
                }
                break;
                case 2: // get one frame
                {
                    ihdl->last_frame_size = frameLen;
                    ihdl->last_bit_rate = bitRate;

                    //             MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0, "skip_count:%d,frameLen:%d,bitRate:%d",skip_count, frameLen, bitRate);

                    cnt = SSHdl_GetDataCount( hdl );
                    if( cnt > buf_len )
                        cnt = buf_len;
                    if( cnt > frameLen )
                        cnt = frameLen;

                    if( ( write_cnt == 0 ) || ( cnt == frameLen ) )
                    {
                        UINT32 temp;
                        if( write_cnt == 0 )
                            PUT_UINT32( payload_header, ihdl->fragment_start );
                        ihdl->fragment_start += cnt;
                        buf_len -= cnt;
                        write_cnt += cnt;

                        cnt2 = sizeof(stDafHeader);
                        temp = GET_UINT32((UINT8 *)&stDafHeader);
                        memcpy(buf, &temp, cnt2 );
                        SSHdl_ShiftReadPointer( hdl, cnt2 );
                        buf += cnt2;
                        cnt -= cnt2;

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

                        ass( ihdl->fragment_start > ihdl->last_frame_size );
                        if( ihdl->fragment_start == ihdl->last_frame_size )
                        {
                            ihdl->fragment_start = 0;
                            state = 0;
                            skip_count = 0;
                        }
                        else
                            break_code = 3;
                        samples += (ihdl->config_header.VersionID==0x3)? 1152: 576;
                    }
                    else
                    {
                        break_code = 2;
                    }
                }
                break;
                default:
                    ass(0);
                    break;
            }
        }
        while( ( break_code == 0 ) && ( buf_len > 0 ) );
    }
    *sample_cnt = (samples * 90000) / DAF_SAMPLE_RATE_TABLE[ihdl->config_header.SampleRateIndex];
    // hdl->wait = FALSE;

    MCI_TRACE ( TSTDOUT | MCI_AUDIO_TRC,0, "buf_len_t:%d,write_cnt:%d,SSHdl_GetDataCount( hdl ):%d,samples:%d,GET_UINT32(payload_header):%d,",
                buf_len_t, write_cnt, SSHdl_GetDataCount( hdl ), samples, GET_UINT32(payload_header));

    if( write_cnt > 0 )
        write_cnt += 4;
    return ( write_cnt );
}

void dafRTPSS_GetDataDone(SSHdl *hdl)
{
    hdl->wait = FALSE;
//   MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"hdl->overflow =%d   ",hdl->overflow );
}

#if defined(__BT_A2DP_PROFILE__)

static struct
{
    A2DP_codec_struct  *a2dp_codec_hdl;
    dafRTPSSHdlInternal *daf_ss_handle;
    A2DP_Callback callback;
} a2dp_daf_strm;

const bt_a2dp_mp3_codec_cap_struct mp3_cap[1] =
{
    {
        0x1, // layer
        TRUE, // CRC
        0xf, // channel_mode
        FALSE, // MPF
        0x3f, // sample_rate
        TRUE, // VBR
        0x7FFE // bit_rate
    }
};

static void a2dp_callback( void *data )
{
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_callback data:%d",data);
    if(a2dp_daf_strm.a2dp_codec_hdl != NULL)
    {
        a2dp_daf_strm.a2dp_codec_hdl->state = (UINT32)data;
        a2dp_daf_strm.callback( A2DP_DATA_NOTIFY, a2dp_daf_strm.a2dp_codec_hdl );
    }
}

static UINT32 a2dp_daf_GetPayload(UINT8 *buf , UINT32 buf_len, UINT32 *sample_cnt)
{
    UINT32 size;
    if( a2dp_daf_strm.daf_ss_handle == NULL )
        return 0;
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_daf_GetPayload -->> buf_len=%d, *sample_cnt=%d", buf_len,*sample_cnt);
    size = dafRTPSS_GetData( (SSHdl *)a2dp_daf_strm.daf_ss_handle, buf, buf_len, sample_cnt );
    //  MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_daf_GetPayload <<--buf_len=%d, *sample_cnt=%d, size=%d", buf_len,*sample_cnt,size);
//  MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"buf[0-10]=%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",buf[0],buf[1],buf[2],buf[3],buf[4], buf[5],buf[6],buf[7],buf[8],buf[9],buf[10]);
    return size;
}

static void a2dp_daf_GetPayloadDone()
{
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_daf_GetPayloadDone\n");

    if( a2dp_daf_strm.daf_ss_handle == NULL )
        return ;

    dafRTPSS_GetDataDone( (SSHdl *)a2dp_daf_strm.daf_ss_handle );

}

static void a2dp_daf_QueryPayloadSize(UINT32 *min, UINT32 *total)
{
    if( a2dp_daf_strm.daf_ss_handle == NULL )
    {
        *min = 0;
        *total = 0;
    }
    else
    {
        UINT32 cnt = SSHdl_GetDataCount( (SSHdl *)a2dp_daf_strm.daf_ss_handle );
        if( a2dp_daf_strm.daf_ss_handle->fragment_start != 0 )
            *min = a2dp_daf_strm.daf_ss_handle->last_frame_size - a2dp_daf_strm.daf_ss_handle->fragment_start;
        else
            *min = a2dp_daf_strm.daf_ss_handle->last_frame_size;
        if(cnt == 0)
            *total = 0;
        else
            *total = cnt + 4;
    }
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_daf_QueryPayloadSize *min=%d,*total=%d", *min,*total);
}

static UINT32 a2dp_daf_SetBitRate(UINT32 bit_rate)
{
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_daf_SetBitRate bit_rate=%d", bit_rate);
    if( a2dp_daf_strm.daf_ss_handle == NULL )
        return 0;
    return a2dp_daf_strm.daf_ss_handle->last_bit_rate;
}

static UINT32 a2dp_daf_AdjustBitRateFromQos(UINT8 qos) // return adjusted bit rate
{
    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"a2dp_daf_AdjustBitRateFromQos qos=%d", qos);
    if( a2dp_daf_strm.daf_ss_handle == NULL )
        return 0;
    return a2dp_daf_strm.daf_ss_handle->last_bit_rate;
}

UINT32 A2DP_DAF_GetMemReq(void)
{
    return 4096;
}
dafRTPSSHdlInternal  ihdl_S;
A2DP_codec_struct a2dp_codec_handle_S;
A2DP_codec_struct *A2DP_DAF_Open( SSHdl **ss_handle, A2DP_Callback pHandler, bt_a2dp_mp3_codec_cap_struct *daf_config_data, UINT8 *buf, UINT32 buf_len)
{
    A2DP_codec_struct *a2dp_codec_handle;


    // ass( buf_len > A2DP_DAF_GetMemReq() );
    ass( daf_config_data == NULL );

    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0,"A2DP_DAF_Open!!");
    {

        dafRTPSSHdlInternal  *ihdl=&ihdl_S;
        //    ihdl = (dafRTPSSHdlInternal *)get_ctrl_buffer( sizeof(dafRTPSSHdlInternal) );
        ihdl->ss_handle.PutData = dafRTPSS_PutData;
        ihdl->ss_handle.Flush = dafRTPSS_Flush;
        ihdl->ss_handle.rb_base = buf;
        ihdl->ss_handle.rb_size = buf_len;
        ihdl->ss_handle.rb_read = 0;
        ihdl->ss_handle.rb_write = 0;
        ihdl->ss_handle.rb_threshold = (1024<(buf_len/2))? 1024: (buf_len/2);
        ihdl->ss_handle.callback = a2dp_callback;
        ihdl->ss_handle.wait = FALSE;
        ihdl->ss_handle.rb_read_total = 0;
        ihdl->last_frame_size = 0;
        ihdl->last_bit_rate = 0;
        ihdl->fragment_start = 0;
        *(UINT32 *)&(ihdl->config_header) = 0;

        if( daf_config_data->layer == 0x1 )
            ihdl->config_header.Layer = 0x1; // Layer III
        else
            ass(1); // not support or error

        if( daf_config_data->channel_mode == 0x01 )
            ihdl->config_header.ChannelMode = 0x1; // joint stereo
        else if( daf_config_data->channel_mode == 0x02 )
            ihdl->config_header.ChannelMode = 0x0; // stereo
        else if( daf_config_data->channel_mode == 0x04 )
            ihdl->config_header.ChannelMode = 0x2; // dual
        else if( daf_config_data->channel_mode == 0x08 )
            ihdl->config_header.ChannelMode = 0x3; // mono
        else
            ass(1); // error

        if( daf_config_data->sample_rate == 0x01 )
            ihdl->config_header.SampleRateIndex = 0x1; // 48000
        else if( daf_config_data->sample_rate == 0x02 )
            ihdl->config_header.SampleRateIndex = 0x0; // 44100
        else if( daf_config_data->sample_rate == 0x04 )
            ihdl->config_header.SampleRateIndex = 0x2; // 32000
        else if( daf_config_data->sample_rate == 0x08 )
            ihdl->config_header.SampleRateIndex = 0x1; // 24000
        else if( daf_config_data->sample_rate == 0x10 )
            ihdl->config_header.SampleRateIndex = 0x0; // 22050
        else if( daf_config_data->sample_rate == 0x20 )
            ihdl->config_header.SampleRateIndex = 0x2; // 16000
        else
            ass(1); // error

        if( daf_config_data->sample_rate & 0x7 )
            ihdl->config_header.VersionID = 0x3;
        else
            ihdl->config_header.VersionID = 0x2;

        ihdl->config_header.Sync = 0x7ff;

        a2dp_daf_strm.daf_ss_handle = ihdl;
        *ss_handle = (SSHdl *)&ihdl->ss_handle;

    }

//  a2dp_codec_handle = (A2DP_codec_struct *)get_ctrl_buffer( sizeof(A2DP_codec_struct) );
    a2dp_codec_handle=&a2dp_codec_handle_S;
    a2dp_codec_handle->GetPayload = a2dp_daf_GetPayload;
    a2dp_codec_handle->GetPayloadDone = a2dp_daf_GetPayloadDone;
    a2dp_codec_handle->QueryPayloadSize = a2dp_daf_QueryPayloadSize;
    a2dp_codec_handle->SetBitRate = a2dp_daf_SetBitRate;
    a2dp_codec_handle->AdjustBitRateFromQos = a2dp_daf_AdjustBitRateFromQos;
    a2dp_codec_handle->state = 0;
    a2dp_daf_strm.a2dp_codec_hdl = a2dp_codec_handle;

    a2dp_daf_strm.callback = pHandler;

    return a2dp_codec_handle;
}

void A2DP_DAF_Close( SSHdl **ss_handle  )
{


    MCI_TRACE (TSTDOUT | MCI_AUDIO_TRC,0, "A2DP_DAF_Close");

//   if( mhdl_handle != NULL )
//      mhdl_handle->ss_handle = NULL;
    *ss_handle = NULL;


    if( a2dp_daf_strm.daf_ss_handle != NULL )
    {
//      void *ptr = (void *)a2dp_daf_strm.daf_ss_handle;
        a2dp_daf_strm.daf_ss_handle->ss_handle.Flush( (SSHdl *)a2dp_daf_strm.daf_ss_handle );
        //   kal_sleep_task(4);
        a2dp_daf_strm.daf_ss_handle = NULL;
        // free_ctrl_buffer( ptr );
    }
    if( a2dp_daf_strm.a2dp_codec_hdl != NULL )
    {
        // void *ptr = (void *)a2dp_daf_strm.a2dp_codec_hdl;
        a2dp_daf_strm.a2dp_codec_hdl = NULL;
        // free_ctrl_buffer( ptr );
    }
}

#endif // ... __BT_A2DP_PROFILE__ 


#endif // ... DAF_DECODE 

BOOL is_SSHdl_valid(SSHdl *hdl)
{

    SSHdl *handle;

    get_sbc_ss_handle( &handle );

#if (defined(__BT_A2DP_PROFILE__) && defined(DAF_DECODE))
    if(( hdl != NULL && hdl == (SSHdl *)a2dp_daf_strm.daf_ss_handle )||( hdl != NULL && hdl == handle)  )
        return TRUE;
#endif

    return FALSE;
}




