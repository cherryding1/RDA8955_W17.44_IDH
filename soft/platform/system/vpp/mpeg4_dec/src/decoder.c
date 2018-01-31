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





#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <cyg/kernel/kapi.h>

#ifdef BFRAMES_DEC_DEBUG
#define BFRAMES_DEC
#endif

#include "xvid.h"
#include "portab.h"
#include "mpeg4_global.h"

#include "decoder.h"
#include "mpeg4_bitstream.h"
#include "mbcoding.h"

#include "quant.h"
#include "idct.h"

#include "mem_transfer.h"
#include "interpolate8x8.h"
//#include "image/font.h"


#include "mbcoding.h"
#include "mbprediction.h"
//#include "utils/timer.h"


#include "image.h"
//#include "colorspace.h"
//#include "postprocessing.h"
#include "mem_align.h"

#include "xvid_hw.h"
//#include "../include/global_macros.h"
//#include "../include/lp_ct810reg.h"

extern uint32_t roundtab_76[16];
extern uint32_t roundtab_79[16];

extern void mpeg4_simple_idct_put(uint8 *dest, int line_size, int16 *block);
extern void mpeg4_simple_idct_add(uint8 *dest, int line_size, int16 *block);

//static cyg_mutex_t vld_access_mutex;

#define DIV2ROUND(n)  (((n)>>1)|((n)&1))
#define DIV2(n)       ((n)>>1)
#define DIVUVMOV(n) (((n) >> 1) + roundtab_79[(n) & 0x3]) //

#if 0 //move to xvid.c for overlay
//add by longyl
const uint8_t MPEG_DC_SCALAR[2][32] =
{
    {
        0, 8, 8, 8, 8, //0-4
        9, 9, 10, 10, 11, //5-9
        11, 12, 12, 13, 13, //10-14
        14, 14, 15, 15, 16, //15-19
        16, 17, 17, 18, 18, //20-24
        19, 20, 21, 22, 23, //25-29
        24, 25              //30-32
    },
    {
        0, 8, 8, 8, 8, //0-4
        10, 12, 14, 16, 17, //5-9
        18, 19, 20, 21, 22, //10-14
        23, 24, 25, 26, 27, //15-19
        28, 29, 30, 31, 32, //20-24
        34, 36, 38, 40, 42, //25-29
        44, 46
    }              //30-32
};
const uint16_t MPEG_DC_SCALAR_MULT[2][32] =
{
    {
        0x0000, 0x4001, 0x4001, 0x4001, 0x4001, //0-4
        0x38e4, 0x38e4, 0x3334, 0x3334, 0x2e8c, //5-9
        0x2e8c, 0x2aab, 0x2aab, 0x2763, 0x2763, //10-14
        0x2493, 0x2493, 0x2223, 0x2223, 0x2001, //15-19
        0x2001, 0x1e1f, 0x1e1f, 0x1c72, 0x1c72, //20-24
        0x1af3, 0x199a, 0x1862, 0x1746, 0x1643, //25-29
        0x1556, 0x147b              //30-32
    },
    {
        0x0000, 0x4001, 0x4001, 0x4001, 0x4001, //0-4
        0x3334, 0x2aab, 0x2493, 0x2001, 0x1e1f, //5-9
        0x1c72, 0x1af3, 0x199a, 0x1862, 0x1746, //10-14
        0x1643, 0x1556, 0x147b, 0x13b2, 0x12f7, //15-19
        0x124a, 0x11a8, 0x1112, 0x1085, 0x1001, //20-24
        0x0f10, 0x0e39, 0x0d7a, 0x0ccd, 0x0c31, //25-29
        0x0ba3, 0x0b22
    }              //30-32
};

const uint16_t iQuant_MULT[32] =
{
    0x0000,0x8001,0x4001,0x2aab,
    0x2001,0x199a,0x1556,0x124a,
    0x1001,0x0e39,0x0ccd,0x0ba3,
    0x0aab,0x09d9,0x0925,0x0889,

    0x0801,0x0788,0x071d,0x06bd,
    0x0667,0x0619,0x05d2,0x0591,
    0x0556,0x051f,0x04ed,0x04be,
    0x0493,0x046a,0x0445,0x0422
};
#endif
extern uint8_t MPEG_DC_SCALAR[2][32];
extern uint16_t MPEG_DC_SCALAR_MULT[2][32];
extern uint16_t iQuant_MULT[32];

static int32_t s32rc[4] = {0, 1, 1, 2};

static void
interpolate8x8_switch(uint8_t * cur,
                      uint8_t * refn,
                      int32_t dx,
                      int dy,
                      int32_t stride,
                      int32_t rounding)
{
    uint8_t *refn_mbsrc;
    int32_t pred_type = ((dx&0x1) << 1) + (dy&0x1);
    static INTERPOLATE8X8_PTR interplolate8x8_array[4] = {mp4_transfer8x8_copy_c,mp4_interpolate8x8_halfpel_v_c,mp4_interpolate8x8_halfpel_h_c,mp4_interpolate8x8_halfpel_hv_c};
    refn_mbsrc = refn + (dy >> 1)*stride + (dx >> 1);
    interplolate8x8_array[pred_type]((uint8_t *)cur,refn_mbsrc,stride,s32rc[pred_type]-rounding);
}

static void
interpolate16x16_switch(uint8_t * cur,
                        uint8_t * refn,
                        int32_t dx,
                        int32_t dy,
                        int32_t stride,
                        int32_t rounding)
{
    uint8_t *refn_mbsrc;
    int32_t pred_type = ((dx&0x1) << 1) + (dy&0x1);
    static INTERPOLATE8X8_PTR interpolate16x16_array[4] = {transfer16x16_copy_c,interpolate16x16_halfpel_v_c,interpolate16x16_halfpel_h_c,interpolate16x16_halfpel_hv_c};
    refn_mbsrc = refn + (dy >> 1)*stride + (dx >> 1);
    interpolate16x16_array[pred_type]((uint8_t *)cur,refn_mbsrc,stride,s32rc[pred_type]-rounding);
}

#if 0 //move to xvid.c for overlay
static void BitstreamInit(Bitstream * const bs,
                          void *const bitstream,
                          uint32_t length)
{
    bs->tail = bs->start = (int8 *)bitstream;
    bs->buf = ((*bs->tail)<<24)|((*(bs->tail+1))<<16)|((*(bs->tail+2))<<8)|(*(bs->tail+3));
    bs->tail += 4;
    bs->pos = 32;
    bs->length = length;
}



//
////////////test by longyl
//FILE * file_test;
//int macroblock_number;
//uint32_t frame_number;
//////////////////////////////////
static int
decoder_resize(DECODER * dec)
{
    int temp_size;
    /* free existing */
    image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
    image_destroy(&dec->refn[0], dec->edged_width, dec->edged_height);
//  image_destroy(&dec->tmp, dec->edged_width, dec->edged_height);

    image_null(&dec->cur);
    image_null(&dec->refn[0]);
//  image_null(&dec->tmp);


    xvid_free(dec->ac_pred_base);
    xvid_free(dec->mv_buffer);
    dec->ac_pred_base = NULL;
    dec->mv_buffer = NULL;
    xvid_free(dec->qscale);
    dec->qscale = NULL;
    /* realloc */
    dec->mb_width = (dec->width + 15) >> 4;
    dec->mb_height = (dec->height + 15) >> 4;

    dec->edged_width =  (short)((dec->mb_width << 4) + (EDGE_SIZE << 1));
    dec->edged_height = (short)((dec->mb_height << 4) + (EDGE_SIZE << 1));
    dec->stride[0] = dec->stride[1] = dec->stride[2] = dec->stride[3] = dec->edged_width;
    dec->stride[4] = dec->stride[5] = dec->edged_width >> 1;
    if (image_create(&dec->cur, dec->edged_width, dec->edged_height)
            || image_create(&dec->refn[0], dec->edged_width, dec->edged_height))
        //    || image_create(&dec->tmp, dec->edged_width, dec->edged_height)   )
        goto memory_error;

    /* nothing happens if that fails */
    temp_size = sizeof(int16_t) * (dec->mb_width + 2);
    dec->qscale =
        xvid_malloc(temp_size, CACHE_LINE);
    if (dec->qscale)
        memset(dec->qscale, 0, temp_size);
    dec->qscale_current = dec->qscale;
    dec->qscale_above = dec->qscale + 1;
    dec->qscale_left = dec->qscale + dec->mb_width + 1;

    temp_size = (sizeof(int16_t) * (dec->mb_width+2)) << 5;
    dec->ac_pred_base = (int16_t *)xvid_malloc(temp_size,CACHE_LINE);
    if(dec->ac_pred_base)
        memset(dec->ac_pred_base,0,temp_size);
    dec->ac_pred_above_y = dec->ac_pred_base + 16;
    dec->ac_pred_left_y = dec->ac_pred_above_y + (dec->mb_width<<4);
    dec->ac_pred_above_u = dec->ac_pred_left_y + 24;
    dec->ac_pred_left_u = dec->ac_pred_above_u +(dec->mb_width<<3);
    dec->ac_pred_above_v = dec->ac_pred_left_u + 16;
    dec->ac_pred_left_v = dec->ac_pred_above_v +(dec->mb_width<<3);

    temp_size = (sizeof(VECTOR) * (dec->mb_width + 2)) << 2;
    dec->mv_buffer_base = (VECTOR *)xvid_malloc(temp_size,CACHE_LINE);
    if(dec->mv_buffer_base)
        memset(dec->mv_buffer_base,0,temp_size);

    temp_size = sizeof(int16_t) * 384;
    dec->block_size = temp_size;
    dec->block = xvid_malloc(temp_size,CACHE_LINE);
    if(dec->block)
        memset(dec->block,0,temp_size);

    dec->data = xvid_malloc(temp_size,CACHE_LINE);
    if(dec->data)
        memset(dec->data,0,temp_size);
    return 0;

memory_error:
    image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
    image_destroy(&dec->refn[0], dec->edged_width, dec->edged_height);
//  image_destroy(&dec->tmp, dec->edged_width, dec->edged_height);

    xvid_free(dec);
    return XVID_ERR_MEMORY;
}


int
decoder_create(xvid_dec_create_t * create)
{
    DECODER *dec;

    //cyg_mutex_init(&vld_access_mutex);

    if (XVID_VERSION_MAJOR(create->version) != 1) /* v1.x.x */
        return XVID_ERR_VERSION;
//  frame_number = 0;//test by longyl
    dec = xvid_malloc(sizeof(DECODER), CACHE_LINE);
    if (dec == NULL)
    {
        return XVID_ERR_MEMORY;
    }

    memset(dec, 0, sizeof(DECODER));
    create->handle = dec;

    dec->width = create->width;
    dec->height = create->height;

    image_null(&dec->cur);
    image_null(&dec->refn[0]);
// image_null(&dec->tmp);

    dec->ac_pred_base = NULL;
    dec->mv_buffer = NULL;
    dec->qscale = NULL;
    dec->block = NULL;
    dec->data = NULL;


    /* For B-frame support (used to save reference frame's time */
    dec->frames = 0;
    dec->time = dec->time_base = 0;
    dec->low_delay = 0;
    dec->time_inc_resolution = 1; /* until VOL header says otherwise */
    dec->ver_id = 1;

    dec->bs_version = (short)0xffff; /* Initialize to very high value -> assume bugfree stream */

    dec->fixed_dimensions = (dec->width > 0 && dec->height > 0);

    if (dec->fixed_dimensions)
        return decoder_resize(dec);
    else
        return 0;
}


int
decoder_destroy(DECODER * dec)
{
    xvid_free(dec->ac_pred_base);
    xvid_free(dec->mv_buffer_base);
    xvid_free(dec->qscale);
    xvid_free(dec->block);
    xvid_free(dec->data);

    image_destroy(&dec->refn[0], dec->edged_width, dec->edged_height);
//  image_destroy(&dec->tmp, dec->edged_width, dec->edged_height);
    image_destroy(&dec->cur, dec->edged_width, dec->edged_height);
    xvid_free(dec);

    return 0;
}
#endif

static const int32_t dquant_table[4] =
{
    -1, -2, 1, 2
};

//static int dongwl_cnt;

//static int inter_base, inter_bias, inter_cnt, inter_total;
#if 0
// only for intra
void mpeg4_hw_iq_idct(int frame_type, uint32_t iDcScaler, uint32_t iQuant,short * block,int i, short * data)
{

    int16_t * pInblock =block;

    unsigned int * temp_p;
    int ind;
    unsigned int tempdata;
    temp_p = (unsigned int *)data;

    //dongwl set IQ_IDCT reg Ctrl_iq_idct
    tempdata =(LILY_RD_REG(MP4_CTRL_IQ_IDCT) & 0xff0000ff) |((iDcScaler&0xff)<<0x8) |((iQuant&0xff)<<0x10) |0x04;
    LILY_WR_REG(MP4_CTRL_IQ_IDCT,tempdata);

    //set interrupt mask intm_vld
    LILY_WR_REG(MP4_INTM_VLD,0X0);
    //input data
    for(ind=0; ind<64; ind+=2)
        LILY_WR_REG(MP4_VLD_BUF+ind*2, (pInblock[ind+0]&0xffff)| (pInblock[ind+1]<<0x10));

    //start Ctrl_iq_idct
    LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x1);
    //waitting... Stat_iq_idct
    while((LILY_RD_REG(MP4_STAT_IQ_IDCT)&0x2)!=0x2);

    for (ind = 0; ind<32; ind++)
    {
        temp_p[ind] = LILY_RD_REG(MP4_IDCT_BUF+ind*4);
    }

    //stop
    LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x2);

}

// only for intra
void mpeg4_hw_vld(int frame_type,Bitstream * bs,int hw_direction,int dc_dif ,short * block,uint32_t i, short *data)
{

    unsigned int tempdata;
    unsigned int vld_length;
    unsigned int bufa;
    int index;

    int * block1;

    unsigned int dwl_bias, dwl_jmp;
    unsigned int dwl_base;

    int vld_cnt=0;


    block1 = block;

    //this is must be!! mohongfee 2007.08.28
    ifc_ch_disable();

    dwl_bias=((bs->pos +7) & 0xfffffff8) -bs->pos ;
    dwl_base=(unsigned int)(bs->tail-((bs->pos +7)>>3));

    dwl_jmp= (dwl_base-0x80000000)& 0x03;
    dwl_base -=dwl_jmp;
    dwl_bias +=(dwl_jmp<<3);


    ifc_nofifo_mp4_init(0,200,MP4_VLD_DATA, (dwl_base-0x80000000));//DATA bit stream buffer
    LILY_WR_REG(MP4_VLD_BIAS,dwl_bias);//BIAS Frame start bits bias

    //ifc_ch_enable();
    tempdata=LILY_RD_REG(IFC_CH_CONTROL(1));
    tempdata|=IFC_CH_ENABLE;
    LILY_WR_REG(IFC_CH_CONTROL(1),tempdata);


    //dongwl set vld reg Ctrl_vld
    tempdata =(LILY_RD_REG(MP4_CTRL_VLD) & 0xffffffcf) | 0x00000084 | (hw_direction<<4) ;
    LILY_WR_REG(MP4_CTRL_VLD, tempdata);

    //disable idct after vld Ctrl_vld_iq_idct
    LILY_WR_REG(MP4_CTRL_VLD_IQ_IDCT, 0x0);

    //mp4_enb_vld();
    LILY_WR_REG(MP4_CTRL_VLD, LILY_RD_REG(MP4_CTRL_VLD)|0x01);

    //wait... Stat_vld
    while((VLD_Finish&LILY_RD_REG(MP4_STAT_VLD))==0)
    {
//          diag_printf("########     hw_vld_run %d##########!\n",dongwl_cnt);
        vld_cnt++;

        if(vld_cnt>100)
        {
            diag_printf("########     hw_vld_run enter rolling still%d##########!\n");
            while(1);
        }
    }

    for (index=0; index<32; index++)
    {
        block1[index] = LILY_RD_REG(MP4_VLD_BUF+4*index);

    }

    block1[0] =(block1[0] &0xffff0000)|((int)dc_dif&0xffff);

    //get vld comsumed length LENGTH
    vld_length = LILY_RD_REG(MP4_VLD_LENGTH);

    //disable vld
    LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffd)|0x2);

    bs->tail =dwl_base+((dwl_bias+vld_length+7)>>3);
    bs->pos = ((dwl_bias+vld_length+7) & 0xfffffff8) - vld_length -dwl_bias;

    bufa=*(bs->tail -1);
    bs->buf=bufa;

}
#endif

/* decode an intra macroblock */
static void
decoder_mbintra(DECODER * dec,
                const uint32_t x_pos,
                const uint32_t y_pos,
                const int16_t acpred_flag,
                const uint32_t cbp,
                Bitstream * bs,
                const uint32_t quant,
                const uint32_t intra_dc_threshold,
                const unsigned int bound)
{
    int16_t *block = dec->block;
    int16_t *data = dec->data;
    uint16_t next_block = dec->edged_width << 3;
    uint16_t i;
    int16_t iQuant = quant;
    uint8_t *pY_Cur, *pU_Cur, *pV_Cur;

    int dc_dif;
    //add by longyl
#ifdef longyl
    int16_t *pCoeffBuffRow;
    int16_t *pCoeffBuffCol;
    int16_t *pQpBuffAbove;
    int16_t *pQpBuffLeft;
    int16_t *dct_codes;
    int16_t top,left,left_top;
    int16_t acpred_dir;
    int16_t predQp;
    int16_t dc_pred,dc;
    uint16_t acpred_bound_flag;
    uint32_t iDcScaler_mult;
    int16_t dc_tmp;
#endif
    int16_t fLeft, fTop, fDiag,mbpos;//added by liyongjian

    pY_Cur = dec->y_row_start + (x_pos<<4);
    pU_Cur = dec->u_row_start + (x_pos<<3);
    pV_Cur = dec->v_row_start + (x_pos<<3);
    pQpBuffLeft = dec->qscale_left;
    pQpBuffAbove = dec->qscale_above + x_pos;

    memset(block, 0, dec->block_size); /* clear */

    for (i = 0; i < 6; i++)
    {
        uint8_t iDcScaler = MPEG_DC_SCALAR[i < 4][iQuant];
#ifndef longyl
        int16_t predictors[8];
#endif
        int start_coeff;

        predQp = iQuant;
        acpred_bound_flag = acpred_flag;

        if(i < 4)
        {
            pCoeffBuffRow = dec->ac_pred_above_y + (x_pos<<4) + ((i&1)<<3);
            pCoeffBuffCol = dec->ac_pred_left_y + ((i>>1)<<3);
        }
        else if(i == 4)
        {
            pCoeffBuffRow = dec->ac_pred_above_u + (x_pos<<3);
            pCoeffBuffCol = dec->ac_pred_left_u;
        }
        else
        {
            pCoeffBuffRow = dec->ac_pred_above_v +(x_pos<<3);
            pCoeffBuffCol = dec->ac_pred_left_v;
        }
#if 0
        top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
        left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
        left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
#else

        fLeft = fTop = fDiag = 0;
        left = top = left_top = 0;
        mbpos = y_pos * dec->mb_width + x_pos;
        if(mbpos >= bound + 1)
            fLeft = 1;
        if(mbpos >= bound + dec->mb_width)
            fTop = 1;
        if(mbpos >= bound + dec->mb_width + 1)
            fDiag = 1;


        switch (i)
        {

            case 0:
                if (fLeft)
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                else
                    left = 0x400;

                if (fTop)
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                else
                    top = 0x400;

                if (fDiag)
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                else
                    left_top = 0x400;

                break;

            case 1:
                left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                if (fTop)
                {
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                }
                else
                {
                    left_top = top = 0x400;
                }
                break;

            case 2:
                if (fLeft)
                {
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                }
                else
                {
                    left_top = left = 0x400;
                }

                top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));

                break;

            case 3:
                left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));

                break;

            case 4:
                if (fLeft)
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                else
                    left = 0x400;

                if (fTop)
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                else
                    top = 0x400;

                if (fDiag)
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                else
                    left_top = 0x400;
                break;

            case 5:
                if (fLeft)
                    left = (*pCoeffBuffCol) + (((*pCoeffBuffCol) >> 15)&(1024 - (*pCoeffBuffCol)));
                else
                    left = 0x400;

                if (fTop)
                    top = (*pCoeffBuffRow) + (((*pCoeffBuffRow) >> 15)&(1024 - (*pCoeffBuffRow)));
                else
                    top = 0x400;

                if (fDiag)
                    left_top = (*(pCoeffBuffRow - 8)) + (((*(pCoeffBuffRow - 8)) >> 15)&(1024 - (*(pCoeffBuffRow - 8))));
                else
                    left_top = 0x400;
                break;

        }
#endif

        if(ABS((left-left_top)) < ABS((top-left_top)))//abs -->ABS error.shenh.08.11.18
        {
            acpred_dir = 1;//vertical
            dc_pred = top;
            acpred_bound_flag -= (((*pCoeffBuffRow)>>15)&acpred_bound_flag);
            if(i<2 || i>3)
                predQp = *pQpBuffAbove;
        }
        else
        {
            acpred_dir = 2;//horizontal
            dc_pred = left;
            acpred_bound_flag -= (((*pCoeffBuffCol)>>15)&acpred_bound_flag);
            if((i!=1)&&(i!=3))
                predQp = *pQpBuffLeft;
        }
        acpred_dir = acpred_dir - (((acpred_flag-1)>>15)&acpred_dir);

        if(quant < intra_dc_threshold)
        {
            int dc_size;


            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            dc_size = i < 4 ? get_dc_size_lum(bs) : get_dc_size_chrom(bs);
            dc_dif = dc_size ? get_dc_dif(bs, dc_size) : 0;
            bs->pos = bs->pos - (((8 - dc_size)>>31)&1);
            block[i << 6] = dc_dif;
            start_coeff = 1;
        }
        else
        {
            start_coeff = 0;
        }

        if (cbp & (32 >> i)) /* coded */
        {
            int hw_direction = 0;


            if(acpred_dir==1)
                hw_direction = 2;
            else if(acpred_dir ==2)
                hw_direction = 1;

///         diag_printf("########     enter mpeg4 vld part  %d##########!\n", dongwl_cnt);
            //dongwl_cnt++;

//          cyg_mutex_lock(&vld_access_mutex);

            //cyg_scheduler_lock();

            //cyg_interrupt_disable();

///         diag_printf("######## buf1 is 0x%x, pos is %d!\n", bs->buf, bs->pos);


            get_intra_block(bs, &block[i << 6], acpred_dir, start_coeff);
            //mpeg4_hw_vld(0,  bs, hw_direction,  dc_dif, &block[i << 6], i, &data[i << 6]);



            //cyg_interrupt_enable();

            //cyg_scheduler_unlock();

///         diag_printf("######## buf2 is 0x%x, pos is %d!\n", bs->buf, bs->pos);
///         diag_printf("\n\n");

///         if(dongwl_cnt>12)
///             while(1);


//          cyg_mutex_unlock(&vld_access_mutex);

        }

        dct_codes = &block[i << 6];
        dc = dct_codes[0];
        iDcScaler_mult = MPEG_DC_SCALAR_MULT[i<4][iQuant];
        dc += ((dc_pred*iDcScaler_mult + 0x10000)>>17) + ((dc_pred >> 15)&0xffff);
        dct_codes[0] = dc;
        dc *= iDcScaler;
        dc = dc + (((dc+2048) >> 15)&(-2048 - dc));
        dc = 2047 + (((dc-2047) >> 15)&(dc - 2047));

        if(acpred_dir == 1)
        {
            //ver
            int16_t j;
            int level;
            for(j = 1; j < 8; j++)
            {
                if (pCoeffBuffRow[j] ==0)
                    level = dct_codes[j] ;
                else
                    level = dct_codes[j]+ (((pCoeffBuffRow[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15);

//              int level = dct_codes[j] + (((pCoeffBuffRow[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15); // + ((pCoeffBuffRow[j]>>15)&0xffff);

                dct_codes[j] = level + (((acpred_bound_flag-1)>>15)&(dct_codes[j]-level));
                pCoeffBuffRow[j] = dct_codes[j];
                pCoeffBuffCol[j] = dct_codes[j<<3];
            }
        }
        else if(acpred_dir == 2)
        {
            //hor
            int16_t j;
            int level;
            for(j = 1; j < 8; j++)
            {
                if (pCoeffBuffCol[j] ==0)
                    level = dct_codes[j<<3] ;
                else
                    level = dct_codes[j<<3] + (((pCoeffBuffCol[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15);
//              int level = dct_codes[j<<3] + (((pCoeffBuffCol[j]*predQp + (iQuant>>1))*iQuant_MULT[iQuant])>>15); // + ((pCoeffBuffCol[j]>>15)&0xffff);

                dct_codes[j<<3] = level + (((acpred_bound_flag-1)>>15)&(dct_codes[j<<3]-level));
                pCoeffBuffCol[j] = dct_codes[j<<3];
                pCoeffBuffRow[j] = dct_codes[j];
            }
        }
        else
        {
            int16_t j;

            for(j = 1; j < 8; j++)
            {
                pCoeffBuffRow[j] = dct_codes[j];
                pCoeffBuffCol[j] = dct_codes[j<<3];
            }
        }

        switch (i)
        {
            case 1:
                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = *pCoeffBuffRow;
                *pCoeffBuffRow = dc;

                break;
            case 3:
                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                * pCoeffBuffCol = dc;
                dc_tmp = *pCoeffBuffRow;
                *pCoeffBuffRow = * (pCoeffBuffCol - 8);
                * (pCoeffBuffCol - 8) = dc_tmp;

                break;
            default:
                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = dc;
                break;
        }

        //mpeg4_hw_iq_idct(0, iDcScaler, iQuant,  &block[i << 6],  i, &data[i << 6]);
        dequant_h263_intra(&data[i<<6], &block[i<<6], iQuant, iDcScaler);

        //idct_int32((short * const)&data[i << 6]);
    }
    *pQpBuffAbove = *pQpBuffLeft = iQuant;
#if 0
    transfer_16to8copy(pY_Cur, &data[0], dec->stride[0]);
    transfer_16to8copy(pY_Cur + 8, &data[64], dec->stride[1]);
    transfer_16to8copy(pY_Cur + next_block, &data[128], dec->stride[2]);
    transfer_16to8copy(pY_Cur + 8 + next_block, &data[192], dec->stride[3]);
    transfer_16to8copy(pU_Cur, &data[256], dec->stride[4]);
    transfer_16to8copy(pV_Cur, &data[320], dec->stride[5]);
#endif

#if 1//for mips idct
    mpeg4_simple_idct_put(pY_Cur, (int)dec->stride[0], data);
    mpeg4_simple_idct_put(pY_Cur+8, (int)dec->stride[1], data+64);
    mpeg4_simple_idct_put(pY_Cur + next_block, (int)dec->stride[2], data+128);
    mpeg4_simple_idct_put(pY_Cur + 8 + next_block, (int)dec->stride[3], data+192);
    mpeg4_simple_idct_put(pU_Cur, (int)dec->stride[4], data+256);
    mpeg4_simple_idct_put(pV_Cur, (int)dec->stride[5], data+320);
#endif
}

static void
decoder_mb_decode(DECODER * dec,
                  uint32_t cbp,
                  Bitstream * bs,
                  uint8_t * pY_Cur,
                  uint8_t * pU_Cur,
                  uint8_t * pV_Cur)
{
    int16_t *data = dec->data;

    int i;
    const uint32_t iQuant = *dec->qscale_current;

    uint8_t *dst[6];

    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 64;
    dst[2] = pY_Cur + 128;
    dst[3] = pY_Cur + 192;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;

    for (i = 0; i < 6; i++)
    {
        /* Process only coded blocks */
        if (cbp & (32 >> i))
        {
            /* Clear the block */
            memset(&data[0], 0, 128);

            /* Decode coeffs and dequantize on the fly */
            get_inter_block_h263(bs, &data[0], 0, iQuant);

            /* iDCT */
            //idct_int32((short * const)&data[0]);

            /* Add this residual to the predicted block */
            //transfer_16to8add(dst[i], &data[0], dec->stride[i]);

            mpeg4_simple_idct_add(dst[i], 8, data);

        }
    }
}

#if 0
static void
decoder_mb_decode_hardBad(DECODER * dec,
                          uint32_t cbp,
                          Bitstream * bs,
                          uint8_t * pY_Cur,
                          uint8_t * pU_Cur,
                          uint8_t * pV_Cur)
{
    int16_t *data = dec->data;

    unsigned int inter_jmp, fpga_bias;
    unsigned int *fpga_base;

    int i, bFlag;
    const uint32_t iQuant = *dec->qscale_current;
    uint8_t *dst[6];

    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 64;
    dst[2] = pY_Cur + 128;
    dst[3] = pY_Cur + 192;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;


    inter_cnt=inter_total=0;
    inter_base = (unsigned int)(bs->tail-((bs->pos +7)>>3));
    inter_bias = ((bs->pos +7) & 0xfffffff8) -bs->pos ;

    inter_jmp =(inter_base-0x80000000)& 0x03;
    fpga_base =( inter_base - inter_jmp);
    fpga_bias = inter_bias + (inter_jmp<<3);


    bFlag=8;

    for (i = 0; i < 6; i++)
    {
        /* Process only coded blocks */
        if (cbp & (32 >> i))
        {
            unsigned int tempdata;
            unsigned int vld_length;

            unsigned int * temp_p;

            temp_p = &data[0];
            int ind;

            //this is must be!! mohongfee 2007.08.28
            ifc_ch_disable();

            ifc_nofifo_mp4_init(0,200,MP4_VLD_DATA, ( fpga_base -0x80000000));
            LILY_WR_REG(MP4_VLD_BIAS, fpga_bias);
            //ifc_ch_enable();
            tempdata=LILY_RD_REG(IFC_CH_CONTROL(1));
            tempdata|=IFC_CH_ENABLE;
            LILY_WR_REG(IFC_CH_CONTROL(1),tempdata);

            //dongwl set vld reg
            tempdata=(LILY_RD_REG(MP4_CTRL_VLD) & 0xffffffcb) | 0x0080;
            LILY_WR_REG(MP4_CTRL_VLD, tempdata);

            //dongwl set IQ_IDCT reg
            tempdata=(LILY_RD_REG(MP4_CTRL_IQ_IDCT) & 0xff00fffb) | ((iQuant&0xff)<<0x10) ;
            LILY_WR_REG(MP4_CTRL_IQ_IDCT, tempdata);


            LILY_WR_REG(MP4_CTRL_VLD_IQ_IDCT, 0x00000001);

            LILY_WR_REG(MP4_CTRL_VLD, LILY_RD_REG(MP4_CTRL_VLD) |0x00000001);

            if(bFlag != 8)
            {
                transfer_16to8add(dst[bFlag], &data[0], dec->stride[bFlag]);
            }
            bFlag=i;

            //waitting... vld
            while((VLD_Finish&LILY_RD_REG(MP4_STAT_VLD))==0);

            //get vld comsumed length
            vld_length = LILY_RD_REG(MP4_VLD_LENGTH);

            inter_total +=vld_length;
            fpga_base += (fpga_bias + vld_length)>>5;
            fpga_bias = (fpga_bias + vld_length)%32;

            //waitting... iq idct
            while((LILY_RD_REG(MP4_STAT_IQ_IDCT)&0x2)!=0x2);

            for (ind = 0; ind<32; ind++)
            {
                temp_p[ind] = LILY_RD_REG(MP4_IDCT_BUF+ind*4);

            }

            //stop vld
            LILY_WR_REG(MP4_CTRL_VLD, LILY_RD_REG(MP4_CTRL_VLD) |0x02);

            //stop iq idct
            LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x2);

        }
    }


    if(bFlag != 8)
    {
        transfer_16to8add(dst[bFlag], &data[0], dec->stride[bFlag]);
    }

    bs->tail = inter_base+((inter_bias+inter_total+7)>>3);
    bs->pos = ((inter_bias+inter_total+7) & 0xfffffff8) - inter_total -inter_bias;
    bs->buf=*(bs->tail -1);

}


static void
decoder_mb_decode_hardOK(DECODER * dec,
                         const uint32_t cbp,
                         Bitstream * bs,
                         uint8_t * pY_Cur,
                         uint8_t * pU_Cur,
                         uint8_t * pV_Cur
                        )

{
    int16_t *data = dec->data;

    unsigned int inter_jmp, fpga_bias;
    unsigned int *fpga_base;

    int i, bFlag;
    const uint32_t iQuant = *dec->qscale_current;
    uint8_t *dst[6];
    unsigned int * temp_p;

    temp_p = &data[0];

    dst[0] = pY_Cur;
    dst[1] = pY_Cur + 64;
    dst[2] = pY_Cur + 128;
    dst[3] = pY_Cur + 192;
    dst[4] = pU_Cur;
    dst[5] = pV_Cur;


    inter_cnt=inter_total=0;
    inter_base = (unsigned int)(bs->tail-((bs->pos +7)>>3));
    inter_bias = ((bs->pos +7) & 0xfffffff8) -bs->pos ;

    inter_jmp =(inter_base-0x80000000)& 0x03;
    fpga_base =( inter_base - inter_jmp);
    fpga_bias = inter_bias + (inter_jmp<<3);


    bFlag=8;


    for (i = 0; i < 6; i++)
    {
        /* Process only coded blocks */
        if (cbp & (1 << (5 - i)))
        {

            {
                unsigned int tempdata;
                unsigned int vld_length;


                int ind;

                memset(&data[0], 0, 128);

                //put_string("******:", 6);
                //put_enter();

                //this is must be!! mohongfee 2007.08.28
                ifc_ch_disable();
                ifc_nofifo_mp4_init(0,200,MP4_VLD_DATA, (fpga_base-0x80000000));
                LILY_WR_REG(MP4_VLD_BIAS,fpga_bias);

                //ifc_ch_enable();
                tempdata=LILY_RD_REG(IFC_CH_CONTROL(1));
                tempdata|=IFC_CH_ENABLE;
                LILY_WR_REG(IFC_CH_CONTROL(1),tempdata);
                //vld inter block
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffb));
                //vld scan type, here zigzag type
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xffffffcf)|0x0);
                //vld endian set
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xffffff7f)|0x80);


                //mp4_enb_vld();
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffe)|0x1);

                //wait...
                while((VLD_Finish&LILY_RD_REG(MP4_STAT_VLD))==0);
//////////////////////////////-------------------------------------------------
                for (ind=0; ind<32; ind++)
                {
                    temp_p[ind] = LILY_RD_REG(MP4_VLD_BUF+4*ind);

                }


                //get vld comsumed length
                vld_length = LILY_RD_REG(MP4_VLD_LENGTH);
                //disable vld
                //mp4_dis_vld();
                LILY_WR_REG(MP4_CTRL_VLD,(LILY_RD_REG(MP4_CTRL_VLD)&0xfffffffd)|0x2);

                //after hardware vld; we should cope with the bs data structure.
                //bs->tail = (bs->tail) + ((bs->pos+vld_length)>>5);
                //bs->pos = (bs->pos+vld_length)%32;
                //bufa = *(bs->tail);
                //bufb = *(bs->tail+1);
                inter_total +=vld_length;
                fpga_base += (fpga_bias + vld_length)>>5;
                fpga_bias = (fpga_bias + vld_length)%32;


                //bs->bufa = bufa;
                //bs->bufb = bufb;
            }

            bs->tail = inter_base+((inter_bias+inter_total+7)>>3);
            bs->pos = ((inter_bias+inter_total+7) & 0xfffffff8) - inter_total -inter_bias;
            bs->buf=*(bs->tail -1);

            // mpeg4_hw_iq_idct(1, 0,  iQuant,  &temp_p[0],  i,  data);
            {
                int16_t * pInblock = &data[0];
                int16_t * pOutblock = &data[0];
                unsigned int * temp_p;
                unsigned int utemp = 0;
                temp_p = (unsigned int *)pOutblock;
                int ind;

                //set inter
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffb));
                //set iQuant
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xff00ffff)|((iQuant&0xff)<<0x10));
                //set interrupt mask
                LILY_WR_REG(MP4_INTM_VLD,0X0);
                //input data
                for(ind=0; ind<64; ind+=2)
                    LILY_WR_REG(MP4_VLD_BUF+ind*2, (pInblock[ind+0]&0xffff)|(pInblock[ind+1]<<0x10));
                //start
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x1);
                //waitting...
                while((LILY_RD_REG(MP4_STAT_IQ_IDCT)&0x2)!=0x2);
                //output data



                for (ind = 0; ind<32; ind++)
                {
                    temp_p[ind] = LILY_RD_REG(MP4_IDCT_BUF+ind*4);

                }
                //stop
                LILY_WR_REG(MP4_CTRL_IQ_IDCT,(LILY_RD_REG(MP4_CTRL_IQ_IDCT)&0xfffffffc)|0x2);
            }
            /* Add this residual to the predicted block */
            transfer_16to8add(dst[i], &data[0], dec->stride[i]);
        }
    }


}
#endif


void
decoder_iframe(DECODER * dec,
               Bitstream * bs,
               int quant,
               int intra_dc_threshold)
{
    uint16_t bound;
    uint16_t temp_code;
    int16_t x, y;
    const int16_t mb_width = dec->mb_width;
    const int16_t mb_height = dec->mb_height;

    bound = 0;
////////////add by longyl
#ifdef longyl
    for(x = -1; x < mb_width; x ++)
    {
        *(dec->ac_pred_above_y + (x << 4)) = -1;
        *(dec->ac_pred_above_y + (x << 4) +8) = -1;
        *(dec->ac_pred_above_u + (x << 3)) = -1;
        *(dec->ac_pred_above_v + (x << 3)) = -1;
    }
    *(dec->ac_pred_left_y) = -1;
    *(dec->ac_pred_left_y + 8) = -1;
    *(dec->ac_pred_left_u) = -1;
    *(dec->ac_pred_left_v) = -1;
    dec->y_row_start = dec->cur.y;
    dec->u_row_start = dec->cur.u;
    dec->v_row_start = dec->cur.v;
#endif
///////////////////////////////////////////
//  file_test = fopen("test.dat","wb"); //test by longyl
//  macroblock_number = 0; //test by longyl
    for (y = 0; y < mb_height; y++)
    {
        for (x = 0; x < mb_width; x++)
        {
            uint16_t mcbpc;
            uint16_t cbpc;
            int16_t acpred_flag;
            uint16_t cbpy;
            uint16_t cbp;

            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
#if 0
            _SHOWBITS(bs->buf,bs->pos,9,temp_code);
            if (temp_code == 1)
                bs->pos -= 9;
#else
            _SHOWBITS(bs->buf,bs->pos,9,temp_code);
            while(temp_code == 1)
            {
                bs->pos -= 9;
                _SHOWBITS(bs->buf,bs->pos,9,temp_code);
            }
#endif
            if (check_resync_marker(bs, 0))
            {
                bound = read_video_packet_header(bs, dec, 0,
                                                 &quant, NULL, NULL, &intra_dc_threshold);
                x = bound % mb_width;
                y = bound / mb_width;
            }
            {
                uint32_t index;

                _SHOWBITS(bs->buf,bs->pos,9,index);
                index >>= 3;
                bs->pos -= mcbpc_intra_table[index].len;
                mcbpc = mcbpc_intra_table[index].code;
            }
            dec->mode = mcbpc & 7;
            cbpc = (mcbpc >> 4);

            _GETBITS(bs->buf,bs->pos,1,acpred_flag);

            {
                uint32_t index;
                _SHOWBITS(bs->buf,bs->pos,6,index);
                bs->pos -= cbpy_table[index].len;
                cbpy = cbpy_table[index].code;
            }
            cbp = (cbpy << 2) | cbpc;

            if (dec->mode == MODE_INTRA_Q)
            {
                uint32_t index;
                _GETBITS(bs->buf,bs->pos,2,index);
                quant += dquant_table[index];
//      quant = 31 + (((quant-31)>>31)&(quant-31));
//      quant = 1 + (((1-quant)>>31)&(1-quant));
                if (quant > 31)
                {
                    quant = 31;
                }
                else if (quant < 1)
                {
                    quant = 1;
                }
            }
            *dec->qscale_current = quant;
            decoder_mbintra(dec, x, y, acpred_flag, cbp, bs, quant,
                            intra_dc_threshold, bound);

        }
        /////add by longyl
#ifdef longyl
        *(dec->ac_pred_above_y + (dec->mb_width<<4) - 8) = *(dec->ac_pred_left_y +8);
        *(dec->ac_pred_above_u + (dec->mb_width<<3) - 8) = *(dec->ac_pred_left_u);
        *(dec->ac_pred_above_v + (dec->mb_width<<3) - 8) = *(dec->ac_pred_left_v);
        *(dec->ac_pred_left_y) = -1;
        *(dec->ac_pred_left_y + 8) = -1;
        *(dec->ac_pred_left_u) = -1;
        *(dec->ac_pred_left_v) = -1;

        dec->y_row_start += dec->edged_width << 4;
        dec->u_row_start += dec->edged_width << 2;
        dec->v_row_start += dec->edged_width << 2;
#endif
        //////////////////////////////////////////////
        /*    if(dec->out_frm)
              output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,0,y,mb_width);*/
    }
}


/* for P_VOP set gmc_warp to NULL */
void
decoder_pframe(DECODER * dec,
               Bitstream * bs,
               int rounding,
               int quant,
               int fcode,
               int intra_dc_threshold)
{

    uint16_t bound;
    int32_t temp_code;
//  int cp_mb, st_mb;
    int16_t x, y;
    int16_t *pCoeffBuffRow;
    int16_t *pCoeffBuffCol;
    VECTOR *pMVBuffPred[3];
    VECTOR *pMVBuff_temp;
    const int16_t mb_width = dec->mb_width;
    const int16_t mb_height = dec->mb_height;
    static DECMBINTER_PTR dec_mb[3] = {dec_mb_inter1v,dec_mb_inter1v,dec_mb_inter4v};
    static uint8_t *block[3];

    if (!dec->is_edged[0])
    {
        image_setedges(&dec->refn[0], dec->edged_width, dec->edged_height,
                       dec->width, dec->height, dec->bs_version);
        dec->is_edged[0] = 1;
    }

    bound = 0;
    block[0] = (uint8_t *)dec->block;
    block[1] = (uint8_t *)(dec->block + 256);
    block[2] = (uint8_t *)(dec->block + 320);

#ifdef longyl
    for(x = -1; x < mb_width; x++)
    {
        *(dec->ac_pred_above_y + (x<<4)) = -1;
        *(dec->ac_pred_above_y + (x<<4) +8) = -1;
        *(dec->ac_pred_above_u + (x<<3)) = -1;
        *(dec->ac_pred_above_v + (x<<3)) = -1;
    }
    *(dec->ac_pred_left_y) = -1;
    *(dec->ac_pred_left_y + 8) = -1;
    *(dec->ac_pred_left_u) = -1;
    *(dec->ac_pred_left_v) = -1;
    pMVBuff_temp = dec->mv_buffer_base+1;
    (pMVBuff_temp++)->x = 0x80000000;
    pMVBuff_temp ++;
    (pMVBuff_temp++)->x = 0x80000000;
    for(x = dec->mb_width+1; x > 0; x--)
    {
        pMVBuff_temp += 2;
        (pMVBuff_temp++)->x = 0x80000000;
        (pMVBuff_temp++)->x = 0x80000000;
    }
    pMVBuffPred[0] = dec->mv_buffer_base; //left
    dec->y_row_start = dec->cur.y;
    dec->u_row_start = dec->cur.u;
    dec->v_row_start = dec->cur.v;
#endif
///////////////////////////////////////////
    for (y = 0; y < mb_height; y++)
    {
//    cp_mb = st_mb = 0;
        for (x = 0; x < mb_width; x++)
        {
            /*    if(macroblock_number == 50)
                  {
                    macroblock_number = 0;
                  }*/
            pMVBuffPred[1] = pMVBuffPred[0] + (x<<2) + 4; //top
            pMVBuffPred[2] = pMVBuffPred[1] + 4;  //topright
            /* skip stuffing */
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _SHOWBITS(bs->buf,bs->pos,10,temp_code);
#if 0
            while (temp_code == 1)
                bs->pos -= 10;
#else
            while(temp_code == 1)
            {
                bs->pos -= 10;
                _SHOWBITS(bs->buf,bs->pos,10,temp_code);
            }
#endif
            if (check_resync_marker(bs, fcode - 1))
            {
                int16 tempx;//added bi liyongjian
                bound = read_video_packet_header(bs, dec, fcode - 1,
                                                 &quant, &fcode, NULL, &intra_dc_threshold);
                x = bound % mb_width;
                y = bound / mb_width;
//add by longyl
#ifdef longyl
                for(tempx = -1; tempx < mb_width; tempx ++)
                {
                    *(dec->ac_pred_above_y + (tempx<<4)) = -1;
                    *(dec->ac_pred_above_y + (tempx<<4) +8) = -1;
                    *(dec->ac_pred_above_u + (tempx<<3)) = -1;
                    *(dec->ac_pred_above_v + (tempx<<3)) = -1;
                }
                *(dec->ac_pred_left_y) = -1;
                *(dec->ac_pred_left_y + 8) = -1;
                *(dec->ac_pred_left_u) = -1;
                *(dec->ac_pred_left_v) = -1;

                pMVBuff_temp = dec->mv_buffer_base+1;
                (pMVBuff_temp++)->x = 0x80000000;
                pMVBuff_temp ++;
                (pMVBuff_temp++)->x = 0x80000000;
                for(tempx = dec->mb_width+1; tempx > 0; tempx--)
                {
                    pMVBuff_temp += 2;
                    (pMVBuff_temp++)->x = 0x80000000;
                    (pMVBuff_temp++)->x = 0x80000000;
                }
                pMVBuffPred[0] = dec->mv_buffer_base; //left
#endif
///////////////////////////////////////////
            }

            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code)   /* block _is_ coded */
            {
                uint16_t mcbpc, cbpc, cbpy, cbp;
                int16_t intra;
                int16_t acpred_flag = 0;

//        cp_mb++;

                {
                    uint16_t index;

                    _SHOWBITS(bs->buf,bs->pos,9,temp_code)
                    index = MIN_32(256,temp_code);
                    bs->pos -= mcbpc_inter_table[index].len;
                    mcbpc = mcbpc_inter_table[index].code;
                }

                dec->mode = mcbpc & 7;
                cbpc = (mcbpc >> 4);

                intra = (dec->mode == MODE_INTRA || dec->mode == MODE_INTRA_Q);

                if (intra)
                    _GETBITS(bs->buf,bs->pos,1,acpred_flag);
                {
                    uint16_t index;
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _SHOWBITS(bs->buf,bs->pos,6,index);
                    bs->pos -= cbpy_table[index].len;
                    cbpy = cbpy_table[index].code + (((intra-1)>>15)&(15-(cbpy_table[index].code << 1)));
                }

                cbp = (cbpy << 2) | cbpc;
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                if (dec->mode == MODE_INTER_Q || dec->mode == MODE_INTRA_Q)
                {
                    _GETBITS(bs->buf,bs->pos,2,temp_code);
                    quant += dquant_table[temp_code];
//        quant = 31 + (((quant-31)>>31)&(quant-31));
//        quant = 1 + (((1-quant)>>31)&(1-quant));
                    if (quant > 31)
                    {
                        quant = 31;
                    }
                    else if (quant < 1)
                    {
                        quant = 1;
                    }
                }
                *dec->qscale_current = quant;
                if (dec->mode < 3)
                {
                    dec_mb[dec->mode](dec,pMVBuffPred,block,x,y,cbp,bs,fcode,rounding);
                    pCoeffBuffRow = dec->ac_pred_above_y + (x<<4);
                    pCoeffBuffCol = dec->ac_pred_left_y;

                    *(pCoeffBuffRow - 8) = *(pCoeffBuffCol + 8);
                    *pCoeffBuffRow = -1;

                    *pCoeffBuffCol = -1;
                    *(pCoeffBuffCol + 8) = -1;

                    pCoeffBuffRow = dec->ac_pred_above_u + (x<<3);
                    pCoeffBuffCol = dec->ac_pred_left_u;

                    *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                    *pCoeffBuffCol = -1;

                    pCoeffBuffRow = dec->ac_pred_above_v + (x<<3);
                    pCoeffBuffCol = dec->ac_pred_left_v;

                    *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                    *pCoeffBuffCol = -1;
                }
                else
                {
                    /* MODE_INTRA, MODE_INTRA_Q */
                    pMVBuffPred[1][2].x = pMVBuffPred[1][3].x = pMVBuffPred[0][1].x
                                          = pMVBuffPred[0][3].x = dec->mvs[0].x = 0;
                    pMVBuffPred[1][2].y = pMVBuffPred[1][3].y = pMVBuffPred[0][1].y
                                          = pMVBuffPred[0][3].y = dec->mvs[0].y = 0;
                    decoder_mbintra(dec, x, y, acpred_flag, cbp, bs, quant,
                                    intra_dc_threshold, bound);
                }
            }
            else
            {
                /* not coded P_VOP macroblock */
                int32_t y_add_temp,uv_add_temp,i;

                dec->mode = MODE_NOT_CODED;
                *dec->qscale_above =  *dec->qscale_left = *dec->qscale_current = quant;

                pMVBuffPred[1][2].x = pMVBuffPred[1][3].x = pMVBuffPred[0][1].x
                                      = pMVBuffPred[0][3].x = dec->mvs[0].x = 0;
                pMVBuffPred[1][2].y = pMVBuffPred[1][3].y = pMVBuffPred[0][1].y
                                      = pMVBuffPred[0][3].y = dec->mvs[0].y = 0;

                y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+(x << 4);
                uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x << 3);
                interpolate16x16_switch(block[0], dec->refn[0].y + y_add_temp,
                                        0, 0, dec->edged_width, rounding);
                interpolate8x8_switch(block[1], dec->refn[0].u + uv_add_temp, 0, 0, (dec->edged_width >> 1), rounding);
                interpolate8x8_switch(block[2], dec->refn[0].v + uv_add_temp, 0, 0, (dec->edged_width >> 1), rounding);
                //y
                {
                    uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
                    uint32_t *dst_addr_tmp = (uint32_t *)(dec->cur.y + y_add_temp);
                    uint32_t stride_tmp = (dec->edged_width >> 2);
                    uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
                    for(i = 8; i > 0; i--)
                    {
                        dst_addr_tmp[0] = ref_addr_tmp0[0];
                        dst_addr_tmp[1] = ref_addr_tmp0[1];
                        dst_addr_tmp[2] = ref_addr_tmp0[16];
                        dst_addr_tmp[3] = ref_addr_tmp0[17];

                        dst_addr_stride[0] = ref_addr_tmp0[32];
                        dst_addr_stride[1] = ref_addr_tmp0[33];
                        dst_addr_stride[2] = ref_addr_tmp0[48];
                        dst_addr_stride[3] = ref_addr_tmp0[49];

                        dst_addr_tmp += stride_tmp;
                        dst_addr_stride += stride_tmp;
                        ref_addr_tmp0 += 2;
                    }
                }
                //u
                {
                    uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
                    uint32_t *dst_addr_tmp = (uint32_t *)(dec->cur.u + uv_add_temp);
                    uint32_t stride_tmp = (dec->edged_width >> 3);

                    for(i = 8; i > 0; i--)
                    {
                        dst_addr_tmp[0] = ref_addr_tmp0[0];
                        dst_addr_tmp[1] = ref_addr_tmp0[1];

                        dst_addr_tmp += stride_tmp;
                        ref_addr_tmp0 += 2;
                    }
                }
                //v
                {
                    uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
                    uint32_t *dst_addr_tmp = (uint32_t *)(dec->cur.v + uv_add_temp);
                    uint32_t stride_tmp = (dec->edged_width >> 3);

                    for(i = 8; i > 0; i--)
                    {
                        dst_addr_tmp[0] = ref_addr_tmp0[0];
                        dst_addr_tmp[1] = ref_addr_tmp0[1];

                        dst_addr_tmp += stride_tmp;
                        ref_addr_tmp0 += 2;
                    }
                }
                pCoeffBuffRow = dec->ac_pred_above_y + (x<<4);
                pCoeffBuffCol = dec->ac_pred_left_y;

                *(pCoeffBuffRow - 8) = *(pCoeffBuffCol + 8);
                *pCoeffBuffRow = -1;

                *pCoeffBuffCol = -1;
                *(pCoeffBuffCol + 8) = -1;

                pCoeffBuffRow = dec->ac_pred_above_u + (x<<3);
                pCoeffBuffCol = dec->ac_pred_left_u;

                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = -1;

                pCoeffBuffRow = dec->ac_pred_above_v + (x<<3);
                pCoeffBuffCol = dec->ac_pred_left_v;

                *(pCoeffBuffRow - 8) = *pCoeffBuffCol;
                *pCoeffBuffCol = -1;

                /*      if(dec->out_frm && cp_mb > 0) {
                        output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);
                        cp_mb = 0;
                      }
                      st_mb = x+1;*/
            }
        }
#ifdef longyl
        *(dec->ac_pred_above_y + (mb_width<<4) - 8) = *(dec->ac_pred_left_y + 8);
        *(dec->ac_pred_above_u + (mb_width<<3) - 8) = *(dec->ac_pred_left_u);
        *(dec->ac_pred_above_v + (mb_width<<3) - 8) = *(dec->ac_pred_left_v);

        *(dec->ac_pred_left_y) = -1;
        *(dec->ac_pred_left_y + 8) = -1;
        *(dec->ac_pred_left_u) = -1;
        *(dec->ac_pred_left_v) = -1;
        pMVBuffPred[0][1].x = pMVBuffPred[0][3].x = 0x80000000;
        dec->y_row_start += dec->edged_width<<4;
        dec->u_row_start += dec->edged_width<<2;
        dec->v_row_start += dec->edged_width<<2;
#endif

        /*    if(dec->out_frm && cp_mb > 0)
              output_slice(&dec->cur, dec->edged_width,dec->width,dec->out_frm,st_mb,y,cp_mb);*/
    }
}

#if 0//move to xvid.c for overlay

/* perform post processing if necessary, and output the image */
static void decoder_output(DECODER * dec, IMAGE * img,xvid_dec_frame_t * frame, xvid_dec_stats_t * stats,
                           int coding_type, zoomPar *pZoom)
{
//  frame->general = 1;
    image_output(img, dec->width, dec->height,
                 dec->edged_width, (uint8_t**)frame->output.plane, pZoom);

    if (stats)
    {
        stats->type = coding2type(coding_type);
        stats->data.vop.time_base = (int)dec->time_base;
        stats->data.vop.time_increment = 0; /* XXX: todo */
        stats->data.vop.qscale_stride = dec->mb_width;
    }
}

int
decoder_decode(DECODER * dec,
               xvid_dec_frame_t * frame, xvid_dec_stats_t * stats,zoomPar *pZoom)
{

    Bitstream bs;
    uint16_t rounding;
    uint16_t quant = 2;
    uint16_t fcode_forward;
    uint16_t intra_dc_threshold;
    int coding_type;
    int success, output, seen_something;

    if (XVID_VERSION_MAJOR(frame->version) != 1 || (stats && XVID_VERSION_MAJOR(stats->version) != 1))  /* v1.x.x */
        return XVID_ERR_VERSION;

    dec->low_delay_default = (frame->general & XVID_LOWDELAY);
    if ((frame->general & XVID_DISCONTINUITY))
        dec->frames = 0;

    dec->out_frm = (frame->output.csp == XVID_CSP_SLICE) ? &frame->output : NULL;

    if(frame->length<0)    /* decoder flush */
    {
        if (stats) stats->type = XVID_TYPE_NOTHING;
        return XVID_ERR_END;
    }

    BitstreamInit(&bs, frame->bitstream, frame->length);

    success = 0;
    output = 0;
    seen_something = 0;

repeat:

    coding_type = BitstreamReadHeaders(&bs, dec, &rounding,
                                       &quant, &fcode_forward, &intra_dc_threshold);

    if(coding_type == -8)//profile unfit
        return -1;
    if (coding_type == -1)   /* nothing */
    {
        if (success) goto done;
        if (stats) stats->type = XVID_TYPE_NOTHING;

        return BitstreamPos(&bs)>>3;
    }

    if (coding_type == -2 || coding_type == -3)   /* vol and/or resize */
    {

        if (coding_type == -3)
            decoder_resize(dec);

        if(stats)
        {
            stats->type = XVID_TYPE_VOL;
            stats->data.vol.general = 0;
            stats->data.vol.width = dec->width;
            stats->data.vol.height = dec->height;
            stats->data.vol.par = dec->aspect_ratio;
            stats->data.vol.par_width = dec->par_width;
            stats->data.vol.par_height = dec->par_height;

            return BitstreamPos(&bs)>>3; /* number of bytes consumed */
        }
        goto repeat;
    }

    if(dec->frames == 0 && coding_type != I_VOP)
    {
        /* 1st frame is not an i-vop */
        goto repeat;
    }
    dec->p_fmv.x = dec->p_fmv.y = 0;  /* init pred vector to 0 */
    switch(coding_type)
    {
        case I_VOP :
            decoder_iframe(dec, &bs, quant, intra_dc_threshold);
//    frame_number ++;//test by longyl
            break;
        case P_VOP :
            decoder_pframe(dec, &bs, rounding, quant,
                           fcode_forward, intra_dc_threshold);//, NULL);
//    frame_number ++;//test by longyl
            break;
        case N_VOP :
            /* XXX: not_coded vops are not used for forward prediction */
            /* we should not swap(last_mbs,mbs) */
            image_copy(&dec->cur, &dec->refn[0], dec->edged_width, dec->height);
            break;
    }
#if 0
    if (dec->frames > 0)   /* is the reference frame valid? */
    {
        /* output the reference frame */
        decoder_output(dec, &dec->refn[0], frame, stats, dec->last_coding_type,  pZoom);
        output = 1;
    }
#else// output current decode frame
    decoder_output(dec, &dec->cur, frame, stats, coding_type,  pZoom);
#endif
    image_swap(&dec->cur, &dec->refn[0]);
    dec->is_edged[0] = 0;
    dec->last_coding_type = coding_type;

//  diag_printf("########     decoder_decode have finished a frame ##########!\n");

    dec->frames++;
    seen_something = 1;

done :

    return (BitstreamPos(&bs)+7)>>3; /* number of bytes consumed */
}

#endif

void dec_mb_inter1v(DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                    Bitstream *bs,int fcode,int rounding)
{
    VECTOR pmv;
    int16_t coord_x,coord_y;
    int32_t y_add_temp,uv_add_temp;

    uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
    int y_dx,y_dy,uv_dx, uv_dy,i;

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,0);
    dec_mv(fcode,&dec->mvs[0],&pmv,bs);
    pMVBuffPred[1][2] = pMVBuffPred[1][3]  = pMVBuffPred[0][1]
                        = pMVBuffPred[0][3] = dec->mvs[3] = dec->mvs[2] = dec->mvs[1] = dec->mvs[0];

    coord_x = x_pos << 4;
    coord_y = y_pos << 4;

    y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+coord_x;
    uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x_pos<<3);
    pY_Cur = dec->y_row_start + coord_x;
    pU_Cur = dec->u_row_start + (x_pos << 3);
    pV_Cur = dec->v_row_start + (x_pos << 3);
    {
        int16_t coord_tmp = (dec->width-coord_x)<<1;
        int mv_dtmp = dec->mvs[0].x + ((EDGE_SIZE + coord_x)<<1);
        y_dx = dec->mvs[0].x + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y_dx - coord_tmp;
        y_dx = coord_tmp + ((mv_dtmp>>31)&mv_dtmp);

        mv_dtmp = dec->mvs[0].y + ((EDGE_SIZE + coord_y)<<1);
        y_dy = dec->mvs[0].y + ((mv_dtmp>>31)&(-mv_dtmp));
        coord_tmp = (dec->height-coord_y)<<1;
        mv_dtmp = y_dy - coord_tmp;
        y_dy = coord_tmp + ((mv_dtmp>>31)&mv_dtmp);
    }
    uv_dx = (y_dx >> 1) + roundtab_79[y_dx & 0x3];
    uv_dy = (y_dy >> 1) + roundtab_79[y_dy & 0x3];

    interpolate16x16_switch(block[0], dec->refn[0].y + y_add_temp,
                            y_dx, y_dy, dec->edged_width, rounding);
    interpolate8x8_switch(block[1],dec->refn[0].u + uv_add_temp,uv_dx, uv_dy, dec->stride[4], rounding);
    interpolate8x8_switch(block[2],dec->refn[0].v + uv_add_temp,uv_dx, uv_dy, dec->stride[5], rounding);

    if (cbp)
    {
        decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);
    }
#if 0
    //y
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
        uint32_t *dst_addr_tmp = (uint32_t *)pY_Cur;
        uint32_t stride_tmp = (dec->edged_width >> 2);
        uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = ref_addr_tmp0[0];
            dst_addr_tmp[1] = ref_addr_tmp0[1];
            dst_addr_tmp[2] = ref_addr_tmp0[16];
            dst_addr_tmp[3] = ref_addr_tmp0[17];

            dst_addr_stride[0] = ref_addr_tmp0[32];
            dst_addr_stride[1] = ref_addr_tmp0[33];
            dst_addr_stride[2] = ref_addr_tmp0[48];
            dst_addr_stride[3] = ref_addr_tmp0[49];

            dst_addr_tmp += stride_tmp;
            dst_addr_stride += stride_tmp;
            ref_addr_tmp0 += 2;
        }
    }
    //u
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
        uint32_t *dst_addr_tmp = (uint32_t *)pU_Cur;
        uint32_t stride_tmp = (dec->edged_width >> 3);

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = ref_addr_tmp0[0];
            dst_addr_tmp[1] = ref_addr_tmp0[1];

            dst_addr_tmp += stride_tmp;
            ref_addr_tmp0 += 2;
        }
    }
    //v
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
        uint32_t *dst_addr_tmp = (uint32_t *)pV_Cur;
        uint32_t stride_tmp = (dec->edged_width >> 3);

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = ref_addr_tmp0[0];
            dst_addr_tmp[1] = ref_addr_tmp0[1];

            dst_addr_tmp += stride_tmp;
            ref_addr_tmp0 += 2;
        }
    }
#else
    {
        uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
        uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
        uint32_t y_stride_tmp = (dec->edged_width >> 2);
        uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

        uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
        uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
        uint32_t uv_stride_tmp = (dec->edged_width >> 3);

        uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
        uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;

        for(i = 8; i > 0; i--)
        {
            y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
            y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
            y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
            y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

            dst_addr_stride[0] = y_ref_addr_tmp0[32];
            dst_addr_stride[1] = y_ref_addr_tmp0[33];
            dst_addr_stride[2] = y_ref_addr_tmp0[48];
            dst_addr_stride[3] = y_ref_addr_tmp0[49];

            y_dst_addr_tmp += y_stride_tmp;
            dst_addr_stride += y_stride_tmp;
            y_ref_addr_tmp0 += 2;

            u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
            u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

            u_dst_addr_tmp += uv_stride_tmp;
            u_ref_addr_tmp0 += 2;

            v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
            v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

            v_dst_addr_tmp += uv_stride_tmp;
            v_ref_addr_tmp0 += 2;
        }
    }
#endif
//  macroblock_number ++;//test by longyl
}

void dec_mb_inter4v(DECODER *dec,VECTOR *pMVBuffPred[3],uint8_t **block,int16_t x_pos,int16_t y_pos,uint16_t cbp,
                    Bitstream *bs,int fcode,int rounding)
{
    VECTOR pmv;
    int32_t coord_x,coord_y;
    int32_t y_add_temp,uv_add_temp;

    uint8_t *pY_Cur, *pU_Cur, *pV_Cur;
    uint32_t next_pos;
    int uv_dx, uv_dy;
    int y1_dx,y2_dx,y3_dx,y4_dx,y1_dy,y2_dy,y3_dy,y4_dy,i;

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,0);
    dec_mv(fcode,&dec->mvs[0],&pmv,bs);
    pMVBuffPred[0][0] = pMVBuffPred[1][0] = dec->mvs[0];

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,1);
    dec_mv(fcode,&dec->mvs[1],&pmv,bs);
    pMVBuffPred[0][1] = pMVBuffPred[1][1] = dec->mvs[1];

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,2);
    dec_mv(fcode,&dec->mvs[2],&pmv,bs);
    pMVBuffPred[0][2] = pMVBuffPred[1][2] = dec->mvs[2];

    pmv = get_pmv2(dec->mb_width,pMVBuffPred,3);
    dec_mv(fcode,&dec->mvs[3],&pmv,bs);
    pMVBuffPred[0][3] = pMVBuffPred[1][3] = dec->mvs[3];

    next_pos = dec->edged_width << 3;
    coord_x = x_pos << 4;
    coord_y = y_pos << 4;

    y_add_temp = coord_y * dec->edged_width + coord_x;
    uv_add_temp = (y_add_temp + coord_x) >> 2;;
    y_add_temp = ((uint32_t)dec->y_row_start)-((uint32_t)dec->cur.y)+coord_x;
    uv_add_temp = ((uint32_t)dec->u_row_start)-((uint32_t)dec->cur.u)+(x_pos<<3);
    pY_Cur = dec->y_row_start + coord_x;
    pU_Cur = dec->u_row_start + (x_pos << 3);
    pV_Cur = dec->v_row_start + (x_pos << 3);
    {
        int16_t coordx_tmp = (dec->width-coord_x)<<1;
        int16_t coordy_tmp = (dec->height-coord_y)<<1;
        int32_t coordx_tmp2 = (EDGE_SIZE + coord_x)<<1;
        int32_t coordy_tmp2 = (EDGE_SIZE + coord_y)<<1;
        int mv_dtmp = dec->mvs[0].x + coordx_tmp2;
        y1_dx = dec->mvs[0].x + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y1_dx - coordx_tmp;
        y1_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
        mv_dtmp = dec->mvs[0].y + coordy_tmp2;
        y1_dy = dec->mvs[0].y + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y1_dy - coordy_tmp;
        y1_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

        mv_dtmp = dec->mvs[1].x + coordx_tmp2;
        y2_dx = dec->mvs[1].x + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y2_dx - coordx_tmp;
        y2_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
        mv_dtmp = dec->mvs[1].y + coordy_tmp2;
        y2_dy = dec->mvs[1].y + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y2_dy - coordy_tmp;
        y2_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

        mv_dtmp = dec->mvs[2].x + coordx_tmp2;
        y3_dx = dec->mvs[2].x + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y3_dx - coordx_tmp;
        y3_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
        mv_dtmp = dec->mvs[2].y + coordy_tmp2;
        y3_dy = dec->mvs[2].y + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y3_dy - coordy_tmp;
        y3_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);

        mv_dtmp = dec->mvs[3].x + coordx_tmp2;
        y4_dx = dec->mvs[3].x + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y4_dx - coordx_tmp;
        y4_dx = coordx_tmp + ((mv_dtmp>>31)&mv_dtmp);
        mv_dtmp = dec->mvs[3].y + coordy_tmp2;
        y4_dy = dec->mvs[3].y + ((mv_dtmp>>31)&(-mv_dtmp));
        mv_dtmp = y4_dy - coordy_tmp;
        y4_dy = coordy_tmp + ((mv_dtmp>>31)&mv_dtmp);
    }
    uv_dx = y1_dx + y2_dx + y3_dx + y4_dx;
    uv_dy = y1_dy + y2_dy + y3_dy + y4_dy;

    uv_dx = (uv_dx >> 3) + roundtab_76[uv_dx & 0xf];
    uv_dy = (uv_dy >> 3) + roundtab_76[uv_dy & 0xf];

    interpolate8x8_switch(block[0], dec->refn[0].y + y_add_temp,
                          y1_dx, y1_dy, dec->stride[0], rounding);
    interpolate8x8_switch(block[0]+64, dec->refn[0].y + y_add_temp+8,
                          y2_dx, y2_dy, dec->stride[1], rounding);
    interpolate8x8_switch(block[0]+128, dec->refn[0].y + y_add_temp+next_pos,
                          y3_dx, y3_dy, dec->stride[2], rounding);
    interpolate8x8_switch(block[0]+192, dec->refn[0].y + y_add_temp+next_pos+8,
                          y4_dx, y4_dy, dec->stride[3], rounding);

    interpolate8x8_switch(block[1],dec->refn[0].u + uv_add_temp,uv_dx, uv_dy, dec->stride[4], rounding);
    interpolate8x8_switch(block[2],dec->refn[0].v + uv_add_temp,uv_dx, uv_dy, dec->stride[5], rounding);
    if (cbp)
    {
        decoder_mb_decode(dec, cbp, bs, block[0], block[1], block[2]);
    }
#if 0
    //y
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)block[0];
        uint32_t *dst_addr_tmp = (uint32_t *)pY_Cur;
        uint32_t stride_tmp = (dec->edged_width >> 2);
        uint32_t *dst_addr_stride = dst_addr_tmp + (dec->edged_width << 1);
        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = ref_addr_tmp0[0];
            dst_addr_tmp[1] = ref_addr_tmp0[1];
            dst_addr_tmp[2] = ref_addr_tmp0[16];
            dst_addr_tmp[3] = ref_addr_tmp0[17];

            dst_addr_stride[0] = ref_addr_tmp0[32];
            dst_addr_stride[1] = ref_addr_tmp0[33];
            dst_addr_stride[2] = ref_addr_tmp0[48];
            dst_addr_stride[3] = ref_addr_tmp0[49];

            dst_addr_tmp += stride_tmp;
            dst_addr_stride += stride_tmp;
            ref_addr_tmp0 += 2;
        }
    }
    //u
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)block[1];
        uint32_t *dst_addr_tmp = (uint32_t *)pU_Cur;
        uint32_t stride_tmp = (dec->edged_width >> 3);

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = ref_addr_tmp0[0];
            dst_addr_tmp[1] = ref_addr_tmp0[1];

            dst_addr_tmp += stride_tmp;
            ref_addr_tmp0 += 2;
        }
    }
    //v
    {
        uint32_t *ref_addr_tmp0 = (uint32_t *)block[2];
        uint32_t *dst_addr_tmp = (uint32_t *)pV_Cur;
        uint32_t stride_tmp = (dec->edged_width >> 3);

        for(i = 8; i > 0; i--)
        {
            dst_addr_tmp[0] = ref_addr_tmp0[0];
            dst_addr_tmp[1] = ref_addr_tmp0[1];

            dst_addr_tmp += stride_tmp;
            ref_addr_tmp0 += 2;
        }
    }
#else
    {
        uint32_t *y_ref_addr_tmp0 = (uint32_t *)block[0];
        uint32_t *y_dst_addr_tmp = (uint32_t *)pY_Cur;
        uint32_t y_stride_tmp = (dec->edged_width >> 2);
        uint32_t *dst_addr_stride = y_dst_addr_tmp + (dec->edged_width << 1);

        uint32_t *u_ref_addr_tmp0 = (uint32_t *)block[1];
        uint32_t *u_dst_addr_tmp = (uint32_t *)pU_Cur;
        uint32_t uv_stride_tmp = (dec->edged_width >> 3);

        uint32_t *v_ref_addr_tmp0 = (uint32_t *)block[2];
        uint32_t *v_dst_addr_tmp = (uint32_t *)pV_Cur;

        for(i = 8; i > 0; i--)
        {
            y_dst_addr_tmp[0] = y_ref_addr_tmp0[0];
            y_dst_addr_tmp[1] = y_ref_addr_tmp0[1];
            y_dst_addr_tmp[2] = y_ref_addr_tmp0[16];
            y_dst_addr_tmp[3] = y_ref_addr_tmp0[17];

            dst_addr_stride[0] = y_ref_addr_tmp0[32];
            dst_addr_stride[1] = y_ref_addr_tmp0[33];
            dst_addr_stride[2] = y_ref_addr_tmp0[48];
            dst_addr_stride[3] = y_ref_addr_tmp0[49];

            y_dst_addr_tmp += y_stride_tmp;
            dst_addr_stride += y_stride_tmp;
            y_ref_addr_tmp0 += 2;

            u_dst_addr_tmp[0] = u_ref_addr_tmp0[0];
            u_dst_addr_tmp[1] = u_ref_addr_tmp0[1];

            u_dst_addr_tmp += uv_stride_tmp;
            u_ref_addr_tmp0 += 2;

            v_dst_addr_tmp[0] = v_ref_addr_tmp0[0];
            v_dst_addr_tmp[1] = v_ref_addr_tmp0[1];

            v_dst_addr_tmp += uv_stride_tmp;
            v_ref_addr_tmp0 += 2;
        }
    }
#endif
//  macroblock_number ++;//test by longyl
}

