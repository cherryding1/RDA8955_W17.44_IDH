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



#ifndef _TGT_PMD_CFG_H
#define _TGT_PMD_CFG_H

#include "cs_types.h"


#define SECOND        * HAL_TICK1S

#define MILLI_SECOND  SECOND / 1000
#define MILLI_SECONDS MILLI_SECOND
#define SECONDS       SECOND
#define MINUTE        * ( 60 SECOND )
#define MINUTES       MINUTE
#define HOUR          * ( 60 MINUTE )
#define HOURS         HOUR


// =============================================================================
// PMD_CONFIG_T
// -----------------------------------------------------------------------------
/// This is the anonymous declaration of the structure used to describes the
/// configuration of the PMD driver. The actual content is specifc to the pmd
/// driver of the actual PM Chip
// =============================================================================
typedef struct PMD_CONFIG_STRUCT_T TGT_PMD_CONFIG_T;


// =============================================================================
// TGT_PMD_VOLT_CAP_T
// -----------------------------------------------------------------------------
/// The structure used to describes a point in the voltage vs capacity
/// curve of the battery.
// =============================================================================
typedef struct
{
    UINT16 volt;
    UINT16 cap;
} TGT_PMD_VOLT_CAP_T;


// =============================================================================
// TGT_PMD_TEMPERATURE_RESISTANCE_T
// -----------------------------------------------------------------------------
/// The structure used to describes a point in the temperature vs resistance
/// curve of the thermistor sensor.
// =============================================================================
typedef struct
{
    UINT16 degree;
    UINT32 ohm;
} TGT_PMD_TEMPERATURE_RESISTANCE_T;


// =============================================================================
// tgt_GetPmdConfig
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to its configuration structure.
/// This is the only way PMD can get this information.
// =============================================================================
PUBLIC CONST TGT_PMD_CONFIG_T* tgt_GetPmdConfig(VOID);


// =============================================================================
// tgt_GetPmdBattCapCurve
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to the battery capacity curve.
// =============================================================================
PUBLIC VOID tgt_GetPmdBattCapCurve(CONST TGT_PMD_VOLT_CAP_T **pArray, UINT8 *pLen);


// =============================================================================
// tgt_GetPmdTemperatureCurve
// -----------------------------------------------------------------------------
/// This function is used by PMD to get access to the temperature curve.
// =============================================================================
PUBLIC VOID tgt_GetPmdTemperatureCurve(CONST TGT_PMD_TEMPERATURE_RESISTANCE_T **pArray, UINT8 *pLen);


#endif // _TGT_PMD_CFG_H

