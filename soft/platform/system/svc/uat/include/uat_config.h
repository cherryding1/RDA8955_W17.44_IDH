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

#ifndef _UAT_CONFIG_H_
#define _UAT_CONFIG_H_

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
} USBAT_STREAM_PARAMETERS_T;

// =============================================================================
// UVIDEOS_STREAM_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the stream
/// @param cfg Configuration for the stream layers
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
#ifdef _TO_ADAPT_SPRD_COM_PC_DRIVER_

typedef struct
{
    /// EndPoint for in
    CONST UINT32 inEpNum ;
    /// EndPoint for out
    CONST UINT32 outEpNum;
} USBAT_CONTROL_PARAMETERS_T;

#else

typedef struct
{
    /// EndPoint for intr ep
    CONST UINT32 intrEpNum ;
} USBAT_CONTROL_PARAMETERS_T;
#endif


typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*USBAT_CONTROL_OPEN_CALLBACK_T)
(CONST USBAT_CONTROL_PARAMETERS_T*  cfg);


// =============================================================================
// UMSS_TRANSPORT_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the transport
// =============================================================================
typedef VOID (*USBAT_CONTROL_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// UAT_TRANSPORT_CALLBACK_T
// -----------------------------------------------------------------------------
/// Transport Callback
// =============================================================================

//typedef VOID (*USBAT_CONTROL_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// UAT_TRANSPORT_CFG_T
// -----------------------------------------------------------------------------
/// Trasnport configuration
// =============================================================================

typedef struct
{
    /// See #UVIDEOS_CONTROL_OPEN_CALLBACK_T
    CONST USBAT_CONTROL_OPEN_CALLBACK_T  open;
    /// See #UVIDEOS_STREAM_CLOSE_CALLBACK_T
    CONST USBAT_CONTROL_CLOSE_CALLBACK_T close;
} USBAT_CONTROL_CALLBACK_T;



typedef struct
{
    /// control callback
    CONST USBAT_CONTROL_CALLBACK_T*   controlCallback;
    /// configuration parameter of control
    CONST USBAT_CONTROL_PARAMETERS_T  controlParameters;
} USBAT_CONTROL_CFG_T;


typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*USBAT_STREAM_OPEN_CALLBACK_T)
(CONST USBAT_STREAM_PARAMETERS_T*  cfg);

typedef VOID (*USBAT_STREAM_CLOSE_CALLBACK_T)(VOID);

typedef struct
{
    /// See #UVIDEOS_STREAM_OPEN_CALLBACK_T
    CONST USBAT_STREAM_OPEN_CALLBACK_T  open;
    /// See #UVIDEOS_STREAM_CLOSE_CALLBACK_T
    CONST USBAT_STREAM_CLOSE_CALLBACK_T close;
} USBAT_STREAM_CALLBACK_T;

typedef struct
{
    /// stream callback
    CONST USBAT_STREAM_CALLBACK_T*   streamCallback;
    /// configuration parameter of stream
    CONST USBAT_STREAM_PARAMETERS_T  streamParameters;
} USBAT_STREAM_CFG_T;


// =============================================================================
// UAT_CFG_T
// -----------------------------------------------------------------------------
/// Structure for AT configuration
// =============================================================================
typedef struct
{
    /// Transport configuration
    // CONST UAT_TRANSPORT_CFG_T         wtransportCfg;

    /// Control configuration
    CONST USBAT_CONTROL_CFG_T         controlCfg;
    /// Stream configuration
    CONST USBAT_STREAM_CFG_T         streamCfg;
} UAT_CFG_T;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

// =============================================================================
/// Callback of SCSI transport + Bulk Only
// =============================================================================
EXPORT PUBLIC CONST USBAT_STREAM_CALLBACK_T   g_uattransportCallback;
EXPORT PUBLIC CONST USBAT_CONTROL_CALLBACK_T   g_uatctrolCallback;

// =============================================================================

#endif // _UVIDEOS_CONFIG_H_

