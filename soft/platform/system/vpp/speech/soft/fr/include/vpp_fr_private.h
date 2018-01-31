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





#ifndef PRIVATE_H
#define PRIVATE_H

#include <string.h>
#include <stdlib.h>
#include "cs_types.h"

//#define USE_FLOAT_MUL /////////////////// HACK ATTACK ////////////////
//#define int long
#define SASR

//#define Word16 short
//#define Word32 long

//typedef short                  INT16;           /* 16 bit signed int    */
//typedef int                    INT32;       /* 32 bit signed int    */



//typedef unsigned short          UINT16;          /* unsigned INT16        */
//typedef unsigned int            UINT32;      /* unsigned INT32    */

struct gsm_state
{

    INT16            dp0[ 280 ];

    INT16            z1;             /* preprocessing.c, Offset_com. */
    INT32        L_z2;           /*                  Offset_com. */
    int             mp;             /*                  Preemphasis */

    INT16            u[8];           /* short_term_aly_filter.c      */
    INT16            LARpp[2][8];    /*                              */
    INT16            j;              /*                              */

    INT16            nrp; /* 40 */   /* long_term.c, synthesis       */
    INT16            v[9];           /* short_term.c, synthesis      */
    INT16            msr;            /* decoder.c,   Postprocessing  */

    char            verbose;        /* only used if !NDEBUG         */
    char            fast;           /* only used if FAST            */

};


//fts add
#define GSM_MAX(a,b)  ((a > b) ? a : b)
#define GSM_MIN(a,b)  ((a < b) ? a : b)

//add end

#define MIN_WORD        (-32768)
#define MAX_WORD        ( 32767)

#define MIN_LONGWORD    ((-2147483647)-1)
#define MAX_LONGWORD    ( 2147483647)

#ifdef  SASR            /* >> is a signed arithmetic shift right */
#undef  SASR
#define SASR(x, by)     ((x) >> (by))
#endif  /* SASR */


#include "vpp_fr_proto.h"

/*
 *      Prototypes from add.c
 */
extern INT16     gsm_mult        P((INT16 a, INT16 b));
//extern INT32 gsm_L_mult      P((INT16 a, INT16 b));
//extern INT16     gsm_mult_r      P((INT16 a, INT16 b));

extern INT16     gsm_div         P((INT16 num, INT16 denum));

//extern INT16     gsm_add         P(( INT16 a, INT16 b ));
//extern INT32 gsm_L_add       P(( INT32 a, INT32 b ));

extern INT16     gsm_sub         P((INT16 a, INT16 b));
extern INT32 gsm_L_sub       P((INT32 a, INT32 b));

//extern INT16     gsm_abs         P((INT16 a));

extern INT16     gsm_norm        P(( INT32 a ));

extern INT32 gsm_L_asl       P((INT32 a, int n));
extern INT16     gsm_asl         P((INT16 a, int n));

extern INT32 gsm_L_asr       P((INT32 a, int n));
extern INT16     gsm_asr         P((INT16 a, int n));

/*
 *  Inlined functions from add.h
 */
//#define INLINE_ADD_H_FUNCTIONS

//#ifdef INLINE_ADD_H_FUNCTIONS

#define GSM_L_MULT(a, b)                                  (((INT32)(a) * (INT32)(b)) << 1)

#define GSM_MULT_R(a, b) /* INT16 a, INT16 b, !(a == b == MIN_WORD) */    \
        ((INT16) (SASR( ((INT32)(a) * (INT32)(b) + 16384), 15 )))

#define GSM_SAT_MULT_R(a,b) \
     (b == MIN_WORD && a == MIN_WORD)? MAX_WORD:\
     ((INT16) (SASR( ((INT32)(a) * (INT32)(b) + 16384), 15 ) & 0xFFFF))

# define GSM_MULT(a,b)   /* INT16 a, INT16 b, !(a == b == MIN_WORD) */    \
        (SASR( ((INT32)(a) * (INT32)(b)), 15 ))

#define GSM_SATMULT(a, b)\
          (((INT16)a) == MIN_WORD && ((INT16)b) == MIN_WORD)? MAX_WORD:(INT16)(SASR( (INT32)a * (INT32)b, 15 ))

#define GSM_L_ADD(a, b)        \
        ( (a) <  0 ? ( (b) >= 0 ? (a) + (b)     \
                 : (utmp = (UINT32)-((a) + 1) + (UINT32)-((b) + 1)) \
                   >= MAX_LONGWORD ? MIN_LONGWORD : -(INT32)utmp-2 )   \
        : ((b) <= 0 ? (a) + (b)   \
                  : (utmp = (UINT32)(a) + (UINT32)(b)) >= MAX_LONGWORD \
                    ? MAX_LONGWORD : utmp))


#define GSM_ADD(a,b)                               (a+b)

#define GSM_SATADD(a, b)   \
        ((unsigned int)((ltmp = (INT32)(a) + (INT32)(b)) - MIN_WORD) > \
                MAX_WORD - MIN_WORD ? (ltmp > 0 ? MAX_WORD : MIN_WORD) : ltmp)

#define GSM_SUB(a,b)                               (a-b)

#define GSM_SATSUB(a, b)  \
        ((unsigned int)((ltmp = (INT32)(a) - (INT32)(b)) >= MAX_WORD \
        ? MAX_WORD : ltmp <= MIN_WORD ? MIN_WORD : ltmp))

#define GSM_ABS(a)     ((a) < 0 ? ((a) == MIN_WORD ? MAX_WORD : -(a)) : (a))




#ifdef TARGET_SIMU


#   define GSM_MLX(hi, lo, x, y)  \
    {\
    register int __x = (int)(x); \
    register int __y = (int)(y); \
    asm ("imull %3"  \
     : "=a" (lo), "=d" (hi)  \
     : "%a" (__x), "rm" (__y)  \
     : "cc");\
    }

#   define GSM_MLA(hi, lo, x, y)  \
    ({ INT32 __hi;  \
       UINT32 __lo;  \
       GSM_MLX(__hi, __lo, (x), (y));  \
       asm ("addl %2,%0\n\t"  \
        "adcl %3,%1"  \
        : "=rm" (lo), "=rm" (hi)  \
        : "r" (__lo), "r" (__hi), "0" (lo), "1" (hi)  \
        : "cc");  \
    })

#   define GSM_MLA16(hi, lo, x, y)        GSM_MLA(hi, lo, x, y)
#   define GSM_MLX16(hi, lo, x, y)        GSM_MLX(hi, lo, x, y)


#else

#   define GSM_MLX(hi, lo, x, y)  \
    asm ("mult	%2,%3"  \
     : "=l" (lo), "=h" (hi)  \
     : "%r" (x), "r" (y))

#   define GSM_MLX16(hi, lo, x, y)  \
    ({ \
    register int __x = (int)x; \
    register int __y = (int)y; \
    asm ("mult	%2,%3"  \
     : "=l" (lo), "=h" (hi)  \
     : "%r" (__x), "r" (__y));\
})

#   define GSM_MLA(hi, lo, x, y)  \
    asm ("madd	%2,%3"  \
     : "+l" (lo), "+h" (hi)  \
     : "%r" (x), "r" (y))

#   define GSM_MLA16(hi, lo, x, y)  \
    ({ \
    register int __x = (int)x; \
    register int __y = (int)y; \
    asm ("madd	%2,%3"  \
     : "+l" (lo), "+h" (hi)  \
     : "%r" (__x), "r" (__y)); \
    })

#endif

/*
 *  More prototypes from implementations..
 */
extern void Gsm_Coder P((
                            struct gsm_state        * S,
                            INT16    * s,    /* [0..159] samples             IN      */
                            INT16    * LARc, /* [0..7] LAR coefficients      OUT     */
                            INT16    * Nc,   /* [0..3] LTP lag               OUT     */
                            INT16    * bc,   /* [0..3] coded LTP gain        OUT     */
                            INT16    * Mc,   /* [0..3] RPE grid selection    OUT     */
                            INT16    * xmaxc,/* [0..3] Coded maximum amplitude OUT   */
                            INT16    * xMc   /* [13*4] normalized RPE samples OUT    */));

extern void Gsm_Long_Term_Predictor P((         /* 4x for 160 samples */
        struct gsm_state * S,
        INT16    * d,    /* [0..39]   residual signal    IN      */
        INT16    * dp,   /* [-120..-1] d'                IN      */
        INT16    * e,    /* [0..40]                      OUT     */
        INT16    * dpp,  /* [0..40]                      OUT     */
        INT16    * Nc,   /* correlation lag              OUT     */
        INT16    * bc    /* gain factor                  OUT     */));

extern void Gsm_LPC_Analysis P((
                                   struct gsm_state * S,
                                   INT16 * s,        /* 0..159 signals      IN/OUT  */
                                   INT16 * LARc));   /* 0..7   LARc's       OUT     */

extern void Gsm_Preprocess P((
                                 struct gsm_state * S,
                                 INT16 * s, INT16 * so));

extern void Gsm_Encoding P((
                               struct gsm_state * S,
                               INT16    * e,
                               INT16    * ep,
                               INT16    * xmaxc,
                               INT16    * Mc,
                               INT16    * xMc));

extern void Gsm_Short_Term_Analysis_Filter P((
            struct gsm_state * S,
            INT16    * LARc, /* coded log area ratio [0..7]  IN      */
            INT16    * d     /* st res. signal [0..159]      IN/OUT  */));

extern void Gsm_Decoder P((
                              struct gsm_state * S,
                              INT16    * LARcr,        /* [0..7]               IN      */
                              INT16    * Ncr,          /* [0..3]               IN      */
                              INT16    * bcr,          /* [0..3]               IN      */
                              INT16    * Mcr,          /* [0..3]               IN      */
                              INT16    * xmaxcr,       /* [0..3]               IN      */
                              INT16    * xMcr,         /* [0..13*4]            IN      */
                              INT16    * s));          /* [0..159]             OUT     */

extern void Gsm_Decoding P((
                               struct gsm_state * S,
                               INT16    xmaxcr,
                               INT16    Mcr,
                               INT16    * xMcr,         /* [0..12]              IN      */
                               INT16    * erp));        /* [0..39]              OUT     */

extern void Gsm_Long_Term_Synthesis_Filtering P((
            struct gsm_state* S,
            INT16    Ncr,
            INT16    bcr,
            INT16    * erp,          /* [0..39]                IN    */
            INT16    * drp));        /* [-120..-1] IN, [0..40] OUT   */

extern void Gsm_Short_Term_Synthesis_Filter P((
            struct gsm_state * S,
            INT16    * LARcr,        /* log area ratios [0..7]  IN   */
            INT16    * drp,          /* received d [0...39]     IN   */
            INT16    * s));          /* signal   s [0..159]    OUT   */

extern void Gsm_Update_of_reconstructed_short_time_residual_signal P((
            INT16    * dpp,          /* [0...39]     IN      */
            INT16    * ep,           /* [0...39]     IN      */
            INT16    * dp));         /* [-120...-1]  IN/OUT  */

extern void Gsm_RPE_Encoding P((struct gsm_state * S,
                                INT16   * e,            /* -5..-1][0..39][40..44        IN/OUT  */
                                INT16   * xmaxc,        /*                              OUT */
                                INT16   * Mc,           /*                              OUT */
                                INT16   * xMc));            /* [0..12]                      OUT */

extern void Gsm_RPE_Decoding P((struct gsm_state        * S,
                                INT16           xmaxcr,
                                INT16           Mcr,
                                INT16           * xMcr,  /* [0..12], 3 bits             IN      */
                                INT16           * erp    /* [0..39]                     OUT     */
                               ));


/*
 *  Tables from table.c
 */
#ifndef GSM_TABLE_C

extern INT16 gsm_A[8], gsm_B[8], gsm_MIC[8], gsm_MAC[8];
extern INT16 gsm_INVA[8];
extern INT16 gsm_DLB[4], gsm_QLB[4];
extern INT16 gsm_H[11];
extern INT16 gsm_NRFAC[8];
extern INT16 gsm_FAC[8];

#endif  /* GSM_TABLE_C */

/*
 *  Debugging
 */
#ifdef NDEBUG

#       define  gsm_debug_words(a, b, c, d)             /* nil */
#       define  gsm_debug_longwords(a, b, c, d)         /* nil */
#       define  gsm_debug_word(a, b)                    /* nil */
#       define  gsm_debug_longword(a, b)                /* nil */

#else   /* !NDEBUG => DEBUG */

extern void  gsm_debug_words     P((char * name, int, int, INT16 *));
extern void  gsm_debug_longwords P((char * name, int, int, INT32 *));
extern void  gsm_debug_longword  P((char * name, INT32));
extern void  gsm_debug_word      P((char * name, INT16));

#endif /* !NDEBUG */

#include "vpp_fr_unproto.h"

#endif  /* PRIVATE_H */
