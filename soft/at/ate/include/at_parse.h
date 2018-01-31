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

#ifndef _AT_PARSE_H_
#define _AT_PARSE_H_

#include <stdint.h>
#include <stdbool.h>

#define AT_COMMAND_PARAM_MAX (16)

enum AT_CMDPARAM_TYPE_T
{
    AT_CMDPARAM_EMPTY,
    AT_CMDPARAM_NUMBER,
    AT_CMDPARAM_STRING,
    AT_CMDPARAM_DSTRING,
    AT_CMDPARAM_DTMFCHAR,

    AT_CMDPARAM_STRING_PARSED, // string parameter, and parsed
    AT_CMDPARAM_DTMF_PARSED    // DTMF parameter, and parsed
};

enum AT_CMD_TYPE_T
{
    AT_CMD_SET,
    AT_CMD_TEST,
    AT_CMD_READ,
    AT_CMD_EXE
};

typedef struct AT_COMMAND_PARAM_T
{
    uint8_t type;
    uint16_t length;
    uint8_t value[1];
} AT_COMMAND_PARAM_T;

struct SA_COMMAND_TABLE_T;
typedef struct AT_COMMAND_T
{
    struct AT_COMMAND_T *next;
    const struct SA_COMMAND_TABLE_T *desc;
    uint8_t type;
    uint8_t paramCount;
    AT_COMMAND_PARAM_T *params[AT_COMMAND_PARAM_MAX];
} AT_COMMAND_T;

// =============================================================================
// at_Cmdlist
// -----------------------------------------------------------------------------
/// AT command line is parsed into a list of commands.
///
/// PushBack: Push back a cmd to list, and change head (if needed)
/// PopFront: Pop the cmd at from, and change head
/// FreeAll: Free all cmds in list, and change head to NULL
/// Free: Free one cmd
//
/// All APIs are NULL-pointer safe.
// =============================================================================
void at_CmdlistPushBack(AT_COMMAND_T **head, AT_COMMAND_T *node);
AT_COMMAND_T *at_CmdlistPopFront(AT_COMMAND_T **head);
unsigned at_CmdlistCount(AT_COMMAND_T *head);
void at_CmdlistFreeAll(AT_COMMAND_T **head);
void at_CmdlistFree(AT_COMMAND_T *cmd);

// =============================================================================
// at_CmdParam...
// -----------------------------------------------------------------------------
/// These APIs will inteprete param, with various options.
///
/// "paramok" is an in-out parameter. At input, inteprete will be bypasss if it
/// it false. At output, it will be set to false if failed.
///
/// Uint: for necessary parameter.
/// UintInRange: for necessary parameter, and should be in specified range.
/// DefUintInRange: for optional parameter. When existed, should be in range.
/// UintInList: for necessary parameter, and should be in specified list.
/// DefUintInList: for optional parameter. When existed, should be in list.
/// Str: for string parameter. \HH will be parsed (in place). The return address
///     is '\0' terminated string and start/end double quotations are removed.
///     Caller shouldn't write or free it.
/// OptStr: for optional string parameter. When not present, return NULL.
/// Dtmf: for DTMF char and DTMF string
// =============================================================================
uint32_t at_ParamUint(AT_COMMAND_PARAM_T *param, bool *paramok);
uint32_t at_ParamUintInRange(AT_COMMAND_PARAM_T *param, uint32_t minval,
                             uint32_t maxval, bool *paramok);
uint32_t at_ParamDefUintInRange(AT_COMMAND_PARAM_T *param, uint32_t defval,
                                uint32_t minval, uint32_t maxval, bool *paramok);
uint32_t at_ParamUintInList(AT_COMMAND_PARAM_T *param, const uint32_t *list,
                            unsigned count, bool *paramok);
uint32_t at_ParamDefUintInList(AT_COMMAND_PARAM_T *param, uint32_t defval,
                               const uint32_t *list, unsigned count, bool *paramok);
const uint8_t *at_ParamStr(AT_COMMAND_PARAM_T *param, bool *paramok);
const uint8_t *at_ParamOptStr(AT_COMMAND_PARAM_T *param, bool *paramok);
const uint8_t *at_ParamDtmf(AT_COMMAND_PARAM_T *param, bool *paramok);

// =============================================================================
// at_ParseLine
// -----------------------------------------------------------------------------
/// Parse a command line (start with 'AT', and end with '\r' or "\r\n").
///
/// @return 0 if success, others if fail
// =============================================================================
int at_ParseLine(const uint8_t *cmdline, unsigned length, AT_COMMAND_T **cmds);

#endif
