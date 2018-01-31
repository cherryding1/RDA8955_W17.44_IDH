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

#ifndef _AT_VERSION_H_
#define _AT_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define AT_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef AT_VERSION_NUMBER
#define AT_VERSION_NUMBER                       (0)
#endif

#ifndef AT_VERSION_DATE
#define AT_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef AT_VERSION_STRING
#define AT_VERSION_STRING                       "AT version string not defined"
#endif

#ifndef AT_VERSION_STRING_WITH_BRANCH
#define AT_VERSION_STRING_WITH_BRANCH           AT_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define AT_VERSION_STRUCT                       {AT_VERSION_REVISION, \
                                                  AT_VERSION_NUMBER, \
                                                  AT_VERSION_DATE, \
                                                  AT_VERSION_STRING_WITH_BRANCH}

#endif // _AT_VERSION_H_
