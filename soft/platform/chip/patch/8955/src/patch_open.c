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
#ifdef ENABLE_PATCH_SPC_CCHRXPROCESS
// For Rx/Tx debugging
extern CONST VOID spc_CCHRxProcess(VOID);
extern CONST VOID patch_spc_CCHRxProcess(VOID);
#endif
#ifdef ENABLE_PATCH_SPC_TCHRXPROCESS
extern CONST VOID spc_TCHRxProcess(VOID);
extern CONST VOID patch_spc_TCHRxProcess(VOID);
#endif
// ----------------------------------------
#ifdef ENABLE_PATCH_SPP_EQUALIZENBURST
// For real fixes
extern CONST VOID spp_EqualizeNBurstSaic(VOID);
extern CONST VOID patch_spp_EqualizeNBurstSaic(VOID);
extern CONST VOID spp_EqualizeNBurst(VOID);
extern CONST VOID patch_spp_EqualizeNBurst(VOID);
#endif
#ifdef ENABLE_PATCH_SPC_AMRDECODE
extern CONST VOID spc_AmrDecode(VOID);
extern CONST VOID patch_spc_AmrDecode(VOID);
#endif
// Improve the performance under fading situation
#ifdef ENABLE_PATCH_BB_PROG_RX_WIN
extern CONST VOID bb_prog_rx_win(VOID);
extern CONST VOID patch_bb_prog_rx_win(VOID);
#endif
// Alleviate voice interrupted issue in bad signal environment
#ifdef ENABLE_PATCH_SPC_SCHRXPROCESS
extern CONST VOID spc_SchProcess(VOID);
extern CONST VOID patch_spc_SchProcess(VOID);
#endif
#ifdef ENABLE_PATCH_SPP_FCCHDETECTASM
extern CONST VOID spp_FcchDetectAsm(VOID);
extern CONST VOID patch_spp_FcchDetectAsm(VOID);
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
#ifdef ENABLE_PATCH_SPC_CCHRXPROCESS
    patch_ReplaceFunction(spc_CCHRxProcess, patch_spc_CCHRxProcess);
#endif

    // ----------------------------------------

    // Fix preample bit error in Tx

    // Fix AMR issue
#ifdef ENABLE_PATCH_SPC_TCHRXPROCESS
    patch_ReplaceFunction(spc_TCHRxProcess, patch_spc_TCHRxProcess);
#endif

#ifdef ENABLE_PATCH_SPP_EQUALIZENBURST
    patch_ReplaceFunction(spp_EqualizeNBurstSaic, patch_spp_EqualizeNBurstSaic);
    patch_ReplaceFunction(spp_EqualizeNBurst,patch_spp_EqualizeNBurst);
#endif

#ifdef ENABLE_PATCH_SPC_AMRDECODE
    patch_ReplaceFunction(spc_AmrDecode, patch_spc_AmrDecode);
#endif
#ifdef ENABLE_PATCH_BB_PROG_RX_WIN
    patch_ReplaceFunction(bb_prog_rx_win, patch_bb_prog_rx_win);
#endif
#ifdef ENABLE_PATCH_SPC_SCHRXPROCESS
    patch_ReplaceFunction(spc_SchProcess, patch_spc_SchProcess);
#endif
#ifdef ENABLE_PATCH_SPP_FCCHDETECTASM
    patch_ReplaceFunction(spp_FcchDetectAsm, patch_spp_FcchDetectAsm);
#endif
#ifdef FREQ_LARGESCALE_SUPPORT
patch_ReplaceFunction(spp_SearchFcch, patch_spp_SearchFcch);
#endif
}

