#ifndef _CG_DLNA_IF_H_
#define _CG_DLNA_IF_H_

#include "cs_types.h"
#include "cos.h"

#define AI_PASSIVE          1
#define AI_CANONNAME        2
#define AI_NUMERICHOST      4

#define SOMAXCONN  0xff

#define malloc   COS_Malloc
#define free     COS_Free
#define realloc  COS_Realloc

#define cg_log_print(a,b,c,d,format,...)  hal_DbgTrace(_CSW|CSW_DBG_TRC,0,format, ##__VA_ARGS__)

//#define open(a,b) FS_Open((a), (b), 0)
//#define read(a,b,c) FS_Read((a), (b), (c))
//#define close(a) FS_Close((a))

#endif