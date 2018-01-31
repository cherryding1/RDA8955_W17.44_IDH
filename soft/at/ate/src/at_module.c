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

#include "at.h"
#include "at_sa.h"
#include "at_ta.h"
#include "at_cmd_special_battery.h"
#include "at_cmd_gc.h"
#include "at_module.h"
#include "at_device.h"
#include "at_dispatch.h"
#include "at_parse.h"
#include "at_data_engine.h"
#include "at_lp_ctrl.h"
#include "at_device_uart.h"
#include "at_device_uat.h"
#include "at_utils.h"
#include "at_errinfo.h"
#include "dm_audio.h"
#include "vds_cache.h"
#include "sockets.h"
#include "hal_uart.h"

#include "at_cfg.h"
#include "sxs_io.h"
#include "hal_gpio.h"
#include "at_cmd_sms.h"
#include "at_cmd_pbk.h"
#include "at_cmd_nw.h"
#include "at_cmd_cc.h"
#include "at_cmd_ss.h"
#include "at_cmd_sim.h"
#include "at_cmd_id.h"
#include "at_cmd_sat.h"
#include "at_cmd_gprs.h"
#include "at_cmd_special_emod.h"
#include "at_cmd_special_audio.h"
#include "at_cmd_special_wifi.h"
#ifdef LTE_NBIOT_SUPPORT
#include "at_cmd_iperf.h"
#endif

#define MAXVOLT_EARPHONE_IN_PHONE 1800
#define MINVOLT_EARPHONE_IN_PHONE 300      // Fit for 8809
#define MINVOLT_EARPHONE_NOT_IN_PHONE 1800 // 如果耳机没插在手机上，电平值最小值

typedef enum {
    AT_SIM_ABSENT = 0x00,
    AT_SIM_NORMAL = 0x01,
    AT_SIM_TEST = 0x02,
    AT_SIM_ABNORMAL = 0x03,
    AT_SIM_STATUS_END = 0x04
} AT_SIM_STATUS;

typedef struct AT_GLOBAL_T
{
    AT_MODULE_INIT_STATUS_T uiModuleInitStatus;
    bool smsReady;
    bool pbkReady;
    bool copsFlag[CFW_SIM_COUNT];
    bool setComFlag[CFW_SIM_COUNT];
    bool powerCommFlag[CFW_SIM_COUNT];
    int urcChannel;
    HAL_GPIO_GPIO_ID_T riGpio;
    HAL_GPIO_GPIO_ID_T dcdGpio;
    HAL_GPIO_GPIO_ID_T dsrGpio;
    AT_DEVICE_T *devices[AT_MODULE_DEVICE_MAX];
} AT_GLOBAL_T;

static AT_GLOBAL_T gAtGlobal = {
    .uiModuleInitStatus = AT_MODULE_NOT_INIT,
    .smsReady = false,
    .pbkReady = false,
    .copsFlag = {[0 ...(CFW_SIM_COUNT - 1)] = false},
    .setComFlag = {[0 ...(CFW_SIM_COUNT - 1)] = false},
    .powerCommFlag = {[0 ...(CFW_SIM_COUNT - 1)] = false},
    .urcChannel = 0,
    .riGpio = HAL_GPIO_NONE,
    .dcdGpio = HAL_GPIO_NONE,
    .dsrGpio = HAL_GPIO_NONE,
    .devices = {[0 ...(AT_MODULE_DEVICE_MAX - 1)] = NULL},
};

CFW_INIT_INFO cfwInitInfo;
BOOL gbPowerCommMsgFlag[CFW_SIM_COUNT] = {FALSE, FALSE};

extern VOID CFW_SetSATIndFormate(BOOL bMode);                // cfw_sim_sat_pro.c
extern UINT8 CFW_GetSimStatus(CFW_SIM_ID nSimID);            // cfw_sim_boot_pro.c
extern UINT32 CFW_SmsMoInit(UINT16 nUti, CFW_SIM_ID nSimID); // cfw_sms_main.c
extern VOID AT_AUDIO_AsyncEventProcess(COS_EVENT *pEvent);
extern VOID AT_CALLREC_AsyncEventProcess(COS_EVENT *pEvent);
extern VOID AT_WIFI_AsyncEventProcess(COS_EVENT *pEvent);
extern VOID AT_MQTT_AsyncEventProcess(COS_EVENT *pEvent);
extern VOID AT_LWM2M_AsyncEventProcess(COS_EVENT *pEvent);
extern UINT8 AT_GPRS_Get_CGSEND_Channel(void); // at_cmd_gprs.c
extern VOID AT_GPRS_CGSEND_Stop(void);         // at_cmd_gprs.c
extern void cipSettings_Init();

static void at_ModuleClearCmdList(AT_CMD_ENGINE_T *engine);
static void at_ModuleClearCurCmd(AT_CMD_ENGINE_T *engine);

#ifdef MMI_ONLY_AT
void lcd_present_CS_log(void)
{
    return;
}
#endif

static void at_ModuleCreateDefaultDevice(void)
{
#ifdef AT_USE_USB_TRAS_DATA
    AT_DEVICE_T *device = at_UatCreate(0);
#else
    AT_DEVICE_UART_CONFIG_T uartcfg;
    uartcfg.id = AT_DEFAULT_UART;
    uartcfg.baud = gATCurrentu32nBaudRate;
    uartcfg.format = AT_DEVICE_FORMAT_8N1;
    uartcfg.parity = AT_DEVICE_PARITY_SPACE;
    uartcfg.ctsEnabled = false;
    uartcfg.rtsEnabled = false;
    uartcfg.autoBaudLC = false;
    AT_DEVICE_T *device = at_UartCreate(&uartcfg, 0);
#endif

    gAtGlobal.devices[0] = device;
    AT_DISPATCH_T *dispatch = at_DispatchCreate(device, NULL, 0);
    at_DeviceSetDispatch(device, dispatch);

    at_DeviceOpen(device);
    at_LpCtrlSetDevice(device);
}

static void at_ModuleToggleGpio(HAL_GPIO_GPIO_ID_T gpio, bool hi)
{
    if (gpio == HAL_GPIO_NONE)
        return;

    if (hi)
        hal_GpioSet((HAL_APO_ID_T)gpio);
    else
        hal_GpioClr((HAL_APO_ID_T)gpio);
}

static void at_ModuleSetGpioOutput(HAL_GPIO_GPIO_ID_T gpio, bool hi)
{
    HAL_GPIO_CFG_T cfg;
    cfg.direction = HAL_GPIO_DIRECTION_OUTPUT;
    cfg.value = hi;
    cfg.irqMask = (HAL_GPIO_IRQ_MASK_T){};
    cfg.irqHandler = NULL;
    cfg.irqParam = 0;
    hal_GpioOpen(gpio, &cfg);
}

static void at_ModuleStartSecUart(void)
{
#ifdef AT_SECOND_UART_SUPPORT
    AT_DEVICE_UART_CONFIG_T uartcfg;
    uartcfg.id = AT_SECOND_UART;
    uartcfg.baud = gAtCurrentSecUartBaud;
    uartcfg.format = gAtCurrentSecUartIcfFormat;
    uartcfg.parity = gAtCurrentSecUartIcfParity;
    uartcfg.ctsEnabled = false;
    uartcfg.rtsEnabled = false;
    uartcfg.autoBaudLC = false;
    AT_DEVICE_T *device = at_UartCreate(&uartcfg, 1);

    gAtGlobal.devices[1] = device;
    AT_DISPATCH_T *dispatch = at_DispatchCreate(device, NULL, 0);
    at_DeviceSetDispatch(device, dispatch);

    at_DeviceOpen(device);
    at_LpCtrlSetDevice(device);
#endif
}

static void at_ModuleStopSecUart(AT_DEVICE_T *device)
{
    at_LpCtrlUnsetDevice(device);
    AT_DISPATCH_T *dispatch = at_DeviceGetDispatch(device);
    at_DispatchDestroy(dispatch);
    at_DeviceDestroy(device);
    gAtGlobal.devices[1] = NULL;
}

bool at_ModuleEnableSecUart(bool enable, uint32_t baud, uint8_t format, uint8_t parity)
{
    gATCurrentSecUartEnable = enable;
    gAtCurrentSecUartBaud = baud;
    gAtCurrentSecUartIcfFormat = format;
    gAtCurrentSecUartIcfParity = parity;

    AT_DEVICE_T *device = gAtGlobal.devices[1];
    if (enable)
    {
        if (device == NULL)
            at_ModuleStartSecUart();
        else
            at_DeviceSetFormat(device, baud, format, parity);
    }
    else if (device != NULL)
    {
        at_ModuleStopSecUart(device);
    }
    return true;
}

VOID AT_ModuleInit(VOID)
{
    ATLOGI("AT_ModuleInit");

    at_ModuleSetGpioOutput(gAtGlobal.riGpio, false);
    at_ModuleSetGpioOutput(gAtGlobal.dcdGpio, false);
    at_ModuleSetGpioOutput(gAtGlobal.dsrGpio, false);

#ifdef AT_USER_DBS
    CFW_CfgSetNwStatus(0, 1);
#else
    CFW_CfgSetNwStatus(1, 1);
#endif
    CFW_CfgSetNwStatus(1, 0);
    CFW_CfgSetCharacterSet(CFW_CHSET_GSM);

    at_CfgInit();
    BOOL bCfg = at_CfgGetAtSettings(USER_SETTING_1, 0);
    ATLOGI("at_CfgGetAtSettings 0: %d", bCfg);
    ATLOGI("gATCurrentu8nBaudRate: %d", gATCurrentu32nBaudRate);

    at_LpCtrlInit();
    at_ModuleCreateDefaultDevice();

#ifdef AT_SECOND_UART_SUPPORT
    if (gATCurrentSecUartEnable)
        at_ModuleStartSecUart();
#endif

    AT_SPECIAL_Battery_Init(); // maybe init at event loop

    if (!AT_GC_Init())
    {
        ATLOGE("Initialize GC module failed, system shutdown automatically");
        AT_ASSERT(0);
    }
#ifdef CFW_GPRS_SUPPORT
    cipSettings_Init();
#endif
// After the initialization of whole AT modem, need to send a hint to TE, as 'AT Ready'.
#ifdef CHIP_HAS_AP
    CFW_SetSATIndFormate(0);
#else
    CFW_SetSATIndFormate(1);
#endif

#if 0
    const char *readyMsg = "\r\nAT Ready\r\n\r\nAST_POWERON\r\n";
    at_DispatchWrite(dispatch, readyMsg, strlen(readyMsg));
#endif

#ifdef AT_MODULE_SUPPORT_OTA
    jMms_comm_init();
    for (int i = 0; i < NUMBER_OF_SIM; i++)
    {
        AT_GetApnCfg(i);
    }
#endif
}

#ifdef CHIP_HAS_AP
static BOOL AT_SmsMoInit(UINT32 nUTI, CFW_SIM_ID nSimID)
{
    UINT32 nMOUTI = 0x00;

    if (0x00 == (nMOUTI = AT_AllocUserTransID()))
    {
        AT_TC(g_sw_SA, "AT_AllocUserTransID Error");
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS MO Init: malloc UTI error", nUTI);
        return FALSE;
    }

    UINT32 nRet = CFW_SmsReadMessage(CFW_SMS_STORAGE_SM, CFW_SMS_TYPE_PP, 1, nMOUTI, nSimID);

    if (ERR_SUCCESS != nRet)
    {
        AT_TC(g_sw_SA, "CFW_SmsReadMessage Error 0x%x", nRet);
        AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "SMS MO Init: CFW_SmsReadMessage error", nUTI);
        return FALSE;
    }
    return TRUE;
}
#endif

static BOOL AT_CFW_InitProcess(COS_EVENT *pEvent)
{
    CFW_EVENT cfw_event;
    UINT8 nSim;

    AT_MemSet(&cfw_event, 0, SIZEOF(cfw_event));
    AT_CosEvent2CfwEvent(pEvent, &cfw_event);
    AT_FREE((PVOID)pEvent);
    nSim = cfw_event.nFlag;
    cfw_event.nUTI = AT_ASYN_GET_DLCI(nSim);

#ifdef LTE_NBIOT_SUPPORT
    VDS_CacheFlushAllowed();
#endif

    if (cfw_event.nEventId == EV_CFW_INIT_IND)
    {
// //////////////////////////////////////////
// Report a URC (just for MAL)
#if (MAL_EXTENDED == 1)
        uint8_t rsp[32];
        AT_Sprintf(rsp, "^CINIT: %u, %u, %u", cfw_event.nType, cfw_event.nParam1, cfw_event.nParam2);
        at_CmdReportUrcText(nSim, rsp);
#endif
        if (g_gc_pwroff)
        {
            g_gc_pwroff = 0; // hameina[+]2007.11.15 support CFUN
            AT_Result_OK(CMD_FUNC_SUCC, CMD_RC_OK, 0, 0, 0, cfw_event.nUTI, nSim);
            AT_ZERO_PARAM1(pEvent);
            return TRUE;
        }

        // g_cfg_cfun = 5;//switch on was called
        AT_TC(g_sw_SA, "EV_CFW_INIT_IND %d %d", cfw_event.nType, cfw_event.nParam1);

        switch (cfw_event.nType)
        {
        case CFW_INIT_STATUS_NO_SIM: //
        {
#if defined(USER_DATA_CACHE_SUPPORT)
            VDS_CacheFlushAllowed();
#endif
            at_ModuleSetInitResult(AT_MODULE_INIT_NO_SIM);
            AT_MemCpy(&(cfwInitInfo.noSim[nSim]), &cfw_event, sizeof(cfw_event));

            UINT8 nSimStatus = AT_SIM_ABSENT; //CFW_SIM_ABSENT
            UINT8 nOtherSimID = 0;
            UINT16 nUti = 0;

            if (FALSE == gAtGlobal.setComFlag[nSim])
            {
                if (CFW_SIM_0 == nSim)
                {
                    nOtherSimID = CFW_SIM_1;
                }
                else
                {
                    nOtherSimID = CFW_SIM_0;
                }

                nSimStatus = (UINT8)CFW_GetSimStatus(nOtherSimID);
                hal_HstSendEvent(0x03140000);
                hal_HstSendEvent(nSimStatus);
                if (AT_SIM_ABSENT == nSimStatus)
                {
                    hal_HstSendEvent(0x03140001);
                    gAtGlobal.setComFlag[nSim] = TRUE;
                    CFW_GetFreeUTI(0, (UINT8 *)&nUti);
                    CFW_SetComm(CFW_DISABLE_COMM, 1, nUti, CFW_SIM_1);
                    gAtGlobal.powerCommFlag[CFW_SIM_1] = TRUE;
                    gbPowerCommMsgFlag[CFW_SIM_1] = TRUE;
                }
                else if (AT_SIM_STATUS_END != nSimStatus)
                {
                    gAtGlobal.setComFlag[nSim] = TRUE;
                    CFW_GetFreeUTI(0, (UINT8 *)&nUti);
                    CFW_SetComm(CFW_DISABLE_COMM, 1, nUti, nSim);
                    gAtGlobal.powerCommFlag[nSim] = TRUE;
                    gbPowerCommMsgFlag[nSim] = TRUE;
                }
                else
                {
                    hal_HstSendEvent(0xF1000000 | nSimStatus);
                }
            }
            break;
        }
        case CFW_INIT_STATUS_SIM_DROP:
        {
            AT_TC(g_sw_AT_SIM, "Received CFW_INIT_STATUS_SIM_DROP in AT(nSim = %d)", nSim);
            // AT_CC_Result_OK(CSW_IND_NOTIFICATION, CMD_RC_SIMDROP, 0, 0, 0, cfw_event.nUTI, nSim);
        }
        case CFW_INIT_STATUS_SIM:
            if (cfw_event.nParam1 & CFW_INIT_SIM_PIN1_READY)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_AUTH_READY);
            }

            if (cfw_event.nParam1 & CFW_INIT_SIM_WAIT_PIN1)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_AUTH_WAIT_PIN1);
            }

            if (cfw_event.nParam1 & CFW_INIT_SIM_WAIT_PUK1)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_AUTH_WAIT_PUK1);
            }

            AT_MemCpy(&(cfwInitInfo.sim), &cfw_event, sizeof(cfw_event));
            break;
#ifndef AT_WITHOUT_SAT
        case CFW_INIT_STATUS_SAT:
        {
            if (!(cfw_event.nParam1 & CFW_INIT_SIM_SAT))
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SAT_OK);
            }
            else //  if (cfw_event.nParam1 & CFW_INIT_SIM_SAT)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SAT_ERROR);
            }

            AT_MemCpy(&(cfwInitInfo.sat), &cfw_event, sizeof(cfw_event));

            break;
        }
#endif
        case CFW_INIT_STATUS_SIMCARD:
        {
            if ((cfw_event.nParam1 & CFW_INIT_SIM_CARD_CHANGED) || (cfw_event.nParam1 & CFW_INIT_SIM_NORMAL_CARD))
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_NORMAL_CHANGED_SIM_CARD);
#ifdef AT_FTP_SUPPORT
#ifdef OLD_TCPIP_SUPPORT
                TCpIPInit();
#endif
#endif
            }
            else if (cfw_event.nParam1 & CFW_INIT_SIM_NORMAL_CARD)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_NORMAL_SIM_CARD);
            }
            else if (cfw_event.nParam1 & CFW_INIT_SIM_CARD_CHANGED)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_CHANGED_SIM_CARD);
            }
            else if (cfw_event.nParam1 & CFW_INIT_SIM_TEST_CARD)
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_TEST_SIM_CARD);
            }

            AT_MemCpy(&(cfwInitInfo.simCard), &cfw_event, sizeof(cfw_event));

            break;
        }
#ifndef AT_WITHOUT_SMS
        case CFW_INIT_STATUS_SMS: // SMS初始化完毕
        {
            UINT32 nUTI = 0x00;

            /* get the UTI and free it after finished calling */
            if (0 == (nUTI = AT_AllocUserTransID()))
            {
                AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", cfw_event.nUTI, nSim);
                AT_ZERO_PARAM1(pEvent);
                return FALSE;
            }

            CFW_SMS_STORAGE_INFO nStorageInfo;

            SUL_ZeroMemory8(&nStorageInfo, SIZEOF(CFW_SMS_STORAGE_INFO));
            nStorageInfo.totalSlot = pEvent->nParam2 & 0xFF;

            CFW_CfgSetSmsStorageInfo(&nStorageInfo, CFW_SMS_STORAGE_SM, nSim);
            AT_TC(g_sw_SA, , TSTXT("Sim SMS GetTotalNum Success Total sms %d\n"), nStorageInfo.totalSlot);

            nStorageInfo.totalSlot = PHONE_SMS_ENTRY_COUNT / NUMBER_OF_SIM;
            AT_TC(g_sw_SA, , TSTXT("ME SMS GetTotalNum Success Total sms %d\n"), nStorageInfo.totalSlot);
            CFW_CfgSetSmsStorageInfo(&nStorageInfo, CFW_SMS_STORAGE_ME, nSim);

//Remove SMS Init for Android issue.
//Add by XP at 20130129 begin;
#ifdef CHIP_HAS_AP
            AT_SmsMoInit(cfw_event.nUTI, nSim);
#else
            CFW_SmsMoInit(cfw_event.nUTI, nSim);

#endif
            UINT8 nSimStatus = 0;
            UINT8 nOtherSimID = 0;
            UINT16 nUti = 0;

            if (FALSE == gAtGlobal.setComFlag[nSim])
            {
                if (CFW_SIM_0 == nSim)
                {
                    nOtherSimID = CFW_SIM_1;
                }
                else
                {
                    nOtherSimID = CFW_SIM_0;
                }
                nSimStatus = (UINT8)CFW_GetSimStatus(nOtherSimID);
                hal_HstSendEvent(0x03140002);
                hal_HstSendEvent(nSimStatus);

                if ((0 == nSimStatus)) //0 means CFW_SIM_ABSENT
                {
                    hal_HstSendEvent(0x03140003);
                    gAtGlobal.powerCommFlag[nOtherSimID] = TRUE;
                    gbPowerCommMsgFlag[nOtherSimID] = TRUE;
                    gAtGlobal.setComFlag[nSim] = TRUE;
                    CFW_GetFreeUTI(0, (UINT8 *)&nUti);
                    CFW_SetComm(CFW_DISABLE_COMM, 1, nUti, nOtherSimID);
                }
            }
            break;
        }
#endif

#ifndef AT_WITHOUT_PBK
        case CFW_INIT_STATUS_PBK:
        {
            if (0 == (cfw_event.nParam1))
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_PBK_OK);
            }

            if (1 == (cfw_event.nParam1))
            {
                at_ModuleSetInitResult(AT_MODULE_INIT_SIM_PBK_ERROR);
            }
            AT_PBK_Init(nSim);
            AT_MemCpy(&(cfwInitInfo.pbk), &cfw_event, sizeof(cfw_event));
            at_ModuleSetPbkReady(true);
#if defined(USER_DATA_CACHE_SUPPORT)
            VDS_CacheFlushAllowed();
#endif
            break;
        }
#endif
        case CFW_INIT_STATUS_AUTODAIL:
        {
            UINT32 ret = 0x00;
            UINT8 DestNumber[2];

            CSW_TRACE(CFW_SHELL_TS_ID, "CFW_INIT_STATUS_AUTODAIL\n");
            DestNumber[0] = 0x11;
            DestNumber[1] = 0xF2;
            if (ERR_SUCCESS == (ret = CFW_CcEmcDial(DestNumber, 2, nSim)))
            {
                CSW_TRACE(CFW_SHELL_TS_ID, "Auto dial 112 Successl!!!\n");
            }
            else
            {
                CSW_TRACE(CFW_SHELL_TS_ID, "Auto dial 112 failed,The errorcode is 0x%x!!!\n", ret);
                while (ERR_SUCCESS != ret)
                {
                    COS_Sleep(500);
                    ret = CFW_CcEmcDial(DestNumber, 2, nSim);
                }
            }
            break;
        }
        case CFW_INIT_STATUS_NET:
        {
            AT_TC(g_sw_ATE, "CFW_INIT_STATUS_NET\n");
#ifdef CFW_GPRS_SUPPORT
#ifndef AT_WITHOUT_GPRS
            CFW_GprsAtt(CFW_GPRS_ATTACHED, 2, nSim);
#endif
#endif
            break;
        }
        default:
            break;
        }
        AT_ZERO_PARAM1(pEvent);
    }
    else if (cfw_event.nEventId == EV_CFW_SRV_STATUS_IND)
    {
        switch (cfw_event.nType)
        {
        case 0x10: //
            AT_TC(g_sw_ATE, "CFW SMS INIT success\n");
            at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_OK);

            break;

        case 0x11:
            at_ModuleSetInitResult(AT_MODULE_INIT_SIM_SMS_ERROR);
            AT_TC(g_sw_ATE, "CFW SMS INIT Error\n");

            break;

        default:
            break;
        }
        UINT32 nUTI = 0x00;

        /* get the UTI and free it after finished calling */
        if (0 == (nUTI = AT_AllocUserTransID()))
        {
            AT_SMS_Result_Err(ERR_AT_CMS_ME_FAIL, NULL, 0, "cmgr: malloc UTI error", cfw_event.nUTI, nSim);
            return FALSE;
        }
#ifndef AT_WITHOUT_SMS
        CFW_SmsInitComplete(nUTI, nSim);

        AT_SMS_INIT(nSim);
        AT_MemCpy(&(cfwInitInfo.sms[nSim]), &cfw_event, sizeof(cfw_event));

        // Notify SA that the SMS (ATS) module is ready for use.
        at_ModuleSetSmsReady(true);
        //AT_TC(g_sw_SA, TSTXT("set gATCurrentuCmer_ind nSim[%d] to 2\n"), nSim);
        //gATCurrentuCmer_ind[nSim]       = 2;
        gATCurrentAtSMSInfo[nSim].sCnmi.nMt = 2;
#endif
    }
    else if (EV_CFW_SIM_GET_SMS_TOTAL_NUM_RSP == cfw_event.nEventId)
    {
        UINT8 nSimStatus = 0;
        UINT8 nOtherSimID = 0;
        UINT16 nUti = 0;

        if (FALSE == gAtGlobal.setComFlag[nSim])
        {
            gAtGlobal.setComFlag[nSim] = TRUE;
            if (CFW_SIM_0 == nSim)
            {
                nOtherSimID = CFW_SIM_1;
            }
            else
            {
                nOtherSimID = CFW_SIM_0;
            }
            nSimStatus = (UINT8)CFW_GetSimStatus(nOtherSimID);
            hal_HstSendEvent(0x03140004);
            hal_HstSendEvent(nSimStatus);

            if (0 == nSimStatus || 4 == nSimStatus)
            {
                hal_HstSendEvent(0x03140005);

                CFW_GetFreeUTI(0, (UINT8 *)&nUti);
                CFW_SetComm(CFW_DISABLE_COMM, 1, nUti, nOtherSimID);
            }
        }
    }
    return FALSE;
}

BOOL AT_InitCfw(COS_EVENT *pEvent)
{
    return AT_CFW_InitProcess(pEvent);
}

BOOL AT_GetCfwInitSmsInfo(CFW_EVENT *pCfwEvent)
{
    BOOL ret = FALSE;

    if (cfwInitInfo.sms[pCfwEvent->nFlag].nType == CFW_INIT_STATUS_SMS)
    {
        pCfwEvent->nEventId = cfwInitInfo.sms[pCfwEvent->nFlag].nEventId;
        pCfwEvent->nParam1 = cfwInitInfo.sms[pCfwEvent->nFlag].nParam1;
        pCfwEvent->nParam2 = cfwInitInfo.sms[pCfwEvent->nFlag].nParam2;

        pCfwEvent->nUTI = cfwInitInfo.sms[pCfwEvent->nFlag].nUTI;
        pCfwEvent->nType = cfwInitInfo.sms[pCfwEvent->nFlag].nType;
        pCfwEvent->nFlag = cfwInitInfo.sms[pCfwEvent->nFlag].nFlag;

        ret = TRUE;
    }

    return ret;
}

BOOL CFW_CheckPowerCommStatus(CFW_SIM_ID nSimID)
{
    return gAtGlobal.powerCommFlag[nSimID];
}

VOID at_ModuleModeSwitch(AT_MODE_SWITCH_CAUSE_T cause, UINT8 nDLCI)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(nDLCI);
    AT_CMD_ENGINE_T *engine = at_DispatchGetCmdEngine(dispatch);
    switch (cause)
    {
    case AT_CMD_MODE_SWITCH_CC_RELEASE_CALL:
    case AT_CMD_MODE_SWITCH_CC_SPEECH_IND:
    case AT_CMD_MODE_SWITCH_CC_ACCEPT_CALL:
    case AT_CMD_MODE_SWITCH_CC_INITIATE_SPEECH_CALL:
    case AT_CMD_MODE_SWITCH_ATH:
        break;
    case AT_MODE_SWITCH_DATA_END:
        at_DispatchEndDataMode(dispatch);
        at_CmdRespOK(engine);
        break;
    case AT_MODE_SWITCH_DATA_START:
        at_CmdClearRemainCmds(engine);
        at_CmdRespIntermCode(engine, CMD_RC_CONNECT);
        at_DispatchSetDataMode(dispatch);
        break;
    case AT_MODE_SWITCH_DATA_ESCAPE:
        at_DispatchSetCmdMode(dispatch);
        at_CmdSetLineMode(engine);
        at_CmdRespOK(engine);
        break;
    case AT_MODE_SWITCH_DATA_RESUME:
        at_CmdRespIntermCode(engine, CMD_RC_CONNECT);
        at_DispatchSetDataMode(dispatch);
        break;
    }
}

int at_ModuleNotifyResult(PAT_CMD_RESULT pCmdResult)
{
    AT_ASSERT(pCmdResult != NULL);

    int channel = at_CmdGetChannel(pCmdResult->engine);
    if (pCmdResult->nDataSize == 0)
        ATCMDLOG(I, "AT CMD%d return=%d result=%d size=%d", channel,
                 pCmdResult->uReturnValue, pCmdResult->uResultCode, pCmdResult->nDataSize);
    else
        ATCMDLOGS(I, TSM(4), "AT CMD%d return=%d result=%d size=%d: %s", channel,
                  pCmdResult->uReturnValue, pCmdResult->uResultCode, pCmdResult->nDataSize, pCmdResult->pData);

    int r = 0;

    // why to output URC here?
    //at_ModuleWriteUrc(pCmdResult->engine);

    switch (pCmdResult->uReturnValue)
    {
    case CMD_FUNC_FAIL:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        if (pCmdResult->nErrorType == CMD_ERROR_CODE_TYPE_CME)
            at_CmdRespCmeError(pCmdResult->engine, pCmdResult->uErrorCode);
        else if (pCmdResult->nErrorType == CMD_ERROR_CODE_TYPE_CMS)
            at_CmdRespCmsError(pCmdResult->engine, pCmdResult->uErrorCode);
        else
            at_CmdRespErrorCode(pCmdResult->engine, pCmdResult->uResultCode);
        break;

    case CMD_FUNC_SUCC:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        at_CmdRespOK(pCmdResult->engine);
        break;

    case CMD_FUNC_SUCC_ASYN:
        AT_SetAsyncTimerMux(pCmdResult->engine, pCmdResult->nDelayTime);
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        break;

    case CMD_FUNC_WAIT_SMS:
        AT_SetAsyncTimerMux(pCmdResult->engine, pCmdResult->nDelayTime);
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        at_CmdSetPromptMode(pCmdResult->engine);
        break;

    case CMD_FUNC_WAIT_IP_DATA:
        AT_SetAsyncTimerMux(pCmdResult->engine, pCmdResult->nDelayTime);
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        at_CmdSetPromptMode(pCmdResult->engine);
        break;

    case CMD_FUNC_CONTINUE:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        break;

    case CSW_IND_NOTIFICATION:
        at_CmdRespInfoNText(pCmdResult->engine, pCmdResult->pData, pCmdResult->nDataSize);
        break;

    default:
        AT_ASSERT(0);
        break;
    }
    return r;
}

// =============================================================================
// at_CmdClearRemainCmds
// =============================================================================
void at_CmdClearRemainCmds(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    if (module->cmds == NULL)
    {
        ATCMDLOG(E, "AT CMD%d not in cmd to clear remain cmds", at_CmdGetChannel(engine));
        return;
    }

    ATCMDLOG(I, "AT CMD%d clear remain cmds", at_CmdGetChannel(engine));
    AT_COMMAND_T *next = module->cmds->next;
    at_CmdlistFreeAll(&next);
}

// =============================================================================
// at_CmdRespInfoNText/at_CmdRespInfoText
// =============================================================================
void at_CmdRespInfoNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    if (text == NULL || length == 0)
        return;

    ATCMDLOGS(I, TSM(2), "AT CMD%d info text len=%d: %s", at_CmdGetChannel(engine), length, text);

    AT_ModuleInfo *module = at_CmdGetModule(engine);
    uint8_t crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};
    uint8_t atv = gATCurrenteResultcodeFormat;

    if (module->firstInfoText && atv == 1)
        at_CmdWrite(engine, crlf, 2);
    module->firstInfoText = false;
    at_CmdWrite(engine, text, length);
    at_CmdWrite(engine, crlf, 2);
}

void at_CmdRespInfoText(AT_CMD_ENGINE_T *engine, const uint8_t *text) { at_CmdRespInfoNText(engine, text, strlen(text)); }

// =============================================================================
// at_CmdRespOutputResultCode
// =============================================================================
static void at_CmdRespOutputResultCode(AT_CMD_ENGINE_T *engine, int code)
{
    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq != 0)
        return;

    uint8_t resp[32];
    uint8_t *p = resp;
    uint8_t atv = gATCurrenteResultcodeFormat;
    if (atv == 0)
    {
        p += AT_Sprintf(p, "%u", code);
        *p++ = gATCurrentnS3;
    }
    else
    {
        *p++ = gATCurrentnS3;
        *p++ = gATCurrentnS4;

        const uint8_t *s = at_GetResultCodeInfo(code);
        unsigned len = strlen(s);
        memcpy(p, s, len);
        p += len;

        *p++ = gATCurrentnS3;
        *p++ = gATCurrentnS4;
    }
    at_CmdWrite(engine, resp, p - resp);
}

// =============================================================================
// at_CmdFinalHandle
// =============================================================================
static void at_CmdFinalHandle(AT_CMD_ENGINE_T *engine, bool ok)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    AT_KillAsyncTimerMux(engine);
    if (ok)
        at_CmdlistFree(at_CmdlistPopFront(&module->cmds));
    else
        at_CmdlistFreeAll(&module->cmds);

    at_CmdSetLineMode(engine);
    if (module->cmds != NULL)
        at_ModuleScheduleNextCommand(engine);
}

// =============================================================================
// at_CmdRespOK
// =============================================================================
void at_CmdRespOK(AT_CMD_ENGINE_T *engine)
{
    ATCMDLOG(I, "AT CMD%d OK", at_CmdGetChannel(engine));

    at_CmdFinalHandle(engine, true);

    AT_ModuleInfo *module = at_CmdGetModule(engine);
    if (module->cmds == NULL)
        at_CmdRespOutputResultCode(engine, CMD_RC_OK);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespOKText
// =============================================================================
void at_CmdRespOKText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD%d OK: %s", at_CmdGetChannel(engine), text);

    at_CmdFinalHandle(engine, true);

    // output it even it is not the last one, and not affected by ATV, ATQ
    uint8_t crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};
    at_CmdWrite(engine, crlf, 2);
    at_CmdWrite(engine, text, strlen(text));
    at_CmdWrite(engine, crlf, 2);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespError
// =============================================================================
void at_CmdRespError(AT_CMD_ENGINE_T *engine)
{
    ATCMDLOG(I, "AT CMD%d ERROR", at_CmdGetChannel(engine));

    at_CmdRespErrorCode(engine, CMD_RC_ERROR);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespErrorText
// =============================================================================
void at_CmdRespErrorText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD%d ERROR: %s", at_CmdGetChannel(engine), text);

    at_CmdFinalHandle(engine, false);

    // not affected by ATV, ATQ
    uint8_t crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};
    at_CmdWrite(engine, crlf, 2);
    at_CmdWrite(engine, text, strlen(text));
    at_CmdWrite(engine, crlf, 2);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespErrorCode
// =============================================================================
void at_CmdRespErrorCode(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d ERROR code=%d", at_CmdGetChannel(engine), code);

    at_CmdFinalHandle(engine, false);
    at_CmdRespOutputResultCode(engine, code);

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespIntermCode
// =============================================================================
void at_CmdRespIntermCode(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d interm code=%d", at_CmdGetChannel(engine), code);

    at_CmdRespOutputResultCode(engine, code);
}

// =============================================================================
// at_CmdRespUrcCode
// =============================================================================
void at_CmdRespUrcCode(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d urc code=%d", at_CmdGetChannel(engine), code);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq != 0)
        return;

    // TODO: CMER is not considered
    at_CmdRespOutputResultCode(engine, code);
}

// =============================================================================
// at_CmdRespUrcNText/at_CmdRespUrcText
// =============================================================================
void at_CmdRespUrcNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    ATCMDLOGS(I, TSM(2), "AT CMD%d urc len=%d: %s", at_CmdGetChannel(engine), length, text);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq != 0)
        return;

    // TODO: CMER is not considered
    uint8_t crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};
    at_CmdWrite(engine, crlf, 2);
    at_CmdWrite(engine, text, length);
    at_CmdWrite(engine, crlf, 2);
}

void at_CmdRespUrcText(AT_CMD_ENGINE_T *engine, const uint8_t *text) { at_CmdRespUrcNText(engine, text, strlen(text)); }

// =============================================================================
// at_CmdRespCmeError
// =============================================================================
void at_CmdRespCmeError(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d CME error code=%d", at_CmdGetChannel(engine), code);

    at_CmdFinalHandle(engine, false);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq == 0)
    {
        uint8_t cmee = gATCurrentuCmee;
        if (cmee == 0)
        {
            at_CmdRespOutputResultCode(engine, CMD_RC_ERROR);
        }
        else
        {
            uint8_t crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};
            at_CmdWrite(engine, crlf, 2);
            if (cmee == 1)
            {
                uint8_t str[256];
                AT_Sprintf(str, "+CME ERROR: %d", code);
                at_CmdWrite(engine, str, strlen(str));
            }
            else
            {
                const uint8_t *str = at_GetCmeCodeInfo(code);
                at_CmdWrite(engine, "+CME ERROR: ", strlen("+CME ERROR: "));
                at_CmdWrite(engine, str, strlen(str));
            }
            at_CmdWrite(engine, crlf, 2);
        }
    }

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespCmsError
// =============================================================================
void at_CmdRespCmsError(AT_CMD_ENGINE_T *engine, int code)
{
    ATCMDLOG(I, "AT CMD%d CMS error code=%d", at_CmdGetChannel(engine), code);

    at_CmdFinalHandle(engine, false);

    uint8_t atq = gATCurrenteResultcodePresentation;
    if (atq == 0)
    {
        uint8_t cmee = gATCurrentuCmee;
        if (cmee == 0)
        {
            at_CmdRespErrorCode(engine, CMD_RC_ERROR);
        }
        else
        {
            // not affected by ATV
            uint8_t crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};
            at_CmdWrite(engine, crlf, 2);
            if (cmee == 1)
            {
                uint8_t str[256];
                AT_Sprintf(str, "+CMS ERROR: %d", code);
                at_CmdWrite(engine, str, strlen(str));
            }
            else
            {
                const uint8_t *str = at_GetCmsCodeInfo(code);
                at_CmdWrite(engine, "+CMS ERROR: ", strlen("+CMS ERROR: "));
                at_CmdWrite(engine, str, strlen(str));
            }
            at_CmdWrite(engine, crlf, 2);
        }
    }

    at_CmdCommandFinished(engine);
}

// =============================================================================
// at_CmdRespIntermText
// =============================================================================
void at_CmdRespIntermText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD%d interm: %s", at_CmdGetChannel(engine), text);
    at_CmdWrite(engine, text, strlen(text));
}

// =============================================================================
// at_CmdReportUrcCode
// =============================================================================
void at_CmdReportUrcCode(uint8_t sim, int code)
{
    ATCMDLOG(I, "AT CMD sim%d urc code=%d", sim, code);

    AT_DISPATCH_T *ch = at_DispatchFindByChannel(at_GetUrcChannel());
    if (ch == NULL)
        return;
    at_CmdRespUrcCode(at_DispatchGetCmdEngine(ch), code);
}

// =============================================================================
// at_CmdReportUrcText
// =============================================================================
void at_CmdReportUrcText(uint8_t sim, const uint8_t *text)
{
    ATCMDLOGS(I, TSM(1), "AT CMD sim%d urc: %s", sim, text);

    AT_DISPATCH_T *ch = at_DispatchFindByChannel(at_GetUrcChannel());
    if (ch == NULL)
        return;
    at_CmdRespUrcText(at_DispatchGetCmdEngine(ch), text);
}

// =============================================================================
// at_CmdRespOutputText
// =============================================================================
void at_CmdRespOutputText(AT_CMD_ENGINE_T *engine, const uint8_t *text)
{
    if (text == NULL)
        return;

    ATCMDLOGS(I, TSM(1), "AT CMD%d output: %s", at_CmdGetChannel(engine), text);
    at_CmdWrite(engine, text, strlen(text));
}

// =============================================================================
// at_CmdRespOutputNText
// =============================================================================
void at_CmdRespOutputNText(AT_CMD_ENGINE_T *engine, const uint8_t *text, unsigned length)
{
    if (text == NULL || length == 0)
        return;

    ATCMDLOG(I, "AT CMD%d output length %d", at_CmdGetChannel(engine), length);
    at_CmdWrite(engine, text, length);
}

// =============================================================================
// at_CmdRespOutputPrompt
// =============================================================================
void at_CmdRespOutputPrompt(AT_CMD_ENGINE_T *engine)
{
    uint8_t rsp[4] = {gATCurrentnS3, gATCurrentnS4, '>', ' '};
    at_CmdWrite(engine, rsp, 4);
}

// =============================================================================
// at_CmdRingInd
// =============================================================================
void at_CmdRingInd(uint8_t sim)
{
    static const uint8_t cring[] = "+CRING: <voice>";
    if (gATCurrentCrc)
        at_CmdReportUrcText(sim, cring);
    else
        at_CmdReportUrcCode(sim, CMD_RC_RING);

    // TODO: other V.24 signals
}

void at_ModuleScheduleNextCommand(AT_CMD_ENGINE_T *engine)
{
    at_TaskCallback((COS_CALLBACK_FUNC_T)at_ModuleRunCommand, engine);
}

void at_ModuleRunCommand(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    AT_COMMAND_T *cmd = module->cmds;
    if (cmd == NULL)
        return;

#ifdef GCF_TEST
    if (AT_GPRS_Get_CGSEND_Channel() == at_CmdGetChannel(engine))
    {
        AT_TC(g_sw_ATE, "new command, stop CGSEND timer");
        AT_GPRS_CGSEND_Stop();
    }
#endif

    unsigned paramlen = 0;
    for (int n = 0; n < cmd->paramCount; n++)
    {
        if (n != 0)
            module->param[paramlen++] = ',';
        memcpy(&module->param[paramlen], cmd->params[n]->value, cmd->params[n]->length);
        paramlen += cmd->params[n]->length;
    }
    module->param[paramlen] = 0;

    ATCMDLOGS(I, TSM(1, 3), "AT CMD%d run %s type=%d param=%s", at_CmdGetChannel(engine),
              cmd->desc->pName, cmd->type, module->param);
    AT_CMD_PARA para = {};
    para.iType = cmd->type;
    para.pPara = module->param;
    para.uCmdStamp = 0; //pCmd->uCmdStamp;
    para.engine = engine;
    para.nDLCI = at_CmdGetChannel(engine);
    para.pExData = NULL;
    para.iExDataLen = 0;
    para.paramCount = cmd->paramCount;
    memcpy(para.params, cmd->params, sizeof(cmd->params));
    module->firstInfoText = true;
    cmd->desc->pFunc(&para);
}

void at_ModuleRunCommandExtra(AT_CMD_ENGINE_T *engine, const char *data, unsigned length)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    AT_COMMAND_T *cmd = module->cmds;

    if (cmd == NULL)
    {
        ATCMDLOG(E, "AT CMD%d no command for prompt", at_CmdGetChannel(engine));
        at_CmdSetLineMode(engine);
        return;
    }

    unsigned paramlen = 0;
    for (int n = 0; n < cmd->paramCount; n++)
    {
        if (n != 0)
            module->param[paramlen++] = ',';
        memcpy(&module->param[paramlen], cmd->params[n]->value, cmd->params[n]->length);
        paramlen += cmd->params[n]->length;
    }
    module->param[paramlen] = 0;

    ATCMDLOGS(I, TSM(1, 3), "AT CMD%d run %s type=%d param=%s extralen=%d",
              at_CmdGetChannel(engine), cmd->desc->pName, cmd->type, module->param, length);
    AT_CMD_PARA para = {};
    para.iType = cmd->type;
    para.pPara = module->param;
    para.uCmdStamp = 0; //pCmd->uCmdStamp;
    para.engine = engine;
    para.nDLCI = at_CmdGetChannel(engine);
    para.pExData = (UINT8 *)data;
    para.iExDataLen = length;
    para.paramCount = cmd->paramCount;
    memcpy(para.params, cmd->params, sizeof(cmd->params));
    cmd->desc->pFunc(&para);
}

static void at_ModuleClearCmdList(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    at_CmdlistFreeAll(&module->cmds);
    at_CmdSetLineMode(engine);
    return;
}

static void at_ModuleClearCurCmd(AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    // if async timer exist, kill it.
    AT_KillAsyncTimerMux(engine);
    at_CmdlistFree(at_CmdlistPopFront(&module->cmds));
    at_CmdSetLineMode(engine);
    return;
}

int at_ModuleProcessLine(UINT8 *pBuf, UINT32 len, AT_CMD_ENGINE_T *engine)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    if (pBuf == NULL && len == 0)
        return -1;

    int result = at_ParseLine(pBuf, len, &module->cmds);
    if (result != 0)
    {
        ATCMDLOG(W, "AT CMD%d parser failed, error code %d", at_CmdGetChannel(engine), result);
        at_CmdlistFreeAll(&module->cmds);
        return result;
    }

#if 0 // comamnd status check is broken now
    AT_COMMAND_T *cmd = module->cmds;
    while (cmd != NULL)
    {
        if (!SA_IsCmdEnabled(engine, cmd->desc) || !SA_IsCmdReady(engine, cmd->desc))
        {
            ATCMDLOGS(W, TSM(0), "AT CMD%d cmd %s not enabled or ready", at_CmdGetChannel(engine), cmd->desc->pName);
            at_CmdlistFreeAll(&module->cmds);
            return -1;
        }
    }
#endif

    if (module->cmds != NULL)
    {
        //if the first AT command is +CMMS ,then record the number of +CMSS.
        if (AT_StrCaselessCmp("+CMMS", module->cmds->desc->pName) == 0)
            module->cmssNum = at_CmdlistCount(module->cmds) - 1;
    }

    return 0;
}

#ifdef LTE_NBIOT_SUPPORT
extern VOID AT_IPERF_AsyncEventProcess(COS_EVENT *pEvent);
#endif
VOID AT_AsyncEventProcess(COS_EVENT *pEvent)
{
    if ((HIUINT16(pEvent->nParam3) == 0) || (HIUINT16(pEvent->nParam3) > MAX_DLC_NUM))
    {
        pEvent->nParam3 = (pEvent->nParam3 & 0x00ffff) | (1 << 16);
    }

    CFW_EVENT cfwEvent;
    cfwEvent.nEventId = pEvent->nEventId;
    cfwEvent.nParam1 = pEvent->nParam1;
    cfwEvent.nParam2 = pEvent->nParam2;
    cfwEvent.nUTI = HIUINT16(pEvent->nParam3);
    cfwEvent.nType = HIUINT8(pEvent->nParam3);
    cfwEvent.nFlag = LOUINT8(pEvent->nParam3);

    UINT8 POSSIBLY_UNUSED nDLCI = cfwEvent.nUTI;

    AT_TC(g_sw_SA, "-->%s\n", TS_GetEventName(pEvent->nEventId));
    AT_TC(g_sw_SA, "P1[0x%x] P2[0x%x] nUTI[%d] nType[0x%x] nFlag[%d]\n",
          cfwEvent.nParam1, cfwEvent.nParam2, cfwEvent.nUTI, cfwEvent.nType, cfwEvent.nFlag);

    switch (pEvent->nEventId)
    {
    case EV_CFW_SIM_SET_PREF_OPERATOR_LIST_RSP:
    case EV_CFW_SIM_GET_PREF_OPERATOR_LIST_RSP:
    case EV_CFW_SIM_GET_PREF_OPT_LIST_MAXNUM_RSP:
    case EV_CFW_NW_SIGNAL_QUALITY_IND:
    case EV_CFW_NW_NETWORKINFO_IND:
    case EV_CFW_NW_REG_STATUS_IND:
    case EV_CFW_NW_STORELIST_IND:
    case EV_CFW_NW_SET_BAND_RSP:
    case EV_CFW_NW_GET_IMEI_RSP:
    case EV_CFW_NW_GET_SIGNAL_QUALITY_RSP:
    case EV_CFW_NW_SET_REGISTRATION_RSP:
    case EV_CFW_NW_GET_AVAIL_OPERATOR_RSP:
    case EV_CFW_NW_ABORT_LIST_OPERATORS_RSP:
    case EV_CFW_SET_COMM_RSP:
    case EV_CFW_NW_GET_QSCANF_RSP:
    case EV_CFW_XCPU_TEST_RSP:
    case EV_CFW_WCPU_TEST_RSP:
#ifdef LTE_NBIOT_SUPPORT
    case EV_CFW_ERRC_CONNSTATUS_IND:
    case EV_CFW_GPRS_EDRX_IND:
    case EV_CFW_GPRS_CCIOTOPT_IND:
#endif
        AT_NW_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_NW_DEREGISTER_RSP:
        if (!g_gc_pwroff)
            AT_NW_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_SPEECH_CALL_IND:
        at_ModuleModeSwitch(AT_CMD_MODE_SWITCH_CC_SPEECH_IND, HIUINT16(pEvent->nParam3));
        AT_CC_AsyncEventProcess(pEvent);
        break;

    //wait platform  submit  reg simulator support
    case EV_CFW_REG_UPDATE_IND:
        AT_EMOD_CFGUPDATE_Indictation(pEvent);
        break;

    case EV_CFW_CC_ERROR_IND:
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_RELEASE_CALL_IND:
        at_ModuleModeSwitch(AT_CMD_MODE_SWITCH_CC_RELEASE_CALL, HIUINT16(pEvent->nParam3));
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_CALL_INFO_IND:
    case EV_CFW_CC_PROGRESS_IND:
    case EV_DM_SPEECH_IND:
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_CRSSINFO_IND:
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_ACCEPT_SPEECH_CALL_RSP:
        at_ModuleModeSwitch(AT_CMD_MODE_SWITCH_CC_ACCEPT_CALL, HIUINT16(pEvent->nParam3));
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_CALL_HOLD_MULTIPARTY_RSP:
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_INITIATE_SPEECH_CALL_RSP:
        at_ModuleModeSwitch(AT_CMD_MODE_SWITCH_CC_INITIATE_SPEECH_CALL, HIUINT16(pEvent->nParam3));
        AT_CC_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_CC_RELEASE_CALL_RSP:
        at_ModuleModeSwitch(AT_CMD_MODE_SWITCH_CC_RELEASE_CALL, HIUINT16(pEvent->nParam3));
        AT_CC_AsyncEventProcess(pEvent);
        break;

#ifndef AT_WITHOUT_SS
    case EV_CFW_SS_SET_CALL_WAITING_RSP:
    case EV_CFW_SS_QUERY_CALL_WAITING_RSP:
    case EV_CFW_SS_SET_CALL_FORWARDING_RSP:
    case EV_CFW_SS_QUERY_CALL_FORWARDING_RSP:
    case EV_CFW_SS_QUERY_CLIP_RSP:
    case EV_CFW_SS_QUERY_CLIR_RSP:
    case EV_CFW_SS_QUERY_COLR_RSP:
    case EV_CFW_SS_QUERY_COLP_RSP:
    case EV_CFW_SS_SEND_USSD_RSP:
    case EV_CFW_SIM_GET_PUCT_RSP:
    case EV_CFW_SIM_GET_ACM_RSP: // [+Changyg moved from SMS
    case EV_CFW_SIM_SET_ACM_RSP: // [+Changyg moved from SMS
    case EV_CFW_SIM_SET_PUCT_RSP:
    case EV_CFW_SS_USSD_IND:
    case EV_CFW_SS_QUERY_FACILITY_LOCK_RSP:
    case EV_CFW_SS_SET_FACILITY_LOCK_RSP:
        AT_SS_AsyncEventProcess(pEvent);
        break;
#endif

    case EV_CFW_SIM_CHANGE_PWD_RSP:
    case EV_CFW_SIM_GET_AUTH_STATUS_RSP:
    case EV_CFW_SIM_ENTER_AUTH_RSP:
    case EV_CFW_SIM_SET_FACILITY_LOCK_RSP:
    case EV_CFW_SIM_GET_FACILITY_LOCK_RSP:
    case EV_CFW_SIM_DELETE_MESSAGE_RSP:
    case EV_CFW_SIM_WRITE_MESSAGE_RSP:
    case EV_CFW_SIM_READ_MESSAGE_RSP:
    case EV_CFW_SIM_LIST_MESSAGE_RSP:
    case EV_CFW_SIM_GET_SMS_STORAGE_INFO_RSP:
    case EV_CFW_SIM_READ_BINARY_RSP:
    case EV_CFW_SIM_UPDATE_BINARY_RSP:
    case EV_CFW_SIM_GET_SMS_PARAMETERS_RSP:
    case EV_CFW_SIM_SET_SMS_PARAMETERS_RSP:
    case EV_CFW_SIM_GET_MR_RSP:
    case EV_CFW_SIM_SET_MR_RSP:
    case EV_CFW_SIM_COMPOSE_PDU_RSP:
    case EV_CFW_SIM_SET_PBKMODE_RSP:
    case EV_CFW_SIM_GET_PBKMODE_RSP:
    case EV_CFW_SIM_INIT_SMS_RSP:
    case EV_CFW_SIM_RESET_RSP:
    case EV_CFW_SIM_GET_ACMMAX_RSP:
    case EV_CFW_SIM_SET_ACMMAX_RSP:
    case EV_CFW_SIM_GET_FILE_STATUS_RSP:
    case EV_CFW_SIM_READ_RECORD_RSP:
    case EV_CFW_SIM_UPDATE_RECORD_RSP:
        AT_SIM_AsyncEventProcess(pEvent);
        AT_PBK_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_SIM_GET_ICCID_RSP:
        AT_Emod_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_SIM_GET_PROVIDER_ID_RSP:
        AT_ID_AsyncEventProcess(pEvent);
        break;

#ifndef AT_WITHOUT_SAT
    case EV_CFW_SAT_CMDTYPE_IND:
    case EV_CFW_SAT_ACTIVATION_RSP:
    case EV_CFW_SAT_GET_STATUS_RSP:
    case EV_CFW_SAT_RESPONSE_RSP:
    case EV_CFW_SAT_GET_INFORMATION_RSP:
        AT_SAT_AsyncEventProcess(pEvent);
        break;
#endif

#ifndef AT_WITHOUT_SMS
    case EV_CFW_NEW_SMS_IND:
    case EV_CFW_CB_MSG_IND:
    case EV_CFW_CB_PAGE_IND:
    case EV_CFW_SMS_INFO_IND:
    case EV_CFW_SMS_DELETE_MESSAGE_RSP:
    case EV_CFW_SMS_LIST_MESSAGE_RSP:
    case EV_CFW_SMS_READ_MESSAGE_RSP:
    case EV_CFW_SMS_SEND_MESSAGE_RSP:
    case EV_CFW_SMS_WRITE_MESSAGE_RSP:
    case EV_CFW_SMS_GET_STORAGE_RSP:
    case EV_CFW_SMS_SET_STORAGE_RSP:
    case EV_CFW_SMS_SET_CENTER_ADDR_RSP:
    case EV_CFW_SMS_GET_CENTER_ADDR_RSP:
    case EV_CFW_SMS_SET_TEXT_MODE_PARAM_RSP:
    case EV_CFW_SMS_GET_TEXT_MODE_PARAM_RSP:
    case EV_CFW_SMS_LIST_MESSAGE_HRD_RSP:
    case EV_CFW_SMS_COPY_MESSAGE_RSP:
    case EV_CFW_SMS_SET_UNREAD2READ_RSP:
    case EV_CFW_SMS_SET_UNSENT2SENT_RSP:
    case EV_CFW_SMS_SET_READ2UNREAD_RSP:
#ifdef AT_MODULE_SUPPORT_OTA
    case EV_CFW_NEW_EMS_IND:
#endif
        AT_SMS_AsyncEventProcess(pEvent);
        break;
#endif

#ifndef AT_WITHOUT_PBK
    case EV_CFW_PBK_ADD_RSP:
    case EV_CFW_PBK_UPDATE_ENTRY_RSP:
    case EV_CFW_PBK_DELETE_RSP:
    case EV_CFW_PBK_DELETE_BATCH_ENTRIES_RSP:
    case EV_CFW_PBK_FIND_ENTRY_RSP:
    case EV_CFW_PBK_LIST_ENTRIES_RSP:
    case EV_CFW_PBK_GET_ENTRY_RSP:
    case EV_CFW_PBK_COPY_ENTRIES_RSP:
    case EV_CFW_PBK_ADD_ENTRY_ITEM_RSP:
    case EV_CFW_PBK_GET_ENTRY_ITEMS_RSP:
    case EV_CFW_PBK_DELETE_ENTRY_ITEM_RSP:
    case EV_CFW_PBK_ADD_CALLLOG_ENTTRY_RSP:
    case EV_CFW_PBK_GET_CALLLOG_ENTRY_RSP:
    case EV_CFW_PBK_LIST_CALLLOG_ENTRIES_RSP:
    case EV_CFW_PBK_DELETE_CALLLOG_ALL_ENTRIES_RSP:
    case EV_CFW_PBK_CANCEL_RSP:
    case EV_CFW_PBK_DELETE_CALLLOG_OLDEST_ENTRY_RSP:
    case EV_CFW_SIM_ADD_PBK_RSP:
    case EV_CFW_SIM_DELETE_PBK_ENTRY_RSP:
    case EV_CFW_SIM_GET_PBK_ENTRY_RSP:
    case EV_CFW_SIM_LIST_PBK_ENTRY_RSP:
    case EV_CFW_SIM_LIST_COUNT_PBK_ENTRY_RSP:
    case EV_CFW_SIM_GET_PBK_STRORAGE_INFO_RSP:
    case EV_CFW_SIM_GET_PBK_STRORAGE_RSP:
    case EV_CFW_SIM_SEARCH_PBK_RSP:
        AT_PBK_AsyncEventProcess(pEvent);
        break;
#endif

#if defined(CFW_GPRS_SUPPORT) && !defined(AT_WITHOUT_GPRS)
    case EV_CFW_GPRS_STATUS_IND:
        AT_GprsStatusInd(pEvent);
        break;
#endif

    case EV_CFW_TSM_INFO_IND:
        AT_NW_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_TSM_INFO_END_RSP:
        break;

    case EV_PM_BC_IND:
        AT_SPECIAL_Battery_AsyncEventProcess(pEvent);
        break;

    case EV_CFW_SIM_CLOSE_RSP:
    case EV_CFW_EXIT_IND:
    case EV_TIM_ALRAM_IND: // hameina[+]07-10-29 for alarm URC
        AT_GC_AsyncEventProcess(pEvent);
        break;

    case EV_DM_GPADC_IND:
    {
        UINT16 AT_Earphone_vol = (UINT16)((pEvent->nParam1) & 0xFFFF);
        if (((pEvent->nParam3) & 0x0F) == 3) // using GPADC detect PMD_EAR_DETECT_POLLING
        {
            if ((MAXVOLT_EARPHONE_IN_PHONE >= AT_Earphone_vol &&
                 MINVOLT_EARPHONE_IN_PHONE <= AT_Earphone_vol) &&
                (GetHandsetInPhone() == 0))
            {
                SetHandsetInPhone(1);
            }
            else if (MINVOLT_EARPHONE_NOT_IN_PHONE <= AT_Earphone_vol && GetHandsetInPhone() == 1)
            {
                SetHandsetInPhone(0);
            }
            else
            {
            }
        }
        break;
    }

    case EV_CFW_SPECIAL_DTMF_KEY_IND:
        AT_AUDIO_AsyncEventProcess(pEvent);
        break;
#ifdef MODEM_CALL_RECORDER
    case EV_CFW_CALL_REC_REQ:
        AT_CALLREC_AsyncEventProcess(pEvent);
        break;
#endif

#ifdef WIFI_SUPPORT
    case EV_CFW_WIFI_INIT_RSP:
    case EV_CFW_WIFI_DEINIT_RSP:
    case EV_CFW_WIFI_SCAN_RSP:
    case EV_CFW_WIFI_SCAN_SINGLE_RSP:
    case EV_CFW_WIFI_SCAN_COMPLETE:
    case EV_CFW_WIFI_CONN_RSP:
    case EV_CFW_WIFI_DISCONN_RSP:
    case EV_CFW_WIFI_DISCONNECT_IND:
    case EV_CFW_WIFI_IPADDR_UPDATE_IND:

#ifdef WIFI_SMARTCONFIG
    case EV_CFW_WIFI_SMART_CONFIG_COMPL:
#endif
        AT_WIFI_AsyncEventProcess(pEvent);
        break;
#endif

#if defined(ATA_SUPPORT) && defined(__MMI_BT_SUPPORT__)
    //case EV_BT_POWERON_RSP:
    case 0xF0000013:
    case 0xF0000014:
    case 0xF0000015:
        AT_BT_ATA_AsyncEventProcess(pEvent);
        break;
#endif

#ifdef LTE_NBIOT_SUPPORT

    case EV_CFW_IPERF_REV_DATA_IND:
        AT_IPERF_AsyncEventProcess(pEvent);
        break;
    case EV_CFW_GPRS_CRTDCP_IND:
        CrtdcpDataHandler(pEvent->nParam1, (CFW_GPRS_DATA *)pEvent->nParam2, cfwEvent.nFlag);
        break;
    case EV_CFW_GPRS_NONIPDATA_IND:
        NonIpDataHandler(pEvent->nParam1, (CFW_GPRS_DATA *)pEvent->nParam2, cfwEvent.nFlag);
        break;
#endif

    default:
        break;
    }

    return;
}

#define ERR_STR "COMMAND NO RESPONSE!"
VOID at_ModuleHandleTimeout(AT_CMD_ENGINE_T *engine)
{
    UINT8 crlf[2] = {AT_GC_CfgGetS3Value(), AT_GC_CfgGetS4Value()};

    ATLOGW("AT CMD async timeout");

    AT_KillAsyncTimerMux(engine);

    // It's a ATE bug, delete this global flag after fixing bug by ATE.
    at_ModuleSetCopsFlag(at_ModuleGetChannelSimID(at_CmdGetChannel(engine)), FALSE);

    at_CmdWrite(engine, crlf, 2);
    at_CmdWrite(engine, ERR_STR, strlen(ERR_STR));
    at_CmdWrite(engine, crlf, 2);

    // Need more work to Verify Ind List
    UINT8 nDLCI = at_CmdGetChannel(engine);
    UINT8 nSimID = AT_SIM_ID(nDLCI);

#if defined(CFW_GPRS_SUPPORT) && !defined(AT_WITHOUT_GPRS)
    AT_CLearGprsCtx(nDLCI);
#endif

    if (CFW_GetNWTimerOutFlag(nSimID))
    {
        AT_TC(g_sw_UTIL, "CFW_GetNWTimerOutFlag nSim[%d] True and set to FALSE", nSimID);
        CFW_SetNWTimerOutFlag(FALSE, nSimID);
    }

    switch (AT_GetWaitingEventOnDLCI(nSimID, nDLCI))
    {
    case EV_CFW_NW_GET_AVAIL_OPERATOR_RSP:
        // AT_TC(g_sw_UTIL, "EV_CFW_NW_GET_AVAIL_OPERATOR_RSP timeout, abort! sim=%d, channel= %d", nSimID, nDLCI);
        CFW_NwAbortListOperators(0, nSimID);
        break;
    default:
        break;
    }
    AT_DelWaitingEventOnDLCI(nDLCI);
    at_ModuleClearCmdList(engine);
    at_CmdSetLineMode(engine);
}

static void at_TimeoutAsync(void *param)
{
    // Async Command Time Over Process
    AT_CMD_ENGINE_T *engine = (AT_CMD_ENGINE_T *)param;
    at_ModuleHandleTimeout(engine);
}

BOOL AT_SetAsyncTimerMux(AT_CMD_ENGINE_T *engine, UINT32 nElapse)
{
    AT_ModuleInfo *module = at_CmdGetModule(engine);

    if (module->cmds == NULL)
    {
        ATCMDLOG(W, "AT CMD%d set async timer period=%d no cmd", at_CmdGetChannel(engine), nElapse);
        return TRUE;
    }

    ATCMDLOGS(I, TSM(2), "AT CMD%d set async timer period=%d cmd=%s", at_CmdGetChannel(engine),
              nElapse, module->cmds->desc->pName);

    if (nElapse == 0)
        nElapse = AT_DEFAULT_ELAPSE;
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutAsync, engine);
    return TRUE;
}

BOOL AT_KillAsyncTimerMux(AT_CMD_ENGINE_T *engine)
{
    at_StopCallbackTimer(at_TimeoutAsync, engine);
    return TRUE;
}

static void at_TimeoutDtmfEx(void *param)
{
    UINT8 nDLCI = (long)param;
#ifdef CHIP_HAS_AP
    extern UINT8 Speeching;
    if (!Speeching)
        return;
#endif

    //StopTone(0);
    DM_PlayTone(22, 1, 1 * 100, 6);
    COS_Sleep(100);
    DM_StopTone();
    COS_Sleep(1000);
    DM_PlayTone(22, 1, 1 * 100, 6);
    COS_Sleep(100);
    DM_StopTone();
    AT_SetDtmfTimerMuxEX(5 * 1000, nDLCI);
}

BOOL AT_SetDtmfTimerMuxEX(UINT32 nElapse, UINT8 nDLCI)
{
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutDtmfEx, (void *)(long)nDLCI);
    return TRUE;
}

BOOL AT_KillDtmfTimerMuxEX(UINT8 nDLCI)
{
    at_StopCallbackTimer(at_TimeoutDtmfEx, (void *)(long)nDLCI);
    return TRUE;
}

static void at_TimeoutDtmf(void *param)
{
    //UINT8 nDLCI = (long)param;
    //StopTone(nDLCI);
}

BOOL AT_SetDtmfTimerMux(UINT32 nElapse, UINT8 nDLCI)
{
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutDtmf, (void *)(long)nDLCI);
    return TRUE;
}

BOOL AT_KillDtmfTimerMux(UINT8 nDLCI)
{
    at_StopCallbackTimer(at_TimeoutDtmf, (void *)(long)nDLCI);
    return TRUE;
}

static void at_TimeoutExtension(void *param)
{
    extern UINT8 uExtensionNum[]; // at_cmd_cc.c

    UINT8 nDLCI = (long)param;
    UINT8 nSim = AT_SIM_ID(nDLCI);
    ATLOGSD(TSM(0), "Extension timeout: %s", uExtensionNum);

    for (UINT8 i = 1; i < AT_StrLen(uExtensionNum); i++)
    {
        CFW_CcPlayTone(uExtensionNum[i], nSim);
        COS_Sleep(100);
        CFW_CcStopTone(nSim);
        COS_Sleep(100);
    }
    AT_KillExtensionTimerMux(nDLCI);
}

BOOL AT_SetExtensionTimerMux(UINT32 nElapse, UINT8 nDLCI)
{
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutExtension, (void *)(long)nDLCI);
    return TRUE;
}

BOOL AT_KillExtensionTimerMux(UINT8 nDLCI)
{
    at_StopCallbackTimer(at_TimeoutExtension, (void *)(long)nDLCI);
    return TRUE;
}

static void at_TimeoutCced(void *param)
{
    UINT8 nDLCI = (long)param;
    AT_ReportRssi(nDLCI);
    AT_SetCCEDTimer(ATE_CCED_ELAPSE);
}

BOOL AT_SetCCEDTimer(UINT32 nElapse)
{
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutCced, (void *)0);
    return TRUE;
}

BOOL AT_KillCCEDTimer(VOID)
{
    at_StopCallbackTimer(at_TimeoutCced, (void *)0);
    return TRUE;
}

#ifdef AT_FTP_SUPPORT
static void at_TimeoutOta(void *param)
{
    // AT_FTP_Refresh(0);
    // AT_SetOTATimer(ATE_OTA_CHECK_ELAPSE);
}

BOOL AT_SetOTATimer(UINT32 nElapse)
{
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutOta, (void *)0);
    return TRUE;
}

BOOL AT_KillOTATimer(VOID)
{
    at_StopCallbackTimer(at_TimeoutOta, (void *)0);
    return TRUE;
}
#endif

#ifdef __IOT_SUPPORT__
extern int test_main(UINT8 cmd);
static void at_TimeoutEdp(void *param)
{
    test_main(3);
}

BOOL AT_SetEdpTestTimer(UINT32 nElapse)
{
    at_StartCallbackTimer(nElapse * 1000, at_TimeoutEdp, 0);
    return true;
}

BOOL AT_KillEdpTestTimer(VOID)
{
    at_StopCallbackTimer(at_TimeoutEdp, 0);
    return true;
}

VOID edp_GPIO_set(UINT8 nGpioID, UINT32 dur)
{
    AT_TC(g_sw_UART, "###set gpio %d", nGpioID);

    UINT32 nGpioRealID = HAL_GPIO_0 + nGpioID;
    HAL_APO_ID_T gpio = {{.type = HAL_GPIO_TYPE_IO,
                          .id = nGpioID}};

    hal_GpioSetOut(nGpioRealID);
    hal_GpioSet(gpio);
    COS_Sleep(dur);
    hal_GpioClr(gpio);
}
VOID edp_GPIO_clr(UINT8 nGpioID)
{
    AT_TC(g_sw_UART, "###clr gpio %d", nGpioID);

    UINT32 nGpioRealID = HAL_GPIO_0 + nGpioID;
    HAL_APO_ID_T gpio = {{.type = HAL_GPIO_TYPE_IO,
                          .id = nGpioID}};

    hal_GpioSetOut(nGpioRealID);
    hal_GpioClr(gpio);
}
#endif

int at_ModuleGetChannelSimID(int channel)
{
    AT_DISPATCH_T *dispatch = at_DispatchGetByChannel(channel);
    int dlci = at_DispatchGetDlci(dispatch);
    if (dlci < 0) // not CMUX channel
        return 0;
    return (dlci < MAX_DLC_NUM / 2) ? 0 : 1;
}

int at_ModuleGetSimDlci(int sim)
{
    return (sim == 0) ? 0 : (MAX_DLC_NUM / 2);
}

void at_ModuleGetSimCid(UINT8 *pSimId, UINT8 *pCid, UINT8 nDLCI)
{
    *pSimId = at_ModuleGetChannelSimID(nDLCI);
    *pCid = DlciGetCid(nDLCI);
}

VOID AT_Clear_CurCMD(UINT8 nDLCI)
{
    at_ModuleClearCurCmd(at_CmdGetByChannel(nDLCI));
}

UINT8 AT_GetCMSSNum(UINT8 nDLCI)
{
    AT_ModuleInfo *module = at_CmdGetModule(at_CmdGetByChannel(nDLCI));
    return module->cmssNum;
}

VOID AT_SetCMSSNum(UINT8 nDLCI, UINT8 num)
{
    AT_ModuleInfo *module = at_CmdGetModule(at_CmdGetByChannel(nDLCI));
    module->cmssNum = num;
}

BOOL AT_IsAsynCmdAvailable(UINT8 *pName, UINT32 uCmdStamp, UINT8 nDLCI)
{
    AT_CMD_ENGINE_T *engine = at_CmdGetByChannel(nDLCI);
    AT_ModuleInfo *module = at_CmdGetModule(engine);
    if (module->cmds != NULL &&
        AT_StrCmp(pName, module->cmds->desc->pName) == 0 /* &&
        module->cmds->uCmdStamp == uCmdStamp*/)
        return TRUE;
    return FALSE;
}

BOOL PM_SetBatteryChargeMode(UINT8 nMode)
{
    return FALSE;
}

typedef struct
{
    AT_CMD_ENGINE_T *engine;
    uint8_t *data;
    uint32_t data_len;
} ASYNC_WRITE_T;

static void write_uart_at_AT(void *param)
{
    ASYNC_WRITE_T *write = (ASYNC_WRITE_T *)param;
    at_CmdWrite(write->engine, write->data, write->data_len);
    AT_FREE(write->data);
    AT_FREE(write);
}

void AT_AsyncWriteUart(AT_CMD_ENGINE_T *engine, const uint8_t *data, uint32_t data_len)
{
    ASYNC_WRITE_T *write = (ASYNC_WRITE_T *)AT_MALLOC(sizeof(ASYNC_WRITE_T));
    write->engine = engine;
    write->data = (uint8_t *)AT_MALLOC(data_len);
    write->data_len = data_len;
    memcpy(write->data, data, data_len);
    at_TaskCallback(write_uart_at_AT, write);
}

void AT_WriteUart(const uint8_t *data, unsigned length)
{
    at_CmdDirectWrite(data, length);
}

void AT_KillAsyncTimer(void)
{
    // TODO:
}

bool at_SetUrcChannel(int channel)
{
    gAtGlobal.urcChannel = channel;
    return true;
}

int at_GetUrcChannel() { return gAtGlobal.urcChannel; }
AT_MODULE_INIT_STATUS_T at_ModuleGetInitResult(void) { return gAtGlobal.uiModuleInitStatus; }
void at_ModuleSetInitResult(AT_MODULE_INIT_STATUS_T uiStatus) { gAtGlobal.uiModuleInitStatus = uiStatus; }
void at_ModuleSetSmsReady(bool ready) { gAtGlobal.smsReady = ready; }
bool at_ModuleGetSmsReady(void) { return gAtGlobal.smsReady; }
void at_ModuleSetPbkReady(bool ready) { gAtGlobal.pbkReady = ready; }
bool at_ModuleGetPbkReady(void) { return gAtGlobal.pbkReady; }
void at_ModuleSetCopsFlag(UINT8 sim, bool flag) { gAtGlobal.copsFlag[sim] = flag; }
bool at_ModuleGetCopsFlag(UINT8 sim) { return gAtGlobal.copsFlag[sim]; }

void at_ModuleSetRI(bool hi) { at_ModuleToggleGpio(gAtGlobal.riGpio, hi); }
void at_ModuleSetDCD(bool hi) { at_ModuleToggleGpio(gAtGlobal.dcdGpio, hi); }
void at_ModuleSetDSR(bool hi) { at_ModuleToggleGpio(gAtGlobal.dsrGpio, hi); }
