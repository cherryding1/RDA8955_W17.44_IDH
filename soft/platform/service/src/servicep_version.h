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

#ifndef _SERVICE_VERSION_H_
#define _SERVICE_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define SERVICE_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef SERVICE_VERSION_NUMBER
#define SERVICE_VERSION_NUMBER                       (0)
#endif

#ifndef SERVICE_VERSION_DATE
#define SERVICE_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef SERVICE_VERSION_STRING
#define SERVICE_VERSION_STRING                       "SERVICE version string not defined"
#endif

#ifndef SERVICE_VERSION_STRING_WITH_BRANCH
#define SERVICE_VERSION_STRING_WITH_BRANCH           SERVICE_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define SERVICE_VERSION_STRUCT                       {SERVICE_VERSION_REVISION, \
                                                  SERVICE_VERSION_NUMBER, \
                                                  SERVICE_VERSION_DATE, \
                                                  SERVICE_VERSION_STRING_WITH_BRANCH}

#endif // _SERVICE_VERSION_H_
