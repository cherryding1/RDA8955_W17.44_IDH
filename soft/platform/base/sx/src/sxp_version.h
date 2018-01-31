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

#ifndef _SX_VERSION_H_
#define _SX_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define SX_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef SX_VERSION_NUMBER
#define SX_VERSION_NUMBER                       (0)
#endif

#ifndef SX_VERSION_DATE
#define SX_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef SX_VERSION_STRING
#define SX_VERSION_STRING                       "SX version string not defined"
#endif

#ifndef SX_VERSION_STRING_WITH_BRANCH
#define SX_VERSION_STRING_WITH_BRANCH           SX_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define SX_VERSION_STRUCT                       {SX_VERSION_REVISION, \
                                                  SX_VERSION_NUMBER, \
                                                  SX_VERSION_DATE, \
                                                  SX_VERSION_STRING_WITH_BRANCH}

#endif // _SX_VERSION_H_
