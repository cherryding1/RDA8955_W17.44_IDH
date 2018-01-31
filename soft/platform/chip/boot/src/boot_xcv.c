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

#include "boot_xcv.h"
#include "boot_sys.h"
#include "hal_rfspi.h"
#include "hal_timers.h"
#include "global_macros.h"
#include "rf_spi.h"

#ifdef CHIP_DIE_8955

// These macros are defined in "drv_xcv.h"
#define CMDWORD_ADDR(n)                 (((n) & 0x1FF) << 22)
#define CMDWORD_DATA(n)                 (((n) & 0x3FFFF) << 4)
#define CMDWORD_READ                    (1 << 31)
#define CMDWORD_WRITE_FMT(addr, data)   (CMDWORD_ADDR(addr) | CMDWORD_DATA(data))
#define CMDWORD_READ_FMT(addr)          (CMDWORD_ADDR(addr) | CMDWORD_READ)

PUBLIC VOID HAL_BOOT_FUNC boot_XcvOpen(VOID)
{
    hwp_rfSpi->Ctrl = RF_SPI_ENABLE
                      | !RF_SPI_CS_POLARITY        // csActiveHi
                      | RF_SPI_DIGRF_READ          // modeDigRfRead
                      | !RF_SPI_CLOCKED_BACK2BACK  // modeClockBack2Back
                      | !RF_SPI_INPUT_MODE         // inputEn
                      | !RF_SPI_CLOCK_POLARITY     // clkFallEdge
                      | RF_SPI_CLOCK_DELAY(HAL_RFSPI_HALF_CLK_PERIOD_1)    // clkDelay
                      | RF_SPI_DO_DELAY(HAL_RFSPI_HALF_CLK_PERIOD_2)       // doDelay
                      | RF_SPI_DI_DELAY(HAL_RFSPI_HALF_CLK_PERIOD_3)       // diDelay
                      | RF_SPI_CS_DELAY(HAL_RFSPI_HALF_CLK_PERIOD_2)       // csDelay
                      | RF_SPI_CS_END_HOLD(HAL_RFSPI_HALF_CLK_PERIOD_0)    // csEndHold
                      | RF_SPI_CS_END_PULSE(HAL_RFSPI_HALF_CLK_PERIOD_3)   // csEndPulse
                      | RF_SPI_FRAME_SIZE(27)          // frameSize
                      | RF_SPI_INPUT_FRAME_SIZE(17)    // inputFrameSize
                      | RF_SPI_TURNAROUND_TIME(2)      // turnAroundCycles
                      ;

    hwp_rfSpi->Command = 0;

    HAL_SYS_FREQ_T clk_sys = boot_SysGetFreq();
    if (clk_sys == HAL_SYS_FREQ_26M)
    {
        hwp_rfSpi->Divider = RF_SPI_DIVIDER(0);
    }
    else if (clk_sys == HAL_SYS_FREQ_52M)
    {
        hwp_rfSpi->Divider = RF_SPI_DIVIDER(1);
    }
    else
    {
        asm("break 1");
    }
}

PUBLIC VOID HAL_BOOT_FUNC boot_XcvClose(VOID)
{
    // Drive all the ouputs of the SPI interface to a logical '0'
    hwp_rfSpi->Command = RF_SPI_DRIVE_ZERO;

    // Disable the RF-SPI and clear the configuration
    // TODO Wouldn't just clear the enable bit be safer ?
    hwp_rfSpi->Ctrl = 0;
}

PUBLIC UINT32 HAL_BOOT_FUNC boot_XcvRead(UINT16 address)
{
    UINT32 cmdWord = CMDWORD_READ_FMT(address);

    UINT32 savedCfg = hwp_rfSpi->Ctrl; // save the original configuration

    // There are hard-coded size: (frameSize=27, inputFrameSize=17)
    // * CmdSize = 10bits -> 2byte
    // * RdSize = 18bits -> 3bytes

    // Change the SPI mode to input and change the frame size
    UINT32 readCfg = savedCfg & ~(RF_SPI_FRAME_SIZE(0x1F));
    readCfg |= RF_SPI_FRAME_SIZE(9);
    readCfg |= RF_SPI_INPUT_MODE;
    hwp_rfSpi->Ctrl = readCfg;

    // Fill the Tx fifo
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO | RF_SPI_FLUSH_RX_FIFO;
    hwp_rfSpi->Cmd_Data = (cmdWord >> 24) & 0xFF;
    hwp_rfSpi->Cmd_Data = (cmdWord >> 16) & 0xFF;

    // Set the readout size
    hwp_rfSpi->Cmd_Size = RF_SPI_CMD_SIZE(2);

    // And send the command
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the SPI to finish
    while ((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);

    // Wait until bytes ready, otherwise Rx FIFO will be underflow
    while (GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_RX_LEVEL) < 3);

    UINT8 data2 = (UINT8)hwp_rfSpi->Rx_Data;
    UINT8 data1 = (UINT8)hwp_rfSpi->Rx_Data;
    UINT8 data0 = (UINT8)(hwp_rfSpi->Rx_Data << 6);

    // Change the SPI mode back to output
    hwp_rfSpi->Ctrl = savedCfg;
    return ((data2 << 16) | (data1 << 8) | data0) >> 6;
}

PUBLIC VOID HAL_BOOT_FUNC boot_XcvWrite(UINT16 address, UINT32 data)
{
    UINT32 cmdWord = CMDWORD_WRITE_FMT(address, data);

    // Flush the Tx fifo
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO | RF_SPI_FLUSH_RX_FIFO;
    hwp_rfSpi->Cmd_Data = (cmdWord >> 24) & 0xFF;
    hwp_rfSpi->Cmd_Data = (cmdWord >> 16) & 0xFF;
    hwp_rfSpi->Cmd_Data = (cmdWord >> 8) & 0xFF;
    hwp_rfSpi->Cmd_Data = (cmdWord >> 0) & 0xFF;

    // Set the cmd size
    hwp_rfSpi->Cmd_Size = RF_SPI_CMD_SIZE(4);

    // And send the command
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the SPI to start - at least one byte has been sent
    while(GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_CMD_DATA_LEVEL) >= 4);
    // Wait for the SPI to finish
    while((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);
}

PUBLIC VOID HAL_BOOT_FUNC boot_XcvConfigure(VOID)
{
    boot_XcvOpen();

    boot_XcvWrite(0x30, 0x00a00); // Soft Resetn down
    boot_SysWaitMicrosecond(10000); //Delay 10ms
#ifdef CHANNEL_FREQUENCY_MODE
    boot_XcvWrite(0x30, 0x00555); // Soft Resetn up
#else
    boot_XcvWrite(0x30, 0x00515); // Soft Resetn up
#endif

#ifdef USE_EXT_XTAL_32K //EXT 32K enable
    boot_XcvWrite(0xc4, 0x163f0); // Pu_xtal_pmu=1
    boot_SysWaitMicrosecond(100); //Delay 100us
    boot_XcvWrite(0xc4, 0x063f0); //clk32K_lpo_en_pmu=0
    boot_SysWaitMicrosecond(100); //Delay 100us
    boot_XcvWrite(0xc4, 0x0e3f0); //clk32K_xtal_en_pmu=0
    boot_SysWaitMicrosecond(100); //Delay 100us
    boot_XcvWrite(0xc4, 0x0a3f0); //lpo_32K_pu=0
#else //26M Div 32k enable
    // Direct-reg pu_xtal=1 before enabling 6p5m to avoid 26M clock burr
    boot_XcvWrite(0x20, 0x310a1);
    boot_XcvWrite(0xc1, 0x3e410); // Enable_clk_6p5m=1
    // Restore pu_xtal setting after enabling 6p5m
    boot_XcvWrite(0x20, 0x110a1);
    boot_XcvWrite(0xc4, 0x043f0); // clk32k_lpo_en_pmu=0
    boot_SysWaitMicrosecond(100);
    boot_XcvWrite(0xc4, 0x203f1); // clk32k_26mXtal_en_pmu=1 Xen_bt_enable=1
#endif

    boot_XcvWrite(0x101, 0x04926); // pll_cpaux_bit=7  0x01f26
    boot_XcvWrite(0x10d, 0x0a021);
    boot_XcvWrite(0x10e, 0x0a021);
    boot_XcvWrite( 0x05, 0xa950);//0x2a950

    boot_XcvClose();
}
#else

#ifdef CHIP_DIE_8909
// TODO:implement them for 8909
PUBLIC VOID HAL_BOOT_FUNC boot_XcvOpen(VOID) {}
PUBLIC VOID HAL_BOOT_FUNC boot_XcvConfigure(VOID) {}
#endif

#endif// CHIP_DIE_8955
