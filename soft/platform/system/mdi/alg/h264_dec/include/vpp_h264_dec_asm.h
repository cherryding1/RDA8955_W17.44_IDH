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




#ifndef VPP_H264_DEC_ASM_H
#define VPP_H264_DEC_ASM_H

#include "cs_types.h"
#define RAM_X_BEGIN_ADDR               0x0000
#define RAM_Y_BEGIN_ADDR               0x4000
#include "voc_map_addr.h"

//#define BINARY_SIM

#ifdef SHEEN_VC_DEBUG

#define VOC_CFG_DMA_EADDR_MASK (0xFFFFFFFF)

#else

#define VOC_CFG_DMA_EADDR_MASK (0xFFFFFF<<2)
//#define GLOBAL_H264_MB_DEC_NUM_ADDR                       (3248 + 0x0000)
//#define VPP_H264_DEC_DATA_EXT_ADDR                        (3250 + 0x0000)

#endif



/*
these const table below will write into
vpp_h264_dec_const_y.tab

INT16 ref2frm[34]; //const table
UINT16 alpha_table[156];// const table, uint8 [52*3]=156 byte-->156 short
UINT16 beta_table[156];// const table, uint8 [52*3]=156 byte-->156 short
UINT16 tc0_table[468];// const table, uint8 [52*3][3]=468 byte-->468 short

*/
#if 0//part voc
//must be 4byte align for dma
typedef struct
{
    INT32 vpp_h264_data_flag_addr;
    INT16 simple;
    INT16 deblocking_filter;

//UINT16 voc_linesize;//block width for voc process
//UINT16 voc_uvlinesize;//block height for voc process

    INT16 chroma_pred_mode;
    INT16 intra16x16_pred_mode;
//INT32 mb_addr;//mb data after vld
    INT16 chroma_qp0;
    INT16 chroma_qp1;

    UINT32 topright_samples_available;
    INT16 img_linesize;//image real linesize
    INT16 img_uvlinesize;//image real uvlinesize

    INT16 mb_x;
    INT16 mb_y;
    INT32 img_y_dest;//mb addr to voc
    INT32 img_cb_dest;//mb addr to voc
    INT32 img_cr_dest;//mb addr to voc
//UINT32 mb_type;
//UINT32 mb_type_left;
//UINT32 mb_type_top;
    UINT16 mb_type;
    UINT16 mb_type_left;
    UINT16 mb_type_top;
//for dblk
    INT16 qp;//from int8_t *qscale_table; //mb_array_size * sizeof(uint8_t)
    INT16 qp_left;//from int8_t *qscale_table; //mb_array_size * sizeof(uint8_t)
    INT16 qp_top;//from int8_t *qscale_table; //mb_array_size * sizeof(uint8_t)
//for dblk end

    UINT32 qmul_y;
    UINT32 qmul_cb;
    UINT32 qmul_cr;
    INT32 left_border_addr;
    INT32 top_border_addr;

//Big data
    INT16 mb[16*24];//mb data after vld
    UINT16 non_zero_count_cache[6*8];
//INT16 intra4x4_pred_mode_cache[5*8];
    INT16 intra4x4_pred_mode_cache[16];

//for dblk
    /******************
    common parameters
    *******************/
//INT32 slice_alpha_c0_offset;
//INT32 slice_beta_offset;
//INT32 chroma_qp_index_offset[2];
    INT16 slice_alpha_c0_offset;
    INT16 slice_beta_offset;
    INT16 chroma_qp_index_offset[2];
    /*******************
    every mb parameters
    ********************/
//INT32 qp;//from int8_t *qscale_table; //mb_array_size * sizeof(uint8_t)
//INT32 qp_left;//from int8_t *qscale_table; //mb_array_size * sizeof(uint8_t)
//INT32 qp_top;//from int8_t *qscale_table; //mb_array_size * sizeof(uint8_t)
//INT32 chroma_qp[2];//0= cb, 1= cr
//INT32 chroma_qp_left[2];//0= cb, 1= cr ,from get_chroma_qp()
//INT32 chroma_qp_top[2];//0= cb, 1= cr  ,from get_chroma_qp()
    INT16 chroma_qp_left[2];//0= cb, 1= cr ,from get_chroma_qp()
    INT16 chroma_qp_top[2];//0= cb, 1= cr  ,from get_chroma_qp()


    UINT16 cur_mb_slice_num;//from slice_table, 8bit->16bit
    UINT16 left_mb_slice_num;//from slice_table, 8bit->16bit
    UINT16 top_mb_slice_num;//from slice_table, 8bit->16bit
    INT16 slice_type;

//INT16 mv_cache[160];//int16 [2][5*8][2]=160 short valid
//INT16 ref_cache[80];//int8 [2][5*8]=80 byte-->80 short
    INT16 mv_cache[80];//int16 [2][5*8][2]=160 short valid, no B frame or error
    INT16 ref_cache[40];//int8 [2][5*8]=80 byte-->80 short, no B frame or error
//dblk end

//for mc
    INT32 ref_addr_y[16];//reference block dma addr, only use last one frame in baseline.
    INT32 ref_addr_cb[16];//reference block dma addr.
    INT32 ref_addr_cr[16];//reference block dma addr.
    INT16 ref_x_exp[2];//expand pixel at edge for luma 16x16 16x8 chroma 8x8 8x4, high bit 0=right 1=left
    INT16 ref_y_exp[2];//expand pixel at edge for luma 16x16 8*16 chroma 8x8 4*8, high bit 0=bottom, 1=top
    INT16 reserve[2];//for error
    INT16 mv_x[16];//mv in voc block
    INT16 mv_y[16];//
    INT16 sub_mb_type[4];
//mc end
    UINT32 topright_border_addr;
//INT16 reserve2[2];//for 8byte algin

} VPP_H264_DEC_DATA;
#endif

#if 0//sheen
typedef struct
{

//INT16 s_gb_buf[1024*2+256];
    INT16 s_gb_buf[1024*5];

    INT16 s_gb_index;
    INT16 s_gb_cachebits;

    INT32 s_gb_size_in_bits;

    INT16 mb_skip_run;
    INT16 slice_type;

    INT16 simple;
    INT16 deblocking_filter;

    INT16 s_picture_structure;
    INT16 s_mb_stride;


    INT16 h_mb_aff_frame;
    INT16 h_slice_num;

    INT16 h_pps_constrained_intra_pred;
    INT16 h_pps_transform_8x8_mode;


    INT16 s_qscale;
    INT16 h_list_count;

    INT16 s_width;
    INT16 s_height;


    INT16 s_mb_width;
    INT16 s_mb_height;

    INT16  ref_count[2];

    INT16 img_linesize;//image real linesize
    INT16 img_uvlinesize;//image real uvlinesize

    INT16 mb_x;
    INT16 mb_y;

    INT16 slice_alpha_c0_offset;
    INT16 slice_beta_offset;
    INT16 chroma_qp_index_offset[2];

//mc end

} VPP_H264_IN_DADA;

typedef struct
{

    INT32 coeff_token_vlc[4];
    INT32 chroma_dc_coeff_token_vlc;

    INT32 run_vlc[6];
    INT32 run7_vlc;

    INT32 total_zeros_vlc[15];
    INT32 chroma_dc_total_zeros_vlc[3];

    INT32 dequant4_coeff[6];
    INT32 current_picture_data[3];
//INT32 topright_borders;
    INT32 left_border;
    INT32 top_borders;
    INT32 non_zero_count;
    INT32 intra4x4_pred_mode;
    INT32 pps_chroma_qp[2];

    INT32 current_picture_motion_val[2];
    INT32 current_picture_ref_index[2];
    INT32  ref_list[3*5];
} VPP_H264_EXTERN_ADDR;
#endif

#endif

