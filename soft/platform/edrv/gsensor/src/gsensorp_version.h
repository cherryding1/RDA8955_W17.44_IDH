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

#ifndef _GSENSOR_VERSION_H_
#define _GSENSOR_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define GSENSOR_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef GSENSOR_VERSION_NUMBER
#define GSENSOR_VERSION_NUMBER                       (0)
#endif

#ifndef GSENSOR_VERSION_DATE
#define GSENSOR_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef GSENSOR_VERSION_STRING
#define GSENSOR_VERSION_STRING                       "GSENSOR version string not defined"
#endif

#ifndef GSENSOR_VERSION_STRING_WITH_BRANCH
#define GSENSOR_VERSION_STRING_WITH_BRANCH           GSENSOR_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define GSENSOR_VERSION_STRUCT                       {GSENSOR_VERSION_REVISION, \
                                                  GSENSOR_VERSION_NUMBER, \
                                                  GSENSOR_VERSION_DATE, \
                                                  GSENSOR_VERSION_STRING_WITH_BRANCH}

#endif // _GSENSOR_VERSION_H_
