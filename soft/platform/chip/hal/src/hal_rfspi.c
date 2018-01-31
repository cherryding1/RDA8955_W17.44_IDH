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
#include "rf_spi.h"
#include "sys_ifc.h"
#include "sys_ctrl.h"

//#include "halp_sys.h" // Could let access to the global var holding the system clock
#include "halp_debug.h"

#include "halp_sys.h"
#include "halp_tcu.h"
#include "halp_lps.h"
#include "halp_rfspi.h"

#include "hal_sys.h"
#include "hal_rfspi.h"
#include "hal_tcu.h"
#include "hal_mem_map.h"
#include "hal_gouda.h"
#include "hal_comregs.h"
#include "hal_clk.h"

#ifdef PAL_WINDOWS_LOCK_MCP_ON_RX
#include "tcu.h"
#endif // PAL_WINDOWS_LOCK_MCP_ON_RX

#ifdef DCDC_FREQ_DIV_WORKAROUND
#include "hal_rda_abb.h"

#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
DCDC_FREQ_DIV_VALUE g_halRxDcdcFreqDivValue[3];
#endif

#ifndef PAL_WINDOWS_LOCK_MCP_ON_RX
UINT32 g_halNBurstEndDate[3];
#endif // !PAL_WINDOWS_LOCK_MCP_ON_RX
#endif // DCDC_FREQ_DIV_WORKAROUND

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
#ifdef DDR_FREQ_260M_WORKAROUND
BOOL g_halApDDR260MDitherFlag[3];
extern VOID hal_ApCommTuneDDR260M(BOOL tune);
#endif
#ifdef GOUDA_TIMING_WORKAROUND
BOOL g_halApGoudaTimingDitherFlag[3];
extern VOID hal_ApCommTuneGoudaTiming(BOOL tune);
#endif
#endif

// =============================================================================
//  MACROS
// =============================================================================

// Macro to get remaining space in cmd fifo
#define CMD_FIFO_SPACE  (CMD_FIFO_LEN - \
                        ((hwp_rfSpi->Status & RF_SPI_CMD_LEVEL_MASK) \
                         >>RF_SPI_CMD_LEVEL_SHIFT))
#define DATA_FIFO_SPACE (CMD_DATA_FIFO_LEN - \
                        ((hwp_rfSpi->Status & RF_SPI_CMD_DATA_LEVEL_MASK) \
                         >>RF_SPI_CMD_DATA_LEVEL_SHIFT))
#define IFC_BUFFER_SPACE (RFSPI_SW_FIFO_SIZE - hwp_sysIfc->ch_rfspi_tc)


// =============================================================================
//  GLOBAL Variables
// =============================================================================
// =============================================================================
//  g_spiCmdQueue
// -----------------------------------------------------------------------------
/// RfSpi Command queue
// =============================================================================
PRIVATE HAL_RFSPI_CMD_QUEUE_T HAL_DATA_INTERNAL g_spiCmdQueue;

// =============================================================================
//  g_spiDataFifo
// -----------------------------------------------------------------------------
/// SW data Fifo used by IFC
// =============================================================================
PRIVATE HAL_RFSPI_SW_FIFO_T HAL_RFSPI_SW_FIFO_LOC g_spiDataFifo;

// =============================================================================
//  g_rfspiReqFreq
// -----------------------------------------------------------------------------
/// RFSPI requested Clock frequency
// =============================================================================
PRIVATE HAL_RFSPI_CLK_T g_rfspiReqFreq = HAL_RFSPI_13M_CLK;

// =============================================================================
//  g_rfspiOpened
// -----------------------------------------------------------------------------
/// RFSPI opened status.
// =============================================================================
PRIVATE BOOL g_rfspiOpened = FALSE;


// =============================================================================
//  Functions
// =============================================================================

// =============================================================================
// hal_RfspiUpdateDivider
// -----------------------------------------------------------------------------
/// Private function used to update RFSPI divider in case
/// of system frequency change.
/// Defined as PROTECTED to be registered in the HAL Sys update divider registry.
/// @param sys_freq : new system frequency.
// =============================================================================
PROTECTED UINT32 hal_RfspiUpdateDivider(HAL_SYS_FREQ_T sysFreq);

#ifdef RFSPI_IFC_WORKAROUND
PROTECTED VOID hal_RfspiStartIfc (VOID);
#endif


#ifdef PAL_WINDOWS_LOCK_MCP_ON_RX
// =============================================================================
// hal_RfspiLockMcp
// -----------------------------------------------------------------------------
/// Lock MCP access during rx slot
// =============================================================================
PRIVATE VOID HAL_FUNC_INTERNAL hal_RfspiLockMcp(VOID)
{
    if (hal_LpsFrameSkipping())
    {
        // This rx window has been abandoned. The frame might have been
        // shortened, and TCU might be stopped later.
        return;
    }

    UINT32 POSSIBLY_UNUSED curFrame = hal_ComregsGetSnap();

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
#ifdef GOUDA_TIMING_WORKAROUND
    if (g_halApGoudaTimingDitherFlag[curFrame])
    {
        hal_ApCommTuneGoudaTiming(TRUE);
    }
#endif
#ifdef DDR_FREQ_260M_WORKAROUND
    if (g_halApDDR260MDitherFlag[curFrame])
    {
        hal_ApCommTuneDDR260M(TRUE);
    }
#endif
#endif

#ifndef CHIP_HAS_AP
#ifndef GALLITE_IS_8805
#if ! defined(FPGA)
    UINT32 lcd_timing = hal_GoudaGetTiming();
    hal_GoudaSetTiming(0xffffffff);
#endif
#endif
#endif

#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
#ifdef DCDC_FREQ_DIV_WORKAROUND
    hal_AbbSetDcdcFreqDivValue(g_halRxDcdcFreqDivValue[curFrame]);
#endif
#endif

    UINT32 startTime = hwp_tcu->Cur_Val;

#define HAL_RFSPI_ONE_SLOT_DURATION 625
#define RX_SLOT_HANDLING_ADVANCE_TIME 30

    // PAL sets TCU0 IRQ (RX_SLOT_HANDLING_ADVANCE_TIME) Qb before the Rx slot
    while ( (hwp_tcu->Cur_Val >= startTime) &&
            ((hwp_tcu->Cur_Val-startTime) < (HAL_RFSPI_ONE_SLOT_DURATION+RX_SLOT_HANDLING_ADVANCE_TIME)) );

#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
#ifdef DCDC_FREQ_DIV_WORKAROUND
    // Restore the default DCDC freq
    hal_AbbSetDcdcFreqDivValue(DCDC_FREQ_DIV_VALUE_12);
#endif
#endif

#ifndef CHIP_HAS_AP
#ifndef GALLITE_IS_8805
#if ! defined(FPGA)
    hal_GoudaSetTiming(lcd_timing);
#endif
#endif
#endif

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
#ifdef DDR_FREQ_260M_WORKAROUND
    if (g_halApDDR260MDitherFlag[curFrame])
    {
        // Restore DDR freq (which was tuned in pal_FrameStart)
        hal_ApCommTuneDDR260M(FALSE);
    }
#endif
#ifdef GOUDA_TIMING_WORKAROUND
    if (g_halApGoudaTimingDitherFlag[curFrame])
    {
        // Restore Gouda timing (which was tuned in pal_FrameStart)
        hal_ApCommTuneGoudaTiming(FALSE);
    }
#endif
#endif
}

#else // !PAL_WINDOWS_LOCK_MCP_ON_RX

#ifdef DCDC_FREQ_DIV_WORKAROUND
// =============================================================================
// hal_RfspiChangeDcdcFreq
// -----------------------------------------------------------------------------
/// Change DCDC frequency during rx slot
// =============================================================================
PRIVATE VOID hal_RfspiChangeDcdcFreq(VOID)
{
    UINT32 curDate = hal_TcuGetCount();
    UINT32 curFrame = hal_ComregsGetSnap();

    if (curDate < g_halNBurstEndDate[curFrame])
    {
#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
        hal_AbbSetDcdcFreqDivValue(g_halRxDcdcFreqDivValue[curFrame]);
#endif
    }
    else
    {
        // Restore the default DCDC freq
#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
        hal_AbbSetDcdcFreqDivValue(DCDC_FREQ_DIV_VALUE_12);
#endif
    }
}
#endif // DCDC_FREQ_DIV_WORKAROUND

#endif // !PAL_WINDOWS_LOCK_MCP_ON_RX


// =============================================================================
// hal_RfspiOpen
// -----------------------------------------------------------------------------
/// RF SPI Opening
/// @param cfg : pointer to the RFSPI configuration
/// @param reqFreq : required spi clock frequency
// =============================================================================
PUBLIC VOID hal_RfspiOpen(CONST HAL_RFSPI_CFG_T* cfg, HAL_RFSPI_CLK_T reqFreq)
{
    HAL_ASSERT(((1<<sizeof(g_spiCmdQueue.cmdIdx)*8) - 1) >= RFSPI_MAX_CMD_GRP,
               "Invalid RFSPI_MAX_CMD_GRP: %u", RFSPI_MAX_CMD_GRP);
    HAL_ASSERT(((1<<sizeof(g_spiCmdQueue.dataIdx)*8) - 1) >= RFSPI_DATA_BUF_SIZE,
               "Invalid RFSPI_DATA_BUF_SIZE: %u", RFSPI_DATA_BUF_SIZE);

    // Because we are in automatic clock gating, we don't need
    // to manually enable the clock.
    // hwp_sysCtrl->Clk_Sys_Enable=SYS_CTRL_ENABLE_SYSD_RF_SPI;
    g_rfspiReqFreq = reqFreq;
    hal_RfspiConfigure(cfg);

    // Set the driver as opened, to ensure the divider is calculated
    // every time the system frequency is changed.
    g_rfspiOpened = TRUE;

#ifdef PAL_WINDOWS_LOCK_MCP_ON_RX
    hal_TcuIrqSetMask(HAL_TCU0_IRQ, TRUE);
    hal_TcuIrqSetHandler(HAL_TCU0_IRQ, hal_RfspiLockMcp);
#else // !PAL_WINDOWS_LOCK_MCP_ON_RX
#ifdef DCDC_FREQ_DIV_WORKAROUND
    hal_TcuIrqSetMask(HAL_TCU0_IRQ, TRUE);
    hal_TcuIrqSetHandler(HAL_TCU0_IRQ, hal_RfspiChangeDcdcFreq);
#endif // DCDC_FREQ_DIV_WORKAROUND
#endif // !PAL_WINDOWS_LOCK_MCP_ON_RX

#ifdef RFSPI_IFC_WORKAROUND
    // enable the TCU1 IT and set the user handler.
    hal_TcuIrqSetMask(HAL_TCU1_IRQ, TRUE);
    hal_TcuIrqSetHandler(HAL_TCU1_IRQ, hal_RfspiStartIfc);
#endif

    UINT32 scStatus = hal_SysEnterCriticalSection();
    // Set the proper frequency
    // FIXME: Use a global variable from the private system interface
    hal_RfspiUpdateDivider(hal_SysGetFreq());
    hal_SysExitCriticalSection(scStatus);
}



// =============================================================================
// hal_RfspiConfigure
// -----------------------------------------------------------------------------
/// Configure the RFSPI
/// @param cfg Pointer to the structure containing the configuration
/// to apply
// =============================================================================
PUBLIC VOID hal_RfspiConfigure(CONST HAL_RFSPI_CFG_T *cfg)
{
    //  Checkers
    HAL_ASSERT((UINT32)cfg->clkDelay < HAL_RFSPI_HALF_CLK_PERIOD_3,       "clkDelay=%d",          cfg->clkDelay        );
    HAL_ASSERT((UINT32)cfg->doDelay < HAL_RFSPI_HALF_CLK_PERIOD_3,        "doDelay=%d",           cfg->doDelay         );
    HAL_ASSERT((UINT32)cfg->diDelay < HAL_RFSPI_HALF_CLK_PERIOD_QTY,      "diDelay=%d",           cfg->diDelay         );
    HAL_ASSERT((UINT32)cfg->csDelay < HAL_RFSPI_HALF_CLK_PERIOD_QTY,      "csDelay=%d",           cfg->csDelay         );
    HAL_ASSERT((UINT32)cfg->csEndHold < HAL_RFSPI_HALF_CLK_PERIOD_QTY,    "csEndHold=%d",         cfg->csEndHold       );
    HAL_ASSERT((UINT32)cfg->csEndPulse < HAL_RFSPI_HALF_CLK_PERIOD_QTY,   "csEndPulse=%d",        cfg->csEndPulse      );
    HAL_ASSERT((UINT32)cfg->turnAroundCycles < 4,                         "turnAroundCycles=%d",  cfg->turnAroundCycles);
    HAL_ASSERT((cfg->frameSize>=HAL_RFSPI_MIN_FRAME_SIZE)
               && (cfg->frameSize<=HAL_RFSPI_MAX_FRAME_SIZE),                    "frameSize=%d",         cfg->frameSize       );
    HAL_ASSERT((!cfg->inputEn)
               || ((cfg->inputFrameSize>=HAL_RFSPI_MIN_FRAME_SIZE)
                   && (cfg->inputFrameSize<=HAL_RFSPI_MAX_FRAME_SIZE)),         "inputFrameSize=%d",    cfg->inputFrameSize  );

    //  Setter
    hwp_rfSpi->Ctrl = ((cfg->enable?RF_SPI_ENABLE:0)
                       | (cfg->csActiveHi?0:RF_SPI_CS_POLARITY)
                       | (cfg->modeDigRFRead?RF_SPI_DIGRF_READ:0)
                       | (cfg->modeClockedBack2back?RF_SPI_CLOCKED_BACK2BACK:0)
                       | (cfg->inputEn?RF_SPI_INPUT_MODE:0)
                       | (cfg->clkFallEdge?RF_SPI_CLOCK_POLARITY:0)
                       | RF_SPI_CLOCK_DELAY(cfg->clkDelay)
                       | RF_SPI_DO_DELAY(cfg->doDelay)
                       | RF_SPI_DI_DELAY(cfg->diDelay)
                       | RF_SPI_CS_DELAY(cfg->csDelay)
                       | RF_SPI_CS_END_HOLD(cfg->csEndHold)
                       | RF_SPI_CS_END_PULSE(cfg->csEndPulse)
                       | RF_SPI_FRAME_SIZE(cfg->frameSize)
                       | RF_SPI_INPUT_FRAME_SIZE(cfg->inputFrameSize)
                       | RF_SPI_TURNAROUND_TIME(cfg->turnAroundCycles));

    // To remove the "FORCE ZERO" in case of low-active CS
    hwp_rfSpi->Command = 0;
}



// =============================================================================
// hal_RfspiUpdateDivider
// -----------------------------------------------------------------------------
/// Private function used to update RFSPI divider in case
/// of system frequency change.
/// @param sys_freq : new system frequency.
/// @todo Might take no parameter and access the global var holding sys freq.
/// @todo Test the openness of the driver before doing anyhting ...
// =============================================================================
UINT32 hal_RfspiUpdateDivider(HAL_SYS_FREQ_T sysFreq)
{
    if (!g_rfspiOpened)
        return 0;

    UINT32 spiClk = 13000000;
    UINT8  spiDivider;
    // the spi clock is given by :
    // spiClk = (sysClk / 2) / (1 + spiDivider)
    switch(g_rfspiReqFreq)
    {
        case HAL_RFSPI_26M_CLK:    spiClk = 26000000; break;
        case HAL_RFSPI_19_5M_CLK:  spiClk = 19500000; break;
        case HAL_RFSPI_13M_CLK:    spiClk = 13000000; break;
        case HAL_RFSPI_6_5M_CLK:   spiClk =  6500000; break;
        case HAL_RFSPI_3_25M_CLK:  spiClk =  3250000; break;
    }

#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
    UINT32 sysClk = 52000000;
    switch(sysFreq)
    {
        // We use the clock limiter feature from the
        // RF_SPI module
        case HAL_SYS_FREQ_156M: sysClk = 52000000; break;
        case HAL_SYS_FREQ_104M: sysClk = 52000000; break;
        case HAL_SYS_FREQ_78M:  sysClk = 39000000; break;
        case HAL_SYS_FREQ_52M:  sysClk = 52000000; break;
        case HAL_SYS_FREQ_39M:  sysClk = 39000000; break;
        case HAL_SYS_FREQ_26M:  sysClk = 26000000; break;
        case HAL_SYS_FREQ_13M:  sysClk = 13000000; break;
        case HAL_SYS_FREQ_32K:  sysClk =    32768; break;
        default:
                                HAL_ASSERT(FALSE, "Invalid system frequency !!:%d\n", sysFreq);
    }
    // Round up the divider.
    spiDivider = (sysClk/2 + spiClk - 1) / spiClk;
#else
#if defined( CHIP_DIE_8955) &&  defined(FPGA)
    UINT32 sysClk = 52000000;
    switch(sysFreq)
    {
        // We use the clock limiter feature from the
        // RF_SPI module
        case HAL_SYS_FREQ_156M: sysClk = 52000000; break;
        case HAL_SYS_FREQ_104M: sysClk = 52000000; break;
        case HAL_SYS_FREQ_78M:  sysClk = 39000000; break;
        case HAL_SYS_FREQ_52M:  sysClk = 26000000; break;
        case HAL_SYS_FREQ_39M:  sysClk = 26000000; break;
        case HAL_SYS_FREQ_26M:  sysClk = 13000000; break;
        case HAL_SYS_FREQ_13M:  sysClk = 13000000; break;
        case HAL_SYS_FREQ_32K:  sysClk =    32768; break;
        default:
                                HAL_ASSERT(FALSE, "Invalid system frequency !!:%d\n", sysFreq);
    }
    sysFreq = sysClk;
#endif
    // choose spi clock not less than 7/8 of the requested
    spiDivider = (sysFreq/2 + spiClk/8) / spiClk;
#endif
    HAL_ASSERT(spiDivider, "RfSPI:Can not achieve %dMHz (sys@%dMHz)\n",
            spiClk/1000000, sysFreq/1000000);

    // Register divides by value plus one.
    spiDivider = spiDivider-1;
#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
    hwp_rfSpi->Divider = RF_SPI_DIVIDER(spiDivider) | RF_SPI_CLOCK_LIMITER;
#else
    hwp_rfSpi->Divider = RF_SPI_DIVIDER(spiDivider);
#endif

    return spiClk;
}



// =============================================================================
// hal_RfspiClose
// -----------------------------------------------------------------------------
/// set all outputs to '0'
/// @todo Rewrite that function
///
// =============================================================================
PUBLIC VOID hal_RfspiClose(VOID)
{
    // Drive all the ouputs of the SPI interface to a logical '0'
    hwp_rfSpi->Command = RF_SPI_DRIVE_ZERO;

    // Disable the RF-SPI and clear the configuration
    // TODO Wouldn't just clear the enable bit be safer ?
    hwp_rfSpi->Ctrl = 0;

    g_rfspiOpened = FALSE;
}



// =============================================================================
// hal_RfspiGetStatus
// -----------------------------------------------------------------------------
///  Get SPI status
/// @param status Pointer to the status structure where the
/// status will be written.
/// @param clear If \c TRUE, the status register is cleared.
///              If \c FALSE, it is kept as is.
// =============================================================================
PUBLIC VOID hal_RfspiGetStatus(HAL_RFSPI_STATUS_T* status, BOOL clear)
{
    UINT32 hwStatus = hwp_rfSpi->Status;
    status->active = (hwStatus & RF_SPI_ACTIVE_STATUS)?1:0;
    status->errCmd =  (hwStatus & RF_SPI_ERROR_CMD)?1:0;
    status->tableOvf = (hwStatus & RF_SPI_TABLE_OVF)?1:0;
    status->tableUdf = (hwStatus & RF_SPI_TABLE_UDF)?1:0;
    status->cmdOvf = (hwStatus & RF_SPI_CMD_OVF)?1:0;
    status->cmdUdf = (hwStatus & RF_SPI_CMD_UDF)?1:0;
    status->dataOvf = (hwStatus & RF_SPI_CMD_DATA_OVF)?1:0;
    status->dataUdf = (hwStatus & RF_SPI_CMD_DATA_UDF)?1:0;
    status->rxOvf = (hwStatus & RF_SPI_RX_OVF)?1:0;
    status->rxUdf = (hwStatus & RF_SPI_RX_UDF)?1:0;
    status->cmdLevel = GET_BITFIELD(hwStatus, RF_SPI_CMD_LEVEL);
    status->dataLevel = GET_BITFIELD(hwStatus, RF_SPI_CMD_DATA_LEVEL);
    status->rxLevel = GET_BITFIELD(hwStatus, RF_SPI_RX_LEVEL);

    if (clear)
    {
        //  Reset
        hwp_rfSpi->Status = RF_SPI_ERROR_CMD | RF_SPI_TABLE_OVF
                            | RF_SPI_TABLE_UDF | RF_SPI_CMD_OVF | RF_SPI_CMD_UDF
                            | RF_SPI_CMD_DATA_OVF | RF_SPI_CMD_DATA_UDF | RF_SPI_RX_OVF
                            | RF_SPI_RX_UDF;
    }
}




// =============================================================================
// hal_RfspiForceRead
// -----------------------------------------------------------------------------
/// Performs an SPI Read access from a Xcver.
/// The read is usually preceeded by a write access
/// that must setup the Xcver in read mode.
/// @param rdSize Number of bytes to read
/// @param dataOut Buffer where the read data are stored.
// =============================================================================
PUBLIC VOID hal_RfspiForceRead(UINT8 rdSize, UINT8* dataOut)
{
    UINT32 loop;
    UINT32 rfspiCtrl = hwp_rfSpi->Ctrl;

    HAL_ASSERT(FALSE, "RF SPI Force Read when resource not active");

    // Change the SPI mode to input and change the frame size
    hwp_rfSpi->Ctrl =  (rfspiCtrl &
                        ~(RF_SPI_FRAME_SIZE(0x1F))) |RF_SPI_FRAME_SIZE(4*rdSize)
                       | RF_SPI_INPUT_MODE ;

    HAL_ASSERT(rdSize<CMD_DATA_FIFO_LEN, "RF SPI command size too large");

    // Fill the Tx Fifo with 0 TODO: check if this is needed...
    for (loop=0; loop<rdSize; loop++)
    {
        hwp_rfSpi->Cmd_Data = RF_SPI_RX_DATA(0);
    }

    // Set the readout size
    hwp_rfSpi->Cmd_Size = RF_SPI_CMD_SIZE(rdSize);

    // And send the command
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the SPI to finish
    // We could deschedule oneself here, if needed.
    while((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);

    // Get the results
    for (loop=0; loop<rdSize; loop++)
    {
        // Wait until at least one byte is ready, otherwise Rx FIFO will be underflow
        while(GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_RX_LEVEL) == 0);
        *(dataOut++) = (UINT8)hwp_rfSpi->Rx_Data;
    }

    // Change the SPI mode back to output
    hwp_rfSpi->Ctrl = rfspiCtrl;
}

// =============================================================================
// hal_RfspiDigRfRead
// -----------------------------------------------------------------------------
/// Performs an SPI Read access inDigRf Mode
/// @param Cmd Formatted read command (read bit, address and dummy data)
/// @param dataOut Buffer where the read data are stored.
// =============================================================================
PUBLIC VOID hal_RfspiDigRfRead(UINT8 *Cmd,UINT8* dataOut)
{
    UINT32 loop;
    UINT32 savedCfg = hwp_rfSpi->Ctrl; // save the original configuration
    UINT32 readCfg = savedCfg & ~(RF_SPI_FRAME_SIZE(0x1F));
    UINT8  FrameSize = ((RF_SPI_FRAME_SIZE_MASK & savedCfg) >>
                        RF_SPI_FRAME_SIZE_SHIFT)+1;
    UINT8  InputSize = ((RF_SPI_INPUT_FRAME_SIZE_MASK & savedCfg) >>
                        RF_SPI_INPUT_FRAME_SIZE_SHIFT)+1;
    UINT8  WriteSize = FrameSize-InputSize;
    UINT8  remBits = (InputSize%8);
    UINT8  CmdSize = !(WriteSize%8)?(WriteSize/8):(WriteSize/8 + 1);
    UINT8  RdSize = !(remBits)?(InputSize/8):(InputSize/8 + 1);
    UINT8  tmpOut[4] = {0,0,0,0};
    UINT32* tmpOut_p32 = (UINT32*)tmpOut;

    // Change the SPI mode to input and change the frame size
    readCfg |= RF_SPI_FRAME_SIZE(FrameSize-InputSize-1);
    readCfg |= RF_SPI_INPUT_MODE;
    hwp_rfSpi->Ctrl = readCfg;

    //HAL_ASSERT(CmdSize<CMD_DATA_FIFO_LEN, "RF SPI command size too large");

    // Fill the Tx fifo
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO | RF_SPI_FLUSH_RX_FIFO;
    for (loop=0; loop<CmdSize; loop++)
    {
        hwp_rfSpi->Cmd_Data = Cmd[loop];
    }
    // Set the readout size
    hwp_rfSpi->Cmd_Size = RF_SPI_CMD_SIZE(CmdSize);

    // And send the command
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the SPI to finish
    while((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);

    // Get the results
    for (loop=RdSize; loop>0; loop--)
    {
        // Wait until at least one byte is ready, otherwise Rx FIFO will be underflow
        while(GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_RX_LEVEL) == 0);
        tmpOut[loop-1] = (UINT8)hwp_rfSpi->Rx_Data;
    }

    // Handle non-multiple of byte data readouts
    if (remBits)
    {
        // last byte is screwed because of missing shifts
        tmpOut[0] = tmpOut[0] << (8-remBits);
        // repack
        tmpOut_p32[0] = tmpOut_p32[0] >> (8-remBits);
    }

    // Copy to byte output
    for (loop=0; loop<RdSize; loop++)
    {
        dataOut[loop] = tmpOut[loop];
    }

    // Change the SPI mode back to output
    hwp_rfSpi->Ctrl = savedCfg;
}


// =============================================================================
// hal_RfspiTriggerCmd
// -----------------------------------------------------------------------------
///  Trigger the previously sent command (like the TCU)
// =============================================================================
PUBLIC VOID hal_RfspiTriggerCmd(VOID)
{
    HAL_ASSERT(FALSE, "RF SPI Trigger Cmd when resource not active");

    hwp_rfSpi->Command = RF_SPI_SEND_CMD;
}



// =============================================================================
// hal_RfspiRestartGain
// -----------------------------------------------------------------------------
/// Place the gain read pointer at the beginning of the gain table.
// =============================================================================
PUBLIC VOID hal_RfspiRestartGain(VOID)
{
    hwp_rfSpi->Command = RF_SPI_RESTART_GAIN;
}




// =============================================================================
// hal_RfspiReloadGain
// -----------------------------------------------------------------------------
/// Place the write pointer at the beginning of the gain table allowing to
/// fill the table.
// =============================================================================
PUBLIC VOID hal_RfspiReloadGain(VOID)
{
    hwp_rfSpi->Command = RF_SPI_RELOAD_GAIN;
}



// =============================================================================
// hal_RfspiNewCmd
// -----------------------------------------------------------------------------
/// Allocate and sort a new command in the command list
/// and setup the corresponding TCU event.
/// @param date Time when the command is to be issued.
/// @param winIdx Index of the window which the command belongs to.
/// @return A pointer to the newly created command
// =============================================================================
PUBLIC HAL_RFSPI_CMD_T * HAL_FUNC_INTERNAL hal_RfspiNewCmd(UINT16 date, UINT8 winIdx)
{
    HAL_ASSERT((g_spiCmdQueue.cmdIdx < RFSPI_MAX_CMD_GRP),
               "CmdQueue Command Overflow with index=%d",
               g_spiCmdQueue.cmdIdx);

    HAL_RFSPI_CMD_T* current = g_spiCmdQueue.firstCmd;
    HAL_RFSPI_CMD_T* tmp = NULL;
    HAL_RFSPI_CMD_T* new = (HAL_RFSPI_CMD_T*)&(g_spiCmdQueue.cmdBuf[g_spiCmdQueue.cmdIdx++]);

    // Init the structure
    new->data = (UINT8 *)&(g_spiCmdQueue.dataBuf[g_spiCmdQueue.dataIdx]);
    new->byteQty = 0;
    new->date = date;
    new->winIdx = winIdx;
    new->maskable = FALSE;

    // Sorting
    while (current && (date > current->date))
    {
        tmp = current;
        current = current->next;
    }

    new->next = current;
    if (tmp)
    {
        tmp->next = new;
    }
    else
    {
        // "Push" the scheduled commands to the SW fifo
        g_spiCmdQueue.firstCmd = new;
    }

    // Setup the TCU event to send command at next frame
    hal_TcuSetEvent(HAL_TCU_SEND_SPI_CMD, date);

    return (new);
}



// =============================================================================
// hal_RfspiPushData
// -----------------------------------------------------------------------------
/// Add data to a command
/// @param cmd The data are added to this command.
/// @param data Pointer to a buffer where the data to add are located.
/// @param byteQty Size of the command to add, in number of bytes.
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_RfspiPushData(HAL_RFSPI_CMD_T* cmd, CONST UINT8* data, UINT8 byteQty)
{
    HAL_ASSERT((g_spiCmdQueue.dataIdx+byteQty <= RFSPI_DATA_BUF_SIZE),
               "CmdQueue Data Overflow, with index=%d",
               g_spiCmdQueue.dataIdx);
    HAL_ASSERT((cmd->byteQty+byteQty <= 255),
               "Spi command too large, size=%d", cmd->byteQty+byteQty);

    UINT32 i;
    for (i=0; i<byteQty; i++)
    {
        cmd->data[cmd->byteQty+i] = data[i];
    }
    cmd->byteQty += byteQty;
    g_spiCmdQueue.dataIdx += byteQty;
}


// TODO: no "free" mechanism implemented for now
// =============================================================================
// hal_RfspiRemWinCmd
// -----------------------------------------------------------------------------
/// Remove all Spi commands belonging to one Window from the queue.
/// @param winIdx The commands for this window are removed from the queue.
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_RfspiRemWinCmd(UINT8 winIdx)
{
    HAL_RFSPI_CMD_T* current = g_spiCmdQueue.firstCmd;
    HAL_RFSPI_CMD_T* previous = NULL;

    while (current)
    {
        // Erase this one
        if (current->winIdx == winIdx)
        {
            // first element ?
            if (current == g_spiCmdQueue.firstCmd)
            {
                g_spiCmdQueue.firstCmd = current->next;
            }
            else
            {
                previous->next = current->next;
            }
        }
        else
        {
            previous = current;
        }
        current = current->next;
    }
}


// =============================================================================
// hal_RfspiInitQueue
// -----------------------------------------------------------------------------
/// Initializes the command queue.
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_RfspiInitQueue(VOID)
{
    // Reset command Queue
    g_spiCmdQueue.firstCmd = NULL;
    g_spiCmdQueue.cmdIdx = 0;
    g_spiCmdQueue.dataIdx = 0;
}


#ifdef RFSPI_IFC_WORKAROUND
#ifdef PAL_WINDOWS_LOCK_MCP_ON_RX
#define RFSPISTARTIFC_FUNC_LOC HAL_FUNC_INTERNAL
#else // !PAL_WINDOWS_LOCK_MCP_ON_RX
#define RFSPISTARTIFC_FUNC_LOC
#endif // !PAL_WINDOWS_LOCK_MCP_ON_RX

PROTECTED VOID RFSPISTARTIFC_FUNC_LOC hal_RfspiStartIfc(VOID)
{
    UINT32 status = hal_SysEnterCriticalSection();

    // If the IT happens after frameEnd, the expression computes forbidIfc for the next frame.
    // If the IT happens before frameEnd, the expression computes forbidIfc for the current frame.
    // In the latter case, g_spiDataFifo.addedData is null as the next frame commands are still
    // not added in the software rfspi fifo, so this will autorize to write to the TC at frameEnd
    if (g_spiDataFifo.forbidIfcStart == FALSE)
    {
        if (g_spiDataFifo.addedData)
        {
            // Raise only if there is data to write.
            g_spiDataFifo.forbidIfcStart = TRUE;

            // The TCU1 IT is generated after completion of the last rfspi command.
            // At this point the IFC is idle, so it is safe to update the TC.
            hwp_sysIfc->ch_rfspi_tc = g_spiDataFifo.addedData;
            g_spiDataFifo.addedData = 0;
        }
    }
    else
    {
        g_spiDataFifo.forbidIfcStart = FALSE;
    }

    hal_SysExitCriticalSection(status);
}
#endif // RFSPI_IFC_WORKAROUND

// =============================================================================
// hal_RfspiSetupFifo
// -----------------------------------------------------------------------------
/// Configure the RFSPI FIFO
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_RfspiSetupFifo(VOID)
{
    // Reset SW Fifo
    g_spiDataFifo.curPos = 0;
    g_spiDataFifo.addedData = 0;
#ifdef RFSPI_IFC_WORKAROUND
    // allow updating the TC at start
    g_spiDataFifo.forbidIfcStart = FALSE;
#endif

    // Clear all TCU events including SEND-RFSPI commands
    hal_TcuClrAllEvents();

    // Make sure TCU has processed the above request
    for (volatile int delay = 0; delay < 10; delay++);

    // Flush RFSPI FIFO first to avoid triggering RFSPI-SEND action while
    // clearing TCU events
    hal_RfspiFlushFifo();

    // TO BE REMOVED WHEN RFSPI DOES NOT SHOW ERRORS AFTER RESET FIFO
    HAL_ASSERT(
        (hwp_rfSpi->Status & (RF_SPI_CMD_DATA_OVF|RF_SPI_CMD_DATA_UDF|
                              RF_SPI_CMD_OVF|RF_SPI_CMD_UDF)) == 0,
        "<1> RFSPI status error: 0x%08x", hwp_rfSpi->Status);

    // Stop RFSPI IFC
    hwp_sysIfc->ch_rfspi_control = SYS_IFC_DISABLE;
    hwp_sysIfc->ch_rfspi_tc = 0;

    // Flush RFSPI FIFO again to remove the data transfered by IFC
    // before stopping IFC
    while (hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS);
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO;

    // Setup IFC Data SW Fifo
    hwp_sysIfc->ch_rfspi_start_addr = (UINT32) &(g_spiDataFifo.data[0]);
    hwp_sysIfc->ch_rfspi_end_addr = (UINT32) &(g_spiDataFifo.data[RFSPI_SW_FIFO_SIZE]);
    hwp_sysIfc->ch_rfspi_control = SYS_IFC_ENABLE;

    // TO BE REMOVED WHEN RFSPI DOES NOT SHOW ERRORS AFTER RESET FIFO
    HAL_ASSERT(
        (hwp_rfSpi->Status & (RF_SPI_CMD_DATA_OVF|RF_SPI_CMD_DATA_UDF|
                              RF_SPI_CMD_OVF|RF_SPI_CMD_UDF)) == 0,
        "<2> RFSPI status error: 0x%08x", hwp_rfSpi->Status);
}

// =============================================================================
// hal_RfspiFillFifo
// -----------------------------------------------------------------------------
/// This function is meant to be called once at FrameEnd (for next frame
/// pre-programming) and once at FINT (to finish frame programming)
/// @param nextFrame \c TRUE if this command is called for next frame programming.
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_RfspiFillFifo(BOOL nextFrame)
{
#ifdef HAL_RFSPI_PROFILING
    HAL_PROFILE_FUNCTION_ENTER(hal_RfspiFillFifo);
#endif

    HAL_ASSERT(
        (hwp_rfSpi->Status & (RF_SPI_CMD_DATA_OVF|RF_SPI_CMD_DATA_UDF|
                              RF_SPI_CMD_OVF|RF_SPI_CMD_UDF)) == 0,
        "RFSPI status error: 0x%08x", hwp_rfSpi->Status);

    UINT32 status;
    HAL_RFSPI_CMD_T* cmd;

    // Get IFC buffer available space
    INT32 spaceLeft = IFC_BUFFER_SPACE;

    HAL_ASSERT((spaceLeft >= 0) && (spaceLeft >= g_spiDataFifo.addedData),
               "RFSPI IFC error: spaceLeft=%d, addedData=%d",
               spaceLeft, g_spiDataFifo.addedData);

    // Subtract the number of data bytes that has been put in IFC buffer
    // but not update in tc count yet.
    spaceLeft -= g_spiDataFifo.addedData;

    // Check available space in HW Cmd fifo
    INT32 cmdLeft = CMD_FIFO_SPACE;

    while (g_spiCmdQueue.firstCmd)
    {
        cmd = g_spiCmdQueue.firstCmd;
        // Only fill data of a full command
        spaceLeft -= cmd->byteQty;
        cmdLeft--;
        if ((spaceLeft >= 0) && (cmdLeft >= 0))
        {
            UINT32 i;
            // Fill Data
            for (i=0; i < cmd->byteQty; i++)
            {
                g_spiDataFifo.data[g_spiDataFifo.curPos] = cmd->data[i];
                if (++g_spiDataFifo.curPos == RFSPI_SW_FIFO_SIZE)
                {
                    g_spiDataFifo.curPos = 0;
                }
            }
            // Fill Cmd
            if (cmd->maskable)
            {
                hwp_rfSpi->Cmd_Size = cmd->byteQty |
                                      RF_SPI_CMD_MARK;
            }
            else
            {
                hwp_rfSpi->Cmd_Size = cmd->byteQty;
            }

            status = hal_SysEnterCriticalSection();
            // store the total number of data to add in the IFC's TC
            g_spiDataFifo.addedData += cmd->byteQty;
            hal_SysExitCriticalSection(status);

            // next command in queue
            g_spiCmdQueue.firstCmd = g_spiCmdQueue.firstCmd->next;
        }
        else
        {
            // Remaining commands in queue at pal_FrameEnd -> retry at
            //   the start of pal_FintIrqHandler in next FINT
            // Remaining commands in queue at pal_FintIrqHandler -> error
            HAL_ASSERT(nextFrame,
                       "Remaining commands in queue at FINT: "
                       "spaceLeft=%d,cmdLeft=%d,byteQty=%d,"
                       "ifcSpace=%d,cmdSpace=%d,status=0x%08x",
                       spaceLeft, cmdLeft, cmd->byteQty,
                       IFC_BUFFER_SPACE, CMD_FIFO_SPACE,
                       hwp_rfSpi->Status);
            break;
        }
    }

#ifdef RFSPI_IFC_WORKAROUND
    // Update the TC only at frameEnd and only if the TCU1 IT for this
    // frame is already passed.
    if (nextFrame)
    {
        hal_RfspiStartIfc();
    }
#else
    hwp_sysIfc->ch_rfspi_tc = g_spiDataFifo.addedData;
    g_spiDataFifo.addedData = 0;
#endif

#ifdef HAL_RFSPI_PROFILING
    HAL_PROFILE_FUNCTION_EXIT(hal_RfspiFillFifo);
#endif
}

// =============================================================================
// hal_RfspiNewSingleCmd
// -----------------------------------------------------------------------------
/// This function should be used only in asyncronous mode together with
/// hal_RfspiSendCmd.
/// It allocates the full Software fifo space for a single command.
/// This command can be built normally (calls to hal_RfspiPushData) and must be
/// sent by a call to hal_RfspiSendCmd.
/// Note that calling this function destroys the command queue and context.
/// Therefore it must be called only when no syncronous commands are queued (i.e
/// at startup or wakeup). The SW Fifo must be re-initialized to be used again
/// in synchronous mode after calls to hal_RfspiNewSingleCmd.
/// @return A pointer to the created command
// =============================================================================
PUBLIC HAL_RFSPI_CMD_T * hal_RfspiNewSingleCmd(VOID)
{
    HAL_RFSPI_CMD_T* new = (HAL_RFSPI_CMD_T*)&(g_spiCmdQueue.cmdBuf[0]);

    // Init the structure, directlly use the SW fifo as only one cmd
    // will be programmed and sent
    new->data = (UINT8 *)&(g_spiDataFifo.data[0]);
    new->byteQty = 0;
    new->date = 0;
    new->winIdx = 0;

    return (new);
}


// =============================================================================
// hal_RfspiSendCmd
// -----------------------------------------------------------------------------
/// Force an SPI command to be send IMMEDIATELY (no TCU timing)
/// Used only in special Init sequences (low power w/ no TCU)
/// The SPI command must be allocated with hal_RfspiNewSingleCmd.
// =============================================================================
PUBLIC VOID hal_RfspiSendCmd(HAL_RFSPI_CMD_T* spiCmd)
{
    // Directly fill the SW Fifo, the size limit comes from the HW command fifo
    // that is only 8 bits wide -> 255 is max
    BOOL wait = TRUE;

    // Reset H
    // Disable the IFC before flushing. (Medow)
    hwp_sysIfc->ch_rfspi_control = SYS_IFC_DISABLE;
    hwp_sysIfc->ch_rfspi_tc = 0;
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO;

    // Setup IFC Xfer
    hwp_sysIfc->ch_rfspi_start_addr = (UINT32) &(g_spiDataFifo.data[0]);
    hwp_sysIfc->ch_rfspi_end_addr = (UINT32) &(g_spiDataFifo.data[RFSPI_SW_FIFO_SIZE]);
    hwp_sysIfc->ch_rfspi_control = SYS_IFC_ENABLE;
    hwp_sysIfc->ch_rfspi_tc = spiCmd->byteQty;

    // And send the command
    hwp_rfSpi->Cmd_Size = spiCmd->byteQty;
    // Command sending is deported after the writing of the reset value of spiCmd->byteQty
    // so that there is something else than APB access on the bus. This way, the
    // IFC will have time to feed some data to the RF-SPI and thing will work way
    // better.
    // hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the TC to have started going down, ie
    // the RFSPI has filled the Fifo.
    while (hwp_sysIfc->ch_rfspi_tc == spiCmd->byteQty);

    // Clear byteQty to "free" the command
    spiCmd->byteQty = 0;

    // End of send command.
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the command to be sent
    while(wait)
    {
        HAL_RFSPI_STATUS_T status;
        hal_RfspiGetStatus(&status, FALSE);
        wait=((status.dataLevel==0) && (status.active == 0))?FALSE:TRUE;
    }

    // Re-init the queue and the SW fifo mechanism
    hal_RfspiInitQueue();
    hal_RfspiSetupFifo();
}


// =============================================================================
// hal_RfspiSendGain
// -----------------------------------------------------------------------------
/// Send SPI gain
/// @param gain Gain to send
// =============================================================================
PUBLIC VOID hal_RfspiSendGain(CONST HAL_RFSPI_GAIN_T* gain)
{
    UINT32 i;

    //  Checkers
    HAL_ASSERT(gain->byteQty < HAL_RFSPI_GAIN_MAX_BYTE_QTY, "byteQt= %d", gain->byteQty);

    //  Setters
    for (i=0; i<gain->byteQty; i++)
    {
        hwp_rfSpi->Gain_Data = gain->data[i];
    }
    hwp_rfSpi->Gain_Size = gain->byteQty;
}



// =============================================================================
// hal_RfspiFlushFifo
// -----------------------------------------------------------------------------
/// Flush CMD Fifo
// =============================================================================
PUBLIC VOID HAL_FUNC_INTERNAL hal_RfspiFlushFifo(VOID)
{
    // Init the command queue
    hal_RfspiInitQueue();
    // wait until the end of this command to avoid skrewing up the SPI
    while (hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS);
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO;
}



// =============================================================================
// hal_RfspiIsActive
// -----------------------------------------------------------------------------
/// Check if a spi command is running
// =============================================================================
PUBLIC BOOL hal_RfspiIsActive()
{
    // Use it to pool until the end of an Spi command
    return ((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);
}


//  ***************
//   IRQ functions
//  ***************
/// Global variable to store the user irq handler
PRIVATE HAL_RFSPI_IRQ_HANDLER_T g_rfspiRegistry = NULL;



// =============================================================================
// hal_RfspiIrqSetHandler
// -----------------------------------------------------------------------------
/// Set the user handler for RF-SPI interrupt
/// @param handler User handlerfunction.
// =============================================================================
PUBLIC VOID hal_RfspiIrqSetHandler(HAL_RFSPI_IRQ_HANDLER_T handler)
{
    g_rfspiRegistry = handler;
}



// =============================================================================
// hal_RfspiIrqSetMask
// -----------------------------------------------------------------------------
/// Set the irq mask
/// @param mask RFSPI IRQ mask
// =============================================================================
PUBLIC VOID hal_RfspiIrqSetMask(HAL_RFSPI_IRQ_STATUS_T mask)
{
    union
    {
        UINT32 reg;
        HAL_RFSPI_IRQ_STATUS_T bitfield;
    } u; // union representing the status bitfield in a 32 bits value
    // loadable in the register

    u.bitfield = mask;
    // set ana_spi mask
    hwp_rfSpi->IRQ_Mask = u.reg;
}



// =============================================================================
// hal_RfspiIrqGetMask
// -----------------------------------------------------------------------------
/// Get the irq mask.
/// @return The IRQ mask for the RFSPI interrupt.
// =============================================================================
PUBLIC HAL_RFSPI_IRQ_STATUS_T hal_RfspiIrqGetMask(VOID)
{
    union
    {
        UINT32 reg;
        HAL_RFSPI_IRQ_STATUS_T bitfield;
    } u;

    u.reg = hwp_rfSpi->IRQ_Mask;

    return (u.bitfield);
}


// =============================================================================
// hal_RfspiIrqHandler
// -----------------------------------------------------------------------------
/// RF-SPI module IRQ handler.
/// This function is used by the private IRQ module to clear the IRQ and call
/// the user handler with the proper status.
// =============================================================================
PROTECTED VOID hal_RfspiIrqHandler(UINT8 interruptId)
{
    union
    {
        UINT32 reg;
        HAL_RFSPI_IRQ_STATUS_T bitfield;
    } cause;

    cause.reg = hwp_rfSpi->IRQ;

    // clear the IFC pulse   // N.b.: this is the only
    // clearable irq. The err is cleared by writing
    // in the status register ...
    if (cause.reg & RF_SPI_CMD_DATA_DMA_DONE_CAUSE)
        hwp_rfSpi->IRQ = RF_SPI_CMD_DATA_DMA_DONE_CAUSE;

    // Die if an error happened (dev feature to be removed <- FIXME)
    HAL_ASSERT(!(cause.bitfield.error),"rf-spi error happened.Cause=%d", cause.reg);

    // Call the handler
    if (g_rfspiRegistry)
    {
        g_rfspiRegistry(cause.bitfield);
    }
}


// =============================================================================
// hal_RfspiImmReadWriteAvail
// -----------------------------------------------------------------------------
/// Check if RFSPI is ready for immediate read or write operation.
/// @return TRUE if ready.
// =============================================================================
PUBLIC BOOL hal_RfspiImmReadWriteAvail()
{
    if (hwp_sysIfc->ch_rfspi_tc != 0 ||
            (hwp_sysIfc->ch_rfspi_status & SYS_IFC_FIFO_EMPTY) == 0)
    {
        return FALSE;
    }

    HAL_RFSPI_STATUS_T status;
    hal_RfspiGetStatus(&status, FALSE);
    if (status.active != 0 || status.cmdLevel != 0 ||
            status.dataLevel != 0 || status.rxLevel != 0)
    {
        return FALSE;
    }

    return TRUE;
}


// =============================================================================
// hal_RfspiImmWrite
// -----------------------------------------------------------------------------
/// Write commands immediately via RFSPI.
/// @param Cmd Command data array.
/// @param CmdSize Command size in bytes.
// =============================================================================
PUBLIC VOID hal_RfspiImmWrite(CONST UINT8 *Cmd, UINT32 CmdSize)
{
    // Assume RFSPI Input_Mode is OFF
    // HAL_ASSERT(CmdSize<CMD_DATA_FIFO_LEN, "RF SPI command size too large");

    // Flush the Tx fifo
    hwp_rfSpi->Command = RF_SPI_FLUSH_CMD_FIFO | RF_SPI_FLUSH_RX_FIFO;
    for (UINT32 loop=0; loop<CmdSize; loop++)
    {
        hwp_rfSpi->Cmd_Data = Cmd[loop];
    }
    // Set the cmd size
    hwp_rfSpi->Cmd_Size = RF_SPI_CMD_SIZE(CmdSize);

    // And send the command
    hwp_rfSpi->Command = RF_SPI_SEND_CMD;

    // Wait for the SPI to start - at least one byte has been sent
    while(GET_BITFIELD(hwp_rfSpi->Status, RF_SPI_CMD_DATA_LEVEL) >= CmdSize);
    // Wait for the SPI to finish
    while((hwp_rfSpi->Status & RF_SPI_ACTIVE_STATUS) != 0);
}

