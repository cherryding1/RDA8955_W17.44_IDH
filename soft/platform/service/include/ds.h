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





















#ifndef _DS_H_
#define _DS_H_

#ifdef __cplusplus
CPP_START
#endif

#include <ts.h>


UINT32 SRVAPI DS_FatalErr (
    PCSTR pszFileName,
    UINT32 nLineNum,
    PCSTR pszErrStr
);

//
// Redefine the Debug API to trace in default level.
//
#if defined(DEBUG)
#if defined(_WIN32)
#define DS_ASSERT(x)
#define DS_FATAL_ERR(x)
#define DS_CHECKHANDLE(handle)
#else // _WIN32
#define DS_CHECKHANDLE(handle)
#define DS_ASSERT(x)
#endif
#else // DEBUG
#define DS_CHECKHANDLE(handle)      {    }
#define DS_ASSERT(x)                {    }
#endif // DEBUG

//////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
CPP_END
#endif

#endif // _H_
