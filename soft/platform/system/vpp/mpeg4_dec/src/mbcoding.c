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

#include "portab.h"
#include "mpeg4_global.h"
#include "mpeg4_bitstream.h"
#include "zigzag.h"
#include "vlc_codes.h"
#include "mbcoding.h"

//#include "../utils/mbfunctions.h"

//#define LEVELOFFSET 32 //move to xvid.c for overlay

const uint16 scan_tables[3][64] =
{
    /* zig_zag_scan */
    {
        0,  1,  8, 16,  9,  2,  3, 10,
        17, 24, 32, 25, 18, 11,  4,  5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13,  6,  7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63
    },

    /* horizontal_scan */
    {
        0,  1,  2,  3,  8,  9, 16, 17,
        10, 11,  4,  5,  6,  7, 15, 14,
        13, 12, 19, 18, 24, 25, 32, 33,
        26, 27, 20, 21, 22, 23, 28, 29,
        30, 31, 34, 35, 40, 41, 48, 49,
        42, 43, 36, 37, 38, 39, 44, 45,
        46, 47, 50, 51, 56, 57, 58, 59,
        52, 53, 54, 55, 60, 61, 62, 63
    },

    /* vertical_scan */
    {
        0,  8, 16, 24,  1,  9,  2, 10,
        17, 25, 32, 40, 48, 56, 57, 49,
        41, 33, 26, 18,  3, 11,  4, 12,
        19, 27, 34, 42, 50, 58, 35, 43,
        51, 59, 20, 28,  5, 13,  6, 14,
        21, 29, 36, 44, 52, 60, 37, 45,
        53, 61, 22, 30,  7, 15, 23, 31,
        38, 46, 54, 62, 39, 47, 55, 63
    }
};

/* Initialized once during xvid_global call
 * RO access is thread safe */

//#ifdef NOACC

//static REVERSE_EVENT DCT3D[2][4096];
extern REVERSE_EVENT DCT3D[2][4096];//move to xvid.c for overlay
//#endif

//#ifdef NOACC
//static VLC coeff_VLC[2][2][64][64];
extern VLC coeff_VLC[2][2][64][64];//move to xvid.c for overlay

#if 0 //move to xvid.c for overlay
void
init_vlc_tables(void)
{
    uint32_t i, j, intra, last, run,  run_esc, level, level_esc, escape, escape_len, offset;
    //int32_t l;
    //uint32_t k;

    for (intra = 0; intra < 2; intra++)
        for (i = 0; i < 4096; i++)
            DCT3D[intra][i].event.level = 0;

    for (intra = 0; intra < 2; intra++)
    {
        for (last = 0; last < 2; last++)
        {
            for (run = 0; run < 63 + last; run++)
            {
                for (level = 0; level < (uint32_t)(32 << intra); level++)
                {
                    offset = !intra * LEVELOFFSET;
                    coeff_VLC[intra][last][level + offset][run].len = 128;
                }
            }
        }
    }

    for (intra = 0; intra < 2; intra++)
    {
        for (i = 0; i < 102; i++)
        {
            offset = !intra * LEVELOFFSET;

            for (j = 0; j < (uint32_t)(1 << (12 - coeff_tab[intra][i].vlc.len)); j++)
            {
                DCT3D[intra][(coeff_tab[intra][i].vlc.code << (12 - coeff_tab[intra][i].vlc.len)) | j].len   = coeff_tab[intra][i].vlc.len;
                DCT3D[intra][(coeff_tab[intra][i].vlc.code << (12 - coeff_tab[intra][i].vlc.len)) | j].event = coeff_tab[intra][i].event;
            }

            coeff_VLC[intra][coeff_tab[intra][i].event.last][coeff_tab[intra][i].event.level + offset][coeff_tab[intra][i].event.run].code
                = coeff_tab[intra][i].vlc.code << 1;
            coeff_VLC[intra][coeff_tab[intra][i].event.last][coeff_tab[intra][i].event.level + offset][coeff_tab[intra][i].event.run].len
                = coeff_tab[intra][i].vlc.len + 1;

            if (!intra)
            {
                coeff_VLC[intra][coeff_tab[intra][i].event.last][offset - coeff_tab[intra][i].event.level][coeff_tab[intra][i].event.run].code
                    = (coeff_tab[intra][i].vlc.code << 1) | 1;
                coeff_VLC[intra][coeff_tab[intra][i].event.last][offset - coeff_tab[intra][i].event.level][coeff_tab[intra][i].event.run].len
                    = coeff_tab[intra][i].vlc.len + 1;
            }
        }
    }

    for (intra = 0; intra < 2; intra++)
    {
        for (last = 0; last < 2; last++)
        {
            for (run = 0; run < 63 + last; run++)
            {
                for (level = 1; level < (uint32_t)(32 << intra); level++)
                {

                    if (level <= max_level[intra][last][run] && run <= max_run[intra][last][level])
                        continue;

                    offset = !intra * LEVELOFFSET;
                    level_esc = level - max_level[intra][last][run];
                    run_esc = run - 1 - max_run[intra][last][level];

                    if (level_esc <= max_level[intra][last][run] && run <= max_run[intra][last][level_esc])
                    {
                        escape     = ESCAPE1;
                        escape_len = 7 + 1;
                        run_esc    = run;
                    }
                    else
                    {
                        if (run_esc <= max_run[intra][last][level] && level <= max_level[intra][last][run_esc])
                        {
                            escape     = ESCAPE2;
                            escape_len = 7 + 2;
                            level_esc  = level;
                        }
                        else
                        {
                            if (!intra)
                            {
                                coeff_VLC[intra][last][level + offset][run].code
                                    = (ESCAPE3 << 21) | (last << 20) | (run << 14) | (1 << 13) | ((level & 0xfff) << 1) | 1;
                                coeff_VLC[intra][last][level + offset][run].len = 30;
                                coeff_VLC[intra][last][offset - level][run].code
                                    = (ESCAPE3 << 21) | (last << 20) | (run << 14) | (1 << 13) | ((-(int32_t)level & 0xfff) << 1) | 1;
                                coeff_VLC[intra][last][offset - level][run].len = 30;
                            }
                            continue;
                        }
                    }

                    coeff_VLC[intra][last][level + offset][run].code
                        = (escape << coeff_VLC[intra][last][level_esc + offset][run_esc].len)
                          |  coeff_VLC[intra][last][level_esc + offset][run_esc].code;
                    coeff_VLC[intra][last][level + offset][run].len
                        = coeff_VLC[intra][last][level_esc + offset][run_esc].len + escape_len;

                    if (!intra)
                    {
                        coeff_VLC[intra][last][offset - level][run].code
                            = (escape << coeff_VLC[intra][last][level_esc + offset][run_esc].len)
                              |  coeff_VLC[intra][last][level_esc + offset][run_esc].code | 1;
                        coeff_VLC[intra][last][offset - level][run].len
                            = coeff_VLC[intra][last][level_esc + offset][run_esc].len + escape_len;
                    }
                }

                if (!intra)
                {
                    coeff_VLC[intra][last][0][run].code
                        = (ESCAPE3 << 21) | (last << 20) | (run << 14) | (1 << 13) | ((-32 & 0xfff) << 1) | 1;
                    coeff_VLC[intra][last][0][run].len = 30;
                }
            }
        }
    }
}
#endif

//#endif

/***************************************************************
 * decoding stuff starts here                                  *
 ***************************************************************/


/*
 * for IVOP addbits == 0
 * for PVOP addbits == fcode - 1
 * for BVOP addbits == max(fcode,bcode) - 1
 * returns true or false
 */
int
check_resync_marker(Bitstream * bs, int addbits)
{
    uint32_t nbits;
    uint32_t code;
    uint32_t bspos;
    uint32_t nbitsresyncmarker = NUMBITS_VP_RESYNC_MARKER + addbits;
    _NUMBITSTOBYTEALIGN(bs->pos,nbits);
    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    _SHOWBITS(bs->buf,bs->pos,nbits,code);
    if (code == (((uint32_t)1 << (nbits - 1)) - 1))
    {
        bspos = bs->pos - nbits;
        if(bspos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            bspos += 16;
        }
        if(bspos < 24)
        {
            _PREVIEW8BITS(bs->tail,bs->buf,bs->pos);
            bspos += 8;
        }
        _SHOWBITS(bs->buf,bspos,nbitsresyncmarker,code);
        return code == RESYNC_MARKER;
    }

    return 0;
}

#if 0
static __inline int
get_mv_data(Bitstream * bs)
{

    uint32_t index,temp_code;
    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    _GETBITS(bs->buf,bs->pos,1,temp_code);
    if (temp_code)
        return 0;
    _SHOWBITS(bs->buf,bs->pos,12,index);
    if (index >= 512)
    {
        index = (index >> 8) - 2;
        bs->pos -= TMNMVtab0[index].len;
        return TMNMVtab0[index].code;
    }

    if (index >= 128)
    {
        index = (index >> 2) - 32;
        bs->pos -= TMNMVtab1[index].len;
        return TMNMVtab1[index].code;
    }

    index -= 4;
    bs->pos -= TMNMVtab2[index].len;
    return TMNMVtab2[index].code;
}
#endif

int32_t
dec_mv(int fcode,VECTOR *retMV,VECTOR * predMV,Bitstream *bs)
{
    int scale_fac;
    int high;
    int low;
    int range;
    int res;
    uint32_t index,temp_code;

    fcode --;
    scale_fac = 1 << fcode;
    high  = scale_fac << 5;
    low   = - high;
    range = high << 1;
    high --;

    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    if(scale_fac != 1)
    {
        //vector-x
        _GETBITS(bs->buf,bs->pos,1,temp_code);
        if (temp_code)
        {
            retMV->x = 0;
        }
        else
        {
            _SHOWBITS(bs->buf,bs->pos,12,index);
            if (index >= 512)
            {
                index = (index >> 8) - 2;
                bs->pos -= TMNMVtab0[index].len;
                retMV->x = TMNMVtab0[index].code;
            }
            else if (index >= 128)
            {
                index = (index >> 2) - 32;
                bs->pos -= TMNMVtab1[index].len;
                retMV->x = TMNMVtab1[index].code;
            }
            else
            {
                index -= 4;
                bs->pos -= TMNMVtab2[index].len;
                retMV->x = TMNMVtab2[index].code;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            if(retMV->x > 0)
            {
                _GETBITS(bs->buf,bs->pos,fcode,res);
                retMV->x = (retMV->x -1) * scale_fac + res + 1;
            }
            else
            {
                _GETBITS(bs->buf,bs->pos,fcode,res);
                retMV->x = (retMV->x + 1) * scale_fac - res -1;
            }
        }
        //vector-y
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }
        _GETBITS(bs->buf,bs->pos,1,temp_code);
        if (temp_code)
        {
            retMV->y = 0;
        }
        else
        {
            _SHOWBITS(bs->buf,bs->pos,12,index);
            if (index >= 512)
            {
                index = (index >> 8) - 2;
                bs->pos -= TMNMVtab0[index].len;
                retMV->y = TMNMVtab0[index].code;
            }
            else if (index >= 128)
            {
                index = (index >> 2) - 32;
                bs->pos -= TMNMVtab1[index].len;
                retMV->y = TMNMVtab1[index].code;
            }
            else
            {
                index -= 4;
                bs->pos -= TMNMVtab2[index].len;
                retMV->y = TMNMVtab2[index].code;
            }
            if(bs->pos < 16)
            {
                _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
            }
            if(retMV->y > 0)
            {
                _GETBITS(bs->buf,bs->pos,fcode,res);
                retMV->y = (retMV->y -1) * scale_fac + res + 1;
            }
            else
            {
                _GETBITS(bs->buf,bs->pos,fcode,res);
                retMV->y = (retMV->y + 1) * scale_fac - res -1;
            }
        }
    }
    else
    {
        //vector-x
        _GETBITS(bs->buf,bs->pos,1,temp_code);
        if (temp_code)
        {
            retMV->x = 0;
        }
        else
        {
            _SHOWBITS(bs->buf,bs->pos,12,index);
            if (index >= 512)
            {
                index = (index >> 8) - 2;
                bs->pos -= TMNMVtab0[index].len;
                retMV->x = TMNMVtab0[index].code;
            }
            else if (index >= 128)
            {
                index = (index >> 2) - 32;
                bs->pos -= TMNMVtab1[index].len;
                retMV->x = TMNMVtab1[index].code;
            }
            else
            {
                index -= 4;
                bs->pos -= TMNMVtab2[index].len;
                retMV->x = TMNMVtab2[index].code;
            }
        }
        //vector-y
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }
        _GETBITS(bs->buf,bs->pos,1,temp_code);
        if (temp_code)
        {
            retMV->y = 0;
        }
        else
        {
            _SHOWBITS(bs->buf,bs->pos,12,index);
            if (index >= 512)
            {
                index = (index >> 8) - 2;
                bs->pos -= TMNMVtab0[index].len;
                retMV->y = TMNMVtab0[index].code;
            }
            else if (index >= 128)
            {
                index = (index >> 2) - 32;
                bs->pos -= TMNMVtab1[index].len;
                retMV->y = TMNMVtab1[index].code;
            }
            else
            {
                index -= 4;
                bs->pos -= TMNMVtab2[index].len;
                retMV->y = TMNMVtab2[index].code;
            }
        }
    }
    retMV->x += predMV->x;
    retMV->y += predMV->y;

    retMV->x += ((retMV->x - low) >> 31) & range;
    retMV->x -= ((high - retMV->x) >> 31) & range;

    retMV->y += ((retMV->y - low) >> 31) & range;
    retMV->y -= ((high - retMV->y) >> 31) & range;

    return 0;
}

int
get_dc_dif(Bitstream * bs,
           uint32_t dc_size)
{

    int code;
    int msb;

    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    _GETBITS(bs->buf,bs->pos,dc_size,code);
    msb = code >> (dc_size - 1);
#if 1
    if (msb == 0)
        return (-(code ^ ((1 << dc_size) - 1)));
#else
    return (code + (((msb-1)>>31)&(-(code ^ ((1 << dc_size) - 1)))-code));
#endif
    return code;
}

int
get_dc_size_lum(Bitstream * bs)
{

    int code, i;

    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    _SHOWBITS(bs->buf,bs->pos,11,code);

    if(code > 256)
    {
        code >>= 8;
        bs->pos -= dc_lum_tab[code].len;
        return dc_lum_tab[code].code;
    }
    else
    {
        for (i = 12; code; i--)
        {
            code >>= 1;
        }
        bs->pos -= i;
        return i+1;
    }
}


int
get_dc_size_chrom(Bitstream * bs)
{

    uint32_t code, i;

    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    _SHOWBITS(bs->buf,bs->pos,12,code);

    if(code > 1024)
    {
        _GETBITS(bs->buf,bs->pos,2,code);
        return 3 - code;
    }
    else
    {
        for (i = 13; code; i--)
        {
            code >>= 1;
        }
        bs->pos -= i;
        return i;
    }
}


//#ifdef NOACC

static __inline int
get_coeff(Bitstream * bs,
          int *run,
          int *last,
          int intra,
          int short_video_header)
{

    uint32_t mode;
    int32_t level;
    uint32_t temp_code;
    REVERSE_EVENT *reverse_event;

    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    if (short_video_header)     /* inter-VLCs will be used for both intra and inter blocks */
        intra = 0;

    _SHOWBITS(bs->buf,bs->pos,7,temp_code);
    if (temp_code != ESCAPE)
    {
        _SHOWBITS(bs->buf,bs->pos,12,temp_code);
        reverse_event = &DCT3D[intra][temp_code];

        if ((level = reverse_event->event.level) == 0)
            goto error;

        *last = reverse_event->event.last;
        *run  = reverse_event->event.run;

        /* Don't forget to update the bitstream position */
        _GETBITS(bs->buf,bs->pos,reverse_event->len+1,temp_code);
        return (temp_code&0x01) ? -level : level;
    }
    bs->pos -= 7;

    if (short_video_header)
    {
        /* escape mode 4 - H.263 type, only used if short_video_header = 1  */
        _GETBITS(bs->buf,bs->pos,1,*last);
        _GETBITS(bs->buf,bs->pos,6,*run);
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }
        _GETBITS(bs->buf,bs->pos,8,level);
        if (level == 0 || level == 128)
            ;
        return (level << 24) >> 24;
    }
    _SHOWBITS(bs->buf,bs->pos,2,mode);
    if (mode < 3)
    {
        const int skip[3] = {1, 1, 2};
        bs->pos -= skip[mode];
        if(bs->pos < 16)
        {
            _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
        }
        _SHOWBITS(bs->buf,bs->pos,12,temp_code);
        reverse_event = &DCT3D[intra][temp_code];

        if ((level = reverse_event->event.level) == 0)
            goto error;

        *last = reverse_event->event.last;
        *run  = reverse_event->event.run;

        if (mode < 2)
        {
            /* first escape mode, level is offset */
            level += max_level[intra][*last][*run];
        }
        else
        {
            /* second escape mode, run is offset */
            *run += max_run[intra][*last][level] + 1;
        }

        /* Update bitstream position */
        _GETBITS(bs->buf,bs->pos,(reverse_event->len+1),temp_code);

        return (temp_code&0x01) ? -level : level;
    }
    /* third escape mode - fixed length codes */
    bs->pos -= 2;
    _GETBITS(bs->buf,bs->pos,1,*last);
    _GETBITS(bs->buf,bs->pos,6,*run);
    if(bs->pos < 16)
    {
        _PREVIEW16BITS(bs->tail,bs->buf,bs->pos);
    }
    bs->pos --;
    _GETBITS(bs->buf,bs->pos,12,level);
    bs->pos --;
    return (level << 20) >> 20;

error:
    *run = VLC_ERROR;
    return 0;
}


void
get_intra_block(Bitstream * bs,
                int16_t * block,
                int direction,
                int coeff)
{

    const uint16_t *scan = scan_tables[direction];
    int level, run, last;

    do
    {
        level = get_coeff(bs, &run, &last, 1, 0);
        if (run == -1)
        {
            break;
        }
        coeff += run;

#ifdef _DEBUG
        if(coeff>=64)
        {
            return;
        }
#endif

        block[scan[coeff]] = level;
        coeff++;
    }
    while (!last);

}


void
get_inter_block_h263(
    Bitstream * bs,
    int16_t * block,
    int direction,
    const int quant)
{

    const uint16_t *scan = scan_tables[direction];
    const uint16_t quant_m_2 = quant << 1;
    const uint16_t quant_add = quant - 1 + (quant & 1);
    int p;
    int level;
    int run;
    int last;

    p = 0;
    do
    {
        level = get_coeff(bs, &run, &last, 0, 0);
        if (run == -1)
        {
            break;
        }
        p += run;
        if (level < 0)
        {
            level = level*quant_m_2 - quant_add;
            block[scan[p]] = (level >= -2048 ? level : -2048);
        }
        else
        {
            level = level * quant_m_2 + quant_add;
            block[scan[p]] = (level <= 2047 ? level : 2047);
        }
        p++;
    }
    while (!last);
}

#if 0

/*****************************************************************************
 * VLC tables and other constant arrays
 ****************************************************************************/

VLC_TABLE const coeff_tab[2][102] =
{
    /* intra = 0 */
    {
        {{ 2,  2}, {0, 0, 1}},
        {{15,  4}, {0, 0, 2}},
        {{21,  6}, {0, 0, 3}},
        {{23,  7}, {0, 0, 4}},
        {{31,  8}, {0, 0, 5}},
        {{37,  9}, {0, 0, 6}},
        {{36,  9}, {0, 0, 7}},
        {{33, 10}, {0, 0, 8}},
        {{32, 10}, {0, 0, 9}},
        {{ 7, 11}, {0, 0, 10}},
        {{ 6, 11}, {0, 0, 11}},
        {{32, 11}, {0, 0, 12}},
        {{ 6,  3}, {0, 1, 1}},
        {{20,  6}, {0, 1, 2}},
        {{30,  8}, {0, 1, 3}},
        {{15, 10}, {0, 1, 4}},
        {{33, 11}, {0, 1, 5}},
        {{80, 12}, {0, 1, 6}},
        {{14,  4}, {0, 2, 1}},
        {{29,  8}, {0, 2, 2}},
        {{14, 10}, {0, 2, 3}},
        {{81, 12}, {0, 2, 4}},
        {{13,  5}, {0, 3, 1}},
        {{35,  9}, {0, 3, 2}},
        {{13, 10}, {0, 3, 3}},
        {{12,  5}, {0, 4, 1}},
        {{34,  9}, {0, 4, 2}},
        {{82, 12}, {0, 4, 3}},
        {{11,  5}, {0, 5, 1}},
        {{12, 10}, {0, 5, 2}},
        {{83, 12}, {0, 5, 3}},
        {{19,  6}, {0, 6, 1}},
        {{11, 10}, {0, 6, 2}},
        {{84, 12}, {0, 6, 3}},
        {{18,  6}, {0, 7, 1}},
        {{10, 10}, {0, 7, 2}},
        {{17,  6}, {0, 8, 1}},
        {{ 9, 10}, {0, 8, 2}},
        {{16,  6}, {0, 9, 1}},
        {{ 8, 10}, {0, 9, 2}},
        {{22,  7}, {0, 10, 1}},
        {{85, 12}, {0, 10, 2}},
        {{21,  7}, {0, 11, 1}},
        {{20,  7}, {0, 12, 1}},
        {{28,  8}, {0, 13, 1}},
        {{27,  8}, {0, 14, 1}},
        {{33,  9}, {0, 15, 1}},
        {{32,  9}, {0, 16, 1}},
        {{31,  9}, {0, 17, 1}},
        {{30,  9}, {0, 18, 1}},
        {{29,  9}, {0, 19, 1}},
        {{28,  9}, {0, 20, 1}},
        {{27,  9}, {0, 21, 1}},
        {{26,  9}, {0, 22, 1}},
        {{34, 11}, {0, 23, 1}},
        {{35, 11}, {0, 24, 1}},
        {{86, 12}, {0, 25, 1}},
        {{87, 12}, {0, 26, 1}},
        {{ 7,  4}, {1, 0, 1}},
        {{25,  9}, {1, 0, 2}},
        {{ 5, 11}, {1, 0, 3}},
        {{15,  6}, {1, 1, 1}},
        {{ 4, 11}, {1, 1, 2}},
        {{14,  6}, {1, 2, 1}},
        {{13,  6}, {1, 3, 1}},
        {{12,  6}, {1, 4, 1}},
        {{19,  7}, {1, 5, 1}},
        {{18,  7}, {1, 6, 1}},
        {{17,  7}, {1, 7, 1}},
        {{16,  7}, {1, 8, 1}},
        {{26,  8}, {1, 9, 1}},
        {{25,  8}, {1, 10, 1}},
        {{24,  8}, {1, 11, 1}},
        {{23,  8}, {1, 12, 1}},
        {{22,  8}, {1, 13, 1}},
        {{21,  8}, {1, 14, 1}},
        {{20,  8}, {1, 15, 1}},
        {{19,  8}, {1, 16, 1}},
        {{24,  9}, {1, 17, 1}},
        {{23,  9}, {1, 18, 1}},
        {{22,  9}, {1, 19, 1}},
        {{21,  9}, {1, 20, 1}},
        {{20,  9}, {1, 21, 1}},
        {{19,  9}, {1, 22, 1}},
        {{18,  9}, {1, 23, 1}},
        {{17,  9}, {1, 24, 1}},
        {{ 7, 10}, {1, 25, 1}},
        {{ 6, 10}, {1, 26, 1}},
        {{ 5, 10}, {1, 27, 1}},
        {{ 4, 10}, {1, 28, 1}},
        {{36, 11}, {1, 29, 1}},
        {{37, 11}, {1, 30, 1}},
        {{38, 11}, {1, 31, 1}},
        {{39, 11}, {1, 32, 1}},
        {{88, 12}, {1, 33, 1}},
        {{89, 12}, {1, 34, 1}},
        {{90, 12}, {1, 35, 1}},
        {{91, 12}, {1, 36, 1}},
        {{92, 12}, {1, 37, 1}},
        {{93, 12}, {1, 38, 1}},
        {{94, 12}, {1, 39, 1}},
        {{95, 12}, {1, 40, 1}}
    },
    /* intra = 1 */
    {
        {{ 2,  2}, {0, 0, 1}},
        {{15,  4}, {0, 0, 3}},
        {{21,  6}, {0, 0, 6}},
        {{23,  7}, {0, 0, 9}},
        {{31,  8}, {0, 0, 10}},
        {{37,  9}, {0, 0, 13}},
        {{36,  9}, {0, 0, 14}},
        {{33, 10}, {0, 0, 17}},
        {{32, 10}, {0, 0, 18}},
        {{ 7, 11}, {0, 0, 21}},
        {{ 6, 11}, {0, 0, 22}},
        {{32, 11}, {0, 0, 23}},
        {{ 6,  3}, {0, 0, 2}},
        {{20,  6}, {0, 1, 2}},
        {{30,  8}, {0, 0, 11}},
        {{15, 10}, {0, 0, 19}},
        {{33, 11}, {0, 0, 24}},
        {{80, 12}, {0, 0, 25}},
        {{14,  4}, {0, 1, 1}},
        {{29,  8}, {0, 0, 12}},
        {{14, 10}, {0, 0, 20}},
        {{81, 12}, {0, 0, 26}},
        {{13,  5}, {0, 0, 4}},
        {{35,  9}, {0, 0, 15}},
        {{13, 10}, {0, 1, 7}},
        {{12,  5}, {0, 0, 5}},
        {{34,  9}, {0, 4, 2}},
        {{82, 12}, {0, 0, 27}},
        {{11,  5}, {0, 2, 1}},
        {{12, 10}, {0, 2, 4}},
        {{83, 12}, {0, 1, 9}},
        {{19,  6}, {0, 0, 7}},
        {{11, 10}, {0, 3, 4}},
        {{84, 12}, {0, 6, 3}},
        {{18,  6}, {0, 0, 8}},
        {{10, 10}, {0, 4, 3}},
        {{17,  6}, {0, 3, 1}},
        {{ 9, 10}, {0, 8, 2}},
        {{16,  6}, {0, 4, 1}},
        {{ 8, 10}, {0, 5, 3}},
        {{22,  7}, {0, 1, 3}},
        {{85, 12}, {0, 1, 10}},
        {{21,  7}, {0, 2, 2}},
        {{20,  7}, {0, 7, 1}},
        {{28,  8}, {0, 1, 4}},
        {{27,  8}, {0, 3, 2}},
        {{33,  9}, {0, 0, 16}},
        {{32,  9}, {0, 1, 5}},
        {{31,  9}, {0, 1, 6}},
        {{30,  9}, {0, 2, 3}},
        {{29,  9}, {0, 3, 3}},
        {{28,  9}, {0, 5, 2}},
        {{27,  9}, {0, 6, 2}},
        {{26,  9}, {0, 7, 2}},
        {{34, 11}, {0, 1, 8}},
        {{35, 11}, {0, 9, 2}},
        {{86, 12}, {0, 2, 5}},
        {{87, 12}, {0, 7, 3}},
        {{ 7,  4}, {1, 0, 1}},
        {{25,  9}, {0, 11, 1}},
        {{ 5, 11}, {1, 0, 6}},
        {{15,  6}, {1, 1, 1}},
        {{ 4, 11}, {1, 0, 7}},
        {{14,  6}, {1, 2, 1}},
        {{13,  6}, {0, 5, 1}},
        {{12,  6}, {1, 0, 2}},
        {{19,  7}, {1, 5, 1}},
        {{18,  7}, {0, 6, 1}},
        {{17,  7}, {1, 3, 1}},
        {{16,  7}, {1, 4, 1}},
        {{26,  8}, {1, 9, 1}},
        {{25,  8}, {0, 8, 1}},
        {{24,  8}, {0, 9, 1}},
        {{23,  8}, {0, 10, 1}},
        {{22,  8}, {1, 0, 3}},
        {{21,  8}, {1, 6, 1}},
        {{20,  8}, {1, 7, 1}},
        {{19,  8}, {1, 8, 1}},
        {{24,  9}, {0, 12, 1}},
        {{23,  9}, {1, 0, 4}},
        {{22,  9}, {1, 1, 2}},
        {{21,  9}, {1, 10, 1}},
        {{20,  9}, {1, 11, 1}},
        {{19,  9}, {1, 12, 1}},
        {{18,  9}, {1, 13, 1}},
        {{17,  9}, {1, 14, 1}},
        {{ 7, 10}, {0, 13, 1}},
        {{ 6, 10}, {1, 0, 5}},
        {{ 5, 10}, {1, 1, 3}},
        {{ 4, 10}, {1, 2, 2}},
        {{36, 11}, {1, 3, 2}},
        {{37, 11}, {1, 4, 2}},
        {{38, 11}, {1, 15, 1}},
        {{39, 11}, {1, 16, 1}},
        {{88, 12}, {0, 14, 1}},
        {{89, 12}, {1, 0, 8}},
        {{90, 12}, {1, 5, 2}},
        {{91, 12}, {1, 6, 2}},
        {{92, 12}, {1, 17, 1}},
        {{93, 12}, {1, 18, 1}},
        {{94, 12}, {1, 19, 1}},
        {{95, 12}, {1, 20, 1}}
    }
};

/* constants taken from momusys/vm_common/inlcude/max_level.h */
uint8_t const max_level[2][2][64] =
{
    {
        /* intra = 0, last = 0 */
        {
            12, 6, 4, 3, 3, 3, 3, 2,
            2, 2, 2, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
        },
        /* intra = 0, last = 1 */
        {
            3, 2, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
        }
    },
    {
        /* intra = 1, last = 0 */
        {
            27, 10, 5, 4, 3, 3, 3, 3,
            2, 2, 1, 1, 1, 1, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
        },
        /* intra = 1, last = 1 */
        {
            8, 3, 2, 2, 2, 2, 2, 1,
            1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0
        }
    }
};

uint8_t const max_run[2][2][64] =
{
    {
        /* intra = 0, last = 0 */
        {
            0, 26, 10, 6, 2, 1, 1, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        /* intra = 0, last = 1 */
        {
            0, 40, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        }
    },
    {
        /* intra = 1, last = 0 */
        {
            0, 14, 9, 7, 3, 2, 1, 1,
            1, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
        /* intra = 1, last = 1 */
        {
            0, 20, 6, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        }
    }
};

//#endif
/******************************************************************
 * decoder tables                                                 *
 ******************************************************************/

VLC const mcbpc_intra_table[64] =
{
    {-1, 0}, {20, 6}, {36, 6}, {52, 6}, {4, 4},  {4, 4},  {4, 4},  {4, 4},
    {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3}, {19, 3},
    {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3}, {35, 3},
    {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3}, {51, 3},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},
    {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1},  {3, 1}
};

VLC const mcbpc_inter_table[257] =
{
    {VLC_ERROR, 0}, {255, 9}, {52, 9}, {36, 9}, {20, 9}, {49, 9}, {35, 8}, {35, 8},
    {19, 8}, {19, 8}, {50, 8}, {50, 8}, {51, 7}, {51, 7}, {51, 7}, {51, 7},
    {34, 7}, {34, 7}, {34, 7}, {34, 7}, {18, 7}, {18, 7}, {18, 7}, {18, 7},
    {33, 7}, {33, 7}, {33, 7}, {33, 7}, {17, 7}, {17, 7}, {17, 7}, {17, 7},
    {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6}, {48, 6},
    {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5},
    {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5}, {3, 5},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4}, {32, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4}, {16, 4},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3}, {2, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3}, {1, 3},
    {0, 1}
};

VLC const cbpy_table[64] =
{
    {-1, 0}, {-1, 0}, {6, 6},  {9, 6},  {8, 5},  {8, 5},  {4, 5},  {4, 5},
    {2, 5},  {2, 5},  {1, 5},  {1, 5},  {0, 4},  {0, 4},  {0, 4},  {0, 4},
    {12, 4}, {12, 4}, {12, 4}, {12, 4}, {10, 4}, {10, 4}, {10, 4}, {10, 4},
    {14, 4}, {14, 4}, {14, 4}, {14, 4}, {5, 4},  {5, 4},  {5, 4},  {5, 4},
    {13, 4}, {13, 4}, {13, 4}, {13, 4}, {3, 4},  {3, 4},  {3, 4},  {3, 4},
    {11, 4}, {11, 4}, {11, 4}, {11, 4}, {7, 4},  {7, 4},  {7, 4},  {7, 4},
    {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2},
    {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}, {15, 2}
};

VLC const TMNMVtab0[] =
{
    {3, 4}, {-3, 4}, {2, 3}, {2, 3}, {-2, 3}, {-2, 3}, {1, 2},
    {1, 2}, {1, 2}, {1, 2}, {-1, 2}, {-1, 2}, {-1, 2}, {-1, 2}
};

VLC const TMNMVtab1[] =
{
    {12, 10}, {-12, 10}, {11, 10}, {-11, 10},
    {10, 9}, {10, 9}, {-10, 9}, {-10, 9},
    {9, 9}, {9, 9}, {-9, 9}, {-9, 9},
    {8, 9}, {8, 9}, {-8, 9}, {-8, 9},
    {7, 7}, {7, 7}, {7, 7}, {7, 7},
    {7, 7}, {7, 7}, {7, 7}, {7, 7},
    {-7, 7}, {-7, 7}, {-7, 7}, {-7, 7},
    {-7, 7}, {-7, 7}, {-7, 7}, {-7, 7},
    {6, 7}, {6, 7}, {6, 7}, {6, 7},
    {6, 7}, {6, 7}, {6, 7}, {6, 7},
    {-6, 7}, {-6, 7}, {-6, 7}, {-6, 7},
    {-6, 7}, {-6, 7}, {-6, 7}, {-6, 7},
    {5, 7}, {5, 7}, {5, 7}, {5, 7},
    {5, 7}, {5, 7}, {5, 7}, {5, 7},
    {-5, 7}, {-5, 7}, {-5, 7}, {-5, 7},
    {-5, 7}, {-5, 7}, {-5, 7}, {-5, 7},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {4, 6}, {4, 6}, {4, 6}, {4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6},
    {-4, 6}, {-4, 6}, {-4, 6}, {-4, 6}
};

VLC const TMNMVtab2[] =
{
    {32, 12}, {-32, 12}, {31, 12}, {-31, 12},
    {30, 11}, {30, 11}, {-30, 11}, {-30, 11},
    {29, 11}, {29, 11}, {-29, 11}, {-29, 11},
    {28, 11}, {28, 11}, {-28, 11}, {-28, 11},
    {27, 11}, {27, 11}, {-27, 11}, {-27, 11},
    {26, 11}, {26, 11}, {-26, 11}, {-26, 11},
    {25, 11}, {25, 11}, {-25, 11}, {-25, 11},
    {24, 10}, {24, 10}, {24, 10}, {24, 10},
    {-24, 10}, {-24, 10}, {-24, 10}, {-24, 10},
    {23, 10}, {23, 10}, {23, 10}, {23, 10},
    {-23, 10}, {-23, 10}, {-23, 10}, {-23, 10},
    {22, 10}, {22, 10}, {22, 10}, {22, 10},
    {-22, 10}, {-22, 10}, {-22, 10}, {-22, 10},
    {21, 10}, {21, 10}, {21, 10}, {21, 10},
    {-21, 10}, {-21, 10}, {-21, 10}, {-21, 10},
    {20, 10}, {20, 10}, {20, 10}, {20, 10},
    {-20, 10}, {-20, 10}, {-20, 10}, {-20, 10},
    {19, 10}, {19, 10}, {19, 10}, {19, 10},
    {-19, 10}, {-19, 10}, {-19, 10}, {-19, 10},
    {18, 10}, {18, 10}, {18, 10}, {18, 10},
    {-18, 10}, {-18, 10}, {-18, 10}, {-18, 10},
    {17, 10}, {17, 10}, {17, 10}, {17, 10},
    {-17, 10}, {-17, 10}, {-17, 10}, {-17, 10},
    {16, 10}, {16, 10}, {16, 10}, {16, 10},
    {-16, 10}, {-16, 10}, {-16, 10}, {-16, 10},
    {15, 10}, {15, 10}, {15, 10}, {15, 10},
    {-15, 10}, {-15, 10}, {-15, 10}, {-15, 10},
    {14, 10}, {14, 10}, {14, 10}, {14, 10},
    {-14, 10}, {-14, 10}, {-14, 10}, {-14, 10},
    {13, 10}, {13, 10}, {13, 10}, {13, 10},
    {-13, 10}, {-13, 10}, {-13, 10}, {-13, 10}
};

VLC const dc_lum_tab[] =
{
    {0, 0}, {4, 3}, {3, 3}, {0, 3},
    {2, 2}, {2, 2}, {1, 2}, {1, 2},
};

#endif


