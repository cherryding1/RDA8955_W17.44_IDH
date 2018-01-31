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

#ifndef _AT_CMUX_ENGINE_H_
#define _AT_CMUX_ENGINE_H_

#include <stdint.h>

struct AT_DISPATCH_T;

struct AT_CMUX_ENGINE_T;
typedef struct AT_CMUX_ENGINE_T AT_CMUX_ENGINE_T;

typedef struct AT_CMUX_CONFIG_T
{
    uint8_t transparency; // 0: basic, 1: advanced
    uint8_t subset;       // 0: UIH, 1: UI, 2: I
    uint8_t portSpeed;
    int maxFrameSize;
    uint8_t ackTimer; // in 10ms
    uint8_t maxRetransCount;
    uint8_t respTimer;       // in 10ms
    uint8_t wakeupRespTimer; // in second
    uint8_t windowSize;
} AT_CMUX_CONFIG_T;

extern AT_CMUX_CONFIG_T gAtCmuxDefaultCfg;

// =============================================================================
// at_CmuxEngineCreate
// -----------------------------------------------------------------------------
/// Create AT cmux engine.
// =============================================================================
AT_CMUX_ENGINE_T *at_CmuxEngineCreate(struct AT_DISPATCH_T *dispatch, AT_CMUX_CONFIG_T *cfg);

// =============================================================================
// at_CmuxEngineDestroy
// -----------------------------------------------------------------------------
/// Destroy AT cmux engine.
// =============================================================================
void at_CmuxEngineDestroy(AT_CMUX_ENGINE_T *th);

// =============================================================================
// at_CmuxPushData
// -----------------------------------------------------------------------------
/// Push data to cmux engine, called by upstream module.
///
/// @return Consumed byte count, or -1 when cmd engine is busy
// =============================================================================
int at_CmuxPushData(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length);

// =============================================================================
// at_CmuxDlcWrite
// -----------------------------------------------------------------------------
/// Write data for DLC, called by downstream module.
// =============================================================================
void at_CmuxDlcWrite(AT_CMUX_ENGINE_T *th, const uint8_t *data, unsigned length, int dcli);

// =============================================================================
// at_Cmuat_CmuxMaxFrameSizexDlcWrite
// -----------------------------------------------------------------------------
/// Return the max frame size.
// =============================================================================
int at_CmuxMaxFrameSize(AT_CMUX_ENGINE_T *th);
AT_CMUX_CONFIG_T at_CmuxGetConfig(AT_CMUX_ENGINE_T *th);
struct AT_DEVICE_T *at_CmuxGetDevice(AT_CMUX_ENGINE_T *th);

#endif
