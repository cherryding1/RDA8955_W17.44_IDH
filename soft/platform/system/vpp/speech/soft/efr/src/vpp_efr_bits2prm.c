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

#include "cs_types.h"

#include "vpp_efr_cnst.h"
#include "vpp_efr_codec.h"

//For profiling
#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

/* Local function */



void Bits2prm_12k2 (
    INT16 bits[],      /* input : serial bits (244 + bfi)                */
    INT16 prm[]        /* output: analysis parameters  (57+1 parameters) */
)
{
    INT16 i;
    INT16 j=1, k=0;

    VPP_EFR_PROFILE_FUNCTION_ENTER(Bits2prm_12k2);

    for (i=1; i<VPP_EFR_COD_FRAME_LEN+1; i++)
    {
        if(k < bitno [j-1])
        {
            prm[j] =  (prm[j]<<1) | ((bits [i/16]>> (i%16))& 1);
            k++;
            if(k==bitno [j-1])
            {
                j++;
                k=0;
            }
        }


    }
    VPP_EFR_PROFILE_FUNCTION_EXIT(Bits2prm_12k2);
    return;
}

/*************************************************************************
 *
 *  FUNCTION:  Bin2int
 *
 *  PURPOSE: Read "no_of_bits" bits from the array bitstream[] and convert
 *           to integer.
 *
 *************************************************************************/

/*INT16 Bin2int (        // Reconstructed parameter
   INT16 no_of_bits,  // input : number of bits associated with value
   INT16 *bitstream   // output: address where bits are written
)
{
   INT16 value, i, bit;

   value = 0;
   for (i = 0; i < no_of_bits; i++)
   {
       //value = shl (value, 1);
       value = SHL(value, 1);
       bit = *bitstream++;
       //if (sub (bit, BIT_1) == 0)
       if (SUB (bit, BIT_1) == 0)
   {
           //value = add (value, 1);
       value = ADD (value, 1);
   }
   }
   return (value);
}*/





















