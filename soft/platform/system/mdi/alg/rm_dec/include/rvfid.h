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



























#ifndef RVFID_H__
#define RVFID_H__


#ifdef __cplusplus
extern "C" {
#endif
/* */
/* Enumerate all of our external and internal image formats. */
/* Each enumeration literal is a format identifier, thus the name RV_FID. */
/* */

typedef enum
{

#define DEFRVFMT(fid, bits, updown, yuv, fourcc, onetoone, amsubtype) fid,
#include "rvdefs.h"

    RV_NUMBER_OF_IMAGE_FORMATS

} RV_FID;

#ifdef __cplusplus
}
#endif
#endif /*RVFID_H__ */
