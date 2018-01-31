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


#include "vppp_pngdec_asm_common.h"
#include "vppp_pngdec_asm_map.h"
#include "vppp_pngdec_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"

// VoC_directive: PARSER_OFF
#include <stdio.h>
int inflate_fast_count=0;
int inflate_count=0;

extern FILE *DebugFileHandle;
unsigned char g_Buffer_test[64*1024*4];     //for testing
// VoC_directive: PARSER_ON

#if 0

// voc_enum mode
voc_alias VPP_PNGDEC_MODE_NO              -1, y
voc_alias VPP_PNGDEC_MODE_HI               0, y
voc_alias VPP_PNGDEC_MODE_BYE              1, y
// voc_enum_end

// voc_enum error
voc_alias VPP_PNGDEC_ERROR_NO              0, y
voc_alias VPP_PNGDEC_ERROR_YES            -1, y
// voc_enum_end

// voc_enum pngdec status
voc_alias VPP_PNGDEC_SAYS_HI          0x0111, y
voc_alias VPP_PNGDEC_SAYS_BYE         0x0B1E, y
// voc_enum_end







/* Possible inflate modes between inflate() calls */
// voc_enum inflate_mode
voc_alias  HEAD   0,y       /* i: waiting for magic header */
voc_alias  FLAGS  1,y       /* i: waiting for method and flags (gzip) */
voc_alias  TIME   2,y       /* i: waiting for modification time (gzip) */
voc_alias  OS     3,y         /* i: waiting for extra flags and operating system (gzip) */
voc_alias  EXLEN  4,y      /* i: waiting for extra length (gzip) */
voc_alias  EXTRA  5,y      /* i: waiting for extra bytes (gzip) */
voc_alias  NAME   6,y       /* i: waiting for end of file name (gzip) */
voc_alias  COMMENT    7,y    /* i: waiting for end of comment (gzip) */
voc_alias  HCRC   8,y       /* i: waiting for header crc (gzip) */
voc_alias  DICTID 9,y     /* i: waiting for dictionary check value */
voc_alias  DICT   10,y       /* waiting for inflateSetDictionary() call */
voc_alias  TYPE   11,y       /* i: waiting for type bits, including last-flag bit */
voc_alias  TYPEDO 12,y     /* i: same, but skip check to exit inflate on new block */
voc_alias  STORED 13,y     /* i: waiting for stored size (length and complement) */
voc_alias  COPY   14,y     /* i/o: waiting for input or output to copy stored block */
voc_alias  TABLE  15,y    /* i: waiting for dynamic block table lengths */
voc_alias  LENLENS 16,y   /* i: waiting for code length code lengths */
voc_alias  CODELENS 17,y   /* i: waiting for length/lit and distance code lengths */
voc_alias  LEN    18,y        /* i: waiting for length/lit code */
voc_alias  LENEXT 19,y     /* i: waiting for length extra bits */
voc_alias  DIST   20,y       /* i: waiting for distance code */
voc_alias  DISTEXT    21,y    /* i: waiting for distance extra bits */
voc_alias  MATCH  22,y      /* o: waiting for output space to copy string */
voc_alias  LIT    23,y        /* o: waiting for output space to write literal */
voc_alias  CHECK  24,y      /* i: waiting for 32-bit check value */
voc_alias  LENGTH 25,y     /* i: waiting for 32-bit length (gzip) */
voc_alias  DONE   26,y       /* finished check, done -- remain here until reset */
voc_alias  BAD    27,y        /* got a data error -- remain here until reset */
voc_alias  MEM    28,y        /* got an inflate() memory error -- remain here until reset */
voc_alias  SYNC  29,y      /* looking for synchronization bytes to restart inflate() */
// voc_enum_end



/* Type of code to build for inftable() */
// voc_enum codetype

voc_alias CODES 0,y
voc_alias LENS 1,y
voc_alias DISTS 2,y
// voc_enum_end






voc_struct VPP_PNGDEC_CFG_STRUCT __attribute__((export)),y

// ----------------
// common config
// ----------------
voc_short GLOBAL_MODE
voc_short GLOBAL_RESET

voc_word GLOBAL_InBufAddr
voc_word GLOBAL_InBufSize

voc_word GLOBAL_PNGOutBufferAddr
voc_word GLOBAL_PNGOutBufferSize

voc_word GLOBAL_AlphaOutBufferAddr
voc_word GLOBAL_AlphaOutBufferSize

voc_word GLOBAL_BkgrdBufferAddr

voc_short GLOBAL_BkgrdClipX1
voc_short GLOBAL_BkgrdClipX2
voc_short GLOBAL_BkgrdClipY1
voc_short GLOBAL_BkgrdClipY2


voc_short GLOBAL_BkgrdOffsetX
voc_short GLOBAL_BkgrdOffsetY

voc_short GLOBAL_BkgrdWidth
voc_short GLOBAL_BkgrdHeight

voc_short GLOBAL_ZoomWidth
voc_short GLOBAL_ZoomHeight

voc_short GLOBAL_DecMode
voc_short GLOBAL_alpha_blending_to_transparentColor

voc_word GLOBAL_ImgWidth
voc_word GLOBAL_ImgHeigh

voc_word GLOBAL_WindowAddr

voc_word GLOBAL_rgb_temp_ptr

voc_word GLOBAL_PngFileHandle


voc_struct_end



voc_struct VPP_PNGDEC_STATUS_STRUCT __attribute__((export)),y

// ----------------
// common status
// ----------------
voc_short GLOBAL_USED_MODE
voc_short GLOBAL_ERR_STATUS

// ----------------
// pngdec status
// ----------------
voc_word  GLOBAL_PNGDEC_STATUS

voc_struct_end


voc_struct VPP_PNGDEC_CODE_CFG_STRUCT __attribute__((export)),y

voc_word  GLOBAL_CONST_LENFIX_DISTFIX_PTR
voc_word  GLOBAL_BYE_CODE_PTR
voc_word  GLOBAL_CONST_PTR

voc_struct_end


voc_struct VPP_PNGDEC_CONST_STRUCT,y

voc_word VPP_PNGDEC_CONST_SAYS_HI
voc_word VPP_PNGDEC_CONST_SAYS_BYE

voc_struct_end

voc_short GLOBAL_INBUF[1024],x//(1<<GLOBAL_WRAP_BIT)

voc_word GLOBAL_byteleft,x
voc_word GLOBAL_OutPutAddr,x

voc_short GLOBAL_PingPangFlag,x
voc_short GLOBAL_resever1,x

voc_word rgb_output_ptr,x
voc_word alpha_output_ptr,x
voc_word bkgrd_ptr,x

voc_word imgOutPtr,x

voc_word PNGSignature0,x
voc_word PNGSignature1,x

voc_short GLOBAL_RawOutBufAddr,x

voc_short GLOBAL_PreRow,x


voc_struct FILECHUNKS ,x

voc_short FileChunks_IHDRNum
voc_short FileChunks_PLTENum

voc_short FileChunks_IDATNum
voc_short FileChunks_IENDNum

voc_short FileChunks_cHRMNum
voc_short FileChunks_gAMANum

voc_short FileChunks_iCCPNum
voc_short FileChunks_sBITNum

voc_short FileChunks_sRGBNum
voc_short FileChunks_bKGDNum

voc_short FileChunks_hISTNum
voc_short FileChunks_tRNSNum

voc_short FileChunks_pHYsNum
voc_short FileChunks_sPLTNum

voc_short FileChunks_tIMENum
voc_short FileChunks_iTXtNum

voc_short FileChunks_tEXtNum
voc_short FileChunks_zTXtNum

voc_struct_end


voc_struct IHDRCHUNK ,x

voc_short IHDRInfo_Width
voc_short IHDRInfo_Height

voc_short IHDRInfo_BitDepth
voc_short IHDRInfo_ColorType

voc_short IHDRInfo_Compression
voc_short IHDRInfo_Fileter

voc_short IHDRInfo_Interlace
voc_short IHDRInfo_reserve

voc_struct_end



voc_struct sBITInfo,x

voc_short sBITInfo_red
voc_short sBITInfo_green
voc_short sBITInfo_blue
voc_short sBITInfo_grey
voc_short sBITInfo_alpha
voc_short sBITInfo_reserve

voc_struct_end


voc_struct pHYsINFO,x

voc_word pHYsInfo_xPixPerUint
voc_word pHYsInfo_yPixPerUnit
voc_short pHYsInfo_UnitSpec
voc_short pHYsInfo_reserve

voc_struct_end



voc_struct PLTECHUNK,x

voc_short DefaultBkgd_Red
voc_short DefaultBkgd_Green
voc_short DefaultBkgd_Blue
voc_short DefaultBkgd_reserve
voc_struct_end

voc_struct bKGDINFO,x

voc_short bKGDInfo_bKGDGrey;
voc_short bKGDInfo_bKGDRed;
voc_short bKGDInfo_bKGDGreen;
voc_short bKGDInfo_bKGDBlue;
voc_short bKGDInfo_bKGDPlteIndex;
voc_short bKGDInfo_reserve;
voc_struct_end


voc_struct TRANSINFO,x
voc_short tRNSInfo_tRNSGrey;
voc_short tRNSInfo_tRNSRed;
voc_short tRNSInfo_tRNSGreen;
voc_short tRNSInfo_tRNSBlue;
voc_struct_end


voc_struct z_stream,x
voc_word    PNGzstream_next_in;  /* next input byte */
voc_word    PNGzstream_avail_in;  /* number of bytes available at next_in */
voc_word    PNGzstream_total_in;  /* total nb of input bytes read so far */

voc_word    PNGzstream_next_out; /* next output byte should be put there */
voc_word    PNGzstream_avail_out; /* remaining free space at next_out */
voc_word    PNGzstream_total_out; /* total nb of bytes output so far */

voc_word    PNGzstream_data_type;
voc_word    PNGzstream_adler;

// int     data_type;  /* best guess about the data type: binary or text */
//  uLong   adler;      /* adler32 value of the uncompressed data */
//uLong   reserved;   /* reserved for future use */
voc_struct_end





voc_struct inflate_state,x

voc_short state_mode;          /* current inflate mode */

/* for string and stored block copying */
voc_short state_length;            /* literal or length of data to copy */
voc_word state_offset;            /* distance back to copy string from */


voc_short state_last;                   /* true if processing last block */
voc_short state_wrap;                   /* bit 0 true for zlib, bit 1 true for gzip */
voc_short state_havedict;               /* true if dictionary provided */
voc_short state_flags;                  /* gzip header method and flags (0 if zlib) */
voc_word state_dmax;              /* zlib header max distance (INFLATE_STRICT) */
voc_word state_check;        /* protected copy of check value */

voc_word state_total;        /* protected copy of output count */
//   gz_headerp head;            /* where to save gzip header information */
/* sliding window */
voc_short state_wbits;             /* log base 2 of requested window size */
voc_short state_reserve001;
voc_word state_wsize;             /* window size or zero if not using window */
voc_word state_whave;             /* valid bytes in the window */
voc_word state_write;             /* window write index */
voc_word state_window;  /* allocated sliding window, if needed */


/* bit accumulator */
voc_word state_hold;         /* input bit accumulator */
voc_short state_bits;              /* number of bits in "in" */

/* for table and code decoding */
voc_short state_extra;             /* extra bits needed */
/* fixed and dynamic code tables */
voc_short state_lencode;    /* starting table for length/literal codes */
voc_short state_distcode;   /* starting table for distance codes */
voc_short state_lenbits;           /* index bits for lencode */
voc_short state_distbits;          /* index bits for distcode */
/* dynamic table building */
voc_short state_ncode;             /* number of code length code lengths */
voc_short state_nlen;              /* number of length code lengths */
voc_short state_ndist;             /* number of distance code lengths */
voc_short state_have;              /* number of code lengths in lens[] */
voc_short state_next;             /* next available space in codes[] */
voc_short state_reserve;
voc_short state_lens[320];   /* temporary storage for code lengths */
voc_short state_work[288];   /* work area for code table building */


//  strmcode state_codes[ENOUGH];         /* space for code tables */

voc_short state_codes_op[ENOUGH];
voc_short state_codes_bits[ENOUGH];
voc_short state_codes_val[ENOUGH];


voc_struct_end

voc_struct PNGCONSTS_X,x
voc_word crc_table[256];
voc_short lbase[31];
voc_short lext[31];
voc_short dbase[32];
voc_short dext[32];
voc_struct_end

voc_struct ROWINFO,x
voc_word RowInfo_Width; /* width of row */
voc_word RowInfo_RowBytes; /* number of bytes in row */
voc_short RowInfo_ColorType; /* color type of row */
voc_short RowInfo_BitDepth; /* bit depth of row */
voc_short RowInfo_Channels; /* number of channels (1, 2, 3, or 4) */
voc_short RowInfo_PixelDepth; /* bits per pixel (depth * channels) */
voc_struct_end

voc_struct cHRMINFO,x
voc_word cHRMInfo_WhiteX;
voc_word cHRMInfo_WhiteY;
voc_word cHRMInfo_RedX;
voc_word cHRMInfo_RedY;
voc_word cHRMInfo_GreenX;
voc_word cHRMInfo_GreenY;
voc_word cHRMInfo_BlueX;
voc_word cHRMInfo_BlueY;
voc_struct_end

voc_short Palete[256*3],x

voc_short GLOBAL_BITCACHE,x
voc_short ImgChannels,x

voc_word RowBytes,x


voc_short png_resized_width,x
voc_short png_resized_height,x


voc_short PngCutOffsetX1,x  //cut area x start offset in png
voc_short PngCutOffsetY1,x  //cut area y start offset in png
voc_short PngCutOffsetX2,x  //cut area x end offset in png
voc_short PngCutOffsetY2,x  //cut area y end offset in png

voc_word IDATSize,x

voc_short transform,x
voc_short CurPass,x

voc_short CurPassRowNum,x
voc_short CurInterlaceRowWidth,x

voc_word CurPassRowBytes,x

voc_short Pass_Xstart[7],x  // = {0, 4, 0, 2, 0, 1, 0};
voc_short Pass_Xinc[7],x    // = {8, 8, 4, 4, 2, 2, 1};

voc_short Pass_Ystart[7],x  //  const int Pass_Ystart[7] = {0, 0, 4, 0, 2, 0, 1};
voc_short Pass_Yinc[7],x    //  const int Pass_Yinc[7] = {8, 8, 8, 4, 4, 2, 2};


voc_short PixelDepth,x
voc_short MaxBitDepth,x

voc_short TransNum,x
voc_short NewColorType,x           // 变换以后的色彩类型

voc_short NewBiteDepth,x             // 变换以后的比特深度
voc_short NewTansNum,x               // 变换以后的透明色数目

voc_word NewRowBytes,x             // 变换以后行的字节数

voc_short ImagePass,x        // 图像Pass数
voc_short IDATHandle_imgOutStride,x        // 图像Pass数

voc_short IDATHandle_start,x
voc_short IDATHandle_end,x


voc_short IDATHandle_ErrCode,x
voc_short IDATHandle_skipRow,x

voc_short CurRowNumber,x            // 当前行号
voc_short png_decode_mode,x

voc_short png_bkgrd_width,x
voc_short png_bkgrd_height,x

voc_short order[20],x//    static const unsigned short order[19] = /* permutation of code lengths */
//{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

voc_short count[MAXBITS+1],x    /* number of codes of each length */
voc_short offs[MAXBITS+1],x     /* offsets in table for each length */

voc_word inflate_in,x
voc_word inflate_out,x

voc_short inflate_table_type,x
voc_short inflate_table_lens,x

voc_short inflate_table_codes,x
voc_short inflate_table_table,x

voc_short inflate_table_bits,x
voc_short inflate_table_work,x


voc_short inflate_table_root,x
voc_short inflate_table_max,x

voc_short inflate_table_min,x
voc_short inflate_table_left,x

voc_short inflate_table_base,x
voc_short inflate_table_extra,x

voc_short inflate_table_end,x
voc_short inflate_table_huff,x

voc_short inflate_table_len,x
voc_short inflate_table_curr,x


voc_short inflate_table_low,x
voc_short inflate_table_used,x

voc_short inflate_table_mask,x
voc_short inflate_table_drop,x

voc_short inflate_table_next,x
voc_short inflate_table_copy,x

voc_short state_codes_op_addr,x
voc_short state_codes_bits_addr,x

voc_short state_codes_val_addr,x
voc_short inflate_fast_len,x

voc_word inflate_fast_dist,x




voc_word zmemcpy_Length,x
voc_word zmemcpy_SrcAddr,x
voc_word zmemcpy_DstAddr,x

voc_word updatewindow_copy,x
voc_word updatewindow_dist,x

//                      UINT32 data_tmp0,data_tmp1,data_tmp2;
//                      sp = Row;
//                      dp = Row;



voc_short BkgdProcess_data_tmp0,x
voc_short BkgdProcess_data_tmp1,x

voc_short BkgdProcess_data_tmp2,x
voc_short BkgdProcess_RowWidth,x

voc_short BkgdProcess_sp,x
voc_short BkgdProcess_dp,x

voc_short BkgdProcess_bkgrdNeed,x
voc_short BkgdProcess_alphaNeed,x

//const int PassMask[7] = {0x80, 0x08, 0x88, 0x22, 0xaa, 0x55, 0xff};

voc_short PassMask[7],x
voc_short inflate_ret,x

voc_short FilterRow_istop,x
voc_short GLOBAL_CONST_8_ADDR,x

voc_word PlteSize,x
voc_word gammer,x         // 图像gammer值

voc_short GLOBAL_RawOutBuf[8192],y

voc_short GLOBAL_DMATEMPBUF[1024],y


voc_short GLOBAL_INBUF_ADDR,x
voc_short GLOBAL_CONST_512_ADDR,x
voc_word GLOBAL_CONST_1024_ADDR,x

voc_short tRNSIndex[256],x

voc_short sRGBRenderContent; // sRGB信息
voc_short reserve0001;






#endif


// ----------------------------
// fct : vpp_PngDecMain
// in  :
// out :
// not modified :
// used : all
// ----------------------------

void vpp_PngDecMain(void)
{
    // set the stack pointers
    VoC_lw16i(SP16,SP16_ADDR);
    VoC_lw16i(SP32,SP32_ADDR);

    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

    // generate first interrupt
    VoC_cfg(CFG_CTRL_IRQ_TO_XCPU | CFG_CTRL_CONTINUE);

PNGDEC_LABEL_MAIN_LOOP:


    // Stall the execution
    // wait for the next audio event
    VoC_cfg(CFG_CTRL_STALL);

// VoC_directive: PARSER_OFF
    // used for simulation
    VoC_cfg_stop;
// VoC_directive: PARSER_ON

    VoC_NOP();

    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

    // Disable all programed events, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);

    // ----------------------------
    // fct : vpp_PngDecCodeConstReload
    // in :  REG0 -> GLOBAL_MODE
    // not modified : REG0
    // unused : REG45, RL67, ACC1
    // ----------------------------
    VoC_jal(vpp_PngDecCodeConstReload);


    VoC_jal(Coolsand_PngDec);


    VoC_jump(PNGDEC_LABEL_MAIN_LOOP);

}


// ----------------------------
// fct : vpp_PngDecCodeConstReload
// in :  REG0 -> GLOBAL_MODE
// not modified : REG0
// unused : REG45, RL67, ACC1
// ----------------------------
void vpp_PngDecCodeConstReload(void)
{
    VoC_push16(RA,DEFAULT);


    VoC_lw32_d(ACC0,GLOBAL_CONST_PTR);
    VoC_lw16i(REG2,PNGCONSTS_X/2);
    VoC_lw16i(REG3,GLOBAL_CONST_SIZE);
    VoC_jal(vpp_PngDecRunDma);

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}


// ----------------------------
// fct : vpp_PngDecRunDma
// in :  REG2 -> LADDR
//       REG3 -> SIZE
//       ACC0 -> EADDR
// modified : none
// ----------------------------
void vpp_PngDecRunDma(void)
{
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_PngDec
 *
 * Description: PNG decoder
 *
 * Inputs:
 *
 * Outputs:
 *
 *
 * Used : ALL.
 *
 * NOTE: no.
 *
 * Version 1.0  Created by  Xuml       02/28/2012
 **************************************************************************************/
void Coolsand_PngDec(void)
{
// VoC_directive: PARSER_OFF
    printf("The PngDec says Hi\n");
// VoC_directive: PARSER_ON

    VoC_push16(RA,DEFAULT);

    VoC_jal(Coolsand_PNG_Init);

    //if(!FileContent ||!ImgWidth ||!ImgHeigh)
    //  return INPUTERR;

//  png_decode_mode= DecMode;
//  png_bkgrd_width= BkgrdWidth;
//  png_bkgrd_height= BkgrdHeight;

    VoC_lw32_d(ACC0,GLOBAL_WindowAddr);
    VoC_lw16_d(REG5,GLOBAL_DecMode);
    VoC_lw16_d(REG6,GLOBAL_BkgrdWidth);
    VoC_lw16_d(REG7,GLOBAL_BkgrdHeight);


    VoC_sw32_d(ACC0,state_window);
    VoC_sw16_d(REG5,png_decode_mode);
    VoC_sw16_d(REG6,png_bkgrd_width);
    VoC_sw16_d(REG7,png_bkgrd_height);



    VoC_lw16i(REG7,INPUTERR);

    VoC_bnez32_d(Coolsand_PngDec_L0,GLOBAL_InBufAddr);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L0:


    //if(BkgrdClipX1>BkgrdClipX2 || BkgrdClipY1>BkgrdClipY2 || BkgrdClipX2 >=BkgrdWidth
    //  || BkgrdClipY2>=BkgrdHeight)
//      return INPUTERR;

    VoC_lw16_d(REG6,GLOBAL_BkgrdClipX1);
    VoC_bngt16_rd(Coolsand_PngDec_L3,REG6,GLOBAL_BkgrdClipX2);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L3:

    VoC_lw16_d(REG6,GLOBAL_BkgrdClipY1);
    VoC_bngt16_rd(Coolsand_PngDec_L4,REG6,GLOBAL_BkgrdClipY2);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L4:

    VoC_lw16_d(REG6,GLOBAL_BkgrdClipX2);
    VoC_blt16_rd(Coolsand_PngDec_L5,REG6,GLOBAL_BkgrdWidth);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L5:

    VoC_lw16_d(REG6,GLOBAL_BkgrdClipY2);
    VoC_blt16_rd(Coolsand_PngDec_L6,REG6,GLOBAL_BkgrdHeight);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L6:

    //  PngStreamBuf.byteleft= FileSize;
    VoC_lw32_d(RL7,GLOBAL_InBufSize);

    VoC_NOP();

    VoC_sw32_d(RL7,GLOBAL_byteleft);


    //DMA DATA
    VoC_lw16i(REG5,GLOBAL_INBUF/2);
    VoC_lw16i(REG4,(1<<GLOBAL_WRAP_BIT)/2);
    VoC_lw32_d(REG67,GLOBAL_InBufAddr);

    VoC_and16_ri(REG6,0xfffc);

    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw32_d(REG67,GLOBAL_InBufAddr);

    VoC_and16_ri(REG6,0x3);

    VoC_sub16_rr(REG4,REG4,REG6,DEFAULT);

    VoC_add32_rd(ACC0,REG45,GLOBAL_InBufAddr);

    VoC_NOP();

    VoC_sw32_d(ACC0,GLOBAL_InBufAddr);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    VoC_lw16i_short(WRAP0,GLOBAL_WRAP_BIT,DEFAULT);

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_lw16i_set_inc(REG0,GLOBAL_INBUF,1);

    VoC_bez16_r(Coolsand_PngDec_L6_S0,REG6)

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);

    VoC_endloop0

Coolsand_PngDec_L6_S0:





    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,8,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lbinc_p(REG0);


    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG23,IN_PARALLEL);//      PngStreamBuf.byteleft -=8;

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_be32_rd(Coolsand_PngDec_L7,REG45,PNGSignature0)
    VoC_lw16i(REG7,IHDRERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L7:

    VoC_be32_rd(Coolsand_PngDec_L8,REG67,PNGSignature1)
    VoC_lw16i(REG7,IHDRERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L8:

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG23,IN_PARALLEL);//      PngStreamBuf.byteleft -=4;

    VoC_lw16i_short(REG6,4,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,REG67,DEFAULT);

    VoC_push32(REG45,DEFAULT);

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_jal(Coolsand_crc32);

    VoC_pop32(REG45,DEFAULT);

    VoC_bez16_r(Coolsand_PngDec_CRC_L0,REG7);
    VoC_lw16i(REG7,IHDRCRCERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_CRC_L0:


    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG23)//       PngStreamBuf.byteleft -=4;

    VoC_lw16i(REG2,0x4452);//IDHR
    VoC_lw16i(REG3,0x4948);

    VoC_be32_rr(Coolsand_PngDec_IHDR,REG67,REG23)
    VoC_lw16i(REG7,IHDRERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_IHDR:

    VoC_jal(Coolsand_IHDRHandle);

    VoC_bez16_r(Coolsand_PngDec_L9,REG7);

    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L9:


    /*  if(png_decode_mode==0 && ZoomWidth>0 && ZoomHeight>0)
        {
            png_resized_width= ZoomWidth;
            png_resized_height= ZoomHeight;
        }
        else//no resize
        {
            png_resized_width= IHDRInfo.Width;
            png_resized_height= IHDRInfo.Height;
        }   */


    VoC_lw16_d(REG2,IHDRInfo_Width);
    VoC_lw16_d(REG3,IHDRInfo_Height);


    VoC_bnez16_d(Coolsand_PngDec_L10,png_decode_mode)
    VoC_bngtz16_d(Coolsand_PngDec_L10,GLOBAL_ZoomWidth)
    VoC_bngtz16_d(Coolsand_PngDec_L10,GLOBAL_ZoomHeight)

    VoC_lw16_d(REG2,GLOBAL_ZoomWidth);
    VoC_lw16_d(REG3,GLOBAL_ZoomHeight);

Coolsand_PngDec_L10:

    VoC_sw16_d(REG2,png_resized_width);
    VoC_sw16_d(REG3,png_resized_height);

    /*  //set cut area in original or resize png
        if(BkgrdOffsetX +png_resized_width > BkgrdClipX1 && BkgrdOffsetX <=BkgrdClipX2)
        {
            res=(INT32)BkgrdClipX1- (INT32)BkgrdOffsetX;
            if(res>0)
                PngCutOffsetX1=res;
            else
            {
                PngCutOffsetX1=0;
                BkgrdClipX1= BkgrdOffsetX;//reset clipX1
            }
            res=(INT32)BkgrdClipX2- ((INT32)BkgrdOffsetX+ (INT32)png_resized_width -1);

            if(res >= 0)
                PngCutOffsetX2=png_resized_width-1;
            else
                PngCutOffsetX2=(INT32)png_resized_width+ res -1;
        }
        else
        {
            mmi_trace(TRUE,"PNG out of display clip area.\n");
            return PNG_NO_ERROR;
        }   */

    VoC_lw16_d(REG5,GLOBAL_BkgrdOffsetX);

    VoC_add16_rr(REG3,REG5,REG2,DEFAULT);

    VoC_lw16_d(REG4,GLOBAL_BkgrdClipX2);

    VoC_bgt16_rd(Coolsand_PngDec_L11,REG3,GLOBAL_BkgrdClipX1)
    VoC_lw16i(REG7,CLIPOVER)
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L11:

    VoC_bgt16_rr(Coolsand_PngDec_L12,REG4,REG5)
    VoC_lw16i(REG7,CLIPOVER);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L12:

    VoC_sub16_dr(REG3,GLOBAL_BkgrdClipX1,REG5);
    VoC_bgtz16_r(Coolsand_PngDec_L13,REG3);

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_sw16_d(REG5,GLOBAL_BkgrdClipX1);
Coolsand_PngDec_L13:

    VoC_sw16_d(REG3,PngCutOffsetX1);

    VoC_sub16_dr(REG3,GLOBAL_BkgrdClipX2,REG2);

    VoC_sub16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);

    VoC_sub16_rr(REG4,REG2,REG6,DEFAULT);


    VoC_bnltz16_r(Coolsand_PngDec_L14,REG3);

    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
Coolsand_PngDec_L14:
    VoC_NOP();
    VoC_sw16_d(REG4,PngCutOffsetX2);


    /*
        if(BkgrdOffsetY +png_resized_height > BkgrdClipY1 && BkgrdOffsetY <=BkgrdClipY2)
        {
            res=(INT32)BkgrdClipY1- (INT32)BkgrdOffsetY;
            if(res>0)
                PngCutOffsetY1=res;
            else
            {
                PngCutOffsetY1=0;
                BkgrdClipY1= BkgrdOffsetY;//reset clipY1
            }
            res=(INT32)BkgrdClipY2- ((INT32)BkgrdOffsetY+ (INT32)png_resized_height -1);
            if(res >= 0)
                PngCutOffsetY2=png_resized_height-1;
            else
                PngCutOffsetY2=(INT32)png_resized_height+ res -1;
        }
        else
        {
            mmi_trace(TRUE,"PNG out of display clip area.\n");
            return PNG_NO_ERROR;
        }

    */


    VoC_lw16_d(REG2,png_resized_height);

    VoC_lw16_d(REG5,GLOBAL_BkgrdOffsetY)

    VoC_add16_rr(REG3,REG5,REG2,DEFAULT);

    VoC_lw16_d(REG4,GLOBAL_BkgrdClipY2);

    VoC_bgt16_rd(Coolsand_PngDec_L15,REG3,GLOBAL_BkgrdClipY1)
    VoC_lw16i(REG7,CLIPOVER)
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L15:

    VoC_bgt16_rr(Coolsand_PngDec_L16,REG4,REG5)
    VoC_lw16i(REG7,CLIPOVER)
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L16:

    VoC_sub16_dr(REG3,GLOBAL_BkgrdClipY1,REG5);
    VoC_bgtz16_r(Coolsand_PngDec_L17,REG3);

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_sw16_d(REG5,GLOBAL_BkgrdClipY1);
Coolsand_PngDec_L17:

    VoC_sw16_d(REG3,PngCutOffsetY1);

    VoC_sub16_dr(REG3,GLOBAL_BkgrdClipY2,REG2);

    VoC_sub16_rr(REG3,REG3,REG5,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG6,DEFAULT);

    VoC_sub16_rr(REG4,REG2,REG6,DEFAULT);

    VoC_bnltz16_r(Coolsand_PngDec_L18,REG3);

    VoC_add16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_NOP();
Coolsand_PngDec_L18:
    VoC_sw16_d(REG4,PngCutOffsetY2);

    /*
        case 0:
            {
                if(!BkgrdBuffer)
                    return INPUTERR;

                rgb_output_ptr= 0;
                alpha_output_ptr= 0;

                bkgrd_ptr= BkgrdBuffer+ ((UINT32)BkgrdWidth*(UINT32)BkgrdClipY1 + (UINT32)BkgrdClipX1)*2;//rgb565 bkgrd
            }

            break;
    */

    VoC_bnez16_d(Coolsand_PngDec_L19,png_decode_mode)

    VoC_lw32z(ACC1,DEFAULT);

    VoC_bnez32_d(Coolsand_PngDec_L19_0,GLOBAL_BkgrdBufferAddr)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L19_0:

    VoC_lw16_d(REG6,GLOBAL_BkgrdClipX1);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL)

    VoC_lw16_d(REG4,GLOBAL_BkgrdWidth);

    VoC_multf32_rd(ACC0,REG4,GLOBAL_BkgrdClipY1);

    VoC_sw32_d(ACC1,rgb_output_ptr);

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);

    VoC_shr32_ri(ACC0,-1,DEFAULT);

    VoC_add32_rd(ACC0,ACC0,GLOBAL_BkgrdBufferAddr);

    VoC_sw32_d(ACC1,alpha_output_ptr);

    VoC_sw32_d(ACC0,bkgrd_ptr);

    VoC_jump(Coolsand_PngDec_L22);
Coolsand_PngDec_L19:


    /*
        case 1:
            {
                if(!PNGOutBuffer || !AlphaOutBuffer)
                    return INPUTERR;
                res= (INT32)(PngCutOffsetX2- PngCutOffsetX1 +1)*(INT32)(PngCutOffsetY2- PngCutOffsetY1 +1)<<1;//rgb565
                if(res > OutBufferSize)
                    return INPUTERR;
                res>>=1;//alpha
                if(res > AlphaBufferSize)
                    return INPUTERR;
                rgb_output_ptr= PNGOutBuffer;
                alpha_output_ptr= AlphaOutBuffer;
                bkgrd_ptr= 0;

            }
            break;
    */

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG2,2,IN_PARALLEL);

    VoC_lw16_d(REG3,png_decode_mode);

    VoC_bne16_rr(Coolsand_PngDec_L20,REG1,REG3)

    VoC_bnez32_d(Coolsand_PngDec_L20_0,GLOBAL_PNGOutBufferAddr)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L20_0:


    VoC_bnez32_d(Coolsand_PngDec_L20_1,GLOBAL_AlphaOutBufferAddr)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L20_1:

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL)

    VoC_lw16_d(REG6,PngCutOffsetX2);

    VoC_sub16_rd(REG6,REG6,PngCutOffsetX1);

    VoC_lw16_d(REG7,PngCutOffsetY2);

    VoC_sub16_rd(REG7,REG7,PngCutOffsetY1);

    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG7,DEFAULT);

    VoC_NOP();

    VoC_shr32_ri(ACC0,-1,DEFAULT);

    VoC_bngt32_rd(Coolsand_PngDec_L20_2,ACC0,GLOBAL_PNGOutBufferSize)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L20_2:

    VoC_shr32_ri(ACC0,1,DEFAULT);

    VoC_bngt32_rd(Coolsand_PngDec_L20_3,ACC0,GLOBAL_AlphaOutBufferSize)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L20_3:

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32_d(ACC1,GLOBAL_PNGOutBufferAddr);
    VoC_lw32_d(RL7,GLOBAL_AlphaOutBufferAddr);
    VoC_sw32_d(ACC1,rgb_output_ptr);
    VoC_sw32_d(RL7,alpha_output_ptr);
    VoC_sw32_d(ACC0,bkgrd_ptr);

    VoC_jump(Coolsand_PngDec_L22);
Coolsand_PngDec_L20:

    /*

        case 2:
            {
                if(!PNGOutBuffer)
                    return INPUTERR;
                res= (INT32)(PngCutOffsetX2- PngCutOffsetX1 +1)*(INT32)(PngCutOffsetY2- PngCutOffsetY1 +1)<<2;//alphaRGB
                if(res > OutBufferSize)
                    return INPUTERR;
                rgb_output_ptr= PNGOutBuffer;
                alpha_output_ptr= 0;
                bkgrd_ptr= 0;

            }
            break;
    */

    VoC_lw16_d(REG3,png_decode_mode);
    VoC_bne16_rr(Coolsand_PngDec_L21,REG2,REG3)

    VoC_bnez32_d(Coolsand_PngDec_L21_0,GLOBAL_PNGOutBufferAddr)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L21_0:


    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL)

    VoC_lw16_d(REG6,PngCutOffsetX2);

    VoC_sub16_rd(REG6,REG6,PngCutOffsetX1);

    VoC_lw16_d(REG7,PngCutOffsetY2);

    VoC_sub16_rd(REG7,REG7,PngCutOffsetY1);

    VoC_add16_rr(REG6,REG6,REG5,DEFAULT);
    VoC_add16_rr(REG7,REG7,REG5,IN_PARALLEL);

    VoC_multf32_rr(ACC0,REG6,REG7,DEFAULT);

    VoC_NOP();

    VoC_shr32_ri(ACC0,-2,DEFAULT);

    VoC_bngt32_rd(Coolsand_PngDec_L21_1,ACC0,GLOBAL_PNGOutBufferSize)
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L21_1:

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw32_d(ACC1,GLOBAL_PNGOutBufferAddr);
    VoC_sw32_d(ACC0,alpha_output_ptr);
    VoC_sw32_d(ACC0,bkgrd_ptr);
    VoC_sw32_d(ACC1,rgb_output_ptr);

    VoC_jump(Coolsand_PngDec_L22);
Coolsand_PngDec_L21:
    /*
        default:
            return INPUTERR;
    */
    VoC_lw16i(REG7,INPUTERR);
    VoC_jump(Coolsand_PngDec_End);

Coolsand_PngDec_L22:


    /*
        if(IHDRInfo.BitDepth>8)
        {
            mmi_trace(TRUE,"PNG BitDepth=%d unspport\n",IHDRInfo.BitDepth);
            return SUPPORTERR;
        }
    */
    VoC_lw16i_short(REG2,8,DEFAULT);

    VoC_bnlt16_rd(Coolsand_PngDec_L23,REG2,IHDRInfo_BitDepth)

    VoC_lw16i(REG7,SUPPORTERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_L23:

    VoC_lw16_d(REG2,IHDRInfo_Width);

    VoC_shr16_ri(REG2,-2,DEFAULT);
    VoC_lw16i_short(REG3,32,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);

    VoC_lw16i(REG3,GLOBAL_RawOutBuf);

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);


    VoC_NOP();

    VoC_sw16_d(REG2,GLOBAL_PreRow);



    //  for (;;)

Coolsand_PngDec_Main_Loop_L0:

    VoC_jal(Coolsand_DmaInData);

    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_lw32_d(RL7,GLOBAL_byteleft);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG23,IN_PARALLEL);//      PngStreamBuf.byteleft -=4;

    VoC_lw16i_short(REG6,4,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,REG67,DEFAULT);

    VoC_push32(REG45,DEFAULT);

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_jal(Coolsand_crc32);

    VoC_pop32(REG45,DEFAULT);

    VoC_bez16_r(Coolsand_PngDec_Main_Loop_L1,REG7);
    VoC_lw16i(REG7,CRCERR);
    VoC_jump(Coolsand_PngDec_End);
Coolsand_PngDec_Main_Loop_L1:

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG23)//       PngStreamBuf.byteleft -=4;

    VoC_lw16i(REG2,0x524d);//cHRM
    VoC_lw16i(REG3,0x6348);

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_cHRM,REG67,REG23)

    VoC_jal(Coolsand_cHRMHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);


Coolsand_PngDec_Main_Loop_cHRM:


    VoC_lw16i(REG2,0x4d41);//gAMA
    VoC_lw16i(REG3,0x6741);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_gAMA,REG67,REG23)

    VoC_jal(Coolsand_gAMAHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_gAMA:



    VoC_lw16i(REG2,0x4350);//iCCP
    VoC_lw16i(REG3,0x6943);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_iCCP,REG67,REG23)

    VoC_jal(Coolsand_iCCPHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_iCCP:


    VoC_lw16i(REG2,0x4954);//sBIT
    VoC_lw16i(REG3,0x7342);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_sBIT,REG67,REG23)

    VoC_jal(Coolsand_sBITHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_sBIT:


    VoC_lw16i(REG2,0x4742);//sRGB
    VoC_lw16i(REG3,0x7352);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_sRGB,REG67,REG23)

    VoC_jal(Coolsand_sRGBHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_sRGB:


    VoC_lw16i(REG2,0x5445);//PLTE
    VoC_lw16i(REG3,0x504c);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_PLTE,REG67,REG23)

    VoC_jal(Coolsand_PLTEHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_PLTE:


    VoC_lw16i(REG2,0x4744);//bKGD
    VoC_lw16i(REG3,0x624b);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_bKGD,REG67,REG23)

    VoC_jal(Coolsand_bKGDHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_bKGD:


    VoC_lw16i(REG2,0x5354);//hIST
    VoC_lw16i(REG3,0x6849);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_hIST,REG67,REG23)

    VoC_jal(Coolsand_hISTHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_hIST:


    VoC_lw16i(REG2,0x4e53);//tRNS
    VoC_lw16i(REG3,0x7452);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_tRNS,REG67,REG23)

    VoC_jal(Coolsand_tRNSHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_tRNS:



    VoC_lw16i(REG2,0x5973);//pHYs
    VoC_lw16i(REG3,0x7048);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_pHYs,REG67,REG23)

    VoC_jal(Coolsand_pHYsHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_pHYs:


    VoC_lw16i(REG2,0x4c54);//sPLT
    VoC_lw16i(REG3,0x7350);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_sPLT,REG67,REG23)

    VoC_jal(Coolsand_sPLTHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_sPLT:



    VoC_lw16i(REG2,0x4154);//IDAT
    VoC_lw16i(REG3,0x4944);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_IDAT,REG67,REG23)

    VoC_jal(Coolsand_IDATHandle);

    VoC_bez16_r(Coolsand_PngDec_Main_Loop_L0,REG7);

    VoC_lw16i(REG6,CLIPOVER);
    VoC_bne16_rr(Coolsand_PngDec_End,REG7,REG6)
    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_jump(Coolsand_PngDec_End);

Coolsand_PngDec_Main_Loop_IDAT:


    VoC_lw16i(REG2,0x4d45);//tIME
    VoC_lw16i(REG3,0x7449);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_tIME,REG67,REG23)

    VoC_jal(Coolsand_tIMEHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_tIME:


    VoC_lw16i(REG2,0x5874);//iTXt
    VoC_lw16i(REG3,0x6954);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_iTXt,REG67,REG23)

    VoC_jal(Coolsand_iTXtHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_iTXt:




    VoC_lw16i(REG2,0x5874);//tEXt
    VoC_lw16i(REG3,0x7445);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_tEXt,REG67,REG23)

    VoC_jal(Coolsand_tEXtHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_tEXt:



    VoC_lw16i(REG2,0x5874);//zTXt
    VoC_lw16i(REG3,0x7a54);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_zTXt,REG67,REG23)

    VoC_jal(Coolsand_zTXtHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);

Coolsand_PngDec_Main_Loop_zTXt:


    VoC_lw16i(REG2,0x4e44);//IEND
    VoC_lw16i(REG3,0x4945);

    VoC_bne32_rr(Coolsand_PngDec_Main_Loop_IEND,REG67,REG23)

    VoC_jal(Coolsand_IENDHandle);

    //  VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_End);

Coolsand_PngDec_Main_Loop_IEND:



    VoC_jal(Coolsand_UnknownHandle);

    VoC_bnez16_r(Coolsand_PngDec_End,REG7);

    VoC_jump(Coolsand_PngDec_Main_Loop_L0);


Coolsand_PngDec_End:

    //output area width/height
    //*ImgWidth = (UINT32)(PngCutOffsetX2- PngCutOffsetX1+ 1);
    //*ImgHeigh = (UINT32)(PngCutOffsetY2- PngCutOffsetY1+ 1);

    VoC_sw16_d(REG7,GLOBAL_ERR_STATUS);

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add16_rd(REG2,REG1,PngCutOffsetX2);
    VoC_sub16_rd(REG2,REG2,PngCutOffsetX1);

    VoC_add16_rd(REG4,REG1,PngCutOffsetY2);
    VoC_sub16_rd(REG4,REG4,PngCutOffsetY1);

    VoC_shru32_ri(REG01,16,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_lw32_d(REG67,GLOBAL_ImgWidth);


    VoC_sw32_d(REG23,CFG_DMA_DATA_SINGLE);

    // single write
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw32_d(REG67,GLOBAL_ImgHeigh);


    VoC_sw32_d(REG45,CFG_DMA_DATA_SINGLE);

    // single write
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_NOP();

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_sw32_d(REG01,GLOBAL_PNGDEC_STATUS);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_PngDecReset
 *
 * Description: Png decoder reset
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/01/2012
 **************************************************************************************/
void Coolsand_PNG_Init(void)
{

    VoC_lw16i_short(REG0,0,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    //clear wrap regs
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_lw16i_short(WRAP1,0,DEFAULT);
    VoC_lw16i_short(WRAP2,0,DEFAULT);
    VoC_lw16i_short(WRAP3,0,DEFAULT);
    VoC_lw16i_short(BITCACHE,0,DEFAULT);

    //clear DMA reg
    VoC_sw16_d(REG0,CFG_DMA_WRAP);

    //clear status reg
    VoC_lw16i(STATUS,STATUS_CLR);

    //init ping pang flag
    VoC_sw16_d(REG0,GLOBAL_PingPangFlag);

    //init err status
    VoC_sw16_d(REG0,GLOBAL_ERR_STATUS);

    //init the PNGSignature
    VoC_sw16_d(REG0,transform);
    VoC_sw16_d(REG0,TransNum);
    VoC_sw16_d(REG0,CurRowNumber);
    VoC_sw32_d(REG01,IDATSize);

    /*  PNGzstream.next_in=NULL;
        PNGzstream.avail_in=0;
        PNGzstream.avail_out=0;
        PNGzstream.total_in=0;
        PNGzstream.total_out=0;
        PNGzstream.adler=0;
        PNGzstream.state=Z_NULL;
        PNGzstream.msg=NULL;
        PNGzstream.next_out=NULL;
        PNGzstream.data_type=0;
        PNGzstream.zalloc=NULL;
        PNGzstream.zfree=NULL;
        PNGzstream.opaque=NULL;
        PNGzstream.adler=0;
        PNGzstream.reserved=0;
        */



    VoC_sw32_d(REG01,PNGzstream_avail_out);
    VoC_sw32_d(REG01,PNGzstream_avail_in);
    VoC_sw32_d(REG01,PNGzstream_total_out);
    VoC_sw32_d(REG01,PNGzstream_total_in);
    VoC_sw32_d(REG01,PNGzstream_data_type);
    VoC_sw32_d(REG01,PNGzstream_adler);

    /*  CurRowNumber=0;
        CurPassRowNum=0;
        CurPassRowBytes=0;
        CurInterlaceRowWidth=0;
        CurPass=0;

        memset(&bKGDInfo,0,sizeof(bKGDINFO));
        memset(&IHDRInfo,0,sizeof(IHDRCHUNK));

        tRNSIndex = NULL;
        RowBytes=0;
        transform=0;
        ImagePass=0;
        PixelDepth=0;
    */
    VoC_sw16_d(REG0,CurRowNumber);
    VoC_sw16_d(REG0,CurPassRowNum);
    VoC_sw32_d(REG01,CurPassRowBytes);
    VoC_sw16_d(REG0,CurInterlaceRowWidth);
    VoC_sw16_d(REG0,CurPass);

    VoC_sw16_d(REG0,transform);
    VoC_sw16_d(REG0,ImagePass);
    VoC_sw32_d(REG01,RowBytes);
    VoC_sw16_d(REG0,PixelDepth);


    VoC_lw16i(REG2,1024);

    VoC_lw16i_short(REG5,8,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16i(REG6,GLOBAL_INBUF);

    VoC_lw16i(REG7,(1<<GLOBAL_WRAP_BIT)/2);

    VoC_sw32_d(REG23,GLOBAL_CONST_1024_ADDR);
    VoC_sw16_d(REG5,GLOBAL_CONST_8_ADDR);
    VoC_sw16_d(REG6,GLOBAL_INBUF_ADDR);
    VoC_sw16_d(REG7,GLOBAL_CONST_512_ADDR);

    //memset(RowBuffer,0,((IHDRInfo.Width<<2)+ 32)<<1);


    //8192=256*16*2

    VoC_lw16i_set_inc(REG2,GLOBAL_RawOutBuf,2);

    VoC_loop_i(1,16)
    VoC_loop_i(0,0)
    VoC_sw32inc_p(REG01,REG2,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

//  memset(&bKGDInfo,0,sizeof(bKGDINFO));
//  memset(&IHDRInfo,0,sizeof(IHDRCHUNK));

    VoC_sw16_d(REG0,bKGDInfo_bKGDGrey);
    VoC_sw16_d(REG0,bKGDInfo_bKGDRed);
    VoC_sw16_d(REG0,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG0,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG0,bKGDInfo_bKGDPlteIndex);

    VoC_sw16_d(REG0,IHDRInfo_Width);
    VoC_sw16_d(REG0,IHDRInfo_Height);
    VoC_sw16_d(REG0,IHDRInfo_BitDepth);
    VoC_sw16_d(REG0,IHDRInfo_ColorType);
    VoC_sw16_d(REG0,IHDRInfo_Compression);
    VoC_sw16_d(REG0,IHDRInfo_Fileter);
    VoC_sw16_d(REG0,IHDRInfo_Interlace);


    VoC_lw16i(REG4,0x8950);
    VoC_lw16i(REG5,0x4e47);
    VoC_lw16i(REG6,0x0d0a);
    VoC_lw16i(REG7,0x1a0a);

    VoC_sw32_d(REG45,PNGSignature0);
    VoC_sw32_d(REG67,PNGSignature1);

    VoC_sw16_d(REG0,FileChunks_IHDRNum);
    VoC_sw16_d(REG0,FileChunks_PLTENum);
    VoC_sw16_d(REG0,FileChunks_IDATNum);
    VoC_sw16_d(REG0,FileChunks_IENDNum);
    VoC_sw16_d(REG0,FileChunks_cHRMNum);
    VoC_sw16_d(REG0,FileChunks_gAMANum);
    VoC_sw16_d(REG0,FileChunks_iCCPNum);
    VoC_sw16_d(REG0,FileChunks_sBITNum);
    VoC_sw16_d(REG0,FileChunks_sRGBNum);
    VoC_sw16_d(REG0,FileChunks_bKGDNum);
    VoC_sw16_d(REG0,FileChunks_hISTNum);
    VoC_sw16_d(REG0,FileChunks_tRNSNum);
    VoC_sw16_d(REG0,FileChunks_hISTNum);
    VoC_sw16_d(REG0,FileChunks_pHYsNum);
    VoC_sw16_d(REG0,FileChunks_sPLTNum);
    VoC_sw16_d(REG0,FileChunks_tIMENum);
    VoC_sw16_d(REG0,FileChunks_iTXtNum);
    VoC_sw16_d(REG0,FileChunks_tEXtNum);
    VoC_sw16_d(REG0,FileChunks_zTXtNum);

    //  const int Pass_Xstart[7] = {0, 4, 0, 2, 0, 1, 0};
    //  const int Pass_Xinc[7] = {8, 8, 4, 4, 2, 2, 1};

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,2,IN_PARALLEL);
    VoC_lw16i_short(REG6,4,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_sw16_d(REG0,Pass_Xstart+0);
    VoC_sw16_d(REG6,Pass_Xstart+1);
    VoC_sw16_d(REG0,Pass_Xstart+2);
    VoC_sw16_d(REG5,Pass_Xstart+3);
    VoC_sw16_d(REG0,Pass_Xstart+4);
    VoC_sw16_d(REG4,Pass_Xstart+5);
    VoC_sw16_d(REG0,Pass_Xstart+6);

    VoC_sw16_d(REG7,Pass_Xinc+0);
    VoC_sw16_d(REG7,Pass_Xinc+1);
    VoC_sw16_d(REG6,Pass_Xinc+2);
    VoC_sw16_d(REG6,Pass_Xinc+3);
    VoC_sw16_d(REG5,Pass_Xinc+4);
    VoC_sw16_d(REG5,Pass_Xinc+5);
    VoC_sw16_d(REG4,Pass_Xinc+6);

// const int Pass_Ystart[7] = {0, 0, 4, 0, 2, 0, 1};
//  const int Pass_Yinc[7] = {8, 8, 8, 4, 4, 2, 2};

    VoC_sw16_d(REG0,Pass_Ystart+0);
    VoC_sw16_d(REG0,Pass_Ystart+1);
    VoC_sw16_d(REG6,Pass_Ystart+2);
    VoC_sw16_d(REG0,Pass_Ystart+3);
    VoC_sw16_d(REG5,Pass_Ystart+4);
    VoC_sw16_d(REG0,Pass_Ystart+5);
    VoC_sw16_d(REG4,Pass_Ystart+6);

    VoC_sw16_d(REG7,Pass_Yinc+0);
    VoC_sw16_d(REG7,Pass_Yinc+1);
    VoC_sw16_d(REG7,Pass_Yinc+2);
    VoC_sw16_d(REG6,Pass_Yinc+3);
    VoC_sw16_d(REG6,Pass_Yinc+4);
    VoC_sw16_d(REG5,Pass_Yinc+5);
    VoC_sw16_d(REG5,Pass_Yinc+6);


//  voc_short order[20],x//    static const unsigned short order[19] = /* permutation of code lengths */
    //{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};


    VoC_lw16i_set_inc(REG0,order,2);

    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_lw16i_short(REG5,17,IN_PARALLEL);
    VoC_lw16i_short(REG6,18,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);


    VoC_lw16i_short(REG4,8,DEFAULT);
    VoC_lw16i_short(REG5,7,IN_PARALLEL);
    VoC_lw16i_short(REG6,9,DEFAULT);
    VoC_lw16i_short(REG7,6,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);


    VoC_lw16i_short(REG4,10,DEFAULT);
    VoC_lw16i_short(REG5,5,IN_PARALLEL);
    VoC_lw16i_short(REG6,11,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);


    VoC_lw16i_short(REG4,12,DEFAULT);
    VoC_lw16i_short(REG5,3,IN_PARALLEL);
    VoC_lw16i_short(REG6,13,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);


    VoC_lw16i_short(REG4,14,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);
    VoC_lw16i_short(REG6,15,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16i(REG1,GLOBAL_RawOutBuf);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_sw32inc_p(REG67,REG0,DEFAULT);


    VoC_sw16_d(REG1,GLOBAL_RawOutBufAddr)

    VoC_lw16i(REG5,state_codes_op);
    VoC_lw16i(REG6,state_codes_bits);
    VoC_lw16i(REG7,state_codes_val);

    VoC_sw16_d(REG5,state_codes_op_addr);
    VoC_sw16_d(REG6,state_codes_bits_addr);
    VoC_sw16_d(REG7,state_codes_val_addr);

//const int PassMask[7] = {0x80, 0x08, 0x88, 0x22, 0xaa, 0x55, 0xff};

    VoC_lw16i(REG4,0x80);
    VoC_lw16i(REG5,0x08);
    VoC_lw16i(REG6,0x88);
    VoC_lw16i(REG7,0x22);

    VoC_sw16_d(REG4,PassMask+0);
    VoC_sw16_d(REG5,PassMask+1);
    VoC_sw16_d(REG6,PassMask+2);
    VoC_sw16_d(REG7,PassMask+3);

    VoC_lw16i(REG4,0xaa);
    VoC_lw16i(REG5,0x55);
    VoC_lw16i(REG6,0xff);

    VoC_sw16_d(REG4,PassMask+4);
    VoC_sw16_d(REG5,PassMask+5);
    VoC_sw16_d(REG6,PassMask+6);

    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_IHDRHandle
 *
 * Description: processing IHDR chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/02/2012
 **************************************************************************************/
void Coolsand_IHDRHandle(void)
{

//  FileChunks.IHDRNum++;

//  if (FileChunks.IHDRNum > 1 || ChunkLength != 13)
//  {
//      return IHDRERR;
//  }

//  VoC_push16(RA,DEFAULT);

    VoC_lw16d_set_inc(REG1,FileChunks_IHDRNum,1);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG6,13,IN_PARALLEL);

    VoC_inc_p(REG1,DEFAULT);
    VoC_lw16i_short(REG2,1,DEFAULT);

    VoC_sw16_d(REG1,FileChunks_IHDRNum);

    VoC_bngt16_rr(Coolsand_IHDRHandle_L0,REG1,REG2)
    VoC_lw16i(REG7,IHDRERR);
    VoC_jump(Coolsand_IHDRHandle_End);
Coolsand_IHDRHandle_L0:

    VoC_be32_rr(Coolsand_IHDRHandle_L1,REG45,REG67)
    VoC_lw16i(REG7,IHDRERR);
    VoC_jump(Coolsand_IHDRHandle_End);
Coolsand_IHDRHandle_L1:



//  IHDRInfo.Width = (*(*PngFileContent) << 24) + ((*(*PngFileContent + 1)) << 16)
//          + ((*(*PngFileContent + 2)) << 8) + (*(*PngFileContent + 3));
//  IHDRInfo.Height = (*(*PngFileContent + 4) << 24) + ((*(*PngFileContent + 5)) << 16)
//          + ((*(*PngFileContent + 6)) << 8) + (*(*PngFileContent + 7));
//  IHDRInfo.BitDepth = (*PngFileContent)[8];
//  IHDRInfo.ColorType = (*PngFileContent)[9];
//  IHDRInfo.Compression = (*PngFileContent)[10];
//  IHDRInfo.Fileter = (*PngFileContent)[11];
//  IHDRInfo.Interlace = (*PngFileContent)[12];


    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG45,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_sw16_d(REG6,IHDRInfo_Width);

    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_sw16_d(REG6,IHDRInfo_Height);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_rbinc_i(REG7,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);

    VoC_sw16_d(REG4,IHDRInfo_BitDepth);
    VoC_sw16_d(REG5,IHDRInfo_ColorType);
    VoC_sw16_d(REG6,IHDRInfo_Compression);
    VoC_sw16_d(REG7,IHDRInfo_Fileter);
    VoC_sw16_d(REG7,IHDRInfo_Interlace);

    /*  switch(IHDRInfo.ColorType)
        {
        case GreyScale:
        case IndexColor:
            ImgChannels = 1;
            break;

        case TrueColor:
            ImgChannels = 3;
            break;

        case GreyScaleAlpha:
            ImgChannels = 2;
            break;

        case TrueColorAlpha:
            ImgChannels = 4;
            break;
        }*/

    //REG5:IHDRInfo.ColorType


    VoC_lw16i_short(REG2,GreyScale,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_be16_rr(Coolsand_IHDRHandle_L2,REG5,REG2)

    VoC_lw16i_short(REG2,IndexColor,DEFAULT);
    //VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_be16_rr(Coolsand_IHDRHandle_L2,REG5,REG2)

    VoC_lw16i_short(REG2,TrueColor,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);

    VoC_be16_rr(Coolsand_IHDRHandle_L2,REG5,REG2)

    VoC_lw16i_short(REG2,GreyScaleAlpha,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_be16_rr(Coolsand_IHDRHandle_L2,REG5,REG2)

    VoC_lw16i_short(REG2,TrueColorAlpha,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

Coolsand_IHDRHandle_L2:

    //  PixelDepth = IHDRInfo.BitDepth * ImgChannels;

    VoC_mult16_rd(REG2,REG7,IHDRInfo_BitDepth);//PixelDepth

    VoC_sw16_d(REG7,ImgChannels);

//  RowBytes = ROWBYTES(PixelDepth, IHDRInfo.Width);

//  #define ROWBYTES(pixel_bits, width) \
//  ((pixel_bits) >= 8 ? \
//  ((width) * (((UINT32)(pixel_bits)) >> 3)) : \
//  (( ((width) * ((UINT32)(pixel_bits))) + 7) >> 3) )

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_sw16_d(REG2,PixelDepth);

    VoC_multf32_rd(ACC0,REG2,IHDRInfo_Width);

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_bngt16_rd(Coolsand_IHDRHandle_L3,REG3,IHDRInfo_BitDepth)

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

Coolsand_IHDRHandle_L3:

    VoC_shr32_ri(ACC0,3,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_sw32_d(ACC0,RowBytes);

//  *PngFileContent += ChunkLength + 4;
//  PngStreamBuf.byteleft -=(ChunkLength +4);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG23,IN_PARALLEL);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);

Coolsand_IHDRHandle_End:

//  VoC_pop16(RA,DEFAULT);

//  VoC_NOP();

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_crc32
 *
 * Description:   CRC32
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/05/2012
 **************************************************************************************/
void Coolsand_crc32(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16_d(REG7,GLOBAL_PingPangFlag);

    VoC_lw16i_set_inc(REG2,GLOBAL_INBUF,2)
    VoC_lw16i_set_inc(REG3,GLOBAL_DMATEMPBUF,2)

    VoC_push16(REG7,DEFAULT);

    VoC_lw32inc_p(ACC1,REG2,DEFAULT);

    VoC_loop_i(1,4)
    VoC_loop_i(0,128)
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

    VoC_lw16i(RL7_LO,256);

    VoC_lw32_d(ACC1,GLOBAL_InBufAddr);

    VoC_sw16_d(BITCACHE,GLOBAL_BITCACHE);

    VoC_lw16i(REG1,crc_table);

    VoC_lw16i_short(RL7_HI,0,DEFAULT);
    VoC_push32(ACC1,IN_PARALLEL);

    VoC_push16(REG0,DEFAULT);
    VoC_push32(RL6,IN_PARALLEL);

    VoC_lw16i_short(REG2,-1,DEFAULT);
    VoC_lw16i_short(REG3,-1,IN_PARALLEL);

    VoC_lw16i_short(ACC1_LO,1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_bngtz32_r(Coolsand_crc32_End,REG45)

    //#define DO1 crc = crc_table[0][((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8)

    VoC_bgt32_rr(Coolsand_crc32_L1,RL7,REG45)

Coolsand_crc32_L0:


    VoC_jal(Coolsand_DmaInData);

    VoC_lw16i(REG7,0xff);

    VoC_loop_i(0,0)

    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_xor16_rr(REG2,REG6,DEFAULT);
    VoC_movreg32(ACC0,REG23,IN_PARALLEL);

    VoC_and16_rr(REG2,REG7,DEFAULT);
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);

    VoC_shr16_ri(REG2,-1,DEFAULT);

    VoC_add16_rr(REG2,REG2,REG1,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_lw32_p(REG23,REG2,DEFAULT);

    VoC_xor32_rr(REG23,ACC0,DEFAULT);
    VoC_sub32_rr(REG45,REG45,ACC1,IN_PARALLEL);

    VoC_endloop0

    VoC_bngt32_rr(Coolsand_crc32_L0,RL7,REG45)

Coolsand_crc32_L1:
    VoC_bngtz32_r(Coolsand_crc32_End,REG45)


    VoC_jal(Coolsand_DmaInData);

    VoC_lw16i(REG7,0xff);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0

    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_xor16_rr(REG2,REG6,DEFAULT);
    VoC_movreg32(ACC0,REG23,IN_PARALLEL);

    VoC_and16_rr(REG2,REG7,DEFAULT);
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);

    VoC_shr16_ri(REG2,-1,DEFAULT);

    VoC_add16_rr(REG2,REG2,REG1,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_lw32_p(REG23,REG2,DEFAULT);

    VoC_xor32_rr(REG23,ACC0,DEFAULT);
    VoC_sub32_rr(REG45,REG45,ACC1,IN_PARALLEL);

    VoC_endloop0

Coolsand_crc32_End:

    VoC_rbinc_i(REG5,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_lw16i_short(ACC1_LO,-1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,-1,IN_PARALLEL);

    VoC_xor32_rr(REG23,ACC1,DEFAULT);
    VoC_pop32(RL6,IN_PARALLEL);

    VoC_pop16(REG0,DEFAULT);
    VoC_pop32(ACC0,IN_PARALLEL);

    VoC_pop16(REG6,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16_d(BITCACHE,GLOBAL_BITCACHE);

    VoC_sw32_d(ACC0,GLOBAL_InBufAddr);

    VoC_sw16_d(REG6,GLOBAL_PingPangFlag);

    VoC_be32_rr(Coolsand_crc32_L2,REG23,REG45);
    VoC_lw16i_short(REG7,1,DEFAULT);
Coolsand_crc32_L2:

    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_set_inc(REG2,GLOBAL_DMATEMPBUF,2)
    VoC_lw16i_set_inc(REG3,GLOBAL_INBUF,2)

    VoC_lw32inc_p(ACC1,REG2,DEFAULT);

    VoC_loop_i(1,4)
    VoC_loop_i(0,128)
    VoC_lw32inc_p(ACC1,REG2,DEFAULT);
    VoC_sw32inc_p(ACC1,REG3,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_cHRMHandle
 *
 * Description: processing cHRM chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_cHRMHandle(void)
{
    /*
        if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before cHRM!");
            return cHRMERR;
        }
        else if (FileChunks.PLTENum != 0)
        {
            //printf("Invalid cHRM after PLTE!");
            return cHRMERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invaild cHRM after IDAT!");
            return cHRMERR;
        }
        else if (FileChunks.cHRMNum > 1)
        {
            //printf("Duplicate cHRM chunk!");
            return cHRMERR;
        }
        else if (ChunkLength != 32)
        {
            //printf("Invalid cHRM chunk length!");
            return cHRMERR;
        }

        FileChunks.cHRMNum++;

    */




    VoC_push16(RA,DEFAULT);

    VoC_bnez16_d(Coolsand_cHRMHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_cHRMHandle_End);
Coolsand_cHRMHandle_L0:

    VoC_bez16_d(Coolsand_cHRMHandle_L1,FileChunks_PLTENum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_cHRMHandle_End);
Coolsand_cHRMHandle_L1:

    VoC_bez16_d(Coolsand_cHRMHandle_L2,FileChunks_IDATNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_cHRMHandle_End);
Coolsand_cHRMHandle_L2:

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_bnlt16_rd(Coolsand_cHRMHandle_L3,REG6,FileChunks_cHRMNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_cHRMHandle_End);
Coolsand_cHRMHandle_L3:

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG2,32,IN_PARALLEL);

    VoC_be32_rr(Coolsand_cHRMHandle_L4,REG23,REG45)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_cHRMHandle_End);
Coolsand_cHRMHandle_L4:

    //FileChunks.iCCPNum++;
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG6,REG6,FileChunks_cHRMNum);
    VoC_movreg32(ACC1,REG45,DEFAULT);
    VoC_sw16_d(REG1,FileChunks_cHRMNum);

    VoC_lw16_d(REG3,FileChunks_sRGBNum)

    VoC_bnez16_r(Coolsand_cHRMHandle_L5,REG3)

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_sw32_d(REG67,cHRMInfo_WhiteX);
    VoC_sw32_d(REG45,cHRMInfo_WhiteY);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_sw32_d(REG67,cHRMInfo_RedX);
    VoC_sw32_d(REG45,cHRMInfo_RedY);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_sw32_d(REG67,cHRMInfo_GreenX);
    VoC_sw32_d(REG45,cHRMInfo_GreenY);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG4,16,DEFAULT);


    VoC_sw32_d(REG67,cHRMInfo_BlueX);
    VoC_sw32_d(REG45,cHRMInfo_BlueY);

    VoC_jump(Coolsand_cHRMHandle_L6);

Coolsand_cHRMHandle_L5:

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_endloop0

Coolsand_cHRMHandle_L6:
    //*PngFileContent += 4;
    //PngStreamBuf.byteleft -= 4;

    VoC_sub32_dr(RL7,GLOBAL_byteleft,ACC1);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);
Coolsand_cHRMHandle_End:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_gAMAHandle
 *
 * Description: processing gAMA chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_gAMAHandle(void)
{

    VoC_push16(RA,DEFAULT);

    /*

        if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before gAMA!");
            return gAMAERR;
        }
        else if (FileChunks.PLTENum != 0)
        {
            //printf("Invalid gAMA after PLTE!");
            return gAMAERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invaild gAMA after IDAT!");
            return gAMAERR;
        }
        else if (FileChunks.gAMANum > 1)
        {
            //printf("Duplicate gAMA chunk!");
            return gAMAERR;
        }
        else if (ChunkLength != 4)
        {
            //printf("Invalid gAMA chunk length!");
            return gAMAERR;
        }
    */


    VoC_bnez16_d(Coolsand_gAMAHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,gAMAERR);
    VoC_jump(Coolsand_gAMAHandle_End);
Coolsand_gAMAHandle_L0:

    VoC_bez16_d(Coolsand_gAMAHandle_L1,FileChunks_PLTENum)
    VoC_lw16i(REG7,gAMAERR);
    VoC_jump(Coolsand_gAMAHandle_End);
Coolsand_gAMAHandle_L1:

    VoC_bez16_d(Coolsand_gAMAHandle_L2,FileChunks_IDATNum)
    VoC_lw16i(REG7,gAMAERR);
    VoC_jump(Coolsand_gAMAHandle_End);
Coolsand_gAMAHandle_L2:

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_bnlt16_rd(Coolsand_gAMAHandle_L3,REG6,FileChunks_gAMANum)
    VoC_lw16i(REG7,gAMAERR);
    VoC_jump(Coolsand_gAMAHandle_End);
Coolsand_gAMAHandle_L3:

    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_be32_rr(Coolsand_gAMAHandle_L4,REG23,REG45)
    VoC_lw16i(REG7,gAMAERR);
    VoC_jump(Coolsand_gAMAHandle_End);
Coolsand_gAMAHandle_L4:

    //FileChunks.iCCPNum++;
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG6,REG6,FileChunks_gAMANum);
    VoC_movreg32(ACC1,REG45,DEFAULT);
    VoC_sw16_d(REG1,FileChunks_gAMANum);

    VoC_lw16_d(REG3,FileChunks_sRGBNum)

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_bnez16_r(Coolsand_gAMAHandle_L7,REG3)

    VoC_lw16i(REG4,0xffff);
    VoC_lw16i(REG5,0x7fff);

    VoC_bngt32_rr(Coolsand_gAMAHandle_L5,REG67,REG45)
    VoC_movreg32(REG67,REG45,DEFAULT);
Coolsand_gAMAHandle_L5:

    VoC_bnltz32_r(Coolsand_gAMAHandle_L6,REG67)
    VoC_lw32z(REG67,DEFAULT);
    VoC_NOP();
Coolsand_gAMAHandle_L6:

    VoC_sw32_d(REG67,gammer);
Coolsand_gAMAHandle_L7:

    //*PngFileContent += 4;
    //PngStreamBuf.byteleft -= 4;

    VoC_sub32_dr(RL7,GLOBAL_byteleft,ACC1);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);
Coolsand_gAMAHandle_End:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_iCCPHandle
 *
 * Description: processing iCCP chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_iCCPHandle(void)
{

    /*  if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before iCCP!");
            return iCCPERR;
        }
        else if (FileChunks.PLTENum != 0)
        {
            //printf("Invalid iCCP after PLTE!");
            return iCCPERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invaild iCCP after IDAT!");
            return iCCPERR;
        }
        else if (FileChunks.iCCPNum > 1)
        {
            //printf("Duplicate iCCP chunk!");
            return iCCPERR;
        }
        else if (FileChunks.sRGBNum != 0)
        {
            //printf("There is a sRGB chunk!");
            return iCCPERR;
        }

        FileChunks.iCCPNum++;

    */
    VoC_push16(RA,DEFAULT);

    VoC_bnez16_d(Coolsand_iCCPHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_iCCPHandle_End);
Coolsand_iCCPHandle_L0:

    VoC_bez16_d(Coolsand_iCCPHandle_L1,FileChunks_PLTENum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_iCCPHandle_End);
Coolsand_iCCPHandle_L1:

    VoC_bez16_d(Coolsand_iCCPHandle_L2,FileChunks_IDATNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_iCCPHandle_End);
Coolsand_iCCPHandle_L2:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bnlt16_rd(Coolsand_iCCPHandle_L3,REG6,FileChunks_iCCPNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_iCCPHandle_End);
Coolsand_iCCPHandle_L3:

    VoC_bez16_d(Coolsand_iCCPHandle_L4,FileChunks_sRGBNum)
    VoC_lw16i(REG7,iCCPERR);
    VoC_jump(Coolsand_iCCPHandle_End);
Coolsand_iCCPHandle_L4:


    //FileChunks.iCCPNum++;
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG3,REG3,FileChunks_iCCPNum);
    VoC_movreg32(ACC1,REG45,DEFAULT);
    VoC_sw16_d(REG3,FileChunks_iCCPNum);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

Coolsand_iCCPHandle_L5:
    VoC_bngtz32_r(Coolsand_iCCPHandle_L6,REG45)

    VoC_jal(Coolsand_DmaInData);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);

    VoC_jump(Coolsand_iCCPHandle_L5);
Coolsand_iCCPHandle_L6:



    //*PngFileContent += 4;
    //PngStreamBuf.byteleft -= 4;

    VoC_sub32_dr(RL7,GLOBAL_byteleft,ACC1);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);
Coolsand_iCCPHandle_End:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}

/**************************************************************************************
 * Function:    Coolsand_sBITHandle
 *
 * Description: processing sBIT chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_sBITHandle(void)
{
    /*

    if (FileChunks.IHDRNum == 0)
    {
        //printf("Missing IHDR before sBIT!");
        return sBITERR;
    }
    else if (FileChunks.PLTENum != 0)
    {
        //printf("Invalid sBIT after PLTE!");
        return sBITERR;
    }
    else if (FileChunks.IDATNum != 0)
    {
        //printf("Invaild sBIT after IDAT!");
        return sBITERR;
    }
    else if (FileChunks.sBITNum > 1)
    {
        //printf("Duplicate sBIT chunk!");
        return sBITERR;
    }
    */

    //FileChunks.sBITNum++;

    VoC_bnez16_d(Coolsand_sBITHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L0:


    VoC_bez16_d(Coolsand_sBITHandle_L1,FileChunks_PLTENum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L1:

    VoC_bez16_d(Coolsand_sBITHandle_L2,FileChunks_IDATNum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L2:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_add16_rd(REG1,REG6,FileChunks_sBITNum);
    VoC_bnlt16_rd(Coolsand_sBITHandle_L3,REG6,FileChunks_sBITNum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L3:
    VoC_sw16_d(REG1,FileChunks_sBITNum);

    /*
        if (IHDRInfo.ColorType == GreyScale)
        {
            if (ChunkLength != 1)
            {
                //printf("Invalid sBIT chunk length!");
                return sBITERR;
            }

            SigBit->grey = **PngFileContent;
        }*/


    VoC_lw16i(REG1,GreyScale);

    VoC_bne16_rd(Coolsand_sBITHandle_L5,REG1,IHDRInfo_ColorType)

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_be32_rr(Coolsand_sBITHandle_L4,REG23,REG45)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L4:
    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_sw16_d(REG6,sBITInfo_grey);

    VoC_jump(Coolsand_sBITHandle_L12);

Coolsand_sBITHandle_L5:


    /*
        else if (IHDRInfo.ColorType == TrueColor || IHDRInfo.ColorType == IndexColor)
        {
            if (ChunkLength != 3)
            {
                //printf("Invalid sBIT chunk length!");
                return sBITERR;
            }

            SigBit->red = **PngFileContent;
            SigBit->green = *(*PngFileContent + 1);
            SigBit->blue = *(*PngFileContent + 2);
        }*/

    VoC_lw16i_short(REG1,TrueColor,DEFAULT);
    VoC_lw16i_short(REG2,IndexColor,IN_PARALLEL);

    VoC_be16_rd(Coolsand_sBITHandle_L6,REG1,IHDRInfo_ColorType)
    VoC_be16_rd(Coolsand_sBITHandle_L6,REG2,IHDRInfo_ColorType)

    VoC_jump(Coolsand_sBITHandle_L8);
Coolsand_sBITHandle_L6:

    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_be32_rr(Coolsand_sBITHandle_L7,REG23,REG45)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L7:
    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,8,DEFAULT);

    VoC_sw16_d(REG5,sBITInfo_red);
    VoC_sw16_d(REG6,sBITInfo_grey);
    VoC_sw16_d(REG7,sBITInfo_blue);

    VoC_jump(Coolsand_sBITHandle_L12);

Coolsand_sBITHandle_L8:




    /*  else if (IHDRInfo.ColorType == GreyScaleAlpha)
        {
            if (ChunkLength != 2)
            {
                //printf("Invalid sBIT chunk length!");
                return sBITERR;
            }

            SigBit->grey = **PngFileContent;
            SigBit->alpha = *(*PngFileContent + 1);
        }
    */



    VoC_lw16i(REG1,GreyScaleAlpha);

    VoC_bne16_rd(Coolsand_sBITHandle_L10,REG1,IHDRInfo_ColorType)

    VoC_lw16i_short(REG2,2,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_be32_rr(Coolsand_sBITHandle_L9,REG23,REG45)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L9:
    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_rbinc_i(REG7,8,DEFAULT);

    VoC_sw16_d(REG6,sBITInfo_grey);
    VoC_sw16_d(REG7,sBITInfo_alpha);

    VoC_jump(Coolsand_sBITHandle_L12);

Coolsand_sBITHandle_L10:


    /*  else if (IHDRInfo.ColorType == TrueColorAlpha)
        {
            if (ChunkLength != 4)
            {
                //printf("Invalid sBIT chunk length!");
                return sBITERR;
            }

            SigBit->red = **PngFileContent;
            SigBit->green = *(*PngFileContent + 1);
            SigBit->blue = *(*PngFileContent + 2);
            SigBit->alpha = *(*PngFileContent + 3);
        }*/


    VoC_lw16i(REG1,TrueColorAlpha);

    VoC_bne16_rd(Coolsand_sBITHandle_L12,REG1,IHDRInfo_ColorType)

    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_be32_rr(Coolsand_sBITHandle_L11,REG23,REG45)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_sBITHandle_End);
Coolsand_sBITHandle_L11:

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_rbinc_i(REG5,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_rbinc_i(REG7,8,DEFAULT);

    VoC_sw16_d(REG4,sBITInfo_red);
    VoC_sw16_d(REG5,sBITInfo_green);
    VoC_sw16_d(REG6,sBITInfo_blue);
    VoC_sw16_d(REG7,sBITInfo_alpha);

Coolsand_sBITHandle_L12:

    VoC_lbinc_p(REG0);

//  *PngFileContent += ChunkLength + 4;
//  PngStreamBuf.byteleft -=(ChunkLength +4);

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG23);//ChunkLength

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);

Coolsand_sBITHandle_End:

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_sRGBHandle
 *
 * Description: processing sRGB chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_sRGBHandle(void)
{


    VoC_push16(RA,DEFAULT);
    /*
        if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before sRGB!");
            return sRGBERR;
        }
        else if (FileChunks.PLTENum != 0)
        {
            //printf("Invalid sRGB after PLTE!");
            return sRGBERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invaild sRGB after IDAT!");
            return sRGBERR;
        }
        else if (FileChunks.sRGBNum > 1)
        {
            //printf("Duplicate sRGB chunk!");
            return sRGBERR;
        }
        else if (FileChunks.iCCPNum != 0)
        {
            //printf("There is a iCCP chunk!");
            return sRGBERR;
        }
        else if (ChunkLength != 1)
        {
            //printf("Invalid chunk length!");
            return sRGBERR;
        }

        FileChunks.sRGBNum++;
    */

    VoC_bnez16_d(Coolsand_sRGBHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);
Coolsand_sRGBHandle_L0:

    VoC_bez16_d(Coolsand_sRGBHandle_L1,FileChunks_PLTENum)
    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);
Coolsand_sRGBHandle_L1:

    VoC_bez16_d(Coolsand_sRGBHandle_L2,FileChunks_IDATNum)
    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);
Coolsand_sRGBHandle_L2:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_add16_rd(REG1,REG6,FileChunks_sRGBNum);
    VoC_bnlt16_rd(Coolsand_sRGBHandle_L3,REG6,FileChunks_sRGBNum)
    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);
Coolsand_sRGBHandle_L3:
    VoC_sw16_d(REG1,FileChunks_sRGBNum);

    VoC_bez16_d(Coolsand_sRGBHandle_L4,FileChunks_iCCPNum)
    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);
Coolsand_sRGBHandle_L4:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_be16_rr(Coolsand_sRGBHandle_L5,REG6,REG4)
    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);
Coolsand_sRGBHandle_L5:

    /*

        sRGBRenderContent = **PngFileContent;

        if (sRGBRenderContent > sRGBRenderingLast)
        {
            //printf("Unknown rendering content!");
            return sRGBERR;
        }

        gammer = 45455;

        cHRMInfo.WhiteX = 31270;
        cHRMInfo.WhiteY = 32900;
        cHRMInfo.RedX = 64000;
        cHRMInfo.RedY = 33000;
        cHRMInfo.GreenX = 30000;
        cHRMInfo.GreenY = 60000;
        cHRMInfo.BlueX = 15000;
        cHRMInfo.BlueY = 6000;
    */


    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_short(REG5,sRGBRenderingLast,IN_PARALLEL);


    VoC_lbinc_p(REG0);

    VoC_sw16_d(REG4,sRGBRenderContent)

    VoC_bngt16_rr(Coolsand_sRGBHandle_L6,REG4,REG5)

    VoC_lw16i(REG7,sRGBERR);
    VoC_jump(Coolsand_sRGBHandle_End);

Coolsand_sRGBHandle_L6:



    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16i(REG6,0xb18f);//45455
    VoC_lw16i(REG3,0x7a26);//31270

    VoC_sw32_d(REG67,gammer);

    VoC_lw16i(REG6,0x8084);//32900

    VoC_sw32_d(REG23,cHRMInfo_WhiteX);
    VoC_sw32_d(REG67,cHRMInfo_WhiteY);

    VoC_lw16i(REG3,0xfa00);//64000
    VoC_lw16i(REG6,0x80e8);//33000

    VoC_sw32_d(REG23,cHRMInfo_RedX);
    VoC_sw32_d(REG67,cHRMInfo_RedY);


    VoC_lw16i(REG3,0x7530);//30000
    VoC_lw16i(REG6,0xea60);//60000

    VoC_sw32_d(REG23,cHRMInfo_GreenX);
    VoC_sw32_d(REG67,cHRMInfo_GreenY);

    VoC_lw16i(REG3,0x3a98);//15000
    VoC_lw16i(REG6,0x1770);//6000

    VoC_sw32_d(REG23,cHRMInfo_BlueX);
    VoC_sw32_d(REG67,cHRMInfo_BlueY);

//  *PngFileContent += ChunkLength + 4;
//  PngStreamBuf.byteleft -=ChunkLength + 4;

    VoC_lbinc_p(REG0);

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG45);//ChunkLength

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);

Coolsand_sRGBHandle_End:


    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_PLTEHandle
 *
 * Description: processing PLTE chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_PLTEHandle(void)
{

    /*  if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before PLTE!");
            return PLTEERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invalid PLTE after IDAT!");
            return PLTEERR;
        }
        else if (FileChunks.PLTENum > 1)
        {
            //printf("Duplicate PLTE chunk!");
            return PLTEERR;
        }
        else if (ChunkLength > 768 || ChunkLength % 3)
        {
            //printf("Invalid PLTE chunk length!");
            return PLTEERR;
        }
    */
    VoC_push16(RA,DEFAULT);

    VoC_bnez16_d(Coolsand_PLTEHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_PLTEHandle_End);
Coolsand_PLTEHandle_L0:

    VoC_bez16_d(Coolsand_PLTEHandle_L1,FileChunks_IDATNum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_PLTEHandle_End);
Coolsand_PLTEHandle_L1:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_bnlt16_rd(Coolsand_PLTEHandle_L2,REG6,FileChunks_PLTENum)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_PLTEHandle_End);
Coolsand_PLTEHandle_L2:

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i(REG6,768);
    VoC_bngt32_rr(Coolsand_PLTEHandle_L3,REG45,REG67)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_PLTEHandle_End);
Coolsand_PLTEHandle_L3:

    VoC_lw16i_short(ACC0_LO,3,DEFAULT);
    VoC_lw16i_short(ACC0_HI,0,IN_PARALLEL);

    VoC_movreg32(ACC1,REG45,DEFAULT);

    VoC_lw16i_set_inc(REG3,0,1);

Coolsand_PLTEHandle_L4:
    VoC_bngtz32_r(Coolsand_PLTEHandle_L5,ACC1)
    VoC_sub32_rr(ACC1,ACC1,ACC0,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);
    VoC_jump(Coolsand_PLTEHandle_L4)
Coolsand_PLTEHandle_L5:

    VoC_bez32_r(Coolsand_PLTEHandle_L6,ACC1)
    VoC_lw16i(REG7,sBITERR);
    VoC_jump(Coolsand_PLTEHandle_End);
Coolsand_PLTEHandle_L6:


    //REG1: EntryNum = ChunkLength / 3;


    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,Palete,1);
    VoC_lbinc_p(REG0);

Coolsand_PLTEHandle_L7:
    VoC_bngtz16_r(Coolsand_PLTEHandle_L8,REG3)

    VoC_jal(Coolsand_DmaInData);

    VoC_rbinc_i(REG4,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,8,DEFAULT);
    VoC_sw16inc_p(REG4,REG1,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sw16inc_p(REG5,REG1,IN_PARALLEL);

    VoC_inc_p(REG3,DEFAULT);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);

    VoC_jump(Coolsand_PLTEHandle_L7);
Coolsand_PLTEHandle_L8:


    //PlteSize = ChunkLength;

    VoC_sw32_d(ACC1,PlteSize);
    //FileChunks.PLTENum++;
    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_add16_rd(REG6,REG6,FileChunks_PLTENum);
    VoC_lbinc_p(REG0);
    VoC_sw16_d(REG1,FileChunks_PLTENum);



    //*PngFileContent += 4;
    //PngStreamBuf.byteleft -= 4;

    VoC_sub32_dr(RL7,GLOBAL_byteleft,ACC1);//ChunkLength

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);
Coolsand_PLTEHandle_End:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_bKGDHandle
 *
 * Description: processing bKGD chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       06/01/2012
 **************************************************************************************/
void Coolsand_bKGDHandle(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    //FileChunks.bKGDNuM++;
    VoC_add16_rd(REG1,REG2,FileChunks_bKGDNum)

    VoC_lw16i_short(REG6,GreyScale,DEFAULT);
    VoC_lw16i_short(REG7,GreyScaleAlpha,IN_PARALLEL);

    VoC_sw16_d(REG1,FileChunks_bKGDNum);


    VoC_be16_rd(Coolsand_bKGDHandle_L0,REG6,IHDRInfo_ColorType)
    VoC_bne16_rd(Coolsand_bKGDHandle_L2,REG7,IHDRInfo_ColorType)
Coolsand_bKGDHandle_L0:
    VoC_lw16i_short(REG6,2,DEFAULT);

    VoC_be16_rr(Coolsand_bKGDHandle_L1,REG6,REG4)
    VoC_lw16i_short(REG7,bKGDERR,DEFAULT);
    VoC_jump(Coolsand_bKGDHandle_End);
Coolsand_bKGDHandle_L1:

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_lbinc_p(REG0);

    VoC_sw16_d(REG6,bKGDInfo_bKGDRed);
    VoC_sw16_d(REG6,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG6,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG6,bKGDInfo_bKGDGrey);
    VoC_jump(Coolsand_bKGDHandle_L8)
Coolsand_bKGDHandle_L2:


    VoC_lw16i_short(REG6,TrueColor,DEFAULT);
    VoC_lw16i_short(REG7,TrueColorAlpha,IN_PARALLEL);

    VoC_be16_rd(Coolsand_bKGDHandle_L3,REG6,IHDRInfo_ColorType)
    VoC_bne16_rd(Coolsand_bKGDHandle_L5,REG7,IHDRInfo_ColorType)
Coolsand_bKGDHandle_L3:
    VoC_lw16i_short(REG6,6,DEFAULT);

    VoC_be16_rr(Coolsand_bKGDHandle_L4,REG6,REG4)
    VoC_lw16i_short(REG7,bKGDERR,DEFAULT);
    VoC_jump(Coolsand_bKGDHandle_End);
Coolsand_bKGDHandle_L4:

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_sw16_d(REG6,bKGDInfo_bKGDRed);
    VoC_sw16_d(REG7,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG6,bKGDInfo_bKGDBlue);
    VoC_jump(Coolsand_bKGDHandle_L8)
Coolsand_bKGDHandle_L5:

    VoC_lw16i_short(REG6,IndexColor,DEFAULT);
    VoC_lw16_d(REG7,IHDRInfo_ColorType)
    VoC_bne16_rr(Coolsand_bKGDHandle_L8,REG6,REG7)

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_be16_rr(Coolsand_bKGDHandle_L6,REG6,REG4)
    VoC_lw16i_short(REG7,bKGDERR,DEFAULT);
    VoC_jump(Coolsand_bKGDHandle_End);
Coolsand_bKGDHandle_L6:

    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,8,DEFAULT);

    VoC_lbinc_p(REG0);
    VoC_sw16_d(REG6,bKGDInfo_bKGDPlteIndex);

    VoC_bez16_d(Coolsand_bKGDHandle_L8,PlteSize)

    VoC_bngt16_rd(Coolsand_bKGDHandle_L7,REG6,PlteSize)
    VoC_lw16i_short(REG7,bKGDERR,DEFAULT);
    VoC_jump(Coolsand_bKGDHandle_End);
Coolsand_bKGDHandle_L7:

    VoC_lw16i_set_inc(REG1,Palete,1);

    VoC_add16_rd(REG1,REG1,bKGDInfo_bKGDPlteIndex);
    VoC_add16_rd(REG1,REG1,bKGDInfo_bKGDPlteIndex);
    VoC_add16_rd(REG1,REG1,bKGDInfo_bKGDPlteIndex);

    VoC_NOP();

    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    VoC_lw16inc_p(REG3,REG1,DEFAULT);
    VoC_lw16_p(REG7,REG1,DEFAULT);

    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);
    VoC_sw16_d(REG3,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG7,bKGDInfo_bKGDBlue);

Coolsand_bKGDHandle_L8:

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG45);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);

Coolsand_bKGDHandle_End:

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_hISTHandle
 *
 * Description: processing hIST chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_hISTHandle(void)
{

    VoC_push16(RA,DEFAULT);


    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_tRNSHandle
 *
 * Description: processing tRNS chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_tRNSHandle(void)
{

    /*  if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before tRNS!");
            return tRNSERR;
        }
        else if (FileChunks.PLTENum == 0)
        {
            //printf("Missing PLTE before tRNS!");
            return tRNSERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invalid tRNS after IDAT!");
            return tRNSERR;
        }
        else if (FileChunks.tRNSNum > 1)
        {
            //printf("Duplicate tRNS chunk!");
            return tRNSERR;
        }

        FileChunks.tRNSNum++;

    */

    VoC_push16(RA,DEFAULT);

    VoC_bnez16_d(Coolsand_tRNSHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_L0:

    VoC_bnez16_d(Coolsand_tRNSHandle_L1,FileChunks_PLTENum)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_L1:

    VoC_bez16_d(Coolsand_tRNSHandle_L2,FileChunks_IDATNum)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_L2:

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_add16_rd(REG7,REG6,FileChunks_tRNSNum);
    VoC_bnlt16_rd(Coolsand_tRNSHandle_L3,REG6,FileChunks_tRNSNum)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_L3:

    VoC_sw16_d(REG7,FileChunks_tRNSNum);
    /*
        if (IHDRInfo.ColorType == GreyScale)
        {
            if (ChunkLength != 2)
            {
                //printf("Invalid tRNS chunk length!");
                return tRNSERR;
            }

            TransNum = 1;

            tRNSInfo.tRNSGrey = (**PngFileContent << 8) + *(*PngFileContent + 1);
        }
    */
    VoC_lw16i_short(REG6,GreyScale,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_tRNSHandle_GreyScale,REG6,IHDRInfo_ColorType)

    VoC_be16_rr(Coolsand_tRNSHandle_GreyScale_L0,REG4,REG7)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_GreyScale_L0:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_sw16_d(REG6,TransNum);
    VoC_sw16_d(REG7,tRNSInfo_tRNSGrey);
    VoC_jump(Coolsand_tRNSHandle_IndexColor);

Coolsand_tRNSHandle_GreyScale:


    /*
        else if (IHDRInfo.ColorType == TrueColor)
        {
            if (ChunkLength != 6)
            {
                //printf("Invalid tRNS chunk length!");
                return tRNSERR;
            }

            TransNum = 1;

            tRNSInfo.tRNSRed = (**PngFileContent << 8) + *(*PngFileContent + 1);
            tRNSInfo.tRNSGreen = (*(*PngFileContent + 2) << 8) + *(*PngFileContent + 3);
            tRNSInfo.tRNSBlue = (*(*PngFileContent + 4) << 8) + *(*PngFileContent + 5);
        }
    */

    VoC_lw16i_short(REG6,TrueColor,DEFAULT);
    VoC_lw16i_short(REG7,6,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_tRNSHandle_TrueColor,REG6,IHDRInfo_ColorType)

    VoC_be16_rr(Coolsand_tRNSHandle_TrueColor_L0,REG4,REG7)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_TrueColor_L0:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16_d(REG6,TransNum);

    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG7,16,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_rbinc_i(REG6,16,DEFAULT);
    VoC_sw16_d(REG6,tRNSInfo_tRNSRed);
    VoC_sw16_d(REG7,tRNSInfo_tRNSGreen);
    VoC_sw16_d(REG6,tRNSInfo_tRNSBlue);

    VoC_jump(Coolsand_tRNSHandle_IndexColor);

Coolsand_tRNSHandle_TrueColor:

    /*
        else if (IHDRInfo.ColorType == IndexColor)
        {
            if (ChunkLength > PlteSize || ChunkLength == 0)
            {
                //printf("Invalid tRNS chunk length!");
                return tRNSERR;
            }

            TransNum = ChunkLength;

    //      tRNSIndex = (BYTE *)OslMalloc(ChunkLength);
            tRNSIndex = tRNSIndexArray;
            memcpy(tRNSIndex, *PngFileContent, ChunkLength);
        }
    */

    VoC_lw16i_short(REG6,IndexColor,DEFAULT);

    VoC_bne16_rd(Coolsand_tRNSHandle_IndexColor,REG6,IHDRInfo_ColorType)

    VoC_bnez16_r(Coolsand_tRNSHandle_IndexColor_L0,REG4)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_IndexColor_L0:

    VoC_bngt16_rd(Coolsand_tRNSHandle_IndexColor_L1,REG4,PlteSize)
    VoC_lw16i(REG7,tRNSERR);
    VoC_jump(Coolsand_tRNSHandle_End);
Coolsand_tRNSHandle_IndexColor_L1:

    VoC_sw16_d(REG4,TransNum);

    VoC_lw16i_set_inc(REG1,tRNSIndex,1);

    VoC_lbinc_p(REG0);

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_lbinc_p(REG0);
    VoC_sw16inc_p(REG7,REG1,DEFAULT);
    VoC_endloop0

Coolsand_tRNSHandle_IndexColor:


    VoC_lbinc_p(REG0);

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG45);//ChunkLength

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);

Coolsand_tRNSHandle_End:

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_pHYsHandle
 *
 * Description: processing pHYs chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_pHYsHandle(void)
{

    /*
        if (FileChunks.IHDRNum == 0)
        {
            //printf("Missing IHDR before pHYs!");
            return pHYsERR;
        }
        else if (FileChunks.IDATNum != 0)
        {
            //printf("Invalid pHYs after IDAT!");
            return pHYsERR;
        }
        else if (FileChunks.pHYsNum > 1)
        {
            //printf("Duplicate pHYs chunk!");
            return pHYsERR;
        }
        else if (ChunkLength != 9)
        {
            //printf("Invalid pHYs chunk length!");
            return pHYsERR;
        }

        FileChunks.pHYsNum++;

        */

    VoC_bnez16_d(Coolsand_pHYsHandle_L0,FileChunks_IHDRNum)
    VoC_lw16i(REG7,pHYsERR);
    VoC_jump(Coolsand_pHYsHandle_End);
Coolsand_pHYsHandle_L0:

    VoC_bez16_d(Coolsand_pHYsHandle_L1,FileChunks_IDATNum)
    VoC_lw16i(REG7,pHYsERR);
    VoC_jump(Coolsand_pHYsHandle_End);
Coolsand_pHYsHandle_L1:

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_add16_rd(REG1,REG6,FileChunks_pHYsNum);
    VoC_bnlt16_rd(Coolsand_pHYsHandle_L2,REG6,FileChunks_pHYsNum)
    VoC_lw16i(REG7,pHYsERR);
    VoC_jump(Coolsand_pHYsHandle_End);
Coolsand_pHYsHandle_L2:

    VoC_lw16i_short(REG6,9,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_be32_rr(Coolsand_pHYsHandle_L3,REG67,REG45)
    VoC_lw16i(REG7,pHYsERR);
    VoC_jump(Coolsand_pHYsHandle_End);
Coolsand_pHYsHandle_L3:

    VoC_sw16_d(REG1,FileChunks_pHYsNum);


    /*  pHYsInfo.xPixPerUint = (*(*PngFileContent) << 24) + (*((*PngFileContent) + 1) << 16)
            + (*((*PngFileContent) + 2) << 8) + (*((*PngFileContent) + 3));
        pHYsInfo.yPixPerUnit = (*((*PngFileContent) + 4) << 24) + (*((*PngFileContent) + 5) << 16)
            + (*((*PngFileContent) + 6) << 8) + (*((*PngFileContent) + 7));
        pHYsInfo.UnitSpec = *((*PngFileContent) + 8);*/

    VoC_sub32_dr(RL7,GLOBAL_byteleft,REG45);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_sw32_d(REG45,pHYsInfo_xPixPerUint);
    VoC_sw32_d(REG67,pHYsInfo_yPixPerUnit);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_sw16_d(REG4,pHYsInfo_UnitSpec);

//  *PngFileContent += ChunkLength + 4;
//  PngStreamBuf.byteleft -=(ChunkLength +4);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);

Coolsand_pHYsHandle_End:

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_sPLTHandle
 *
 * Description: processing sPLT chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_sPLTHandle(void)
{

    VoC_push16(RA,DEFAULT);


    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_IDATHandle
 *
 * Description: processing IDAT chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_IDATHandle(void)
{

    VoC_push16(RA,DEFAULT);

//      FileChunks.IDATNum++;

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG2,16,IN_PARALLEL);

    VoC_add16_rd(REG1,REG1,FileChunks_IDATNum);

//  IDATSize = ChunkLength;
    VoC_sw32_d(REG45,IDATSize);

    VoC_sw16_d(REG1,FileChunks_IDATNum);

    /*  if (IHDRInfo.BitDepth == 16)
        {
            transform |= DEPTH16to8;
        }*/

    VoC_lw16_d(REG1,transform);

    VoC_bne16_rd(Coolsand_IDATHandle_L0,REG2,IHDRInfo_BitDepth)

    VoC_or16_ri(REG1,DEPTH16to8);

Coolsand_IDATHandle_L0:

    /*  if (IHDRInfo.BitDepth < 8)
        {
            transform |= (EXPAND | tRNSEXPAND);
        }*/

    VoC_lw16i_short(REG2,8,DEFAULT);

    VoC_bngt16_rd(Coolsand_IDATHandle_L1,REG2,IHDRInfo_BitDepth)

    VoC_or16_ri(REG1,EXPAND);
    VoC_or16_ri(REG1,tRNSEXPAND);

Coolsand_IDATHandle_L1:

    /*  if (IHDRInfo.ColorType == IndexColor)
        {
            transform |= (EXPAND | tRNSEXPAND);
        }*/

    VoC_lw16i_short(REG2,IndexColor,DEFAULT);

    VoC_bne16_rd(Coolsand_IDATHandle_L2,REG2,IHDRInfo_ColorType)

    VoC_or16_ri(REG1,EXPAND);
    VoC_or16_ri(REG1,tRNSEXPAND);

Coolsand_IDATHandle_L2:

    /*  if (FileChunks.tRNSNum != 0)
        {
            transform |= (EXPAND | tRNSEXPAND);
        }*/

    VoC_bez16_d(Coolsand_IDATHandle_L3,FileChunks_tRNSNum)

    VoC_or16_ri(REG1,EXPAND);
    VoC_or16_ri(REG1,tRNSEXPAND);

Coolsand_IDATHandle_L3:

    /*  if (IHDRInfo.ColorType == GreyScale || IHDRInfo.ColorType == GreyScaleAlpha)
        {
            transform |= GREY2RGB;
        }*/

    VoC_lw16i_short(REG2,GreyScale,DEFAULT);
    VoC_lw16i_short(REG3,GreyScaleAlpha,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_IDATHandle_L4,REG2,IHDRInfo_ColorType)
    VoC_or16_ri(REG1,GREY2RGB);
Coolsand_IDATHandle_L4:

    VoC_bne16_rd(Coolsand_IDATHandle_L5,REG3,IHDRInfo_ColorType)
    VoC_or16_ri(REG1,GREY2RGB);
Coolsand_IDATHandle_L5:

    //transform |= BKGDImage;       //set it as the default transform method
    VoC_or16_ri(REG1,BKGDImage);

    /*  if (FileChunks.bKGDNuM != 0)
        {
            transform |= bKGDEXPAND;
        }
        else// if(!(transform & BKGDImage))     //else
        {
        //if there's no defined background colour in bKGDHandle, these code will be executed---zhangl
        //set the default back ground as our image viewer brackground color  elisa 2009-9-29
            DefaultBkgd.Red =   216;
            DefaultBkgd.Green = 240;
            DefaultBkgd.Blue = 248;

            transform |= DEFAULTbKGD;
        }*/

    VoC_bez16_d(Coolsand_IDATHandle_L6,FileChunks_bKGDNum)

    VoC_or16_ri(REG1,bKGDEXPAND);

    VoC_jump(Coolsand_IDATHandle_L7);
Coolsand_IDATHandle_L6:

    VoC_or16_ri(REG1,DEFAULTbKGD);

    VoC_lw16i(REG2,216);
    VoC_lw16i(REG3,240);
    VoC_lw16i(REG4,248);

    VoC_sw16_d(REG2,DefaultBkgd_Red);
    VoC_sw16_d(REG3,DefaultBkgd_Green);
    VoC_sw16_d(REG4,DefaultBkgd_Blue);

Coolsand_IDATHandle_L7:

    VoC_sw16_d(REG1,transform);

    //UpdateInfo();

    VoC_jal(Coolsand_UpdateInfo);


//  ErrCode = inflateInit(&PNGzstream);

    VoC_jal(Coolsand_inflateInit);

    VoC_sw16_d(REG7,IDATHandle_ErrCode);

    /*  if (IHDRInfo.Interlace)
        {
            transform |= INTERLACE;
            ImagePass = 7;

            imgOutPtr= rgb_temp_ptr;
            imgOutStride= IHDRInfo.Width*3;//RGB565alpha
            start=0;
            end=IHDRInfo.Height-1;
        }
        else
        {
            ImagePass = 1;

            if(png_decode_mode==0)
            {
                if(png_resized_width== IHDRInfo.Width && png_resized_height== IHDRInfo.Height)
                {
                    imgOutPtr= bkgrd_ptr;
                    imgOutStride= png_bkgrd_width<<1;//rgb565
                    start= PngCutOffsetY1;
                    end= PngCutOffsetY2;
                }
                else//resize
                {
                    imgOutPtr= rgb_temp_ptr;
                    imgOutStride= IHDRInfo.Width*3;//RGB565alpha
                    start= 0;
                    end= IHDRInfo.Height-1;
                }
            }
            else if(png_decode_mode==1)
            {
                imgOutPtr= rgb_output_ptr;
                imgOutStride= (PngCutOffsetX2-PngCutOffsetX1+1)<<1;//rgb565
                start= PngCutOffsetY1;
                end= PngCutOffsetY2;
            }
            else if(png_decode_mode==2)
            {
                imgOutPtr= rgb_output_ptr;
                imgOutStride= (PngCutOffsetX2-PngCutOffsetX1+1)<<2;//alphaRGB
                start= PngCutOffsetY1;
                end= PngCutOffsetY2;
            }
        }*/





    VoC_bez16_d(Coolsand_IDATHandle_L8,IHDRInfo_Interlace)

    VoC_lw32_d(ACC0,GLOBAL_rgb_temp_ptr);


    VoC_lw16_d(REG1,transform);
    VoC_or16_ri(REG1,INTERLACE);

    VoC_lw16_d(REG3,IHDRInfo_Width);

    VoC_lw16i_short(REG2,7,DEFAULT);
    VoC_shr16_ri(REG3,-1,IN_PARALLEL);

    VoC_add16_rd(REG3,REG3,IHDRInfo_Width);

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_sub16_dr(REG5,IHDRInfo_Width,REG5);

    VoC_sw16_d(REG1,transform);
    VoC_sw16_d(REG2,ImagePass);
    VoC_sw16_d(REG3,IDATHandle_imgOutStride);
    VoC_sw16_d(REG4,IDATHandle_start);
    VoC_sw16_d(REG5,IDATHandle_end);

    VoC_sw32_d(ACC0,imgOutPtr);


    VoC_jump(Coolsand_IDATHandle_L13);
Coolsand_IDATHandle_L8:

    VoC_lw16i_short(REG1,1,DEFAULT);

    //IDATHandle_start IDATHandle_imgOutStride IDATHandle_end
    VoC_lw32z(REG45,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_lw16_d(REG2,png_resized_width);

    VoC_sw16_d(REG1,ImagePass);


    VoC_bnez16_d(Coolsand_IDATHandle_L10,png_decode_mode)

    VoC_bne16_rd(Coolsand_IDATHandle_L9,REG2,IHDRInfo_Width)
    VoC_lw16_d(REG2,png_resized_height);
    VoC_bne16_rd(Coolsand_IDATHandle_L9,REG2,IHDRInfo_Height)

    VoC_lw32_d(ACC0,bkgrd_ptr);

    VoC_lw16_d(REG3,png_bkgrd_width);
    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_lw16_d(REG4,PngCutOffsetY1);
    VoC_lw16_d(REG5,PngCutOffsetY2);

    VoC_sw32_d(ACC0,imgOutPtr);


    VoC_jump(Coolsand_IDATHandle_L12);
Coolsand_IDATHandle_L9:
    VoC_lw32_d(ACC0,GLOBAL_rgb_temp_ptr);

    VoC_lw16i_short(REG5,1,DEFAULT);

    VoC_lw16_d(REG3,IHDRInfo_Width);

    VoC_shr16_ri(REG3,-1,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_add16_rd(REG3,REG3,IHDRInfo_Width);

    VoC_sub16_dr(REG5,IHDRInfo_Height,REG5);

    VoC_sw32_d(ACC0,imgOutPtr);
    VoC_jump(Coolsand_IDATHandle_L12);
Coolsand_IDATHandle_L10:

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_bne16_rd(Coolsand_IDATHandle_L11,REG1,png_decode_mode)

    VoC_lw32_d(ACC0,rgb_output_ptr);

    VoC_lw16_d(REG4,PngCutOffsetY1);
    VoC_lw16_d(REG5,PngCutOffsetY2);

    VoC_sub16_rr(REG3,REG5,REG4,DEFAULT);

    VoC_add16_rr(REG3,REG3,REG3,DEFAULT);

    VoC_shr16_ri(REG3,-1,DEFAULT);

    VoC_sw32_d(ACC0,imgOutPtr);
    VoC_jump(Coolsand_IDATHandle_L12);
Coolsand_IDATHandle_L11:

    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_bne16_rd(Coolsand_IDATHandle_L12,REG1,png_decode_mode)

    VoC_lw32_d(ACC0,rgb_output_ptr);


    VoC_lw16_d(REG4,PngCutOffsetY1);
    VoC_lw16_d(REG5,PngCutOffsetY2);

    VoC_sub16_rr(REG3,REG5,REG4,DEFAULT);

    VoC_add16_rr(REG3,REG3,REG3,DEFAULT);

    VoC_shr16_ri(REG3,-2,DEFAULT);
    VoC_sw32_d(ACC0,imgOutPtr);

Coolsand_IDATHandle_L12:

    VoC_sw16_d(REG4,IDATHandle_start);
    VoC_sw16_d(REG3,IDATHandle_imgOutStride);
    VoC_sw16_d(REG5,IDATHandle_end);


Coolsand_IDATHandle_L13:

    /*
        for (i = 0; i < ImagePass; i++)
        {
            if (IHDRInfo.Interlace)
            {
                imgOutPtr= rgb_temp_ptr;
            }

            for (j = 0; j < IHDRInfo.Height; j++)
            {
                if(j<start)
                    skipRow=1;
                else
                    skipRow=0;
                if(j>end)
                {
                    ErrCode=CLIPOVER;//not an err
                    break;
                }
                ErrCode = ProcessOneRow(PngFileContent, imgOutPtr,skipRow);
                if(!skipRow)
                    imgOutPtr+=imgOutStride;
            }
        }
    */


    VoC_lw16i_short(REG2,0,DEFAULT);

Coolsand_IDATHandle_L14:
    VoC_lw16_d(REG1,ImagePass);
    VoC_bngt16_rr(Coolsand_IDATHandle_L20,REG1,REG2)
    VoC_lw16i_short(REG1,1,DEFAULT);

    VoC_add16_rr(REG2,REG2,REG1,DEFAULT);

    VoC_bez16_d(Coolsand_IDATHandle_L15,IHDRInfo_Interlace)
    VoC_lw32_d(ACC0,GLOBAL_rgb_temp_ptr);
    VoC_NOP();
    VoC_sw32_d(ACC0,imgOutPtr);
Coolsand_IDATHandle_L15:


    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_push16(REG2,IN_PARALLEL);
Coolsand_IDATHandle_L16:
    VoC_lw16_d(REG4,IHDRInfo_Height);
    VoC_bngt16_rr(Coolsand_IDATHandle_L19,REG4,REG3)

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_movreg16(REG4,REG3,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bnlt16_rd(Coolsand_IDATHandle_L17,REG4,IDATHandle_start)
    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_NOP();
Coolsand_IDATHandle_L17:

    VoC_sw16_d(REG5,IDATHandle_skipRow);

    VoC_bngt16_rd(Coolsand_IDATHandle_L18,REG4,IDATHandle_end)
    VoC_lw16i(REG7,CLIPOVER);
    VoC_NOP();
    VoC_sw16_d(REG7,IDATHandle_ErrCode);
    VoC_jump(Coolsand_IDATHandle_L19);
Coolsand_IDATHandle_L18:


    VoC_lw16i(REG5,GLOBAL_RawOutBuf);
    VoC_push16(REG3,DEFAULT);
    VoC_sw16_d(REG5,GLOBAL_RawOutBufAddr);

    VoC_jal(Coolsand_ProcessOneRow);

    VoC_sw16_d(REG7,IDATHandle_ErrCode);

    VoC_bnez16_d(Coolsand_IDATHandle_L21,IDATHandle_skipRow)


    /*          VoC_sw32_d(REG67,updatewindow_copy);
                VoC_sub16_dr(REG7,GLOBAL_RawOutBufAddr,REG6);
                VoC_lw32_d(ACC1,GLOBAL_WindowAddr);

                VoC_lw16_d()


                VoC_sw16_d(REG6,zmemcpy_Length);
                VoC_sw16_d(REG7,zmemcpy_SrcAddr);
                VoC_sw32_d(ACC1,zmemcpy_DstAddr);

                VoC_jal(Coolsand_zmemcpyFromVoC);   */

    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16_d(ACC0_LO,IDATHandle_imgOutStride);
    VoC_add32_rd(ACC0,ACC0,imgOutPtr);
    VoC_NOP();
    VoC_sw32_d(ACC0,imgOutPtr);

Coolsand_IDATHandle_L21:
    VoC_pop16(REG3,DEFAULT);
    VoC_jump(Coolsand_IDATHandle_L16);
Coolsand_IDATHandle_L19:

    VoC_pop16(REG2,DEFAULT);

    VoC_jump(Coolsand_IDATHandle_L14);

Coolsand_IDATHandle_L20:

    VoC_pop16(RA,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);

    VoC_lw16_d(REG7,IDATHandle_ErrCode);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_tIMEHandle
 *
 * Description: processing tIME chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_tIMEHandle(void)
{
    /*

        if (FileChunks.tIMENum > 1)
        {
            //printf("Duplicate tIME chunk!");
            return tIMEERR;
        }
        else if (ChunkLength != 7)
        {
            //printf("Invalid tIME chunk length!");
            return tIMEERR;
        }

        FileChunks.tIMENum++;
    */
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG3,1,DEFAULT);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);

    VoC_bnlt16_rd(Coolsand_tIMEHandle_L0,REG3,FileChunks_tIMENum)
    VoC_lw16i(REG7,tIMEERR);
    VoC_jump(Coolsand_tIMEHandle_End);
Coolsand_tIMEHandle_L0:

    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add16_rd(REG3,REG3,FileChunks_tIMENum);

    VoC_be32_rr(Coolsand_tIMEHandle_L1,REG45,REG67)
    VoC_lw16i(REG7,tIMEERR);
    VoC_jump(Coolsand_tIMEHandle_End);
Coolsand_tIMEHandle_L1:

//  FileChunks.tIMENum++;
    VoC_sw16_d(REG3,FileChunks_tIMENum);


    VoC_bngtz32_r(Coolsand_tIMEHandle_L4,REG45)

    VoC_lw32z(RL7,DEFAULT);

    VoC_lw16i(RL7_LO,256);

    VoC_bgt32_rr(Coolsand_tIMEHandle_L3,RL7,REG45)

Coolsand_tIMEHandle_L2:

    VoC_jal(Coolsand_DmaInData);

    VoC_loop_i(0,0)

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);

    VoC_endloop0

    VoC_bngt32_rr(Coolsand_tIMEHandle_L2,RL7,REG45)

Coolsand_tIMEHandle_L3:
    VoC_bngtz32_r(Coolsand_tIMEHandle_L4,REG45)

    VoC_jal(Coolsand_DmaInData);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);
    VoC_endloop0

Coolsand_tIMEHandle_L4:

    //*PngFileContent += 4;
    //PngStreamBuf.byteleft -= 4;

    VoC_sub32_dr(RL7,GLOBAL_byteleft,ACC1);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);
Coolsand_tIMEHandle_End:
    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_iTXtHandle
 *
 * Description: processing iTXt chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_iTXtHandle(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,REG23,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG45,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    //FileChunks.tEXtNum++;
    VoC_add16_rd(REG1,REG2,FileChunks_iTXtNum)

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_sw16_d(REG1,FileChunks_iTXtNum);

    VoC_bngtz32_r(Coolsand_iTXtHandle_End,REG45)

    VoC_lw32z(RL7,DEFAULT);

    VoC_lw16i(RL7_LO,256);

    VoC_bgt32_rr(Coolsand_iTXtHandle_L1,RL7,REG45)

Coolsand_iTXtHandle_L0:


    VoC_jal(Coolsand_DmaInData);

    VoC_loop_i(0,0)

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);

    VoC_endloop0

    VoC_bngt32_rr(Coolsand_iTXtHandle_L0,RL7,REG45)

Coolsand_iTXtHandle_L1:
    VoC_bngtz32_r(Coolsand_iTXtHandle_End,REG45)

    VoC_jal(Coolsand_DmaInData);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);
    VoC_endloop0

Coolsand_iTXtHandle_End:
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_tEXtHandle
 *
 * Description: processing tEXt chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_tEXtHandle(void)
{
    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,REG23,DEFAULT);

    VoC_sub32_rr(RL7,RL7,REG45,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    //FileChunks.tEXtNum++;
    VoC_add16_rd(REG1,REG2,FileChunks_tEXtNum)

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_sw16_d(REG1,FileChunks_tEXtNum);

    VoC_bngtz32_r(Coolsand_tEXtHandle_End,REG45)

    VoC_lw32z(RL7,DEFAULT);

    VoC_lw16i(RL7_LO,256);

    VoC_bgt32_rr(Coolsand_tEXtHandle_L1,RL7,REG45)

Coolsand_tEXtHandle_L0:


    VoC_jal(Coolsand_DmaInData);

    VoC_loop_i(0,0)

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);

    VoC_endloop0

    VoC_bngt32_rr(Coolsand_tEXtHandle_L0,RL7,REG45)

Coolsand_tEXtHandle_L1:
    VoC_bngtz32_r(Coolsand_tEXtHandle_End,REG45)

    VoC_jal(Coolsand_DmaInData);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);
    VoC_endloop0

Coolsand_tEXtHandle_End:
    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_zTXtHandle
 *
 * Description: processing zTXt chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_zTXtHandle(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,REG23,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG45,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    //FileChunks.tEXtNum++;
    VoC_add16_rd(REG1,REG2,FileChunks_zTXtNum)

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_sw16_d(REG1,FileChunks_zTXtNum);

    VoC_bngtz32_r(Coolsand_zTXtHandle_End,REG45)

    VoC_lw32z(RL7,DEFAULT);

    VoC_lw16i(RL7_LO,256);

    VoC_bgt32_rr(Coolsand_zTXtHandle_L1,RL7,REG45)

Coolsand_zTXtHandle_L0:


    VoC_jal(Coolsand_DmaInData);

    VoC_loop_i(0,0)

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);

    VoC_endloop0

    VoC_bngt32_rr(Coolsand_zTXtHandle_L0,RL7,REG45)

Coolsand_zTXtHandle_L1:
    VoC_bngtz32_r(Coolsand_zTXtHandle_End,REG45)

    VoC_jal(Coolsand_DmaInData);

    VoC_loop_r_short(REG4,DEFAULT)
    VoC_startloop0

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);
    VoC_endloop0

Coolsand_zTXtHandle_End:
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_IENDHandle
 *
 * Description: processing IEND chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_IENDHandle(void)
{

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_bnlt16_rd(Coolsand_IENDHandle_L0,REG1,FileChunks_IENDNum)
    VoC_lw16i_short(REG7,IENDERR,DEFAULT);

Coolsand_IENDHandle_L0:

    VoC_lw16i_short(REG2,4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_bne32_rr(Coolsand_IENDHandle_L1,REG45,REG23)
    VoC_lw16i_short(REG7,IENDERR,DEFAULT);
Coolsand_IENDHandle_L1:

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_UnknownHandle
 *
 * Description: processing Unknown chunk
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/20/2012
 **************************************************************************************/
void Coolsand_UnknownHandle(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_movreg32(ACC1,REG45,IN_PARALLEL);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

Coolsand_UnknownHandle_L0:
    VoC_bngtz32_r(Coolsand_UnknownHandle_L1,REG45)

    VoC_jal(Coolsand_DmaInData);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_sub32_rr(REG45,REG45,REG23,IN_PARALLEL);

    VoC_jump(Coolsand_UnknownHandle_L0);
Coolsand_UnknownHandle_L1:

    //*PngFileContent += 4;
    //PngStreamBuf.byteleft -= 4;

    VoC_sub32_dr(RL7,GLOBAL_byteleft,ACC1);//ChunkLength

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub32_rr(RL7,RL7,REG23,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT)//for return

    VoC_sw32_d(RL7,GLOBAL_byteleft);


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}


/**************************************************************************************
 * Function:    Coolsand_UpdateInfo
 *
 * Description: UpdateInfo
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/21/2012
 **************************************************************************************/
void Coolsand_UpdateInfo(void)
{

    VoC_push16(RA,DEFAULT);

    VoC_jal(Coolsand_PngRowInit);
    VoC_jal(Coolsand_UpdateTransformInfo);

    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_PngRowInit
 *
 * Description: PngRowInit
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/21/2012
 **************************************************************************************/
void Coolsand_PngRowInit(void)
{
    //  const int Pass_Xstart[7] = {0, 4, 0, 2, 0, 1, 0};
    //  const int Pass_Xinc[7] = {8, 8, 4, 4, 2, 2, 1};




    //  const int Pass_Ystart[7] = {0, 0, 4, 0, 2, 0, 1};
    //  const int Pass_Yinc[7] = {8, 8, 8, 4, 4, 2, 2};

    VoC_lw32z(ACC0,DEFAULT);
    VoC_push16(RA,DEFAULT);
    VoC_sw32_d(ACC0,PNGzstream_avail_in);

    VoC_jal(Coolsand_InitTransform);

    /*
        if (IHDRInfo.Interlace)
        {
            CurPassRowNum = (IHDRInfo.Height + Pass_Yinc[0] - 1 - Pass_Ystart[0]) >> 3;

            CurInterlaceRowWidth = (IHDRInfo.Width + Pass_Xinc[CurPass]
                                    - 1 - Pass_Xstart[CurPass]) / Pass_Xinc[CurPass];

    //      CurPassRowBytes = ROWBYTES(IHDRInfo.BitDepth, CurInterlaceRowWidth) + 1;
                  CurPassRowBytes = ROWBYTES(PixelDepth, CurInterlaceRowWidth) + 1;
    ////CSW_TRACE(MMI_TS_ID,"PixelDepth = 0x%x     CurPassRowBytes = 0x%x\n", PixelDepth, CurPassRowBytes);
        }
        else
        {
            CurPassRowNum = IHDRInfo.Height;
            CurInterlaceRowWidth = IHDRInfo.Width;
            CurPassRowBytes = RowBytes + 1;
        }
    */


    VoC_bez16_d(Coolsand_PngRowInit_L2,IHDRInfo_Interlace);

    VoC_lw16i_short(REG1,7,DEFAULT);//Pass_Yinc[0] - 1 - Pass_Ystart[0]

    VoC_add16_rd(REG1,REG1,IHDRInfo_Height);

    VoC_shr16_ri(REG1,3,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_lw16i(REG3,Pass_Xinc);

    VoC_sw16_d(REG1,CurPassRowNum);

    VoC_lw16i(REG4,Pass_Xstart);

    VoC_lw16_d(REG1,CurPass);

    VoC_add16_rr(REG5,REG3,REG1,DEFAULT);
    VoC_add16_rr(REG6,REG4,REG1,IN_PARALLEL);

    VoC_sub16_dr(REG2,IHDRInfo_Width,REG2);

    VoC_add16_rp(REG2,REG2,REG5,DEFAULT);
    VoC_lw16_p(REG4,REG5,IN_PARALLEL);

    VoC_sub16_rp(REG2,REG2,REG6,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_be16_rr(Coolsand_PngRowInit_L0,REG5,REG3)

    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_be16_rr(Coolsand_PngRowInit_L0,REG5,REG3)


    VoC_lw16i_short(REG3,4,DEFAULT);
    VoC_lw16i_short(REG4,2,IN_PARALLEL);

    VoC_be16_rr(Coolsand_PngRowInit_L0,REG5,REG3)

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(REG4,3,IN_PARALLEL);

Coolsand_PngRowInit_L0:

    VoC_shr16_rr(REG2,REG4,DEFAULT);

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rd(ACC0,REG2,PixelDepth);

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_PngRowInit_L1,REG3,PixelDepth)

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

Coolsand_PngRowInit_L1:

    VoC_shr32_ri(ACC0,3,DEFAULT);

    VoC_sw16_d(REG2,CurInterlaceRowWidth);

    VoC_sw32_d(ACC0,CurPassRowBytes);


    VoC_jump(Coolsand_PngRowInit_L3);
Coolsand_PngRowInit_L2:

    //CurPassRowNum = IHDRInfo.Height;
    //CurInterlaceRowWidth = IHDRInfo.Width;
    //CurPassRowBytes = RowBytes + 1;

    VoC_lw16i_short(REG1,1,DEFAULT);

    VoC_lw16_d(REG2,IHDRInfo_Height);
    VoC_lw16_d(REG3,IHDRInfo_Width);
    VoC_add16_rd(REG4,REG1,RowBytes);

    VoC_sw16_d(REG2,CurPassRowNum);
    VoC_sw16_d(REG3,CurInterlaceRowWidth);
    VoC_sw16_d(REG4,CurPassRowBytes);


Coolsand_PngRowInit_L3:

//  MaxBitDepth = PixelDepth;

    VoC_lw16_d(REG1,PixelDepth);

    VoC_lw16_d(REG2,transform);

    VoC_sw16_d(REG1,MaxBitDepth);

    /*  if (transform & EXPAND)
        {
            if (IHDRInfo.ColorType == IndexColor)
            {
                if (TransNum)
                    MaxBitDepth = 32;
                else
                    MaxBitDepth = 24;
            }
            else if (IHDRInfo.ColorType == GreyScale)
            {
                if (MaxBitDepth < 8)
                    MaxBitDepth = 8;
                else
                    MaxBitDepth = MaxBitDepth << 1;
            }
            else if (IHDRInfo.ColorType == TrueColor)
            {
                if (TransNum)
                {
                    MaxBitDepth <<= 2;
                    MaxBitDepth /= 3;
                }

            }
        }
    */
    //REG1:MaxBitDepth

    VoC_and16_ri(REG2,EXPAND);

    VoC_bez16_r(Coolsand_PngRowInit_L8,REG2)

    VoC_lw16i_short(REG2,IndexColor,DEFAULT);
    VoC_lw16i_short(REG3,GreyScale,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_PngRowInit_L4,REG2,IHDRInfo_ColorType)

    VoC_lw16i_short(REG1,24,DEFAULT);

    VoC_bez16_d(Coolsand_PngRowInit_L8,TransNum)
    VoC_lw16i(REG1,32);
    VoC_jump(Coolsand_PngRowInit_L8)
Coolsand_PngRowInit_L4:


    VoC_bne16_rd(Coolsand_PngRowInit_L6,REG3,IHDRInfo_ColorType)

    VoC_lw16i_short(REG2,8,DEFAULT);

    VoC_bngt16_rr(Coolsand_PngRowInit_L5,REG2,REG1)

    VoC_lw16i_short(REG1,8,DEFAULT);
    VoC_jump(Coolsand_PngRowInit_L8);
Coolsand_PngRowInit_L5:
    VoC_shr16_ri(REG1,-1,DEFAULT);
    VoC_jump(Coolsand_PngRowInit_L8);
Coolsand_PngRowInit_L6:

    VoC_lw16i_short(REG2,TrueColor,DEFAULT);

    VoC_bne16_rd(Coolsand_PngRowInit_L8,REG2,IHDRInfo_ColorType)

    VoC_bez16_d(Coolsand_PngRowInit_L8,TransNum)

    VoC_shr16_ri(REG1,-2,DEFAULT);
    VoC_lw16i_short(REG2,3,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,0,1);

    VoC_loop_i(1,4)
    VoC_loop_i(0,0)

    VoC_sub16_rr(REG1,REG1,REG2,DEFAULT);

    VoC_bngtz16_r(Coolsand_PngRowInit_L7,REG1)

    VoC_inc_p(REG3,DEFAULT);
    VoC_endloop0
    VoC_NOP();
    VoC_endloop1

Coolsand_PngRowInit_L7:
    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_NOP();

Coolsand_PngRowInit_L8:

    VoC_sw16_d(REG1,MaxBitDepth);

    /*
        if (transform & GREY2RGB)
        {
            if ((TransNum && (transform & EXPAND)) || IHDRInfo.ColorType == GreyScaleAlpha)
            {
                if (MaxBitDepth <= 16)
                    MaxBitDepth = 32;
                else
                    MaxBitDepth = 64;
            }
            else
            {
                if (MaxBitDepth <= 8)
                {
                    if (IHDRInfo.ColorType == GreyScaleAlpha)
                        MaxBitDepth = 32;
                    else
                        MaxBitDepth = 24;
                }
                else if (IHDRInfo.ColorType == GreyScaleAlpha)
                    MaxBitDepth = 64;
                else
                    MaxBitDepth = 48;
            }
        }
    */
    VoC_lw16_d(REG2,transform);

    VoC_movreg16(REG3,REG2,DEFAULT);

    VoC_and16_ri(REG2,GREY2RGB);
    VoC_and16_ri(REG3,EXPAND);

    VoC_bez16_r(Coolsand_PngRowInit_L13,REG2)

    VoC_bez16_r(Coolsand_PngRowInit_L9,REG3)
    VoC_bez16_d(Coolsand_PngRowInit_L9,TransNum)
    VoC_jump(Coolsand_PngRowInit_L10);
Coolsand_PngRowInit_L9:

    VoC_lw16i_short(REG4,GreyScaleAlpha,DEFAULT);
    VoC_bne16_rd(Coolsand_PngRowInit_L11,REG4,IHDRInfo_ColorType)
Coolsand_PngRowInit_L10:
    VoC_lw16i_short(REG2,16,DEFAULT);
    VoC_lw16i_short(REG1,32,IN_PARALLEL);
    VoC_bngt16_rr(Coolsand_PngRowInit_L13,REG1,REG2)
    VoC_lw16i(REG1,64);
    VoC_jump(Coolsand_PngRowInit_L13);
Coolsand_PngRowInit_L11:

    VoC_lw16i_short(REG2,8,DEFAULT);

    VoC_bgt16_rr(Coolsand_PngRowInit_L12,REG1,REG2)
    VoC_lw16i_short(REG1,24,DEFAULT);
    VoC_lw16i_short(REG3,GreyScaleAlpha,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_PngRowInit_L13,REG3,IHDRInfo_ColorType)
    VoC_lw16i(REG1,48);
    VoC_jump(Coolsand_PngRowInit_L13);
Coolsand_PngRowInit_L12:

    VoC_lw16i_short(REG3,GreyScaleAlpha,DEFAULT);
    VoC_lw16i_short(REG1,48,IN_PARALLEL);
    VoC_bne16_rd(Coolsand_PngRowInit_L13,REG3,IHDRInfo_ColorType)
    VoC_lw16i(REG1,64);
    VoC_NOP();
Coolsand_PngRowInit_L13:

    VoC_pop16(RA,DEFAULT);

    VoC_sw16_d(REG1,MaxBitDepth);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_UpdateTransformInfo
 *
 * Description: UpdateTransformInfo
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/21/2012
 **************************************************************************************/
void Coolsand_UpdateTransformInfo(void)
{

    //NewColorType = IHDRInfo.ColorType;
    //NewBiteDepth = IHDRInfo.BitDepth;

    VoC_lw16_d(REG1,IHDRInfo_ColorType);
    VoC_lw16_d(REG6,IHDRInfo_BitDepth);

    VoC_sw16_d(REG1,NewColorType);
    VoC_sw16_d(REG6,NewBiteDepth);

    /*  if (transform & EXPAND)
        {
            if (IHDRInfo.ColorType == IndexColor)
            {
                if (TransNum && (transform & tRNSEXPAND))
                    NewColorType = TrueColorAlpha;
                else
                    NewColorType = TrueColor;

                NewBiteDepth = 8;
                NewTansNum = 0;
            }
            else
            {
                if (TransNum)
                {
                    if (transform & tRNSEXPAND)
                        NewColorType = IHDRInfo.ColorType | 4;
                    else
                        NewColorType = IHDRInfo.ColorType | 2;
                }

                if (IHDRInfo.BitDepth < 8)
                    NewBiteDepth = 8;
                NewTansNum = 0;
            }
        }
    */
    VoC_lw16_d(REG1,transform);

    VoC_and16_ri(REG1,EXPAND);

    VoC_lw16_d(REG6,NewBiteDepth);
    VoC_lw16_d(REG5,NewColorType);
    VoC_lw16_d(REG7,NewTansNum);

    VoC_bez16_r(Coolsand_UpdateTransformInfo_L5,REG1)

    VoC_lw16i_short(REG2,IndexColor,DEFAULT);

    VoC_bne16_rd(Coolsand_UpdateTransformInfo_L2,REG2,IHDRInfo_ColorType)

    VoC_lw16i_short(REG5,TrueColor,DEFAULT);
    VoC_bez16_d(Coolsand_UpdateTransformInfo_L1,TransNum);
    VoC_lw16_d(REG1,transform);
    VoC_and16_ri(REG1,EXPAND);
    VoC_bez16_r(Coolsand_UpdateTransformInfo_L1,REG1);
    VoC_lw16i_short(REG5,TrueColorAlpha,DEFAULT);
Coolsand_UpdateTransformInfo_L1:

    VoC_lw16i_short(REG6,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_jump(Coolsand_UpdateTransformInfo_L5);
Coolsand_UpdateTransformInfo_L2:

    VoC_bez16_d(Coolsand_UpdateTransformInfo_L4,TransNum);

    VoC_lw16_d(REG1,transform);
    VoC_and16_ri(REG1,tRNSEXPAND);


    VoC_lw16_d(REG5,IHDRInfo_ColorType)
    VoC_bez16_r(Coolsand_UpdateTransformInfo_L3,REG1);

    VoC_or16_ri(REG5,4);
    VoC_jump(Coolsand_UpdateTransformInfo_L4);

Coolsand_UpdateTransformInfo_L3:

    VoC_or16_ri(REG5,2);

Coolsand_UpdateTransformInfo_L4:

    VoC_lw16i_short(REG2,8,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_UpdateTransformInfo_L5,REG2,IHDRInfo_BitDepth)

    VoC_lw16i_short(REG6,8,DEFAULT);

Coolsand_UpdateTransformInfo_L5:

//  VoC_sw16_d(REG5,NewColorType);
//  VoC_sw16_d(REG6,NewBiteDepth);
//  VoC_sw16_d(REG7,NewTansNum);

//  if ((transform & DEPTH16to8) && (NewBiteDepth == 16))
//      NewBiteDepth = 8;

//  if (transform & GREY2RGB)
//      NewColorType |= 2;

//  if ((transform & DEFAULTbKGD) || (transform & bKGDEXPAND))
//  {
//      NewColorType &= ~4;
//      NewTansNum = 0;

//  }

    VoC_lw16_d(REG1,transform);

    VoC_lw16i_short(REG2,16,DEFAULT);
    VoC_movreg16(REG3,REG1,IN_PARALLEL);

    VoC_and16_ri(REG1,DEPTH16to8);


    VoC_bez16_r(Coolsand_UpdateTransformInfo_L6,REG1)
    VoC_bne16_rr(Coolsand_UpdateTransformInfo_L6,REG6,REG2)
    VoC_lw16i_short(REG6,8,DEFAULT);
Coolsand_UpdateTransformInfo_L6:

    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_movreg16(REG2,REG3,IN_PARALLEL);

    VoC_and16_ri(REG3,GREY2RGB);

    VoC_bez16_r(Coolsand_UpdateTransformInfo_L7,REG3)
    VoC_or16_ri(REG5,2);
Coolsand_UpdateTransformInfo_L7:

    VoC_and16_ri(REG1,DEFAULTbKGD);
    VoC_and16_ri(REG2,bKGDEXPAND);

    VoC_bez16_r(Coolsand_UpdateTransformInfo_L8,REG1)

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_and16_ri(REG5,0xFFFB);

Coolsand_UpdateTransformInfo_L8:

    VoC_bez16_r(Coolsand_UpdateTransformInfo_L9,REG2)

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_and16_ri(REG5,0xFFFB);

Coolsand_UpdateTransformInfo_L9:

    VoC_sw16_d(REG6,NewBiteDepth);
    VoC_sw16_d(REG5,NewColorType);
    VoC_sw16_d(REG7,NewTansNum);

    /*  if (NewColorType == IndexColor)
            NewChannel = 1;
        else if (NewColorType & 2)
            NewChannel = 3;
        else
            NewChannel = 1;*/


    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_movreg16(REG1,REG5,IN_PARALLEL)

    VoC_and16_ri(REG1,2);

    VoC_bez16_r(Coolsand_UpdateTransformInfo_L10,REG1)

    VoC_lw16i_short(REG4,3,DEFAULT);

Coolsand_UpdateTransformInfo_L10:

//  if (NewColorType & 4)
//      NewChannel++;

    VoC_and16_ri(REG5,4);

    VoC_bez16_r(Coolsand_UpdateTransformInfo_L11,REG5)

    VoC_lw16i_short(REG1,1,DEFAULT);

    VoC_add16_rr(REG4,REG4,REG1,DEFAULT)

Coolsand_UpdateTransformInfo_L11:

//  NewPixelDepth = (UINT8)(NewChannel * NewBiteDepth);
//  NewRowBytes = ROWBYTES(NewPixelDepth, IHDRInfo.Width);

//  #define ROWBYTES(pixel_bits, width) \
//  ((pixel_bits) >= 8 ? \
//  ((width) * (((UINT32)(pixel_bits)) >> 3)) : \
//  (( ((width) * ((UINT32)(pixel_bits))) + 7) >> 3) )

    VoC_mult16_rr(REG2,REG4,REG6,DEFAULT);

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rd(ACC0,REG2,IHDRInfo_Width);

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_UpdateTransformInfo_L12,REG3,IHDRInfo_BitDepth)

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

Coolsand_UpdateTransformInfo_L12:

    VoC_shr32_ri(ACC0,3,DEFAULT);

    VoC_NOP();

    VoC_sw32_d(ACC0,NewRowBytes);


    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_InitTransform
 *
 * Description: InitTransform
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/21/2012
 **************************************************************************************/
void Coolsand_InitTransform(void)
{

    /*  if ((transform & bKGDEXPAND) && (transform & GREY2RGB) &&
            bKGDInfo.bKGDRed == bKGDInfo.bKGDGreen && bKGDInfo.bKGDRed == bKGDInfo.bKGDBlue)
        {
            bKGDInfo.bKGDGrey = bKGDInfo.bKGDRed;
        }
    */
    VoC_lw16_d(REG1,transform);

    VoC_movreg16(REG2,REG1,DEFAULT);

    VoC_and16_ri(REG1,bKGDEXPAND);

    VoC_and16_ri(REG2,GREY2RGB);

    VoC_lw16_d(REG3,bKGDInfo_bKGDRed);

    VoC_bez16_r(Coolsand_InitTransform_L0,REG1)
    VoC_bez16_r(Coolsand_InitTransform_L0,REG2)
    VoC_bne16_rd(Coolsand_InitTransform_L0,REG3,bKGDInfo_bKGDGreen)
    VoC_bne16_rd(Coolsand_InitTransform_L0,REG3,bKGDInfo_bKGDBlue)
    VoC_sw16_d(REG3,bKGDInfo_bKGDGrey);
Coolsand_InitTransform_L0:

//  if ((transform & EXPAND) && (transform & bKGDEXPAND))

    VoC_lw16_d(REG1,transform);

    VoC_movreg16(REG2,REG1,DEFAULT);

    VoC_and16_ri(REG1,EXPAND);

    VoC_and16_ri(REG2,bKGDEXPAND);

    VoC_bez16_r(Coolsand_InitTransform_End,REG1)
    VoC_bez16_r(Coolsand_InitTransform_End,REG2)

//      if (!(IHDRInfo.ColorType & 0x02))  // 灰度图或具有alpha通道的灰度图

    VoC_lw16_d(REG1,IHDRInfo_ColorType);

    VoC_and16_ri(REG1,0x02);

    VoC_bnez16_r(Coolsand_InitTransform_L1,REG1)

    //  switch(IHDRInfo.BitDepth)

    /*  case 1:
        bKGDInfo.bKGDGrey *= (UINT16)0xFF;
        bKGDInfo.bKGDRed = bKGDInfo.bKGDGreen = bKGDInfo.bKGDBlue = bKGDInfo.bKGDGrey;

        if (!(transform & tRNSEXPAND))
        {
            tRNSInfo.tRNSGrey *= (UINT16)0xFF;
            tRNSInfo.tRNSRed = tRNSInfo.tRNSGreen = tRNSInfo.tRNSBlue = tRNSInfo.tRNSGrey;
        }
        break;*/
    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16_d(REG2,IHDRInfo_BitDepth);

    VoC_bne16_rr(Coolsand_InitTransform_Case_1,REG1,REG2)

    VoC_lw16i(REG1,0xFF);

    VoC_mult16_rd(REG2,REG1,bKGDInfo_bKGDGrey);

    VoC_lw16_d(REG3,transform);

    VoC_and16_ri(REG3,tRNSEXPAND);

    VoC_sw16_d(REG2,bKGDInfo_bKGDGrey);
    VoC_sw16_d(REG2,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG2,bKGDInfo_bKGDGreen);

    VoC_mult16_rd(REG3,REG1,tRNSInfo_tRNSGrey);

    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);

    VoC_bnez16_r(Coolsand_InitTransform_End,REG3)

    VoC_sw16_d(REG3,tRNSInfo_tRNSGrey);
    VoC_sw16_d(REG3,tRNSInfo_tRNSBlue);
    VoC_sw16_d(REG3,tRNSInfo_tRNSGreen);
    VoC_sw16_d(REG3,tRNSInfo_tRNSRed);

    VoC_jump(Coolsand_InitTransform_End);
Coolsand_InitTransform_Case_1:



    /*case 2:
        bKGDInfo.bKGDGrey *= (UINT16)0x55;
        bKGDInfo.bKGDRed = bKGDInfo.bKGDGreen = bKGDInfo.bKGDBlue = bKGDInfo.bKGDGrey;

        if (!(transform & tRNSEXPAND))
        {
            tRNSInfo.tRNSGrey *= (UINT16)0x55;
            tRNSInfo.tRNSRed = tRNSInfo.tRNSGreen = tRNSInfo.tRNSBlue = tRNSInfo.tRNSGrey;
        }
        break;*/

    VoC_lw16i_short(REG1,2,DEFAULT);
    VoC_lw16_d(REG2,IHDRInfo_BitDepth);

    VoC_bne16_rr(Coolsand_InitTransform_Case_2,REG1,REG2)

    VoC_lw16i(REG1,0x55);

    VoC_mult16_rd(REG2,REG1,bKGDInfo_bKGDGrey);

    VoC_lw16_d(REG3,transform);

    VoC_and16_ri(REG3,tRNSEXPAND);

    VoC_sw16_d(REG2,bKGDInfo_bKGDGrey);
    VoC_sw16_d(REG2,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG2,bKGDInfo_bKGDGreen);

    VoC_mult16_rd(REG3,REG1,tRNSInfo_tRNSGrey);

    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);

    VoC_bnez16_r(Coolsand_InitTransform_End,REG3)

    VoC_sw16_d(REG3,tRNSInfo_tRNSGrey);
    VoC_sw16_d(REG3,tRNSInfo_tRNSBlue);
    VoC_sw16_d(REG3,tRNSInfo_tRNSGreen);
    VoC_sw16_d(REG3,tRNSInfo_tRNSRed);

    VoC_jump(Coolsand_InitTransform_End);

Coolsand_InitTransform_Case_2:

    /*case 4:
        bKGDInfo.bKGDGrey *= (UINT16)0x11;
        bKGDInfo.bKGDRed = bKGDInfo.bKGDGreen = bKGDInfo.bKGDBlue = bKGDInfo.bKGDGrey;

        if (!(transform & tRNSEXPAND))
        {
            tRNSInfo.tRNSGrey *= (UINT16)0x11;
            tRNSInfo.tRNSRed = tRNSInfo.tRNSGreen = tRNSInfo.tRNSBlue = tRNSInfo.tRNSGrey;
        }
        break;*/

    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_lw16_d(REG2,IHDRInfo_BitDepth);

    VoC_bne16_rr(Coolsand_InitTransform_Case_4,REG1,REG2)

    VoC_lw16i(REG1,0x11);

    VoC_mult16_rd(REG2,REG1,bKGDInfo_bKGDGrey);

    VoC_lw16_d(REG3,transform);

    VoC_and16_ri(REG3,tRNSEXPAND);

    VoC_sw16_d(REG2,bKGDInfo_bKGDGrey);
    VoC_sw16_d(REG2,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG2,bKGDInfo_bKGDGreen);

    VoC_mult16_rd(REG3,REG1,tRNSInfo_tRNSGrey);

    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);

    VoC_bnez16_r(Coolsand_InitTransform_End,REG3)

    VoC_sw16_d(REG3,tRNSInfo_tRNSGrey);
    VoC_sw16_d(REG3,tRNSInfo_tRNSBlue);
    VoC_sw16_d(REG3,tRNSInfo_tRNSGreen);
    VoC_sw16_d(REG3,tRNSInfo_tRNSRed);

    VoC_jump(Coolsand_InitTransform_End);



Coolsand_InitTransform_Case_4:


    /*case 8:
    case 16:
      bKGDInfo.bKGDRed = bKGDInfo.bKGDGreen = bKGDInfo.bKGDBlue = bKGDInfo.bKGDGrey;
      break;*/

    VoC_lw16i_short(REG1,8,DEFAULT);

    VoC_lw16_d(REG2,bKGDInfo_bKGDGrey);

    VoC_bne16_rd(Coolsand_InitTransform_Case_8,REG1,IHDRInfo_BitDepth)

    VoC_sw16_d(REG2,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG2,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);

    VoC_jump(Coolsand_InitTransform_End);
Coolsand_InitTransform_Case_8:

    VoC_lw16i_short(REG1,16,DEFAULT);

    VoC_bne16_rd(Coolsand_InitTransform_Case_16,REG1,IHDRInfo_BitDepth)

    VoC_sw16_d(REG2,bKGDInfo_bKGDBlue);
    VoC_sw16_d(REG2,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);

    VoC_jump(Coolsand_InitTransform_End);
Coolsand_InitTransform_Case_16:

    VoC_jump(Coolsand_InitTransform_End);



Coolsand_InitTransform_L1:

    /*  else if (IHDRInfo.ColorType == IndexColor)    // 索引图
        {
            bKGDInfo.bKGDRed = Palete[bKGDInfo.bKGDPlteIndex].Red;
            bKGDInfo.bKGDGreen = Palete[bKGDInfo.bKGDPlteIndex].Green;
            bKGDInfo.bKGDBlue = Palete[bKGDInfo.bKGDPlteIndex].Blue;
        }*/


    VoC_lw16i(REG1,IndexColor);

    VoC_bne16_rd(Coolsand_InitTransform_End,REG1,IHDRInfo_ColorType)

    VoC_lw16i_set_inc(REG1,Palete,1);

    VoC_add16_rd(REG1,REG1,bKGDInfo_bKGDPlteIndex);
    VoC_add16_rd(REG1,REG1,bKGDInfo_bKGDPlteIndex);
    VoC_add16_rd(REG1,REG1,bKGDInfo_bKGDPlteIndex);

    VoC_NOP();

    VoC_lw16inc_p(REG2,REG1,DEFAULT);
    VoC_lw16inc_p(REG3,REG1,DEFAULT);
    VoC_lw16_p(REG7,REG1,DEFAULT);

    VoC_sw16_d(REG2,bKGDInfo_bKGDRed);
    VoC_sw16_d(REG3,bKGDInfo_bKGDGreen);
    VoC_sw16_d(REG7,bKGDInfo_bKGDBlue);

Coolsand_InitTransform_End:

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ProcessOneRow
 *
 * Description: UpdateInfo
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *              REG45:ChunkLength
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       03/30/2012
 **************************************************************************************/
void Coolsand_ProcessOneRow(void)
{

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);//return 0;

    //  if (IHDRInfo.Interlace)
    VoC_lw16_d(REG1,IHDRInfo_Interlace);
    VoC_bez16_r(Coolsand_ProcessOneRow_CurPass_6,REG1)



    /*      switch(CurPass)
            {
            case 0:
                if (CurRowNumber & 0x07)
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break; */
    VoC_lw16_d(REG1,CurRowNumber);

    VoC_bnez16_d(Coolsand_ProcessOneRow_CurPass_0,CurPass)

    VoC_and16_ri(REG1,0x07);
    VoC_bez16_r(Coolsand_ProcessOneRow_CurPass_6,REG1)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);

Coolsand_ProcessOneRow_CurPass_0:


    /*
            case 1:
                if ((CurRowNumber & 0x07) || IHDRInfo.Width < 5)
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break;

            case 2:
                if ((CurRowNumber & 0x07) != 4)
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break;
    */

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,2,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_ProcessOneRow_CurPass_1,REG2,CurPass)


    VoC_lw16i_short(REG2,5,DEFAULT);

    VoC_bngt16_rd(Coolsand_ProcessOneRow_L0,REG2,IHDRInfo_Width)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_L0:

    VoC_and16_ri(REG1,0x07);
    VoC_bez16_r(Coolsand_ProcessOneRow_CurPass_6,REG1)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);

Coolsand_ProcessOneRow_CurPass_1:

    VoC_bne16_rd(Coolsand_ProcessOneRow_CurPass_2,REG3,CurPass)

    VoC_lw16i_short(REG2,4,DEFAULT);

    VoC_and16_ri(REG1,0x07);

    VoC_be16_rr(Coolsand_ProcessOneRow_CurPass_6,REG1,REG2)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);

Coolsand_ProcessOneRow_CurPass_2:



    /*
            case 3:
                if ((CurRowNumber & 3) || IHDRInfo.Width < 3)
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break;

            case 4:
                if ((CurRowNumber & 3) != 2)
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break;

    */

    VoC_lw16i_short(REG2,3,DEFAULT);
    VoC_lw16i_short(REG3,4,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_ProcessOneRow_CurPass_3,REG2,CurPass)


    VoC_lw16i_short(REG2,3,DEFAULT);

    VoC_bngt16_rd(Coolsand_ProcessOneRow_L1,REG2,IHDRInfo_Width)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_L1:

    VoC_and16_ri(REG1,0x03);
    VoC_bez16_r(Coolsand_ProcessOneRow_CurPass_6,REG1)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);

Coolsand_ProcessOneRow_CurPass_3:



    VoC_bne16_rd(Coolsand_ProcessOneRow_CurPass_4,REG3,CurPass)


    VoC_lw16i_short(REG2,2,DEFAULT);

    VoC_and16_ri(REG1,0x03);

    VoC_be16_rr(Coolsand_ProcessOneRow_CurPass_6,REG1,REG2)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);


Coolsand_ProcessOneRow_CurPass_4:

    /*
            case 5:
                if ((CurRowNumber & 1) || IHDRInfo.Width < 2)
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break;

            case 6:
                if (!(CurRowNumber & 1))
                {
                    ReadFinishRow();

    //IMGDP_EXIT(10022);

                    return 0;
                }
                break;

    */




    VoC_lw16i_short(REG2,5,DEFAULT);
    VoC_lw16i_short(REG3,6,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_ProcessOneRow_CurPass_5,REG2,CurPass)

    VoC_lw16i_short(REG2,2,DEFAULT);

    VoC_bngt16_rd(Coolsand_ProcessOneRow_L2,REG2,IHDRInfo_Width)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_L2:

    VoC_and16_ri(REG1,0x01);
    VoC_bez16_r(Coolsand_ProcessOneRow_CurPass_6,REG1)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);

Coolsand_ProcessOneRow_CurPass_5:
    VoC_bne16_rd(Coolsand_ProcessOneRow_CurPass_6,REG3,CurPass)

    VoC_and16_ri(REG1,0x01);

    VoC_bnez16_r(Coolsand_ProcessOneRow_CurPass_6,REG1)

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_jump(Coolsand_ProcessOneRow_End);

Coolsand_ProcessOneRow_CurPass_6:

//  PNGzstream.next_out = RowBuffer;
//  PNGzstream.avail_out = CurPassRowBytes;


    VoC_lw32_d(ACC0,CurPassRowBytes);

    VoC_NOP();

    VoC_sw32_d(ACC0,PNGzstream_avail_out);

    /*  do
        {

            if (!(PNGzstream.avail_in))
            {
                UINT32 IDATChunkLength;

                while (!IDATSize)
                {
                    UINT32 IDATcrc = 0;
                    UINT32 FileIDATcrc = 0;

                    FileChunks.IDATNum++;

                    IDATChunkLength = ((**PngFileContent) << 24) + ((*(*PngFileContent + 1)) << 16)
                        + ((*(*PngFileContent + 2)) << 8) + (*(*PngFileContent + 3));
                    *PngFileContent += 4;
                    PngStreamBuf.byteleft -=4;

                    if(PngStreamBuf.byteleft< (INT32)IDATChunkLength && PngFileHandle!=-1){
                        INT32 pngfile_offect=0;
                        INT32 byte_load=0;
                        INT32 byte_cosumed=(char *)PNGzstream.next_in-PngStreamBuf.PNGSTREAM;
                        memmove(PngStreamBuf.PNGSTREAM,PNGzstream.next_in,MAXPNG_BUF-byte_cosumed);
                        MMI_FS_Read(PngFileHandle, (PngStreamBuf.PNGSTREAM+MAXPNG_BUF-byte_cosumed), byte_cosumed,&byte_load );
                        pngfile_offect=*PngFileContent-PNGzstream.next_in;
                        PNGzstream.next_in=PngStreamBuf.PNGSTREAM;
                        *PngFileContent=PNGzstream.next_in+pngfile_offect;
                        PngStreamBuf.byteleft += byte_load;
                }

                    IDATcrc = crc32(IDATcrc, *PngFileContent, IDATChunkLength + 4);
                    *PngFileContent += 4;
                    PngStreamBuf.byteleft -=4;
                    FileIDATcrc = (*((*PngFileContent) + IDATChunkLength) << 24) + (*((*PngFileContent) + 1 + IDATChunkLength) << 16)
                        + (*((*PngFileContent) + 2 + IDATChunkLength) << 8) + (*((*PngFileContent) + 3 +IDATChunkLength));

                    if (IDATcrc != FileIDATcrc)
                    {
                        return IDATCRCERR;
                    }

                    IDATSize = IDATChunkLength;

    //              *PngFileContent += 4;
                }

                PNGzstream.avail_in = 0x2000;
                PNGzstream.next_in = *PngFileContent;

                if (IDATSize > 0x2000){
                    *PngFileContent += 0x2000;
                    PngStreamBuf.byteleft -=0x2000;
                    }
                else{
                    *PngFileContent += IDATSize + 4;
                    PngStreamBuf.byteleft -=IDATSize + 4;

                    }

                if (IDATSize < 0x2000)
                    PNGzstream.avail_in = IDATSize;

                IDATSize -= PNGzstream.avail_in;
            }

            ret = inflate(&PNGzstream, Z_PARTIAL_FLUSH);


            if (ret == Z_STREAM_END)
            {
                if (PNGzstream.avail_out || PNGzstream.avail_in || IDATSize)
                    return INFLATEERR;


                break;
            }

            if (ret != Z_OK)
            {
            mmi_trace(1,"error happens here:2, ret:%d",ret);
            return INFLATEERR;
            }


        } while(PNGzstream.avail_out);

      */

Coolsand_ProcessOneRow_main_loop:

    VoC_lw32_d(ACC0,PNGzstream_avail_in);

    VoC_bnez32_r(Coolsand_ProcessOneRow_main_loop_L4,ACC0)

Coolsand_ProcessOneRow_main_loop_L0:

    VoC_lw32_d(ACC0,IDATSize);
    VoC_bnez32_r(Coolsand_ProcessOneRow_main_loop_L2,ACC0)


    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_add16_rd(REG1,REG1,FileChunks_IDATNum);
    VoC_NOP();
    VoC_sw16_d(REG1,FileChunks_IDATNum);

    VoC_jal(Coolsand_DmaInData);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);


    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_lw32_d(RL7,GLOBAL_byteleft);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG5,16,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,16,DEFAULT);
    VoC_sub32_rr(RL7,RL7,REG23,IN_PARALLEL);//      PngStreamBuf.byteleft -=4;

    VoC_lw16i_short(REG6,4,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rr(REG45,REG45,REG67,DEFAULT);

    VoC_push32(REG45,DEFAULT);

    VoC_sw32_d(RL7,GLOBAL_byteleft);

    VoC_jal(Coolsand_crc32);

    VoC_pop32(REG45,DEFAULT);

    VoC_bez16_r(Coolsand_ProcessOneRow_main_loop_L1,REG7);
    VoC_lw16i(REG7,CRCERR);
    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_main_loop_L1:

    VoC_sw32_d(REG45,IDATSize);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,16,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG6,16,DEFAULT);

    VoC_jump(Coolsand_ProcessOneRow_main_loop_L0);
Coolsand_ProcessOneRow_main_loop_L2:

    /*

                    PNGzstream.next_in = *PngFileContent;

                if (IDATSize > 0x2000){
                    *PngFileContent += 0x2000;
                    PngStreamBuf.byteleft -=0x2000;
                    }
                else{
                    *PngFileContent += IDATSize + 4;
                    PngStreamBuf.byteleft -=IDATSize + 4;

                    }

                if (IDATSize < 0x2000)
                    PNGzstream.avail_in = IDATSize;

                IDATSize -= PNGzstream.avail_in;
    */

    VoC_lw16i_short(ACC0_HI,0,DEFAULT);

    VoC_lw16i(ACC0_LO,0x2000);

    VoC_bngt32_rd(Coolsand_ProcessOneRow_main_loop_L3,ACC0,IDATSize)
    VoC_lw32_d(ACC0,IDATSize);
Coolsand_ProcessOneRow_main_loop_L3:
    VoC_sub32_dr(ACC1,IDATSize,ACC0);
    VoC_sw32_d(ACC0,PNGzstream_avail_in);
    VoC_sw32_d(ACC1,IDATSize);


Coolsand_ProcessOneRow_main_loop_L4:



// VoC_directive: PARSER_OFF
//  printf("inflate_count:%d",inflate_count);

    if (inflate_count==55)
    {
        //  printf("ok");
    }




    inflate_count++;



// VoC_directive: PARSER_ON


    VoC_jal(Coolsand_inflate);
    //REG7:return;








    VoC_lw16i_short(REG6,Z_STREAM_END,DEFAULT);
    VoC_bne16_rr(Coolsand_ProcessOneRow_main_loop_L8,REG7,REG6)

    VoC_bez32_d(Coolsand_ProcessOneRow_main_loop_L5,PNGzstream_avail_out)
    VoC_lw16i(REG7,INFLATEERR);
    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_main_loop_L5:

    VoC_bez32_d(Coolsand_ProcessOneRow_main_loop_L6,PNGzstream_avail_in)
    VoC_lw16i(REG7,INFLATEERR);
    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_main_loop_L6:

    VoC_bez32_d(Coolsand_ProcessOneRow_main_loop_L7,IDATSize)
    VoC_lw16i(REG7,INFLATEERR);
    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_main_loop_L7:

    VoC_jump(Coolsand_ProcessOneRow_main_loop_End)
Coolsand_ProcessOneRow_main_loop_L8:

    VoC_lw16i_short(REG6,Z_OK,DEFAULT);
    VoC_be16_rr(Coolsand_ProcessOneRow_main_loop_L9,REG7,REG6)
    VoC_lw16i(REG7,INFLATEERR);
    VoC_jump(Coolsand_ProcessOneRow_End);
Coolsand_ProcessOneRow_main_loop_L9:


    VoC_lw32_d(ACC0,PNGzstream_avail_out);
    VoC_bnez32_r(Coolsand_ProcessOneRow_main_loop,ACC0)
Coolsand_ProcessOneRow_main_loop_End:


    VoC_lw16_d(REG1,IHDRInfo_ColorType);
    VoC_lw16_d(REG2,CurInterlaceRowWidth);
    VoC_lw16_d(REG3,ImgChannels);
    VoC_lw16_d(REG4,IHDRInfo_BitDepth);
    VoC_lw16_d(REG5,PixelDepth);

    VoC_sw16_d(REG1,RowInfo_ColorType);
    VoC_sw16_d(REG2,RowInfo_Width);
    VoC_sw16_d(REG3,RowInfo_Channels);
    VoC_sw16_d(REG4,RowInfo_BitDepth);
    VoC_sw16_d(REG5,RowInfo_PixelDepth);


    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_lw16_d(REG2,RowInfo_PixelDepth);

    VoC_multf32_rd(ACC0,REG2,RowInfo_Width);

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_bngt16_rd(Coolsand_ProcessOneRow_CurPass_7,REG3,RowInfo_PixelDepth)

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

Coolsand_ProcessOneRow_CurPass_7:

    VoC_shr32_ri(ACC0,3,DEFAULT);
    VoC_NOP();
    VoC_sw32_d(ACC0,RowInfo_RowBytes);


// VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP


    {
        short *PoutbufI=(short*)&RAM_Y[(GLOBAL_RawOutBuf-0x4000)/2];
        int i=0;

        fread(g_Buffer_test,1,RAM_X[RowBytes/2],DebugFileHandle);

        for (i=0; i<RAM_X[RowBytes/2]; i++)
        {
            if(g_Buffer_test[i]!=PoutbufI[i])
            {
                printf("\ninflate ERROR!\n");
                while (1)
                {

                }
            }
        }
    }



#endif

// VoC_directive: PARSER_ON


    VoC_bez16_d(Coolsand_ProcessOneRow_CurPass_8,GLOBAL_RawOutBuf)


    VoC_lw16i_short(REG3,1,DEFAULT);

    VoC_lw16i_set_inc(REG1,GLOBAL_RawOutBuf,1);
    VoC_lw16d_set_inc(REG2,GLOBAL_PreRow,1);

    VoC_lw16inc_p(REG3,REG1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG3,IN_PARALLEL);

    VoC_jal(Coolsand_FilterRow);

Coolsand_ProcessOneRow_CurPass_8:

    VoC_lw16i_set_inc(REG1,GLOBAL_RawOutBuf,1);
    VoC_lw16d_set_inc(REG2,GLOBAL_PreRow,1);

    VoC_lw16i_set_inc(REG3,0,1);

Coolsand_ProcessOneRow_CurPass_9:
    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_bgt16_rd(Coolsand_ProcessOneRow_CurPass_10,REG3,RowBytes)

    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_inc_p(REG3,IN_PARALLEL);

    VoC_jump(Coolsand_ProcessOneRow_CurPass_9);
Coolsand_ProcessOneRow_CurPass_10:



// VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP


    {
        short *PoutbufI=(short*)&RAM_Y[(GLOBAL_RawOutBuf-0x4000)/2];
        int i=0;

        fread(g_Buffer_test,1,RAM_X[RowBytes/2],DebugFileHandle);

        for (i=0; i<RAM_X[RowBytes/2]; i++)
        {
            if(g_Buffer_test[i]!=PoutbufI[i])
            {
                printf("\n Coolsand_FilterRow ERROR!\n");
                while (1)
                {

                }

            }
        }
    }



#endif

// VoC_directive: PARSER_ON
    VoC_bnez16_d(Coolsand_ProcessOneRow_CurPass_13,IDATHandle_skipRow)

    VoC_bez16_d(Coolsand_ProcessOneRow_CurPass_11,transform)

    VoC_lw16i_set_inc(REG1,GLOBAL_RawOutBuf,1);

    VoC_lw32_d(RL7,imgOutPtr);

    VoC_jal(Coolsand_ImageTransform);

Coolsand_ProcessOneRow_CurPass_11:

    // VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP





    {
        short *PoutbufI=(short*)&RAM_Y[(GLOBAL_RawOutBuf-0x4000)/2];
        int i=0;

        fread(g_Buffer_test,1,RAM_X[RowBytes/2],DebugFileHandle);

        for (i=0; i<RAM_X[RowBytes/2]; i++)
        {
            if(g_Buffer_test[i]!=PoutbufI[i])
            {

                printf("\nCoolsand_ImageTransform ERROR!\n");

                while (1)
                {

                }
            }


        }
    }



#endif

// VoC_directive: PARSER_ON

    VoC_lw16i_set_inc(REG1,GLOBAL_RawOutBuf,1);

    VoC_lw32_d(RL7,imgOutPtr);

    VoC_lw16i(REG7,0xff);

    VoC_bez16_d(Coolsand_ProcessOneRow_CurPass_12,IHDRInfo_Interlace)

    VoC_lw16i(REG7,PassMask);

    VoC_add16_rd(REG7,REG7,CurPass);

    VoC_NOP();

    VoC_lw16_p(REG7,REG7,DEFAULT);

Coolsand_ProcessOneRow_CurPass_12:

    VoC_jal(Coolsand_CombineRow);





// VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP


    {
        unsigned char  *PoutbufI=(unsigned char*)&RAM_Y[(GLOBAL_RawOutBuf-0x4000)/2];
        int i=0;
        unsigned short pngcutx2=RAM_X[PngCutOffsetX2/2]&0xffff;
        unsigned short pngcutx1=RAM_X[PngCutOffsetX1/2]&0xffff;
        PoutbufI=PoutbufI+2;

        fread(g_Buffer_test,1,(pngcutx2-pngcutx1+1)*2,DebugFileHandle);

        for (i=0; i<(pngcutx2-pngcutx1+1)*2; i++)
        {
            if(g_Buffer_test[i]!=PoutbufI[i])
            {
                printf("\nCoolsand_CombineRow ERROR!\n");

                while (1)
                {


                }

            }

        }
    }



#endif

// VoC_directive: PARSER_ON

Coolsand_ProcessOneRow_CurPass_13:

    VoC_jal(Coolsand_ReadFinishRow);

    VoC_lw16i_short(REG7,0,DEFAULT);

Coolsand_ProcessOneRow_End:
    VoC_pop16(RA,DEFAULT);

    VoC_NOP();

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ReadFinishRow
 *
 * Description: read
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/05/2012
 **************************************************************************************/
void Coolsand_ReadFinishRow(void)
{
    //const int PassXStart[7] = {0, 4, 0, 2, 0, 1, 0};
    //const int PassXInc[7] = {8, 8, 4, 4, 2, 2, 1};
    //const int PassYStart[7] = {0, 0, 4, 0, 2, 0, 1};
    //const int PassYInc[7] = {8, 8, 8, 4, 4, 2, 2};


//  CurRowNumber++;
//  if (CurRowNumber < IHDRInfo.Height)
//      return;

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_lw16_d(REG2,CurRowNumber);

    VoC_add16_rr(REG2,REG2,REG1,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    VoC_lw16_d(REG3,IHDRInfo_Interlace)

    VoC_sw16_d(REG2,CurRowNumber);

    VoC_bnlt16_rd(Coolsand_ReadFinishRow_L0,REG2,IHDRInfo_Height)

    VoC_jump(Coolsand_ReadFinishRow_End)

Coolsand_ReadFinishRow_L0:

//  if (IHDRInfo.Interlace)
    VoC_bez16_r(Coolsand_ReadFinishRow_End,REG3)

    VoC_sw16_d(REG1,CurRowNumber);

    VoC_lw16d_set_inc(REG0,Pass_Xstart,1);
    VoC_lw16d_set_inc(REG1,Pass_Xinc,1);
    VoC_lw16d_set_inc(REG2,Pass_Ystart,1);
    VoC_lw16d_set_inc(REG3,Pass_Yinc,1);

    VoC_lw16_d(REG4,CurPass);
    VoC_lw16_d(REG5,CurInterlaceRowWidth);
    VoC_lw16_d(REG6,CurPassRowNum);

    VoC_lw32_d(ACC0,CurPassRowBytes);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);
    VoC_add16_rr(REG1,REG1,REG4,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

//      VoC_movreg32(RL6,REG01,DEFAULT);
//      VoC_movreg32(RL7,REG23,IN_PARALLEL);

    VoC_lw16i_short(REG7,1,DEFAULT);

Coolsand_ReadFinishRow_L1:

    VoC_add16_rr(REG4,REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,7,IN_PARALLEL);

    VoC_bngt16_rr(Coolsand_ReadFinishRow_LX,REG7,REG4);

    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_lw16_p(REG5,REG0,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_be16_rr(Coolsand_ReadFinishRow_L2,REG5,REG7)

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_be16_rr(Coolsand_ReadFinishRow_L2,REG5,REG7)

    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_lw16i_short(REG6,2,IN_PARALLEL);

    VoC_be16_rr(Coolsand_ReadFinishRow_L2,REG5,REG7)

    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

Coolsand_ReadFinishRow_L2:

    VoC_lw16_d(REG7,IHDRInfo_Width);

    VoC_sub16inc_rp(REG5,REG7,REG0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG7,DEFAULT);

    VoC_add16inc_rp(REG5,REG5,REG1,DEFAULT)
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_shr16_rr(REG5,REG6,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_multf32_rd(ACC0,REG5,PixelDepth);

    VoC_bngt16_rd(Coolsand_ReadFinishRow_L3,REG7,IHDRInfo_BitDepth)

    VoC_lw16i_short(REG6,7,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,REG67,DEFAULT);

Coolsand_ReadFinishRow_L3:

    VoC_shr32_ri(ACC0,3,DEFAULT);

    VoC_lw16_d(REG6,CurPassRowNum);

    VoC_lw16_d(REG7,transform);

    VoC_and16_ri(REG7,INTERLACE);

    VoC_bnez16_r(Coolsand_ReadFinishRow_LX,REG7)

    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_sub16_dr(REG6,IHDRInfo_Height,REG7);

    VoC_add16_rp(REG6,REG6,REG3,DEFAULT);
    VoC_movreg32(ACC1,REG01,IN_PARALLEL)

    VoC_add16_rp(REG6,REG6,REG2,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_lw16_p(REG1,REG3,DEFAULT);
    VoC_lw16i_short(REG0,1,IN_PARALLEL);

    VoC_be16_rr(Coolsand_ReadFinishRow_L4,REG1,REG7)

    VoC_lw16i_short(REG1,4,DEFAULT);
    VoC_lw16i_short(REG0,2,IN_PARALLEL);

    VoC_be16_rr(Coolsand_ReadFinishRow_L4,REG1,REG7)

    VoC_lw16i_short(REG1,8,DEFAULT);
    VoC_lw16i_short(REG0,3,IN_PARALLEL);

Coolsand_ReadFinishRow_L4:

    VoC_shr16_rr(REG6,REG0,DEFAULT);

    VoC_movreg32(REG01,ACC1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);


    VoC_bez16_r(Coolsand_ReadFinishRow_L1,REG6);

    VoC_bez16_r(Coolsand_ReadFinishRow_L1,REG5)

Coolsand_ReadFinishRow_LX:


    VoC_sw16_d(REG4,CurPass);
    VoC_sw16_d(REG5,CurInterlaceRowWidth);
    VoC_sw16_d(REG6,CurPassRowNum);

    VoC_sw32_d(ACC0,CurPassRowBytes);

    VoC_lw16i_short(REG7,7,DEFAULT);

    VoC_bgt16_rr(Coolsand_ReadFinishRow_End,REG7,REG4);

    //inflateReset(&PNGzstream);
    VoC_jal(Coolsand_inflateReset);

Coolsand_ReadFinishRow_End:

    VoC_pop16(RA,DEFAULT);


    VoC_lw16i_short(WRAP0,GLOBAL_WRAP_BIT,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_inflate
 *
 * Description: read
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/10/2012
 **************************************************************************************/
void Coolsand_inflate(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,Z_OK,IN_PARALLEL);

    VoC_lw16i_short(REG4,TYPE,DEFAULT);
    VoC_lw16i_short(REG5,TYPEDO,IN_PARALLEL);

    VoC_sw16_d(REG7,inflate_ret);

    VoC_bne16_rd(Coolsand_inflate_L0,REG4,state_mode);

    VoC_sw16_d(REG5,state_mode);
Coolsand_inflate_L0:

    VoC_lw32_d(ACC0,PNGzstream_avail_in);
    VoC_lw32_d(ACC1,PNGzstream_avail_out);

    VoC_sw32_d(ACC0,inflate_in);
    VoC_sw32_d(ACC1,inflate_out);

    VoC_lw16_d(REG1,state_mode);

    VoC_lw32_d(RL7,state_hold);

Coolsand_inflate_main_loop:

    /*
              case HEAD:
                if (state->wrap == 0)
                {
                    state->mode = TYPEDO;
                    break;
                }
                NEEDBITS(16);
    #ifdef GUNZIP
                if ((state->wrap & 2) && hold == 0x8b1f) {  // gzip header
                    state->check = crc32(0L, Z_NULL, 0);
                    CRC2(state->check, hold);
                    INITBITS();
                    state->mode = FLAGS;
                    break;
                }
                state->flags = 0;           // expect zlib header
                if (state->head != Z_NULL)
                    state->head->done = -1;
                if (!(state->wrap & 1) ||   // check if zlib header allowed
    #else
                if (
    #endif
                    ((BITS(8) << 8) + (hold >> 8)) % 31) {
                    strm->msg = (char *)"incorrect header check";
                    state->mode = BAD;
                    break;
                }
                if (BITS(4) != Z_DEFLATED) {
                    strm->msg = (char *)"unknown compression method";
                    state->mode = BAD;
                    break;
                }
                DROPBITS(4);
                len = BITS(4) + 8;
                if (len > state->wbits) {
                    strm->msg = (char *)"invalid window size";
                    state->mode = BAD;
                    break;
                }
                state->dmax = 1U << len;
                Tracev((stderr, "inflate:   zlib header ok\n"));
                strm->adler = state->check = adler32(0L, Z_NULL, 0);
                state->mode = hold & 0x200 ? DICTID : TYPE;
                INITBITS();
                break;
    */

    VoC_lw16i_short(REG2,HEAD,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_HEAD,REG1,REG2)

    VoC_bnez16_d(Coolsand_inflate_case_HEAD_L0,state_wrap);
    VoC_lw16i_short(REG1,TYPEDO,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_TYPEDO_Begin);
Coolsand_inflate_case_HEAD_L0:

    VoC_lw16i_short(REG7,16,DEFAULT);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX0,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX0:

    VoC_lw16_d(REG2,state_wrap);

    VoC_and16_ri(REG2,2);

    VoC_bez16_r(Coolsand_inflate_case_HEAD_L0_0,REG2)

    VoC_movreg16(REG4,RL7_LO,DEFAULT);

    VoC_lw16i(REG3,0x8b1f);

    VoC_bne16_rr(Coolsand_inflate_case_HEAD_L0_0,REG3,REG4)
    VoC_movreg16(REG1,REG3,DEFAULT);
    VoC_jal(Coolsand_CRC2);
    VoC_sw32_d(REG23,state_check);

    VoC_jal(Coolsand_INITBITS);
    VoC_lw16i_short(REG1,FLAGS,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_FLAGS_Begin);
Coolsand_inflate_case_HEAD_L0_0:

//            state->flags = 0;           /* expect zlib header */

    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_lw16i_short(REG7,4,DEFAULT);

    VoC_sw16_d(REG3,state_flags);

    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG2,Z_DEFLATED,DEFAULT);

    VoC_be16_rr(Coolsand_inflate_case_HEAD_L1,REG6,REG2);

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);


    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_HEAD_L1:

    VoC_lw16i_short(REG7,4,DEFAULT);

    VoC_jal(Coolsand_DROPBITS);

    /*
                DROPBITS(4);
                len = BITS(4) + 8;
                if (len > state->wbits) {
                    strm->msg = (char *)"invalid window size";
                    state->mode = BAD;
                    break;
                }

    */
    VoC_lw16i_short(REG7,4,DEFAULT);

    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG2,8,DEFAULT);

    VoC_add16_rr(REG4,REG6,REG2,DEFAULT);

    VoC_bngt16_rd(Coolsand_inflate_case_HEAD_L2,REG4,state_wbits)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);

Coolsand_inflate_case_HEAD_L2:

    /*            state->dmax = 1U << len;
                Tracev((stderr, "inflate:   zlib header ok\n"));
                strm->adler = state->check = adler32(0L, Z_NULL, 0);
                state->mode = hold & 0x200 ? DICTID : TYPE;
    */


    VoC_lw16i_short(REG3,0,DEFAULT);
    VoC_movreg16(REG5,RL7_LO,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_shru16_rr(REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_and16_ri(REG5,0x200);

    VoC_sw32_d(REG23,state_dmax);

    VoC_sw16_d(REG6,state_check);

    VoC_lw16i_short(REG1,DICTID,DEFAULT);

    VoC_bnez16_r(Coolsand_inflate_case_HEAD_L3,REG5)

    VoC_lw16i_short(REG1,TYPE,DEFAULT);
    VoC_NOP();

Coolsand_inflate_case_HEAD_L3:

    VoC_sw16_d(REG1,state_mode);

    VoC_jal(Coolsand_INITBITS);

    VoC_jump(Coolsand_inflate_main_loop);
Coolsand_inflate_case_HEAD:
    /*

            case FLAGS:
                NEEDBITS(16);
                state->flags = (int)(hold);
                if ((state->flags & 0xff) != Z_DEFLATED) {
                    strm->msg = (char *)"unknown compression method";
                    state->mode = BAD;
                    break;
                }
                if (state->flags & 0xe000) {
                    strm->msg = (char *)"unknown header flags set";
                    state->mode = BAD;
                    break;
                }
                if (state->head != Z_NULL)
                    state->head->text = (int)((hold >> 8) & 1);
                if (state->flags & 0x0200) CRC2(state->check, hold);
                INITBITS();
                state->mode = TIME;
    */

    VoC_lw16i_short(REG2,FLAGS,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_FLAGS,REG1,REG2)
Coolsand_inflate_case_FLAGS_Begin:







Coolsand_inflate_case_FLAGS:










    /*        case TYPE:
                if (flush == Z_BLOCK) goto inf_leave;
            */


    VoC_lw16i_short(REG2,TYPE,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_TYPE,REG1,REG2)
Coolsand_inflate_case_TYPE_Begin:
    VoC_jump(Coolsand_inflate_case_TYPEDO_Begin);
Coolsand_inflate_case_TYPE:


    /*        case TYPEDO:
                if (state->last)
                {
                    BYTEBITS();
                    state->mode = CHECK;
                    break;
                }
                NEEDBITS(3);
                state->last = BITS(1);
                DROPBITS(1);
                switch (BITS(2))
                {
                case 0:                             // stored block
                    Tracev((stderr, "inflate:     stored block%s\n",
                            state->last ? " (last)" : ""));
                    state->mode = STORED;
                    break;
                case 1:                             // fixed block
                    fixedtables(state);
                    Tracev((stderr, "inflate:     fixed codes block%s\n",
                            state->last ? " (last)" : ""));
                    state->mode = LEN;              // decode codes
                    break;
                case 2:                             // dynamic block
                    Tracev((stderr, "inflate:     dynamic codes block%s\n",
                            state->last ? " (last)" : ""));
                    state->mode = TABLE;
                    break;
                case 3:
                    strm->msg = (char *)"invalid block type";
                    state->mode = BAD;
                }
                DROPBITS(2);
                break;
    */


    VoC_lw16i_short(REG2,TYPEDO,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_TYPEDO,REG1,REG2)
Coolsand_inflate_case_TYPEDO_Begin:

    VoC_bez16_d(Coolsand_inflate_case_TYPEDO_L0,state_last)


    VoC_jal(Coolsand_BYTEBITS);
    VoC_lw16i_short(REG1,CHECK,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_CHECK_Begin)

Coolsand_inflate_case_TYPEDO_L0:



    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX1,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX1:

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_movreg16(REG4,REG6,IN_PARALLEL);
    VoC_jal(Coolsand_DROPBITS);

    VoC_sw16_d(REG4,state_last);

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);
    VoC_jal(Coolsand_DROPBITS);

    VoC_lw16i_short(REG1,STORED,DEFAULT);

    VoC_bez16_r(Coolsand_inflate_case_TYPEDO_L1,REG5)

    VoC_lw16i_short(REG1,LEN,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL)


    VoC_be16_rr(Coolsand_inflate_case_TYPEDO_L1,REG5,REG2)

    VoC_lw16i_short(REG1,TABLE,DEFAULT);
    VoC_lw16i_short(REG2,2,IN_PARALLEL);

    VoC_be16_rr(Coolsand_inflate_case_TYPEDO_L1,REG5,REG2)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);


    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_TYPEDO_L1:

    VoC_lw16i_short(REG2,LEN,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_bne16_rr(Coolsand_inflate_case_TYPEDO_L2,REG1,REG2)

    VoC_jal(Coolsand_fixedtables);

Coolsand_inflate_case_TYPEDO_L2:

    VoC_jump(Coolsand_inflate_main_loop);
Coolsand_inflate_case_TYPEDO:






    /*

              case STORED:
                BYTEBITS();                         // go to byte boundary
                NEEDBITS(32);
                if ((hold & 0xffff) != ((hold >> 16) ^ 0xffff)) {
                    strm->msg = (char *)"invalid stored block lengths";
                    state->mode = BAD;
                    break;
                }
                state->length = (unsigned)hold & 0xffff;
                Tracev((stderr, "inflate:       stored length %u\n",
                        state->length));
                INITBITS();
                state->mode = COPY;

    */



    VoC_lw16i_short(REG2,STORED,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_STORED,REG1,REG2)
//Coolsand_inflate_case_STORED_Begin:

    VoC_jal(Coolsand_BYTEBITS);
    VoC_lw16i(REG7,32);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_movreg32(ACC0,RL7,DEFAULT);
    VoC_movreg32(ACC1,RL7,IN_PARALLEL);

    VoC_lw16i_short(REG6,-1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_and32_rr(ACC0,REG67,DEFAULT);
    VoC_shru32_ri(ACC1,16,IN_PARALLEL);

    VoC_xor32_rr(ACC1,REG67,DEFAULT);

    VoC_be32_rr(Coolsand_inflate_case_STORED_L0,ACC0,ACC1)
    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);

    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_STORED_L0:

    VoC_sw16_d(ACC0_LO,state_length);

    VoC_lw16i(REG7,COPY);

    VoC_jal(Coolsand_INITBITS);

    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_case_COPY_Begin);
Coolsand_inflate_case_STORED:





    /*

              case COPY:
                copy = state->length;
                if (copy) {
                    if (copy > have) copy = have;
                    if (copy > left) copy = left;
                    if (copy == 0) goto inf_leave;
                    zmemcpy(put, next, copy);
                    have -= copy;
                    next += copy;
                    left -= copy;
                    put += copy;
                    state->length -= copy;
                    break;
                }
                Tracev((stderr, "inflate:       stored end\n"));
                state->mode = TYPE;
                break;
    */



    VoC_lw16i_short(REG2,COPY,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_COPY,REG1,REG2)
Coolsand_inflate_case_COPY_Begin:

    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw16_d(ACC0_LO,state_length);

    VoC_bez32_r(Coolsand_inflate_case_COPY_L0,ACC0)

    VoC_bngt32_rd(Coolsand_inflate_case_COPY_L1,ACC0,PNGzstream_avail_in)

    VoC_lw32_d(ACC0,PNGzstream_avail_in);

Coolsand_inflate_case_COPY_L1:

    VoC_bngt32_rd(Coolsand_inflate_case_COPY_L2,ACC0,PNGzstream_avail_out)

    VoC_lw32_d(ACC0,PNGzstream_avail_out);

Coolsand_inflate_case_COPY_L2:

    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC0)

    VoC_lw32z(REG67,DEFAULT);

    VoC_sub32_dr(REG23,PNGzstream_avail_in,ACC0);
    VoC_sub32_dr(REG45,PNGzstream_avail_out,ACC0);

    VoC_lw16_d(REG6,state_length);

    VoC_sub32_rr(REG67,REG67,ACC0,DEFAULT);

    VoC_sw32_d(REG23,PNGzstream_avail_in);
    VoC_sw32_d(REG45,PNGzstream_avail_out);

    VoC_sw16_d(REG6,state_length);

    VoC_lw16d_set_inc(REG1,GLOBAL_RawOutBufAddr,1);

    VoC_lbinc_p(REG0);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

Coolsand_inflate_case_COPY_L3:
    VoC_bngtz32_r(Coolsand_inflate_case_COPY_L4,ACC0)

    VoC_jal(Coolsand_DmaInData);

    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,REG23,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    VoC_jump(Coolsand_inflate_case_COPY_L3);
Coolsand_inflate_case_COPY_L4:

    VoC_sw16_d(REG1,GLOBAL_RawOutBufAddr);

    VoC_lw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_main_loop);

Coolsand_inflate_case_COPY_L0:

    VoC_lw16i(REG1,TYPE);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_main_loop);
Coolsand_inflate_case_COPY:










    /*        case TABLE:
                NEEDBITS(14);
                state->nlen = BITS(5) + 257;
                DROPBITS(5);
                state->ndist = BITS(5) + 1;
                DROPBITS(5);
                state->ncode = BITS(4) + 4;
                DROPBITS(4);
    #ifndef PKZIP_BUG_WORKAROUND
                if (state->nlen > 286 || state->ndist > 30) {
                    strm->msg = (char *)"too many length or distance symbols";
                    state->mode = BAD;
                    break;
                }
    #endif
                Tracev((stderr, "inflate:       table sizes ok\n"));
                state->have = 0;
                state->mode = LENLENS;

    */

    VoC_lw16i_short(REG2,TABLE,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_TABLE,REG1,REG2)
//Coolsand_inflate_case_TABLE_Begin:


    VoC_jal(Coolsand_DmaInData);

    VoC_lw16i(REG2,257);

    VoC_lw16i_short(REG7,14,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX2,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX2:

    VoC_lw16i_short(REG7,5,DEFAULT);

    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,5,DEFAULT);
    VoC_add16_rr(REG4,REG6,REG2,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);

    VoC_lw16i_short(REG7,5,DEFAULT);
    VoC_lw16i_short(REG2,4,IN_PARALLEL);

    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,5,DEFAULT);
    VoC_add16_rr(REG5,REG6,REG3,DEFAULT);
    VoC_jal(Coolsand_DROPBITS);

    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG2,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);


    VoC_sw16_d(REG4,state_nlen);
    VoC_sw16_d(REG5,state_ndist);
    VoC_sw16_d(REG6,state_ncode);

    VoC_lw16i(REG2,286);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw16i_short(REG1,LENLENS,IN_PARALLEL);

    VoC_lw16i_short(REG3,30,DEFAULT);



    VoC_bngt16_rr(Coolsand_inflate_case_TABLE_L0,REG4,REG2)
    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);

    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_TABLE_L0:

    VoC_bngt16_rr(Coolsand_inflate_case_TABLE_L1,REG5,REG3)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);

    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave);

Coolsand_inflate_case_TABLE_L1:

    VoC_sw16_d(REG7,state_have);
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_LENLENS_Begin);
Coolsand_inflate_case_TABLE:

    /*

                case LENLENS:
                while (state->have < state->ncode) {
                    NEEDBITS(3);
                    state->lens[order[state->have++]] = (unsigned short)BITS(3);
                    DROPBITS(3);
                }
                while (state->have < 19)
                    state->lens[order[state->have++]] = 0;
                state->next = state->codes;
                state->lencode = (strmcode const FAR *)(state->next);
                state->lenbits = 7;
                ret = inflate_table(CODES, state->lens, 19, &(state->next),
                                    &(state->lenbits), state->work);
                if (ret) {
                    strm->msg = (char *)"invalid code lengths set";
                    state->mode = BAD;
                    break;
                }
                Tracev((stderr, "inflate:       code lengths ok\n"));
                state->have = 0;
                state->mode = CODELENS;
    */


    VoC_lw16i_short(REG2,LENLENS,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_LENLENS,REG1,REG2)
Coolsand_inflate_case_LENLENS_Begin:

    VoC_lbinc_p(REG0);
    VoC_lw16d_set_inc(REG2,state_have,1);
    VoC_lw16i(REG1,order);
    VoC_lw16i(REG5,state_lens);

Coolsand_inflate_case_LENLENS_L0:
    VoC_bnlt16_rd(Coolsand_inflate_case_LENLENS_L1,REG2,state_ncode)

    VoC_add16_rr(REG3,REG2,REG1,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);

    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX3,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX3:

    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_lw16_p(REG3,REG3,IN_PARALLEL);


    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);

    VoC_sw16_p(REG6,REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_jump(Coolsand_inflate_case_LENLENS_L0);

Coolsand_inflate_case_LENLENS_L1:


    VoC_lw16i_short(REG7,19,DEFAULT);

Coolsand_inflate_case_LENLENS_L2:

    VoC_bngt16_rr(Coolsand_inflate_case_LENLENS_L3,REG7,REG2)

    VoC_add16_rr(REG3,REG2,REG1,DEFAULT);

    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_lw16inc_p(REG3,REG3,DEFAULT);

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);

    VoC_NOP();

    VoC_sw16_p(REG4,REG3,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);

    VoC_jump(Coolsand_inflate_case_LENLENS_L2);

Coolsand_inflate_case_LENLENS_L3:


    VoC_lw16i_short(REG1,CODES,DEFAULT);
    VoC_lw16i_short(REG5,7,IN_PARALLEL);

    VoC_lw16i(REG2,state_lens);

    VoC_lw16i_short(REG3,19,DEFAULT);

    VoC_lw16_d(REG4,state_next);

    VoC_lw16i(REG6,state_work);

    VoC_sw16_d(REG5,state_lenbits);
    VoC_sw16_d(REG4,state_lencode);

    VoC_sw16_d(REG1,inflate_table_type);
    VoC_sw16_d(REG2,inflate_table_lens);
    VoC_sw16_d(REG3,inflate_table_codes);
    VoC_sw16_d(REG4,inflate_table_table);
    VoC_sw16_d(REG5,inflate_table_bits);
    VoC_sw16_d(REG6,inflate_table_work);


    VoC_jal(Coolsand_inflate_table);

    VoC_sw16_d(REG4,state_next);
    VoC_sw16_d(REG5,state_lenbits);

    VoC_bez16_r(Coolsand_inflate_case_LENLENS_L4,REG7)
    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_LENLENS_L4:


    VoC_lw16i_short(REG2,0,DEFAULT);

    VoC_lw16i_short(REG1,CODELENS,DEFAULT);

    VoC_sw16_d(REG2,state_have);

    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_CODELENS_Begin);
Coolsand_inflate_case_LENLENS:

    /*        case CODELENS:
                while (state->have < state->nlen + state->ndist) {
                    for (;;) {
                        this = state->lencode[BITS(state->lenbits)];
                        if ((unsigned)(this.bits) <= bits) break;
                        PULLBYTE();
                    }
                    if (this.val < 16) {
                        NEEDBITS(this.bits);
                        DROPBITS(this.bits);
                        state->lens[state->have++] = this.val;
                    }
                    else {
                        if (this.val == 16) {
                            NEEDBITS(this.bits + 2);
                            DROPBITS(this.bits);
                            if (state->have == 0) {
                                strm->msg = (char *)"invalid bit length repeat";
                                state->mode = BAD;
                                break;
                            }
                            len = state->lens[state->have - 1];
                            copy = 3 + BITS(2);
                            DROPBITS(2);
                        }
                        else if (this.val == 17) {
                            NEEDBITS(this.bits + 3);
                            DROPBITS(this.bits);
                            len = 0;
                            copy = 3 + BITS(3);
                            DROPBITS(3);
                        }
                        else {
                            NEEDBITS(this.bits + 7);
                            DROPBITS(this.bits);
                            len = 0;
                            copy = 11 + BITS(7);
                            DROPBITS(7);
                        }
                        if (state->have + copy > state->nlen + state->ndist) {
                            strm->msg = (char *)"invalid bit length repeat";
                            state->mode = BAD;
                            break;
                        }
                        while (copy--)
                            state->lens[state->have++] = (unsigned short)len;
                    }
                }

                // handle error breaks in while
                if (state->mode == BAD) break;

               // build code tables
                state->next = state->codes;
                state->lencode = (strmcode const FAR *)(state->next);
                state->lenbits = 9;
                ret = inflate_table(LENS, state->lens, state->nlen, &(state->next),
                                    &(state->lenbits), state->work);
                if (ret) {
                    strm->msg = (char *)"invalid literal/lengths set";
                    state->mode = BAD;
                    break;
                }
                state->distcode = (strmcode const FAR *)(state->next);
                state->distbits = 6;
                ret = inflate_table(DISTS, state->lens + state->nlen, state->ndist,
                                &(state->next), &(state->distbits), state->work);
                if (ret) {
                    strm->msg = (char *)"invalid distances set";
                    state->mode = BAD;
                    break;
                }
                Tracev((stderr, "inflate:       codes ok\n"));
                state->mode = LEN;

            */

    VoC_lw16i_short(REG2,CODELENS,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_CODELENS,REG1,REG2)
Coolsand_inflate_case_CODELENS_Begin:


    VoC_lw16_d(REG2,state_ndist);
    VoC_add16_rd(REG2,REG2,state_nlen);

    VoC_lw16_d(REG3,state_have);

    VoC_bngt16_rr(Coolsand_inflate_case_CODELENS_L8,REG2,REG3)

    VoC_lw16_d(REG7,state_lenbits);

    VoC_lw16i(REG2,state_codes_bits);
    VoC_lw16i(REG3,state_codes_val);

    VoC_add16_rd(REG2,REG2,state_lencode);
    VoC_add16_rd(REG3,REG3,state_lencode);

    VoC_loop_i_short(4,DEFAULT)
    VoC_movreg16(REG1,REG7,IN_PARALLEL)
    VoC_startloop0

    VoC_jal(Coolsand_BITS);

    VoC_add16_rr(REG4,REG2,REG6,DEFAULT);

    VoC_add16_rr(REG5,REG3,REG6,DEFAULT);

    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_movreg16(REG7,REG1,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_inflate_case_CODELENS_L0,REG4,state_bits);

    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);

    VoC_jal(Coolsand_PULLBYTE);

    VoC_endloop0

Coolsand_inflate_case_CODELENS_L0:



    VoC_lw16_p(REG5,REG5,DEFAULT);//this.val;
    VoC_lw16i_short(REG6,16,IN_PARALLEL);

    VoC_bngt16_rr(Coolsand_inflate_case_CODELENS_L1,REG6,REG5)

    VoC_lw16i(REG1,state_lens);

    VoC_lw16d_set_inc(REG2,state_have,1);

    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_movreg16(REG7,REG4,IN_PARALLEL);

    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX4,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX4:

    VoC_inc_p(REG2,DEFAULT);

    VoC_sw16_p(REG5,REG1,DEFAULT);
    VoC_movreg16(REG7,REG4,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);

    VoC_sw16_d(REG2,state_have);

    VoC_jump(Coolsand_inflate_case_CODELENS_Begin);

Coolsand_inflate_case_CODELENS_L1:


    VoC_bne16_rr(Coolsand_inflate_case_CODELENS_L3,REG6,REG5)

    VoC_lw16i_short(REG7,2,DEFAULT);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX5,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX5:

    VoC_movreg16(REG7,REG4,DEFAULT);

    VoC_jal(Coolsand_DROPBITS);

    VoC_bnez16_d(Coolsand_inflate_case_CODELENS_L2,state_have)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);

    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_CODELENS_L2:
    VoC_lw16i(REG1,state_lens-1);

    VoC_add16_rd(REG1,REG1,state_have);

    VoC_lw16i_short(REG7,2,DEFAULT);
    VoC_lw16i_short(REG2,3,IN_PARALLEL);

    VoC_jal(Coolsand_BITS);

    VoC_lw16_p(REG3,REG1,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);

    VoC_lw16i_short(REG7,2,DEFAULT);

    VoC_jal(Coolsand_DROPBITS);

    VoC_sw16_d(REG3,inflate_table_len);

    VoC_sw16_d(REG2,inflate_table_copy);

    VoC_jump(Coolsand_inflate_case_CODELENS_L5);
Coolsand_inflate_case_CODELENS_L3:


    VoC_lw16i_short(REG6,17,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);

    VoC_bne16_rr(Coolsand_inflate_case_CODELENS_L4,REG6,REG5)

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX6,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX6:
    VoC_movreg16(REG7,REG4,DEFAULT);

    VoC_jal(Coolsand_DROPBITS);

    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_lw16i_short(REG2,3,IN_PARALLEL);

    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,3,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);

    VoC_sw16_d(REG3,inflate_table_len);

    VoC_sw16_d(REG2,inflate_table_copy);

    VoC_jump(Coolsand_inflate_case_CODELENS_L5);
Coolsand_inflate_case_CODELENS_L4:

    VoC_lw16i_short(REG3,7,DEFAULT);

    VoC_add16_rr(REG7,REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX7,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX7:

    VoC_movreg16(REG7,REG4,DEFAULT);

    VoC_jal(Coolsand_DROPBITS);

    VoC_lw16i_short(REG7,7,DEFAULT);
    VoC_lw16i_short(REG2,11,IN_PARALLEL);

    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,7,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG6,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);

    VoC_sw16_d(REG3,inflate_table_len);

    VoC_sw16_d(REG2,inflate_table_copy);
Coolsand_inflate_case_CODELENS_L5:


    VoC_lw16_d(REG1,state_have);
    VoC_lw16_d(REG2,state_nlen);

    VoC_add16_rd(REG1,REG1,inflate_table_copy);
    VoC_add16_rd(REG2,REG2,state_ndist);

    VoC_bngt16_rr(Coolsand_inflate_case_CODELENS_L6,REG1,REG2)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);

    VoC_lw16i_short(REG1,BAD,DEFAULT);

    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_CODELENS_L6:

    VoC_lw16_d(REG1,inflate_table_copy);

    VoC_lw16i_set_inc(REG2,state_lens,1);

    VoC_add16_rd(REG3,REG1,state_have);

    VoC_add16_rd(REG2,REG2,state_have);

    VoC_lw16_d(REG6,inflate_table_len);

    VoC_sw16_d(REG3,state_have);

    VoC_loop_r_short(REG1,DEFAULT)
    VoC_startloop0
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_inflate_case_CODELENS_Begin);

Coolsand_inflate_case_CODELENS_L8:


    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_lw16i_short(REG1,LENS,DEFAULT);
    VoC_lw16i_short(REG5,9,IN_PARALLEL);

    VoC_lw16i(REG2,state_lens);

    VoC_lw16_d(REG3,state_nlen);

    VoC_lw16i(REG6,state_work);

    VoC_sw16_d(REG5,state_lenbits);
    VoC_sw16_d(REG4,state_lencode);
    VoC_sw16_d(REG4,state_next);

    VoC_sw16_d(REG1,inflate_table_type);
    VoC_sw16_d(REG2,inflate_table_lens);
    VoC_sw16_d(REG3,inflate_table_codes);
    VoC_sw16_d(REG4,inflate_table_table);
    VoC_sw16_d(REG5,inflate_table_bits);
    VoC_sw16_d(REG6,inflate_table_work);


    VoC_jal(Coolsand_inflate_table);

    VoC_sw16_d(REG4,state_next);
    VoC_sw16_d(REG5,state_lenbits);

    VoC_bez16_r(Coolsand_inflate_case_CODELENS_L9,REG7)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_CODELENS_L9:


    VoC_lw16i_short(REG1,DISTS,DEFAULT);
    VoC_lw16i_short(REG5,6,IN_PARALLEL);

    VoC_lw16i(REG2,state_lens);

    VoC_lw16_d(REG3,state_ndist);

    VoC_add16_rd(REG2,REG2,state_nlen);

    VoC_lw16_d(REG4,state_next);

    VoC_lw16i(REG6,state_work);

    VoC_sw16_d(REG5,state_distbits);
    VoC_sw16_d(REG4,state_distcode);

    VoC_sw16_d(REG1,inflate_table_type);
    VoC_sw16_d(REG2,inflate_table_lens);
    VoC_sw16_d(REG3,inflate_table_codes);
    VoC_sw16_d(REG4,inflate_table_table);
    VoC_sw16_d(REG5,inflate_table_bits);
    VoC_sw16_d(REG6,inflate_table_work);


    VoC_jal(Coolsand_inflate_table);

    VoC_sw16_d(REG4,state_next);
    VoC_sw16_d(REG5,state_distbits);

    VoC_bez16_r(Coolsand_inflate_case_CODELENS_L10,REG7)
    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_CODELENS_L10:

    VoC_lw16i_short(REG1,LEN,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_LEN_Begin);

Coolsand_inflate_case_CODELENS:

    /*
            case LEN:
                if (have >= 6 && left >= 258)
                {
                    RESTORE();
                  inflate_fast(strm, out);
                    LOAD();
                    break;
                }

                for (;;)
                {
                    this = state->lencode[BITS(state->lenbits)];
                    if ((unsigned)(this.bits) <= bits) break;
                    PULLBYTE();
                }

                if (this.op && (this.op & 0xf0) == 0)
                {
                    last = this;
                    for (;;)
                    {
                        this = state->lencode[last.val + (BITS(last.bits + last.op) >> last.bits)];
                        if ((unsigned)(last.bits + this.bits) <= bits) break;
                        PULLBYTE();
                    }
                    DROPBITS(last.bits);
                }

                DROPBITS(this.bits);
                state->length = (unsigned)this.val;

                if ((int)(this.op) == 0)
                {

                  //  Tracevv((stderr, this.val >= 0x20 && this.val < 0x7f ?
                   //         "inflate:         literal '%c'\n" :
                   //         "inflate:         literal 0x%02x\n", this.val));

                    state->mode = LIT;
                    break;
                }
                if (this.op & 32)
                {
                    Tracevv((stderr, "inflate:         end of block\n"));
                    state->mode = TYPE;
                    break;
                }
                if (this.op & 64)
                {
                    strm->msg = (char *)"invalid literal/length code";
                    state->mode = BAD;
                    break;
                }
                state->extra = (unsigned)(this.op) & 15;
                state->mode = LENEXT;

    */
    VoC_lw16i_short(REG2,LEN,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_LEN,REG1,REG2)
Coolsand_inflate_case_LEN_Begin:

    VoC_lw16i_short(REG1,6,DEFAULT);
    VoC_lw16i(REG2,258);

    VoC_bgt16_rd(Coolsand_inflate_case_LEN_L0,REG2,PNGzstream_avail_out)
    VoC_bgt16_rd(Coolsand_inflate_case_LEN_L0,REG1,PNGzstream_avail_in)
    VoC_lw16_d(REG6,inflate_out);
    VoC_jal(Coolsand_inflate_fast);
    VoC_lw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_main_loop);
Coolsand_inflate_case_LEN_L0:

// VoC_directive: PARSER_OFF
    printf("inflate_fast_count:%d\r",inflate_fast_count);

    if (inflate_fast_count==7)
    {
        printf("ok\n");
    }

    inflate_fast_count++;

// VoC_directive: PARSER_ON

    VoC_jal(Coolsand_DmaInData);

    VoC_lw16_d(REG7,state_lenbits);

    VoC_lw16_d(REG4,state_lencode);

    VoC_add16_rd(REG1,REG4,state_codes_bits_addr);
    VoC_add16_rd(REG2,REG4,state_codes_op_addr);
    VoC_add16_rd(REG3,REG4,state_codes_val_addr);


    VoC_loop_i(0,4);

    //  VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_movreg16(REG6,RL7_LO,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG7,REG4,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG7,IN_PARALLEL);

    VoC_shru16_rr(REG6,REG5,DEFAULT);

    VoC_shru16_rr(REG6,REG4,DEFAULT);

    VoC_add16_rr(REG4,REG1,REG6,DEFAULT);

    VoC_add16_rr(REG5,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG6,IN_PARALLEL);

    VoC_lw16_p(REG4,REG4,DEFAULT);

    VoC_bngt16_rd(Coolsand_inflate_case_LEN_L2,REG4,state_bits);

    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);

    //VoC_jal(Coolsand_PULLBYTE);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub16_rd(REG6,REG5,state_bits);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);

    VoC_add16_rd(REG6,REG6,state_bits);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

    VoC_endloop0

Coolsand_inflate_case_LEN_L2:

    VoC_lw16_p(REG5,REG5,DEFAULT);//this->op.

    VoC_lw16_p(REG6,REG6,DEFAULT);//this->val
    VoC_movreg16(REG1,REG5,IN_PARALLEL);

    VoC_bez16_r(Coolsand_inflate_case_LEN_L5,REG1)
    VoC_and16_ri(REG1,0xf0);
    VoC_bnez16_r(Coolsand_inflate_case_LEN_L5,REG1)

    VoC_movreg16(ACC0_LO,REG4,DEFAULT);//last.bit
    VoC_add16_rr(REG7,REG4,REG5,IN_PARALLEL);//last.bits + last.op

    VoC_add16_rd(REG4,REG6,state_lencode);

    VoC_add16_rd(REG1,REG4,state_codes_bits_addr);
    VoC_add16_rd(REG2,REG4,state_codes_op_addr);
    VoC_add16_rd(REG3,REG4,state_codes_val_addr);

    VoC_loop_i_short(4,DEFAULT)
    VoC_movreg16(ACC0_HI,REG7,IN_PARALLEL);//last.bits + last.op
    VoC_startloop0

    VoC_movreg16(REG5,ACC0_LO,DEFAULT);//last.bit
    VoC_movreg16(REG7,ACC0_HI,IN_PARALLEL);///last.bits + last.op

    VoC_jal(Coolsand_BITS);

    VoC_shr16_rr(REG6,REG5,DEFAULT);

    VoC_add16_rr(REG4,REG1,REG6,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);//last.bit

    VoC_add16_rr(REG5,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG6,IN_PARALLEL);

    VoC_add16_rp(REG7,REG7,REG4,DEFAULT);

    VoC_bngt16_rd(Coolsand_inflate_case_LEN_L4,REG7,state_bits);

    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);

    VoC_jal(Coolsand_PULLBYTE);

    VoC_endloop0

Coolsand_inflate_case_LEN_L4:

    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);

    //  VoC_jal(Coolsand_DROPBITS);

    VoC_shru32_rr(RL7,REG7,DEFAULT);
    VoC_lw16_p(REG5,REG5,IN_PARALLEL);

    VoC_sub16_dr(REG7,state_bits,REG7);

    VoC_lw16_p(REG6,REG6,DEFAULT);

    VoC_sw16_d(REG7,state_bits);


Coolsand_inflate_case_LEN_L5:

    //  VoC_jal(Coolsand_DROPBITS);

    VoC_shru32_rr(RL7,REG4,DEFAULT);
    VoC_lw16i_short(REG1,LIT,IN_PARALLEL);

    VoC_sub16_dr(REG7,state_bits,REG4);

    VoC_sw16_d(REG6,state_length);

    VoC_sw16_d(REG7,state_bits);

    VoC_bnez16_r(Coolsand_inflate_case_LEN_L6,REG5)
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_LIT_Begin);
Coolsand_inflate_case_LEN_L6:

    VoC_movreg16(REG6,REG5,DEFAULT);
    VoC_lw16i_short(REG1,TYPE,IN_PARALLEL);

    VoC_and16_ri(REG6,32);

    VoC_bez16_r(Coolsand_inflate_case_LEN_L7,REG6)
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_TYPE_Begin);

Coolsand_inflate_case_LEN_L7:

    VoC_movreg16(REG6,REG5,DEFAULT);

    VoC_and16_ri(REG6,64);

    VoC_bez16_r(Coolsand_inflate_case_LEN_L8,REG6)
    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_LEN_L8:

    VoC_and16_ri(REG5,15);
    VoC_lw16i_short(REG1,LENEXT,DEFAULT);
    VoC_sw16_d(REG5,state_extra);
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_LENEXT_Begin);
Coolsand_inflate_case_LEN:

    /*       case LENEXT:
               if (state->extra)
           {
                   NEEDBITS(state->extra);
                   state->length += BITS(state->extra);
                   DROPBITS(state->extra);
               }
               //Tracevv((stderr, "inflate:         length %u\n", state->length));
               state->mode = DIST;
    */

    VoC_lw16i_short(REG2,LENEXT,DEFAULT);
    VoC_bne16_rr(Coolsand_inflate_case_LENEXT,REG1,REG2)
Coolsand_inflate_case_LENEXT_Begin:
    VoC_bez16_d(Coolsand_inflate_case_LENEXT_L0,state_extra)


    VoC_lw16_d(REG7,state_extra);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX8,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX8:

    VoC_lw16_d(REG7,state_extra);
    VoC_jal(Coolsand_BITS);

    VoC_add16_rd(REG6,REG6,state_length);
    VoC_lw16_d(REG7,state_extra);
    VoC_sw16_d(REG6,state_length);
    VoC_jal(Coolsand_DROPBITS);

Coolsand_inflate_case_LENEXT_L0:

    VoC_lw16i(REG1,DIST);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_DIST_Begin)
Coolsand_inflate_case_LENEXT:

    /*

              case DIST:
                for (;;)
                {
                    this = state->distcode[BITS(state->distbits)];
                    if ((unsigned)(this.bits) <= bits) break;
                    PULLBYTE();
                }
                if ((this.op & 0xf0) == 0)
                {
                    last = this;
                    for (;;)
                    {
                        this = state->distcode[last.val +(BITS(last.bits + last.op) >> last.bits)];
                        if ((unsigned)(last.bits + this.bits) <= bits) break;
                        PULLBYTE();
                    }
                    DROPBITS(last.bits);
                }
                DROPBITS(this.bits);
                if (this.op & 64) {
                    strm->msg = (char *)"invalid distance code";
                    state->mode = BAD;
                    break;
                }
                state->offset = (unsigned)this.val;
                state->extra = (unsigned)(this.op) & 15;
                state->mode = DISTEXT;
    */

    VoC_lw16i_short(REG2,DIST,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_DIST,REG1,REG2)
Coolsand_inflate_case_DIST_Begin:

    VoC_lw16_d(ACC0_LO,state_distbits);

    VoC_lw16i(REG1,state_codes_bits);
    VoC_lw16i(REG2,state_codes_op);
    VoC_lw16i(REG3,state_codes_val);

    VoC_add16_rd(REG1,REG1,state_distcode);
    VoC_add16_rd(REG2,REG2,state_distcode);
    VoC_add16_rd(REG3,REG3,state_distcode);

    VoC_loop_i_short(4,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);
    VoC_startloop0

    VoC_jal(Coolsand_BITS);

    VoC_add16_rr(REG4,REG1,REG6,DEFAULT);

    VoC_add16_rr(REG5,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG6,IN_PARALLEL);

    VoC_lw16_p(REG4,REG4,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_inflate_case_DIST_L0,REG4,state_bits);

    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);

    VoC_jal(Coolsand_PULLBYTE);

    VoC_endloop0

Coolsand_inflate_case_DIST_L0:

    VoC_lw16_p(REG5,REG5,DEFAULT);//this->op.

    VoC_lw16_p(REG6,REG6,DEFAULT);//this->val
    VoC_movreg16(REG1,REG5,IN_PARALLEL);

    VoC_and16_ri(REG1,0xf0);
    VoC_bnez16_r(Coolsand_inflate_case_DIST_L2,REG1)

    VoC_movreg16(ACC0_LO,REG4,DEFAULT);//last.bit
    VoC_add16_rr(REG7,REG4,REG5,IN_PARALLEL);//last.bits + last.op

    VoC_add16_rd(REG4,REG6,state_distcode);

    VoC_add16_rd(REG1,REG4,state_codes_bits_addr);
    VoC_add16_rd(REG2,REG4,state_codes_op_addr);
    VoC_add16_rd(REG3,REG4,state_codes_val_addr);


    VoC_loop_i_short(4,DEFAULT)
    VoC_movreg16(ACC0_HI,REG7,IN_PARALLEL);//last.bits + last.op

    VoC_startloop0

    VoC_movreg16(REG5,ACC0_LO,DEFAULT);//last.bit
    VoC_movreg16(REG7,ACC0_HI,IN_PARALLEL);///last.bits + last.op

    VoC_jal(Coolsand_BITS);

    VoC_shr16_rr(REG6,REG5,DEFAULT);

    VoC_add16_rr(REG4,REG1,REG6,DEFAULT);
    VoC_movreg16(REG7,ACC0_LO,DEFAULT);//last.bit

    VoC_add16_rr(REG5,REG2,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG3,REG6,IN_PARALLEL);

    VoC_add16_rp(REG7,REG7,REG4,DEFAULT);

    VoC_bngt16_rd(Coolsand_inflate_case_DIST_L1,REG7,state_bits);

    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);

    VoC_jal(Coolsand_PULLBYTE);

    VoC_endloop0

Coolsand_inflate_case_DIST_L1:


    VoC_movreg16(REG7,ACC0_LO,DEFAULT);
    VoC_lw16_p(REG4,REG4,IN_PARALLEL);

    VoC_jal(Coolsand_DROPBITS);

    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);

Coolsand_inflate_case_DIST_L2:

    VoC_movreg16(REG7,REG4,DEFAULT);

    VoC_jal(Coolsand_DROPBITS);

    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_and16_ri(REG7,64);

    VoC_bez16_r(Coolsand_inflate_case_DIST_L3,REG7)
    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_DIST_L3:

    VoC_movreg16(REG2,REG6,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_and16_ri(REG5,15);
    VoC_sw32_d(REG23,state_offset);
    VoC_sw16_d(REG5,state_extra);
    VoC_lw16i(REG1,DISTEXT);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_DISTEXT_Begin);

Coolsand_inflate_case_DIST:
    /*

              case DISTEXT:
                if (state->extra)
                {
                    NEEDBITS(state->extra);
                    state->offset += BITS(state->extra);
                    DROPBITS(state->extra);
                }
    #ifdef INFLATE_STRICT
                if (state->offset > state->dmax)
                {
                    strm->msg = (char *)"invalid distance too far back";
                    state->mode = BAD;
                    break;
                }
    #endif
                if (state->offset > state->whave + out - left)
                {
                    strm->msg = (char *)"invalid distance too far back";
                    state->mode = BAD;
                    break;
                }
                //Tracevv((stderr, "inflate:         distance %u\n", state->offset));
                state->mode = MATCH;

      */


    VoC_lw16i_short(REG2,DISTEXT,DEFAULT);
    VoC_bne16_rr(Coolsand_inflate_case_DISTEXT,REG1,REG2)
Coolsand_inflate_case_DISTEXT_Begin:
    VoC_bez16_d(Coolsand_inflate_case_DISTEXT_L0,state_extra)



    VoC_lw16_d(REG7,state_extra);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX9,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX9:

    VoC_lw16_d(REG7,state_extra);
    VoC_jal(Coolsand_BITS);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_add32_rd(REG67,REG67,state_offset);
    VoC_lw16_d(REG7,state_extra);
    VoC_sw32_d(REG67,state_offset);
    VoC_jal(Coolsand_DROPBITS);

Coolsand_inflate_case_DISTEXT_L0:

    VoC_lw32_d(ACC0,state_whave);

    VoC_add32_rd(ACC0,ACC0,inflate_out);

    VoC_sub32_rd(ACC0,ACC0,PNGzstream_avail_out);

    VoC_bnlt32_rd(Coolsand_inflate_case_DISTEXT_L1,ACC0,state_offset)

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);
    VoC_lw16i_short(REG1,BAD,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);

    VoC_jump(Coolsand_inflate_main_inf_leave);
Coolsand_inflate_case_DISTEXT_L1:

    VoC_lw16i(REG1,MATCH);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);
    VoC_jump(Coolsand_inflate_case_MATCH_Begin);
Coolsand_inflate_case_DISTEXT:


    /*
              case MATCH:
                if (left == 0) goto inf_leave;
                copy = out - left;
                if (state->offset > copy)
                {        // copy from window
                    copy = state->offset - copy;
                    if (copy > state->write)
                    {
                        copy -= state->write;
                        from = state->window + (state->wsize - copy);
                    }
                    else
                        from = state->window + (state->write - copy);
                    if (copy > state->length) copy = state->length;
                }
                else
                {                             // copy from output
                    from = put - state->offset;
                    copy = state->length;
                }
                if (copy > left) copy = left;
                left -= copy;
                state->length -= copy;
                do
                {
                    *put++ = *from++;
                } while (--copy);
                if (state->length == 0) state->mode = LEN;
                break;
    */

    VoC_lw16i_short(REG2,MATCH,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_MATCH,REG1,REG2)
Coolsand_inflate_case_MATCH_Begin:

    VoC_lw32_d(ACC0,PNGzstream_avail_out);
    VoC_lw16i(REG1,MATCH);

    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC0)

    VoC_lw32_d(ACC0,inflate_out);

    VoC_sub32_rd(ACC0,ACC0,PNGzstream_avail_out);

    VoC_lw32_d(ACC1,state_offset);

    VoC_bngt32_rr(Coolsand_inflate_case_MATCH_L0,ACC1,ACC0);


    /* copy from window */
    // copy = state->offset - copy;

    VoC_sub32_rr(REG67,ACC1,ACC0,DEFAULT);

    VoC_bngt32_rd(Coolsand_inflate_case_MATCH_Window_L0,REG67,state_write)

    VoC_sub32_rd(REG67,REG67,state_write);

    VoC_sub32_dr(ACC0,state_wsize,REG67);

    VoC_shr32_ri(ACC0,-1,DEFAULT);

    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_jump(Coolsand_inflate_case_MATCH_Window_L1)
Coolsand_inflate_case_MATCH_Window_L0:

    VoC_sub32_dr(ACC0,state_write,REG67);

    VoC_shr32_ri(ACC0,-1,DEFAULT);

    VoC_add32_rd(ACC0,ACC0,state_window);

Coolsand_inflate_case_MATCH_Window_L1:

    VoC_lw16i_short(ACC1_HI,0,DEFAULT);
    VoC_lw16_d(ACC1_LO,state_length);


    VoC_bngt32_rr(Coolsand_inflate_case_MATCH_Window_L2,REG67,ACC1)

    VoC_movreg32(REG67,ACC1,DEFAULT);

Coolsand_inflate_case_MATCH_Window_L2:


    VoC_bngt32_rd(Coolsand_inflate_case_MATCH_Window_L3,REG67,PNGzstream_avail_out)

    VoC_lw32_d(REG67,PNGzstream_avail_out);

Coolsand_inflate_case_MATCH_Window_L3:

    VoC_sub32_dr(ACC1,PNGzstream_avail_out,REG67);
    VoC_sub16_dr(REG2,state_length,REG6);
    VoC_sw32_d(ACC1,PNGzstream_avail_out);
    VoC_sw16_d(REG2,state_length);
    VoC_lw16d_set_inc(REG1,GLOBAL_RawOutBufAddr,1);

    VoC_sw32_d(REG67,zmemcpy_Length);
    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);

    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);

    VoC_sw16_d(REG1,GLOBAL_RawOutBufAddr);


    VoC_jump(Coolsand_inflate_case_MATCH_LX);

Coolsand_inflate_case_MATCH_L0:

    VoC_lw16d_set_inc(REG1,GLOBAL_RawOutBufAddr,1);

    VoC_lw16_d(REG4,state_length);

    VoC_lw32_d(REG67,state_offset);

    VoC_lw16i_short(REG5,0,DEFAULT);

    VoC_bngt32_rd(Coolsand_inflate_case_MATCH_L1,REG45,PNGzstream_avail_out)

    VoC_lw32_d(REG45,PNGzstream_avail_out);

Coolsand_inflate_case_MATCH_L1:

    VoC_sub32_dr(ACC0,PNGzstream_avail_out,REG45);

    VoC_sub16_dr(REG2,state_length,REG4);

    VoC_sw32_d(ACC0,PNGzstream_avail_out);

    VoC_sw16_d(REG2,state_length);

    //REG67:dist
    //REG4:length

    VoC_jal(Coolsand_CopyBytesFromOutBuf);

    VoC_sw16_d(REG1,GLOBAL_RawOutBufAddr);


Coolsand_inflate_case_MATCH_LX:

    VoC_lw16i(REG1,MATCH);

    VoC_bnez16_d(Coolsand_inflate_case_MATCH_LX_0,state_length)

    VoC_lw16i(REG1,LEN);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_LEN_Begin);

Coolsand_inflate_case_MATCH_LX_0:

    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_MATCH_Begin);

Coolsand_inflate_case_MATCH:

    /*
            case LIT:
                if (left == 0) goto inf_leave;
                *put++ = (unsigned char)(state->length);
                left--;
                state->mode = LEN;
                break;
    */

    VoC_lw16i_short(REG2,LIT,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_LIT,REG1,REG2)
Coolsand_inflate_case_LIT_Begin:

    VoC_lw32_d(ACC0,PNGzstream_avail_out)
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC0);

    VoC_lw16d_set_inc(REG1,GLOBAL_RawOutBufAddr,1);

    VoC_lw16_d(REG2,state_length);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sw16inc_p(REG2,REG1,DEFAULT);
    VoC_sub32_rr(ACC0,ACC0,REG45,IN_PARALLEL);

    VoC_sw16_d(REG1,GLOBAL_RawOutBufAddr);

    VoC_lw16i_short(REG1,LEN,DEFAULT);

    VoC_sw32_d(ACC0,PNGzstream_avail_out);

    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_LEN_Begin);


Coolsand_inflate_case_LIT:

    /*        case CHECK:
                if (state->wrap)
                {
                    NEEDBITS(32);
                    out -= left;
                    strm->total_out += out;
                    state->total += out;
                    if (out)
                        strm->adler = state->check =
                            UPDATE(state->check, put - out, out);
                    out = left;
                    if ((
    #ifdef GUNZIP
                         state->flags ? hold :
    #endif
                         REVERSE(hold)) != state->check) {
                        strm->msg = (char *)"incorrect data check";
                        state->mode = BAD;
                        break;
                    }
                    INITBITS();
                    Tracev((stderr, "inflate:   check matches trailer\n"));
                }
    #ifdef GUNZIP
                state->mode = LENGTH;
    */

    VoC_lw16i_short(REG2,CHECK,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_CHECK,REG1,REG2)
Coolsand_inflate_case_CHECK_Begin:

    VoC_bez16_d(Coolsand_inflate_case_CHECK_L0,state_wrap)
    VoC_lw16i(REG7,32);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX10,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX10:

    /*               out -= left;
                    strm->total_out += out;
                    state->total += out;
    */
    VoC_lw32_d(ACC0,PNGzstream_avail_out);
    VoC_sub32_dr(ACC1,inflate_out,ACC0);
    VoC_add32_rd(REG45,ACC1,PNGzstream_total_out);
    VoC_add32_rd(REG67,ACC1,state_total);
    VoC_sw32_d(ACC0,inflate_out);
    VoC_sw32_d(REG45,PNGzstream_total_out);
    VoC_sw32_d(REG67,state_total);

    VoC_jal(Coolsand_INITBITS);
Coolsand_inflate_case_CHECK_L0:

    VoC_lw16i(REG1,LENGTH);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_LENGTH_Begin)
Coolsand_inflate_case_CHECK:


    /*
            case LENGTH:
            if (state->wrap && state->flags)
            {
                NEEDBITS(32);
                if (hold != (state->total & 0xffffffffUL))
                {
                    strm->msg = (char *)"incorrect length check";
                    state->mode = BAD;
                    break;
                }
                INITBITS();
                Tracev((stderr, "inflate:   length matches trailer\n"));
            }
    #endif
            state->mode = DONE;

    */


    VoC_lw16i_short(REG2,LENGTH,DEFAULT);

    VoC_bne16_rr(Coolsand_inflate_case_LENGTH,REG1,REG2)
Coolsand_inflate_case_LENGTH_Begin:

    VoC_bez16_d(Coolsand_inflate_case_LENGTH_L0,state_wrap)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_lw16i(REG1,DISTEXT);
    VoC_bez32_r(Coolsand_inflate_case_LENGTH_L0,ACC1);

    VoC_lw16i(REG7,32);
    VoC_jal(Coolsand_NEEDBITS);

    VoC_bngt16_rd(Coolsand_NEEDBITS_LX11,REG7,state_bits)
    VoC_lw32_d(ACC1,PNGzstream_avail_in);
    VoC_bez32_r(Coolsand_inflate_main_inf_leave,ACC1);
Coolsand_NEEDBITS_LX11:

    VoC_jal(Coolsand_INITBITS);

Coolsand_inflate_case_LENGTH_L0:

    VoC_lw16i(REG1,DONE);
    VoC_NOP();
    VoC_sw16_d(REG1,state_mode);

    VoC_jump(Coolsand_inflate_case_DONE_Begin)
Coolsand_inflate_case_LENGTH:

    /*        case DONE:
                ret = Z_STREAM_END;
                goto inf_leave;
            case BAD:
                ret = Z_DATA_ERROR;
                goto inf_leave;
            case MEM:
                return Z_MEM_ERROR;
            case SYNC:
            default:
                return Z_STREAM_ERROR;

    */

    VoC_lw16i_short(REG2,DONE,DEFAULT);
    VoC_bne16_rr(Coolsand_inflate_case_DONE,REG1,REG2)
Coolsand_inflate_case_DONE_Begin:
    VoC_lw16i(REG7,Z_STREAM_END);
    VoC_lw16i(REG1,DONE);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave)
Coolsand_inflate_case_DONE:

    VoC_lw16i_short(REG2,BAD,DEFAULT);
    VoC_bne16_rr(Coolsand_inflate_case_BAD,REG1,REG2)
//Coolsand_inflate_case_BAD_Begin:
    VoC_lw16i(REG7,Z_DATA_ERROR);
    VoC_lw16i(REG1,BAD);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_sw16_d(REG7,state_mode);
    VoC_jump(Coolsand_inflate_main_inf_leave)
Coolsand_inflate_case_BAD:

    VoC_lw16i(REG7,Z_STREAM_ERROR);
    VoC_pop16(RA,DEFAULT);
    VoC_sw16_d(REG7,inflate_ret);
    VoC_return;

Coolsand_inflate_main_inf_leave:

    VoC_sw32_d(RL7,state_hold);

    VoC_bez16_d(Coolsand_inflate_L1,state_wsize)

    VoC_lw16_d(REG6,inflate_out);

    VoC_jal(Coolsand_updatewindow);

    VoC_jump(Coolsand_inflate_L2)
Coolsand_inflate_L1:

    VoC_lw16i(REG2,CHECK);

    VoC_bngt16_rd(Coolsand_inflate_L2,REG2,state_mode)

    VoC_lw32_d(ACC0,inflate_out);

    VoC_be32_rd(Coolsand_inflate_L2,ACC0,PNGzstream_avail_out)

    VoC_lw16_d(REG6,inflate_out);

    VoC_jal(Coolsand_updatewindow);

Coolsand_inflate_L2:



// VoC_directive: PARSER_OFF
#ifdef DEBUG_STEP

    {
        short *PoutbufI=(short*)&RAM_EXTERN[0x40046f];
        int i=0;

        fread(g_Buffer_test,1,RAM_X[state_wsize/2],DebugFileHandle);

        for (i=0; i<RAM_X[state_wsize/2]; i++)
        {
            if(g_Buffer_test[i]!=PoutbufI[i])
            {
                printf("Window upadate ERROR!\n");

                while (1)
                {
                }
            }
        }
    }

#endif

// VoC_directive: PARSER_ON


    VoC_lw16_d(REG7,inflate_ret);

    VoC_lw32_d(ACC0,inflate_in);

    VoC_sub32_rd(ACC0,ACC0,PNGzstream_avail_in);

    VoC_lw32_d(ACC1,inflate_out);

    VoC_sub32_rd(ACC1,ACC1,PNGzstream_avail_out);

    VoC_sw32_d(ACC0,inflate_in);
    VoC_sw32_d(ACC1,inflate_out);

    VoC_add32_rd(REG45,ACC1,state_total);
    VoC_add32_rd(ACC0,ACC0,PNGzstream_total_in);
    VoC_add32_rd(ACC1,ACC1,PNGzstream_total_out);

    VoC_sw32_d(ACC0,PNGzstream_total_in);
    VoC_sw32_d(ACC1,PNGzstream_total_out);
    VoC_sw32_d(REG45,state_total);

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_inflateInit
 *
 * Description: read
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/11/2012
 **************************************************************************************/
void Coolsand_inflateInit(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_lw16i_short(REG2,15,DEFAULT);

    VoC_sw16_d(REG1,state_wrap);

    VoC_sw16_d(REG2,state_wbits);


    VoC_jal(Coolsand_inflateReset);


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_inflateReset
 *
 * Description: read
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/11/2012
 **************************************************************************************/
void Coolsand_inflateReset(void)
{
    VoC_lw16i_short(REG4,HEAD,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sw32_d(ACC0,PNGzstream_total_out);
    VoC_sw32_d(ACC0,PNGzstream_total_in);
    VoC_sw32_d(ACC0,state_total);

    VoC_lw16i(REG2,0x8000);

    VoC_sw16_d(REG4,state_mode);

    VoC_sw32_d(REG23,state_dmax);

    VoC_sw16_d(REG3,state_last);
    VoC_sw16_d(REG3,state_havedict);

    VoC_sw32_d(ACC0,state_wsize);

    VoC_sw32_d(ACC0,state_whave);
    VoC_sw32_d(ACC0,state_write);
    VoC_sw32_d(ACC0,state_hold);
    VoC_sw16_d(REG3,state_bits);


//   state->lencode = state->distcode = state->next = state->codes;

    VoC_sw16_d(REG3,state_lencode);
    VoC_sw16_d(REG3,state_distcode);
    VoC_sw16_d(REG3,state_next);

    VoC_lw16i_short(REG7,Z_OK,DEFAULT);

    VoC_return;

}
/**************************************************************************************
 * Function:    Coolsand_inflate_table
 *
 * Description: read
 *
 * Inputs:
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/11/2012
 **************************************************************************************/
void Coolsand_inflate_table(void)
{




    /* accumulate lengths for codes (assumes lens[] all in 0..MAXBITS) */

    /*
        for (len = 0; len <= MAXBITS; len++)
            count[len] = 0;
        for (sym = 0; sym < codes; sym++)
            count[lens[sym]]++;

    */
    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_lw16i_set_inc(REG1,count,1);

    VoC_loop_i(0,MAXBITS+1);
    VoC_sw16inc_p(REG4,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG1,count,1);
    VoC_lw16d_set_inc(REG2,inflate_table_lens,1);

    VoC_lw16d_set_inc(REG3,inflate_table_codes,-1);

    VoC_lw16i_short(REG6,1,DEFAULT);
Coolsand_inflate_table_L0_0:
    VoC_bez16_r(Coolsand_inflate_table_L0_1,REG3)
    VoC_add16inc_rp(REG4,REG1,REG2,DEFAULT);
    VoC_NOP();
    VoC_add16_rp(REG5,REG6,REG4,DEFAULT);
    VoC_inc_p(REG3,DEFAULT);
    VoC_sw16_p(REG5,REG4,DEFAULT);
    VoC_jump(Coolsand_inflate_table_L0_0);
Coolsand_inflate_table_L0_1:

    /* bound code lengths, force root to be within code lengths */

    /*    root = *bits;
        for (max = MAXBITS; max >= 1; max--)
            if (count[max] != 0) break;
        if (root > max) root = max;
        if (max == 0) {                     // no symbols to code at all
            this.op = (unsigned char)64;    // invalid code marker
            this.bits = (unsigned char)1;
            this.val = (unsigned short)0;
            *(*table)++ = this;             // make a table to force an error
            *(*table)++ = this;
            *bits = 1;
            return 0;     // no symbols, but wait for decoding to report error
        }
        for (min = 1; min <= MAXBITS; min++)
            if (count[min] != 0) break;
        if (root < min) root = min;
    */

    VoC_lw16_d(REG4,inflate_table_bits);

    VoC_lw16i_set_inc(REG1,MAXBITS,-1);
    VoC_lw16i_set_inc(REG2,count+MAXBITS,-1);

    VoC_loop_i(0,MAXBITS);

    VoC_lw16inc_p(REG3,REG2,DEFAULT);

    VoC_bnez16_r(Coolsand_inflate_table_L0,REG3)

    VoC_inc_p(REG1,DEFAULT);

    VoC_endloop0

Coolsand_inflate_table_L0:

    VoC_bngt16_rr(Coolsand_inflate_table_L1,REG4,REG1)
    VoC_movreg16(REG4,REG1,DEFAULT);
Coolsand_inflate_table_L1:
    VoC_sw16_d(REG1,inflate_table_max);

    VoC_bnez16_r(Coolsand_inflate_table_L2,REG1)

    VoC_lw16_d(REG4,inflate_table_table);

    VoC_lw16d_set_inc(REG1,state_codes_op,1);
    VoC_lw16d_set_inc(REG2,state_codes_bits,1);
    VoC_lw16d_set_inc(REG3,state_codes_val,1);


    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,IN_PARALLEL);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG5,64,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG4,IN_PARALLEL);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);

    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sw16_d(REG6,inflate_table_bits)

    VoC_jump(Coolsand_inflate_table_End);
Coolsand_inflate_table_L2:

    VoC_lw16i_set_inc(REG1,1,1);
    VoC_lw16i_set_inc(REG2,count+1,1);

    VoC_loop_i(0,MAXBITS);

    VoC_lw16inc_p(REG3,REG2,DEFAULT);

    VoC_bnez16_r(Coolsand_inflate_table_L3,REG3)

    VoC_inc_p(REG1,DEFAULT);

    VoC_endloop0

Coolsand_inflate_table_L3:

    VoC_bngt16_rr(Coolsand_inflate_table_L4,REG1,REG4)
    VoC_movreg16(REG4,REG1,DEFAULT);
    VoC_NOP();
Coolsand_inflate_table_L4:

    VoC_sw16_d(REG4,inflate_table_root);

    VoC_sw16_d(REG1,inflate_table_min);

    /* check for an over-subscribed or incomplete set of lengths */
    /*   left = 1;
       for (len = 1; len <= MAXBITS; len++) {
           left <<= 1;
           left -= count[len];
           if (left < 0) return -1;        // over-subscribed
       }
       if (left > 0 && (type == CODES || max != 1))
           return -1;                      // incomplete set
    */


    VoC_lw16i_set_inc(REG2,count+1,1);

    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_loop_i(0,MAXBITS)
    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_sub16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);
    VoC_bltz16_r(Coolsand_inflate_table_End,REG4)
    VoC_endloop0


    VoC_sw16_d(REG4,inflate_table_left);

    VoC_bngtz16_r(Coolsand_inflate_table_L6,REG4)

    VoC_lw16i_short(REG5,CODES,DEFAULT);
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);

    VoC_bne16_rd(Coolsand_inflate_table_L5,REG5,inflate_table_type)
    VoC_jump(Coolsand_inflate_table_End);
Coolsand_inflate_table_L5:

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);

    VoC_be16_rd(Coolsand_inflate_table_L6,REG5,inflate_table_max)
    VoC_jump(Coolsand_inflate_table_End);
Coolsand_inflate_table_L6:


    /* generate offsets into symbol table for each length for sorting */
    /* offs[1] = 0;
     for (len = 1; len < MAXBITS; len++)
         offs[len + 1] = offs[len] + count[len];*/

    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_lw16i_set_inc(REG2,offs+1,1);
    VoC_lw16i_set_inc(REG1,count+1,1);

    VoC_sw16_d(REG4,offs+1);

    VoC_loop_i(0,MAXBITS-1)

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_add16inc_rp(REG4,REG4,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG4,REG2,DEFAULT);
    VoC_endloop0

    /* sort symbols by length, by symbol order within each length */
    //for (sym = 0; sym < codes; sym++)
    //  if (lens[sym] != 0) work[offs[lens[sym]]++] = (unsigned short)sym;

    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_lw16d_set_inc(REG1,inflate_table_lens,1);
    VoC_lw16i_set_inc(REG2,0,1);

    VoC_lw16i(REG5,offs);
    VoC_lw16_d(REG6,inflate_table_work);

Coolsand_inflate_table_L7_0:
    VoC_bnlt16_rd(Coolsand_inflate_table_L7_1,REG2,inflate_table_codes)

    VoC_lw16inc_p(REG3,REG1,DEFAULT)

    VoC_bez16_r(Coolsand_inflate_table_L7,REG3)

    VoC_add16_rr(REG3,REG3,REG5,DEFAULT);

    VoC_NOP();

    VoC_add16_rp(REG4,REG6,REG3,DEFAULT);

    VoC_add16_rp(REG4,REG7,REG3,DEFAULT);

    VoC_sw16_p(REG2,REG4,DEFAULT);

    VoC_sw16_p(REG4,REG3,DEFAULT);

Coolsand_inflate_table_L7:

    VoC_inc_p(REG2,DEFAULT);

    VoC_jump(Coolsand_inflate_table_L7_0);

Coolsand_inflate_table_L7_1:


    /* set up for code type */
    /*   switch (type) {
       case CODES:
           base = extra = work;    // dummy value--not used
           end = 19;
           break;
       case LENS:
           base = lbase;
           base -= 257;
           extra = lext;
           extra -= 257;
           end = 256;
           break;
       default:            // DISTS
           base = dbase;
           extra = dext;
           end = -1;


       }
    */

    VoC_lw16i_short(REG4,CODES,DEFAULT);
    VoC_bne16_rd(Coolsand_inflate_table_case_CODES,REG4,inflate_table_type)

    VoC_lw16_d(REG4,inflate_table_work);

    VoC_movreg16(REG5,REG4,DEFAULT);
    VoC_lw16i_short(REG6,19,IN_PARALLEL);

    VoC_jump(Coolsand_inflate_table_case_end)
Coolsand_inflate_table_case_CODES:

    VoC_lw16i_short(REG4,LENS,DEFAULT);
    VoC_bne16_rd(Coolsand_inflate_table_case_LENS,REG4,inflate_table_type)

    VoC_lw16i(REG3,257);

    VoC_lw16i(REG4,lbase);
    VoC_lw16i(REG5,lext);

    VoC_sub16_rr(REG4,REG4,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG3,IN_PARALLEL);

    VoC_lw16i(REG6,256);

    VoC_jump(Coolsand_inflate_table_case_end)
Coolsand_inflate_table_case_LENS:

    VoC_lw16i(REG4,dbase);
    VoC_lw16i(REG5,dext);

    VoC_lw16i_short(REG6,-1,DEFAULT);

Coolsand_inflate_table_case_end:

    VoC_sw16_d(REG4,inflate_table_base);
    VoC_sw16_d(REG5,inflate_table_extra);
    VoC_sw16_d(REG6,inflate_table_end);


    /* initialize state for loop */
//   huff = 0;                   /* starting code */
//   sym = 0;                    /* starting code symbol */
//   len = min;                  /* starting code length */
//   next = *table;              /* current table to fill in */
//   curr = root;                /* current table index bits */
//   drop = 0;                   /* current bits to drop from code for index */
//   low = (unsigned)(-1);       /* trigger new sub-table when len > root */
//   used = 1U << root;          /* use root table entries */
//   mask = used - 1;            /* mask for comparing low */





    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_lw16_d(REG2,inflate_table_min);
    VoC_lw16_d(REG3,inflate_table_root);

    VoC_sw16_d(REG1,inflate_table_huff);
    VoC_sw16_d(REG2,inflate_table_len);
    VoC_sw16_d(REG1,inflate_table_drop);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,-1,IN_PARALLEL);

    VoC_sw16_d(REG3,inflate_table_curr);

    VoC_sub16_rr(REG6,REG1,REG3,DEFAULT);

    VoC_shr16_rr(REG4,REG6,DEFAULT);

    VoC_sw16_d(REG5,inflate_table_low);

    VoC_add16_rr(REG5,REG4,REG5,DEFAULT);
    VoC_lw16i_short(REG1,LENS,IN_PARALLEL);

    VoC_sw16_d(REG4,inflate_table_used);

    VoC_lw16_d(REG2,inflate_table_table);

    VoC_sw16_d(REG5,inflate_table_mask);

    VoC_sw16_d(REG2,inflate_table_next);

    /* check available table space */
    //  if (type == LENS && used >= ENOUGH - MAXD)
    //    return 1;


    VoC_lw16i(REG2,(ENOUGH - MAXD));

    VoC_bne16_rd(Coolsand_inflate_table_L8,REG1,inflate_table_type);
    VoC_bgt16_rd(Coolsand_inflate_table_L8,REG2,inflate_table_used);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_jump(Coolsand_inflate_table_End);
Coolsand_inflate_table_L8:

    /* process all codes and make table entries */
//    for (;;) {

    VoC_lw16d_set_inc(REG1,inflate_table_work,1)

Coolsand_inflate_table_main_loop_Begin:
    /* create table entry */

    /*
            this.bits = (unsigned char)(len - drop);
            if ((int)(work[sym]) < end) {
                this.op = (unsigned char)0;
                this.val = work[sym];
            }
            else if ((int)(work[sym]) > end) {
                this.op = (unsigned char)(extra[work[sym]]);
                this.val = base[work[sym]];
            }
            else {
                this.op = (unsigned char)(32 + 64);         // end of block
                this.val = 0;
            }
    */
    VoC_lw16_d(REG4,inflate_table_len);

    VoC_sub16_rd(REG4,REG4,inflate_table_drop);

    VoC_lw16inc_p(REG2,REG1,DEFAULT);//work[sym];

    VoC_bnlt16_rd(Coolsand_inflate_table_main_loop_L0,REG2,inflate_table_end)
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_movreg16(REG6,REG2,IN_PARALLEL);
    VoC_jump(Coolsand_inflate_table_main_loop_L2);
Coolsand_inflate_table_main_loop_L0:

    VoC_bngt16_rd(Coolsand_inflate_table_main_loop_L1,REG2,inflate_table_end)

    VoC_add16_rd(REG5,REG2,inflate_table_extra);
    VoC_add16_rd(REG6,REG2,inflate_table_base);

    VoC_lw16_p(REG5,REG5,DEFAULT);
    VoC_lw16_p(REG6,REG6,DEFAULT);

    VoC_jump(Coolsand_inflate_table_main_loop_L2);
Coolsand_inflate_table_main_loop_L1:

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG5,(32+64),IN_PARALLEL);

Coolsand_inflate_table_main_loop_L2:


    /* replicate for those indices with low len bits equal to huff */
    /*      incr = 1U << (len - drop);
          fill = 1U << curr;
          min = fill;                 // save offset to next table
          do {
              fill -= incr;
              next[(huff >> drop) + fill] = this;
          } while (fill != 0);

    */

    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG2,REG4,DEFAULT);
    VoC_movreg32(ACC0,REG45,IN_PARALLEL);

    VoC_shr16_rr(REG7,REG3,DEFAULT);//incr
    VoC_movreg16(ACC1_LO,REG6,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG2,REG7,DEFAULT);

    VoC_sub16_rd(REG4,REG2,inflate_table_curr);

    VoC_movreg16(ACC1_HI,REG3,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_shr16_rr(REG2,REG4,DEFAULT);

    VoC_lw16_d(REG3,inflate_table_huff);

    VoC_lw16_d(REG7,inflate_table_drop);

    VoC_sw16_d(REG2,inflate_table_min);

    VoC_shr16_rr(REG3,REG7,DEFAULT);//huff>>drop;

    VoC_add16_rd(REG3,REG3,inflate_table_next);

    VoC_lw16i(REG4,state_codes_bits);
    VoC_lw16i(REG5,state_codes_op);
    VoC_lw16i(REG6,state_codes_val);

    VoC_add16_rr(REG4,REG3,REG4,DEFAULT);
    VoC_add16_rr(REG5,REG3,REG5,IN_PARALLEL);

    VoC_add16_rr(REG6,REG3,REG6,DEFAULT);
    VoC_movreg16(REG3,ACC1_HI,IN_PARALLEL);

Coolsand_inflate_table_main_loop_L3:

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);

    VoC_add16_rr(REG3,REG4,REG2,DEFAULT);
    VoC_add16_rr(REG7,REG5,REG2,IN_PARALLEL);

    VoC_add16_rr(REG3,REG6,REG2,DEFAULT);

    VoC_sw16_p(ACC0_LO,REG3,DEFAULT);

    VoC_sw16_p(ACC0_HI,REG7,DEFAULT);

    VoC_sw16_p(ACC1_LO,REG3,DEFAULT);
    VoC_movreg16(REG3,ACC1_HI,IN_PARALLEL);

    VoC_bnez16_r(Coolsand_inflate_table_main_loop_L3,REG2)


    /* backwards increment the len-bit code huff */
    /*     incr = 1U << (len - 1);
          while (huff & incr)
              incr >>= 1;
          if (incr != 0) {
              huff &= incr - 1;
              huff += incr;
          }
          else
              huff = 0;
    */

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub16_dr(REG4,inflate_table_len,REG2);

    VoC_sub16_rr(REG4,REG3,REG4,DEFAULT);

    VoC_shr16_rr(REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_lw16_d(REG5,inflate_table_huff);

    VoC_and16_rr(REG5,REG2,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);

Coolsand_inflate_table_main_loop_L4_0:
    VoC_bez16_r(Coolsand_inflate_table_main_loop_L4,REG5)

    VoC_shr16_ri(REG2,1,DEFAULT);
    VoC_movreg16(REG5,REG6,IN_PARALLEL);

    VoC_and16_rr(REG5,REG2,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);

    VoC_jump(Coolsand_inflate_table_main_loop_L4_0);

Coolsand_inflate_table_main_loop_L4:


    VoC_bez16_r(Coolsand_inflate_table_main_loop_L5,REG2)

    VoC_sub16_rr(REG4,REG2,REG3,DEFAULT);

    VoC_and16_rr(REG6,REG4,DEFAULT);

    VoC_add16_rr(REG5,REG6,REG2,DEFAULT);

    VoC_jump(Coolsand_inflate_table_main_loop_L6)
Coolsand_inflate_table_main_loop_L5:
    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_NOP();
Coolsand_inflate_table_main_loop_L6:

    VoC_sw16_d(REG5,inflate_table_huff);

    /* go to next symbol, update count, len */
    /*       sym++;
           if (--(count[len]) == 0) {
               if (len == max) break;
               len = lens[work[sym]];
           }
    */

    VoC_lw16i(REG7,count);

    VoC_lw16_d(REG2,inflate_table_len);

    VoC_add16_rr(REG5,REG2,REG7,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_sub16_pr(REG6,REG5,REG4,DEFAULT);

    VoC_lw16_p(REG3,REG1,DEFAULT);

    VoC_sw16_p(REG6,REG5,DEFAULT);

    VoC_bnez16_r(Coolsand_inflate_table_main_loop_L8,REG6)

    VoC_add16_rd(REG3,REG3,inflate_table_lens);


    VoC_bne16_rd(Coolsand_inflate_table_main_loop_L7,REG2,inflate_table_max)

    VoC_jump(Coolsand_inflate_table_main_loop_End);
Coolsand_inflate_table_main_loop_L7:

    VoC_lw16_p(REG2,REG3,DEFAULT);
    VoC_NOP();

Coolsand_inflate_table_main_loop_L8:

    VoC_sw16_d(REG2,inflate_table_len);

    /* create new sub-table if needed */
    //  if (len > root && (huff & mask) != low) {

    VoC_lw16_d(REG3,inflate_table_root)
    VoC_bngt16_rr(Coolsand_inflate_table_main_loop_Begin,REG2,REG3)

    VoC_lw16_d(REG3,inflate_table_huff);

    VoC_and16_rd(REG3,inflate_table_mask);

    VoC_lw16_d(REG4,inflate_table_low);

    VoC_be16_rr(Coolsand_inflate_table_main_loop_Begin,REG3,REG4)
    /* if first time, transition to sub-tables */
    //    if (drop == 0)
    //      drop = root;

    /* increment past last table */
    //     next += min;            /* here min is 1 << curr */

    VoC_lw16_d(REG4,inflate_table_root)
    VoC_bnez16_d(Coolsand_inflate_table_main_loop_L9,inflate_table_drop)
    VoC_sw16_d(REG4,inflate_table_drop);
Coolsand_inflate_table_main_loop_L9:

    VoC_lw16_d(REG4,inflate_table_next);

    VoC_add16_rd(REG4,REG4,inflate_table_min);

    VoC_sub16_rd(REG2,REG2,inflate_table_drop);//curr

    VoC_sw16_d(REG4,inflate_table_next);

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG3,count,1);

    VoC_add16_rd(REG7,REG2,inflate_table_drop);

    VoC_sub16_rr(REG4,REG4,REG2,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG7,IN_PARALLEL);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_shr16_rr(REG6,REG4,IN_PARALLEL);//;left

Coolsand_inflate_table_main_loop_L10:

    VoC_bnlt16_rd(Coolsand_inflate_table_main_loop_L11,REG7,inflate_table_max)

    VoC_sub16inc_rp(REG6,REG6,REG3,DEFAULT);

    VoC_bngtz16_r(Coolsand_inflate_table_main_loop_L11,REG6);

    VoC_inc_p(REG2,DEFAULT);
    VoC_shr16_ri(REG6,-1,DEFAULT);

    VoC_add16_rd(REG7,REG2,inflate_table_drop);

    VoC_jump(Coolsand_inflate_table_main_loop_L10)
Coolsand_inflate_table_main_loop_L11:

    //REG2:curr
    //REG6:left

    VoC_sw16_d(REG6,inflate_table_left);
    VoC_sw16_d(REG2,inflate_table_curr);

    /* check for enough space */
    /*      used += 1U << curr;
          if (type == LENS && used >= ENOUGH - MAXD)
              return 1;
    */

    VoC_lw16i_short(REG5,0,DEFAULT)
    VoC_lw16i_short(REG6,LENS,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG2,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_shr16_rr(REG7,REG5,DEFAULT);

    VoC_add16_rd(REG5,REG7,inflate_table_used);

    VoC_lw16i(REG4,(ENOUGH - MAXD));

    VoC_sw16_d(REG5,inflate_table_used);

    VoC_bne16_rd(Coolsand_inflate_table_main_loop_L12,REG6,inflate_table_type);
    VoC_bgt16_rr(Coolsand_inflate_table_main_loop_L12,REG4,REG5);
    VoC_lw16i_short(REG7,1,DEFAULT);
    VoC_jump(Coolsand_inflate_table_End);
Coolsand_inflate_table_main_loop_L12:

    VoC_lw16_d(REG3,inflate_table_huff);

    VoC_and16_rd(REG3,inflate_table_mask);

    /* point entry in root table to sub-table */
    /*    low = huff & mask;
        (*table)[low].op = (unsigned char)curr;
        (*table)[low].bits = (unsigned char)root;
        (*table)[low].val = (unsigned short)(next - *table);
    */
    //REG2:curr

    VoC_add16_rd(REG4,REG3,inflate_table_table);

    VoC_sw16_d(REG3,inflate_table_low);

    VoC_lw16i(REG5,state_codes_op);
    VoC_lw16i(REG6,state_codes_bits);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_lw16i(REG7,state_codes_val);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_sw16_p(REG2,REG5,IN_PARALLEL);

    VoC_lw16_d(REG3,inflate_table_table);

    VoC_sub16_dr(REG4,inflate_table_next,REG3);

    VoC_lw16_d(REG3,inflate_table_root);

    VoC_sw16_p(REG4,REG7,DEFAULT);

    VoC_sw16_p(REG3,REG6,DEFAULT);

    VoC_jump(Coolsand_inflate_table_main_loop_Begin)

Coolsand_inflate_table_main_loop_End:

    /*
       Fill in rest of table for incomplete codes.  This loop is similar to the
       loop above in incrementing huff for table indices.  It is assumed that
       len is equal to curr + drop, so there is no loop needed to increment
       through high index bits.  When the current sub-table is filled, the loop
       drops back to the root table to fill in any remaining entries there.
     */
    //this.op = (unsigned char)64;                /* invalid code marker */
    //this.bits = (unsigned char)(len - drop);
    //this.val = (unsigned short)0;
    //while (huff != 0) {

    VoC_lw16i_short(ACC1_LO,0,DEFAULT);
    VoC_lw16i_short(ACC0_LO,64,IN_PARALLEL);

    VoC_lw16_d(REG3,inflate_table_drop);

    VoC_sub16_dr(REG5,inflate_table_len,REG3);

    VoC_movreg16(ACC0_HI,REG5,DEFAULT);

    VoC_lw16_d(REG7,inflate_table_huff);

Coolsand_inflate_table_L9:


    /* when done with sub-table, drop back to root table */
    /*if (drop != 0 && (huff & mask) != low) {
        drop = 0;
        len = root;
        next = *table;
        this.bits = (unsigned char)len;
    }
    */
    VoC_bez16_r(Coolsand_inflate_table_L12,REG7)

    VoC_bez16_d(Coolsand_inflate_table_L10,inflate_table_drop)
    VoC_movreg16(REG3,REG7,DEFAULT);
    VoC_and16_rd(REG3,inflate_table_mask);

    VoC_be16_rd(Coolsand_inflate_table_L10,REG3,inflate_table_low)
    VoC_lw16i_short(REG2,0,DEFAULT);
    VoC_lw16_d(REG3,inflate_table_root);
    VoC_sw16_d(REG2,inflate_table_drop);
    VoC_lw16_d(REG2,inflate_table_table);
    VoC_sw16_d(REG3,inflate_table_len);
    VoC_sw16_d(REG2,inflate_table_next);
    VoC_movreg16(REG5,REG3,DEFAULT);

Coolsand_inflate_table_L10:



    /* put invalid code marker in table */
    //next[huff >> drop] = this;

    VoC_lw16_d(REG2,inflate_table_drop);

    VoC_shr16_rr(REG7,REG2,DEFAULT);
    VoC_movreg16(REG3,REG7,IN_PARALLEL);


    VoC_add16_rd(REG4,REG7,inflate_table_next);


    VoC_lw16i(REG5,state_codes_op);
    VoC_lw16i(REG6,state_codes_bits);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_lw16i(REG7,state_codes_val);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);
    VoC_sw16_p(ACC0_LO,REG5,IN_PARALLEL);

    VoC_sw16_p(ACC0_HI,REG6,DEFAULT);

    VoC_sw16_p(ACC1_LO,REG7,DEFAULT);
    VoC_movreg16(REG7,REG3,IN_PARALLEL);


    /* backwards increment the len-bit code huff */
    /*     incr = 1U << (len - 1);
          while (huff & incr)
              incr >>= 1;
          if (incr != 0) {
              huff &= incr - 1;
              huff += incr;
          }
          else
              huff = 0;


    */
    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub16_dr(REG4,inflate_table_len,REG2);

    VoC_add16_rr(REG4,REG3,REG4,DEFAULT);
    VoC_movreg16(REG5,REG7,IN_PARALLEL);

    VoC_shr16_rr(REG2,REG4,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);


Coolsand_inflate_table_L11:

    VoC_and16_rr(REG5,REG2,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);

    VoC_movreg16(REG7,REG5,DEFAULT);
    VoC_shr16_ri(REG2,1,IN_PARALLEL);

    VoC_movreg16(REG5,REG6,DEFAULT);

    VoC_bnez16_r(Coolsand_inflate_table_L11,REG7)


    VoC_bez16_r(Coolsand_inflate_table_L12,REG2)

    VoC_sub16_rr(REG4,REG2,REG3,DEFAULT);

    VoC_and16_rr(REG5,REG4,DEFAULT);

    VoC_add16_rr(REG7,REG5,REG2,DEFAULT);

    VoC_jump(Coolsand_inflate_table_L9)


Coolsand_inflate_table_L12:

    /* set return parameters */
    //*table += used;
    //*bits = root;

    VoC_lw16_d(REG4,inflate_table_used);

    VoC_add16_rd(REG4,REG4,inflate_table_table);

    VoC_lw16_d(REG5,inflate_table_root);

    VoC_sw16_d(REG4,inflate_table_table);

    VoC_sw16_d(REG5,inflate_table_bits);

    VoC_lw16i_short(REG7,0,DEFAULT);//return;

Coolsand_inflate_table_End:


    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_inflate_fast
 *
 * Description: read
 *
 * Inputs:      REG7:start
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/11/2012
 **************************************************************************************/
void Coolsand_inflate_fast(void)
{

#if 0

    voc_short inflate_fast_lmask,x
    voc_short inflate_fast_dmask,x
    voc_short inflate_fast_beg,x
#endif

// VoC_directive: PARSER_OFF
    /*  printf("inflate_fast_count:%d\n",inflate_fast_count);

        if (inflate_fast_count==114)
        {
            printf("ok");
        }




        inflate_fast_count++;

    */

// VoC_directive: PARSER_ON





    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);

    VoC_sub16_rd(REG4,REG7,state_lenbits);

    VoC_sub16_rd(REG5,REG7,state_distbits);

    VoC_lw16i_short(REG2,1,DEFAULT);
    VoC_lw16i_short(REG3,1,IN_PARALLEL);

    VoC_shru16_rr(REG2,REG4,DEFAULT)
    VoC_shru16_rr(REG3,REG5,IN_PARALLEL);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_sub16_rr(REG2,REG2,REG4,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_lw16d_set_inc(REG1,GLOBAL_RawOutBufAddr,1);

    VoC_sub32_rd(REG45,REG67,PNGzstream_avail_out);

    VoC_sub16_rr(REG5,REG1,REG4,DEFAULT);

    VoC_sw16_d(REG2,inflate_fast_lmask);
    VoC_sw16_d(REG3,inflate_fast_dmask);
    VoC_sw16_d(REG5,inflate_fast_beg);


Coolsand_inflate_fast_main_loop:

    VoC_jal(Coolsand_DmaInData);

    VoC_lw16i_short(REG7,15,DEFAULT);

    VoC_bngt16_rd(Coolsand_inflate_fast_LX0,REG7,state_bits)

    VoC_lw16_d(REG3,state_bits);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sub16_rr(REG6,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG4,-8,DEFAULT);

    VoC_or16_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,16,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG7,REG3,IN_PARALLEL);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

Coolsand_inflate_fast_LX0:


    VoC_movreg16(REG4,RL7_LO,DEFAULT);

    VoC_and16_rd(REG4,inflate_fast_lmask);

    VoC_lw16_d(REG3,state_lencode);

    VoC_add16_rd(REG5,REG3,state_codes_op_addr);
    VoC_add16_rd(REG6,REG3,state_codes_bits_addr);
    VoC_add16_rd(REG7,REG3,state_codes_val_addr);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_lw16_p(REG5,REG5,DEFAULT);//this->op
    VoC_lw16_p(REG6,REG6,DEFAULT);//this->bits
    VoC_lw16_p(REG7,REG7,DEFAULT);//this->val

Coolsand_inflate_fast_dolen:

    VoC_sub16_dr(REG2,state_bits,REG6);

    VoC_shru32_rr(RL7,REG6,DEFAULT);

    VoC_sw16_d(REG2,state_bits);

    VoC_bnez16_r(Coolsand_inflate_fast_L0,REG5)

    /*      VoC_lw16_p(REG4,REG1,DEFAULT);
            VoC_shru16_ri(REG7,-8,IN_PARALLEL)

            VoC_shru16_ri(REG4,-3,DEFAULT);
            VoC_shru16_ri(REG7,8,IN_PARALLEL);

            VoC_or16_rr(REG7,REG4,DEFAULT);

            VoC_lw16i_short(REG3,1,DEFAULT);
            VoC_movreg16(INC1,REG2,IN_PARALLEL);

            VoC_sw16inc_p(REG7,REG1,DEFAULT);
            VoC_sub16_rr(REG2,REG3,REG2,IN_PARALLEL);*/

    VoC_sw16inc_p(REG7,REG1,DEFAULT);


    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_L0:

    VoC_movreg16(REG4,REG5,DEFAULT);

    VoC_and16_ri(REG4,16);

    VoC_bez16_r(Coolsand_inflate_fast_L4,REG4)

    VoC_sw16_d(REG7,inflate_fast_len);

    VoC_and16_ri(REG5,15);


    VoC_bez16_r(Coolsand_inflate_fast_L4_1,REG5)

    VoC_bngt16_rd(Coolsand_inflate_fast_L4_0,REG5,state_bits)

    VoC_push16(REG5,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);

    VoC_lw16_d(REG3,state_bits);

    VoC_lbinc_p(REG0);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sub16_rr(REG6,REG5,REG3,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG7,REG3,IN_PARALLEL);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

    VoC_pop16(REG5,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);


Coolsand_inflate_fast_L4_0:

    //REG5:op

    VoC_lw16i_short(REG2,16,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG5,REG2,DEFAULT);
    VoC_sub16_rr(REG2,REG2,REG5,IN_PARALLEL);

    VoC_shru16_rr(REG6,REG3,DEFAULT);

    VoC_shru16_rr(REG6,REG2,DEFAULT);
    VoC_shru32_rr(RL7,REG5,IN_PARALLEL);

    VoC_add16_rd(REG7,REG6,inflate_fast_len);

    VoC_sub16_dr(REG4,state_bits,REG5);

    VoC_sw16_d(REG7,inflate_fast_len);

    VoC_sw16_d(REG4,state_bits);

Coolsand_inflate_fast_L4_1:
    VoC_lw16i_short(REG5,15,DEFAULT);

    VoC_bngt16_rd(Coolsand_inflate_fast_L4_2,REG5,state_bits)

    VoC_lw16_d(REG3,state_bits);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG7,8,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sub16_rr(REG6,REG5,REG3,IN_PARALLEL);

    VoC_shru16_ri(REG4,-8,DEFAULT);

    VoC_or16_rr(REG4,REG7,DEFAULT);
    VoC_lw16i_short(REG7,16,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG7,REG3,IN_PARALLEL);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

Coolsand_inflate_fast_L4_2:
    VoC_movreg16(REG4,RL7_LO,DEFAULT)

    VoC_and16_rd(REG4,inflate_fast_dmask);

    VoC_lw16_d(REG3,state_distcode);

    VoC_add16_rd(REG5,REG3,state_codes_op_addr);
    VoC_add16_rd(REG6,REG3,state_codes_bits_addr);
    VoC_add16_rd(REG7,REG3,state_codes_val_addr);

    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_lw16_p(REG5,REG5,DEFAULT);//this->op
    VoC_lw16_p(REG6,REG6,DEFAULT);//this->bits
    VoC_lw16_p(REG7,REG7,DEFAULT);//this->val

Coolsand_inflate_fast_dodist:

    VoC_sub16_dr(REG2,state_bits,REG6);

    VoC_shru32_rr(RL7,REG6,DEFAULT);
    VoC_movreg16(REG4,REG5,IN_PARALLEL);

    VoC_sw16_d(REG2,state_bits);

    VoC_and16_ri(REG4,16);

    VoC_bez16_r(Coolsand_inflate_fast_L2,REG4)

    VoC_shru32_ri(REG67,16,DEFAULT);

    VoC_and16_ri(REG5,15);

    VoC_sw32_d(REG67,inflate_fast_dist);

    VoC_lw16_d(REG3,state_bits);

    VoC_bngt16_rr(Coolsand_inflate_fast_L2_0,REG5,REG3)

    VoC_push16(REG5,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);

    VoC_lw16_d(REG3,state_bits);

    VoC_lbinc_p(REG0);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sub16_rr(REG6,REG5,REG3,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG7,REG3,IN_PARALLEL);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

    VoC_pop16(REG5,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_inflate_fast_L2_0,REG5,state_bits)

    VoC_push16(REG5,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);

    VoC_lw16_d(REG3,state_bits);

    VoC_lbinc_p(REG0);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG7,8,IN_PARALLEL);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_sub16_rr(REG6,REG5,REG3,IN_PARALLEL);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_add16_rr(REG6,REG7,REG3,IN_PARALLEL);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

    VoC_pop16(REG5,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

Coolsand_inflate_fast_L2_0:

    //REG5:op
    VoC_lw16i_short(REG6,16,DEFAULT);
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_sub16_rr(REG7,REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG6,REG6,REG5,IN_PARALLEL);

    VoC_shru16_rr(REG2,REG7,DEFAULT);

    VoC_shru16_rr(REG2,REG6,DEFAULT);
    VoC_shru32_rr(RL7,REG5,IN_PARALLEL);

    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_add32_rd(REG67,REG23,inflate_fast_dist);

    VoC_sub16_dr(REG4,state_bits,REG5);

    VoC_sw32_d(REG67,inflate_fast_dist);

    VoC_sw16_d(REG4,state_bits);

    VoC_lw16_d(REG3,inflate_fast_beg);

    VoC_sub16_rr(REG4,REG1,REG3,DEFAULT);//                op = (unsigned)(out - beg);     /* max distance in output */
    VoC_lw16i_short(REG5,0,IN_PARALLEL);


    VoC_bngt32_rr(Coolsand_inflate_fast_L1,REG67,REG45)

    VoC_sub32_rr(REG67,REG67,REG45,DEFAULT);

    VoC_bngt32_rd(Coolsand_inflate_fast_Window_L0,REG67,state_whave)

    VoC_lw16i_short(REG3,BAD,DEFAULT);
    VoC_NOP();
    VoC_sw16_d(REG3,state_mode);
    VoC_jump(Coolsand_inflate_fast_End);
Coolsand_inflate_fast_Window_L0:
    //VoC_lw32_d(ACC0,state_window);

    VoC_bnez32_d(Coolsand_inflate_fast_Window_L2,state_write)

    /* very common case */
    //from += wsize - op;
    VoC_sub32_dr(ACC0,state_wsize,REG67);

    VoC_shru32_ri(ACC0,-1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_lw16_d(ACC1_LO,inflate_fast_len);

    VoC_bngt32_rr(Coolsand_inflate_fast_Window_L1,ACC1,REG67)
    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_sub16_dr(REG4,inflate_fast_len,REG6)

    VoC_sw32_d(REG67,zmemcpy_Length);
    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);

    VoC_sw16_d(REG4,inflate_fast_len);

    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);


    //REG1:outbuf pointer
    VoC_lw16_d(REG4,inflate_fast_len);
    VoC_lw32_d(REG67,inflate_fast_dist);
    //REG67:dist
    //REG4:Length

    VoC_jal(Coolsand_CopyBytesFromOutBuf);



    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_Window_L1:

    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_sw32_d(REG67,zmemcpy_Length);
    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);


    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);

    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_Window_L2:

    VoC_lw32_d(ACC0,state_write)
    VoC_bngt32_rr(Coolsand_inflate_fast_Window_L4,REG67,ACC0)

    /* very common case */
    //from += wsize - op;
    VoC_sub32_dr(ACC0,state_wsize,REG67);

    VoC_add32_rd(ACC0,ACC0,state_write);

    VoC_shru32_ri(ACC0,-1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_sub32_rd(REG67,REG67,state_write);

    VoC_lw16_d(ACC1_LO,inflate_fast_len);

    VoC_bngt32_rr(Coolsand_inflate_fast_Window_L3,ACC1,REG67)


    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_sub16_dr(REG4,inflate_fast_len,REG6);


    VoC_sw32_d(REG67,zmemcpy_Length);
    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);


    VoC_sw16_d(REG4,inflate_fast_len);


    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);



    VoC_lw16_d(REG6,inflate_fast_len);

    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_lw32z(ACC0,IN_PARALLEL);

    VoC_lw32_d(ACC1,state_write);

    VoC_bngt32_rr(Coolsand_inflate_fast_Window_L3,REG67,ACC1)


    VoC_lw32_d(REG67,state_write);

    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_sub16_dr(REG4,inflate_fast_len,REG6);



    VoC_sw32_d(REG67,zmemcpy_Length);
    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);

    VoC_sw16_d(REG4,inflate_fast_len);



    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);


    //REG1:outbuf pointer
    VoC_lw16_d(REG4,inflate_fast_len);
    VoC_lw32_d(REG67,inflate_fast_dist);
    //REG67:dist
    //REG4:Length

    VoC_jal(Coolsand_CopyBytesFromOutBuf);

    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_Window_L3:
    VoC_lw16_d(REG6,inflate_fast_len);

    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);
    VoC_sw32_d(REG67,zmemcpy_Length);


    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);

    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_Window_L4:

    /* contiguous in window */
    //from += wsize - op;
    VoC_sub32_dr(ACC0,state_write,REG67);

    VoC_shru32_ri(ACC0,-1,DEFAULT);
    VoC_lw16i_short(ACC1_HI,0,IN_PARALLEL);

    VoC_lw16_d(ACC1_LO,inflate_fast_len);

    VoC_bngt32_rr(Coolsand_inflate_fast_Window_Lx1,ACC1,REG67)
    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_sub16_dr(REG4,inflate_fast_len,REG6)

    VoC_sw32_d(REG67,zmemcpy_Length);
    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);

    VoC_sw16_d(REG4,inflate_fast_len);


    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);


    //REG1:outbuf pointer
    VoC_lw16_d(REG4,inflate_fast_len);
    VoC_lw32_d(REG67,inflate_fast_dist);
    //REG67:dist
    //REG4:Length

    VoC_jal(Coolsand_CopyBytesFromOutBuf);



    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_Window_Lx1:

    VoC_lw16_d(REG6,inflate_fast_len);

    VoC_add32_rd(ACC0,ACC0,state_window);

    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_sw32_d(ACC0,zmemcpy_SrcAddr);
    VoC_sw32_d(REG67,zmemcpy_Length);


    //REG1:put.
    //ACC0:from:
    //REG67:copy

    VoC_jal(Coolsand_zmemcpyToVoC);

    VoC_jump(Coolsand_inflate_fast_main_loop_End);


Coolsand_inflate_fast_L1:

    //REG1:outbuf pointer
    VoC_lw16_d(REG4,inflate_fast_len);

    //REG67:dist
    //REG4:Length

    VoC_jal(Coolsand_CopyBytesFromOutBuf);

    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_L2:

    VoC_movreg16(REG4,REG5,DEFAULT);

    VoC_and16_ri(REG4,64);

    VoC_bnez16_r(Coolsand_inflate_fast_L3,REG4)

    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_movreg32(REG23,RL7,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);

    VoC_shru16_rr(REG2,REG5,DEFAULT);

    VoC_shru16_rr(REG2,REG4,DEFAULT);

    VoC_add16_rr(REG4,REG7,REG2,DEFAULT);

    VoC_lw16_d(REG3,state_distcode);

    VoC_add16_rd(REG5,REG3,state_codes_op_addr);
    VoC_add16_rd(REG6,REG3,state_codes_bits_addr);
    VoC_add16_rd(REG7,REG3,state_codes_val_addr);


    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_lw16_p(REG5,REG5,DEFAULT);//this->op
    VoC_lw16_p(REG6,REG6,DEFAULT);//this->bits
    VoC_lw16_p(REG7,REG7,DEFAULT);//this->val


    VoC_jump(Coolsand_inflate_fast_dodist);
Coolsand_inflate_fast_L3:


    VoC_lw16i_short(REG3,BAD,DEFAULT);

    VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);

    VoC_sw16_d(REG3,state_mode);
    VoC_sw16_d(REG7,inflate_ret);

    VoC_jump(Coolsand_inflate_fast_End);

Coolsand_inflate_fast_L4:

    VoC_movreg16(REG4,REG5,DEFAULT);

    VoC_and16_ri(REG4,64);

    VoC_bnez16_r(Coolsand_inflate_fast_L5,REG4)

    VoC_lw16i_short(REG4,16,DEFAULT);
    VoC_movreg16(REG2,RL7_LO,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);

    VoC_shru16_rr(REG2,REG5,DEFAULT);

    VoC_shru16_rr(REG2,REG4,DEFAULT);

    VoC_add16_rr(REG4,REG7,REG2,DEFAULT);

    VoC_lw16_d(REG3,state_lencode);

    VoC_add16_rd(REG5,REG3,state_codes_op_addr);
    VoC_add16_rd(REG6,REG3,state_codes_bits_addr);
    VoC_add16_rd(REG7,REG3,state_codes_val_addr);


    VoC_add16_rr(REG5,REG5,REG4,DEFAULT);
    VoC_add16_rr(REG6,REG6,REG4,IN_PARALLEL);

    VoC_add16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_lw16_p(REG5,REG5,DEFAULT);//this->op
    VoC_lw16_p(REG6,REG6,DEFAULT);//this->bits
    VoC_lw16_p(REG7,REG7,DEFAULT);//this->val

    VoC_jump(Coolsand_inflate_fast_dolen);
Coolsand_inflate_fast_L5:

    VoC_movreg16(REG4,REG5,DEFAULT);

    VoC_and16_ri(REG4,32);

    VoC_bez16_r(Coolsand_inflate_fast_L6,REG4)

    VoC_lw16i_short(REG3,TYPE,DEFAULT);

    VoC_NOP();

    VoC_sw16_d(REG3,state_mode);

    VoC_jump(Coolsand_inflate_fast_End);


    VoC_jump(Coolsand_inflate_fast_main_loop_End);
Coolsand_inflate_fast_L6:

    VoC_lw16i_short(REG3,BAD,DEFAULT);

    VoC_NOP();

    VoC_sw16_d(REG3,state_mode);

    VoC_jump(Coolsand_inflate_fast_End);

    VoC_NOP();
Coolsand_inflate_fast_main_loop_End:

    VoC_lw16i(REG4,257);

    VoC_lw16i_short(REG5,0,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sub16_rd(REG2,REG1,GLOBAL_RawOutBufAddr);

    VoC_sub32_dr(REG67,PNGzstream_avail_out,REG23);

    VoC_bngt32_rr(Coolsand_inflate_fast_End,REG67,REG45);

    VoC_lw32_d(REG67,PNGzstream_avail_in);

    VoC_lw16i_short(REG4,6,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bngt32_rr(Coolsand_inflate_fast_main_loop,REG45,REG67);


Coolsand_inflate_fast_End:

    VoC_lw16i_short(REG3,0,DEFAULT);

    VoC_sub16_rd(REG2,REG1,GLOBAL_RawOutBufAddr);

    VoC_sub32_dr(REG23,PNGzstream_avail_out,REG23);

    VoC_sw16_d(REG1,GLOBAL_RawOutBufAddr);
    VoC_sw32_d(REG23,PNGzstream_avail_out);

    /* return unused bytes (on entry, bits < 8, so in won't go too far back) */
    // len = bits >> 3;
    // in -= len;
    // bits -= len << 3;
    // hold &= (1U << bits) - 1;


    VoC_lw16_d(REG1,state_bits)

    VoC_shru16_ri(REG1,3,DEFAULT);
    VoC_movreg16(REG3,REG1,IN_PARALLEL);

    VoC_shru16_ri(REG1,-3,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG3,REG1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG7,REG3,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);

    VoC_sw16_d(REG3,state_bits);

    VoC_movreg32(ACC1,REG67,DEFAULT);
    VoC_shru32_rr(REG67,REG4,IN_PARALLEL);

    VoC_sub32_rr(REG67,REG67,ACC1,DEFAULT);
    VoC_push16(BITCACHE,IN_PARALLEL);

    VoC_and32_rr(RL7,REG67,DEFAULT);
    VoC_pop16(REG3,IN_PARALLEL);

    VoC_shru16_ri(REG3,3,DEFAULT);
    VoC_lw32z(RL6,IN_PARALLEL);

    VoC_add16_rr(REG3,REG3,REG2,DEFAULT);
    VoC_movreg16(REG6,REG2,IN_PARALLEL);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL)

    VoC_add32_rd(REG67,REG67,PNGzstream_avail_in);

    VoC_add16_rr(REG3,REG3,REG4,DEFAULT);
    VoC_movreg16(REG5,REG3,IN_PARALLEL);

    VoC_sw32_d(REG67,PNGzstream_avail_in);

    VoC_and16_rr(REG5,REG4,DEFAULT);
    VoC_shru16_ri(REG3,1,IN_PARALLEL);

    VoC_lw16i_short(BITCACHE,0,DEFAULT);
    VoC_lw16i_short(REG4,0,IN_PARALLEL);

    VoC_sub16_rr(REG3,REG4,REG3,DEFAULT);
    VoC_shru16_ri(REG5,-3,IN_PARALLEL);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_add16_rr(REG0,REG0,REG3,IN_PARALLEL);


    VoC_lw16i(REG4,1024);

    VoC_add16_rr(REG0,REG0,REG4,DEFAULT);

    VoC_and16_ri(REG0,0x3ff);

    VoC_pop16(RA,DEFAULT);

    VoC_lbinc_p(REG0);

    VoC_rbinc_r(REG4,REG5,DEFAULT);

    /* update state and return */
    /* strm->next_in = in + OFF;
       strm->next_out = out + OFF;
       strm->avail_in = (unsigned)(in < last ? 5 + (last - in) : 5 - (in - last));
       strm->avail_out = (unsigned)(out < end ?
                                    257 + (end - out) : 257 - (out - end));
       state->hold = hold;
       state->bits = bits;
    */


    VoC_return;
}
/**************************************************************************************
 * Function:    Coolsand_CopyBytesFromWindow
 *
 * Description: read
 *
 * Inputs:
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/19/2012
 **************************************************************************************/
void Coolsand_CopyBytesFromWindow(void)
{






    VoC_return;
}


//  VoC_sub32_rr(REG45,REG67,REG45,DEFAULT);//DIST
/*      VoC_lw16i(REG2,32768);
    VoC_lw16i(REG3,0,DEFAULT);

            VoC_bgt32_rr(Coolsand_inflate_fast_L0,REG45,REG23)

            VoC_bngt32_rd(Coolsand_inflate_fast_L1,REG45,PNGzstream_total_out)
Coolsand_inflate_fast_L0:

                VoC_lw16i_short(REG1,BAD,DEFAULT);

                VoC_lw16i_short(REG7,Z_DATA_ERROR,DEFAULT);

                VoC_sw16_d(REG1,state_mode);

                VoC_jump(Coolsand_inflate_fast_main_loop_End);

Coolsand_inflate_fast_L1:*/

/*  VoC_lw32_d(ACC0,GLOBAL_OutPutAddr);

    VoC_lw32_d(RL7,PNGzstream_avail_out);

    VoC_movreg32(ACC1,REG67,DEFAULT);

    VoC_lw32_d(REG67,IDATHandle_imgOutStride);
Coolsand_CopyBytesFromWindow_L0:
    VoC_bngt32_rd(Coolsand_CopyBytesFromWindow_L1,ACC1,RL7)

        VoC_sub32_rr(ACC1,ACC1,RL7,DEFAULT);
        VoC_sub32_rr(ACC0,ACC0,REG67,IN_PARALLEL);

        VoC_jump(Coolsand_CopyBytesFromWindow_L0);
Coolsand_CopyBytesFromWindow_L1:

    VoC_sub32_rr(ACC0,ACC0,REG67,DEFAULT);

    VoC_add32_rr(ACC0,ACC0,RL7,DEFAULT);

    VoC_sub32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_lw16_d(REG4,inflate_fast_len);

    VoC_bngt32_rr(Coolsand_CopyBytesFromWindow_LX,REG45,ACC1)

        VoC_sub32_rr(RL7,REG45,ACC1,DEFAULT);
        VoC_movreg32(REG45,ACC1,IN_PARALLEL);

        VoC_movreg32(REG23,ACC0,DEFAULT);
        VoC_lw16i_short(REG5,3,IN_PARALLEL);

        VoC_and16_ri(REG2,0x3);

        VoC_push16(REG4,DEFAULT);

        VoC_movreg32(REG67,REG23,DEFAULT)
        VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

        VoC_lw16i(REG3,GLOBAL_DMATEMPBUF);

        VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
        VoC_add16_rr(REG4,REG4,REG2,IN_PARALLEL);

        VoC_shr16_ri(REG4,2,DEFAULT);
        VoC_shr16_ri(REG3,1,IN_PARALLEL);

        VoC_or16_ri(REG7,(DMA_UNSIGNED_B2S)>>16);

        VoC_sw16_d(REG3,CFG_DMA_LADDR);
        VoC_sw16_d(REG4,CFG_DMA_SIZE);
        VoC_sw32_d(REG67,CFG_DMA_EADDR);

        VoC_pop16(REG4,DEFAULT);

        VoC_lw16inc_p(REG3,REG2,DEFAULT);
        VoC_lw16i_short(REG5,20,IN_PARALLEL);

        VoC_bngt16_rr(Coolsand_CopyBytesFromWindow_Ly1,REG4,REG5)

            VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);

            VoC_loop_i_short(20,DEFAULT);
            VoC_lw16i_short(INC2,1,IN_PARALLEL);
            VoC_startloop0
                VoC_lw16inc_p(REG3,REG2,DEFAULT);
                VoC_sw16inc_p(REG3,REG1,DEFAULT);
            VoC_endloop0

Coolsand_CopyBytesFromWindow_Ly1:

        VoC_loop_r_short(REG4,DEFAULT);
        VoC_lw16i_short(INC2,1,IN_PARALLEL);
        VoC_startloop0
            VoC_lw16inc_p(REG3,REG2,DEFAULT);
            VoC_sw16inc_p(REG3,REG1,DEFAULT);
        VoC_endloop0

        VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

        VoC_sub32_rd(ACC0,ACC0,PNGzstream_avail_out);

        VoC_add32_rd(ACC0,ACC0,IDATHandle_imgOutStride);



Coolsand_CopyBytesFromWindow_L2:
        VoC_lw32_d(REG45,PNGzstream_next_out)
        VoC_bez32_r(Coolsand_CopyBytesFromWindow_End,RL7);
        VoC_bngt32_rr(Coolsand_CopyBytesFromWindow_L4,REG45,ACC0)


            VoC_movreg32(ACC1,RL7,DEFAULT);

            VoC_bngt32_rd(Coolsand_CopyBytesFromWindow_L3,RL7,PNGzstream_avail_out)
                    VoC_lw32_d(ACC1,PNGzstream_avail_out);
Coolsand_CopyBytesFromWindow_L3:

            VoC_sub32_rr(RL7,RL7,ACC1,DEFAULT);
            VoC_movreg32(REG45,ACC1,IN_PARALLEL);

            VoC_movreg32(REG23,ACC0,DEFAULT);
            VoC_lw16i_short(REG5,3,IN_PARALLEL);

            VoC_and16_ri(REG2,0x3);

            VoC_push16(REG4,DEFAULT);

            VoC_movreg32(REG67,REG23,DEFAULT)
            VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

            VoC_lw16i(REG3,GLOBAL_DMATEMPBUF);

            VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
            VoC_add16_rr(REG4,REG4,REG2,IN_PARALLEL);

            VoC_shr16_ri(REG4,2,DEFAULT);
            VoC_shr16_ri(REG3,1,IN_PARALLEL);

            VoC_or16_ri(REG7,(DMA_UNSIGNED_B2S)>>16);

            VoC_sw16_d(REG3,CFG_DMA_LADDR);
            VoC_sw16_d(REG4,CFG_DMA_SIZE);
            VoC_sw32_d(REG67,CFG_DMA_EADDR);

            VoC_pop16(REG4,DEFAULT);
            VoC_lw16inc_p(REG3,REG2,DEFAULT);
            VoC_lw16i_short(REG5,20,IN_PARALLEL);

            VoC_bngt16_rr(Coolsand_CopyBytesFromWindow_Ly2,REG4,REG5)

                VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);

                VoC_loop_i_short(20,DEFAULT);
                VoC_lw16i_short(INC2,1,IN_PARALLEL);
                VoC_startloop0
                    VoC_lw16inc_p(REG3,REG2,DEFAULT);
                    VoC_sw16inc_p(REG3,REG1,DEFAULT);
                VoC_endloop0

        Coolsand_CopyBytesFromWindow_Ly2:

            VoC_loop_r_short(REG4,DEFAULT);
            VoC_lw16i_short(INC2,1,IN_PARALLEL);
            VoC_startloop0
                VoC_lw16inc_p(REG3,REG2,DEFAULT);
                VoC_sw16inc_p(REG3,REG1,DEFAULT);
            VoC_endloop0

            VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);

            VoC_sub32_rd(ACC0,ACC0,PNGzstream_avail_out);

            VoC_add32_rd(ACC0,ACC0,IDATHandle_imgOutStride);


            VoC_jump(Coolsand_CopyBytesFromWindow_L2);
Coolsand_CopyBytesFromWindow_L4:


        VoC_lw16i(REG2,GLOBAL_RawOutBuf);

        VoC_movreg32(REG45,RL7,DEFAULT);

        VoC_lw16inc_p(REG3,REG2,DEFAULT);
        VoC_lw16i_short(REG5,20,IN_PARALLEL);

        VoC_bngt16_rr(Coolsand_CopyBytesFromWindow_Ly3,REG4,REG5)

            VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);

            VoC_loop_i_short(20,DEFAULT);
            VoC_lw16i_short(INC2,1,IN_PARALLEL);
            VoC_startloop0
                VoC_lw16inc_p(REG3,REG2,DEFAULT);
                VoC_sw16inc_p(REG3,REG1,DEFAULT);
            VoC_endloop0

    Coolsand_CopyBytesFromWindow_Ly3:

        VoC_loop_r_short(REG4,DEFAULT);
        VoC_lw16i_short(INC2,1,IN_PARALLEL);
        VoC_startloop0
            VoC_lw16inc_p(REG3,REG2,DEFAULT);
            VoC_sw16inc_p(REG3,REG1,DEFAULT);
        VoC_endloop0

        VoC_jump(Coolsand_CopyBytesFromWindow_End);
Coolsand_CopyBytesFromWindow_LX:

    VoC_movreg32(REG23,ACC0,DEFAULT);
    VoC_lw16i_short(REG5,3,IN_PARALLEL);

    VoC_and16_ri(REG2,0x3);

    VoC_movreg32(REG67,REG23,DEFAULT)
    VoC_add16_rr(REG4,REG4,REG5,IN_PARALLEL);

    VoC_lw16i(REG3,GLOBAL_DMATEMPBUF);

    VoC_add16_rr(REG2,REG2,REG3,DEFAULT);
    VoC_add16_rr(REG4,REG4,REG2,IN_PARALLEL);

    VoC_shr16_ri(REG4,2,DEFAULT);
    VoC_shr16_ri(REG3,1,IN_PARALLEL);

    VoC_or16_ri(REG7,(DMA_UNSIGNED_B2S)>>16);

    VoC_sw16_d(REG3,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_lw16_d(REG4,inflate_fast_len);

    VoC_lw16inc_p(REG3,REG2,DEFAULT);
    VoC_lw16i_short(REG5,20,IN_PARALLEL);

    VoC_bngt16_rr(Coolsand_CopyBytesFromWindow_Ly0,REG4,REG5)

        VoC_sub16_rr(REG4,REG5,REG4,DEFAULT);

        VoC_loop_i_short(20,DEFAULT);
        VoC_lw16i_short(INC2,1,IN_PARALLEL);
        VoC_startloop0
            VoC_lw16inc_p(REG3,REG2,DEFAULT);
            VoC_sw16inc_p(REG3,REG1,DEFAULT);
        VoC_endloop0

Coolsand_CopyBytesFromWindow_Ly0:

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_lw16i_short(INC2,1,IN_PARALLEL);
    VoC_startloop0
        VoC_lw16inc_p(REG3,REG2,DEFAULT);
        VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0


Coolsand_CopyBytesFromWindow_End:
    VoC_return;*/



/**************************************************************************************
 * Function:    Coolsand_CopyBytesInDMA
 *
 * Description: read
 *
 * Inputs:
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/19/2012
 **************************************************************************************/
void Coolsand_CopyBytesFromOutBuf(void)
{
    VoC_lw16i_short(REG5,20,DEFAULT);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_sub16_rr(REG2,REG1,REG6,IN_PARALLEL);


    VoC_bngt16_rr(Coolsand_CopyBytesFromOutBuf_L0,REG4,REG5)

    VoC_loop_i_short(20,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG3,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0

Coolsand_CopyBytesFromOutBuf_L0:

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG3,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_DmaInData
 *
 * Description: read
 *
 * Inputs:      REG1:PingPangFlag
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 *
 * Outputs:     no
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/23/2012
 **************************************************************************************/
void Coolsand_DmaInData(void)
{

    VoC_bngt16_rd(Coolsand_DmaInData_L0,REG0,GLOBAL_CONST_512_ADDR)
    VoC_bnez16_d(Coolsand_DmaInData_End,GLOBAL_PingPangFlag)
    VoC_lw16i(REG7,520);
    VoC_bngt16_rr(Coolsand_DmaInData_End,REG0,REG7)

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_lw16i(REG7,GLOBAL_INBUF/2);

    VoC_jump(Coolsand_DmaInData_L1);
Coolsand_DmaInData_L0:
    VoC_bez16_d(Coolsand_DmaInData_End,GLOBAL_PingPangFlag)
    VoC_bngt16_rd(Coolsand_DmaInData_End,REG0,GLOBAL_CONST_8_ADDR)

    VoC_lw16i_short(REG6,1,DEFAULT);

    VoC_lw16i(REG7,GLOBAL_INBUF/2+(1<<GLOBAL_WRAP_BIT)/4);

Coolsand_DmaInData_L1:

    VoC_sub16_rd(REG6,REG6,GLOBAL_PingPangFlag);

    VoC_lw16i(REG6,(1<<GLOBAL_WRAP_BIT)/4);

    VoC_sw16_d(REG6,GLOBAL_PingPangFlag);

    VoC_sw16_d(REG7,CFG_DMA_LADDR);

    VoC_lw32_d(REG67,GLOBAL_InBufAddr);

    VoC_sw16_d(REG6,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_add32_rd(REG67,REG67,GLOBAL_CONST_1024_ADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_sw32_d(REG67,GLOBAL_InBufAddr);

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

Coolsand_DmaInData_End:


    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_CRC2
 *
 * Description: read
 *
 * Inputs:      REG1:word
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *              RL7:byteleft
 *
 *
 * Outputs:     REG23:check
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_CRC2(void)
{

    VoC_push16(REG1,DEFAULT);

    VoC_and16_ri(REG1,0xff);

    VoC_lw16i(REG4,crc_table);

    VoC_lw16i_short(REG2,-1,DEFAULT);
    VoC_lw16i_short(REG3,-1,IN_PARALLEL);

    VoC_xor16_rr(REG2,REG6,DEFAULT);
    VoC_movreg32(ACC0,REG23,IN_PARALLEL);

    VoC_and16_ri(REG2,0xff);

    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);

    VoC_pop16(REG1,DEFAULT);

    VoC_lw32_p(REG23,REG2,DEFAULT);

    VoC_xor32_rr(REG23,ACC0,DEFAULT);
    VoC_shru16_ri(REG1,8,IN_PARALLEL);

    VoC_xor16_rr(REG2,REG1,DEFAULT);
    VoC_movreg32(ACC0,REG23,IN_PARALLEL);

    VoC_and16_ri(REG2,0xff);

    VoC_shr16_ri(REG2,-1,DEFAULT);
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);

    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);

    VoC_NOP();

    VoC_lw32_p(REG23,REG2,DEFAULT);

    VoC_xor32_rr(REG23,ACC0,DEFAULT);

    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_NEEDBITS
 *
 * Description: read
 *
 * Inputs:      REG7:n
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_NEEDBITS(void)
{
    VoC_push16(REG6,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

Coolsand_NEEDBITS_L0:
    VoC_bez32_d(Coolsand_NEEDBITS_End,PNGzstream_avail_in);
    VoC_bngt16_rd(Coolsand_NEEDBITS_End,REG7,state_bits)

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub16_rd(REG6,REG5,state_bits);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);

    VoC_add16_rd(REG6,REG6,state_bits);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_sw16_d(REG6,state_bits);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

    VoC_jump(Coolsand_NEEDBITS_L0);
Coolsand_NEEDBITS_End:

    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_DROPBITS
 *
 * Description:
 *
 * Inputs:      REG7:n
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_DROPBITS(void)
{
    VoC_shru32_rr(RL7,REG7,DEFAULT);

    VoC_sub16_dr(REG7,state_bits,REG7);

    VoC_NOP();

    VoC_sw16_d(REG7,state_bits);

    VoC_return;

}

/**************************************************************************************
 * Function:    Coolsand_BITS
 *
 * Description:
 *
 * Inputs:      REG7:n
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:     REG6:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_BITS(void)
{

    VoC_lw16i_short(REG6,16,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_sub16_rr(REG5,REG7,REG6,DEFAULT);
    VoC_sub16_rr(REG4,REG6,REG7,IN_PARALLEL);

    VoC_movreg16(REG6,RL7_LO,DEFAULT);

    VoC_shru16_rr(REG6,REG5,DEFAULT);

    VoC_shru16_rr(REG6,REG4,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_BYTEBITS
 *
 * Description:
 *
 * Inputs:
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:     REG23:check
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_BYTEBITS(void)
{
    VoC_push16(REG7,DEFAULT);

    VoC_lw16_d(REG7,state_bits);

    VoC_and16_ri(REG7,0x7);

    VoC_shru32_rr(RL7,REG7,DEFAULT);

    VoC_sub16_dr(REG7,state_bits,REG7);

    VoC_pop16(REG7,DEFAULT);

    VoC_sw16_d(REG7,state_bits);

    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_BYTEBITS
 *
 * Description:
 *
 * Inputs:
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:     REG23:check
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_INITBITS(void)
{
    VoC_lw32z(RL7,DEFAULT);

    VoC_NOP();

    VoC_sw16_d(RL7_LO,state_bits);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_PULLBYTE
 *
 * Description:
 *
 * Inputs:
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       04/25/2012
 **************************************************************************************/
void Coolsand_PULLBYTE(void)
{

    VoC_push16(REG6,DEFAULT);
    VoC_push32(REG45,IN_PARALLEL);

    VoC_lbinc_p(REG0);

    VoC_rbinc_i(REG4,8,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub16_rd(REG6,REG5,state_bits);

    VoC_shru32_rr(REG45,REG6,DEFAULT);
    VoC_lw16i_short(REG6,8,IN_PARALLEL);

    VoC_add16_rd(REG6,REG6,state_bits);

    VoC_or32_rr(RL7,REG45,DEFAULT);

    VoC_sw16_d(REG6,state_bits);

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_sub32_dr(REG45,PNGzstream_avail_in,REG45);

    VoC_pop16(REG6,DEFAULT);
    VoC_pop32(REG45,IN_PARALLEL);

    VoC_sw32_d(REG45,PNGzstream_avail_in);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_updatewindow
 *
 * Description:
 *
 * Inputs:      REG7:out
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 * Outputs:
 *
 * Used : All.
 *

   Update the window with the last wsize (normally 32K) bytes written before
   returning.  If window does not exist yet, create it.  This is only called
   when a window is already in use, or when output has been written during this
   inflate call, but the end of the deflate stream has not been reached yet.
   It is also called to create a window for dictionary data when a dictionary
   is loaded.

   Providing output buffers larger than 32K to inflate() should provide a speed
   advantage, since only the last 32K of output is copied to the sliding window
   upon return from inflate(), and since all distances after the first 32K of
   output will fall in the output data, making match copies simpler and faster.
   The advantage may be dependent on the size of the processor's data caches.

 * Version 1.0  Create by  Xuml       05/03/2012
 **************************************************************************************/
void Coolsand_updatewindow(void)
{

    /* if window not in use yet, initialize */
    /*    if (state->wsize == 0) {
            state->wsize = 1U << state->wbits;
            state->write = 0;
            state->whave = 0;
        }

    */
    VoC_push16(RA,DEFAULT);

    VoC_bnez32_d(Coolsand_updatewindow_L0,state_wsize)

    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw16i_short(REG4,1,DEFAULT)
    VoC_lw16i_short(REG5,0,IN_PARALLEL)

    VoC_sub16_rd(REG7,REG5,state_wbits);

    VoC_shru32_rr(REG45,REG7,DEFAULT);

    VoC_sw32_d(ACC0,state_write);
    VoC_sw32_d(ACC0,state_whave);

    VoC_sw32_d(REG45,state_wsize);

Coolsand_updatewindow_L0:

    /* copy state->wsize or less output bytes into the circular window */
    /*    copy = out - strm->avail_out;
        if (copy >= state->wsize)
        {
            zmemcpy(state->window, strm->next_out - state->wsize, state->wsize);
            state->write = 0;
            state->whave = state->wsize;
        }
        else {
            dist = state->wsize - state->write;
            if (dist > copy) dist = copy;
            zmemcpy(state->window + state->write, strm->next_out - copy, dist);
            copy -= dist;
            if (copy) {
                zmemcpy(state->window, strm->next_out - copy, copy);
                state->write = copy;
                state->whave = state->wsize;
            }
            else {
                state->write += dist;
                if (state->write == state->wsize) state->write = 0;
                if (state->whave < state->wsize) state->whave += dist;
            }
        }

    */


    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_sub32_rd(REG67,REG67,PNGzstream_avail_out);

    VoC_NOP();

    VoC_sw32_d(REG67,updatewindow_copy);

    VoC_blt32_rd(Coolsand_updatewindow_L1,REG67,state_wsize)

    VoC_lw32_d(REG67,state_wsize);
    VoC_sub16_dr(REG7,GLOBAL_RawOutBufAddr,REG6);
    VoC_lw32_d(ACC1,GLOBAL_WindowAddr);

    VoC_sw16_d(REG6,zmemcpy_Length);
    VoC_sw16_d(REG7,zmemcpy_SrcAddr);
    VoC_sw32_d(ACC1,zmemcpy_DstAddr);

    VoC_jal(Coolsand_zmemcpyFromVoC);

    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw32_d(ACC1,state_wsize);

    VoC_sw32_d(ACC0,state_write);
    VoC_sw32_d(ACC1,state_whave);

    VoC_jump(Coolsand_updatewindow_L6)
Coolsand_updatewindow_L1:

    VoC_lw32_d(ACC0,state_wsize);

    VoC_sub32_rd(ACC0,ACC0,state_write);

    VoC_bngt32_rr(Coolsand_updatewindow_L2,ACC0,REG67)
    VoC_movreg32(ACC0,REG67,DEFAULT);
    VoC_NOP();
Coolsand_updatewindow_L2:

    VoC_sw32_d(ACC0,updatewindow_dist);
    VoC_sub16_dr(REG7,GLOBAL_RawOutBufAddr,REG6);
    VoC_lw32_d(ACC1,state_write);
    VoC_shru32_ri(ACC1,-1,DEFAULT);
    VoC_add32_rd(ACC1,ACC1,state_window);

    VoC_sw16_d(ACC0_LO,zmemcpy_Length);
    VoC_sw16_d(REG7,zmemcpy_SrcAddr);
    VoC_sw32_d(ACC1,zmemcpy_DstAddr);

    VoC_jal(Coolsand_zmemcpyFromVoC);

    VoC_lw32_d(REG67,updatewindow_copy);

    VoC_sub32_rd(REG67,REG67,updatewindow_dist);


    VoC_bez32_r(Coolsand_updatewindow_L3,REG67);

    VoC_sw32_d(REG67,updatewindow_copy);
    VoC_sub16_dr(REG7,GLOBAL_RawOutBufAddr,REG6);
    VoC_lw32_d(ACC1,state_window);


    VoC_sw16_d(REG6,zmemcpy_Length);
    VoC_sw16_d(REG7,zmemcpy_SrcAddr);
    VoC_sw32_d(ACC1,zmemcpy_DstAddr);

    VoC_jal(Coolsand_zmemcpyFromVoC);

    VoC_lw32_d(REG67,updatewindow_copy);
    VoC_lw32_d(ACC0,state_wsize);
    VoC_sw32_d(REG67,state_write);
    VoC_sw32_d(ACC0,state_whave);

    VoC_jump(Coolsand_updatewindow_L6);
Coolsand_updatewindow_L3:

    VoC_lw32_d(ACC0,updatewindow_dist);

    VoC_add32_rd(ACC0,ACC0,state_write);

    VoC_bne32_rd(Coolsand_updatewindow_L4,ACC0,state_wsize)
    VoC_lw32z(ACC0,DEFAULT);

Coolsand_updatewindow_L4:
    VoC_lw32_d(ACC1,state_whave);
    VoC_sw32_d(ACC0,state_write);

    VoC_bnlt32_rd(Coolsand_updatewindow_L5,ACC1,state_wsize)
    VoC_add32_rd(ACC1,ACC1,updatewindow_dist);
    VoC_NOP();
Coolsand_updatewindow_L5:

    VoC_sw32_d(ACC1,state_whave);

Coolsand_updatewindow_L6:


    VoC_pop16(RA,DEFAULT);

    VoC_lw16i_short(REG7,0,DEFAULT);

    VoC_return;
}


/**************************************************************************************
 * Function:    Coolsand_zmemcpyFromVoC
 *
 * Description:
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/04/2012
 **************************************************************************************/
void Coolsand_zmemcpyFromVoC(void)
{
    //GLOBAL_DMATEMPBUF
    VoC_lw16i_set_inc(REG1,GLOBAL_DMATEMPBUF,1);
    VoC_lw16d_set_inc(REG2,zmemcpy_SrcAddr,1);

    VoC_lw32_d(REG67,zmemcpy_DstAddr);

    VoC_and16_ri(REG6,0x3);

    VoC_lw16_d(REG4,zmemcpy_Length);

    VoC_lw16i(REG3,256);

    VoC_bez16_r(Coolsand_zmemcpyFromVoC_L0,REG6)

    VoC_lw32_d(REG67,zmemcpy_DstAddr);

    VoC_and16_ri(REG6,0xfffc);

    VoC_and16_ri(REG7,0x0fff);

    VoC_or16_ri(REG7,((DMA_SINGLE|DMA_READ) >> 16));

    VoC_lw16i_short(REG5,1,DEFAULT);
    // single read
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_NOP();

    VoC_lw32_d(REG67,CFG_DMA_DATA_SINGLE);

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);
    VoC_sub16_rr(REG3,REG3,REG5,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);

Coolsand_zmemcpyFromVoC_L0:

    VoC_lw16_d(REG4,zmemcpy_Length);
Coolsand_zmemcpyFromVoC_L1:

    VoC_bgt16_rr(Coolsand_zmemcpyFromVoC_L2,REG3,REG4)

    VoC_loop_r(0,REG3)

    VoC_lw16inc_p(REG7,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG7,REG1,DEFAULT);

    VoC_endloop0

    VoC_lw16i(REG4,256/2);
    VoC_lw16i(REG5,GLOBAL_DMATEMPBUF/2);
    VoC_lw32_d(REG67,zmemcpy_DstAddr);

    VoC_and16_ri(REG6,0xfffc);
    VoC_and16_ri(REG7,0x0fff);
    VoC_or16_ri(REG7,((DMA_BURST|DMA_WRITE) >> 16));

    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_shru16_ri(REG4,-2,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_add32_rd(REG67,REG45,zmemcpy_DstAddr);

    VoC_sub16_dr(REG4,zmemcpy_Length,REG3);
    VoC_lw16i(REG3,256);
    VoC_sw16_d(REG4,zmemcpy_Length);

    VoC_and16_ri(REG6,0xfffc);

    VoC_lw16i_set_inc(REG1,GLOBAL_DMATEMPBUF,1);

    VoC_sw32_d(REG67,zmemcpy_DstAddr);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_jump(Coolsand_zmemcpyFromVoC_L1);
Coolsand_zmemcpyFromVoC_L2:

    VoC_bngtz16_r(Coolsand_zmemcpyFromVoC_L4,REG4)

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_movreg16(REG7,REG4,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw16i(REG5,GLOBAL_DMATEMPBUF);

    VoC_sub16_rr(REG7,REG1,REG5,DEFAULT);

    VoC_and16_ri(REG7,0x1);

    VoC_bez16_r(Coolsand_zmemcpyFromVoC_L3,REG7)

    VoC_lw16i_short(REG5,0,DEFAULT);

    VoC_shru32_ri(REG45,-1,DEFAULT);

    VoC_add32_rd(REG67,REG45,zmemcpy_DstAddr)

    VoC_and16_ri(REG6,0xfffc);

    VoC_and16_ri(REG7,0x0fff);

    VoC_or16_ri(REG7,((DMA_SINGLE|DMA_READ) >> 16));

    VoC_lw16i_short(REG5,1,DEFAULT);
    // single read
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_NOP();

    VoC_lw32_d(REG67,CFG_DMA_DATA_SINGLE);

    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);

    VoC_sw16inc_p(REG7,REG1,DEFAULT);

Coolsand_zmemcpyFromVoC_L3:

    VoC_lw16i(REG5,GLOBAL_DMATEMPBUF);

    VoC_sub16_rr(REG4,REG1,REG5,DEFAULT);

    VoC_shru16_ri(REG4,1,DEFAULT);
    VoC_shru16_ri(REG5,1,IN_PARALLEL);

    VoC_lw32_d(REG67,zmemcpy_DstAddr);

    VoC_and16_ri(REG6,0xfffc);
    VoC_and16_ri(REG7,0x0fff);
    VoC_or16_ri(REG7,((DMA_BURST|DMA_WRITE) >> 16));

    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

Coolsand_zmemcpyFromVoC_L4:


    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_zmemcpyToVoC
 *
 * Description:
 *
 * Inputs:      REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/04/2012
 **************************************************************************************/
void Coolsand_zmemcpyToVoC(void)
{


    VoC_lw16i_short(REG4,2,DEFAULT);

    VoC_add16_rd(REG4,REG4,zmemcpy_Length);

    VoC_shr16_ri(REG4,1,DEFAULT);

    VoC_lw32_d(REG67,zmemcpy_SrcAddr);

    VoC_lw16i(REG5,GLOBAL_DMATEMPBUF/2);

    VoC_and16_ri(REG6,0xfffc);
    VoC_and16_ri(REG7,0x0fff);
    VoC_or16_ri(REG7,((DMA_BURST|DMA_READ) >> 16));

    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_lw32_d(REG67,zmemcpy_SrcAddr);
    VoC_and16_ri(REG6,0x3);

    VoC_lw16i_set_inc(REG2,GLOBAL_DMATEMPBUF,1);

    VoC_bez16_r(Coolsand_zmemcpyToVoC_L0,REG6)

    VoC_inc_p(REG2,DEFAULT);

Coolsand_zmemcpyToVoC_L0:

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw16_d(REG4,zmemcpy_Length);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(REG5,20,IN_PARALLEL);


    VoC_bngt16_rr(Coolsand_zmemcpyToVoC_L1,REG4,REG5)

    VoC_loop_i_short(20,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG5,IN_PARALLEL);
    VoC_startloop0
    VoC_lw16inc_p(REG3,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0

Coolsand_zmemcpyToVoC_L1:

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG3,REG2,DEFAULT);
    VoC_NOP();
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0



    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_FilterRow
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *              REG2:PreviousRow
 *              REG3:FilterType
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_FilterRow(void)
{

    VoC_lw16i(REG4,FilterTypeNone);

    VoC_be16_rr(Coolsand_FilterRow_End,REG3,REG4)



    VoC_lw16i(REG4,FilterTypeSub);

    VoC_bne16_rr(Coolsand_FilterRow_case_FilterTypeSub,REG3,REG4)

    VoC_lw16i_short(REG4,7,DEFAULT);

    VoC_add16_rd(REG4,REG4,RowInfo_PixelDepth);

    VoC_shr16_ri(REG4,3,DEFAULT);

    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);

    VoC_lw16i_set_inc(REG3,0,-1);

    VoC_sub16_dr(REG3,RowInfo_RowBytes,REG4);

Coolsand_FilterRow_case_FilterTypeSub_L0:




    VoC_lw16i(REG5,256);

    VoC_bngt16_rr(Coolsand_FilterRow_case_FilterTypeSub_L1,REG3,REG5)

    VoC_loop_i(0,0);

    VoC_lw16inc_p(REG5,REG1,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG2,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_inc_p(REG3,DEFAULT);

    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_case_FilterTypeSub_L0);


Coolsand_FilterRow_case_FilterTypeSub_L1:

    VoC_bngtz16_r(Coolsand_FilterRow_End,REG3)

    VoC_loop_r_short(REG3,DEFAULT);
    VoC_startloop0

    VoC_lw16inc_p(REG5,REG1,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG2,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_inc_p(REG3,DEFAULT);

    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_End);

Coolsand_FilterRow_case_FilterTypeSub:


    VoC_lw16i(REG4,FilterTypeUp);

    VoC_bne16_rr(Coolsand_FilterRow_case_FilterTypeUp,REG3,REG4)

    VoC_lw16d_set_inc(REG3,RowInfo_RowBytes,-1);

Coolsand_FilterRow_case_FilterTypeUp_L0:

    VoC_lw16i(REG5,256);

    VoC_bngt16_rr(Coolsand_FilterRow_case_FilterTypeUp_L1,REG3,REG5)

    VoC_loop_i(0,0);

    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_inc_p(REG3,DEFAULT);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_case_FilterTypeUp_L0);

Coolsand_FilterRow_case_FilterTypeUp_L1:

    VoC_bngtz16_r(Coolsand_FilterRow_End,REG3)

    VoC_loop_r_short(REG3,DEFAULT);
    VoC_startloop0

    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_inc_p(REG3,DEFAULT);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_End);

Coolsand_FilterRow_case_FilterTypeUp:

    VoC_lw16i(REG4,FilterTypeAverage);

    VoC_bne16_rr(Coolsand_FilterRow_case_FilterTypeAverage,REG3,REG4)


    /*  REG1:   BYTE *dest = CurrentROW;
        REG2:   BYTE *src = PreviousRow;
        REG3:   BYTE*left = CurrentROW; */

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_movreg16(REG3,REG1,IN_PARALLEL);

    VoC_add16_rd(REG4,REG4,RowInfo_PixelDepth);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_shr16_ri(REG4,3,IN_PARALLEL);


    VoC_sub16_dr(REG7,RowInfo_RowBytes,REG4);

    VoC_loop_r_short(REG4,DEFAULT);
    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    VoC_shr16_ri(REG5,1,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_NOP();

    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    VoC_endloop0


Coolsand_FilterRow_case_FilterTypeAverage_L0:

    VoC_lw16i(REG5,256);

    VoC_bngt16_rr(Coolsand_FilterRow_case_FilterTypeAverage_L1,REG7,REG5)

    VoC_loop_i(0,0);

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);

    VoC_shr16_ri(REG5,1,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_case_FilterTypeAverage_L0);

Coolsand_FilterRow_case_FilterTypeAverage_L1:

    VoC_bngtz16_r(Coolsand_FilterRow_End,REG7)

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_add16inc_rp(REG5,REG5,REG3,DEFAULT);

    VoC_shr16_ri(REG5,1,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_sub16_rr(REG7,REG7,REG4,DEFAULT);

    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_End);
Coolsand_FilterRow_case_FilterTypeAverage:


    VoC_lw16i(REG4,FilterTypePaeth);

    VoC_bne16_rr(Coolsand_FilterRow_case_FilterTypePaeth,REG3,REG4)


    /*  REG1:   BYTE *dest = CurrentROW;
        REG2:   BYTE *src = PreviousRow;
        REG0:   BYTE *left = CurrentROW;
        REG3:   BYTE *up = PreviousRow;*/


    VoC_lw16i_short(REG4,7,DEFAULT);

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_add16_rd(REG4,REG4,RowInfo_PixelDepth);

    VoC_lw16i_short(INC3,1,DEFAULT);
    VoC_shr16_ri(REG4,3,IN_PARALLEL);


    VoC_sub16_dr(REG7,RowInfo_RowBytes,REG4);

    VoC_movreg16(REG0,REG1,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_loop_r_short(REG4,DEFAULT);

    VoC_startloop0
    VoC_lw16inc_p(REG5,REG2,DEFAULT);

    VoC_add16_rp(REG5,REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xff);

    VoC_NOP();

    VoC_sw16inc_p(REG5,REG1,DEFAULT);

    VoC_endloop0


Coolsand_FilterRow_case_FilterTypePaeth_L0:


    VoC_lw16i(REG5,256);

    VoC_bngt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L3,REG7,REG5)

    VoC_loop_i(0,0);

    //  a = *left++;
    //  b = *src++;
    //  c = *up++;

    VoC_lw16inc_p(REG4,REG0,DEFAULT);

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_movreg16(ACC1_LO,REG7,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    VoC_movreg32(ACC0,REG45,IN_PARALLEL);


    //  p = b - c;
    //  pc = a - c;

    VoC_sub16_rr(REG4,REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG5,REG4,REG6,IN_PARALLEL);

    //  pa = p < 0 ? -p : p;
    //  pb = pc < 0 ? -pc : pc;
    //  pc = (p + pc) < 0 ? -(p + pc) : p + pc;

    VoC_abs16_r(REG4,DEFAULT);
    VoC_add16_rr(REG7,REG4,REG5,IN_PARALLEL);

    VoC_abs16_r(REG5,DEFAULT);
    VoC_abs16_r(REG7,IN_PARALLEL);


    //p = (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;

    VoC_bgt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L1,REG5,REG7)
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);
Coolsand_FilterRow_case_FilterTypePaeth_L1:

    VoC_bgt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L2,REG4,REG5)
    VoC_bgt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L2,REG4,REG7)
    VoC_movreg16(REG6,ACC0_LO,DEFAULT);
Coolsand_FilterRow_case_FilterTypePaeth_L2:

    VoC_add16_rp(REG6,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_and16_ri(REG6,0xff);

    VoC_movreg16(REG7,ACC1_LO,DEFAULT);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);
    VoC_endloop0

    VoC_jump(Coolsand_FilterRow_case_FilterTypePaeth_L0);


Coolsand_FilterRow_case_FilterTypePaeth_L3:


    VoC_bngtz16_r(Coolsand_FilterRow_case_FilterTypePaeth_L6,REG7)

    VoC_loop_r_short(REG7,DEFAULT);
    VoC_startloop0

    //  a = *left++;
    //  b = *src++;
    //  c = *up++;

    VoC_lw16inc_p(REG4,REG0,DEFAULT);

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_movreg16(ACC1_LO,REG7,IN_PARALLEL);

    VoC_lw16inc_p(REG6,REG3,DEFAULT);
    VoC_movreg32(ACC0,REG45,IN_PARALLEL);


    //  p = b - c;
    //  pc = a - c;

    VoC_sub16_rr(REG4,REG5,REG6,DEFAULT);
    VoC_sub16_rr(REG5,REG4,REG6,IN_PARALLEL);

    //  pa = p < 0 ? -p : p;
    //  pb = pc < 0 ? -pc : pc;
    //  pc = (p + pc) < 0 ? -(p + pc) : p + pc;

    VoC_abs16_r(REG4,DEFAULT);
    VoC_add16_rr(REG7,REG4,REG5,IN_PARALLEL);

    VoC_abs16_r(REG5,DEFAULT);
    VoC_abs16_r(REG7,IN_PARALLEL);


    //p = (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;

    VoC_bgt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L4,REG5,REG7)
    VoC_movreg16(REG6,ACC0_HI,DEFAULT);
Coolsand_FilterRow_case_FilterTypePaeth_L4:

    VoC_bgt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L5,REG4,REG5)
    VoC_bgt16_rr(Coolsand_FilterRow_case_FilterTypePaeth_L5,REG4,REG7)
    VoC_movreg16(REG6,ACC0_LO,DEFAULT);
Coolsand_FilterRow_case_FilterTypePaeth_L5:

    VoC_add16_rp(REG6,REG6,REG1,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_and16_ri(REG6,0xff);

    VoC_movreg16(REG7,ACC1_LO,DEFAULT);

    VoC_sw16inc_p(REG6,REG1,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG4,IN_PARALLEL);

    VoC_endloop0

Coolsand_FilterRow_case_FilterTypePaeth_L6:

    VoC_lw16i_short(WRAP0,GLOBAL_WRAP_BIT,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);

    VoC_jump(Coolsand_FilterRow_End)
Coolsand_FilterRow_case_FilterTypePaeth:


    VoC_lw16i_short(REG7,0,DEFAULT);
    VoC_NOP();
    VoC_sw16_p(REG7,REG1,DEFAULT);

Coolsand_FilterRow_End:

    VoC_NOP();



    VoC_return;
}



/**************************************************************************************
 * Function:    Coolsand_FilterRow
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *              REG2:PreviousRow
 *              REG3:FilterType
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_ImageTransform(void)
{
    VoC_push16(RA,DEFAULT);


    /*      if (RowInfo.ColorType == IndexColor)
                ExpandPalette(Row + 1);//out put TrueColorAlpha
            else
                ExpandImage(Row + 1);//out put GreyScaleAlpha/TrueColorAlpha

    */

    VoC_lw16i(REG4,IndexColor);

    VoC_push16(REG1,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);
    VoC_bne16_rd(Coolsand_ImageTransform_L0,REG4,RowInfo_ColorType)
    VoC_inc_p(REG1,DEFAULT);
    VoC_jal(Coolsand_ExpandPalette);
    VoC_jump(Coolsand_ImageTransform_L1);
Coolsand_ImageTransform_L0:
    VoC_inc_p(REG1,DEFAULT);
    VoC_jal(Coolsand_ExpandImage);
Coolsand_ImageTransform_L1:

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

//  if (transform & GREY2RGB)
//     ExpandGrey2RGB(Row + 1);//GreyScaleAlpha-->TrueColorAlpha


    VoC_lw16_d(REG4,transform);

    VoC_and16_ri(REG4,GREY2RGB);

    VoC_bez16_r(Coolsand_ImageTransform_L2,REG4)


    VoC_push16(REG1,DEFAULT);
    VoC_lw16i_short(REG2,1,IN_PARALLEL);

    VoC_add16_rr(REG1,REG1,REG2,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);

    VoC_jal(Coolsand_ExpandGrey2RGB);

    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

Coolsand_ImageTransform_L2:


    //only TrueColorAlpha into BkgdProcess
//  if ((transform & DEFAULTbKGD) || (transform & bKGDEXPAND) || (IHDRInfo.ColorType & 4))
//      BkgdProcess(Row + 1,destRow);
    VoC_lw16i_short(INC1,1,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);


    VoC_lw16_d(REG4,transform);

    VoC_and16_ri(REG4,DEFAULTbKGD);

    VoC_inc_p(REG1,DEFAULT);
    VoC_push32(RL7,IN_PARALLEL);


    VoC_bez16_r(Coolsand_ImageTransform_L3,REG4)


    VoC_jal(Coolsand_BkgdProcess);


    VoC_jump(Coolsand_ImageTransform_L5);

Coolsand_ImageTransform_L3:

    VoC_lw16_d(REG4,transform);

    VoC_and16_ri(REG4,bKGDEXPAND);

    VoC_bez16_r(Coolsand_ImageTransform_L4,REG4)

    VoC_jal(Coolsand_BkgdProcess);
    VoC_jump(Coolsand_ImageTransform_L5);

Coolsand_ImageTransform_L4:


    VoC_lw16_d(REG4,IHDRInfo_ColorType);

    VoC_and16_ri(REG4,4);

    VoC_bez16_r(Coolsand_ImageTransform_L5,REG4)

    VoC_jal(Coolsand_BkgdProcess);

Coolsand_ImageTransform_L5:


    VoC_pop16(REG1,DEFAULT);
    VoC_pop32(RL7,IN_PARALLEL);

//  if (transform & DEPTH16to8)
//      Chop16to8(Row + 1);//16bit to 8bit

    VoC_lw16_d(REG4,transform);

    VoC_and16_ri(REG4,DEPTH16to8);

    VoC_bez16_r(Coolsand_ImageTransform_L6,REG4)

    VoC_jal(Coolsand_Chop16to8);

Coolsand_ImageTransform_L6:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;


}


/**************************************************************************************
 * Function:    Coolsand_FilterRow
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *              REG2:PreviousRow
 *              REG3:FilterType
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_CombineRow(void)
{
    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_lw16i_short(INC0,-1,DEFAULT);
    VoC_push16(RA,IN_PARALLEL);


    VoC_bez16_d(Coolsand_CombineRow_L1,IHDRInfo_Interlace)

    VoC_lw16_d(REG0,RowInfo_Width);

    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_jump(Coolsand_CombineRow_L6);
Coolsand_CombineRow_L1:

    VoC_lw16_d(REG4,png_resized_width)
    VoC_be16_rd(Coolsand_CombineRow_L2,REG4,IHDRInfo_Width)
    VoC_lw16_d(REG0,RowInfo_Width);

    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_jump(Coolsand_CombineRow_L6);
Coolsand_CombineRow_L2:

    VoC_lw16_d(REG4,png_resized_height)
    VoC_be16_rd(Coolsand_CombineRow_L3,REG4,IHDRInfo_Height)

    VoC_lw16_d(REG0,RowInfo_Width);

    VoC_lw16i_short(REG6,2,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_jump(Coolsand_CombineRow_L6);
Coolsand_CombineRow_L3:


    VoC_bnez16_d(Coolsand_CombineRow_L4,png_decode_mode)

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_add16_rd(REG4,REG4,PngCutOffsetX2);

    VoC_sub16_rd(REG0,REG4,PngCutOffsetX1);

    VoC_lw16_d(REG4,PngCutOffsetX1);

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);



    VoC_jump(Coolsand_CombineRow_L6);
Coolsand_CombineRow_L4:

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_bne16_rd(Coolsand_CombineRow_L5,REG4,png_decode_mode)

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_add16_rd(REG4,REG4,PngCutOffsetX2);

    VoC_sub16_rd(REG0,REG4,PngCutOffsetX1);

    VoC_lw16_d(REG4,PngCutOffsetX1);

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG6,1,IN_PARALLEL);


    VoC_jump(Coolsand_CombineRow_L6);
Coolsand_CombineRow_L5:

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_bne16_rd(Coolsand_CombineRow_L5_1,REG4,png_decode_mode)

    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_add16_rd(REG4,REG4,PngCutOffsetX2);

    VoC_sub16_rd(REG0,REG4,PngCutOffsetX1);

    VoC_lw16_d(REG4,PngCutOffsetX1);

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_add16_rr(REG1,REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG6,0,IN_PARALLEL);

    VoC_jump(Coolsand_CombineRow_L6);
Coolsand_CombineRow_L5_1:
    VoC_lw16i_short(REG0,0,DEFAULT);

    VoC_lw16i_short(REG6,1,DEFAULT);//U8 rgb565mod=1;
    VoC_inc_p(REG1,IN_PARALLEL);//BYTE *sp= Row + 1;
Coolsand_CombineRow_L6:


    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_movreg16(REG2,REG1,IN_PARALLEL);


    VoC_lw16i(REG4,0xff);

    VoC_sw16_d(REG2,zmemcpy_SrcAddr);

    VoC_bne16_rr(Coolsand_CombineRow_Lx,REG4,REG7)

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_bne16_rr(Coolsand_CombineRow_L8,REG4,REG6)

Coolsand_CombineRow_L7:
    VoC_lw16i(REG5,256);
    VoC_bngt16_rr(Coolsand_CombineRow_L7_0,REG0,REG5)

    VoC_loop_i(0,0)

    VoC_lw16inc_p(REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xf8);

    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-8,IN_PARALLEL);

    VoC_and16_ri(REG6,0xfc);

    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    VoC_shru16_ri(REG6,-3,IN_PARALLEL);

    VoC_and16_ri(REG7,0xf8)

    VoC_or16_rr(REG5,REG6,DEFAULT);
    VoC_shru16_ri(REG7,3,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_inc_p(REG0,DEFAULT);

    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_CombineRow_L7)

Coolsand_CombineRow_L7_0:
    VoC_bngtz16_r(Coolsand_CombineRow_End,REG0)

    VoC_loop_r_short(REG0,DEFAULT);
    VoC_startloop0

    VoC_lw16inc_p(REG5,REG1,DEFAULT);

    VoC_and16_ri(REG5,0xf8);

    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_shru16_ri(REG5,-8,IN_PARALLEL);

    VoC_and16_ri(REG6,0xfc);

    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    VoC_shru16_ri(REG6,-3,IN_PARALLEL);

    VoC_and16_ri(REG7,0xf8)

    VoC_or16_rr(REG5,REG6,DEFAULT);
    VoC_shru16_ri(REG7,3,IN_PARALLEL);

    VoC_or16_rr(REG5,REG7,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_inc_p(REG0,DEFAULT);

    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_endloop0
    VoC_jump(Coolsand_CombineRow_End);

Coolsand_CombineRow_L8:


    VoC_lw16i_short(REG1,2,DEFAULT);

    VoC_bne16_rr(Coolsand_CombineRow_L10,REG4,REG6)

Coolsand_CombineRow_L9:
    VoC_bngtz16_r(Coolsand_CombineRow_End,REG0)

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG4,-8,IN_PARALLEL);

    VoC_or16_rr(REG4,REG5,DEFAULT)
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    VoC_shru16_ri(REG6,-8,IN_PARALLEL);

    VoC_or16_rr(REG6,REG7,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);

    VoC_inc_p(REG0,DEFAULT);

    VoC_sw16inc_p(REG6,REG2,DEFAULT);

    VoC_jump(Coolsand_CombineRow_L9)
Coolsand_CombineRow_L10:

    VoC_lw16i_short(REG1,0,DEFAULT);
    VoC_bne16_rr(Coolsand_CombineRow_End,REG4,REG6)

Coolsand_CombineRow_L11:
    VoC_bngtz16_r(Coolsand_CombineRow_End,REG0)

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG5,REG1,DEFAULT);
    VoC_shru16_ri(REG4,-8,IN_PARALLEL);

    VoC_or16_rr(REG4,REG5,DEFAULT)
    VoC_lw16inc_p(REG6,REG1,IN_PARALLEL);

    VoC_lw16inc_p(REG7,REG1,DEFAULT);
    VoC_shru16_ri(REG6,-8,IN_PARALLEL);

    VoC_or16_rr(REG6,REG7,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,IN_PARALLEL);

    VoC_inc_p(REG0,DEFAULT);

    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_jump(Coolsand_CombineRow_L11);
Coolsand_CombineRow_Lx:
    VoC_NOP();
Coolsand_CombineRow_End:
    VoC_lw16_d(REG3,zmemcpy_SrcAddr);

    VoC_sub16_rr(REG2,REG2,REG3,DEFAULT);

    VoC_shru16_ri(REG2,1,DEFAULT);

    VoC_sw16_d(REG2,zmemcpy_Length);
    VoC_sw16_d(REG3,zmemcpy_SrcAddr);
    VoC_sw32_d(RL7,zmemcpy_DstAddr);

    VoC_jal(Coolsand_zmemcpyFromVoC);



    VoC_lw16i_short(WRAP0,GLOBAL_WRAP_BIT,DEFAULT);
    VoC_pop16(RA,IN_PARALLEL);

    VoC_lw16i_short(INC0,1,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);


    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ExpandPalette
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_ExpandPalette(void)
{

//  VoC_push16(RA,DEFAULT);


    VoC_lw16i_short(REG7,IndexColor,DEFAULT);
    VoC_lw16_d(REG6,RowInfo_ColorType);

    VoC_bne16_rr(Coolsand_ExpandPalette_End,REG6,REG7)

    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_push16(REG1,IN_PARALLEL);

    VoC_lw16_d(REG6,RowInfo_BitDepth);

    VoC_bngt16_rr(Coolsand_ExpandPalette_LessThan_8,REG7,REG6)

    /*
                case 1:
                    {
                        sp = Row + (UINT32)((RowWidth - 1) >> 3);
                        dp = Row + (UINT32)RowWidth - 1;
                        shift = 7 - (int)((RowWidth + 7) & 0x07);

                        for (i = 0; i < RowWidth; i++)
                        {
                            if ((*sp >> shift) & 0x01)
                                *dp = 1;
                            else
                                *dp = 0;

                            if (shift == 7)
                            {
                                shift = 0;
                                sp--;
                            }
                            else
                                shift++;

                            dp--;
                        }

                        break;
                    }
    */


    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_bne16_rr(Coolsand_ExpandPalette_LessThan_8_Case_1,REG6,REG7)


    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i_short(REG6,7,IN_PARALLEL);

    VoC_lw16_d(REG7,RowInfo_Width);

    VoC_sub16_rr(REG2,REG7,REG5,DEFAULT);
    VoC_add16_rr(REG4,REG7,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG2,3,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_and16_rr(REG4,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG1,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG1,IN_PARALLEL);

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);


Coolsand_ExpandPalette_LessThan_8_Case_1_L0:

    VoC_bngtz16_r(Coolsand_ExpandPalette_LessThan_8_Case_4,REG7)

    VoC_lw16_p(REG1,REG2,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG5,IN_PARALLEL);

    VoC_shr16_rr(REG1,REG4,DEFAULT);

    VoC_and16_rr(REG1,REG5,DEFAULT);
    VoC_lw16i_short(REG6,7,IN_PARALLEL);

    VoC_bne16_rr(Coolsand_ExpandPalette_LessThan_8_Case_1_L1,REG4,REG6)
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_jump(Coolsand_ExpandPalette_LessThan_8_Case_1_L2);
Coolsand_ExpandPalette_LessThan_8_Case_1_L1:
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);

Coolsand_ExpandPalette_LessThan_8_Case_1_L2:

    VoC_sw16inc_p(REG1,REG3,DEFAULT);

    VoC_jump(Coolsand_ExpandPalette_LessThan_8_Case_1_L0)

Coolsand_ExpandPalette_LessThan_8_Case_1:
    /*

                case 2:
                    {
                        sp = Row + (UINT32)((RowWidth - 1) >> 2);
                        dp = Row + (UINT32)RowWidth - 1;
                        shift = (int)((3 - ((RowWidth + 3) & 0x03)) << 1);

                        for (i = 0; i < RowWidth; i++)
                        {
                            value = (*sp >> shift) & 0x03;
                            *dp = (BYTE)value;

                            if (shift == 6)
                            {
                                shift = 0;
                                sp--;
                            }
                            else
                                shift += 2;

                            dp--;
                        }

                        break;
                    }
    */



    VoC_lw16i_short(REG7,2,DEFAULT);

    VoC_bne16_rr(Coolsand_ExpandPalette_LessThan_8_Case_2,REG6,REG7)

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_lw16_d(REG7,RowInfo_Width);

    VoC_sub16_rr(REG2,REG7,REG5,DEFAULT);
    VoC_add16_rr(REG4,REG7,REG6,IN_PARALLEL);

    VoC_shr16_ri(REG2,2,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_and16_rr(REG4,REG6,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG1,IN_PARALLEL);

    VoC_sub16_rr(REG4,REG6,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG1,IN_PARALLEL);

    VoC_shr16_ri(REG4,-1,DEFAULT);
    VoC_lw16i_short(REG5,2,IN_PARALLEL);

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_lw16i_short(REG1,1,DEFAULT);

Coolsand_ExpandPalette_LessThan_8_Case_2_L0:

    VoC_bngtz16_r(Coolsand_ExpandPalette_LessThan_8_Case_4,REG7)

    VoC_lw16_p(REG1,REG2,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG1,IN_PARALLEL);

    VoC_shr16_rr(REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_and16_rr(REG1,REG6,DEFAULT);
    VoC_lw16i_short(REG6,6,IN_PARALLEL);

    VoC_bne16_rr(Coolsand_ExpandPalette_LessThan_8_Case_2_L1,REG4,REG6)
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_jump(Coolsand_ExpandPalette_LessThan_8_Case_2_L2);
Coolsand_ExpandPalette_LessThan_8_Case_2_L1:
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);

Coolsand_ExpandPalette_LessThan_8_Case_2_L2:

    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);


    VoC_jump(Coolsand_ExpandPalette_LessThan_8_Case_2_L0)



Coolsand_ExpandPalette_LessThan_8_Case_2:

    /*
                case 4:
                    {
                        sp = Row + (UINT32)((RowWidth - 1) >> 1);
                        dp = Row + (UINT32)RowWidth - 1;
                        shift = (int)((RowWidth & 0x01) << 2);

                        for (i = 0; i < RowWidth; i++)
                        {
                            value = (*sp >> shift) & 0x0f;
                            *dp = (BYTE)value;

                            if (shift == 4)
                            {
                                shift = 0;
                                sp--;
                            }
                            else
                                shift += 4;

                            dp--;
                        }

                        break;
                    }
    */

    VoC_lw16i_short(REG7,4,DEFAULT);

    VoC_bne16_rr(Coolsand_ExpandPalette_LessThan_8_Case_4,REG6,REG7)

    VoC_lw16i_short(REG5,1,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_lw16_d(REG7,RowInfo_Width);

    VoC_sub16_rr(REG2,REG7,REG5,DEFAULT);
    VoC_movreg16(REG4,REG7,IN_PARALLEL);

    VoC_shr16_ri(REG2,1,DEFAULT);
    VoC_movreg16(REG3,REG2,IN_PARALLEL);

    VoC_and16_rr(REG4,REG5,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG1,IN_PARALLEL);

    VoC_shr16_ri(REG4,-2,DEFAULT);
    VoC_add16_rr(REG3,REG3,REG1,IN_PARALLEL);

    VoC_lw16i_short(REG1,1,DEFAULT);
    VoC_lw16i_short(REG5,4,IN_PARALLEL);

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

Coolsand_ExpandPalette_LessThan_8_Case_4_L0:

    VoC_bngtz16_r(Coolsand_ExpandPalette_LessThan_8_Case_4,REG7)

    VoC_lw16_p(REG1,REG2,DEFAULT);
    VoC_sub16_rr(REG7,REG7,REG1,IN_PARALLEL);

    VoC_shr16_rr(REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG6,0xf,IN_PARALLEL);

    VoC_and16_rr(REG1,REG6,DEFAULT);

    VoC_bne16_rr(Coolsand_ExpandPalette_LessThan_8_Case_4_L1,REG4,REG5)
    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL);
    VoC_jump(Coolsand_ExpandPalette_LessThan_8_Case_4_L2);
Coolsand_ExpandPalette_LessThan_8_Case_4_L1:
    VoC_add16_rr(REG4,REG4,REG5,DEFAULT);

Coolsand_ExpandPalette_LessThan_8_Case_4_L2:

    VoC_sw16inc_p(REG1,REG3,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    VoC_jump(Coolsand_ExpandPalette_LessThan_8_Case_4_L0)

Coolsand_ExpandPalette_LessThan_8_Case_4:

    VoC_lw16i_short(REG6,8,DEFAULT);
    VoC_lw16_d(REG7,RowInfo_Width);

    VoC_sw16_d(REG6,RowInfo_BitDepth);
    VoC_sw16_d(REG6,RowInfo_PixelDepth);
    VoC_sw16_d(REG7,RowInfo_Width);


Coolsand_ExpandPalette_LessThan_8:


    VoC_lw16i_short(REG7,8,DEFAULT);
    VoC_pop16(REG1,IN_PARALLEL);

    VoC_lw16_d(REG6,RowInfo_BitDepth);

    VoC_bne16_rr(Coolsand_ExpandPalette_End,REG7,REG6)

    VoC_lw16_d(REG6,TransNum);
    VoC_bez16_r(Coolsand_ExpandPalette_case_8_TransNum,REG6)


    /*
                        BYTE    *tmp_p;
                        BYTE    *tmp_p1;
                        UINT32 data_tmp;

                        sp = Row + (UINT32)RowWidth - 1;
                        dp = Row + (UINT32)(RowWidth << 2) - 1;
                        tmp_p = (BYTE *)Palete;

                        for (i = RowWidth; i >0; i--)
                        {
                            data_tmp = *sp--;

                            if (data_tmp >= TransNum)
                                *dp-- = 0xff;
                            else
                                *dp-- = tRNSIndex[data_tmp];

                            tmp_p1 = tmp_p + 3*data_tmp;

                            *dp-- = *(tmp_p1+2);
                            *dp-- = *(tmp_p1+1);
                            *dp-- = *tmp_p1;
                        }

                        RowInfo.BitDepth = 8;
                        RowInfo.PixelDepth = 32;
                        RowInfo.RowBytes = RowWidth << 2;
                        RowInfo.ColorType = TrueColorAlpha;
                        RowInfo.Channels = 4;
    */

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_lw16_d(REG5,RowInfo_Width)

    VoC_sub16_rr(REG6,REG5,REG7,DEFAULT);
    VoC_shr16_ri(REG5,-2,IN_PARALLEL);

    VoC_add16_rr(REG2,REG1,REG6,DEFAULT);
    VoC_sub16_rr(REG4,REG5,REG7,IN_PARALLEL);

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG4,IN_PARALLEL);

    VoC_lw16i(REG7,0xff);

    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_lw16_d(REG5,RowInfo_Width);

Coolsand_ExpandPalette_case_8_TransNum_L1:
    VoC_bngtz16_r(Coolsand_ExpandPalette_case_8_TransNum_L3,REG5)

    VoC_lw16_p(REG4,REG2,DEFAULT);
    VoC_mult16_rp(REG6,REG6,REG2,IN_PARALLEL);

    VoC_bnlt16_rd(Coolsand_ExpandPalette_case_8_TransNum_L2,REG4,TransNum)
    VoC_lw16i(REG7,tRNSIndex);
    VoC_add16_rr(REG7,REG4,REG7,DEFAULT);
    VoC_NOP();
    VoC_lw16_p(REG7,REG7,DEFAULT);
Coolsand_ExpandPalette_case_8_TransNum_L2:

    VoC_lw16i(REG4,Palete+2);

    VoC_add16_rr(REG1,REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG4,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG1,DEFAULT);

    VoC_lw16inc_p(REG6,REG1,DEFAULT);

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_inc_p(REG2,IN_PARALLEL)

    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG7,-1,IN_PARALLEL);

    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_shru16_ri(REG7,8,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_jump(Coolsand_ExpandPalette_case_8_TransNum_L1);
Coolsand_ExpandPalette_case_8_TransNum_L3:

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(REG4,32,IN_PARALLEL);

    VoC_lw16_d(REG5,RowInfo_Width);

    VoC_shr16_ri(REG5,-2,DEFAULT);

    VoC_lw16i_short(REG6,TrueColorAlpha,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

    VoC_sw16_d(REG3,RowInfo_BitDepth);
    VoC_sw16_d(REG4,RowInfo_PixelDepth);
    VoC_sw16_d(REG5,RowInfo_RowBytes);
    VoC_sw16_d(REG6,RowInfo_ColorType);
    VoC_sw16_d(REG7,RowInfo_Channels);

    VoC_jump(Coolsand_ExpandPalette_End);
Coolsand_ExpandPalette_case_8_TransNum:

    /*
                        BYTE    *tmp_p;
                        BYTE    *tmp_p1;
                        UINT32 data_tmp;

                        sp = Row + (UINT32)RowWidth - 1;
                        dp = Row +(UINT32)(RowWidth << 2) - 1;
                        tmp_p = (BYTE *)Palete;

                        for (i = RowWidth; i >0; i--)
                        {
                            data_tmp = *sp--;
                            tmp_p1 = tmp_p + 3*data_tmp;

                            *dp-- = 0xff;
                            *dp-- = *(tmp_p1+2);
                            *dp-- = *(tmp_p1+1);
                            *dp-- = *tmp_p1;
                        }

                        RowInfo.BitDepth = 8;
                        RowInfo.PixelDepth = 24;
                        RowInfo.RowBytes = RowWidth + (RowWidth << 1);
                        RowInfo.ColorType = TrueColorAlpha;
                        RowInfo.Channels = 4;
    */

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_lw16_d(REG5,RowInfo_Width)

    VoC_sub16_rr(REG6,REG5,REG7,DEFAULT);
    VoC_shr16_ri(REG5,-2,IN_PARALLEL);

    VoC_add16_rr(REG2,REG1,REG6,DEFAULT);
    VoC_sub16_rr(REG4,REG5,REG7,IN_PARALLEL);

    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG4,IN_PARALLEL);

    VoC_lw16i(REG7,0xff);

    VoC_lw16i_short(REG6,3,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

Coolsand_ExpandPalette_case_8_L01:
    VoC_bnlt16_rd(Coolsand_ExpandPalette_case_8_L02,REG5,RowInfo_Width)
    VoC_lw16i_short(INC1,-1,DEFAULT);
    VoC_mult16inc_rp(REG4,REG6,REG2,IN_PARALLEL);

    VoC_lw16i(REG6,Palete+2);

    VoC_add16_rr(REG1,REG4,REG6,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_add16_rr(REG5,REG5,REG4,IN_PARALLEL);

    VoC_lw16inc_p(REG4,REG1,DEFAULT);
    VoC_lw16inc_p(REG6,REG1,DEFAULT);
    VoC_lw16inc_p(REG4,REG1,DEFAULT);

    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_lw16i_short(REG6,3,IN_PARALLEL);

    VoC_jump(Coolsand_ExpandPalette_case_8_L01);
Coolsand_ExpandPalette_case_8_L02:

    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(REG4,24,IN_PARALLEL);

    VoC_lw16_d(REG5,RowInfo_Width);

    VoC_shr16_ri(REG5,-1,DEFAULT);
    VoC_movreg16(REG6,REG5,IN_PARALLEL);

    VoC_add16_rr(REG5,REG5,REG6,DEFAULT);

    VoC_lw16i_short(REG6,TrueColorAlpha,DEFAULT);
    VoC_lw16i_short(REG7,4,IN_PARALLEL);

    VoC_sw16_d(REG3,RowInfo_BitDepth);
    VoC_sw16_d(REG4,RowInfo_PixelDepth);
    VoC_sw16_d(REG5,RowInfo_RowBytes);
    VoC_sw16_d(REG6,RowInfo_ColorType);
    VoC_sw16_d(REG7,RowInfo_Channels);

Coolsand_ExpandPalette_End:

//  VoC_pop16(RA,DEFAULT);
//  VoC_NOP();
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_ExpandImage
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *              REG2:PreviousRow
 *              REG3:FilterType
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_ExpandImage(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16i_short(REG7,GreyScale,IN_PARALLEL);

    VoC_lw16_d(REG6,RowInfo_ColorType);

    VoC_bne16_rr(Coolsand_ExpandImage_GreyScale,REG6,REG7)





    VoC_jump(Coolsand_ExpandImage_End);
Coolsand_ExpandImage_GreyScale:
    VoC_lw16i_short(REG7,TrueColor,DEFAULT);

    VoC_bne16_rr(Coolsand_ExpandImage_TrueColor,REG6,REG7)

    VoC_lw16i_short(REG7,1,DEFAULT);

    VoC_lw16_d(REG5,RowInfo_Width);

    VoC_sub16_dr(REG6,RowInfo_RowBytes,REG7);

    VoC_shr16_ri(REG5,-2,DEFAULT);
    VoC_add16_rr(REG2,REG1,REG6,IN_PARALLEL);

    VoC_lw16i_short(INC2,-1,DEFAULT);
    VoC_sub16_rr(REG5,REG5,REG7,IN_PARALLEL);


    VoC_lw16i_short(INC3,-1,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG5,IN_PARALLEL);


    VoC_lw16i_set_inc(REG1,0,1);

Coolsand_ExpandImage_TrueColor_L0:

    VoC_bnlt16_rd(Coolsand_ExpandImage_TrueColor_L2,REG1,RowInfo_Width)

    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_lw16inc_p(REG6,REG2,DEFAULT);
    VoC_lw16inc_p(REG7,REG2,DEFAULT);

    VoC_lw16i(REG4,0xff);

    VoC_bez16_d(Coolsand_ExpandImage_TrueColor_L1,TransNum)
    VoC_bne16_rd(Coolsand_ExpandImage_TrueColor_L1,REG7,tRNSInfo_tRNSRed)
    VoC_bne16_rd(Coolsand_ExpandImage_TrueColor_L1,REG6,tRNSInfo_tRNSGreen)
    VoC_bne16_rd(Coolsand_ExpandImage_TrueColor_L1,REG5,tRNSInfo_tRNSBlue)

    VoC_lw16i_short(REG4,0,DEFAULT);
    VoC_NOP();

Coolsand_ExpandImage_TrueColor_L1:

    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG6,REG3,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_inc_p(REG1,IN_PARALLEL);

    VoC_jump(Coolsand_ExpandImage_TrueColor_L0)

Coolsand_ExpandImage_TrueColor_L2:







    VoC_NOP();





Coolsand_ExpandImage_TrueColor:

    VoC_NOP();

Coolsand_ExpandImage_End:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_ExpandGrey2RGB
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_ExpandGrey2RGB(void)
{

    //  if (RowInfo.BitDepth >= 8 && !(RowInfo.ColorType & 2))

    VoC_lw16i_short(REG2,8,DEFAULT);
    VoC_lw16_d(REG3,RowInfo_BitDepth);

    VoC_bgt16_rr(Coolsand_ExpandGrey2RGB_End,REG2,REG3)

    VoC_lw16_d(REG3,RowInfo_ColorType);
    VoC_and16_ri(REG3,2);
    VoC_bnez16_r(Coolsand_ExpandGrey2RGB_End,REG3)


    VoC_lw16i_short(REG2,GreyScale,DEFAULT);
    VoC_lw16i_short(REG3,8,IN_PARALLEL);

    VoC_lw16_d(REG4,RowInfo_ColorType);
    VoC_bne16_rr(Coolsand_ExpandGrey2RGB_GreyScale,REG2,REG4)





    VoC_jump(Coolsand_ExpandGrey2RGB_GreyScaleAlpha);
Coolsand_ExpandGrey2RGB_GreyScale:


    VoC_lw16i_short(REG2,GreyScaleAlpha,DEFAULT);
    VoC_lw16i_short(REG3,8,IN_PARALLEL);

    VoC_lw16_d(REG4,RowInfo_ColorType);
    VoC_bne16_rr(Coolsand_ExpandGrey2RGB_GreyScaleAlpha,REG2,REG4)

    VoC_lw16_d(REG4,RowInfo_BitDepth);
    VoC_bne16_rr(Coolsand_ExpandGrey2RGB_GreyScaleAlpha_L2,REG3,REG4)

    VoC_lw16_d(REG4,RowInfo_Width);

    VoC_shru16_ri(REG4,-1,DEFAULT);
    VoC_movreg16(REG6,REG4,IN_PARALLEL);

    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_lw16i_short(REG5,1,IN_PARALLEL);

    VoC_sub16_rr(REG2,REG2,REG5,DEFAULT);
    VoC_lw16i_short(INC2,-1,IN_PARALLEL);

    VoC_add16_rr(REG3,REG2,REG4,DEFAULT);
    VoC_lw16i_short(INC3,-1,IN_PARALLEL);

    VoC_lw16i(REG7,256);

Coolsand_ExpandGrey2RGB_GreyScaleAlpha_L0:
    VoC_bngt16_rr(Coolsand_ExpandGrey2RGB_GreyScaleAlpha_L1,REG6,REG7)
    VoC_sub16_rr(REG6,REG6,REG7,DEFAULT);

    VoC_loop_i(0,0)
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_ExpandGrey2RGB_GreyScaleAlpha_L0);

Coolsand_ExpandGrey2RGB_GreyScaleAlpha_L1:

    VoC_bngtz16_r(Coolsand_ExpandGrey2RGB_GreyScaleAlpha,REG6)

    VoC_loop_r_short(REG6,DEFAULT)
    VoC_startloop0
    VoC_lw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_sw16inc_p(REG5,REG3,DEFAULT);
    VoC_endloop0

    VoC_jump(Coolsand_ExpandGrey2RGB_GreyScaleAlpha);
Coolsand_ExpandGrey2RGB_GreyScaleAlpha_L2:


    VoC_NOP();


Coolsand_ExpandGrey2RGB_GreyScaleAlpha:

    VoC_lw16i_short(REG3,2,DEFAULT);
    VoC_add16_rd(REG4,REG3,RowInfo_Channels);

    VoC_mult16_rd(REG6,REG4,RowInfo_BitDepth);

    VoC_lw16_d(REG5,RowInfo_ColorType);
    VoC_or16_rr(REG5,REG3,DEFAULT);

    VoC_sw16_d(REG4,RowInfo_Channels);
    VoC_sw16_d(REG5,RowInfo_ColorType);


    VoC_lw16i_short(REG3,8,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);



    VoC_multf32_rd(ACC0,REG6,RowInfo_Width);

    VoC_lw16i_short(REG4,7,DEFAULT);
    VoC_lw16i_short(REG5,0,IN_PARALLEL);

    VoC_bngt16_rd(Coolsand_ExpandGrey2RGB_L0,REG3,RowInfo_BitDepth)

    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

Coolsand_ExpandGrey2RGB_L0:

    VoC_shr32_ri(ACC0,3,DEFAULT);

    VoC_sw16_d(REG6,RowInfo_PixelDepth);

    VoC_sw32_d(ACC0,RowInfo_RowBytes);

Coolsand_ExpandGrey2RGB_End:


    VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_BkgdProcess
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *              REG2:PreviousRow
 *              REG3:FilterType
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_BkgdProcess(void)
{
    VoC_lw16_d(REG6,RowInfo_Width);
    VoC_lw16i(REG4,GreyScale);
    VoC_lw16_d(REG5,RowInfo_ColorType);
    VoC_sw16_d(REG6,BkgdProcess_RowWidth);
    VoC_bne16_rr(Coolsand_BkgdProcess_case_GreyScale,REG4,REG5)

    VoC_jump(Coolsand_BkgdProcess_End)
Coolsand_BkgdProcess_case_GreyScale:

    VoC_lw16i(REG4,TrueColor);
    VoC_lw16_d(REG5,RowInfo_ColorType);
    VoC_bne16_rr(Coolsand_BkgdProcess_case_TrueColor,REG4,REG5)

    VoC_jump(Coolsand_BkgdProcess_End)
Coolsand_BkgdProcess_case_TrueColor:

    VoC_lw16i(REG4,GreyScaleAlpha);
    VoC_lw16_d(REG5,RowInfo_ColorType);
    VoC_bne16_rr(Coolsand_BkgdProcess_case_GreyScaleAlpha,REG4,REG5)



    VoC_jump(Coolsand_BkgdProcess_End)
Coolsand_BkgdProcess_case_GreyScaleAlpha:

    VoC_lw16i(REG4,TrueColorAlpha);
    VoC_lw16_d(REG5,RowInfo_ColorType);
    VoC_bne16_rr(Coolsand_BkgdProcess_case_TrueColorAlpha,REG4,REG5)

    VoC_lw16i_short(REG4,8,DEFAULT);

    VoC_lw16_d(REG5,RowInfo_BitDepth);

    VoC_bne16_rr(Coolsand_BkgdProcess_case_TrueColorAlpha_Lx,REG4,REG5)

    //          sp = Row;
    //          dp = Row;

    VoC_movreg16(REG2,REG1,DEFAULT);
    VoC_movreg16(REG3,REG1,IN_PARALLEL);


    VoC_lw16_d(REG4,transform);

    VoC_and16_ri(REG4,DEFAULTbKGD);

    VoC_lw16_d(REG5,DefaultBkgd_Red);
    VoC_lw16_d(REG6,DefaultBkgd_Green);
    VoC_lw16_d(REG7,DefaultBkgd_Blue);

    VoC_bnez16_r(Coolsand_BkgdProcess_case_TrueColorAlpha_L0,REG4)

    VoC_lw16_d(REG5,bKGDInfo_bKGDRed);
    VoC_lw16_d(REG6,bKGDInfo_bKGDGreen);
    VoC_lw16_d(REG7,bKGDInfo_bKGDBlue);

Coolsand_BkgdProcess_case_TrueColorAlpha_L0:

    VoC_sw16_d(REG5,BkgdProcess_data_tmp0);
    VoC_sw16_d(REG6,BkgdProcess_data_tmp1);
    VoC_sw16_d(REG7,BkgdProcess_data_tmp2);

    VoC_lw16_d(REG5,BkgdProcess_RowWidth);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_bez16_d(Coolsand_BkgdProcess_case_TrueColorAlpha_L1,IHDRInfo_Interlace)

    VoC_lw16_d(REG5,RowInfo_Width);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);

    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L6);
Coolsand_BkgdProcess_case_TrueColorAlpha_L1:

    VoC_lw16_d(REG4,png_resized_width)
    VoC_be16_rd(Coolsand_BkgdProcess_case_TrueColorAlpha_L2,REG4,IHDRInfo_Width)
    VoC_lw16_d(REG5,RowInfo_Width);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);

    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L6);
Coolsand_BkgdProcess_case_TrueColorAlpha_L2:

    VoC_lw16_d(REG4,png_resized_height)
    VoC_be16_rd(Coolsand_BkgdProcess_case_TrueColorAlpha_L3,REG4,IHDRInfo_Height)

    VoC_lw16_d(REG5,RowInfo_Width);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,3,IN_PARALLEL);

    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L6);
Coolsand_BkgdProcess_case_TrueColorAlpha_L3:


    /*          else if(png_decode_mode==0)
                {
                    RowWidth = PngCutOffsetX2- PngCutOffsetX1+ 1;
                    bkgrdNeed= 1;
                    alphaNeed= 0;
                    sp= Row+ (PngCutOffsetX1<<2);//RGB alpha offset
                    dp= sp;//keep RGB alpha
                }
    */

    VoC_bnez16_d(Coolsand_BkgdProcess_case_TrueColorAlpha_L4,png_decode_mode)

    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_add16_rd(REG4,REG4,PngCutOffsetX2);

    VoC_sub16_rd(REG5,REG4,PngCutOffsetX1);

    VoC_lw16i_short(REG6,1,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_lw16_d(REG4,PngCutOffsetX1);

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG4,IN_PARALLEL);


    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L6);
Coolsand_BkgdProcess_case_TrueColorAlpha_L4:

    VoC_lw16i_short(REG4,1,DEFAULT);
    VoC_bne16_rd(Coolsand_BkgdProcess_case_TrueColorAlpha_L5,REG4,png_decode_mode)

    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_add16_rd(REG4,REG4,PngCutOffsetX1);

    VoC_sub16_dr(REG5,PngCutOffsetX2,REG4);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,1,IN_PARALLEL);

    VoC_lw16_d(REG4,PngCutOffsetX1);

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG4,IN_PARALLEL);

    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L6);
Coolsand_BkgdProcess_case_TrueColorAlpha_L5:

    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_bne16_rd(Coolsand_BkgdProcess_case_TrueColorAlpha_L6,REG4,png_decode_mode)

    VoC_lw16i_short(REG4,1,DEFAULT);

    VoC_add16_rd(REG4,REG4,PngCutOffsetX1);

    VoC_sub16_dr(REG5,PngCutOffsetX2,REG4);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i_short(REG7,2,IN_PARALLEL);

    VoC_lw16_d(REG4,PngCutOffsetX1);

    VoC_shr16_ri(REG4,-2,DEFAULT);

    VoC_add16_rr(REG2,REG1,REG4,DEFAULT);
    VoC_add16_rr(REG3,REG1,REG4,IN_PARALLEL);

Coolsand_BkgdProcess_case_TrueColorAlpha_L6:

    VoC_sw16_d(REG5,BkgdProcess_RowWidth);
    VoC_sw16_d(REG6,BkgdProcess_bkgrdNeed);
    VoC_sw16_d(REG7,BkgdProcess_alphaNeed);

    VoC_lw16i_short(REG4,3,DEFAULT);

    VoC_be16_rr(Coolsand_BkgdProcess_End,REG4,REG7)

    VoC_bnez16_r(Coolsand_BkgdProcess_case_TrueColorAlpha_Lx2,REG7)

    VoC_lw16i_short(REG4,2,DEFAULT);

    VoC_add16_rd(REG4,REG4,BkgdProcess_RowWidth);

    VoC_shr16_ri(REG4,1,DEFAULT);
    VoC_movreg32(REG67,RL7,IN_PARALLEL);

    VoC_lw16i(REG5,GLOBAL_DMATEMPBUF/2);

    VoC_and16_ri(REG6,0xfffc);
    VoC_and16_ri(REG7,0x0fff);
    VoC_or16_ri(REG7,((DMA_BURST|DMA_READ) >> 16));

    VoC_sw16_d(REG5,CFG_DMA_LADDR);
    VoC_sw16_d(REG4,CFG_DMA_SIZE);
    VoC_sw32_d(REG67,CFG_DMA_EADDR);

    VoC_movreg32(REG67,RL7,DEFAULT);

    VoC_and16_ri(REG6,0x3);

    VoC_lw16i_set_inc(REG1,GLOBAL_DMATEMPBUF,1);

    VoC_bez16_r(Coolsand_BkgdProcess_case_TrueColorAlpha_L7,REG6)

    VoC_inc_p(REG1,DEFAULT);

Coolsand_BkgdProcess_case_TrueColorAlpha_L7:

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw16i_short(WRAP0,0,DEFAULT);
    VoC_push16(REG0,IN_PARALLEL);

    VoC_lw16_d(REG5,BkgdProcess_data_tmp0);
    VoC_lw16_d(REG6,BkgdProcess_data_tmp1);
    VoC_lw16_d(REG7,BkgdProcess_data_tmp2);

    VoC_lw16i_short(INC2,1,DEFAULT);
    VoC_lw16i_short(REG0,0,IN_PARALLEL);

Coolsand_BkgdProcess_case_TrueColorAlpha_L8:



    VoC_lw16_d(REG4,BkgdProcess_RowWidth);
    VoC_bngt16_rr(Coolsand_BkgdProcess_case_TrueColorAlpha_L13,REG4,REG0)

    VoC_lw16i_short(REG4,3,DEFAULT);

    VoC_add16_rr(REG3,REG2,REG4,DEFAULT);

    VoC_lw16_d(REG4,BkgdProcess_bkgrdNeed);

    VoC_lw16_p(REG3,REG3,DEFAULT);
    VoC_lw16i_short(FORMAT32,0,IN_PARALLEL);

    VoC_bez16_r(Coolsand_BkgdProcess_case_TrueColorAlpha_L9,REG4)

    VoC_lw16i(REG4,0xff);

    VoC_be16_rr(Coolsand_BkgdProcess_case_TrueColorAlpha_L9,REG3,REG4)

    VoC_lw16_p(REG5,REG1,DEFAULT);
    VoC_lw16_p(REG6,REG1,IN_PARALLEL);

    VoC_and16_ri(REG5,0xf800);
    VoC_and16_ri(REG6,0x7e0);

    VoC_shru16_ri(REG5,8,DEFAULT);
    VoC_lw16inc_p(REG7,REG1,IN_PARALLEL);

    VoC_and16_ri(REG7,0x1f);

    VoC_shru16_ri(REG6,3,DEFAULT);
    VoC_shru16_ri(REG7,-3,IN_PARALLEL);


    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L9_0)
Coolsand_BkgdProcess_case_TrueColorAlpha_L9:
    VoC_inc_p(REG1,DEFAULT);
Coolsand_BkgdProcess_case_TrueColorAlpha_L9_0:



    VoC_bnez16_r(Coolsand_BkgdProcess_case_TrueColorAlpha_L10,REG3)

    VoC_lw16i_short(REG4,0,DEFAULT);

    VoC_bez16_d(Coolsand_BkgdProcess_case_TrueColorAlpha_L9_1,GLOBAL_alpha_blending_to_transparentColor)

    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_lw16i(REG4,0xff);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_sw16inc_p(REG4,REG2,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);

    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L12);
Coolsand_BkgdProcess_case_TrueColorAlpha_L9_1:


    VoC_sw16inc_p(REG5,REG2,DEFAULT);
    VoC_sw16inc_p(REG6,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_inc_p(REG2,DEFAULT);
    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L12);
Coolsand_BkgdProcess_case_TrueColorAlpha_L10:

    VoC_lw16i(REG4,0xff);
    VoC_be16_rr(Coolsand_BkgdProcess_case_TrueColorAlpha_L11,REG3,REG4);


    VoC_push16(REG2,DEFAULT);
    VoC_push32(REG67,IN_PARALLEL);

    VoC_push32(REG01,DEFAULT);
    VoC_sub16_rr(REG4,REG4,REG3,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG3,REG2,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG5,IN_PARALLEL);

    VoC_lw16i(REG0,128);

    VoC_multf32inc_rp(ACC0,REG3,REG2,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG6,IN_PARALLEL);

    VoC_add32_rr(RL7,ACC0,ACC1,DEFAULT);
    VoC_lw16i_short(REG1,0,IN_PARALLEL);

    VoC_multf32inc_rp(ACC0,REG3,REG2,DEFAULT);
    VoC_multf32_rr(ACC1,REG4,REG7,IN_PARALLEL);

    VoC_add32_rr(REG67,ACC0,ACC1,DEFAULT);
    VoC_add32_rr(RL7,RL7,REG01,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,ACC1,DEFAULT);
    VoC_add32_rr(REG67,REG67,REG01,IN_PARALLEL);

    VoC_add32_rr(ACC0,ACC0,REG01,DEFAULT);
    VoC_shru32_ri(RL7,8,IN_PARALLEL);

    VoC_pop16(REG2,DEFAULT);
    VoC_pop32(REG01,IN_PARALLEL);

    VoC_shru32_ri(REG67,8,DEFAULT);
    VoC_shru32_ri(ACC0,8,IN_PARALLEL);

    VoC_sw16inc_p(RL7_LO,REG2,DEFAULT);

    VoC_sw16inc_p(REG6,REG2,DEFAULT);

    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);

    VoC_inc_p(REG2,DEFAULT);
    VoC_pop32(REG67,IN_PARALLEL);

    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L12);
Coolsand_BkgdProcess_case_TrueColorAlpha_L11:
    VoC_lw16i_short(REG4,4,DEFAULT);
    VoC_add16_rr(REG2,REG2,REG4,DEFAULT);
Coolsand_BkgdProcess_case_TrueColorAlpha_L12:

    VoC_inc_p(REG0,DEFAULT);
    VoC_jump(Coolsand_BkgdProcess_case_TrueColorAlpha_L8)

Coolsand_BkgdProcess_case_TrueColorAlpha_L13:

    VoC_lw16i_short(WRAP0,GLOBAL_WRAP_BIT,DEFAULT);
    VoC_pop16(REG0,IN_PARALLEL);


    VoC_jump(Coolsand_BkgdProcess_End);
Coolsand_BkgdProcess_case_TrueColorAlpha_Lx2:

    VoC_lw16i_short(REG4,1,DEFAULT)

    VoC_bne16_rr(Coolsand_BkgdProcess_case_TrueColorAlpha_Lx1,REG4,REG7)




    VoC_jump(Coolsand_BkgdProcess_End);
Coolsand_BkgdProcess_case_TrueColorAlpha_Lx1:


    VoC_lw16i_short(REG4,2,DEFAULT);
    VoC_bne16_rr(Coolsand_BkgdProcess_End,REG4,REG7)





    VoC_jump(Coolsand_BkgdProcess_End);
Coolsand_BkgdProcess_case_TrueColorAlpha_Lx:






    VoC_jump(Coolsand_BkgdProcess_End)
Coolsand_BkgdProcess_case_TrueColorAlpha:



    VoC_NOP();

Coolsand_BkgdProcess_End:

    VoC_NOP();

    VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_Chop16to8
 *
 * Description:
 *
 * Inputs:
 *              REG1:CurrentROW
 *              REG2:PreviousRow
 *              REG3:FilterType
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/07/2012
 **************************************************************************************/
void Coolsand_Chop16to8(void)
{

    VoC_return;

}


/**************************************************************************************
 * Function:    Coolsand_fixedtables
 *
 * Description:
 *
 * Inputs:
 *
 *              REG0:input bitsteam
 *              BITCACHE:cache bit
 *              RL6:cache data
 *
 *
 *
 * Outputs:
 *
 * Used : All.
 *
 * Version 1.0  Create by  Xuml       05/16/2012
 **************************************************************************************/
void Coolsand_fixedtables(void)
{

    VoC_lw16i_short(REG6,9,DEFAULT);
    VoC_lw16i_short(REG7,5,IN_PARALLEL);

    VoC_push16(RA,DEFAULT);
    VoC_lw32z(REG45,IN_PARALLEL);

    VoC_sw16_d(REG6,state_lenbits);
    VoC_sw16_d(REG7,state_distbits);

    //voc_short state_codes_op[ENOUGH];
    //voc_short state_codes_bits[ENOUGH];
    //voc_short state_codes_val[ENOUGH];

    VoC_lw16i(REG4,(512+32)*2);


    // DMA op

    VoC_lw16i(REG2,state_codes_op/2);
    VoC_lw16i(REG3,(512+32)/2);
    VoC_lw32_d(ACC0,GLOBAL_CONST_LENFIX_DISTFIX_PTR);

    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);


    // DMA bits


    VoC_lw16i(REG2,state_codes_bits/2);
    VoC_lw16i(REG3,(512+32)/2);
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);


    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    // DMA bits


    VoC_lw16i(REG2,state_codes_val/2);
    VoC_lw16i(REG3,(512+32)/2);
    VoC_add32_rr(ACC0,ACC0,REG45,DEFAULT);

    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);

    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);

    VoC_lw16i_short(REG6,0,DEFAULT);
    VoC_lw16i(REG7,512);
    VoC_sw16_d(REG6,state_lencode);
    VoC_sw16_d(REG7,state_distcode);


    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;

}

