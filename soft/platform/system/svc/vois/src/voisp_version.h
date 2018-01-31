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

#ifndef _VOIS_VERSION_H_
#define _VOIS_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define VOIS_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef VOIS_VERSION_NUMBER
#define VOIS_VERSION_NUMBER                       (0)
#endif

#ifndef VOIS_VERSION_DATE
#define VOIS_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef VOIS_VERSION_STRING
#define VOIS_VERSION_STRING                       "VOIS version string not defined"
#endif

#ifndef VOIS_VERSION_STRING_WITH_BRANCH
#define VOIS_VERSION_STRING_WITH_BRANCH           VOIS_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define VOIS_VERSION_STRUCT                       {VOIS_VERSION_REVISION, \
                                                  VOIS_VERSION_NUMBER, \
                                                  VOIS_VERSION_DATE, \
                                                  VOIS_VERSION_STRING_WITH_BRANCH}

#endif // _VOIS_VERSION_H_
