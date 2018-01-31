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





















#ifndef PAL_SPY_H
#define PAL_SPY_H

#include "cs_types.h"

enum palSpyId
{
    PALSPY_CPU_FREE,
    PALSPY_CPU_IT,
    PALSPY_WRAP_ADJUST,
    PALSPY_MAX_TCU_VALUE,
    PALSPY_TOTAL_TIME_OFFSET,
    PALSPY_TOTAL_FREQUENCY_OFFSET,
    PALSPY_MON_WIN_QTY,
    PALSPY_MON_VALUE,
    PALSPY_FCCH_WIN_QTY,
    PALSPY_FCCH_FOF_VALUE,
    PALSPY_FCCH_TOF_VALUE,
    PALSPY_FCCH_OK_QTY,
    PALSPY_FCCH_KO_QTY,
    PALSPY_SCH_WIN_QTY,
    PALSPY_SCH_FOF_VALUE,
    PALSPY_SCH_TOF_VALUE,
    PALSPY_SCH_SNR_VALUE,
    PALSPY_SCH_OK_QTY,
    PALSPY_SCH_KO_QTY,
    PALSPY_INTER_WIN_QTY,
    PALSPY_NBURST_WIN_QTY,
    PALSPY_NBURST_FOF_VALUE,
    PALSPY_NBURST_TOF_VALUE,
    PALSPY_NBURST_SNR_VALUE,
    PALSPY_NBURST_PWR_VALUE,
    PALSPY_NBURST_BER_VALUE,
    PALSPY_NBURST_OK_QTY,
    PALSPY_NBURST_KO_QTY,
    PALSPY_TX_ABURST_QTY,
    PALSPY_TX_NBLOCK_QTY,
    PALSPY_TX_WIN_QTY,

    PALSPY_QTY
};

#ifdef PAL_SPY
#define PALSPY_AFFECT(id,val)               (SpyPalValues[(id)]=(val))
#define PALSPY_ADD(id,val)              (SpyPalValues[(id)]+=(val))
#else
#define PALSPY_AFFECT(id,val)
#define PALSPY_ADD(id,val)
#endif

extern INT16 SpyPalValues[];
extern CHAR *SpyPalText;
extern const CHAR *SpyPalItem[];

#endif /* PAL_SPY_H */
