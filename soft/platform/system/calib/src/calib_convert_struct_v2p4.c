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

#include "snd_types.h"
#include "calib_m.h"

#include "string.h"


/// Number of power management driver parameters.
#define CALIB_PMD_QTY_V2P4                       (2)


// ============================================================================
// CALIB_GPADC_T
// -----------------------------------------------------------------------------
/// Calib GPADC analog type.
// =============================================================================
typedef struct
{
    UINT8                          bandgap;                      //0x00000000
    UINT16                         sensorGainA;                  //0x00000002
    UINT16                         sensorGainB;                  //0x00000004
} CALIB_GPADC_V2P4_T; //Size : 0x6



// ============================================================================
// CALIB_RF_ANALOG_T
// -----------------------------------------------------------------------------
/// Calib RF analog type.
// =============================================================================
typedef struct
{
    UINT8                          txRefVolt;                    //0x00000000
    UINT8                          txDacCurQ;                    //0x00000001
    UINT8                          txDacCurI;                    //0x00000002
    UINT8                          rxRefCal;                     //0x00000003
} CALIB_RF_ANALOG_V2P4_T; //Size : 0x4



// ============================================================================
// CALIB_PMD_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef UINT32 CALIB_PMD_V2P4_T[CALIB_PMD_QTY_V2P4];



// ============================================================================
// CALIB_BB_T
// -----------------------------------------------------------------------------
/// Baseband calibration.
// =============================================================================
typedef struct
{
    CALIB_PAL_TIMES_T              times;                        //0x00000000
    /// Audio calibration, for each interface
    CALIB_AUDIO_ITF_T              audio[CALIB_AUDIO_ITF_QTY];   //0x00000028
    /// Analog macros calibration: GPADC.
    CALIB_GPADC_V2P4_T             gpadc;                        //0x00000988
    /// Analog macros calibration: RF modules.
    CALIB_RF_ANALOG_V2P4_T         rfAnalog;                     //0x0000098E
    CALIB_PMD_V2P4_T               pmd;                          //0x00000994
} CALIB_BB_V2P4_T; //Size : 0x99C



PUBLIC VOID calib_ConvertStructV2p4(CALIB_BB_T *pCalibBb)
{
    CALIB_BB_V2P4_T *pCalibBbV2p4 = (CALIB_BB_V2P4_T *)pCalibBb;
    CALIB_GPADC_T gpadc;

    gpadc.sensorGainA = pCalibBbV2p4->gpadc.sensorGainA;
    gpadc.sensorGainB = pCalibBbV2p4->gpadc.sensorGainB;

    memcpy(&pCalibBb->gpadc, &gpadc, sizeof(pCalibBb->gpadc));
}


