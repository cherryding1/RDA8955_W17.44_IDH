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

#ifndef _PAL_VERSION_H_
#define _PAL_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define PAL_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef PAL_VERSION_NUMBER
#define PAL_VERSION_NUMBER                       (0)
#endif

#ifndef PAL_VERSION_DATE
#define PAL_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef PAL_VERSION_STRING
#define PAL_VERSION_STRING                       "PAL version string not defined"
#endif

#ifndef PAL_VERSION_STRING_WITH_BRANCH
#define PAL_VERSION_STRING_WITH_BRANCH           PAL_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define PAL_VERSION_STRUCT                       {PAL_VERSION_REVISION, \
                                                  PAL_VERSION_NUMBER, \
                                                  PAL_VERSION_DATE, \
                                                  PAL_VERSION_STRING_WITH_BRANCH}

#endif // _PAL_VERSION_H_
