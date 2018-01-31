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


#ifndef _VPP_FR_DEBUG_H_
#define _VPP_FR_DEBUG_H_

#include "hal_debug.h"

#ifdef VPP_FR_FUNC_PROFILING
#define VPP_FR_PROFILE_FUNCTION_ENTER(eventName) \
     hal_DbgPxtsProfileFunctionEnter(HAL_DBG_PXTS_SVC, (CP_ ## eventName))

#define VPP_FR_PROFILE_FUNCTION_EXIT(eventName) \
     hal_DbgPxtsProfileFunctionExit(HAL_DBG_PXTS_SVC, (CP_ ## eventName))
#else
#define VPP_FR_PROFILE_FUNCTION_ENTER(eventName)
#define VPP_FR_PROFILE_FUNCTION_EXIT(eventName)
#endif


#endif // _SPCP_DEBUG_H_

