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



#define PN_INITIAL_SEED 0x70816958L   /* Pseudo noise generator seed value  */

#define CN_INT_PERIOD 24              /* Comfort noise interpolation period
                                         (nbr of frames between successive
                                         SID updates in the decoder) */

#define DTX_HANGOVER 7                /* Period when SP=1 although VAD=0.
                                         Used for comfort noise averaging */


/* Frame classification constants */

#define VALID_SID_FRAME          1
#define INVALID_SID_FRAME        2
#define GOOD_SPEECH_FRAME        3
#define UNUSABLE_FRAME           4

/* Encoder DTX control flags */

#define TX_SP_FLAG               0x0001
#define TX_VAD_FLAG              0x0002
#define TX_HANGOVER_ACTIVE       0x0004
#define TX_PREV_HANGOVER_ACTIVE  0x0008
#define TX_SID_UPDATE            0x0010
#define TX_USE_OLD_SID           0x0020

/* Decoder DTX control flags */

#define RX_SP_FLAG               0x0001
#define RX_UPD_SID_QUANT_MEM     0x0002
#define RX_FIRST_SID_UPDATE      0x0004
#define RX_CONT_SID_UPDATE       0x0008
#define RX_LOST_SID_FRAME        0x0010
#define RX_INVALID_SID_FRAME     0x0020
#define RX_NO_TRANSMISSION       0x0040
#define RX_DTX_MUTING            0x0080
#define RX_PREV_DTX_MUTING       0x0100
#define RX_CNI_BFI               0x0200
#define RX_FIRST_SP_FLAG         0x0400


#define INV_DTX_HANGOVER (0x7fff / DTX_HANGOVER)
#define INV_DTX_HANGOVER_P1 (0x7fff / (DTX_HANGOVER+1))

#define NB_PULSE 10 /* Number of pulses in fixed codebook excitation */

/* SID frame classification thresholds */

#define VALID_SID_THRESH 2
#define INVALID_SID_THRESH 16

/* Constant DTX_ELAPSED_THRESHOLD is used as threshold for allowing
   SID frame updating without hangover period in case when elapsed
   time measured from previous SID update is below 24 */

#define DTX_ELAPSED_THRESHOLD (24 + DTX_HANGOVER - 1)



void reset_tx_dtx (void);       /* Reset tx dtx variables */
void reset_rx_dtx (void);       /* Reset rx dtx variables */

void tx_dtx (
    INT16 VAD_flag,
    INT16 *txdtx_ctrl
);

void rx_dtx (
    INT16 *rxdtx_ctrl,
    INT16 TAF,
    INT16 bfi,
    INT16 SID_flag
);

void CN_encoding (
    INT16 params[],
    INT16 txdtx_ctrl
);

void sid_codeword_encoding (
    INT16 ser2[]
);

INT16 sid_frame_detection (
    INT16 ser2[]
);

void update_lsf_history (
    INT16 lsf1[M],
    INT16 lsf2[M],
    INT16 lsf_old[DTX_HANGOVER][M]
);

void update_lsf_p_CN (
    INT16 lsf_old[DTX_HANGOVER][M],
    INT16 lsf_p_CN[M]
);

void aver_lsf_history (
    INT16 lsf_old[DTX_HANGOVER][M],
    INT16 lsf1[M],
    INT16 lsf2[M],
    INT16 lsf_aver[M]
);

void update_gain_code_history_tx (
    INT16 new_gain_code,
    INT16 gain_code_old_tx[4 * DTX_HANGOVER]
);

void update_gain_code_history_rx (
    INT16 new_gain_code,
    INT16 gain_code_old_rx[4 * DTX_HANGOVER]
);

INT16 compute_CN_excitation_gain (
    INT16 res2[L_SUBFR]
);

INT16 update_gcode0_CN (
    INT16 gain_code_old_tx[4 * DTX_HANGOVER]
);

INT16 aver_gain_code_history (
    INT16 CN_excitation_gain,
    INT16 gain_code_old[4 * DTX_HANGOVER]
);

void build_CN_code (
    INT16 cod[],
    INT32 *seed
);

INT16 pseudonoise (
    INT32 *shift_reg,
    INT16 no_bits
);

INT16 interpolate_CN_param (
    INT16 old_param,
    INT16 new_param,
    INT16 rx_dtx_state
);

void interpolate_CN_lsf (
    INT16 lsf_old_CN[M],
    INT16 lsf_new_CN[M],
    INT16 lsf_interp_CN[M],
    INT16 rx_dtx_state
);
