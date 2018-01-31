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



#ifndef _PMD_CONFIG_H_
#define _PMD_CONFIG_H_

#include "cs_types.h"
#include "hal_gpio.h"
#include "hal_ana_gpadc.h"

// =============================================================================
// PMD_CONFIG_T
// -----------------------------------------------------------------------------
/// The type PMD_CONFIG_T is defined in pmd_m.h using the structure
/// #PMD_CONFIG_STRUCT_T, the following macro allow to map the correct structure
/// and keep the actual structures different between each models for the
/// documentation.
///
/// Make sure to include pmd_config.h before pmd_m.h !
// =============================================================================

#include "pmd_m.h"




// svn propset svn:keywords "HeadURL Author Date Revision" dummy.h

/// @defgroup pmdOpal PMD Opal config
/// @{


// =============================================================================
//
// -----------------------------------------------------------------------------
// =============================================================================



// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_LDO_ID_TYPE_T
// -----------------------------------------------------------------------------
/// Used to give a type to an LDO_ID that can be an Opal internal LDO or an
/// external LDO controlled by an output PIN (#HAL_APO_ID_T).
/// This type is compatible with #HAL_GPIO_TYPE_T and extend it.
// =============================================================================
typedef enum
{
    /// invalid LDO ID type
    PMD_LDO_ID_TYPE_INVALID = HAL_GPIO_TYPE_INVALID,

    /// no LDO ID type
    PMD_LDO_ID_TYPE_NONE = HAL_GPIO_TYPE_NONE,

    /// controlled by GPIO type
    PMD_LDO_ID_TYPE_IO = HAL_GPIO_TYPE_IO,

    /// controlled by GPO type
    PMD_LDO_ID_TYPE_O = HAL_GPIO_TYPE_O,

    /// controlled by TCO type
    PMD_LDO_ID_TYPE_TCO = HAL_GPIO_TYPE_TCO,

    /// Opal Internal LDO
    PMD_LDO_ID_TYPE_OPAL,

    PMD_LDO_ID_TYPE_QTY
} PMD_LDO_ID_TYPE_T;

// =============================================================================
// PMD_LDO_OPAL_ID_T
// -----------------------------------------------------------------------------
/// This type is used to name all internal LDO in Opal
// =============================================================================
typedef enum
{
    /// To be used in configuration when the field for this GPO is not used
    PMD_LDO_NONE    = PMD_LDO_ID_TYPE_NONE << 16,
    /// there are less than 16 LDOs so we use bitfield, this also allows to
    /// control several LDO at the same time by oring '|' the IDs
    PMD_LDO_BLLED   = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<2),
    PMD_LDO_MIC     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<3),
    PMD_LDO_USB     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<4),
    PMD_LDO_VIBR    = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<5),
    PMD_LDO_MMC     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<6),
    PMD_LDO_LCD     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<7),
    PMD_LDO_CAM     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<8),
    PMD_LDO_RF      = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<9),
    PMD_LDO_ABB     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<10),
    PMD_LDO_PAD     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<11),
    PMD_LDO_MEM     = (PMD_LDO_ID_TYPE_OPAL << 16) | (1<<12),

    /// number of physical LDOs id (max id + 1)
    PMD_LDO_QTY = 13,
} PMD_LDO_OPAL_ID_T;

// =============================================================================
// PMD_LDO_ID_T
// -----------------------------------------------------------------------------
/// LDO IDentifier
/// Used to identify an LDO in OPAL or a pin to control the LDO.
/// NOTE: LDO is used here as most power sources are LDOs, it might be an other kind of circuits (like a bias circuit for the microphone).
// =============================================================================
typedef union
{
    struct
    {
        // NOTE: THIS IS ENDIANNES DEPENDANT
        UINT16 id:16;
        /// Defines the kind of the LDO internal, pin (GPIO, GPO, TCO), none
        PMD_LDO_ID_TYPE_T type:16;
    };
    /// OPAL internal LDO
    PMD_LDO_OPAL_ID_T opal;
    /// ID of the pin controlling the external LDO
    HAL_APO_ID_T pin;
} PMD_LDO_ID_T;

// =============================================================================
// PMD_LEVEL_TYPE_OPAL_T
// -----------------------------------------------------------------------------
/// Used to give a type to a general purpose pin that can be either a GPIO pin
/// or a GPO pin.
// =============================================================================
typedef enum
{
    /// no level type
    PMD_LEVEL_TYPE_NONE = 0,

    /// PWL0 type
    PMD_LEVEL_TYPE_PWL0,

    /// PWL1 type
    PMD_LEVEL_TYPE_PWL1,

    /// LPG type
    PMD_LEVEL_TYPE_LPG,

    /// LDO / APO type (any internal LDO or out pin uses #PMD_LDO_ID_T)
    PMD_LEVEL_TYPE_LDO,

    /// Specific control in Opal for #PMD_LEVEL_SIM or #PMD_LEVEL_LOUD_SPEAKER
    PMD_LEVEL_TYPE_OPAL,

    /// BackLight LED control in Opal
    PMD_LEVEL_TYPE_BACKLIGHT

} PMD_LEVEL_TYPE_OPAL_T;

// =============================================================================
// PMD_CONFIG_STRUCT_T
// -----------------------------------------------------------------------------
/// This structure describes the configuration of the PMD driver specifc
/// to a given board.
// =============================================================================
struct PMD_CONFIG_STRUCT_T
{
    struct
    {
        PMD_LDO_ID_T ldo;
        /// \c TRUE Active high, \c FALSE Active Low (used for pin only)
        BOOL polarity;
        /// \c TRUE This LDO is also used by another power ID,
        /// \c FALSE this LDO is dedicated
        BOOL shared;
        /// Power On state, this state is set by pmd_Open
        BOOL powerOnState;
    } power[PMD_POWER_ID_QTY];
    struct
    {
        /// select the kind of level pin used: PWM0/PWM0/OTHER PIN/LPG/
        /// Opal dedicated SIM, Loudspeaker or Backlight control
        PMD_LEVEL_TYPE_OPAL_T type;
        /// When not using a specific control nor a pwm but an opal LDO or a pin
        PMD_LDO_ID_T ldo;
        /// Power On state, this state is set by pmd_Open
        UINT32 powerOnState;
    } level[PMD_LEVEL_ID_QTY];
    /// convert PMD_BACK_LIGHT_LEVEL_T to BackLight LED control value
    UINT8 lightLevelToBacklight[PMD_BACK_LIGHT_LEVEL_QTY];
    /// convert PMD_BACK_LIGHT_LEVEL_T to PWL0 value
    UINT8 lightLevelToPwm0[PMD_BACK_LIGHT_LEVEL_QTY];
    /// convert PMD_BACK_LIGHT_LEVEL_T to PWL1 value
    UINT8 lightLevelToPwm1[PMD_BACK_LIGHT_LEVEL_QTY];
    /// Starting state for all LDO in normal mode.
    /// NOTE: power controlled by #power or #level are set according
    /// to the corresponding .powerOnState value
    PMD_LDO_OPAL_ID_T ldoEnableNormal;
    /// Starting state for all LDO in low power
    PMD_LDO_OPAL_ID_T ldoEnableLowPower;
    /// VIBRATE LDO Voltage configuration: \c FALSE 1.8V, \c TRUE 2.8V
    BOOL ldoIbrIs2_8;
    /// MMC LDO Voltage configuration: \c FALSE 1.8V, \c TRUE 2.8V
    BOOL ldoMMCIs2_8;
    /// LCD LDO Voltage configuration: \c FALSE 1.8V, \c TRUE 2.8V
    BOOL ldoLcdIs2_8;
    /// Camera LDO Voltage configuration: \c FALSE 1.8V, \c TRUE 2.8V
    BOOL ldoCamIs2_8;
    /// RF LDO Voltage configuration: \c FALSE 1.8V, \c TRUE 2.8V
    BOOL ldoRfIs2_8;
    /// @todo: add a mode for 1.8 xcvr ex VRF1=> pads, VPAD=> mem, VMEM=>xcvr

    /// the GPADC channel used for battery monitoring.
    HAL_ANA_GPADC_CHAN_T batteryGpadcChannel;
    /// the battery charge termination level in milivolts:
    /// usually 4200 (4.2V) for LiIon batteries.
    UINT16 batteryLevelChargeTermMV;
    /// the battery recharge level in milivolts after charge terminated:
    /// less than or equal to batteryLevelChargeTermMV.
    UINT16 batteryLevelRechargeMV;
    /// the battery full level in milivolts:
    /// used by percentage calculation.
    /// less than or equal to batteryLevelChargeTermMV.
    UINT16 batteryLevelFullMV;
    /// Charge timout value (in HAL_TICK1S units) usually 4 hours
    UINT32 batteryChargeTimeout;
    /// when the system is active (LCD on ...) the battery level measured
    /// drops by a few milivolts. To compensate that drop, the following
    /// offset is added to any battery measure when the system is active
    /// usually 30 (30mV)
    INT16 batteryOffsetHighActivityMV;
    /// when the system is charged in power down state, the battery level
    /// measured rises by a few milivolts. To compensate that rise, the
    /// following offset (a negative value or 0) is added from the measured
    /// level.
    INT16 batteryOffsetPowerDownChargeMV;
    /// the power-on battery level in milivolts
    UINT16 powerOnVoltageMV;
    /// the power-down battery level in milivolts
    UINT16 powerDownVoltageMV;
    /// the battery charge current
    PMD_CHARGE_CURRENT_T batteryChargeCurrent;
    /// the battery charge current on USB connection:
    /// less than or equal to batteryChargeCurrent
    PMD_CHARGE_CURRENT_T batteryChargeCurrentOnUsb;

    /// the GPADC channel used for charger monitoring.
    HAL_ANA_GPADC_CHAN_T chargerGpadcChannel;
    /// the max charger valid level in milivolts:
    /// usually 7000 (7V).
    UINT16 chargerLevelUpperLimit;
    /// if the system stops charging due to high charger level, it can restart
    /// charging when the charge level falls under the upper limit plus the
    /// back-to-normal offset (a negative value or 0)
    INT16 chargerOffsetBackToNormal;

    /// the GPADC channel used for temperature monitoring.
    HAL_ANA_GPADC_CHAN_T temperatureGpadcChannel;
    /// the max valid temperature:
    UINT16 temperatureUpperLimit;
    /// if the system stops charging due to high temperature, it can restart
    /// charging when the temperature falls under the upper limit plus the
    /// back-to-normal offset (a negative value or 0)
    INT16 temperatureOffsetBackToNormal;

    /// the battery level to enter screen anti-flicker mode.
    UINT16 batteryMVScreenAntiFlicker;
    /// if screen anti flicker is enabled, it can be disabled when the battery
    /// level rises up to the activation threshold plus the screen normal offset
    INT16 batteryOffsetScreenNormal;

    /// The GPIO pin to detect earpiece
    HAL_GPIO_GPIO_ID_T earpieceDetectGpio;
    /// The GPADC channel used for earpiece monitoring.
    HAL_ANA_GPADC_CHAN_T earpieceGpadcChannel;

    /// @todo add bat temp conversion ...
    /// The GPIO pin to quick enter VCOM for bbat
    HAL_GPIO_GPIO_ID_T vcomDetectGpio;
};

/// @} // <-- End of pmdOpal group

#endif // _PMD_CONFIG_H_

