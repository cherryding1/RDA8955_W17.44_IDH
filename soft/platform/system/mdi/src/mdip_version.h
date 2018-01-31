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

#ifndef _MDI_VERSION_H_
#define _MDI_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define MDI_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef MDI_VERSION_NUMBER
#define MDI_VERSION_NUMBER                       (0)
#endif

#ifndef MDI_VERSION_DATE
#define MDI_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef MDI_VERSION_STRING
#define MDI_VERSION_STRING                       "MDI version string not defined"
#endif

#ifndef MDI_VERSION_STRING_WITH_BRANCH
#define MDI_VERSION_STRING_WITH_BRANCH           MDI_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define MDI_VERSION_STRUCT                       {MDI_VERSION_REVISION, \
                                                  MDI_VERSION_NUMBER, \
                                                  MDI_VERSION_DATE, \
                                                  MDI_VERSION_STRING_WITH_BRANCH}

#endif // _MDI_VERSION_H_
