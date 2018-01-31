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




#ifndef  _HALP_VOC_IRQ_H_
#define  _HALP_VOC_IRQ_H_


#include "cs_types.h"

// ============================================================================
// TYPES
// ============================================================================



// ============================================================================
// HAL_VOC_STATE_T
// ----------------------------------------------------------------------------
/// Describes the VoC state.
// ============================================================================
typedef enum
{
    HAL_VOC_CLOSE_DONE,
    HAL_VOC_CLOSE_STARTED,
    HAL_VOC_OPEN_STARTED,
    HAL_VOC_OPEN_IN_PROGRESS,
    HAL_VOC_OPEN_FAILED,
    HAL_VOC_OPEN_DONE,
} HAL_VOC_STATE_T;


// ============================================================================
// FUNCTIONS
// ============================================================================

// ============================================================================
// hal_VocIrqHandler
// ----------------------------------------------------------------------------
/// VoC IRQ handler, clearing the IRQ cause regiter and calling the user handler
/// defined by hal_VocIrqSetHandler. It also checks if the open procedure is finished
/// when the user chooses to confirm the end of the open procedure with an VoC DMA
/// interrupt
/// @param interruptId Id with which the handler is entered
// ============================================================================
PROTECTED VOID hal_VocIrqHandler(UINT8 interruptId);


#endif //  HAL_VOC_IRQ_H

