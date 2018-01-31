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
















#ifndef vpp_RM_DEC_ASM_H
#define vpp_RM_DEC_ASM_H

#ifndef SHEEN_VC_DEBUG
#include "cs_types.h"
#endif

#define VPP_RMVB_CFG_STRUCT                   (3244  + 0x0000)


#define    VOC_PROCESS_NON  0
#define    VOC_PROCESS_RM       1


typedef struct
{
    INT16 intra16_16;
    INT16 intra;
    INT16 top;
    INT16 left;

    INT16 topright;
    INT16 mb_offsetx;

    INT16 bidir_pred;
    INT16 mb_pskip;
    INT16 ratio0;
    INT16 ratio1;

    INT16 inter16;
    INT16 inter16_16;
    INT16 dequant_y_qp;
    INT16 dequant_y_qdc0;

    INT16 dequant_y_qdc;
    INT16 dequant_c_qdc;
    INT16 dequant_c_qp;

    INT16 defilter_alpha;
    INT16 defilter_beta;
    INT16 defilter_beta2;
    INT16 defilter_beta2_c;

    INT16 defilter_mbclip0;
    INT16 defilter_mbclip2;

    INT16 defilter_mbclip;
    INT16 defilter_mbclip_left;
    INT16 defilter_mbclip_up;

    INT16 defilter_refdiff_up;
    INT16 defilter_refdiff_left;

    INT32 defilter_bnzs_up;
    INT32 defilter_bnzs_left;
    INT32 defilter_bnzs;

    INT32 defilter_bh_filter;
    INT32 defilter_bv_filter;
    INT32 defilter_h_filter;
    INT32 defilter_v_filter;

    INT32 mb_cbp;

    INT16 vld_cof[400];
    INT16 intra_mode[16];
    INT16 motion_vectors[16];

    INT32 interplate_yblk_addr[4];
    INT32 interplate_ublk_addr[4];
    INT32 interplate_vblk_addr[4];

    INT32 interplate_yblk_b_addr[4];
    INT32 interplate_ublk_b_addr[4];
    INT32 interplate_vblk_b_addr[4];

    INT32 out_buffer_y_ptr;
    INT32 out_buffer_u_ptr;
    INT32 out_buffer_v_ptr;

} VPP_RMVB_DATA_T;

// ============================================================================
// VPP_RMVB_ADDR_T
// ----------------------------------------------------------------------------
/// VPP RMVB configuration structure
// ============================================================================

typedef struct
{
    INT16 reset;
    INT16 status;
    INT16 mb_num;
    INT16 rv8_flag;

    INT16 pic_width;
    INT16 pic_height;
    INT16 pic_pitch;
    INT16 filter_pass;

    INT32 para_buf_ptr;
    INT32 contex_buf_ptr;

} VPP_RMVB_CFG_T;


#endif
