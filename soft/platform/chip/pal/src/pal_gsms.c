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
#include "string.h"
#include "gsm.h"
#include "global_macros.h"
// HAL includes
#include "hal_rfspi.h"
#include "hal_tcu.h"
#include "hal_mem_map.h"
#include "hal_uart.h"
#include "hal_lps.h"
#include "sys_ctrl.h"

//#include "hal_sim.h"
#include "hal_cipher.h"
#include "hal_dma.h"
#include "hal_comregs.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_fint.h"
#include "sxs_io.h"
#include "sxr_tls.h"

// PAL includes
#include "pal_gsm.h"
#include "pal.h"
#include "pal_internal.h"
#include "hal_debugpaltrc.h"
#include "palp_cfg.h"

// RFD include
#include "rfd_xcv.h"
#include "rfd_pa.h"
#include "rfd_sw.h"
#include "tgt_rfd_cfg.h"

// SPC include
#include "spc_ctx.h" // TODO: only needed for info printout...
#include "spc_mailbox.h"

// Calibration
#include "calib_m.h"

// Power Management
#include "pmd_m.h"

// ASSERT Macros. File specific.
#define PAL_INTERNAL_CHECK(BOOL, param)                 \
  PAL_ASSERT(BOOL, "palInternalCheck: PAL and SPAPI not synched... \n Param = %d", param)

// FIXME this shouldn't exist
#undef pal_SetUsrVector
#undef pal_SetUsrVectorMsk


extern VOID pal_EnableAuxClkCtrl(BOOL on);
extern VOID pal_AuxClkOff(VOID);
#ifdef GCF_TEST
extern UINT8  g_mailbox_pal2spc_statWin_rx_Usf[4];
#endif
#ifdef CHIP_DIE_8955
extern UINT8 g_mailbox_pal2spc_RxBufInd;
#endif
//--------------------------------------------------------------------//
//                                                                    //
//                       PAL Internal Structures                      //
//                                                                    //
//--------------------------------------------------------------------//

// TOf/FOf adjustement threshold, exported in pal_gsm.h
UINT8 pal_low_rla_var = 103;

// TOf/FOf adjustement threshold, exported in pal_gsm.h
UINT8 pal_snr_factor_var = 30;//39//48


UINT16 pal_pll_lock_time = 308;


// FIXME: Please add a comment here.
PROTECTED CALIB_CALIBRATION_T * g_palCalibration = NULL;

//--------------------------------------------------------------------//
//                                                                    //
//                         PAL API functions                          //
//                                                                    //
//--------------------------------------------------------------------//

// Global variables to store external handlers

PAL_HANDLERS_T g_palHandlers = {NULL, NULL, NULL};

UINT8 g_Calib_OnSite = 0;
static UINT8  g_TriBandSearchSuppor  = 0;

//======================================================================
// pal_Open
//----------------------------------------------------------------------
/// Low level initialization. Called early by hal_Open.
//======================================================================
VOID pal_Open (VOID)
{
    HAL_LPS_CONFIG_T lpsCfg;
    union
    {
        UINT32 reg;
        HAL_LPS_POWERUP_CTRL_T bitfield;
    } lpsPuCtrl;
    HAL_LPS_POWERUP_TIMINGS_T lpsPuTimings;

    // Set aux clock status handler
    pmd_SetAuxClockStatusHandler(pal_EnableAuxClkCtrl);

    /******************/
    /* HW level Inits */
    /******************/

    // Set the PAL calibration pointer
    g_palCalibration = calib_GetPointers();

    // Open all RF devices
    // TODO rfd_XcvOpen should take AfcOffset in parameter
    //      for aging comp. -> we need ffs access for that
#ifdef LTE_NBIOT_SUPPORT
    rfd_nXcvOpen(0, tgt_GetXcvConfig()); // Also Handles RFSPI setup
#else
    rfd_XcvOpen(0, tgt_GetXcvConfig()); // Also Handles RFSPI setup
#endif
    rfd_PaOpen(tgt_GetPaConfig());
    rfd_SwOpen(tgt_GetSwConfig());

    // Get pointer to XCV and PA specific parameters
    g_palCtx.xcv = rfd_XcvGetParameters();
    g_palCtx.pa  = rfd_PaGetParameters();
    g_palCtx.sw  = rfd_SwGetParameters();

    // Get max setup and hold times of windows, taking into account
    // RF parts and internal analog (if applicable)
    // Take care of the Adc to dBm conversion gain.
    if (g_palCtx.xcv->digRf.used) // no analog in case of DigRf
    {
        g_palCtx.winBounds.rxSetup = 0;
        g_palCtx.winBounds.rxHold = 0;
        g_palCtx.winBounds.txSetup = 0;
        g_palCtx.winBounds.txHold = 0;
        g_palCtx.Adc2dBmGain = g_palCtx.xcv->digRf.rxAdc2DbmGain;
    }
    else // init with analog timings
    {
        g_palCtx.winBounds.rxSetup = PAL_TIME(PAL_TIME_ADC_UP);
        g_palCtx.winBounds.rxHold = (PAL_TIME(PAL_TIME_ADC_DN) >
                                     PAL_TIME(PAL_TIME_RF_IN_DN)) ?
                                    PAL_TIME(PAL_TIME_ADC_DN)  :
                                    PAL_TIME(PAL_TIME_RF_IN_DN);
        g_palCtx.winBounds.txSetup = PAL_TIME(PAL_TIME_DAC_UP);
        g_palCtx.winBounds.txHold = PAL_TIME(PAL_TIME_DAC_DN);
        g_palCtx.Adc2dBmGain = PAL_IQ_ADC2DBM_GAIN;
    }

    if (g_palCtx.xcv->winBounds.rxSetup < g_palCtx.winBounds.rxSetup)
        g_palCtx.winBounds.rxSetup = g_palCtx.xcv->winBounds.rxSetup;
    if (g_palCtx.sw->winBounds.rxSetup < g_palCtx.winBounds.rxSetup)
        g_palCtx.winBounds.rxSetup = g_palCtx.sw->winBounds.rxSetup;


    if (g_palCtx.xcv->winBounds.rxHold > g_palCtx.winBounds.rxHold)
        g_palCtx.winBounds.rxHold = g_palCtx.xcv->winBounds.rxHold;
    if (g_palCtx.sw->winBounds.rxHold > g_palCtx.winBounds.rxHold)
        g_palCtx.winBounds.rxHold = g_palCtx.sw->winBounds.rxHold;

    if (g_palCtx.xcv->winBounds.txSetup < g_palCtx.winBounds.txSetup)
        g_palCtx.winBounds.txSetup = g_palCtx.xcv->winBounds.txSetup;
    if (g_palCtx.sw->winBounds.txSetup < g_palCtx.winBounds.txSetup)
        g_palCtx.winBounds.txSetup = g_palCtx.sw->winBounds.txSetup;
    if (g_palCtx.pa->winBounds.txSetup < g_palCtx.winBounds.txSetup)
        g_palCtx.winBounds.txSetup = g_palCtx.pa->winBounds.txSetup;


    if (g_palCtx.xcv->winBounds.txHold > g_palCtx.winBounds.txHold)
        g_palCtx.winBounds.txHold = g_palCtx.xcv->winBounds.txHold;
    if (g_palCtx.sw->winBounds.txHold > g_palCtx.winBounds.txHold)
        g_palCtx.winBounds.txHold = g_palCtx.sw->winBounds.txHold;
    if (g_palCtx.pa->winBounds.txHold > g_palCtx.winBounds.txHold)
        g_palCtx.winBounds.txHold = g_palCtx.pa->winBounds.txHold;

    // TCU  & LPS Inits
    hal_TcuOpen(0);
    hal_TcuSetWrap(PAL_TCU_DEFAULT_WRAP_COUNT);

    // TODO : pass all xcv's PU parameters -> add missing params to RFD API
    lpsPuTimings.puCoState1ToCoState2 = 0;
#ifndef LTE_NBIOT_SUPPORT
    lpsPuTimings.puCoState2ToRfClkEnState = g_palCtx.xcv->pu2VcoEnDelay;
    lpsPuTimings.puRfClkEnStateToRfClkReadyState = g_palCtx.xcv->vcoEn2VcoStableDelay;
#else
    lpsPuTimings.puCoState2ToRfClkEnState = 0; //g_palCtx.xcv->pu2VcoEnDelay;
    lpsPuTimings.puRfClkEnStateToRfClkReadyState = 0; //g_palCtx.xcv->vcoEn2VcoStableDelay;
#endif
#if (CHIP_ASIC_ID == CHIP_ASIC_ID_GALLITE)
    lpsPuTimings.puRfClkReadyStateToPllClockReadyState = 1;
#else // 8808 or later
    lpsPuTimings.puRfClkReadyStateToPllClockReadyState = 4;
#endif
#if defined(USE_DIG_CLK_ONLY_AT_IDLE) || defined(CHIP_XTAL_CLK_IS_52M)
    lpsPuTimings.puWaitPllLock = 0;
#else
    lpsPuTimings.puWaitPllLock = 1;
#endif

    // LPSCO0 power down state and polarity
    // TODO move this to PMD as Opal is not using LPSCO0 for VRF
#ifndef LTE_NBIOT_SUPPORT
    switch (g_palCtx.xcv->vrfBehaviour)
#else
    switch (RFD_LDO_BEHAVIOR_DONT_USE)
#endif
    {
        case RFD_LDO_BEHAVIOR_NORMAL:
            lpsPuCtrl.reg = pmd_ConfigureRfLowPower(TRUE, FALSE);
            break;
        case RFD_LDO_BEHAVIOR_ALWAYS_HIGH:
            lpsPuCtrl.reg = pmd_ConfigureRfLowPower(TRUE, TRUE);
            break;
        case RFD_LDO_BEHAVIOR_DONT_USE:
            lpsPuCtrl.reg = pmd_ConfigureRfLowPower(FALSE, FALSE);
            break;
        default: // normal, could assert ...
            lpsPuCtrl.reg = pmd_ConfigureRfLowPower(TRUE, FALSE);
    }
    // default power down state
    lpsPuCtrl.bitfield.puClkRfEnForceOn = 0;
    lpsPuCtrl.bitfield.puSpiForceOn = 0;
    // default polarity
    lpsPuCtrl.bitfield.puClkRfEnPol = 0;
    // power up actions
    lpsPuCtrl.bitfield.puSpiOnState = 0;
    lpsPuCtrl.bitfield.puCoActivationMaskInCoState1 = 1;
    lpsPuCtrl.bitfield.puCoActivationMaskInCoState2 = 3;
    lpsPuCtrl.bitfield.puMaskRfClkEn = 1;
#ifdef CHIP_XTAL_CLK_IS_52M
    lpsPuCtrl.bitfield.puMaskPllEn = 0;
    lpsPuCtrl.bitfield.puMaskSwitchToPllClk = 0;  
#else
    lpsPuCtrl.bitfield.puMaskPllEn = 1;
    lpsPuCtrl.bitfield.puMaskSwitchToPllClk = 1;    
#endif
    lpsPuCtrl.bitfield.puMaskSwitchToRfClk = 1;

    lpsPuCtrl.bitfield.puEnableTcuWakeup = 0;

    // PAL should only set this
#ifndef LTE_NBIOT_SUPPORT
    lpsCfg.lpsWakeUpFrameNbr = g_palCtx.xcv->wakeUpFrameNbr;
#else
    lpsCfg.lpsWakeUpFrameNbr = 0;//g_palCtx.xcv->wakeUpFrameNbr;
#endif
    lpsCfg.puTimings = lpsPuTimings;
    lpsCfg.puCtrl = lpsPuCtrl.bitfield;
#ifndef LTE_NBIOT_SUPPORT
    lpsCfg.lpsAccuracy = 16; // Max accuracy
#else
    lpsCfg.lpsAccuracy = 12; // Max accuracy
#endif
    hal_LpsOpen(&lpsCfg);
}


// *** pal_SetUsrVector ***
VOID
pal_SetUsrVector (UINT8 interruptId, VOID (*handler) (UINT32 status))
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetUsrVector);

    // Dispatcher
    switch (interruptId)
    {
        case PAL_DSP_IRQ :
        {
            g_palHandlers.dspHandler=handler;
            hal_ComregsSetIrqHandler(HAL_COMREGS_XCPU_IRQ_SLOT_0, pal_DspIrqHandler);
            break;
        }
        case PAL_FINT_IRQ:
        {
            g_palHandlers.fintHandler=handler;
            hal_FintIrqSetHandler(pal_FintIrqHandler);
            break;
        }

        case PAL_SIM_IRQ:
        {
            g_palHandlers.simHandler=handler;
            // For the IRQ handler, bypass simd and call HAL directly.
            hal_SimIrqSetHandler(pal_SimIrqHandler);
            break;

        }
        // add a case for LPS
        default         :
        {
            //assert ?
            break;
        }

    }

    palDbgTrc((HAL_DBGPALTRC_SETUSRVECT,interruptId,(UINT32)handler,0,0,0,0));
    PAL_TRACE(PAL_LEV(1),0, "IT Vector:%d / 0x%08x",interruptId,handler);
    PAL_PROFILE_FUNCTION_EXIT(pal_SetUsrVector);
}

// *** pal_SetUsrVectorMsk ***
VOID
pal_SetUsrVectorMsk (UINT8 interruptId, UINT32 mask)
{
    HAL_SIM_IRQ_STATUS_T  simMask = {mask};
    HAL_COMREGS_IRQ_MASK_T comMask = {mask};

    PAL_PROFILE_FUNCTION_ENTRY(pal_SetUsrVectorMsk);

//    UINT16 transItrMsk = 0;
    switch (interruptId)
    {
        case PAL_DSP_IRQ :
        {
            hal_ComregsSetMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, comMask);
            break;
        }
        case PAL_FINT_IRQ:
        {
            if (0 == mask)
            {
                hal_FintIrqSetMask(FALSE);
                hal_TcuStop();
                // Reset RFSPI FIFO
                hal_RfspiSetupFifo();
            }
            else
            {
                hal_FintIrqSetMask(TRUE);
                // TCU Start
                hal_TcuStart (0);
            }
            break;
        }
        case PAL_SIM_IRQ:
            hal_SimIrqSetMask(simMask);
            break;
        default:
        {
            //assert ?
            break;
        }
    }
    PAL_PROFILE_FUNCTION_EXIT(pal_SetUsrVectorMsk);
}


// *** pal_Initialisation ***
VOID
pal_Initialisation (PAL_INIT_T InitType, UINT8 BandMap)
{
    BOOL total;

    PAL_PROFILE_FUNCTION_ENTRY(pal_Initialisation);

    total = (InitType==PAL_INIT_GLOBAL)?TRUE:FALSE;
    // A Full init is needed the first time
    PAL_ASSERT(!(g_palDebug.FirstInit && (!total)),
               "pal_Initialisation: A Full init is needed. Total=%d", total);

    // FULL PAL reset, including HW.
    // Should be called only once at start time
    if (total)
    {
        PAL_TRACE(PAL_LEV(1), 0, "PAL First init.");
        palDbgTrc((HAL_DBGPALTRC_PALINIT,0,0,0,0,0,0));

        PAL_ASSERT(g_palDebug.FirstInit,
                   "pal_Initialisation: only one total init supported (%d totals)", total);

#ifdef LTE_NBIOT_SUPPORT
        rfd_nXcvRfInit();
#else
        rfd_XcvRfInit();
#endif

        // DEBUG checks and settings
        // -------------------------

        // HW and driver stuff that should be set only once
        palHwInit();
        // Print Calibration info
        calib_DaemonTracePrint();

        // Start BCPU
#ifdef FPGA_NBCPU
        palStartNBcpu ();
#else
        palStartBcpu ();
#endif

        g_palDebug.FirstInit= FALSE;
    }

    // Ensure RF is enabled: if pal_FintSuspend(PAL_FINT_SUSPEND_STOP)
    // was called then a pal_FintResume is needed.
    PAL_ASSERT(g_palDebug.RFEnabled, "pal_Initialisation: RF Should be enabled now...");

    // TCU event reset
    // As this can be called in the middle of a window, don't reset
    // the active part of the TCU events, but only the latch
    hal_TcuClrEvtRange(0,PAL_MAX_TCU_EVENTS);
    hal_TcuSetEvtPtr(0);

    // Inform XCV that FINT is initialized
    rfd_XcvFrameInit();

    // Flush RF SPI command HW-FIFO and setup SW Fifo
    switch(InitType)
    {
        case PAL_INIT_GLOBAL:
            // Reset RFSPI FIFO
            hal_RfspiSetupFifo();
            // All the TCU events are cleared too
            PAL_TRACE(PAL_LEV(8), 0, "PAL INIT GLOBAL.");
            break;
        case PAL_INIT_ASYNCHRO:
        case PAL_INIT_ASYNCHRO_WITH_RESETXCV:
        case PAL_INIT_ASYNCHRO_WITH_ONSITE_CALIB:
            // check if a FINT is pending to clear the active part
            // of the TCU also
            if (hal_FintIrqGetStatus())
            {
                hal_TcuClrAllEvents();
            }

            // postpone the Rfspi FiFo flush to the next Frame Start
            g_palDebug.RfspiFlushFifo = TRUE;
            if(InitType == PAL_INIT_ASYNCHRO_WITH_ONSITE_CALIB)
            {
                g_Calib_OnSite = 1;
                PAL_TRACE(PAL_LEV(8), 0, "PAL INIT For OnSite Calib.");
            }
            else
            {
                PAL_TRACE(PAL_LEV(8), 0, "PAL INIT ASYNCHRONE.");
            }
            break;
        case PAL_INIT_SYNCHRO:
            // do nothing since there should be no TCU event left
            PAL_TRACE(PAL_LEV(8), 0, "PAL INIT SYNCHRONE.");
            break;
        default:
            break;
    }

    // Init Contexts, Reset mailbox IRQs etc...
    palCommonInit ();

    // Disable Ciphering
    pal_StopCiphering ();

    // Reset TCH mode
    pal_StopTch ();

    // Open loops
    pal_SetTchLoop (PAL_OPEN_LOOP);

    // Disable DTX
    pal_SetDtx (FALSE);

    //disable tuning
    pal_TimeTuningSuspend(FALSE, 0);
    // Flag the selected "high band" (priority to PCS)
    g_palCtx.PcsH_DcsL = (BandMap & PAL_PCS1900) ? TRUE : FALSE;


    // Print out the selected bands
    PAL_TRACE(PAL_LEV(1), 0, "Selected Bands :");
    if ((BandMap & PAL_GSM850))
        PAL_TRACE(PAL_LEV(1),0,  "   -GSM 850");
    if ((BandMap & PAL_GSM900))
        PAL_TRACE(PAL_LEV(1),0,  "   -GSM 900");
    if ((BandMap & PAL_PCS1900))
        PAL_TRACE(PAL_LEV(1), 0, "   -PCS 1900");
    else
        PAL_TRACE(PAL_LEV(1), 0, "   -DCS 1800");

    PAL_TRACE(PAL_LEV(1), 0, "PAL init done.");
    PAL_PROFILE_FUNCTION_EXIT(pal_Initialisation);
}

// *** pal_SetGsmCounters ***
// counters: GSM counters
//
VOID
pal_SetGsmCounters (pal_GsmCounters_t * counters)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetGsmCounters);
    PAL_ASSERT(counters != NULL, "SetGsmCounters: bad argument.");

    palDbgTrc((HAL_DBGPALTRC_SETCOUNTERS,counters->T1,counters->T2,counters->T3,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "SetGsmCounters T1=%d T2=%d T3=%d",
              counters->T1,counters->T2,counters->T3);

    /* Copy the counters for the next frame */
    g_palCtx.NextCounter.T1 = counters->T1;
    g_palCtx.NextCounter.T2 = counters->T2;
    g_palCtx.NextCounter.T3 = counters->T3;
    g_mailbox.pal2spc.counters[g_palCtx.FrameValid] = g_palCtx.NextCounter;

    PAL_PROFILE_FUNCTION_EXIT(pal_SetGsmCounters);
}



// *** pal_RxBufferCfg ***
// Cfg: Rx buffer configuration
VOID
pal_RxBufferCfg (pal_RxBufferCfg_t * Cfg)
{
    UINT8 transItr = 0;
    PAL_PROFILE_FUNCTION_ENTRY(pal_RxBufferCfg);
    palDbgTrc((HAL_DBGPALTRC_RXBUFFCFG,Cfg->BufferIdx,Cfg->ChannelType,Cfg->InterruptReq,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "RxBufferCfg(%d) Type:0x%x ItReq:0x%x ",
              Cfg->BufferIdx,Cfg->ChannelType, Cfg->InterruptReq);
    g_mailbox.pal2spc.statWin.rx.cfg[Cfg->BufferIdx].type = Cfg->ChannelType;

    // Translate IT requests
    if (Cfg->InterruptReq & PAL_USF_READY)
    {
        transItr |= MBOX_DSP_USF_READY;
        g_palCtx.palModeIsGprs = TRUE;
#ifdef CHIP_SAIC_CTRL_BY_SOFT
        // request the resource for PAL (the stack)
        if(Cfg->ChannelType  == PAL_PDCH_CHN)
        {

            hal_SwRequestClk(FOURCC_PAL, PAL_STACK_SYS_FREQ_GPRS);
            pal_EnableSaic(TRUE);
        }
#endif
    }
    else
    {
#ifdef CHIP_SAIC_CTRL_BY_SOFT
        if((Cfg->ChannelType != PAL_TCHF_CHN) && (Cfg->ChannelType != PAL_TCHH_CHN))
        {
             pal_EnableSaic(FALSE);
        }
#endif
        g_palCtx.palModeIsGprs = FALSE;
    }
#ifdef  CHIP_DIE_8955
    if (g_palCtx.palModeIsGprs == TRUE)
    {
        g_mailbox_pal2spc_RxBufInd = 1;
    }
   else g_mailbox_pal2spc_RxBufInd=0;
#endif   
    g_mailbox.pal2spc.statWin.rx.cfg[Cfg->BufferIdx].itr = transItr;
    PAL_PROFILE_FUNCTION_EXIT(pal_RxBufferCfg);
}



// *** pal_SetTxWin_calib ***
// This function is NOT part of the API
// Should only be used by calibration software
VOID
pal_CalibSetTxWin(pal_TxWin_t * win, UINT16 dacValueIdx)
{
    UINT8 snap = g_palCtx.FrameValid;
    int i;
    GSM_RFBAND_T band = palGetBand(win->Arfcn);

    PAL_PROFILE_FUNCTION_ENTRY(pal_SetTxWin_calib);
    PAL_ASSERT(win != NULL, "SetTxWin: bad argument.");
    PAL_ASSERT(g_mailbox.pal2spc.win[snap].tx.qty == 0,
               "SetTxWin: tx already in the pipe. (Qty = %d)",
               g_mailbox.pal2spc.win[snap].tx.qty);
    PAL_ASSERT((UINT16) win->Start <= PAL_TCU_DEFAULT_WRAP_COUNT,
               "SetTxWin: bad start time: %d.", win->Start);

    PAL_TRACE(PAL_LEV(14), 0, "SetTxCalib[%d](%d):0x%x @=%d BitMap=%d DAC=%d",
              win->WinIdx,
              win->BstIdx,
              win->Arfcn,
              win->Start,
              win->TxBitMap,
              dacValueIdx);


    // Context for BCPU
    for(i=0; i<MBOX_MAX_TS_QTY; i++)
    {
        g_mailbox.pal2spc.win[snap].tx.routeMap[i] = win->RouteMap[i];
    }
    g_mailbox.pal2spc.win[snap].tx.winIdx = win->WinIdx;
    g_mailbox.pal2spc.win[snap].tx.bstIdx = win->BstIdx;
    g_mailbox.pal2spc.win[snap].tx.tsc = win->Tsc;
    g_mailbox.pal2spc.win[snap].tx.qty++;
    g_mailbox.pal2spc.win[snap].tx.bitMap = win->TxBitMap;

    // Time ordering
    g_palFrame.Win[win->WinIdx].date = win->Start - win->TA;
    palReorderWin(win->WinIdx);

    // Win parameters WARNING : use directly g_palCtx.TxCtx
    if (g_palCtx.TxCtx[win->RouteMap[0]].type == MBOX_CHAN_TYPE_RACH)
    {
        g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_TXABURST;
    }
    else
    {
        g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_TXNBURST;
    }
    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = band;
    g_palFrame.Win[win->WinIdx].bitmap =  win->TxBitMap;

    // Force PA ramps with DAC Values
    // It must be called before palProgramTxWin(), to tell digRf XCV to use DAC values
    palSetRamps_calib (dacValueIdx, band);

    // TCU programming
    palProgramTxWin(win->WinIdx);

    PAL_PROFILE_FUNCTION_EXIT(pal_SetTxWin_calib);
}


// *** pal_StartCiphering ***
VOID
pal_StartCiphering (UINT8 algo, UINT8 * key)
{
    UINT32 kcLow, kcHigh;

    PAL_PROFILE_FUNCTION_ENTRY(pal_StartCiphering);
    PAL_ASSERT(key != NULL, "StartCiphering: bad argument.");
    PAL_ASSERT(((algo == PAL_A51) || (algo == PAL_A52) || (algo == PAL_A53)),
               "StartCiphering: bad algo (%d).", algo);

    // Build the key
    kcHigh = key[3] | (key[2] << 8) | (key[1] << 16) | (key[0] << 24);
    kcLow = key[7] | (key[6] << 8) | (key[5] << 16) | (key[4] << 24);

    // PAL write the key through the driver
    hal_CipherKcInit (kcLow, kcHigh);

    // BCPU context
    g_mailbox.pal2spc.statWin.ciphMode = algo;

    palDbgTrc((HAL_DBGPALTRC_STARTCIPH,algo,kcLow,kcHigh,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "StartCiphering:Algo=%d KeyLow=0x%8x KeyHigh=0x%8x",
              algo, kcLow, kcHigh);
    PAL_PROFILE_FUNCTION_EXIT(pal_StartCiphering);
}

// *** pal_StopCiphering ***
VOID
pal_StopCiphering (VOID)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_StopCiphering);
    // BCPU context
    g_mailbox.pal2spc.statWin.ciphMode = PAL_UNDEF_VALUE;
    palDbgTrc((HAL_DBGPALTRC_STOPCIPH,0,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "StopCiphering");
    PAL_PROFILE_FUNCTION_EXIT(pal_StopCiphering);
}

// *** pal_GprsCiphering ***
VOID
pal_GprsCiphering(pal_GprsCiphering_t *GprsCiphering)
{
    UINT32 kcLow, kcHigh;
    HAL_ERR_T status = HAL_ERR_RESOURCE_BUSY;
    HAL_DMA_GEA_CFG_T dmaGeaConfig;



    PAL_PROFILE_FUNCTION_ENTRY(pal_GprsCiphering);
    PAL_ASSERT(GprsCiphering->Kc != NULL, "StartCiphering: bad argument.");
    PAL_ASSERT(((GprsCiphering->Algo == PAL_GEA1) ||
                (GprsCiphering->Algo == PAL_GEA2) ||
                (GprsCiphering->Algo == PAL_GEA3) ),
               "GprsCiphering: bad algo: %d.",
               GprsCiphering->Algo);

    // Build the key
    kcHigh = GprsCiphering->Kc[3] | (GprsCiphering->Kc[2] << 8) |
             (GprsCiphering->Kc[1] << 16) | (GprsCiphering->Kc[0] << 24);
    kcLow = GprsCiphering->Kc[7] | (GprsCiphering->Kc[6] << 8) |
            (GprsCiphering->Kc[5] << 16) | (GprsCiphering->Kc[4] << 24);

    dmaGeaConfig.srcAddr = GprsCiphering->DataBlock;
    dmaGeaConfig.dstAddr = GprsCiphering->DataBlock;
    dmaGeaConfig.transferSize = GprsCiphering->Length;
    dmaGeaConfig.geaAlgo = GprsCiphering->Algo;
    dmaGeaConfig.direction = GprsCiphering->Direction;
    dmaGeaConfig.kcLow =kcLow ;
    dmaGeaConfig.kcHigh =kcHigh ;
    dmaGeaConfig.messKey = GprsCiphering->Input;
    dmaGeaConfig.userHandler = NULL;

    while (status != HAL_ERR_NO)
    {
        status = hal_DmaGeaStart(&dmaGeaConfig);
    }

    palDbgTrc((HAL_DBGPALTRC_GPRSCIPH,GprsCiphering->Algo, kcLow, kcHigh,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "GprsCiphering:Algo=%d KeyLow=0x%8x KeyHigh=0x%8x",
              GprsCiphering->Algo, kcLow, kcHigh);

    // Blocking function
    // while(!(hal_DmaDone())); // <-- Blocked by the GEA DMA function, no more need here

    // Data cache must be consistent after the function call
    hal_SysInvalidateCache((UINT8*) GprsCiphering->DataBlock, GprsCiphering->Length);

    PAL_PROFILE_FUNCTION_EXIT(pal_GprsCiphering);
}

// *** pal_StartTch ***
VOID
pal_StartTch (UINT8 mode, UINT8 tn, pal_AMRCfg_t * AMRCfg) // tn is useless
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_StartTch);
    g_mailbox.pal2spc.statWin.dedicated.mode = mode;
    g_mailbox.pal2spc.statWin.dedicated.active = TRUE;
    g_mailbox.pal2spc.statWin.dedicated.initTch= TRUE;

    // Fill AMR configuration if relevant
    if (TCH_ALGO(mode) == PAL_TCH_FR_HR_AMR)
    {
        int i;

        for (i=0; i<4; i++)
        {
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[i] =
                AMRCfg->ActiveCodecSet[i];
        }
        for (i=0; i<3; i++)
        {
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i] =
                AMRCfg->Hysteresis[i];
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i] =
                AMRCfg->Threshold[i];
        }
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.version =
            AMRCfg->Version;
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.noiseSuppression =
            AMRCfg->NoiseSuppression;
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx =
            AMRCfg->StartModeIdx;
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet =
            AMRCfg->NbActiveCodecSet;
#ifndef __NO_AMR__
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase =
            AMRCfg->CMIPhase;
#endif
        g_mailbox.pal2spc.statWin.dedicated.changedAMRCfg = 1;
    }

    palDbgTrc((HAL_DBGPALTRC_STARTTCH,mode,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "StartTch mode=0x%x",
              mode);
#ifdef CHIP_SAIC_CTRL_BY_SOFT
    pal_EnableSaic(TRUE);
#endif
    PAL_PROFILE_FUNCTION_EXIT(pal_StartTch);
}

// *** pal_ChangeTchMode ***
VOID
pal_ChangeTchMode (UINT8 mode, pal_AMRCfg_t * AMRCfg)
{
    UINT8 tchMode;

    PAL_PROFILE_FUNCTION_ENTRY(pal_ChangeTchMode);
    tchMode = g_mailbox.pal2spc.statWin.dedicated.mode & 0x3;
    g_mailbox.pal2spc.statWin.dedicated.mode = (mode & 0xFC) | tchMode;
    g_mailbox.pal2spc.statWin.dedicated.active = TRUE;

    // AMR configuration has changed
    if (AMRCfg != NULL)
    {
        int i;

        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.version =
            AMRCfg->Version;
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.noiseSuppression =
            AMRCfg->NoiseSuppression;
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx =
            AMRCfg->StartModeIdx;
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet =
            AMRCfg->NbActiveCodecSet;

        for (i=0; i<4; i++)
        {
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[i] =
                AMRCfg->ActiveCodecSet[i];
        }
        for (i=0; i<3; i++)
        {
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i] =
                AMRCfg->Hysteresis[i];
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i] =
                AMRCfg->Threshold[i];
        }
#ifndef __NO_AMR__
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase =
            AMRCfg->CMIPhase;
#endif

        g_mailbox.pal2spc.statWin.dedicated.changedAMRCfg = 1;
    }

    palDbgTrc((HAL_DBGPALTRC_CHANGETCH,mode,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "ChangeTchMode mode=0x%x",
              mode);
    PAL_PROFILE_FUNCTION_EXIT(pal_ChangeTchMode);
}

// *** pal_RetrieveCurrentAMRCfg (pal_AMRCfg_t *AMRCfg) ***
VOID
pal_RetrieveCurrentAMRCfg (pal_AMRCfg_t *AMRCfg)
{
    int i;

    PAL_PROFILE_FUNCTION_ENTRY(pal_RetrieveCurrentAMRCfg);

    AMRCfg->Version =
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.version;
    AMRCfg->NoiseSuppression =
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.noiseSuppression;
    AMRCfg->StartModeIdx =
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.startModeIdx;
    AMRCfg->NbActiveCodecSet =
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.nbActiveCodecSet;

    for (i=0; i<4; i++)
    {
        AMRCfg->ActiveCodecSet[i] =
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.activeCodecSet[i];
    }
    for (i=0; i<3; i++)
    {
        AMRCfg->Hysteresis[i] =
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.hysteresis[i];
        AMRCfg->Threshold[i] =
            g_mailbox.pal2spc.statWin.dedicated.AMRCfg.threshold[i];
    }

#ifndef __NO_AMR__
    AMRCfg->CMIPhase =
        g_mailbox.pal2spc.statWin.dedicated.AMRCfg.CMIPhase;
#endif

    palDbgTrc((HAL_DBGPALTRC_RETAMRCFG,0,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "RetrieveCurrentAMRCfg");
    PAL_PROFILE_FUNCTION_EXIT(pal_RetrieveCurrentAMRCfg);

}

// *** pal_SetDtx ***
VOID
pal_SetDtx (BOOL dtxAllowed)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetDtx);
    g_mailbox.pal2spc.dtxAllowed = dtxAllowed;
    g_mailbox.spc2pal.dtxUsed = 0;
    palDbgTrc((HAL_DBGPALTRC_SETDTX,dtxAllowed,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "SetDtx Allowed=%d",dtxAllowed);
    PAL_PROFILE_FUNCTION_EXIT(pal_SetDtx);
}

// *** pal_GetDtx ***
BOOL
pal_GetDtx (VOID)
{
    BOOL DtxUsed = g_mailbox.spc2pal.dtxUsed;
    PAL_PROFILE_FUNCTION_ENTRY(pal_GetDtx);
    g_mailbox.spc2pal.dtxUsed = 0;
    palDbgTrc((HAL_DBGPALTRC_GETDTX,DtxUsed,0,0,0,0,0));
    if (DtxUsed) PAL_TRACE(PAL_LEV(1), 0, "Dtx Used");
    PAL_PROFILE_FUNCTION_EXIT(pal_GetDtx);
    return DtxUsed;
}

// *** pal_SetTchLoop ***
VOID
pal_SetTchLoop (UINT16 mode)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetTchLoop);
    g_mailbox.pal2spc.statWin.loopMode = mode;
    palDbgTrc((HAL_DBGPALTRC_SETLOOP,mode,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "SetLoop mode=%d", mode);

    // DAI loop management.
    if ( (mode == PAL_SPCH_DECOD_LOOP) || (mode == PAL_SPCH_ENCOD_LOOP) )
    {
        // FIXME Implement
        // hal_AudioDAI(TRUE, FALSE, 1);
    }
    else if (mode == PAL_ACOUSTIC_TEST)
    {
        // FIXME Implement
        // hal_AudioDAI(TRUE, TRUE, 1);
    }
    else if (mode == PAL_OPEN_LOOP)
    {
        // FIXME Implement
        // hal_AudioDAI(FALSE, FALSE, 1);
    }

    PAL_PROFILE_FUNCTION_EXIT(pal_SetTchLoop);
}

// *** pal_StopTch ***
VOID
pal_StopTch (VOID)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_StopTch);
    g_mailbox.pal2spc.statWin.dedicated.mode = MBOX_NO_TCH;
    g_mailbox.pal2spc.statWin.dedicated.active = FALSE;
    palDbgTrc((HAL_DBGPALTRC_STOPTCH,0,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(1), 0, "StopTch");
#ifdef CHIP_SAIC_CTRL_BY_SOFT
    pal_EnableSaic(FALSE);
#endif
    PAL_PROFILE_FUNCTION_EXIT(pal_StopTch);
}



/*************************************************
 *** Frame interrupt management
 *************************************************/

// *** pal_FintSuspend ***
VOID
pal_FintSuspend (UINT16 frameQty)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_FintSuspend);
// request the resource for PAL (the stack)

    hal_SwReleaseClk(FOURCC_PAL);

#ifdef __LPS_ENABLED__
    if (frameQty >= PAL_FINT_SUSPEND_STOP)
    {
        // Stop the TCU
        hal_TcuStop();
        // Reset RFSPI FIFO
        hal_RfspiSetupFifo();
#ifdef CHIP_DIE_8809E2
        hal_SysSwitchPll(HAL_SYS_PLL_MODE_FLIGHT);
#endif
        // The fint has been stopped, put the RF component in light low power mode
        // We don't know yet if we will turn off the power
#ifdef LTE_NBIOT_SUPPORT
        RF_Sleep(RFD_LOW_POWER);
#else
        rfd_XcvSleep(RFD_LOW_POWER);
#endif
        rfd_SwSleep(RFD_LOW_POWER);
        rfd_PaSleep(RFD_LOW_POWER);
        // Turn off PAL Rf Interface
#if (CHIP_HAS_ANA_RF)
        hal_AnaRfIfTurnOff();
#endif
        // clear the GPRS mode flag
        g_palCtx.palModeIsGprs = FALSE;

        // RF disabled
        g_palDebug.RFEnabled = FALSE;
    }
    else
        // Check wether we can Suspend the Fint right now and do it:
        if (hal_LpsSkipFrame (frameQty))
        {
            // Reset RFSPI FIFO
            hal_RfspiSetupFifo();
            // The fint has been suspended, put the RF component in light low power mode
            // We don't know yet if we will turn off the power
#ifdef LTE_NBIOT_SUPPORT
            RF_Sleep(RFD_LOW_POWER);
#else
            rfd_XcvSleep(RFD_LOW_POWER);
#endif
            rfd_SwSleep(RFD_LOW_POWER);
            rfd_PaSleep(RFD_LOW_POWER);
            // Turn off aux clock
            pal_AuxClkOff();
            // Turn off PAL Rf Interface
#if (CHIP_HAS_ANA_RF)
            hal_AnaRfIfTurnOff();
#endif
        }
#endif
#ifdef __LPS_VERBOSE__
    palDbgTrc((HAL_DBGPALTRC_FSUSPEND,frameQty,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(15)|TNB_ARG(1), 0, "FintSuspend FrameQty=%d",
              frameQty);
#endif
    PAL_PROFILE_FUNCTION_EXIT(pal_FintSuspend);
}

// *** pal_FintResume ***
VOID
pal_FintResume (VOID)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_FintResume);
#ifdef __LPS_VERBOSE__
    palDbgTrc((HAL_DBGPALTRC_FRESUME,0,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(15), 0, "FintResume");
#endif
#ifdef __LPS_ENABLED__
    if(g_palDebug.RFEnabled)
    {
        hal_LpsResumeFrame();
    }
    else
    {
#ifdef CHIP_DIE_8809E2
        hal_SysSwitchPll(HAL_SYS_PLL_MODE_NORMAL);
#endif
        hal_TcuStart (0);
        // RF parts will be waken up by pal_GetElapsedTime
        g_palDebug.RFEnabled = TRUE;
    }
#endif
    PAL_PROFILE_FUNCTION_EXIT(pal_FintResume);
}

// *** pal_GetElapsedTime ***
UINT16
pal_GetElapsedTime (VOID)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_GetElapsedTime);

#ifdef __LPS_ENABLED__
    UINT16 skippedFrames = (UINT16)hal_LpsGetElapsedFrames();
    // Turn on PAL Rf Interface

#if (CHIP_HAS_ANA_RF)
    hal_AnaRfIfTurnOn();
#endif

    // Full Xcver wake up has already been performed if we really
    // did reach the Low Power mode
    rfd_XcvWakeUp();
    // Update the GSM timers
    palTimersSkippedUpdate(skippedFrames);

#ifdef __LPS_VERBOSE__
    palDbgTrc((HAL_DBGPALTRC_GETELAPSED,skippedFrames,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(15)|TNB_ARG(1), 0, "GetElapsedTime : %d frames",skippedFrames);
#endif
    PAL_PROFILE_FUNCTION_EXIT(pal_GetElapsedTime);
    return (skippedFrames);
#else
#ifdef __LPS_VERBOSE__
    palDbgTrc((HAL_DBGPALTRC_GETELAPSED,1,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(15)|TNB_ARG(1), 0, "GetElapsedTime : 1 frames");
#endif
    PAL_PROFILE_FUNCTION_EXIT(pal_GetElapsedTime);
    return (1);
#endif
}

//======================================================================
// pal_RfWakeUp
//----------------------------------------------------------------------
// Useless in our implementation
// Actual WakeUp done in Either pal_Initialisation or pal_GetElapsedTime
// Rf resources that require more than one frame of setup time are also
// handled internally
//======================================================================
VOID pal_RfWakeUp(VOID)
{

}

//======================================================================
// pal_RfSleep
//----------------------------------------------------------------------
// Useless in our implementation
// Sleep done in hal_LpsSkipFrame when actually powering down
//======================================================================
VOID
pal_RfSleep(VOID)
{
    // The fint has been suspended, put the RF component in light low power mode
    // We don't know yet if we will turn off the power
#ifdef LTE_NBIOT_SUPPORT
        RF_Sleep(RFD_LOW_POWER);
#else
        rfd_XcvSleep(RFD_LOW_POWER);
#endif
    rfd_SwSleep(RFD_LOW_POWER);
    rfd_PaSleep(RFD_LOW_POWER);
}


//======================================================================
// pal_GetBcpuSysFreq
//----------------------------------------------------------------------
// return current BCPU freq
//======================================================================
UINT32 pal_GetBcpuSysFreq()
{
    return  g_palCtx.palBcpuSysFreqCurr;
}

/*************************************************
 *** VCO dependant resources flag management
 *************************************************/

// *** pal_ResourceActive ***
VOID
pal_ResourceActive (UINT8 resourceNbr)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_ResourceActive);

    // Check resourceNbr
    // FIXME Implement with freq things, if this function is useful
    // PAL_ASSERT((resourceNbr < PAL_VCO_QTY), resourceNbr,
    //                "ResourceActive: bad ressNbr.");

    // Change activity flag
    // FIXME Implement with freq things, if this function is useful
    // hal_SetResourceActivity(resourceNbr, TRUE);

    palDbgTrc((HAL_DBGPALTRC_RESACTIVE,resourceNbr,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(15), 0, "ResourceActive: %d",resourceNbr);

    PAL_PROFILE_FUNCTION_EXIT(pal_ResourceActive);
}

// *** pal_ResourceInactive ***
VOID
pal_ResourceInactive (UINT8 resourceNbr)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_ResourceInactive);

    // Check resourceNbr
    // FIXME Implement with freq things, if this function is useful
    // PAL_ASSERT((resourceNbr < PAL_VCO_QTY), resourceNbr,
    //                "ResourceInactive: bad ressNbr.");

    // Change VCO activity flag
    // FIXME Implement with freq things, if this function is useful
    // hal_SetResourceActivity(resourceNbr, FALSE);

    palDbgTrc((HAL_DBGPALTRC_RESINACTIVE,resourceNbr,0,0,0,0,0));
    PAL_TRACE(PAL_LEV(15), 0, "ResourceInactive: %d",resourceNbr);

    PAL_PROFILE_FUNCTION_EXIT(pal_ResourceInactive);
}

//======================================================================
// pal_Sleep
//----------------------------------------------------------------------
// Called continuously while in idle task to put the system in sleep
// mode when applicable.
// Powering down is actually handled in HAL.
//======================================================================
VOID pal_Sleep (VOID)
{
    // Profiling useless because of number of calls
#ifdef __LPS_ENABLED__
    hal_LpsSleep();
#endif
}

//--------------------------------------------------------------------//
//                                                                    //
//                      PAL Internal functions                        //
//                                                                    //
//--------------------------------------------------------------------//
#ifdef FPGA_NBCPU
int nbcpu_main();
VOID palStartNBcpu (VOID)
{
    UINT32 bcpuStackBase;
    UINT32 startTime;
    UINT32 counter;
	UINT32* src = NULL;
    UINT32* dst = NULL;
    UINT32 i;
    // BCPU stack start at the end of SRAM
    bcpuStackBase = UNCACHED (((UINT32)&_nb_sram_romed_globals_start - 4));
    {
        UINT32* ptr=(UINT32*)(UNCACHED((UINT32)&_nb_sram_romed_end));
        while (ptr < (UINT32*)bcpuStackBase)
            *(ptr++) = 0xCAFEBEEF;
    }

	/*src = (UINT32)&_nb_sram_patch_flash_start;
    dst = (UINT32*)UNCACHED((UINT32)&_bb_sram_patch_start);
    for (i=0;i<((UINT32)&_nb_sram_patch_text_size)>>2;i++)
    {
        *dst = *src;
        dst++;
        src++;
    }*/

    // Copy patch code & data from ROM space to BB SRAM.
    for (src = (UINT32*) (UNCACHED((UINT32)&_nb_sram_patch_flash_start)),
            dst = (UINT32*) (UNCACHED((UINT32)&_nb_sram_patch_start));
            src < (UINT32*) (UNCACHED((UINT32)&_nb_sram_patch_flash_end));
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear patch BSS in BSS SRAM.
    for (dst = (UINT32*) (UNCACHED((UINT32)&_nb_sram_patch_bss_start));
            dst < (UINT32*) (UNCACHED((UINT32)&_nb_sram_patch_bss_end));
            dst++)
    {
        *dst = 0;
    }



    // Copy code & data from ROM space to External RAM.
    for (src = (UINT32*) UNCACHED((UINT32)&_bcpu_ram_flash_start_location),
            dst = (UINT32*) UNCACHED((UINT32)&_bcpu_ram_start);
            src < (UINT32*) UNCACHED((UINT32)&_bcpu_ram_flash_end_location);
            src++, dst++)
    {
        *dst = *src;
    }

    // Clear BSS in External RAM.
    for (dst = (UINT32*) UNCACHED((UINT32)&_bcpu_ram_bss_start);
            dst < (UINT32*) UNCACHED((UINT32)&_bcpu_ram_bss_end);
            dst++)
    {
        *dst = 0;
    }


    // Handshake process to be sure BCPU is well started
    g_mailbox.spc2pal.debug[0] = MBOX_XCPU_HELLO_WORD;
    hal_SysStartBcpu((VOID*)nbcpu_main, (VOID*)bcpuStackBase);
    // Clean-up used mailbox area
    g_mailbox.spc2pal.debug[0] = 0;



}
#endif

//======================================================================
// palStartBcpu
//----------------------------------------------------------------------
// Start the BCPU
//  - sets its Stack address
//  - check it actually boots (handcheck)
//  - displays BB_SRAM info
//======================================================================
VOID palStartBcpu (VOID)
{
    UINT32 bcpuStackBase;
    UINT32 startTime;
    UINT32 counter;

    // BCPU stack start at the end of SRAM
    bcpuStackBase = UNCACHED (((UINT32)&_bb_sram_romed_globals_start - 4));
    {
        UINT32* ptr=(UINT32*)(UNCACHED((UINT32)&_bb_sram_romed_end));
        while (ptr < (UINT32*)bcpuStackBase)
            *(ptr++) = 0xCAFEBEEF;
    }
#ifdef CHIP_SAIC_CTRL_BY_SOFT
    pal_EnableSaic(FALSE);
#endif
    hal_SysSetBBClock(PAL_BCPU_SYS_FREQ_DEFAULT);
    // Handshake process to be sure BCPU is well started
    g_mailbox.spc2pal.debug[0] = MBOX_XCPU_HELLO_WORD;
    hal_SysStartBcpu((VOID*)bcpu_main, (VOID*)(bcpuStackBase & ~HAL_SYS_CACHED_UNCACHED_OFFSET));

    counter = 0;
    UINT32 status = hal_SysEnterCriticalSection();    
    startTime = hal_TimGetUpTime();
    // Wait until other cpu is done to stop: read at mailbox top.
    while (g_mailbox.spc2pal.debug[0] == MBOX_XCPU_HELLO_WORD)
    {
        counter = hal_TimGetUpTime() - startTime;
        if (counter >= PAL_DEADLOCK_COUNTER)
        {
            break;
        }
    }
    hal_SysExitCriticalSection(status);
    PAL_ASSERT(counter < PAL_DEADLOCK_COUNTER,
               "palStartBcpu: Deadlock in CPUs handshake (counter = %d).", counter);
    PAL_ASSERT(g_mailbox.spc2pal.debug[0] == MBOX_BCPU_ACKNOWLEDGE_WORD,
               "palStartBcpu: BCPU did not respond properly (debug info: %d) %08x %08x.",g_mailbox.spc2pal.debug[0],
               &g_mailbox, &g_mailbox.spc2pal.debug[0]);
    // Clean-up used mailbox area
    g_mailbox.spc2pal.debug[0] = 0;

    // Memory status printout
    {
#ifndef PAL_NO_TRACE
        UINT32 EoSB = (UINT32)(&_bb_sram_romed_end);//+sizeof(SPP_STATIC_BUFFERS_T);
        UINT32 GSize = (UINT32)(&_bb_sram_romed_cached_end) - (UINT32)(&_bb_sram_romed_itlv_buf_end);
        UINT32 EoFB = (UINT32)(&_bb_sram_romed_cached_end) + 9*4*BB_FCCH_SIZE;
        PAL_TRACE(_PAL|TSTDOUT, 0,"************ BCPU Started ************");
        PAL_TRACE(_PAL|TSTDOUT, 0,"GLOBALS BASE:          0x%x",(UINT32)(&_bb_sram_romed_itlv_buf_end));
        PAL_TRACE(_PAL|TSTDOUT, 0,"STATIC_BUFFER_BASE:    0x%x\n",(UINT32)(&_bb_sram_romed_cached_end));
        PAL_TRACE(_PAL|TSTDOUT, 0,"globals buffer size:   %d bytes",GSize);
        PAL_TRACE(_PAL|TSTDOUT, 0,"static buffers size:   %d bytes\n",sizeof(SPC_STATIC_BUFFERS_T));
        PAL_TRACE(_PAL|TSTDOUT, 0,"End of static buffers: 0x%x",EoSB);
        PAL_TRACE(_PAL|TSTDOUT, 0,"End of FCCH buffers:   0x%x\n",EoFB);
        PAL_TRACE(_PAL|TSTDOUT, 0,"BCPU Stack @:          0x%x",bcpuStackBase);
        PAL_TRACE(_PAL|TSTDOUT, 0,"BCPU Stack max size:   %d bytes",
                  bcpuStackBase - EoSB);
        PAL_TRACE(_PAL|TSTDOUT, 0,"size of mailbox:       %d bytes", sizeof(SPC_MAILBOX_T));
        PAL_TRACE(_PAL|TSTDOUT, 0,"bbsramchend :%#x:", &_bb_sram_romed_cached_end);
        PAL_TRACE(_PAL|TSTDOUT, 0,"itlvbuffend :%#x:", &_bb_sram_romed_itlv_buf_end);
        PAL_TRACE(_PAL|TSTDOUT, 0,"**************************************");
#endif
    }

#if( TRI_BAND_SEARCH_SUPPORT == 1)
    g_TriBandSearchSuppor = 1;
#else
    g_TriBandSearchSuppor = 0;
#endif
}

//======================================================================
// palTimersSkippedUpdate
//----------------------------------------------------------------------
// Update GSM timers when returning from "Skipped Frames" period
//======================================================================
VOID palTimersSkippedUpdate (UINT16 elapsedFrames)
{
    UINT32 Fn;
    UINT8 Mod26;

    if (elapsedFrames > 1)
    {
        // Counter will be incremented by one in pal_FrameStart
        elapsedFrames--;

        // Recover old Fn
        Mod26 = (g_palCtx.NextCounter.T3 < g_palCtx.NextCounter.T2) ?
                (26 + g_palCtx.NextCounter.T3 - g_palCtx.NextCounter.T2) :
                (g_palCtx.NextCounter.T3 - g_palCtx.NextCounter.T2);

        if (Mod26 >= 26) Mod26 -= 26;

        Fn = ((51 * Mod26) + g_palCtx.NextCounter.T3 + (51 * 26 * g_palCtx.NextCounter.T1));

        // Update Fn
        Fn += elapsedFrames;

        // Derive counters
        g_palCtx.NextCounter.T2 = Fn % PAL_T2_MODULO ;
        g_palCtx.NextCounter.T3 = Fn % PAL_T3_MODULO ;
        g_palCtx.NextCounter.T1 = (Fn / 1326) % PAL_T1_MODULO;
    }
}

// *** palWinInit (int WinIdx) ***
VOID
palWinInit (int WinIdx)
{
    UINT8 i;
    g_palFrame.Win[WinIdx].type = PAL_UNDEF_VALUE;
    g_palFrame.Win[WinIdx].date = PAL_UNDEF_VALUE;
    g_palFrame.Win[WinIdx].arfcn = PAL_UNDEF_VALUE;
    g_palFrame.Win[WinIdx].band = PAL_UNDEF_VALUE;
    for (i = 0; i < MBOX_MAX_TS_QTY; i++)
    {
        g_palFrame.Win[WinIdx].gain[i] = PAL_UNDEF_VALUE;
    }
    g_palFrame.Win[WinIdx].bitmap = 0;
    g_palFrame.Win[WinIdx].firstTcu = PAL_UNDEF_VALUE;
    g_palFrame.Win[WinIdx].lastTcu = PAL_UNDEF_VALUE;
    g_palFrame.Win[WinIdx].winIdx = PAL_UNDEF_VALUE;
}


// *** palFrameInit (VOID) ***
VOID
palFrameInit(VOID)
{
    int i;

    for (i=0; i<PAL_NB_MAX_WIN_PER_FRAME+PAL_NB_EXTR_WIN_PER_FRAME; i++)
    {
        palWinInit(i);
        g_palFrame.WinOrder[i] = PAL_UNDEF_VALUE;
    }
    g_palFrame.WinNb = 0;
    g_palFrame.lastStop = 0;
    g_palFrame.WrapValue = PAL_TCU_DEFAULT_WRAP_COUNT;
    g_palDebug.FrameStarted = FALSE;
}


// *** palSetRamps_calib (int WinIdx,UINT16 minDac,UINT16 dacValue) ***
// Special version of palSetRamps that should be used only for calibration
// purposes.
VOID
palSetRamps_calib (UINT16 dacValueIdx, GSM_RFBAND_T band)
{
    UINT16 dacValueHigh[4];
    UINT16 dacValueLow;

    // This version of ramp building supports only one-burst
    // window (ie. TxBitMap = 1). Therefore only ramps 0 and 1
    // will be used.
#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207)) || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
    if (GSM_BAND_PCS1900 == band)
    {
        dacValueLow = g_palCalibration->pa->palcust.profileInterpP[0];
    }
    else if (GSM_BAND_DCS1800 == band)
    {
        dacValueLow = g_palCalibration->pa->palcust.profileInterpD[0];
    }
    else if (GSM_BAND_GSM900 == band)
    {
        dacValueLow = g_palCalibration->pa->palcust.profileInterpG[0];
    }
    else
    {
        dacValueLow = g_palCalibration->pa->palcust.profileInterpG850[0];
    }
#else
    if (GSM_BAND_DCS1800 == band || GSM_BAND_PCS1900 == band)
    {
        dacValueLow = g_palCalibration->pa->palcust.profileInterpDp[0];
    }
    else
    {
        dacValueLow = g_palCalibration->pa->palcust.profileInterpG[0];
    }
#endif

    if (dacValueIdx & 0x8000) // This is a DAC value
    {
        if((dacValueIdx & 0x3ff) < 5)
        {
#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207)) || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
            if (GSM_BAND_PCS1900 == band)
            {
                dacValueHigh[3] = g_palCalibration->pa->palcust.profileInterpP[(dacValueIdx & 0x7)] & 0x3FF; // 10 bits
                dacValueHigh[2] = g_palCalibration->pa->palcust.profileInterpP[(dacValueIdx & 0x7)+5] & 0x3FF; // 10 bits
                dacValueHigh[1] = g_palCalibration->pa->palcust.profileInterpP[(dacValueIdx & 0x7)+10] & 0x3FF; // 10 bits
                dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpP[15] & 0x3FF; // 10 bits
            }
            else if (GSM_BAND_DCS1800 == band)
            {
                dacValueHigh[3] = g_palCalibration->pa->palcust.profileInterpD[(dacValueIdx & 0x7)] & 0x3FF; // 10 bits
                dacValueHigh[2] = g_palCalibration->pa->palcust.profileInterpD[(dacValueIdx & 0x7)+5] & 0x3FF; // 10 bits
                dacValueHigh[1] = g_palCalibration->pa->palcust.profileInterpD[(dacValueIdx & 0x7)+10] & 0x3FF; // 10 bits
                dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpD[15] & 0x3FF; // 10 bits
            }
            else if (GSM_BAND_GSM900 == band)
            {
                dacValueHigh[3] = g_palCalibration->pa->palcust.profileInterpG[(dacValueIdx & 0x7)] & 0x3FF; // 10 bits
                dacValueHigh[2] = g_palCalibration->pa->palcust.profileInterpG[(dacValueIdx & 0x7)+5] & 0x3FF; // 10 bits
                dacValueHigh[1] = g_palCalibration->pa->palcust.profileInterpG[(dacValueIdx & 0x7)+10] & 0x3FF; // 10 bits
                dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpG[15] & 0x3FF; // 10 bits
            }
            else
            {
                dacValueHigh[3] = g_palCalibration->pa->palcust.profileInterpG850[(dacValueIdx & 0x7)] & 0x3FF; // 10 bits
                dacValueHigh[2] = g_palCalibration->pa->palcust.profileInterpG850[(dacValueIdx & 0x7)+5] & 0x3FF; // 10 bits
                dacValueHigh[1] = g_palCalibration->pa->palcust.profileInterpG850[(dacValueIdx & 0x7)+10] & 0x3FF; // 10 bits
                dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpG850[15] & 0x3FF; // 10 bits
            }
#else
            if(band < 2)
            {

                dacValueHigh[3] = g_palCalibration->pa->palcust.profileInterpG[(dacValueIdx & 0x7)] & 0x3FF; // 10 bits
                dacValueHigh[2] = g_palCalibration->pa->palcust.profileInterpG[(dacValueIdx & 0x7)+5] & 0x3FF; // 10 bits
                dacValueHigh[1] = g_palCalibration->pa->palcust.profileInterpG[(dacValueIdx & 0x7)+10] & 0x3FF; // 10 bits
                dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpG[15] & 0x3FF; // 10 bits
            }
            else
            {

                dacValueHigh[3] = g_palCalibration->pa->palcust.profileInterpDp[(dacValueIdx & 0x7)] & 0x3FF; // 10 bits
                dacValueHigh[2] = g_palCalibration->pa->palcust.profileInterpDp[(dacValueIdx & 0x7)+5] & 0x3FF; // 10 bits
                dacValueHigh[1] = g_palCalibration->pa->palcust.profileInterpDp[(dacValueIdx & 0x7)+10] & 0x3FF; // 10 bits
                dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpDp[15] & 0x3FF; // 10 bits
            }
#endif
        }
        else
        {
            dacValueHigh[0] = dacValueIdx & 0x3ff;
        }
    }
    else // This is a DAC value index
    {
        dacValueIdx &= 0xF; // 16 values
#if (defined(CHIP_DIE_8809E2) && !defined(CALIB_USE_0207))  || (defined(CHIP_DIE_8955) && !defined(FPGA)) 
        if (GSM_BAND_PCS1900 == band)
        {
            dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpP[dacValueIdx];
        }
        else if (GSM_BAND_DCS1800 == band)
        {
            dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpD[dacValueIdx];
        }
        else if (GSM_BAND_GSM900 == band)
        {
            dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpG[dacValueIdx];
        }
        else
        {
            dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpG850[dacValueIdx];
        }
#else
        if (GSM_BAND_DCS1800 == band || GSM_BAND_PCS1900 == band)
        {
            dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpDp[dacValueIdx];
        }
        else
        {
            dacValueHigh[0] = g_palCalibration->pa->palcust.profileInterpG[dacValueIdx];
        }
#endif
    }

#if (CHIP_HAS_ANA_RF)
    hal_AnaPaSetRampCalib (dacValueLow,dacValueHigh);
#else
    rfd_XcvCalibSetPaRamp(dacValueHigh);
#endif
}

// *** palCommonInit (VOID) ***
VOID
palCommonInit (VOID)
{
    INT32 i;
    HAL_COMREGS_IRQ_MASK_T fullMask = {0xF};

    // Flag that an Init has been requested during the frame
    g_palDebug.InitPending = TRUE;

    // Reset mailbox IRQs
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_XCPU_IRQ_SLOT_0, fullMask);
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_XCPU_IRQ_SLOT_1, fullMask);
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_BCPU_IRQ_SLOT_0, fullMask);
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_BCPU_IRQ_SLOT_1, fullMask);
    //TODO: check Interrupt mask setting
    //hal_ComregsSetItReg(MBOX_DSP_EQUALIZATION_COMPLETE | MBOX_DSP_USF_READY);


    // Stop/Reset Skip Frame and Low Power Count
    // FIXME Implement ...
    // hal_LpsStopSkipFrame();

    // Reset AFC related stuff
    g_palCtx.Afc.Update = FALSE;

    // Reset BCPU window context
    for (i = 0; i < MBOX_CTX_SNAP_QTY; i++)
    {
        g_mailbox.pal2spc.win[i].rx.qty = 0;
        g_mailbox.pal2spc.win[i].tx.qty = 0;
    }

    // Reset Tx buf types
    for (i = 0; i < MBOX_MAX_BUF_IDX_QTY; i++)
    {
        g_palCtx.TxCtx[i].type = PAL_UNDEF_VALUE;
        g_palCtx.TxCtx[i].encDone = 0;
        g_palCtx.TxCtx[i].cs = PAL_UNDEF_VALUE;
    }

    // Reset Temporary counters
    g_palCtx.NextCounter.T1 = 0;
    g_palCtx.NextCounter.T2 = 0;
    g_palCtx.NextCounter.T3 = 0;

    // Reset Nburst Gain Buffer
    for (i = 0; i < MBOX_CTX_SNAP_QTY; i++)
    {
        g_palCtx.NBGain[i] = 0;
    }

    // Don't Reset Frame snaps !!
    // This leads to problems when palInit is called after FrameStart
    //g_palCtx.FrameValid = 0;
    //g_palCtx.FrameResult = 0;

    // Init Frame structure
    palFrameInit();


#if (CHIP_HAS_ANA_RF)
    // Flush AFC Fifo
    hal_AnaAfcFlushFifo();
    // TODO: we might need to force a mid-range value on the AFC.
#endif


    // TCU wrap context
    g_palCtx.internalTimeShift = 0;
    g_palFrame.WrapValue = PAL_TCU_DEFAULT_WRAP_COUNT;
    g_palFrame.lastStop = 0;

    // Global Setup Time
    g_palCtx.SetUpTime = PAL_HW_SETUP;

    // Reset BCPU clock management
    g_palCtx.palBcpuSysFreqNext=HAL_SYS_FREQ_32K;
    g_palCtx.palBcpuSysFreqCurr=HAL_SYS_FREQ_32K;

    hal_LpsInvalidateCalib();
}

VOID
palHwInit (VOID)
{
    UINT32 mBoxSize = sizeof (SPC_MAILBOX_T);
    HAL_COMREGS_IRQ_MASK_T fullMask    = {(0xF)};
    //HAL_COMREGS_IRQ_MASK_T dspDoneMask = {(MBOX_DSP_TASK_COMPLETE>>4)};

    // Clear the mailbox
    memset ((UINT8 *) &g_mailbox, 0, mBoxSize);

    /******************/
    /* HW level Inits */
    /******************/

    // initialize the GPRS mode flag
    g_palCtx.palModeIsGprs = FALSE;

    // Request the default GSM clock

    hal_SwRequestClk(FOURCC_PAL, PAL_STACK_SYS_FREQ_GPRS);

    // WakeUp the RF parts first:
    // --------------------------
    // Turn on PAL Rf Interface
#if (CHIP_HAS_ANA_RF)
    hal_AnaRfIfTurnOn();
#endif

    g_palDebug.RFEnabled = TRUE;

    // Set snapshot quantity to 3
    hal_ComregsSetSnapCfg(TRUE);

    // Stack "PLL_LOCK_TIME" is actually the Rx to Rx minimum time (used to
    // stop the FCCH win as close as possible from next Rx)
    // TODO : see if this is relevant with Stack usage and find the missing term
    //pal_pll_lock_time = g_palCtx.winBounds.rxHold -
    //                    g_palCtx.winBounds.rxSetup + PAL_TIME_SYNC_MARGIN;

    pal_low_rla_var = g_palCtx.xcv->lowRla;

    // share RF-related parameters with SPC
    g_mailbox.pal2spc.rf.dcoCalEnable =         g_palCtx.xcv->dcocMode;
    g_mailbox.pal2spc.rf.dtxTcoSettings =       g_palCtx.xcv->dtxTcoSettings |
            g_palCtx.sw->dtxTcoSettings |
            g_palCtx.pa->dtxTcoSettings;
#if (SPC_IF_VER<=4)
    g_mailbox.rxOnTcoSettings = g_palCtx.xcv->rxOnTcoSettings |
                                g_palCtx.sw->rxOnTcoSettings |
                                g_palCtx.pa->rxOnTcoSettings;
#else
    g_mailbox.pal2spc.rf.rxOnTcoSettings = g_palCtx.xcv->rxOnTcoSettings |
                                           g_palCtx.sw->rxOnTcoSettings |
                                           g_palCtx.pa->rxOnTcoSettings;
#endif
    // Translation needed to avoid dependency between SPC and RFD
    // NOTE: translation into corresponding RfIF settings must be done in SPC
    g_mailbox.pal2spc.rf.digrfEnable =          g_palCtx.xcv->digRf.used;
#if (SPC_IF_VER == 1)
    // Keep backward compatibility with Greenstone (and Granite TC)
    // FIXME Might need to remove the -4
    g_mailbox.pal2spc.rf.digrfPreambleBits =    g_palCtx.xcv->txBstMap.preamble -4;
    g_mailbox.pal2spc.rf.digrfPostambleBits =   g_palCtx.xcv->txBstMap.postamble-4;
    g_mailbox.pal2spc.rf.digrfRxSampleShift =   16 - g_palCtx.xcv->digRf.rxSampleWidth;//FIXME
#else
    // TODO If !digrf, there is a  preamb += 4 in spp_Modulation
    g_mailbox.pal2spc.rf.txBstMap.preamble =    g_palCtx.xcv->txBstMap.preamble;
    g_mailbox.pal2spc.rf.txBstMap.guardStart =  g_palCtx.xcv->txBstMap.guardStart;
    g_mailbox.pal2spc.rf.txBstMap.postamble =   g_palCtx.xcv->txBstMap.postamble;
    // TODO: This depends on Modem datapath width capability, but also on
    // Xcver's average "target" sample size. Therefore these two settings
    // might have to be tweaked to get the best performance out of the Xcver.
#if (CHIP_HAS_14_BIT_RF_PATH == 0)
    g_mailbox.pal2spc.rf.digrfRxSampleWidth = 16 - g_palCtx.xcv->digRf.rxSampleWidth;//FIXME
#else

#ifdef __SATURATE_SAMPLES_TO_MODEM_WIDTH__
    // Here we try to keep samples size within bounds insuring proper handling
    // by the modem.
    // TODO: PAL_MAX_MODEM_WIDTH should be returned by a function in chip
    INT32 sample_diff = g_palCtx.xcv->digRf.rxSampleWidth - PAL_MAX_MODEM_WIDTH;

    g_mailbox.pal2spc.rf.digrfRxSampleWidth = PAL_MAX_MODEM_WIDTH;
    g_mailbox.pal2spc.rf.digrfRxSampleAlign =
        (g_palCtx.xcv->digRf.rxSampleAlign==RFD_DIGRF_SAMPLE_ALIGN_MSB)?
        SPC_DIGRF_SAMPLE_ALIGN_MSB:SPC_DIGRF_SAMPLE_ALIGN_LSB;
    if (sample_diff > 0)
    {
        // If aligned on LSB we can saturate without impacting the gain
        // but in case of MSB alignment we will lose LSBs and therefore gain.
        // FIXME: doing this might have too much impact on perfs.
        if (g_palCtx.xcv->digRf.rxSampleAlign==RFD_DIGRF_SAMPLE_ALIGN_MSB)
        {
            g_palCtx.Adc2dBmGain -= (6*sample_diff);
        }
    }
#else // don't __SATURATE_SAMPLES_TO_MODEM_WIDTH__
    // Here we don't care about maximum sample width and just use all the Xcver
    // has to offer. This might lead to unproper calculations along the modem
    // datapath due to lack of saturation.
    g_mailbox.pal2spc.rf.digrfRxSampleWidth = g_palCtx.xcv->digRf.rxSampleWidth;
    g_mailbox.pal2spc.rf.digrfRxSampleAlign =
        (g_palCtx.xcv->digRf.rxSampleAlign==RFD_DIGRF_SAMPLE_ALIGN_MSB)?
        SPC_DIGRF_SAMPLE_ALIGN_MSB:SPC_DIGRF_SAMPLE_ALIGN_LSB;
#endif  // __SATURATE_SAMPLES_TO_MODEM_WIDTH__

#endif  // (CHIP_HAS_14_BIT_RF_PATH == 0)

#endif // SPC_VER

    g_mailbox.pal2spc.rf.digrfRxRate =
        (g_palCtx.xcv->digRf.rxRate==RFD_DIGRF_1_SAMPLE_PER_SYMB)?
        SPC_DIGRF_1_SAMPLE_PER_SYMB:SPC_DIGRF_2_SAMPLE_PER_SYMB;
    g_mailbox.pal2spc.rf.digrfRxClkPol =
        (g_palCtx.xcv->digRf.rxClkPol==RFD_DIGRF_NORM_CLK_POL)?
        SPC_DIGRF_NORM_CLK_POL:SPC_DIGRF_INV_CLK_POL;
    g_mailbox.pal2spc.rf.digrfTxMode =
        (g_palCtx.xcv->digRf.txMode==RFD_DIGRF_TX_STREAM)?
        SPC_DIGRF_TX_STREAM:SPC_DIGRF_TX_BLOCK;
    g_mailbox.pal2spc.rf.digrfTxClkPol =
        (g_palCtx.xcv->digRf.txClkPol==RFD_DIGRF_NORM_CLK_POL)?
        SPC_DIGRF_NORM_CLK_POL:SPC_DIGRF_INV_CLK_POL;


    // TCU Start: Done in the pal_SetUsrVectorMsk to avoid getting a FINT
    // when unmasking the interrupt the first time
    // hal_TcuStart (0);

    /***********************/
    /* comregs level Inits */
    /***********************/

    // Reset mailbox IRQs
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_XCPU_IRQ_SLOT_0, fullMask);
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_XCPU_IRQ_SLOT_1, fullMask);
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_BCPU_IRQ_SLOT_0, fullMask);
    hal_ComregsXcpuIrqClearCause(HAL_COMREGS_BCPU_IRQ_SLOT_1, fullMask);

    hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_0,fullMask);
    hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_1,fullMask);

    // enable IT used to manage the BCPU clk resource
    //hal_ComregsSetMask(HAL_COMREGS_XCPU_IRQ_SLOT_1, dspDoneMask);
    //hal_ComregsSetIrqHandler(HAL_COMREGS_XCPU_IRQ_SLOT_1, pal_DspDoneIrqHandler);

    PAL_TRACE(_PAL|TSTDOUT, 0,"*********** PAL First Init ***********");
    PAL_TRACE(_PAL|TSTDOUT, 0, "Mobile code info :");
    PAL_TRACE(_PAL|TSTDOUT, 0, "RAM allocated : %d",(UINT32) &_ram_bss_end - (UINT32) &_ram_start);
    PAL_TRACE(_PAL|TSTDOUT, 0, "Sys SRAM allocated : %d",(UINT32) &_sys_sram_bss_end - (UINT32) &_sys_sram_start);
    PAL_TRACE(_PAL|TSTDOUT, 0, "Flash used : %d",(UINT32) &_flash_end - (UINT32) &_flash_start);
    PAL_TRACE(_PAL|TSTDOUT, 0, "HEAP0 Size : %d",(UINT32) &_heap_size);
    PAL_TRACE(_PAL|TSTDOUT, 0, "HEAP1 Size : %d",(UINT32) &_sys_sram_heap_size);
    PAL_TRACE(_PAL|TSTDOUT, 0, "Mailbox infos size is %d Bytes", mBoxSize);
    PAL_TRACE(_PAL|TSTDOUT, 0, "PLL_LOCK_TIME: %d Qb", pal_pll_lock_time);
}

VOID pal_CalibGetDcOffset(INT16 *dco_I, INT16 *dco_Q)
{
    *dco_I = g_mailbox.spc2pal.win[g_palCtx.FrameResult].rx.burstRes.dco_I;
    *dco_Q = g_mailbox.spc2pal.win[g_palCtx.FrameResult].rx.burstRes.dco_Q;
}

/// @todo: doc
// imported from Total_integration
VOID pal_SetPcsFlag (BOOL flag)
{
    g_palCtx.PcsH_DcsL = flag;
}

//======================================================================
// pal_LimitMaxTxPower
// This function used to limit the maximum Tx power
//----------------------------------------------------------------------
VOID pal_LimitMaxTxPower(BOOL on)
{
    rfd_XcvLimitMaxTxPower(on);
}

//======================================================================
// pal_LimitMaxTxPower
// This function used to judge the two or Tri or four Band Search support
//----------------------------------------------------------------------

UINT8  pal_GetTriBandSearchSupport(VOID)
{
    return g_TriBandSearchSuppor;
}

VOID pal_SetUSF(UINT8 USF0, UINT8 USF1,UINT8 USF2,UINT8 USF3)
{
#if ((defined GCF_TEST)&&(defined ENABLE_PATCH_SPP_GETUSF))
    g_mailbox_pal2spc_statWin_rx_Usf[0] = USF0;
    g_mailbox_pal2spc_statWin_rx_Usf[1] = USF1;
    g_mailbox_pal2spc_statWin_rx_Usf[2] = USF2;
    g_mailbox_pal2spc_statWin_rx_Usf[3] = USF3;
#endif
}


//======================================================================
//  pal_CloseToNextFINT
// This function used to check whether it is close to next FINT, compare the time left with threshold
//----------------------------------------------------------------------
 UINT8 pal_CloseToNextFINT(VOID)
{
 #define MIN_QB_FOR_L1 1000
  UINT32 CurVal=hal_TcuGetCount();
  if(CurVal>g_palFrame.WrapValue)
  	return 1;
  else return( ((g_palFrame.WrapValue-CurVal)<MIN_QB_FOR_L1)?1:0);
}


#ifdef CHIP_SAIC_CTRL_BY_SOFT
//======================================================================
//  pal_EnableSaic
// This function used to update saic status according the state of pll
//----------------------------------------------------------------------
//======================================================================
VOID pal_EnableSaic(UINT32 flag)
{
    if((hwp_sysCtrl->Pll_Ctrl & SYS_CTRL_PLL_ENABLE) == SYS_CTRL_PLL_ENABLE_POWER_DOWN)
    {
    	  g_mailbox.spc2pal.debug[1] = MBOX_SAIC_STATE_REQ_SET(0);
    }
    else
    {
    	  g_mailbox.spc2pal.debug[1] = MBOX_SAIC_STATE_REQ_SET(flag);
    }
}
#endif

