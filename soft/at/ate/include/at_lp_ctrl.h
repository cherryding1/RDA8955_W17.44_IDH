/* Copyright (C) 2017 RDA Technologies Limited and/or its affiliates("RDA").
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

#ifndef _AT_LP_CTRL_H_
#define _AT_LP_CTRL_H_

#include <stdbool.h>

struct AT_DEVICE_T;

typedef enum AT_LP_CTRL_MODE_T {
    AT_LP_CTRL_DISABLE,
    AT_LP_CTRL_BY_DTR,
    AT_LP_CTRL_BY_RX
} AT_LP_CTRL_MODE_T;

// =============================================================================
// at_LpCtrlInit
// -----------------------------------------------------------------------------
/// Initialize AT LP control data structure.
// =============================================================================
void at_LpCtrlInit(void);

// =============================================================================
// at_LpCtrlGetMode/at_LpCtrlSetMode
// -----------------------------------------------------------------------------
/// Get/set LP control mode.
// =============================================================================
AT_LP_CTRL_MODE_T at_LpCtrlGetMode(void);
bool at_LpCtrlSetMode(AT_LP_CTRL_MODE_T mode);

// =============================================================================
// at_LpCtrlSetDevice
// -----------------------------------------------------------------------------
/// Set AT device to ba managed by LP control.
// =============================================================================
bool at_LpCtrlSetDevice(struct AT_DEVICE_T *device);

// =============================================================================
// at_LpCtrlUnsetDevice
// -----------------------------------------------------------------------------
/// Unset AT device to ba managed by LP control.
// =============================================================================
void at_LpCtrlUnsetDevice(struct AT_DEVICE_T *device);

// =============================================================================
// at_LpCtrlInputReceived
// -----------------------------------------------------------------------------
/// Call by AT device to indicate there are input. It will postpone timer to
/// sleep AT devices at AT_LP_CTRL_BY_RX mode.
// =============================================================================
void at_LpCtrlInputReceived(void);

// =============================================================================
// at_LpCtrlOutputRequest
// -----------------------------------------------------------------------------
/// Call this when device is in sleep and want to output. This will wakeup
/// device or not based on LP mode. Caller should ensure device is in sleep.
///
/// @return true if wakeup is permitted.
// =============================================================================
bool at_LpCtrlOutputRequest(void);

#endif
