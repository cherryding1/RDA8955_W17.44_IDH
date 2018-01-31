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

#ifndef _SPP_VERSION_H_
#define _SPP_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define SPP_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef SPP_VERSION_NUMBER
#define SPP_VERSION_NUMBER                       (0)
#endif

#ifndef SPP_VERSION_DATE
#define SPP_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef SPP_VERSION_STRING
#define SPP_VERSION_STRING                       "SPP version string not defined"
#endif

#ifndef SPP_VERSION_STRING_WITH_BRANCH
#define SPP_VERSION_STRING_WITH_BRANCH           SPP_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define SPP_VERSION_STRUCT                       {SPP_VERSION_REVISION, \
                                                  SPP_VERSION_NUMBER, \
                                                  SPP_VERSION_DATE, \
                                                  SPP_VERSION_STRING_WITH_BRANCH}

#endif // _SPP_VERSION_H_
