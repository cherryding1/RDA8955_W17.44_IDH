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

#include "mci.h"


#ifndef AUDIO_ADPCM_H
#define AUDIO_ADPCM_H



enum AdpcmCodecID
{
    AdpcmCODEC_ID_NONE,

    /* various PCM "codecs" */
    AdpcmCODEC_ID_PCM_S16LE= 0x10000,
    AdpcmCODEC_ID_PCM_S16BE,
    AdpcmCODEC_ID_PCM_U16LE,
    AdpcmCODEC_ID_PCM_U16BE,
    AdpcmCODEC_ID_PCM_S8,
    AdpcmCODEC_ID_PCM_U8,
    AdpcmCODEC_ID_PCM_MULAW,
    AdpcmCODEC_ID_PCM_ALAW,
    AdpcmCODEC_ID_PCM_S32LE,
    AdpcmCODEC_ID_PCM_S32BE,
    AdpcmCODEC_ID_PCM_U32LE,
    AdpcmCODEC_ID_PCM_U32BE,
    AdpcmCODEC_ID_PCM_S24LE,
    AdpcmCODEC_ID_PCM_S24BE,
    AdpcmCODEC_ID_PCM_U24LE,
    AdpcmCODEC_ID_PCM_U24BE,
    AdpcmCODEC_ID_PCM_S24DAUD,
    AdpcmCODEC_ID_PCM_ZORK,
    AdpcmCODEC_ID_PCM_S16LE_PLANAR,
    AdpcmCODEC_ID_PCM_DVD,
    AdpcmCODEC_ID_PCM_F32BE,
    AdpcmCODEC_ID_PCM_F32LE,
    AdpcmCODEC_ID_PCM_F64BE,
    AdpcmCODEC_ID_PCM_F64LE,

    /* various ADPCM codecs */
    AdpcmCODEC_ID_ADPCM_IMA_QT= 0x11000,
    AdpcmCODEC_ID_ADPCM_IMA_WAV,
    AdpcmCODEC_ID_ADPCM_IMA_DK3,
    AdpcmCODEC_ID_ADPCM_IMA_DK4,
    AdpcmCODEC_ID_ADPCM_IMA_WS,
    AdpcmCODEC_ID_ADPCM_IMA_SMJPEG,
    AdpcmCODEC_ID_ADPCM_MS,
    AdpcmCODEC_ID_ADPCM_4XM,
    AdpcmCODEC_ID_ADPCM_XA,
    AdpcmCODEC_ID_ADPCM_ADX,
    AdpcmCODEC_ID_ADPCM_EA,
    AdpcmCODEC_ID_ADPCM_G726,
    AdpcmCODEC_ID_ADPCM_CT,
    AdpcmCODEC_ID_ADPCM_SWF,
    AdpcmCODEC_ID_ADPCM_YAMAHA,
    AdpcmCODEC_ID_ADPCM_SBPRO_4,
    AdpcmCODEC_ID_ADPCM_SBPRO_3,
    AdpcmCODEC_ID_ADPCM_SBPRO_2,
    AdpcmCODEC_ID_ADPCM_THP,
    AdpcmCODEC_ID_ADPCM_IMA_AMV,
    AdpcmCODEC_ID_ADPCM_EA_R1,
    AdpcmCODEC_ID_ADPCM_EA_R3,
    AdpcmCODEC_ID_ADPCM_EA_R2,
    AdpcmCODEC_ID_ADPCM_IMA_EA_SEAD,
    AdpcmCODEC_ID_ADPCM_IMA_EA_EACS,
    AdpcmCODEC_ID_ADPCM_EA_XAS,
    AdpcmCODEC_ID_ADPCM_EA_MAXIS_XA,
    AdpcmCODEC_ID_ADPCM_IMA_ISS,

};

/**
 * all in native-endian format
 */
enum AdpcmSampleFormat
{
    AdpcmSAMPLE_FMT_NONE = -1,
    AdpcmSAMPLE_FMT_U8,              ///< unsigned 8 bits
    AdpcmSAMPLE_FMT_S16,             ///< signed 16 bits
    AdpcmSAMPLE_FMT_S32,             ///< signed 32 bits
    AdpcmSAMPLE_FMT_FLT,             ///< float
    AdpcmSAMPLE_FMT_DBL,             ///< double
    AdpcmSAMPLE_FMT_NB               ///< Number of sample formats. DO NOT USE if dynamically linking to libavcodec
};

enum AdpcmCodecType
{
    AdpcmCODEC_TYPE_UNKNOWN = -1,
    AdpcmCODEC_TYPE_VIDEO,
    AdpcmCODEC_TYPE_AUDIO,
    AdpcmCODEC_TYPE_DATA,
    AdpcmCODEC_TYPE_SUBTITLE,
    AdpcmCODEC_TYPE_ATTACHMENT,
    AdpcmCODEC_TYPE_NB
};


/**
 * AVCodec.
 */
typedef struct AdpcmCodec
{
    /**
     * Name of the codec implementation.
     * The name is globally unique among encoders and among decoders (but an
     * encoder and a decoder can share the same name).
     * This is the primary way to find a codec from the user perspective.
     */
    const char *name;
    enum AdpcmCodecType type;
    enum AdpcmCodecID id;
    int priv_data_size;

} AdpcmCodec;



typedef struct AdpcmCodecContext
{

    /**
     * the average bitrate
     * - encoding: Set by user; unused for constant quantizer encoding.
     * - decoding: Set by libavcodec. 0 or some bitrate if this info is available in the stream.
     */
    int bit_rate;

    /**
     * number of bits the bitstream is allowed to diverge from the reference.
     *           the reference can be CBR (for CBR pass1) or VBR (for pass2)
     * - encoding: Set by user; unused for constant quantizer encoding.
     * - decoding: unused
     */
    int bit_rate_tolerance;

    /**
     * CODEC_FLAG_*.
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int flags;

    /**
     * Some codecs need additional format info. It is stored here.
     * If any muxer uses this then ALL demuxers/parsers AND encoders for the
     * specific codec MUST set it correctly otherwise stream copy breaks.
     * In general use of this field by muxers is not recommanded.
     * - encoding: Set by libavcodec.
     * - decoding: Set by libavcodec. (FIXME: Is this OK?)
     */
    int sub_id;

    /**
    * some codecs need / can use extradata like Huffman tables.
    * mjpeg: Huffman tables
    * rv10: additional flags
    * mpeg4: global headers (they can be in the bitstream or here)
    * The allocated memory should be FF_INPUT_BUFFER_PADDING_SIZE bytes larger
    * than extradata_size to avoid prolems if it is read with the bitstream reader.
    * The bytewise contents of extradata must not depend on the architecture or CPU endianness.
    * - encoding: Set/allocated/freed by libavcodec.
    * - decoding: Set/allocated/freed by user.
    */
    uint8 *extradata;
    int extradata_size;






    /* audio only */
    int sample_rate; ///< samples per second
    int channels;    ///< number of audio channels


    /**
     * audio sample format
     * - encoding: Set by user.
     * - decoding: Set by libavcodec.
     */
    enum AdpcmSampleFormat sample_fmt;  ///< sample format, currently unused

    /* The following data should not be initialized. */
    /**
     * Samples per packet, initialized when calling 'init'.
     */
    int frame_size;
    int frame_number;   ///< audio or video frame number
    int real_pict_num;  ///< Returns the real picture number of previous encoded frame.


    struct AVCodec *codec;
    void *priv_data;

    /**
     * number of bytes per packet if constant and known or 0
     * Used by some WAV based audio codecs.
     */
    int block_align;

    char codec_name[32];
    enum AdpcmCodecType codec_type; /* see CODEC_TYPE_xxx */
    enum AdpcmCodecID codec_id; /* see CODEC_ID_xxx */

    /**
     * fourcc (LSB first, so "ABCD" -> ('D'<<24) + ('C'<<16) + ('B'<<8) + 'A').
     * This is used to work around some encoder bugs.
     * A demuxer should set this to what is stored in the field used to identify the codec.
     * If there are multiple such fields in a container then the demuxer should choose the one
     * which maximizes the information about the used codec.
     * If the codec tag field in a container is larger then 32 bits then the demuxer should
     * remap the longer ID to 32 bits with a table or other structure. Alternatively a new
     * extra_codec_tag + size could be added but for this a clear advantage must be demonstrated
     * first.
     * - encoding: Set by user, if not then the default based on codec_id will be used.
     * - decoding: Set by user, will be converted to uppercase by libavcodec during init.
     */
    unsigned int codec_tag;


    /**
     * debug
     * - encoding: Set by user.
     * - decoding: Set by user.
     */
    int debug;

} AdpcmCodecContext;











#define AV_RL32(x) ((((const uint8*)(x))[3] << 24) | \
                    (((const uint8*)(x))[2] << 16) | \
                    (((const uint8*)(x))[1] <<  8) | \
                     ((const uint8*)(x))[0])


#define AV_RL16(x)  ((((const uint8*)(x))[1] << 8) | \
                      ((const uint8*)(x))[0])

#define FFSWAP(type,a,b) do{type SWAP_tmp= b; b= a; a= SWAP_tmp;}while(0)


#ifndef UINT32_MAX
#define UINT32_MAX      0xffffffff
#endif




typedef struct AVCodecTag
{
    int id;
    unsigned int tag;
} AVCodecTag;




typedef struct ADPCMChannelStatus
{
    int predictor;
    short int step_index;
    int step;
    /* for encoding */
    int prev_sample;
    /* MS version */
    short sample1;
    short sample2;
    int coeff1;
    int coeff2;
    int idelta;
} ADPCMChannelStatus;

typedef struct ADPCMContext
{
    ADPCMChannelStatus status[6];
} ADPCMContext;






enum AdpcmCodecID wav_codec_get_id(unsigned int tag, int bps);



#endif /* AUDIO_ADPCM_H */
