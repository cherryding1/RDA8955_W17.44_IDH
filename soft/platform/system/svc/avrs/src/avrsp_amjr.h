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


#ifndef _AVRSP_AMJPG_H_
#define _AVRSP_AMJPG_H_

#include "cs_types.h"
#include "vpp_amjr.h"

/// Audio configuration for the AMJR decoder.
/// Initial setup is done by #avrs_Record.
EXPORT PROTECTED VPP_AMJR_AUDIO_CFG_T g_avrsVppAmjrAudioCfg ;

// =============================================================================
// avrs_VppAmjrHandler
// -----------------------------------------------------------------------------
/// Handler of the 'real' interruption from the VPP AMJR module. It dispatches
/// the causes to call accordingly one or both the handler for the audio or the
/// video handler.
/// It also handle the initialization interrupt cause.
///
/// @param status VPP AMJR interrupt status.
// =============================================================================
PROTECTED VOID avrs_VppAmjrHandler(HAL_VOC_IRQ_STATUS_T* status);


// =============================================================================
// avrs_VppAmjrAudioHandler
// -----------------------------------------------------------------------------
/// Handler for the audio cause of the VPP AMJR interrupt.
/// @param status VPP AMJR interrupt status.
// =============================================================================
PROTECTED VOID avrs_VppAmjrAudioHandler(HAL_VOC_IRQ_STATUS_T* status);


// =============================================================================
// avrs_VppAmjrVideoHandler
// -----------------------------------------------------------------------------
/// Handler for the video cause of the VPP AMJR interrupt.
/// @param status VPP AMJR interrupt status.
// =============================================================================
PROTECTED VOID avrs_VppAmjrVideoHandler(HAL_VOC_IRQ_STATUS_T* status);


#endif // _AVRSP_AMJPG_H_










