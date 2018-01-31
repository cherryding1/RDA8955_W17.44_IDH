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

#ifndef _MAGIC_SOUND_VERSION_H_
#define _MAGIC_SOUND_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define MAGIC_SOUND_VERSION_REVISION                     (0x6806430c)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef MAGIC_SOUND_VERSION_NUMBER
#define MAGIC_SOUND_VERSION_NUMBER                       (0)
#endif

#ifndef MAGIC_SOUND_VERSION_DATE
#define MAGIC_SOUND_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef MAGIC_SOUND_VERSION_STRING
#define MAGIC_SOUND_VERSION_STRING                       "MAGIC_SOUND version string not defined"
#endif

#ifndef MAGIC_SOUND_VERSION_STRING_WITH_BRANCH
#define MAGIC_SOUND_VERSION_STRING_WITH_BRANCH           MAGIC_SOUND_VERSION_STRING " Branch: " "8955.W17.44"
#endif

#define MAGIC_SOUND_VERSION_STRUCT                       {MAGIC_SOUND_VERSION_REVISION, \
                                                  MAGIC_SOUND_VERSION_NUMBER, \
                                                  MAGIC_SOUND_VERSION_DATE, \
                                                  MAGIC_SOUND_VERSION_STRING_WITH_BRANCH}

#endif // _MAGIC_SOUND_VERSION_H_
