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

#include "cs_types.h"
#include "global_macros.h"
#include "sys_ctrl.h"
#include "spi.h"
#include "rf_spi.h"
#include "psram8_ctrl.h"
#include "debug_host.h"
#include "timer.h"
#include "iomux.h"
#include "../pmu_8955/include/rda1203.h"
#include "pmd_m.h"
#include "bl_config.h"
#include "bl_platform.h"
#include "bl_rom_api.h"
#include "bl_pmd.h"

#define BL_SYS_FREQ (104000000)

#define hwp_ispi hwp_spi3
#define ISPI_FREQ 13000000
#define RFSPI_FREQ 13000000

// This is for LDO/6 156M
#define PSRAM8_CLK_CTRL 0x2b
#define PSRAM8_DQS_CTRL 0x2b1010
#define PSRAM8_MR0 0xf
#define PSRAM8_MR4 0

// These macros are defined in "drv_xcv.h"
#define CMDWORD_ADDR(n) (((n)&0x1FF) << 22)
#define CMDWORD_DATA(n) (((n)&0x3FFFF) << 4)
#define CMDWORD_READ (1 << 31)
#define CMDWORD_WRITE_FMT(addr, data) (CMDWORD_ADDR(addr) | CMDWORD_DATA(data))
#define CMDWORD_READ_FMT(addr) (CMDWORD_ADDR(addr) | CMDWORD_READ)

void bl_watchdog_start(unsigned second)
{
    hwp_timer->WDTimer_LoadVal = TIMER_LOADVAL(second * 32 * 1024);
    hwp_timer->WDTimer_Ctrl = (hwp_timer->WDTimer_Ctrl & TIMER_WDENABLED) ? TIMER_RELOAD : TIMER_START;
}

void bl_watchdog_stop(void)
{
    hwp_timer->WDTimer_Ctrl = TIMER_STOP;
}

void bl_watchdog_reload(void)
{
    if (hwp_timer->WDTimer_Ctrl & TIMER_WDENABLED)
        hwp_timer->WDTimer_Ctrl = TIMER_RELOAD;
}

uint32_t bl_hw_tick(void)
{
    return hwp_timer->HWTimer_CurVal;
}

void bl_clock_init(void)
{
    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;

    hwp_sysCtrl->CFG_XTAL_DIV = SYS_CTRL_CFG_XTAL_DIV(1) | SYS_CTRL_CFG_XTAL_DIV_UPDATE;

    // Switch to slow clock
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF |
                             SYS_CTRL_SYS_SEL_FAST_SLOW |
                             SYS_CTRL_BB_SEL_FAST_SLOW |
                             SYS_CTRL_SPIFLASH_SEL_FAST_SLOW |
                             SYS_CTRL_VOC_SEL_FAST_SLOW |
                             SYS_CTRL_MEM_BRIDGE_SEL_FAST_SLOW |
                             SYS_CTRL_TCU_13M_SEL_13M;

    hwp_sysCtrl->Cfg_Clk_Sys = SYS_CTRL_SYS_FREQ_104M;
    hwp_sysCtrl->Cfg_Clk_Mem_Bridge = SYS_CTRL_MEM_FREQ_156M;

    // Turning off the Pll and reset of the Lock detector
    hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_POWER_DOWN |
                            SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE |
                            SYS_CTRL_PLL_LOCK_RESET_RESET;

    // Turning on the Pll
    hwp_sysCtrl->Pll_Ctrl = SYS_CTRL_PLL_ENABLE_ENABLE |
                            SYS_CTRL_PLL_CLK_FAST_ENABLE_DISABLE |
                            SYS_CTRL_PLL_LOCK_RESET_NO_RESET;

    // Wait for lock.
    while ((hwp_sysCtrl->Sel_Clock & SYS_CTRL_PLL_LOCKED) == 0)
        ;

    // Insert delay for PLL ready
    bl_delay_tick(1);

    // Enable PLL clock for fast clock
    hwp_sysCtrl->Pll_Ctrl |= SYS_CTRL_PLL_CLK_FAST_ENABLE_ENABLE;

    // Switch to fast clocks
    hwp_sysCtrl->Sel_Clock = SYS_CTRL_SLOW_SEL_RF_RF |
                             SYS_CTRL_SYS_SEL_FAST_FAST |
                             SYS_CTRL_BB_SEL_FAST_FAST |
                             SYS_CTRL_SPIFLASH_SEL_FAST_FAST |
                             SYS_CTRL_VOC_SEL_FAST_FAST |
                             SYS_CTRL_MEM_BRIDGE_SEL_FAST_FAST |
                             SYS_CTRL_TCU_13M_SEL_13M;

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_LOCK;
}

void SECTION_SRAMTEXT bl_delay_us(unsigned us)
{
// 1 (nop) + 1 (addiu) + 3 (btnez) = 5 cycles
#define WAIT_US_LOOP_CYCLE 5

    unsigned counter = ((BL_SYS_FREQ / 1000000) * us) / WAIT_US_LOOP_CYCLE;

    while (counter--)
        asm("nop");
}

void bl_delay_tick(unsigned tick)
{
    unsigned init = hwp_timer->HWTimer_CurVal;
    while ((hwp_timer->HWTimer_CurVal - init) <= tick)
        ;
}

void bl_ispi_init(void)
{
    unsigned divider = (BL_SYS_FREQ + (2 * ISPI_FREQ - 1)) / (2 * ISPI_FREQ);
    unsigned cfg = hwp_ispi->cfg;

    divider = (divider == 0) ? 0 : (divider - 1);
    cfg &= ~(SPI_CLOCK_DIVIDER_MASK | SPI_CLOCK_LIMITER);
    hwp_ispi->cfg = cfg | SPI_CLOCK_DIVIDER(divider);
}

uint16_t bl_ispi_read(uint8_t cs, uint16_t address)
{
    unsigned cmd = SPI_READ_ENA | SPI_CS(cs) | (1 << 25) | ((address & 0x1ff) << 16);

    // wait at least one tx space
    while (GET_BITFIELD(hwp_ispi->status, SPI_TX_SPACE) == 0)
        ;

    hwp_ispi->rxtx_buffer = cmd;

    // wait tx finish and rx ready
    for (;;)
    {
        unsigned status = hwp_ispi->status;
        if ((status & SPI_ACTIVE_STATUS) == 0 &&
            GET_BITFIELD(status, SPI_TX_SPACE) == SPI_TX_FIFO_SIZE &&
            GET_BITFIELD(status, SPI_RX_LEVEL) > 0)
            break;
    }

    return hwp_ispi->rxtx_buffer & 0xffff;
}

void bl_ispi_write(uint8_t cs, uint16_t address, uint16_t data)
{
    unsigned cmd = SPI_CS(cs) | (0 << 25) | ((address & 0x1ff) << 16) | data;

    // wait at least one tx space
    while (GET_BITFIELD(hwp_ispi->status, SPI_TX_SPACE) == 0)
        ;

    hwp_ispi->rxtx_buffer = cmd;

    // wait tx finish
    for (;;)
    {
        unsigned status = hwp_ispi->status;
        if ((status & SPI_ACTIVE_STATUS) == 0 &&
            GET_BITFIELD(status, SPI_TX_SPACE) == SPI_TX_FIFO_SIZE)
            break;
    }
}

void bl_xcv_init(void)
{
    hwp_rfSpi->Ctrl = RF_SPI_ENABLE | !RF_SPI_CS_POLARITY // csActiveHi
                      | RF_SPI_DIGRF_READ                 // modeDigRfRead
                      | !RF_SPI_CLOCKED_BACK2BACK         // modeClockBack2Back
                      | !RF_SPI_INPUT_MODE                // inputEn
                      | !RF_SPI_CLOCK_POLARITY            // clkFallEdge
                      | RF_SPI_CLOCK_DELAY(1)             // clkDelay
                      | RF_SPI_DO_DELAY(2)                // doDelay
                      | RF_SPI_DI_DELAY(3)                // diDelay
                      | RF_SPI_CS_DELAY(2)                // csDelay
                      | RF_SPI_CS_END_HOLD(0)             // csEndHold
                      | RF_SPI_CS_END_PULSE(3)            // csEndPulse
                      | RF_SPI_FRAME_SIZE(27)             // frameSize
                      | RF_SPI_INPUT_FRAME_SIZE(17)       // inputFrameSize
                      | RF_SPI_TURNAROUND_TIME(2)         // turnAroundCycles
        ;

    hwp_rfSpi->Command = 0;

    unsigned divider = ((BL_SYS_FREQ / 2) / RFSPI_FREQ) - 1;
    hwp_rfSpi->Divider = RF_SPI_DIVIDER(divider);
}

void bl_xcv_close(void)
{
    // Drive all the ouputs of the SPI interface to a logical '0'
    hwp_rfSpi->Command = RF_SPI_DRIVE_ZERO;

    // Disable the RF-SPI and clear the configuration
    // TODO Wouldn't just clear the enable bit be safer ?
    hwp_rfSpi->Ctrl = 0;
}

uint32_t bl_xcv_read(uint16_t address)
{
    unsigned cmdWord = CMDWORD_READ_FMT(address);
    unsigned savedCfg = hwp_rfSpi->Ctrl; // save the original configuration

    // There are hard-coded size: (frameSize=27, inputFrameSize=17)
    // * CmdSize = 10bits -> 2byte
    // * RdSize = 18bits -> 3bytes

    // Change the SPI mode to input and change the frame size
    unsigned readCfg = savedCfg & ~(RF_SPI_FRAME_SIZE(0x1F));
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
    while ((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0)
        ;

    // Wait until bytes ready, otherwise Rx FIFO will be underflow
    while (GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_RX_LEVEL) < 3)
        ;

    UINT8 data2 = (UINT8)hwp_rfSpi->Rx_Data;
    UINT8 data1 = (UINT8)hwp_rfSpi->Rx_Data;
    UINT8 data0 = (UINT8)(hwp_rfSpi->Rx_Data << 6);

    // Change the SPI mode back to output
    hwp_rfSpi->Ctrl = savedCfg;
    return ((data2 << 16) | (data1 << 8) | data0) >> 6;
}

void bl_xcv_write(uint16_t address, uint32_t data)
{
    unsigned cmdWord = CMDWORD_WRITE_FMT(address, data);

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
    while (GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_CMD_DATA_LEVEL) >= 4)
        ;
    // Wait for the SPI to finish
    while ((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0)
        ;
}

void bl_xcv_configure(void)
{
    bl_xcv_init();

    bl_xcv_write(0x30, 0x00a00); // Soft Resetn down
    bl_delay_us(10000);          // Delay 10ms
    bl_xcv_write(0x30, 0x00515); // Soft Resetn up
    bl_delay_us(10000);          // Delay 10ms

    uint32_t val005 = bl_xcv_read(0x005);
    val005 &= ~0x30000;
    bl_xcv_write(0x005, val005);

    bl_xcv_write(0x0c0, 0x20004); // reset val: 20100
    bl_xcv_close();
}

void bl_psram_init(void)
{
    hwp_psram8Ctrl->power_up = PSRAM8_CTRL_HW_POWER_PULSE;
    while (!((hwp_psram8Ctrl->power_up & PSRAM8_CTRL_INIT_DONE) == PSRAM8_CTRL_INIT_DONE))
        ;

    hwp_psram8Ctrl->timeout_val = PSRAM8_CTRL_TIMEOUT_VALUE(0x7ffff);
    hwp_psram8Ctrl->clk_ctrl = PSRAM8_CLK_CTRL;
    hwp_psram8Ctrl->dqs_ctrl = PSRAM8_DQS_CTRL;

    bl_delay_us(2);
    hwp_psram8Ctrl->cre = 1;
    *((volatile UINT32 *)0xa2000000) = PSRAM8_MR0;
    *((volatile UINT32 *)0xa2000000) = PSRAM8_MR0;
    hwp_psram8Ctrl->cre = 0;

    bl_delay_us(2);
    hwp_psram8Ctrl->cre = 1;
    *((volatile UINT32 *)0xa2000004) = PSRAM8_MR4;
    *((volatile UINT32 *)0xa2000004) = PSRAM8_MR4;
    hwp_psram8Ctrl->cre = 0;

    bl_delay_us(2);
    hwp_psram8Ctrl->cre = 1;
    uint32_t POSSIBLY_UNUSED rb_mr0 = *((volatile UINT32 *)0xa2000000);
    uint32_t POSSIBLY_UNUSED rb_mr4 = *((volatile UINT32 *)0xa2000004);
    hwp_psram8Ctrl->cre = 0;
}

void bl_platform_init(void)
{
    hwp_sysCtrl->Cfg_Rsd |= (1 << 3); // enable debughost access invalid address

    // Set SDMMC pads to GPIO
    hwp_iomux->pad_SDMMC_CLK_cfg = IOMUX_PAD_SDMMC_CLK_SEL_FUN_GPIO_8_SEL;
    hwp_iomux->pad_SDMMC_CMD_cfg = IOMUX_PAD_SDMMC_CMD_SEL_FUN_GPIO_9_SEL;
    hwp_iomux->pad_SDMMC_DATA_0_cfg = IOMUX_PAD_SDMMC_DATA_0_SEL_FUN_GPIO_10_SEL;
    hwp_iomux->pad_SDMMC_DATA_1_cfg = IOMUX_PAD_SDMMC_DATA_1_SEL_FUN_GPIO_11_SEL;
    hwp_iomux->pad_SDMMC_DATA_2_cfg = IOMUX_PAD_SDMMC_DATA_2_SEL_FUN_GPIO_12_SEL;
    hwp_iomux->pad_SDMMC_DATA_3_cfg = IOMUX_PAD_SDMMC_DATA_3_SEL_FUN_GPIO_13_SEL;

    bl_clock_init();
    bl_pmu_init();
    bl_psram_init();
}

void bl_platform_reset(void)
{
    bl_send_event(0x1a110011);

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SOFT_RST;

    while (1) // We die here ...
        ;
}

void bl_platform_shutdown(void)
{
    bl_send_event(0x1a11dead);

    bl_pmd_close();

    hwp_sysCtrl->REG_DBG = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->WakeUp = 0;

    uint32_t POSSIBLY_UNUSED wakeup = hwp_sysCtrl->WakeUp;
}
