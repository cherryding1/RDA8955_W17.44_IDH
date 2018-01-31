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

#ifndef _AUD_VERSION_H_
#define _AUD_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define AUD_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef AUD_VERSION_NUMBER
#define AUD_VERSION_NUMBER                       (0)
#endif

#ifndef AUD_VERSION_DATE
#define AUD_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef AUD_VERSION_STRING
#define AUD_VERSION_STRING                       "AUD version string not defined"
#endif

#ifndef AUD_VERSION_STRING_WITH_BRANCH
#define AUD_VERSION_STRING_WITH_BRANCH           AUD_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define AUD_VERSION_STRUCT                       {AUD_VERSION_REVISION, \
                                                  AUD_VERSION_NUMBER, \
                                                  AUD_VERSION_DATE, \
                                                  AUD_VERSION_STRING_WITH_BRANCH}

#endif // _AUD_VERSION_H_
