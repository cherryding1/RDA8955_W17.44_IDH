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



























#ifndef RVSTATUS_H__
#define RVSTATUS_H__

#ifdef __cplusplus
extern "C" {
#endif


/* */
/* Enumerate all of our return codes. */
/* */

typedef enum
{

#define DEFRVSTATUS(pianame, vfwname, amname, rvname)  pianame,
#include "rvdefs.h"

    RV_NUMBER_OF_STATUS_CODES

} RV_Status;

#ifdef __cplusplus
}
#endif


#endif /*RVSTATUS_H__ */
