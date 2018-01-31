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


#ifndef _SPC_DEFINE_STD_H_
#define _SPC_DEFINE_STD_H_



// =============================================================================
//  MACROS
// =============================================================================
#define SPC_ITLV_BUFF_TX_DEDICATED               (22*4)
#define SPC_ITLV_BUFF_RX_DEDICATED               (22*32)
#define SPC_ITLV_RX_BUFF_OFFSET                  (128)
#define SPC_ITLV_TX_BUFF_OFFSET                  (4*4)
#define SPC_TMP_ITLV_RX_BUFF_OFFSET              (5*128)
#define SPC_PDCH_DATA_BLOCK_SIZE                 (14)
#define GMSK_BLK_EQOUT_SIZE                      (32)

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SPC_PDCH_DATA_BLOCK_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT32 SPC_PDCH_DATA_BLOCK_T[14];




#endif

