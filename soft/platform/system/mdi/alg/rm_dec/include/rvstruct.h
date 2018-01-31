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
































#ifndef RVSTRUCT_H__
#define RVSTRUCT_H__


#include "rvtypes.h"
#include "rvfid.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RV_Dimensions
{
    U32     width;
    U32     height;
};

typedef enum
{
    INTRAPIC    = 0,
    INTERPIC    = 1,
    TRUEBPIC    = 2,
    FRUPIC      = 3
} EnumPicCodType;

#define FID_I420  100
#define FID_YUV12 120
#define FID_IYUV  140
#define FID_NOTSUPPORTED 999

/* Macroblock type definitions */
/* Keep these ordered such that intra types are first, followed by */
/* inter types.  Otherwise you'll need to change the definitions */
/* of IS_INTRA_MBTYPE and IS_INTER_MBTYPE. */
/* */
/* WARNING:  Because the decoder exposes macroblock types to the application, */
/* these values cannot be changed without affecting users of the decoder. */
/* If new macro block types need to be inserted in the middle of the list, */
/* then perhaps existing types should retain their numeric value, the new */
/* type should be given a new value, and for coding efficiency we should */
/* perhaps decouple these values from the ones that are encoded in the */
/* bitstream. */
/* */
/* */

typedef enum
{
    MBTYPE_INTRA,           /* 0 */
    MBTYPE_INTRA_16x16,     /* 1 */
    MBTYPE_INTER,           /* 2 */
    MBTYPE_INTER_4V,        /* 3 */
    MBTYPE_FORWARD,         /* 4 */
    MBTYPE_BACKWARD,        /* 5 */
    MBTYPE_SKIPPED,         /* 6 */
    MBTYPE_DIRECT,          /* 7 */
    MBTYPE_INTER_16x8V,     /* 8 */
    MBTYPE_INTER_8x16V,     /* 9  */
    MBTYPE_BIDIR,           /* 10 */
    MBTYPE_INTER_16x16,     /* 11 */
#ifdef INTERLACED_CODE
    MBTYPE_INTER_16x16i,    // 12
    MBTYPE_INTER_16x8i,     // 13
    MBTYPE_INTER_8x8i,      // 14
    MBTYPE_FORWARD_16x16i,   //15
    MBTYPE_BACKWARD_16x16i,  //16
    MBTYPE_FORWARD_16x8i,    //17
    MBTYPE_BACKWARD_16x8i,   //18
#endif
    NUMBER_OF_MBTYPES       /* 19 */
} MB_Type;


/* macro - yields TRUE if a given MB type is INTRA */
#define IS_INTRA_MBTYPE(mbtype) ((mbtype) < MBTYPE_INTER)

/* macro - yields TRUE if a given MB type is INTER */
#define IS_INTER_MBTYPE(mbtype) ((mbtype) >= MBTYPE_INTER)

/* macro - yields TRUE if a given MB type is INTERLACED */
#define IS_INTERLACE_MBTYPE(mbtype) ((mbtype) >= MBTYPE_INTER_16x16i)

/* macro - yields TRUE if a given MB type is INTERLACED & B Frame type*/
#define IS_INTERLACE_B_MBTYPE(mbtype) ((mbtype) >= MBTYPE_FORWARD_16x16i)

#define IS_FORWARD_OR_BIDIR(mbtype)(((mbtype) == MBTYPE_FORWARD) || \
                                    ((mbtype) == MBTYPE_FORWARD_16x16i) || \
                                    ((mbtype) == MBTYPE_FORWARD_16x8i) || \
                                    ((mbtype) == MBTYPE_BIDIR))

#define IS_BACKWARD_OR_BIDIR(mbtype)(((mbtype) == MBTYPE_BACKWARD) || \
                                     ((mbtype) == MBTYPE_BACKWARD_16x16i) || \
                                     ((mbtype) == MBTYPE_BACKWARD_16x8i) || \
                                     ((mbtype) == MBTYPE_BIDIR))



/* Reference Picture Resampling Parameter (RPRP) definitions */

/* RPR displacement parameters */
/* Each field contains a luma edge displacement in 1/2 pel units. The */
/* displacements are interpreted as the edge displacements to be applied */
/* to the reference frame independent of any resizing. Each edge displacement */
/* is independent of the others, as opposed to the RPR warping parameters */
/* defined in Annex P. */
/* The relationship between the warping parameters and these edge parameters */
/* is: */
/* iDeltaLeft = wx0 */
/* iDeltaRight = wx0 + wxx */
/* iDeltaTop = wy0 */
/* iDeltaBottom = wy0 + wyy */
typedef struct
{
    I32 iDeltaLeft;
    I32 iDeltaRight;
    I32 iDeltaTop;
    I32 iDeltaBottom;
} T_RPR_EdgeParams;

typedef struct YUVFrame
{
    /* Pointers to Y, V, and U planes */
    U8 *pYPlane;
    U8 *pVPlane;
    U8 *pUPlane;
} T_YUVFrame;


/* Warning: The FillMode enum value assignments here match the bit assignments */
/* used in the bitstream and therefore must not be changed. */
enum EnumRPRFillModeType
{
    FILL_COLOR=0,
    FILL_BLACK=1,
    FILL_GRAY=2,
    FILL_CLIP=3
};

/* RPR fill mode parameter */
typedef struct
{
    enum EnumRPRFillModeType uMode;
    U8 uColor [3];      /* Y, Cb, Cr fill color: only valid when uMode == COLOR */
} T_RPR_FillParam;


typedef struct
{
    RV_Boolean          bParamsPresent; /* true if following params in header */
    T_RPR_EdgeParams    RPREdgeParams;
    T_RPR_FillParam     RPRFillParam;
} T_RPRP;



/* The PictureHeader structure represents the information that */
/* goes into the picture header of a single bitstream picture. */

/*        bool        isFRUFrame() { return PicCodType == FRUPIC; } */
/*        bool        isBFrame()   { return PicCodType == TRUEBPIC; } */
/*        bool        isPFrame()   { return PicCodType == INTERPIC; } */
/*        bool        isKeyFrame() { return PicCodType == INTRAPIC; } */

#define ISBFRAME(hdr) \
    ((hdr).PicCodType == TRUEBPIC)
#define ISFRUFRAME(hdr) \
    ((hdr).PicCodType == FRUPIC)
#define ISPFRAME(hdr) \
    ((hdr).PicCodType == INTERPIC)
#define ISKEYFRAME(hdr) \
    ((hdr).PicCodType == INTRAPIC)

struct PictureHeader
{
    EnumPicCodType  PicCodType;
    struct RV_Dimensions  dimensions;
    U32             TR;

    RV_Boolean      UMV;
    RV_Boolean      DF;         /* In-the-loop deblocking filter (Annex J) */
    RV_Boolean      SS;         /* Slice-structured (Annex K) */

    RV_Boolean      RPR;        /* Reference-picture resampling (RPR Annex P) */

    RV_Boolean      Rounding;   /* Round-off for half-pixel motion compensation   */
    /* RV8 (not used, but still in bitstream) */

    T_RPRP          RPRP;       /* RPR parameters */

    RV_Boolean      Deblocking_Filter_Passthrough;
    /* This is a non-standard option used in RealVideo */
    /* that indicates that the deblocking filter in annex J */
    /* should not be called. The option will be ignored if */
    /* Annex J is off or if the bitstream is standards-based. */

    U32             pixel_aspect_ratio;

    U8              PQUANT;
    U8              OSVQUANT;
    U8              TRB;
    U8              DBQUANT;

#ifdef INTERLACED_CODE
    RV_Boolean      x10;
    RV_Boolean      Interlaced;
    RV_Boolean      InterlacedTopFieldFirst;
    RV_Boolean      RepeatField;
    U32             DeblockingQpAdjust;
#endif
};


/* A DecoderMBInfo structure describes one decoded macroblock. */
/* */
/* We should keep this structure reasonably small and well aligned, */
/* since we need to allocate one per luma macroblock, twice (one set */
/* for B frames and one set for non-B frames). */
/* Technically, we don't need two sets of the 'missing' and 'edge_type' */
/* members.  But they are relatively small, and keeping them along with */
/* QP and mbtype will help localize data cache accesses. */

struct DecoderMBInfo
{
    U8      edge_type;
    /* Indicates whether the MB falls on a picture or slice boundary. */
    /* */
    /* WARNING: Do not change the following EDGE value definitions */
    /* without making corresponding changes to dscalup.cpp, */
    /* specifically the jump table in ExtendBlockEdges(). */
    /* These bit masks are also used by error concealment. */

#define         D_LEFT_EDGE     0x1
#define         D_RIGHT_EDGE    0x2
#define         D_TOP_EDGE      0x4
#define         D_BOTTOM_EDGE   0x8


    U8      QP;

    U8      mbtype;

    Bool8   missing;
    /* Indicates whether the MB was absent from the bitstream. */
    /* Error concealment is performed on such MBs. */

    U32     cbpcoef;
    U32     mvdelta;
    /* Indicates coded blocks with coefficients. */

    /*void        clearMissing()      { missing = 0; } */
    /*void        setMissing()        { missing = 1; } */
    /*Bool32      isMissing() const   { return Bool32(missing); } */

};

struct RV_Rectangle
{
    U32     width;
    U32     height;
    U32     x;
    U32     y;
};

/* A RV_Image_Format object describes the *type* of an image.  The intent */
/* is that it contains enough descriptive information so that a codec can */
/* perform capability negotiations (i.e., advertise and agree on input and */
/* output format types) and also allocate any dimension-dependent memory. */

struct RV_Image_Format
{
    RV_FID         fid;

    struct RV_Dimensions  dimensions;
    /* Generally, this "dimensions" member specifies the dimensions */
    /* of the image at hand (or perhaps the desired dimensions, if */
    /* this format is describing a request for zoomed output). */
    /* However, for DCI, this is not the case. */
    /* For DCI (and only DCI), when a RV_Image_Format is describing */
    /* a RV_Decoder's output image, then this "dimensions" member */
    /* specifies the dimensions of the display screen, and the desired */
    /* dimensions of the image are found in the "rectangle" member . */

    struct RV_Rectangle   rectangle;
    /* In general, this "rectangle" member has origin (0, 0) and */
    /* dimensions equivalent to the image at hand. */
    /* However, when this RV_Image_Format is describing a */
    /* RV_Decoder's output image for DCI, then this "rectangle" */
    /* member specifies a rectangle within the screen.  In this */
    /* case, it will have a non-zero origin, though its dimensions */
    /* will still reflect those of the image at hand (or its desired */
    /* dimensions, it zooming is being requested). */
    /* */
    /* In all cases (DCI and not DCI), the dimensions in the */
    /* "rectangle" member are the dimensions of the image at hand */
    /* (or its desired dimensions, if requesting zoomed output). */
    /* In the non-DCI case, the rectangle's origin is zero, and its */
    /* dimensions match the "dimensions" member. */
    /* In the DCI case, the rectangle's origin may be non-zero, its */
    /* dimensions represent the image at hand, and the above */
    /* "dimensions" member reflects the display screen size. */



    /* The yuv_info struct represents the pitch values for YUV */
    /* formats, e.g. YUV12, YVU9 et.al. */
    struct
    {
        U32     y_pitch;
        U32     u_pitch;
        U32     v_pitch;
    } yuv_info;
};



/* A RV_Image object represents an actual instance of an image.  Thus it */
/* includes a RV_Image_Format that describes the image, as well as pointers */
/* to the image's data, among other information. */

struct RV_Image
{
    struct RV_Image_Format    format;
    U32                 size;
    /* "size" gives the total length, in bytes, of the image's data. */

    U32                 sequence_number;
    /* This number indicates the temporal position of this image */
    /* with respect to previous and future images.  Its value is */
    /* specific to each video environment. */

    /* The yuv_info struct points to the Y, U and V planes' data. */
    /* It is used for YUV formats, e.g. YUV12, YVU9, et. al. */
    struct
    {
        U8         *y_plane;
        U8         *u_plane;
        U8         *v_plane;
    } yuv_info;

};

#ifdef __cplusplus
}
#endif




#endif /* RVSTRUCT_H__ */

