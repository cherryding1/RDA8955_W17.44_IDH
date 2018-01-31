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

#include "mpeg4_global.h"
#include "quant.h"

/*****************************************************************************
 * Global function pointers
 ****************************************************************************/

/* DeQuant */
quant_intraFuncPtr dequant_h263_intra;
quant_interFuncPtr dequant_h263_inter;
quant_intraFuncPtr dequant_mpeg_intra;

/*****************************************************************************
 * Function definitions
 ****************************************************************************/

/*  dequantize intra-block & clamp to [-2048,2047]
 */
#if 1
/*****************************************************************************
 * Default matrices
 ****************************************************************************/
extern const uint16 scan_tables[3][64];
#define FIX(X)   (((X)==1) ? 0xFFFF : ((1UL << 16) / (X) + 1))
#define FIXL(X)    ((1UL << 16) / (X) - 1)

static const uint8_t default_intra_matrix[64] =
{
    8, 17, 18, 19, 21, 23, 25, 27,
    17, 18, 19, 21, 23, 25, 27, 28,
    20, 21, 22, 23, 24, 26, 28, 30,
    21, 22, 23, 24, 26, 28, 30, 32,
    22, 23, 24, 26, 28, 30, 32, 35,
    23, 24, 26, 28, 30, 32, 35, 38,
    25, 26, 28, 30, 32, 35, 38, 41,
    27, 28, 30, 32, 35, 38, 41, 45
};

static const uint8_t default_inter_matrix[64] =
{
    16, 17, 18, 19, 20, 21, 22, 23,
    17, 18, 19, 20, 21, 22, 23, 24,
    18, 19, 20, 21, 22, 23, 24, 25,
    19, 20, 21, 22, 23, 24, 26, 27,
    20, 21, 22, 23, 25, 26, 27, 28,
    21, 22, 23, 24, 26, 27, 28, 30,
    22, 23, 24, 26, 27, 28, 30, 31,
    23, 24, 25, 27, 28, 30, 31, 33
};
const uint16_t *
get_intra_matrix(const uint16_t * mpeg_quant_matrices)
{
    return(mpeg_quant_matrices + 0*64);
}

void
set_intra_matrix(uint16_t * mpeg_quant_matrices, const uint8_t * matrix)
{
    int i;
    uint16_t *intra_matrix = mpeg_quant_matrices + 0*64;

    for (i = 0; i < 64; i++)
    {
        intra_matrix[i] = (!i) ? (uint16_t)8: (uint16_t)matrix[i];
    }
}

void
set_inter_matrix(uint16_t * mpeg_quant_matrices, const uint8_t * matrix)
{
    int i;
    uint16_t *inter_matrix = mpeg_quant_matrices + 4*64;
    uint16_t *inter_matrix1 = mpeg_quant_matrices + 5*64;
    uint16_t *inter_matrix_fix = mpeg_quant_matrices + 6*64;
    uint16_t *inter_matrix_fixl = mpeg_quant_matrices + 7*64;

    for (i = 0; i < 64; i++)
    {
        inter_matrix1[i] = ((inter_matrix[i] = (int16_t) matrix[i])>>1);
        inter_matrix1[i] += ((inter_matrix[i] == 1) ? 1: 0);
        inter_matrix_fix[i] = (uint16_t) FIX(inter_matrix[i]);
        inter_matrix_fixl[i] = (uint16_t) FIXL(inter_matrix[i]);
    }
}

const uint8_t *
get_default_intra_matrix(void)
{
    return default_intra_matrix;
}

const uint8_t *
get_default_inter_matrix(void)
{
    return default_inter_matrix;
}
#endif

uint32_t
dequant_h263_intra_c(int16_t * data,
                     const int16_t * coeff,
                     const uint16_t quant,
                     const uint8_t dcscalar,
                     const uint16_t * mpeg_quant_matrices)
{
    const int32_t quant_m_2 = quant << 1;
    const int32_t quant_add = (quant & 1 ? quant : quant - 1);
    int i;

    data[0] = coeff[0] * dcscalar;
//  data[0] = data[0] + (((data[0]+2048) >> 15)&(-2048 - data[0]));
//  data[0] = 2047 + (((data[0]-2047) >> 15)&(data[0] - 2047));
    if (data[0] < -2048)
    {
        data[0] = -2048;
    }
    else if (data[0] > 2047)
    {
        data[0] = 2047;
    }

    for (i = 1; i < 64; i++)
    {
        int32_t acLevel = coeff[i];

        if (acLevel == 0)
        {
            data[i] = 0;
        }
        else if (acLevel < 0)
        {
            acLevel = quant_m_2 * -acLevel + quant_add;
            data[i] = (acLevel <= 2048 ? -acLevel : -2048);
        }
        else
        {
            acLevel = quant_m_2 * acLevel + quant_add;
            data[i] = (acLevel <= 2047 ? acLevel : 2047);
        }
    }

    return(0);
}



/* dequantize inter-block & clamp to [-2048,2047]
 */

uint32_t
dequant_h263_inter_c(int16_t * data,
                     const int16_t * coeff,
                     const uint16_t quant)
{
    const uint16_t quant_m_2 = quant << 1;
    const uint16_t quant_add = (quant & 1 ? quant : quant - 1);
    int i;

    for (i = 0; i < 64; i++)
    {
        int16_t acLevel = coeff[i];

        if (acLevel == 0)
        {
            data[i] = 0;
        }
        else if (acLevel < 0)
        {
            acLevel = acLevel * quant_m_2 - quant_add;
            data[i] = (acLevel >= -2048 ? acLevel : -2048);
        }
        else
        {
            acLevel = acLevel * quant_m_2 + quant_add;
            data[i] = (acLevel <= 2047 ? acLevel : 2047);
        }
    }

    return(0);
}

/* dequantize intra-block & clamp to [-2048,2047]
 *
 * data[i] = (coeff[i] * default_intra_matrix[i] * quant2) >> 4;
 */

uint32_t
dequant_mpeg_intra_c(int16_t * data,
                     const int16_t * coeff,
                     const uint16_t quant,
                     const uint8_t dcscalar,
                     const uint16_t * mpeg_quant_matrices)
{
    const uint16_t *intra_matrix = get_intra_matrix(mpeg_quant_matrices);
    int i;

    data[0] = coeff[0] * dcscalar;
    if (data[0] < -2048)
    {
        data[0] = -2048;
    }
    else if (data[0] > 2047)
    {
        data[0] = 2047;
    }

    for (i = 1; i < 64; i++)
    {
        if (coeff[i] == 0)
        {
            data[i] = 0;
        }
        else if (coeff[i] < 0)
        {
            uint32_t level = -coeff[i];

            level = (level * intra_matrix[i] * quant) >> 3;
            data[i] = (level <= 2048 ? -(int16_t) level : -2048);
        }
        else
        {
            uint32_t level = coeff[i];

            level = (level * intra_matrix[i] * quant) >> 3;
            data[i] = (level <= 2047 ? level : 2047);
        }
    }

    return(0);
}

void
init_mpeg_matrix(uint16_t * mpeg_quant_matrices)
{

    set_intra_matrix(mpeg_quant_matrices, default_intra_matrix);
    set_inter_matrix(mpeg_quant_matrices, default_inter_matrix);//yh
}

#endif

