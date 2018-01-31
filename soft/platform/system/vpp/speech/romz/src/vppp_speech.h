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

















#ifndef VPPP_SPEECH_H
#define VPPP_SPEECH_H

#include "cs_types.h"

///@defgroup vppp_struct
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_SPEECH_CODE_IDX_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech code pointer indexes
// ============================================================================
typedef enum
{
    VPP_SPEECH_EFR_AMR_IDX = 0,
    VPP_SPEECH_FR_IDX,
    VPP_SPEECH_HR_IDX,
    VPP_SPEECH_AMR_IDX,
    VPP_SPEECH_EFR_IDX,
    VPP_SPEECH_AMR_ENC_475_IDX,
    VPP_SPEECH_AMR_ENC_515_IDX,
    VPP_SPEECH_AMR_ENC_59_IDX,
    VPP_SPEECH_AMR_ENC_67_IDX,
    VPP_SPEECH_AMR_ENC_74_IDX,
    VPP_SPEECH_AMR_ENC_795_IDX,
    VPP_SPEECH_AMR_ENC_102_IDX,
    VPP_SPEECH_AMR_ENC_122_IDX,
    VPP_SPEECH_AMR_DEC_475_IDX,
    VPP_SPEECH_AMR_DEC_515_IDX,
    VPP_SPEECH_AMR_DEC_59_IDX,
    VPP_SPEECH_AMR_DEC_67_IDX,
    VPP_SPEECH_AMR_DEC_74_IDX,
    VPP_SPEECH_AMR_DEC_795_IDX,
    VPP_SPEECH_AMR_DEC_102_IDX,
    VPP_SPEECH_AMR_DEC_122_IDX,
    VPP_SPEECH_CODE_QTY

} VPP_SPEECH_CODE_IDX_T;


// ============================================================================
// VPP_SPEECH_CONST_IDX_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech const pointer indexes
// ============================================================================
typedef enum
{
    VPP_SPEECH_EFR_AMR_CONST_X_IDX = 0,
    VPP_SPEECH_EFR_AMR_CONST_Y_IDX,
    VPP_SPEECH_HR_CONST_X_IDX,
    VPP_SPEECH_HR_CONST_Y_IDX,
    VPP_SPEECH_CONST_QTY

} VPP_SPEECH_CONST_IDX_T;


// ============================================================================
// VPP_SPEECH_IRQGEN_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech indexes for interrupt generation
// ============================================================================
typedef enum
{
    VPP_SPEECH_IRQGEN_NONE    = 0,
    VPP_SPEECH_IRQGEN_ENC     = 1,
    VPP_SPEECH_IRQGEN_DEC     = 2,
    VPP_SPEECH_IRQGEN_DECENC  = 3

} VPP_SPEECH_IRQGEN_T;


// ============================================================================
// VPP_SPEECH_INIT_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech initialisation structure
// ============================================================================
typedef struct
{
    INT16 codeStateTab[4];
    INT32 *codePtrTab[VPP_SPEECH_CODE_QTY];
    INT16 reset;
    INT16 irqgen;

} VPP_SPEECH_INIT_T;


///  @} <- End of the vppp_struct group


#endif  // VPPP_SPEECH_H
