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




#include "hal_usb_monitor.h"
#include "uctls_m.h"
#include "uctls_callback.h"

#include "sxr_ops.h"

// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================


// =============================================================================
// PRIVATE VARIABLES
// =============================================================================


// =============================================================================
// PRIVATE FUNCTIONS
// =============================================================================

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T**
utraces_Open(CONST UCTLS_SERVICE_CFG_T* config);

PRIVATE VOID utraces_Close(VOID);

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

PUBLIC CONST UCTLS_SERVICE_CALLBACK_T g_utracesCallback =
{
    .open  =         utraces_Open,
    .close =         utraces_Close
};

// =============================================================================
// FUNCTIONS
// =============================================================================

PRIVATE HAL_USB_INTERFACE_DESCRIPTOR_T**
utraces_Open(CONST UCTLS_SERVICE_CFG_T* config)
{
    HAL_USB_INTERFACE_DESCRIPTOR_T** desc;

    desc = (HAL_USB_INTERFACE_DESCRIPTOR_T**) sxr_Malloc(sizeof(HAL_USB_INTERFACE_DESCRIPTOR_T*)*2);

    config   = config;
    desc[0]  = hal_HostUsbOpen();
    desc[1]  = 0;

    return desc;
}

PRIVATE VOID utraces_Close(VOID)
{
    hal_HostUsbClose();
}
