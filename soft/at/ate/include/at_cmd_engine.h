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

#ifndef _AT_CMD_ENGINE_H_
#define _AT_CMD_ENGINE_H_

#include <stdint.h>
#include <stdbool.h>

struct AT_CMD_ENGINE_T;
typedef struct AT_CMD_ENGINE_T AT_CMD_ENGINE_T;

struct AT_DISPATCH_T;
struct AT_ModuleInfo;

typedef int (*AT_CMD_BYPASS_CALLBACK_T)(void *param, const uint8_t *data, unsigned length);

// =============================================================================
// at_CmdEngineCreate
// -----------------------------------------------------------------------------
/// Create AT cmd engine.
// =============================================================================
AT_CMD_ENGINE_T *at_CmdEngineCreate(struct AT_DISPATCH_T *dispatch);

// =============================================================================
// at_CmdEngineDestroy
// -----------------------------------------------------------------------------
/// Destroy AT cmd engine.
// =============================================================================
void at_CmdEngineDestroy(AT_CMD_ENGINE_T *th);

// =============================================================================
// at_CmdPushData
// -----------------------------------------------------------------------------
/// Push data to cmd engine, called by upstream module.
///
/// @return Consumed byte count, or -1 when cmd engine is busy
// =============================================================================
int at_CmdPushData(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length);

// =============================================================================
// at_CmdWrite
// -----------------------------------------------------------------------------
/// Write data, called by downstream module.
// =============================================================================
void at_CmdWrite(AT_CMD_ENGINE_T *th, const uint8_t *data, unsigned length);
void at_CmdChannelWrite(const uint8_t *data, unsigned length, int channel);

// =============================================================================
// at_CmdDirectWrite
// -----------------------------------------------------------------------------
/// Write data without specified engine, called by downstream module.
// =============================================================================
void at_CmdDirectWrite(const uint8_t *data, unsigned length);

// =============================================================================
// at_CmdSetPromptMode
// -----------------------------------------------------------------------------
/// Change cmd engine to prompt mode, called by downstream module.
// =============================================================================
void at_CmdSetPromptMode(AT_CMD_ENGINE_T *th);

// =============================================================================
// at_CmdDeviceSetFormatNeeded
// -----------------------------------------------------------------------------
/// After +IPR and +ICF, UART configuration should be changed after the current
/// command line. So, just set a flag.
// =============================================================================
void at_CmdDeviceSetFormatNeeded(AT_CMD_ENGINE_T *th);

// =============================================================================
// at_CmdCommandFinished
// -----------------------------------------------------------------------------
/// Callback to be called after an AT command is finished for cleanup.
// =============================================================================
void at_CmdCommandFinished(AT_CMD_ENGINE_T *th);

// =============================================================================
// at_CmdSetRawMode
// -----------------------------------------------------------------------------
/// Change cmd engine to fixed length mode, called by downstream module.
/// Return false when the requested length is too large
// =============================================================================
bool at_CmdSetRawMode(AT_CMD_ENGINE_T *th, unsigned length);

// =============================================================================
// at_CmdSetBypassMode
// -----------------------------------------------------------------------------
/// Change cmd engine to bypass mode, called by downstream module. In bypass
/// mode, the callback will be called for data from upstream.
/// Return false when the callback is NULL
// =============================================================================
void at_CmdSetBypassMode(AT_CMD_ENGINE_T *th, AT_CMD_BYPASS_CALLBACK_T callback, void *param);

// =============================================================================
// at_CmdSetLineMode
// -----------------------------------------------------------------------------
/// Command cmd engine to line mode and clear busy, called by downstream module.
// =============================================================================
void at_CmdSetLineMode(AT_CMD_ENGINE_T *th);

// =============================================================================
// (various APIs to get private variables)
// =============================================================================
bool at_CmdIsLineMode(AT_CMD_ENGINE_T *th);
bool at_CmdIsPromptMode(AT_CMD_ENGINE_T *th);
bool at_CmdIsRawMode(AT_CMD_ENGINE_T *th);
bool at_CmdIsBypassMode(AT_CMD_ENGINE_T *th);
int at_CmdGetChannel(AT_CMD_ENGINE_T *th);
struct AT_ModuleInfo *at_CmdGetModule(AT_CMD_ENGINE_T *th);
AT_CMD_ENGINE_T *at_CmdGetByChannel(int channel);

#endif
