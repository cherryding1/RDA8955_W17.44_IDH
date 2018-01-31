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


#include "cs_types.h"

#include "chip_id.h"

#include "global_macros.h"
#include "gpio.h"

#include "hal_gpio.h"
#include "halp_gpio.h"
#include "hal_tcu.h"
#include "hal_mem_map.h"
#include "halp_debug.h"


// =============================================================================
// MACROS
// =============================================================================

// GPIO use for charger detect
#ifdef CHIP_GPIO_NUM_CHARGER_DETECT
#define HAL_GPIO_NUM_CHARGER_DETECT CHIP_GPIO_NUM_CHARGER_DETECT
#else
#error "Charger detect GPIO must be integrated to this chip def"
#endif // CHIP_GPIO_NUM_CHARGER_DETECT

#define HAL_GPIO_SET_NUM_CHARGER_DETECT2(X) (HAL_GPIO_##X)
#define HAL_GPIO_SET_NUM_CHARGER_DETECT(X) HAL_GPIO_SET_NUM_CHARGER_DETECT2(X)

#define HAL_GPIO_CHARGER_DETECT HAL_GPIO_SET_NUM_CHARGER_DETECT(HAL_GPIO_NUM_CHARGER_DETECT)

#define SWAP_8808S_GPIO_6_AND_13(gpio)
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)
#ifndef USE_8808S_GPIO_6_IRQ
#undef SWAP_8808S_GPIO_6_AND_13
#define SWAP_8808S_GPIO_6_AND_13(gpio) \
    if (hal_SysGetChipId(HAL_SYS_CHIP_PROD_ID) != HAL_SYS_CHIP_PROD_ID_8808) \
    { \
        if (gpio == HAL_GPIO_6) { gpio = HAL_GPIO_13; } \
        else if (gpio == HAL_GPIO_13) { gpio = HAL_GPIO_6; } \
    }
#endif // USE_8808S_GPIO_6_IRQ
#endif // (CHIP_ASIC_ID == CHIP_ASIC_ID_8808)

#ifdef CHIP_HAS_AP
#define SWAP_FP_GPIO_24_AND_25(gpio)
#else
#define SWAP_FP_GPIO_24_AND_25(gpio) \
    if (gpio == HAL_GPIO_24) { gpio = HAL_GPIO_25; } \
    else if (gpio == HAL_GPIO_25) { gpio = HAL_GPIO_24; }
#endif


// =============================================================================
// GLOBAL VARIABLE
// =============================================================================

PRIVATE CONST CHAR* gc_errNonIntGpio
    = "Trying to set interrupt on non interruptible gpio (not between 0 and 7).";

PRIVATE CONST CHAR* gc_errWrongGpioType
    = "Wrong gpio type";

PRIVATE struct {
    HAL_GPIO_IRQ_HANDLER_T irqHandler;
    UINT32 irqParam;
} g_halGpioRegistry[8];


// =============================================================================
// FUNCTIONS
// =============================================================================

// =============================================================================
// hal_GpioOpen
// -----------------------------------------------------------------------------
/// Opens the a GPIO and sets its initial configuration. The GPIO's state can
/// then be changed by the others functions of this API.
/// It also sets the corresponding pin in GPIO mode.
///
/// @param gpio Identifier of the GPIO to open
/// @param cfg Initial configuration of this GPIO.
// =============================================================================
PUBLIC VOID hal_GpioOpen(HAL_GPIO_GPIO_ID_T gpio, CONST HAL_GPIO_CFG_T* cfg)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    //Set direction and value (if direction is out)
    if(cfg->direction == HAL_GPIO_DIRECTION_INPUT)
    {
        hwp_gpio->gpio_oen_set_in = GPIO_OEN_SET_IN(1 << apo.id);
    }
    else
    {
        hwp_gpio->gpio_oen_set_out = GPIO_OEN_SET_OUT(1 << apo.id);

        if(cfg->value)
        {
            hwp_gpio->gpio_set = GPIO_GPIO_SET(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpio_clr = GPIO_GPIO_CLR(1 << apo.id);
        }
    }

    //Set the irq handler
    hal_GpioIrqSetHandler(gpio, cfg->irqHandler, cfg->irqParam);

    //Set mask
    hal_GpioIrqSetMask(gpio, &cfg->irqMask);
}



// =============================================================================
// hal_GpioClose
// -----------------------------------------------------------------------------
/// Closes a GPIO driver. The GPIO is returned to its reset state
/// @param gpio GPIO to close.
// =============================================================================
PUBLIC VOID hal_GpioClose(HAL_GPIO_GPIO_ID_T gpio)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    hwp_gpio->gpio_oen_set_in = GPIO_OEN_SET_IN(1 << apo.id);
    hwp_gpio->gpint_ctrl_clr = GPIO_GPINT_R_CLR(1 << apo.id)|
                               GPIO_GPINT_F_CLR(1 << apo.id)|
                               GPIO_DBN_EN_CLR(1 << apo.id)|
                               GPIO_GPINT_MODE_CLR(1 << apo.id);
    hal_GpioResetIrq(gpio);
}



// =============================================================================
// hal_GpioSetOut
// -----------------------------------------------------------------------------
/// This function sets one GPIO to output and leaves the others'directions
/// unchanged.
// =============================================================================
PUBLIC VOID hal_GpioSetOut(HAL_GPIO_GPIO_ID_T gpio)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);

    hwp_gpio->gpio_oen_set_out = 1 << apo.id;
}



// =============================================================================
// hal_GpioSetIn
// -----------------------------------------------------------------------------
/// This function sets one GPIO to input and leaves the others'directions
/// unchanged.
// =============================================================================
PUBLIC VOID hal_GpioSetIn(HAL_GPIO_GPIO_ID_T gpio)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);

    hwp_gpio->gpio_oen_set_in = 1 << apo.id;
}



// =============================================================================
// hal_GpioGet
// -----------------------------------------------------------------------------
/// This function reads a GPIO, and returns its value. It
/// returns the input value of GPIO configured as input and the output value
/// for GPIO configured as output.
///
/// @return The input value for a GPIO configured as input and
/// the output value for a GPIO configured as output.
// =============================================================================
PUBLIC UINT32 hal_GpioGet(HAL_GPIO_GPIO_ID_T gpio)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return 0;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);

    if (hwp_gpio->gpio_val & (1 << apo.id))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


// =============================================================================
// hal_GpioSet
// -----------------------------------------------------------------------------
/// This function sets to '1' the value of the given GPIO, GPO or TCO and leaves
/// the others unchanged
/// @param apo A GPIO, a GPO or a TCO set to '1', others being unchanged.
// =============================================================================
VOID HAL_FUNC_INTERNAL hal_GpioSet(HAL_APO_ID_T apo)
{
    SWAP_8808S_GPIO_6_AND_13(apo.gpioId);
    SWAP_FP_GPIO_24_AND_25(apo.gpioId);

    switch (apo.type)
    {
        case HAL_GPIO_TYPE_NONE:
            break;
        case HAL_GPIO_TYPE_IO:
            hwp_gpio->gpio_set = 1 << apo.id;
            break;
        case HAL_GPIO_TYPE_O:
            hwp_gpio->gpo_set = 1 << apo.id;
            break;
        case HAL_GPIO_TYPE_TCO:
            hal_TcuSetTco( apo.id );
            break;
        default:
            HAL_ASSERT(FALSE, "Trying to use an invalid type of gpio %08x", apo.gpioId);
            break;
    }
}



// =============================================================================
// hal_GpioClr
// -----------------------------------------------------------------------------
/// This function sets to '0' the value of the given GPIO, GPO or TCO and leaves
/// the others unchanged
/// @param apo A GPIO, a GPO or a TCO set to '0', others being unchanged.
// =============================================================================
VOID HAL_FUNC_INTERNAL hal_GpioClr(HAL_APO_ID_T apo)
{
    SWAP_8808S_GPIO_6_AND_13(apo.gpioId);
    SWAP_FP_GPIO_24_AND_25(apo.gpioId);

    switch (apo.type)
    {
        case HAL_GPIO_TYPE_NONE:
            break;
        case HAL_GPIO_TYPE_IO:
            hwp_gpio->gpio_clr = 1 << apo.id;
            break;
        case HAL_GPIO_TYPE_O:
            hwp_gpio->gpo_clr = 1 << apo.id;
            break;
        case HAL_GPIO_TYPE_TCO:
            hal_TcuClrTco( apo.id );
            break;
        default:
            HAL_ASSERT(FALSE, "Trying to use an invalid type of gpio %08x", apo.gpioId);
            break;
    }
}



// =============================================================================
// hal_GpioResetIrq
// -----------------------------------------------------------------------------
/// Clears the gpio IRQ,
/// This function should only be called to avoid unwanted IRQ. \n
/// For regular usage, IRQ clearing is done automatically by the driver. \n
/// Rise and fall cause are cleared simultaneously
///
/// @param gpio A GPIO whose interrupt cause is cleared.
// ==========================================================================
PUBLIC VOID hal_GpioResetIrq(HAL_GPIO_GPIO_ID_T gpio)
{
    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    hwp_gpio->int_clr = 1 << apo.id;
}


// =============================================================================
// hal_GpioIrqSetHandler
// -----------------------------------------------------------------------------
/// Sets the user IRQ handler called when an interruption is triggerd on a
/// given GPIO.
///
/// @param gpio The GPIO whose interrupt will trig the handler
/// @param handler The user function called on an interruption on this GPIO.
// =============================================================================
PUBLIC VOID hal_GpioIrqSetHandler(HAL_GPIO_GPIO_ID_T gpio, HAL_GPIO_IRQ_HANDLER_T handler, UINT32 param)
{
    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    g_halGpioRegistry[apo.id].irqHandler = handler ;
    g_halGpioRegistry[apo.id].irqParam = param ;
}

// =============================================================================
// hal_GpioIrqSetMask
// -----------------------------------------------------------------------------
/// Set the condition under which an interrupt is triggered on a GPIO
/// @param gpio The GPIO
/// @param cfg Pointer to the configuration defining the condition
/// under which the interrupts are triggered, ie the interrupt mask.
// =============================================================================
PUBLIC VOID hal_GpioIrqSetMask(HAL_GPIO_GPIO_ID_T gpio, CONST HAL_GPIO_IRQ_MASK_T* cfg)
{
    UINT32 ctrlSet = 0;
    UINT32 ctrlClr = 0;
    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    hal_GpioResetIrq(gpio);

    if(cfg->rising)
    {
        ctrlSet = GPIO_GPINT_R_SET(1 << apo.id);
    }
    else
    {
        ctrlClr = GPIO_GPINT_R_CLR(1 << apo.id);
    }

    if(cfg->falling)
    {
        ctrlSet |= GPIO_GPINT_F_SET(1 << apo.id);
    }
    else
    {
        ctrlClr |= GPIO_GPINT_F_CLR(1 << apo.id);
    }

    if(cfg->debounce)
    {
        ctrlSet |= GPIO_DBN_EN_SET(1 << apo.id);
    }
    else
    {
        ctrlClr |= GPIO_DBN_EN_CLR(1 << apo.id);
    }

    if(cfg->level)
    {
        ctrlSet |= GPIO_GPINT_MODE_SET(1 << apo.id);
    }
    else
    {
        ctrlClr |= GPIO_GPINT_MODE_CLR(1 << apo.id);
    }
    hwp_gpio->gpint_ctrl_clr = ctrlClr;
    hwp_gpio->gpint_ctrl_set = ctrlSet;
}



// =============================================================================
// hal_GpioGetCfg
// -----------------------------------------------------------------------------
/// Return the configuration of a GPIO.
/// @param gpio Identifier of the GPIO.
/// @param cfg Current configuration of this GPIO.
// =============================================================================
PUBLIC VOID hal_GpioGetCfg(HAL_GPIO_GPIO_ID_T gpio, HAL_GPIO_CFG_T* cfg)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    if (hwp_gpio->gpio_oen_val & (1 << apo.id))
    {
        cfg->direction = HAL_GPIO_DIRECTION_INPUT;
        cfg->value = ((hwp_gpio->gpio_val & (1 << apo.id)) != 0);
    }
    else
    {
        cfg->direction = HAL_GPIO_DIRECTION_OUTPUT;
        cfg->value = ((hwp_gpio->gpio_set & (1 << apo.id)) != 0);
    }

    cfg->irqMask.rising =
        ((hwp_gpio->gpint_ctrl_set & GPIO_GPINT_R_SET(1 << apo.id)) != 0);
    cfg->irqMask.falling =
        ((hwp_gpio->gpint_ctrl_set & GPIO_GPINT_F_SET(1 << apo.id)) != 0);
    cfg->irqMask.debounce =
        ((hwp_gpio->gpint_ctrl_set & GPIO_DBN_EN_SET(1 << apo.id)) != 0);
    cfg->irqMask.level =
        ((hwp_gpio->gpint_ctrl_set & GPIO_GPINT_MODE_SET(1 << apo.id)) != 0);

    cfg->irqHandler = g_halGpioRegistry[apo.id].irqHandler;
    cfg->irqParam = g_halGpioRegistry[apo.id].irqParam;
}



// =============================================================================
// hal_GpioGetIntStatus
// -----------------------------------------------------------------------------
/// Return the interrupt status of a GPIO.
/// @param gpio Identifier of the GPIO.
/// @return The interrupt status of this GPIO.
// =============================================================================
PUBLIC BOOL hal_GpioGetIntStatus(HAL_GPIO_GPIO_ID_T gpio)
{
    SWAP_8808S_GPIO_6_AND_13(gpio);
    SWAP_FP_GPIO_24_AND_25(gpio);

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return FALSE;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    HAL_ASSERT(gpio >= HAL_GPIO_0 && gpio <= HAL_GPIO_7, gc_errNonIntGpio);

    UINT32 status = GET_BITFIELD(hwp_gpio->int_status, GPIO_GPINT_STATUS);
    return ((status & (1 << apo.id)) != 0);
}



#if 0
// ----------   DCON / CHG driver ------------

// =============================================================================
// hal_GpioDcOnOpen
// =============================================================================
/// Configure the DC-ON command
/// @param outTime Time for which GPIO0 is set to output mode, after a start
/// read DCON command is issued.
/// @param waitTime Time for which GPIO0 should wait before reading DC_ON,
/// after a start read DCON command is issued (Note: \p waitTime must be
/// strictly greater than \p outTime).
/// @param intMode Interruption mode of GPIO0 in mode DC_ON detection.
// =============================================================================
PUBLIC VOID hal_GpioDcOnOpen(UINT32 outTime, UINT32 waitTime,
                             HAL_GPIO_DC_ON_INT_MODE_T intMode)
{
    UINT32 ctrl = 0;

    HAL_ASSERT(waitTime>outTime, "hal_GpioDcOnOpen: waitTime must be greater than outTime", waitTime);
    HAL_ASSERT(waitTime<0x40, "hal_GpioDcOnOpen: waitTime out of range 0-63", waitTime);
    HAL_ASSERT(outTime<0x10, "hal_GpioDcOnOpen: waitTime out of range 0-15", waitTime);

    ctrl = GPIO_OUT_TIME(outTime) | GPIO_WAIT_TIME(waitTime);
    switch (intMode)
    {
        case HAL_GPIO_DC_ON_INT_L2H:
            ctrl |= GPIO_INT_MODE_L2H;
            break;
        case HAL_GPIO_DC_ON_INT_H2L:
            ctrl |= GPIO_INT_MODE_H2L;
            break;
        case HAL_GPIO_DC_ON_INT_RR:
            ctrl |= GPIO_INT_MODE_RR;
            break;
        case HAL_GPIO_DC_ON_INT_NO:
            break;
        default:
            HAL_ASSERT(FALSE, "Improper DC-ON irq mode", intMode); //die
            break;
    }

    hwp_gpio->chg_ctrl = ctrl;

    hwp_gpio->chg_cmd = GPIO_DCON_MODE_SET;

    // clear the HAL_GPIO_5
    hwp_gpio->gpio_clr = HAL_GPIO_BIT(HAL_GPIO_NUM_CHARGER_DETECT);

    if (intMode == HAL_GPIO_DC_ON_INT_NO)
    {
        // disable all int source of GPINT5
        hwp_gpio->gpint_ctrl_clr = GPIO_GPINT_R_CLR(HAL_GPIO_BIT(HAL_GPIO_NUM_CHARGER_DETECT))
                                   | GPIO_GPINT_F_CLR(HAL_GPIO_BIT(HAL_GPIO_NUM_CHARGER_DETECT))
                                   | GPIO_DBN_EN_CLR(HAL_GPIO_BIT (HAL_GPIO_NUM_CHARGER_DETECT));
    }
    else
    {
        // bypass debounce of GPINT5
        hwp_gpio->gpint_ctrl_clr = GPIO_DBN_EN_CLR(HAL_GPIO_BIT(HAL_GPIO_NUM_CHARGER_DETECT));
        // enable interrupt
        hwp_gpio->gpint_ctrl_set = GPIO_GPINT_R_SET(HAL_GPIO_BIT(HAL_GPIO_NUM_CHARGER_DETECT))
                                   | GPIO_GPINT_F_SET(HAL_GPIO_BIT(HAL_GPIO_NUM_CHARGER_DETECT));
    }

}

// =============================================================================
// hal_GpioDcOnClose
// =============================================================================
/// Disable the DC-ON command: set back GPIO0 as regular GPIO
// =============================================================================
PUBLIC VOID hal_GpioDcOnClose(VOID)
{
    hwp_gpio->chg_cmd = GPIO_DCON_MODE_CLR;
}


// =============================================================================
// hal_GpioChgSetMode
// -----------------------------------------------------------------------------
/// Enable or disable the charger according to \p on
/// when enabled, #hal_GpioChgDown can be called to pulse down the charger.
/// @param on charger state
// =============================================================================
PUBLIC VOID hal_GpioChgSetMode(BOOL on)
{
    // clear HAL_GPO_0 so when CHG is disabled the pin is 0
    // Be careful here.  The Charge Enable bit is hardcoded as GPO 0 but in
    // some designs, Charge Enable is incorrectly connected to another GPO such
    // as in the Opal Eng Board (GPO 2)
    hwp_gpio->gpo_clr = HAL_GPIO_BIT(0);
    // set the charger state
    hwp_gpio->chg_cmd = (on)?GPIO_CHG_MODE_SET :GPIO_CHG_MODE_CLR;

}


// =============================================================================
// hal_GpioChgDown
// -----------------------------------------------------------------------------
/// Sends the down signal to the charger to avoid a time-out. The user will
/// have to call that function at least every 5 seconds.
// =============================================================================
PUBLIC VOID hal_GpioChgDown(VOID)
{
    hwp_gpio->chg_cmd = GPIO_CHG_DOWN;
}


// =============================================================================
// hal_GpioChgGet
// -----------------------------------------------------------------------------
/// This function reads the charger dedicated GPIO, and returns its value.
///
/// @return The input value of the charger dedicated GPIO.
// =============================================================================
PUBLIC UINT32 hal_GpioChgGet(VOID)
{
    return hal_GpioGet(HAL_GPIO_CHARGER_DETECT);
}


// =============================================================================
// hal_GpioChgIrqSetHandler
// -----------------------------------------------------------------------------
/// Sets the user IRQ handler called when an interruption is triggerd on the
/// charger dedicated GPIO.
///
/// @param gpio The GPIO whose interrupt will trig the handler
/// @param handler The user function called on an interruption on the charger
/// dedicated GPIO.
// =============================================================================
PUBLIC VOID hal_GpioChgIrqSetHandler(HAL_GPIO_IRQ_HANDLER_T handler)
{
    hal_GpioIrqSetHandler(HAL_GPIO_CHARGER_DETECT, handler, 0);
}

#endif



// =============================================================================
// GPIO's driver 'private' functions, accessible
// through the halp_gpio.h header (Private API)
// =============================================================================

// =============================================================================
// hal_GpioIrqHandler
// -----------------------------------------------------------------------------
/// GPIO module IRQ handler
///
///     Clear the IRQ and call the IRQ handler
///     user function
///     @param interruptId The interruption ID
// =============================================================================
PROTECTED VOID hal_GpioIrqHandler(UINT8 interruptId)
{
    // Get cause
    UINT32 status = GET_BITFIELD(hwp_gpio->int_status, GPIO_GPINT_STATUS);

    // Clear IRQ
    hwp_gpio->int_clr = GPIO_GPINT_CLR(status);

    // Call user handler
    UINT32 i;

    for (i=0; i<8 ; i++)
        // i from HAL_GPIO_0 to HAL_GPIO_7
    {
        if (status & (1<<i))
        {
            if (g_halGpioRegistry[i].irqHandler)
            {
                g_halGpioRegistry[i].irqHandler(g_halGpioRegistry[i].irqParam);
            }
        }
    }
}

