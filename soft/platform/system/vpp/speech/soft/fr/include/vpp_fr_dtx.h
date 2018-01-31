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

#ifndef DTX_H
#define DTX_H

#define TX_SP_FLAG               0x0001
#define TX_VAD_FLAG              0x0002
#define TX_HANGOVER_ACTIVE       0x0004
//#define TX_PREV_HANGOVER_ACTIVE  0x0008
#define TX_SID_UPDATE            0x0010
#define TX_USE_OLD_SID           0x0020

#define VALID_SID_FRAME          1
#define INVALID_SID_FRAME        2
#define GOOD_SPEECH_FRAME        3
#define UNUSABLE_FRAME           4

#define RX_SP_FLAG               0x0001
#define RX_UPD_SID_QUANT_MEM     0x0002
#define RX_FIRST_SID_UPDATE      0x0004
#define RX_CONT_SID_UPDATE       0x0008
#define RX_LOST_SID_FRAME        0x0010
#define RX_INVALID_SID_FRAME     0x0020
#define RX_NO_TRANSMISSION       0x0040
#define RX_DTX_MUTING            0x0080
#define RX_SPH_MUTING            0x0100

//#define RX_PREV_DTX_MUTING       0x0100
#define RX_CNI_BFI               0x0200
//#define RX_FIRST_SP_FLAG         0x0400


#define VALID_SID_THRESH 2
#define INVALID_SID_THRESH 16

#define DTX_HANGOVER  4
#define DTX_ELAPSED_THRESHOLD (24 + DTX_HANGOVER - 1)

#define CN_INT_PERIOD 24              /* Comfort noise interpolation period
                                         (nbr of frames between successive
                                         SID updates in the decoder) */
#define PN_INITIAL_SEED 0x70816958L   /* Pseudo noise generator seed value  */

void dtx_init();

void tx_dtx (
    INT16 VAD_flag,
    INT16 *txdtx_flag
);

void rx_dtx (
    INT16 *rxdtx_flag,
    INT16 TAF,
    INT16 bfi,
    INT16 SID_flag
);

INT16 pseudonoise (
    long *shift_reg,
    INT16 no_bits
);

void lost_frame_process(
    INT16    * LARc,        /* [0..7]               OUT      */
    INT16    * Nc,          /* [0..3]               OUT      */
    INT16    * bc,          /* [0..3]               OUT      */
    INT16    * Mc,          /* [0..3]               OUT      */
    INT16    * xmaxc,       /* [0..3]               OUT      */
    INT16    * xmc,         /* [0..13*4]            OUT      */
    long    * seed
);

void silence_frame_paramer(
    INT16    * LARc,        /* [0..7]               OUT      */
    INT16    * Nc,          /* [0..3]               OUT      */
    INT16    * bc,          /* [0..3]               OUT      */
    INT16    * Mc,          /* [0..3]               OUT      */
    INT16    * xmaxc,       /* [0..3]               OUT      */
    INT16    * xMc          /* [0..13*4]            OUT      */
);

void CN_paramer(
    INT16    * Nc,          /* [0..3]               OUT      */
    INT16    * bc,          /* [0..3]               OUT      */
    INT16    * Mc,          /* [0..3]               OUT      */
    INT16    * xmc,         /* [0..13*4]            OUT      */
    long    * seed
);

INT16 sid_frame_detection (
    INT16 * xmc);



INT16 interpolate_CN_param (
    INT16 old_param,
    INT16 new_param,
    INT16 rxdtx_state
);

void interpolate_CN_LARc (
    INT16 * LARc_old_CN,
    INT16 * LARc_new_CN,
    INT16 * LARc_interp_CN,
    INT16 rxdtx_state
);

long gsm_L_mac (long L_var3, INT16 var1, INT16 var2);

INT16 gsm_round (long L_var1);

#endif

