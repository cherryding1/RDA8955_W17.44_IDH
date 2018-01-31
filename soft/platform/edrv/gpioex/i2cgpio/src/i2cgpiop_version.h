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

#ifndef _I2CGPIO_VERSION_H_
#define _I2CGPIO_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define I2CGPIO_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef I2CGPIO_VERSION_NUMBER
#define I2CGPIO_VERSION_NUMBER                       (0)
#endif

#ifndef I2CGPIO_VERSION_DATE
#define I2CGPIO_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef I2CGPIO_VERSION_STRING
#define I2CGPIO_VERSION_STRING                       "I2CGPIO version string not defined"
#endif

#ifndef I2CGPIO_VERSION_STRING_WITH_BRANCH
#define I2CGPIO_VERSION_STRING_WITH_BRANCH           I2CGPIO_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define I2CGPIO_VERSION_STRUCT                       {I2CGPIO_VERSION_REVISION, \
                                                  I2CGPIO_VERSION_NUMBER, \
                                                  I2CGPIO_VERSION_DATE, \
                                                  I2CGPIO_VERSION_STRING_WITH_BRANCH}

#endif // _I2CGPIO_VERSION_H_
