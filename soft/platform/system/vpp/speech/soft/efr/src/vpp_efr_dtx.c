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
#include "vpp_efr_cnst.h"
#include "vpp_efr_sig_proc.h"
#include "vpp_efr_dtx.h"
#include "vpp_efr_basic_macro.h"

/* Inverse values of DTX hangover period and DTX hangover period + 1 */


/* Index map for encoding and detecting SID codeword */

static const INT16 SID_codeword_bit_idx[95] =
{
    45,  46,  48,  49,  50,  51,  52,  53,  54,  55,
    56,  57,  58,  59,  60,  61,  62,  63,  64,  65,
    66,  67,  68,  94,  95,  96,  98,  99, 100, 101,
    102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 148, 149, 150,
    151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
    161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
    171, 196, 197, 198, 199, 200, 201, 202, 203, 204,
    205, 206, 207, 208, 209, 212, 213, 214, 215, 216,
    217, 218, 219, 220, 221
};

INT16 txdtx_ctrl;              /* Encoder DTX control word                */
INT16 rxdtx_ctrl;              /* Decoder DTX control word                */
INT16 CN_excitation_gain;      /* Unquantized fixed codebook gain         */
INT32 L_pn_seed_tx;            /* PN generator seed (encoder)             */
INT32 L_pn_seed_rx;            /* PN generator seed (decoder)             */
INT16 rx_dtx_state;            /* State of comfort noise insertion period */

static INT16 txdtx_hangover;   /* Length of hangover period (VAD=0, SP=1) */
static INT16 rxdtx_aver_period;/* Length of hangover period (VAD=0, SP=1) */
static INT16 txdtx_N_elapsed;  /* Measured time from previous SID frame   */
static INT16 rxdtx_N_elapsed;  /* Measured time from previous SID frame   */
static INT16 old_CN_mem_tx[6]; /* The most recent CN parameters are stored*/
static INT16 prev_SID_frames_lost; /* Counter for lost SID frames         */
static INT16 buf_p_tx;         /* Circular buffer pointer for gain code
                                   history  update in tx                   */
static INT16 buf_p_rx;         /* Circular buffer pointer for gain code
                                   history update in rx                    */

INT16 lsf_old_tx[DTX_HANGOVER][M]; /* Comfort noise LSF averaging buffer  */
INT16 lsf_old_rx[DTX_HANGOVER][M]; /* Comfort noise LSF averaging buffer  */

INT16 gain_code_old_tx[4 * DTX_HANGOVER]; /* Comfort noise gain averaging
                                              buffer                       */
INT16 gain_code_old_rx[4 * DTX_HANGOVER]; /* Comfort noise gain averaging
                                              buffer                       */

/*************************************************************************
 *
 *   FUNCTION NAME: reset_tx_dtx
 *
 *   PURPOSE:  Resets the static variables of the TX DTX handler to their
 *             initial values
 *
 *************************************************************************/

void __attribute__((section(".sramtext"))) reset_tx_dtx ()
{
    INT16 i;

    /* suppose infinitely long speech period before start */

    txdtx_hangover = DTX_HANGOVER;
    txdtx_N_elapsed = 0x7fff;
    txdtx_ctrl = TX_SP_FLAG | TX_VAD_FLAG;

    for (i = 0; i < 6; i++)
    {
        old_CN_mem_tx[i] = 0;
    }

    for (i = 0; i < DTX_HANGOVER; i++)
    {
        lsf_old_tx[i][0] = 1384;
        lsf_old_tx[i][1] = 2077;
        lsf_old_tx[i][2] = 3420;
        lsf_old_tx[i][3] = 5108;
        lsf_old_tx[i][4] = 6742;
        lsf_old_tx[i][5] = 8122;
        lsf_old_tx[i][6] = 9863;
        lsf_old_tx[i][7] = 11092;
        lsf_old_tx[i][8] = 12714;
        lsf_old_tx[i][9] = 13701;
    }

    for (i = 0; i < 4 * DTX_HANGOVER; i++)
    {
        gain_code_old_tx[i] = 0;
    }

    L_pn_seed_tx = PN_INITIAL_SEED;

    buf_p_tx = 0;
    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: reset_rx_dtx
 *
 *   PURPOSE:  Resets the static variables of the RX DTX handler to their
 *             initial values
 *
 *************************************************************************/

void reset_rx_dtx ()
{
    INT16 i;

    /* suppose infinitely long speech period before start */

    rxdtx_aver_period = DTX_HANGOVER;
    rxdtx_N_elapsed = 0x7fff;
    rxdtx_ctrl = RX_SP_FLAG;

    for (i = 0; i < DTX_HANGOVER; i++)
    {
        lsf_old_rx[i][0] = 1384;
        lsf_old_rx[i][1] = 2077;
        lsf_old_rx[i][2] = 3420;
        lsf_old_rx[i][3] = 5108;
        lsf_old_rx[i][4] = 6742;
        lsf_old_rx[i][5] = 8122;
        lsf_old_rx[i][6] = 9863;
        lsf_old_rx[i][7] = 11092;
        lsf_old_rx[i][8] = 12714;
        lsf_old_rx[i][9] = 13701;
    }

    for (i = 0; i < 4 * DTX_HANGOVER; i++)
    {
        gain_code_old_rx[i] = 0;
    }

    L_pn_seed_rx = PN_INITIAL_SEED;
    rx_dtx_state = CN_INT_PERIOD - 1;

    prev_SID_frames_lost = 0;
    buf_p_rx = 0;

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: tx_dtx
 *
 *   PURPOSE: DTX handler of the speech encoder. Determines when to add
 *            the hangover period to the end of the speech burst, and
 *            also determines when to use old SID parameters, and when
 *            to update the SID parameters. This function also initializes
 *            the pseudo noise generator shift register.
 *
 *            Operation of the TX DTX handler is based on the VAD flag
 *            given as input from the speech encoder.
 *
 *   INPUTS:      VAD_flag      Voice activity decision
 *                *txdtx_ctrl   Old encoder DTX control word
 *
 *   OUTPUTS:     *txdtx_ctrl   Updated encoder DTX control word
 *                L_pn_seed_tx  Initialized pseudo noise generator shift
 *                              register (global variable)
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void tx_dtx (
    INT16 VAD_flag,
    INT16 *txdtx_flag
)
{

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */

    //txdtx_N_elapsed = add (txdtx_N_elapsed, 1);
    txdtx_N_elapsed = ADD_SAT16(txdtx_N_elapsed, 1);

    /* If voice activity was detected, reset hangover counter */


    //if (sub (VAD_flag, 1) == 0)
    if (SUB (VAD_flag, 1) == 0)
    {
        txdtx_hangover = DTX_HANGOVER;
        *txdtx_flag = TX_SP_FLAG | TX_VAD_FLAG;
    }
    else
    {
        if (txdtx_hangover == 0)
        {
            /* Hangover period is over, SID should be updated */

            txdtx_N_elapsed = 0;

            /* Check if this is the first frame after hangover period */
            if ((*txdtx_flag & TX_HANGOVER_ACTIVE) != 0)
            {
                *txdtx_flag = TX_PREV_HANGOVER_ACTIVE
                              | TX_SID_UPDATE;
                L_pn_seed_tx = PN_INITIAL_SEED;
            }
            else
            {
                *txdtx_flag = TX_SID_UPDATE;
            }
        }
        else
        {
            /* Hangover period is not over, update hangover counter */
            //txdtx_hangover = sub (txdtx_hangover, 1);
            txdtx_hangover = SUB (txdtx_hangover, 1);

            /* Check if elapsed time from last SID update is greater than
               threshold. If not, set SP=0 (although hangover period is not
               over) and use old SID parameters for new SID frame.
               N_elapsed counter must be summed with hangover counter in order
               to avoid erroneus SP=1 decision in case when N_elapsed is grown
               bigger than threshold and hangover period is still active */

            //if (sub (add (txdtx_N_elapsed, txdtx_hangover),
            //         DTX_ELAPSED_THRESHOLD) < 0)
            if (SUB (ADD_SAT16(txdtx_N_elapsed, txdtx_hangover), DTX_ELAPSED_THRESHOLD) < 0)
            {
                /* old SID frame should be used */
                *txdtx_flag = TX_USE_OLD_SID;
            }
            else
            {

                if ((*txdtx_flag & TX_HANGOVER_ACTIVE) != 0)
                {
                    *txdtx_flag = TX_PREV_HANGOVER_ACTIVE
                                  | TX_HANGOVER_ACTIVE
                                  | TX_SP_FLAG;
                }
                else
                {
                    *txdtx_flag = TX_HANGOVER_ACTIVE
                                  | TX_SP_FLAG;
                }
            }
        }
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: rx_dtx
 *
 *   PURPOSE: DTX handler of the speech decoder. Determines when to update
 *            the reference comfort noise parameters (LSF and gain) at the
 *            end of the speech burst. Also classifies the incoming frames
 *            according to SID flag and BFI flag
 *            and determines when the transmission is active during comfort
 *            noise insertion. This function also initializes the pseudo
 *            noise generator shift register.
 *
 *            Operation of the RX DTX handler is based on measuring the
 *            lengths of speech bursts and the lengths of the pauses between
 *            speech bursts to determine when there exists a hangover period
 *            at the end of a speech burst. The idea is to keep in sync with
 *            the TX DTX handler to be able to update the reference comfort
 *            noise parameters at the same time instances.
 *
 *   INPUTS:      *rxdtx_ctrl   Old decoder DTX control word
 *                TAF           Time alignment flag
 *                bfi           Bad frame indicator flag
 *                SID_flag      Silence descriptor flag
 *
 *   OUTPUTS:     *rxdtx_ctrl   Updated decoder DTX control word
 *                rx_dtx_state  Updated state of comfort noise interpolation
 *                              period (global variable)
 *                L_pn_seed_rx  Initialized pseudo noise generator shift
 *                              register (global variable)
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void rx_dtx (
    INT16 *rxdtx_flag,
    INT16 TAF,
    INT16 bfi,
    INT16 SID_flag
)
{
    INT16 frame_type;


    /* Frame classification according to bfi-flag and ternary-valued
       SID flag. The frames between SID updates (not actually trans-
       mitted) are also classified here; they will be discarded later
       and provided with "NO TRANSMISSION"-flag */

    // if ((sub (SID_flag, 2) == 0) && (bfi == 0))
    if ((SUB (SID_flag, 2) == 0) && (bfi == 0))
    {
        frame_type = VALID_SID_FRAME;
    }
    else if ((SID_flag == 0) && (bfi == 0))
    {
        frame_type = GOOD_SPEECH_FRAME;
    }
    else if ((SID_flag == 0) && (bfi != 0))
    {
        frame_type = UNUSABLE_FRAME;
    }
    else
    {
        frame_type = INVALID_SID_FRAME;
    }

    /* Update of decoder state */
    /* Previous frame was classified as a speech frame */
    if ((*rxdtx_flag & RX_SP_FLAG) != 0)
    {

        //if (sub (frame_type, VALID_SID_FRAME) == 0)
        if (SUB (frame_type, VALID_SID_FRAME) == 0)
        {
            *rxdtx_flag = RX_FIRST_SID_UPDATE;
        }
        //else if (sub (frame_type, INVALID_SID_FRAME) == 0)
        else if (SUB (frame_type, INVALID_SID_FRAME) == 0)
        {
            *rxdtx_flag = RX_FIRST_SID_UPDATE
                          | RX_INVALID_SID_FRAME;
        }
        //else if (sub (frame_type, UNUSABLE_FRAME) == 0)
        else if (SUB (frame_type, UNUSABLE_FRAME) == 0)
        {
            *rxdtx_flag = RX_SP_FLAG;
        }
        //else if (sub (frame_type, GOOD_SPEECH_FRAME) == 0)
        else if (SUB (frame_type, GOOD_SPEECH_FRAME) == 0)
        {
            *rxdtx_flag = RX_SP_FLAG;
        }
    }
    else
    {

        //if (sub (frame_type, VALID_SID_FRAME) == 0)
        if (SUB (frame_type, VALID_SID_FRAME) == 0)
        {
            *rxdtx_flag = RX_CONT_SID_UPDATE;
        }
        //else if (sub (frame_type, INVALID_SID_FRAME) == 0)
        else if (SUB (frame_type, INVALID_SID_FRAME) == 0)
        {
            *rxdtx_flag = RX_CONT_SID_UPDATE
                          | RX_INVALID_SID_FRAME;
        }
        //else if (sub (frame_type, UNUSABLE_FRAME) == 0)
        else if (SUB (frame_type, UNUSABLE_FRAME) == 0)
        {
            *rxdtx_flag = RX_CNI_BFI;
        }
        //else if (sub (frame_type, GOOD_SPEECH_FRAME) == 0)
        else if (SUB (frame_type, GOOD_SPEECH_FRAME) == 0)
        {
            /* If the previous frame (during CNI period) was muted,
               raise the RX_PREV_DTX_MUTING flag */
            ;
            if ((*rxdtx_flag & RX_DTX_MUTING) != 0)
            {
                *rxdtx_flag = RX_SP_FLAG | RX_FIRST_SP_FLAG
                              | RX_PREV_DTX_MUTING;
            }
            else
            {
                *rxdtx_flag = RX_SP_FLAG | RX_FIRST_SP_FLAG;

            }
        }
    }


    ;
    if ((*rxdtx_flag & RX_SP_FLAG) != 0)
    {
        prev_SID_frames_lost = 0;
        rx_dtx_state = CN_INT_PERIOD - 1;
    }
    else
    {
        /* First SID frame */
        ;
        if ((*rxdtx_flag & RX_FIRST_SID_UPDATE) != 0)
        {
            prev_SID_frames_lost = 0;
            rx_dtx_state = CN_INT_PERIOD - 1;
        }

        /* SID frame detected, but not the first SID */
        if ((*rxdtx_flag & RX_CONT_SID_UPDATE) != 0)
        {
            prev_SID_frames_lost = 0;


            //if (sub (frame_type, VALID_SID_FRAME) == 0)
            if (SUB (frame_type, VALID_SID_FRAME) == 0)
            {
                rx_dtx_state = 0;
            }
            //else if (sub (frame_type, INVALID_SID_FRAME) == 0)
            else if (SUB (frame_type, INVALID_SID_FRAME) == 0)
            {

                //if (sub(rx_dtx_state, (CN_INT_PERIOD - 1)) < 0)
                if (SUB(rx_dtx_state, (CN_INT_PERIOD - 1)) < 0)
                {
                    //rx_dtx_state = add(rx_dtx_state, 1);
                    rx_dtx_state = ADD(rx_dtx_state, 1);

                }
            }
        }

        /* Bad frame received in CNI mode */
        ;
        if ((*rxdtx_flag & RX_CNI_BFI) != 0)
        {

            //if (sub (rx_dtx_state, (CN_INT_PERIOD - 1)) < 0)
            if (SUB (rx_dtx_state, (CN_INT_PERIOD - 1)) < 0)
            {
                //rx_dtx_state = add (rx_dtx_state, 1);
                rx_dtx_state = ADD (rx_dtx_state, 1);

            }

            /* If an unusable frame is received during CNI period
               when TAF == 1, the frame is classified as a lost
               SID frame */

            //if (sub (TAF, 1) == 0)
            if (SUB (TAF, 1) == 0)
            {
                *rxdtx_flag = *rxdtx_flag | RX_LOST_SID_FRAME;

                //prev_SID_frames_lost = add (prev_SID_frames_lost, 1);
                prev_SID_frames_lost = ADD (prev_SID_frames_lost, 1);
            }
            else /* No transmission occurred */
            {
                *rxdtx_flag = *rxdtx_flag | RX_NO_TRANSMISSION;

            }


            //if (sub (prev_SID_frames_lost, 1) > 0)
            if (SUB (prev_SID_frames_lost, 1) > 0)
            {
                *rxdtx_flag = *rxdtx_flag | RX_DTX_MUTING;

            }
        }
    }

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */


    //rxdtx_N_elapsed = add(rxdtx_N_elapsed, 1);
    rxdtx_N_elapsed = ADD_SAT16(rxdtx_N_elapsed,1);

    ;
    if ((*rxdtx_flag & RX_SP_FLAG) != 0)
    {
        rxdtx_aver_period = DTX_HANGOVER;
    }
    else
    {

        //if (sub (rxdtx_N_elapsed, DTX_ELAPSED_THRESHOLD) > 0)
        if (SUB (rxdtx_N_elapsed, DTX_ELAPSED_THRESHOLD) > 0)
        {
            *rxdtx_flag |= RX_UPD_SID_QUANT_MEM;
            rxdtx_N_elapsed = 0;
            rxdtx_aver_period = 0;
            L_pn_seed_rx = PN_INITIAL_SEED;
        }
        else if (rxdtx_aver_period == 0)
        {
            rxdtx_N_elapsed = 0;
        }
        else
        {
            //rxdtx_aver_period = sub (rxdtx_aver_period, 1);
            rxdtx_aver_period = SUB (rxdtx_aver_period, 1);
        }

    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: CN_encoding
 *
 *   PURPOSE:  Encoding of the comfort noise parameters into a SID frame.
 *             Use old SID parameters if necessary. Set the parameter
 *             indices not used by comfort noise parameters to zero.
 *
 *   INPUTS:      params[0..56]  Comfort noise parameter frame from the
 *                               speech encoder
 *                txdtx_ctrl     TX DTX handler control word
 *
 *   OUTPUTS:     params[0..56]  Comfort noise encoded parameter frame
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void CN_encoding (
    INT16 params[],
    INT16 txdtx_flag
)
{
    INT16 i;


    if ((txdtx_flag & TX_SID_UPDATE) != 0)
    {
        /* Store new CN parameters in memory to be used later as old
           CN parameters */

        /* LPC parameter indices */
        for (i = 0; i < 5; i++)
        {
            old_CN_mem_tx[i] = params[i];
        }
        /* Codebook index computed in last subframe */
        old_CN_mem_tx[5] = params[56];
    }

    if ((txdtx_flag & TX_USE_OLD_SID) != 0)
    {
        /* Use old CN parameters previously stored in memory */
        for (i = 0; i < 5; i++)
        {
            params[i] = old_CN_mem_tx[i];
        }
        params[17] = old_CN_mem_tx[5];
        params[30] = old_CN_mem_tx[5];
        params[43] = old_CN_mem_tx[5];
        params[56] = old_CN_mem_tx[5];
    }
    /* Set all the rest of the parameters to zero (SID codeword will
       be written later) */
    for (i = 0; i < 12; i++)
    {
        params[i + 5] = 0;
        params[i + 18] = 0;
        params[i + 31] = 0;
        params[i + 44] = 0;
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: sid_codeword_encoding
 *
 *   PURPOSE:  Encoding of the SID codeword into the SID frame. The SID
 *             codeword consists of 95 bits, all set to '1'.
 *
 *   INPUTS:      ser2[0..243]  Serial-mode speech parameter frame before
 *                              writing SID codeword into it
 *
 *   OUTPUTS:     ser2[0..243]  Serial-mode speech parameter frame with
 *                              SID codeword written into it
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void sid_codeword_encoding (
    INT16 ser2[]
)
{
    INT16 i;

    for (i = 0; i < 95; i++)
    {
        ser2[SID_codeword_bit_idx[i]] = 1;
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: sid_frame_detection
 *
 *   PURPOSE:  Detecting of SID codeword from a received frame. The frames
 *             are classified into three categories based on how many bit
 *             errors occur in the SID codeword:
 *                 - VALID SID FRAME
 *                 - INVALID SID FRAME
 *                 - SPEECH FRAME
 *
 *   INPUTS:      ser2[0..243]  Received serial-mode speech parameter frame
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Ternary-valued SID classification flag
 *
 *************************************************************************/

INT16 sid_frame_detection (
    INT16 ser2[]
)
{
    INT16 i, nbr_errors, sid;

    /* Search for bit errors in SID codeword */
    nbr_errors = 0;
    for (i = 0; i < 95; i++)
    {

        if (ser2[SID_codeword_bit_idx[i]] == 0)
        {
            //nbr_errors = add (nbr_errors, 1);
            nbr_errors = ADD (nbr_errors, 1);
        }
    }

    /* Frame classification */

    //if (sub (nbr_errors, VALID_SID_THRESH) < 0)
    if (SUB (nbr_errors, VALID_SID_THRESH) < 0)
    {
        /* Valid SID frame */
        sid = 2;
    }
    if (SUB (nbr_errors, INVALID_SID_THRESH) < 0)
    {
        /* Invalid SID frame */
        sid = 1;
    }
    else
    {
        /* Speech frame */
        sid = 0;
    }

    return sid;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_lsf_history
 *
 *   PURPOSE: Update the LSF parameter history. The LSF parameters kept
 *            in the buffer are used later for computing the reference
 *            LSF parameter vector and the averaged LSF parameter vector.
 *
 *   INPUTS:      lsf1[0..9]    LSF vector of the 1st half of the frame
 *                lsf2[0..9]    LSF vector of the 2nd half of the frame
 *                lsf_old[0..DTX_HANGOVER-1][0..M-1]
 *                              Old LSF history
 *
 *   OUTPUTS:     lsf_old[0..DTX_HANGOVER-1][0..M-1]
 *                              Updated LSF history
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void update_lsf_history (
    INT16 lsf1[M],
    INT16 lsf2[M],
    INT16 lsf_old[DTX_HANGOVER][M]
)
{
    INT16 i, j, temp;

    /* shift LSF data to make room for LSFs from current frame */
    /* This can also be implemented by using circular buffering */

    for (i = DTX_HANGOVER - 1; i > 0; i--)
    {
        for (j = 0; j < M; j++)
        {
            lsf_old[i][j] = lsf_old[i - 1][j];
        }
    }

    /* Store new LSF data to lsf_old buffer */

    for (i = 0; i < M; i++)
    {
        //temp = add (shr (lsf1[i], 1), shr (lsf2[i], 1));
        temp = ADD (SHR_D(lsf1[i], 1), SHR_D(lsf2[i], 1));

        lsf_old[0][i] = temp;
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_lsf_p_CN
 *
 *   PURPOSE: Update the reference LSF parameter vector. The reference
 *            vector is computed by averaging the quantized LSF parameter
 *            vectors which exist in the LSF parameter history.
 *
 *   INPUTS:      lsf_old[0..DTX_HANGOVER-1][0..M-1]
 *                                 LSF parameter history
 *
 *   OUTPUTS:     lsf_p_CN[0..9]   Computed reference LSF parameter vector
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void update_lsf_p_CN (
    INT16 lsf_old[DTX_HANGOVER][M],
    INT16 lsf_p_CN[M]
)
{
    INT16 i, j;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;


    for (j = 0; j < M; j++)
    {
        //L_temp = L_mult (INV_DTX_HANGOVER, lsf_old[0][j]);
        //L_temp = L_MULT(INV_DTX_HANGOVER, lsf_old[0][j]);
        VPP_MLX16 (tmp_hi, tmp_lo, INV_DTX_HANGOVER, lsf_old[0][j]);

        for (i = 1; i < DTX_HANGOVER; i++)
        {
            //L_temp = L_mac (L_temp, INV_DTX_HANGOVER, lsf_old[i][j]);
            //L_temp = L_MAC(L_temp, INV_DTX_HANGOVER, lsf_old[i][j]);
            VPP_MLA16(tmp_hi, tmp_lo, INV_DTX_HANGOVER, lsf_old[i][j]);

        }
        //lsf_p_CN[j] = round(L_temp);
        lsf_p_CN[j] = ROUND(VPP_SCALE64_TO_16(tmp_hi, tmp_lo));

    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: aver_lsf_history
 *
 *   PURPOSE: Compute the averaged LSF parameter vector. Computation is
 *            performed by averaging the LSF parameter vectors which exist
 *            in the LSF parameter history, together with the LSF
 *            parameter vectors of the current frame.
 *
 *   INPUTS:      lsf_old[0..DTX_HANGOVER-1][0..M-1]
 *                                   LSF parameter history
 *                lsf1[0..M-1]       LSF vector of the 1st half of the frame
 *                lsf2[0..M-1]       LSF vector of the 2nd half of the frame
 *
 *   OUTPUTS:     lsf_aver[0..M-1]   Averaged LSF parameter vector
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void aver_lsf_history (
    INT16 lsf_old[DTX_HANGOVER][M],
    INT16 lsf1[M],
    INT16 lsf2[M],
    INT16 lsf_aver[M]
)
{
    INT16 i, j;
    //INT32 L_temp;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;

    for (j = 0; j < M; j++)
    {
        //L_temp = L_mult (0x3fff, lsf1[j]);
        //L_temp = L_MULT(0x3fff, lsf1[j]);
        VPP_MLX16(tmp_hi, tmp_lo, 0x3fff, lsf1[j]);
        //L_temp = L_mac (L_temp, 0x3fff, lsf2[j]);
        //L_temp = L_MAC(L_temp, 0x3fff, lsf2[j]);
        VPP_MLA16(tmp_hi, tmp_lo, 0x3fff, lsf2[j]);
        //L_temp = L_mult (INV_DTX_HANGOVER_P1, extract_h (L_temp));
        //L_temp = L_MULT(INV_DTX_HANGOVER_P1, EXTRACT_H(L_temp));
        VPP_MLX16(tmp_hi, tmp_lo, INV_DTX_HANGOVER_P1, EXTRACT_H(VPP_SCALE64_TO_16(tmp_hi, tmp_lo)));
        for (i = 0; i < DTX_HANGOVER; i++)
        {
            //L_temp = L_mac (L_temp, INV_DTX_HANGOVER_P1, lsf_old[i][j]);
            //L_temp = L_MAC(L_temp, INV_DTX_HANGOVER_P1, lsf_old[i][j]);.
            VPP_MLA16(tmp_hi, tmp_lo, INV_DTX_HANGOVER_P1, lsf_old[i][j]);
        }

        //lsf_aver[j] = extract_h (L_temp);
        lsf_aver[j] = EXTRACT_H(VPP_SCALE64_TO_16(tmp_hi, tmp_lo));
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gain_code_history_tx
 *
 *   PURPOSE: Update the fixed codebook gain parameter history of the
 *            encoder. The fixed codebook gain parameters kept in the buffer
 *            are used later for computing the reference fixed codebook
 *            gain parameter value and the averaged fixed codebook gain
 *            parameter value.
 *
 *   INPUTS:      new_gain_code   New fixed codebook gain value
 *
 *                gain_code_old_tx[0..4*DTX_HANGOVER-1]
 *                                Old fixed codebook gain history of encoder
 *
 *   OUTPUTS:     gain_code_old_tx[0..4*DTX_HANGOVER-1]
 *                                Updated fixed codebook gain history of encoder
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void  __attribute__((section(".sramtext"))) update_gain_code_history_tx (
    INT16 new_gain_code,
    INT16 gain_code_oldtx[4 * DTX_HANGOVER]
)
{

    /* Circular buffer */
    gain_code_oldtx[buf_p_tx] = new_gain_code;


    //if (sub (buf_p_tx, (4 * DTX_HANGOVER - 1)) == 0)
    if (SUB (buf_p_tx, (4 * DTX_HANGOVER - 1)) == 0)
    {
        buf_p_tx = 0;
    }
    else
    {
        //buf_p_tx = add (buf_p_tx, 1);
        buf_p_tx = ADD (buf_p_tx, 1);
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gain_code_history_rx
 *
 *   PURPOSE: Update the fixed codebook gain parameter history of the
 *            decoder. The fixed codebook gain parameters kept in the buffer
 *            are used later for computing the reference fixed codebook
 *            gain parameter value.
 *
 *   INPUTS:      new_gain_code   New fixed codebook gain value
 *
 *                gain_code_old_tx[0..4*DTX_HANGOVER-1]
 *                                Old fixed codebook gain history of decoder
 *
 *   OUTPUTS:     gain_code_old_tx[0..4*DTX_HANGOVER-1]
 *                                Updated fixed codebk gain history of decoder
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void update_gain_code_history_rx (
    INT16 new_gain_code,
    INT16 gain_code_oldrx[4 * DTX_HANGOVER]
)
{

    /* Circular buffer */
    gain_code_oldrx[buf_p_rx] = new_gain_code;


    //if (sub (buf_p_rx, (4 * DTX_HANGOVER - 1)) == 0)
    if (SUB (buf_p_rx, (4 * DTX_HANGOVER - 1)) == 0)
    {
        buf_p_rx = 0;
    }
    else
    {
        //buf_p_rx = add(buf_p_rx, 1);
        buf_p_rx = ADD(buf_p_rx, 1);
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: compute_CN_excitation_gain
 *
 *   PURPOSE: Compute the unquantized fixed codebook gain. Computation is
 *            based on the energy of the Linear Prediction residual signal.
 *
 *   INPUTS:      res2[0..39]   Linear Prediction residual signal
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Unquantized fixed codebook gain
 *
 *************************************************************************/

INT16 compute_CN_excitation_gain (
    INT16 res2[L_SUBFR]
)
{
    INT16 i, norm, norm1, temp, overfl;
    INT32 L_temp;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;

    /* Compute the energy of the LP residual signal */

    norm = 0;
    do
    {
        overfl = 0;

        //L_temp = 0L;
        tmp_lo = 0;
        for (i = 0; i < L_SUBFR; i++)
        {
            //temp = shr (res2[i], norm);
            temp = SHR_V(res2[i], norm);
            //L_temp = L_mac (L_temp, temp, temp);
            //L_temp = L_MAC(L_temp, temp, temp);
            VPP_MLA16(tmp_hi, tmp_lo,temp, temp);
        }
        L_temp = VPP_SCALE64_TO_16(tmp_hi, tmp_lo);

        if (L_SUB(L_temp, MAX_32) == 0)
        {
            //norm = add (norm, 1);
            norm = ADD (norm, 1);
            overfl = 1;                  /* Set the overflow flag */
        }

    }
    while (overfl != 0);

    //L_temp = L_add (L_temp, 1L);
    L_temp = L_ADD(L_temp, 1L);              /* Avoid the case of all zeros */

    /* Take the square root of the obtained energy value (sqroot is a 2nd
       order Taylor series approximation) */

    norm1 = norm_l (L_temp);
    //temp = extract_h (L_shl (L_temp, norm1));
    temp = EXTRACT_H (L_SHL(L_temp, norm1));

    //L_temp = L_mult (temp, temp);
    L_temp = L_MULT(temp, temp);
    //L_temp = L_sub (805306368L, L_shr (L_temp, 3));
    L_temp = L_SUB (805306368L, L_SHR_D(L_temp, 3));
    //L_temp = L_add (L_temp, L_mult (24576, temp));
    L_temp = L_ADD(L_temp, L_MULT (24576, temp));


    //temp = extract_h (L_temp);
    temp = EXTRACT_H(L_temp);
    tmp_hi =0;

    if ((norm1 & 0x0001) != 0)
    {
        //temp = mult_r (temp, 23170);
        //temp = MULT_R(temp, 23170);
        VPP_MULT_R(tmp_hi, tmp_lo, temp, 23170);
        temp = (INT32) L_SHR_D((INT32)tmp_lo, 15);
        //norm1 = sub (norm1, 1);
        norm1 = SUB (norm1, 1);
    }
    /* Divide the result of sqroot operation by sqroot(10) */

    //temp = mult_r (temp, 10362);
    //temp = MULT_R(temp, 10362);
    tmp_hi =0;
    VPP_MULT_R(tmp_hi, tmp_lo, temp, 10362);
    temp = (INT32) L_SHR_D((INT32)tmp_lo, 15);

    /* Re-scale to get the final value */

    //norm1 = shr (norm1, 1);
    norm1 = SHR_D(norm1, 1);
    //norm1 = sub (norm1, norm);
    norm1 = SUB (norm1, norm);


    /*if (norm1 >= 0)
    {
        //temp = shr (temp, norm1);
        temp = SHR_V(temp, norm1);
    }
    else
    {
        //temp = shl (temp, abs_s (norm1));
        temp = SHL(temp, ABS_S(norm1));
    }*/
    temp = SHR_V(temp, norm1);

    return temp;
}

/*************************************************************************
 *
 *   FUNCTION NAME: update_gcode0_CN
 *
 *   PURPOSE: Update the reference fixed codebook gain parameter value.
 *            The reference value is computed by averaging the quantized
 *            fixed codebook gain parameter values which exist in the
 *            fixed codebook gain parameter history.
 *
 *   INPUTS:      gain_code_old[0..4*DTX_HANGOVER-1]
 *                              fixed codebook gain parameter history
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Computed reference fixed codebook gain
 *
 *************************************************************************/

INT16 update_gcode0_CN (
    INT16 gain_code_old[4 * DTX_HANGOVER]
)
{
    INT16 i, j;
    //INT32 L_temp, L_ret;
    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;
    register INT32 ret_hi=0;
    register UINT32 ret_lo=0;

    ret_lo = 0L;
    for (i = 0; i < DTX_HANGOVER; i++)
    {
        //L_temp = L_MULT(0x1fff, gain_code_old[4 * i]);
        VPP_MLX16(tmp_hi, tmp_lo, 0x1fff, gain_code_old[4 * i]);
        for (j = 1; j < 4; j++)
        {
            //L_temp = L_mac (L_temp, 0x1fff, gain_code_old[4 * i + j]);
            //L_temp = L_MAC(L_temp, 0x1fff, gain_code_old[4 * i + j]);
            VPP_MLA16(tmp_hi, tmp_lo, 0x1fff, gain_code_old[4 * i + j]);

        }
        //L_ret = L_mac (L_ret, INV_DTX_HANGOVER, extract_h (L_temp));
        //L_ret = L_MAC (L_ret, INV_DTX_HANGOVER, EXTRACT_H(L_temp));
        VPP_MLA16(ret_hi, ret_lo, INV_DTX_HANGOVER, EXTRACT_H(VPP_SCALE64_TO_16(tmp_hi, tmp_lo)));

    }

    //return extract_h (L_ret);
    return EXTRACT_H(VPP_SCALE64_TO_16(ret_hi, ret_lo));
}

/*************************************************************************
 *
 *   FUNCTION NAME: aver_gain_code_history
 *
 *   PURPOSE: Compute the averaged fixed codebook gain parameter value.
 *            Computation is performed by averaging the fixed codebook
 *            gain parameter values which exist in the fixed codebook
 *            gain parameter history, together with the fixed codebook
 *            gain parameter value of the current subframe.
 *
 *   INPUTS:      cn_excitation_gain
 *                              Unquantized fixed codebook gain value
 *                              of the current subframe
 *                gain_code_old[0..4*DTX_HANGOVER-1]
 *                              fixed codebook gain parameter history
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Averaged fixed codebook gain value
 *
 *************************************************************************/

INT16 aver_gain_code_history (
    INT16 cn_excitation_gain,
    INT16 gain_code_old[4 * DTX_HANGOVER]
)
{
    INT16 i;
    //INT32 L_ret;
    register INT32 ret_hi=0;
    register UINT32 ret_lo=0;

    //L_ret = L_MULT(0x470, cn_excitation_gain);
    VPP_MLX16(ret_hi, ret_lo, 0x470, cn_excitation_gain);

    for (i = 0; i < (4 * DTX_HANGOVER); i++)
    {
        //L_ret = L_mac (L_ret, 0x470, gain_code_old[i]);
        //L_ret = L_MAC(L_ret, 0x470, gain_code_old[i]);
        VPP_MLA16(ret_hi, ret_lo,0x470, gain_code_old[i]);
    }
    //return extract_h (L_ret);
    return EXTRACT_H(VPP_SCALE64_TO_16(ret_hi, ret_lo));
}

/*************************************************************************
 *
 *   FUNCTION NAME: build_CN_code
 *
 *   PURPOSE: Compute the comfort noise fixed codebook excitation. The
 *            gains of the pulses are always +/-1.
 *
 *   INPUTS:      *seed         Old CN generator shift register state
 *
 *   OUTPUTS:     cod[0..39]    Generated comfort noise fixed codebook vector
 *                *seed         Updated CN generator shift register state
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void build_CN_code (
    INT16 cod[],
    INT32 *seed
)
{
    INT16 i, j, k;

    for (i = 0; i < L_SUBFR; i++)
    {
        cod[i] = 0;
    }

    for (k = 0; k < NB_PULSE; k++)
    {
        i = pseudonoise (seed, 2);      /* generate pulse position */
        //i = shr (extract_l (L_mult (i, 10)), 1);
        i = SHR_D(EXTRACT_L(L_MULT(i, 10)), 1);

        //i = add (i, k);
        i = ADD (i, k);

        j = pseudonoise (seed, 1);      /* generate sign           */


        if (j > 0)
        {
            cod[i] = 4096;
        }
        else
        {
            cod[i] = -4096;
        }
    }

    return;
}

/*************************************************************************
 *
 *   FUNCTION NAME: pseudonoise
 *
 *   PURPOSE: Generate a random integer value to use in comfort noise
 *            generation. The algorithm uses polynomial x^31 + x^3 + 1
 *            (length of PN sequence is 2^31 - 1).
 *
 *   INPUTS:      *shift_reg    Old CN generator shift register state
 *
 *
 *   OUTPUTS:     *shift_reg    Updated CN generator shift register state
 *
 *   RETURN VALUE: Generated random integer value
 *
 *************************************************************************/

INT16 pseudonoise (
    INT32 *shift_reg,
    INT16 no_bits
)
{
    INT16 noise_bits, Sn, i;

    noise_bits = 0;
    for (i = 0; i < no_bits; i++)
    {
        /* State n == 31 */
        if ((*shift_reg & 0x00000001L) != 0)
        {
            Sn = 1;
        }
        else
        {
            Sn = 0;
        }

        /* State n == 3 */
        if ((*shift_reg & 0x10000000L) != 0)
        {
            Sn = Sn ^ 1;
        }
        else
        {
            Sn = Sn ^ 0;
        }

        //noise_bits = shl (noise_bits, 1);
        noise_bits = SHL(noise_bits, 1);

        //noise_bits = noise_bits | (extract_l (*shift_reg) & 1);
        noise_bits = noise_bits | (EXTRACT_L(*shift_reg) & 1);

        //*shift_reg = L_shr (*shift_reg, 1);
        *shift_reg = L_SHR_D(*shift_reg, 1);

        if (Sn & 1)
        {
            *shift_reg = *shift_reg | 0x40000000L;
        }
    }

    return noise_bits;
}

/*************************************************************************
 *
 *   FUNCTION NAME: interpolate_CN_param
 *
 *   PURPOSE: Interpolate a comfort noise parameter value over the comfort
 *            noise update period.
 *
 *   INPUTS:      old_param     The older parameter of the interpolation
 *                              (the endpoint the interpolation is started
 *                              from)
 *                new_param     The newer parameter of the interpolation
 *                              (the endpoint the interpolation is ended to)
 *                rxdtx_state  State of the comfort noise insertion period
 *
 *   OUTPUTS:     none
 *
 *   RETURN VALUE: Interpolated CN parameter value
 *
 *************************************************************************/

INT16 interpolate_CN_param (
    INT16 old_param,
    INT16 new_param,
    INT16 rxdtx_state
)
{
    static const INT16 interp_factor[CN_INT_PERIOD] =
    {
        0x0555, 0x0aaa, 0x1000, 0x1555, 0x1aaa, 0x2000,
        0x2555, 0x2aaa, 0x3000, 0x3555, 0x3aaa, 0x4000,
        0x4555, 0x4aaa, 0x5000, 0x5555, 0x5aaa, 0x6000,
        0x6555, 0x6aaa, 0x7000, 0x7555, 0x7aaa, 0x7fff
    };

    register INT32 tmp_hi=0;
    register UINT32 tmp_lo=0;

    INT16 temp;
    //INT32 L_temp;

    //temp = sub (0x7fff, interp_factor[rxdtx_state]);
    temp = SUB (0x7fff, interp_factor[rxdtx_state]);

    //temp = add (temp, 1);
    temp = ADD (temp, 1);

    //L_temp = L_mult (interp_factor[rxdtx_state], new_param);
    //L_temp = L_MULT(interp_factor[rxdtx_state], new_param);
    VPP_MLX16 (tmp_hi, tmp_lo, interp_factor[rxdtx_state], new_param);
    //L_temp = L_mac (L_temp, temp, old_param);
    //L_temp = L_MAC(L_temp, temp, old_param);
    VPP_MLA16 (tmp_hi, tmp_lo, temp, old_param);
    //temp = round(L_temp);
    temp = ROUND(VPP_SCALE64_TO_16(tmp_hi, tmp_lo));

    return temp;
}

/*************************************************************************
 *
 *   FUNCTION NAME:  interpolate_CN_lsf
 *
 *   PURPOSE: Interpolate comfort noise LSF parameter vector over the comfort
 *            noise update period.
 *
 *   INPUTS:      lsf_old_CN[0..9]
 *                              The older LSF parameter vector of the
 *                              interpolation (the endpoint the interpolation
 *                              is started from)
 *                lsf_new_CN[0..9]
 *                              The newer LSF parameter vector of the
 *                              interpolation (the endpoint the interpolation
 *                              is ended to)
 *                rx_dtx_state  State of the comfort noise insertion period
 *
 *   OUTPUTS:     lsf_interp_CN[0..9]
 *                              Interpolated LSF parameter vector
 *
 *   RETURN VALUE: none
 *
 *************************************************************************/

void interpolate_CN_lsf (
    INT16 lsf_old_CN[M],
    INT16 lsf_new_CN[M],
    INT16 lsf_interp_CN[M],
    INT16 rxdtx_state
)
{
    INT16 i;

    for (i = 0; i < M; i++)
    {
        lsf_interp_CN[i] = interpolate_CN_param (lsf_old_CN[i],
                           lsf_new_CN[i],
                           rxdtx_state);
    }

    return;
}































































