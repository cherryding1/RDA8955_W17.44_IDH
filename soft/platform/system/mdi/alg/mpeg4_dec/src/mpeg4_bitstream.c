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

#include <string.h>
#include <stdio.h>

#include "mpeg4_bitstream.h"
#include "zigzag.h"
#include "mbcoding.h"

const uint32_t msk[33] =
{
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000f, 0x0000001f, 0x0000003f, 0x0000007f,
    0x000000ff, 0x000001ff, 0x000003ff, 0x000007ff,
    0x00000fff, 0x00001fff, 0x00003fff, 0x00007fff,
    0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
    0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff,
    0x00ffffff, 0x01ffffff, 0x03ffffff, 0x07ffffff,
    0x0fffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff,
    0xffffffff
};



static const uint8_t log2_tab_16[16] =  { 0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4 };

//static uint32_t __inline log2bin(uint32_t value)
uint32_t log2bin(uint32_t value)
{
    int n = 0;
    if (value & 0xffff0000)
    {
        value >>= 16;
        n += 16;
    }
    if (value & 0xff00)
    {
        value >>= 8;
        n += 8;
    }
    if (value & 0xf0)
    {
        value >>= 4;
        n += 4;
    }
    return n + log2_tab_16[value];
}

//static const uint32_t intra_dc_threshold_table[] = {
const uint32_t intra_dc_threshold_table[] =
{
    32,                         /* never use */
    13,
    15,
    17,
    19,
    21,
    23,
    1,
};

#ifndef MEDIA_VOCVID_SUPPORT
/*
 * for PVOP addbits == fcode - 1
 * for BVOP addbits == max(fcode,bcode) - 1
 * returns mbpos
 */
int
read_video_packet_header(Bitstream *bs,
                         DECODER * dec,
                         const int addbits,
                         int * quant,
                         int * fcode_forward,
                         int  * fcode_backward,
                         int * intra_dc_threshold)
{
    int startcode_bits = NUMBITS_VP_RESYNC_MARKER + addbits;
    int mbnum_bits = log2bin(dec->mb_width *  dec->mb_height - 1);
    int mbnum;
    int nbits,temp_code;
    int hec = 0;
    _NUMBITSTOBYTEALIGN(bs->pos,nbits);
    while(bs->pos<24)//sheen
    {
        _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
    }
    bs->pos -= (nbits+startcode_bits);

    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    _GETBITS(bs->buf,bs->pos,mbnum_bits,mbnum);
    if(bs->pos < 16)//sheen
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }

    _GETBITS(bs->buf,bs->pos,dec->quant_bits,*quant);
    if(bs->pos < 16)//sheen
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }

    _GETBITS(bs->buf,bs->pos,1,hec);

    if (hec)
    {
        int time_base;
        int time_increment;
        int coding_type;
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }
        _GETBITS(bs->buf,bs->pos,1,temp_code);
        for (time_base=0; temp_code!=0; time_base++)        /* modulo_time_base */
        {
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if(bs->pos < 16)//sheen
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }

        }
        _GETBITS(bs->buf,bs->pos,1,temp_code);
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }

        if (dec->time_inc_bits)
            _GETBITS(bs->buf,bs->pos,dec->time_inc_bits,time_increment);    /* vop_time_increment */
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }

        _GETBITS(bs->buf,bs->pos,1,temp_code);
        _GETBITS(bs->buf,bs->pos,2,coding_type);
        _GETBITS(bs->buf,bs->pos,3,temp_code);
        *intra_dc_threshold = intra_dc_threshold_table[temp_code];

        if (coding_type != I_VOP && fcode_forward)
        {
            _GETBITS(bs->buf,bs->pos,3,*fcode_forward);
        }
        if (coding_type == B_VOP && fcode_backward)
        {
            _GETBITS(bs->buf,bs->pos,3,*fcode_backward);
        }
    }
    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    return mbnum;
}
#endif// MEDIA_VOCVID_SUPPORT
/*
decode headers
returns coding_type, or -1 if error
*/

#if 0//move to xvid.c for overlay
#define VIDOBJ_START_CODE_MASK      0x0000001f
#define VIDOBJLAY_START_CODE_MASK   0x0000000f

int
BitstreamReadHeaders(Bitstream * bs,
                     DECODER * dec,
                     uint16_t * rounding,
                     uint16_t * quant,
                     uint16_t * fcode_forward,
                     uint16_t * intra_dc_threshold)
{
    uint16_t vol_ver_id;
    uint16_t coding_type;
    uint32_t start_code;
    uint32_t temp_code;
    uint32_t time_incr = 0;
    int32_t time_increment = 0;
    int resize = 0;

    while ((BitstreamPos(bs) >> 3) + 4 <= bs->length)
    {

        _BITSTREAMBYTEALIGN(bs->pos);
        while(bs->pos < 32)//sheen
        {
            _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
        }
        _SHOWBITS(bs->buf,bs->pos,32,start_code);

        if (start_code == VISOBJSEQ_START_CODE)
        {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            _GETBITS(bs->buf,bs->pos,8,temp_code);
            _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
            /*      if(temp_code != XVID_PROFILE_S_L0 && temp_code != XVID_PROFILE_S_L1 &&
                         temp_code != XVID_PROFILE_S_L2 && temp_code != XVID_PROFILE_S_L3)//profile unfit
                         return -8;*/
        }
        else if ((start_code & ~VIDOBJLAY_START_CODE_MASK) == VIDOBJLAY_START_CODE)
        {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            bs->pos -= 9;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* is_object_layer_identifier */
            {
                _GETBITS(bs->buf,bs->pos,4,vol_ver_id);
                bs->pos -= 3;
            }
            else
            {
                vol_ver_id = dec->ver_id;
            }
            _GETBITS(bs->buf,bs->pos,4,dec->aspect_ratio);
            if (dec->aspect_ratio == VIDOBJLAY_AR_EXTPAR)   /* aspect_ratio_info */
            {
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,8,dec->par_width);
                _GETBITS(bs->buf,bs->pos,8,dec->par_height);
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            if (temp_code)  /* vol_control_parameters */
            {
                bs->pos -= 2;
                _GETBITS(bs->buf,bs->pos,1,dec->low_delay);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if (temp_code)  /* vbv_parameters */
                {
                    unsigned int bitrate;
                    unsigned int buffer_size;
                    unsigned int occupancy;

                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,bitrate);
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    temp_code = bitrate << 15;
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,bitrate);
                    bitrate |= temp_code;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,buffer_size);
                    buffer_size <<= 3;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,3,temp_code);
                    buffer_size |= temp_code;
                    _GETBITS(bs->buf,bs->pos,11,occupancy);
                    occupancy <<= 15;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                    if(bs->pos < 16)
                    {
                        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                    }
                    _GETBITS(bs->buf,bs->pos,15,temp_code);
                    occupancy |= temp_code;
                    _GETBITS(bs->buf,bs->pos,1,temp_code);
                }
            }
            else
            {
                dec->low_delay = dec->low_delay_default;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,2,dec->shape);
            if (dec->shape != VIDOBJLAY_SHAPE_RECTANGULAR)
            {
                return -1;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            /********************** for decode B-frame time ***********************/
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,16,dec->time_inc_resolution);
            if (dec->time_inc_resolution > 0)
            {
                dec->time_inc_bits = MAX_16(log2bin(dec->time_inc_resolution-1), 1);
            }
            else
            {
                /* for "old" xvid compatibility, set time_inc_bits = 1 */
                dec->time_inc_bits = 1;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* fixed_vop_rate */
            {
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                bs->pos -= dec->time_inc_bits;
            }
            {
                uint32_t width, height;
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                _GETBITS(bs->buf,bs->pos,13,width);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,13,height);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if (dec->width != width || dec->height != height)
                {
                    if (dec->fixed_dimensions)
                    {
                        return -1;
                    }
                    resize = 1;
                    dec->width = width;
                    dec->height = height;
                }
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if(temp_code)
            {
                return -1;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code) /* obmc_disable */
            {
                return -1;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            _GETBITS(bs->buf,bs->pos,(vol_ver_id == 1 ? 1 : 2),temp_code);
            if (temp_code == SPRITE_STATIC || temp_code == SPRITE_GMC)
            {
                return -1;
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* not_8_bit */
            {
                return -1;
            }
            dec->quant_bits = 5;
            _GETBITS(bs->buf,bs->pos,1,dec->quant_type);
            if (vol_ver_id != 1)
            {
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if(temp_code)
                {
                    return -1;
                }

            }
            else
                temp_code = 0;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code)
            {
                return -1;
            }
            bs->pos --;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)  /* data_partitioned */
            {
                return -1;
            }

            if (vol_ver_id != 1)
            {
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if (temp_code)  /* newpred_enable */
                {
                    return -1;
                }
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                if(temp_code)
                {
                    return -1;
                }
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (temp_code)
            {
                return -1;
            }
            return (resize ? -3 : -2 ); /* VOL */

        } /*else if (start_code == GRPOFVOP_START_CODE) {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            {
                int hours, minutes, seconds;
                _GETBITS(bs->buf,bs->pos,5,hours);
                _GETBITS(bs->buf,bs->pos,1,minutes);
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                _GETBITS(bs->buf,bs->pos,1,seconds);
            }
            bs->pos -= 2;
        }*/ else if (start_code == VOP_START_CODE)
        {
            bs->pos -= 32;
            _PREVIEW32BITS(bs->tail,bs->buf,bs->pos);
            _GETBITS(bs->buf,bs->pos,2,coding_type);
            /*********************** for decode B-frame time ***********************/
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            while (temp_code != 0)  /* time_base */
            {
                _GETBITS(bs->buf,bs->pos,1,temp_code);
                time_incr++;
                if(bs->pos < 8)//sheen
                {
                    _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
                }
            }
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (dec->time_inc_bits)
            {
                if(bs->pos < 16)
                {
                    _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
                }
                _GETBITS(bs->buf,bs->pos,dec->time_inc_bits,time_increment);
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
//          dec->last_time_base = dec->time_base;
            dec->time_base += time_incr;
            dec->time = dec->time_base*dec->time_inc_resolution + time_increment;
            //      dec->time_pp = (int32_t)(dec->time - dec->last_non_b_time);
            //      dec->last_non_b_time = dec->time;
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            _GETBITS(bs->buf,bs->pos,1,temp_code);
            if (!temp_code) /* vop_coded */
            {
                return N_VOP;
            }
            /* fix a little bug by MinChen <chenm002@163.com> */
            if (coding_type == P_VOP)
            {
                _GETBITS(bs->buf,bs->pos,1,*rounding);
            }
            /* intra_dc_vlc_threshold */
            _GETBITS(bs->buf,bs->pos,3,temp_code);
            *intra_dc_threshold =
                intra_dc_threshold_table[temp_code];

            _GETBITS(bs->buf,bs->pos,dec->quant_bits,*quant);
            if (coding_type != I_VOP)
            {
                _GETBITS(bs->buf,bs->pos,3,*fcode_forward);
            }
            return coding_type;
        }
        else                    /* start_code == ? */
        {
            bs->pos -= 8;
            _PREVIEW8BITS(bs->tail,bs->buf,bs->pos)
        }
    }
    return -1;                  /* ignore it */
}
#endif

#endif

