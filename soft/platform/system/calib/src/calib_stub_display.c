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

#include "calib_m.h"
#include "hal_pwm.h"

#include "fmg_m.h"
#include "lcdd_m.h"


#include "calibp_debug.h"
#include "calibp_calibration.h"
#include "calibp_stub.h"





/// ----------------- Some internal defines and global variables.

/// Needs to be defined before we include itf_msg.h
typedef UINT8 MsgBody_t;
#include "itf_msg.h"

/* ----------------- Don't use the screen and the sin generation when
 * compiling the mini version. */

#ifndef CES_DISPLAY
#define fmg_PrintfSetXY(x, y)
#define fmg_Printf(x, ...)
#endif /// CES_DISPLAY




/// ----------------- Display functions and stuff.

/// Adjustment for small screens.
#ifndef LCM_SM
#define MMI_SKIP_LINE()    fmg_Printf("                ");
#define TITLE_LINE        6
#define STATE_LINE        8
#define DICTAPHONE_LINE TITLE_LINE
#define DICTAPHONE_COL  15
#else
#define MMI_SKIP_LINE()
#define TITLE_LINE        6
#define STATE_LINE        7
#define DICTAPHONE_LINE TITLE_LINE
#define DICTAPHONE_COL  15
#endif


// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================
// =============================================================================
// g_calibStubDispBandStr
// -----------------------------------------------------------------------------
/// Array to easily print which band is the info related to.
// =============================================================================
#ifdef CES_DISPLAY
PRIVATE CHAR* g_calibStubDispBandStr [CALIB_STUB_BAND_QTY] =
{
    "     GSM850    ", "    GSM900     ",
    "     DCS1800   ", "    PCS1900    "
};
#endif

// =============================================================================
// calib_StubClearScreen
// -----------------------------------------------------------------------------
/// Clear screen function.
// =============================================================================
#ifdef CES_DISPLAY
PRIVATE VOID calib_StubClearScreen(VOID)
{
    fmg_PrintfSetXY(0, STATE_LINE);
    fmg_Printf("                "
               "                "
               "                "
               "                "
               "                "
               "                "
               "                "
               "                "
               "                ");
}
#endif


// =============================================================================
// calib_DispState
// -----------------------------------------------------------------------------
/// Display depending on the states.
/// @param full If \c TRUE,  clear the screen and rewrite everything.
// =============================================================================
PROTECTED VOID calib_DispState(BOOL full)
{
#ifdef CES_DISPLAY
    CALIB_PROFILE_FUNCTION_ENTER(calib_StubDispState);

    switch (g_calibStubLocalCtx.state)
    {

        /// ----- NO_STATE.

        case CALIB_STUB_NO_STATE:
        {
            fmg_PrintfSetXY(0, TITLE_LINE);
///        fmg_Printf("      JCES     ");
            fmg_Printf(" CES v0.%02d.%02d", // FIXME SET to 1 !
                       CALIB_MAJ_VERSION, CALIB_MIN_VERSION);
            fmg_PrintfSetXY(0, TITLE_LINE + 1);
            fmg_Printf("                ");
            fmg_Printf("                ");

///        fmg_Printf(" Version 1.%02d.%02d",
///                CALIB_MAJ_VERSION, CALIB_MIN_VERSION);
#ifndef LCM_SM
            fmg_Printf(" Use Host tools ");
            fmg_Printf(" and scripts to ");
            fmg_Printf(" control CES.   ");
            fmg_Printf(" Calib Tool etc ");
#endif
            fmg_Printf("                ");
            fmg_Printf("                ");
            fmg_Printf("                ");
        }
        break;

        /// ----- MONIT_STATE.

        case CALIB_STUB_MONIT_STATE:
        {
            if (full)
            {
                calib_StubClearScreen();
            }
            fmg_PrintfSetXY(0, STATE_LINE);
            fmg_Printf("  Power Monitor ");
            MMI_SKIP_LINE();
            fmg_Printf("%s\n", g_calibStubDispBandStr[g_calibStubLocalCtx.monBand]);
            fmg_Printf(" ARFCN: %4d   \n", g_calibStubLocalCtx.monArfcn);
            fmg_Printf(" Exptd: %3d dBm\n", g_calibStubLocalCtx.monExpPow);
            fmg_Printf(" MoPow: %3d dBm\n", g_calibStubGlobalCtxPtr->monPower);
            fmg_Printf(" NBPow: %3d dBm\n", g_calibStubGlobalCtxPtr->nbPower);
            MMI_SKIP_LINE();
        }
        break;

        /// ----- FOF_STATE.

        case CALIB_STUB_FOF_STATE:
        {
            if (full)
            {
                calib_StubClearScreen();
            }

            fmg_PrintfSetXY(0, STATE_LINE);
            fmg_Printf("  Freq Monitor  ");
            fmg_Printf("                ");
            fmg_Printf("%s\n", g_calibStubDispBandStr[g_calibStubLocalCtx.monBand]);
            fmg_Printf(" ARFCN: %4d   \n", g_calibStubLocalCtx.monArfcn);

            if (g_calibStubGlobalCtxPtr->meanFOf == CALIB_STUB_SEARCH_FOF)
            {
                fmg_Printf(" FOf:    SEARCH ");
            }
            else
            {
                fmg_Printf(  " FOf:    %+6d ", g_calibStubGlobalCtxPtr->meanFOf);
            }
            MMI_SKIP_LINE();
            fmg_Printf(" Exptd: %3d dBm ", g_calibStubLocalCtx.monExpPow);
            MMI_SKIP_LINE();
        }
        break;

        /// ----- SYNCH_STATE.

        case CALIB_STUB_SYNCH_STATE:
        {
            if (full)
            {
                calib_StubClearScreen();
            }

            fmg_PrintfSetXY(0, STATE_LINE);
            fmg_Printf(" Synchronizing  ");
            fmg_Printf("                ");
            fmg_Printf("%s\n", g_calibStubDispBandStr[g_calibStubLocalCtx.monBand]);
            fmg_Printf(" ARFCN: %4d   \n", g_calibStubLocalCtx.monArfcn);
            MMI_SKIP_LINE();
            fmg_Printf("      ...       ");
            fmg_Printf("                ");
            MMI_SKIP_LINE();
        }
        break;

        /// ----- IDLE_STATE.

        case CALIB_STUB_IDLE_STATE:
        {
            if (full)
            {
                calib_StubClearScreen();
            }

            fmg_PrintfSetXY(0, STATE_LINE);
            fmg_Printf("   Idle State   ");
            MMI_SKIP_LINE();
            fmg_Printf("%s\n", g_calibStubDispBandStr[g_calibStubLocalCtx.monBand]);
            fmg_Printf(" ARFCN:    %4d\n", g_calibStubLocalCtx.monArfcn);
            fmg_Printf(" RSSI:  %3d dBm ", g_calibStubGlobalCtxPtr->nbPower);
            fmg_Printf(" SnR:       %3d ", g_calibStubGlobalCtxPtr->snR);
            fmg_Printf(" BitErr:    %3d ", g_calibStubGlobalCtxPtr->bitError);
            MMI_SKIP_LINE();
        }
        break;

        /// ----- TX_STATE.

        case CALIB_STUB_TX_STATE:
        {
            if (full)
            {
                calib_StubClearScreen();
            }

            fmg_PrintfSetXY(0, STATE_LINE);
            fmg_Printf("  Sending Burst ");
            fmg_Printf("                ");
            fmg_Printf("%s\n", g_calibStubDispBandStr[g_calibStubLocalCtx.txBand]);
            fmg_Printf(" ARFCN: %4d   \n", g_calibStubLocalCtx.txArfcn);
            fmg_Printf(  " PCL:  %5d    ", g_calibStubLocalCtx.txPcl);
            fmg_Printf(  " TSC:  %5d    ", g_calibStubLocalCtx.txTsc);
            MMI_SKIP_LINE();
            MMI_SKIP_LINE();
        }
        break;

        /// ----- PA_STATE.

        case CALIB_STUB_PA_STATE:
        {
            if (full)
            {
                calib_StubClearScreen();
            }

            fmg_PrintfSetXY(0, STATE_LINE);
            fmg_Printf("  PA Profiling  ");
            fmg_Printf("                ");
            fmg_Printf("%s\n", g_calibStubDispBandStr[g_calibStubLocalCtx.txBand]);
            fmg_Printf(" ARFCN: %4d   \n", g_calibStubLocalCtx.txArfcn);

            // If the bit 15 of TxDac is 1, it means that TxDac is a value,
            // else it is an index.
            if (g_calibStubLocalCtx.txDac & (1 << 15))
            {
                fmg_Printf(  " DAC Val: %5d ",
                             g_calibStubLocalCtx.txDac & ~(1 << 15));
            }
            else
            {

                switch (g_calibStubLocalCtx.txBand)
                {
                    case CALIB_STUB_BAND_PCS1900:
                    case CALIB_STUB_BAND_DCS1800:
                        fmg_Printf(  " DAC Val: %5d ",
                                     g_calibCalibration.pa->palcust.profileInterpDp
                                     [g_calibStubLocalCtx.txDac]);
                        break;

                    case CALIB_STUB_BAND_GSM850:
                    case CALIB_STUB_BAND_GSM900:
                    default:
                        fmg_Printf(  " DAC Val: %5d ",
                                     g_calibCalibration.pa->palcust.profileInterpG
                                     [g_calibStubLocalCtx.txDac]);
                }
            }
            MMI_SKIP_LINE();
            MMI_SKIP_LINE();
            MMI_SKIP_LINE();
        }
        break;

        default:
            // To avoid warnings
            break;

    } /// switch (g_calibStubLocalCtx.state)

    CALIB_PROFILE_FUNCTION_EXIT(calib_StubDispState);
#endif // CES_DISPLAY
}



// =============================================================================
// calib_PulseLight
// -----------------------------------------------------------------------------
/// I am alive and I claim it ! function, through a PWL (Keyboard backlighting
/// presumably. The pulsing speed is used to display the calibration state.
///
/// @param speed Speed of the glow.
// =============================================================================
PROTECTED VOID calib_PulseLight(INT32 speed)
{
    PRIVATE INT32 lightPulser = 0;
    PRIVATE BOOL lightDir = 0;

    if (lightDir)
    {
        lightPulser += speed;
        if (lightPulser > 1023 - 256)
        {
            lightPulser = 1023 - 256;
            lightDir = 0;
        }
    }
    else
    {
        lightPulser -= speed;
        if (lightPulser < 0 + 256)
        {
            lightPulser = 0 + 256;
            lightDir = 1;
        }
    }

    // Pwl Simple is the screen backlight, presumably ...
    hal_PwlGlow((lightPulser / 4) % 255, 0, 0, FALSE);
}





