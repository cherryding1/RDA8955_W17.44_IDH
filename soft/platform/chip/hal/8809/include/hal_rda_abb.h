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



#ifndef _HAL_RDA_ABB_H_
#define _HAL_RDA_ABB_H_

#include "cs_types.h"

// ----------------
// Default volumes
// ----------------

// Default ABB DAC digital gain is -2dB for receiver
#define DEFAULT_ABB_DAC_GAIN_RECEIVER (-2)
// Default ABB DAC digital gain is -2dB for speaker
#define DEFAULT_ABB_DAC_GAIN_SPEAKER (-2)
// Earpiece output energy is too large
// Reduce default ABB DAC digital gain to -2dB
#define DEFAULT_ABB_DAC_GAIN_HEAD (-2)
// Default ABB ADC (MIC) digital gain is 0dB
#define DEFAULT_ABB_ADC_GAIN (0)

// Max ABB DAC digital gain is -2dB for earpiece
#define MAX_ABB_DAC_GAIN_HEAD (-2)
// Max ABB ADC (MIC) digital gain is 12dB
#define MAX_ABB_ADC_GAIN (12)


// ----------------
// Types
// ----------------

// SIM selection state
typedef enum
{
    HAL_ABB_SIM_SELECT_SIM0 = (1<<0),
    HAL_ABB_SIM_SELECT_SIM1 = (1<<1),
    HAL_ABB_SIM_SELECT_SIM2 = (1<<2),
    HAL_ABB_SIM_SELECT_SIM3 = (1<<3),
} HAL_ABB_SIM_SELECT_T;


// ----------------
// APIs
// ----------------

PUBLIC VOID hal_AbbOpen(VOID);
PUBLIC VOID hal_AudSetHPMode(BOOL isHPMode);

PUBLIC BOOL hal_AbbRegRead(UINT32 addr, UINT32* pData);
PUBLIC VOID hal_AbbRegBlockingRead(UINT32 addr, UINT32* pData);
PUBLIC BOOL hal_AbbRegWrite(UINT32 addr, UINT32 data);

PUBLIC VOID hal_AbbEnableUsbPhy(BOOL enable);

PUBLIC VOID hal_AbbEnableSim(UINT32 simMask);

#endif // _HAL_RDA_ABB_H_

