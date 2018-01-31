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



























#ifndef DIRECTB_H__
#define DIRECTB_H__

#include "beginhdr.h"
#include "rvtypes.h"

#define TR_SHIFT    14
#define TR_RND      (1 << (TR_SHIFT - 1))

typedef void (RV_CDECL T_DirectB_InterpolMB)(
    U8 *const       pDirB,      /* pointer to current direct mode MB buffer */
    const U8 *const pPrev,      /* pointer to previous ref plane buffer */
    const U8 *const pFutr,      /* pointer to future ref plane buffer */
    const U32       uPredPitch, /* for pPrev and pFutr */
    const U32       uPitch,     /* buffer pitch for pDirB */
    const Bool32    bSkipInterp,
    const I32       uFwdRatio,
    const I32       uBwdRatio
) GNUCDECL;

#ifdef __cplusplus
extern "C" {
#endif
T_DirectB_InterpolMB C_DirectB_InterpolMB_Lu;
T_DirectB_InterpolMB C_DirectB_InterpolMB_Cr;
#ifdef APK
T_DirectB_InterpolMB MMX_DirectB_InterpolMB_Lu;
T_DirectB_InterpolMB MMX_DirectB_InterpolMB_Cr;
T_DirectB_InterpolMB KNI_DirectB_InterpolMB_Cr;
T_DirectB_InterpolMB KNI_DirectB_InterpolMB_Lu;
T_DirectB_InterpolMB AltiVec_DirectB_InterpolMB_Cr;
T_DirectB_InterpolMB AltiVec_DirectB_InterpolMB_Lu;
#endif
#if defined(ARM) || defined(_ARM_)
T_DirectB_InterpolMB SA_DirectB_InterpolMB_Lu;
T_DirectB_InterpolMB SA_DirectB_InterpolMB_Cr;
#ifdef ARM_COTULLA
T_DirectB_InterpolMB XC_DirectB_InterpolMB_Lu;
T_DirectB_InterpolMB XC_DirectB_InterpolMB_Cr;
#endif
#endif
#ifdef __cplusplus
}
#endif

#endif /* DIRECTB_H__ */
