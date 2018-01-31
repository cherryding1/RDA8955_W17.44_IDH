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

#ifndef __AT_MODULE_H__
#define __AT_MODULE_H__

#include "at_common.h"
#include "at_sa.h"
#include "at_cmd_engine.h"
#include "at_device.h"

#define AT_MODULE_DEVICE_MAX    (2)

// For at module initializing
// TODO: delete them, use the SA flags instead.
typedef enum AT_MODULE_INIT_STATUS_T {
    AT_MODULE_INIT_NO_SIM,
    AT_MODULE_INIT_SIM_AUTH_READY,
    AT_MODULE_INIT_SIM_AUTH_WAIT_PUK1,
    AT_MODULE_INIT_SIM_AUTH_WAIT_PIN1,
    AT_MODULE_INIT_SAT_OK,
    AT_MODULE_INIT_SAT_ERROR,
    AT_MODULE_INIT_NORMAL_SIM_CARD,
    AT_MODULE_INIT_CHANGED_SIM_CARD,
    AT_MODULE_INIT_NORMAL_CHANGED_SIM_CARD,
    AT_MODULE_INIT_TEST_SIM_CARD,
    AT_MODULE_INIT_SIM_SMS_OK,
    AT_MODULE_INIT_SIM_SMS_ERROR,
    AT_MODULE_INIT_SIM_PBK_OK,
    AT_MODULE_INIT_SIM_PBK_ERROR,
    AT_MODULE_INIT_ME_SMS_OK,
    AT_MODULE_INIT_ME_SMS_ERROR,
    AT_MODULE_INIT_ME_PBK_OK,
    AT_MODULE_INIT_ME_PBK_ERROR,
    AT_MODULE_INIT_BATT_OK,
    AT_MODULE_NOT_INIT = 0xff
} AT_MODULE_INIT_STATUS_T;

#define AT_MUXSOCKET_NUM 8
#define AT_BLUETOOTH_CHANNEL 9

typedef enum AT_MODE_SWITCH_CAUSE_T {
    AT_CMD_MODE_SWITCH_CC_RELEASE_CALL,
    AT_CMD_MODE_SWITCH_CC_SPEECH_IND,
    AT_CMD_MODE_SWITCH_CC_ACCEPT_CALL,
    AT_CMD_MODE_SWITCH_CC_INITIATE_SPEECH_CALL,
    AT_CMD_MODE_SWITCH_ATH,
    AT_MODE_SWITCH_DATA_START,
    AT_MODE_SWITCH_DATA_END,
    AT_MODE_SWITCH_DATA_ESCAPE,
    AT_MODE_SWITCH_DATA_RESUME
} AT_MODE_SWITCH_CAUSE_T;

typedef struct AT_COM_BUF
{
    UINT8 padding[2];
    UINT8 Mark; // 0 :the data in buffer have been processed;
    // Other value : the data need to be parse.
    UINT32 DataLen;                      // This variable is the length of the data in DataBuf;
    UINT8 DataBuf[AT_CMD_LINE_BUFF_LEN]; // The address of the Buffer.
} AT_CMD_BUF;

struct AT_COMMAND_T;
typedef struct AT_ModuleInfo
{
    bool firstInfoText;
    struct AT_COMMAND_T *cmds;
    UINT8 param[AT_CMD_PARA_SIZE + 1];
    AT_CMD_BUF cmd_buf;     // Command Buffer
    AT_CMD_BUF cmd_buf_bak; // Backup command buffer for supporting of A/ command.

    AT_IND *pIndList; // Indicator List
    int cmssNum;
} AT_ModuleInfo;

typedef struct
{
    CFW_EVENT noSim[NUMBER_OF_SIM]; // DBS
    CFW_EVENT sim;
    CFW_EVENT sat;
    CFW_EVENT simCard;
    CFW_EVENT sms[NUMBER_OF_SIM];
    CFW_EVENT pbk;
} CFW_INIT_INFO;

void AT_ModuleInit(void);
int at_ModuleNotifyResult(PAT_CMD_RESULT pCmdResult);
void at_ModuleRunCommand(AT_CMD_ENGINE_T *engine);
void at_ModuleRunCommandExtra(AT_CMD_ENGINE_T *engine, const char *data, unsigned length);
void at_ModuleScheduleNextCommand(AT_CMD_ENGINE_T *engine);
int at_ModuleProcessLine(UINT8 *pBuf, UINT32 len, AT_CMD_ENGINE_T *engine);
VOID at_ModuleHandleException(AT_CMD_ENGINE_T *engine, UINT32 uErrCode);

BOOL AT_SetAsyncTimerMux(AT_CMD_ENGINE_T *engine, UINT32 nElapse);
BOOL AT_KillAsyncTimerMux(AT_CMD_ENGINE_T *engine);

VOID AT_AsyncEventProcess(COS_EVENT *pEvent);
VOID uart_SendDataBTCallBack(UINT8 *pDataToSend, UINT16 data_len);

// Global API
void at_ModuleSetInitResult(AT_MODULE_INIT_STATUS_T uiStatus);
AT_MODULE_INIT_STATUS_T at_ModuleGetInitResult(VOID);
void at_ModuleSetSmsReady(bool ready);
bool at_ModuleGetSmsReady(void);
void at_ModuleSetPbkReady(bool ready);
bool at_ModuleGetPbkReady(void);

int at_ModuleGetChannelSimID(int channel);
int at_ModuleGetSimDlci(int sim);

VOID at_ModuleGetSimCid(UINT8 *pSimId, UINT8 *pCid, UINT8 nDLCI);
VOID at_ModuleHandleTimeout(AT_CMD_ENGINE_T *engine);

void at_ModuleSetCopsFlag(UINT8 sim, BOOL flag);
BOOL at_ModuleGetCopsFlag(UINT8 sim);

void at_ModuleSetRI(bool hi);
void at_ModuleSetDCD(bool hi);
void at_ModuleSetDSR(bool hi);

bool at_SetUrcChannel(int channel);
int at_GetUrcChannel(void);

#define AT_CMD_RETURN(r) \
    do                   \
    {                    \
        r;               \
        return;          \
    } while (0)

// =============================================================================
// Various response functions
// -----------------------------------------------------------------------------
/// InfoText (V.250): infomation text.
/// Final Result Codes:
/// * OK:
/// * Error:
/// * FinalCode: (BUSY/NO_CARRIER/NO_ANSWER/NO_DAILTONE)
/// * CmeError:
/// * CmsError:
/// Intermediate Result Code:
/// * IntermCode: (CONNECT)
/// * IntermText: output verbose text even in ATV0
/// URC:
/// * UrcCode: (RING)
/// * UrcText: output verbose text even in ATV0
/// URC not during command handling:
/// * ReportUrcCode:
/// * ReportUrcText:
/// Others:
/// * Output: output directly, not add header and tailer
/// * OutputPrompt: '\r\n> '
/// Customized:
/// * OKText: success without numeric code, and specified text
/// * ErrorText: fail without numeric code, and specified text
///
/// NOTE:
/// * Except InfoText, "text" in other APIs msut be not-NULL.
// =============================================================================
void at_CmdRespInfoText(AT_CMD_ENGINE_T *engine, const uint8_t *text);
void at_CmdRespInfoNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length);
void at_CmdRespOK(AT_CMD_ENGINE_T *engine);
void at_CmdRespError(AT_CMD_ENGINE_T *engine);
void at_CmdRespErrorCode(AT_CMD_ENGINE_T *engine, int rcode);
void at_CmdRespIntermCode(AT_CMD_ENGINE_T *engine, int rcode);
void at_CmdRespUrcCode(AT_CMD_ENGINE_T *engine, int code);

void at_CmdRespCmeError(AT_CMD_ENGINE_T *engine, int errcode);
void at_CmdRespCmsError(AT_CMD_ENGINE_T *engine, int errcode);
void at_CmdRespUrcText(AT_CMD_ENGINE_T *engine, const uint8_t *text);
void at_CmdRespUrcNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length);
void at_CmdRespIntermText(AT_CMD_ENGINE_T *engine, const uint8_t *text);

void at_CmdReportUrcCode(uint8_t sim, int code);
void at_CmdReportUrcText(uint8_t sim, const uint8_t *text);

void at_CmdRespOutputText(AT_CMD_ENGINE_T *engine, const uint8_t *text);
void at_CmdRespOutputNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length);
void at_CmdRespOutputPrompt(AT_CMD_ENGINE_T *engine);

void at_CmdRespOKText(AT_CMD_ENGINE_T *engine, const uint8_t *text);
void at_CmdRespErrorText(AT_CMD_ENGINE_T *engine, const uint8_t *text);

void at_CmdRingInd(uint8_t sim);

void at_CmdClearRemainCmds(AT_CMD_ENGINE_T *engine);

// The following function should use channel rather than object pointer
BOOL AT_SetDtmfTimerMux(UINT32 nElapse, UINT8 nDLCI);
BOOL AT_KillDtmfTimerMux(UINT8 nDLCI);
BOOL AT_SetDtmfTimerMuxEX(UINT32 nElapse, UINT8 nDLCI);
BOOL AT_KillDtmfTimerMuxEX(UINT8 nDLCI);
BOOL AT_SetExtensionTimerMux(UINT32 nElapse, UINT8 nDLCI);
BOOL AT_KillExtensionTimerMux(UINT8 nDLCI);
BOOL AT_SetCCEDTimer(UINT32 nElapse);
BOOL AT_KillCCEDTimer(VOID);
BOOL AT_SetOTATimer(UINT32 nElapse);
BOOL AT_KillOTATimer(VOID);
#ifdef GCF_TEST
BOOL AT_SetCgsendTimer(UINT32 nElapse);
BOOL AT_KillCgsendTimer(VOID);
#endif

BOOL AT_IsAsynCmdAvailable(UINT8 *pName, UINT32 uCmdStamp, UINT8 nDLCI);
VOID AT_Clear_CurCMD(UINT8 nDLCI);
UINT8 AT_GetCMSSNum(UINT8 nDLCI);
VOID AT_SetCMSSNum(UINT8 nDLCI, UINT8 num);
VOID at_ModuleModeSwitch(AT_MODE_SWITCH_CAUSE_T cause, UINT8 nDLCI);

VOID AT_AddWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci);
BOOL AT_isWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci);
VOID AT_DelWaitingEvent(UINT32 event, UINT8 sim, UINT8 dlci);
UINT8 AT_GetDLCIFromWaitingEvent(UINT32 event, UINT8 sim);
VOID AT_DelWaitingEventOnDLCI(UINT8 dlci);
UINT32 AT_GetWaitingEventOnDLCI(UINT8 sim, UINT8 dlci);

BOOL PM_SetBatteryChargeMode(UINT8 nMode);

void AT_WriteUart(const uint8_t *data, unsigned length);
bool at_ModuleEnableSecUart(bool enable, uint32_t baud, uint8_t format, uint8_t parity);

#endif
