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



#ifndef _MMC_ANALOGTV_H_
#define _MMC_ANALOGTV_H_

#include "cs_types.h"

#include "mci.h"
#include "atvd_m.h"

/// @file mmc_analogtv.h
/// @mainpage analog TV API
/// @page apbs_mainpage  analog TV API

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================
// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
///
///
// =============================================================================

typedef enum
{
    //IFC interrupt
    MMC_ANALOG_TV_REALTIME_PROCESS=0x10,

} MMC_ANALOG_TV_MSGID;


// =============================================================================
// MMC_ANALOG_MSGID
// -----------------------------------------------------------------------------
///
///
// =============================================================================


// =============================================================================
//  FUNCTIONS
// =============================================================================
PUBLIC UINT32 mmc_AnalogTvAudioSetup(UINT8 volume);
PUBLIC UINT32 mmc_AnalogTvAudioOpen(VOID);
PUBLIC UINT32 mmc_AnalogTvAudioClose(VOID);

#endif
