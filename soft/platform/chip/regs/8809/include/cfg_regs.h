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


#ifndef _CFG_REGS_H_
#define _CFG_REGS_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'cfg_regs'."
#endif



// =============================================================================
//  MACROS
// =============================================================================
#define GALLITE_GPIO_ONLY_NB                     (1)
#define GALLITE_GPIO_NB                          (32)
#define GALLITE_GPO_NB                           (10)
#define PROD_ID                                  (0X8809)

// ============================================================================
// GPIO_MAPPING_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef enum
{
    PIN_KEYIN_6                                 = 0x00000001,
    PIN_HST_CTS                                 = 0x00000002,
    PIN_HST_RTS                                 = 0x00000003,
    PIN_LPSCO_1                                 = 0x00000004,
    PIN_UART2_CTS                               = 0x00000005,
    PIN_UART2_TXD                               = 0x00000006,
    PIN_UART2_RTS                               = 0x00000007,
    PIN_UART2_RXD                               = 0x00000008,
    PIN_KEYOUT_5                                = 0x00000009,
    PIN_KEYIN_5                                 = 0x0000000A,
    PIN_I2S_DI_1                                = 0x0000000B,
    PIN_SPI1_CLK                                = 0x0000000C,
    PIN_SPI1_CS_0                               = 0x0000000D,
    PIN_SPI1_CS_1                               = 0x0000000E,
    PIN_SPI1_DIO                                = 0x0000000F,
    PIN_SPI1_DI                                 = 0x00000010,
    PIN_I2S_BCK                                 = 0x00000011,
    PIN_LCD_DATA_8                              = 0x00000012,
    PIN_LCD_DATA_9                              = 0x00000013,
    PIN_UART1_CTS                               = 0x00000014,
    PIN_UART1_RTS                               = 0x00000015,
    PIN_M_CS_2                                  = 0x00000016,
    PIN_M_CS_3                                  = 0x00000017,
    PIN_I2C2_SDA                                = 0x00000018,
    PIN_I2C2_SCL                                = 0x00000019,
    PIN_LCD_DATA_10                             = 0x0000001A,
    PIN_LCD_DATA_11                             = 0x0000001B,
    PIN_LCD_DATA_12                             = 0x0000001C,
    PIN_LCD_DATA_13                             = 0x0000001D,
    PIN_LCD_DATA_14                             = 0x0000001E,
    PIN_LCD_DATA_15                             = 0x0000001F
} GPIO_MAPPING_T;


// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// CFG_REGS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CONFIG_REGS_BASE        0x01A24000

typedef volatile struct
{
    REG32                          CHIP_ID;                      //0x00000000
    /// This register contain the synthesis date and version
    REG32                          Build_Version;                //0x00000004
    /// Setting bit n to '1' selects GPIO Usage for PAD connected to GPIOn. Setting
    /// bit n to '0' selects Alt.
    REG32                          GPIO_Mode;                    //0x00000008
    REG32                          Alt_mux_select;               //0x0000000C
    REG32                          IO_Drive1_Select;             //0x00000010
    REG32                          IO_Drive2_Select;             //0x00000014
    REG32                          audio_pd_set;                 //0x00000018
    REG32                          audio_pd_clr;                 //0x0000001C
    REG32                          audio_sel_cfg;                //0x00000020
    REG32                          audio_mic_cfg;                //0x00000024
    REG32                          audio_spk_cfg;                //0x00000028
    REG32                          audio_rcv_gain;               //0x0000002C
    REG32                          audio_head_gain;              //0x00000030
} HWP_CFG_REGS_T;

#define hwp_configRegs              ((HWP_CFG_REGS_T*) KSEG1(REG_CONFIG_REGS_BASE))


//CHIP_ID
#define CFG_REGS_METAL_ID(n)        (((n)&0xFFF)<<0)
#define CFG_REGS_METAL_ID_MASK      (0xFFF<<0)
#define CFG_REGS_METAL_ID_SHIFT     (0)
#define CFG_REGS_BOND_ID(n)         (((n)&7)<<12)
#define CFG_REGS_BOND_ID_MASK       (7<<12)
#define CFG_REGS_BOND_ID_SHIFT      (12)
#define CFG_REGS_PROD_ID(n)         (((n)&0xFFFF)<<16)
#define CFG_REGS_PROD_ID_MASK       (0xFFFF<<16)
#define CFG_REGS_PROD_ID_SHIFT      (16)

//Build_Version
#define CFG_REGS_MAJOR(n)           (((n)&15)<<28)
#define CFG_REGS_YEAR(n)            (((n)&15)<<24)
#define CFG_REGS_MONTH(n)           (((n)&0xFF)<<16)
#define CFG_REGS_DAY(n)             (((n)&0xFF)<<8)
#define CFG_REGS_BUILD_STYLE_FPGA   (0<<4)
#define CFG_REGS_BUILD_STYLE_CHIP   (1<<4)
#define CFG_REGS_BUILD_STYLE_FPGA_USB (2<<4)
#define CFG_REGS_BUILD_STYLE_FPGA_GSM (3<<4)
#define CFG_REGS_BUILD_REVISION(n)  (((n)&15)<<0)

//GPIO_Mode
#define CFG_REGS_MODE_PIN_KEYIN_6   (1<<1)
#define CFG_REGS_MODE_PIN_KEYIN_6_MASK (1<<1)
#define CFG_REGS_MODE_PIN_KEYIN_6_SHIFT (1)
#define CFG_REGS_MODE_PIN_KEYIN_6_ALT (0<<1)
#define CFG_REGS_MODE_PIN_KEYIN_6_GPIO (1<<1)
#define CFG_REGS_MODE_PIN_HST_CTS   (1<<2)
#define CFG_REGS_MODE_PIN_HST_CTS_MASK (1<<2)
#define CFG_REGS_MODE_PIN_HST_CTS_SHIFT (2)
#define CFG_REGS_MODE_PIN_HST_CTS_ALT (0<<2)
#define CFG_REGS_MODE_PIN_HST_CTS_GPIO (1<<2)
#define CFG_REGS_MODE_PIN_HST_RTS   (1<<3)
#define CFG_REGS_MODE_PIN_HST_RTS_MASK (1<<3)
#define CFG_REGS_MODE_PIN_HST_RTS_SHIFT (3)
#define CFG_REGS_MODE_PIN_HST_RTS_ALT (0<<3)
#define CFG_REGS_MODE_PIN_HST_RTS_GPIO (1<<3)
#define CFG_REGS_MODE_PIN_LPSCO_1   (1<<4)
#define CFG_REGS_MODE_PIN_LPSCO_1_MASK (1<<4)
#define CFG_REGS_MODE_PIN_LPSCO_1_SHIFT (4)
#define CFG_REGS_MODE_PIN_LPSCO_1_ALT (0<<4)
#define CFG_REGS_MODE_PIN_LPSCO_1_GPIO (1<<4)
#define CFG_REGS_MODE_PIN_UART2_CTS (1<<5)
#define CFG_REGS_MODE_PIN_UART2_CTS_MASK (1<<5)
#define CFG_REGS_MODE_PIN_UART2_CTS_SHIFT (5)
#define CFG_REGS_MODE_PIN_UART2_CTS_ALT (0<<5)
#define CFG_REGS_MODE_PIN_UART2_CTS_GPIO (1<<5)
#define CFG_REGS_MODE_PIN_SPI1_CS_0 (1<<6)
#define CFG_REGS_MODE_PIN_SPI1_CS_0_MASK (1<<6)
#define CFG_REGS_MODE_PIN_SPI1_CS_0_SHIFT (6)
#define CFG_REGS_MODE_PIN_SPI1_CS_0_ALT (0<<6)
#define CFG_REGS_MODE_PIN_SPI1_CS_0_GPIO (1<<6)
#define CFG_REGS_MODE_PIN_UART2_RTS (1<<7)
#define CFG_REGS_MODE_PIN_UART2_RTS_MASK (1<<7)
#define CFG_REGS_MODE_PIN_UART2_RTS_SHIFT (7)
#define CFG_REGS_MODE_PIN_UART2_RTS_ALT (0<<7)
#define CFG_REGS_MODE_PIN_UART2_RTS_GPIO (1<<7)
#define CFG_REGS_MODE_PIN_UART2_RXD (1<<8)
#define CFG_REGS_MODE_PIN_UART2_RXD_MASK (1<<8)
#define CFG_REGS_MODE_PIN_UART2_RXD_SHIFT (8)
#define CFG_REGS_MODE_PIN_UART2_RXD_ALT (0<<8)
#define CFG_REGS_MODE_PIN_UART2_RXD_GPIO (1<<8)
#define CFG_REGS_MODE_PIN_KEYOUT_5  (1<<9)
#define CFG_REGS_MODE_PIN_KEYOUT_5_MASK (1<<9)
#define CFG_REGS_MODE_PIN_KEYOUT_5_SHIFT (9)
#define CFG_REGS_MODE_PIN_KEYOUT_5_ALT (0<<9)
#define CFG_REGS_MODE_PIN_KEYOUT_5_GPIO (1<<9)
#define CFG_REGS_MODE_PIN_KEYIN_5   (1<<10)
#define CFG_REGS_MODE_PIN_KEYIN_5_MASK (1<<10)
#define CFG_REGS_MODE_PIN_KEYIN_5_SHIFT (10)
#define CFG_REGS_MODE_PIN_KEYIN_5_ALT (0<<10)
#define CFG_REGS_MODE_PIN_KEYIN_5_GPIO (1<<10)
#define CFG_REGS_MODE_PIN_I2S_DI_1  (1<<11)
#define CFG_REGS_MODE_PIN_I2S_DI_1_MASK (1<<11)
#define CFG_REGS_MODE_PIN_I2S_DI_1_SHIFT (11)
#define CFG_REGS_MODE_PIN_I2S_DI_1_ALT (0<<11)
#define CFG_REGS_MODE_PIN_I2S_DI_1_GPIO (1<<11)
#define CFG_REGS_MODE_PIN_SPI1_CLK  (1<<12)
#define CFG_REGS_MODE_PIN_SPI1_CLK_MASK (1<<12)
#define CFG_REGS_MODE_PIN_SPI1_CLK_SHIFT (12)
#define CFG_REGS_MODE_PIN_SPI1_CLK_ALT (0<<12)
#define CFG_REGS_MODE_PIN_SPI1_CLK_GPIO (1<<12)
#define CFG_REGS_MODE_PIN_UART2_TXD (1<<13)
#define CFG_REGS_MODE_PIN_UART2_TXD_MASK (1<<13)
#define CFG_REGS_MODE_PIN_UART2_TXD_SHIFT (13)
#define CFG_REGS_MODE_PIN_UART2_TXD_ALT (0<<13)
#define CFG_REGS_MODE_PIN_UART2_TXD_GPIO (1<<13)
#define CFG_REGS_MODE_PIN_SPI1_CS_1 (1<<14)
#define CFG_REGS_MODE_PIN_SPI1_CS_1_MASK (1<<14)
#define CFG_REGS_MODE_PIN_SPI1_CS_1_SHIFT (14)
#define CFG_REGS_MODE_PIN_SPI1_CS_1_ALT (0<<14)
#define CFG_REGS_MODE_PIN_SPI1_CS_1_GPIO (1<<14)
#define CFG_REGS_MODE_PIN_SPI1_DIO  (1<<15)
#define CFG_REGS_MODE_PIN_SPI1_DIO_MASK (1<<15)
#define CFG_REGS_MODE_PIN_SPI1_DIO_SHIFT (15)
#define CFG_REGS_MODE_PIN_SPI1_DIO_ALT (0<<15)
#define CFG_REGS_MODE_PIN_SPI1_DIO_GPIO (1<<15)
#define CFG_REGS_MODE_PIN_SPI1_DI   (1<<16)
#define CFG_REGS_MODE_PIN_SPI1_DI_MASK (1<<16)
#define CFG_REGS_MODE_PIN_SPI1_DI_SHIFT (16)
#define CFG_REGS_MODE_PIN_SPI1_DI_ALT (0<<16)
#define CFG_REGS_MODE_PIN_SPI1_DI_GPIO (1<<16)
#define CFG_REGS_MODE_PIN_I2S_BCK   (1<<17)
#define CFG_REGS_MODE_PIN_I2S_BCK_MASK (1<<17)
#define CFG_REGS_MODE_PIN_I2S_BCK_SHIFT (17)
#define CFG_REGS_MODE_PIN_I2S_BCK_ALT (0<<17)
#define CFG_REGS_MODE_PIN_I2S_BCK_GPIO (1<<17)
#define CFG_REGS_MODE_PIN_LCD_DATA_8 (1<<18)
#define CFG_REGS_MODE_PIN_LCD_DATA_8_MASK (1<<18)
#define CFG_REGS_MODE_PIN_LCD_DATA_8_SHIFT (18)
#define CFG_REGS_MODE_PIN_LCD_DATA_8_ALT (0<<18)
#define CFG_REGS_MODE_PIN_LCD_DATA_8_GPIO (1<<18)
#define CFG_REGS_MODE_PIN_LCD_DATA_9 (1<<19)
#define CFG_REGS_MODE_PIN_LCD_DATA_9_MASK (1<<19)
#define CFG_REGS_MODE_PIN_LCD_DATA_9_SHIFT (19)
#define CFG_REGS_MODE_PIN_LCD_DATA_9_ALT (0<<19)
#define CFG_REGS_MODE_PIN_LCD_DATA_9_GPIO (1<<19)
#define CFG_REGS_MODE_PIN_UART1_CTS (1<<20)
#define CFG_REGS_MODE_PIN_UART1_CTS_MASK (1<<20)
#define CFG_REGS_MODE_PIN_UART1_CTS_SHIFT (20)
#define CFG_REGS_MODE_PIN_UART1_CTS_ALT (0<<20)
#define CFG_REGS_MODE_PIN_UART1_CTS_GPIO (1<<20)
#define CFG_REGS_MODE_PIN_UART1_RTS (1<<21)
#define CFG_REGS_MODE_PIN_UART1_RTS_MASK (1<<21)
#define CFG_REGS_MODE_PIN_UART1_RTS_SHIFT (21)
#define CFG_REGS_MODE_PIN_UART1_RTS_ALT (0<<21)
#define CFG_REGS_MODE_PIN_UART1_RTS_GPIO (1<<21)
#define CFG_REGS_MODE_PIN_M_CS_2    (1<<22)
#define CFG_REGS_MODE_PIN_M_CS_2_MASK (1<<22)
#define CFG_REGS_MODE_PIN_M_CS_2_SHIFT (22)
#define CFG_REGS_MODE_PIN_M_CS_2_ALT (0<<22)
#define CFG_REGS_MODE_PIN_M_CS_2_GPIO (1<<22)
#define CFG_REGS_MODE_PIN_M_CS_3    (1<<23)
#define CFG_REGS_MODE_PIN_M_CS_3_MASK (1<<23)
#define CFG_REGS_MODE_PIN_M_CS_3_SHIFT (23)
#define CFG_REGS_MODE_PIN_M_CS_3_ALT (0<<23)
#define CFG_REGS_MODE_PIN_M_CS_3_GPIO (1<<23)
#define CFG_REGS_MODE_PIN_I2C2_SDA  (1<<24)
#define CFG_REGS_MODE_PIN_I2C2_SDA_MASK (1<<24)
#define CFG_REGS_MODE_PIN_I2C2_SDA_SHIFT (24)
#define CFG_REGS_MODE_PIN_I2C2_SDA_ALT (0<<24)
#define CFG_REGS_MODE_PIN_I2C2_SDA_GPIO (1<<24)
#define CFG_REGS_MODE_PIN_I2C2_SCL  (1<<25)
#define CFG_REGS_MODE_PIN_I2C2_SCL_MASK (1<<25)
#define CFG_REGS_MODE_PIN_I2C2_SCL_SHIFT (25)
#define CFG_REGS_MODE_PIN_I2C2_SCL_ALT (0<<25)
#define CFG_REGS_MODE_PIN_I2C2_SCL_GPIO (1<<25)
#define CFG_REGS_MODE_PIN_LCD_DATA_10 (1<<26)
#define CFG_REGS_MODE_PIN_LCD_DATA_10_MASK (1<<26)
#define CFG_REGS_MODE_PIN_LCD_DATA_10_SHIFT (26)
#define CFG_REGS_MODE_PIN_LCD_DATA_10_ALT (0<<26)
#define CFG_REGS_MODE_PIN_LCD_DATA_10_GPIO (1<<26)
#define CFG_REGS_MODE_PIN_LCD_DATA_11 (1<<27)
#define CFG_REGS_MODE_PIN_LCD_DATA_11_MASK (1<<27)
#define CFG_REGS_MODE_PIN_LCD_DATA_11_SHIFT (27)
#define CFG_REGS_MODE_PIN_LCD_DATA_11_ALT (0<<27)
#define CFG_REGS_MODE_PIN_LCD_DATA_11_GPIO (1<<27)
#define CFG_REGS_MODE_PIN_LCD_DATA_12 (1<<28)
#define CFG_REGS_MODE_PIN_LCD_DATA_12_MASK (1<<28)
#define CFG_REGS_MODE_PIN_LCD_DATA_12_SHIFT (28)
#define CFG_REGS_MODE_PIN_LCD_DATA_12_ALT (0<<28)
#define CFG_REGS_MODE_PIN_LCD_DATA_12_GPIO (1<<28)
#define CFG_REGS_MODE_PIN_LCD_DATA_13 (1<<29)
#define CFG_REGS_MODE_PIN_LCD_DATA_13_MASK (1<<29)
#define CFG_REGS_MODE_PIN_LCD_DATA_13_SHIFT (29)
#define CFG_REGS_MODE_PIN_LCD_DATA_13_ALT (0<<29)
#define CFG_REGS_MODE_PIN_LCD_DATA_13_GPIO (1<<29)
#define CFG_REGS_MODE_PIN_LCD_DATA_14 (1<<30)
#define CFG_REGS_MODE_PIN_LCD_DATA_14_MASK (1<<30)
#define CFG_REGS_MODE_PIN_LCD_DATA_14_SHIFT (30)
#define CFG_REGS_MODE_PIN_LCD_DATA_14_ALT (0<<30)
#define CFG_REGS_MODE_PIN_LCD_DATA_14_GPIO (1<<30)
#define CFG_REGS_MODE_PIN_LCD_DATA_15 (1<<31)
#define CFG_REGS_MODE_PIN_LCD_DATA_15_MASK (1<<31)
#define CFG_REGS_MODE_PIN_LCD_DATA_15_SHIFT (31)
#define CFG_REGS_MODE_PIN_LCD_DATA_15_ALT (0<<31)
#define CFG_REGS_MODE_PIN_LCD_DATA_15_GPIO (1<<31)
#define CFG_REGS_GPIO_MODE(n)       (((n)&0x7FFFFFFF)<<1)
#define CFG_REGS_GPIO_MODE_MASK     (0x7FFFFFFF<<1)
#define CFG_REGS_GPIO_MODE_SHIFT    (1)

//Alt_mux_select
#define CFG_REGS_DEBUG_PORT_MASK    (3<<0)
#define CFG_REGS_DEBUG_PORT_DEBUG   (0<<0)
#define CFG_REGS_DEBUG_PORT_CAMERA  (1<<0)
#define CFG_REGS_DEBUG_PORT_DAI     (2<<0)
#define CFG_REGS_DEBUG_PORT_DAI_SIMPLE (3<<0)
#define CFG_REGS_LCD_CS_0_MASK      (1<<2)
#define CFG_REGS_LCD_CS_0_LCD_CS_0  (0<<2)
#define CFG_REGS_LCD_CS_0_SPI1_CS_2 (1<<2)
#define CFG_REGS_LCD_CS_1_MASK      (1<<3)
#define CFG_REGS_LCD_CS_1_LCD_CS_1  (0<<3)
#define CFG_REGS_LCD_CS_1_GPO_7     (1<<3)
#define CFG_REGS_MEMORY_ADDRESS_24_MASK (1<<4)
#define CFG_REGS_MEMORY_ADDRESS_24_MA_24 (0<<4)
#define CFG_REGS_MEMORY_ADDRESS_24_M_CS4 (1<<4)
#define CFG_REGS_SPI1_SELECT_MASK   (1<<5)
#define CFG_REGS_SPI1_SELECT_SPI1   (0<<5)
#define CFG_REGS_SPI1_SELECT_I2C_3  (1<<5)
#define CFG_REGS_SPI1_CS0_SELECT_MASK (1<<6)
#define CFG_REGS_SPI1_CS0_SELECT_SPI1_CS0 (0<<6)
#define CFG_REGS_SPI1_CS0_SELECT_KEYIN_7 (1<<6)
#define CFG_REGS_UART2_MASK         (1<<7)
#define CFG_REGS_UART2_UART2        (0<<7)
#define CFG_REGS_UART2_UART1        (1<<7)
#define CFG_REGS_PWL1_MASK          (3<<8)
#define CFG_REGS_PWL1_PWL1          (0<<8)
#define CFG_REGS_PWL1_GPO_6         (1<<8)
#define CFG_REGS_PWL1_CLK_32K       (2<<8)
#define CFG_REGS_SDMMC_MASK         (1<<10)
#define CFG_REGS_SDMMC_SDMMC        (0<<10)
#define CFG_REGS_SDMMC_DIGRF        (1<<10)
#define CFG_REGS_I2C1_MASK          (1<<12)
#define CFG_REGS_I2C1_UART1         (0<<12)
#define CFG_REGS_I2C1_I2C1          (1<<12)
#define CFG_REGS_I2S_DI_2_MASK      (1<<13)
#define CFG_REGS_I2S_DI_2_LPSCO_1   (0<<13)
#define CFG_REGS_I2S_DI_2_I2S_DI_2  (1<<13)
#define CFG_REGS_GPO_0_MASK         (3<<14)
#define CFG_REGS_GPO_0_GPO_0        (0<<14)
#define CFG_REGS_GPO_0_KEYOUT_6     (1<<14)
#define CFG_REGS_GPO_0_USB          (2<<14)
#define CFG_REGS_GPO_4_1_MASK       (3<<16)
#define CFG_REGS_GPO_4_1_GPO_4_1    (0<<16)
#define CFG_REGS_GPO_4_1_SPI2       (1<<16)
#define CFG_REGS_GPO_4_1_USB        (2<<16)
#define CFG_REGS_GPO_5_MASK         (3<<18)
#define CFG_REGS_GPO_5_GPO_5        (0<<18)
#define CFG_REGS_GPO_5_KEYOUT_7     (1<<18)
#define CFG_REGS_GPO_5_USB          (2<<18)
#define CFG_REGS_GPO_5_SPI2_CS_1    (3<<18)
#define CFG_REGS_I2C2_MASK          (1<<20)
#define CFG_REGS_I2C2_I2C2          (0<<20)
#define CFG_REGS_I2C2_USB           (1<<20)
#define CFG_REGS_TCO_4_MASK         (1<<21)
#define CFG_REGS_TCO_4_TCO_4        (0<<21)
#define CFG_REGS_TCO_4_GPO_9        (1<<21)
#define CFG_REGS_TCO_3_MASK         (1<<22)
#define CFG_REGS_TCO_3_TCO_3        (0<<22)
#define CFG_REGS_TCO_3_GPO_8        (1<<22)
#define CFG_REGS_KEYOUT_5_MASK      (1<<23)
#define CFG_REGS_KEYOUT_5_KEYOUT_5  (0<<23)
#define CFG_REGS_KEYOUT_5_LPG       (1<<23)
#define CFG_REGS_KEYIN_5_MASK       (1<<24)
#define CFG_REGS_KEYIN_5_KEYIN_5    (0<<24)
#define CFG_REGS_KEYIN_5_PWT        (1<<24)
#define CFG_REGS_I2S_DO_MASK        (1<<25)
#define CFG_REGS_I2S_DO_I2S_DO      (0<<25)
#define CFG_REGS_I2S_DO_TCO_5       (1<<25)
#define CFG_REGS_SPI2_CAM_MASK      (1<<26)
#define CFG_REGS_SPI2_CAM_CAM       (0<<26)
#define CFG_REGS_SPI2_CAM_SPI2      (1<<26)
#define CFG_REGS_SERIAL_CAM_MASK    (1<<27)
#define CFG_REGS_SERIAL_CAM_DISABLE (0<<27)
#define CFG_REGS_SERIAL_CAM_ENABLE  (1<<27)
#define CFG_REGS_GPIO_CAM1_MASK     (1<<28)
#define CFG_REGS_GPIO_CAM1_CAM      (0<<28)
#define CFG_REGS_GPIO_CAM1_GPIO     (1<<28)
#define CFG_REGS_GPIO_CAM2_MASK     (1<<29)
#define CFG_REGS_GPIO_CAM2_CAM      (0<<29)
#define CFG_REGS_GPIO_CAM2_GPIO     (1<<29)
#define CFG_REGS_GPIO_CAM3_MASK     (1<<30)
#define CFG_REGS_GPIO_CAM3_CAM      (0<<30)
#define CFG_REGS_GPIO_CAM3_GPIO     (1<<30)

//IO_Drive1_Select
#define CFG_REGS_M_D_0_DRIVE_MASK     (7<<0)
#define CFG_REGS_M_D_0_DRIVE_FAST_FAST (4<<0)
#define CFG_REGS_M_D_1_DRIVE_MASK     (7<<3)
#define CFG_REGS_M_D_1_DRIVE_FAST_FAST (4<<3)
#define CFG_REGS_M_SPI_DRIVE_MASK     (7<<6)
#define CFG_REGS_M_SPI_DRIVE_FAST_FAST (4<<6)
#define CFG_REGS_LCD_DATA_DRIVE_MASK  (3<<9)
#define CFG_REGS_LCD_DATA_DRIVE_FAST_FAST (0<<9)
#define CFG_REGS_CAM_DRIVE_MASK     (3<<11)
#define CFG_REGS_CAM_DRIVE_FAST_FAST (0<<11)
#define CFG_REGS_GPO_DRIVE_MASK     (3<<13)
#define CFG_REGS_GPO_DRIVE_FAST_FAST (0<<13)
#define CFG_REGS_KEYPAD_DRIVE_MASK  (3<<15)
#define CFG_REGS_KEYPAD_DRIVE_FAST_FAST (0<<15)
#define CFG_REGS_SDMMC_CTRL_DRIVE_MASK (3<<17)
#define CFG_REGS_SDMMC_CTRL_DRIVE_FAST_FAST (0<<17)

//IO_Drive2_Select
#define CFG_REGS_M_SPI_D_MASK    (3<<0)
#define CFG_REGS_M_SPI_D_NORMAL  (0<<0)
#define CFG_REGS_M_SPI_D_INPUT   (1<<0)
#define CFG_REGS_M_SPI_D_OUTPUT0 (2<<0)
#define CFG_REGS_M_SPI_D_OUTPUT1 (3<<0)
#define CFG_REGS_SPI_CAM_LCD_MASK    (3<<2)
#define CFG_REGS_SPI_CAM_LCD_NORMAL  (0<<2)
#define CFG_REGS_SPI_CAM_LCD_DIRECT  (1<<2)
#define CFG_REGS_SPI_CAM_LCD_HIGHZ   (2<<2)
#define CFG_REGS_SPI_CAM_LCD_DIRECEGPIO (3<<2)
#define CFG_REGS_BT_FEED_THROUGH     (1<<4)
#define CFG_REGS_BT_POWER_ON         (1<<5)
#define CFG_REGS_FM_POWER_ON         (1<<6)
#define CFG_REGS_SDMMC2_MASK         (3<<7)
#define CFG_REGS_SDMMC2_NONE         (0<<7)
#define CFG_REGS_SDMMC2_SPI1_SDMMC2  (1<<7)
#define CFG_REGS_SDMMC2_LCD_SDMMC2   (2<<7)
#define CFG_REGS_SDMMC2_KEY_SDMMC2   (3<<7)
#define CFG_REGS_KEYIN_0_GPIO_20     (1<<9)
#define CFG_REGS_KEYIN_4_GPIO_21     (1<<10)
#define CFG_REGS_KEYOUT_1_GPIO_17    (1<<11)
#define CFG_REGS_KEYOUT_2_GPIO_11    (1<<12)
#define CFG_REGS_KEYOUT_3_GPIO_7     (1<<13)
#define CFG_REGS_KEYOUT_4_GPIO_8     (1<<14)
#define CFG_REGS_UART2_SPI1_UART2    (1<<15)
#define CFG_REGS_USB_ID              (1<<16)

//audio_pd_set
#define CFG_REGS_AU_DEEP_PD_N       (1<<0)
#define CFG_REGS_AU_REF_PD_N        (1<<1)
#define CFG_REGS_AU_MIC_PD_N        (1<<2)
#define CFG_REGS_AU_AUXMIC_PD_N     (1<<3)
#define CFG_REGS_AU_AD_PD_N         (1<<4)
#define CFG_REGS_AU_DAC_PD_N        (1<<5)
#define CFG_REGS_AU_DAC_RESET_N     (1<<8)

//audio_pd_clr
//#define CFG_REGS_AU_DEEP_PD_N     (1<<0)
//#define CFG_REGS_AU_REF_PD_N      (1<<1)
//#define CFG_REGS_AU_MIC_PD_N      (1<<2)
//#define CFG_REGS_AU_AUXMIC_PD_N   (1<<3)
//#define CFG_REGS_AU_AD_PD_N       (1<<4)
//#define CFG_REGS_AU_DAC_PD_N      (1<<5)
//#define CFG_REGS_AU_DAC_RESET_N   (1<<8)

//audio_sel_cfg
#define CFG_REGS_AU_AUXMIC_SEL      (1<<0)
#define CFG_REGS_AU_SPK_SEL         (1<<1)
#define CFG_REGS_AU_SPK_MONO_SEL    (1<<2)
#define CFG_REGS_AU_RCV_SEL         (1<<3)
#define CFG_REGS_AU_HEAD_SEL        (1<<4)
#define CFG_REGS_AU_FM_SEL          (1<<5)

//audio_mic_cfg
#define CFG_REGS_AU_MIC_GAIN(n)     (((n)&15)<<0)
#define CFG_REGS_AU_MIC_MUTE_N      (1<<4)

//audio_spk_cfg
#define CFG_REGS_AU_SPK_GAIN(n)     (((n)&15)<<0)
#define CFG_REGS_AU_SPK_MUTE_N      (1<<4)

//audio_rcv_gain
#define CFG_REGS_AU_RCV_GAIN_MASK    (15<<0)
#define CFG_REGS_AU_RCV_GAIN(n)     (((n)&15)<<0)

//audio_head_gain
#define CFG_REGS_AU_HEAD_GAIN_MASK    (15<<0)
#define CFG_REGS_AU_HEAD_GAIN(n)    (((n)&15)<<0)





#endif

