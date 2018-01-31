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







#ifndef _KAL_NON_SPECIFIC_GENERAL_TYPES_H
#define _KAL_NON_SPECIFIC_GENERAL_TYPES_H

/*
#ifdef WIN32
#include "windows.h"
#endif
*/
typedef void (*kal_func_ptr)(void);


/*******************************************************************************
 * Type Definitions
 *******************************************************************************/
typedef unsigned char           kal_uint8;
typedef signed char             kal_int8;
typedef char                    kal_char;
typedef unsigned short          kal_wchar;

typedef unsigned short int      kal_uint16;
typedef signed short int        kal_int16;

typedef unsigned int            kal_uint32;
typedef signed int              kal_int32;

#ifndef MMI_ON_HARDWARE_P
//typedef ULONG64              kal_uint64;
//typedef LONG64               kal_int64;
typedef unsigned __int64      kal_uint64;
typedef __int64               kal_int64;
#else
typedef unsigned long long   kal_uint64;
typedef signed long long     kal_int64;
#endif

typedef enum
{
    KAL_FALSE,
    KAL_TRUE
} kal_bool;

/*******************************************************************************
 * Constant definition
 *******************************************************************************/

#if 1 /* wufasong added 2007.06.14 */
//#ifdef KAL_ON_NUCLEUS

#define KAL_AND               NU_AND
#define KAL_CONSUME           NU_OR_CONSUME
#define KAL_AND_CONSUME       NU_AND_CONSUME
#define KAL_NO_SUSPEND        NU_NO_SUSPEND
#define KAL_OR                NU_OR
#define KAL_OR_CONSUME        NU_OR_CONSUME
#define KAL_SUSPEND           NU_SUSPEND

#endif /* KAL_ON_NUCLEUS */

#endif  /* _KAL_NON_SPECIFIC_GENERAL_TYPES_H */


