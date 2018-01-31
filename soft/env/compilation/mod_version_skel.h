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

#ifndef _@{MOD}_VERSION_H_
#define _@{MOD}_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define @{MOD}_VERSION_REVISION                     (@{LOCAL_REVISION})

// =============================================================================
//  TYPES
// =============================================================================

#ifndef @{MOD}_VERSION_NUMBER
#define @{MOD}_VERSION_NUMBER                       (0)
#endif

#ifndef @{MOD}_VERSION_DATE
#define @{MOD}_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef @{MOD}_VERSION_STRING
#define @{MOD}_VERSION_STRING                       "@{MOD} version string not defined"
#endif

#ifndef @{MOD}_VERSION_STRING_WITH_BRANCH
#define @{MOD}_VERSION_STRING_WITH_BRANCH           @{MOD}_VERSION_STRING " Branch: " @{LOCAL_BRANCH}
#endif

#define @{MOD}_VERSION_STRUCT                       {@{MOD}_VERSION_REVISION, \
                                                  @{MOD}_VERSION_NUMBER, \
                                                  @{MOD}_VERSION_DATE, \
                                                  @{MOD}_VERSION_STRING_WITH_BRANCH}

#endif // _@{MOD}_VERSION_H_
