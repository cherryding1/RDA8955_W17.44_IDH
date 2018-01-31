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


#ifndef _HAL_CONFIG_H_
#define _HAL_CONFIG_H_

#include "cs_types.h"
#include "chip_id.h"
#include "hal_pwm.h"
#include "hal_gpio.h"
#include "hal_i2c.h"


// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// HAL_CFG_SDIO2_PORT_SEL_T
// -----------------------------------------------------------------------------
/// The SDIO2 port can be selected in 3 IOs:
// =============================================================================
typedef enum
{
    /// The SDIO2 port is not used.
    HAL_CFG_SDIO2_NONE = 0,
    /// The SDIO2 port is used at SPI1 & TCO3/4.
    HAL_CFG_SDIO2_SPI1 = 1,
    /// The SDIO2 port is used at LCD high 8-bit.
    HAL_CFG_SDIO2_LCD  = 2,
    /// The SDIO2 port is used at KEYIN/OUT.
    HAL_CFG_SDIO2_KEY  = 3,
} HAL_CFG_SDIO2_PORT_SEL_T;

// =============================================================================
// HAL_CFG_UART_KEY_MUX_T
// -----------------------------------------------------------------------------
/// This structure describes the UART1/UART2 PORT selected and KEYIN/KEYOUT
/// GPIO mode selected.
// =============================================================================
typedef struct
{
    /// \c TRUE if UART1 is used at TCO_3/4 LPSCO_0 PWL_1
    BOOL uart1UseTco :1;
    /// \c TRUE if UART2 is used at TCO_3/4 LPSCO_0 SPI1_CS_0
    BOOL uart2UseTco :1;
    /// \c TRUE if KEYIN_0 is used as GPIO_20
    BOOL keyin0GpioEn :1;
    /// \c TRUE if KEYIN_4 is used as GPIO_21
    BOOL keyin4GpioEn :1;
    /// \c TRUE if KEYOUT_1 is used as GPIO_17
    BOOL keyout1GpioEn :1;
    /// \c TRUE if KEYOUT_2 is used as GPIO_11
    BOOL keyout2GpioEn :1;
    /// \c TRUE if KEYOUT_3 is used as GPIO_7
    BOOL keyout3GpioEn :1;
    /// \c TRUE if KEYOUT_4 is used as GPIO_8
    BOOL keyout4GpioEn :1;
} HAL_CFG_UART_KEY_MUX_T;

// =============================================================================
// HAL_CFG_CAM_T
// -----------------------------------------------------------------------------
/// This structure describes the camera configuration for a given target.
/// The first field identify if camera is used.
/// The second and third field is the RemapFlag which identify if camera PDN/RST
/// need to be remapped to other GPIOs instead of default (GPIO_5 and GPIO_4),
/// for the first camera sensor.
/// The fourth and fifth field is the remapped pin when RemapFlag is set, for the
/// first camera sensor.
/// The sixth and seventh fields are used to describe which GPIOs are used for
/// the PDN/RST lines of the optional second camera sensor. This second camera
/// sensor can only be used on with GPIOs to control its PDN and RST lines.
// =============================================================================
typedef struct
{
    /// \c TRUE if the rear camera is used
    BOOL            camUsed :1;
    /// The polarity of the Power DowN line, for the rear sensor.
    BOOL            camPdnActiveH;
    /// The polarity of the Reset line, for the rear sensor.
    BOOL            camRstActiveH;
    /// The remapped GPIO controlling PDN (HAL_GPIO_NONE if not remapped), for the rear sensor.
    HAL_APO_ID_T    camPdnRemap;
    /// The remapped GPIO controlling RST (HAL_GPIO_NONE if not remapped), for the rear sensor.
    HAL_APO_ID_T    camRstRemap;
    /// \c TRUE if the front camera is used
    BOOL            cam1Used :1;
    /// The polarity of the Power DowN line, for the front sensor.
    BOOL            cam1PdnActiveH;
    /// The polarity of the Reset line, for the front sensor.
    BOOL            cam1RstActiveH;
    /// The remapped GPIO controlling PDN (HAL_GPIO_NONE if not remapped), for the front sensor.
    HAL_APO_ID_T    cam1PdnRemap;
    /// The remapped GPIO controlling RST (HAL_GPIO_NONE if not remapped), for the front sensor.
    HAL_APO_ID_T    cam1RstRemap;
    /// \c TRUE if the serial interface camera is used
    // CAM_D1:CTS; CAM_D4:OverFlow; CAM_D5:Ready; CAM_D6:SSN; CAM_D7:SCK; CAM_PCLK:DI; CAM_CLK:CLK
    BOOL            camSerialIfMode :1;
    /// \c TRUE if the CAM_RST used as GPIO5; CAM_PDN used as GPIO7
    BOOL            camGpio1En :1;
    /// \c TRUE if the CAM_VSYNC used as GPIO31; CAM_HREF:GPIO16; CAM_D0:GPIO12; CAM_D2:GPIO14; CAM_D3:GPIO15
    BOOL            camGpio2En :1;
    /// \c TRUE if the CAM_PCLK used as GPIO28; CAM_D4:GPIO18; CAM_D5:GPIO19; CAM_D6:GPIO26; CAM_D7:GPIO27
    // Note: camGPIO3En useless when camSerialIfMode=TRUE
    BOOL            camGpio3En :1;
} HAL_CFG_CAM_T;

// =============================================================================
// HAL_CFG_PWM_T
// -----------------------------------------------------------------------------
/// This structure describes the PWM configuration for a given target.
/// The first field identify which PWL is used for GLOW (if any).
/// The lasts fields tell wether the pin corresponding to PWM output
/// is actually used as PWM output and not as something else (for
/// instance as a GPIO).
// =============================================================================
typedef struct
{
    /// the glowing PWL position
    HAL_PWL_ID_T pwlGlowPosition;
    /// \c TRUE if the PWL0 is used
    BOOL pwl0Used :1;
    /// \c TRUE if the PWL1 is used
    BOOL pwl1Used :1;
    /// \c TRUE if the PWT is used
    BOOL pwtUsed :1;
    /// \c TRUE if the LPG is used
    BOOL lpgUsed :1;
} HAL_CFG_PWM_T;

// =============================================================================
// HAL_CFG_I2C_T
// -----------------------------------------------------------------------------
/// This structure describes the I2C configuration for a given target. The
/// fields tell wether the corresponding I2C pins are actually used
/// for I2C and not as something else (for instance as a GPIO).
// =============================================================================
typedef struct
{
    /// \c TRUE if the I2C pins are used
    BOOL i2cUsed :1;
    /// Bitrate for the I2C bus.
    HAL_I2C_BPS_T   i2cBps;
    /// \c TRUE if the I2C2 pins are used
    BOOL i2c2Used :1;
    /// Bitrate for the I2C2 bus.
    HAL_I2C_BPS_T   i2c2Bps;
    /// \c TRUE if the I2C3 pins are used
    BOOL i2c3Used :1;
    /// Bitrate for the I2C3 bus.
    HAL_I2C_BPS_T   i2c3Bps;
} HAL_CFG_I2C_T;

// =============================================================================
// HAL_CFG_I2S_T
// -----------------------------------------------------------------------------
/// This structure describes the I2S configuration for a given target. The
/// fields tell wether the corresponding I2S pin is actually used
/// for I2S and not as something else (for instance as a GPIO).
// =============================================================================
typedef struct
{
    /// \c TRUE if the data out pin is used
    BOOL doUsed :1;
    BOOL :3;
    /// \c TRUE if corresponding input is used
    BOOL di0Used :1;
    BOOL di1Used :1;
    BOOL di2Used :1;
} HAL_CFG_I2S_T;

// =============================================================================
// HAL_UART_CONFIG_T
// -----------------------------------------------------------------------------
/// Used to describes a configuration for used pin by an UART for a given target.
// =============================================================================
typedef enum
{
    /// invalid
    HAL_UART_CONFIG_INVALID = 0,

    /// UART is not used
    HAL_UART_CONFIG_NONE,

    /// UART use only data lines (TXD & RXD)
    HAL_UART_CONFIG_DATA,

    /// UART use data and flow control lines (TXD, RXD, RTS & CTS)
    HAL_UART_CONFIG_FLOWCONTROL,

    /// UART use all lines (TXD, RXD, RTS, CTS, RI, DSR, DCD & DTR)
    HAL_UART_CONFIG_MODEM,

    HAL_UART_CONFIG_QTY
} HAL_UART_CONFIG_T;

// =============================================================================
// HAL_CFG_SPI_T
// -----------------------------------------------------------------------------
/// This structure describes the SPI configuration for a given target. The first
/// fields tell wether the pin corresponding to chip select is actually used
/// as a chip select and not as something else (for instance as a GPIO).
/// Then, the polarity of the Chip Select is given. It is only relevant
/// if the corresponding Chip Select is used as a Chip Select.
/// Finally which pin is used as input, Can be none, one or the other.
/// On most chip configuration the input 0 (di0) is on the output pin: SPI_DIO
// =============================================================================
typedef struct
{
    /// \c TRUE if the corresponding pin is used as a Chip Select.
    BOOL cs0Used :1;
    BOOL cs1Used :1;
    BOOL cs2Used :1;
    BOOL cs3Used :1;
    /// \c TRUE if the first edge is falling
    BOOL cs0ActiveLow :1;
    BOOL cs1ActiveLow :1;
    BOOL cs2ActiveLow :1;
    BOOL cs3ActiveLow :1;
    /// \c TRUE if corresponding input is used
    BOOL di0Used :1;
    BOOL di1Used :1;
} HAL_CFG_SPI_T;

// =============================================================================
// HAL_CFG_EBC_T
// -----------------------------------------------------------------------------
/// This structure describes the EBC configuration for a given target. The first
/// fields tell wether the pin corresponding to chip select is actually used
/// as a chip select and not as something else (for instance as a GPIO).
// =============================================================================
typedef struct
{
    /// \c TRUE if the corresponding pin is used as a Chip Select.
    BOOL cs2Used :1;
    BOOL cs3Used :1;
    BOOL cs4Used :1;
} HAL_CFG_EBC_T;

// =============================================================================
// HAL_CFG_GOUDA_T
// -----------------------------------------------------------------------------
/// This structure describes the GOUDA configuration for a given target.
/// The first fields tell wether the pin corresponding to chip select is
/// actually used as a chip select and not as something else (for instance
/// as a GPIO). If none are used, the GOUDA is considered unused.
// =============================================================================
typedef struct
{
    /// \c TRUE if the corresponding pin is used as a Chip Select.
    BOOL cs0Used :1;
    BOOL cs1Used :1;
    /// \c TRUE if the LCD data width is 16 bits.
    BOOL lcdData16Bit :1;
    /// \c TRUE if the active LCD reset signal is low.
    BOOL lcdResetActiveLow :1;
    /// \c TRUE swap byte
    BOOL swapByte :1;
    /// \c ID of the pin to reset LCD
    HAL_APO_ID_T lcdResetPin;
} HAL_CFG_GOUDA_T;

// =============================================================================
// HAL_CFG_IO_DRIVE_T
// -----------------------------------------------------------------------------
/// This structure describes the IO Drive configuration for a given target.
// =============================================================================
typedef union
{
    struct
    {
        UINT32 vMem0Domain:3;
        UINT32 vMem1Domain:3;
        UINT32 vMemSpiDomain:3;
        UINT32 vLcdDomain:2;
        UINT32 vCamDomain:2;
        UINT32 vPadDomain:2;
        UINT32 keyInKeyOut:2;
        UINT32 vSdmmcDomain:2;
    };
    struct
    {
        UINT32 select1;
    };
} HAL_CFG_IO_DRIVE_T;

// =============================================================================
// HAL_CFG_CONFIG_T
// -----------------------------------------------------------------------------
/// HAL configuration structure.
/// This structure is used to setup HAL with regards to the specific
/// configuration of the board.
/// Enabling a feature such as i2sCfg will set the corresponding pins which
/// could also be a GPIO to their alternative use, that is I2S for this
/// example.
/// GPIO Pins not connected must be set in the noConnectPins bitfield, for
/// power saving. They are configured as output and driven to '0'.
/// Pins which are not not connected (ie are connected) but are not used in
/// an alternative mode as I2S, are considered to be used as GPIOs.
// =============================================================================
typedef struct
{
    /// Chip version
    UINT32  chipVersion;
    /// RF Clock Frequency
    UINT32  rfClkFreq;
    /// \c TRUE if LPS_CO_1 is used as LPS
    BOOL    useLpsCo1;
    /// Bitfield with '1' for keypad pins used as input on the keypad
    UINT8   keyInMask;
    /// Bitfield with '1' for keypad pins used as output on the keypad
    UINT8   keyOutMask;
    /// PWM Configuration
    HAL_CFG_PWM_T pwmCfg;
    BOOL useUsbBackup;
    BOOL useClk32k;
    HAL_CFG_I2C_T i2cCfg;
    /// \c TRUE if the I2C2 is located on CAM Pins:
    BOOL i2c2UseCamPins :1;
    /// I2S Configuration
    HAL_CFG_I2S_T i2sCfg;
    /// EBC Configuration
    HAL_CFG_EBC_T ebcCfg;
    /// UART Configuration
    HAL_UART_CONFIG_T uartCfg[CHIP_STD_UART_QTY];
    /// SPI Configuration
    HAL_CFG_SPI_T spiCfg[2];
    /// \c TRUE if the SPI2 is located on CAM Pins
    // CAM_D0:CS_0; CAM_D1:CS_1; CAM_D2:DO; CAM_D3:CLK; CAM_HREF:DI_1; CAM_VSYNC:DI_0
    BOOL    spi2UseCamPins;
    /// TRUE if the SD Card interface is used
    BOOL    useSdmmc;
    UINT8   useSdmmc2; //HAL_CFG_SDIO2_PORT_SEL_T
    HAL_CFG_UART_KEY_MUX_T uartKeyinoutSel;
    /// TRUE if the Camera interface is used
    HAL_CFG_CAM_T   camCfg;

    HAL_CFG_GOUDA_T goudaCfg;

    /// Bitfield with '1' for physically not connected GPIO pins
    UINT32  noConnectGpio;
    /// Bitfield with '1' for GPIO really used as GPIO
    UINT32  usedGpio;
    /// Bitfield with '1' for GPO really used as GPO
    UINT32  usedGpo;
    /// Bitfield with '1' for TCO really used as TCO
    UINT32  usedTco;

    /// IO Drive Value
    HAL_CFG_IO_DRIVE_T ioDrive;

} HAL_CFG_CONFIG_T;


///@todo
/// - add: ADMUX (check only, in target only ?)
/// - change pwlGlowPosition: new enum (add in hal_pwl): invalid, none, pwl0, pwl1
/// .





// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

//

// =============================================================================
//  FUNCTIONS
// =============================================================================

#endif // _HAL_CONFIG_H_

