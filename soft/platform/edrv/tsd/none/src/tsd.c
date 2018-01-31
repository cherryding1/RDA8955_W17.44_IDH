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

#include "tsd_m.h"

PUBLIC VOID tsd_Open(TSD_CALLBACK_T callback)
{
}

PUBLIC VOID tsd_Close(VOID)
{
}

PUBLIC VOID tsd_SetPowerMode(TSD_POWER_MODE_T mode)
{
}

PUBLIC VOID tsd_SetReportingMode(TSD_REPORTING_MODE_T* mode)
{
}

PUBLIC VOID tsd_GetReportingMode(TSD_REPORTING_MODE_T* mode)
{
}

PUBLIC TSD_ERR_T tsd_SetCalibPoints(TSD_CALIBRATION_POINT_T calibPoints[3])
{
    return TSD_ERR_NO;
}

PUBLIC VOID tsd_SetCalibStatus(BOOL isCalibrated)
{
}

PUBLIC CHAR* tsd_GetHwVersion(VOID)
{
    return "Unknown";
}
