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

#ifndef _URNDIS_CONFIG_H_
#define _URNDIS_CONFIG_H_

#include "cs_types.h"

#include "hal_usb.h"

// =============================================================================
// URNDIS_CONTROL_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of control configuration
// =============================================================================
typedef struct
{
    /// EndPoint for intr ep
    CONST UINT32 intrEpNum ;
} URNDIS_CONTROL_PARAMETERS_T;
// =============================================================================
// URNDIS_CONTROL_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the control
/// @param cfg Configuration for the control layers
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*URNDIS_CONTROL_OPEN_CALLBACK_T)
(CONST URNDIS_CONTROL_PARAMETERS_T*  cfg);
// =============================================================================
// URNDIS_DATA_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the control
// =============================================================================
typedef VOID (*URNDIS_CONTROL_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// URNDIS_CONTROL_CALLBACK_T
// -----------------------------------------------------------------------------
/// Data Callback
// =============================================================================

typedef struct
{
    /// See #URNDIS_CONTROL_OPEN_CALLBACK_T
    CONST URNDIS_CONTROL_OPEN_CALLBACK_T  open;
    /// See #URNDIS_DATA_CLOSE_CALLBACK_T
    CONST URNDIS_CONTROL_CLOSE_CALLBACK_T close;
} URNDIS_CONTROL_CALLBACK_T;

// =============================================================================
// URNDIS_CONTROL_CFG_T
// -----------------------------------------------------------------------------
/// Control configuration
// =============================================================================
typedef struct
{
    /// control callback
    CONST URNDIS_CONTROL_CALLBACK_T*   controlCallback;
    /// configuration parameter of control
    CONST URNDIS_CONTROL_PARAMETERS_T  controlParameters;
} URNDIS_CONTROL_CFG_T;

// =============================================================================
// URNDIS_DATA_PARAMETERS_T
// -----------------------------------------------------------------------------
/// Parameter of data configuration
// =============================================================================
typedef struct
{
    /// EndPoint for in
    CONST UINT32 inEpNum ;
    /// EndPoint for out
    CONST UINT32 outEpNum;
} URNDIS_DATA_PARAMETERS_T;

// =============================================================================
// URNDIS_DATA_OPEN_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for open the data
/// @param cfg Configuration for the data layers
/// @return Usb interface (must be allocated with sxr_Malloc())
// =============================================================================
typedef HAL_USB_INTERFACE_DESCRIPTOR_T* (*URNDIS_DATA_OPEN_CALLBACK_T)
(CONST URNDIS_DATA_PARAMETERS_T*  cfg);


// =============================================================================
// URNDIS_DATA_CLOSE_CALLBACK_T
// -----------------------------------------------------------------------------
/// Callback call for close the data
// =============================================================================
typedef VOID (*URNDIS_DATA_CLOSE_CALLBACK_T)(VOID);

// =============================================================================
// URNDIS_DATA_CALLBACK_T
// -----------------------------------------------------------------------------
/// Data Callback
// =============================================================================

typedef struct
{
    /// See #URNDIS_DATA_OPEN_CALLBACK_T
    CONST URNDIS_DATA_OPEN_CALLBACK_T  open;
    /// See #URNDIS_DATA_CLOSE_CALLBACK_T
    CONST URNDIS_DATA_CLOSE_CALLBACK_T close;
} URNDIS_DATA_CALLBACK_T;

// =============================================================================
// URNDIS_DATA_CFG_T
// -----------------------------------------------------------------------------
/// Data configuration
// =============================================================================
typedef struct
{
    /// data callback
    CONST URNDIS_DATA_CALLBACK_T*   dataCallback;
    /// configuration parameter of data
    CONST URNDIS_DATA_PARAMETERS_T  dataParameters;
} URNDIS_DATA_CFG_T;

// =============================================================================
// URNDIS_CFG_T
// -----------------------------------------------------------------------------
/// Structure for rndis configuration
// =============================================================================
typedef struct
{
    /// IAD
    CONST HAL_USB_INTERFACE_ASSOCIATION_DESCRIPTOR_REAL_T* iad;
    /// Control configuration
    CONST URNDIS_CONTROL_CFG_T         controlCfg;
    /// Data configuration
    CONST URNDIS_DATA_CFG_T         dataCfg;
} URNDIS_CFG_T;

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================
// =============================================================================
// g_urndisControlCallback
// -----------------------------------------------------------------------------
/// Callback of RNDIS Control
// =============================================================================
EXPORT PUBLIC CONST URNDIS_CONTROL_CALLBACK_T   g_urndisControlCallback;
// =============================================================================
// g_urndisDataCallback
// -----------------------------------------------------------------------------
/// Callback of RNDIS Data
// =============================================================================
EXPORT PUBLIC CONST URNDIS_DATA_CALLBACK_T   g_urndisDataCallback;

#endif // _URNDIS_CONFIG_H_

