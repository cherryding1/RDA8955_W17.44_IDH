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























#if !defined(__CSWTYPE_H__)
#define __CSWTYPE_H__

#define HAVE_TM_MODULE    1
#define HAVE_LIBC_MALLOC  0
#include "cs_types.h"

// CSW specific types
typedef UINT16              RESID;

typedef HANDLE              HRES;
typedef UINT32              EVID;

typedef UINT32              EVPARAM;
typedef UINT32              HAO;

/*function point type*/
typedef BOOL (*PFN_BOOL)(VOID);

#endif // _H_

