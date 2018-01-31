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

#ifndef _AT_DATA_ENGINE_H_
#define _AT_DATA_ENGINE_H_

#include <stdint.h>
#include <stdbool.h>

struct AT_DATA_ENGINE_T;
typedef struct AT_DATA_ENGINE_T AT_DATA_ENGINE_T;

struct AT_DISPATCH_T;

typedef int (*AT_DATA_BYPASS_CALLBACK_T)(void *param, const uint8_t *data, unsigned length);

// =============================================================================
// at_DataEngineCreate
// -----------------------------------------------------------------------------
/// Create AT data engine.
// =============================================================================
AT_DATA_ENGINE_T *at_DataEngineCreate(struct AT_DISPATCH_T *dispatch);

// =============================================================================
// at_DataEngineDestroy
// -----------------------------------------------------------------------------
/// Destroy AT data engine.
// =============================================================================
void at_DataEngineDestroy(AT_DATA_ENGINE_T *th);

// =============================================================================
// at_DataPushData
// -----------------------------------------------------------------------------
/// Push data to data engine, called by upstream module.
///
/// @return Consumed byte count, or -1 when cmd engine is busy
// =============================================================================
int at_DataPushData(AT_DATA_ENGINE_T *th, const uint8_t *data, unsigned length);

// =============================================================================
// at_DataWrite
// -----------------------------------------------------------------------------
/// Called by downstream to write data in data mode.
// =============================================================================
void at_DataWrite(AT_DATA_ENGINE_T *th, const uint8_t *data, unsigned length);

// =============================================================================
// at_DataChannelWrite
// -----------------------------------------------------------------------------
/// Called by downstream to write data in data mode.
// =============================================================================
void at_DataChannelWrite(const uint8_t *data, unsigned length, int channel);

// =============================================================================
// at_DataSetPPPMode/at_DataSetBypassMode
// -----------------------------------------------------------------------------
/// Set and check PPP and bypass mode.
// =============================================================================
void at_DataSetPPPMode(AT_DATA_ENGINE_T *th);
void at_DataSetBypassMode(AT_DATA_ENGINE_T *th, AT_DATA_BYPASS_CALLBACK_T callback, void *param);
bool at_DataIsPPPMode(AT_DATA_ENGINE_T *th);
bool at_DataIsBypassMode(AT_DATA_ENGINE_T *th);

#endif
