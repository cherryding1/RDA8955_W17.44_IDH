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

#include "calib_m.h"

#include "rfd_defs.h"
#include "rfd_xcv.h"
#include "rfd_pa.h"

#include "calibp_debug.h"
#include "calibp_calibration.h"
#include "calibp_stub.h"

#include "sxs_io.h"

#include "myl1.h"
#include "itf_calib.h"
#include "calib_stub_nst.h"
#include "pmd_m.h"
#ifdef __VDS_QUICK_FLUSH__
#include "tgt_m.h"
#endif
// =============================================================================
//  MACROS
// =============================================================================


// =============================================================================
//  TYPES
// =============================================================================


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
#define CALIB_UNCACHED_BSS __attribute__((section(".ucbss")))

/// The global context.
PROTECTED VOLATILE CALIB_STUB_CTX_T  CALIB_UNCACHED_BSS  g_calibStubGlobalCtx;

/// The local context.
PROTECTED CALIB_STUB_CTX_T          g_calibStubLocalCtx;

/// Pointer to the context, uncached.
PROTECTED VOLATILE CALIB_STUB_CTX_T*  CALIB_UNCACHED_BSS  g_calibStubGlobalCtxPtr;

/// DSP/EQUALIZATION READY.
EXPORT VOLATILE UINT8 g_calibEquRdyFlag;



// =============================================================================
//  FUNCTIONS
// =============================================================================


// =============================================================================
// calib_StubGetFromGlobal
// -----------------------------------------------------------------------------
/// Copy the global ctx values to the local ctx. This will
/// avoid that the value of ARFCN changes before the value
/// of the Band, for example.
// =============================================================================
PRIVATE VOID calib_StubGetFromGlobal(VOID)
{
    CALIB_PROFILE_FUNCTION_ENTER(calib_StubGetFromGlobal);

    g_calibStubLocalCtx.state       = g_calibStubGlobalCtxPtr->state;
    g_calibStubLocalCtx.firstFint   = g_calibStubGlobalCtxPtr->firstFint;

    g_calibStubLocalCtx.txArfcn     = g_calibStubGlobalCtxPtr->txArfcn;
    g_calibStubLocalCtx.txPcl       = g_calibStubGlobalCtxPtr->txPcl;
    g_calibStubLocalCtx.txBand      = g_calibStubGlobalCtxPtr->txBand;
    g_calibStubLocalCtx.txTsc       = g_calibStubGlobalCtxPtr->txTsc;
    g_calibStubLocalCtx.txDac       = g_calibStubGlobalCtxPtr->txDac;

    g_calibStubLocalCtx.monArfcn    = g_calibStubGlobalCtxPtr->monArfcn;
    g_calibStubLocalCtx.monBand     = g_calibStubGlobalCtxPtr->monBand;
    g_calibStubLocalCtx.monExpPow   = g_calibStubGlobalCtxPtr->monExpPow;

    CALIB_PROFILE_FUNCTION_EXIT(calib_StubGetFromGlobal);
}




// =============================================================================
// calib_StubEquRdyHandler
// -----------------------------------------------------------------------------
/// Handler the PAL Dsp Interrupt.
// =============================================================================
PRIVATE VOID calib_StubEquRdyHandler(VOID)
{
    g_calibEquRdyFlag = TRUE;
}



/// ----------------- FINT treatement

// =============================================================================
// calib_StubFintIrqHandler
// -----------------------------------------------------------------------------
/// This function implements the calibration stub state machine as a handler
/// for the FINT interrupt.
// =============================================================================
PRIVATE VOID calib_StubFintIrqHandler(VOID)
{
    UINT32 i;
    PRIVATE UINT32 state=0xff;
    // Very first FINT. Used to skip the handling of the very first
    // FINT. This is useful because, when the FINT is demasked, we don't
    // know when the previous FINT happened before. So if the demasking
    // is done very late in the frame, the FINT handling might take too
    // much time and lead to a FINT re-entrance.
    PRIVATE BOOL veryfirstFint = TRUE;

    CALIB_PROFILE_FUNCTION_ENTER(calib_StubFintIrqHandler);

    /// We skip the very first FINT handling.
    if (veryfirstFint)
    {
        veryfirstFint = FALSE;
        CALIB_PROFILE_FUNCTION_EXIT(calib_StubFintIrqHandler);
        return;
    }
    if(state != g_calibStubLocalCtx.state)
    {
        SXS_TRACE(_SXR|TSTDOUT, "calib_Stub:%d->%d", state, g_calibStubLocalCtx.state);
        state = g_calibStubLocalCtx.state;
    }

    pal_FrameStart();

    /// ----- Update the local parameters if a new state came from HST.

    /// Copy the global ctx values to the local ctx.
    if (TRUE == g_calibStubGlobalCtxPtr->firstFint)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdGlobal);

        SXS_TRACE(_SXR|TSTDOUT, "g_calibStubGlobalCtxPtr:%d->%d", state, g_calibStubGlobalCtxPtr->state);
        calib_StubGetFromGlobal();

        /// Execute the command only once.
        g_calibStubGlobalCtxPtr->firstFint = FALSE;
    }

    /// Update GSM counters.
    g_calibStubGlobalCtxPtr->fn = (++g_calibStubGlobalCtxPtr->fn) % 2715648;
    g_calibStubGlobalCtxPtr->t3 = (++g_calibStubGlobalCtxPtr->t3) % 51;
    g_calibStubGlobalCtxPtr->t2 = (++g_calibStubGlobalCtxPtr->t2) % 26;

    /// Some LED blinking.
    static int m_dacnumindex = 0;
    if (g_calibStubLocalCtx.state == CALIB_STUB_NO_STATE)
    {
#ifdef __VDS_QUICK_FLUSH__
        extern VOID calib_SetStationFlag();
        calib_SetStationFlag();
#endif
        m_dacnumindex = 0;
        calib_PulseLight(2);
    }
    else
    {
        calib_PulseLight(15);
    }

    /// ----- Execute the subcommands from HST.

    // Set the crystal frequency offset, used to adjust to rough
    // frequency parameter (sometimes called CDAC).
    if (CALIB_STUB_XTAL_IDLE != g_calibStubGlobalCtxPtr->setXtalFreqOffset)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdXtal);

        // Execute the command and send the result to HST thanks to
        // the field 'XtalCalibDone'.
        g_calibStubGlobalCtxPtr->xtalCalibDone =
            rfd_XcvCalibXtalFreqOffset(g_calibStubGlobalCtxPtr->setXtalFreqOffset);

        SXS_TRACE(TSTDOUT, "Calibration: Crystal offset set to: %d",
                  g_calibStubGlobalCtxPtr->setXtalFreqOffset);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setXtalFreqOffset = CALIB_STUB_XTAL_IDLE;
    }

    // Set the AFC bound, used to calculate the min and max AFC
    // influence on the frequency.
    if (CALIB_STUB_AFC_BOUND_NO != g_calibStubGlobalCtxPtr->setAfcBound)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdAfcBound);

        rfd_XcvCalibSetAfcBound((RFD_AFC_BOUND_T)g_calibStubGlobalCtxPtr->setAfcBound);

        SXS_TRACE(TSTDOUT, "Calibration: AFC bound set to: %d",
                  g_calibStubGlobalCtxPtr->setAfcBound);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setAfcBound = CALIB_STUB_AFC_BOUND_NO;
    }

    /// Set the AFC frequency offset, used to calculate the AFC gain.
    if (0 != g_calibStubGlobalCtxPtr->setAfcFreqOffset)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdAfcFOf);

        rfd_XcvCalibCalcAfcGain(    g_calibStubGlobalCtxPtr->setAfcFreqOffset,
                                    (GSM_RFBAND_T)g_calibStubGlobalCtxPtr->txBand);

        SXS_TRACE(TSTDOUT, "Calibration: AFC frequency offset set to: %d"
                  ", for band: %d",
                  g_calibStubGlobalCtxPtr->setAfcFreqOffset,
                  g_calibStubGlobalCtxPtr->txBand);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setAfcFreqOffset = 0;
    }
    if (0 != g_calibStubGlobalCtxPtr->setChangeDACAfcFreq)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdAfcFOf);
        rfd_XcvCalibCalcAfcFreqChangeDAC(g_calibStubGlobalCtxPtr->setChangeDACAfcFreq, g_calibStubGlobalCtxPtr->txBand);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setChangeDACAfcFreq = 0;
    }

    /// Set the changed CDAC AFC frequency offset, used to calculate the AFC gain.
    // influence on the frequency.
    if (CALIB_STUB_AFC_BOUND_NO != g_calibStubGlobalCtxPtr->setChangeDACAfcBound)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdAfcBound);

        rfd_XcvCalibSetAfcBoundChangeDAC((RFD_AFC_BOUND_T)g_calibStubGlobalCtxPtr->setChangeDACAfcBound);

        SXS_TRACE(TSTDOUT, "Calibration: AFC bound set to: %d",
                  g_calibStubGlobalCtxPtr->setChangeDACAfcBound);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setChangeDACAfcBound = CALIB_STUB_AFC_BOUND_NO;
    }

    /// Set the AFC frequency offset, used to calculate the AFC gain.
    if (0 != g_calibStubGlobalCtxPtr->setChangeDACAfcFreqOffset)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdAfcFOf);

        rfd_XcvCalibCalcAfcGainChangeDAC(    g_calibStubGlobalCtxPtr->setChangeDACAfcFreqOffset,
                                             (GSM_RFBAND_T)g_calibStubGlobalCtxPtr->txBand);

        SXS_TRACE(TSTDOUT, "Calibration: AFC frequency offset set to: %d"
                  ", for band: %d",
                  g_calibStubGlobalCtxPtr->setChangeDACAfcFreqOffset,
                  g_calibStubGlobalCtxPtr->txBand);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setChangeDACAfcFreqOffset = 0;
    }
// Set the insertion loss offset, used to adjust the ILoss.
    if (CALIB_STUB_ILOSS_IDLE != g_calibStubGlobalCtxPtr->setILossOffset)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdILoss);

        // Execute the command and send the result to HST thanks to
        // the field 'ILossCalibDone'.
        g_calibStubGlobalCtxPtr->iLossCalibDone = rfd_XcvCalibSetILoss(
                    g_calibStubGlobalCtxPtr->setILossOffset,
                    (GSM_RFBAND_T)g_calibStubGlobalCtxPtr->monBand,
                    (UINT16*)&g_calibStubGlobalCtxPtr->iLossNextArfcn);

        SXS_TRACE(TSTDOUT, "Calibration: Insertion loss offset set to: %d"
                  ", for band: %d",
                  g_calibStubGlobalCtxPtr->setILossOffset,
                  g_calibStubGlobalCtxPtr->monBand);
        SXS_TRACE(TSTDOUT, "Calibration: Next ARFCN value: %d",
                  g_calibStubGlobalCtxPtr->iLossNextArfcn);
        SXS_TRACE(TSTDOUT, "Calibration: Local State: %d",
                  g_calibStubLocalCtx.state);

        /* Execute this command only once. */
        g_calibStubGlobalCtxPtr->setILossOffset = CALIB_STUB_ILOSS_IDLE;

        CALIB_PROFILE_PULSE(calib_StubCmdILoss);
    }

    /// Set a PA profile measurement point, used interpolate the PA profile.
    if (CALIB_STUB_PA_PROF_IDLE != g_calibStubGlobalCtxPtr->setPAProfMeas)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdPaProf);

        // Execute the command and send the result to HST thanks to
        // the field 'paProfCalibDone' and tell what is the next PA DAC
        // point to measure.
        if(g_calibStubGlobalCtxPtr->setPAProfMeas == 5000)
            g_calibStubGlobalCtxPtr->paProfCalibDone = rfd_PaCalibfastPaProfile(
                        (GSM_RFBAND_T)g_calibStubGlobalCtxPtr->txBand);
        else
        {
            g_calibStubGlobalCtxPtr->paProfCalibDone = rfd_PaCalibPaProfile(
                        (GSM_RFBAND_T)g_calibStubGlobalCtxPtr->txBand,
                        g_calibStubGlobalCtxPtr->setPAProfMeas,
                        (UINT16*)&g_calibStubGlobalCtxPtr->paProfNextDacVal);
        }

        SXS_TRACE(TSTDOUT, "Calibration: PA profile point set to power: %d"
                  ", for band: %d",
                  g_calibStubGlobalCtxPtr->setPAProfMeas,
                  g_calibStubGlobalCtxPtr->txBand);
        SXS_TRACE(TSTDOUT, "Calibration: Next DAC value: %d",
                  g_calibStubGlobalCtxPtr->paProfNextDacVal);

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setPAProfMeas = CALIB_STUB_PA_PROF_IDLE;
    }

    /// Request a calibration update.
    if (0 != g_calibStubGlobalCtxPtr->setCalibUpdate)
    {
        CALIB_PROFILE_PULSE(calib_StubCmdUpdate);

        // Execute the function of all RF modules to update
        // the calibration values.
        rfd_XcvCalibUpdateValues();

        SXS_TRACE(TSTDOUT, "Calibration: Update values done.");

        /// Execute this command only once.
        g_calibStubGlobalCtxPtr->setCalibUpdate = 0;
    }

    // Set the DC offset fields to "error" because DC offset can only
    // be measured when the Target is in power minotoring mode.
    if (g_calibStubLocalCtx.state != CALIB_STUB_MONIT_STATE)
    {
        g_calibStubGlobalCtxPtr->dcoAverI = CALIB_STUB_DCO_ERROR;
        g_calibStubGlobalCtxPtr->dcoAverQ = CALIB_STUB_DCO_ERROR;
    }

    /// Do the action, depending on the state.
    switch (g_calibStubLocalCtx.state)
    {
        ///  State off.

        case CALIB_STUB_NO_STATE:
        {
            /// Init at first FINT.
            if (g_calibStubLocalCtx.firstFint)
            {
                g_calibStubLocalCtx.firstFint = FALSE;
            }
        }
        break;

        /// ----- MONIT_STATE.

        case CALIB_STUB_MONIT_STATE:
        {
            /// Init at first FINT.
            if (g_calibStubLocalCtx.firstFint)
            {
                g_calibStubLocalCtx.firstFint = FALSE;
                g_calibStubGlobalCtxPtr->monPower = CALIB_STUB_SEARCH_POWER;
                g_calibStubGlobalCtxPtr->nbPower = CALIB_STUB_SEARCH_POWER;
                g_calibStubGlobalCtxPtr->monBitmap = 0;

                /// Initialize PAL with the selected band.
                myl1_BandInit(g_calibStubLocalCtx.monBand);

                /// Init the DC offset average system.
                g_calibStubGlobalCtxPtr->dcoAccCount = 0;
                g_calibStubGlobalCtxPtr->dcoAverI = CALIB_STUB_DCO_IDLE;
                g_calibStubGlobalCtxPtr->dcoAverQ = CALIB_STUB_DCO_IDLE;
            }
            else
            {
                /// Display only when the monit is done.
                myl1_Monit( g_calibStubLocalCtx.monArfcn,
                            g_calibStubLocalCtx.monBand,
                            g_calibStubLocalCtx.monExpPow,
                            g_calibStubGlobalCtxPtr->t2,
                            (UINT8*)&g_calibStubGlobalCtxPtr->monPower,
                            (UINT8*)&g_calibStubGlobalCtxPtr->nbPower );

                // When there is enough DC offset measurements,
                // do the average for HST needs.
                if (g_calibStubGlobalCtxPtr->dcoAccCount >= CALIB_DCO_ACC_COUNT)
                {
                    INT32 dcoAccI = 0;
                    INT32 dcoAccQ = 0;

                    for (i = 0; i < CALIB_DCO_ACC_COUNT; i++)
                    {
                        dcoAccI += g_calibStubGlobalCtxPtr->dcoI[i];
                        dcoAccQ += g_calibStubGlobalCtxPtr->dcoQ[i];
                    }

                    g_calibStubGlobalCtxPtr->dcoAverI =
                        dcoAccI / CALIB_DCO_ACC_COUNT;
                    g_calibStubGlobalCtxPtr->dcoAverQ =
                        dcoAccQ / CALIB_DCO_ACC_COUNT;
                }
            }
        }
        break;

        /// ----- FOF_STATE.

        case CALIB_STUB_FOF_STATE:
        {
            /// Init at first FINT.
            if (g_calibStubLocalCtx.firstFint)
            {
                g_calibStubLocalCtx.firstFint = FALSE;
                g_calibStubGlobalCtxPtr->meanFOf = CALIB_STUB_SEARCH_FOF;

                /// L1 init for the get FOf FSM.
                myl1_GetFOf(    TRUE,
                                g_calibStubLocalCtx.monArfcn,
                                g_calibStubLocalCtx.monBand,
                                g_calibStubLocalCtx.monExpPow,
                                (INT32*)&g_calibStubGlobalCtxPtr->meanFOf );
            }
            /// Search FOf
            else
            {
                myl1_GetFOf(    FALSE,
                                g_calibStubLocalCtx.monArfcn,
                                g_calibStubLocalCtx.monBand,
                                g_calibStubLocalCtx.monExpPow,
                                (INT32*)&g_calibStubGlobalCtxPtr->meanFOf );
            }
        }
        break;

        /// ----- IDLE_STATE.

        case CALIB_STUB_SYNCH_STATE:
            if (g_calibStubLocalCtx.firstFint)
            {
                g_calibStubLocalCtx.firstFint = FALSE;

                /// Initialize PAL with the selected band.
                myl1_BandInit(g_calibStubLocalCtx.monBand);
            }
            else
            {
                myl1_Synch();
            }
            break;

        /// Called by MyL1.
        case CALIB_STUB_IDLE_STATE:
            myl1_Idle();
            break;

        /// ----- PA_STATE.

        case CALIB_STUB_PA_STATE:
        {
            /// Init at first FINT.
            if (g_calibStubLocalCtx.firstFint)
            {
                g_calibStubGlobalCtxPtr->bsic = 0;
                g_calibStubGlobalCtxPtr->fn = 0;
                g_calibStubGlobalCtxPtr->t2 = 0;
                g_calibStubGlobalCtxPtr->t3 = 0;
                g_calibStubLocalCtx.firstFint = FALSE;

                /// Initialize PAL with the selected band.
                myl1_BandInit(g_calibStubLocalCtx.txBand);
            }
            else
            {
                /// Send the burst.
                if((g_calibStubLocalCtx.txDac & 0x3ff) < 5)
                {

                    if(m_dacnumindex < 2)
                    {
                        g_calibStubLocalCtx.txDac = 0x8000;
                    }
                    else if(1 < m_dacnumindex && m_dacnumindex < 4)
                    {
                        g_calibStubLocalCtx.txDac = 0x8001;
                    }
                    else if(3 < m_dacnumindex && m_dacnumindex< 6)
                    {
                        g_calibStubLocalCtx.txDac = 0x8002;
                    }
                    else if(5 < m_dacnumindex && m_dacnumindex< 8)
                    {
                        g_calibStubLocalCtx.txDac = 0x8003;
                    }
                    else if(7 < m_dacnumindex && m_dacnumindex< 10)
                    {
                        g_calibStubLocalCtx.txDac = 0x8004;
                    }
                    if(m_dacnumindex < 10)
                    {
                        myl1_SendBurstDac(  g_calibStubLocalCtx.txArfcn,
                                            g_calibStubLocalCtx.txDac,
                                            g_calibStubLocalCtx.txBand,
                                            g_calibStubLocalCtx.txTsc);
                    }
                    m_dacnumindex++;
                }
                else
                {
                    myl1_SendBurstDac(  g_calibStubLocalCtx.txArfcn,
                                        g_calibStubLocalCtx.txDac,
                                        g_calibStubLocalCtx.txBand,
                                        g_calibStubLocalCtx.txTsc);
                }
            }
        }
        break;

        /// ----- TX_STATE.

        case CALIB_STUB_TX_STATE:
        {
            /// Init at first FINT.
            if (g_calibStubLocalCtx.firstFint)
            {
                g_calibStubGlobalCtxPtr->bsic = 0;
                g_calibStubGlobalCtxPtr->fn = 0;
                g_calibStubGlobalCtxPtr->t2 = 0;
                g_calibStubGlobalCtxPtr->t3 = 0;
                g_calibStubLocalCtx.firstFint = FALSE;

                /// Initialize PAL with the selected band.
                myl1_BandInit(g_calibStubLocalCtx.txBand);
            }
            else
            {
                if(g_calibStubLocalCtx.txPcl > 63)
                {
                    if(m_dacnumindex < 2)
                    {
                        if(g_calibStubLocalCtx.txBand < 2)
                        {
                            g_calibStubLocalCtx.txPcl = 0x45;
                            if(g_calibStubLocalCtx.txBand == 0)
                            {
                                g_calibStubLocalCtx.txArfcn = 128;
                            }
                            else
                            {
                                g_calibStubLocalCtx.txArfcn = 1;
                            }
                        }
                        else
                        {
                            g_calibStubLocalCtx.txPcl = 0x40;
                            g_calibStubLocalCtx.txArfcn = 512;
                        }
                    }
                    else if(1 < m_dacnumindex && m_dacnumindex < 4)
                    {
                        if(g_calibStubLocalCtx.txBand < 2)
                        {
                            g_calibStubLocalCtx.txPcl = 0x46;
                            if(g_calibStubLocalCtx.txBand == 0)
                            {
                                g_calibStubLocalCtx.txArfcn = 128;
                            }
                            else
                            {
                                g_calibStubLocalCtx.txArfcn = 1;
                            }
                        }
                        else
                        {
                            g_calibStubLocalCtx.txArfcn = 512;
                            g_calibStubLocalCtx.txPcl = 0x41;
                        }
                    }
                    else if(3 < m_dacnumindex && m_dacnumindex< 6)
                    {
                        if(g_calibStubLocalCtx.txBand < 2)
                        {
                            g_calibStubLocalCtx.txPcl = 0x47;
                            if(g_calibStubLocalCtx.txBand == 0)
                            {
                                g_calibStubLocalCtx.txArfcn = 128;
                            }
                            else
                            {
                                g_calibStubLocalCtx.txArfcn = 1;
                            }
                        }
                        else
                        {
                            g_calibStubLocalCtx.txPcl = 0x42;
                            g_calibStubLocalCtx.txArfcn = 512;
                        }
                    }
                    else if(5 < m_dacnumindex && m_dacnumindex< 8)
                    {
                        if(g_calibStubLocalCtx.txBand < 2)
                        {
                            g_calibStubLocalCtx.txPcl = 0x48;
                            if(g_calibStubLocalCtx.txBand == 0)
                            {
                                g_calibStubLocalCtx.txArfcn = 128;
                            }
                            else
                            {
                                g_calibStubLocalCtx.txArfcn = 1;
                            }
                        }
                        else
                        {
                            g_calibStubLocalCtx.txPcl = 0x43;
                            g_calibStubLocalCtx.txArfcn = 512;
                        }
                    }
                    else if(7 < m_dacnumindex && m_dacnumindex< 10)
                    {
                        if(g_calibStubLocalCtx.txBand < 2)
                        {
                            g_calibStubLocalCtx.txPcl = 0x85;
                            if(g_calibStubLocalCtx.txBand == 0)
                            {
                                g_calibStubLocalCtx.txArfcn = 251;
                            }
                            else
                            {
                                g_calibStubLocalCtx.txArfcn = 124;
                            }
                        }
                        else
                        {
                            g_calibStubLocalCtx.txPcl = 0x80;
                            if(g_calibStubLocalCtx.txBand == 2)
                            {
                                g_calibStubLocalCtx.txArfcn = 885;
                            }
                            else
                            {
                                g_calibStubLocalCtx.txArfcn = 810;
                            }
                        }
                    }
                    if(m_dacnumindex < 10)
                    {

                        myl1_SendBurst( g_calibStubLocalCtx.txArfcn,
                                        g_calibStubLocalCtx.txPcl,
                                        g_calibStubLocalCtx.txBand,
                                        g_calibStubLocalCtx.txTsc);
                    }
                    m_dacnumindex++;
                }
                else
                {
                    myl1_SendBurst( g_calibStubLocalCtx.txArfcn,
                                    g_calibStubLocalCtx.txPcl,
                                    g_calibStubLocalCtx.txBand,
                                    g_calibStubLocalCtx.txTsc);
                }
            }
        }
        break;

        case CALIB_STUB_NSTREQ_STATE:
            calib_StubNstParse();
            break;
        case CALIB_STUB_NST_STATE:
            if (calib_GetReqCurrState())
            {
                switch (calib_GetReqState())
                {
                    case CALIB_NST_SE_BER_CONNECT_REQ:
                        calib_SetNststateIdle();
                        myl1_BandInit(g_calibStubGlobalCtxPtr->monBand);
                        break;
                }
                calib_StubNstParse();
                calib_SetReqCurrState(0);
            }

            calib_StubNstProcess();
            break;
			
    case  CALIB_STUB_GPS_STATE:
		#ifdef GPS_TEST
	            pmd_EnablePower(PMD_POWER_CAMERA, TRUE);
		     SXS_TRACE(TSTDOUT, "GPS POWER ON");
		#endif
		     g_calibStubLocalCtx.state=CALIB_STUB_NO_STATE;
	     break;
    case   CALIB_STUB_SHUTDOWN_STATE:
   	        hal_SysShutdown();
		 SXS_TRACE(TSTDOUT, "PHONE SHUT DOWN!");
		 g_calibStubLocalCtx.state=CALIB_STUB_NO_STATE;
	     break;
        default:
            break;
    }

    /// End of FINT treatment.
    pal_FrameEnd();
    g_calibEquRdyFlag = FALSE;

    CALIB_PROFILE_FUNCTION_EXIT(calib_StubFintIrqHandler);
}




// =============================================================================
// calib_StubOpen
// -----------------------------------------------------------------------------
/// Initializes the calibration stub. Among operation is the registering of the
/// calib stub state machine as the FINT Irq handler.
// =============================================================================
PROTECTED VOID calib_StubOpen(BOOL Flag)
{
    /// Inform HCS which calib context to use to send commands.
    calib_DaemonSetContext((VOID*)&g_calibStubGlobalCtx);
    calib_DaemonSetNsftContext((VOID*)&g_calibStubGlobalNstCtx);
    g_calibStubGlobalCtxPtr = (CALIB_STUB_CTX_T*)&g_calibStubGlobalCtx;

#if !defined(_T_UPGRADE_PROGRAMMER) && !defined(CHIP_HAS_AP)
    FACTORY_Init();
#endif
    /// Init some of the context parameters.
    g_calibStubLocalCtx.state = CALIB_STUB_NO_STATE;
    g_calibStubLocalCtx.firstFint = FALSE;

    g_calibStubGlobalCtxPtr->state = CALIB_STUB_NO_STATE;
    g_calibStubGlobalCtxPtr->firstFint = FALSE;
    g_calibStubGlobalCtxPtr->monBitmap = 0;

    g_calibStubGlobalCtxPtr->setXtalFreqOffset = CALIB_STUB_XTAL_IDLE;
    g_calibStubGlobalCtxPtr->setPAProfMeas = CALIB_STUB_PA_PROF_IDLE;
    g_calibStubGlobalCtxPtr->setAfcBound = CALIB_STUB_AFC_BOUND_NO;
    g_calibStubGlobalCtxPtr->setILossOffset = CALIB_STUB_ILOSS_IDLE;
    g_calibStubGlobalCtxPtr->setChangeDACAfcBound = CALIB_STUB_AFC_BOUND_NO;
    g_calibStubGlobalCtxPtr->setChangeDACAfcFreqOffset = 0;
    g_calibStubGlobalCtxPtr->setChangeDACAfcFreq = 0;
    g_calibStubGlobalCtxPtr->setAfcFreqOffset = 0;
    g_calibStubGlobalCtxPtr->setCalibUpdate = 0;

    /// Init the DC offset average system.
    g_calibStubGlobalCtxPtr->dcoAccCount = 0;
    g_calibStubGlobalCtxPtr->dcoAverI = CALIB_STUB_DCO_ERROR;
    g_calibStubGlobalCtxPtr->dcoAverQ = CALIB_STUB_DCO_ERROR;

    if(Flag == TRUE)
    {
        // PA calib init.
        rfd_PaCalibInit();
    }

    /// RF init.
    pal_RfWakeUp();
#ifdef __VDS_QUICK_FLUSH__
    extern VOID calib_GetStationPassFlag();
    calib_GetStationPassFlag();
#endif
    /// FINT init.
    pal_SetUsrVector(PAL_FINT_IRQ, (VOID (*)(UINT32))calib_StubFintIrqHandler);
    pal_SetUsrVectorMsk(PAL_FINT_IRQ, PAL_FINT_OCCURED);
    pal_SetUsrVector (PAL_DSP_IRQ, (VOID (*)(UINT32))calib_StubEquRdyHandler);
    if(Flag == TRUE)
    {
        calib_StubNstOpen();
    }
}



