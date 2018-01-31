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

#ifndef _PMD_VERSION_H_
#define _PMD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define PMD_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef PMD_VERSION_NUMBER
#define PMD_VERSION_NUMBER                       (0)
#endif

#ifndef PMD_VERSION_DATE
#define PMD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef PMD_VERSION_STRING
#define PMD_VERSION_STRING                       "PMD version string not defined"
#endif

#ifndef PMD_VERSION_STRING_WITH_BRANCH
#define PMD_VERSION_STRING_WITH_BRANCH           PMD_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define PMD_VERSION_STRUCT                       {PMD_VERSION_REVISION, \
                                                  PMD_VERSION_NUMBER, \
                                                  PMD_VERSION_DATE, \
                                                  PMD_VERSION_STRING_WITH_BRANCH}

#endif // _PMD_VERSION_H_
