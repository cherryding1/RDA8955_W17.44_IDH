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






























#if 1
#include <cswtype.h>
#include <drv.h>
#include "chip_id.h"

#include "pmd_m.h"
#include "hal_gpio.h"
// Interrupt handling is of a different type
#define drv_gipo_handler(a) drv_gipo_handler()

#include "hal_pwm.h"
#include "hal_ebc.h"
#include "hal_sys.h"

#include "csw_csp.h"

#include <drv_prv.h>
#include <errorcode.h>
#include <cos.h>
#include <base_prv.h>
#include <cos.h>

UINT32 gpio_ind_mask = 0;

extern void _sxr_StartFunctionTimer (u32 Period, void (*Function)(void *), void *Param, u16 Id, u8 I2d);
extern BOOL COS_SendSysEvent(
    COS_EVENT *pEvent,
    UINT16 nOption
);

//jinsong add it in 060807
extern UINT8 uLcdLevel;
extern UINT8 uKeyLevel;
//end by jinsong

UINT32 GPIO_Debonc[32] ;
// TODO Get initial state from the Target module.
HAL_GPIO_DIRECTION_T g_gpioDirection[HAL_GPIO_QTY];

void drv_gipo_handler(UINT16 status);

UINT32 DRV_GpioSetMode (
    UINT8 nGpio,
    UINT8 nMode
)
{
    // Nobody in is right mind would do something like that
    // at this level. Muxing is dealt with and configure
    // by the target module. If some muxing to GPIO have to
    // be done for very special cases (Power saving, or configure
    // a module in SPI by forcing some lines) that have to
    // handled within the driver, not there
    return ERR_SUCCESS;
}

UINT32 DRV_GpioSetDirection( UINT8 nGpio, UINT8 nDirection)
{
    if(nGpio > DRV_GPIO_MAX)
        return ERR_DRV_INVALID_PARAMETER;
    if (nDirection == 1)
    {
        hal_GpioSetOut(HAL_GPIO_0 + nGpio);
        g_gpioDirection[nGpio] = HAL_GPIO_DIRECTION_OUTPUT;
    }
    else
    {
        hal_GpioSetIn(HAL_GPIO_0 + nGpio);
        g_gpioDirection[nGpio] = HAL_GPIO_DIRECTION_INPUT;
    }
    return ERR_SUCCESS;
}

UINT32 DRV_GpioGetDirection(UINT8 nGpio, UINT8 *pDirection)
{
    if(nGpio > DRV_GPIO_MAX)

        return ERR_DRV_INVALID_PARAMETER;
    if (g_gpioDirection[nGpio] == HAL_GPIO_DIRECTION_OUTPUT)
    {
        *pDirection = 1;
    }
    else
    {
        *pDirection = 0;
    }

    return ERR_SUCCESS;
}

UINT32 DRV_GpioSetLevel(UINT8 nGpio, UINT8 nLevel)
{
    if(nGpio > DRV_GPIO_MAX)
        return ERR_DRV_INVALID_PARAMETER;
    HAL_APO_ID_T gpio = {{
            .type = HAL_GPIO_TYPE_IO,
            .id = nGpio
        }
    };
    if (nLevel == 1)
    {
        hal_GpioSet(gpio);
    }
    else
    {
        hal_GpioClr(gpio);
    }

    return ERR_SUCCESS;
}

UINT32 DRV_GpioGetLevel(UINT8 nGgpio, UINT8 *nLevel)
{

    if(nGgpio > DRV_GPIO_MAX)
        return ERR_DRV_INVALID_PARAMETER;

    *nLevel = hal_GpioGet(HAL_GPIO_0 + nGgpio);
    return ERR_SUCCESS;
}
#define HAL_GPIO_RISE_DISABLE(n) (~(((n>>8) & 0xff)))
#define HAL_GPIO_FALL_DISABLE(n) (~(((n) & 0xff00)))

UINT32 DRV_GpioSetEdgeIndication( UINT8 nGpio, UINT8 nEdge, UINT32 nDebonce )
{

    if((nGpio < DRV_GPIO_INT_FIRST) || (nGpio > DRV_GPIO_INT_LAST))
        return ERR_DRV_INVALID_PARAMETER;
    GPIO_Debonc[nGpio] = nDebonce;
    if(nDebonce < 0)
        GPIO_Debonc[nGpio] = 50;

    DRV_GpioSetDirection(nGpio, DRV_GPIO_IN);
    HAL_GPIO_CFG_T config;
    config.direction = HAL_GPIO_DIRECTION_INPUT;
    config.value = FALSE;
    config.irqMask.falling = (nEdge == DRV_GPIO_EDGE_F || nEdge == DRV_GPIO_EDGE_RF);
    config.irqMask.rising = (nEdge == DRV_GPIO_EDGE_R || nEdge == DRV_GPIO_EDGE_RF);
    config.irqMask.debounce = (nDebonce != 0);
    config.irqMask.level = TRUE; // FIXME ?
    config.irqHandler = drv_gipo_handler;

    hal_GpioResetIrq(HAL_GPIO_0 + nGpio);
    hal_GpioOpen(HAL_GPIO_0 + nGpio, &config);

    return ERR_SUCCESS;
}

#define GPIO_INT_TIMER_ID   0x55555555  //for GPIO debounc

typedef struct
{
    UINT8 number;
    UINT8 level;
} GPIO_Debounc;

#if 1
void drv_gipo_handler(UINT16 status)
{
    // Modem2G API handles Interrupt on a per GPIO basis,
    // and debouncing is done on hardware.
    UINT8 number = 0;
    UINT8 leve;

    COS_EVENT ev;


    ///@todo FIXME even dirtier than bellow
#define HAL_GPIO_ADDRESS (0xA0B83000)

    // FIXME Dirtiest thing ever:
    // I don't want to add a get status function in the API, nor
    // give the number of the GPIO to the handler. The HAL GPIO
    // driver allow higher level things than that...
    UINT32 status = *((UINT32 *)HAL_GPIO_ADDRESS + 9);
    for(number = 0; number < 32; number++)
    {
        if(status & (1 << number))
        {
            // Does this behaves properly ?
            hal_GpioResetIrq(1 << number);
            DRV_GpioGetLevel(number, &leve);
            ev.nEventId = EV_CSW_PRV_GPIO;
            ev.nParam1  = number;
            ev.nParam2  = leve;
            COS_SendSysEvent(&ev, 0);
        }
    }
}
#endif

////////////////exterior interrupter//////////////////////////
UINT32 DRV_SetPwl1( UINT8 nLevel)
{
    hal_pwl1_Threshold(nLevel);

    uKeyLevel = nLevel;
    return ERR_SUCCESS;
}

//chenhe add for greenstone
UINT32 DRV_SetKeyBacklight( UINT8 nLevel)
{
    pmd_SetLevel(PMD_LEVEL_KEYPAD, nLevel);
    uKeyLevel = nLevel;
    return ERR_SUCCESS;
}

UINT32 DRV_SetPwl0( UINT8 nLevel)
{
    hal_pwl0_Threshold(nLevel);
    uLcdLevel = nLevel;
    return ERR_SUCCESS;
}

/* for test backlight
UINT32 Test_SetPwl0( UINT8 nLevel)
{
    hal_pwl0_Threshold(nLevel);
       uLcdLevel = nLevel;

    return ERR_SUCCESS;
}
*/



////////////////EBC/////////////////
PVOID DRV_MemorySetupCS( UINT8 nCS, DRV_MEM_CFG   *pMemCfg)
{
    // FIXME This function shouldn't be in CSW. The LCD drivers must
    // not be in MMI, but in EDRV. Using the provided LCDD API, or
    // equivalent for other kind of external chip, there is no need
    // to manually configure the EBC. The CS copening is done in the
    // driver (eg lcdd_Open), and MUST stay at that low level, where
    // the configuration is got from the Target module. HAL must be
    // the only API to propose a Ebc configure function taking
    // EBC config parameters. This function must be used only in
    // driver code, where those parameters are obtained from target.
    PVOID ebc_cs;
    CONST HAL_EBC_CS_CFG_T csConfig =
    {
        .time = {
            .rdLatHalfMode  =  0,
            .rdLat          = 10,
            .oedlyHalfMode  =  0,
            .oedly          =  3,
            .pageLat        =  0,
            .wrLatHalfMode  =  0,
            .wrLat          = 10,
            .wedlyHalfMode  =  0,
            .wedly          =  3,
            .relax          =  2,
            .forceRelax     =  1
        },
        .mode = {
            .csEn           =  1,
            .polarity       =  0,
            .admuxMode      =  0,
            .writeAllow     =  1,
            .wbeMode        =  0,
            .bedlyMode      =  0,
            .waitMode       =  0,
            .pageSize       =  0,
            .pageMode       =  0,
            .writePageMode  =  0,
            .burstMode      =  0,
            .writeBurstMode =  0,
            .advAsync       =  0,
            .advwePulse     =  0,
            .admuxDly       =  0,
            .writeSingle    =  0,
            .writeHold      =  0
        }
    };
    ebc_cs = hal_EbcCsOpen(nCS, &csConfig);
    hal_ResetOut(FALSE);

    return ebc_cs;
}

UINT32 DRV_ResetOut( BOOL pRest)
{
    hal_ResetOut(pRest);

    return ERR_SUCCESS;
}

UINT32 DRV_MemoryDisableCS( UINT8  nCS)
{
    hal_EbcCsClose(nCS);
    return ERR_SUCCESS;
}

#endif

