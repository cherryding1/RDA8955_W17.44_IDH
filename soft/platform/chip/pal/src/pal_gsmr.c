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
#include "string.h"
#include "gsm.h"
#include "baseband_defs.h"

// HAL includes
#include "hal_speech.h"
#include "hal_comregs.h"
#include "hal_rfspi.h"
#include "hal_tcu.h"
#include "hal_lps.h"
#include "hal_sys.h"
#include "hal_clk.h"
#include "hal_fint.h"
#ifdef DCDC_FREQ_DIV_WORKAROUND
#include "hal_rda_abb.h"
#endif // DCDC_FREQ_DIV_WORKAROUND

// PAL includes
#include "pal_gsm.h"
#include "pal.h"
#include "pal_internal.h"
#include "hal_debugpaltrc.h"
#include "palp_cfg.h"

// RFD includes
#include "rfd_defs.h"
#include "rfd_xcv.h"
#include "rfd_pa.h"
#include "rfd_sw.h"

// SIMD includes
#include "simd_m.h"

// SPC includes

// SPP includes

// OS includes
#include "sxr_jbsd.h"
#include "sxr_mem.h"
#include "sxs_io.h"

// Calibration
#include "calib_m.h"

// Assert output strings
// Declared here as 'const' to be excluded from code (hence from IntSRAM)
const char pal_assert_string_0[] = "palReorderWin: Window already programmed. Type %d WinIdx %d";
const char pal_assert_string_1[] = "FrameStart: got 2nd FrameStart without FrameEnd";
const char pal_assert_string_2[] = "FrameEnd: frame already stopped";
const char pal_assert_string_3[] = "FINT received before FrameEnd with WinNb %d";
const char pal_assert_string_4[] = "Too much TCU events recorded %d";
const char pal_assert_string_5[] = "TimeTuning: time offset too high %d";
const char pal_assert_string_6[] = "SetMonRxWin: bad WinIdx. %d";
const char pal_assert_string_7[] = "GetMonResult: bad winIdx. %d";
const char pal_assert_string_8[] = "SetFchRxWin: bad winIdx. %d";
const char pal_assert_string_9[] = "GetSchResult: Result not yet available. %d";
const char pal_assert_string_10[] = "SetIntRxWin: nbSlots too large. %d";
const char pal_assert_string_11[] = "SetIntRxWin: bad WinIdx. %d";
const char pal_assert_string_12[] = "GetIntResult: bad winIdx. %d";
const char pal_assert_string_13[] = "ABurstEncoding: bad BufIdx. %d";
const char pal_assert_string_14[] = "NblockEncoding: bad bufIdx. %d";
const char pal_assert_string_15[] = "pal_SynchronizationChange: TCU Wrap value too large %d";
const char pal_assert_string_16[] = "Negative time for first win (%d)";
const char pal_assert_string_17[] = "pal_SetTxWin: Null TxBitmap @ win %d";
const char pal_assert_string_18[] = "pal_SetTxWin: Invalid BufIdx %d in RouteMap %d BitMap %d @ win %d";

#define PAL_ASSERT_STRING(n) pal_assert_string_##n

#ifdef  LOW_TEMPERATURE_RETRY_5_TIMES
uint8 pal_fch_sync_max_attempt = 15;
#else
#ifdef FREQ_LARGESCALE_SUPPORT
uint8 pal_fch_sync_max_attempt = 9;
#else
uint8 pal_fch_sync_max_attempt = 6;
#endif
#endif
//--------------------------------------------------------------------//
//                                                                    //
//                       PAL Internal Structures                      //
//                                                                    //
//--------------------------------------------------------------------//

PAL_CONTEXT_T g_palCtx =
{
    .FchOpened=FALSE,
    .SynchedOnce=FALSE,
    .PcsH_DcsL=TRUE,
    .Afc.UpdtPending=FALSE,
    .Afc.Update=FALSE,
    .FrameValid=0,
    .FrameResult=0,
    .palBcpuSysFreqNext=HAL_SYS_FREQ_32K,
    .palBcpuSysFreqCurr=HAL_SYS_FREQ_32K,
    .palTuSuspend = PAL_TU_SUSPEND_IDLE,
#ifdef USE_BB_DC_CANCEL
    .DCOff_I = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}},
    .DCOff_Q = {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}}
#endif
};

// Stores Frame Informations
PAL_FRAME_T g_palFrame;

// Stores all Debug linked data
PAL_DEBUG_T g_palDebug =
{
    .FirstInit=TRUE,
    .RFEnabled=FALSE,
    .FrameStarted=FALSE,
    .Req=0,
    .ReqStack=0,
    .ReqOngoing=0,
    .RfspiFlushFifo=FALSE,
    .calibProcessEnable = TRUE
};

const UINT8 palSizeFromCs[PAL_CS_QTY] =
{
    PAL_CS_1_SIZE,
    PAL_CS_2_SIZE,
    PAL_CS_3_SIZE,
    PAL_CS_4_SIZE
};

const UINT8 palSizeFromCSD[PAL_CSD_QTY] =
{
    PAL_CSD_2400_SIZE,
    PAL_CSD_4800_SIZE,
    PAL_CSD_9600_SIZE,
    PAL_CSD_14400_SIZE
};


#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
#ifdef DCDC_FREQ_DIV_WORKAROUND

extern DCDC_FREQ_DIV_VALUE g_halRxDcdcFreqDivValue[];

#ifndef PAL_WINDOWS_LOCK_MCP_ON_RX
extern UINT32 g_halNBurstEndDate[];
#endif // !PAL_WINDOWS_LOCK_MCP_ON_RX

CONST PRIVATE UINT16 palDcdcFreqDiv32Arfcn_850[] =
{
    137, 148, 159, 180, 202, 213, 224, 245,
};

CONST PRIVATE UINT16 palDcdcFreqDiv32Arfcn_900[] =
{
    0, 5, 10, 16, 27, 32, 37, 38, 43, 48, 59, 65, 70,
    75, 80, 81, 84, 92, 97, 102, 103, 108, 109, 113,
    124, 975, 986, 991, 1002, 1007, 1018,
};

CONST PRIVATE UINT16 palDcdcFreqDiv12Arfcn_900[] =
{
    1, 21, 54, 86, 119, 980, 1013,
};

CONST PRIVATE UINT16 palDcdcFreqDiv32Arfcn_1800[] =
{
    516, 521, 526, 531, 532, 536, 543, 548, 553, 554,
    559, 560, 564, 575, 581, 585, 586, 591, 597, 608,
    613, 618, 619, 624, 629, 640, 641, 646, 651, 656,
    661, 662, 666, 673, 678, 683, 684, 689, 694, 705,
    711, 716, 721, 727, 738, 743, 748, 749, 754, 759,
    770, 771, 776, 781, 786, 791, 792, 796, 803, 808,
    813, 814, 819, 824, 835, 841, 846, 851, 857, 868,
    873, 878, 879, 884, 885,
};

CONST PRIVATE UINT16 palDcdcFreqDiv12Arfcn_1800[] =
{
    537, 570, 602, 635, 667, 700, 732, 765, 797, 830,
    862, 873,
};

#endif // DCDC_FREQ_DIV_WORKAROUND
#endif // GALLITE_IS_8805 or GALLITE_IS_8806

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
#ifdef DDR_FREQ_260M_WORKAROUND
extern UINT8 g_halApDDR260MDitherFlag[];
#endif

#ifdef GOUDA_TIMING_WORKAROUND
extern UINT8 g_halApGoudaTimingDitherFlag[];

CONST PRIVATE UINT16 palGoudaTimingArfcn_900[] =
{
    12, 13, 43, 44, 75, 106, 107, 1005, 1006,
};

CONST PRIVATE UINT16 palGoudaTimingArfcn_1800[] =
{
    517, 548, 549, 580, 586, 611, 626, 642, 673, 674,
    705, 716, 736, 737, 767, 768, 798, 799, 830, 846,
    861,
};
#endif
#endif

// Used to keep win info for FCCH close function
RFD_WIN_T FcchWin ;

PRIVATE BOOL g_palAuxClkActionWithRf = FALSE;
PRIVATE BOOL g_palAuxClkEnabled = FALSE;

UINT8 g_fcch_detect_mode = 0; //global varible used by fcch patch.

#ifdef USE_BB_DC_CANCEL
EXPORT INT16  g_mailbox_spc2pal_win_rx_burstRes_dcI[MBOX_CTX_SNAP_QTY];
EXPORT INT16  g_mailbox_spc2pal_win_rx_burstRes_dcQ[MBOX_CTX_SNAP_QTY];
PAL_DC_T g_palDC =
{
    {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}},
    {{0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}}
};
CONST PRIVATE INT16 g_DigG2Amp[PAL_DC_MAXDIGLEV + 1] =
{
    8192,7301,6507,5799,5169,4607,4106,3659,3261,2907,2591,2309,
    2058,1834,1635,1457,1298,1157,1031,919,819,730,651,580,517
};
#define DC_AMP_SHIFT (13)
#endif

#ifdef XCV_PLL_SDM_MODE
#if ((defined CHIP_DIE_8909) || (defined CHIP_DIE_8955) || (defined CHIP_DIE_8809E2))
#define FOF_THRESHOLD_UPPER (2000)
#define FOF_THRESHOLD_LOWER (-2000)
#else
#define FOF_THRESHOLD_UPPER (600)
#define FOF_THRESHOLD_LOWER (-600)
#endif
#endif
#if (defined CHIP_DIE_8955)
    extern UINT8 g_mailbox_pal2spc_rxBandInd;
#endif
//--------------------------------------------------------------------//
//                                                                    //
//                      PAL Internal functions                        //
//                                                                    //
//--------------------------------------------------------------------//


#ifdef PAL_WINDOW_EXTENSION_FOR_AHB_LATENCY

#define PAL_TIME_RFIF_TO_IFC2_WRITE_ON_AHB_LATENCY_ANARF     32
#define PAL_TIME_RFIF_TO_IFC2_WRITE_ON_AHB_LATENCY_DIGRF     64

VOID palSetBcpuTcuIrqAfterWinEndDate(INT32 WinEndDate)
{
    UINT16 time;
    time = WinEndDate + PAL_TIME(PAL_TIME_RF_IN_DN);
    if (g_palCtx.xcv->digRf.used == TRUE)
    {
        // in case of digital rf the TCU has to be delayed because later IFC2 on bcpu
        time += PAL_TIME_RFIF_TO_IFC2_WRITE_ON_AHB_LATENCY_DIGRF;
    }
    else
    {
        time += PAL_TIME_RFIF_TO_IFC2_WRITE_ON_AHB_LATENCY_ANARF;
    }
    hal_TcuSetEvent (HAL_TCU_TRIGGER_BCPU_SOFTIRQ1,time);
    palSetUpperBound (time);
}

#endif

#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
#ifdef DCDC_FREQ_DIV_WORKAROUND
#define CHECK_DCDC_FREQ_DIV_VALUE(div, band, arfcn) \
    for (i=0; i<sizeof(palDcdcFreqDiv##div##Arfcn_##band)/sizeof(UINT16); i++) \
    { \
        if (palDcdcFreqDiv##div##Arfcn_##band[i] == arfcn) \
        { \
            return DCDC_FREQ_DIV_VALUE_##div; \
        } \
    }

PRIVATE DCDC_FREQ_DIV_VALUE pal_GetRxDcdcFreqDivValue(GSM_RFBAND_T band, UINT16 arfcn)
{
    int i;
    if (band == GSM_BAND_GSM850)
    {
        CHECK_DCDC_FREQ_DIV_VALUE(32, 850, arfcn);
    }
    else if (band == GSM_BAND_GSM900)
    {
        CHECK_DCDC_FREQ_DIV_VALUE(32, 900, arfcn);
        CHECK_DCDC_FREQ_DIV_VALUE(12, 900, arfcn);
    }
    else if (band == GSM_BAND_DCS1800)
    {
        CHECK_DCDC_FREQ_DIV_VALUE(32, 1800, arfcn);
        CHECK_DCDC_FREQ_DIV_VALUE(12, 1800, arfcn);
    }

    return DCDC_FREQ_DIV_VALUE_24;
}
#endif // DCDC_FREQ_DIV_WORKAROUND
#endif // GALLITE_IS_8805 or GALLITE_IS_8806

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
#ifdef GOUDA_TIMING_WORKAROUND
PRIVATE BOOL pal_GetGoudaTimingFlag(GSM_RFBAND_T band, UINT16 arfcn)
{
    int i;
    if (band == GSM_BAND_GSM900)
    {
        for (i = 0; i < ARRAY_SIZE(palGoudaTimingArfcn_900); i++)
        {
            if (arfcn == palGoudaTimingArfcn_900[i])
            {
                return TRUE;
            }
        }
    }
    else if (band == GSM_BAND_DCS1800)
    {
        for (i = 0; i < ARRAY_SIZE(palGoudaTimingArfcn_1800); i++)
        {
            if (arfcn == palGoudaTimingArfcn_1800[i])
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}
#endif
#endif

// *** palTimersUpdate (VOID) ***
// Update timers in mailbox
INLINE VOID
palTimersUpdate (VOID)
{
    g_palCtx.NextCounter.T2++;
    if (g_palCtx.NextCounter.T2 == PAL_T2_MODULO)
    {
        g_palCtx.NextCounter.T2 = 0;
    }
    g_palCtx.NextCounter.T3++;
    if (g_palCtx.NextCounter.T3 == PAL_T3_MODULO)
    {
        g_palCtx.NextCounter.T3 = 0;
    }
    if ((g_palCtx.NextCounter.T2 | g_palCtx.NextCounter.T3) == 0)
    {
        g_palCtx.NextCounter.T1++;
        if (g_palCtx.NextCounter.T1 == PAL_T1_MODULO)
        {
            g_palCtx.NextCounter.T1 = 0;
        }
    }
    g_mailbox.pal2spc.counters[g_palCtx.FrameValid] = g_palCtx.NextCounter;
}

// *** palAdjustWrap (VOID) ***
// Move back the FINT if needed and possible
INLINE VOID
palAdjustWrap (VOID)
{
    g_palFrame.WrapValue = PAL_TCU_DEFAULT_WRAP_COUNT;

    // We "pushed" the Fint to keep a window in frame
    if (g_palFrame.lastStop > g_palFrame.WrapValue)
    {
        // We have to shift back TCU wrap
        g_palCtx.internalTimeShift +=
            (g_palFrame.lastStop - g_palFrame.WrapValue);
        g_palFrame.WrapValue = g_palFrame.lastStop;
    }
    else
    {
        // No shift so we try to come back
        int offset = g_palFrame.WrapValue - g_palFrame.lastStop;
        if (offset > g_palCtx.internalTimeShift)
        {
            // All the shift can be undone
            g_palFrame.WrapValue -= g_palCtx.internalTimeShift;
            g_palCtx.internalTimeShift = 0;
        }
        else
        {
            // Partial FINT resynch
            g_palFrame.WrapValue -= offset;
            g_palCtx.internalTimeShift -= offset;
        }
    }

    g_palCtx.SetUpTime = PAL_HW_SETUP - g_palCtx.internalTimeShift;
    g_palFrame.lastStop = 0;

    hal_TcuSetWrap (g_palFrame.WrapValue);
    hal_SysSetTcuWrapValue(g_palFrame.WrapValue);
}


// *** palBuildReorderedCtx (VOID) ***
//  This function manage some actions that need
//  the final and time-ordered set of Windows:
//  Fill BCPU Rx context in mailbox
//  Program the RX/TX/Band switch and Ramps
INLINE VOID
palBuildReorderedCtx(VOID)
{
    int i;
    int RxIdx = 0;
#ifdef CHIP_HAS_EXTRA_WINDOWS
    int RxIdxExt = 0;
#endif
    UINT8 snap =  g_palCtx.FrameValid;
    RFD_WIN_T *Win = &(g_palFrame.Win[g_palFrame.WinOrder[0]]);

    // First check for negative time
    if (g_palFrame.WinNb)
    {
        INT16 realSetup;
        realSetup =
            ((Win->type==GSM_WIN_TYPE_TXABURST)||(Win->type==GSM_WIN_TYPE_TXNBURST))?
            Win->date + g_palCtx.SetUpTime + g_palCtx.winBounds.txSetup:
            Win->date + g_palCtx.SetUpTime + g_palCtx.winBounds.rxSetup;
        PAL_ASSERT((realSetup > 0), PAL_ASSERT_STRING(16), realSetup);
    }

    // Init BCPU context
    g_mailbox.pal2spc.win[snap].rx.qty = 0;
    for (i = 0; i < PAL_WINDOWS_QTY; i++)
    {
        g_mailbox.pal2spc.win[snap].rx.rxTypeWin[i] = 0;
    }
#ifdef CHIP_HAS_EXTRA_WINDOWS
    g_mailbox.extRef->pal2spc.rxwin[snap].qty = 0;
    for (i = 0; i < PAL_WINDOWS_QTY; i++)
    {
        g_mailbox.extRef->pal2spc.rxwin[snap].rxTypeWin[i] = 0;
    }
#endif

    // Go through the reordered windows to set up optimaly Rf Switches
    // and fill BCPU context
    for (i=0; i<g_palFrame.WinNb; i++)
    {
        UINT8 order = g_palFrame.WinOrder[i];
        UINT8 BitMap;
        RFD_RFDIR_T RfDirUp,RfDirDn;
        UINT32 idx;
        Win = &(g_palFrame.Win[order]);


        // TX Window
        if ((Win->type == GSM_WIN_TYPE_TXABURST) ||
                (Win->type == GSM_WIN_TYPE_TXNBURST))
        {
            RfDirUp = RFD_DIR_TX;
            RfDirDn = RFD_DIR_TX_DOWN;
            rfd_PaSetRamps(Win, Win->date + g_palCtx.SetUpTime);
        }
        // RX Windows
        else
        {
            RfDirUp = RFD_DIR_RX;
            RfDirDn = RFD_DIR_RX_DOWN;
            // mailbox context
            if(RxIdx < PAL_WINDOWS_QTY)
            {
                g_mailbox.pal2spc.win[snap].rx.monWinIdx[RxIdx] = order;
                g_mailbox.pal2spc.win[snap].rx.rxTypeWin[RxIdx] = Win->type;
                g_mailbox.pal2spc.win[snap].rx.swapIQ[RxIdx] = Win->swapIQ;
                g_mailbox.pal2spc.win[snap].rx.qty++;
                RxIdx++;
            }
#ifdef CHIP_HAS_EXTRA_WINDOWS
            else
            {
                g_mailbox.extRef->pal2spc.rxwin[snap].monWinIdx[RxIdxExt] = order;
                g_mailbox.extRef->pal2spc.rxwin[snap].rxTypeWin[RxIdxExt] = Win->type;
                g_mailbox.extRef->pal2spc.rxwin[snap].swapIQ[RxIdxExt] = Win->swapIQ;
                g_mailbox.extRef->pal2spc.rxwin[snap].qty++;
                RxIdxExt++;
            }
#endif
        }
#if (SPC_IF_VER >= 4)
        if (Win->type == GSM_WIN_TYPE_FCCH)
        {
            g_mailbox.pal2spc.FchSearchMode = g_fcch_detect_mode;
        }
#endif
        // Bitmap
        BitMap = Win->bitmap << 1;

        // RX/TX/Band Switch
        for (idx=0; idx<PAL_TS_IN_BM_QTY; idx++)
        {
            UINT8 prev_slot = BitMap & 0x1;
            UINT8 curr_slot = BitMap & 0x2;
            UINT8 next_slot = BitMap & 0x4;
            // Slot is active...
            if (curr_slot)
            {
                // ...previous was not
                if (!prev_slot)
                {
                    int date =  Win->date + g_palCtx.SetUpTime;

                    if (Win->type==GSM_WIN_TYPE_FCCH)
                    {
                        date -= 4*BB_FCCH_PRE_OPENING;
                    }
                    rfd_SwSetPosition(    RfDirUp, Win->band,
                                          (idx * PAL_TS_DURATION) + date);
                }
                // ...next is not
                if (!next_slot)
                {
                    rfd_SwSetPosition(    RfDirDn, Win->band,
                                          ((idx+1) * PAL_TS_DURATION) + Win->date + g_palCtx.SetUpTime);
                }
            }
            BitMap >>= 1;
        }
        //hal_DbgTrace( _PAL|TSTDOUT, 0, "%d %08x",order,(UINT32)Win);
    }
}

// *** palLpsFrameEndChecks()
// Called in Frame End to do some checkings on Low Power Stuff
INLINE VOID
palLpsFrameEndChecks (VOID)
{
    // Is Frame-suspension allowed ?

    // .TCU wrap must be regular,
    // .No Tcu event programmed for next frame,
    // The TCU events of current frame will be executed anyway.
    if ( (g_palFrame.WrapValue == PAL_TCU_DEFAULT_WRAP_COUNT)
            && (hal_TcuGetEvtPtr() == 0) )
    {
        PAL_PROFILE_WINDOW_ENTRY(SkipFrameAllowed);
        hal_LpsAllowSkipFrame(TRUE, g_palCtx.activeLastStop);
    }
    else
    {
        PAL_PROFILE_WINDOW_EXIT(SkipFrameAllowed);
        hal_LpsAllowSkipFrame(FALSE, g_palCtx.activeLastStop);
    }
    // store programmed frame last event in context
    g_palCtx.activeLastStop = g_palFrame.lastStop;
}

// *** palReorderWin (int WinIdx) ***
// WinIndex: window index
VOID
palReorderWin (int WinIdx)
{
    int j,k;

    PAL_ASSERT((g_palFrame.Win[WinIdx].type == (GSM_WIN_TYPE_T)PAL_UNDEF_VALUE),
               PAL_ASSERT_STRING(0), g_palFrame.Win[WinIdx].type, WinIdx);

    // Win ordering
    g_palFrame.WinOrder[g_palFrame.WinNb] = WinIdx;
    for (j=0; j<g_palFrame.WinNb; j++)
    {
        if (g_palFrame.Win[WinIdx].date < g_palFrame.Win[g_palFrame.WinOrder[j]].date)
        {
            for (k=g_palFrame.WinNb; k>j; k--)
            {
                g_palFrame.WinOrder[k] = g_palFrame.WinOrder[k-1];
            }
            g_palFrame.WinOrder[j] = WinIdx;
            break;
        }
    }
    g_palFrame.WinNb++;
}

VOID
palRxBstOn(INT16 start)
{
    start += g_palCalibration->xcv->palcust.rxIqTimeOffset;
    if (g_palCtx.xcv->dcocMode & RFD_HW_DCOC_ENABLED)
    {
        // This is for DC Offset calibration based on "pre-reception" levels
        hal_TcuSetEvent (HAL_TCU_RX_SOC, start + g_palCalibration->xcv->palcust.dcoCalTime);
        // TODO : should check/adjust ADC UP time to be sure it's ON when we do this
        // pre- DCO calibration
    }

#if (CHIP_HAS_ANA_RF)
    // I/Q ADC Enable
    if (! g_palCtx.xcv->digRf.used)
        hal_TcuSetEvent (HAL_TCU_ENABLE_IQ_ADC, start + PAL_TIME(PAL_TIME_ADC_UP));
#endif
    // RF sampling On
    // In DigRf the recording is activated by the Xcver itself
    if (!g_palCtx.xcv->digRf.used)
    {
        hal_TcuSetEvent (HAL_TCU_START_RFIN_RECORD, start + PAL_TIME(PAL_TIME_RF_IN_UP));
    }
}

VOID
palRxBstOff(INT16 stop)
{
    stop += g_palCalibration->xcv->palcust.rxIqTimeOffset;
    // Flag end of window
    palSetUpperBound (stop + g_palCtx.winBounds.rxHold);
    // RF Sampling off
    // In DigRf the recording is desactivated by the Xcver itself
    if (!g_palCtx.xcv->digRf.used)
    {
        hal_TcuSetEvent (HAL_TCU_STOP_RFIN_RECORD, stop + PAL_TIME(PAL_TIME_RF_IN_DN));
    }
#if (CHIP_HAS_ANA_RF)
    // I/Q ADC Disable
    if (! g_palCtx.xcv->digRf.used)
        hal_TcuSetEvent (HAL_TCU_DISABLE_IQ_ADC, stop + PAL_TIME(PAL_TIME_ADC_DN));
#endif
}

// *** palProgramRxWin (int WinIdx) ***
//  WinIndex: window index
//  this function programs TCU events needed for RX windows
//  The RF SPI fifo is NOT filled here (done in pal_FrameEnd)
VOID
palProgramRxWin (int WinIdx)
{
    RFD_WIN_T *Win = &(g_palFrame.Win[WinIdx]);
    int Windate;
    int WinEndDate = 0;
    int Wintype = Win->type;
    UINT8 XcvGain, XcvAnaLev, XcvDigLev;
    HAL_SYS_FREQ_T nextBcpuFreq = HAL_SYS_FREQ_32K;
    UINT8 DcBand = ((Win->band == GSM_BAND_GSM850) || (Win->band == GSM_BAND_GSM900))? 0:1;
    UINT32 curFrame = hal_ComregsGetSnap();
    // Pre-opening on FCCH windows
    // this is to open a window that is a multiple of the granularity
    // of FCCH search algorithm.
    // FIXME:
    // This can lead to problems if another window is programmed before
    // FCCH and ends at less than PLL_LOCK_TIME+FCCH_PRE_OPENING from
    // the FCCH window.
    // This issue can not be handled by PAL, thus if the case actually
    // happens it will require a L1 modification.
    Windate = (Wintype == GSM_WIN_TYPE_FCCH)?
              Win->date + g_palCtx.SetUpTime - 4*BB_FCCH_PRE_OPENING:
              Win->date + g_palCtx.SetUpTime;

    if(PAL_TU_SUSPEND_LOCK == g_palCtx.palTuSuspend)
    {
        if(g_palCtx.palTuFrame == curFrame)
        {
              pal_TimeTuning(g_palCtx.palTuVal);
              g_palCtx.palTuVal = 0;
              g_palCtx.palTuSuspend = PAL_TU_SUSPEND_IDLE;
        }
        else
        {
              Windate += g_palCtx.palTuVal; 
              g_palCtx.palTuSuspend = PAL_TU_SUSPEND_WAITFINT;
        }
    }
    // Set bitmap to this special value to avoid to switch
    // the RF switch to the RF_TX_DOWN position after one burst
    Win->bitmap = (Wintype == GSM_WIN_TYPE_FCCH)?PAL_FCCH_BITMAP:Win->bitmap;
    Win->winIdx = WinIdx;

    // Record first TCU event used for this window
    Win->firstTcu = hal_TcuGetEvtPtr();

    // Rx Window opening is common to all win types
    // Prepare Xcver for Rx win
#ifdef USE_BB_DC_CANCEL
    XcvGain = rfd_XcvRxOn(Win,Windate,g_palCtx.DCOff_I[DcBand], g_palCtx.DCOff_Q[DcBand], &XcvAnaLev, &XcvDigLev);
    PAL_TRACE(PAL_LEV(7), 0, "gain:%d, Arfcn:%d, XcvAnaLev:%d, XcvDigLev:%d, rxdcI:%d, rxdcQ:%d",
              Win->gain[0], Win->arfcn, XcvAnaLev, XcvDigLev, g_palCtx.DCOff_I[DcBand][XcvAnaLev], g_palCtx.DCOff_Q[DcBand][XcvAnaLev]);
#else
    XcvGain = rfd_XcvRxOn(Win,Windate);
#endif
    // Burst(s) Recording and window's end depends on win type
    switch(Wintype)
    {
        case GSM_WIN_TYPE_FCCH:
            // Program only window start (multiframe window)
            // multiframe window: we ensure start is not wrapped-out (FCCH)
            // use pre opening for in-TCH syncro needs
            palSetUpperBound (Windate);
            // Burst recording
            palRxBstOn(Windate);
            // Save a copy for later closing of the window
            // memcpy (&FcchWin,Win,sizeof(RFD_WIN_T));
            // TODO:for now copy only usefull fields
            FcchWin.arfcn = Win->arfcn;
            FcchWin.band = Win->band;
            FcchWin.type = Win->type;
            // at least 39M needed for FCCH processing
            nextBcpuFreq = PAL_BCPU_SYS_FREQ_HIGH_ACTIVITY;
            break;

        case GSM_WIN_TYPE_SCH:
            // Burst recording
            palRxBstOn(Windate);
            WinEndDate = Windate + PAL_SCH_WIN_DURATION;

#ifdef PAL_WINDOW_EXTENSION_FOR_AHB_LATENCY
            palSetBcpuTcuIrqAfterWinEndDate(WinEndDate);
#endif

            palRxBstOff(WinEndDate);
            // Save Rx gain
            g_palCtx.SchGain = XcvGain;
            // 26M should be OK if the SCH is programmed early
            nextBcpuFreq = (Windate < PAL_TIME_LATE_WINDOW_THRESHOLD) ?
                           PAL_BCPU_SYS_FREQ_DEFAULT :
                           PAL_BCPU_SYS_FREQ_HIGH_ACTIVITY;
            break;

        case GSM_WIN_TYPE_MONIT:
#ifdef CHIP_HAS_EXTRA_WINDOWS
        case GSM_WIN_TYPE_MONIT_EXT:
#endif
            // Burst recording
            palRxBstOn(Windate);
            WinEndDate = Windate + PAL_MON_WIN_DURATION;
            palRxBstOff(WinEndDate);
            // save gain in case of predictive AGC
            g_palCtx.MonGain[g_palCtx.FrameValid][Win->winIdx] = XcvGain;
            // 26M is OK for monitoring
            nextBcpuFreq = PAL_BCPU_SYS_FREQ_DEFAULT;
            break;

        case GSM_WIN_TYPE_INTRF:
        {
            int i;
            int tmpWindate = Windate;
            // Open first recording...
            palRxBstOn(Windate);

            // ...then generate "recording holes" and AGC setup
            for(i=0; i<g_palCtx.IntrfSlotQty-1; i++)
            {
                // RF sampling Off
                // In DigRf the recording is desactivated by the Xcver itself
                if (!g_palCtx.xcv->digRf.used)
                {
                    hal_TcuSetEvent (HAL_TCU_STOP_RFIN_RECORD, tmpWindate +
                                     PAL_TIME(PAL_TIME_RF_IN_UP) +
                                     PAL_INTRF_DURATION);
                }

                tmpWindate += PAL_TS_DURATION ;

                // RF sampling On
                // In DigRf the recording is activated by the Xcver itself
                if (!g_palCtx.xcv->digRf.used)
                {
                    hal_TcuSetEvent (HAL_TCU_START_RFIN_RECORD, tmpWindate +
                                     PAL_TIME(PAL_TIME_RF_IN_UP));
                }
            }
            // Close last recording
            WinEndDate =    Windate + PAL_TS_DURATION * (g_palCtx.IntrfSlotQty-1) +
                            PAL_INTRF_DURATION;
            palRxBstOff(WinEndDate);
            // TODO : is 26M OK?
            nextBcpuFreq = PAL_BCPU_SYS_FREQ_DEFAULT;
        }
        break;

        case GSM_WIN_TYPE_NBURST:
        {
            UINT8 Winbitmap = (0xF & Win->bitmap) << 1;
            UINT8 prev_slot,curr_slot,next_slot;
            int i;

            // Window depends on the bitmap allocation
            for (i = 0; i < PAL_TS_IN_BM_QTY; i++)
            {
                prev_slot = Winbitmap & 0x1;
                curr_slot = Winbitmap & 0x2;
                next_slot = Winbitmap & 0x4;
                // Slot i is ON and...
                if (curr_slot)
                {
                    if (!prev_slot)
                    {
                        // ... previous slot is OFF
                        palRxBstOn(Windate + (i * PAL_TS_DURATION));
                    }
                    if (!next_slot)
                    {
                        // ... next slot is OFF
                        WinEndDate =    (Windate +
                                         ((i * PAL_TS_DURATION) +
                                          PAL_NBURST_RX_WIN_DURATION));

                        palRxBstOff(WinEndDate);
                    }
                }
                Winbitmap >>= 1;
            }

#ifdef PAL_WINDOW_EXTENSION_FOR_AHB_LATENCY
            palSetBcpuTcuIrqAfterWinEndDate(WinEndDate);
#endif

#ifdef PAL_WINDOWS_LOCK_MCP_ON_RX
#define RX_SLOT_HANDLING_ADVANCE_TIME 30
            // Only lock the first Rx slot for now
            // Generate TCU0 IRQ (RX_SLOT_HANDLING_ADVANCE_TIME) Qb before the Rx slot
            hal_TcuSetEvent (HAL_TCU_TRIGGER_XCPU_SOFTIRQ0,
                             Windate>RX_SLOT_HANDLING_ADVANCE_TIME?Windate-RX_SLOT_HANDLING_ADVANCE_TIME:0);
#else // !PAL_WINDOWS_LOCK_MCP_ON_RX
// Temporarily disable DCDC switching if MCP is not locked, as
// too many TCU interrupts will impact the system performance.
#if 0 //def DCDC_FREQ_DIV_WORKAROUND
            g_halNBurstEndDate[g_palCtx.FrameValid] = WinEndDate;
            // Generate TCU0 IRQ (RX_SLOT_HANDLING_ADVANCE_TIME) Qb before the Rx slot
            hal_TcuSetEvent (HAL_TCU_TRIGGER_XCPU_SOFTIRQ0,
                             Windate>RX_SLOT_HANDLING_ADVANCE_TIME?Windate-RX_SLOT_HANDLING_ADVANCE_TIME:0);
            // Generate another TCU0 IRQ at the end of the Rx slot
            hal_TcuSetEvent (HAL_TCU_TRIGGER_XCPU_SOFTIRQ0, WinEndDate);
#endif // DCDC_FREQ_DIV_WORKAROUND
#endif // !PAL_WINDOWS_LOCK_MCP_ON_RX

#if defined(GALLITE_IS_8805) || defined(GALLITE_IS_8806)
#ifdef DCDC_FREQ_DIV_WORKAROUND
            g_halRxDcdcFreqDivValue[g_palCtx.FrameValid] =
                pal_GetRxDcdcFreqDivValue(Win->band, Win->arfcn);
#endif // DCDC_FREQ_DIV_WORKAROUND
#endif // GALLITE_IS_8805 or GALLITE_IS_8806

#if (CHIP_ASIC_ID == CHIP_ASIC_ID_8810)
#ifdef DDR_FREQ_260M_WORKAROUND
            g_halApDDR260MDitherFlag[g_palCtx.FrameValid] =
                ( (Win->band == GSM_BAND_DCS1800 &&
                   (Win->arfcn >= 581 && Win->arfcn <= 591)) ||
                  (Win->band == GSM_BAND_GSM850 &&
                   (Win->arfcn >= 201 && Win->arfcn <= 203))
                );
#endif

#ifdef GOUDA_TIMING_WORKAROUND
            g_halApGoudaTimingDitherFlag[g_palCtx.FrameValid] =
                pal_GetGoudaTimingFlag(Win->band, Win->arfcn);
#endif
#endif

            // Save Rx chain gain
            g_palCtx.NBGain[g_palCtx.FrameValid] = XcvGain;
            // 26M for normal burst unless programmed late
            nextBcpuFreq = (Windate < PAL_TIME_LATE_WINDOW_THRESHOLD) ?
                           PAL_BCPU_SYS_FREQ_DEFAULT :
                           PAL_BCPU_SYS_FREQ_HIGH_ACTIVITY;
#ifdef USE_BB_DC_CANCEL
            g_palCtx.NBGainAnaLev[g_palCtx.FrameValid] = (XcvAnaLev>(PAL_DC_ANALEV_NUM-1))? (PAL_DC_ANALEV_NUM-1):XcvAnaLev;
            XcvDigLev = (XcvDigLev>(PAL_DC_MAXDIGLEV))?(PAL_DC_MAXDIGLEV):XcvDigLev;
            XcvDigLev = (XcvDigLev<(PAL_DC_MINDIGLEV))?(PAL_DC_MINDIGLEV):XcvDigLev;
            g_palCtx.NBGainDigLev[g_palCtx.FrameValid] = XcvDigLev;
#endif
        }
        break;
    }

    // Close the Rx window
    if (Wintype != GSM_WIN_TYPE_FCCH) // Dont close the window for FCCH
    {
        // Program window end
        rfd_XcvRxOff(Win,WinEndDate);
    }

    // check if the mode is GPRS
    nextBcpuFreq = (g_palCtx.palModeIsGprs) ? PAL_BCPU_SYS_FREQ_GPRS : nextBcpuFreq;

    // take max of all windows
    if (g_palCtx.palBcpuSysFreqNext < nextBcpuFreq)
    {
        g_palCtx.palBcpuSysFreqNext = nextBcpuFreq;
    }

    // Record last TCU event used for this window
    Win->lastTcu = hal_TcuGetEvtPtr() - 1;
}

// *** UINT8 palCalcMonPower(UINT8 WinIdx, UINT16 Arfcn)
// Calculate Rf power received on an Rx window (Monit/Intrf)
// and returns it in -dBm
UINT8
palCalcMonPower(UINT8 WinIdx, UINT16 Arfcn)
{
    UINT8 snap = g_palCtx.FrameResult;
    GSM_RFBAND_T band;
    UINT8 xcver_gain;
    UINT8 result=0;

    // Can not use g_palFrame.Win[WinIdx].band as this function
    // is called at N+2
    band = palGetBand (Arfcn);

    xcver_gain = g_palCtx.MonGain[snap][WinIdx];

    // Add the calculated Power
    if(WinIdx < PAL_WINDOWS_QTY)
    {
        result =  xcver_gain + g_palCtx.Adc2dBmGain -
                  g_mailbox.spc2pal.win[snap].rx.monRes[WinIdx].power;
    }
#ifdef CHIP_HAS_EXTRA_WINDOWS
    else
    {
        result =  xcver_gain + g_palCtx.Adc2dBmGain -
                  g_mailbox.extRef->spc2pal.rxwin[snap].monRes[WinIdx-PAL_WINDOWS_QTY].power;
    }
#endif
    //PAL_TRACE( _PAL|TSTDOUT, 0, "xcver_gain:%d  power:%d",
    //                xcver_gain,
    //                g_mailbox.spc2pal.win[snap].rx.monRes[WinIdx].power);

    return result;
}

// *** VOID palCheckXtalAging(UINT8 Snr)
// Checks that adverage Xtal value (on good cells) still corresponds
// to a mid-range value of AFC DAC or Xcver's internal AFC.
// If on adverage this value is not on par with Xcver's expectations,
// then the XTAL will be changed accordingly at next power on.
#define AGING_NB_MEAS       64 // Number of good measures
#define AGING_SNR_THRESH    19 // Minimum Snr to consider a measure
INLINE VOID
palCheckXtalAging(UINT8 Snr)
{
    static UINT8 xtal_nb_meas = 0;
    static UINT32 xtal_mean_val = 0;

    // TODO : is this small averaging enough to represent aging ?
    if (xtal_nb_meas < AGING_NB_MEAS)
    {
        // TODO : is this filtering enough ? (add fof ?)
        if (Snr > AGING_SNR_THRESH) // Pass Snr threshold
        {
            xtal_nb_meas++;
            xtal_mean_val += g_palCtx.XtalAgeValue;
        }
    }
    else if (xtal_nb_meas == AGING_NB_MEAS)
    {
        // Average Xtal Value
        xtal_mean_val /= AGING_NB_MEAS;

        // Schedule the update to flash
        // FIXME: no Flash access for now...
        // TODO :this should be done at pal_Close or something...
        //palWriteFlashXtal( g_palCtx.XtalAgeValue );
        PAL_TRACE(_PAL | TSTDOUT, 0, "XTAL Aging Comp. XTAL: %d",g_palCtx.XtalAgeValue);

        // only one update can be done at each power on of the phone
        xtal_nb_meas++;
    }
}

// *** UINT8 palCalcPower(UINT8 bstIdx)
// Calculate Rf power received on an NBurst Rx window
// and returns it in -dBm
INLINE UINT8
palCalcPower(UINT8 bstIdx)
{
    UINT8 xcver_gain;
    UINT8 result;
    UINT8 snap = g_palCtx.FrameResult;

    // Get Xcver gain
    xcver_gain = g_palCtx.NBGain[snap];
    result = (xcver_gain + g_palCtx.Adc2dBmGain) -
             g_mailbox.spc2pal.win[snap].rx.burstRes.pwr;

    PAL_TRACE( PAL_LEV(1), 0, "xcver_gain:%ddB calc:%ddB res=-%ddBm",
               xcver_gain,
               g_mailbox.spc2pal.win[snap].rx.burstRes.pwr,
               result);
    return result;
}

VOID palTxBstOn(UINT16 start)
{
#if (CHIP_HAS_ANA_RF)
    // Adjust TX I/Q delay with respect to the window (debug only)
    start += g_palCalibration->xcv->palcust.txIqTimeOffset;
    if (g_palCtx.xcv->digRf.used == FALSE)
    {
        // Handles I/Q DAC Enable
        hal_TcuSetEvent (HAL_TCU_ENABLE_IQ_DAC, start + PAL_TIME(PAL_TIME_DAC_UP));

        // Enable Tx output of the tranceiver
        hal_TcuSetEvent(HAL_TCU_DRIVE_IQ_DAC, start + PAL_TIME(PAL_TIME_TX_OEN_UP));

        // Handles GMSK Enable
        hal_TcuSetEvent (HAL_TCU_START_GMSK, start); // No offset as this is the reference
    }
#endif
}
VOID palTxBstOff(UINT16 stop)
{
#if (CHIP_HAS_ANA_RF)
    // Adjust TX I/Q delay with respect to the window (debug only)
    stop += g_palCalibration->xcv->palcust.txIqTimeOffset;
    if (g_palCtx.xcv->digRf.used == FALSE)
    {
        // Handles I/Q DAC Enable
        hal_TcuSetEvent (HAL_TCU_DISABLE_IQ_DAC, stop + PAL_TIME(PAL_TIME_DAC_DN));

        // Set Tx output of the tranceiver to Hi-Impedence
        hal_TcuSetEvent(HAL_TCU_HIGHZ_IQ_DAC, stop + PAL_TIME(PAL_TIME_TX_OEN_DN));

        // Handles GMSK Enable
        hal_TcuSetEvent (HAL_TCU_STOP_GMSK, stop); // No offset as this is the reference
    }
#endif
}


// *** palProgramTxWin (int WinIdx) ***
//  WinIndex: window index
//  this function programs TCU events needed for TX windows
VOID
palProgramTxWin (int WinIdx)
{
    RFD_WIN_T *Win = &(g_palFrame.Win[WinIdx]);

    int start = 0;
    int stop = 0;
    UINT8 firstSlot =0;

    // Record first TCU event used for this window
    Win->firstTcu = hal_TcuGetEvtPtr();
    Win->winIdx = WinIdx;

    // Apply Rx/Tx offset delay
    Win->date += g_palCalibration->xcv->palcust.rxTxTimeOffset;

    // Just in case bitmap is not odd...
    while (!(Win->bitmap & 0x1))
    {
        Win->bitmap >>= 1;
        Win->date +=PAL_TS_DURATION;
        firstSlot++;
    }

    if(firstSlot)
    {
        UINT8 i;

        // Shift gain if first slot not active.
        for(i=0; i<(MBOX_MAX_TS_QTY-firstSlot); i++)
        {
            Win->gain[i] = Win->gain[firstSlot+i];
        }
    }

    // derive overall window extension
    start = Win->date + g_palCtx.SetUpTime;

    // Calls to RFD to create the window
    rfd_XcvTxOn(Win,start);
    rfd_PaTxOn(start, Win->band);

    // Create 'modulation' windows for analog IQ RF Interface
    // NOTE2: multiple AB (or AB/NB mixes) are not handled
    if (Win->type == GSM_WIN_TYPE_TXABURST)
    {
#if (CHIP_HAS_ANA_RF)
        if (g_palCtx.xcv->digRf.used == FALSE)
        {
            palTxBstOn(start - g_palCtx.xcv->txBstMap.preamble * 4);
            stop = start + PAL_ABURST_TX_ACTIVE_DURATION
                   + g_palCtx.xcv->txBstMap.postamble * 4
                   + g_palCtx.xcv->txBstMap.preamble * 4;
            palTxBstOff(stop);
        }
        else
#endif
        {
            stop = start + PAL_ABURST_TX_ACTIVE_DURATION;
        }
    }
    else // GSM_WIN_TYPE_TXNBURST:
    {
#if (CHIP_HAS_ANA_RF)
        if (g_palCtx.xcv->digRf.used == FALSE)
        {
            UINT8 Winbitmap = (0xF & Win->bitmap) << 1;
            UINT32 i;
            for (i = 0; i < PAL_TS_IN_BM_QTY; i++)
            {
                UINT8 prev_slot,curr_slot,next_slot;
                prev_slot = Winbitmap & 0x1;
                curr_slot = Winbitmap & 0x2;
                next_slot = Winbitmap & 0x4;
                if (curr_slot) // Slot i is ON and...
                {
                    if (!prev_slot) // ... previous slot is OFF
                    {
                        palTxBstOn(start + (i * PAL_TS_DURATION) -
                                   g_palCtx.xcv->txBstMap.preamble * 4);

                    }
                    if (!next_slot) // ... next slot is OFF
                    {
                        stop = start + (i * PAL_TS_DURATION) +
                               PAL_NBURST_TX_ACTIVE_DURATION
                               + g_palCtx.xcv->txBstMap.postamble * 4
                               + g_palCtx.xcv->txBstMap.preamble * 4;
                        palTxBstOff(stop);

                    }
                }
                Winbitmap >>= 1;
            }
        }
        else // In DirRf everything must be done on driver side as
            // every Xcver has specific ways of handling Windows
#endif // CHIP_HAS_ANA_RF
        {
            UINT8 nbBursts = (Win->bitmap & 0x8)? 3:
                             (Win->bitmap & 0x4) ? 2:
                             (Win->bitmap & 0x2) ? 1:0;

            //PAL_TRACE(TSTDOUT, 0, "nbBursts = %d ", nbBursts);
            stop = start + (nbBursts * PAL_TS_DURATION)
                   + PAL_NBURST_TX_ACTIVE_DURATION
                   + g_palCtx.xcv->txBstMap.postamble * 4;

        }
    }

    // Calls to RFD to close the window
    rfd_XcvTxOff(Win,stop );
    rfd_PaTxOff(stop , Win->band);

    // Send IT to BCPU to handle the "out-of-DTX"
    hal_TcuSetEvent (HAL_TCU_TRIGGER_BCPU_SOFTIRQ0, stop + PAL_TIME(PAL_TIME_DTX_OFF_DN));

    // Flag Frame boundary
    palSetUpperBound (stop + g_palCtx.winBounds.txHold);

    // Record last TCU event used for this window
    // XXX: this is actually not the last TCU events as
    //      RF Switch and Ramps events are programmed at
    //      frame end.
    Win->lastTcu = hal_TcuGetEvtPtr() - 1;
}


// *** palResizeBitMap (UINT8 oldBitMap,INT16 winStart,UINT16 stopPos) ***
//  Derive new Bitmap from end date
INLINE UINT8
palResizeBitMap(UINT8 oldBitMap,INT16 winStart,UINT16 stopPos)
{
    UINT8 newBitMap;
    UINT16 duration = stopPos - winStart;

    if (stopPos < winStart) return 0;

    newBitMap = (oldBitMap & (0x0F >> (4 - (duration / PAL_TS_DURATION))));

    return (newBitMap);
}

// *** palSetUpperBound (int stop) ***
//  stop: last event time
//  Memorize the last TCU event of the last window
//  (with a sync-margin to cope with eventual TimeTuning)
//  to know how much we can move back the FINT if needed.
VOID palSetUpperBound (int stop)
{
    if (stop + PAL_TIME_SYNC_MARGIN > g_palFrame.lastStop)
    {
        g_palFrame.lastStop = stop + PAL_TIME_SYNC_MARGIN + 1;
    }
}

// Returns Band from Arfcn
GSM_RFBAND_T palGetBand(UINT16 arfcn)
{
    GSM_RFBAND_T band;

    // Find the band: not checking exactly all the EGSM sub_bands,
    if ((arfcn <= RFD_ARFCN_DCS_MAX) &&
            (arfcn >= RFD_ARFCN_DCS_MIN))
    {
        // Bands DCS and PCS have overlapping arfcn numbers
        band = (g_palCtx.PcsH_DcsL ? GSM_BAND_PCS1900 : GSM_BAND_DCS1800);
    }
    else
    {
        if ((arfcn <= RFD_ARFCN_GSM850_MAX) &&
                (arfcn >= RFD_ARFCN_GSM850_MIN))
            band = GSM_BAND_GSM850;
        else
            band = GSM_BAND_GSM900;
    }
    return band;
}


//--------------------------------------------------------------------//
//                                                                    //
//                         PAL API functions                          //
//                                                                    //
//--------------------------------------------------------------------//

VOID pal_EnableAuxClkCtrl(BOOL on)
{
    g_palAuxClkActionWithRf = on;
}

// Turn off aux clock when starting to skip frame
VOID pal_AuxClkOff(VOID)
{
    if (g_palAuxClkEnabled)
    {
        g_palAuxClkEnabled = FALSE;
        hal_SysAuxClkOut(FALSE);
    }
}

//======================================================================
// Frame boundaries function : pal_FrameStart
//----------------------------------------------------------------------
// Must be called by Layer One at the beginning of a new frame (which
// means at the beginning of the frame interrupt) before invoking any
// PAL control function in the synchronous domain.
//======================================================================
VOID pal_FrameStart (VOID)
{
    HAL_COMREGS_IRQ_MASK_T mask;
    INT32 i;

    // Snap register is read only one time
    UINT32 curFrame = hal_ComregsGetSnap();

    PAL_PROFILE_FUNCTION_ENTRY(pal_FrameStart);
    PAL_PROFILE_WINDOW_ENTRY(PAL_FRAME);

    PAL_ASSERT(g_palDebug.FrameStarted == FALSE, PAL_ASSERT_STRING(1));

    // reset the enable flag for dummy PCH detection
    g_palCtx.palDummyPchDetection=FALSE;

    // request the resource for PAL (the stack)

    hal_SwRequestClk(FOURCC_PAL, (g_palCtx.palModeIsGprs) ? PAL_STACK_SYS_FREQ_GPRS :
            (g_mailbox.pal2spc.statWin.dedicated.active ? PAL_STACK_SYS_FREQ_TCH : PAL_STACK_SYS_FREQ_DEFAULT));


    // save the BCPU ressource for late frames
    g_palCtx.palBcpuSysFreqCurr = g_palCtx.palBcpuSysFreqNext;

    // do not clear requested frequency in FCCH mode (multi-frame)
    if (!g_palCtx.FchOpened)
    {
        g_palCtx.palBcpuSysFreqNext = HAL_SYS_FREQ_32K;
    }

    // Re-enable the frame entrance check mechanism
    g_palDebug.InitPending = FALSE;
    g_palDebug.FrameStarted = TRUE;

    // check Rfspi Fifo Flush
    if (g_palDebug.RfspiFlushFifo)
    {
        g_palDebug.RfspiFlushFifo = FALSE;
        // Reset RFSPI FIFO
        hal_RfspiSetupFifo();
        PAL_TRACE(PAL_LEV(8), 0, "PAL Flushed Rfspi Fifo.");
    }
#ifdef __MBOX_DEBUG__
#define PAL_DBG_REQ_DONE 0x10
    if (g_palDebug.ReqOngoing & PAL_DBG_REQ_DONE)
        g_mailbox.palDbgAck = g_palDebug.Req;
    g_palDebug.ReqOngoing <<= 1;
#endif

    // Calculate snap numbers
    g_palCtx.FrameValid = (curFrame == 2 ? 0 : curFrame + 1);
    g_palCtx.FrameResult = (curFrame == 0 ? 2 : curFrame - 1);
    g_mailbox.pal2spc.win[g_palCtx.FrameValid].rx.nBurst.bitMap = 0;

    // Inform XCV that an FINT just starts
    rfd_XcvFrameStart();

    // Update PA Ramps
    rfd_PaFrameStart();

    // Update timers
    palTimersUpdate ();

    // Shift frame end if needed
    palAdjustWrap ();

    pal_TimeTuningSuspendCheck();
    
#ifdef HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_FRAMESTART,  g_palCtx.NextCounter.T1,
               g_palCtx.NextCounter.T2,
               g_palCtx.NextCounter.T3,
               g_palFrame.WrapValue,
               g_palCtx.internalTimeShift,
               g_palCtx.SetUpTime));
#else // !HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_FRAMESTART,  g_palCtx.NextCounter.T1,
               g_palCtx.NextCounter.T2,
               g_palCtx.NextCounter.T3,0,0,0));
#endif // !HAL_PAL_TRC_TCU_VERBOSE

    // Clean Frame & Window structures
    for (i=0; i<g_palFrame.WinNb; i++)
    {
        palWinInit(g_palFrame.WinOrder[i]);
        g_palFrame.WinOrder[i] = PAL_UNDEF_VALUE;
    }
    g_palFrame.WinNb = 0;

    // Clean Tx context
    g_mailbox.pal2spc.win[g_palCtx.FrameValid].tx.qty = 0;

    // Clean RFSpi queue
    hal_RfspiInitQueue();

    // All TCU events are now free
    hal_TcuSetEvtPtr(0);

    // TODO:
    // Move the job event status initialization to l1s_FrameInterrupt() when
    // BV5 is no longer supported (and the related SX codes are released)
    // ------------------------------
    // Init the job event status before enabling COM0 IRQs
    sxr_InitJobEventStatus();

    // Manage the Equalization interrupt UnMasking
    g_palCtx.EquIrqMap <<= 1;

    // NOTE:
    // The job event status must be initialized before enabling COM0 IRQs
    mask.mask = MBOX_DSP_EQUALIZATION_COMPLETE;
    if (g_palCtx.EquIrqMap & PAL_RX_FRAME_EXPECTED)
    {
        hal_ComregsSetMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, mask);
    }
    else
    {
        hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, mask);
    }
#ifdef CHIP_DIE_8955
    mask.mask = MBOX_DSP_NB_DECODING_COMPLETE;
    hal_ComregsSetMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, mask);
#endif

    // Process the calibration commands received from the calibration
    // tool. If the flag PAL_CALIB is not set, only some commands
    // are available.
    if (g_palDebug.calibProcessEnable)
    {
        calib_DaemonProcess();
    }

    PAL_PROFILE_FUNCTION_EXIT(pal_FrameStart);
}

//======================================================================
// Frame boundaries function : pal_FrameEnd
//----------------------------------------------------------------------
// Called at the end of the L1 logical frame when all the PAL
// synchronous functions are invoked for the current frame.
//======================================================================
VOID pal_FrameEnd (VOID)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_FrameEnd);

    // Don't check this if an init has been done during the frame as
    // this resets the flag
    if (g_palDebug.InitPending == FALSE)
        PAL_ASSERT(g_palDebug.FrameStarted == TRUE, PAL_ASSERT_STRING(2));
    g_palDebug.FrameStarted = FALSE;

#ifdef HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_FRAMEEND,g_palFrame.lastStop,0,0,0,0,0));
#else // !HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_FRAMEEND,0,0,0,0,0,0));
#endif // !HAL_PAL_TRC_TCU_VERBOSE

    // clear TCU event in case of previous Pal_initialisation from L1a
    // in the current frame
    if (g_palDebug.RfspiFlushFifo==TRUE)
    {
        // TCU event reset
        // As this can be called in the middle of a window, don't reset
        // the active part of the TCU events, but only the latch
        hal_TcuClrEvtRange(0,PAL_MAX_TCU_EVENTS);
        hal_TcuSetEvtPtr(0);
        PAL_TRACE(PAL_LEV(8), 0, "PAL Clear latched TCU events on Frame End.");
    }

    // RF Fifo Programming & context for BCPU
    palBuildReorderedCtx();

    // Considering that:
    //  - a frequency tuning asked at frame N will take effect at N+1
    //  - L1 expects that N+1 results from frame N  are including an eventual
    //    frequency tuning (asked a N)
    //  we have to remove last asked correction from results of N+1 in order to
    //  avoid to count twice the reported offset.
    //  UpdtPending flag is used for that purpose.
    if (g_palCtx.Afc.Update)
    {
        g_palCtx.Afc.Update = FALSE;
        g_palCtx.Afc.UpdtPending = TRUE;
    }
    else
    {
        g_palCtx.Afc.UpdtPending = FALSE;
    }

    // Check TCU Overflow
    {
        UINT8 TcuPtr = hal_TcuGetEvtPtr();
        PAL_ASSERT(TcuPtr < PAL_MAX_TCU_EVENTS, PAL_ASSERT_STRING(4), TcuPtr);
    }

#ifdef __LPS_ENABLED__
    palLpsFrameEndChecks();
    if (g_palCtx.SynchedOnce == TRUE)
    {
        // Calibration should be done only when synch with the
        // network has been found at least once.
        // TODO : find a better way of checking this criteria ?
        hal_LpsCalibrate();
    }
#endif

    // Prepare RfSpi Fifo for next frame
    hal_RfspiFillFifo(TRUE);

#ifdef RFSPI_IFC_WORKAROUND
    // Generate a TCU1 IT after the last SPI command in order to update the TC register of the IFC rfspi channel when the latter is not running.
    // Warning : this mechanism may not work if an SPI command is required to close the last programmed window
    // as the IT may occur before its completion. In this case an extra offset in the date may need to be added.
    if (g_palFrame.WinNb)
    {
        // generate the IT only if events are programmed for the next frame.
        // (otherwise the rfspi is not used the next frame and the TC does not need to be updated)
        hal_TcuSetEvent (HAL_TCU_TRIGGER_XCPU_SOFTIRQ1, g_palFrame.Win[g_palFrame.WinOrder[g_palFrame.WinNb-1]].date + g_palCtx.SetUpTime);
    }
#endif // RFSPI_IFC_WORKAROUND

    // Check if FINT already happened before the end of PAL processing
    // This is authorized only if no window has been scheduled.
    // (it allows heavy CPU-loaded frames that have no real-time needs)
    PAL_ASSERT(!((hal_FintIrqGetStatus()!=0) && (g_palFrame.WinNb!=0)),
               PAL_ASSERT_STRING(3),
               g_palFrame.WinNb);

    // Aux clk switching between running and stopping (e.g., BT
    // sleeping and waking-up) will impact xcv 26M .
    // Here we avoid such switching by always running aux clk
    // when rf will be working at next frame.
    // The control is activated only if BCPU is idle in current frame.
    if (g_palCtx.palBcpuSysFreqCurr == HAL_SYS_FREQ_32K)
    {
        if (g_palCtx.palBcpuSysFreqNext == HAL_SYS_FREQ_32K)
        {
            // RF/BCPU will be idle in next frame
            if (g_palAuxClkEnabled)
            {
                g_palAuxClkEnabled = FALSE;
                hal_SysAuxClkOut(FALSE);
            }
        }
        else
        {
            // RF/BCPU will have some work to do in next frame
            // Enable aux clock only when BT is opened
            if (g_palAuxClkActionWithRf && !g_palAuxClkEnabled)
            {
                g_palAuxClkEnabled = TRUE;
                hal_SysAuxClkOut(TRUE);
            }
        }
    }

    hal_SysSetBBClock(MAX(g_palCtx.palBcpuSysFreqNext, g_palCtx.palBcpuSysFreqCurr));

    PAL_PROFILE_FUNCTION_EXIT(pal_FrameEnd);
    PAL_PROFILE_WINDOW_EXIT(PAL_FRAME);
}

//======================================================================
// pal_TimeTuning
//----------------------------------------------------------------------
// Function used for correcting small timing drifts during connected
// mode.
//
// @param tof The timing offset expressed in ?bits (12/13 us)
//            corresponding to the timing drift to be compensated
//            by PAL.
//======================================================================
VOID pal_TimeTuning (INT8 tof)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_TimeTuning);
    PAL_ASSERT((PAL_ABS (tof) <= (PAL_TIME_SYNC_MARGIN - 1)),
               PAL_ASSERT_STRING(5), tof);

    g_palFrame.WrapValue += tof;

#ifdef HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_TIMETUNING,tof,g_palFrame.WrapValue,0,0,0,0));
    PAL_TRACE( PAL_LEV(12), 0, "TimeTuning[%d]: Wrap=%d",
               tof, g_palFrame.WrapValue);
#else // !HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_TIMETUNING,(UINT32)tof,0,0,0,0,0));
    PAL_TRACE( PAL_LEV(12)|TNB_ARG(1), 0, "TimeTuning[%d]", tof);
#endif // !HAL_PAL_TRC_TCU_VERBOSE

    hal_TcuSetWrap(g_palFrame.WrapValue);

    PAL_PROFILE_FUNCTION_EXIT(pal_TimeTuning);
}

//======================================================================
// pal_TimeTuningSuspendCheck
//----------------------------------------------------------------------
// Function used to check if a suspend time tunningis running.
//
// @param  none
//======================================================================
VOID pal_TimeTuningSuspendCheck(VOID)
{
    if(PAL_TU_SUSPEND_WAITFINT == g_palCtx.palTuSuspend)
    {
          pal_TimeTuning(g_palCtx.palTuVal);
          g_palCtx.palTuVal = 0;
          g_palCtx.palTuSuspend = PAL_TU_SUSPEND_IDLE;
    }
}
// pal_TimeTuningSuspend
//----------------------------------------------------------------------
// Function used to suspend a time tunning of small drifts.
//
// @param tof The timing offset expressed in ?bits (12/13 us)
//            corresponding to the timing drift to be compensated
//            by PAL.
//======================================================================
VOID pal_TimeTuningSuspend (BOOL on, INT8 tof)
{
    UINT32 curFrame = hal_ComregsGetSnap();
    if(on)
    {
        g_palCtx.palTuFrame = (UINT8)curFrame;
        if(PAL_TU_SUSPEND_WAITFINT != g_palCtx.palTuSuspend)
        {
            g_palCtx.palTuSuspend = PAL_TU_SUSPEND_LOCK;
        }
        g_palCtx.palTuVal += tof;
        if(g_palCtx.palTuVal == 0)
        {
            g_palCtx.palTuSuspend = PAL_TU_SUSPEND_IDLE;
        }
        PAL_TRACE( PAL_LEV(16)|TNB_ARG(1), 0, "tunning suspend %d",tof);
    }
    else
    {
        g_palCtx.palTuSuspend = PAL_TU_SUSPEND_IDLE;
        g_palCtx.palTuVal = 0;
    }
}

//======================================================================
// pal_SynchronizationChange
//----------------------------------------------------------------------
// This function is invoked by L1 when a cell change or a timeslot
// change occurs. During normal operation, L1 is expected to keep PAL
// frame position aligned with the first reception burst corresponding
// to the current MS allocation.
//
// @param tof Timing offset corresponding to the distance between the
//            current frame position and the new frame position.
//            It is expressed in quarter bits and is always positive.
//======================================================================
VOID pal_SynchronizationChange (UINT16 tof)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SynchronizationChange);

    if ((tof - g_palCtx.internalTimeShift) >= 0)
    {
        g_palFrame.WrapValue += (tof - g_palCtx.internalTimeShift);
        g_palCtx.SetUpTime = PAL_HW_SETUP;
        g_palCtx.internalTimeShift = 0;
    }
    else // dont touch the wrap
    {
        g_palCtx.SetUpTime = PAL_HW_SETUP + tof - g_palCtx.internalTimeShift;
        g_palCtx.internalTimeShift -= tof;
    }
    g_palFrame.lastStop = 0;

#ifdef HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_SYNCCHANGE,tof,
               g_palFrame.WrapValue,
               g_palCtx.internalTimeShift,
               g_palCtx.SetUpTime,
               0,0));
    PAL_TRACE( PAL_LEV(12), 0, "SynchChange[%d]: Wrap=%d Shift=%d Setup=%d",
               tof,
               g_palFrame.WrapValue,
               g_palCtx.internalTimeShift,
               g_palCtx.SetUpTime);
#else // !HAL_PAL_TRC_TCU_VERBOSE
    palDbgTrc((HAL_DBGPALTRC_SYNCCHANGE,tof,0,0,0,0,0));
    //PAL_TRACE( PAL_LEV(12), 0, "SynchChange[%d]", tof);
#endif // !HAL_PAL_TRC_TCU_VERBOSE
    PAL_ASSERT((g_palFrame.WrapValue <= 16383),
               PAL_ASSERT_STRING(15), g_palFrame.WrapValue);

    hal_TcuSetWrap(g_palFrame.WrapValue);
    hal_DbgTrace(_PAL|PAL_LEV(12)|TSTDOUT,0,"SynchChange[%d,%d]", tof,hal_TcuGetCount());

    PAL_PROFILE_FUNCTION_EXIT(pal_SynchronizationChange);
}


//======================================================================
// pal_FrequencyTuning
//----------------------------------------------------------------------
// @param FOf expressed in Hertz and corresponds to the frequency
//            drift to be compensated by PAL.
// @param Arfcn parameter corresponding to the Absolute Radio
//              Frequency Channel used for the measurement.
//              It is needed to get a relative frequency drift
//              measurement.
//======================================================================
VOID pal_FrequencyTuning (INT32 FOf, UINT16 Arfcn)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_FrequencyTuning);

    // Flag that an Afc update has been asked
    g_palCtx.Afc.Update = TRUE;
    g_palCtx.Afc.Freq   = FOf;
    g_palCtx.Afc.Arfcn  = Arfcn;
    rfd_XcvFrequencyTuning(g_palCtx.Afc.Freq, palGetBand(g_palCtx.Afc.Arfcn));

    palDbgTrc((HAL_DBGPALTRC_FREQTUNING,Arfcn, (UINT32)FOf,0,0,0,0));
    PAL_TRACE( PAL_LEV(12), 0, "FreqTuning @0x%x %iHz",
               Arfcn, (INT16)FOf);

    PAL_PROFILE_FUNCTION_EXIT(pal_FrequencyTuning);
}

//======================================================================
// Reception management : pal_SetMonRxWin
//----------------------------------------------------------------------
// This function opens a monitoring window on next frame to perform
// a power measurement.
// @param win pointer to the window structure used for monitoring,
//            FCH and SCH reception purpose.
//======================================================================
VOID pal_SetMonRxWin (pal_RxWin_t * win)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetMonRxWin);
    PAL_ASSERT(win->WinIdx < (PAL_RX_WIN_MONIT_QTY+PAL_RX_WIN_MON_EXT_QTY),
               PAL_ASSERT_STRING(6), win->WinIdx);

    palDbgTrc((HAL_DBGPALTRC_SETMONWIN,win->WinIdx, win->Arfcn, win->Start,0,0,0));
    PAL_TRACE( PAL_LEV(4), 0, "SetMon[%d]:0x%x @:%d Gain:%d",
               win->WinIdx, win->Arfcn, win->Start,win->Gain);

    // Time ordering
    g_palFrame.Win[win->WinIdx].date = win->Start;
    palReorderWin(win->WinIdx);

    // Win parameters
    g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_MONIT;

#ifdef CHIP_HAS_EXTRA_WINDOWS
    if(win->WinIdx >= PAL_RX_WIN_MONIT_QTY)
    {
        g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_MONIT_EXT;
    }
#endif

    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = palGetBand(win->Arfcn);
    // AGC Gain not forced anymore to handle "AGC-less" Xcvers
    g_palFrame.Win[win->WinIdx].gain[0] = win->Gain;
    g_palFrame.Win[win->WinIdx].bitmap = 0x1;

#ifdef XCV_PLL_SDM_MODE
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
    g_palFrame.Win[win->WinIdx].fof = 0;
#endif
#endif

#ifdef MULTI_BANDWIDE
    g_palFrame.Win[win->WinIdx].bw = 0;
#endif
    // TCU programming
    palProgramRxWin(win->WinIdx);

    PAL_PROFILE_FUNCTION_EXIT(pal_SetMonRxWin);
}

//======================================================================
// Reception management :
//----------------------------------------------------------------------
//======================================================================
UINT8 pal_GetMonResult (pal_RxWin_t * win)
{
    UINT8 result;

    PAL_PROFILE_FUNCTION_ENTRY(pal_GetMonResult);
    PAL_ASSERT(win->WinIdx < (PAL_RX_WIN_MONIT_QTY+PAL_RX_WIN_MON_EXT_QTY),
               PAL_ASSERT_STRING(7), win->WinIdx);

    // Calculates power
    result = palCalcMonPower(win->WinIdx, win->Arfcn);
    result = (result<110)?result:110;

    palDbgTrc((HAL_DBGPALTRC_GETMONRES,win->WinIdx,win->Arfcn,result,0,0,0));
    PAL_TRACE( PAL_LEV(5), 0, "GetMon[%d]:0x%x Pwr=%d",
               win->WinIdx, win->Arfcn, result);

    PAL_PROFILE_FUNCTION_EXIT(pal_GetMonResult);
    return result;
}

//======================================================================
// Reception management :
//----------------------------------------------------------------------
// @param win pointer to the window structure used for monitoring,
//            FCH and SCH reception purpose.
//======================================================================
VOID pal_SetFchRxWin (pal_RxWin_t * win)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetFchRxWin);
    PAL_ASSERT((win->WinIdx < PAL_WINDOWS_QTY), PAL_ASSERT_STRING(8),
               win->WinIdx);

    if (g_palCtx.FchOpened)
    {
        // FCCH already open
        return;
    }
    palDbgTrc((HAL_DBGPALTRC_SETFCHWIN,win->WinIdx,win->Arfcn,win->Start,0,0,0));
    PAL_TRACE( PAL_LEV(2), 0, "SetFCH[%d]:0x%x @:%d",
               win->WinIdx,
               win->Arfcn,
               win->Start);

    g_palCtx.FchOpened = TRUE;

    // Reset valid flag and results
    g_mailbox.spc2pal.statWin.rx.fcch.valid = FALSE;
    g_mailbox.spc2pal.statWin.rx.fcch.fof = 0;
    g_mailbox.spc2pal.statWin.rx.fcch.tof = 0;

    // Time ordering
    g_palFrame.Win[win->WinIdx].date = win->Start;
    palReorderWin(win->WinIdx);

    // Win parameters
    g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_FCCH;
    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = palGetBand(win->Arfcn);
    g_palFrame.Win[win->WinIdx].gain[0] = win->Gain;
    g_palFrame.Win[win->WinIdx].bitmap = 0x1;

#ifdef XCV_PLL_SDM_MODE
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
    g_palFrame.Win[win->WinIdx].fof = 0;
#endif
#endif

#ifdef MULTI_BANDWIDE
    if(g_fcch_detect_mode == GSM_FCCH_SEARCH_ALGO_FILTER)
        g_palFrame.Win[win->WinIdx].bw = 0;
    else
        g_palFrame.Win[win->WinIdx].bw = 1;
#endif
#if (defined CHIP_DIE_8955)
g_mailbox_pal2spc_rxBandInd = g_palFrame.Win[win->WinIdx].band;
#endif
    // TCU programming
    palProgramRxWin(win->WinIdx);


    PAL_PROFILE_FUNCTION_EXIT(pal_SetFchRxWin);
}

//======================================================================
// Reception management :
//----------------------------------------------------------------------
//======================================================================
VOID pal_CloseFchRxWin (UINT16 stopPos)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_CloseFchRxWin);
    palDbgTrc((HAL_DBGPALTRC_CLOSEFCHWIN,stopPos,g_palCtx.FchOpened,0,0,0,0));
    PAL_TRACE( PAL_LEV(2), 0, "Close FCH win @:%d",stopPos);

    // Multiple FCCH close for the same window can happen
    if (g_palCtx.FchOpened)
    {
        // Force the end of the Rx window
        palRxBstOff(stopPos + g_palCtx.SetUpTime);
        rfd_XcvRxOff(&FcchWin,stopPos + g_palCtx.SetUpTime);
        rfd_SwSetPosition(RFD_DIR_RX_DOWN, FcchWin.band, stopPos + g_palCtx.SetUpTime);

        // no windows programmed in current frame, put the RF component in light low power mode
        if(stopPos == 0)
        {
            pal_RfSleep();
        }

#ifdef HAL_PAL_TRC_TCU_VERBOSE
        palDbgTrc((HAL_DBGPALTRC_TEMP1,
                   stopPos + g_palCtx.SetUpTime,
                   g_palCalibration->xcv->palcust.rxIqTimeOffset,
                   g_palCtx.winBounds.rxHold,
                   g_palFrame.lastStop,
                   0,0));
#endif // HAL_PAL_TRC_TCU_VERBOSE

        g_palCtx.FchOpened = FALSE;
    }

    PAL_PROFILE_FUNCTION_EXIT(pal_CloseFchRxWin);
    return;
}

// *** pal_GetFchResult ***
BOOL
pal_GetFchResult (UINT8 WinIdx, pal_FchResult_t * res)
{
    BOOL valid = g_mailbox.spc2pal.statWin.rx.fcch.valid;

    PAL_PROFILE_FUNCTION_ENTRY(pal_GetFchResult);

    if (valid)
    {
        palDbgTrc((HAL_DBGPALTRC_GETFCHOK,(UINT32) g_mailbox.spc2pal.statWin.rx.fcch.fof,
                   g_mailbox.spc2pal.statWin.rx.fcch.tof,0,0,0,0
                  ));
        PAL_TRACE( PAL_LEV(2), 0, "GetFch: fof=%d tof=%i",
                   (INT16) g_mailbox.spc2pal.statWin.rx.fcch.fof,
                   g_mailbox.spc2pal.statWin.rx.fcch.tof
                 );
    }
    else
    {
        palDbgTrc((HAL_DBGPALTRC_GETFCHKO,0,0,0,0,0,0));
        PAL_TRACE( PAL_LEV(2), 0, "GetFch: not found");
    }

    // Retrieve FCCH results
    res->FOf = g_mailbox.spc2pal.statWin.rx.fcch.fof;
    res->TOf = g_mailbox.spc2pal.statWin.rx.fcch.tof;


    PAL_PROFILE_FUNCTION_EXIT(pal_GetFchResult);

    return valid;
}

// *** pal_SetSchRxWin ***
VOID
pal_SetSchRxWin (pal_RxWin_t * win, INT16 fof)
{
    PAL_PROFILE_FUNCTION_ENTRY(pal_SetSchRxWin);

    palDbgTrc((HAL_DBGPALTRC_SETSCHWIN,win->WinIdx,win->Arfcn,win->Start,win->Gain,0,0));
    PAL_TRACE( PAL_LEV(3), 0, "SetSch[%d]:0x%x @:%d Gain:%d",
               win->WinIdx,
               win->Arfcn,
               win->Start,
               win->Gain);

#ifdef XCV_PLL_SDM_MODE
    fof = (fof <= FOF_THRESHOLD_LOWER)? FOF_THRESHOLD_LOWER:fof;
    fof = (fof >= FOF_THRESHOLD_UPPER)? FOF_THRESHOLD_UPPER:fof;
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
    g_palFrame.Win[win->WinIdx].fof = fof;
#else
    if (fof != 0)
    {
        pal_FrequencyTuning((INT16)fof, win->Arfcn);
    }
#endif
#endif

    // Invalidate previous result
    g_mailbox.spc2pal.statWin.rx.sch[0].valid = PAL_SCH_PENDING;
    // Prepare for Xtal Aging Check
    g_palCtx.XtalAgeValue = rfd_XcvGetAfcOffset();

    // Time ordering
    g_palFrame.Win[win->WinIdx].date = win->Start - PAL_RX_SCH_MARGIN;
    palReorderWin(win->WinIdx);

    // Win parameters
    g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_SCH;
    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = palGetBand(win->Arfcn);
    g_palFrame.Win[win->WinIdx].gain[0] = win->Gain;
    g_palFrame.Win[win->WinIdx].bitmap = 0x1;

#ifdef MULTI_BANDWIDE
    g_palFrame.Win[win->WinIdx].bw = 0;
#endif

    // TCU programming
    palProgramRxWin(win->WinIdx);

    PAL_PROFILE_FUNCTION_EXIT(pal_SetSchRxWin);
}

// *** pal_GetSchResult ***
BOOL
pal_GetSchResult (UINT8 bufIdx, pal_SchResult_t * res)
{
    BOOL valid = g_mailbox.spc2pal.statWin.rx.sch[0].valid;
    int i;
    UINT16 Power;

    PAL_PROFILE_FUNCTION_ENTRY(pal_GetSchResult);
    PAL_ASSERT(g_mailbox.spc2pal.statWin.rx.sch[0].valid != PAL_SCH_PENDING,
               PAL_ASSERT_STRING(9), g_mailbox.spc2pal.statWin.rx.sch[0].valid);
    // Retrieve SCH result
    res->Snr = g_mailbox.spc2pal.statWin.rx.sch[0].snr;
    res->FOf = g_mailbox.spc2pal.statWin.rx.sch[0].fof;
    res->TOf = g_mailbox.spc2pal.statWin.rx.sch[0].tof;

    // For DEBUG : Calculate received power
    Power = (g_palCtx.SchGain + g_palCtx.Adc2dBmGain) -
            g_mailbox.spc2pal.statWin.rx.sch[0].power;

    // and Data
    for (i = 0; i < MBOX_SCH_DATA_BYTE_QTY; i++)
    {
        res->Data[i] = g_mailbox.spc2pal.statWin.rx.sch[0].data[i];
    }

    if (valid)
    {
        palDbgTrc((HAL_DBGPALTRC_GETSCHOK,res->FOf, res->TOf,res->Snr,Power,0,0));
        PAL_TRACE( PAL_LEV(3), 0, "GetSch: fof=%i tof=%d snr=%d pow=%d",
                   res->FOf, res->TOf,res->Snr,Power);
    }
    else
    {
        palDbgTrc((HAL_DBGPALTRC_GETSCHKO,res->FOf, res->TOf,res->Snr,Power,0,0));
        PAL_TRACE( PAL_LEV(3), 0, "GetSch: not found fof=%i tof=%d snr=%d pow=%d",
                   res->FOf, res->TOf,res->Snr,Power);
    }

    if (valid)
    {
        palCheckXtalAging(res->Snr);
        g_palCtx.SynchedOnce = TRUE;
    }

    PAL_PROFILE_FUNCTION_EXIT(pal_GetSchResult);

    return valid;
}

// *** pal_SetIntRxWin ***
VOID
pal_SetIntRxWin (pal_RxWin_t * win, UINT8 nbSlots)
{
    UINT8 snap = g_palCtx.FrameValid;
    UINT8 bitMap = 0;
    int i;

    PAL_PROFILE_FUNCTION_ENTRY(pal_SetIntRxWin);
    PAL_ASSERT(nbSlots <= MBOX_INTRF_SLOT_QTY,
               PAL_ASSERT_STRING(10), nbSlots);
    PAL_ASSERT(win->WinIdx < PAL_RX_WIN_MONIT_QTY,
               PAL_ASSERT_STRING(11), win->WinIdx);
    palDbgTrc((HAL_DBGPALTRC_SETINTWIN,win->WinIdx,win->Arfcn,win->Start,nbSlots,0,0));
    PAL_TRACE( PAL_LEV(8), 0, "SetInt[%d]:0x%x @=%d nbTs=%d",
               win->WinIdx,win->Arfcn,win->Start,nbSlots);

    // Save number of slots
    g_palCtx.IntrfSlotQty = nbSlots;

    // Build bitMap needed by spapi
    for(i=0; i<nbSlots; i++)
        bitMap |= (1 << i);

    g_mailbox.pal2spc.win[snap].rx.nBurst.bitMap = bitMap;

    // Time ordering
    g_palFrame.Win[win->WinIdx].date = win->Start;
    palReorderWin(win->WinIdx);

    // Win parameters
    g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_INTRF;
    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = palGetBand(win->Arfcn);
    g_palFrame.Win[win->WinIdx].gain[0] = RFD_AGC;
    g_palFrame.Win[win->WinIdx].bitmap = bitMap;

#ifdef XCV_PLL_SDM_MODE
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
    g_palFrame.Win[win->WinIdx].fof = 0;
#endif
#endif

#ifdef MULTI_BANDWIDE
    g_palFrame.Win[win->WinIdx].bw = 1;
#endif

    // TCU programming
    palProgramRxWin(win->WinIdx);

    PAL_PROFILE_FUNCTION_EXIT(pal_SetIntRxWin);
}

// *** pal_GetIntResult ***
VOID
pal_GetIntResult (pal_RxWin_t * win, UINT8 * intMeas)
{
    UINT8 i;

    PAL_PROFILE_FUNCTION_ENTRY(pal_GetIntResult);
    PAL_ASSERT (win->WinIdx < PAL_RX_WIN_MONIT_QTY,
                PAL_ASSERT_STRING(12), win->WinIdx);

    for (i=0; i<g_palCtx.IntrfSlotQty; i++)
    {
        intMeas[i] = palCalcMonPower(i, win->Arfcn);
    }

    palDbgTrc((HAL_DBGPALTRC_GETINTRES,g_palCtx.IntrfSlotQty,
               intMeas[0],intMeas[1],intMeas[2],intMeas[3],intMeas[4]));
    PAL_TRACE( PAL_LEV(9), 0, "GetInt[%d Ts]: Ts0=%d Ts1=%d Ts2=%d",
               g_palCtx.IntrfSlotQty, intMeas[0],intMeas[1],intMeas[2]);
    PAL_TRACE( PAL_LEV(9), 0, "Ts3=%d Ts4=%d Ts5=%d",
               intMeas[3],intMeas[4],intMeas[5]);

    PAL_PROFILE_FUNCTION_EXIT(pal_GetIntResult);
}

// *** pal_SetNBurstRxWin ***
VOID
pal_SetNBurstRxWin (pal_NBurstRxWin_t * win)
{
    UINT8 snap = g_palCtx.FrameValid;

    PAL_PROFILE_FUNCTION_ENTRY(pal_SetNBurstRxWin);

    palDbgTrc((HAL_DBGPALTRC_SETNBWIN,win->WinIdx, win->RxBitMap, win->BstIdx,
               win->Arfcn, win->Start, win->Gain));
    PAL_TRACE( PAL_LEV(6), 0, "SetNB[%d](%d){%d}:0x%x @=%d Gain=%d",
               win->WinIdx,
               win->BufIdx,
               win->BstIdx,
               win->Arfcn,
               win->Start,
               win->Gain);

    // Memorize that an Equalization irq is expected
    g_palCtx.EquIrqMap |= 1;

    // Context for BCPU (no reordering on this)
    g_mailbox.pal2spc.win[snap].rx.nBurst.bufIdx = win->BufIdx;
    g_mailbox.pal2spc.win[snap].rx.nBurst.bstIdx = win->BstIdx;
    g_mailbox.pal2spc.win[snap].rx.nBurst.tsc = win->Tsc;
    g_mailbox.pal2spc.win[snap].rx.nBurst.bitMap = win->RxBitMap;

    // ask the BCPU for dummy PCH detection if enabled
    if (g_palCtx.palDummyPchDetection)
    {
        // set the thresholds for vector a and b
        g_mailbox.dummyPchThresh=(UINT32)((g_palCtx.palDummyPchThresh_a)<<8 |
                                          (g_palCtx.palDummyPchThresh_b));
        g_mailbox.pal2spc.statWin.rx.cfg[win->BufIdx].itr |= MBOX_DSP_PCH_READY;
    }
    else
    {
        g_mailbox.pal2spc.statWin.rx.cfg[win->BufIdx].itr &= ~MBOX_DSP_PCH_READY;
    }
    // Time ordering
    g_palFrame.Win[win->WinIdx].date = win->Start - PAL_RX_NBURST_MARGIN;


    if(g_palCtx.palCchDecDone)
    {
        g_mailbox.pal2spc.statWin.rx.cfg[win->BufIdx].itr |= MBOX_DSP_CCH_DECODING_COMPLETE;
    }
    else
    {
        g_mailbox.pal2spc.statWin.rx.cfg[win->BufIdx].itr &= ~MBOX_DSP_CCH_DECODING_COMPLETE;
    }

    palReorderWin(win->WinIdx);

    // Win parameters
    g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_NBURST;
    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = palGetBand(win->Arfcn);
    g_palFrame.Win[win->WinIdx].gain[0] = win->Gain;
    g_palFrame.Win[win->WinIdx].bitmap = win->RxBitMap;

#ifdef XCV_PLL_SDM_MODE
    win->FOf = (win->FOf <= FOF_THRESHOLD_LOWER)? FOF_THRESHOLD_LOWER : win->FOf;
    win->FOf = (win->FOf >= FOF_THRESHOLD_UPPER)? FOF_THRESHOLD_UPPER : win->FOf;
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
    g_palFrame.Win[win->WinIdx].fof = win->FOf;
#else
    if (win->FOf != 0)
    {
        pal_FrequencyTuning((INT16)win->FOf, win->Arfcn);
    }
#endif
#endif

#ifdef MULTI_BANDWIDE
    if(g_palCtx.palModeIsGprs)
    {
#ifdef GCF_TEST
        g_palFrame.Win[win->WinIdx].bw = 2; //wide band width in case of gprs & gcf test
#else
        g_palFrame.Win[win->WinIdx].bw = 1; //middle band width in case of gprs & field test
#endif
    }
    else
    {
        if(g_palFrame.Win[win->WinIdx].gain[0] > 100)
            g_palFrame.Win[win->WinIdx].bw = 1;  // if use narrow band width, will decrease sensitivity.
        else
#if ((SPC_IF_VER ==5) && (defined CHIP_DIE_8955))  
#ifdef GCF_TEST
            g_palFrame.Win[win->WinIdx].bw = 0; //middle band width in case of GSM
#else          
            g_palFrame.Win[win->WinIdx].bw = 1; //middle band width in case of GSM
#endif
#else
            g_palFrame.Win[win->WinIdx].bw = 0; //narrow band width in case of GSM
#endif
    }
#endif

    // TCU programming
    palProgramRxWin(win->WinIdx);
    //added by zhanghy. if Nburst is later than 1700 Qb, move FINT backward by 800 Qb to make xcpu finish the job after dummy dsp irq.
    if( (g_palCtx.palDummyPchDetection) && (win->Start > PAL_DUMMY_DETECT_QBOF))
        palSetUpperBound(PAL_TCU_DEFAULT_WRAP_COUNT+win->Start - PAL_DUMMY_DETECT_QBOF);

    PAL_PROFILE_FUNCTION_EXIT(pal_SetNBurstRxWin);
}

// *** pal_GetNBurstResult ***
VOID
pal_GetNBurstResult (UINT8 bstIdx, UINT16 arfcn, pal_NBurstResult_t * res)
{
    UINT8 snap = g_palCtx.FrameResult;
    UINT32 curFrame = hal_ComregsGetSnap();
    GSM_RFBAND_T band = palGetBand (arfcn);
#ifdef USE_BB_DC_CANCEL
    UINT8 AnaLev = g_palCtx.NBGainAnaLev[snap];
    UINT8 DigLev = (UINT8)(g_palCtx.NBGainDigLev[snap]);
    UINT8 ushift = 4;
    UINT8 DcBand = ((band == GSM_BAND_GSM850) || (band == GSM_BAND_GSM900))? 0:1;
#endif

#if ((SPC_IF_VER ==5) && ((defined CHIP_DIE_8955) || (defined CHIP_DIE_8909)))
    INT16 dco_I = g_mailbox.spc2pal.win[snap].rx.burstRes.dco_I;
    INT16 dco_Q = g_mailbox.spc2pal.win[snap].rx.burstRes.dco_Q;
    PAL_TRACE( PAL_LEV(15), 0, "dcI:%d, dcQ:%d",dco_I,dco_Q);
#else
#ifdef USE_BB_DC_CANCEL
    INT16 dco_I = g_mailbox_spc2pal_win_rx_burstRes_dcI[snap];
    INT16 dco_Q =g_mailbox_spc2pal_win_rx_burstRes_dcQ[snap];
    PAL_TRACE( PAL_LEV(15), 0, "dcI:%d, dcQ:%d",dco_I,dco_Q);
#endif
#endif

    PAL_PROFILE_FUNCTION_ENTRY(pal_GetNBurstResult);

    // Get Burst results
    res->FOf = g_mailbox.spc2pal.win[snap].rx.burstRes.fof;
    res->FOf -= (g_palCtx.Afc.UpdtPending) ? g_palCtx.Afc.Freq : 0;
    res->FOf -= g_palCalibration->xcv->palcust.rxTxFreqOffset[band];
    res->TOf = g_mailbox.spc2pal.win[snap].rx.burstRes.tof;
    res->Snr = g_mailbox.spc2pal.win[snap].rx.burstRes.snr;
#ifndef __NO_AMR__
    res->SID = g_mailbox.spc2pal.win[snap].rx.burstRes.sid;
#endif
    res->Pwr = palCalcPower(bstIdx);

    palDbgTrc((HAL_DBGPALTRC_GETNBSTRES,bstIdx, res->FOf, res->TOf, res->Snr, res->Pwr,0));
    PAL_TRACE( PAL_LEV(7), 0, "GetNB{%d}: FOf=%i TOf=%d Snr=%d Pwr=%d",
               bstIdx, res->FOf,  res->TOf, res->Snr, res->Pwr);

#ifdef USE_BB_DC_CANCEL
    g_palDC.DCOff_I_temp[DcBand][AnaLev] += dco_I*g_DigG2Amp[DigLev];
    g_palDC.DCOff_Q_temp[DcBand][AnaLev] += dco_Q*g_DigG2Amp[DigLev];
    if (g_palDC.DCOff_I_temp[DcBand][AnaLev] > (DC_THRESHOLD << (DC_AMP_SHIFT+ushift)))
    {
        g_palDC.DCOff_I_temp[DcBand][AnaLev] = (DC_THRESHOLD << (DC_AMP_SHIFT+ushift));
    }
    else if (g_palDC.DCOff_I_temp[DcBand][AnaLev] < (-(DC_THRESHOLD << (DC_AMP_SHIFT+ushift))))
    {
        g_palDC.DCOff_I_temp[DcBand][AnaLev] = -(DC_THRESHOLD << (DC_AMP_SHIFT+ushift));
    }


    if (g_palDC.DCOff_Q_temp[DcBand][AnaLev] > (DC_THRESHOLD << (DC_AMP_SHIFT+ushift)))
    {
        g_palDC.DCOff_Q_temp[DcBand][AnaLev] = (DC_THRESHOLD << (DC_AMP_SHIFT+ushift));
    }
    else if (g_palDC.DCOff_Q_temp[DcBand][AnaLev] < (-(DC_THRESHOLD << (DC_AMP_SHIFT+ushift))))
    {
        g_palDC.DCOff_Q_temp[DcBand][AnaLev] = -(DC_THRESHOLD << (DC_AMP_SHIFT+ushift));
    }

    g_palCtx.DCOff_I[DcBand][AnaLev] = (g_palDC.DCOff_I_temp[DcBand][AnaLev]>>(DC_AMP_SHIFT+ushift));
    g_palCtx.DCOff_Q[DcBand][AnaLev] = (g_palDC.DCOff_Q_temp[DcBand][AnaLev]>>(DC_AMP_SHIFT+ushift));

    PAL_TRACE(PAL_LEV(15), 0, "AnaLev:%d, DigLev:%d, dcIBack:%d, dcQBack:%d, Band:%d",
              AnaLev, DigLev, g_palCtx.DCOff_I[DcBand][AnaLev], g_palCtx.DCOff_Q[DcBand][AnaLev], DcBand);
#endif
    //retrieve FrameResult whenever changed
    if(curFrame == g_palCtx.FrameResult)
    {
        g_palCtx.FrameResult = (curFrame == 0 ? 2 : curFrame - 1);
    }
    PAL_PROFILE_FUNCTION_EXIT(pal_GetNBurstResult);
}


// *** pal_GetNBlockResult ***
VOID
pal_GetNBlockResult (UINT8 bufIdx, UINT8 headOffset, pal_NBlockResult_t * res)
{
    volatile SPC_MBOX_NBLOCK_RESULT_T *result = &g_mailbox.spc2pal.statWin.rx.nBlock[bufIdx];
    UINT8 cs = result->cs;
    UINT32 *buffer;

    PAL_PROFILE_FUNCTION_ENTRY(pal_GetNBlockResult);
    palDbgTrc((HAL_DBGPALTRC_GETNBLKRES,bufIdx,g_mailbox.pal2spc.statWin.dedicated.mode,
               cs,result->bitError,result->bfi,0));
    PAL_TRACE( PAL_LEV(10), 0, "GetNBlock(%d): mode=%02x CS=%d Ber=%d Bfi=%d",
               bufIdx,
               g_mailbox.pal2spc.statWin.dedicated.mode,
               cs,
               result->bitError,
               result->bfi);

    // Frame results
    res->Bfi        = result->bfi;
    res->Cs         = cs;
    res->MeanBEP    = result->meanBep;
    res->CvBEP      = result->cvBep;
    res->BitError   = result->bitError;
#ifndef __NO_AMR__
    res->SID        = result->sid;
    res->BitTotal   = result->bitTotal;
#endif

    // If the frame is correct, allocate and copy data in a buffer
    // freeing this buffer is under GSM stack responsability
    if (res->Bfi == 0)
    {

        buffer = (UINT32 *) result->data[0];
        if (buffer != NULL)
        {
            // Allocate buffer
            res->Data[0] = (UINT16 *) sxr_Malloc (palSizeFromCs[cs] + headOffset);

            // fill it
            memcpy (    (((UINT8 *) res->Data[0]) + headOffset), // destination
                        (UINT8*) UNCACHED((UINT32)buffer), // source
                        palSizeFromCs[cs]); // size
        }
        else
        {
            res->Data[0] = 0;
        }

        buffer = (UINT32 *) result->data[1];
        // Should only be set in case of CSD channel
        if (buffer != NULL)
        {
            // Get the Baudrate
            UINT8 baudrate = TCH_BAUDRATE(g_mailbox.pal2spc.statWin.dedicated.mode);
            // Allocate buffer
            res->Data[1] = (UINT16 *) sxr_Malloc (palSizeFromCSD[baudrate] + headOffset);

            // fill it
            memcpy (    (((UINT8 *) res->Data[1]) + headOffset), // destination
                        (UINT8*) UNCACHED((UINT32)buffer), // source
                        palSizeFromCSD[baudrate]); // size
        }
        else
        {
            res->Data[1] = 0;
        }

    }
    else
    {
        //PAL_PROFILE_FUNCTION_ENTRY(BFI);
        // NULL pointers in case of bad frame
        res->Data[0] = 0;
        res->Data[1] = 0;
        //PAL_PROFILE_FUNCTION_EXIT(BFI);
    }

    // update speech buffers in Mbx with HAL function
    if (((g_mailbox.pal2spc.statWin.dedicated.mode)>>2)&0x1)
    {
        if ((g_mailbox.pal2spc.statWin.rx.cfg[bufIdx].type == PAL_TCHF_CHN) ||
                (g_mailbox.pal2spc.statWin.rx.cfg[bufIdx].type == PAL_TCHH_CHN))
        {
            hal_SpeechPushRxPopTx( (HAL_SPEECH_DEC_IN_T*) &(g_mailbox.spc2pal.speechDecIn),
                                   (HAL_SPEECH_ENC_OUT_T*) &(g_mailbox.pal2spc.speechEncOut) );
        }
    }

    PAL_PROFILE_FUNCTION_EXIT(pal_GetNBlockResult);
}

// *** pal_GetUsf ***

VOID
pal_GetUsf (UINT8 * usf)
{
    int i;
    PAL_PROFILE_FUNCTION_ENTRY(pal_GetUsf);

    for (i=0; i<MBOX_MAX_TS_QTY; i++)
    {
        usf[i] = g_mailbox.spc2pal.statWin.rx.decodedUsf[i];
    }

    palDbgTrc((HAL_DBGPALTRC_GETUSF,usf[0],usf[1],usf[2],usf[3],0,0));
    PAL_TRACE( PAL_LEV(11), 0, "GetUsf: USF0=0x%x USF1=0x%x USF2=0x%x USF3=0x%x",
               usf[0],usf[1],usf[2],usf[3]);
    PAL_PROFILE_FUNCTION_EXIT(pal_GetUsf);
}

// *** pal_ABurstEncoding ***
VOID
pal_ABurstEncoding (pal_ABurstDesc_t * ab)
{
    UINT16 Data = ab->Data;
    PAL_PROFILE_FUNCTION_ENTRY(pal_ABurstEncoding);

    PAL_ASSERT(ab->BufIdx < MBOX_MAX_BUF_IDX_QTY,
               PAL_ASSERT_STRING(13), ab->BufIdx);

    palDbgTrc((HAL_DBGPALTRC_ABSTENC,ab->BufIdx,ab->Data,ab->Bsic,ab->Size,0,0));
    PAL_TRACE( PAL_LEV(13), 0, "ABEnc(%d): data=0x%x Bsic=%d Size=%d",
               ab->BufIdx,ab->Data,ab->Bsic,ab->Size);

    // Buffer context
    g_palCtx.TxCtx[ab->BufIdx].type = MBOX_CHAN_TYPE_RACH;
    g_palCtx.TxCtx[ab->BufIdx].encDone = 1 ;

    // 11 bits PRACH special format
    Data = (ab->Size == 11) ? ((Data >> 3) | (Data << 8)) : Data;
    g_mailbox.pal2spc.statWin.tx.enc.data = Data;
    g_mailbox.pal2spc.statWin.tx.enc.bsic = ab->Bsic;
    g_mailbox.pal2spc.statWin.tx.enc.size = ab->Size;

    PAL_PROFILE_FUNCTION_EXIT(pal_ABurstEncoding);
}


// *** pal_NblockEncoding ***
VOID
pal_NblockEncoding (pal_NBlockDesc_t * nb)
{
    UINT8 tmp_bufIdx=nb->BufIdx;
    UINT8 tmp_size=palSizeFromCs[nb->Cs];
    PAL_PROFILE_FUNCTION_ENTRY(pal_NblockEncoding);

    PAL_ASSERT(nb->BufIdx < MBOX_MAX_BUF_IDX_QTY,
               PAL_ASSERT_STRING(14), nb->BufIdx);

    palDbgTrc((HAL_DBGPALTRC_NBLKENC,nb->BufIdx,nb->ChannelType,nb->Cs,0,0,0));
    PAL_TRACE( PAL_LEV(13), 0, "NBEnc(%d): ChType=%d Cs=%d",
               nb->BufIdx,nb->ChannelType,nb->Cs);

    // Buffer context
    if (nb->ChannelType==MBOX_CDATA_CHN)
    {
        tmp_bufIdx=MBOX_MAX_BUF_IDX_QTY;
        tmp_size=palSizeFromCSD[TCH_BAUDRATE(g_mailbox.pal2spc.statWin.dedicated.mode)];
        if (g_palCtx.TxCtx[nb->BufIdx].encDone == 0)
            g_palCtx.TxCtx[nb->BufIdx].type = nb->ChannelType;
    }
    else
    {
        g_palCtx.TxCtx[nb->BufIdx].type = nb->ChannelType;
    }
    g_palCtx.TxCtx[nb->BufIdx].encDone = 1 ;
    g_palCtx.TxCtx[nb->BufIdx].cs = nb->Cs ;

    memcpy (    g_mailbox.spc2pal.encBuff[tmp_bufIdx], // destination
                nb->Data, // source
                tmp_size); // size

    PAL_PROFILE_FUNCTION_EXIT(pal_NblockEncoding);
}

// *** pal_SetTxWin ***
VOID
pal_SetTxWin (pal_TxWin_t * win)
{
    UINT8 snap = g_palCtx.FrameValid;
    int i, slot;

    PAL_PROFILE_FUNCTION_ENTRY(pal_SetTxWin);

    PAL_ASSERT(win->TxBitMap, PAL_ASSERT_STRING(17), win->WinIdx);

    palDbgTrc((HAL_DBGPALTRC_SETTXWIN,win->WinIdx, win->BstIdx, win->Arfcn,
               win->Start, win->TxBitMap, win->TxPower[0]));
    PAL_TRACE( PAL_LEV(14), 0, "SetTx[%d](%d):0x%x @=%d BitMap=%d PCL=%d",
               win->WinIdx,
               win->BstIdx,
               win->Arfcn,
               win->Start,
               win->TxBitMap,
               win->TxPower[0]);

    // Context for BCPU
    slot = 0;
    for (i = 0; i < MBOX_MAX_TS_QTY; i++)
    {
        if (win->TxBitMap & (1 << i)) // Slot Used
        {
            UINT8 usedBuf = win->RouteMap[slot];

            PAL_ASSERT(usedBuf < MBOX_MAX_BUF_IDX_QTY, PAL_ASSERT_STRING(18),
                       usedBuf, slot, win->TxBitMap, win->WinIdx);

            g_mailbox.pal2spc.win[snap].tx.routeMap[slot] = usedBuf;

            // Update Tx buffer context on first Tx
            if (win->BstIdx == 0)
            {
                // Update BCPU context
                g_mailbox.pal2spc.statWin.tx.ctx[usedBuf].type = g_palCtx.TxCtx[usedBuf].type;
                g_mailbox.pal2spc.statWin.tx.ctx[usedBuf].encDone = g_palCtx.TxCtx[usedBuf].encDone;
                g_mailbox.pal2spc.statWin.tx.ctx[usedBuf].cs = g_palCtx.TxCtx[usedBuf].cs;
                g_palCtx.TxCtx[usedBuf].encDone = 0;
            }
            // Increase used slot index
            slot++;
        }
    }
    g_mailbox.pal2spc.win[snap].tx.winIdx = win->WinIdx;
    g_mailbox.pal2spc.win[snap].tx.bstIdx = win->BstIdx;
    g_mailbox.pal2spc.win[snap].tx.tsc = win->Tsc;
    g_mailbox.pal2spc.win[snap].tx.qty++;
    g_mailbox.pal2spc.win[snap].tx.bitMap = win->TxBitMap;

    // Win parameters
    if (g_mailbox.pal2spc.statWin.tx.ctx[win->RouteMap[0]].type == MBOX_CHAN_TYPE_RACH)
    {
        // No TA for ABursts
        g_palFrame.Win[win->WinIdx].date = win->Start;
        // Time ordering
        palReorderWin(win->WinIdx);
        g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_TXABURST;
        palDbgTrc((HAL_DBGPALTRC_SETRACH,0,0,0,0,0,0));
        PAL_TRACE( PAL_LEV(14), 0, "Tx Rach");
    }
    else
    {
        g_palFrame.Win[win->WinIdx].date = win->Start - win->TA;
        // Time ordering
        palReorderWin(win->WinIdx);
        g_palFrame.Win[win->WinIdx].type = GSM_WIN_TYPE_TXNBURST;
    }
    g_palFrame.Win[win->WinIdx].arfcn = win->Arfcn;
    g_palFrame.Win[win->WinIdx].band = palGetBand(win->Arfcn);

    for (i = 0; i < MBOX_MAX_TS_QTY; i++)
    {
        g_palFrame.Win[win->WinIdx].gain[i] = win->TxPower[i];
#ifdef FPGA
        // On FPGA platforms using ANA Daughter Card, some PAs
        // are drawing too much current leading to crashes.
        if (win->TxPower[i] < 9)
        {
            g_palFrame.Win[win->WinIdx].gain[i] = 9;
        }
#endif
    }
    g_palFrame.Win[win->WinIdx].bitmap =  win->TxBitMap;
    g_palFrame.Win[win->WinIdx].txModBitmap =  0; // TODO: build up the modulation bitmap

#ifdef XCV_PLL_SDM_MODE
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
    g_palFrame.Win[win->WinIdx].fof = 0;
#endif
#endif

    // TCU and RF drivers programming
    palProgramTxWin(win->WinIdx);
    // Update swapIQ field (filled by rfd calls)
    g_mailbox.pal2spc.win[snap].tx.swapIQ = g_palFrame.Win[win->WinIdx].swapIQ;

    PAL_PROFILE_FUNCTION_EXIT(pal_SetTxWin);
}

// *** pal_WinResize ***
// This function's purpose is to shortened last programmed window (Tx) to
// make room on TN 6 & 7 for a FCCH/SCH window.
// This function has been implemented according to the following restrictions
// (given by ADA):
// - Called only on TX window
// - Can erase a window completely
// - Called after Rx and Tx windows progs, but before FCCH/SCH prog.
VOID
pal_WinResize (UINT8 WinIdx, UINT16 stopPos)
{
    UINT8 snap = g_palCtx.FrameValid;
    INT16 winStart = g_palFrame.Win[WinIdx].date;
    UINT8 oldBitMap = g_palFrame.Win[WinIdx].bitmap;
    UINT8 newBitMap;
    UINT8 SavedTcuNextEvt = hal_TcuGetEvtPtr();

    PAL_PROFILE_FUNCTION_ENTRY(pal_WinResize);
    palDbgTrc((HAL_DBGPALTRC_WINRESIZE,WinIdx,stopPos,0,0,0,0));
    PAL_TRACE( PAL_LEV(14), 0, "WinResize[%d]: StopPos=%d",WinIdx,stopPos);


    if (g_palFrame.Win[WinIdx].type == GSM_WIN_TYPE_TXNBURST)
    {
        // Check the win type - do something only if it is a Tx Window

        newBitMap = palResizeBitMap(oldBitMap,winStart,stopPos);
        g_palFrame.Win[WinIdx].bitmap = newBitMap;

        // Delete window's TCU events
        hal_TcuClrEvtRange(g_palFrame.Win[WinIdx].firstTcu,g_palFrame.Win[WinIdx].lastTcu);

        // Set the TCU event pointer back at the beginning of the window to
        // be resized. As the window is smaller it should fit.
        hal_TcuSetEvtPtr(g_palFrame.Win[WinIdx].firstTcu);

        // The resized window is the last one
        // The lastStop will then be setup by the next FCCH/SCH

        // Remove the Spi commands of this window
        hal_RfspiRemWinCmd(WinIdx);

        // Update g_palFrame & mailbox
        if (newBitMap == 0) // Win completely removed
        {
            int i;

            g_mailbox.pal2spc.win[snap].tx.qty = 0;
            g_mailbox.pal2spc.win[snap].tx.bitMap = 0;

            // Remove this win from the list to prevent RF Switch toggling
            for(i=0; i<g_palFrame.WinNb-1; i++)
            {
                // Find current win in ordered list...
                if (WinIdx != g_palFrame.WinOrder[i])
                {
                    continue;
                }
                // ...and remove it
                g_palFrame.WinOrder[i] = g_palFrame.WinOrder[i+1];
            }
            g_palFrame.WinNb--;
        }
        else
        {
            // Update BitMap for BCPU
            g_mailbox.pal2spc.win[snap].tx.bitMap = newBitMap;

#ifdef XCV_PLL_SDM_MODE
#if (defined(CHIP_DIE_8955) || defined(CHIP_DIE_8809E2))
            g_palFrame.Win[WinIdx].fof = 0;
#endif
#endif

            // TCU re-programming
            palProgramTxWin(WinIdx);
        }

        // Restore the TCU event pointer
        hal_TcuSetEvtPtr(SavedTcuNextEvt);
    }

    PAL_PROFILE_FUNCTION_EXIT(pal_WinResize);
}




//--------------------------------------------------------------------//
//                                                                    //
//                         PAL IRQ functions                          //
//                                                                    //
//--------------------------------------------------------------------//

// *** palFintIrqHandler (UINT8 interrupt_id) ***
// Called before Stack FINT handler to manage the Fint irq
VOID pal_FintIrqHandler (VOID)
{
    // Finish up filling the RfSpi Command Fifo
    hal_RfspiFillFifo(FALSE);

    // Call handler provided by stack
    if (g_palHandlers.fintHandler)
        g_palHandlers.fintHandler(1);
}

PROTECTED VOID pal_SimIrqHandler(HAL_SIM_IRQ_STATUS_T cause)
{
    // Call handler provided by stack
    if (g_palHandlers.simHandler)
    {
        if (cause.resetTimeout)
        {
            g_palHandlers.simHandler(PAL_SIM_RESET_TIMEOUT);
        }
        if (cause.parityError)
        {
            g_palHandlers.simHandler(PAL_SIM_PARITY_ERROR);
        }

        if (cause.badTs)
        {
            g_palHandlers.simHandler(PAL_SIM_BAD_TS);
        }

        if (cause.atrSuccess)
        {
#ifdef PAL_SIMD_IMPLEMENTATION
            // On a successful ATR (with ARG), need to
            // update simd with detected Reset polarity.
            simd_ConfigReset();
#endif // PAL_SIMD_IMPLEMENTATION
            g_palHandlers.simHandler(PAL_SIM_ATR_SUCCESS);
        }

        if (cause.atrStart)
        {
            g_palHandlers.simHandler(PAL_SIM_ATR_START);
        }

        if (cause.wwtTimeout)
        {
            g_palHandlers.simHandler(PAL_SIM_WWT_TIMEOUT);
        }

        if (cause.extraRx)
        {
            g_palHandlers.simHandler(PAL_SIM_EXTRA_RX);
        }
        else if (cause.dmaSuccess)
        {
            g_palHandlers.simHandler(PAL_SIM_DMA_SUCCESS);
        }


    }
}


// Called before Stack DSP handler to manage the mailbox (COM0) irq
VOID pal_DspIrqHandler (HAL_COMREGS_IRQ_STATUS_T status)
{
    // Call handler provided by stack
    if (g_palHandlers.dspHandler)
    {
        // USF ready
        if (status.status & MBOX_DSP_USF_READY)
        {
            g_palHandlers.dspHandler(PAL_USF_READY);
        }
        // equalization complete (burst, block and/or monitoring results ready)
        if (status.status & MBOX_DSP_EQUALIZATION_COMPLETE)
        {
            // mask IT until next FINT
            HAL_COMREGS_IRQ_MASK_T mask = {MBOX_DSP_EQUALIZATION_COMPLETE};
            hal_ComregsClrMask(HAL_COMREGS_XCPU_IRQ_SLOT_0, mask);
            g_palHandlers.dspHandler(PAL_EQUALIZATION_COMPLETE);
        }
        // SCH result ready
        if (status.status & MBOX_DSP_SCH_DECODING_COMPLETE)
        {
            g_palHandlers.dspHandler(PAL_SCH_DECODING_COMPLETE);
        }
        // dummy PCH detection result ready
        if (status.status & MBOX_DSP_PCH_READY)
        {
            g_palHandlers.dspHandler(PAL_PCH_READY);
        }
#if ((defined CHIP_DIE_8955) || (defined CHIP_DIE_8909))
        if (status.status & MBOX_DSP_NB_DECODING_COMPLETE)
        {
            g_palHandlers.dspHandler(PAL_DSP_NB_DECODING_COMPLETE);
        }
#endif		
        // dummy PCH detection result ready
        if (status.status & MBOX_DSP_CCH_DECODING_COMPLETE)
        {
            g_palHandlers.dspHandler(PAL_CCH_DECODING_COMPLETE);
        }
    }
}

#if 0
// Called before Stack DSP handler to manage the mailbox (COM1) irq
VOID pal_DspDoneIrqHandler(HAL_COMREGS_IRQ_STATUS_T status)
{
    // BCPU processing complete
    if (status.status & (MBOX_DSP_TASK_COMPLETE>>4))
    {
    }
}
#endif

//======================================================================
// Reception management : set dummy PCH detection active
/// This function is invoked to enable the dummy PCH detection in idle
/// mode, it sets a palCtx variable that enables the detection on the
/// BCPU and writes the thresholds into the mailbox. This function must
/// be called before pal_SetNBrxWin().
//----------------------------------------------------------------------
VOID pal_SetDummyPchDetection(UINT8 Thresh_a, UINT8 Thresh_b, INT32 winDate)
{
    // 39M at least needed when the dummy PCH window date is late
    HAL_SYS_FREQ_T nextBcpuFreq = (winDate < PAL_TIME_DUMMY_PCH_LATE_WINDOW_THRESHOLD) ?
                                  PAL_BCPU_SYS_FREQ_DEFAULT :
                                  PAL_BCPU_SYS_FREQ_DUMMY_PCH;
    // take the max
    if (g_palCtx.palBcpuSysFreqNext < nextBcpuFreq)
    {
        g_palCtx.palBcpuSysFreqNext = nextBcpuFreq;
    }

    g_palCtx.palDummyPchThresh_a = Thresh_a;
    g_palCtx.palDummyPchThresh_b = Thresh_b;
    g_palCtx.palDummyPchDetection = TRUE;
}


//======================================================================
// Reception management : get/clr dummy PCH detection active
/// This function is invoked to get the dummy PCH detection result, it
/// also resets the palCtx variable that enable the detection. This
//function has to be called on the recption of the DSP irq PAL_PCH_READY.
//----------------------------------------------------------------------
PAL_DUMMY_PCH_RESULT_T pal_GetDummyPchDetection(VOID)
{
    PAL_TRACE( PAL_LEV(1), 0, "dummy PCH detection=%d",g_mailbox.dummyPchResult);
    return (PAL_DUMMY_PCH_RESULT_T) (g_mailbox.dummyPchResult);
}

//======================================================================
// switch the result snap to next
VOID  pal_SnapToNext ()
{
    g_palCtx.FrameResult = (g_palCtx.FrameResult == 2 ? 0 : g_palCtx.FrameResult + 1);
}


//======================================================================
// pch nblock decode done irq request management
// enable or disable pch decode done irq
VOID  pal_CchDecDoneIrqEnable(BOOL flag)
{
    g_palCtx.palCchDecDone = flag;
}


//======================================================================
// calibration process management:
// get calibProcessEnable flag, TRUE when enable, FALSE when disable
//----------------------------------------------------------------------
BOOL pal_GetCalibProcess(VOID)
{
    return g_palDebug.calibProcessEnable;
}

//======================================================================
// calibration process management:
// set calibProcessEnable flag, TRUE when enable, FALSE when disable
//----------------------------------------------------------------------
VOID  pal_SetCalibProcess(BOOL flag)
{
    g_palDebug.calibProcessEnable = flag;
}

//======================================================================
// pal_GetAfcDacOffset
// This function used to get the difference between current g_afcDacVal and AFC_MID
//----------------------------------------------------------------------
INT16 pal_GetAfcDacOffset(VOID)
{
    INT16 offset = rfd_XcvGetAfcDacOffset();
    PAL_TRACE(PAL_LEV(16), 0, "Get AfcDacOffset=%d", offset);
    return offset;
}

//======================================================================
// pal_GetCdacOffset
// This function used to get current CDAC offset
//----------------------------------------------------------------------
INT16 pal_GetCdacOffset(VOID)
{
    INT16 offset = rfd_XcvGetCdacOffset();
    PAL_TRACE(PAL_LEV(16), 0, "Get CdacOffset=%d", offset);
    return offset;
}

//======================================================================
// pal_UpdateAfcDacValue
// This function used to set g_afcDacVal
//----------------------------------------------------------------------
VOID pal_UpdateAfcDacValue(INT16 offset)
{
    rfd_XcvSetAfcDacOffset(offset);
    PAL_TRACE(PAL_LEV(16), 0, "Set AfcDacOffset=%d", offset);
}

//======================================================================
// pal_UpdateCdacValue
// This function used to set CDAC offset
//----------------------------------------------------------------------
VOID pal_UpdateCdacValue(INT16 offset)
{
    rfd_XcvSetCdacOffset(offset);
    PAL_TRACE(PAL_LEV(16), 0, "Set CdacOffset=%d", offset);
}

//======================================================================
// pal_SetFchDetectMode(UINT8 mode)
// This function used to set fch dected mode: mode 0: fch detect before afc is locked. 1: fch detect after afc is locked.
//----------------------------------------------------------------------
VOID pal_SetFchDetectMode(UINT8 mode)
{
#if (CHIP_ASIC_ID != CHIP_ASIC_ID_GALLITE) // 8808 or later
    if(mode == 0) //afc unlocked
        g_fcch_detect_mode = GSM_FCCH_SEARCH_ALGO_DELAY4;
    else if (mode == 1) // afc locked
        g_fcch_detect_mode = GSM_FCCH_SEARCH_ALGO_FILTER;
    else if(mode ==2) //afc unlocked
#ifdef FREQ_LARGESCALE_SUPPORT	
        g_fcch_detect_mode = GSM_FCCH_SEARCH_ALGO_DELAY2;
#else
		g_fcch_detect_mode = GSM_FCCH_SEARCH_ALGO_DELAY4;
#endif		
#endif
}

//add following for AGC update of GPRS
#if 1
extern UINT8 g_mailbox_spc2pal_win_rx_burstRes_pwr[MBOX_CTX_SNAP_QTY][4];
extern UINT8 g_mailbox_spc2pal_win_rx_burstRes_snr[MBOX_CTX_SNAP_QTY][4];

// *** UINT8 palCalcPower(UINT8 bstIdx)
// Calculate Rf power received on an NBurst Rx window for multislot
// and returns it in -dBm
INLINE UINT8
palCalcPowerMultSlot(UINT8 bstIdx)
{
    UINT8 xcver_gain;
    UINT8 result;
    UINT8 snap = g_palCtx.FrameResult;

    // Get Xcver gain
    xcver_gain = g_palCtx.NBGain[snap];
#if (SPC_IF_VER <= 6)
#if ((defined CHIP_DIE_8955) || (defined CHIP_DIE_8909))
    result = (xcver_gain + g_palCtx.Adc2dBmGain) -
             g_mailbox.spc2pal.pwrs[4*snap+bstIdx];
#else
    result = (xcver_gain + g_palCtx.Adc2dBmGain) -
             g_mailbox_spc2pal_win_rx_burstRes_pwr[snap][bstIdx];
    PAL_TRACE( PAL_LEV(1), 0, "%d:%6x",bstIdx,
               (result<<16)|(xcver_gain<<8)|g_mailbox_spc2pal_win_rx_burstRes_pwr[snap][bstIdx]);
#endif
#else
    result = (xcver_gain + g_palCtx.Adc2dBmGain) -
             g_mailbox.spc2pal.win[snap].rx.burstRes.pwrs[bstIdx];
#endif
    return result;
}


// *** pal_GetNBurstResultMultSlot ***
VOID
pal_GetNBurstResultMultslot (UINT8 bstIdx, UINT16 arfcn, pal_NBurstResult_t * res)
{
    UINT8 snap = g_palCtx.FrameResult;
    PAL_PROFILE_FUNCTION_ENTRY(pal_GetNBurstResult);

    // Get Burst results

    // Get Burst results
#if (SPC_IF_VER == 5)
#if ((defined CHIP_DIE_8955) || (defined CHIP_DIE_8909))
    res->Snr = g_mailbox.spc2pal.bstSnr[4*snap+bstIdx];
    res->Pwr = palCalcPowerMultSlot(bstIdx);
#else
    res->Snr = g_mailbox_spc2pal_win_rx_burstRes_snr[snap][bstIdx];
    res->Pwr = palCalcPowerMultSlot(bstIdx);
#endif
#elif (SPC_IF_VER >= 6)
    res->Snr = g_mailbox.spc2pal.win[snap].rx.bstSnr[bstIdx];
    res->Pwr = palCalcPowerMultSlot(bstIdx);
#else
    res->Snr = 0;
    res->Pwr = 0;
#endif
    PAL_TRACE( PAL_LEV(7), 0, "GetNB{%d}: Snr=%d",bstIdx, res->Snr);

    PAL_PROFILE_FUNCTION_EXIT(pal_GetNBurstResult);
}

#endif


