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


#include "syscmdsp_hdlr.h"
#include "syscmdsp_debug.h"
#include "syscmds_gpio_cmd_id.h"

#include "string.h"
#include "hal_gpio.h"


// =============================================================================
// FORWARD DECLARATIONS
// =============================================================================

PRIVATE VOID syscmds_GpioHandler_0(VOID);
PRIVATE VOID syscmds_GpioHandler_1(VOID);
PRIVATE VOID syscmds_GpioHandler_2(VOID);
PRIVATE VOID syscmds_GpioHandler_3(VOID);
PRIVATE VOID syscmds_GpioHandler_4(VOID);
PRIVATE VOID syscmds_GpioHandler_5(VOID);
PRIVATE VOID syscmds_GpioHandler_6(VOID);
PRIVATE VOID syscmds_GpioHandler_7(VOID);


// =============================================================================
// MACROS
// =============================================================================


// =============================================================================
// TYPES
// =============================================================================

typedef struct
{
    BOOL risingIrq;
    BOOL fallingIrq;
    BOOL debounce;
    BOOL levelIrq;
} SYSCMDS_GPIO_IRQ_MASK_T;

typedef struct
{
    UINT8 id;
    BOOL isOut;
    BOOL initOutValue;
    SYSCMDS_GPIO_IRQ_MASK_T irqMask;
} SYSCMDS_GPIO_OPEN_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE CONST HAL_GPIO_IRQ_HANDLER_T g_syscmdsGpioHandler[8] =
{
    syscmds_GpioHandler_0,
    syscmds_GpioHandler_1,
    syscmds_GpioHandler_2,
    syscmds_GpioHandler_3,
    syscmds_GpioHandler_4,
    syscmds_GpioHandler_5,
    syscmds_GpioHandler_6,
    syscmds_GpioHandler_7,
};


// =============================================================================
//  FUNCTIONS
// =============================================================================

PROTECTED BOOL syscmds_HandleGpioCmd(SYSCMDS_RECV_MSG_T *pRecvMsg)
{
    BOOL result = TRUE;

    SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,
                  "syscmds_HandleGpioCmd: msgId=0x%04x", pRecvMsg->msgId);

    switch (pRecvMsg->msgId)
    {
        case AP_GPIO_CMD_OPEN:
        {
            SYSCMDS_GPIO_OPEN_T open;
            HAL_GPIO_GPIO_ID_T gpio;
            HAL_GPIO_CFG_T cfg;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(open));

            memcpy(&open, pRecvMsg->param, sizeof(open));

            gpio = HAL_GPIO_0 + open.id;
            cfg.direction = open.isOut ?
                            HAL_GPIO_DIRECTION_OUTPUT : HAL_GPIO_DIRECTION_INPUT;
            cfg.value = open.initOutValue;
            cfg.irqMask.rising = open.irqMask.risingIrq;
            cfg.irqMask.falling = open.irqMask.fallingIrq;
            cfg.irqMask.debounce = open.irqMask.debounce;
            cfg.irqMask.level = open.irqMask.levelIrq;
            if (cfg.irqMask.rising || cfg.irqMask.falling)
            {
                cfg.irqHandler = g_syscmdsGpioHandler[open.id % 8] ;
            }
            else
            {
                cfg.irqHandler = NULL;
            }

            hal_GpioOpen(gpio, &cfg);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GPIO_CMD_CLOSE:
        {
            UINT8 id;
            HAL_GPIO_GPIO_ID_T gpio;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(id));

            memcpy(&id, pRecvMsg->param, sizeof(id));

            gpio = HAL_GPIO_0 + id;

            hal_GpioClose(gpio);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GPIO_CMD_SET_IO:
        {
            UINT8 id;
            BOOL isOut;
            HAL_GPIO_GPIO_ID_T gpio;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(id) + sizeof(isOut));

            memcpy(&id, pRecvMsg->param, sizeof(id));
            memcpy(&isOut, (UINT8 *)pRecvMsg->param + sizeof(id), sizeof(isOut));

            gpio = HAL_GPIO_0 + id;

            if (isOut)
            {
                hal_GpioSetOut(gpio);
            }
            else
            {
                hal_GpioSetIn(gpio);
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GPIO_CMD_GET_VALUE:
        {
            UINT8 id;
            HAL_GPIO_GPIO_ID_T gpio;
            UINT8 value;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(id));

            memcpy(&id, pRecvMsg->param, sizeof(id));

            gpio = HAL_GPIO_0 + id;

            value = hal_GpioGet(gpio);

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, &value, sizeof(value));
        }
        break;
        case AP_GPIO_CMD_SET_VALUE:
        {
            UINT8 id;
            UINT8 value;
            HAL_APO_ID_T apo;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(id) + sizeof(value));

            memcpy(&id, pRecvMsg->param, sizeof(id));
            memcpy(&value, (UINT8 *)pRecvMsg->param + sizeof(id), sizeof(value));

            apo.gpioId = HAL_GPIO_0 + id;

            if (value)
            {
                hal_GpioSet(apo);
            }
            else
            {
                hal_GpioClr(apo);
            }

            syscmds_Reply(pRecvMsg->modId, pRecvMsg->reqId, 0,
                          pRecvMsg->msgId, NULL, 0);
        }
        break;
        case AP_GPIO_CMD_ENABLE_IRQ:
        {
            UINT8 id;
            SYSCMDS_GPIO_IRQ_MASK_T irqMask;
            HAL_GPIO_GPIO_ID_T gpio;
            HAL_GPIO_IRQ_MASK_T irqCfg;

            syscmds_CheckParamLen(pRecvMsg->modId, pRecvMsg->msgId,
                                  pRecvMsg->paramLen, sizeof(id) + sizeof(irqMask));

            memcpy(&id, pRecvMsg->param, sizeof(id));
            memcpy(&irqMask, (UINT8 *)pRecvMsg->param + sizeof(id), sizeof(irqMask));

            gpio = HAL_GPIO_0 + id;
            irqCfg.rising = irqMask.risingIrq;
            irqCfg.falling = irqMask.fallingIrq;
            irqCfg.debounce = irqMask.debounce;
            irqCfg.level = irqMask.levelIrq;

            hal_GpioIrqSetMask(gpio, &irqCfg);
            hal_GpioResetIrq(gpio);

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

PRIVATE VOID syscmds_GpioHandler(UINT8 id)
{
    HAL_GPIO_GPIO_ID_T gpio;
    UINT8 ret[2];
    HAL_GPIO_IRQ_MASK_T irqCfg;

    gpio = HAL_GPIO_0 + id;

    // Disable this IRQ
    irqCfg.rising = FALSE;
    irqCfg.falling = FALSE;
    irqCfg.debounce = FALSE;
    irqCfg.level = FALSE;
    hal_GpioIrqSetMask(gpio, &irqCfg);

    ret[0] = id;
    ret[1] = hal_GpioGet(gpio);

    SYSCMDS_TRACE(SYSCMDS_INFO_TRC, 0,
                  "syscmds_GpioHandler: id=%d, value=%d", ret[0], ret[1]);

    syscmds_IrqSend(SYSCMDS_MOD_ID_GPIO, MODEM_GPIO_CMD_IRQ_TRIGGER,
                    ret, sizeof(ret));
}

PRIVATE VOID syscmds_GpioHandler_0(VOID)
{
    syscmds_GpioHandler(0);
}

PRIVATE VOID syscmds_GpioHandler_1(VOID)
{
    syscmds_GpioHandler(1);
}

PRIVATE VOID syscmds_GpioHandler_2(VOID)
{
    syscmds_GpioHandler(2);
}

PRIVATE VOID syscmds_GpioHandler_3(VOID)
{
    syscmds_GpioHandler(3);
}

PRIVATE VOID syscmds_GpioHandler_4(VOID)
{
    syscmds_GpioHandler(4);
}

PRIVATE VOID syscmds_GpioHandler_5(VOID)
{
    syscmds_GpioHandler(5);
}

PRIVATE VOID syscmds_GpioHandler_6(VOID)
{
    syscmds_GpioHandler(6);
}

PRIVATE VOID syscmds_GpioHandler_7(VOID)
{
    syscmds_GpioHandler(7);
}

