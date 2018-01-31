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


#define MP3_DEC_CONST_X_SIZE       1102
#define MP3_DEC_CONST_Y_SIZE        978
#define MP12_DEC_CONST_Y_SIZE       312


void vpp_AmjpCircBuffDmaRd(void);
void vpp_AmjpCircBuffInit(void);
void vpp_AmjpAudioEqualizer(void);

void CII_div_mode(void);
void CII_mad_header_decode(void );
void CII_mad_layer_III( void );
void CII_mad_layer_II(void);
void CII_mad_frame_decode( void );
void CII_III_sideinfo(void);
void CII_mad_bit_crc( void );
void CII_III_decode(void);
void CII_III_aliasreduce( void);
void CII_III_imdct_l(void);
void CII_III_imdct_s(void);

void CII_III_overlap_z(void);
void CII_III_freqinver(void);
void CII_imdct36(void);
void CII_III_scalefactors(void);
void CII_III_scalefactors_lsf(void );
void CII_III_stereo(void);
void CII_III_reorder(void);
void CII_dct32(void);
void CII_mad_synth_frame(void);
void CII_III_exponents(void);

void CII_III_requantize(void);
void CII_huff_sub1(void);
void CII_huff_sub2(void);
void CII_huff_sub3(void);
void CII_III_huffdecode(void);
void CII_MP3_DECODER_MAIN(void) ;
void VPP_MP3_DECODE(void) ;
void CII_mad_stream_sync( void);
void CII_decode_header(void);
void CII_mad_layer_I(void);
void CII_mad_frame_mute(void);
void CII_decode_error(void  );
void CII_tagtype( void );
void CII_id3_tag_query(void);
void CII_parse_header(void);
void CII_free_bitrate( void );
void fifo_write(void);
void fifo_sync(void);
void CII_reload_data_temp(void);
void CII_reload_data_blocking(void);
void CII_creat_crc_table(void);
void CII_II_samples(void);
void CoolSand_audio_linear_dither(void);
void Coolsand_layer12_code_reload( void );


#define MAD_FLAG_NPRIVATE_III    0x0007     /* number of Layer III private bits */
#define  MAD_FLAG_INCOMPLETE     0x0008     /* header but not data is decoded */

#define  MAD_FLAG_PROTECTION     0x0010     /* frame has CRC protection */
#define  MAD_FLAG_COPYRIGHT  0x0020     /* frame is copyright */
#define  MAD_FLAG_ORIGINAL   0x0040     /* frame is original (else copy) */
#define  MAD_FLAG_PADDING    0x0080     /* frame has additional slot */

#define  MAD_FLAG_I_STEREO   0x0100     /* uses intensity joint stereo */
#define  MAD_FLAG_MS_STEREO  0x0200     /* uses middle/side joint stereo */
#define  MAD_FLAG_FREEFORMAT     0x0400     /* uses free format bitrate */

#define  MAD_FLAG_LSF_EXT    0x1000     /* lower sampling freq. extension */
#define  MAD_FLAG_MC_EXT     0x2000     /* multichannel audio extension */
#define  MAD_FLAG_MPEG_2_5_EXT   0x4000         /* MPEG 2.5 (unofficial) extension */


#define  MAD_PRIVATE_HEADER  0x0100     /* header private bit */
#define  MAD_PRIVATE_III     0x001f           /* Layer III private bits (up to 5) */





#define  MAD_ERROR_NONE              0x0000     /* no error */

#define  MAD_ERROR_BUFLEN        0x0001     /* input buffer too small (or EOF) */
#define  MAD_ERROR_BUFPTR        0x0002     /* invalid (null) buffer pointer */

#define  MAD_ERROR_NOMEM         0x0031     /* not enough memory */

#define  MAD_ERROR_LOSTSYNC      0x0101     /* lost synchronization */
#define  MAD_ERROR_BADLAYER      0x0102     /* reserved header layer value */
#define  MAD_ERROR_BADBITRATE        0x0103     /* forbidden bitrate value */
#define  MAD_ERROR_BADSAMPLERATE     0x0104     /* reserved sample frequency value */
#define  MAD_ERROR_BADEMPHASIS       0x0105     /* reserved emphasis value */

#define  MAD_ERROR_BADCRC        0x0201     /* CRC check failed */
#define  MAD_ERROR_BADBITALLOC       0x0211     /* forbidden bit allocation value */
#define  MAD_ERROR_BADSCALEFACTOR    0x0221     /* bad scalefactor index */
#define  MAD_ERROR_BADFRAMELEN       0x0231     /* bad frame length */
#define  MAD_ERROR_BADBIGVALUES      0x0232     /* bad big_values count */
#define  MAD_ERROR_BADBLOCKTYPE      0x0233     /* reserved block_type */
#define  MAD_ERROR_BADSCFSI      0x0234     /* bad scalefactor selection info */
#define  MAD_ERROR_BADDATAPTR        0x0235     /* bad main_data_begin pointer */
#define  MAD_ERROR_BADPART3LEN       0x0236     /* bad audio data length */
#define  MAD_ERROR_BADHUFFTABLE      0x0237     /* bad Huffman table select */
#define  MAD_ERROR_BADHUFFDATA       0x0238     /* Huffman data overrun */
#define MAD_ERROR_BADSTEREO         0x0239 /* incompatible block_type for JS */

#define  MAD_OPTION_IGNORECRC        0x0001 /* ignore CRC errors */
#define  MAD_OPTION_HALFSAMPLERATE   0x0002    /* generate PCM at 1/2 sample rate */


#define   MAD_MODE_SINGLE_CHANNEL   0       /* single channel */
#define   MAD_MODE_DUAL_CHANNEL     1       /* dual channel */
#define   MAD_MODE_JOINT_STEREO     2       /* joint (MS/intensity) stereo */
#define   MAD_MODE_STEREO       3       /* normal LR stereo */


#define  MAD_LAYER_I     1          /* Layer I */
#define  Mad_layer_II    2          /* Layer II */
#define  MAD_LAYER_III   3          /* Layer III */


#define  count1table_select   0x01
#define  scalefac_scale       0x02
#define  preflag          0x04
#define  mixed_block_flag     0x08

#define  MAD_FLOW_CONTINUE   0x0000     /* continue normally */
#define  MAD_FLOW_STOP       0x0010     /* stop decoding normally */
#define  MAD_FLOW_BREAK      0x0011     /* stop decoding and signal an error */
#define  MAD_FLOW_IGNORE     0x0020 /* ignore the current frame */
#define I_STEREO   0x1
#define MS_STEREO  0x2
#define CRC_POLY  0x8005


#define  TAGTYPE_NONE           0
#define  TAGTYPE_ID3V1          1
#define  TAGTYPE_ID3V2          2
#define  TAGTYPE_ID3V2_FOOTER   3

#define  ID3_TAG_FLAG_UNSYNCHRONISATION       0x80
#define  ID3_TAG_FLAG_EXTENDEDHEADER          0x40
#define  ID3_TAG_FLAG_EXPERIMENTALINDICATOR   0x20
#define  ID3_TAG_FLAG_FOOTERPRESENT           0x10
#define  ID3_TAG_FLAG_KNOWNFLAGS              0xf0




#define MPEG_BUFSZ  2048//40000 /* 2.5 s at 128 kbps; 1 s at 320 kbps */

#define DCT32_SHIFT 11


#define MIN_RELOAD_SIZE 8
