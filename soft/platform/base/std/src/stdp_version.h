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

#ifndef _STD_VERSION_H_
#define _STD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define STD_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef STD_VERSION_NUMBER
#define STD_VERSION_NUMBER                       (0)
#endif

#ifndef STD_VERSION_DATE
#define STD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef STD_VERSION_STRING
#define STD_VERSION_STRING                       "STD version string not defined"
#endif

#ifndef STD_VERSION_STRING_WITH_BRANCH
#define STD_VERSION_STRING_WITH_BRANCH           STD_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define STD_VERSION_STRUCT                       {STD_VERSION_REVISION, \
                                                  STD_VERSION_NUMBER, \
                                                  STD_VERSION_DATE, \
                                                  STD_VERSION_STRING_WITH_BRANCH}

#endif // _STD_VERSION_H_
