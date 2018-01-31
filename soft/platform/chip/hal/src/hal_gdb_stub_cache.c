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

#include "cs_types.h"
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
#include "hal_cache.h"
#endif
#include "hal_sys.h"

extern VOID hal_GdbInvalidCache(VOID);
extern VOID hal_GdbInvalidICache(VOID);
extern VOID hal_GdbInvalidCacheData(VOID);

VOID hal_GdbFlushCache(VOID)
{
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DcacheFlushAll(TRUE);
    UINT32 status = hal_SysEnterCriticalSection();
    hal_GdbInvalidICache();
    hal_SysExitCriticalSection(status);
#else
    UINT32 status = hal_SysEnterCriticalSection();
    hal_GdbInvalidCache();
    hal_SysExitCriticalSection(status);
#endif
}


VOID hal_GdbFlushCacheData(VOID)
{
#if(XCPU_CACHE_MODE_WRITE_BACK==1)
    hal_DcacheFlushAll(TRUE);
#else
    UINT32 status = hal_SysEnterCriticalSection();
    hal_GdbInvalidCacheData();
    hal_SysExitCriticalSection(status);
#endif
}


