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


#ifndef __AT_CMD_SAT_H__
#define __AT_CMD_SAT_H__

#include "cfw.h"

struct COS_EVENT;

struct AT_CMD_PARA;

//BER-TLV tags in SIM to ME direction
#define PROACTIVE_SIM_COM                0xD0

//Proactive command
#define NOT_USED_COM                             0x00
#define REFRESH_COM                              0x01
#define MORE_TIME_COM                            0x02
#define POLL_INTERVAL_COM                        0x03
#define POLLING_OFF_COM                          0x04
#define SETUP_EVENT_LIST_COM                     0x05
#define SETUP_CALL_COM                           0X10
#define SEND_SS_COM                              0x11
#define SEND_USSD_COM                            0x12
#define SEND_SHORT_MESSAGE_COM                   0x13
#define SEND_DTMF_COM                            0x14
#define LAUNCH_BROWSER_C_COM                     0x15
#define PLAY_TONE_COM                            0x20
#define DISPLAY_TEXT_COM                         0x21
#define GET_INKEY_COM                            0x22
#define GET_INPUT_COM                            0x23
#define SELECT_ITEM_COM                          0x24
#define SETUP_MENU_COM                           0x25
#define PROVIDE_LOCAL_INFO_COM                   0x26
#define TIMER_MANAGEMENT_COM                     0x27
#define SETUP_IDLE_MODE_TEXT_COM                 0x28
#define PERFORM_CARD_APDU_A_COM                  0x30
#define POWER_ON_CARD_A_COM                      0x31
#define POWER_OFF_CARD_A_COM                     0x32
#define GET_READER_STATUS_A_COM                  0x33
#define RUN_AT_COMMAND_B_COM                     0x34
#define LANGUAGE_NOTIFICATION_COM                0x35
#define OPEN_CHANNEL_E_COM                       0x40
#define CLOSE_CHANNEL_E_COM                      0x41
#define RECEIVE_DATA_E_COM                       0x42
#define SEND_DATA_E_COM                          0x43
#define GET_CHANNEL_STATUS_E_COM                 0x44
#define REVERVED_COM                             0x60

#ifndef MEM_ALIGNMENT
#define MEM_ALIGNMENT                   4
#endif

#ifndef MEM_ALIGN_SIZE
#define MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))
#endif

VOID AT_SAT_AsyncEventProcess(COS_EVENT *pEvent);

VOID AT_SAT_CmdFunc_STGI(AT_CMD_PARA *pParam);
VOID AT_SAT_CmdFunc_STA(AT_CMD_PARA *pParam);
VOID AT_SAT_CmdFunc_STR(AT_CMD_PARA *pParam);
VOID AT_SAT_CmdFunc_STNR(AT_CMD_PARA *pParam);
VOID AT_SAT_CmdFunc_STRC(AT_CMD_PARA *pParam);
#ifdef AT_STSF
VOID AT_SAT_CmdFunc_STSF(AT_CMD_PARA *pParam);
#endif
UINT32 AT_SAT_Count(CFW_SAT_ITEM_LIST *pMenu);
UINT8 AT_SAT_TransmitBinary(UINT8 *pBuffer, UINT8 *pStr, UINT8 nLen);
void AT_SAT_CmdFunc_STF(AT_CMD_PARA *pParam);
UINT8 _GetTimerIdentifier(UINT8 *p);
UINT32 _GetTimerValue(UINT8 *p);
u32 sxs_ExistTimer (u32 Id);
UINT32 CFW_SimGetMeProfile(CFW_PROFILE **pMeProfile, CFW_SIM_ID nSimID);

extern UINT8 g_ss_ussdUTI;
extern UINT8 g_ss_ussdUTIflag;

VOID CFW_EmodGetIMEIBCD(UINT8 *pImei, UINT8 *pImeiLen, CFW_SIM_ID nSimID);

#ifndef SECOND
#define SECOND        * 16384
#endif
#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND

#endif
