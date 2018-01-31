#ifndef _CG_HTTP_IF_H_
#define _CG_HTTP_IF_H_

#include "cs_types.h"
#include "cos.h"
#include "csw_debug.h"

#define AI_PASSIVE          1
#define AI_CANONNAME        2
#define AI_NUMERICHOST      4

#define SOMAXCONN  0xff

#define malloc(size)   COS_Malloc((UINT32)size,COS_MMI_HEAP)
#define free     COS_Free
#define realloc  COS_Realloc

//#define cg_log_print(a,b,c,d,format,...)  SXS_TRACE(_CSW|CSW_DBG_TRC,format, ##__VA_ARGS__)

//#define open(a,b) FS_Open((a), (b), 0)
//#define read(a,b,c) FS_Read((a), (b), (c))
//#define close(a) FS_Close((a))

#endif