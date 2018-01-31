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


#ifndef SHEEN_VC_DEBUG
#include "global.h"
#endif

#ifdef SHEEN_VC_DEBUG
typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;

typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;

typedef char bool;
typedef char zoomPar;
#define mmc_MemMalloc malloc
#define mmc_MemGetFree() 1024*1024
#ifndef ABS
#define ABS abs
#endif
#define MICRON_ALIGN(a)
#endif

#ifdef MEDIA_VOCVID_SUPPORT
#ifndef SHEEN_VC_DEBUG
#define RAM_DCT3D//put table in ram or flash. not use in C model.
#endif
#endif

#ifdef MP4_3GP_SUPPORT

#ifndef _MPEG4_GLOBAL_H_
#define _MPEG4_GLOBAL_H_

#include "xvid.h"
#include "portab.h"
//add by longyl
#define longyl
//#define _BIG_BUFFER_ADJUST_
/////////////////////////
/* --- macroblock modes --- */

#define MODE_INTER      0
#define MODE_INTER_Q    1
#define MODE_INTER4V    2
#define MODE_INTRA      3
#define MODE_INTRA_Q    4
#define MODE_NOT_CODED  16
/*
 * vop coding types
 * intra, prediction, backward, sprite, not_coded
 */
#define I_VOP   0
#define P_VOP   1
#define B_VOP   2
#define S_VOP   3
#define N_VOP   4
/////////
/* --- bframe specific --- */

#define MODE_DIRECT         0
#define MODE_INTERPOLATE    1
#define MODE_BACKWARD       2
#define MODE_FORWARD        3
#define MODE_DIRECT_NONE_MV 4
#define MODE_DIRECT_NO4V    5
/* --- macroblock modes --- */

#define MODE_INTER      0
#define MODE_INTER_Q    1
#define MODE_INTER4V    2
#define MODE_INTRA      3
#define MODE_INTRA_Q    4
#define MODE_NOT_CODED  16
#define MODE_NOT_CODED_GMC  17


/* convert mpeg-4 coding type i/p/b/s_VOP to XVID_TYPE_xxx */
static __inline int
coding2type(int coding_type)
{
    return coding_type + 1;
}

/* convert XVID_TYPE_xxx to bitstream coding type i/p/b/s_VOP */
static __inline int
type2coding(int xvid_type)
{
    return xvid_type - 1;
}


typedef struct
{
    int x;
    int y;
}
VECTOR;

typedef struct
{
    VECTOR mvs[4];
}
VECTOR4;//add yh;

typedef struct
{
    VECTOR duv[3];
}
WARPPOINTS;//add yh;

typedef struct _NEW_GMC_DATA
{
    /*  0=none, 1=translation, 2,3 = warping
    *  a value of -1 means: "structure not initialized!" */
    int num_wp;

    /* {0,1,2,3}  =>   {1/2,1/4,1/8,1/16} pel */
    int accuracy;

    /* sprite size * 16 */
    int sW, sH;

    /* gradient, calculated from warp points */
    int dU[2], dV[2], Uo, Vo, Uco, Vco;

    void (*predict_16x16)(const struct _NEW_GMC_DATA * const This,
                          uint8_t *dst, const uint8_t *src,
                          int dststride, int srcstride, int x, int y, int rounding);
    void (*predict_8x8)  (const struct _NEW_GMC_DATA * const This,
                          uint8_t *uDst, const uint8_t *uSrc,
                          uint8_t *vDst, const uint8_t *vSrc,
                          int dststride, int srcstride, int x, int y, int rounding);
    void (*get_average_mv)(const struct _NEW_GMC_DATA * const Dsp, VECTOR * const mv,
                           int x, int y, int qpel);
} NEW_GMC_DATA;//add yh

typedef struct
{
    uint8_t *y;
    uint8_t *u;
    uint8_t *v;
}
IMAGE;

typedef struct
{
    uint32_t buf;
    uint16_t pos;//left bits num in buf,diff with original code.sheen
    uint8_t *tail;
    uint8_t *start;
    uint32_t length;
    uint32_t initpos;
}
Bitstream;

/* useful macros */
//#define MIN(X, Y) ((X)<(Y)?(X):(Y))

#define MIN_16(X, Y) ((Y) + (((X-Y)>>15)&((X)-(Y))))
#define MIN_32C(X, Y) ((Y) + (((X-Y)>>31)&((X)-(Y))))

//#define MAX(X, Y) ((X)>(Y)?(X):(Y))
#define MAX_16(X, Y) ((Y) + (((Y-X)>>15)&((X)-(Y))))
#define MAX_32C(X, Y) ((Y) + (((Y-X)>>31)&((X)-(Y))))



#define SIGN(X)   (((X)>0)?1:-1)
#define CLIP(X,AMIN,AMAX)   (((X)<(AMIN)) ? (AMIN) : ((X)>(AMAX)) ? (AMAX) : (X))
#define SWAP(_T_,A,B)    { _T_ tmp = A; A = B; B = tmp; }


#endif                          /* _GLOBAL_H_ */
#endif

