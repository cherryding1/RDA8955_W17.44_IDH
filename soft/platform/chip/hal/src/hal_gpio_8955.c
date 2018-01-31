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


// =============================================================================
// GLOBAL VARIABLE
// =============================================================================

PRIVATE CONST CHAR* gc_errWrongGpioType
    = "Wrong gpio type";

PRIVATE struct {
    HAL_GPIO_IRQ_HANDLER_T irqHandler;
    UINT32 irqParam;
} g_halGpioRegistry[64];


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
    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);

    UINT32 sc = hal_SysEnterCriticalSection();
    if(gpio < HAL_GPIO_32 )
    {
        //Set direction and value (if direction is out)
        if(cfg->direction == HAL_GPIO_DIRECTION_INPUT)
        {
            hwp_gpio->gpio_oen_set_in_l = GPIO_OEN_SET_IN_L(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpio_oen_set_out_l = GPIO_OEN_SET_OUT_L(1 << apo.id);

            if(cfg->value)
            {
                hwp_gpio->gpio_set_l = GPIO_GPIO_SET_L(1 << (apo.id));
            }
            else
            {
                hwp_gpio->gpio_clr_l = GPIO_GPIO_CLR_L(1 << (apo.id));
            }
        }
    }
    else
    {
        //Set direction and value (if direction is out)
        if(cfg->direction == HAL_GPIO_DIRECTION_INPUT)
        {
            hwp_gpio->gpio_oen_set_in_h = GPIO_OEN_SET_IN_H(1 << (apo.id -32));
        }
        else
        {
            hwp_gpio->gpio_oen_set_out_h = GPIO_OEN_SET_OUT_H(1 << (apo.id -32));

            if(cfg->value)
            {
                hwp_gpio->gpio_set_h = GPIO_GPIO_SET_H(1 << (apo.id -32));
            }
            else
            {
                hwp_gpio->gpio_clr_h = GPIO_GPIO_CLR_H(1 << (apo.id -32));
            }
        }

    }
    //Set the irq handler
    hal_GpioIrqSetHandler(gpio, cfg->irqHandler, cfg->irqParam);

    //Set mask
    hal_GpioIrqSetMask(gpio, &cfg->irqMask);

    hal_SysExitCriticalSection(sc);
}
// =============================================================================
// hal_GpioClose
// -----------------------------------------------------------------------------
/// Closes a GPIO driver. The GPIO is returned to its reset state
/// @param gpio GPIO to close.
// =============================================================================
PUBLIC VOID hal_GpioClose(HAL_GPIO_GPIO_ID_T gpio)
{
    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);

    UINT32 sc = hal_SysEnterCriticalSection();

    hal_GpioIrqSetHandler(gpio, NULL, 0);

    if(gpio < HAL_GPIO_32 )
    {

        hwp_gpio->gpio_oen_set_in_l = GPIO_OEN_SET_IN_L(1 << apo.id);
        hwp_gpio->gpint_r_clr_l = GPIO_GPINT_R_CLR_L(1 << apo.id);
        hwp_gpio->gpint_f_clr_l = GPIO_GPINT_F_CLR_L(1 << apo.id);
        hwp_gpio->gpint_dbn_en_clr_l = GPIO_DBN_EN_CLR_L(1 << apo.id);
        hwp_gpio->gpint_mode_clr_l = GPIO_GPINT_MODE_CLR_L(1 << apo.id);
    }
    else
    {
        hwp_gpio->gpio_oen_set_in_h = GPIO_OEN_SET_IN_H(1 << (apo.id -32));
        hwp_gpio->gpint_r_clr_h = GPIO_GPINT_R_CLR_H(1 << (apo.id -32));
        hwp_gpio->gpint_f_clr_h = GPIO_GPINT_F_CLR_H(1 << (apo.id -32));
        hwp_gpio->gpint_dbn_en_clr_h = GPIO_DBN_EN_CLR_H(1 << (apo.id -32));
        hwp_gpio->gpint_mode_clr_h = GPIO_GPINT_MODE_CLR_H(1 << (apo.id -32));
    }
    hal_GpioResetIrq(gpio);

    hal_SysExitCriticalSection(sc);
}




// =============================================================================
// hal_GpioSetOut
// -----------------------------------------------------------------------------
/// This function sets one GPIO to output and leaves the others'directions
/// unchanged.
// =============================================================================
PUBLIC VOID hal_GpioSetOut(HAL_GPIO_GPIO_ID_T gpio)
{
    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    if(gpio < HAL_GPIO_32 )
    {
        hwp_gpio->gpio_oen_set_out_l = 1 << apo.id;
    }
    else
    {
        hwp_gpio->gpio_oen_set_out_h = 1 << (apo.id -32);
    }
}

// =============================================================================
// hal_GpioSetIn
// -----------------------------------------------------------------------------
/// This function sets one GPIO to input and leaves the others'directions
/// unchanged.
// =============================================================================
PUBLIC VOID hal_GpioSetIn(HAL_GPIO_GPIO_ID_T gpio)
{

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);

    if(gpio < HAL_GPIO_32 )
    {
        hwp_gpio->gpio_oen_set_in_l = 1 << apo.id;
    }
    else
    {
        hwp_gpio->gpio_oen_set_in_h = 1 << (apo.id -32);
    }
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

    HAL_APO_ID_T apo;
    apo.gpioId = gpio;
    if(apo.type == HAL_GPIO_TYPE_NONE)
    {
        return 0;
    }
    HAL_ASSERT(apo.type == HAL_GPIO_TYPE_IO, gc_errWrongGpioType);
    if(gpio < HAL_GPIO_32 )
    {
        if (hwp_gpio->gpio_val_l & (1 << apo.id))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (hwp_gpio->gpio_val_h & (1 << (apo.id -32)))
        {
            return 1;
        }
        else
        {
            return 0;
        }

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
    switch (apo.type)
    {
        case HAL_GPIO_TYPE_NONE:
            break;
        case HAL_GPIO_TYPE_IO:

            if(apo.gpioId < HAL_GPIO_32 )
            {
                hwp_gpio->gpio_set_l = 1 << apo.id;
            }
            else
            {
                hwp_gpio->gpio_set_h = 1 << (apo.id -32);
            }
            break;
        case HAL_GPIO_TYPE_O:
            break;
        case HAL_GPIO_TYPE_TCO:
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
    switch (apo.type)
    {
        case HAL_GPIO_TYPE_NONE:
            break;
        case HAL_GPIO_TYPE_IO:

            if(apo.gpioId < HAL_GPIO_32 )
            {
                hwp_gpio->gpio_clr_l = 1 << apo.id;
            }
            else
            {

                hwp_gpio->gpio_clr_h = 1 << (apo.id -32);
            }
            break;
        case HAL_GPIO_TYPE_O:
            break;
        case HAL_GPIO_TYPE_TCO:
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
    if(gpio < HAL_GPIO_32 )
    {
        hwp_gpio->int_clr_l = 1 << apo.id;
    }
    else
    {
        hwp_gpio->int_clr_h = 1 << (apo.id -32);
    }
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


    hal_GpioResetIrq(gpio);
    if(gpio < HAL_GPIO_32 )
    {
        if(cfg->rising)
        {
            hwp_gpio->gpint_r_set_l  = GPIO_GPINT_R_SET_L(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_r_clr_l  = GPIO_GPINT_R_CLR_L(1 << apo.id);
        }

        if(cfg->falling)
        {
            hwp_gpio->gpint_f_set_l = GPIO_GPINT_F_SET_L(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_f_clr_l = GPIO_GPINT_R_CLR_L(1 << apo.id);
        }

        if(cfg->debounce)
        {
            hwp_gpio->gpint_dbn_en_set_l = GPIO_DBN_EN_SET_L(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_dbn_en_clr_l = GPIO_DBN_EN_CLR_L(1 << apo.id);
        }

        if(cfg->level)
        {
            hwp_gpio->gpint_mode_set_l = GPIO_GPINT_MODE_SET_L(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_mode_clr_l = GPIO_GPINT_MODE_CLR_L(1 << apo.id);
        }

    }
    else
    {
        apo.id -= 32;
        if(cfg->rising)
        {
            hwp_gpio->gpint_r_set_h  = GPIO_GPINT_R_SET_H(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_r_clr_h  = GPIO_GPINT_R_CLR_H(1 << apo.id);
        }

        if(cfg->falling)
        {
            hwp_gpio->gpint_f_set_h = GPIO_GPINT_F_SET_H(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_f_clr_h = GPIO_GPINT_R_CLR_H(1 << apo.id);
        }

        if(cfg->debounce)
        {
            hwp_gpio->gpint_dbn_en_set_h = GPIO_DBN_EN_SET_H(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_dbn_en_clr_h = GPIO_DBN_EN_CLR_H(1 << apo.id);
        }

        if(cfg->level)
        {
            hwp_gpio->gpint_mode_set_h = GPIO_GPINT_MODE_SET_H(1 << apo.id);
        }
        else
        {
            hwp_gpio->gpint_mode_clr_h = GPIO_GPINT_MODE_CLR_H(1 << apo.id);
        }

    }
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
}

// =============================================================================
// hal_GpioGetIntStatus
// -----------------------------------------------------------------------------
/// Return the interrupt status of a GPIO.
/// @param gpio Identifier of the GPIO.
/// @return The interrupt status of this GPIO.
// =============================================================================
PUBLIC BOOL hal_GpioGetIntStatus(HAL_GPIO_GPIO_ID_T gpio)
{}





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
    UINT32 i;
    // Get cause
    UINT32 statusl =  hwp_gpio->int_status_l ;
    UINT32 statush =  hwp_gpio->int_status_h ;

    // Clear IRQ
    hwp_gpio->int_clr_l = statusl;
    hwp_gpio->int_clr_h = statush;

    // Call user handler

    if(statusl !=0)   // i from HAL_GPIO_0 to HAL_GPIO_31
    {
        for (i=0; i<32 ; i++)
        {
            if (statusl & (1<<i))
            {
                if (g_halGpioRegistry[i].irqHandler)
                {
                    g_halGpioRegistry[i].irqHandler(g_halGpioRegistry[i].irqParam);
                }
            }
        }
    }

    if(statush !=0)     // i from HAL_GPIO_32 to HAL_GPIO_63
    {
        for (i=0; i<32 ; i++)
        {
            if (statush & (1<<i))
            {
                if (g_halGpioRegistry[i+32].irqHandler)
                {
                    g_halGpioRegistry[i+32].irqHandler(g_halGpioRegistry[i+32].irqParam);
                }
            }
        }
    }
}


