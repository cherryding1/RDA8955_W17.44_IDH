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

#include "fs.h"
#include "mpegheader_parser.h"
#include "mcip_debug.h"
#include <errorcode.h>
#include "mmc.h"


static const int sampRateTab[NUM_SAMPLE_RATES] = {
    96000, 88200, 64000, 48000, 44100, 32000,
        24000, 22050, 16000, 12000, 11025,  8000
};
static const unsigned int mpeg_mpa_bitrate_tab[2][3][15] = {
    { {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 },
    {0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384 },
    {0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320 } },
    { {0, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256},
    {0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160},
    {0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160}
    }
};


static unsigned long const bitrate_table[5][15] = {
    /* MPEG-1 */
    { 0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,  /* Layer I   */
        256000, 288000, 320000, 352000, 384000, 416000, 448000 },
    { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,  /* Layer II  */
    128000, 160000, 192000, 224000, 256000, 320000, 384000 },
    { 0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,  /* Layer III */
    112000, 128000, 160000, 192000, 224000, 256000, 320000 },

    /* MPEG-2 LSF */
    { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,  /* Layer I   */
    128000, 144000, 160000, 176000, 192000, 224000, 256000 },
    { 0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,  /* Layers    */
    64000,  80000,  96000, 112000, 128000, 144000, 160000 } /* II & III  */
};
static const unsigned int mpeg_mpa_freq_tab[3] = { 44100, 48000, 32000 };




static int mpeg_unpack_adts_header(adts_header *adts, struct mpeg_stream *stream);

static int mpeg_id3v2_match(const unsigned char *buf, const char * magic);
static int mpeg_id3v2_tag_len(const unsigned char * buf);
static  int mpeg_mpa_check_header(unsigned int header);
static int mpeg_header_probe(MPADecodeHeader *headerinfo,struct mpeg_stream *stream);
static int mpeg_mpa_decode_header( unsigned int data, MPADecodeHeader *headerinfo);
static int mpeg_audio_decode_header(MPADecodeHeader *s, unsigned int header);
static void mpeg_bit_init(struct mpeg_bitptr *bitptr, unsigned char const *byte);
static void mpeg_bit_skip(struct mpeg_bitptr *bitptr, unsigned int len);
static unsigned long mpeg_bit_read(struct mpeg_bitptr *bitptr, unsigned int len);
static int Xing_parse(struct xing_Fraunhofer_info *xing, struct mpeg_stream *stream);
static int Fraunhofer_parse(struct xing_Fraunhofer_info *Fraunhofer,struct mpeg_stream *stream);

static void mpeg_set_stream_buffer(int file,struct mpeg_stream *stream,
                       unsigned char const *buffer, unsigned int length);
static int get_sample_rate(const uint8_t sr_index);
static int program_config_element(program_config *pce, struct mpeg_stream *stream);
static int  mpeg_unpack_ADIF_header(adif_header *adif, struct mpeg_stream *stream);

static int  mpeg_adts_header_check(unsigned int  header);

int mpeg_stream_init(int file,unsigned char *inputbuf,unsigned int datalength,struct mpeg_stream *stream) {

    int readlen = 0,TotalFileSize,checklength = 2048;
    if ((TotalFileSize=FS_GetFileSize(file))<= 0)
    {
        return -1;
    }
    if (TotalFileSize < checklength) {

        checklength = TotalFileSize;
    }


    if ((stream->skiplen = mpeg_stream_error_check(file,stream->audiotype,inputbuf,checklength)) < 0){

         return -1;
    }

    readlen = FS_Read(file,inputbuf,datalength);//try to read 1kB.

    if(readlen < datalength){

        return -1;

    }

    mpeg_set_stream_buffer(file,stream,inputbuf,readlen);

    return 0;
}


int  mpeg_adts_header_check(unsigned int header){

     //layer
    if (((header >> 17) & 0x3) != 0)
        return -1;
    //profile
    if (((header >>14)& 0x3) != 1)
        return -1;
    //sf_index
    if (((header >> 10) & 0x0f) >= (NUM_SAMPLE_RATES))
        return -1;
    //channel_configuration
    if (((header >> 6) & 0x7) > 2)
        return -1;

    return 0;

}




int mpeg_unpack_adts_header(adts_header *adts, struct mpeg_stream *stream)
{


    adts->syncword = (uint16_t)mpeg_bit_read(&stream->ptr,12);
    if (adts->syncword != 0xFFF)
    {
        return -1 ;
    }
    adts->id = mpeg_bit_read(&stream->ptr,1);

    adts->layer = mpeg_bit_read(&stream->ptr,2);
    adts->protection_absent = mpeg_bit_read(&stream->ptr,1);
    adts->profile = mpeg_bit_read(&stream->ptr,2);
    adts->sf_index = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    adts->private_bit = mpeg_bit_read(&stream->ptr,1);
    adts->channel_configuration = (uint8_t)mpeg_bit_read(&stream->ptr,3);
    adts->original = mpeg_bit_read(&stream->ptr,1);
    adts->home = mpeg_bit_read(&stream->ptr,1);


    adts->copyright_identification_bit =mpeg_bit_read(&stream->ptr,1);
    adts->copyright_identification_start = mpeg_bit_read(&stream->ptr,1);
    adts->aac_frame_length = (uint16_t)mpeg_bit_read(&stream->ptr,13);
    adts->adts_buffer_fullness = (uint16_t)mpeg_bit_read(&stream->ptr,11);
    adts->no_raw_data_blocks_in_frame = (uint8_t)mpeg_bit_read(&stream->ptr,2);

    if (adts->layer != 0 || adts->profile != AAC_PROFILE_LC ||
        adts->sf_index >= NUM_SAMPLE_RATES || adts->channel_configuration > 2){
        return -1;
    }

    return 0;
}
int mpeg_id3v2_match(const unsigned char *buf, const char * magic)
{
    return  buf[0]         == magic[0] &&
        buf[1]         == magic[1] &&
        buf[2]         == magic[2] &&
        buf[3]         != 0xff &&
        buf[4]         != 0xff &&
        (buf[6] & 0x80) ==    0 &&
        (buf[7] & 0x80) ==    0 &&
        (buf[8] & 0x80) ==    0 &&
        (buf[9] & 0x80) ==    0;
}


int mpeg_id3v2_tag_len(const unsigned char * buf)
{
    int len = ((buf[6] & 0x7f) << 21) +
        ((buf[7] & 0x7f) << 14) +
        ((buf[8] & 0x7f) << 7) +
        (buf[9] & 0x7f) +
        ID3v2_HEADER_SIZE;
    if (buf[5] & 0x10)
        len += ID3v2_HEADER_SIZE;
    return len;
}

int mpeg_mpa_check_header(unsigned int header){
    /* header */
    if ((header & 0xffe00000) != 0xffe00000)
        return -1;
    /* layer check */
    if ((header & (3<<17)) == 0 || (header & (3<<17)) == 3<<17) //skip layer1
        return -1;
    /* bit rate */
    if ((header & (0xf<<12)) == 0xf<<12)
        return -1;
    /* frequency */
    if ((header & (3<<10)) == 3<<10)
        return -1;
    return 0;
}



int mpeg_stream_info_get(mpeg_stream_play_info *playinfo, struct mpeg_stream stream){

    int status;
    unsigned int data,durition;
    MPADecodeHeader headerinfo;
    adif_header adif;
    struct xing_Fraunhofer_info xing_Fraunhofer;
    const int xing_offtbl[2][2] = {{32, 17}, {17,9}};
    //status = xing_parse(&xing,&stream.ptr);
    if (stream.audiotype == MUSIC_TYPE_MP3) {

        data = mpeg_bit_read(&stream.ptr,32);
        status = mpeg_mpa_decode_header(data, &headerinfo);
        if (status < 0) {

            MCI_TRACE (MCI_AUDIO_TRC,0,"mpeg_mpa_decode_header fail");
            return status;
        }

        xing_Fraunhofer.xing_offset = (xing_offtbl[headerinfo.lsf == 1][headerinfo.nb_channels==1]);
        xing_Fraunhofer.Fraunhofer_offset = 36;/// Fraunhofer VBR header is hardcoded 'VBRI' at offset 0x24 (36)

        if (Xing_parse(&xing_Fraunhofer,&stream) == 0 || Fraunhofer_parse(&xing_Fraunhofer,&stream) == 0)
        {

            if (xing_Fraunhofer.iscbr == mpeg_ISCBR) {

                playinfo->bitRate = headerinfo.bit_rate;
            } else {

                durition =  (xing_Fraunhofer.frames * headerinfo.spf)/headerinfo.sample_rate;
                playinfo->bitRate =  (xing_Fraunhofer.bytes*8)/durition;

            }

        } else {

            status = mpeg_header_probe(&headerinfo,&stream);

            if (status < 0)
            {
                 MCI_TRACE (MCI_AUDIO_TRC,0,"mp3 mpeg_header_probe fail");
                return -1;
            }

            playinfo->bitRate = headerinfo.bit_rate;
        }

         playinfo->nb_channels = headerinfo.nb_channels;
         playinfo->sampleRate = headerinfo.sample_rate;

    } else {

        if (mpeg_unpack_ADIF_header(&adif,&stream) ==0)
        {

            playinfo->sampleRate = get_sample_rate(adif.pce[0].sf_index);
            if ( playinfo->sampleRate == -1){

                MCI_TRACE (MCI_AUDIO_TRC,0,"AAC mpeg_unpack_ADIF_header fail");
                return -1;
            }

            playinfo ->nb_channels = adif.pce[0].channels;
            playinfo->bitRate = adif.bitrate;

        } else {

            status = mpeg_header_probe(&headerinfo,&stream);

            if (status < 0){
                MCI_TRACE (MCI_AUDIO_TRC,0,"AAC mpeg_header_probe fail");
                return -1;
            }

            playinfo->bitRate = headerinfo.bit_rate;
            playinfo->nb_channels = headerinfo.nb_channels;
            playinfo->sampleRate = headerinfo.sample_rate;
        }
    }

    return 0;
}

int mpeg_header_probe(MPADecodeHeader *headerinfo,struct mpeg_stream *stream){

    int readbtes=0;
    unsigned long frames ,errorcount = 0,duraition,detectlen=0;
    long  bitratesum=0,totalframes=0;

    unsigned int header;
    unsigned char *buf, *buf0, *buf2, *end;
    adts_header adtsheader;

    if (DETECTLEN < (stream->buflength)){
       detectlen = DETECTLEN;
    } else {

       detectlen = stream->buflength;

    }

    if ((readbtes = FS_Read(stream->file,(stream->buffer + stream->length),(detectlen- stream->length)))>0){

      stream->length += readbtes;

    }

    buf0 = stream->buffer;
    end = stream->buffer + stream->length - 8;
    while(buf0 < end && !*buf0)
        buf0++;

    buf = buf0;
    for (; buf < end; buf= buf2+1) {

        buf2 = buf;

        if (errorcount >50)
        {
            return -1;
        }

        for (frames = 0; buf2 < end; frames++) {

            mpeg_bit_init(&stream->ptr,buf2);
            if (stream->audiotype == MUSIC_TYPE_AAC) {

                if (mpeg_unpack_adts_header(&adtsheader,stream) < 0) {

                    errorcount++;
                    break;
                }
                buf2 += adtsheader.aac_frame_length;
                totalframes ++;
                bitratesum += adtsheader.aac_frame_length;

            } else {
                header = mpeg_bit_read(&stream->ptr,32);

                if (mpeg_mpa_decode_header(header,headerinfo) < 0) {

                    errorcount++;
                    break;

                }
                buf2 += headerinfo->frame_size;
                totalframes ++;
                bitratesum += headerinfo->bit_rate;
            }


        }
    }

    if (stream->audiotype == MUSIC_TYPE_AAC)
    {

        headerinfo->sample_rate = sampRateTab[adtsheader.sf_index];

        duraition = (totalframes*1024)/headerinfo->sample_rate;//assume spf =1024

        headerinfo->bit_rate = (stream->length *8)/duraition;

        headerinfo->nb_channels =adtsheader.channel_configuration;

    } else {

        headerinfo->bit_rate = bitratesum/totalframes;

    }

    return 0;
}

int mpeg_mpa_decode_header( unsigned int data, MPADecodeHeader *headerinfo)
{
    MPADecodeHeader s1, *s = &s1;

    if (mpeg_mpa_check_header(data) != 0)
        return -1;

    if (mpeg_audio_decode_header(s, data) != 0) {
        return -1;
    }

    switch(s->layer) {
    case 1:

        headerinfo->spf = 384;
        break;
    case 2:

        headerinfo->spf = 1152;
        break;
    default:
    case 3:
        if (s->lsf)
            headerinfo->spf = 576;
        else
            headerinfo->spf = 1152;
        break;
    }

    headerinfo->sample_rate = s->sample_rate;
    headerinfo->nb_channels = s->nb_channels;
    headerinfo->bit_rate = s->bit_rate;
    headerinfo->lsf = s->lsf;
    headerinfo->frame_size = s->frame_size;

    return 0;
}




int mpeg_audio_decode_header(MPADecodeHeader *s, unsigned int header)
{
    int sample_rate, frame_size, mpeg25, padding;
    int sample_rate_index, bitrate_index;
    if (header & (1<<20)) {
        s->lsf = (header & (1<<19)) ? 0 : 1;
        mpeg25 = 0;
    } else {
        s->lsf = 1;
        mpeg25 = 1;
    }

    s->layer = 4 - ((header >> 17) & 3);

    sample_rate_index = (header >> 10) & 3;
    sample_rate = mpeg_mpa_freq_tab[sample_rate_index] >> (s->lsf + mpeg25);
    sample_rate_index += 3 * (s->lsf + mpeg25);
    s->sample_rate_index = sample_rate_index;
    s->error_protection = ((header >> 16) & 1) ^ 1;
    s->sample_rate = sample_rate;

    bitrate_index = (header >> 12) & 0xf;
    padding = (header >> 9) & 1;
    s->mode = (header >> 6) & 3;
    s->mode_ext = (header >> 4) & 3;

    if (s->mode == MPA_MONO)
        s->nb_channels = 1;
    else
        s->nb_channels = 2;

    if (bitrate_index != 0) {
        frame_size = mpeg_mpa_bitrate_tab[s->lsf][s->layer - 1][bitrate_index];
        s->bit_rate = frame_size * 1000;
        switch(s->layer) {
        case 1:
            frame_size = (frame_size * 12000) / sample_rate;
            frame_size = (frame_size + padding) * 4;
            break;
        case 2:
            frame_size = (frame_size * 144000) / sample_rate;
            frame_size += padding;
            break;
        default:
        case 3:
            frame_size = (frame_size * 144000) / (sample_rate << s->lsf);
            frame_size += padding;
            break;
        }
        s->frame_size = frame_size;
    } else {
        /* if no frame size computed, signal it */
        return 1;
    }

    return 0;
}

void mpeg_bit_init(struct mpeg_bitptr *bitptr, unsigned char const *byte)
{
    bitptr->byte  = byte;
    bitptr->cache = 0;
    bitptr->left  = CHAR_BIT;
}
/*
 * NAME:    bit->skip()
 * DESCRIPTION: advance bit pointer
 */
void mpeg_bit_skip(struct mpeg_bitptr *bitptr, unsigned int len)
{
    bitptr->byte += len / CHAR_BIT;
    bitptr->left -= len % CHAR_BIT;

    if (bitptr->left > CHAR_BIT) {
        bitptr->byte++;
        bitptr->left += CHAR_BIT;
    }

    if (bitptr->left < CHAR_BIT)
        bitptr->cache = *bitptr->byte;
}

/*
 * NAME:    bit->read()
 * DESCRIPTION: read an arbitrary number of bits and return their UIMSBF value
 */
unsigned long mpeg_bit_read(struct mpeg_bitptr *bitptr, unsigned int len)
{
    register unsigned long value;

    if (bitptr->left == CHAR_BIT)
        bitptr->cache = *bitptr->byte;

    if (len < bitptr->left) {
        value = (bitptr->cache & ((1 << bitptr->left) - 1)) >>
            (bitptr->left - len);
        bitptr->left -= len;

        return value;
    }

    /* remaining bits in current byte */

    value = bitptr->cache & ((1 << bitptr->left) - 1);
    len  -= bitptr->left;

    bitptr->byte++;
    bitptr->left = CHAR_BIT;

    /* more bytes */

    while (len >= CHAR_BIT) {
        value = (value << CHAR_BIT) | *bitptr->byte++;
        len  -= CHAR_BIT;
    }

    if (len > 0) {
        bitptr->cache = *bitptr->byte;

        value = (value << len) | (bitptr->cache >> (CHAR_BIT - len));
        bitptr->left -= len;
    }

    return value;
}


 /* NAME:   xing->parse()
 * DESCRIPTION: parse a Xing VBR header
 */
int Xing_parse(struct xing_Fraunhofer_info *xing, struct mpeg_stream *stream)
{

  unsigned int tmp;

  mpeg_bit_init(&stream->ptr,stream->buffer);
  mpeg_bit_skip(&stream->ptr,(xing->xing_offset +4)*8);

  tmp = mpeg_bit_read(&stream->ptr, 32);

  if (tmp == XING_MAGIC ){

     xing->iscbr = mpeg_ISVBR;

  } else if(tmp == INFO_MAGIC){

     xing->iscbr = mpeg_ISCBR;

  } else {
      goto fail;
  }
  xing->flags = mpeg_bit_read(&stream->ptr, 32);


  if (xing->flags & XING_FRAMES) {

    xing->frames = mpeg_bit_read(&stream->ptr, 32);
  } else {

    goto fail;
  }

  if (xing->flags & XING_BYTES) {
    xing->bytes = mpeg_bit_read(&stream->ptr, 32);
  } else {

      goto fail;
  }

  return 0;

fail:
  xing->flags = 0;
  return -1;
}


int Fraunhofer_parse(struct xing_Fraunhofer_info *Fraunhofer,struct mpeg_stream *stream) {

    mpeg_bit_init(&stream->ptr,stream->buffer);

    mpeg_bit_skip(&stream->ptr,Fraunhofer->Fraunhofer_offset*8); //assume Fraunhofer header.


    if (mpeg_bit_read(&stream->ptr,32) == VBRI_MAGIC)
    {
        if (mpeg_bit_read(&stream->ptr,16) == 1)
        {
            Fraunhofer->iscbr = mpeg_ISVBR;

            mpeg_bit_skip(&stream->ptr,32);
            Fraunhofer->bytes = mpeg_bit_read(&stream->ptr,32);
            Fraunhofer->frames = mpeg_bit_read(&stream->ptr,32);

            return 0;

        }
    }

    return -1;
}
void mpeg_set_stream_buffer(int file,struct mpeg_stream *stream,
                       unsigned char const *buffer, unsigned int length)
{
    stream->buffer = buffer;
    stream->bufend = buffer + length;
    stream->length = length;
    stream->file   = file;
    mpeg_bit_init(&stream->ptr, buffer);
}

int mpeg_stream_error_check(int file,int filetype,uint8_t *buffer,uint32_t checklength) {

    uint8_t *buf0,*end;
    int fileoffset = 0,id3len,readbytes,result =0;
    uint32_t header;

    checklength=FS_Read(file,buffer,checklength);

    buf0=buffer;
    end = buffer+checklength-10;

    while (buf0 <= end) {

        if (buf0 >= end)
        {
            return -1;
        }

        if (*buf0) {
            if (IS_ATDS(buf0) || IS_SYNC(buf0) || IS_ADIF(buf0)) {

                header = BEWORD(buf0[0], buf0[1], buf0[2], buf0[3]);
                
                if (filetype == MUSIC_TYPE_AAC) {
                    if (!IS_ADIF(buf0)) {

                        result = mpeg_adts_header_check(header);

                    }

                }else if (filetype == MUSIC_TYPE_MP3) {
                  
                    result = mpeg_mpa_check_header(header);
                } 

                if (result == 0) {

                    fileoffset += (buf0 - buffer);
                    FS_Seek(file,fileoffset,FS_SEEK_SET);
                    return fileoffset;
    
                }  
            } else if(mpeg_id3v2_match(buf0,ID3v2_DEFAULT_MAGIC)){

                    fileoffset += buf0 - buffer;
                    fileoffset += mpeg_id3v2_tag_len(buf0);

                    FS_Seek(file,fileoffset,FS_SEEK_SET);
                    readbytes = FS_Read(file,buffer,1024);
                    if (readbytes <= 0){

                        return -1;
                    }

                    buf0 = buffer;
                    end  = buffer +readbytes - 10;
                    continue;
            }

        }
        buf0++;
    }

}

int get_sample_rate(const uint8_t sr_index)
{

    if (sr_index < 12)
        return sampRateTab[sr_index];
    else
        return -1;
}

 int program_config_element(program_config *pce, struct mpeg_stream *stream)
{
    uint8_t i;

    memset(pce, 0, sizeof(program_config));

    pce->channels = 0;

    pce->element_instance_tag = (uint8_t)mpeg_bit_read(&stream->ptr,4);

    pce->object_type = (uint8_t)mpeg_bit_read(&stream->ptr,2);
    pce->sf_index = (uint8_t) mpeg_bit_read(&stream->ptr,4);
#if 1
    pce->num_front_channel_elements = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    pce->num_side_channel_elements = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    pce->num_back_channel_elements = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    pce->num_lfe_channel_elements = (uint8_t)mpeg_bit_read(&stream->ptr,2);
    pce->num_assoc_data_elements = (uint8_t)mpeg_bit_read(&stream->ptr,3);
    pce->num_valid_cc_elements = (uint8_t)mpeg_bit_read(&stream->ptr,4);

    pce->mono_mixdown_present = mpeg_bit_read(&stream->ptr,1);
    if (pce->mono_mixdown_present == 1)
    {
        pce->mono_mixdown_element_number = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    }

    pce->stereo_mixdown_present = mpeg_bit_read(&stream->ptr,1);
    if (pce->stereo_mixdown_present == 1)
    {
        pce->stereo_mixdown_element_number = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    }

    pce->matrix_mixdown_idx_present = mpeg_bit_read(&stream->ptr,1);
    if (pce->matrix_mixdown_idx_present == 1)
    {
        pce->matrix_mixdown_idx = (uint8_t)mpeg_bit_read(&stream->ptr,2);
        pce->pseudo_surround_enable = mpeg_bit_read(&stream->ptr,1);
    }

    for (i = 0; i < pce->num_front_channel_elements; i++)
    {
        pce->front_element_is_cpe[i] = mpeg_bit_read(&stream->ptr,1);
        pce->front_element_tag_select[i] = (uint8_t)mpeg_bit_read(&stream->ptr,4);

    if (pce->front_element_is_cpe[i] & 1)
    {
        pce->cpe_channel[pce->front_element_tag_select[i]] = pce->channels;
        pce->num_front_channels += 2;
        pce->channels += 2;
    } else {
        pce->sce_channel[pce->front_element_tag_select[i]] = pce->channels;
        pce->num_front_channels++;
        pce->channels++;
    }
    }

    for (i = 0; i < pce->num_side_channel_elements; i++)
    {
        pce->side_element_is_cpe[i] = mpeg_bit_read(&stream->ptr,1);
        pce->side_element_tag_select[i] = (uint8_t)mpeg_bit_read(&stream->ptr,4);

        if (pce->side_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->side_element_tag_select[i]] = pce->channels;
            pce->num_side_channels += 2;
            pce->channels += 2;
        } else {
            pce->sce_channel[pce->side_element_tag_select[i]] = pce->channels;
            pce->num_side_channels++;
            pce->channels++;
        }
    }

    for (i = 0; i < pce->num_back_channel_elements; i++)
    {
        pce->back_element_is_cpe[i] = mpeg_bit_read(&stream->ptr,1);
        pce->back_element_tag_select[i] = (uint8_t)mpeg_bit_read(&stream->ptr,4);

    if (pce->back_element_is_cpe[i] & 1)
    {
        pce->cpe_channel[pce->back_element_tag_select[i]] = pce->channels;
        pce->channels += 2;
        pce->num_back_channels += 2;
    } else {
        pce->sce_channel[pce->back_element_tag_select[i]] = pce->channels;
        pce->num_back_channels++;
        pce->channels++;
    }
    }

    for (i = 0; i < pce->num_lfe_channel_elements; i++)
    {
        pce->lfe_element_tag_select[i] = (uint8_t)mpeg_bit_read(&stream->ptr,4);

        pce->sce_channel[pce->lfe_element_tag_select[i]] = pce->channels;
        pce->num_lfe_channels++;
        pce->channels++;
    }

    for (i = 0; i < pce->num_assoc_data_elements; i++)
        pce->assoc_data_element_tag_select[i] = (uint8_t)mpeg_bit_read(&stream->ptr,4);

    for (i = 0; i < pce->num_valid_cc_elements; i++)
    {
        pce->cc_element_is_ind_sw[i] = mpeg_bit_read(&stream->ptr,1);
        pce->valid_cc_element_tag_select[i] = (uint8_t)mpeg_bit_read(&stream->ptr,4);
    }

    //faad_byte_align(ld);

    pce->comment_field_bytes = (uint8_t)mpeg_bit_read(&stream->ptr,8);
    for (i = 0; i < pce->comment_field_bytes; i++)
    {
        pce->comment_field_data[i] = (uint8_t)mpeg_bit_read(&stream->ptr,8);
    }
    pce->comment_field_data[i] = 0;

    if (pce->channels > 6)
        return -1;
#endif
    return 0;
}




int  mpeg_unpack_ADIF_header(adif_header *adif, struct mpeg_stream *stream)
{
    uint8_t i;
    uint32_t temp;
    int result = 0;
    temp = mpeg_bit_read(&stream->ptr,32);

    if (temp != ADIF_MAGIC)
    {
        return -1;
    }
    adif->copyright_id_present = mpeg_bit_read(&stream->ptr,1);
    if (adif->copyright_id_present)
    {
        for (i = 0; i < 72/8; i++)
        {
            adif->copyright_id[i] = (int8_t)mpeg_bit_read(&stream->ptr,8);
        }
        adif->copyright_id[i] = 0;
    }
    adif->original_copy  = mpeg_bit_read(&stream->ptr,1);
    adif->home = mpeg_bit_read(&stream->ptr,1);
    adif->bitstream_type = mpeg_bit_read(&stream->ptr,1);
    adif->bitrate = mpeg_bit_read(&stream->ptr,23);
    adif->num_program_config_elements = (uint8_t)mpeg_bit_read(&stream->ptr,4);

    //for (i = 0; i < adif->num_program_config_elements + 1; i++)
    {
        if(adif->bitstream_type == 0)
        {
            adif->adif_buffer_fullness = mpeg_bit_read(&stream->ptr,20);
        } else {
            adif->adif_buffer_fullness = 0;
        }

        result = program_config_element(&adif->pce[0], &stream->ptr);


    }

    return result;

}


