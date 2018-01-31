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
#include "vpp_efr_cnst.h"
#include "vpp_efr_dtx.h"
#include "vpp_efr_codec.h"
#include "vpp_efr_d_homing.h"
#include "vpp_efr_q_plsf_5.tab"

#include "vpp_efr_debug.h"
#include "vpp_efr_profile_codes.h"

#define PRM_NO    57

/***************************************************************************
 *
 *   FUNCTION NAME:  decoder_homing_frame_test
 *
 *   PURPOSE:
 *      Checks if a frame of input speech parameters matches the Decoder
 *      Homing Frame pattern, which is:
 *
 *      parameter    decimal value    hexidecimal value
 *      ---------    -------------    -----------------
 *      LPC 1        4                0x0004
 *      LPC 2        47               0x002F
 *      LPC 3        180              0x00B4
 *      LPC 4        144              0x0090
 *      LPC 5        62               0x003E
 *      LTP-LAG 1    342              0x0156
 *      LTP-GAIN 1   11               0x000B
 *      PULSE1_1     0                0x0000
 *      PULSE1_2     1                0x0001
 *      PULSE1_3     15               0x000F
 *      PULSE1_4     1                0x0001
 *      PULSE1_5     13               0x000D
 *      PULSE1_6     0                0x0000
 *      PULSE1_7     3                0x0003
 *      PULSE1_8     0                0x0000
 *      PULSE1_9     3                0x0003
 *      PULSE1_10    0                0x0000
 *      FCB-GAIN 1   3                0x0003
 *      LTP-LAG 2    54               0x0036
 *      LTP-GAIN 2   1                0x0001
 *      PULSE2_1     8                0x0008
 *      PULSE2_2     8                0x0008
 *      PULSE2_3     5                0x0005
 *      PULSE2_4     8                0x0008
 *      PULSE2_5     1                0x0001
 *      PULSE2_6     0                0x0000
 *      PULSE2_7     0                0x0000
 *      PULSE2_8     1                0x0001
 *      PULSE2_9     1                0x0001
 *      PULSE2_10    0                0x0000
 *      FCB-GAIN 2   0                0x0000
 *      LTP-LAG 3    342              0x0156
 *      LTP-GAIN 3   0                0x0000
 *      PULSE3_1     0                0x0000
 *      PULSE3_2     0                0x0000
 *      PULSE3_3     0                0x0000
 *      PULSE3_4     0                0x0000
 *      PULSE3_5     0                0x0000
 *      PULSE3_6     0                0x0000
 *      PULSE3_7     0                0x0000
 *      PULSE3_8     0                0x0000
 *      PULSE3_9     0                0x0000
 *      PULSE3_10    0                0x0000
 *      FCB-GAIN 3   0                0x0000
 *      LTP-LAG 4    54               0x0036
 *      LTP-GAIN 4   11               0x000B
 *      PULSE4_1     0                0x0000
 *      PULSE4_2     0                0x0000
 *      PULSE4_3     0                0x0000
 *      PULSE4_4     0                0x0000
 *      PULSE4_5     0                0x0000
 *      PULSE4_6     0                0x0000
 *      PULSE4_7     0                0x0000
 *      PULSE4_8     0                0x0000
 *      PULSE4_9     0                0x0000
 *      PULSE4_10    0                0x0000
 *      FCB-GAIN 4   0                0x0000
 *
 *   INPUT:
 *      parm[]  one frame of speech parameters in parallel format
 *
 *      nbr_of_params
 *              the number of consecutive parameters in parm[] to match
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      0       input frame does not match the Decoder Homing Frame pattern.
 *      1       input frame matches the Decoder Homing Frame pattern.
 *
 **************************************************************************/

INT16 decoder_homing_frame_test (INT16 parm[], INT16 nbr_of_params)
{
    INT16 i, j=0;

    for (i = 0; i < nbr_of_params; i++)
    {
        j = parm[i] ^ dhf_mask[i];

        if (j)
            break;
    }

    return !j;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  decoder_reset
 *
 *   PURPOSE:
 *      resets all of the state variables for the decoder
 *
 *   INPUT:
 *      None
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      None
 *
 **************************************************************************/

void decoder_reset (void)
{
    /* External declarations for decoder variables which need to be reset */

    /* variable defined in decoder.c */
    /* ----------------------------- */
    // extern INT16 synth_buf[L_FRAME + M];

    /* variable defined in agc.c */
    /* -------------------------- */
    extern INT16 past_gain;

    /* variables defined in d_gains.c */
    /* ------------------------------ */
    /* Error concealment */
    extern INT16 pbuf[5], past_gain_pit, prev_gp, gbuf[5], past_gain_code,
           prev_gc;

    /* CNI */
    extern INT16 gcode0_CN, gain_code_old_CN, gain_code_new_CN;
    extern INT16 gain_code_muting_CN;

    /* Memories of gain dequantization: */
    extern INT16 past_qua_en[4], pred[4];

    /* variables defined in d_plsf_5.c */
    /* ------------------------------ */
    /* Past quantized prediction error */
    extern INT16 past_r2_q[M];

    /* Past dequantized lsfs */
    extern INT16 past_lsf_q[M];

    /* CNI */
    extern INT16 lsf_p_CN[M], lsf_new_CN[M], lsf_old_CN[M];

    /* variables defined in dec_lag6.c */
    /* ------------------------------ */
    extern INT16 old_T0;

    /* variable defined in preemph.c */
    /* ------------------------------ */
    extern INT16 mem_pre;

    INT16 i;

    /* reset all the decoder state variables */
    /* ------------------------------------- */

    /* Variable in decoder.c: */
    //  for (i = 0; i < M; i++)
    // {
    //     synth_buf[i] = 0;
    // }

    /* Variables in dec_12k2.c: */
    Init_Decoder_12k2 ();

    /* Variable in agc.c: */
    past_gain = 4096;

    /* Variables in d_gains.c: */
    for (i = 0; i < 5; i++)
    {
        pbuf[i] = 410;          /* Error concealment */
        gbuf[i] = 1;            /* Error concealment */
    }

    past_gain_pit = 0;          /* Error concealment */
    prev_gp = 4096;             /* Error concealment */
    past_gain_code = 0;         /* Error concealment */
    prev_gc = 1;                /* Error concealment */
    gcode0_CN = 0;              /* CNI */
    gain_code_old_CN = 0;       /* CNI */
    gain_code_new_CN = 0;       /* CNI */
    gain_code_muting_CN = 0;    /* CNI */

    for (i = 0; i < 4; i++)
    {
        past_qua_en[i] = -2381; /* past quantized energies */
    }

    pred[0] = 44;               /* MA prediction coeff */
    pred[1] = 37;               /* MA prediction coeff */
    pred[2] = 22;               /* MA prediction coeff */
    pred[3] = 12;               /* MA prediction coeff */

    /* Variables in d_plsf_5.c: */
    for (i = 0; i < M; i++)
    {
        past_r2_q[i] = 0;               /* Past quantized prediction error */
        past_lsf_q[i] = mean_lsf[i];    /* Past dequantized lsfs */
        lsf_p_CN[i] = mean_lsf[i];      /* CNI */
        lsf_new_CN[i] = mean_lsf[i];    /* CNI */
        lsf_old_CN[i] = mean_lsf[i];    /* CNI */
    }

    /* Variable in dec_lag6.c: */
    old_T0 = 40;                /* Old integer lag */

    /* Variable in preemph.c: */
    mem_pre = 0;                /* Filter memory */

    /* Variables in pstfilt2.c: */
    Init_Post_Filter ();

    return;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  reset_dec
 *
 *   PURPOSE:
 *      resets all of the state variables for the decoder, and for the
 *      receive DTX and Comfort Noise.
 *
 *   INPUT:
 *      None
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      None
 *
 **************************************************************************/

void reset_dec (void)
{
    decoder_reset (); /* reset all the state variables in the speech decoder*/
    reset_rx_dtx ();  /* reset all the receive DTX and CN state variables */

    return;
}
