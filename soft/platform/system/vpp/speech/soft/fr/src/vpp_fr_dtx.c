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



#include "vpp_fr_private.h"
#include "vpp_fr_dtx.h"

#include "vpp_fr_debug.h"
#include "vpp_fr_profile_codes.h"


INT16 txdtx_ctrl;
INT16 rxdtx_ctrl;
INT16 rx_dtx_state;

long L_pn_seed_rx;            /* PN generator seed (decoder)             */

//  long L_pn_seed_tx;                      // remarked by Cui 2004.11.25

static INT16 prev_SID_frames_lost; /* Counter for lost SID frames         */
static INT16 rxdtx_N_elapsed;  /* Measured time from previous SID frame   */
static INT16 rxdtx_aver_period;/* Length of hangover period (VAD=0, SP=1) */

INT16 LAR_SID_tx[DTX_HANGOVER][8];
INT16 xmax_SID_tx[DTX_HANGOVER][4];

INT16 LARc_old_tx[8],xmaxc_old_tx;

static INT16 txdtx_hangover;   /* Length of hangover period (VAD=0, SP=1) */
//static INT16  rxdtx_aver_period;/* Length of hangover period (VAD=0, SP=1) */
static INT16 txdtx_N_elapsed;  /* Measured time from previous SID frame   */

extern INT16    LARc_old_rx[8], Nc_old_rx[4], Mc_old_rx[4], bc_old_rx[4],
       xmaxc_old_rx[4], xmc_old_rx[13*4];
extern INT16 LARc_old_2_rx[8];//,   LARc_new_CN_rx[8];
extern INT16 xmaxc_old_2_rx[4];//, xmaxc_new_CN_rx;

extern INT16 lost_speech_frames;
//extern INT16 ptch;
//extern INT16 Nc_old_tx;

void dtx_init()
{
    int i,j;

    VPP_FR_PROFILE_FUNCTION_ENTER(dtx_init);

    //send
    txdtx_hangover = DTX_HANGOVER;
    txdtx_N_elapsed = 0x7fff;
    txdtx_ctrl = TX_SP_FLAG | TX_VAD_FLAG;

    for(i=0; i<DTX_HANGOVER; i++)
    {
        for(j=0; j<8; j++)  LAR_SID_tx[i][j] = 0; //for avering use
        for(j=0; j<4; j++) xmax_SID_tx[i][j] = 0;
    }

    for(i=0; i<8; i++) LARc_old_tx[i] = 0; //for use old SID value
    xmaxc_old_tx = 0;

//    L_pn_seed_tx = PN_INITIAL_SEED;                 // remarked by Cui 2004.11.25



//  ptch = 1;
//  Nc_old_tx = 40; //for pitch calculate

    //rcv

    rxdtx_aver_period = DTX_HANGOVER;
    rxdtx_N_elapsed = 0x7fff;
    rxdtx_ctrl = RX_SP_FLAG;
    rx_dtx_state = CN_INT_PERIOD - 1;
    prev_SID_frames_lost = 0;
    L_pn_seed_rx = PN_INITIAL_SEED;

    lost_speech_frames = 0;

    //for use old frme parm use
    for(i=0; i<8; i++) LARc_old_rx[i] = 0;

    for(i=0; i<4; i++)
    {
        Nc_old_rx[i] = 40;
        Mc_old_rx[i] = 0;
        bc_old_rx[i] = 0;
        xmaxc_old_rx[i] = 0;
    }

    for(i=0; i<52; i++)
        xmc_old_rx[i] = 0;

    //for CN interpalate use
    for(i=0; i<8; i++)
    {
        LARc_old_2_rx[i] = 0;
        //LARc_new_CN_rx[i] = 0;
    }

    for(i=0; i<4; i++)
    {
        xmaxc_old_2_rx[i] = 0;
        //  xmaxc_new_CN_rx = 0;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(dtx_init);

}




/*
void tx_dtx (
    INT16 VAD_flag,
    INT16 *txdtx_ctrl
)
{
* txdtx_ctrl = TX_SP_FLAG | TX_VAD_FLAG;
}
*/

void tx_dtx (
    INT16 VAD_flag,
    INT16 *txdtx_flag
)
{

    register INT32 ltmp;

    VPP_FR_PROFILE_FUNCTION_ENTER(tx_dtx);

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */

    txdtx_N_elapsed = GSM_SATADD (txdtx_N_elapsed, 1); //saturates

    /* If voice activity was detected, reset hangover counter */

    //if (gsm_sub (VAD_flag, 1) == 0)
    if (VAD_flag == 1)
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


            //       if ((*txdtx_ctrl & TX_HANGOVER_ACTIVE) != 0)            // remarked by Cui 2004.11.25
            //    {


//                *txdtx_ctrl = TX_PREV_HANGOVER_ACTIVE
//                    | TX_SID_UPDATE;



            //      L_pn_seed_tx = PN_INITIAL_SEED;                  // remarked by Cui 2004.11.25
            //     }




//            else
//            {
            *txdtx_flag = TX_SID_UPDATE;
//            }
        }
        else
        {
            /* Hangover period is not over, update hangover counter */
            txdtx_hangover = GSM_SUB (txdtx_hangover, 1);

            /* Check if elapsed time from last SID update is greater than
               threshold. If not, set SP=0 (although hangover period is not
               over) and use old SID parameters for new SID frame.
               N_elapsed counter must be summed with hangover counter in order
               to avoid erroneus SP=1 decision in case when N_elapsed is grown
               bigger than threshold and hangover period is still active */

            //if (gsm_sub (GSM_SATADD(txdtx_N_elapsed, txdtx_hangover),
            //         DTX_ELAPSED_THRESHOLD) < 0)
            if (GSM_SATADD(txdtx_N_elapsed, txdtx_hangover)< DTX_ELAPSED_THRESHOLD)
            {
                /* old SID frame should be used */

                //    *txdtx_ctrl = TX_USE_OLD_SID;

                *txdtx_flag= TX_USE_OLD_SID|TX_HANGOVER_ACTIVE ;  //  | TX_SP_FLAG ;     // Changed by Cui   2004.11.25

            }
            else
            {
                //hangover period first frame
//                if ((*txdtx_ctrl & TX_HANGOVER_ACTIVE) != 0)
//                {
//                    *txdtx_ctrl = TX_PREV_HANGOVER_ACTIVE
//                        | TX_HANGOVER_ACTIVE
//                        | TX_SP_FLAG;

//                  for(i=0;i<DTX_HANGOVER;i++)
//                  {
//                      for(j=0;j<8;j++)  LAR_SID_tx[i][j] = 0;
//                      for(j=0;j<4;j++) xmax_SID_tx[i][j] = 0;
//                  }
//                }
//                else
//                {
                *txdtx_flag= TX_HANGOVER_ACTIVE
                             | TX_SP_FLAG;
//                }
            }
        }
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(tx_dtx);
    return;
}


void rx_dtx (
    INT16  *rxdtx_flag,
    INT16  TAF,
    INT16  bfi,
    INT16  SID_flag
)
{
    INT16  frame_type;
    register INT32 ltmp;


    VPP_FR_PROFILE_FUNCTION_ENTER(rx_dtx);

    /* Frame classification according to bfi-flag and ternary-valued
       SID flag. The frames between SID updates (not actually trans-
       mitted) are also classified here; they will be discarded later
       and provided with "NO TRANSMISSION"-flag */

    //if ((gsm_sub (SID_flag, 2) == 0) && (bfi == 0))
    if ((SID_flag == 2)&& (bfi == 0))
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
    else//bfi == 0 && SID_flag == 1   //bfi != 0 && SID_flag == 1 2
    {
        frame_type = INVALID_SID_FRAME;
    }

    /* Update of decoder state */
    /* Previous frame was classified as a speech frame */

    if ((*rxdtx_flag  & RX_SP_FLAG) != 0)
    {

        //if (gsm_sub (frame_type VALID_SID_FRAME) == 0)
        if (frame_type == VALID_SID_FRAME)
        {
            *rxdtx_flag = RX_FIRST_SID_UPDATE;
        }
        //else if (gsm_sub (frame_type, INVALID_SID_FRAME) == 0)
        else if (frame_type == INVALID_SID_FRAME)
        {
            *rxdtx_flag = RX_FIRST_SID_UPDATE
                          | RX_INVALID_SID_FRAME;
        }
        //else if (gsm_sub (frame_type, UNUSABLE_FRAME) == 0)
        else if (frame_type == UNUSABLE_FRAME)
        {
            *rxdtx_flag = RX_SP_FLAG;
            lost_speech_frames ++ ;
        }
        //else if (gsm_sub (frame_type, GOOD_SPEECH_FRAME) == 0)
        else if (frame_type == GOOD_SPEECH_FRAME)
        {
            *rxdtx_flag = RX_SP_FLAG;
            lost_speech_frames = 0;
        }
    }
    else
    {

        //if (gsm_sub (frame_type, VALID_SID_FRAME) == 0)
        if (frame_type == VALID_SID_FRAME)
        {
            *rxdtx_flag = RX_CONT_SID_UPDATE;
        }
        //else if (gsm_sub (frame_type, INVALID_SID_FRAME) == 0)
        else if (frame_type == INVALID_SID_FRAME)
        {
            *rxdtx_flag = RX_CONT_SID_UPDATE
                          | RX_INVALID_SID_FRAME;
        }
        //else if (gsm_sub (frame_type, UNUSABLE_FRAME) == 0)
        else if (frame_type == UNUSABLE_FRAME)
        {
            *rxdtx_flag = RX_CNI_BFI;
        }
        //else if (gsm_sub (frame_type, GOOD_SPEECH_FRAME) == 0)
        else if (frame_type == GOOD_SPEECH_FRAME)
        {
            /* If the previous frame (during CNI period) was muted,
               raise the RX_PREV_DTX_MUTING flag */

//            if ((*rxdtx_ctrl & RX_DTX_MUTING) != 0)
//            {
//                *rxdtx_ctrl = RX_SP_FLAG | RX_FIRST_SP_FLAG
//                            | RX_PREV_DTX_MUTING;

//            else
//            {
            *rxdtx_flag = RX_SP_FLAG ;//| RX_FIRST_SP_FLAG;

//            }
        }
    }



    if ((*rxdtx_flag & RX_SP_FLAG) != 0)
    {
        prev_SID_frames_lost = 0;
        rx_dtx_state = CN_INT_PERIOD - 1;
    }
    else
    {
        /* First SID frame */

        if ((*rxdtx_flag & RX_FIRST_SID_UPDATE) != 0)
        {
            prev_SID_frames_lost = 0;
            rx_dtx_state = CN_INT_PERIOD - 1;
        }

        /* SID frame detected, but not the first SID */

        if ((*rxdtx_flag & RX_CONT_SID_UPDATE) != 0)
        {
            prev_SID_frames_lost = 0;


            //if (gsm_sub (frame_type, VALID_SID_FRAME) == 0)
            if (frame_type == VALID_SID_FRAME)
            {
                rx_dtx_state = 0;
            }
            //else if (gsm_sub (frame_type, INVALID_SID_FRAME) == 0)
            else if (frame_type == INVALID_SID_FRAME)
            {

                //if (gsm_sub(rx_dtx_state, (CN_INT_PERIOD - 1)) < 0)
                if (rx_dtx_state < (CN_INT_PERIOD - 1))
                {
                    rx_dtx_state = GSM_ADD(rx_dtx_state, 1);
                }
            }
        }

        /* Bad frame received in CNI mode */

        if ((*rxdtx_flag & RX_CNI_BFI) != 0)
        {

            //if (gsm_sub (rx_dtx_state, (CN_INT_PERIOD - 1)) < 0)
            if (rx_dtx_state < (CN_INT_PERIOD - 1))
            {
                rx_dtx_state = GSM_ADD (rx_dtx_state, 1);
            }

            /* If an unusable frame is received during CNI period
               when TAF == 1, the frame is classified as a lost
               SID frame */

            //if (gsm_sub (TAF, 1) == 0)
            if (TAF == 1)
            {
                *rxdtx_flag = *rxdtx_flag | RX_LOST_SID_FRAME;

                prev_SID_frames_lost = GSM_ADD (prev_SID_frames_lost, 1);
            }
            else /* No transmission occurred */
            {
                *rxdtx_flag = *rxdtx_flag | RX_NO_TRANSMISSION;

            }


            //if (gsm_sub (prev_SID_frames_lost, 1) > 0)
            if (prev_SID_frames_lost > 1)
            {
                *rxdtx_flag = *rxdtx_flag | RX_DTX_MUTING;

            }
        }
    }

    /* N_elapsed (frames since last SID update) is incremented. If SID
       is updated N_elapsed is cleared later in this function */

    rxdtx_N_elapsed = GSM_SATADD (rxdtx_N_elapsed, 1);


    if ((*rxdtx_flag & RX_SP_FLAG) != 0)
    {
        rxdtx_aver_period = DTX_HANGOVER;
    }
    else
    {

        //if (gsm_sub (rxdtx_N_elapsed, DTX_ELAPSED_THRESHOLD) > 0)
        if (rxdtx_N_elapsed > DTX_ELAPSED_THRESHOLD)
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
            //rxdtx_aver_period = gsm_sub (rxdtx_aver_period, 1);
            rxdtx_aver_period = GSM_SUB (rxdtx_aver_period, 1);
        }
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(rx_dtx);

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
    long *shift_reg,
    INT16 no_bits
)
{
    INT16 noise_bits, Sn, i;

    VPP_FR_PROFILE_FUNCTION_ENTER(pseudonoise);

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

        noise_bits = gsm_asl (noise_bits, 1);
        noise_bits = noise_bits | ((INT16) (*shift_reg) & 1);


        *shift_reg = gsm_L_asr (*shift_reg, 1);

        if (Sn & 1)
        {
            *shift_reg = *shift_reg | 0x40000000L;
        }
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(pseudonoise);
    return noise_bits;
}
//SID position:
//52个 xmc 3bit 3个bit 第0 bit不要 第68-76 xmc  3bit 第1 bit也不要
//13*4*3 - 13*4 - 9 = 95
INT16 sid_frame_detection (
    INT16  xmc[]
)
{
    VPP_FR_PROFILE_FUNCTION_ENTER(sid_frame_detection);

    INT16 i, nbr_errors, sid,temp;

    nbr_errors = 0;

    for(i=0; i<43; i++)
    {
        temp = xmc[i]&0x06;
        if(temp==2 || temp == 4) nbr_errors = GSM_ADD (nbr_errors, 1);
        if(temp == 6) nbr_errors = GSM_ADD (nbr_errors, 2);
    }

    for(i=43; i<52; i++)
    {
        temp = xmc[i]&0x04;
        if(temp == 4) nbr_errors = GSM_ADD (nbr_errors, 1);
    }

    // Frame classification

    //if (gsm_sub (nbr_errors, VALID_SID_THRESH) < 0)
    if (nbr_errors < VALID_SID_THRESH)
    {
        // Valid SID frame
        sid = 2;
    }
    //else if (gsm_sub (nbr_errors, INVALID_SID_THRESH) < 0)
    else if (nbr_errors < INVALID_SID_THRESH)
    {
        // Invalid SID frame
        sid = 1;
    }
    else
    {
        // Speech frame
        sid = 0;
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(sid_frame_detection);
    return sid;

}

/*
static const INT16 SID_codeword_bit_idx[95] =
{
     55,  56,  58,  59,  61,  62,  64,  65,  67,  68,
     70,  71,  73,  74,  76,  77,  79,  80,  82,  83,
     85,  86,  88,  89,  91,  92, 111, 112, 114, 115,
    117, 118, 120, 121, 123, 124, 126, 127, 129, 130,
    132, 133, 135, 136, 138, 139, 141, 142, 144, 145,
    147, 148, 167, 168, 170, 171, 173, 174, 176, 177,
    179, 180, 182, 183, 185, 186, 188, 189, 191, 192,
    194, 195, 197, 198, 200, 201, 203, 204, 223, 224,
    226, 227, 229, 230, 232, 233, 236, 239, 242, 245,
    248, 251, 254, 257, 260
};
//52个 xmc 3bit 3个bit 第0 bit不要 第68-76 xmc  3bit 第1 bit也不要
//13*4*3 - 13*4 - 9 = 95

INT16 sid_frame_detection (
    INT16 ser2[]
)
{
    INT16 i, nbr_errors, sid;

    // Search for bit errors in SID codeword
    nbr_errors = 0;
    for (i = 0; i < 95; i++)
    {

        if (ser2[SID_codeword_bit_idx[i]] == 1)
        {
            nbr_errors = GSM_ADD (nbr_errors, 1);
        }
    }

    // Frame classification

    if (gsm_sub (nbr_errors, VALID_SID_THRESH) < 0)
    {                                                   // Valid SID frame
        sid = 2;
    }
    else if (gsm_sub (nbr_errors, INVALID_SID_THRESH) < 0)
    {                                                   // Invalid SID frame
        sid = 1;
    }
    else
    {                                                   // Speech frame
        sid = 0;
    }

    return sid;
}
*/

#define M 8

void interpolate_CN_LARc (
    INT16 LARc_old_CN[M],
    INT16 LARc_new_CN[M],
    INT16 LARc_interp_CN[M],
    INT16 rxdtx_state
)
{
    INT16 i;

    VPP_FR_PROFILE_FUNCTION_ENTER(interpolate_CN_LARc);

    for (i = 0; i < M; i++)
    {
        LARc_interp_CN[i] = interpolate_CN_param (LARc_old_CN[i],
                            LARc_new_CN[i],
                            rxdtx_state);
    }

    VPP_FR_PROFILE_FUNCTION_EXIT(interpolate_CN_LARc);

    return;
}


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
//{0.0000013,0.083,0.125,........,1}

    INT16 temp;
    long L_temp;//L_temp1;

    VPP_FR_PROFILE_FUNCTION_ENTER(interpolate_CN_param);

    L_temp = GSM_L_MULT(interp_factor[rxdtx_state], new_param);
    //temp = gsm_sub (0x7fff, interp_factor[rxdtx_state]);
    temp = GSM_SUB (0x7fff, interp_factor[rxdtx_state]);

    temp = GSM_ADD (temp, 1);

    L_temp = gsm_L_mac (L_temp, temp, old_param);
    //L_temp1 = gsm_L_mult (temp, old_param);
    //L_temp = GSM_L_ADD (L_temp,L_temp1);

    temp = gsm_round (L_temp);

    VPP_FR_PROFILE_FUNCTION_EXIT(interpolate_CN_param);
    return temp;
}


long gsm_L_mac (long L_var3, INT16 var1, INT16 var2)
{
    long L_var_out;
    long L_product;
    register UINT32 utmp;       /* for L_ADD */

    L_product = GSM_L_MULT (var1, var2);
    L_var_out = GSM_L_ADD (L_var3, L_product);
    return (L_var_out);
}


INT16 gsm_round (long L_var1)
{
    INT16 var_out;
    long L_rounded;
    register UINT32 utmp;       /* for L_ADD */

    L_rounded = GSM_L_ADD (L_var1, (long) 0x00008000L);

    var_out = (INT16)(L_rounded>>16);

    return (var_out);
}
