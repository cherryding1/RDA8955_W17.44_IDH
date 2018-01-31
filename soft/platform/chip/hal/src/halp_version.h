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

#ifndef _HAL_VERSION_H_
#define _HAL_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define HAL_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef HAL_VERSION_NUMBER
#define HAL_VERSION_NUMBER                       (0)
#endif

#ifndef HAL_VERSION_DATE
#define HAL_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef HAL_VERSION_STRING
#define HAL_VERSION_STRING                       "HAL version string not defined"
#endif

#ifndef HAL_VERSION_STRING_WITH_BRANCH
#define HAL_VERSION_STRING_WITH_BRANCH           HAL_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define HAL_VERSION_STRUCT                       {HAL_VERSION_REVISION, \
                                                  HAL_VERSION_NUMBER, \
                                                  HAL_VERSION_DATE, \
                                                  HAL_VERSION_STRING_WITH_BRANCH}

#endif // _HAL_VERSION_H_
