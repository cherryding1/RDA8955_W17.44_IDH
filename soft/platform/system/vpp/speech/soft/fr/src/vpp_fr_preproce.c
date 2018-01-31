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







#include        <stdio.h>
#include        <assert.h>

#include "vpp_fr_private.h"

#include        "vpp_fr_gsm.h"
#include        "vpp_fr_proto.h"

#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"

/*      4.2.0 .. 4.2.3  PREPROCESSING SECTION
 *
 *      After A-law to linear conversion (or directly from the
 *      Ato D converter) the following scaling is assumed for
 *      input to the RPE-LTP algorithm:
 *
 *      in:  0.1.....................12
 *           S.v.v.v.v.v.v.v.v.v.v.v.v.*.*.*
 *
 *      Where S is the sign bit, v a valid bit, and * a "don't care" bit.
 *      The original signal is called sop[..]
 *
 *      out:   0.1................... 12
 *           S.S.v.v.v.v.v.v.v.v.v.v.v.v.0.0
 */
INT16 sof[160]; //fts add

void Gsm_Preprocess P3((S, s, so),
                       struct gsm_state * S,
                       INT16            * s,
                       INT16            * so )         /* [0..159]     IN/OUT  */
{


    INT16      z1 = S->z1;
    INT32 L_z2 = S->L_z2;
    INT16      mp = (INT16) (S->mp);

    INT16           s1;
    INT32     L_s2;

    INT32     L_temp;

    INT16           msp, lsp;//, temp;
    INT16           SO;

    INT32       ltmp;           /* for   ADD */
    UINT32      utmp;           /* for L_ADD */

    register int            k = 160;

    INT16 * sof_p; //fts

    VPP_FR_PROFILE_FUNCTION_ENTER(GSM_Preprocess);

    sof_p = &sof[0];

    while (k--)
    {

        /*  4.2.1   Downscaling of the input signal
         */
        SO = SASR( *s, 3 ) << 2;
        s++;

        //assert (SO >= -0x4000); /* downscaled by   */
        //assert (SO <=  0x3FFC); /* previous routine. */


        /*  4.2.2   Offset compensation
         *
         *  This part implements a high-pass filter and requires extended
         *  arithmetic precision for the recursive part of this filter.
         *  The input of this procedure is the array so[0...159] and the
         *  output the array sof[ 0...159 ].
         */
        /*   Compute the non-recursive part
         */

        s1 = SO - z1;                   /* s1 = gsm_sub( *so, z1 ); */
        z1 = SO;

        //assert(s1 != MIN_WORD);

        /*   Compute the recursive part
         */
        L_s2 = s1;
        L_s2 <<= 15;

        /*   Execution of a 31 bv 16 bits multiplication
         */

        msp = (INT16) (SASR( L_z2, 15 ));
        lsp = (INT16) (L_z2-((INT32)msp<<15)); /* gsm_L_sub(L_z2,(msp<<15)); */

        L_s2  += GSM_MULT_R( lsp, 32735 );
        L_temp = (INT32)msp * 32735; /* GSM_L_MULT(msp,32735) >> 1;*/
        L_z2   = GSM_L_ADD( L_temp, L_s2 );

        /*    Compute sof[k] with rounding
         */
        L_temp = GSM_L_ADD( L_z2, 16384 );
        *sof_p++ = (INT16) (SASR( L_temp, 15 )); //fts add

        /*   4.2.3  Preemphasis
         */

        msp   = (INT16) (GSM_MULT_R( mp, -28180 ));
        mp    = (INT16) (SASR( L_temp, 15 ));
        *so++ = (INT16) (GSM_SATADD( mp, msp ));
    }

    S->z1   = z1;
    S->L_z2 = L_z2;
    S->mp   = mp;

    VPP_FR_PROFILE_FUNCTION_EXIT(GSM_Preprocess);
}
