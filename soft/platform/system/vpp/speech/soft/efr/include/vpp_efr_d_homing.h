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
#include "cs_types.h"

#define EHF_MASK 0x0008 /* Encoder Homing Frame pattern */

#define D_HOMING

static const INT16 dhf_mask[PRM_SIZE] =
{
    0x0004,                 /* LPC 1 */
    0x002f,                 /* LPC 2 */
    0x00b4,                 /* LPC 3 */
    0x0090,                 /* LPC 4 */
    0x003e,                 /* LPC 5 */

    0x0156,                 /* LTP-LAG 1 */
    0x000b,                 /* LTP-GAIN 1 */
    0x0000,                 /* PULSE 1_1 */
    0x0001,                 /* PULSE 1_2 */
    0x000f,                 /* PULSE 1_3 */
    0x0001,                 /* PULSE 1_4 */
    0x000d,                 /* PULSE 1_5 */
    0x0000,                 /* PULSE 1_6 */
    0x0003,                 /* PULSE 1_7 */
    0x0000,                 /* PULSE 1_8 */
    0x0003,                 /* PULSE 1_9 */
    0x0000,                 /* PULSE 1_10 */
    0x0003,                 /* FCB-GAIN 1 */

    0x0036,                 /* LTP-LAG 2 */
    0x0001,                 /* LTP-GAIN 2 */
    0x0008,                 /* PULSE 2_1 */
    0x0008,                 /* PULSE 2_2 */
    0x0005,                 /* PULSE 2_3 */
    0x0008,                 /* PULSE 2_4 */
    0x0001,                 /* PULSE 2_5 */
    0x0000,                 /* PULSE 2_6 */
    0x0000,                 /* PULSE 2_7 */
    0x0001,                 /* PULSE 2_8 */
    0x0001,                 /* PULSE 2_9 */
    0x0000,                 /* PULSE 2_10 */
    0x0000,                 /* FCB-GAIN 2 */

    0x0156,                 /* LTP-LAG 3 */
    0x0000,                 /* LTP-GAIN 3 */
    0x0000,                 /* PULSE 3_1 */
    0x0000,                 /* PULSE 3_2 */
    0x0000,                 /* PULSE 3_3 */
    0x0000,                 /* PULSE 3_4 */
    0x0000,                 /* PULSE 3_5 */
    0x0000,                 /* PULSE 3_6 */
    0x0000,                 /* PULSE 3_7 */
    0x0000,                 /* PULSE 3_8 */
    0x0000,                 /* PULSE 3_9 */
    0x0000,                 /* PULSE 3_10 */
    0x0000,                 /* FCB-GAIN 3 */

    0x0036,                 /* LTP-LAG 4 */
    0x000b,                 /* LTP-GAIN 4 */
    0x0000,                 /* PULSE 4_1 */
    0x0000,                 /* PULSE 4_2 */
    0x0000,                 /* PULSE 4_3 */
    0x0000,                 /* PULSE 4_4 */
    0x0000,                 /* PULSE 4_5 */
    0x0000,                 /* PULSE 4_6 */
    0x0000,                 /* PULSE 4_7 */
    0x0000,                 /* PULSE 4_8 */
    0x0000,                 /* PULSE 4_9 */
    0x0000,                 /* PULSE 4_10 */
    0x0000                  /* FCB-GAIN 4 */
};


/* Function Prototypes */




INT16 decoder_homing_frame_test (INT16 parm[], INT16 nbr_of_params);

void decoder_reset (void);

void reset_dec (void);


