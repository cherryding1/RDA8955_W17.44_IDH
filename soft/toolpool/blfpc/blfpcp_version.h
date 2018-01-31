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

#ifndef _BLFPC_VERSION_H_
#define _BLFPC_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define BLFPC_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef BLFPC_VERSION_NUMBER
#define BLFPC_VERSION_NUMBER                       (0)
#endif

#ifndef BLFPC_VERSION_DATE
#define BLFPC_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef BLFPC_VERSION_STRING
#define BLFPC_VERSION_STRING                       "BLFPC version string not defined"
#endif

#ifndef BLFPC_VERSION_STRING_WITH_BRANCH
#define BLFPC_VERSION_STRING_WITH_BRANCH           BLFPC_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define BLFPC_VERSION_STRUCT                       {BLFPC_VERSION_REVISION, \
                                                  BLFPC_VERSION_NUMBER, \
                                                  BLFPC_VERSION_DATE, \
                                                  BLFPC_VERSION_STRING_WITH_BRANCH}

#endif // _BLFPC_VERSION_H_
