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

#ifndef _UVIDEOS_CONFIG_H_
#define _UVIDEOS_CONFIG_H_

#include "cs_types.h"

#include "hal_usb.h"

// =============================================================================
// UVIDEOS_CONTROL_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of control configuration
// =============================================================================
typedef struct
{
    /// EndPoint for intr ep
    CONST UINT32 intrEpNum ;
} UVIDEOS_CONTROL_PARAMETERS_T;
// =============================================================================
// UVIDEOS_CONTROL_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the control
/// @param cfg Configuration for the control layers
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*UVIDEOS_CONTROL_OPEN_CALLBACK_T)
(CONST UVIDEOS_CONTROL_PARAMETERS_T*  cfg);
// =============================================================================
// UVIDEOS_STREAM_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the control
// =============================================================================
typedef VOID (*UVIDEOS_CONTROL_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// UVIDEOS_CONTROL_CALLBACK_T
// -----------------------------------------------------------------------------
/// Stream Callback
// =============================================================================

typedef struct
{
    /// See #UVIDEOS_CONTROL_OPEN_CALLBACK_T
    CONST UVIDEOS_CONTROL_OPEN_CALLBACK_T  open;
    /// See #UVIDEOS_STREAM_CLOSE_CALLBACK_T
    CONST UVIDEOS_CONTROL_CLOSE_CALLBACK_T close;
} UVIDEOS_CONTROL_CALLBACK_T;

// =============================================================================
// UVIDEOS_CONTROL_CFG_T
// -----------------------------------------------------------------------------
/// Control configuration
// =============================================================================
typedef struct
{
    /// control callback
    CONST UVIDEOS_CONTROL_CALLBACK_T*   controlCallback;
    /// configuration parameter of control
    CONST UVIDEOS_CONTROL_PARAMETERS_T  controlParameters;
} UVIDEOS_CONTROL_CFG_T;

// =============================================================================
// UVIDEOS_STREAM_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of stream configuration
// =============================================================================
typedef struct
{
    /// EndPoint for in
    CONST UINT32 inEpNum ;
} UVIDEOS_STREAM_PARAMETERS_T;

// =============================================================================
// UVIDEOS_STREAM_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the stream
/// @param cfg Configuration for the stream layers
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*UVIDEOS_STREAM_OPEN_CALLBACK_T)
(CONST UVIDEOS_STREAM_PARAMETERS_T*  cfg);


// =============================================================================
// UVIDEOS_STREAM_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the stream
// =============================================================================
typedef VOID (*UVIDEOS_STREAM_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// UVIDEOS_STREAM_CALLBACK_T
// -----------------------------------------------------------------------------
/// Stream Callback
// =============================================================================

typedef struct
{
    /// See #UVIDEOS_STREAM_OPEN_CALLBACK_T
    CONST UVIDEOS_STREAM_OPEN_CALLBACK_T  open;
    /// See #UVIDEOS_STREAM_CLOSE_CALLBACK_T
    CONST UVIDEOS_STREAM_CLOSE_CALLBACK_T close;
} UVIDEOS_STREAM_CALLBACK_T;

// =============================================================================
// UVIDEOS_STREAM_CFG_T
// -----------------------------------------------------------------------------
/// Stream configuration
// =============================================================================
typedef struct
{
    /// stream callback
    CONST UVIDEOS_STREAM_CALLBACK_T*   streamCallback;
    /// configuration parameter of stream
    CONST UVIDEOS_STREAM_PARAMETERS_T  streamParameters;
} UVIDEOS_STREAM_CFG_T;

// =============================================================================
// UVIDEOS_CFG_T
// -----------------------------------------------------------------------------
/// Structure for videos configuration
// =============================================================================
typedef struct
{
    /// IAD
    CONST HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T* iad;
    /// Control configuration
    CONST UVIDEOS_CONTROL_CFG_T         controlCfg;
    /// Stream configuration
    CONST UVIDEOS_STREAM_CFG_T         streamCfg;
} UVIDEOS_CFG_T;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
// =============================================================================
// g_uvideosStreamCallback
// -----------------------------------------------------------------------------
/// Callback of Video Stream
// =============================================================================
EXPORT PUBLIC CONST UVIDEOS_CONTROL_CALLBACK_T   g_uvideosControlCallback;
// =============================================================================
// g_uvideosStreamCallback
// -----------------------------------------------------------------------------
/// Callback of Video Stream
// =============================================================================
EXPORT PUBLIC CONST UVIDEOS_STREAM_CALLBACK_T   g_uvideosStreamCallback;

#endif // _UVIDEOS_CONFIG_H_

