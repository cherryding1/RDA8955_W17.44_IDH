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


#ifndef _SXP_DEBUG_H_
#define _SXP_DEBUG_H_

#include "cs_types.h"
#include "hal_debug.h"

#include "sx_profile_codes.h"




#ifdef SX_PRINTF
#define SX_TRACE(format, ...)                                    \
    hal_DbgTrace(TSTDOUT, 0, format, ##__VA_ARGS__)
#else
#define SX_TRACE(fmt, ...)
#endif


#ifdef SX_NO_ASSERT
#define SX_ASSERT(BOOL, format, ...)
#else
#define SX_ASSERT(BOOL, format, ...)                   \
    if (!(BOOL)) {            \
        hal_DbgAssert(format, ##__VA_ARGS__);                             \
    }
#endif






#ifdef SX_PROFILING


// =============================================================================
//  SX_PROFILE_FUNCTION_ENTER
// -----------------------------------------------------------------------------
/// Use this macro at the begining of a profiled function or window.
// =============================================================================
#define SX_PROFILE_FUNCTION_ENTER(eventName) \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SX, (CP_ ## eventName))

// =============================================================================
//  SX_PROFILE_FUNCTION_EXIT
// -----------------------------------------------------------------------------
/// Use this macro at the end of a profiled function or window.
// =============================================================================
#define SX_PROFILE_FUNCTION_EXIT(eventName) \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SX, (CP_ ## eventName))

// =============================================================================
//  SX_PROFILE_PULSE
// -----------------------------------------------------------------------------
/// Use this macro to generate a profiling pulse.
// =============================================================================
#define SX_PROFILE_PULSE(pulseName)  \
        hal_DbgPxtsProfilePulse(HAL_DBG_PXTS_SX, (CP_ ## pulseName))


#define CPJOBSPACEMASK 0x1f


#define SX_PROFILE_JOB_START_ENTER(id) \
    if ((id) != SXR_NIL_JOB && (id) < SXR_NB_MAX_JOB) \
    { \
        UINT16 prfJobId = sxr_Job -> Ctx [(id)].Desc -> Id; \
        if (prfJobId > CPJOBSPACEMASK) prfJobId = CPJOBSPACEMASK; \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SX, CP_sxr_StartJob_0 + prfJobId); \
    }

#define SX_PROFILE_JOB_START_EXIT(id) \
    if ((id) != SXR_NIL_JOB && (id) < SXR_NB_MAX_JOB) \
    { \
        UINT16 prfJobId = sxr_Job -> Ctx [(id)].Desc -> Id; \
        if (prfJobId > CPJOBSPACEMASK) prfJobId = CPJOBSPACEMASK; \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SX, CP_sxr_StartJob_0 + prfJobId); \
    }

#define SX_PROFILE_JOB_STOP_ENTER(id) \
    if ((id) != SXR_NIL_JOB && (id) < SXR_NB_MAX_JOB) \
    { \
        UINT16 prfJobId = sxr_Job -> Ctx [(id)].Desc -> Id; \
        if (prfJobId > CPJOBSPACEMASK) prfJobId = CPJOBSPACEMASK; \
        hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SX, CP_sxr_StopJob_0 + prfJobId); \
    }

#define SX_PROFILE_JOB_STOP_EXIT(id) \
    if ((id) != SXR_NIL_JOB && (id) < SXR_NB_MAX_JOB) \
    { \
        UINT16 prfJobId = sxr_Job -> Ctx [(id)].Desc -> Id; \
        if (prfJobId > CPJOBSPACEMASK) prfJobId = CPJOBSPACEMASK; \
        hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SX, CP_sxr_StopJob_0 + prfJobId); \
    }

#define SX_PROFILE_JOB_ENTER(id) \
    if ((id) != SXR_NIL_JOB && (id) < SXR_NB_MAX_JOB) \
    { \
        hal_DbgPxtsProfileJobEnter(HAL_DBG_PXTS_SX, sxr_Job -> Ctx [id].Desc -> Id);\
    }

#define SX_PROFILE_JOB_EXIT(id) \
    if ((id) != SXR_NIL_JOB && (id) < SXR_NB_MAX_JOB) \
    { \
        hal_DbgPxtsProfileJobExit(HAL_DBG_PXTS_SX, sxr_Job -> Ctx [id].Desc -> Id);\
    }

#define SX_PRFOILE_ENV_ENABLE       (1<<0)
#define SX_PROFILE_QUEUE_ENABLE     (1<<1)
extern u32 sxr_ProfileCfg;

// Profile the usage of an envelop
#define SX_PROFILE_ENVELOP_USE_START(id)\
    if (sxr_ProfileCfg & SX_PRFOILE_ENV_ENABLE) \
    {\
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_SX, CP_SX_ENVELOP_USE_0 + id);\
    }

// Profile the usage of an envelop
#define SX_PROFILE_ENVELOP_USE_END(id)\
    if (sxr_ProfileCfg & SX_PRFOILE_ENV_ENABLE) \
    {\
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_SX, CP_SX_ENVELOP_USE_0 + id);\
    }

// Profile the queue when waiting a msg
#define SX_PROFILE_WAIT_QUEUE_START(id)\
    if (sxr_ProfileCfg & SX_PROFILE_QUEUE_ENABLE) \
    {\
        hal_DbgPxtsProfileWindowEnter(HAL_DBG_PXTS_SX, CP_SX_WAIT_QUEUE_0 + id);\
    }

// Profile the queue when waiting a msg
#define SX_PROFILE_WAIT_QUEUE_END(id)\
    if (sxr_ProfileCfg & SX_PROFILE_QUEUE_ENABLE) \
    {\
        hal_DbgPxtsProfileWindowExit(HAL_DBG_PXTS_SX, CP_SX_WAIT_QUEUE_0 + id);\
    }

#else // NO SX_PROFILING

#define SX_PROFILE_FUNCTION_ENTER(eventName)
#define SX_PROFILE_FUNCTION_EXIT(eventName)
#define SX_PROFILE_PULSE(pulseName)
#define SX_PROFILE_JOB_START_ENTER(id)
#define SX_PROFILE_JOB_START_EXIT(id)
#define SX_PROFILE_JOB_STOP_ENTER(id)
#define SX_PROFILE_JOB_STOP_EXIT(id)
#define SX_PROFILE_JOB_ENTER(id)
#define SX_PROFILE_JOB_EXIT(id)
#define SX_PROFILE_ENVELOP_USE_START(id)
#define SX_PROFILE_ENVELOP_USE_END(id)
#define SX_PROFILE_WAIT_QUEUE_START(id)
#define SX_PROFILE_WAIT_QUEUE_END(id)

#endif // SX_PROFILING

typedef struct
{
    u8 InUse;
    u8 SemaId;
    u8 Pad;
    u8 CallerTask;
    u32 CallerAddr;
} SemaInfo_t;

extern SemaInfo_t sxr_SemaInfo[SXR_NB_MAX_SEM];

#endif // _SXP_DEBUG_H_

