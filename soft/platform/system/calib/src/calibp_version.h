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

#ifndef _CALIB_VERSION_H_
#define _CALIB_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define CALIB_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef CALIB_VERSION_NUMBER
#define CALIB_VERSION_NUMBER                       (0)
#endif

#ifndef CALIB_VERSION_DATE
#define CALIB_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef CALIB_VERSION_STRING
#define CALIB_VERSION_STRING                       "CALIB version string not defined"
#endif

#ifndef CALIB_VERSION_STRING_WITH_BRANCH
#define CALIB_VERSION_STRING_WITH_BRANCH           CALIB_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define CALIB_VERSION_STRUCT                       {CALIB_VERSION_REVISION, \
                                                  CALIB_VERSION_NUMBER, \
                                                  CALIB_VERSION_DATE, \
                                                  CALIB_VERSION_STRING_WITH_BRANCH}

#endif // _CALIB_VERSION_H_
