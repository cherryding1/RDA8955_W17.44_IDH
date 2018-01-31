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
#include "global_macros.h"
#include "hal_debug.h"
#include "hal_mem_map.h"
#include "halp_profile.h"

#ifdef HAL_PROFILE_ON_BUFFER
#include "hal_sys.h"

#if  defined(HAL_PROFILE_NOT_ON_ROMULATOR)
#include "hal_map.h"
#include "sxr_mem.h"
#include "string.h"
#endif // defined(HAL_PROFILE_NOT_ON_ROMULATOR)

#define CPMASK 0x3fff
#define CPEXITFLAG 0x8000
#define CPFINT 0x7fff

// Hardcoded in hal_PXTS_SendProfilingCode function.
#define CPBCPU 0x4000

#define CPPULSESSPACESTART 0x2f00
#define CPPULSESSPACEMASK 0x7ff

#define CPWINDOWSPACESTART 0x3700
#define CPWINDOWSPACEMASK 0x7ff

#define CPTASKSPACESTART 0x3f00
#define CPTASKSPACEMASK 0x1f

#define CPIRQPACESTART 0x3f20
#define CPIRQSPACEMASK 0x1f

#define CPJOBPACESTART 0x3f40
#define CPJOBSPACEMASK 0x1f

// Trigger to stop the PXTS record.
#define HAL_DBG_PXTS_TRIGGER    (0xFFFF)


#if defined(HAL_PROFILE_ON_BUFFER)
#include "string.h"
#define PXTX_BUF_SIZE (12*1024)
PUBLIC UINT32 HAL_UNCACHED_BSS g_pxts_buffer[PXTX_BUF_SIZE];
#endif // defined(HAL_PROFILE_ON_BUFFER)

// =============================================================================
// hal_DbgPxtsSetEnabled
// -----------------------------------------------------------------------------
/// Enable or disable all profiling.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsSetEnabled(BOOL enabled)
{
#ifdef HAL_PROFILE_ON_BUFFER
    HAL_PROFILE_CONTROL_T* pControl = &g_halMapAccess.profileControl;
    pControl->config = enabled? 0 : 1;
#endif
}


// =============================================================================
// hal_DbgPxtsSendTrigger
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pre-defined "trigger
/// tag". If the "PXTS Trigger" mode is enabled in the profiling tool, this
/// "trigger tag" will stop the PXTS recording. This can be used to stop the
/// PXTS recording from the embedded code.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsSendTrigger(HAL_DBG_PXTS_LEVEL_T level)
{
    hal_DbgPxtsSendCode(level, HAL_DBG_PXTS_TRIGGER);
}



// =============================================================================
// hal_DbgPxtsProfileFunctionEnter
// -----------------------------------------------------------------------------
/// This function has to be called when entering the function you want to profile.
///
/// @param functionId Code representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId)
{
    hal_DbgPxtsSendCode(level, functionId & CPMASK);
}



// =============================================================================
// hal_DbgPxtsProfileFunctionExit
// -----------------------------------------------------------------------------
/// This function has to be called when exiting the function you want to profile.
///
/// @param functionId Code representing the function.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId)
{
    hal_DbgPxtsSendCode(level, (functionId & CPMASK) | CPEXITFLAG);
}



// =============================================================================
// hal_DbgPxtsProfileWindowEnter
// -----------------------------------------------------------------------------
/// This function has to be called to mark the entrance in a window.
///
/// @param windowId Code representing the window which has been entered in.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId)
{
    hal_DbgPxtsSendCode(level, (windowId & CPMASK));
}



// =============================================================================
// hal_DbgPxtsProfileWindowExit
// -----------------------------------------------------------------------------
/// This function has to be called to mark the exit of a window.
///
/// @param windowId Code representing the window which has been exited.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId)
{
    hal_DbgPxtsSendCode(level, (windowId & CPMASK) | CPEXITFLAG);
}



// =============================================================================
// hal_DbgPxtsProfileTaskEnter
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code corresponding to scheduled task Id.
///
/// @param taskId Task id of the scheduled task.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileTask(HAL_DBG_PXTS_LEVEL_T level,
                                   UINT16 taskId)
{
    if (taskId > CPTASKSPACEMASK)
    {
        taskId = CPTASKSPACEMASK;
    }
    hal_DbgPxtsSendCode(level, CPTASKSPACESTART + taskId);
}



// =============================================================================
// hal_DbgPxtsProfilePulse
// -----------------------------------------------------------------------------
/// Send to the profiling tool the code correponding to a pulse.
///
/// @param code Code representing the pulse
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_LEVEL_T level,
                                    UINT16 code)
{
    hal_DbgPxtsSendCode(level, code & CPMASK);
}



// =============================================================================
// hal_DbgPxtsProfileJobEnter
// -----------------------------------------------------------------------------
/// This job has to be called when entering the job you want to profile.
///
/// @param jobId Representing the job.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileJobEnter(HAL_DBG_PXTS_LEVEL_T level,
                                       UINT16 jobId)
{
    if (jobId > CPJOBSPACEMASK)
    {
        jobId = CPJOBSPACEMASK;
    }
    hal_DbgPxtsSendCode(level, CPJOBPACESTART + jobId);
}



// =============================================================================
// hal_DbgPxtsProfileJobExit
// -----------------------------------------------------------------------------
/// This job has to be called when exiting the job you want to profile.
///
/// @param jobId Code representing the job.
/// @param level PXTS level on which to send the code.
// =============================================================================
PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileJobExit(HAL_DBG_PXTS_LEVEL_T level,
                                      UINT16 jobId)
{
    if (jobId > CPJOBSPACEMASK)
    {
        jobId = CPJOBSPACEMASK;
    }
    hal_DbgPxtsSendCode(level, (CPJOBPACESTART + jobId) | CPEXITFLAG);
}


#if defined(HAL_PROFILE_NOT_ON_ROMULATOR) || defined(HAL_PROFILE_ON_BUFFER)
// =============================================================================
// hal_DbgPxtsOpen
// -----------------------------------------------------------------------------
/// Initializes the global variable (Part of HAL Map structure) used to enable
/// the feature of the recording the PXTS tags in a RAM buffer, etc.
///
/// This function must be called by hal_Open before doing profiling.
// =============================================================================
PROTECTED VOID hal_DbgPxtsOpen(VOID)
{
    // Clear the structure
    memset(&g_halMapAccess.profileControl, 0, sizeof(HAL_PROFILE_CONTROL_T));

#if defined(HAL_PROFILE_ON_BUFFER)
    g_halMapAccess.profileControl.startAddr = (UINT32)g_pxts_buffer;
    g_halMapAccess.profileControl.size = PXTX_BUF_SIZE*4;
    g_halMapAccess.profileControl.writePointer = (UINT32)g_pxts_buffer;
#elif defined(HAL_PROFILE_NOT_ON_ROMULATOR)
    // Set the handler
    g_halMapAccess.profileControl.mallocRamBuffer = (VOID*)hal_DbgPxtsMallocRamBuffer;
    g_halMapAccess.profileControl.freeRamBuffer = (VOID*)hal_DbgPxtsFreeRamBuffer;
#endif // defined(HAL_PROFILE_NOT_ON_ROMULATOR)
}

#endif // defined(HAL_PROFILE_NOT_ON_ROMULATOR) || defined(HAL_PROFILE_ON_BUFFER)


#if defined(HAL_PROFILE_NOT_ON_ROMULATOR)
// =============================================================================
// hal_DbgPxtsMallocRamBuffer
// -----------------------------------------------------------------------------
/// Allocate the RAM buffer used by to record PXTS tags in RAM, and fill the
/// appropriate global variables. If the buffer is already allocated (ie the
/// \c startAddr field is not 0), the previously allocated buffer is kept and
/// no more memory is allocated.
///
/// @param size Size of the buffer to allocate.
/// @return Pointer to the allocated buffer
// =============================================================================
PROTECTED VOID* hal_DbgPxtsMallocRamBuffer(UINT32 size)
{
    if (g_halMapAccess.profileControl.startAddr != 0)
    {
        return (VOID*) g_halMapAccess.profileControl.startAddr;
    }
    else
    {
        VOID* allocatedBuffer = sxr_Malloc(size);
        if (allocatedBuffer != NULL)
        {
            g_halMapAccess.profileControl.startAddr     = (UINT32) allocatedBuffer;
            g_halMapAccess.profileControl.writePointer  = (UINT32) allocatedBuffer;
            g_halMapAccess.profileControl.readPointer   = (UINT32) allocatedBuffer;
            g_halMapAccess.profileControl.size          = size;
            g_halMapAccess.profileControl.remainingSize = size;
            memset(allocatedBuffer, 0, size);
        }

        return allocatedBuffer;
    }
}

// =============================================================================
// hal_DbgPxtsFreeRamBuffer
// -----------------------------------------------------------------------------
/// Free the RAM buffer used by to record PXTS tags in RAM, and clear the
/// appropriate global variables. If the buffer is already freed (ie the
/// \c startAddr field is not 0), nothing is done.
// =============================================================================
PROTECTED VOID hal_DbgPxtsFreeRamBuffer(VOID)
{
    // Use an intermediate variable to avoid sending PXTS tags while
    // freeing the buffer. (Once g_halMapAccess.profileControl.startAddr
    // is 0, nothing is sent anymore)
    UINT32 bufAddr = g_halMapAccess.profileControl.startAddr;

    // Stop profiling trough RAM.
    g_halMapAccess.profileControl.startAddr = 0;

    // Free and clear.
    sxr_Free(bufAddr);
    g_halMapAccess.profileControl.startAddr     = 0;
    g_halMapAccess.profileControl.writePointer  = 0;
    g_halMapAccess.profileControl.readPointer   = 0;
    g_halMapAccess.profileControl.size          = 0;
    g_halMapAccess.profileControl.remainingSize = 0;

}


// =============================================================================
// hal_DbgPxtsNotOnRomulatorEnableFromWithin
// -----------------------------------------------------------------------------
/// Enable the profiling not on the romulator, from the embedded code, so that
/// the beginning of the execution can be profiled, even before CoolWatcher
/// Profiler Plugin has time to configure profiling.
///
/// @param ramMode. If \c TRUE, the profiling is done in a Ram buffer, whose
/// size is passed as the second parameter. If \c FALSE, profiling is done through
/// a trace flow, of id SXS_PROFILE_RMC.
/// @param size Size of the buffer to allocate.
/// @param maskLevel Bitfield of the profiling levels to enable.
// =============================================================================
PUBLIC VOID hal_DbgPxtsNotOnRomulatorEnableFromWithin(BOOL ramMode, UINT32 bufferSize,
        UINT16 maskLevel)
{
    // Reset config.
    UINT32* pConfig = &g_halMapAccess.profileControl.config;
    *pConfig = 0;

    if (ramMode == TRUE)
    {
        // Allocate ram buffer.
        hal_DbgPxtsMallocRamBuffer(bufferSize);
    }
    else
    {
        // Nothing to do.
    }

    // Set Mask.
    hal_DbgPxtsSetup(maskLevel);

    // Enable profiling.
    if (ramMode == TRUE)
    {
        *pConfig |= HAL_PROFILE_CONTROL_T_GLOBAL_ENABLE_RAM;
    }
    else
    {
        *pConfig |= HAL_PROFILE_CONTROL_T_GLOBAL_ENABLE_TRACE;
    }
}


#endif // defined(HAL_PROFILE_NOT_ON_ROMULATOR)

#else

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsSetEnabled(BOOL enabled)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsSendTrigger(HAL_DBG_PXTS_LEVEL_T level)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId)
{

}


PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 functionId)
{

}


PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_LEVEL_T level,
        UINT16 windowId)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileTask(HAL_DBG_PXTS_LEVEL_T level,
                                   UINT16 taskId)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_LEVEL_T level,
                                    UINT16 code)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileJobEnter(HAL_DBG_PXTS_LEVEL_T level,
                                       UINT16 jobId)
{

}

PUBLIC VOID HAL_FUNC_ROM hal_DbgPxtsProfileJobExit(HAL_DBG_PXTS_LEVEL_T level,
                                      UINT16 jobId)
{

}

PUBLIC VOID hal_DbgPxtsOpen(VOID)
{

}
#endif
