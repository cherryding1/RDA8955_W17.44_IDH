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






#include "mpeg4_global.h"
#include "quant.h"

/*****************************************************************************
 * Global function pointers
 ****************************************************************************/

/* DeQuant */
quant_intraFuncPtr dequant_h263_intra;
quant_interFuncPtr dequant_h263_inter;

/*****************************************************************************
 * Function definitions
 ****************************************************************************/

/*  dequantize intra-block & clamp to [-2048,2047]
 */

uint32_t
dequant_h263_intra_c(int16_t * data,
                     const int16_t * coeff,
                     const uint16_t quant,
                     const uint8_t dcscalar)
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

