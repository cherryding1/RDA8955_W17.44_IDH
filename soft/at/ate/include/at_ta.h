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


#ifndef __AT_TA_H__
#define __AT_TA_H__

#include "at.h"

typedef enum
{
    mode_te2mt, // from TE device
    mode_mt2te, // from MT device
    mode_COUNT_,
    mode_INVALID_ = 0x8F,
} ta_convert_mode_t;

typedef enum
{
    cs_cvt_okay,
    cs_cvt_failed,
    cs_cvt_needless,
    cs_cvt_COUNT_,
    cs_cvt_INVALID_ = 0x8F,
} ta_chset_convert_result_t;

extern ta_chset_convert_result_t ate_TaConvert(ta_convert_mode_t ds, UINT8 *src, UINT16 slen, UINT8 *dst,
        UINT16 *pdlen);

#endif
