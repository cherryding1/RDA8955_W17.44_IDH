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
















#ifndef VPP_HELLO_H
#define VPP_HELLO_H

#include "cs_types.h"
#include "hal_voc.h"


///@defgroup vpp_hello
///
///  This document describes the characteristics of the VPP HELLO module.
///
///  @par configuration :
///  @par
///         The VPP HELLO configuration ...
///
///  @par Resource management :
///  @par
///         The VPP HELLO module needs to be first opened and configured using function
///         <b> \c vpp_HelloOpen </b>. When the module is no longer needed it should be closed using
///         <b> \c vpp_HelloClose </b>. Not closing the module would not free the VoC resource
///         for other applications.
///
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_HELLO_CFG_T
// ----------------------------------------------------------------------------
/// VPP HELLO configuration structure
// ============================================================================

typedef struct
{
    INT16  mode;
    INT16  reset;
    INT16  pitch_shift;
} VPP_MORPHVOICE_CFG_T;


// ============================================================================
// VPP_HELLO_STATUS_T
// ----------------------------------------------------------------------------
/// VPP HELLO status structure
// ============================================================================

//struct of output parameters
typedef struct
{
    // ----------------
    // common status
    // ----------------
    INT16 mode;
    INT16 errorStatus;

    // ----------------
    // hello status
    // ----------------
    INT32 helloStatus;

} VPP_HELLO_STATUS_T;


// ============================================================================
// VPP_HELLO_MODE_T
// ----------------------------------------------------------------------------
/// VPP HELLO code mode enumerator
// ============================================================================

typedef enum
{
    VPP_HELLO_MODE_NO = -1,
    VPP_HELLO_MODE_HI =  0,
    VPP_HELLO_MODE_BYE

} VPP_HELLO_MODE_T;

// ============================================================================
// VPP_HELLO_WAKEUP_MODE_T
// ----------------------------------------------------------------------------
/// Defines the order in which the tasks are executed an the way these
/// tasks are started
// ============================================================================

typedef enum
{
    /// No wakeup enabled
    VPP_HELLO_WAKEUP_NO = 0,
    /// Wake on software event SOF0.
    VPP_HELLO_WAKEUP_SW0,
    /// Wake on hardware event IFC0.
    VPP_HELLO_WAKEUP_HW0

} VPP_HELLO_WAKEUP_MODE_T;


// ============================================================================
// VPP_HELLO_ERROR_T
// ----------------------------------------------------------------------------
/// VPP HELLO error enumerator
// ============================================================================

typedef enum
{
    /// There is no error
    VPP_HELLO_ERROR_NO                  =  0,
    /// There is error
    VPP_HELLO_ERROR_YES                 = -1

} VPP_HELLO_ERROR_T;


// ============================================================================
// VPP_HELLO_SAYS_T
// ----------------------------------------------------------------------------
/// VPP HELLO enumerator for what the VPP code said
// ============================================================================

typedef enum
{
    /// There is no error
    VPP_HELLO_SAYS_HI                   = 0x0111,
    /// There is error
    VPP_HELLO_SAYS_BYE                  = 0x0B1E

} VPP_HELLO_SAYS_T;



// ============================================================================
// API Functions
// ============================================================================

//=============================================================================
// vpp_HelloOpen function
//-----------------------------------------------------------------------------
/// Initialize all VPP HELLO code and environment variables. It also set the
/// VoC wakeup masks necessary for the synchronization.
/// @param vocIrqHandler : user defined handler function for the VoC interrupt.
/// If the function is set to NULL, no interrupts are used (the interrupt mask
/// is not set).
/// @param wakeupMode : Select whether the VoC module wakeup is controlled by
/// XCPU or by IFC interrupts.
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_HelloOpen(HAL_VOC_IRQ_HANDLER_T vocIrqHandler,
                               VPP_HELLO_WAKEUP_MODE_T wakeupMode);

//=============================================================================
// vpp_HelloClose function
//-----------------------------------------------------------------------------
/// Close VPP HELLO, clear VoC wakeup masks.
//=============================================================================
PUBLIC VOID vpp_HelloClose(VOID);

//=============================================================================
// vpp_HelloStatus function
//-----------------------------------------------------------------------------
/// Return the VPP HELLO status structure.
/// @param pStatus : result status structure of type VPP_HELLO_STATUS_T
//=============================================================================
PUBLIC VOID vpp_HelloStatus(VPP_HELLO_STATUS_T * pStatus);

//=============================================================================
// vpp_HelloCfg function
//-----------------------------------------------------------------------------
/// @param cfg : configuration (codec mode, reset...)
//=============================================================================
PUBLIC VOID vpp_HelloCfg(VPP_MORPHVOICE_CFG_T* pCfg);

//=============================================================================
// vpp_HelloSchedule function
//-----------------------------------------------------------------------------
/// This function will write a software wakeup event if the wakeup mode is
/// VPP_HELLO_WAKEUP_SW0
/// @return error of type HAL_ERR_T
//=============================================================================
PUBLIC HAL_ERR_T vpp_HelloSchedule(VOID);



///  @} <- End of the vpp_hello group


#endif // VPP_HELLO_H
