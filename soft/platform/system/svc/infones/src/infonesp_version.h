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

#ifndef _INFONES_VERSION_H_
#define _INFONES_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define INFONES_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef INFONES_VERSION_NUMBER
#define INFONES_VERSION_NUMBER                       (0)
#endif

#ifndef INFONES_VERSION_DATE
#define INFONES_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef INFONES_VERSION_STRING
#define INFONES_VERSION_STRING                       "INFONES version string not defined"
#endif

#ifndef INFONES_VERSION_STRING_WITH_BRANCH
#define INFONES_VERSION_STRING_WITH_BRANCH           INFONES_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define INFONES_VERSION_STRUCT                       {INFONES_VERSION_REVISION, \
                                                  INFONES_VERSION_NUMBER, \
                                                  INFONES_VERSION_DATE, \
                                                  INFONES_VERSION_STRING_WITH_BRANCH}

#endif // _INFONES_VERSION_H_
