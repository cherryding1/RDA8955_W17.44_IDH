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



#ifndef HELIX_RESULT_H
#define HELIX_RESULT_H

#include "helix_types.h"
#include "hxresult.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */


/* General error definition macro */
#ifndef MAKE_HX_FACILITY_RESULT
#define MAKE_HX_FACILITY_RESULT(sev,fac,code) \
    ((HX_RESULT) (((UINT32)(sev) << 31) | ((UINT32)(fac)<<16) | ((UINT32)(code))))
#endif /* #ifndef MAKE_HX_FACILITY_RESULT */


#define HXR_CORRUPT_FILE                MAKE_HX_RESULT(1,SS_FIL,17)           /* 80040091 */
#define HXR_READ_ERROR                  MAKE_HX_RESULT(1,SS_FIL,18)           /* 80040092 */

/* Define success and failure macros */
#define HX_SUCCEEDED(status) (((UINT32) (status) >> 31) == 0)
#define HX_FAILED(status)    (((UINT32) (status) >> 31) != 0)

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef HELIX_RESULT_H */
