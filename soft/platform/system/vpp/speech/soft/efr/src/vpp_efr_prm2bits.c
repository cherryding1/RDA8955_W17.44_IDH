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





#include "cs_types.h"
#include "vpp_efr_basic_op.h"

#include "vpp_efr_basic_macro.h"
#include "vpp_efr_codec.h"

/* Local function */

#define MASK      0x0001

void Int2bin (
    INT16 value,       /* input : value to be converted to binary      */
    INT16 no_of_bits,  /* input : number of bits associated with value */
    INT16 *bitstream   /* output: address where bits are written       */
);


void Prm2bits_12k2 (
    INT16 prm[],       /* input : analysis parameters  (57 parameters)   */
    INT16 bits[]       /* output: 244 serial bits                        */
)
{
    INT16 i;


    for (i = 0; i < PRM_NO; i++)
    {
        Int2bin (prm[i], bitno[i], bits);
        bits += bitno[i];
    }

    return;
}

/*************************************************************************
 *
 *  FUNCTION:  Int2bin
 *
 *  PURPOSE:  convert integer to binary and write the bits to the array
 *            bitstream[]. The most significant bits are written first.
 *
 *************************************************************************/

void Int2bin (
    INT16 value,       /* input : value to be converted to binary      */
    INT16 no_of_bits,  /* input : number of bits associated with value */
    INT16 *bitstream   /* output: address where bits are written       */
)
{
    INT16 *pt_bitstream, i, bit;

    pt_bitstream = &bitstream[no_of_bits];

    for (i = 0; i < no_of_bits; i++)
    {
        bit = value & MASK;

        if (bit == 0)
        {
            *--pt_bitstream = BIT_0;
        }
        else
        {
            *--pt_bitstream = BIT_1;
        }
        //value = shr (value, 1);
        value = SHR_D(value, 1);
    }
}
