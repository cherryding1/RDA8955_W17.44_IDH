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


#include "syscmds_pm.h"
#include "syscmdsp_hdlr.h"
#include "syscmdsp_debug.h"
#include "syscmds_pm_cmd_id.h"

#include "string.h"
#include "hal_ana_gpadc.h"
#include "pmd_m.h"
#include "sxr_tls.h"
#include "sxr_tim.h"


// =============================================================================
// MACROS
// =============================================================================

#define MAXVOLT_FOR_EARPHONE_PRESSKEY           200

#define BATT_CAP_PERCENT_REPORT_DELTA_THRESH    3

#define BATT_CAP_PERCENT_REPORT_LOW_THRESH      20

#define BATT_CAP_PERCENT_REPORT_HIGH_THRESH     100


// =============================================================================
// TYPES
// =============================================================================


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE UINT8 g_syscmdsBattPercent = 0;
PRIVATE UINT8 g_syscmdsChargerStatus = 0xFF;
PRIVATE INT16 g_syscmdsBattTemperature = PMD_BAD_TEMPERATURE;
PRIVATE UINT32 g_syscmdsEarKeyGateValue = 200;
PRIVATE BOOL g_syscmdsEarKeyPressed = FALSE;
PRIVATE SYSCMDS_EAR_STATUS_HANDLER_T g_syscmdsEarStatusHandler = NULL;
PRIVATE SYSCMDS_MON_EAR_KEY_HANDLER_T g_syscmdsMonEarKeyHandler = NULL;
PRIVATE BOOL g_APBuzzyInitDone = FALSE;
PRIVATE BOOL g_APBuzzyPlaying = FALSE;

PRIVATE UINT32 g_syscmdsLEDS_Breathing_ontime = 0;
PRIVATE UINT32 g_syscmdsLEDS_Breathing_offtime = 0;

PROTECTED VOID syscmds_LEDBreathingOn_Hander(UINT32 nLED);
PROTECTED VOID syscmds_LEDBreathingOff_Hander(UINT32 nLED);

// =============================================================================
//  FUNCTIONS
// =============================================================================

PROTECTED VOID syscmds_LEDBreathingOff_Hander(UINT32 nLED)
{
    sxr_StartFunctionTimer(g_syscmdsLEDS_Breathing_offtime * HAL_MILLI_TICK1S, syscmds_LEDBreathingOn_Hander,
                           nLED, 0);
    pmd_SetLevel(PMD_LEVEL_LED0+nLED, 0);
}

PROTECTED VOID syscmds_LEDBreathingOn_Hander(UINT32 nLED)
{
    sxr_StartFunctionTimer(g_syscmdsLEDS_Breathing_ontime * HAL_MILLI_TICK1S, syscmds_LEDBreathingOff_Hander,
                           nLED, 0);
    pmd_SetLevel(PMD_LEVEL_LED0+nLED,5);
}

PROTECTED BOOL syscmds_HandlePmCmd(SYSCMDS_RECV_MSG_T *pRecvMsg)
{
    BOOL result = TRUE;

    switch (pRecvMsg->msgId)
    {
        case AP_PM_CMD_GET_ADC_VALUE:
        {
            UINT32 chan;
            UINT32 volt;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&chan, pRecvMsg->param, 4);

            SYSCMDS_ASSERT(chan < HAL_ANA_GPADC_CHAN_QTY,
                           "Invalid channel id from AP: %d", chan);

            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "AP_PM_CMD_GET_ADC_VALUE chan=%d =%d",chan, HAL_ANA_GPADC_CHAN_7);

            volt = hal_AnaGpadcGet(chan);


            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &volt, sizeof(volt));
        }
        break;
        case AP_PM_CMD_ENABLE_POWER:
        {
            UINT32 id, open;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 8);

            memcpy(&id, pRecvMsg->param, 4);
            memcpy(&open, (UINT8 *)pRecvMsg->param + 4, 4);

            SYSCMDS_ASSERT(id < PMD_POWER_ID_QTY,
                           "Invalid power id from AP: %d", id);

            pmd_EnablePower(id, open);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_SET_LEVEL:
        {
            UINT32 id, level;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 8);

            memcpy(&id, pRecvMsg->param, 4);
            memcpy(&level, (UINT8 *)pRecvMsg->param + 4, 4);

            SYSCMDS_ASSERT(id < PMD_LEVEL_ID_QTY,
                           "Invalid level id from AP: %d", id);

#ifdef AUD_3_IN_1_SPK

            if (id == PMD_LEVEL_VIBRATOR)
            {
                if(level == 1)
                {
                    aud_BypassNotchFilterReq(TRUE);
                    if(!g_APBuzzyInitDone)
                    {
                        aud_PlayBuzzy();
                        g_APBuzzyPlaying =TRUE;
                        g_APBuzzyInitDone = TRUE;
                    }
                }
                else
                {
                    if(g_APBuzzyPlaying)
                    {
                        aud_StopBuzzy();
                        g_APBuzzyPlaying = FALSE;
                    }
                    aud_BypassNotchFilterReq(FALSE);
                }
            }
            else
#endif
                pmd_SetLevel(id, level);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_SET_LDO_VOLT:
        {
        }
        break;
        case AP_PM_CMD_GET_BATT_VOLT:
        {
            UINT32 battery = pmd_GetBatteryLevel(NULL);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &battery, sizeof(battery));
        }
        break;
        case AP_PM_CMD_GET_EP_VOLT:
        {
        }
        break;
        case AP_PM_CMD_EP_STATUS:
        {
            UINT32 status;
            UINT32 gatevalue;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 8);

            memcpy(&status, pRecvMsg->param, 4);
            memcpy(&gatevalue, ((unsigned char*)pRecvMsg->param) + 4, 4);

            g_syscmdsEarKeyGateValue = gatevalue;

            if(!status)
            {
                g_syscmdsEarKeyPressed = FALSE;
            }

            if (g_syscmdsEarStatusHandler)
            {
                (*g_syscmdsEarStatusHandler)(
                    status ? PMD_EAR_STATUS_IN : PMD_EAR_STATUS_OUT);
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_MON_EP_KEY:
        {
            UINT32 start;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&start, pRecvMsg->param, 4);

            if (g_syscmdsMonEarKeyHandler)
            {
                (*g_syscmdsMonEarKeyHandler)((BOOL)start);
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_ENABLE_ADC:
        {
            UINT32 id, open;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 8);

            memcpy(&id, pRecvMsg->param, 4);
            memcpy(&open, (UINT8 *)pRecvMsg->param + 4, 4);

            SYSCMDS_ASSERT(id < HAL_ANA_GPADC_CHAN_QTY,
                           "Invalid GPADC id from AP: %d", id);

            if (open)
            {
                hal_AnaGpadcOpen(id, HAL_ANA_GPADC_ATP_122US);
            }
            else
            {
                hal_AnaGpadcClose(id);
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_GET_ADC_CALIB_VALUE:
        {
            UINT32 calibVals[2];

            hal_AnaGpadcGetCalibData(&calibVals[0], &calibVals[1]);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, calibVals, sizeof(calibVals));
        }
        break;
        case AP_PM_CMD_ENABLE_CHARGER:
        {
            UINT32 enable;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&enable, pRecvMsg->param, 4);

            pmd_EnableCharger(enable);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_ENABLE_CHARGER_CURRENT:
        {
            UINT32 value;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&value, pRecvMsg->param, 4);

            pmd_EnableChargeCurrentDynamic(value == 0 ? FALSE : TRUE);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_SET_CHARGER_CURRENT:
        {
            UINT32 value;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&value, pRecvMsg->param, 4);

            pmd_SetChargeCurrentDynamic(value);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_GET_CHARGER_CURRENT:
        {
            UINT32 currentVals;
#ifdef PMD_CHARGE_CURRNET_CONTROL
            currentVals = pmd_GetChargerMeasuredCurrent();
#endif
            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &currentVals, sizeof(currentVals));

        }
        break;

        case AP_PM_CMD_SET_VOLT_PCT_MIN:
        {
            UINT32 value;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&value, pRecvMsg->param, 4);

            pmd_SetChargeVoltPctMin(value);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_SET_VOLT_PCT_MAX:
        {
            UINT32 value;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&value, pRecvMsg->param, 4);

            pmd_SetChargeVoltPctMax(value);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_PM_CMD_RESTART_SECOND_PAD_POWER:
        {
            pmd_EnableSecondPadPower(FALSE);
            sxr_Sleep(20 MS_WAITING);
            pmd_EnableSecondPadPower(TRUE);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;

#if ((CHIP_ASIC_ID == CHIP_ASIC_ID_8810) ||(CHIP_ASIC_ID == CHIP_ASIC_ID_8810E)||(CHIP_ASIC_ID == CHIP_ASIC_ID_8850E))
        case AP_PM_CMD_CHARGER_TYPE:
        {
            PMD_CHARGER_TYPE_T type;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&type, pRecvMsg->param, 4);

            pmd_SetChargerType(type);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
#endif

        case AP_PM_CMD_GET_CHARGER_STATUS:
        {
            UINT32 chrStatus = g_syscmdsChargerStatus;

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &chrStatus, sizeof(chrStatus));
        }
        break;
        case AP_PM_CMD_GET_CHARGER_VOLT:
        {
            UINT32 chrVolt = pmd_GetChargerLevel();

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &chrVolt, sizeof(chrVolt));
        }
        break;
        case AP_PM_CMD_NOTIFY_VPU_STATUS:
        {
            INT32 status;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);
            memcpy(&status, pRecvMsg->param, 4);
            pmd_SaveVpuStatsu(status);
            syscmds_Reply(2, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;


        case AP_PM_CMD_SET_LEDS_BREATHING_ON:
        {
            UINT32   led_ID;
            UINT32 on_time, off_time;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 12);

            memcpy(&led_ID, pRecvMsg->param, 4);
            memcpy(&g_syscmdsLEDS_Breathing_ontime, pRecvMsg->param+4, 4);
            memcpy(&g_syscmdsLEDS_Breathing_offtime, pRecvMsg->param+8, 4);

            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "AP_PM_CMD_SET_LEDS_BREATHING_ON led_ID=%d =%d =%d",led_ID, g_syscmdsLEDS_Breathing_ontime,g_syscmdsLEDS_Breathing_offtime);

            pmd_SetLevel(PMD_LEVEL_LED0+ led_ID, 5);
            sxr_StartFunctionTimer(g_syscmdsLEDS_Breathing_ontime * HAL_MILLI_TICK1S, syscmds_LEDBreathingOff_Hander,
                                   led_ID, 0);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;

        case AP_PM_CMD_SET_LEDS_BREATHING_OFF:
        {
            UINT32  led_ID;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, 4);

            memcpy(&led_ID, pRecvMsg->param, 4);
            SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0, "AP_PM_CMD_SET_LEDS_BREATHING_OFFled_ID=%d",led_ID);

            pmd_SetLevel(PMD_LEVEL_LED0+led_ID, 0);
            sxr_StopFunctionTimer(syscmds_LEDBreathingOn_Hander);
            sxr_StopFunctionTimer(syscmds_LEDBreathingOff_Hander);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;

        default:
            result = FALSE;
            break;
    }

    return result;
}

PUBLIC VOID syscmds_NotifyBatteryStatus(UINT8 percent, UINT8 status, UINT16 level)
{
    BOOL send = FALSE;
    INT16 tp;
    UINT32 value[2];

    tp = pmd_GetBatteryTemperature();

    if (g_syscmdsChargerStatus != status)
    {
        send = TRUE;
    }
    else if (pmd_GetHighActivity())
    {
        if (percent != g_syscmdsBattPercent || tp != g_syscmdsBattTemperature)
        {
            send = TRUE;
        }
    }
    else
    {
        if (ABS(percent - g_syscmdsBattPercent) >= BATT_CAP_PERCENT_REPORT_DELTA_THRESH ||
                (percent != g_syscmdsBattPercent &&
                 (percent >= BATT_CAP_PERCENT_REPORT_HIGH_THRESH ||
                  g_syscmdsBattPercent >= BATT_CAP_PERCENT_REPORT_HIGH_THRESH ||
                  percent <= BATT_CAP_PERCENT_REPORT_LOW_THRESH ||
                  g_syscmdsBattPercent <= BATT_CAP_PERCENT_REPORT_LOW_THRESH
                 )
                )
           )
        {
            send = TRUE;
        }
    }

    if (send)
    {
        SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,
                      "syscmds_NotifyBatteryStatus: status=%d, %dmV, %d%%, tp=%d",
                      status, level, percent, tp);

        g_syscmdsBattPercent = percent;
        g_syscmdsChargerStatus = status;
        g_syscmdsBattTemperature = tp;

        value[0] = (percent & 0xFF) | ((status & 0xFF) << 8) | ((level & 0xFFFF) << 16);
        value[1] = (pmd_GetChargerLineState() & 0xFF) | ((tp & 0xFFFF) << 16);
        syscmds_IrqSend(SYSCMDS_MOD_ID_PM, MODEM_PM_CMD_BATT_STATUS, &value[0], sizeof(value));
    }
}


PUBLIC VOID syscmds_NotifyChipTemplaterStatus( UINT16 bHigetTemp, UINT16 nTemplater)
{

    UINT16 value[2];
    value[0] = nTemplater;
    value[1] = bHigetTemp;
    syscmds_IrqSend(SYSCMDS_MOD_ID_PM, MODEM_PM_CMD_CHIPTEMPER_STATUS, &value[0], sizeof(value));
}

PUBLIC VOID syscmds_NotifyBattTemplaterStatus( UINT16 bHigetTemp, UINT16 nTemplater)
{
    UINT16 value[2];
    value[0] = nTemplater;
    value[1] = bHigetTemp;
    syscmds_IrqSend(SYSCMDS_MOD_ID_PM,MODEM_PM_CMD_BATTTEMPER_STATUS,  &value[0], sizeof(value));
}


PUBLIC VOID syscmds_NotifyEarPieceStatus(UINT32 on)
{
    syscmds_IrqSend(SYSCMDS_MOD_ID_PM, MODEM_PM_CMD_EP_STATUS, &on, sizeof(on));
}

#ifdef FM_LININ_AUXMIC
extern BOOL g_syscmds_FM_Playing ;
#endif

PUBLIC VOID syscmds_NotifyEarPieceKeyStatus(UINT32 gpadc)
{
    BOOL sendMsg = FALSE;
    UINT32 down;

    if (g_syscmdsEarKeyGateValue >= gpadc)
    {
        if (!g_syscmdsEarKeyPressed)
        {
            sendMsg = TRUE;
            g_syscmdsEarKeyPressed = TRUE;
        }
    }
    else
    {
        // Previous key status is down
        if (g_syscmdsEarKeyPressed)
        {
            sendMsg = TRUE;
            g_syscmdsEarKeyPressed = FALSE;
        }
    }

#ifdef FM_LININ_AUXMIC
    if(g_syscmds_FM_Playing) sendMsg=FALSE;
#endif

    if (sendMsg)
    {
        down = g_syscmdsEarKeyPressed;
        syscmds_IrqSend(SYSCMDS_MOD_ID_PM, MODEM_PM_CMD_EP_KEY_STATUS, &gpadc, sizeof(gpadc));
    }
}

PUBLIC VOID syscmds_SetEarPieceStatusHandler(SYSCMDS_EAR_STATUS_HANDLER_T handler)
{
    g_syscmdsEarStatusHandler = handler;
}

PUBLIC VOID syscmds_SetMonEarPieceKeyHandler(SYSCMDS_MON_EAR_KEY_HANDLER_T handler)
{
    g_syscmdsMonEarKeyHandler = handler;
}

