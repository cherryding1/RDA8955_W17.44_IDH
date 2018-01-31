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

#ifndef _MEMD_VERSION_H_
#define _MEMD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define MEMD_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef MEMD_VERSION_NUMBER
#define MEMD_VERSION_NUMBER                       (0)
#endif

#ifndef MEMD_VERSION_DATE
#define MEMD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef MEMD_VERSION_STRING
#define MEMD_VERSION_STRING                       "MEMD version string not defined"
#endif

#ifndef MEMD_VERSION_STRING_WITH_BRANCH
#define MEMD_VERSION_STRING_WITH_BRANCH           MEMD_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define MEMD_VERSION_STRUCT                       {MEMD_VERSION_REVISION, \
                                                  MEMD_VERSION_NUMBER, \
                                                  MEMD_VERSION_DATE, \
                                                  MEMD_VERSION_STRING_WITH_BRANCH}

#endif // _MEMD_VERSION_H_
