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

#ifndef _AT_DISPATCH_H_
#define _AT_DISPATCH_H_

#include <stdint.h>
#include <stdbool.h>

struct AT_DISPATCH_T;
typedef struct AT_DISPATCH_T AT_DISPATCH_T;

struct AT_DEVICE_T;
struct AT_CMUX_ENGINE_T;
struct AT_CMD_ENGINE_T;
struct AT_DATA_ENGINE_T;
struct AT_CMUX_CONFIG_T;

// =============================================================================
// at_DispatchCreate
// -----------------------------------------------------------------------------
/// Create AT dispatch.
// =============================================================================
AT_DISPATCH_T *at_DispatchCreate(struct AT_DEVICE_T *device, struct AT_CMUX_ENGINE_T *cmuxParent, int dlci);

// =============================================================================
// at_DispatchDestroy
// -----------------------------------------------------------------------------
/// Destroy AT dispatch, and release managed resources.
// =============================================================================
void at_DispatchDestroy(AT_DISPATCH_T *th);

// =============================================================================
// at_DispatchGetByChannel
// -----------------------------------------------------------------------------
/// Get AT dispatch instance by channel ID. Existed codes can only use UINT8
/// ID to identify channels. For invalid channel ID, the first dispatch will be
/// returned.
///
/// Find: return NULL is no matching.
// =============================================================================
AT_DISPATCH_T *at_DispatchGetByChannel(int channel);
AT_DISPATCH_T *at_DispatchFindByChannel(int channel);

// =============================================================================
// at_DispatchGetChannel
// -----------------------------------------------------------------------------
/// Get AT dispatch instance channel ID.
// =============================================================================
int at_DispatchGetChannel(AT_DISPATCH_T *th);

// =============================================================================
// at_DispatchRead
// -----------------------------------------------------------------------------
/// Read data from AT device, and push to downstreams.
// =============================================================================
void at_DispatchRead(AT_DISPATCH_T *th);

// =============================================================================
// at_DispatchPushData
// -----------------------------------------------------------------------------
/// Push to downstreams. It is only used on CMUX mode.
// =============================================================================
int at_DispatchPushData(AT_DISPATCH_T *th, const uint8_t *data, unsigned length);

// =============================================================================
// at_DispatchWrite
// -----------------------------------------------------------------------------
/// Write data to underlay device. There are no cache in dispatch, all data
/// will be written or dropped. Downstream modules should check whether the
/// data should be written based on dispatch mode. After the function return,
/// writing is finished, and data memory can be freed/reused.
// =============================================================================
void at_DispatchWrite(AT_DISPATCH_T *th, const uint8_t *data, unsigned length);

// =============================================================================
// at_DispatchIsCmdMode/at_DispatchIsDataMode/at_DispatchIsCmuxMode
// -----------------------------------------------------------------------------
/// Whether AT dispatch is in cmd/data/cmux mode.
// =============================================================================
bool at_DispatchIsCmdMode(AT_DISPATCH_T *th);
bool at_DispatchIsDataMode(AT_DISPATCH_T *th);
bool at_DispatchIsCmuxMode(AT_DISPATCH_T *th);

// =============================================================================
// at_DispatchSetCmdMode/at_DispatchSetDataMode/at_DispatchSetCmuxMode
// -----------------------------------------------------------------------------
/// Set dispatch mode, and create the engine if needed.
// =============================================================================
void at_DispatchSetCmdMode(AT_DISPATCH_T *th);
void at_DispatchSetDataMode(AT_DISPATCH_T *th);
void at_DispatchSetCmuxMode(AT_DISPATCH_T *th, struct AT_CMUX_CONFIG_T *cfg);

// =============================================================================
// at_DispatchEndDataMode/at_DispatchEndCmuxMode
// -----------------------------------------------------------------------------
/// End data/cmux mode and destroy engine. NOTE: can't end current mode.
// =============================================================================
void at_DispatchEndDataMode(AT_DISPATCH_T *th);
void at_DispatchEndCmuxMode(AT_DISPATCH_T *th);

// =============================================================================
// at_DispatchGetDevice/at_DispatchGetCmdEngine/at_DispatchGetDlci
// -----------------------------------------------------------------------------
/// Get private variables
// =============================================================================
struct AT_DEVICE_T *at_DispatchGetDevice(AT_DISPATCH_T *th);
struct AT_CMD_ENGINE_T *at_DispatchGetCmdEngine(AT_DISPATCH_T *th);
struct AT_CMUX_ENGINE_T *at_DispatchGetParentCmux(AT_DISPATCH_T *th);
int at_DispatchGetDlci(AT_DISPATCH_T *th);

// =============================================================================
// channel = (devno << 6) | dlci
// NOTE: for compatibility, keep it in uint8_t
// =============================================================================
static inline uint8_t at_Channel2DeviceNum(int channel) { return (channel >> 6) & 0x3; }
static inline uint8_t at_Channel2Dlci(int channel) { return channel & 0x3f; }
static inline int at_ChannelFromDeviceDlci(uint8_t devno, uint8_t dlci) { return ((devno & 3) << 6) | dlci; }
static inline int at_ChannelFromDevice(uint8_t devno) { return at_ChannelFromDeviceDlci(devno, 0); }

#endif
