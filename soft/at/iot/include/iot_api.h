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

#ifndef  __AT_IOT_API_H__
#define  __AT_IOT_API_H__

extern "C" {
#include "cos.h"
#include "cs_types.h"
    void *operator new(size_t sz);
    void operator delete(void *p);

    int AT_OutputText(int nIndex,  char *fmt, ...);
#define IOT_trace(fmt, ...) \
    do{\
        AT_OutputText(1, #fmt, ##__VA_ARGS__);\
    } while(0)

    PVOID COS_Malloc(UINT32 nSize, COS_HEAP_ID nHeapID);
    BOOL COS_Free(PVOID pMemBlock);
#define COS_MALLOC(size) COS_Malloc(size, COS_MMI_HEAP)
#define COS_FREE(p)      COS_Free((PVOID)p)

    UINT32 CFW_SmsSendPresetMessage();
    BOOL COS_Sleep ( UINT32 nMillisecondes);

    VOID IOT_Set_Event_Handler(VOID (*Event_Handler)(COS_EVENT *pev));
}
#endif
