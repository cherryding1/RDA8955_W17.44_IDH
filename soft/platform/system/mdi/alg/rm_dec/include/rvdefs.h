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
































































#if defined(DEFRVFMT)

DEFRVFMT(RV_FID_UNDEFINED,     0, 0, 0,
         FOURCC_UNDEFINED,      0,
         MEDIASUBTYPE_NULL)
/* an uninitialized color format 0*/

DEFRVFMT(RV_FID_RGB4,          4, 1, 0,
         BI_RGB,                0,
         MEDIASUBTYPE_RGB4)
/* 16 level gray scale */

DEFRVFMT(RV_FID_CLUT8,         8, 1, 0,
         BI_RGB,                0,
         MEDIASUBTYPE_RGB8)
/* 8-bit color lookup table */

DEFRVFMT(RV_FID_XRGB16_1555,  16, 1, 0,
         BI_RGB,                0,
         MEDIASUBTYPE_555)
/* 1 unused, 5-bits each red, green, blue */

DEFRVFMT(RV_FID_RGB16_565,    16, 1, 0,
         BI_BITFIELDS,          0,
         MEDIASUBTYPE_565)
/* 16-bit color variant */

DEFRVFMT(RV_FID_RGB16_655,    16, 1, 0,
         BI_BITFIELDS,          0,
         MEDIASUBTYPE_NULL)
/* 16-bit color variant */

DEFRVFMT(RV_FID_RGB16_664,    16, 1, 0,
         BI_BITFIELDS,          0,
         MEDIASUBTYPE_NULL)
/* 16-bit color variant */

DEFRVFMT(RV_FID_RGB24,        24, 1, 0,
         BI_RGB,                0,
         MEDIASUBTYPE_RGB24)
/* 8-bits each: red, green, blue 7 */

DEFRVFMT(RV_FID_XRGB32,       32, 1, 0,
         BI_RGB,                0,
         MEDIASUBTYPE_RGB32)
/* 8-bits each: blue, green, red */

DEFRVFMT(RV_FID_MPEG2V,       24, 0, 0,
         FOURCC_MPEG2V,         1,
         MEDIASUBTYPE_MPEG2_VIDEO)

DEFRVFMT(RV_FID_YVU9,          9, 0, 1,
         FOURCC_YVU9,           1,
         MEDIASUBTYPE_YVU9)

DEFRVFMT(RV_FID_YUV12,        12, 0, 1,
         FOURCC_YUV12,          1,
         AM_MEDIASUBTYPE_I420)
/* Also known as I420 */
/* Y-plane comes first, followed by U-plane then V-plane */

DEFRVFMT(RV_FID_IYUV,         12, 0, 1,
         FOURCC_IYUV,           1,
         AM_MEDIASUBTYPE_IYUV)
/* This format has the same representation as RV_FID_YUV12 */

DEFRVFMT(RV_FID_YV12,         12, 0, 1,
         FOURCC_YV12,           1,
         AM_MEDIASUBTYPE_YV12)
/* Like RV_FID_YUV12, except order of V and U planes is swapped */

DEFRVFMT(RV_FID_YUY2,         16, 0, 0,
         FOURCC_YUY2,           1,
         MEDIASUBTYPE_YUY2)

DEFRVFMT(RV_FID_BITMAP16,     16, 1, 0,
         BI_BITMAP,             0,
         MEDIASUBTYPE_NULL)

DEFRVFMT(RV_FID_H261,         24, 0, 0,
         FOURCC_H261,           1,
         AM_MEDIASUBTYPE_H261)

DEFRVFMT(RV_FID_H263,         24, 0, 0,
         FOURCC_H263,           1,
         AM_MEDIASUBTYPE_H263)     /* 17 */

DEFRVFMT(RV_FID_H263PLUS,     24, 0, 0,
         FOURCC_H263PLUS,       1,
         AM_MEDIASUBTYPE_H263PLUS)

DEFRVFMT(RV_FID_RV89COMBO,     24, 0, 0,
         FOURCC_RV89COMBO,       1,
         AM_MEDIASUBTYPE_RV89COMBO)
/* Temporary FID to use for RV89COMBO encoder */

DEFRVFMT(RV_FID_ILVR,         24, 0, 0,
         FOURCC_ILVR,           1,
         AM_MEDIASUBTYPE_ILVR)
/* ILVR is Intel's H.263+ format that includes IETF RTP */
/* bitstream extensions which assist in network packetization. */

DEFRVFMT(RV_FID_REALVIDEO21,   24, 0, 0,
         FOURCC_UNDEFINED,      1,
         MEDIASUBTYPE_NULL)
/* Represents the RealVideo 2 compression format, version 2.1 */
/* The actual bit stream format might change periodically, */
/* as RealVideo is enhanced.  To prevent version problems, */
/* the RealVideo RealVideo takes care of negotiating with the */
/* application that the appropriate version of a RealVideo 2 */
/* bitstream version is in fact being used. */
DEFRVFMT(RV_FID_REALVIDEO22,   24, 0, 0,
         FOURCC_UNDEFINED,      1,
         MEDIASUBTYPE_NULL)
/* Represents the RealVideo 2 compression format, version 2.2 */

DEFRVFMT(RV_FID_REALVIDEO30,   24, 0, 0,
         FOURCC_UNDEFINED,      1,
         MEDIASUBTYPE_NULL)           /* 23 */
/* Represents the future RealVideo compression format, version 3.0 */

#undef  DEFRVFMT

#endif /* DEFRVFMT */



/* DEFRVSTATUS lists the error codes returned by HIVE/RV methods, and */
/* identifies a corresponding error code for each video environment. */
/* The mapping from a RV status to an environment specific status always */
/* gives an appropriate value, but the reverse is not always true. */
/* */
/* The first argument will be used to define an enumeration type that */
/* lists all the HIVE/RV status codes. */
/* */
/* The second argument gives the closest corresponding Video-for-Windows */
/* error code. */
/* */
/* The third argument gives the closest corresponding Active Movie */
/* error code. */
/* */
/* The fourth argument gives the closest corresponding RealVideo */
/* error code. */

#if defined(DEFRVSTATUS)

/* Everything is a-okay. */
DEFRVSTATUS( RV_S_OK
             , ICERR_OK
             , S_OK
             , HXR_OK
           )

/* A non-specific error. */
DEFRVSTATUS( RV_S_ERROR
             , ICERR_ERROR
             , E_FAIL
             , HXR_FAIL
           )

/* A memory allocation request failed */
DEFRVSTATUS( RV_S_OUT_OF_MEMORY
             , ICERR_MEMORY
             , E_OUTOFMEMORY
             , HXR_OUTOFMEMORY
           )

/* An unsupported operation or feature was requested. */
DEFRVSTATUS( RV_S_UNSUPPORTED
             , ICERR_UNSUPPORTED
             , E_NOTIMPL
             , HXR_NOTIMPL
           )

/* User does not have the privileges to use a requested feature. */
DEFRVSTATUS( RV_S_ACCESS_DENIED
             , ICERR_UNSUPPORTED
             , E_ACCESSDENIED
             , HXR_ACCESSDENIED
           )

/* A supported operation or feature was requested, but is not */
/* available in the current state. */
DEFRVSTATUS( RV_S_UNAVAILABLE
             , ICERR_UNSUPPORTED
             , E_ACCESSDENIED
             , HXR_ACCESSDENIED
           )

/* An unexpected NULL parameter was seen. */
DEFRVSTATUS( RV_S_NULL_PARAM
             , ICERR_BADPARAM
             , E_INVALIDARG
             , HXR_POINTER
           )

/* The image dimensions are not supported. */
DEFRVSTATUS( RV_S_BAD_DIMENSIONS
             , ICERR_BADIMAGESIZE
             , E_INVALIDARG
             , HXR_INVALID_PARAMETER
           )

/* The image format is not supported. */
DEFRVSTATUS( RV_S_BAD_FORMAT
             , ICERR_BADFORMAT
             , VFW_E_INVALIDMEDIATYPE
             , HXR_BAD_FORMAT
           )

/* The requested functionality is not supported on the current CPU. */
DEFRVSTATUS( RV_S_BAD_CPU
             , ICERR_UNSUPPORTED
             , VFW_E_PROCESSOR_NOT_SUITABLE
             , HXR_NOTIMPL
           )

/* A numerical parameter was not within a specified range of legal values. */
DEFRVSTATUS( RV_S_OUT_OF_RANGE
             , ICERR_BADPARAM
             , E_INVALIDARG
             , HXR_INVALID_PARAMETER
           )

/* The image is not prepared for display. */
DEFRVSTATUS( RV_S_DONT_DRAW
             , ICERR_DONTDRAW
             , S_FALSE
             , HXR_FAIL   /* Sorry, there is no such concept in RealVideo. */
           )

/* skip and not decode a B frame. sheen */
DEFRVSTATUS( RV_S_SKIP_BFRAME
             , 0
             , 0
             , HXR_SKIP_BFRAME
           )

#undef  DEFRVSTATUS

#endif
