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

#ifndef _NET_VERSION_H_
#define _NET_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define NET_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef NET_VERSION_NUMBER
#define NET_VERSION_NUMBER                       (0)
#endif

#ifndef NET_VERSION_DATE
#define NET_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef NET_VERSION_STRING
#define NET_VERSION_STRING                       "NET version string not defined"
#endif

#ifndef NET_VERSION_STRING_WITH_BRANCH
#define NET_VERSION_STRING_WITH_BRANCH           NET_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define NET_VERSION_STRUCT                       {NET_VERSION_REVISION, \
                                                  NET_VERSION_NUMBER, \
                                                  NET_VERSION_DATE, \
                                                  NET_VERSION_STRING_WITH_BRANCH}

#endif // _NET_VERSION_H_
