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













#ifndef RVDEBUG_H__
#define RVDEBUG_H__

#include "rvtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void*    RV_Allocate(U32 size, Bool32 zero_init);
/* Allocate 'size' bytes of memory, optionally clear it, and */
/* return a pointer to it.  Returns NULL if allocation fails. */
/* The caller is repsonsible for recovering from a failed allocation. */
/* Allocating 0 bytes is supported, though it will result in */
/* some non-zero number of bytes actually being allocated. */

extern void      RV_Free(void*);
/* Free memory previously allocated by RV_Allocate. */




#define  RV_TL_ALWAYS   0
#define  RV_TL_INIT     2
#define  RV_TL_PICT     4
#define  RV_TL_SUBPICT  6
#define  RV_TL_MBLK     8
#define  RV_TL_BLK      10
#define  RV_TL_HDET     12

typedef U32                 RV_Trace_Level;
#if defined (DEBUG)

extern  void   RV_Debug_Message
(
    RV_Trace_Level,
    char *fmt,
    ...
);

extern  void    RV_Assert_Failed
(
    char   *file_name,
    I32     line,
    char*   expr
);
#define RVAssert(expr)   do { \
                              if (!(expr)) \
                                  RV_Assert_Failed(__FILE__,__LINE__,#expr);\
                          } while (0)


#define RVDebug(params)  //RV_Debug_Message params


#else
#define RVDebug(params)
#define RVAssert(expr)

#endif



#ifdef __cplusplus
}
#endif

#endif /* RVDEBUG_H__ */
