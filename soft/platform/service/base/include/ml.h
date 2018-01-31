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

#ifndef __ML_H__
#define __ML_H__
#include "ts.h"
#ifdef _FS_SIMULATOR_
#include "dsm_cf.h"
//#define NULL ((void*)0)

//#include "ts.h"
#define CSW_ML_FREE DSM_Free
#define CSW_ML_MALLOC DSM_MAlloc
#define __func__ "func"
#define SUL_StrCopy DSM_StrCpy
#define SUL_MemSet8 DSM_MemSet
#define SUL_StrCaselessCompare DSM_StrCaselessCmp
struct ML_Table
{
    UINT8 *charset;
    INT32 (*uni2char) (UINT16 uni, UINT8 *out, UINT32 boundlen);
    INT32 (*char2uni) (UINT8 *rawstring, UINT32 boundlen, UINT16 *uni);
    UINT8 *charset2lower;
    UINT8 *charset2upper;
    struct ML_Table *next;
};
UINT32 ML_RegisterTable(struct ML_Table *ML);
UINT32 ML_UnRegisterTable(struct ML_Table *ML);

#define CP936
#define CP0
#define CP1256
#define CP1258
#define CP874
//#define ISO8859_1
#define ISO8859_5
#define ISO8859_6


#define ML_DEF

#else

#include "cfw.h"

struct ML_Table
{
    UINT8 *charset;
    INT32 (*uni2char) (UINT16 uni, UINT8 *out, UINT32 boundlen);
    INT32 (*char2uni) (UINT8 *rawstring, UINT32 boundlen, UINT16 *uni);
    UINT8 *charset2lower;
    UINT8 *charset2upper;
    struct ML_Table *next;
};
UINT32 ML_RegisterTable(struct ML_Table *ML);
UINT32 ML_UnRegisterTable(struct ML_Table *ML);

#ifdef ML_SUPPORT_CP0
#define CP0
#endif

#ifdef ML_SUPPORT_CP437
#define CP437
#endif

#ifdef ML_SUPPORT_CP737
#define CP737
#endif

#ifdef ML_SUPPORT_CP775
#define CP775
#endif

#ifdef ML_SUPPORT_CP850
#define CP850
#endif

#ifdef ML_SUPPORT_CP852
#define CP852
#endif

#ifdef ML_SUPPORT_CP855
#define CP855
#endif

#ifdef ML_SUPPORT_CP857
#define CP857
#endif

#ifdef ML_SUPPORT_CP860
#define CP860
#endif

#ifdef ML_SUPPORT_CP861
#define CP861
#endif

#ifdef ML_SUPPORT_CP862
#define CP862
#endif

#ifdef ML_SUPPORT_CP863
#define CP863
#endif

#ifdef ML_SUPPORT_CP864
#define CP864
#endif

#ifdef ML_SUPPORT_CP865
#define CP865
#endif

#ifdef ML_SUPPORT_CP869
#define CP869
#endif

#ifdef ML_SUPPORT_CP874
#define CP874
#endif

#ifdef ML_SUPPORT_CP932
#define CP932
#endif

#ifdef ML_SUPPORT_CP936
#define CP936
#endif

#ifdef ML_SUPPORT_CP950
#define CP950
#endif

#ifdef ML_SUPPORT_CP1250
#define CP1250
#endif

#ifdef ML_SUPPORT_CP1251
#define CP1251
#endif

#ifdef ML_SUPPORT_CP1255
#define CP1255
#endif

#ifdef ML_SUPPORT_CP1256
#define CP1256
#endif

#ifdef ML_SUPPORT_CP1258
#define CP1258
#endif

#ifdef ML_SUPPORT_ISO8859_1
#define ISO8859_1
#endif

#ifdef ML_SUPPORT_ISO8859_2
#define ISO8859_2
#endif

#ifdef ML_SUPPORT_ISO8859_3
#define ISO8859_3
#endif

#ifdef ML_SUPPORT_ISO8859_4
#define ISO8859_4
#endif

#ifdef ML_SUPPORT_ISO8859_5
#define ISO8859_5
#endif

#ifdef ML_SUPPORT_ISO8859_6
#define ISO8859_6
#endif

#ifdef ML_SUPPORT_ISO8859_7
#define ISO8859_7
#endif

#ifdef ML_SUPPORT_ISO8859_9
#define ISO8859_9
#endif

#ifdef ML_SUPPORT_ISO8859_13
#define ISO8859_13
#endif

#ifdef ML_SUPPORT_ISO8859_14
#define ISO8859_14
#endif

#ifdef ML_SUPPORT_ISO8859_15
#define ISO8859_15
#endif


#endif



#endif

