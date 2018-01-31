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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _DECODER_H_
#define _DECODER_H_

#include "xvid.h"
#include "portab.h"
#include "mpeg4_global.h"
#include "image.h"
//#include "postprocessing.h"

/*****************************************************************************
 * Structures
 ****************************************************************************/
typedef struct
{
    int mode;
    int quant;                  /* absolute quant */
    VECTOR mvs[4];
    VECTOR amv;                /* average motion vectors from GMC  */

    /* This structure has become way to big! What to do? Split it up?   */

}
MACROBLOCK;

typedef struct
{
    /* vol bitstream */

    //int16_t time_inc_resolution;
    uint16_t time_inc_resolution;//must be unsigned refer to ISO/IEC-14496-2:2001, sheen
    int16_t fixed_time_inc;
    uint32_t time_inc_bits;

    int16_t shape;
    int16_t ver_id;
    int16_t mode;
    int16_t quant_bits;
    int16_t quant_type;

    int16_t aspect_ratio;
    int16_t par_width;
    int16_t par_height;
    int16_t bs_version;
    int interlacing;
    uint32_t top_field_first;
    uint32_t alternate_vertical_scan;
    /* image */

    int16_t fixed_dimensions;
    int16_t width;
    int16_t height;
    int16_t edged_width;
    int16_t edged_height;
    IMAGE cur;
    IMAGE refn[3];
    //IMAGE refn[1];                /* 0   -- last I or P VOP */
    /* 1   -- first I or P */
    IMAGE tmp;      // bframe interpolation, and post processing tmp buffer

    /* macroblock */
    uint16_t mb_width;
    uint16_t mb_height;
    MACROBLOCK *mbs;
    //MACROBLOCK *last_mbs;
    int16_t *last_mbs_mode;
    VECTOR4 *last_mbs_mv;
    int16_t last_coding_type;           /* last coding type value */
    int32_t frames;             /* total frame number */
    VECTOR p_fmv,p_bmv;     /* pred forward & backward motion vector */
    int32_t time;               /* for record time */
    int32_t time_base;
    int32_t last_time_base;
    int32_t last_non_b_time;
    int32_t time_pp;
    int32_t time_bp;
    uint16_t low_delay;         /* low_delay flage (1 means no B_VOP) */
    uint16_t low_delay_default; /* default value for low_delay flag */

    xvid_image_t* out_frm;                /* This is used for slice rendering */
    /* Tells if the reference image is edged or not */
    int16_t is_edged[2];//yongheng
    //int16_t is_edged[1];
    VECTOR *mv_buffer_base;
    //VECTOR *bmv_buffer_base;
    VECTOR *mv_buffer;
    VECTOR mvs[4];
    VECTOR bmvs[4];
    VECTOR amv;

    int16_t *ac_pred_base;
    int16_t *ac_pred_above_y;
    int16_t *ac_pred_left_y;
    int16_t *ac_pred_above_u;
    int16_t *ac_pred_left_u;
    int16_t *ac_pred_above_v;
    int16_t *ac_pred_left_v;

    int16_t *qscale;
    int16_t *qscale_current;
    int16_t *qscale_above;
    int16_t *qscale_left;

    int16_t *block;
    int16_t *data;
    int16_t block_size;
    uint16_t stride[6];
    uint8_t *y_row_start;
    uint8_t *u_row_start;
    uint8_t *v_row_start;

    int sprite_enable;
    int sprite_warping_points;
    int sprite_warping_accuracy;
    int sprite_brightness_change;
    NEW_GMC_DATA new_gmc_data;
    uint16_t *mpeg_quant_matrices;
    int32_t quarterpel;

}
DECODER;
/*****************************************************************************
 * Decoder prototypes
 ****************************************************************************/

void init_decoder(uint32_t cpu_flags);

int decoder_create(xvid_dec_create_t * param);
int decoder_destroy(DECODER * dec);
int decoder_decode(DECODER * dec,
                   xvid_dec_frame_t * frame, xvid_dec_stats_t * stats, zoomPar *pZoom);

typedef void (DECMBINTER) (DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x,int16_t y,uint16_t cbp,
                           Bitstream *bs,int fcode,int rounding);
typedef DECMBINTER *DECMBINTER_PTR;
void decoder_mbgmc(DECODER * dec, VECTOR *pMVBuffPred[3], uint8_t **block,const uint32_t x_pos, const uint32_t y_pos, const uint32_t fcode,
                   const uint32_t cbp, Bitstream * bs, const uint32_t rounding);
void dec_mb_inter1v(DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x,int16_t y,uint16_t cbp,
                    Bitstream *bs,int fcode,int rounding);
void dec_mb_inter4v(DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x,int16_t y,uint16_t cbp,
                    Bitstream *bs,int fcode,int rounding);
static void decoder_bf_interpolate_mbinter(DECODER *dec,uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
        Bitstream *bs,int rounding);
void dec_bf_mb_inter1v(DECODER *dec,uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                       Bitstream *bs,int rounding,int ref);

#endif
#endif

