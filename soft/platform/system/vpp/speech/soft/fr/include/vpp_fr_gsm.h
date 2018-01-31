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





#ifndef GSM_H
#define GSM_H

#include "hal_speech.h"

#define  NeedFunctionPrototypes  1

#ifdef __cplusplus
#       define  NeedFunctionPrototypes  1
#endif

#if __STDC__
#       define  NeedFunctionPrototypes  1
#endif

#ifdef _NO_PROTO
#       undef   NeedFunctionPrototypes
#endif

#undef  GSM_P   /* gnu stdio.h actually defines this...         */

#if NeedFunctionPrototypes
#       define  GSM_P( protos ) protos
#else
#       define  GSM_P( protos ) ( /* protos */ )
#endif

//#ifdef NeedFunctionPrototypes
#   include     <stdio.h>               /* for FILE *   */
//#endif

/* AUTO_SPARC_HACK added by John Walker.  If defined, USE_FLOAT_MUL
   automatically set when compiling on a Sparc.  This lets most
   people avoid editing the Makefile. */

#ifdef AUTO_SPARC_HACK
#ifdef sparc
#define USE_FLOAT_MUL
#define FAST
#endif
#endif

/*
 *      Interface
 */

typedef struct gsm_state *      gsm;
typedef short                   gsm_signal;             /* signed 16 bit */
typedef unsigned char           gsm_byte;
typedef gsm_byte                gsm_frame[33];          /* 33 * 8 bits   */

#define GSM_MAGIC       0xD                             /* 13 kbit/s RPE-LTP */

#define GSM_PATCHLEVEL  2
#define GSM_MINOR       0
#define GSM_MAJOR       1

#define GSM_OPT_VERBOSE 1
#define GSM_OPT_FAST    2

extern void  gsm_create  GSM_P((void));
//extern void gsm_destroy GSM_P((gsm));

//extern int  gsm_print   GSM_P((FILE *, gsm, gsm_byte  *));
extern int  gsm_option  GSM_P((gsm, long, long *));

//extern void gsm_encode  GSM_P((gsm_signal *, gsm_byte  *));
extern void gsm_encode GSM_P((HAL_SPEECH_PCM_HALF_BUF_T source, HAL_SPEECH_ENC_OUT_T* c));
extern int  gsm_decode  GSM_P((HAL_SPEECH_DEC_IN_T* DecIn, HAL_SPEECH_PCM_HALF_BUF_T DecOut));

extern int  gsm_explode GSM_P((gsm, gsm_byte   *, gsm_signal *));
extern void gsm_implode GSM_P((gsm, gsm_signal *, gsm_byte   *));

#undef  GSM_P

#endif  /* GSM_H */
