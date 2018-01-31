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

#if !defined(_SPIFFS_DEBUG_HEADER)
#define _SPIFFS_DEBUG_HEADER

#define SPIFFS_DEAD_DEBUG 0

#include <cos.h>

#define SF_TRACE_ID (TID(_CSW) | TLEVEL(1))
#define SF_LOG(level, fmt, ...) COS_TRACE_##level(SF_TRACE_ID, fmt, ##__VA_ARGS__)
#define SF_LOGS(level, tsmap, fmt, ...) COS_TRACE_##level(SF_TRACE_ID | (tsmap), fmt, ##__VA_ARGS__)

#if (SPIFFS_DEAD_DEBUG == 1)
#define SF_ASSERT(b, fmt, ...) COS_Assert(b, fmt, ##__VA_ARGS__)
#else
#define SF_ASSERT(...)
#endif

#endif /* _SPIFFS_DEBUG_HEADER */
