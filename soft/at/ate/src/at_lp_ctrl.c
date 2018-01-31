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

#include "at_lp_ctrl.h"
#include "at_device.h"
#include "at_utils.h"
#include "hal_sys.h"
#include "hal_gpio.h"
#include "hal_host.h"
#include "cos.h"
#include "event.h"

#ifdef AT_MODULE_SUPPORT_SLEEP
#define DTR_GPIO (HAL_GPIO_0 + _USE_AT_OC_GPIO_)
#else
#define DTR_GPIO (HAL_GPIO_NONE)
#endif
#define DTR_HIGH_FOR_LP true
#define DEVICE_MAX 2
#define DTR_STABLE_PERIOD 500
#define RX_TIMEOUT_PERIOD 500
#define LP_EVENT(lp) hal_HstSendEvent(0xaa000020 | (lp))

struct AT_LP_CTRL_CONTEXT
{
    AT_LP_CTRL_MODE_T mode;
    bool lpcurr;
    bool lpnext;
    AT_DEVICE_T *devices[DEVICE_MAX];
};

// singleton global variable
static struct AT_LP_CTRL_CONTEXT gLpCtrlCtx;

// =============================================================================
// at_LpCtrlSetDeviceSleep
// =============================================================================
static void at_LpCtrlSetDeviceSleep(bool lp)
{
    if (lp == gLpCtrlCtx.lpcurr)
        return;

    LP_EVENT(lp);
    gLpCtrlCtx.lpcurr = lp;
    bool wakeupEnabled = (gLpCtrlCtx.mode == AT_LP_CTRL_BY_RX) ? true : false;
    for (int n = 0; n < DEVICE_MAX; n++)
    {
        if (gLpCtrlCtx.devices[n] != NULL)
        {
            if (lp)
                at_DeviceSleep(gLpCtrlCtx.devices[n], wakeupEnabled);
            else
                at_DeviceWakeup(gLpCtrlCtx.devices[n]);
        }
    }
}

// =============================================================================
// at_LpCtrlTimeout
// =============================================================================
static void at_LpCtrlTimeout(void *param)
{
    at_LpCtrlSetDeviceSleep(gLpCtrlCtx.lpnext);
}

// =============================================================================
// at_LpCtrlDtrUpdateLevel
// =============================================================================
static void at_LpCtrlDtrUpdateLevel(void)
{
    if (DTR_GPIO == HAL_GPIO_NONE)
        return;

    UINT32 gpio = hal_GpioGet(DTR_GPIO);
    gLpCtrlCtx.lpnext = ((DTR_HIGH_FOR_LP && gpio != 0) || (!DTR_HIGH_FOR_LP && gpio == 0));
}

// =============================================================================
// at_LpCtrlDtrGpioIrq
// =============================================================================
static void at_LpCtrlDtrGpioIrq(UINT32 param)
{
    at_LpCtrlDtrUpdateLevel();
    at_StartCallbackTimer(DTR_STABLE_PERIOD, at_LpCtrlTimeout, NULL);
}

// =============================================================================
// at_LpCtrlDtrGpioOpen
// =============================================================================
static void at_LpCtrlDtrGpioOpen(void)
{
    if (DTR_GPIO == HAL_GPIO_NONE)
        return;

    HAL_GPIO_CFG_T cfg;
    // use edge interrupt for both cases
    cfg.direction = HAL_GPIO_DIRECTION_INPUT;
    cfg.irqMask.rising = TRUE;
    cfg.irqMask.falling = TRUE;
    cfg.irqMask.debounce = TRUE;
    cfg.irqMask.level = FALSE;
    cfg.irqHandler = at_LpCtrlDtrGpioIrq;
    cfg.irqParam = NULL;
    hal_GpioOpen(DTR_GPIO, &cfg);
}

// =============================================================================
// at_LpCtrlInputReceived
// =============================================================================
void at_LpCtrlInputReceived(void)
{
    if (gLpCtrlCtx.mode != AT_LP_CTRL_BY_RX)
        return;

    at_LpCtrlSetDeviceSleep(false);
    at_StartCallbackTimer(RX_TIMEOUT_PERIOD, at_LpCtrlTimeout, NULL);
}

// =============================================================================
// at_LpCtrlOutputRequest
// =============================================================================
bool at_LpCtrlOutputRequest(void)
{
    if (gLpCtrlCtx.mode == AT_LP_CTRL_BY_DTR)
        return !gLpCtrlCtx.lpcurr;

    at_LpCtrlSetDeviceSleep(false);
    if (gLpCtrlCtx.mode == AT_LP_CTRL_BY_RX)
        at_StartCallbackTimer(RX_TIMEOUT_PERIOD, at_LpCtrlTimeout, NULL);
    return true;
}

// =============================================================================
// at_LpCtrlSetMode
// =============================================================================
bool at_LpCtrlSetMode(AT_LP_CTRL_MODE_T mode)
{
    if (mode == gLpCtrlCtx.mode)
        return true;

    at_StopCallbackTimer(at_LpCtrlTimeout, NULL);
    if (gLpCtrlCtx.mode == AT_LP_CTRL_BY_DTR && DTR_GPIO != HAL_GPIO_NONE)
        hal_GpioClose(DTR_GPIO);

    gLpCtrlCtx.mode = mode;

    if (mode == AT_LP_CTRL_BY_DTR)
    {
        if (DTR_GPIO != HAL_GPIO_NONE)
        {
            uint32_t sc = hal_SysEnterCriticalSection();
            at_LpCtrlDtrGpioOpen();
            at_LpCtrlDtrUpdateLevel();
            at_StartCallbackTimer(DTR_STABLE_PERIOD, at_LpCtrlTimeout, NULL);
            hal_SysExitCriticalSection(sc);
        }
        else
        {
            gLpCtrlCtx.lpnext = false;
            at_LpCtrlSetDeviceSleep(false);
        }
    }
    else if (mode == AT_LP_CTRL_BY_RX)
    {
        gLpCtrlCtx.lpnext = true;
        at_StartCallbackTimer(RX_TIMEOUT_PERIOD, at_LpCtrlTimeout, NULL);
    }
    else
    {
        gLpCtrlCtx.lpnext = false;
        at_LpCtrlSetDeviceSleep(false);
    }
    return true;
}

// =============================================================================
// at_LpCtrlGetMode
// =============================================================================
AT_LP_CTRL_MODE_T at_LpCtrlGetMode(void)
{
    return gLpCtrlCtx.mode;
}

// =============================================================================
// at_LpCtrlSetDevice
// =============================================================================
bool at_LpCtrlSetDevice(AT_DEVICE_T *device)
{
    for (int n = 0; n < DEVICE_MAX; n++)
    {
        if (gLpCtrlCtx.devices[n] == device)
            return true;
        if (gLpCtrlCtx.devices[n] == NULL)
        {
            gLpCtrlCtx.devices[n] = device;
            return true;
        }
    }
    return false;
}

// =============================================================================
// at_LpCtrlUnsetDevice
// =============================================================================
void at_LpCtrlUnsetDevice(AT_DEVICE_T *device)
{
    for (int n = 0; n < DEVICE_MAX; n++)
    {
        if (gLpCtrlCtx.devices[n] == device)
        {
            gLpCtrlCtx.devices[n] = NULL;
            break;
        }
    }
}

// =============================================================================
// at_LpCtrlInit
// =============================================================================
void at_LpCtrlInit(void)
{
    gLpCtrlCtx.mode = AT_LP_CTRL_DISABLE;
    gLpCtrlCtx.lpcurr = false;
    gLpCtrlCtx.lpnext = false;
}
