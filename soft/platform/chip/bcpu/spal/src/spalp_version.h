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

#ifndef _SPAL_VERSION_H_
#define _SPAL_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define SPAL_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef SPAL_VERSION_NUMBER
#define SPAL_VERSION_NUMBER                       (0)
#endif

#ifndef SPAL_VERSION_DATE
#define SPAL_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef SPAL_VERSION_STRING
#define SPAL_VERSION_STRING                       "SPAL version string not defined"
#endif

#ifndef SPAL_VERSION_STRING_WITH_BRANCH
#define SPAL_VERSION_STRING_WITH_BRANCH           SPAL_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define SPAL_VERSION_STRUCT                       {SPAL_VERSION_REVISION, \
                                                  SPAL_VERSION_NUMBER, \
                                                  SPAL_VERSION_DATE, \
                                                  SPAL_VERSION_STRING_WITH_BRANCH}

#endif // _SPAL_VERSION_H_
