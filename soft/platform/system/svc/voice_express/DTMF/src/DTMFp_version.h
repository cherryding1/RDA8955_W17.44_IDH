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

#ifndef _DTMF_VERSION_H_
#define _DTMF_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define DTMF_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef DTMF_VERSION_NUMBER
#define DTMF_VERSION_NUMBER                       (0)
#endif

#ifndef DTMF_VERSION_DATE
#define DTMF_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef DTMF_VERSION_STRING
#define DTMF_VERSION_STRING                       "DTMF version string not defined"
#endif

#ifndef DTMF_VERSION_STRING_WITH_BRANCH
#define DTMF_VERSION_STRING_WITH_BRANCH           DTMF_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define DTMF_VERSION_STRUCT                       {DTMF_VERSION_REVISION, \
                                                  DTMF_VERSION_NUMBER, \
                                                  DTMF_VERSION_DATE, \
                                                  DTMF_VERSION_STRING_WITH_BRANCH}

#endif // _DTMF_VERSION_H_
