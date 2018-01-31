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
#include "gsm.h"
#include "hal_tcu.h"
#include "calib_m.h"

#include "drv_pa.h"
#include "drv_pa_calib.h"

#include "rfd_cfg.h"
#include "rfd_pa.h"
#include "rfd_defs.h"


const RFD_PA_PARAMS_T g_paParams=
{
    .dtxTcoSettings = 0,
    .rxOnTcoSettings = 0,
    // Window bounds
    // NOTE : these timings will be wrong if changed by a calibration
    // file, they should be updated at execution time
    .winBounds.rxSetup = 0,
    .winBounds.rxHold  = 0,
    .winBounds.txSetup = 0,
    .winBounds.txHold  = 0
};

const RFD_PA_PARAMS_T* rfd_PaGetParameters(VOID)
{
    return &g_paParams;
}

void rfd_PaTxOn(INT16 start, GSM_RFBAND_T band)
{
    return;
}

void rfd_PaTxOff(INT16 stop, GSM_RFBAND_T band)
{
    return;
}

VOID rfd_PaSetRamps(RFD_WIN_T *Win, INT16 start)
{
    return;
}

VOID rfd_PaFrameStart(VOID)
{
    return;
}


VOID rfd_PaOpen(CONST PA_CONFIG_T* paConfig)
{
    return;
}

VOID rfd_PaClose(VOID)
{
    return;
}

VOID rfd_PaWakeUp(VOID)
{
    return;
}

VOID rfd_PaSleep(RFD_LP_MODE_T lp_mode)
{
    return;
}



//----------------------//
// Calibration Related  //
//----------------------//

// TODO : see how to handle calibration in the case of a "dummy" component

const CALIB_PA_T pa_default_calib =
{
    DEFAULT_CALIB_TIMES,
    DEFAULT_CALIB_PARAM,
    DEFAULT_CALIB_PALCUST
};

const CALIB_PA_T *
rfd_PaCalibGetDefault(void)
{
    return &pa_default_calib;
}

void
rfd_PaCalibUpdateValues(void)
{
    return;
}

void
rfd_PaCalibResetValues(void)
{
    return;
}

UINT8
rfd_PaCalibPaProfile(GSM_RFBAND_T band, INT32 PowerMeas, UINT16 *NextDAC)
{
    rfd_fprintf((RFD_INFO_TRC, "Calib error : Not implemented !"));
    return CALIB_PROCESS_ERROR;
}
