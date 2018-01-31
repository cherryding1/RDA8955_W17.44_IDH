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











#if !defined(__BASE_H__)
#define __BASE_H__

//
// the necessary header and pragma defines.
//
#include <cswtype.h>
#include <errorcode.h>
#include <event.h>
#include <rfm.h>
#include <rcm.h>

BOOL SRVAPI BS_Init(VOID);
BOOL SRVAPI BS_Exit(VOID);

#endif // _H_
