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

#ifndef _DLL_VERSION_H_
#define _DLL_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define DLL_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef DLL_VERSION_NUMBER
#define DLL_VERSION_NUMBER                       (0)
#endif

#ifndef DLL_VERSION_DATE
#define DLL_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef DLL_VERSION_STRING
#define DLL_VERSION_STRING                       "DLL version string not defined"
#endif

#ifndef DLL_VERSION_STRING_WITH_BRANCH
#define DLL_VERSION_STRING_WITH_BRANCH           DLL_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define DLL_VERSION_STRUCT                       {DLL_VERSION_REVISION, \
                                                  DLL_VERSION_NUMBER, \
                                                  DLL_VERSION_DATE, \
                                                  DLL_VERSION_STRING_WITH_BRANCH}

#endif // _DLL_VERSION_H_
