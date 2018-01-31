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

#ifndef _LCDD_VERSION_H_
#define _LCDD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define LCDD_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef LCDD_VERSION_NUMBER
#define LCDD_VERSION_NUMBER                       (0)
#endif

#ifndef LCDD_VERSION_DATE
#define LCDD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef LCDD_VERSION_STRING
#define LCDD_VERSION_STRING                       "LCDD version string not defined"
#endif

#ifndef LCDD_VERSION_STRING_WITH_BRANCH
#define LCDD_VERSION_STRING_WITH_BRANCH           LCDD_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define LCDD_VERSION_STRUCT                       {LCDD_VERSION_REVISION, \
                                                  LCDD_VERSION_NUMBER, \
                                                  LCDD_VERSION_DATE, \
                                                  LCDD_VERSION_STRING_WITH_BRANCH}

#endif // _LCDD_VERSION_H_
