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
#ifndef MPEGHEADER_PARSER_H
#define MPEGHEADER_PARSER_H
#include "cs_types.h"

#define MKBETAG(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))
#define BEWORD(a,b,c,d) ((d) | ((c) << 8) | ((b) << 16) | ((unsigned)(a) << 24))


#define CHAR_BIT      8         /* number of bits in a char */
#define DETECTLEN      (200*1024)

enum mpeg_layer {
    mpeg_LAYER_I   = 1,			/* Layer I */
    mpeg_LAYER_II  = 2,			/* Layer II */
    mpeg_LAYER_III = 3			/* Layer III */
 };

enum mpeg_mode {
    mpeg_MODE_SINGLE_CHANNEL = 0,		/* single channel */
    mpeg_MODE_DUAL_CHANNEL	  = 1,		/* dual channel */
    mpeg_MODE_JOINT_STEREO	  = 2,		/* joint (MS/intensity) stereo */
    mpeg_MODE_STEREO	  = 3		/* normal LR stereo */
};

enum mpeg_emphasis {
    mpeg_EMPHASIS_NONE	  = 0,		/* no emphasis */
    mpeg_EMPHASIS_50_15_US	  = 1,		/* 50/15 microseconds emphasis */
    mpeg_EMPHASIS_CCITT_J_17 = 3		/* CCITT J.17 emphasis */
};
enum mpeg_iscbr {
    mpeg_ISCBR_NONE	  = 0,
    mpeg_ISCBR	  = 1,
    mpeg_ISVBR = 2

};

struct mpeg_header {
    enum mpeg_layer layer;			/* audio layer (1, 2, or 3) */
    enum mpeg_mode mode;			/* channel mode (see above) */
    int mode_extension;			/* additional mode info */
    enum mpeg_emphasis emphasis;		/* de-emphasis to use (see above) */

    unsigned long bitrate;		/* stream bitrate (bps) */
    unsigned int samplerate;		/* sampling frequency (Hz) */

    unsigned short crc_check;		/* frame CRC accumulator */
    unsigned short crc_target;		/* final target CRC checksum */

    int flags;				/* flags (see below) */
    int private_bits;			/* private bits (see below) */
    int channel;
    //	mpeg_timer_t duration;			/* audio playing time of frame */
};


enum {
    mpeg_FLAG_NPRIVATE_III	= 0x0007,	/* number of Layer III private bits */
    mpeg_FLAG_INCOMPLETE	= 0x0008,	/* header but not data is decoded */

    mpeg_FLAG_PROTECTION	= 0x0010,	/* frame has CRC protection */
    mpeg_FLAG_COPYRIGHT	= 0x0020,	/* frame is copyright */
    mpeg_FLAG_ORIGINAL	= 0x0040,	/* frame is original (else copy) */
    mpeg_FLAG_PADDING	= 0x0080,	/* frame has additional slot */

    mpeg_FLAG_I_STEREO	= 0x0100,	/* uses intensity joint stereo */
    mpeg_FLAG_MS_STEREO	= 0x0200,	/* uses middle/side joint stereo */
    mpeg_FLAG_FREEFORMAT	= 0x0400,	/* uses free format bitrate */

    mpeg_FLAG_LSF_EXT	= 0x1000,	/* lower sampling freq. extension */
    mpeg_FLAG_MC_EXT	= 0x2000,	/* multichannel audio extension */
    mpeg_FLAG_MPEG_2_5_EXT	= 0x4000	/* MPEG 2.5 (unofficial) extension */
};

enum mpeg_error {
    mpeg_ERROR_NONE	   = 0x0000,	/* no error */

    mpeg_ERROR_BUFLEN	   = 0x0001,	/* input buffer too small (or EOF) */
    mpeg_ERROR_BUFPTR	   = 0x0002,	/* invalid (null) buffer pointer */

    mpeg_ERROR_NOMEM	   = 0x0031,	/* not enough memory */

    mpeg_ERROR_LOSTSYNC	   = 0x0101,	/* lost synchronization */
    mpeg_ERROR_BADLAYER	   = 0x0102,	/* reserved header layer value */
    mpeg_ERROR_BADBITRATE	   = 0x0103,	/* forbidden bitrate value */
    mpeg_ERROR_BADSAMPLERATE  = 0x0104,	/* reserved sample frequency value */
    mpeg_ERROR_BADEMPHASIS	   = 0x0105,	/* reserved emphasis value */

    mpeg_ERROR_BADCRC	   = 0x0201,	/* CRC check failed */
    mpeg_ERROR_BADBITALLOC	   = 0x0211,	/* forbidden bit allocation value */
    mpeg_ERROR_BADSCALEFACTOR = 0x0221,	/* bad scalefactor index */
    mpeg_ERROR_BADFRAMELEN	   = 0x0231,	/* bad frame length */
    mpeg_ERROR_BADBIGVALUES   = 0x0232,	/* bad big_values count */
    mpeg_ERROR_BADBLOCKTYPE   = 0x0233,	/* reserved block_type */
    mpeg_ERROR_BADSCFSI	   = 0x0234,	/* bad scalefactor selection info */
    mpeg_ERROR_BADDATAPTR	   = 0x0235,	/* bad main_data_begin pointer */
    mpeg_ERROR_BADPART3LEN	   = 0x0236,	/* bad audio data length */
    mpeg_ERROR_BADHUFFTABLE   = 0x0237,	/* bad Huffman table select */
    mpeg_ERROR_BADHUFFDATA	   = 0x0238,	/* Huffman data overrun */
    mpeg_ERROR_BADSTEREO	   = 0x0239	/* incompatible block_type for JS */
};
#define mpeg_BUFFER_GUARD	8
#define mpeg_BUFFER_MDLEN	(511 + 2048 + mpeg_BUFFER_GUARD)

#define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
#define INFO_MAGIC  (('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')
#define VBRI_MAGIC  (('V' << 24) | ('B' << 16) | ('R' << 8) | 'I')
#define ADIF_MAGIC (('A' << 24) | ('D' << 16) | ('I' << 8) | 'F')

enum {
    XING_FRAMES = 0x00000001L,
    XING_BYTES  = 0x00000002L,
    XING_TOC    = 0x00000004L,
    XING_SCALE  = 0x00000008L
};

struct mpeg_bitptr {
    unsigned char const *byte;
    unsigned short cache;
    unsigned short left;
};

struct mpeg_stream {
    unsigned char const *buffer;		/* input bitstream buffer */
    unsigned char const *bufend;		/* end of buffer */
    unsigned int   length;
    unsigned int audiotype;
	int          file   ;
    unsigned long skiplen;		/* bytes to skip before next frame */
    unsigned long buflength;
    int sync;				/* stream sync found */

    struct mpeg_bitptr ptr;		/* current processing bit pointer */

   	/* decoding options (see below) */
    enum mpeg_error error;			/* error code (see above) */
};

enum {
    mpeg_PRIVATE_HEADER	= 0x0100,	/* header private bit */
    mpeg_PRIVATE_III	= 0x001f	/* Layer III private bits (up to 5) */
};
enum {

    MUSIC_TYPE_NONE = 0,
    MUSIC_TYPE_MP3 =  1,
    MUSIC_TYPE_AAC = 9

};
struct xing_Fraunhofer_info {
    long flags;			/* valid fields (see below) */
    unsigned long frames;		/* total number of frames */
    unsigned long bytes;		/* total number of bytes */
    unsigned int  xing_offset;
    unsigned int  Fraunhofer_offset;
    enum mpeg_iscbr  iscbr;
    //unsigned char toc[100];	/* 100-point seek table */
    //	long scale;			/* ?? */
};
#define MPA_STEREO  0
#define MPA_JSTEREO 1
#define MPA_DUAL    2
#define MPA_MONO    3

typedef struct MPADecodeHeader {
    int frame_size;
    int spf;
    int error_protection;
    int layer;
    int sample_rate;
    int sample_rate_index;
    int bit_rate;
    int nb_channels;
    int mode;
    int mode_ext;
    int lsf;
} MPADecodeHeader;

//typedef unsigned short uint16_t;
//typedef unsigned char uint8_t;
//typedef signed char int8_t;
//typedef unsigned int uint32_t;
typedef struct
{
    uint16_t syncword;
    uint8_t id;
    uint8_t layer;
    uint8_t protection_absent;
    uint8_t profile;
    uint8_t sf_index;
    uint8_t private_bit;
    uint8_t channel_configuration;
    uint8_t original;
    uint8_t home;
    uint8_t emphasis;
    uint8_t copyright_identification_bit;
    uint8_t copyright_identification_start;
    uint16_t aac_frame_length;
    uint16_t adts_buffer_fullness;
    uint8_t no_raw_data_blocks_in_frame;
    uint16_t crc_check;

    /* control param */
    uint8_t old_format;
}adts_header;
typedef struct
{
    uint8_t element_instance_tag;
    uint8_t object_type;
    uint8_t sf_index;
    uint8_t num_front_channel_elements;
    uint8_t num_side_channel_elements;
    uint8_t num_back_channel_elements;
    uint8_t num_lfe_channel_elements;
    uint8_t num_assoc_data_elements;
    uint8_t num_valid_cc_elements;
    uint8_t mono_mixdown_present;
    uint8_t mono_mixdown_element_number;
    uint8_t stereo_mixdown_present;
    uint8_t stereo_mixdown_element_number;
    uint8_t matrix_mixdown_idx_present;
    uint8_t pseudo_surround_enable;
    uint8_t matrix_mixdown_idx;
    uint8_t front_element_is_cpe[16];
    uint8_t front_element_tag_select[16];
    uint8_t side_element_is_cpe[16];
    uint8_t side_element_tag_select[16];
    uint8_t back_element_is_cpe[16];
    uint8_t back_element_tag_select[16];
    uint8_t lfe_element_tag_select[16];
    uint8_t assoc_data_element_tag_select[16];
    uint8_t cc_element_is_ind_sw[16];
    uint8_t valid_cc_element_tag_select[16];

    uint8_t channels;

    uint8_t comment_field_bytes;
    uint8_t comment_field_data[257];

    /* extra added values */
    uint8_t num_front_channels;
    uint8_t num_side_channels;
    uint8_t num_back_channels;
    uint8_t num_lfe_channels;
    uint8_t sce_channel[16];
    uint8_t cpe_channel[16];
} program_config;

typedef struct
{
    uint8_t copyright_id_present;
    int8_t copyright_id[10];
    uint8_t original_copy;
    uint8_t home;
    uint8_t bitstream_type;
    uint32_t bitrate;
    uint8_t num_program_config_elements;
    uint32_t adif_buffer_fullness;

    /* maximum of 16 PCEs */
    program_config pce[1];
} adif_header;

typedef struct{
    uint32_t  bitRate;
    uint32_t  sampleRate;
    uint32_t  nb_channels;
} mpeg_stream_play_info;

#define AAC_NUM_PROFILES	3
#define AAC_PROFILE_MP		0
#define AAC_PROFILE_LC		1
#define AAC_PROFILE_SSR		2

#define NUM_SAMPLE_RATES	12
#define NUM_DEF_CHAN_MAPS	8


#define IS_ATDS(p)    ((p)[0] == 0xff && ((p)[1] &0xf0) == 0xf0)
#define IS_SYNC(p)    ((p)[0] == 0xff && ((p)[1] &0xE0) == 0xE0)
#define IS_ADIF(p)	  ((p)[0] == 'A' && (p)[1] == 'D' && (p)[2] == 'I' && (p)[3] == 'F')

#define ID3v2_DEFAULT_MAGIC "ID3"
#define ID3v2_HEADER_SIZE 10

int mpeg_stream_error_check(int file,int filetype,uint8_t *buffer,uint32_t checklength);
int mpeg_stream_init(int file,unsigned char *inputbuf,unsigned int datalength,struct mpeg_stream *stream);
int mpeg_stream_info_get(mpeg_stream_play_info *playinfo, struct mpeg_stream stream);

#endif
