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




























#include "string.h" /* memset */

#include "beginhdr.h"
#include "rvdecoder.h"
#include "frame.h"
#include "hxtypes.h"

/* Static functions declared in RV_Decoder class. */
RV_Status
Decoder_Startup()
{
    RV_Status Status;

    Status = Decoder_Load();

    return Status;
}

RV_Status
Decoder_Shutdown()
{
    return RV_S_OK;
}

/* Construct the specific core Decoder object. */
void *
Decoder_CreateDecoder(RV_FID fid, struct RVDecoder* rvdecoder, RV_Status *status)
{
    struct Decoder *dec = NULL;

    if (    fid == RV_FID_RV89COMBO
            || fid == RV_FID_REALVIDEO30
       )
    {
        *status = RV_S_OK;
        dec = (struct Decoder *)RV_Allocate(sizeof(struct Decoder), 0);
        memset(dec,0,sizeof(struct Decoder));
        Decoder_Init(dec, FALSE, C_IMPL, fid, rvdecoder, status);

    }
    else
    {
        *status = RV_S_BAD_FORMAT;
    }

    return (void *)dec;
}

/* Methods defined to provide versions for abstract virtuals */
/* in RV_Decoder class. */
RV_Status
Decoder_Propose_Format
(
    struct Decoder *t,
    U32               position,
    const struct RV_Image_Format* src,
    struct RV_Image_Format* dst
)
{
    RV_Status  ps;

    ps = Decoder_CheckInput(t,src);
    if (ps == RV_S_OK)
    {
        {
            switch (position)
            {
                case 0:
                    dst->fid = RV_FID_YUV12;
                    dst->dimensions = src->dimensions;
                    DecodedFrame_Propagate_Defaults(dst);
                    break;
                default:
                    ps = RV_S_OUT_OF_RANGE;
                    break;
            }
        }
    }

    return ps;
}


RV_Status
Decoder_CheckInput(struct Decoder *t, const struct RV_Image_Format* src)
{
    RV_Status ps     = RV_S_BAD_DIMENSIONS;
    U32        width  = src->dimensions.width;
    U32        height = src->dimensions.height;


    /* m_fid is the RV_FID we were originally opened with. */
    /* src.fid is the input format being checked. */

    if (   src->fid == RV_FID_RV89COMBO
            || src->fid == RV_FID_REALVIDEO30
       )
    {
        /* H.263+ supports custom picture format with width [4,...,2048], */
        /* height [4,...,1152], and both a multiple of 4. */
        /* We put a limit on the low end of 32. */
        if (   width  <= MAX_XDIM
                && height <= MAX_YDIM
                && width  >= MIN_XDIM
                && height >= MIN_YDIM
                && (width & ~3) == width
                && (height & ~3) == height
           )
        {
            ps = RV_S_OK;
        }
    }
    else
    {
        ps = RV_S_BAD_FORMAT;
    }

    if (ps != RV_S_OK)
    {
        /*        RVDebug((RV_TL_INIT, "ERROR: CheckInput --" */
        /*            " Unsupported format (%s) or src dimen %d x %d", */
        /*            RV_Get_FID_Name(src->fid), width, height)); */
        RVDebug((RV_TL_INIT, "ERROR: CheckInput --"
                 " Unsupported format (%d) or src dimen %d x %d",
                 src->fid, width, height));
    }

    return ps;

}

RV_Status
Decoder_CheckTransform(struct Decoder *t,
                       const struct RV_Image_Format* src,
                       const struct RV_Image_Format* dst)
{
    RV_Status Status = RV_S_OK;
    U32 width  = dst->dimensions.width;
    U32 height = dst->dimensions.height;

    Status = Decoder_CheckInput(t,src);
    if (RV_S_OK == Status)
    {
        {
            /* The output format must be YUV12. */
            if (dst->fid != RV_FID_YUV12)
            {
                RVDebug((RV_TL_ALWAYS,"WARNING Decoder::CheckTransform"
                         " Bad destination format %d",
                         dst->fid));
                Status = RV_S_BAD_FORMAT;
            }
            else
            {
#if defined(CPK)
                /* If the src and dst dimensions do not match, a smart zoom is */
                /* used to re-size the src to match the dst.  It is assumed */
                /* that there is enough space allocated for the dst.  Make sure */
                /* the dst format is within the size limits for H.263+ since */
                /* post filters may depend on these limits. */
                if ((width  > MAX_XDIM)        ||
                        (height > MAX_YDIM)        ||
                        (width  < MIN_XDIM)        ||
                        (height < MIN_YDIM)        ||
                        ((width  & ~3) != width)   ||
                        ((height & ~3) != height))
                {
                    RVDebug((RV_TL_ALWAYS,
                             "WARNING Decoder::CheckTransform"
                             " Bad destination dimensions"));
                    Status = RV_S_BAD_DIMENSIONS;
                }
#else
                /* This must be the encoder's local decoder, and must be */
                /* our VFW/DirectShow codec. */
                /* TBD, does such a beast exist for RV89COMBO? */

                /* Smart zoom is not currently being used for this case, */
                /* so the src and dst dimensions must be the same. */
                if (src->dimensions != dst->dimensions)
                {
                    RVDebug((RV_TL_ALWAYS,
                             "WARNING Decoder::CheckTransform"
                             " Bad destination dimensions for encoder's decoder"));
                    Status = RV_S_BAD_DIMENSIONS;
                }
#endif
            }
        }
    }

    return Status;
}

/* */
/* Decoder CPU scalability setting */
/* */


RV_Status
Decoder_Get_CPU_Scalability_Setting(struct Decoder *t, U32 * iValue)
{
    *iValue = t->m_options.CPU_Scalability_Setting;
    return RV_S_OK;
}


RV_Status
Decoder_Get_Default_CPU_Scalability_Setting(struct Decoder *t, U32 * iValue)
{
    *iValue = CPU_Scalability_Default;
    return RV_S_OK;
}

RV_Status
Decoder_Set_CPU_Scalability_Setting(struct Decoder *t, U32 iValue)
{
    RV_Status ps = RV_S_OK;

    /* We need to be careful to keep our CPU setting in agreement with */
    /* our YUV_Displayer's setting. */

    if (CPU_Scalability_Maximum < iValue ||
            CPU_Scalability_Minimum > iValue)
    {
        RVDebug((RV_TL_ALWAYS,"ERROR CPU_Scalability out of range"
                 ": %ld", iValue));
        return RV_S_OUT_OF_RANGE;
    }

    t->m_options.CPU_Scalability_Setting = iValue;

    return ps;
}
