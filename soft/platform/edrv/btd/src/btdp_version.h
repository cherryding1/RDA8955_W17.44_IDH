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

#ifndef _BTD_VERSION_H_
#define _BTD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define BTD_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef BTD_VERSION_NUMBER
#define BTD_VERSION_NUMBER                       (0)
#endif

#ifndef BTD_VERSION_DATE
#define BTD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef BTD_VERSION_STRING
#define BTD_VERSION_STRING                       "BTD version string not defined"
#endif

#ifndef BTD_VERSION_STRING_WITH_BRANCH
#define BTD_VERSION_STRING_WITH_BRANCH           BTD_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define BTD_VERSION_STRUCT                       {BTD_VERSION_REVISION, \
                                                  BTD_VERSION_NUMBER, \
                                                  BTD_VERSION_DATE, \
                                                  BTD_VERSION_STRING_WITH_BRANCH}

#endif // _BTD_VERSION_H_
