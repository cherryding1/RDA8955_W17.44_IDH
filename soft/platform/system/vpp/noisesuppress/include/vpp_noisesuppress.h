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
















#ifndef VPP_NOISESUPPRESS_H
#define VPP_NOISESUPPRESS_H

#include "cs_types.h"
#include "hal_voc.h"


///@defgroup vpp_hello
///
///  This document describes the characteristics of the VPP NOISESUPPRESS module.
///
///  @par configuration :
///  @par
///         The VPP NOISESUPPRESS configuration ...
///
///  @par Resource management :
///  @par
///         The VPP NOISESUPPRESS module needs to be first opened and configured using function
///         <b> \c vpp_NoiseSuppressOpen </b>. When the module is no longer needed it should be closed using
///         <b> \c vpp_NoiseSuppressClose </b>. Not closing the module would not free the VoC resource
///         for other applications.
///
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_NOISESUPPRESS_CFG_T
// ----------------------------------------------------------------------------
/// VPP NOISESUPPRESS configuration structure
// ============================================================================

typedef struct
{
// INT16  mode;
//  INT16  reset;

    INT32   *InPtr;
    INT32   *OutPtr;
    INT32     *OutPara;
    INT16 mic_reset;
    INT16 receiver_reset;

} VPP_NOISESUPPRESS_CFG_T;


// ============================================================================
// VPP_NOISESUPPRESS_STATUS_T
// ----------------------------------------------------------------------------
/// VPP NOISESUPPRESS status structure
// ============================================================================

//struct of output parameters
typedef struct
{
    // ----------------
    // common status
    // ----------------
    INT16 mode;
    INT16 ErrorStatus;

    INT32 OutSize;



} VPP_NOISESUPPRESS_STATUS_T;


// ============================================================================
// VPP_NOISESUPPRESS_WAKEUP_MODE_T
// ----------------------------------------------------------------------------
/// Defines the order in which the tasks are executed an the way these
/// tasks are started
// ============================================================================

typedef enum
{
    /// No wakeup enabled
    VPP_NOISESUPPRESS_WAKEUP_NO = 0,
    /// Wake on software event SOF0.
    VPP_NOISESUPPRESS_WAKEUP_SW0,
    /// Wake on hardware event IFC0.
    VPP_NOISESUPPRESS_WAKEUP_HW0

} VPP_NOISESUPPRESS_WAKEUP_MODE_T;


// ============================================================================
// VPP_NOISESUPPRESS_ERROR_T
// ----------------------------------------------------------------------------
/// VPP NOISESUPPRESS error enumerator
// ============================================================================

typedef enum
{
    /// There is no error
    VPP_NOISESUPPRESS_ERROR_NO                  =  0,
    /// There is error
    VPP_NOISESUPPRESS_ERROR_YES                 = -1

} VPP_NOISESUPPRESS_ERROR_T;



// ============================================================================
// API Functions
// ============================================================================

//=============================================================================
// vpp_NoiseSuppressOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP NOISESUPPRESS code and environment variables. It also set the
/// VoC wakeup masks necessary for the synchronization.
/// @param vocIrqHandler : user defined handler function for the VoC interrupt.
/// If the function is set to NULL, no interrupts are used (the interrupt mask
/// is not set).
/// @param wakeupMode : Select whether the VoC module wakeup is controlled by
/// XCPU or by IFC interrupts.
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_NoiseSuppressOpen(HAL_VOC_IRQ_HANDLER_T  vocIrqHandler);

//=============================================================================
// vpp_NoiseSuppressClose function
//-----------------------------------------------------------------------------
/// Close VPP NOISESUPPRESS, clear VoC wakeup masks.
//=============================================================================
PUBLIC VOID vpp_NoiseSuppressClose(VOID);

//=============================================================================
// vpp_NoiseSuppressStatus function
//-----------------------------------------------------------------------------
/// Return the VPP NOISESUPPRESS status structure.
/// @param pStatus : result status structure of type VPP_NOISESUPPRESS_STATUS_T
//=============================================================================
PUBLIC VOID vpp_NoiseSuppressStatus(VPP_NOISESUPPRESS_STATUS_T * pStatus);

//=============================================================================
// vpp_NoiseSuppressCfg function
//-----------------------------------------------------------------------------
/// @param cfg : configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_NoiseSuppressCfg(VPP_NOISESUPPRESS_CFG_T* pCfg);

//=============================================================================
// vpp_NoiseSuppressSchedule function
//-----------------------------------------------------------------------------
/// This function will write a software wakeup event if the wakeup mode is
/// VPP_NOISESUPPRESS_WAKEUP_SW0
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_NoiseSuppressSchedule(VOID);



///  @} <- End of the vpp_hello group


#endif // VPP_NOISESUPPRESS_H
