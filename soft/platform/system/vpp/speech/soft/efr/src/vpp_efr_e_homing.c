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
#include "vpp_efr_vad.h"
#include "vpp_efr_dtx.h"
#include "vpp_efr_codec.h"
#include "vpp_efr_sig_proc.h"
#include "vpp_efr_e_homing.h"

/***************************************************************************
 *
 *   FUNCTION NAME:  encoder_homing_frame_test
 *
 *   PURPOSE:
 *      Checks if a frame of input samples matches the Encoder Homing Frame
 *      pattern, which is 0x0008 for all 160 samples in the frame.
 *
 *   INPUT:
 *      input_frame[]    one frame of speech samples
 *
 *   OUTPUT:
 *      None
 *
 *   RETURN:
 *      0       input frame does not match the Encoder Homing Frame pattern.
 *      1       input frame matches the Encoder Homing Frame pattern.
 *
 **************************************************************************/

INT16 encoder_homing_frame_test (INT16 input_frame[])
{
    INT16 i, j;

    for (i = 0; i < L_FRAME; i++)
    {
        j = input_frame[i] ^ EHF_MASK;

        if (j)
            break;
    }

    return !j;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  encoder_reset
 *
 *   PURPOSE:
 *      resets all of the state variables for the encoder
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

void encoder_reset (void)
{
    /* External declarations for encoder variables which need to be reset */

    /* Variables defined in levinson.c */
    /* ------------------------------- */
    extern INT16 old_A[M + 1]; /* Last A(z) for case of unstable filter */

    /* Variables defined in q_gains.c */
    /* ------------------------------- */
    /* Memories of gain quantization: */
    extern INT16 past_qua_en[4], pred[4];

    /* Variables defined in q_plsf_5.c */
    /* ------------------------------- */
    /* Past quantized prediction error */
    extern INT16 past_r2_q[M];

    INT16 i;

    /* reset all the encoder state variables */
    /* ------------------------------------- */

    /* Variables in cod_12k2.c: */
    Init_Coder_12k2 ();

    /* Variables in levinson.c: */
    old_A[0] = 4096;            /* Last A(z) for case of unstable filter */
    for (i = 1; i < M + 1; i++)
    {
        old_A[i] = 0;
    }

    /* Variables in pre_proc.c: */
    Init_Pre_Process ();

    /* Variables in q_gains.c: */
    for (i = 0; i < 4; i++)
    {
        past_qua_en[i] = -2381; /* past quantized energies */
    }

    pred[0] = 44;               /* MA prediction coeff */
    pred[1] = 37;               /* MA prediction coeff */
    pred[2] = 22;               /* MA prediction coeff */
    pred[3] = 12;               /* MA prediction coeff */

    /* Variables in q_plsf_5.c: */
    for (i = 0; i < M; i++)
    {
        past_r2_q[i] = 0;       /* Past quantized prediction error */
    }

    return;
}

/***************************************************************************
 *
 *   FUNCTION NAME:  reset_enc
 *
 *   PURPOSE:
 *      resets all of the state variables for the encoder and VAD, and for
 *      the transmit DTX and Comfort Noise.
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

void reset_enc (void)
{
    encoder_reset (); /* reset all the state variables in the speech encoder*/
    vad_reset ();     /* reset all the VAD state variables */
    reset_tx_dtx ();  /* reset all the transmit DTX and CN variables */

    return;
}
