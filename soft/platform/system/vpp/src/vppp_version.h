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

#ifndef _VPP_VERSION_H_
#define _VPP_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define VPP_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef VPP_VERSION_NUMBER
#define VPP_VERSION_NUMBER                       (0)
#endif

#ifndef VPP_VERSION_DATE
#define VPP_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef VPP_VERSION_STRING
#define VPP_VERSION_STRING                       "VPP version string not defined"
#endif

#ifndef VPP_VERSION_STRING_WITH_BRANCH
#define VPP_VERSION_STRING_WITH_BRANCH           VPP_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define VPP_VERSION_STRUCT                       {VPP_VERSION_REVISION, \
                                                  VPP_VERSION_NUMBER, \
                                                  VPP_VERSION_DATE, \
                                                  VPP_VERSION_STRING_WITH_BRANCH}

#endif // _VPP_VERSION_H_
