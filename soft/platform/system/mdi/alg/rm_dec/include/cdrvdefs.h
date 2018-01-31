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





























#ifndef CDRVDEFS_H__
#define CDRVDEFS_H__

#if defined(_WIN32)
#include <tchar.h>
#endif

#include "beginhdr.h"
#include "rvtypes.h"
/*#include "rvmain.h" */
#include "rvmsg.h"
#include "rvstruct.h"
#include "hxtypes.h"

#if !defined(CPK) && defined(APK)
#error "Sorry, you cannot define APK without CPK"
#endif



/* Although the standard allows for a minimum width or height of 4, this */
/* implementation restricts the minimum value to 32. */
#define MIN_XDIM                       32
#define MAX_XDIM                     2048

#define MIN_YDIM                       32
#define MAX_YDIM                     1152

/* pjg - changed this to long to avoid pointer truncation on 64 bit architectures */
#define ALIGN_RV(memptr,align)   ( ((long)(memptr) + ((align)-1)) & ~((align)-1) )
/* The alignment macro is assuming that the align value is a power of 2. */

/* Timing macros */
/* START_TIMER initializes start_ctr */
/* STOP_TIMER adds elapsed time to ctr and reinitializes start_ctr */
/* Note: start_ctr should be type size_t and ctr should be type U32 */

/* Some hosts require a timing object and/or initialization to access */
/* precise timers. The following 3 macros are used in those special cases. */
/* Define here to null for the usual case. */
#define TIME_DETAIL_INST
#define ALLOC_TIME_DETAIL_INST
#define FREE_TIME_DETAIL_INST

#if defined(_WIN32_WCE) && (defined(ARM) || defined(_ARM_))

#include "ixs_timer.h"
#undef ALLOC_TIME_DETAIL_INST
#undef FREE_TIME_DETAIL_INST

#define ALLOC_TIME_DETAIL_INST ixs_timer_alloc();
#define FREE_TIME_DETAIL_INST
#define START_TIMER(ctr)                            \
{                                                   \
        (ctr) = ixs_get_OSCR_time();                \
}
#define STOP_TIMER(ctr, start_ctr)                      \
{                                                       \
    size_t utmp = ixs_get_OSCR_time();              \
    if (utmp >= (start_ctr))                                \
        (ctr) += U32(utmp - (start_ctr));                   \
    else                                                    \
        (ctr) += U32(utmp - (start_ctr) + size_t(-1) + 1);  \
    (start_ctr) = utmp;                                     \
}

#elif defined(_WIN64) && defined(_M_IA64)

extern "C" unsigned __int64 __getReg(int whichReg);
#define INL_REGID_APITC 3116
#pragma intrinsic(__getReg)

#define START_TIMER(ctr)                \
{                                       \
    (ctr) = __getReg(INL_REGID_APITC);  \
}

#define STOP_TIMER(ctr,start_ctr)                           \
{                                                           \
    size_t utmp = __getReg(INL_REGID_APITC);                \
    if (utmp >= (start_ctr))                                \
        (ctr) += U32(utmp - (start_ctr));                   \
    else                                                    \
        (ctr) += U32(utmp - (start_ctr) + size_t(-1) + 1);  \
    (start_ctr) = utmp;                                     \
}

#elif defined(_WIN32) && defined(_M_IX86)

#define START_TIMER(start_ctr)  \
{                               \
    size_t utmp;                \
    {                           \
        __asm xor eax, eax      \
        __asm xor edx, edx      \
        __asm _emit 0x0f        \
        __asm _emit 0x31        \
        __asm mov utmp,eax      \
    }                           \
    (start_ctr) = utmp;         \
}

#define STOP_TIMER(ctr,start_ctr)                           \
{                                                           \
    size_t utmp;                                            \
    {                                                       \
        __asm xor eax, eax                                  \
        __asm xor edx, edx                                  \
        __asm _emit 0x0f                                    \
        __asm _emit 0x31                                    \
        __asm mov utmp,eax                                  \
    }                                                       \
    if (utmp >= (start_ctr))                                \
        (ctr) += U32(utmp - (start_ctr));                   \
    else                                                    \
        (ctr) += U32(utmp - (start_ctr) + size_t(-1) + 1);  \
    (start_ctr) = utmp;                                     \
}

#else

#define START_TIMER(ctr)
#define STOP_TIMER(ctr,start_ctr)

#endif

/* */
/* The Implementation_ID type provides symbolic names for CPU-specific */
/* implementations of algorithms.  An object of this type is used in */
/* various places as an index into an array that contains pointers to */
/* CPU-specific implementations of certain functions.  For this reason, */
/* do not change the order of declaration of these enumeration literals. */
/* */
typedef enum
{
    C_IMPL,
    PENTIUM_IMPL,
    PENTIUM_PRO_IMPL,
    MMX_IMPL,
    SSE_IMPL,
    SSE2_IMPL,
    NUMBER_OF_IMPLS
} Implementation_ID;




/* */
/* Define the location of our registry entries. */
/* */
/* In general, the use of registry entries to control the codec's behavior */
/* is discouraged.  However, in a couple particular circumstances a registry */
/* entry is appropriate.  One entry, when present, causes our displayer to */
/* suppress its use of direct draw.  This mechanism is being provided as a */
/* way to work around direct draw problems in the field. */
/* */
/* Other entries exist to enable the display of our Active Movie */
/* property pages, which are suppressed by default. */
/* */
/* Other entries exist to enable the simple compression, and the */
/* use of the encoder's scalability features. */
/* */

/* Define some constants to be used with the CPU registry entries. */
/* These same values are also used in our backdoor "ini" file CPU settings. */
/* As new processors come online, these existing values must not be changed. */
/* These values do not need to coincide with the Implementation_ID enumeration. */

#define Backdoor_HLL_cpu         0 /* High level language (C or C++, e.g.) */
#define Backdoor_IA_cpu          1
#define Backdoor_PPRO_cpu        2
#define Backdoor_MMX_cpu         3
#define Backdoor_SSE_cpu         4
#define Backdoor_SSE2_cpu        5

extern  Implementation_ID Backdoor_CPU_to_Implementation(U32 backdoor_value);
/* Translate a backdoor CPU constant into an Implementation_ID. */
/* Input values exceeding the largest backdoor value, are clipped */
/* to that value. */

extern  U32               Implementation_to_Backdoor_CPU(Implementation_ID);
/* Translate an Implementation_ID to a backdoor CPU constant. */
/* Input values exceeding the largest Implementation_ID, are clipped */
/* to that id. */

/*#if defined(_WIN32) */
#if 0

#define Major_Registry_Key HKEY_LOCAL_MACHINE
extern TCHAR Registry_Key_Path[];
extern TCHAR Disable_Direct_Draw_Registry_Value_Name[];
extern TCHAR Enable_Decoder_Dialogues_Value_Name[];
extern TCHAR Enable_Encoder_Dialogues_Value_Name[];
extern TCHAR Enable_Encoder_Compression_Value_Name[];
extern TCHAR Enable_Encoder_Scalability_Value_Name[];
extern TCHAR Decoder_CPU_Registry_Value_Name[];
extern TCHAR Encoder_CPU_Registry_Value_Name[];

extern TCHAR INI_FILE_NAME[];

extern  void    Adjust_Implementation_From_Registry
(
    Implementation_ID &id,
    Implementation_ID minimum,
    TCHAR name[]
);
/* Given an Implementation_ID "id", and a registry entry, downgrade "id" */
/* if the registry entry exists and indicates a "lesser" CPU. */
/* However, don't downgrade to an Implementation_ID below "minimum". */

#endif /* defined(_WIN32) */

/* Define constants representing the pixel aspect ratio codes from table 5 */
/* of the H.263+ standard. */

#define PARC_FORBIDDEN              0
#define PARC_SQUARE                 1
#define PARC_CIF                    2
#define PARC_10_11                  3
#define PARC_16_11                  4
#define PARC_40_33                  5
/* values between 6 and 14 are reserved */
#define PARC_EXTENDED              15


enum FrameSize      {   FORBIDDEN   = 0, Reserved000    = 0,
                        SQCIF       = 1,
                        QCIF        = 2,
                        CIF         = 3,
                        fCIF        = 4,
                        ssCIF       = 5,
                        CUSTOM      = 6, Reserved110    = 6,
                        EPTYPE      = 7, Reserved111    = 7
                    };




typedef struct
{
    unsigned short StartCodeZeros:16;
    unsigned short StartCode:1;
    unsigned short GN:5;
    unsigned short GLCI:2;
    unsigned short GFID:2;
    unsigned short GQUANT:5;
} T_H263GOBHeader;



#if !defined(CPK) || defined(APK)

#if defined(ARM) || defined(_ARM_)

#define EMIT_EMMS

#elif defined(_M_IX86)

#define EMIT_EMMS __asm emms

#elif defined(__linux) || defined(_MAC_UNIX)

#define EMIT_EMMS __asm("emms")

#else

#error "Don't know how to express emms on this system."

#endif

#else /* !defined(CPK) || defined(APK) */

#define EMIT_EMMS

#endif /* !defined(CPK) || defined(APK) */


#endif /* CDRVDEFS_H__ */
