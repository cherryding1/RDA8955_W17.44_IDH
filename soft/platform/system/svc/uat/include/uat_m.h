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

#ifndef _UAT_M_H_
#define _UAT_M_H_

#include "cs_types.h"

/// @file uvideos_m.h
/// @mainpage USB CDCACM service
///
/// This service provides CDCACM service on usb
///
/// This document is composed of:
/// - @ref uvideos
///
/// @addtogroup uvideos USB CDCACM Service (ucdcacm)
/// @{
///

// =============================================================================
// TYPES
// =============================================================================

#define UAT_DATA_DIR_IN 1
#define UAT_DATA_DIR_OUT 0

#define UAT_RX_EP_BUF_SIZE (4096)
#define UAT_TX_EP_BUF_SIZE (4096)

typedef VOID (*UAT_TRANS_CALLBACK_T)(UINT8 direction, UINT16 len);
PUBLIC UAT_TRANS_CALLBACK_T uat_transSetCallback(UAT_TRANS_CALLBACK_T cbFunc);
PUBLIC UINT32 uat_transGetBufferAdd(UINT8 direction);
PUBLIC INT32 uat_send_data(UINT32 len, UINT32 flag);
PUBLIC INT32 uat_recv_data(UINT32 len, UINT32 flag);
PUBLIC BOOL uat_get_send_state(VOID);
PUBLIC VOID uat_set_send_state(BOOL state);
PUBLIC BOOL usb_at_is_ready(VOID);

/// @} <- End of the uvideos group

///  @} <- End of the uvideos group

#endif // _UVIDEOS_M_H_
