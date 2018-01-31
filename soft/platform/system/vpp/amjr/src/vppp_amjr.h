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


















#ifndef VPPP_AMJR_H
#define VPPP_AMJR_H

#include "cs_types.h"

///@defgroup vppp_amjr_struct
///@{

// ============================================================================
// Types
// ============================================================================


// ============================================================================
// VPP_AMJR_CFG_T
// ----------------------------------------------------------------------------
/// VPP AMJR configuration structure
// ============================================================================

typedef struct
{
    // ----------------
    // common config
    // ----------------
    INT8 audioMode;
    INT8 videoMode;

    // ----------------
    // audio config
    // ----------------
    INT16  reset;
    INT32* inStreamBufAddr;
    INT32* outStreamBufAddr;
    INT32  sampleRate;
    INT16  bitRate;
    INT16  channelNum;
    INT32* inStreamBufIfcPtr;

    // ----------------
    // video config
    // ----------------
    INT32* imagInBufAddr;
    INT32* imagOutBufAddr;
    INT16  imagWidth;
    INT16  imagHeight;
    INT16  imagQuality;
    INT16  imagFormat;
    INT16  imagPxlSwap;
    INT16  imagReserved;
    INT32* previewOutBufAddr;
    INT16  previewWidth;
    INT16  previewHeight;
    INT16  previewFormat;
    INT16  previewScaleFactor;

} VPP_AMJR_CFG_T;



// ============================================================================
// VPP_AMJR_CODE_CFG_T
// ----------------------------------------------------------------------------
/// Internal VPP Audio Jpeg configuration structure
// ============================================================================

typedef struct
{
    /// pointers to the amr code and constant data
    INT32* amrPtrs[3];
    /// pointers to the mp3 code and constant data
    INT32* mp3Ptrs[3];
    /// pointers to the jpeg code and constant data
    INT32* jpegPtrs[2];

} VPP_AMJR_CODE_CFG_T;


///  @} <- End of the vppp_amjr_struct group


#endif  // VPPP_AMJR_H
