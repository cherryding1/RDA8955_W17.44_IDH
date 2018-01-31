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



#ifndef _XVID_H_
#define _XVID_H_


#ifdef __cplusplus
extern "C" {
#endif

//#include "yuv420_bilinear_resample.h"
#include "cs_types.h"
/*****************************************************************************
 * versioning
 ****************************************************************************/

/* versioning
    version takes the form "$major.$minor.$patch"
    $patch is incremented when there is no api change
    $minor is incremented when the api is changed, but remains backwards compatible
    $major is incremented when the api is changed significantly

    when initialising an xvid structure, you must always zero it, and set the version field.
        memset(&struct,0,sizeof(struct));
        struct.version = XVID_VERSION;

    XVID_UNSTABLE is defined only during development.
    */

#define XVID_MAKE_VERSION(a,b,c) ((((a)&0xff)<<16) | (((b)&0xff)<<8) | ((c)&0xff))
#define XVID_VERSION_MAJOR(a)    ((int8)(((a)>>16) & 0xff))
#define XVID_VERSION_MINOR(a)    ((int8)(((a)>> 8) & 0xff))
#define XVID_VERSION_PATCH(a)    ((int8)(((a)>> 0) & 0xff))

#define XVID_MAKE_API(a,b)       ((((a)&0xff)<<16) | (((b)&0xff)<<0))
#define XVID_API_MAJOR(a)        (((a)>>16) & 0xff)
#define XVID_API_MINOR(a)        (((a)>> 0) & 0xff)

#define XVID_VERSION             XVID_MAKE_VERSION(1,1,2)
#define XVID_API                 XVID_MAKE_API(4, 1)

/* Bitstream Version
 * this will be writen into the bitstream to allow easy detection of xvid
 * encoder bugs in the decoder, without this it might not possible to
 * automatically distinquish between a file which has been encoded with an
 * old & buggy XVID from a file which has been encoded with a bugfree version
 * see the infamous interlacing bug ...
 *
 * this MUST be increased if an encoder bug is fixed, increasing it too often
 * doesnt hurt but not increasing it could cause difficulty for decoders in the
 * future
 */
#define XVID_BS_VERSION 46

/*****************************************************************************
 * error codes
 ****************************************************************************/

/*  all functions return values <0 indicate error */

#define XVID_ERR_FAIL       -1      /* general fault */
#define XVID_ERR_MEMORY     -2      /* memory allocation error */
#define XVID_ERR_FORMAT     -3      /* file format error */
#define XVID_ERR_VERSION    -4      /* structure version not supported */
#define XVID_ERR_END        -5      /* encoder only; end of stream reached */



/*****************************************************************************
 * xvid_image_t
 ****************************************************************************/

/* colorspace values */

#define XVID_CSP_PLANAR   (1<< 0) /* 4:2:0 planar (==I420, except for pointers/strides) */
#define XVID_CSP_USER     XVID_CSP_PLANAR
#define XVID_CSP_I420     (1<< 1) /* 4:2:0 planar */
#define XVID_CSP_YV12     (1<< 2) /* 4:2:0 planar */
#define XVID_CSP_YUY2     (1<< 3) /* 4:2:2 packed */
#define XVID_CSP_UYVY     (1<< 4) /* 4:2:2 packed */
#define XVID_CSP_YVYU     (1<< 5) /* 4:2:2 packed */
#define XVID_CSP_BGRA     (1<< 6) /* 32-bit bgra packed */
#define XVID_CSP_ABGR     (1<< 7) /* 32-bit abgr packed */
#define XVID_CSP_RGBA     (1<< 8) /* 32-bit rgba packed */
#define XVID_CSP_ARGB     (1<<15) /* 32-bit argb packed */
#define XVID_CSP_BGR      (1<< 9) /* 24-bit bgr packed */
#define XVID_CSP_RGB555   (1<<10) /* 16-bit rgb555 packed */
#define XVID_CSP_RGB565   (1<<11) /* 16-bit rgb565 packed */
#define XVID_CSP_SLICE    (1<<12) /* decoder only: 4:2:0 planar, per slice rendering */
#define XVID_CSP_INTERNAL (1<<13) /* decoder only: 4:2:0 planar, returns ptrs to internal buffers */
#define XVID_CSP_NULL     (1<<14) /* decoder only: dont output anything */
#define XVID_CSP_VFLIP    (1<<31) /* vertical flip mask */

/* xvid_image_t
    for non-planar colorspaces use only plane[0] and stride[0]
    four plane reserved for alpha*/
typedef struct
{
    int csp;                /* [in] colorspace; or with XVID_CSP_VFLIP to perform vertical flip */
    void * plane[4];        /* [in] image plane ptrs */
    int stride[4];          /* [in] image stride; "bytes per row"*/
} xvid_image_t;

/* video-object-sequence profiles */
#define XVID_PROFILE_S_L0    0x08 /* simple */
#define XVID_PROFILE_S_L1    0x01
#define XVID_PROFILE_S_L2    0x02
#define XVID_PROFILE_S_L3    0x03
#define XVID_PROFILE_ARTS_L1 0x91 /* advanced realtime simple */
#define XVID_PROFILE_ARTS_L2 0x92
#define XVID_PROFILE_ARTS_L3 0x93
#define XVID_PROFILE_ARTS_L4 0x94
#define XVID_PROFILE_AS_L0   0xf0 /* advanced simple */
#define XVID_PROFILE_AS_L1   0xf1
#define XVID_PROFILE_AS_L2   0xf2
#define XVID_PROFILE_AS_L3   0xf3
#define XVID_PROFILE_AS_L4   0xf4

/* aspect ratios */
#define XVID_PAR_11_VGA    1 /* 1:1 vga (square), default if supplied PAR is not a valid value */
#define XVID_PAR_43_PAL    2 /* 4:3 pal (12:11 625-line) */
#define XVID_PAR_43_NTSC   3 /* 4:3 ntsc (10:11 525-line) */
#define XVID_PAR_169_PAL   4 /* 16:9 pal (16:11 625-line) */
#define XVID_PAR_169_NTSC  5 /* 16:9 ntsc (40:33 525-line) */
#define XVID_PAR_EXT      15 /* extended par; use par_width, par_height */

/* frame type flags */
#define XVID_TYPE_VOL     -1 /* decoder only: vol was decoded */
#define XVID_TYPE_NOTHING  0 /* decoder only (encoder stats): nothing was decoded/encoded */
#define XVID_TYPE_AUTO     0 /* encoder: automatically determine coding type */
#define XVID_TYPE_IVOP     1 /* intra frame */
#define XVID_TYPE_PVOP     2 /* predicted frame */
#define XVID_TYPE_BVOP     3 /* bidirectionally encoded */
#define XVID_TYPE_SVOP     4 /* predicted+sprite frame */


/*****************************************************************************
 * xvid_global()
 ****************************************************************************/

/* cpu_flags definitions (make sure to sync this with cpuid.asm for ia32) */

#define XVID_CPU_FORCE    (1<<31) /* force passed cpu flags */
#define XVID_CPU_ASM      (1<< 7) /* native assembly */
/* ARCH_IS_IA32 */
#define XVID_CPU_MMX      (1<< 0) /*       mmx : pentiumMMX,k6 */
#define XVID_CPU_MMXEXT   (1<< 1) /*   mmx-ext : pentium2, athlon */
#define XVID_CPU_SSE      (1<< 2) /*       sse : pentium3, athlonXP */
#define XVID_CPU_SSE2     (1<< 3) /*      sse2 : pentium4, athlon64 */
#define XVID_CPU_3DNOW    (1<< 4) /*     3dnow : k6-2 */
#define XVID_CPU_3DNOWEXT (1<< 5) /* 3dnow-ext : athlon */
#define XVID_CPU_TSC      (1<< 6) /*       tsc : Pentium */
/* ARCH_IS_PPC */
#define XVID_CPU_ALTIVEC  (1<< 0) /* altivec */


#define XVID_DEBUG_ERROR     (1<< 0)
#define XVID_DEBUG_STARTCODE (1<< 1)
#define XVID_DEBUG_HEADER    (1<< 2)
#define XVID_DEBUG_TIMECODE  (1<< 3)
#define XVID_DEBUG_MB        (1<< 4)
#define XVID_DEBUG_COEFF     (1<< 5)
#define XVID_DEBUG_MV        (1<< 6)
#define XVID_DEBUG_RC        (1<< 7)
#define XVID_DEBUG_DEBUG     (1<<31)

/* XVID_GBL_INIT param1 */
typedef struct
{
    int version;
    unsigned int cpu_flags; /* [in:opt] zero = autodetect cpu; XVID_CPU_FORCE|{cpu features} = force cpu features */
    int debug;     /* [in:opt] debug level */
} xvid_gbl_init_t;

#define XVID_GBL_INIT    0 /* initialize xvidcore; must be called before using xvid_decore, or xvid_encore) */
#define XVID_GBL_INFO    1 /* return some info about xvidcore, and the host computer */
#define XVID_GBL_CONVERT 2 /* colorspace conversion utility */

extern int xvid_global(void *handle, int opt, void *param1, void *param2);


/*****************************************************************************
 * xvid_decore()
 ****************************************************************************/

#define XVID_DEC_CREATE  0 /* create decore instance; return 0 on success */
#define XVID_DEC_DESTROY 1 /* destroy decore instance: return 0 on success */
#define XVID_DEC_DECODE  2 /* decode a frame: returns number of bytes consumed >= 0 */

extern int xvid_decore(void *handle, int opt, void *param1, void *param2);

/* XVID_DEC_CREATE param 1
    image width & height may be specified here when the dimensions are
    known in advance. */
typedef struct
{
    int version;
    int width;     /* [in:opt] image width */
    int height;    /* [in:opt] image width */
    void * handle; /* [out]    decore context handle */
} xvid_dec_create_t;


/* XVID_DEC_DECODE param1 */
/* general flags */
#define XVID_LOWDELAY      (1<<0) /* lowdelay mode  */
#define XVID_DISCONTINUITY (1<<1) /* indicates break in stream */
#define XVID_DEBLOCKY      (1<<2) /* perform luma deblocking */
#define XVID_DEBLOCKUV     (1<<3) /* perform chroma deblocking */
#define XVID_FILMEFFECT    (1<<4) /* adds film grain */
#define XVID_DERINGUV      (1<<5) /* perform chroma deringing, requires deblocking to work */
#define XVID_DERINGY       (1<<6) /* perform luma deringing, requires deblocking to work */

#define XVID_DEC_FAST      (1<<29) /* disable postprocessing to decrease cpu usage *todo* */
#define XVID_DEC_DROP      (1<<30) /* drop bframes to decrease cpu usage *todo* */
#define XVID_DEC_PREROLL   (1<<31) /* decode as fast as you can, don't even show output *todo* */

typedef struct
{
    int version;
    int general;         /* [in:opt] general flags */
    void *bitstream;     /* [in]     bitstream (read from)*/
    int length;          /* [in]     bitstream length */
    xvid_image_t output; /* [in]     output image (written to) */
    /* ------- v1.1.x ------- */
    int brightness;      /* [in]     brightness offset (0=none) */
} xvid_dec_frame_t;

/* XVID_DEC_DECODE param2 :: optional */
typedef struct
{
    int version;

    int type;                   /* [out] output data type */
    union
    {
        struct   /* type>0 {XVID_TYPE_IVOP,XVID_TYPE_PVOP,XVID_TYPE_BVOP,XVID_TYPE_SVOP} */
        {
            int general;        /* [out] flags */
            int time_base;      /* [out] time base */
            int time_increment; /* [out] time increment */

            /* XXX: external deblocking stuff */
            //      int * qscale;       /* [out] pointer to quantizer table */
            int qscale_stride;  /* [out] quantizer scale stride */

        } vop;
        struct      /* XVID_TYPE_VOL */
        {
            int general;        /* [out] flags */
            int width;          /* [out] width */
            int height;         /* [out] height */
            int edged_width;
            int edged_height;
            int par;            /* [out] pixel aspect ratio (refer to XVID_PAR_xxx above) */
            int par_width;      /* [out] aspect ratio width  [1..255] */
            int par_height;     /* [out] aspect ratio height [1..255] */
        } vol;
    } data;
} xvid_dec_stats_t;


#ifdef __cplusplus
}
#endif

#endif

