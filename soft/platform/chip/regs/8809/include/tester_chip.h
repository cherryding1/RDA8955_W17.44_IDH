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


#ifndef _TESTER_CHIP_H_
#define _TESTER_CHIP_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'tester_chip'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// TESTER_CHIP_T
// -----------------------------------------------------------------------------
/// This module is not inside the gallite chip. It's connected to a Chip select of
/// EBC and placed in the testbench for simulation. It is also partially included
/// in the FPGA.
// =============================================================================
#define REG_TESTER_CHIP_BASE        0x01880000

typedef volatile struct
{
    REG32                          Stopper;                      //0x00000000
    REG32                          Watch;                        //0x00000004
    REG32                          W0;                           //0x00000008
    REG32                          W1;                           //0x0000000C
    /// Test Mode reg, the bits of this register control some test component or test
    /// muxing in the testbench. (some of them are also used on the FPGA).
    REG32                          Test_Mode;                    //0x00000010
    REG32                          Input;                        //0x00000014
    /// Control bits to enable or disable some test components of the testbench.
    REG32                          Test_Mode1;                   //0x00000018
    REG32 Reserved_0000001C;                    //0x0000001C
    REG32                          W2_set;                       //0x00000020
    REG32                          W2_clr;                       //0x00000024
    REG32                          Key0;                         //0x00000028
    REG32                          Key1;                         //0x0000002C
    REG16                          STR_Print;                    //0x00000030
    REG16 Reserved_00000032;                    //0x00000032
    /// For simulation only.
    /// Write test number and state to display it as a string, the tester chip use
    /// a file to associate a string to the numbers.
    REG16                          TestPrint[2];                 //0x00000034
    REG32 Reserved_00000038[2];                 //0x00000038
    /// This register is not in the FPGA but on the Romulator connected to it
    REG32                          Led_Ctrl;                     //0x00000040
    /// This register is not in the FPGA but on the Romulator connected to it
    REG32                          EXL_Ctrl;                     //0x00000044
    /// This register is not in the FPGA but on the Romulator connected to it
    REG32                          EXL_Trigger_Set;              //0x00000048
    /// This register is not in the FPGA but on the Romulator connected to it
    REG32                          EXL_Trigger_Clr;              //0x0000004C
    /// This register is not in the FPGA but on the Romulator connected to it
    /// Logic Analyser register
    /// the 16 lower bits of register LA(0) and LA(1) are common.
    REG32                          LA_set[4];                    //0x00000050
    /// This register is not in the FPGA but on the Romulator connected to it
    /// Logic Analizer register
    /// the 16 lower bits of register LA(0) and LA(1) are common.
    REG32                          LA_clr[4];                    //0x00000060
    /// This register is not in the FPGA but on the Romulator connected to it
    REG32                          PXTS;                         //0x00000070
} HWP_TESTER_CHIP_T;

#define hwp_testerChip              ((HWP_TESTER_CHIP_T*) KSEG1(REG_TESTER_CHIP_BASE))


//Stopper
#define TESTER_CHIP_STOP_CAUSE(n)   (((n)&0xFFFF)<<0)
#define TESTER_CHIP_STOP_CAUSE_SUCCESS (0x1<<0)
#define TESTER_CHIP_STOP_CAUSE_ERROR (0x2<<0)

//Watch
#define TESTER_CHIP_WATCH(n)        (((n)&0xFFFF)<<0)

//W0
#define TESTER_CHIP_W0(n)           (((n)&0xFFFFFFFF)<<0)

//W1
#define TESTER_CHIP_W1(n)           (((n)&0xFFFFFFFF)<<0)

//Test_Mode
#define TESTER_CHIP_CLK_SYSTEM_SEL_CLK_26M (0<<0)
#define TESTER_CHIP_CLK_SYSTEM_SEL_CLK_OUT (1<<0)
#define TESTER_CHIP_CLK_HST_DISABLE (1<<1)
#define TESTER_CHIP_RFSPI_AREO1     (1<<2)
#define TESTER_CHIP_RFSPI_AREO2     (1<<3)
#define TESTER_CHIP_SPI_LOOP0_EN    (1<<4)
#define TESTER_CHIP_SPI_SIMULATOR_SEL (1<<5)
#define TESTER_CHIP_DAI_MODE        (1<<6)
#define TESTER_CHIP_KEYON_VALUE     (1<<7)
#define TESTER_CHIP_UART_DIVIDER(n) (((n)&0xFF)<<8)
#define TESTER_CHIP_AUDIO_CLK_DIV2_DIS (1<<16)
#define TESTER_CHIP_UART_SEL(n)     (((n)&3)<<17)
#define TESTER_CHIP_GPIO_SEL        (1<<19)
#define TESTER_CHIP_GPO_SEL         (1<<20)
#define TESTER_CHIP_I2C_SEL         (1<<21)
#define TESTER_CHIP_DAI_I2S_MODE    (1<<22)
#define TESTER_CHIP_I2S_SIMULATOR_SEL (1<<23)
#define TESTER_CHIP_ANALOG_TEST_SEL (1<<24)
#define TESTER_CHIP_BOOT_MODE_TEST_SEL (1<<25)
#define TESTER_CHIP_DEBUG_PORT_SIMULATOR_SEL_DISABLE (0<<26)
#define TESTER_CHIP_DEBUG_PORT_SIMULATOR_SEL_ENABLE (1<<26)
#define TESTER_CHIP_DEBUG_CLOCK_CHECKER_SEL (1<<27)
#define TESTER_CHIP_USB_SIMULATOR_SEL(n) (((n)&3)<<28)
#define TESTER_CHIP_USB_EXTPHY_SEL  (1<<30)
#define TESTER_CHIP_ADMUX_MEMORIES  (1<<31)

//Input
#define TESTER_CHIP_INPUT(n)        (((n)&0xFFFFFFFF)<<0)

//Test_Mode1
#define TESTER_CHIP_CAMERA_SIMULATOR_DISABLE (0<<0)
#define TESTER_CHIP_CAMERA_SIMULATOR_ENABLE (1<<0)
#define TESTER_CHIP_SDMMC_SIMULATOR_DISABLE (0<<1)
#define TESTER_CHIP_SDMMC_SIMULATOR_ENABLE (1<<1)
#define TESTER_CHIP_DAC_BIST_SIMULATOR_DISABLE (0<<2)
#define TESTER_CHIP_DAC_BIST_SIMULATOR_ENABLE (1<<2)
#define TESTER_CHIP_SPI2_SIMULATOR_DISABLE (0<<3)
#define TESTER_CHIP_SPI2_SIMULATOR_ENABLE (1<<3)
#define TESTER_CHIP_PWM_CHECKER_DISABLE (0<<4)
#define TESTER_CHIP_PWM_CHECKER_ENABLE (1<<4)
#define TESTER_CHIP_DAI_SIMULATOR_DISABLE (0<<5)
#define TESTER_CHIP_DAI_SIMULATOR_ENABLE (1<<5)
#define TESTER_CHIP_I2S_SIMULATOR_DISABLE (0<<6)
#define TESTER_CHIP_I2S_SIMULATOR_ENABLE (1<<6)
#define TESTER_CHIP_DIGRF_SIMULATOR_DISABLE (0<<7)
#define TESTER_CHIP_DIGRF_SIMULATOR_ENABLE (1<<7)
#define TESTER_CHIP_SLCD_SIMULATOR_DISABLE (0<<8)
#define TESTER_CHIP_SLCD_SIMULATOR_ENABLE (1<<8)
#define TESTER_CHIP_PLCD_SIMULATOR_DISABLE (0<<9)
#define TESTER_CHIP_PLCD_SIMULATOR_ENABLE (1<<9)
#define TESTER_CHIP_SPI_SIMULATOR_DISABLE (0<<10)
#define TESTER_CHIP_SPI_SIMULATOR_ENABLE (1<<10)
#define TESTER_CHIP_SPI_SIMULATOR_MODE_LOOP_CS0 (0<<11)
#define TESTER_CHIP_SPI_SIMULATOR_MODE_LOOP_CS1 (1<<11)
#define TESTER_CHIP_SPI_SIMULATOR_MODE_LOOP_CS2 (2<<11)
#define TESTER_CHIP_SPI_SIMULATOR_MODE_DIO_D0 (3<<11)
#define TESTER_CHIP_SPI_SIMULATOR_MODE_DIO_D1 (4<<11)
#define TESTER_CHIP_TESTER_CHIP_CHIPSELECT_M_CS_3 (0<<14)
#define TESTER_CHIP_TESTER_CHIP_CHIPSELECT_M_CS_4 (1<<14)
#define TESTER_CHIP_GPIO_IN_DISABLE (0<<15)
#define TESTER_CHIP_GPIO_IN_ENABLE  (1<<15)
#define TESTER_CHIP_DRIVE_BOND_ID_DISABLE (0<<16)
#define TESTER_CHIP_DRIVE_BOND_ID_ENABLE (1<<16)
#define TESTER_CHIP_EXT_M_CS_0_DESTINATION_FLASH (1<<17)
#define TESTER_CHIP_EXT_M_CS_0_DESTINATION_ROM (0<<17)
#define TESTER_CHIP_REMAP_CHECKER_DISABLE (0<<18)
#define TESTER_CHIP_REMAP_CHECKER_ENABLE (1<<18)
#define TESTER_CHIP_CLOCK_CHECKER_SOURCE_CLK_DEBUG (0<<19)
#define TESTER_CHIP_CLOCK_CHECKER_SOURCE_CLK_OUT (1<<19)
#define TESTER_CHIP_CLOCK_CHECKER_SOURCE_CLK_OSC (2<<19)
#define TESTER_CHIP_DEBUG_PORT_SPY_SIMULATOR_SEL_DISABLE (0<<21)
#define TESTER_CHIP_DEBUG_PORT_SPY_SIMULATOR_SEL_ENABLE (1<<21)
#define TESTER_CHIP_I2C2_SEL_DISABLE (0<<22)
#define TESTER_CHIP_I2C2_SEL_ENABLE (1<<22)
#define TESTER_CHIP_I2C3_SEL_DISABLE (0<<23)
#define TESTER_CHIP_I2C3_SEL_ENABLE (1<<23)
#define TESTER_CHIP_SDAT3_SEL_DISABLE (0<<24)
#define TESTER_CHIP_SDAT3_SEL_ENABLE (1<<24)
#define TESTER_CHIP_SDAT3_VAL_VAL_0 (0<<25)
#define TESTER_CHIP_SDAT3_VAL_VAL_1 (1<<25)
#define TESTER_CHIP_RF_CLK_26M_DISABLE (0<<26)
#define TESTER_CHIP_RF_CLK_26M_ENABLE (1<<26)
#define TESTER_CHIP_KEYPAD_DISABLE  (0<<27)
#define TESTER_CHIP_KEYPAD_ENABLE   (1<<27)

//W2_set
#define TESTER_CHIP_W2_SET(n)       (((n)&0xFFFFFFFF)<<0)

//W2_clr
#define TESTER_CHIP_W2_CLR(n)       (((n)&0xFFFFFFFF)<<0)

//Key0
#define TESTER_CHIP_KEY0(n)         (((n)&0xFFFFFFFF)<<0)

//Key1
#define TESTER_CHIP_KEY1(n)         (((n)&0xFFFFFFFF)<<0)

//STR_Print
#define TESTER_CHIP_CHAR(n)         (((n)&0xFF)<<0)

//TestPrint
#define TESTER_CHIP_TEST_NUMBER(n)  (((n)&0x3F)<<0)
#define TESTER_CHIP_STATE(n)        (((n)&3)<<6)

//Led_Ctrl
#define TESTER_CHIP_LED_CTRL(n)     (((n)&0xFFFF)<<0)

//EXL_Ctrl
#define TESTER_CHIP_EXL_CTRL        (1<<0)

//EXL_Trigger_Set
#define TESTER_CHIP_EXL_TRIGGER_SET(n) (((n)&0xFFFF)<<0)

//EXL_Trigger_Clr
#define TESTER_CHIP_EXL_TRIGGER_CLR(n) (((n)&0xFFFF)<<0)

//LA_set
#define TESTER_CHIP_LA_SET(n)       (((n)&0xFFFFFFFF)<<0)

//LA_clr
#define TESTER_CHIP_LA_CLR(n)       (((n)&0xFFFFFFFF)<<0)

//PXTS
#define TESTER_CHIP_PXTS(n)         (((n)&0xFFFF)<<0)





#endif

