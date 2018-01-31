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




#include "vpp_efr_cnst.h"
#include "vpp_efr_d_homing.h"
#include "vpp_efr_basic_macro.h"
#include "vpp_efr_codec.h"

#include "hal_speech.h"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

#define TO_FIRST_SUBFRAME 18

INT16 synth_buf[L_FRAME + M]= {0};
INT16 gdec_reset_flag = 1;

void vpp_efr_decode (HAL_SPEECH_DEC_IN_T DecIn, HAL_SPEECH_PCM_HALF_BUF_T DecOut)
{
    INT16 *synth = synth_buf + M;      /* Synthesis                  */
    INT16 parm[PRM_SIZE + 1] = {0};         /* Synthesis parameters       */
    INT16 Az_dec[AZ_SIZE];             /* Decoded Az for post-filter */
    /* in 4 subframes, length= 44 */
    INT16 i;

    INT16 reset_flag;

    VPP_EFR_PROFILE_FUNCTION_ENTER (vpp_efr_decode);

    Bits2prm_12k2(DecIn.decInBuf, parm);

    /* BFI == 0, perform DHF check */
    /* Check for second and further successive DHF (to first subfr.) */
    /* BFI==1, bypass DHF check (frame is taken as not being a DHF) */

    reset_flag = ((!DecIn.bfi)&&(gdec_reset_flag==1))?
                 decoder_homing_frame_test (&parm[1],TO_FIRST_SUBFRAME):0;

    if (reset_flag  && gdec_reset_flag)
    {
        /* Force the output to be the encoder homing frame pattern */

        for (i = 0; i < L_FRAME; i++)
        {
            synth[i] = EHF_MASK;
        }
    }
    else
    {
        /* Synthesis */
        Decoder_12k2 (parm, synth, Az_dec, DecIn.taf, DecIn.sid);

        Post_Filter (synth, Az_dec);                      /* Post-filter */

        for (i = 0; i < L_FRAME; i++)
            /* Upscale the 15 bit linear PCM to 16 bits,
               then truncate to 13 bits */
        {
            DecOut[i]= SHL(synth[i], 1 ) & 0xfff8;
        }

    }
    /* BFI == 0, perform check for first DHF (whole frame) */
    if ((!DecIn.bfi) && (!gdec_reset_flag))
    {
        reset_flag = decoder_homing_frame_test (&parm[1], PRM_SIZE);
    }

    if (reset_flag != 0)
    {
        /* Bring the decoder and receive DTX to the home state */
        reset_dec ();
    }
    gdec_reset_flag = reset_flag;

    VPP_EFR_PROFILE_FUNCTION_EXIT (vpp_efr_decode);
}
