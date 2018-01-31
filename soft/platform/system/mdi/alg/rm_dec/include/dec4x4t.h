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



#ifndef DEC4X4T_H__
#define DEC4X4T_H__

/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
/* 4614 frames processed */
#include "dec4x4m.h"

typedef struct
{
    unsigned long intra_cbp[MAX_INTRA_QP_REGIONS][2][(MAX_CBP+7)/8];
    unsigned long intra_8x8_dsc[MAX_INTRA_QP_REGIONS][2][4][(MAX_8x8_DSC+7)/8];
    unsigned long intra_luma_4x4_dsc[MAX_INTRA_QP_REGIONS][3][(MAX_4x4_DSC+7)/8];
    unsigned long intra_luma_2x2_dsc[MAX_INTRA_QP_REGIONS][2][(MAX_2x2_DSC+7)/8];
    unsigned long intra_chroma_4x4_dsc[MAX_INTRA_QP_REGIONS][(MAX_4x4_DSC+7)/8];
    unsigned long intra_chroma_2x2_dsc[MAX_INTRA_QP_REGIONS][2][(MAX_2x2_DSC+7)/8];
    unsigned long intra_level_dsc[MAX_INTRA_QP_REGIONS][(MAX_LEVEL_DSC+7)/8];
    unsigned long inter_cbp[MAX_INTER_QP_REGIONS][(MAX_CBP+7)/8];
    unsigned long inter_8x8_dsc[MAX_INTER_QP_REGIONS][4][(MAX_8x8_DSC+7)/8];
    unsigned long inter_luma_4x4_dsc[MAX_INTER_QP_REGIONS][(MAX_4x4_DSC+7)/8];
    unsigned long inter_luma_2x2_dsc[MAX_INTER_QP_REGIONS][2][(MAX_2x2_DSC+7)/8];
    unsigned long inter_chroma_4x4_dsc[MAX_INTER_QP_REGIONS][(MAX_4x4_DSC+7)/8];
    unsigned long inter_chroma_2x2_dsc[MAX_INTER_QP_REGIONS][2][(MAX_2x2_DSC+7)/8];
    unsigned long inter_level_dsc[MAX_INTER_QP_REGIONS][(MAX_LEVEL_DSC+7)/8];
} rv_table_x;

extern const rv_table_x _x_rv;

extern const int intra_qp_to_idx [MAX_QP+1];

extern const int inter_qp_to_idx [MAX_QP+1];

extern const unsigned int _y[8] ;

#endif