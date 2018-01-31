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
#include "sci.h"

#include "halp_sys_ifc.h"
#include "halp_sys.h"
#include "halp_debug.h"

#include "hal_sim.h"
#include "hal_timers.h"
#include "hal_clk.h"

#include "pmd_m.h"

#include "sxr_sbx.h"
#include "sxr_tim.h"
#include "sxr_tls.h"
#include "halp_abb_regs.h"

#ifdef GALLITE_IS_8806
#include "hal_host.h"
#include "sys_ctrl.h"
// Dual sim includes
#include "drv_dualsim.h"

extern BOOL pmd_SelectSimCard(UINT8 sim_card);
extern PUBLIC DUALSIM_ERR_T foursim_SetSimVoltage_8806(DUALSIM_SIM_VOLTAGE_T sim0Level,DUALSIM_SIM_VOLTAGE_T sim1Level,
        DUALSIM_SIM_VOLTAGE_T sim2Level, DUALSIM_SIM_VOLTAGE_T sim3Level);
#endif


// ============================================================================
//  MACROS
// ============================================================================
#define N_TX_CMD_BYTES          5
#define MAX_RX_BYTES            1023


// SCI_MAINDIV_*_8* = F/D -1 (round down) ; SCI_DIV16 = (F/D  /16) -1 (round down)
// To support 512/32, need set:   sys_ctrl  cfg_reserve[4] = 0; but the new version need not this configure
#define SCI_MAINDIV_372_1       0x173
#define SCI_DIV16_372_1         0x16

#define SCI_MAINDIV_372_8       0x2d
#define SCI_DIV16_372_8         0x1

#define SCI_MAINDIV_372_12      0x1e
#define SCI_DIV16_372_12        0x1

#define SCI_MAINDIV_372_16      0x16
#define SCI_DIV16_372_16        0x1    // SCI_DIV16_372_12 is 1, but it will be set to 0 in the new version


#define SCI_MAINDIV_512_8       0x3f
#define SCI_DIV16_512_8         0x03

#define SCI_MAINDIV_512_12      0x29
#define SCI_DIV16_512_12        0x01

#define SCI_MAINDIV_512_16      0x1f
#define SCI_DIV16_512_16        0x01

#define SCI_MAINDIV_512_32      0x0f
#define SCI_DIV16_512_32        0x00

#define SCI_CLK_MAX             3250000

/// Time to wait after a change of the ETU duration, in number of ms.
#define  HAL_SIM_ETU_WAIT       100


// ============================================================================
//  TYPES
// ============================================================================

// ============================================================================
// HAL_SIM_IRQ_INTERNAL_CAUSE_T
// ----------------------------------------------------------------------------
/// Sim irq cause, as in the hardware registers
// ============================================================================
typedef struct
{
    UINT32 rxDone:1;
    UINT32 rxHalf:1;
    UINT32 wwtTimeout:1;
    UINT32 extraRx:1;
    UINT32 resendOvfl:1;
    UINT32 argEnd:1;
} HAL_SIM_IRQ_INTERNAL_CAUSE_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

PRIVATE HWP_SCI_T* CONST gc_halSimHwpArray[] =
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    hwp_sci,
#else // smart phones, 8809P or later feature phones
    hwp_sci1,
    hwp_sci2,
    hwp_sci3,
#endif
};

PRIVATE CONST HAL_IFC_REQUEST_ID_T gc_halSimIfcTxId[] =
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    HAL_IFC_SCI_TX,
#else // smart phones, 8809P or later feature phones
    HAL_IFC_SCI_TX,
    HAL_IFC_SCI2_TX,
    HAL_IFC_SCI3_TX,
#endif
};

PRIVATE CONST HAL_IFC_REQUEST_ID_T gc_halSimIfcRxId[] =
{
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8808) || \
    (CHIP_ASIC_ID == CHIP_ASIC_ID_8809)
    HAL_IFC_SCI_RX,
#else // smart phones, 8809P or later feature phones
    HAL_IFC_SCI_RX,
    HAL_IFC_SCI2_RX,
    HAL_IFC_SCI3_RX,
#endif
};

PRIVATE UINT8 g_halSimIndex = 0;

PRIVATE UINT8  g_halReceivingAtr;
PRIVATE UINT8  g_halIfcChan;
PRIVATE UINT8  g_halRxDmaInProgress;
PRIVATE UINT8* g_halRxBuf;

PRIVATE UINT16 g_halRxBytes;
//PRIVATE volatile BOOL SplitXfer, SplitAutoClockMode;

PRIVATE UINT16 g_halRxExpected;
PRIVATE UINT32 g_halSciMainDivVal;

/// Variable to store the user handler
PRIVATE HAL_SIM_IRQ_HANDLER_T g_halSimRegistry = NULL;

PRIVATE HAL_SIM_IRQ_STATUS_T g_halSimUserMask  = {1,1,1,1,1,1,1,1};

PRIVATE HWP_SCI_T g_sci_reg;

#ifdef GALLITE_IS_8806
PRIVATE UINT32 g_ResetIndex = 0;
#endif

#ifdef SIM_TX_IFC_IN_SRAM
PRIVATE UINT8 HAL_UNCACHED_BSS_INTERNAL g_sim_tx_data_buffer[255];
#endif

static HAL_CLK_T *gSimClk = NULL;

// ============================================================================
//  FUNCTIONS
// ============================================================================


PUBLIC UINT32 hal_SimGetRegConfig(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Config;
}

PUBLIC UINT32 hal_SimGetRegClkDiv(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->ClkDiv;
}

PUBLIC UINT32 hal_SimGetRegTimes(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Times;
}

PUBLIC UINT32 hal_SimGetRegChFilt(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Ch_Filt;
}

PUBLIC UINT32 hal_SimGetRegIntMask(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Int_Mask;
}

PUBLIC UINT32 hal_SimGetRegIntCause(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Int_Cause;
}

PUBLIC UINT32 hal_SimGetRegRxCnt(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->RxCnt;
}

PUBLIC UINT32 hal_SimGetRegStatus(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Status;
}

PUBLIC VOID hal_simParseReg(VOID)
{
    g_sci_reg.Config = hal_SimGetRegConfig();
    g_sci_reg.Status = hal_SimGetRegStatus();
    g_sci_reg.ClkDiv = hal_SimGetRegClkDiv();
    g_sci_reg.RxCnt = hal_SimGetRegRxCnt();
    g_sci_reg.Times = hal_SimGetRegTimes();
    g_sci_reg.Ch_Filt = hal_SimGetRegChFilt();
    g_sci_reg.Int_Cause = hal_SimGetRegIntCause();
    g_sci_reg.Int_Mask = hal_SimGetRegIntMask();
}

PUBLIC VOID hal_SimRestoreRegConfig(UINT32 Restore)
{
    // Don't write the ARG_H or AFD_EN_H bits
    gc_halSimHwpArray[g_halSimIndex]->Config = Restore & ~(SCI_ARG_H | SCI_AFD_EN_H) ;
}

PUBLIC VOID hal_SimRestoreRegClkDiv(UINT32 Restore)
{
    gc_halSimHwpArray[g_halSimIndex]->ClkDiv = Restore;
}

PUBLIC VOID hal_SimRestoreRegTimes(UINT32 Restore)
{
    gc_halSimHwpArray[g_halSimIndex]->Times = Restore;
}

PUBLIC VOID hal_SimRestoreRegChFilt(UINT32 Restore)
{
    gc_halSimHwpArray[g_halSimIndex]->Ch_Filt = Restore;
}

PUBLIC VOID hal_SimRestoreRegIntMask(UINT32 Restore)
{
    gc_halSimHwpArray[g_halSimIndex]->Int_Mask = Restore;
}


PUBLIC UINT32 hal_SimGetResetPol(VOID)
{
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Config;
}


// ============================================================================
// hal_SimUpdateDivider
// ----------------------------------------------------------------------------
/// Update the SIM main divider when the system clock change
/// @param newSysFreq New system Frequency.
// ============================================================================
UINT32 hal_SimUpdateDivider(HAL_SYS_FREQ_T newSysFreq)
{
    UINT32 clkReg = gc_halSimHwpArray[g_halSimIndex]->ClkDiv & ~SCI_MAINDIV_MASK;

#if defined(FPGA)

#ifdef FPGA_SLOW
    g_halSciMainDivVal = (newSysFreq/(2*SCI_CLK_MAX)) - 1;
#else
    // Fixed to 52M for FPGA
#ifdef FPGA_TOP_CLK_312M
    g_halSciMainDivVal = ((newSysFreq/2)/(2*SCI_CLK_MAX)) - 1;
#else
    g_halSciMainDivVal = (newSysFreq/(2*SCI_CLK_MAX)) - 1;

#endif
#endif
#else
    g_halSciMainDivVal = (newSysFreq/(2*SCI_CLK_MAX)) - 1;

#endif
    gc_halSimHwpArray[g_halSimIndex]->ClkDiv = clkReg | SCI_MAINDIV(g_halSciMainDivVal);
//    HAL_TRACE(HAL_SIM_TRC | TSTDOUT, 0, "SIM clock sys:%d div:%d", newSysFreq, g_halSciMainDivVal);
    return SCI_CLK_MAX;
}


// ============================================================================
// hal_SimSetEtuBitflied
// ----------------------------------------------------------------------------
/// Set the ETU duration bitfield
///
/// @param F, the clock divider param F for generating the baud clock
/// from SCI clock
/// @param D, the clock divider param D
// ============================================================================
INLINE VOID hal_SimSetEtuBitfield(UINT16 F, UINT8 D, UINT8 SciInvFlag)
{
    // Need to change the SCI clock frequency and maybe also
    // the internal SCI dividers
    UINT32 InvFlag;
    InvFlag =     (SciInvFlag == 1) ? SCI_CLK_INV : 0;
    UINT32 temp = 0;
#ifdef CHIP_DIE_8955
    temp = gc_halSimHwpArray[g_halSimIndex]->ClkDiv & 0x7e00;
#endif

    if (F == 372)
    {
        if(D == 16)
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_372_16) |
                    SCI_CLKDIV(SCI_MAINDIV_372_16) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
        else if(D == 12)
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_372_12) |
                    SCI_CLKDIV(SCI_MAINDIV_372_12) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
        else if(D == 8)
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_372_8) |
                    SCI_CLKDIV(SCI_MAINDIV_372_8) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
        else
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_372_1) |
                    SCI_CLKDIV(SCI_MAINDIV_372_1) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
    }
    else if(F == 512)
    {
        if (D == 16)
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_512_16) |
                    SCI_CLKDIV(SCI_MAINDIV_512_16) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
        else if(D == 12)
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_512_12) |
                    SCI_CLKDIV(SCI_MAINDIV_512_12) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
        else
        {
            gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_512_8) |
                    SCI_CLKDIV(SCI_MAINDIV_512_8) |
                    SCI_MAINDIV(g_halSciMainDivVal)|
                    temp|
                    InvFlag;
        }
    }

    else // default case
    {
        gc_halSimHwpArray[g_halSimIndex]->ClkDiv =   SCI_CLKDIV_16(SCI_DIV16_512_8) |
                SCI_CLKDIV(SCI_MAINDIV_512_8) |
                SCI_MAINDIV(g_halSciMainDivVal)|
                temp|
                InvFlag;
    }

}


// ============================================================================
// hal_SimClrConfigBit
// ----------------------------------------------------------------------------
/// Clear the config bits in the mask parameter.
/// @param mask Bit to '1' will be cleared.
// ============================================================================
INLINE VOID hal_SimClrConfigBit(UINT32 mask)
{
    gc_halSimHwpArray[g_halSimIndex]->Config &= ~(mask);
}


// ============================================================================
// hal_SimSetConfigBit
// ----------------------------------------------------------------------------
/// Set the selected bit in the config register.
///
/// @param mask Bitfield where the bit whose
/// value is '1' in the mask will be set in the config
/// register. The other bits are unchanged
// ============================================================================
INLINE VOID hal_SimSetConfigBit(UINT32 mask)
{
    gc_halSimHwpArray[g_halSimIndex]->Config |= mask;
}

// TODO: Add a enum type for different data convention formats
PUBLIC UINT32 hal_SimGetDataConvFormat(VOID)
{
    UINT32 simConfig = (UINT32)gc_halSimHwpArray[g_halSimIndex]->Config;
    if((simConfig & SCI_LLI)!=0 && (simConfig & SCI_MSBH_LSBL)!=0)
    {
        return 1;
    }
    else if((simConfig & SCI_LLI)==0 && (simConfig & SCI_MSBH_LSBL)==0)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}


PUBLIC VOID hal_SimSetDataConvFormat(UINT32 Ts)
{
    // Ensure AFD is disabled to support different SIM data convetions in dual SIM mode.
    hal_SimClrConfigBit(SCI_AFD_EN_H);

    if(Ts == 0x3B)
    {
        hal_SimClrConfigBit(SCI_LLI | SCI_MSBH_LSBL);
    }
    else
    {
        hal_SimSetConfigBit(SCI_LLI | SCI_MSBH_LSBL);
    }
}


// ============================================================================
// hal_SimTxByte
// ----------------------------------------------------------------------------
/// Send a byte to the sim card
/// @param txByte The byte to transmit.
// ============================================================================
INLINE VOID hal_SimTxByte(UINT8 txByte)
{
    gc_halSimHwpArray[g_halSimIndex]->Data = txByte;
}


// ============================================================================
// hal_SimSendNTxBytes
// ----------------------------------------------------------------------------
/// Send a bunch of byte to the SIM using the IFC
///
/// @param nTx Number of bytes to send.
/// @param pTxBytes Buffer of bytes to send.
/// @return Channel affected to the transfer.
// ============================================================================
INLINE UINT8 hal_SimSendNTxBytes(UINT8 nTx, CONST UINT8 *pTxBytes)
{
#ifdef SIM_TX_IFC_IN_SRAM
    memcpy(&g_sim_tx_data_buffer[0], pTxBytes, nTx);
    return (hal_IfcTransferStart(gc_halSimIfcTxId[g_halSimIndex], (UINT8*)&g_sim_tx_data_buffer[0], nTx, HAL_IFC_SIZE_8_MODE_AUTO));
#else // !SIM_TX_IFC_IN_SRAM
    return (hal_IfcTransferStart(gc_halSimIfcTxId[g_halSimIndex], (UINT8*)pTxBytes, nTx, HAL_IFC_SIZE_8_MODE_AUTO));
#endif
}


// ============================================================================
// hal_SimSetNRxBytes
// ----------------------------------------------------------------------------
/// Configure the reception of a bunch of data from the sim
/// @param nRx Number of bytes to be expected by the Rx fifo
/// @param nRxIfc Number of bytes to read through the IFC if used
/// @param pRxBytes Buffer where read data will be written
/// @param useIfc  \c TRUE if IFC is to be used \n
///                 \c FALSE otherwise.
/// @param stopClock If \c TRUE and in auto-mode, the clock is
/// stopped after this transfer.
/// @return The IFC channel used, if any.
// ============================================================================
PRIVATE UINT8 hal_SimSetNRxBytes(UINT16 nRx, UINT16 nRxIfc, UINT8* pRxBytes,
                                 BOOL useIfc, BOOL stopClock)
{
    UINT32 channel;
    if (useIfc == TRUE)
    {
        channel = hal_IfcTransferStart(gc_halSimIfcRxId[g_halSimIndex], pRxBytes, nRxIfc, HAL_IFC_SIZE_8_MODE_AUTO);
    }
    else
    {
        channel =  HAL_UNKNOWN_CHANNEL;
    }
    g_halRxExpected = nRxIfc;

    if (stopClock)
    {
        gc_halSimHwpArray[g_halSimIndex]->RxCnt = nRx; //clk_persist bit set to 0
    }
    else
    {
        gc_halSimHwpArray[g_halSimIndex]->RxCnt = nRx | SCI_CLK_PERSIST; // clk_persit bit set to 1
    }

    return channel;
}


// ============================================================================
// hal_SimAutoReset
// ----------------------------------------------------------------------------
/// Does the basic write to begin the Auto
/// Reset sequence
///
/// The interrupt should be previously enabled to handle the end of ARG
/// The bits are set according to the following
/// 00: SCI_ENABLE = 1;\n
/// 01: SCI_PARITY = 0;\n
/// 02: SCI_PERF = 0;\n
/// 03: SCI_FILTER_DISABLE = 0;\n\n
///
/// 04: SCI_CLOCKSTOP = 0\n;
/// 05: SCI_AUTOSTOP_EN_H = 0\n;
/// 06: SCI_MSBH_LSBL = <unchanged>\n;
/// 07: SCI_LLI = <unchanged>\n\n;
///
/// 08: SCI_PEGEN_LEN = 1;\n\n
///
/// 16: SCI_ARG_H = 1\n;
/// 17: SCI_AFD_EN_H = 0\n;
/// 18: SCI_TX_RESEND_EN_H = 1;\n\n
///
/// 20: SCI_RESET = 1;\n\n
///
/// Don't enable parity checking on Rst, use Manual Mode for Clk_SCI
/// Keep the default value for parity offset.
// ============================================================================
INLINE VOID hal_SimAutoReset(VOID)
{
    // Clear all but upper bits and SCI_RESET/SCI_LLI/SCI_MSBH_LSBL
    // AFD must be disabled to support different SIM data convention in dual SIM mode
    hal_SimClrConfigBit(SCI_ENABLE |
                        SCI_PARITY_ODD_PARITY |
                        SCI_PERF |
                        SCI_FILTER_DISABLE |
                        SCI_CLOCKSTOP |
                        SCI_AUTOSTOP_EN_H |
                        //SCI_MSBH_LSBL |
                        //SCI_LLI |
                        SCI_PEGEN_LEN |
                        SCI_PARITY_EN |
                        SCI_STOP_LEVEL |
                        SCI_ARG_H |
                        SCI_AFD_EN_H |
                        SCI_TX_RESEND_EN_H |
#ifdef CHIP_DIE_8955
                        SCI_IN_AVG_EN |
#endif
                        //SCI_RESET |
                        SCI_DLY_SEL);


    // ARG is enabled in auto reset, but if ARG fails, it will be disabled in power down process.
    // SCI_RESET must be set to ensure SIM card is NOT reset even when disabling ARG.
    // SCI_PARITY must be cleared in reset process.
    hal_SimSetConfigBit(SCI_ENABLE |
                        SCI_PEGEN_LEN |
                        SCI_ARG_H |
                        SCI_TX_RESEND_EN_H |
                        SCI_DLY_SEL);

    hal_TimDelay(3);

    hal_SimSetConfigBit(SCI_RESET);
}


PUBLIC VOID hal_SimRegReset(VOID)
{
    hal_SimSetConfigBit(SCI_RESET);
    hal_SimClrConfigBit(SCI_ARG_H);
}


PUBLIC VOID hal_SimDeactive(u8 type)
{
    hal_SimClrConfigBit(SCI_ARG_H);
    hal_SimClrConfigBit(SCI_RESET);
    hal_TimDelay(50);

    hal_SimClrConfigBit(SCI_AUTOSTOP_EN_H);
    hal_SimSetConfigBit(SCI_CLOCKSTOP);
    hal_TimDelay(50);

    if(type == 1)  // PULL DOWN IO to 0
    {
        UINT16 value = 0;

        if(g_halSimIndex == 0)
            value |= ABB_SIM_SELECT_SIM2 | ABB_PULLDOWN_ENABLE_1 ;
        else if(g_halSimIndex == 1)
            value |= ABB_SIM_SELECT_SIM1 | ABB_PULLDOWN_ENABLE_2;

        hal_AbbRegWrite(CODEC_SIM_INTERFACE, value);
        hal_TimDelay(50);
    }
}


// ============================================================================
// hal_SimSetWwi
// ----------------------------------------------------------------------------
/// Set the WWI factor defining the WWT
///
/// It is the Work Waiting Time factor.  A timeout will be generated
/// when the WWT is exceeded.  The WWT is calculated by:
/// <code> WWT = 960 x WI x (F/Fi) </code> \n
/// where \c Fi is the main SCI clock frequency (3-5 MHz) and
/// \c F is 372 before an enhanced PPS and 512 after an enhanced PPS.
/// \n
/// The SCI_WI value must be calculated as follows: \n
///   <code> SCI_WI = WI * D </code> \n
/// Thus, by default (WI = 10) this value needs to be set to 10 before
/// an EPPS, but needs to be scaled to WI*D=80 after the EPPS procedure.
///
/// @param WI WWI value to set
// ============================================================================
INLINE VOID hal_SimSetWwi(UINT8 wi)
{
#if (CHIP_HAS_SIM_T0T1 == 0)
    gc_halSimHwpArray[g_halSimIndex]->Times &= ~(SCI_WI_MASK);   // Clear old WI value
    gc_halSimHwpArray[g_halSimIndex]->Times |= SCI_WI(wi);
#else
    gc_halSimHwpArray[g_halSimIndex]->Bwt_Wwt = 960*wi;
#endif
}


// ============================================================================
// hal_SimGetArgStatus
// ----------------------------------------------------------------------------
/// Returns the status of the automatic reset procedure:
/// @return 0 = ARG detection has failed. \n
/// 1 = ARG detection has detected that the SIM has responded to the reset
// ============================================================================
INLINE BOOL hal_SimGetArgStatus(VOID)
{

    if ((gc_halSimHwpArray[g_halSimIndex]->Status & SCI_ARG_DET) == 0)
    {
        // SCI_ARG_DET bit = 0, failed
        return(FALSE);
    }
    else
    {
        return(TRUE);
    }
}


// ============================================================================
// hal_SimGetClkStatus
// ----------------------------------------------------------------------------
/// Returns the status of the clock:
/// @return 0 = clock on. \n
/// 1 = clock off
// ============================================================================
PUBLIC BOOL hal_SimGetClkStatus(VOID)
{

    if ((gc_halSimHwpArray[g_halSimIndex]->Status & SCI_CLK_OFF) == 0)
    {
        // SCI_CLK_OFF bit = 0, clock on
        return(TRUE);
    }
    else
    {
        // clock off
        return(FALSE);
    }
}

// ============================================================================
// hal_SimGetClkStopStatus
// ----------------------------------------------------------------------------
/// Returns the status of the clock stop mode:
/// @return 0 = manual mode. \n
/// 1 = auto stop mode
// ============================================================================
PUBLIC BOOL hal_SimGetClkStopStatus(VOID)
{

    if ((gc_halSimHwpArray[g_halSimIndex]->Status & SCI_AUTOSTOP_STATE_MASK) == 0  ) //manual clock mode
    {
        return(FALSE);
    }
    else
    {
        // auto stop mode
        return(TRUE);
    }
}

// ============================================================================
// hal_SimGetResetStatus
// ----------------------------------------------------------------------------
/// Returns the status of the reset pin after automatic reset procedure.  Since
/// the card is _not_ in reset after a successful ATR, the reset pin (and the
/// status bit) is in the _NON-ACTIVE RESET_ state.
/// @return 1 = Reset pin at 0 (Active high Reset) \n
/// 1 = Reset pin at 0 (Active low Reset)
// ============================================================================
PUBLIC UINT8 hal_SimGetResetStatus(VOID)
{
    if ((gc_halSimHwpArray[g_halSimIndex]->Status & SCI_RESET_DET) == 0)
    {
        return(0);
    }
    else
    {
        return(1);
    }
}

#ifdef GALLITE_IS_8806
PUBLIC VOID hal_SimSetDbg(VOID)
{
    if ((gc_halSimHwpArray[g_halSimIndex]->Status  & SCI_AUTOSTOP_RXCOUNTER_MASK) != 0)
    {
        hal_HstSendEvent(0x22110000 );
    }
    else
    {
        gc_halSimHwpArray[g_halSimIndex]->dbg = 0x1;
        while( (gc_halSimHwpArray[g_halSimIndex]->Status & SCI_AUTOSTOP_RXCOUNTER_MASK) != 0)
        {
            gc_halSimHwpArray[g_halSimIndex]->dbg = 0x1;
        };

        gc_halSimHwpArray[g_halSimIndex]->dbg = 0x0;
        gc_halSimHwpArray[g_halSimIndex]->dbg = 0x0;
    }
}


PUBLIC VOID hal_SimResetSys(VOID)
{
    hwp_sysCtrl->REG_DBG     = SYS_CTRL_PROTECT_UNLOCK;
    hwp_sysCtrl->Sys_Rst_Set = SYS_CTRL_SET_RST_SCI;
    hwp_sysCtrl->Sys_Rst_Clr  = SYS_CTRL_CLR_RST_SCI;
    hwp_sysCtrl->REG_DBG    = SYS_CTRL_PROTECT_LOCK;
}
#endif


// ============================================================================
// hal_SimGetRxCnt
// ----------------------------------------------------------------------------
/// @return The number of byte yet to receive.
// ============================================================================
INLINE UINT32 hal_SimGetRxCnt(VOID)
{
    return (g_halRxExpected - gc_halSimHwpArray[g_halSimIndex]->RxCnt);
}


// ============================================================================
// hal_SimReleaseIFC
// ----------------------------------------------------------------------------
///  Release the IFC channel used by the sci
// ============================================================================
INLINE VOID hal_SimReleaseIFC(VOID)
{
    hal_IfcChannelFlush(gc_halSimIfcRxId[g_halSimIndex], g_halIfcChan);
    hal_IfcChannelRelease(gc_halSimIfcRxId[g_halSimIndex], g_halIfcChan);
}


// ============================================================================
// hal_SimGetRxByte
// ----------------------------------------------------------------------------
/// Read a byte receive from the SIM card.
/// @return A byte read from the SIM card.
// ============================================================================
INLINE UINT8 hal_SimGetRxByte(VOID)
{
    return (gc_halSimHwpArray[g_halSimIndex]->Data);
}


// ============================================================================
// hal_SimEnableParity
// ----------------------------------------------------------------------------
/// Selects the parity generation/detection
///
/// @param enable If \c FALSE, even parity is selected.
///               If \c TRUE, odd parity is selected.
// ============================================================================
INLINE VOID hal_SimEnableParity(BOOL enable)
{
    if (enable == TRUE)
    {
        hal_SimSetConfigBit(SCI_PARITY_EN);
    }
    else
    {
        hal_SimClrConfigBit(SCI_PARITY_EN);
    }
}


// ============================================================================
// hal_SimOpen
// ----------------------------------------------------------------------------
/// Open the SIM driver and enables the use of its API,
/// and initializes the SIM.
// ============================================================================
PUBLIC VOID hal_SimOpen(VOID)
{
    g_halReceivingAtr = 0;
    g_halRxDmaInProgress = 0;

    for (int i=0; i<sizeof(gc_halSimHwpArray)/sizeof(gc_halSimHwpArray[0]); i++)
    {
        // Set the physical IRQ mask to let the driver properly work
        gc_halSimHwpArray[i]->Int_Mask = SCI_RX_DONE |
                                         SCI_WWT_TIMEOUT |
                                         SCI_EXTRA_RX |
                                         SCI_RESEND_OVFL |
                                         SCI_ARG_END;
    }

    // Set main divider according to the system frequency.
    g_halSciMainDivVal = (g_halSysSystemFreq/(2*SCI_CLK_MAX)) - 1;
#if defined(FPGA)

#ifdef FPGA_SLOW
    g_halSciMainDivVal = (g_halSysSystemFreq/(2*SCI_CLK_MAX)) - 1;
#else
    // Fixed to 52M for FPGA
#ifdef FPGA_TOP_CLK_312M
    g_halSciMainDivVal = ((g_halSysSystemFreq/2)/(2*SCI_CLK_MAX)) - 1;
#else
    g_halSciMainDivVal = (g_halSysSystemFreq/(2*SCI_CLK_MAX)) - 1;

#endif
#endif
#else
    g_halSciMainDivVal = (g_halSysSystemFreq/(2*SCI_CLK_MAX)) - 1;

#endif
    g_halRxExpected = 0;

    // Set some other stuff?
    hal_SimSetEtuBitfield(372, 1, 1);

    UINT32 scStatus = hal_SysEnterCriticalSection();
    // Update the divider
    hal_SimUpdateDivider(hal_SysGetFreq());
    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
// hal_SimClose
// ----------------------------------------------------------------------------
/// Close the SIM driver and disables the use of its API.
/// #hal_SimPowerDown has been called before that function.
// ============================================================================
PUBLIC VOID hal_SimClose(VOID)
{
    // Clear all but upper bits
    hal_SimClrConfigBit(SCI_ENABLE |
                        SCI_PARITY_ODD_PARITY |
                        SCI_PERF |
                        SCI_FILTER_DISABLE |
                        SCI_CLOCKSTOP |
                        SCI_AUTOSTOP_EN_H |
                        SCI_MSBH_LSBL |
                        SCI_LLI |
                        SCI_PEGEN_LEN |
                        SCI_PARITY_EN |
                        SCI_STOP_LEVEL |
                        SCI_ARG_H |
                        SCI_AFD_EN_H |
                        SCI_TX_RESEND_EN_H |
                        SCI_RESET |
                        SCI_DLY_SEL);
}


#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8808) && \
    (CHIP_ASIC_ID != CHIP_ASIC_ID_8809)
// smart phones, 8809P or later feature phones

// ============================================================================
// hal_SimSetSimIndex
// ----------------------------------------------------------------------------
/// Set current SIM index.
/// @param index The SIM card interface index.
// ============================================================================
PUBLIC VOID hal_SimSetSimIndex(UINT8 index)
{
    HAL_ASSERT(index < sizeof(gc_halSimHwpArray)/sizeof(gc_halSimHwpArray[0]),
               "Invalid SIM index: %d", index);

    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_halSimIndex = index;
    // Update the clock divider
    UINT32 clkReg = gc_halSimHwpArray[g_halSimIndex]->ClkDiv & ~SCI_MAINDIV_MASK;
    gc_halSimHwpArray[g_halSimIndex]->ClkDiv = clkReg | SCI_MAINDIV(g_halSciMainDivVal);
    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
// hal_SimGetSimIndex
// ----------------------------------------------------------------------------
/// Get current SIM index.
/// @return current SIM index.
// ============================================================================
PUBLIC UINT8 hal_SimGetSimIndex(VOID)
{
    return g_halSimIndex;
}


// ============================================================================
// hal_SimSciEnabledEx
// ----------------------------------------------------------------------------
/// Check whether the specified SIM card interface hardware module has been enabled.
/// @param index The SIM card interface index.
/// @return TRUE if enabled, FALSE otherwise.
// ============================================================================
INLINE BOOL hal_SimSciEnabledEx(UINT8 index)
{
    return ((gc_halSimHwpArray[index]->Config & SCI_ENABLE) != 0);
}


// ============================================================================
// hal_SimClrConfigBitEx
// ----------------------------------------------------------------------------
/// Clear the config bits in the mask parameter.
/// @param index The SIM card interface index.
/// @param mask Bit to '1' will be cleared.
// ============================================================================
INLINE VOID hal_SimClrConfigBitEx(UINT8 index, UINT32 mask)
{
    gc_halSimHwpArray[index]->Config &= ~(mask);
}


// ============================================================================
// hal_SimSetConfigBitEx
// ----------------------------------------------------------------------------
/// Set the selected bit in the config register.
/// @param index The SIM card interface index.
/// @param mask Bitfield where the bit whose
/// value is '1' in the mask will be set in the config
/// register. The other bits are unchanged
// ============================================================================
INLINE VOID hal_SimSetConfigBitEx(UINT8 index, UINT32 mask)
{
    gc_halSimHwpArray[index]->Config |= mask;
}


// ============================================================================
// hal_SimEnableSciEx
// ----------------------------------------------------------------------------
/// Enalbe the specified SIM card interface hardware module.
/// @param index The SIM card interface index.
// ============================================================================
PUBLIC VOID hal_SimEnableSciEx(UINT8 index)
{
    HAL_ASSERT(index < sizeof(gc_halSimHwpArray)/sizeof(gc_halSimHwpArray[0]),
               "Invalid SIM index: %d", index);

    if (!hal_SimSciEnabledEx(index))
    {
        // Clear previous IRQs if any
        gc_halSimHwpArray[index]->Int_Clr = gc_halSimHwpArray[index]->Int_Cause;

        // Recover some config bits, to avoid messing up SIM state when
        // switching to another SIM card.
        // If failing to reset previous SIM card (ARG failure), the
        // power down process will clear most config bits.
        hal_SimSetConfigBitEx(index, SCI_ENABLE |
                              SCI_PEGEN_LEN |
                              SCI_PARITY_EN |
                              SCI_TX_RESEND_EN_H |
                              SCI_RESET);
    }
}


// ============================================================================
// hal_SimDisableSciEx
// ----------------------------------------------------------------------------
/// Disable the specified SIM card interface hardware module.
/// @param index The SIM card interface index.
// ============================================================================
PUBLIC VOID hal_SimDisableSciEx(UINT8 index)
{
    HAL_ASSERT(index < sizeof(gc_halSimHwpArray)/sizeof(gc_halSimHwpArray[0]),
               "Invalid SIM index: %d", index);

    hal_SimClrConfigBitEx(index, SCI_ENABLE);
}

#endif


// ============================================================================
// hal_SimSciEnabled
// ----------------------------------------------------------------------------
/// Check whether the SIM card interface hardware module has been enabled.
/// @return TRUE if enabled, FALSE otherwise.
// ============================================================================
INLINE BOOL hal_SimSciEnabled(VOID)
{
    return ((gc_halSimHwpArray[g_halSimIndex]->Config & SCI_ENABLE) != 0);
}


// ============================================================================
// hal_SimEnableSci
// ----------------------------------------------------------------------------
/// Enalbe the SIM card interface hardware module.
// ============================================================================
PUBLIC VOID hal_SimEnableSci(VOID)
{
    if (!hal_SimSciEnabled())
    {
        // Clear previous IRQs if any
        gc_halSimHwpArray[g_halSimIndex]->Int_Clr = gc_halSimHwpArray[g_halSimIndex]->Int_Cause;

        // Recover some config bits, to avoid messing up SIM state when
        // switching to another SIM card.
        // If failing to reset previous SIM card (ARG failure), the
        // power down process will clear most config bits.
        hal_SimSetConfigBit(SCI_ENABLE |
                            SCI_PEGEN_LEN |
                            SCI_PARITY_EN |
                            SCI_TX_RESEND_EN_H);
    }
}


// ============================================================================
// hal_SimDisableSci
// ----------------------------------------------------------------------------
/// Disable the SIM card interface hardware module.
// ============================================================================
PUBLIC VOID hal_SimDisableSci(VOID)
{
    hal_SimClrConfigBit(SCI_ENABLE);
//    hal_SimClrConfigBit(SCI_RESET);
}


// ============================================================================
// hal_SimReset
// ----------------------------------------------------------------------------
/// This function starts the reset sequence, during which
/// the timing of each signal clock, data, vcc are directly controlled by
/// the driver. At the end of the reset, an ATR success interrupt
/// is sent
///
/// @param pRxByte Pointer to the buffer used to store the reset
/// command response.
// ============================================================================
PUBLIC VOID hal_SimReset(UINT8* pRxBytes, U8 length)
{
#ifdef GALLITE_IS_8806
    g_ResetIndex = 1;
#endif
    g_halRxDmaInProgress = 1;
    HAL_TRACE(HAL_SIM_TRC | TSTDOUT, 0, "HAL_SIM[%d]: SIM reset buffer: %#x",
              g_halSimIndex, pRxBytes);
    g_halIfcChan = hal_SimSetNRxBytes(length, length, pRxBytes, TRUE, FALSE);
    hal_SimAutoReset();
    g_halRxBuf = pRxBytes;
}


// ============================================================================
// hal_SimPowerDown
// ----------------------------------------------------------------------------
/// This function does a power down of the SIM. The timings of each signals
/// are controlled by the driver, which switch off the clock and Vcc.
// ============================================================================
PUBLIC VOID hal_SimPowerDown(VOID)
{
    // Clear all but upper bits and SCI_RESET/SCI_LLI/SCI_MSBH_LSBL
    // AFD must be disabled to support different SIM data convention in dual SIM mode
    hal_SimClrConfigBit(SCI_ENABLE |
                        SCI_PARITY_ODD_PARITY |
                        SCI_PERF |
                        SCI_FILTER_DISABLE |
                        SCI_CLOCKSTOP |
                        SCI_AUTOSTOP_EN_H |
                        //SCI_MSBH_LSBL |
                        //SCI_LLI |
                        SCI_PEGEN_LEN |
                        SCI_PARITY_EN |
                        SCI_STOP_LEVEL |
                        SCI_ARG_H |
                        SCI_AFD_EN_H |
                        SCI_TX_RESEND_EN_H);

    // Disable the SCI clock (auto clock control has been disabled)
    hal_SimSetConfigBit(SCI_CLOCKSTOP);
}


// ============================================================================
// hal_SimInterfaceActive
// ----------------------------------------------------------------------------
/// This is used to determine when the interface is no longer busy.  It does
/// that by checking whether the SCI clock is off.  If it is off, the interface
/// has completed its current transaction including the shutdown delay.
/// @return TRUE if the interface is active and the clock is still on, FALSE
/// if the interface is inactive and the clock is off.
// ============================================================================
PUBLIC BOOL hal_SimInterfaceActive(VOID)
{
    // SCI_CLK_OFF = 0 => SCI Clock is on (active)
    // SCI_CLK_OFF = 1 => SCI Clock is off (inactive)
    return (((gc_halSimHwpArray[g_halSimIndex]->Status) & SCI_CLK_OFF)==0);
}


// ============================================================================
// hal_SimCheckClockStatus
// ----------------------------------------------------------------------------
/// This function checks the clock status of the SIM modules. When it is not
/// needed, the request for a clock is given up (ie request for no clock
/// is done.)
// ============================================================================
PROTECTED VOID hal_SimCheckClockStatus(VOID)
{
    // No more need for a clock
    if((gc_halSimHwpArray[g_halSimIndex]->Status) & SCI_CLK_OFF)
    {
        // Set the resource as inactive
        if (hal_ClkIsEnabled(gSimClk))
            hal_ClkDisable(gSimClk);
    }
}


// ============================================================================
// hal_SimSetClockStopMode
// ----------------------------------------------------------------------------
/// This function is used to indicate to the driver how the sim clock should be
/// switch off, depending on the ATR response.
///
/// @param mode This is the stop mode to be set.
// ============================================================================
PUBLIC VOID hal_SimSetClockStopMode(HAL_SIM_CLOCK_STOP_MODE_T mode)
{
    HAL_TRACE(HAL_SIM_TRC, 0, "hal_SimSetClockStopMode %d", mode);
    if (mode == HAL_SIM_CLOCK_NO_STOP)
    {
        // This will be called when switching from any Auto mode to Manual
        // mode.  Normally, this should not be done.
        // Immediately switch to _MANUAL ON_ mode only and no timings will
        // be respected.
        hal_SimClrConfigBit(SCI_AUTOSTOP_EN_H | SCI_CLOCKSTOP);
    }
    else
    {
        // This will be called when switching from manual mode to
        // enable automatic clock stop.  The clock should normally
        // be on already and calling this will go to Auto Shutdown, then
        // Auto Off states.  If the clock is off, the SCI will go directly to
        // Auto Off.
        if (mode == HAL_SIM_CLOCK_STOP_L)
        {
            hal_SimClrConfigBit(SCI_STOP_LEVEL);
        }
        else // HAL_SIM_CLOCK_STOP_H
        {
            hal_SimSetConfigBit(SCI_STOP_LEVEL);
        }
        hal_SimSetConfigBit(SCI_AUTOSTOP_EN_H | SCI_CLOCKSTOP);
    }
}


// ============================================================================
// hal_SimTxByteRxByte
// ----------------------------------------------------------------------------
/// Starts a SIM transfer.
/// A sim transfer consists in sending nTxByte and receiving an answer of
/// nRxByte.
/// At the end of the transfer, an interrupt is sent, which is recognized
/// by the user handler as #HAL_SIM_DMA_SUCCESS.
///
/// @param nTxByte Number of byte to send (Size of the command)
/// @param pTxByte Pointer to the buffer containing the command to send
/// @param nRxByte Number of byte to receive (Size of the answer)
/// @param pRxByte Pointer to the buffer where to store the answer
/// @param stopClock In case of an operation consisting in two commands,
/// we do not want to stop the sim clock between the two commands. In that
/// case we set this parameter to \c FALSE for the first command.
/// Setting it to \c TRUE will stop the clock after the command (For the
/// second command of the pair or a standalone command).
// ============================================================================
PUBLIC VOID hal_SimTxByteRxByte(
    UINT16 nTxByte, CONST UINT8* pTxByte,
    UINT16 nRxByte, UINT8* pRxByte,
    BOOL stopClock)

{
    UINT32 txCnt;

    HAL_PROFILE_FUNCTION_ENTER(hal_SimTxByteRxByte);
    // Set resource as active
    // Rq: The setting of the divider is handled by the frequency
    // scaling mechanism.
    if (!hal_ClkIsEnabled(gSimClk))
        hal_ClkEnable(gSimClk);
    HAL_TRACE(HAL_SIM_TRC, 0, "hal_SimTxByteRxByte nTxByte=%d, nRxByte=%d, stopClock=%d", nTxByte, nRxByte, stopClock);

#ifdef GALLITE_IS_8806
#ifdef CT1129_OLD_SIM
    if(!g_ResetIndex)
    {
        extern UINT8 g_SimIndex ;

        extern UINT16 g_palEtuParaF[];
        extern UINT8   g_palEtuParaD[];
        extern UINT8  g_palEtuClkInvFlag[];
        extern HAL_SIM_CLOCK_STOP_MODE_T g_palSimClkStopMode[] ;
        extern UINT8 g_TS[];

        while( (gc_halSimHwpArray[g_halSimIndex]->Status & SCI_AUTOSTOP_RXCOUNTER_MASK) != 0);

        // Set resource as active
        // Rq: The setting of the divider is handled by the frequency
        // scaling mechanism.

        UINT32 cri_status = hal_SysEnterCriticalSection();

        pmd_SelectSimCard( (g_SimIndex +2) & 3);

        hal_SimResetSys();

        hal_SimOpen();

        hal_SimSetEtuDuration(g_palEtuParaF[g_SimIndex], g_palEtuParaD[g_SimIndex],  g_palEtuClkInvFlag[g_SimIndex]);
        hal_SimSetClockStopMode(g_palSimClkStopMode[g_SimIndex]);

        hal_SimSetDataConvFormat(g_TS[g_SimIndex]);

        hal_HstSendEvent(gc_halSimHwpArray[g_halSimIndex]->Status & SCI_AUTOSTOP_RXCOUNTER_MASK);

        pmd_SelectSimCard(g_SimIndex);

        hal_SysExitCriticalSection(cri_status);
    }
#endif
#endif

    if (nTxByte <= N_TX_CMD_BYTES)
    {
        if (nRxByte <= 4)
        {
            HAL_PROFILE_FUNCTION_ENTER(hal_SimTxByteRxByte_nRxBytebl4);
            g_halRxDmaInProgress = 0;
            g_halIfcChan = hal_SimSetNRxBytes(nRxByte, nRxByte, pRxByte, FALSE, stopClock);
            HAL_PROFILE_FUNCTION_EXIT(hal_SimTxByteRxByte_nRxBytebl4);
        }
        else
        {
            HAL_PROFILE_FUNCTION_ENTER(hal_SimTxByteRxByte_nRxByteab4);
            // Hardware is not supposed to be able to cope with such bigs xfers ...
            // MAX_RX_BYTES being big enough to handle all requets ...
            HAL_ASSERT(nRxByte<=MAX_RX_BYTES,"Rx size too big: %d", nRxByte);

            g_halRxDmaInProgress = 1;
            g_halIfcChan = hal_SimSetNRxBytes(nRxByte, nRxByte, pRxByte, TRUE, stopClock);
            HAL_PROFILE_FUNCTION_EXIT(hal_SimTxByteRxByte_nRxByteab4);
        }

        // Don't use DMA for Tx
        for (txCnt = 0; txCnt < nTxByte; ++txCnt)
        {
            hal_SimTxByte(pTxByte[txCnt]);
        }
    }
    else
    {
        g_halRxDmaInProgress = 0;
        hal_SimSetNRxBytes(nRxByte, nRxByte, pRxByte, FALSE, stopClock);
        g_halIfcChan = hal_SimSendNTxBytes(nTxByte, pTxByte);
    }
    g_halRxBuf = pRxByte;

#ifdef GALLITE_IS_8806
    g_ResetIndex = 1;
#endif
    HAL_PROFILE_FUNCTION_EXIT(hal_SimTxByteRxByte);
}


// ============================================================================
// hal_SimSetWorkWaitingTime
// ----------------------------------------------------------------------------
/// Set the work waiting time.
/// A timeout will be generated when the WWT is exceeded.  The WWT is calculated
/// by:
/// <code> WWT = 960 x WI x (F/Fi) </code>,
/// where Fi is the main SCI clock frequency (3-5 MHz) and F is 372 before an
/// enhanced PPS and 512 after an enhanced PPS. \n
/// The SCI_WI value must be calculated as follows:
/// <code> SCI_WI = WI * D </code>
/// Thus, by default (WI = 10) this value needs to be set to 10 before an EPPS,
/// but needs to be scaled to WI*D=80 after the EPPS procedure.
///
/// The hal driver is in charge of checking the time between each received sim
/// character. The default value (10) is not initialised by HAL.
///
/// @param wi WI factor used to set the WWT according to the previous formula.
// ============================================================================
PUBLIC VOID hal_SimSetWorkWaitingTime(UINT8 wi)
{
    hal_SimSetWwi(wi);
}


// ============================================================================
// hal_SimSetEtuDuration
// ----------------------------------------------------------------------------
/// Select the clock rate.
/// @param etuDuration F param to set.
/// @param etuDuration D param to set.
// ============================================================================
PUBLIC VOID hal_SimSetEtuDuration(UINT16 F, UINT8 D, UINT8 SciInvFlag)
{
    if (F == 372)
    {
        if(D == 8)
            hal_SimSetWwi(80);
        else if(D == 12)
            hal_SimSetWwi(120);
        else if(D == 16)
            hal_SimSetWwi(160);
        else
            hal_SimSetWwi(10);
    }
    else
    {
        if(D == 12)
            hal_SimSetWwi(120);  // the register only 8 bit
        else if(D == 16)
            hal_SimSetWwi(160);
        else
            hal_SimSetWwi(80);
    }

    hal_SimSetEtuBitfield(F, D, SciInvFlag);
}


// ============================================================================
// hal_SimDelayAfterEpps
// ----------------------------------------------------------------------------
/// Delay after EPPS procedure required for some SIM cards
// ============================================================================
PUBLIC VOID hal_SimDelayAfterEpps(VOID)
{
    // Wait randomly for about 100ms, that is 1640 cycles of the 16kHz delay clock.
    hal_TimDelay(HAL_SIM_ETU_WAIT MS_WAITING);
}


// ============================================================================
// hal_SimGetNumRxBytes
// ----------------------------------------------------------------------------
/// That function lets us know the number of bytes received from the SIM.
/// @return The number of bytes read from the SIM
// ============================================================================
// To know the number of bytes received by
PUBLIC UINT16 hal_SimGetNumRxBytes(VOID)
{
    return g_halRxBytes;
}


// ============================================================================
// hal_SimClockStart
// ----------------------------------------------------------------------------
/// Start the clock sent to the SIM card.
/// This function should not be necessary anymore since the
/// automatic clock stop feature should be useable.  However, this function
/// would be used when the SIM clock configuration is in _MANUAL OFF_
/// mode and will go to _AUTOMATIC ON_ mode via the _AUTO STARTUP_ state,
/// meaning that the startup timing will be respected.  This function will
/// BLOCK during the startup.  Maybe in the future, a parameter can be added
/// to the interface to select blocking or non-blocking mode.
// ============================================================================
PUBLIC VOID hal_SimClockStart(VOID)
{
    // Set resource as active
    if (!hal_ClkIsEnabled(gSimClk))
        hal_ClkEnable(gSimClk);

    // Configure clock mode as AUTO OFF
    // Manual Clock Enable must be _OFF_ while auto mode is used
    hal_SimSetConfigBit(SCI_CLOCKSTOP);
    hal_SimSetConfigBit(SCI_AUTOSTOP_EN_H);

    // Use the manual start pulse to start the clock
    gc_halSimHwpArray[g_halSimIndex]->dbg = 0x00010000;

    // Wait here until the SCI clock is valid
    while ((gc_halSimHwpArray[g_halSimIndex]->Status & SCI_CLK_RDY_H) == 0);
}


// -------------------------------------------------------------------------//
// ----------------------  Irq handling related things  --------------------//
// -------------------------------------------------------------------------//


// ============================================================================
// hal_SimIrqSetHandler
// ----------------------------------------------------------------------------
/// Set the user function called when an interrupt is generated by the SIM
/// driver.
/// @param handler Handler function to set.
// ============================================================================
// set the user function to call when a fint occurs
PUBLIC VOID hal_SimIrqSetHandler(HAL_SIM_IRQ_HANDLER_T handler)
{
    g_halSimRegistry = handler;
}


// ============================================================================
// hal_SimIrqSetMask
// ----------------------------------------------------------------------------
/// Set the mask for the interruption. A '1' in a field of the structure means
/// an interruption must be triggered in the case if the corresponding
/// condition occurs (cf the type details for more information).
///
/// @param mask Mask to set.
// ============================================================================
PUBLIC VOID hal_SimIrqSetMask(HAL_SIM_IRQ_STATUS_T mask)
{
    g_halSimUserMask = mask;
}


// ============================================================================
// hal_SimIrqGetMask
// ----------------------------------------------------------------------------
/// Get the IRQ mask for the SIM driver.
/// @return The Sim mask
// ============================================================================
PUBLIC HAL_SIM_IRQ_STATUS_T hal_SimIrqGetMask(VOID)
{
    return (g_halSimUserMask);
}

// ============================================================================
// hal_SimDMAReleaseIFC
// ----------------------------------------------------------------------------
/// release IFC when dma in progress
// ============================================================================
PUBLIC VOID hal_SimDMAReleaseIFC(VOID)
{
    if (g_halRxDmaInProgress == 1)
    {
        hal_SimReleaseIFC();
    }

    gc_halSimHwpArray[g_halSimIndex]->Int_Clr = 1;
}

#if 0
// ============================================================================
// hal_Switch2Manual
// ----------------------------------------------------------------------------
/// This function should be called after a successful ATR ONLY when using the
/// ARG.  After an ARG, the LLI and MSBH_LSBL control lines are internally
/// driven by the ARG state machine.  When in Dual SIM mode, the state machine
/// is restarted so the internal signals can no longer be used.  This function
/// parses the Ts value and switches to manual control for the format params.
/// This should only be necessary for Dual SIM drivers.
/// @param Ts is the Ts byte that is read and which will be used to determine
/// the transfer format.
// ============================================================================
PUBLIC UINT32 hal_SimSwitch2Manual(UINT8 Ts)
{
    // Need to switch the format detection to manual mode since the state will be lost
    // when switching to the second SIM
    if (Ts == 0x23)
    {
        hal_SimClrConfigBit(SCI_LLI | SCI_MSBH_LSBL);
    }
    else
    {
        hal_SimSetConfigBit(SCI_LLI | SCI_MSBH_LSBL);
    }
    hal_SimSetConfigBit(SCI_ARG_H | SCI_AFD_EN_H);
    return (UINT32)gc_halSimHwpArray[g_halSimIndex]->Config;
}



// FIXME: The three following functions should not be useful.
// ============================================================================
// hal_SimSetConfig
// ----------------------------------------------------------------------------
/// Set the config for the SIM driver.
/// @return The Sim config
// ============================================================================

PUBLIC VOID hal_SimSetConfig(VOID)
{
    // Clear all but upper bits
    hal_SimClrConfigBit(SCI_ENABLE |
                        SCI_PARITY_ODD_PARITY |
                        SCI_PERF |
                        SCI_FILTER_DISABLE |
                        SCI_CLOCKSTOP |
                        SCI_AUTOSTOP_EN_H |
                        SCI_MSBH_LSBL |
                        SCI_LLI |
                        SCI_PEGEN_LEN |
                        SCI_PARITY_EN |
                        SCI_STOP_LEVEL);

    hal_SimSetConfigBit(SCI_TX_RESEND_EN_H |SCI_PEGEN_LEN |SCI_ENABLE|SCI_RESET|SCI_AUTOSTOP_EN_H | SCI_CLOCKSTOP);
}

// ============================================================================
// hal_SimSetConfig
// ----------------------------------------------------------------------------
/// Set the config for the SIM driver.
/// @return The Sim config
// ============================================================================

PUBLIC VOID hal_SimSetConfInvTS(VOID)
{
    // Clear all but upper bits
    hal_SimClrConfigBit(SCI_ENABLE |
                        SCI_PARITY_ODD_PARITY |
                        SCI_PERF |
                        SCI_FILTER_DISABLE |
                        SCI_CLOCKSTOP |
                        SCI_AUTOSTOP_EN_H |
                        SCI_MSBH_LSBL |
                        SCI_LLI |
                        SCI_PEGEN_LEN |
                        SCI_PARITY_EN |
                        SCI_STOP_LEVEL);

    hal_SimSetConfigBit(SCI_TX_RESEND_EN_H |SCI_PEGEN_LEN |SCI_ENABLE|SCI_LLI |SCI_MSBH_LSBL|
                        SCI_RESET|SCI_AUTOSTOP_EN_H | SCI_CLOCKSTOP);
}
#endif

// ============================================================================
// hal_SimIrqHandler
// ----------------------------------------------------------------------------
/// Sim module IRQ handler, to be used exclusively by the private internal
/// IRQ module.
/// @param interruptId IRQ line causing the IRQ Id
// ============================================================================
PROTECTED VOID hal_SimIrqHandler(UINT8 interruptId)
{
    union
    {
        UINT32 cause;
        HAL_SIM_IRQ_INTERNAL_CAUSE_T formatedCause;
    } u;

    HAL_SIM_IRQ_STATUS_T usrCause = {0,0,0,0,0,0,0,0};

    UINT32 idx=0;

#ifdef CHIP_HAS_AP
    HAL_ASSERT(g_halSimIndex == (interruptId - SYS_IRQ_SCI1),
               "HAL_SIM: Invalid SIM interrupt. SimCur=%d, SimInt=%d",
               g_halSimIndex, (interruptId - SYS_IRQ_SCI1));
#endif

    HAL_TRACE(HAL_SIM_TRC|TNB_ARG(3), 0, "HAL_SIM[%d]: IRQ cause=0x%08x status=0x%08x",
              g_halSimIndex,
              gc_halSimHwpArray[g_halSimIndex]->Int_Cause,
              gc_halSimHwpArray[g_halSimIndex]->Status);
    //  get cause
    u.cause = gc_halSimHwpArray[g_halSimIndex]->Int_Cause;
    //  to clear irq
    gc_halSimHwpArray[g_halSimIndex]->Int_Clr = u.cause;

    // "Mecanical treatment needed before calling
    // the user handler

    // Status is the _interrupt_ status
    if (u.formatedCause.argEnd)
    {
        HAL_PROFILE_FUNCTION_ENTER(hal_SimIrqHandler_argEnd);
        if (hal_SimGetArgStatus())
        {
            //HAL_TRACE(HAL_SIM_TRC | TSTDOUT, 0, "SIM:  argEnd Receiving ATR");
            g_halReceivingAtr = 1;
        }
        else
        {
            hal_SimReleaseIFC();
            //sim_ISR(HAL_SIM_RESET_TIMEOUT, 0);
            if (g_halSimUserMask.resetTimeout)
            {
                usrCause.resetTimeout = 1;
                HAL_TRACE(HAL_SIM_TRC | TSTDOUT, 0, "HAL_SIM: argEnd Timeout");
            }
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_SimIrqHandler_argEnd)    ;
    }

    if (u.formatedCause.rxDone)
    {
        HAL_PROFILE_FUNCTION_ENTER(hal_SimIrqHandler_rxDone);
        g_halRxBytes = hal_SimGetRxCnt();

        if (g_halReceivingAtr == 1)
        {
            // Need to update the XCPU cache after DMA
            hal_SysInvalidateCache(g_halRxBuf, g_halRxBytes);

            if (g_halSimUserMask.atrSuccess)
            {
                usrCause.atrSuccess = 1;
            }
            hal_SimEnableParity(TRUE);
            g_halReceivingAtr = 0;
        }
        else
        {
            if (g_halRxDmaInProgress == 0)
            {
                // Need to get the data from SCI to Rx Pointer
                for (idx = 0; idx < g_halRxBytes; ++idx)
                {
                    g_halRxBuf[idx] = hal_SimGetRxByte();
                }
            }
            else
            {
                // Need to update the XCPU cache after DMA
                hal_SysInvalidateCache(g_halRxBuf, g_halRxBytes);
            }
            g_halRxDmaInProgress = 0;
            //sim_ISR(HAL_SIM_DMA_SUCCESS, g_halRxBytes);
            if (g_halSimUserMask.dmaSuccess)
            {
                usrCause.dmaSuccess = 1;
            }
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_SimIrqHandler_rxDone);
    }

    if (u.formatedCause.wwtTimeout)
    {
        // Flush the IFC here!!!
        HAL_PROFILE_FUNCTION_ENTER(hal_SimIrqHandler_wwtTimeout);
        if (g_halRxDmaInProgress == 1)
        {
            hal_SimReleaseIFC();
        }
        g_halRxBytes = hal_SimGetRxCnt();
        // Previous statement incorrect if multi-transfer
        // but no multitransfert here as tc is big enough
        if (g_halReceivingAtr == 1)
        {
            // Need to update the XCPU cache after DMA
            hal_SysInvalidateCache(g_halRxBuf, g_halRxBytes);

            if (g_halSimUserMask.atrSuccess)
            {
                //while (1);
                usrCause.atrSuccess = 1;
            }
            hal_SimEnableParity(TRUE);
            g_halReceivingAtr = 0;
        }
        else
        {
            if (g_halRxDmaInProgress == 0)
            {
                for (idx = 0; idx < g_halRxBytes; ++idx)
                {
                    g_halRxBuf[idx] = hal_SimGetRxByte();
                }
            }
            else
            {
                // Need to update the XCPU cache after DMA
                hal_SysInvalidateCache(g_halRxBuf, g_halRxBytes);
            }
            g_halRxDmaInProgress = 0;
            usrCause.wwtTimeout = 1;
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_SimIrqHandler_wwtTimeout);
    }

    if (u.formatedCause.resendOvfl)
    {
        HAL_PROFILE_FUNCTION_ENTER(hal_SimIrqHandler_resendOvfl);
        hal_SimClrConfigBit(SCI_TX_RESEND_EN_H);

        if (g_halRxDmaInProgress == 1)
        {
            hal_SimReleaseIFC();
        }

        if (g_halSimUserMask.parityError)
        {
            usrCause.parityError = 1;
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_SimIrqHandler_resendOvfl);
    }

    if (u.formatedCause.extraRx)
    {
        HAL_PROFILE_FUNCTION_ENTER(hal_SimIrqHandler_extraRx);
#ifdef GALLITE_IS_8806
        if(g_halRxBuf==0)
        {
            hal_SimGetRxByte();
        }
        else
#endif
        {
            g_halRxBuf[g_halRxBytes] = hal_SimGetRxByte();
            ++g_halRxBytes;
        }

        if (g_halRxDmaInProgress == 1)
        {
            hal_SimReleaseIFC();
        }
        else if (g_halSimUserMask.extraRx)
        {
            usrCause.extraRx = 1;
        }
        HAL_PROFILE_FUNCTION_EXIT(hal_SimIrqHandler_extraRx);
    }

    if (g_halSimRegistry)
        g_halSimRegistry(usrCause);
}


#if 0
// That is what the stack handler will have to do ...
// "Mecanical treatment needed before calling
// the user handler

// Status is the _interrupt_ status
if (formatedCause.argEnd)
{
    if (hal_SimGetArgStatus())
    {
        g_halReceivingAtr = 1;
    }
    else
    {
        hal_SimReleaseIFC();
        //sim_ISR(SIM_RESET_TIMEOUT, 0);
        usrCause = SIM_RESET_TIMEOUT;
    }
}
if (formatedCause.rxDone)
{
    // CT_sci_ReleaseIFC();
    g_halRxBytes = hal_SimGetRxCnt();
    if (g_halRxDmaInProgress == 0)
    {
        // Need to get the data from SCI to Rx Pointer
        for (idx = 0; idx < g_halRxBytes; ++idx)
        {
            g_halRxBuf[idx] = hal_SimGetRxByte();
        }
    }
    else
    {
        // Need to update the XCPU cache after DMA
        ///  @todo Change this with the invalidate
///             cache instruction
        for (idx = 0; idx < g_halRxBytes; ++idx)
        {
            g_halRxBuf[idx] = *(UINT8*)KSEG1((UINT32)g_halRxBuf + idx);
        }
    }
    g_halRxDmaInProgress = 0;
    //sim_ISR(SIM_DMA_SUCCESS, g_halRxBytes);
    usrCause = SIM_DMA_SUCCESS;
}


if (formatedCause.wwtTimeout)
{
    // Flush the IFC here!!!
    if (g_halRxDmaInProgress == 1)
    {
        hal_SimReleaseIFC();
    }
    g_halRxBytes = hal_SimGetRxCnt(); /// @todo incorrect if multi-transfer
    if (g_halReceivingAtr == 1)
    {
        for (idx = 0; idx < g_halRxBytes; ++idx)
        {
            g_halRxBuf[idx] = *(UINT8*)KSEG1((UINT32)g_halRxBuf + idx);
        }
        usrCause = SIM_ATR_SUCCESS;
        hal_SimEnableParity(TRUE);
        g_halReceivingAtr = 0;
    }
    else
    {
        if (g_halRxDmaInProgress == 0)
        {
            for (idx = 0; idx < g_halRxBytes; ++idx)
            {
                g_halRxBuf[idx] = hal_SimGetRxByte();
            }
        }
        else
        {
            for (idx = 0; idx < g_halRxBytes; ++idx)
            {
                g_halRxBuf[idx] = *(UINT8*)KSEG1((UINT32)g_halRxBuf + idx);
            }
        }
        g_halRxDmaInProgress = 0;
        usrCause = SIM_WWT_TIMEOUT;
    }
}
if (formatedCause.resendOvfl)
{
    hal_SimClrConfigBit(SCI_TX_RESEND_EN_H);
    usrCause = SIM_PARITY_ERROR;
}

if (formatedCause.extraRxX)
{
    g_halRxBuf[g_halRxBytes] = hal_SimGetRxByte();
    ++g_halRxBytes;
    usrCause = SIM_EXTRA_RX;
}
#endif

HAL_MODULE_CLK_INIT(SIM)
{
    gSimClk = hal_ClkGet(FOURCC_SIM);
    HAL_ASSERT((gSimClk!= NULL), "Clk SIM not found!");
}
