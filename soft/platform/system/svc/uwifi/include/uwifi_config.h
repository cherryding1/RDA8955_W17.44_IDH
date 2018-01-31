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

#ifndef _UWIFI_CONFIG_H_
#define _UWIFI_CONFIG_H_

#include "cs_types.h"

#include "hal_usb.h"

// =============================================================================
// UWIFI_TRANSPORT_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of storage configuration
// =============================================================================
typedef struct
{
    /// EndPoint for in
    CONST UINT32 inEpNum ;
    /// EndPoint for out
    CONST UINT32 outEpNum;
} UWIFI_TRANSPORT_PARAMETERS_T;

// =============================================================================
// UVIDEOS_STREAM_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the stream
/// @param cfg Configuration for the stream layers
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*UWIFI_TRANSPORT_OPEN_CALLBACK_T)
(CONST UWIFI_TRANSPORT_PARAMETERS_T*  cfg);

// =============================================================================
// UMSS_TRANSPORT_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the transport
// =============================================================================
typedef VOID (*UWIFI_TRANSPORT_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// UWIFI_TRANSPORT_CALLBACK_T
// -----------------------------------------------------------------------------
/// Transport Callback
// =============================================================================
typedef struct
{
    /// See #UWIFI_TRANSPORT_OPEN_CALLBACK_T
    CONST UWIFI_TRANSPORT_OPEN_CALLBACK_T  open;
    /// See #UWIFI_TRANSPORT_CLOSE_CALLBACK_T
    CONST UWIFI_TRANSPORT_CLOSE_CALLBACK_T close;
} UWIFI_TRANSPORT_CALLBACK_T;

// =============================================================================
// UWIFI_TRANSPORT_CFG_T
// -----------------------------------------------------------------------------
/// Trasnport configuration
// =============================================================================
typedef struct
{


    /// transport callback
    CONST UWIFI_TRANSPORT_CALLBACK_T*   wtransportCallback;
    /// configuration parameter of transport
    CONST UWIFI_TRANSPORT_PARAMETERS_T  wtransportParameters;

} UWIFI_TRANSPORT_CFG_T;


// =============================================================================
// UWIFI_CFG_T
// -----------------------------------------------------------------------------
/// Structure for WIFI configuration
// =============================================================================
typedef struct
{
    /// Transport configuration
    CONST UWIFI_TRANSPORT_CFG_T         wtransportCfg;
} UWIFI_CFG_T;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
/// Callback of SCSI transport + Bulk Only
// =============================================================================
EXPORT PUBLIC CONST UWIFI_TRANSPORT_CALLBACK_T   g_uwifitransportCallback;

// =============================================================================

#endif // _UVIDEOS_CONFIG_H_

