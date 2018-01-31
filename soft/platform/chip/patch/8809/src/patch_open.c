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

#include "patch_m.h"


// =============================================================================
//  FUNCTIONS
// =============================================================================

// declare patched function as external and (CONST VOID (*)(VOID)) to avoid a cast

// For Rx/Tx debugging
extern CONST VOID spc_RxProcess(VOID);
extern CONST VOID patch_spc_RxProcess(VOID);
extern CONST VOID spp_BuildNBurst(VOID);
extern CONST VOID patch_spp_BuildNBurst(VOID);
extern CONST VOID spc_SchedulerMain(VOID);
extern CONST VOID patch_spc_SchedulerMain(VOID);
// ----------------------------------------

// For real fixes

extern CONST VOID spp_Modulation(VOID);
extern CONST VOID patch_spp_Modulation(VOID);
extern CONST VOID spc_AmrDecode(VOID);
extern CONST VOID patch_spc_AmrDecode(VOID);
extern CONST VOID spc_AmrEncode(VOID);
extern CONST VOID patch_spc_AmrEncode(VOID);
// Improve the performance under fading situation
extern CONST VOID patch_spp_EqualizeNBurst(VOID);
extern CONST VOID spp_EqualizeNBurst(VOID);
// Alleviate voice interrupted issue in bad signal environment
extern CONST VOID spc_UpdateCMR(VOID);
extern CONST VOID patch_spc_UpdateCMR(VOID);
extern CONST VOID spc_AmrRatscch(VOID);
extern CONST VOID patch_spc_AmrRatscch(VOID);
extern CONST VOID patch_spc_BfiFilter(VOID);
extern CONST VOID spc_BfiFilter(VOID);
extern CONST VOID patch_spc_CheckLoopMode(VOID);
extern CONST VOID spc_CheckLoopMode(VOID);
#ifdef PATCH_SPC_CCHRXPROCESS_ENABLE
extern CONST VOID spc_CCHRxProcess(VOID);
extern CONST VOID patch_spc_CCHRxProcess(VOID);
#endif
#ifdef ENABLE_PATCH_SPP_GETUSF
extern CONST VOID spp_GetUSF(VOID);
extern CONST VOID patch_spp_GetUSF(VOID);
#endif
#ifdef FREQ_LARGESCALE_SUPPORT
extern CONST  VOID spp_SearchFcch(VOID);
extern CONST  VOID patch_spp_SearchFcch(VOID);
#endif
// =============================================================================
// patch_Open
// -----------------------------------------------------------------------------
/// This function install all the hardware patch available for the current chip
// =============================================================================
PUBLIC VOID patch_Open(VOID)
{
    // NOTE:
    // The macros are defined in make.srcs

    // For Rx/Tx debugging
#ifdef ENABLE_PATCH_SPC_RXPROCESS
    patch_ReplaceFunction(spc_RxProcess, patch_spc_RxProcess);
#endif
#ifdef ENABLE_PATCH_SPP_BUILDNBURST
    patch_ReplaceFunction(spp_BuildNBurst, patch_spp_BuildNBurst);
#endif

    // ----------------------------------------

    // Fix preample bit error in Tx

#ifdef ENABLE_PATCH_SPP_MODULATION
    patch_ReplaceFunction(spp_Modulation, patch_spp_Modulation);
#endif

    // Fix AMR issue
#ifdef ENABLE_PATCH_SPC_AMRDEDODE
    patch_ReplaceFunction(spc_AmrDecode, patch_spc_AmrDecode);
    patch_ReplaceFunction(spc_UpdateCMR, patch_spc_UpdateCMR);
    patch_ReplaceFunction(spc_AmrRatscch, patch_spc_AmrRatscch);
#endif // ENABLE_PATCH_SPC_AMRDEDODE
#ifdef ENABLE_PATCH_SPC_AMRENCODE
    patch_ReplaceFunction(spc_AmrEncode, patch_spc_AmrEncode);
#endif

#ifdef ENABLE_PATCH_SPP_EQUALIZENBURST
    patch_ReplaceFunction(spp_EqualizeNBurst, patch_spp_EqualizeNBurst);
#endif

#ifdef ENABLE_PATCH_SPC_BFIFILTER
    patch_ReplaceFunction(spc_BfiFilter, patch_spc_BfiFilter);
#endif

#ifdef ENABLE_PATCH_CHECK_LOOP_MODE
    patch_ReplaceFunction(spc_CheckLoopMode, patch_spc_CheckLoopMode);
#endif

#ifdef ENABLE_PATCH_SPC_SCHEDULERMAIN
    patch_ReplaceFunction(spc_SchedulerMain, patch_spc_SchedulerMain);
#endif

#ifdef PATCH_SPC_CCHRXPROCESS_ENABLE
    patch_ReplaceFunction(spc_CCHRxProcess, patch_spc_CCHRxProcess);
#endif

#ifdef GCF_TEST
#ifdef ENABLE_PATCH_SPP_GETUSF
    patch_ReplaceFunction(spp_GetUSF, patch_spp_GetUSF);
#endif
#endif
#ifdef FREQ_LARGESCALE_SUPPORT
patch_ReplaceFunction(spp_SearchFcch, patch_spp_SearchFcch);
#endif
}

