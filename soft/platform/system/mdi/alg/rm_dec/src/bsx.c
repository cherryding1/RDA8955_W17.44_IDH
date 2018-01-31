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


















#ifdef SHEEN_VC_DEBUG
#include "memory.h"
#include "string.h"
#include "stdlib.h"
#endif

#include "beginhdr.h"
#include "bsx.h"
#include "supervlc.h"
#include "rvdebug.h"


/* Bit stream field sizes */

#define FIELDLEN_PSC 22
const U32 FIELDLEN_TR = 8;      /* temporal reference */
const U32 FIELDLEN_TR_RV = 13;

const U32 FIELDLEN_PTYPE = 13;  /* picture type */
const U32 FIELDLEN_PTYPE_CONST = 2;
const U32 FIELDLEN_PTYPE_SPLIT = 1;
const U32 FIELDLEN_PTYPE_DOC = 1;
const U32 FIELDLEN_PTYPE_RELEASE = 1;
const U32 FIELDLEN_PTYPE_SRCFORMAT = 3;
const U32 FIELDLEN_PTYPE_CODINGTYPE = 1;
const U32 FIELDLEN_PTYPE_UMV = 1;
const U32 FIELDLEN_PTYPE_AP = 1;
const U32 FIELDLEN_PTYPE_PB = 1;

const U32 FIELDLEN_PPTYPE_UFEP = 3;
const U32 FIELDLEN_OPTIONAL_EPTYPE_RESERVED =  3;
const U32 FIELDLEN_MANDATORY_EPTYPE_RESERVED    =  2;

const U32 FIELDLEN_MPPTYPE_SRCFORMAT = 3;
const U32 FIELDLEN_MPPTYPE_RPR = 1;
const U32 FIELDLEN_MPPTYPE_RRU = 1;
const U32 FIELDLEN_MPPTYPE_ROUNDING = 1;

const U32 FIELDLEN_CSFMT_PARC = 4;
const U32 FIELDLEN_CSFMT_FWI = 9;
const U32 FIELDLEN_CSFMT_CONST = 1;
const U32 FIELDLEN_CSFMT_FHI = 9;

const U32 FIELDLEN_EPAR_WIDTH = 8;
const U32 FIELDLEN_EPAR_HEIGHT = 8;

const U32 FIELDLEN_PQUANT = 5;  /* picture quant value */
const U32 FIELDLEN_CPM = 1;     /* continuous presence multipoint indicator */
const U32 FIELDLEN_TRB = 3;     /* temporal reference for B frames */
const U32 FIELDLEN_DBQUANT = 2; /* B frame differential quant value */
const U32 FIELDLEN_PSPARE = 8;  /* spare information */

#define FIELDLEN_GBSC 17    /* Group of blocks start code */
const U32 FIELDLEN_GN = 5;      /* GOB number. */
const U32 FIELDLEN_GLCI = 2;    /* GOB logical channel indicator */
const U32 FIELDLEN_GFID = 2;    /* GOB Frame ID */
const U32 FIELDLEN_GQUANT = 5;  /* GQUANT */
const U32 FIELDLEN_SQUANT = 5;  /* SQUANT */

#if 0//move to bsx.h sheen
#define FIELDLEN_SSC 24         /* Slice start code */
#define FIELDLEN_SSC_RV8 17     /* Slice start code */
#endif

const U32 FIELDLEN_WDA = 2;     /* RPR warping displacement accuracy */
const U32 FIELDLEN_RPR_FILL_MODE = 2;
const U32 FIELDLEN_RPR_COLOR = 8;

const U32 FIELDLEN_PWIDTH_RV = 8;
const U32 FIELDLEN_PHEIGHT_RV = 8;

/* Bit stream field values */

const U32 FIELDVAL_PSC  = (0x00008000 >> (32 - FIELDLEN_PSC));
const U32 FIELDVAL_SSC  = (0x1d1c105e >> (32 - FIELDLEN_SSC));
const U32 FIELDVAL_SSC_RV8  = (0x00008000 >> (32 - FIELDLEN_SSC_RV8));
const U32 FIELDVAL_GBSC = (0x00008000 >> (32 - FIELDLEN_GBSC));
const U32 FIELDVAL_EPTYPE_RESERVED  =  0;

/* Bitstream Version Information */
/* */
/* Starting with RV_FID_REALVIDEO30, for RealVideo formats we embed the */
/* minor bitstream version number in the slice header. */
/* The encoder only ever produces one bitstream format.  But the decoder */
/* must be backwards compatible, and able to decode any prior minor */
/* bitstream version number. */
/* */
/* It is assumed that if the bitstream major version number changes, */
/* then a new RV_FID is introduced.  So, only the minor version number */
/* is present in the bitstream. */
/* */
/* The minor version number is encoded in the bitstream using 3 bits. */
/* RV89Combo's first bitstream minor version number is "2", which is encoded */
/* as all 0's.  The following table maps the bitstream value to the */
/* actual minor version number. */

const U32 FIELDLEN_RV_BITSTREAM_VERSION = 3;

#define UNSUPPORTED_RV_BITSTREAM_VERSION 9999

#define ENCODERS_CURRENT_RV_BITSTREAM_VERSION 2
/* This should match RV_BITSTREAM_MINOR_VERSION in "hiverv.h" */

static const U32 s_RVVersionEncodingToMinorVersion[8] =
{
    2,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION,
    UNSUPPORTED_RV_BITSTREAM_VERSION
};

#define NUMBER_OF_RV_BITSTREAM_VERSIONS \
                   (sizeof(s_RVVersionEncodingToMinorVersion) \
                   / sizeof(s_RVVersionEncodingToMinorVersion[0]))

#if 0//move to bsx.h sheen
/* macros for getting slice valid flag and slice offset */
#define SliceFlag(x)        (t->m_sliceInfo[(x)].is_valid)
#define SliceOffset(x)      (t->m_sliceInfo[(x)].offset)
#define SliceBitOffset(x)   (SliceOffset(x) << 3)
#endif

#if 0//sheen

/* --------------------------------------------------------------------------- */
/*  CB_CRealVideoBs() */
/*      H.263+ bitstream constructor used in the decoder */
/* --------------------------------------------------------------------------- */

void CB_Init1(
    struct CRealVideoBs *t,
    const RV_FID    fid,
    U8* const       pb,
    const U32       maxsize)

{
    t->m_pbsBase    = pb;
    t->m_pbs        = pb;
    t->m_bitOffset  = 0;
    t->m_maxBsSize  = maxsize;
    t->m_localBs    = FALSE;


    t->m_fid            = fid;
    t->m_gob            = 0;
    t->m_MBRowNum       = 0;
    t->m_mb         = 0;
    t->m_mbID           = 0;
    t->m_pbsSliceBase   = NULL;
    t->m_slice          = FALSE;
    t->m_isForcedKey    = FALSE;

    /* not known at this point, initialized to 0 */
    t->m_nGOBs          = 0;
    t->m_nMBs           = 0;
    t->m_nSlices        = 0;
    t->m_sliceInfo      = NULL; /* to be initialized */
    t->m_mbaSize        = 0;
    t->m_numSizes       = 0;
    t->m_pctszSize      = 0;
    t->m_pSizes     = NULL;

} /* CB_CRealVideoBs() */
#endif

/* --------------------------------------------------------------------------- */
/*  CB_CRealVideoBs() */
/*      default constructor used in the decoder */
/* --------------------------------------------------------------------------- */

void CB_Init2(
    struct CRealVideoBs *t,
    const RV_FID    fid)
{
    t->m_pbs       = NULL;
    t->m_pbsBase   = NULL;
    t->m_bitOffset = 0;
    t->m_localBs   = FALSE;
    t->m_maxBsSize  = 0;

    t->m_fid            = fid;
    t->m_gob            = 0;
    t->m_MBRowNum       = 0;
    t->m_mb         = 0;
    t->m_mbID           = 0;
    t->m_pbsSliceBase   = NULL;
    t->m_slice          = FALSE;
    t->m_isForcedKey    = FALSE;

    /* not known at this point, initialized to 0 */
    t->m_nGOBs          = 0;
    t->m_nMBs           = 0;
    t->m_nSlices        = 0;
    t->m_sliceInfo      = NULL;
    t->m_mbaSize        = 0;
    t->m_numSizes       = 0;
    t->m_pctszSize      = 0;
    t->m_pSizes     = NULL;

} /* CB_CRealVideoBs() */

#if 0//sheen
void
CB_copyState(
    struct CRealVideoBs *t,
    struct CRealVideoBs *pBs)
{

    t->m_fid            = pBs->m_fid;
    t->m_gob            = pBs->m_gob;
    t->m_MBRowNum       = pBs->m_MBRowNum;

    t->m_mb         = pBs->m_mb;
    t->m_mbID           = pBs->m_mbID;
    t->m_pbsSliceBase   = pBs->m_pbsSliceBase;
    t->m_pbs            = pBs->m_pbs;
    t->m_pbsBase        = pBs->m_pbsBase;
    t->m_bitOffset      = pBs->m_bitOffset;

    t->m_maxBsSize      = pBs->m_maxBsSize;
    t->m_localBs        = pBs->m_localBs;

    t->m_slice          = pBs->m_slice;
    t->m_isForcedKey    = pBs->m_isForcedKey;

    t->m_nGOBs          = pBs->m_nGOBs;
    t->m_nMBs           = pBs->m_nMBs;
    t->m_nSlices        = pBs->m_nSlices;
    t->m_sliceInfo      = pBs->m_sliceInfo;
    t->m_mbaSize        = pBs->m_mbaSize;
    t->m_numSizes       = pBs->m_numSizes;
    t->m_pctszSize      = pBs->m_pctszSize;
    t->m_pSizes     = pBs->m_pSizes;

    t->m_rv         = pBs->m_rv;

}
#endif

/* --------------------------------------------------------------------------- */
/*  CB_Delete() */
/* --------------------------------------------------------------------------- */

void CB_Delete(struct CRealVideoBs *t)
{
    if (t->m_sliceInfo)
    {
        /*      delete [] t->m_sliceInfo; */
        RV_Free (t->m_sliceInfo);
        t->m_sliceInfo = NULL;
    }


} /* CB_~CRealVideoBs() */


/* --------------------------------------------------------------------------- */
/*  CB_IsSliceValid() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_IsSliceValid(struct CRealVideoBs *t, I32 iSliceNum)
{
    RV_Boolean retval;

    retval = SliceFlag(iSliceNum) ? TRUE : FALSE;
    return retval;

} /* CB_IsSliceValid() */


/* --------------------------------------------------------------------------- */
/*  CB_Reset() */
/*      reset bitstream; used in the decoder */
/* --------------------------------------------------------------------------- */

void CB_Reset(
    struct CRealVideoBs *t,
    U8* const   pb,
    const U32   maxsize)
{
    t->m_pbs       = pb;
    t->m_pbsBase   = pb;
    t->m_bitOffset = 0;
    t->m_maxBsSize  = maxsize;

    t->m_gob            = 0;
    t->m_MBRowNum       = 0;
    t->m_mb         = 0;
    t->m_mbID           = 0;
    t->m_pbsSliceBase   = NULL;
    t->m_slice          = FALSE;

    /* note: m_nSlices and m_sliceInfo are reset by PutSliceOffsets */

} /* CRealVideoBs::Reset() */



/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetForcedKeyState() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetForcedKeyState(struct CRealVideoBs *t)
{
    return t->m_isForcedKey;

} /* CRealVideoBs::GetForcedKeyState() */

#if 0//sheen
/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetSliceOffsets() */
/* --------------------------------------------------------------------------- */

void CB_GetSliceOffsets(
    struct CRealVideoBs *t,
    U32     *nOffsets,
    Bitstream_Segment_Info *pSegInfo)
{
    *nOffsets = t->m_nSlices;
    pSegInfo = t->m_sliceInfo;

} /* CRealVideoBs::GetSliceOffsets() */

#endif

/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::PutSliceOffsets() */
/* --------------------------------------------------------------------------- */

void CB_PutSliceOffsets(
    struct CRealVideoBs *t,
    const U32           nOffsets,
    Bitstream_Segment_Info  *pSegInfo)
{
#if defined(DEBUG)
    U32 slice;
#endif
    t->m_nSlices = nOffsets;
    t->m_sliceInfo = pSegInfo;

#if defined(DEBUG)
    RVDebug((RV_TL_PICT, "PutSliceOffsets  %ld", t->m_nSlices));
    for (slice = 0; slice < t->m_nSlices; slice++)
    {
        RVDebug((RV_TL_PICT, "    offset  %6ld%s",
                 SliceOffset(slice), SliceFlag(slice) ? "" : "    INVALID"));
    }
#endif /* defined(DEBUG) */

} /* struct CRealVideoBs::PutSliceOffsets() */

#if 0//sheen

/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::SetSliceOffset() */
/* --------------------------------------------------------------------------- */
void CB_SetSliceOffset(struct CRealVideoBs *t, I32 iSliceNum)
{
    t->m_pbs        = t->m_pbsBase + SliceOffset(iSliceNum);
    t->m_bitOffset = 0;

}  /*  CRealVideoBs::SetSliceOffset() */
#endif


/* lookup table used for chroma blocks when modified quantization is in effect */
/* Annex T tables used in encoder too  */
const I32 gNewTAB_DQUANT_MQ_RV89[32][2] =
{
    { 0, 0}, { 2, 1}, {-1, 1}, {-1, 1}, /* 0 .. 3 */
    {-1, 1}, {-1, 1}, {-1, 1}, {-1, 1}, /* 4 .. 7 */
    {-1, 1}, {-1, 1}, {-1, 1}, {-2, 2}, /* 8 .. 11 */
    {-2, 2}, {-2, 2}, {-2, 2}, {-2, 2}, /* 12 .. 15 */
    {-2, 2}, {-2, 2}, {-2, 2}, {-2, 2}, /* 16 .. 19 */
    {-2, 2}, {-3, 3}, {-3, 3}, {-3, 3}, /* 20 .. 23 */
    {-3, 3}, {-3, 3}, {-3, 3}, {-3, 3}, /* 24 .. 27 */
    {-3, 3}, {-3, 2}, {-3, 1}, {-3,-5}  /* 28 .. 31 */
};

#if 0// move to rv_overlay_set.c sheen
/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetDQUANT() */
/* --------------------------------------------------------------------------- */
void CB_GetDQUANT
(
    struct CRealVideoBs *t,
    U8 * quant,
    const U8 quant_prev,
    const RV_Boolean mq
)
{
    I32 dquant;
    I32 qp=0;

    if (mq)
    {
        if (CB_Get1Bit(t))
        {
            dquant = gNewTAB_DQUANT_MQ_RV89[quant_prev][CB_Get1Bit(t)];
            qp = quant_prev + dquant;
        }
        else
        {
            qp = CB_GetBits(t,5);
        }
    }
    else
    {
        /*      dquant  = gNewTAB_DQUANT[GetBits(2)]; */
        /*      qp = quant_prev + dquant */
        RVDebug((RV_TL_ALWAYS,"ERROR -- "
                 "CB_GetDQUANT : mq != TRUE"));
    }

    if (qp < 0)
    {
        qp = 0;
        RVDebug((RV_TL_ALWAYS,"ERROR -- "
                 "CB_GetDQUANT : quant < 0"));
    }
    else if (qp > 31)
    {
        qp = 31;
        RVDebug((RV_TL_ALWAYS,"ERROR -- "
                 "CB_GetDQUANT : quant > 31"));
    }

    *quant = (U8)qp;


} /* CRealVideoBs::GetDQUANT() */


/* --------------------------------------------------------------------------- */
/*  CRealVideoBs::GetPicSize() */
/*  Gets the picture size from the bitstream. */
/* --------------------------------------------------------------------------- */

void CB_GetPicSize(struct CRealVideoBs *t, U32 *width, U32 *height)
{
    U32 code;
    U32 w, h;

    const U32 code_width[8] = {160, 176, 240, 320, 352, 640, 704, 0};
    const U32 code_height1[8] = {120, 132, 144, 240, 288, 480, 0, 0};
    const U32 code_height2[4] = {180, 360, 576, 0};

    /* If we're decoding a P or B frame, get 1 bit that signals */
    /* difference in size from previous reference frame. */
    if (t->m_rv.ptype == RV_INTERPIC || t->m_rv.ptype == RV_TRUEBPIC)
    {
        /* no diff in size */
        if (CB_Get1Bit(t))
        {
            *width = t->m_rv.pwidth_prev;
            *height = t->m_rv.pheight_prev;
            return;
        }
    }


    /* width */
    code = CB_GetBits(t,3);
    w = code_width[code];
    if (w == 0)
    {
        do
        {
            code = CB_GetBits(t,8);
            w += (code << 2);
        }
        while (code == 255);
    }

    /* height */
    code = CB_GetBits(t,3);
    h = code_height1[code];
    if (h == 0)
    {
        code <<= 1;
        code |= CB_Get1Bit(t);
        code &= 3;
        h = code_height2[code];
        if (h == 0)
        {
            do
            {
                code = CB_GetBits(t,8);
                h += (code << 2);
            }
            while (code == 255);
        }
    }

    *width = w;
    *height = h;
}


/* --------------------------------------------------------------------------- */
/*  CB_GetPictureHeader() */
/* --------------------------------------------------------------------------- */

RV_Status CB_GetPictureHeader(struct CRealVideoBs *t, struct PictureHeader* hdr)
{
    RV_Status Status = RV_S_OK;
    I32 gfid, mba, squant;
    I32 tmp;


    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        /* note: header should be initialized earlier by SetFrameProperties() */

        /* find the first valid slice */
        if (!CB_IsSegmentValid(t) && CB_LossRecovery(t,TRUE) != BSLR_STATUS_OK)
            return RV_S_ERROR;

        /* get the first slice header */
        /* set m_rv.mba to -1 to indicate this is a call to GetSliceHeader from  */
        /* GetPicHeader */
        t->m_rv.mba = -1;
        if (CB_GetSliceHeader(t, FALSE, &mba, &gfid, &squant,
                              hdr))
        {
            switch(t->m_rv.ptype)
            {
                case RV_FORCED_INTRAPIC:
                    hdr->PicCodType = INTRAPIC;
                    t->m_isForcedKey    = TRUE;
                    break;
                case RV_INTRAPIC:
                    hdr->PicCodType = INTRAPIC;
                    t->m_isForcedKey    = FALSE;
                    break;
                case RV_INTERPIC:
                    hdr->PicCodType = INTERPIC;
                    break;
                case RV_TRUEBPIC:
                    hdr->PicCodType = TRUEBPIC;
                    break;
                default:
                    RVAssert(0);
                    break;
            }
            hdr->Deblocking_Filter_Passthrough  = t->m_rv.deblocking_filter_passthrough;
            hdr->PQUANT     = t->m_rv.pquant;
            hdr->TR         = t->m_rv.tr;
            hdr->TRB        = t->m_rv.trb;
            hdr->DBQUANT    = t->m_rv.dbq;
#ifdef INTERLACED_CODE
            hdr->x10        = t->m_rv.x10;
            hdr->Interlaced = t->m_rv.interlaced;
            hdr->InterlacedTopFieldFirst = t->m_rv.interlacedTopFieldFirst;
            hdr->DeblockingQpAdjust      = t->m_rv.DeblockingQpAdjust;
            hdr->RepeatField             = t->m_rv.RepeatField;
#endif

            /* initialize m_nGOBS, m_nMBs, and m_mbaSize */
            CB_SetDimensions(t,hdr);
        }
        else
            Status = RV_S_ERROR;
    }
    else
    {
        U32 length, info;
        const U32 PSC_LEN = 31;

        (void) memset(hdr, 0, sizeof(*hdr));

        /* SUPER_VLC needs longer start code to avoid emulation */
        tmp = CB_GetBits(t,16);
        if (tmp != 21845)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }
        tmp = CB_GetBits(t,16);
        if (tmp != 21845)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }

        length = CB_GetVLCBits(t,&info);

        if (length < PSC_LEN)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }

        if (info & 1)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR GetPictureHeader -- "
                     "Found sequence end code"));
            return RV_S_OUT_OF_RANGE;
            /* This is probably not the right error code to use, */
            /* but it will have to do for now. */
        }


        if (!((info>>1) & 1))
        {
            /* QCIF; */
            hdr->dimensions.width = 176;
            hdr->dimensions.height = 144;
        }
        else
        {
            hdr->dimensions.width = 0;
            hdr->dimensions.height = 0;
            /* Read dimensions from the bitstream, below */
        }

        hdr->PQUANT = (U8)((info>>2) & 31);
        hdr->TR = (info>>(2+5)) & 255;

        /* Picture type */
        length = CB_GetVLCBits(t,&info);

        if (length == 1)
        {
            hdr->PicCodType = INTERPIC;
        }
        else if (length == 3 && info == 1)
        {
            hdr->PicCodType = INTRAPIC;
        }
        else if (length == 5 && info == 0)
        {
            hdr->PicCodType = TRUEBPIC;
        }
        else
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Unsupported picture coding type"));
            return RV_S_UNSUPPORTED;
        }

        /* Non-QCIF frame size */
        if (hdr->dimensions.width == 0)
        {
            CB_GetCPFMT(t, hdr);
        }

        CB_SetDimensions(t,hdr);


#if defined(INTERLACED_CODE) && defined(RV10i_RAW_BITSTREAM)

        /* x10 Header */
        if(CB_Get1Bit(t))
        {
            hdr->x10 = TRUE;
            /* Interlaced */
            hdr->Interlaced = (CB_Get1Bit(t))?(TRUE):(FALSE);
            hdr->InterlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            /* Repeat Field */
            hdr->RepeatField = (CB_Get1Bit(t))?(TRUE):(FALSE);
            /* Bitstream Version */
            CB_Get1Bit(t);
            /* Deblocking filter Adjust */
            if(CB_Get1Bit(t))
            {
                /* qp */
                hdr->DeblockingQpAdjust = CB_GetBits(t,2);
                hdr->DeblockingQpAdjust = hdr->DeblockingQpAdjust + 2;
            }
            else
            {
                hdr->DeblockingQpAdjust = 0;
            }
        }
        else
        {
            hdr->x10 = FALSE;
            /* Interlaced */
            hdr->Interlaced = FALSE;
            hdr->InterlacedTopFieldFirst = TRUE;
            /* Repeat Field */
            hdr->RepeatField = FALSE;
            hdr->DeblockingQpAdjust = 0;
        }
#endif

        /* OSV Quant */
        hdr->OSVQUANT = (U8) CB_GetBits(t,2);
    }
    return Status;
}
#endif


/* --------------------------------------------------------------------------- */
/*  CB_GetPictureHeader_RV8() */
/* --------------------------------------------------------------------------- */

RV_Status CB_GetPictureHeader_RV8(struct CRealVideoBs *t, struct PictureHeader* hdr)
{
    RV_Status Status = RV_S_OK;
    I32 gfid, mba, squant;
    U32 length, info;
    const U32 PSC_LEN = 31;
    I32 tmp;

    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        /* note: header should be initialized earlier by SetFrameProperties() */

        /* find the first valid slice */
        if (!CB_IsSegmentValid(t) && CB_LossRecovery(t,TRUE) != BSLR_STATUS_OK)
            return RV_S_ERROR;

        /* get the first slice header */
        /* set t->m_rv.mba to -1 to indicate this is a call to GetSliceHeader from  */
        /* GetPicHeader */
        t->m_rv.mba = -1;
        if (CB_GetSliceHeader_RV8(t,FALSE, &mba, &gfid, &squant,hdr))
        {
            switch(t->m_rv.ptype)
            {
                case RV_FORCED_INTRAPIC:
                    hdr->PicCodType = INTRAPIC;
                    t->m_isForcedKey    = TRUE;
                    break;
                case RV_INTRAPIC:
                    hdr->PicCodType = INTRAPIC;
                    t->m_isForcedKey    = FALSE;
                    break;
                case RV_INTERPIC:
                    hdr->PicCodType = INTERPIC;
                    break;
                case RV_TRUEBPIC:
                    hdr->PicCodType = TRUEBPIC;
                    break;
                default:
                    RVAssert(0);
                    break;
            }
            hdr->Rounding   = t->m_rv.rtype;
            hdr->Deblocking_Filter_Passthrough  = t->m_rv.deblocking_filter_passthrough;
            hdr->PQUANT     = t->m_rv.pquant;
            hdr->TR         = t->m_rv.tr;
            hdr->TRB        = t->m_rv.trb;
            hdr->DBQUANT    = t->m_rv.dbq;

            /* initialize m_nGOBS, m_nMBs, and m_mbaSize */
            CB_SetDimensions(t,hdr);
        }
        else
            Status = RV_S_ERROR;
    }
    else
    {

        (void) memset(hdr, 0, sizeof(*hdr));

#if 0
        /* SUPER_VLC needs longer start code to avoid emulation */
        I32 tmp;
        tmp = CB_GetBits(t,16);
        if (tmp != 21845)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }
        tmp = CB_GetBits(t.16);
        if (tmp != 21845)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }
#endif
        tmp = CB_GetBits(t,24);
        if (tmp != 1)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }

        length = CB_GetVLCBits(t,&info);

        if (length < PSC_LEN)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader_RV8 -- "
                     "Failed to find Picture Start Code"));
            return RV_S_ERROR;
        }

        if (info & 1)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR GetPictureHeader_RV8 -- "
                     "Found sequence end code"));
            return RV_S_OUT_OF_RANGE;
            /* This is probably not the right error code to use, */
            /* but it will have to do for now. */
        }


        if (!((info>>1) & 1))
        {
            /* QCIF; */
            hdr->dimensions.width = 176;
            hdr->dimensions.height = 144;
        }
        else
        {
            hdr->dimensions.width = 0;
            hdr->dimensions.height = 0;
            /* Read dimensions from the bitstream, below */
        }

        hdr->PQUANT = (U8)((info>>2) & 31);
        hdr->TR = (info>>(2+5)) & 255;

        /* Picture type */
        length = CB_GetVLCBits(t,&info);

        if (length == 1)
        {
            hdr->PicCodType = INTERPIC;
        }
        else if (length == 3 && info == 1)
        {
            hdr->PicCodType = INTRAPIC;
        }
        else if (length == 5 && info == 0)
        {
            hdr->PicCodType = TRUEBPIC;
        }
        else
        {
            RVDebug((RV_TL_ALWAYS,"ERROR: GetPictureHeader_RV8 -- "
                     "Unsupported picture coding type"));
            return RV_S_UNSUPPORTED;
        }

        /* Non-QCIF frame size */
        if (hdr->dimensions.width == 0)
        {
            CB_GetCPFMT(t, hdr);
        }

        CB_SetDimensions(t,hdr);
    }
    return Status;
} /* getpictureheader_rv8 */


#if 0//move to rv_overlay_set.c sheen

U32
CB_GetSuperCBP(struct CRealVideoBs *t, U32 mbType, U32 QP)
{
    /* TBD, the ::decode_cbp prototype currently uses "int" instead */
    /* of U32.  Thus we jump through some hoops to guarantee appropriate */
    /* type conversions. */

    int cbp;
    unsigned int bitOffset = t->m_bitOffset;

    cbp = decode_cbp((int)(mbType), (int)(QP), &t->m_pbs, &bitOffset);

    t->m_bitOffset = (U32)(bitOffset);

    return (U32)(cbp);
}


void CB_DecodeSuperSubBlockCoefficients
(
    struct CRealVideoBs *t,
    I16    *pDst,
    U32     mbType,
    RV_Boolean    isChroma,
    U32     QP
)
{
    unsigned int bitOffset = t->m_bitOffset;

    decode_4x4_block((short *)pDst, (int)mbType, (int)isChroma, (int)(QP),
                     &t->m_pbs, &bitOffset);

    t->m_bitOffset = (U32)(bitOffset);
}





/* --------------------------------------------------------------------------- */
/*  CB_GetCPFMT() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetCPFMT(struct CRealVideoBs *t, struct PictureHeader *hdr)
{
    hdr->pixel_aspect_ratio = CB_GetBits(t,FIELDLEN_CSFMT_PARC);

    /* Frame width indication */
    /* The number of pixels per line is given by (FWI+1)*4 */
    hdr->dimensions.width = (CB_GetBits(t,FIELDLEN_CSFMT_FWI) + 1) << 2;

    /* Bit 14 must be "1" to prevent start code emulation */
    if (CB_Get1Bit(t) != 1)
    {
        RVDebug((RV_TL_ALWAYS, "ERROR GetCSFMT - CSFMT bit 14 != 1"));
        return(FALSE);
    }

    /* Frame height indication */
    /* The number of lines is given by FHI*4 */
    hdr->dimensions.height = CB_GetBits(t,FIELDLEN_CSFMT_FHI) << 2;

    return(TRUE);

} /* CB_GetCPFMT() */


/* --------------------------------------------------------------------------- */
/*  CB_GetSSC() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetSSC(struct CRealVideoBs *t)
{
    U32 offset, i, deltaofs;
    const U32 MAX_SSC_LOOKAHEAD_NUMBER = 7;


    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        offset = CB_GetBsOffset(t);
#ifdef BITSTREAM_ERROR_RESILIENT
        if (offset < SliceBitOffset(0) ||
                offset >= (CB_GetMaxBsSize(t) << 3))
            return FALSE;
#else
        RVAssert(offset >= SliceBitOffset(0) &&
                 offset < (CB_GetMaxBsSize(t) << 3));
#endif

        /* locate current slice, i - 1 */
        for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++) {};

        /* current slice must be valid at this point */
        RVAssert(SliceFlag(i - 1));

        /* check if just beginning to decode the current slice */
        if (offset == SliceBitOffset(i - 1))
            return TRUE;

        /* there is no more slice header in the bitstream */
        if (i == t->m_nSlices)
            return FALSE;

        /* skip stuffing if any */
        deltaofs = SliceBitOffset(i) - offset;
        if (deltaofs < 8 && CB_SearchBits(t,deltaofs, 0, 0))
            return TRUE;
    }
    else
    {

        if (CB_SearchBits_IgnoreOnly0( t,FIELDLEN_SSC, FIELDVAL_SSC,
                                       MAX_SSC_LOOKAHEAD_NUMBER))
            return TRUE;

        RVDebug((RV_TL_SUBPICT, "GetSSC - SSC not found"));
    }

    return FALSE;

} /* CB_GetSSC() */
#endif

RV_Boolean CB_GetSSC_RV8(struct CRealVideoBs *t)
{
    U32 offset, deltaofs, i;
    const U32 MAX_SSC_LOOKAHEAD_NUMBER = 7;

    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        offset = CB_GetBsOffset(t);
#ifdef BITSTREAM_ERROR_RESILIENT
        if (offset < SliceBitOffset(0) &&
                offset >= (CB_GetMaxBsSize(t) << 3))
        {
            return FALSE;
        }
#else
        RVAssert(offset >= SliceBitOffset(0) &&
                 offset < (CB_GetMaxBsSize(t) << 3));
#endif

        /* locate current slice, i - 1 */
        for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++) {};

        /* current slice must be valid at this point */
        RVAssert(SliceFlag(i - 1));

        /* check if just beginning to decode the current slice */
        if (offset == SliceBitOffset(i - 1))
            return TRUE;

        /* there is no more slice header in the bitstream */
        if (i == t->m_nSlices)
            return FALSE;

        /* skip stuffing if any */
        deltaofs = SliceBitOffset(i) - offset;
        if (deltaofs < 8 && CB_SearchBits(t,deltaofs, 0, 0))
            return TRUE;
    }
    else
    {

        if (CB_SearchBits_IgnoreOnly0( t,FIELDLEN_SSC_RV8, FIELDVAL_SSC_RV8,
                                       MAX_SSC_LOOKAHEAD_NUMBER))
            return TRUE;

        RVDebug((RV_TL_SUBPICT, "GetSSC - SSC not found"));
    }

    return FALSE;

} /* CB_GetSSC_RV8() */


/* --------------------------------------------------------------------------- */
/*  CB_SetRPRSizes() */
/* --------------------------------------------------------------------------- */

void CB_SetRPRSizes(struct CRealVideoBs *t,U32 num_sizes, U32 *sizes)
{
    U8 size[9] = {0,1,1,2,2,3,3,3,3};

    t->m_numSizes = num_sizes;
    t->m_pctszSize = size[num_sizes];

    t->m_pSizes = sizes;
}

/* --------------------------------------------------------------------------- */
/*  CB_SetFID() */
/* --------------------------------------------------------------------------- */
void CB_SetFID(struct CRealVideoBs *t, const RV_FID fid)
{
    t->m_fid = fid ;
}

/* --------------------------------------------------------------------------- */
/*  CB_GetNumSlices() */
/* --------------------------------------------------------------------------- */
U32             CB_GetNumSlices(struct CRealVideoBs *t)
{
    return t->m_nSlices;
}



#if 0//move to rv_overlay_set.c sheen
/* --------------------------------------------------------------------------- */
/*  CB_GetSliceHeader() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetSliceHeader(
    struct CRealVideoBs *t,
    RV_Boolean  bFirst,
    I32*        iSliceMBA,
    I32*        iSliceGFID,
    I32*        iSQUANT,
    struct PictureHeader* hdr)
{
    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        /* skip if this is the first slice called from IAPass1ProcessFrame */
        /* otherwise read the slice header */
        if (!bFirst)
        {
            U32 fieldlen;

            /* this was called from GetPicHeader */
            if (t->m_rv.mba == -1)
                bFirst = TRUE;

#ifdef USE_INTEL_VLC
            /* Bitstream version */
            U32 versionEncoding = CB_GetBits(t,3 /*FIELDLEN_RV_BITSTREAM_VERSION*/);
            if (versionEncoding > 0)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unrecognized RV bitstream version %ld",
                         versionEncoding));
                return FALSE;
            }

            /* PTYPE */
            if (bFirst)
                t->m_rv.ptype = (EnumRVPicCodType) CB_GetBits(t,2);
            else if ((EnumRVPicCodType) CB_GetBits(t,2) != t->m_rv.ptype)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unmatched PTYPE"));
                return FALSE;
            }

            /* ECC */
            CB_Get1Bit(t);

#ifdef INTERLACED_CODE
#ifdef COMPATIBILITY_BITSTREAM
            t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
            if(t->m_rv.interlaced) t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            t->m_rv.x10 = FALSE;
            /* Repeat Field */
            t->m_rv.RepeatField = FALSE;
            t->m_rv.DeblockingQpAdjust = 0;
#else
            /* x10 Header */
            if((CB_Get1Bit(t) ? TRUE : FALSE))
            {
                t->m_rv.x10 = TRUE;
                t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
                t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
                /* repeat field */
                t->m_rv.RepeatField = CB_Get1Bit(t);
                /* x10 bitstream version */
                CB_Get1Bit(t);
                /* Deblocking Qp Adjust */
                if(CB_Get1Bit(t))
                {
                    t->m_rv.DeblockingQpAdjust = CB_GetBits(t,2);
                    t->m_rv.DeblockingQpAdjust = t->m_rv.DeblockingQpAdjust +2;
                }
                else
                {
                    t->m_rv.DeblockingQpAdjust = 0;
                }
            }
            else
            {
                t->m_rv.x10 = FALSE;
                /* Interlaced */
                t->m_rv.interlaced = FALSE;
                t->m_rv.interlacedTopFieldFirst = TRUE;
                /* Repeat Field */
                t->m_rv.RepeatField = FALSE;
                t->m_rv.DeblockingQpAdjust = 0;
            }
#endif
#endif
            /* PQUANT/SQUANT */
            t->m_rv.pquant = (U8) CB_GetBits(t,FIELDLEN_SQUANT);

            /* 0-31 are legitimate QP values for RV89Combo */
            if (t->m_rv.pquant > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "SQUANT %d > 31", t->m_rv.pquant));
                return FALSE;
            }

            /* OSV Quant */
            hdr->OSVQUANT = (U8) CB_GetBits(t,2);

            /* In-loop deblocking */
            t->m_rv.deblocking_filter_passthrough = CB_GetBits(t,1)?TRUE:FALSE;

            fieldlen = FIELDLEN_TR_RV;

            if (bFirst)
                t->m_rv.tr = CB_GetBits(t,fieldlen);
            else if (CB_GetBits(t,fieldlen) != t->m_rv.tr)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unmatched TR"));
                return FALSE;
            }

            /* PCTSZ (RealVideo RPR picture size) */
            {
                U32 width, height;

                if (bFirst)
                {
                    switch (t->m_rv.ptype)
                    {
                        case RV_INTRAPIC:
                        case RV_FORCED_INTRAPIC:
                            t->m_rv.pwidth_prev  = 0;
                            t->m_rv.pheight_prev = 0;
                            break;
                        case RV_INTERPIC:
                            t->m_rv.pwidth_prev  = t->m_rv.pwidth;
                            t->m_rv.pheight_prev = t->m_rv.pheight;
                            break;
                    }
                }

                CB_GetPicSize(t, &width, &height);

                if (bFirst)
                {
                    t->m_rv.pwidth = width;
                    t->m_rv.pheight = height;
                    hdr->pixel_aspect_ratio = PARC_SQUARE;
                    /* TBD, why is parc set here? */
                    hdr->dimensions.width = width;
                    hdr->dimensions.height = height;
                    SetDimensions(*hdr);
                }
                else if (t->m_rv.pwidth != width || t->m_rv.pheight != height)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                             "Unmatched PCTSZ"));
                    return FALSE;
                }
            }

            /* MBA */
            t->m_rv.mba = CB_GetBits(t,t->m_mbaSize);

            /* RTYPE */
            /* RTYPE not used anymore.  This code is hear to provide */
            /* compatibility with version sent to Intel. */
            CB_GetBits(this,FIELDLEN_MPPTYPE_ROUNDING);

#else /* USE_INTEL_VLC */

            /* RealNetworks Slice Header */

            /* ECC */
            if (CB_Get1Bit(t) != 0)
            {
                RVDebug((RV_TL_SUBPICT, "WARNING GetSliceHeader_RV9 - "
                         "ECC packet"));
                return FALSE;
            }

            /* PTYPE */
            if (bFirst)
                t->m_rv.ptype = (EnumRVPicCodType) CB_GetBits(t,2);
            else if ((EnumRVPicCodType) CB_GetBits(t,2) != t->m_rv.ptype)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unmatched PTYPE"));
                return FALSE;
            }

            /* PQUANT/SQUANT */
            t->m_rv.pquant = (U8) CB_GetBits(t,FIELDLEN_SQUANT);
            /* 0-31 are legitimate QP values for RV89Combo */
            if (t->m_rv.pquant > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "SQUANT %d > 31", t->m_rv.pquant));
                return FALSE;
            }

            /* Bitstream version */
            if (CB_Get1Bit(t) != 0)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Unrecognized RV bitstream version"));
                return FALSE;
            }

#ifdef INTERLACED_CODE
#ifdef COMPATIBILITY_BITSTREAM
            t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
            if(t->m_rv.interlaced) t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            t->m_rv.x10 = FALSE;
            /* Repeat Field */
            t->m_rv.RepeatField = FALSE;
            t->m_rv.DeblockingQpAdjust = 0;
#else
            /* x10 Header */
            if((CB_Get1Bit(t) ? TRUE : FALSE))
            {
                t->m_rv.x10 = TRUE;
                t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
                t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
                /* repeat field */
                t->m_rv.RepeatField = (CB_Get1Bit(t)? TRUE : FALSE);
                /* x10 bitstream version */
                CB_Get1Bit(t);
                /* Deblocking Qp Adjust */
                if(CB_Get1Bit(t))
                {
                    t->m_rv.DeblockingQpAdjust = CB_GetBits(t,2);
                    t->m_rv.DeblockingQpAdjust = t->m_rv.DeblockingQpAdjust +2;
                }
                else
                {
                    t->m_rv.DeblockingQpAdjust = 0;
                }
            }
            else
            {
                t->m_rv.x10 = FALSE;
                /* Interlaced */
                t->m_rv.interlaced = FALSE;
                t->m_rv.interlacedTopFieldFirst = TRUE;
                /* Repeat Field */
                t->m_rv.RepeatField = FALSE;
                t->m_rv.DeblockingQpAdjust = 0;
            }
#endif
#else
            if (CB_Get1Bit(t))
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "Interlaced bit set."));
                return FALSE;
            }
#endif

            /* OSV Quant */
            hdr->OSVQUANT = (U8) CB_GetBits(t,2);

            /* In-loop deblocking */
            t->m_rv.deblocking_filter_passthrough = CB_GetBits(t,1)?TRUE:FALSE;

            fieldlen = FIELDLEN_TR_RV;

            if (bFirst)
                t->m_rv.tr = CB_GetBits(t,fieldlen);
            else if (CB_GetBits(t,fieldlen) != t->m_rv.tr)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV8 - "
                         "Unmatched TR"));
                return FALSE;
            }

            /* PCTSZ (RealVideo RPR picture size) */
            {
                U32 width, height;

                if (bFirst)
                {
                    switch (t->m_rv.ptype)
                    {
                        case RV_INTRAPIC:
                        case RV_FORCED_INTRAPIC:
                            t->m_rv.pwidth_prev  = 0;
                            t->m_rv.pheight_prev = 0;
                            break;
                        case RV_INTERPIC:
                            t->m_rv.pwidth_prev  = t->m_rv.pwidth;
                            t->m_rv.pheight_prev = t->m_rv.pheight;
                            break;
                        case RV_TRUEBPIC: //sheen
                            break;
                    }
                }

                CB_GetPicSize(t, &width, &height);

                if (bFirst)
                {
                    t->m_rv.pwidth = width;
                    t->m_rv.pheight = height;
                    /*                  hdr->pixel_aspect_ratio = PARC_SQUARE; */
                    /* TBD, why is parc set here? */
                    hdr->dimensions.width = width;
                    hdr->dimensions.height = height;
                    CB_SetDimensions(t,hdr);
                }
                else if (t->m_rv.pwidth != width || t->m_rv.pheight != height)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                             "Unmatched PCTSZ"));
                    return FALSE;
                }
            }

            /* MBA */
            t->m_rv.mba = CB_GetBits(t,t->m_mbaSize);

#endif /* USE_INTEL_VLC */
        }

        *iSliceMBA  = t->m_rv.mba;
        *iSQUANT    = t->m_rv.pquant;
        *iSliceGFID = 0;
    }
    else
    {
        /* The SSC either does not exist because this is the first slice */
        /* after the picture start code, or the SSC has already been read */
        /* in IAPass1. */

        if (!bFirst)
        {

            /* Must be 1 to prevent start code emulation (SEPB1) */
            if (CB_Get1Bit(t) != 1)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
                return FALSE;
            }

            /* No SSBI since CPM can not be "1". */

#ifdef INTERLACED_CODE
#ifdef COMPATIBILITY_BITSTREAM
            t->m_rv.interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
            if(t->m_rv.interlaced) t->m_rv.interlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
            t->m_rv.x10 = FALSE;
            /* Repeat Field */
            t->m_rv.RepeatField = FALSE;
            t->m_rv.DeblockingQpAdjust = 0;
#else


            if((CB_Get1Bit(t) ? TRUE : FALSE))
            {
                hdr->x10        = TRUE;
                hdr->Interlaced = (CB_Get1Bit(t) ? TRUE : FALSE);
                hdr->InterlacedTopFieldFirst = (CB_Get1Bit(t))?(TRUE):(FALSE);
                /* repeat field */
                hdr->RepeatField = (CB_Get1Bit(t)? TRUE : FALSE);
                /* x10 bitstream version */
                CB_Get1Bit(t);
                /* Deblocking Qp Adjust */
                if(CB_Get1Bit(t))
                {
                    hdr->DeblockingQpAdjust = CB_GetBits(t,2);
                    hdr->DeblockingQpAdjust = hdr->DeblockingQpAdjust +2;
                }
                else
                {
                    hdr->DeblockingQpAdjust = 0;
                }
            }
            else
            {
                hdr->x10        = FALSE;
                /* Interlaced */
                hdr->Interlaced = FALSE;
                hdr->InterlacedTopFieldFirst = TRUE;
                /* Repeat Field */
                hdr->RepeatField = FALSE;
                hdr->DeblockingQpAdjust = 0;
            }
#endif
#endif

            /* Get macro-block address */
            *iSliceMBA = CB_GetBits(t,t->m_mbaSize);

            if (t->m_mbaSize > 11)
            {
                /* Must be 1 to prevent start code emulation (SEPB2) */
                if (CB_Get1Bit(t) != 1)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
                    return FALSE;
                }
            }

            /* Get SQUANT */
            *iSQUANT = CB_GetBits(t,FIELDLEN_SQUANT);

            /* 0-31 are legitimate QP values for RV89Combo */
            if (*iSQUANT > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - "
                         "SQUANT %d > 31", *iSQUANT));
                return FALSE;
            }

            /* OSV Quant */
            hdr->OSVQUANT = (U8) CB_GetBits(t,2);

            /* No SWI field. */

            /* Must be 1 to prevent start code emulation (SEPB3) */
            if (CB_Get1Bit(t) != 1)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader_RV9 - Start code emulation error"));
                return FALSE;
            }

            /* Get GOB frame ID. */
            *iSliceGFID = CB_GetBits(t,FIELDLEN_GFID);

            RVDebug((RV_TL_SUBPICT,
                     "CB_GetSliceHeader_RV8 - MBA=%ld SQUANT=%ld GFID=%ld",
                     *iSliceMBA, *iSQUANT, *iSliceGFID));

        }
        else
        {
            *iSliceMBA = 0;
            *iSQUANT = -1;
        }

    }

    return TRUE;

} /* CB_GetSliceHeader() */
#endif


/* --------------------------------------------------------------------------- */
/*  CB_GetSliceHeader() */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_GetSliceHeader_RV8(
    struct CRealVideoBs *t,
    RV_Boolean  bFirst,
    I32*        iSliceMBA,
    I32*        iSliceGFID,
    I32*        iSQUANT,
    struct PictureHeader* hdr)
{
    U32 versionEncoding;
    U32 fieldlen;

    if (t->m_fid == RV_FID_REALVIDEO30)
    {
        /* skip if this is the first slice called from IAPass1ProcessFrame */
        /* otherwise read the slice header */
        if (!bFirst)
        {

            /* this was called from GetPicHeader */
            if (t->m_rv.mba == -1)
                bFirst = TRUE;

            /* Bitstream version */
            versionEncoding = CB_GetBits(t,FIELDLEN_RV_BITSTREAM_VERSION);
            if (versionEncoding >= NUMBER_OF_RV_BITSTREAM_VERSIONS
                    || s_RVVersionEncodingToMinorVersion[versionEncoding]
                    > ENCODERS_CURRENT_RV_BITSTREAM_VERSION
               )
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                         "Unrecognized RV bitstream version %ld",
                         versionEncoding));
                return FALSE;
            }
            /* Eventually, store the bitstream version in m_rv. */
            /* Currently not necessary since we only support one */
            /* bitstream version. */

            /* PTYPE */
            if (bFirst)
                t->m_rv.ptype = (EnumRVPicCodType) CB_GetBits(t,2);
            else if ((EnumRVPicCodType) CB_GetBits(t,2) != t->m_rv.ptype)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                         "Unmatched PTYPE"));
                return FALSE;
            }

            /* ECC */
            CB_Get1Bit(t);

            /* PQUANT/SQUANT */
            t->m_rv.pquant = (U8) CB_GetBits(t,FIELDLEN_SQUANT);

            /* 0-31 are legitimate QP values for RV89Combo */
            if (t->m_rv.pquant > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                         "SQUANT %d > 31", t->m_rv.pquant));
                return FALSE;
            }

            /* In-loop deblocking */
            t->m_rv.deblocking_filter_passthrough = CB_GetBits(t,1)?TRUE:FALSE;

            fieldlen = FIELDLEN_TR_RV;

            if (bFirst)
                t->m_rv.tr = CB_GetBits(t,fieldlen);
            else if (CB_GetBits(t,fieldlen) != t->m_rv.tr)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                         "Unmatched TR"));
                return FALSE;
            }

            /* PCTSZ (RealVideo RPR picture size) */
            if (t->m_pctszSize > 0)
            {
                if (bFirst)
                {
                    U32 width, height;
                    t->m_rv.pctsz = (U8)CB_GetBits(t,t->m_pctszSize);

                    if (t->m_pSizes != NULL)
                    {
                        width = t->m_pSizes[2*t->m_rv.pctsz];
                        height = t->m_pSizes[2*t->m_rv.pctsz+1];
                        /*                        hdr->pixel_aspect_ratio = PARC_SQUARE; */
                        /* TBD, why is parc set here? */
                        hdr->dimensions.width = width;
                        hdr->dimensions.height = height;
                        CB_SetDimensions(t,hdr);
                    }
                    else
                    {
                        RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                                 "t->m_pSizes array not initialized"));
                        return FALSE;
                    }
                }
                else if (CB_GetBits(t,t->m_pctszSize) != t->m_rv.pctsz)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                             "Unmatched PCTSZ"));
                    return FALSE;
                }
            }

            /* MBA */
            t->m_rv.mba = CB_GetBits(t,t->m_mbaSize);

            /* RTYPE */
            if (bFirst)
                t->m_rv.rtype = (RV_Boolean) CB_GetBits(t,FIELDLEN_MPPTYPE_ROUNDING);
            else if ((RV_Boolean) CB_GetBits(t,FIELDLEN_MPPTYPE_ROUNDING) != t->m_rv.rtype)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                         "Unmatched RTYPE"));
                return FALSE;
            }
        }

        *iSliceMBA  = t->m_rv.mba;
        *iSQUANT    = t->m_rv.pquant;
        *iSliceGFID = 0;
    }
    else
    {
        /* The SSC either does not exist because this is the first slice */
        /* after the picture start code, or the SSC has already been read */
        /* in IAPass1. */

        if (!bFirst)
        {

            /* Must be 1 to prevent start code emulation (SEPB1) */
            if (CB_Get1Bit(t) != 1)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - Start code emulation error"));
                return FALSE;
            }

            /* No SSBI since CPM can not be "1". */

            /* Get macro-block address */
            *iSliceMBA = CB_GetBits(t,t->m_mbaSize);

            if (t->m_mbaSize > 11)
            {
                /* Must be 1 to prevent start code emulation (SEPB2) */
                if (CB_Get1Bit(t) != 1)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - Start code emulation error"));
                    return FALSE;
                }
            }

            /* Get SQUANT */
            *iSQUANT = CB_GetBits(t,FIELDLEN_SQUANT);

            /* 0-31 are legitimate QP values for RV89Combo */
            if (*iSQUANT > 31)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - "
                         "SQUANT %d > 31", *iSQUANT));
                return FALSE;
            }

            /* No SWI field. */

            /* Must be 1 to prevent start code emulation (SEPB3) */
            if (CB_Get1Bit(t) != 1)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR GetSliceHeader - Start code emulation error"));
                return FALSE;
            }

            /* Get GOB frame ID. */
            *iSliceGFID = CB_GetBits(t,FIELDLEN_GFID);

            RVDebug((RV_TL_SUBPICT,
                     "CB_GetSliceHeader - MBA=%ld SQUANT=%ld GFID=%ld",
                     *iSliceMBA, *iSQUANT, *iSliceGFID));

        }
        else
        {
            *iSliceMBA = 0;
            *iSQUANT = -1;
        }

    }

    return TRUE;

} /* CB_GetSliceHeader_RV8() */



/* --------------------------------------------------------------------------- */
/*  CB_SetDimensions() */
/*      use in the decoder to initialize size, m_nGOBs, m_nMBs, and m_mbaSize */
/* --------------------------------------------------------------------------- */

/* for PutSliceHeader              SQCIF, QCIF, CIF, 4CIF, 16CIF, 2048x1152 */
const U32 MBA_NumMBs[]     = { 47,   98, 395, 1583,  6335, 9215 };
const U32 MBA_FieldWidth[] = {  6,    7,   9,   11,    13,   14 };
const U32 MBA_Widths =
    sizeof( MBA_FieldWidth ) / sizeof( MBA_FieldWidth[0] );

#if 0//move to rv_overlay_set.c sheen

void CB_SetDimensions(struct CRealVideoBs *t, struct PictureHeader *hdr)
{
    U32 i,j;
    /* Set m_nMBs and m_nGOBs based on the image dimensions. */
    /* TBD, there's probably a formula that can be used here instead */
    /* of this nested if-then-else sequence. */

    if (hdr->dimensions.width == 128 && hdr->dimensions.height == 96)
    {
        t->m_nMBs   = 8;
        t->m_nGOBs  = 6;
    }
    else if (hdr->dimensions.width == 176 && hdr->dimensions.height == 144)
    {
        t->m_nMBs   = 11;
        t->m_nGOBs  = 9;
    }
    else if (hdr->dimensions.width == 352 && hdr->dimensions.height == 288)
    {
        t->m_nMBs   = 22;
        t->m_nGOBs  = 18;
    }
    else if (hdr->dimensions.width == 704 && hdr->dimensions.height == 576)
    {
        t->m_nMBs   = 44;
        t->m_nGOBs  = 36;
    }
    else if (hdr->dimensions.width == 1408 && hdr->dimensions.height == 1152)
    {
        t->m_nMBs   = 88;
        t->m_nGOBs  = 72;
    }
    else
    {
        t->m_nMBs  = (hdr->dimensions.width + 15) >> 4;
        t->m_nGOBs = (hdr->dimensions.height + 15) >> 4;
    }

    /* initialize m_mbaSize */

    j = t->m_nGOBs * t->m_nMBs - 1;
    for (i = 0; i < (MBA_Widths - 1) && MBA_NumMBs[i] <  j; i++)
        ;

    t->m_mbaSize = MBA_FieldWidth[i];

} /* CB_SetDimensions() */

/* --------------------------------------------------------------------------- */
/*  CB_LossRecovery() */
/*  used by the recv side to recover from a packet loss */
/*  Returns: */
/*      BSLR_STATUS_OK      Packet loss recovered from */
/*      BSLR_STATUS_ERROR   Packet loss not recognized */
/* --------------------------------------------------------------------------- */

BSLR_Status CB_LossRecovery(struct CRealVideoBs *t, RV_Boolean bUpdateState)
{
    U32 offset1, offset2, i, maxBitLimit;
    BSLR_Status status = BSLR_STATUS_OK;

    if (t->m_nSlices)
    {
        maxBitLimit = CB_GetMaxBsSize(t) << 3;
        offset1 = CB_GetBsOffset(t);
        offset2 = maxBitLimit;

        for (i = 1; i < t->m_nSlices; i ++)
        {
            if (SliceFlag(i) && offset1 <= SliceBitOffset(i))
            {
                offset2 = SliceBitOffset(i);
                break;
            }
        }

        if (offset2 == maxBitLimit)
            status = BSLR_STATUS_EOS;

        if (bUpdateState)
            if (offset2 > offset1)
                CB_UpdateState(t,offset2 - offset1);
    }
    else
        status = BSLR_STATUS_ERROR;

    return status;

} /* CB_LossRecovery() */
#endif


/* --------------------------------------------------------------------------- */
/*  [DEC] CB_IsDataValid() */
/*  Checks if data associated with the given offset located within a valid  */
/*  packet or a valid segment. */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_IsDataValid(struct CRealVideoBs *t, const U32 offset)
{
    U32 i;
    /* if segment information is not available, we can't verify the data,  */
    /* just return TRUE */
    if (!t->m_nSlices)
        return TRUE;

    if (offset < SliceBitOffset(0) || offset >= (CB_GetMaxBsSize(t) << 3))
        return FALSE;

    for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++) {};

    return((RV_Boolean) SliceFlag(i - 1));

} /* CB_IsDataValid() */

#if 0//move to rv_overlay_set.c sheen
/* --------------------------------------------------------------------------- */
/*  [DEC] CB_IsSegmentValid() */
/*  This should be called before decoding each MB to determine if the current */
/*  bitstream pointer is pointing inside a valid segment/packet.  If the  */
/*  pointer has reached the end of a segment, forward the pointer to the next  */
/*  segment to be decoded and check if it is valid. */
/* --------------------------------------------------------------------------- */

RV_Boolean CB_IsSegmentValid(struct CRealVideoBs *t)
{
    U32 offset,maxBitLimit,delta_offset,i;
    /* in the case where segment information is not available, assume each */
    /* segment given to the is properly formatted (with header and decodable */
    /* data, or 4 bytes of 0's which will be caught by the decoder later) */
    if (!t->m_nSlices)
        return TRUE;

    offset = CB_GetBsOffset(t);
    maxBitLimit = CB_GetMaxBsSize(t) << 3;
    if (offset < SliceBitOffset(0) || offset >= maxBitLimit)
        return FALSE;

    /* locate current slice, i - 1 */
    for (i = 1; i < t->m_nSlices && offset >= SliceBitOffset(i); i ++) {};

    //delta_offset; //sheen
    if (i == t->m_nSlices)
    {
        /* this is the last slice */
        /* skip stuffing if any */
        delta_offset = maxBitLimit - offset;
        if (delta_offset < 8 && CB_SearchBits(t,delta_offset, 0, 0))
            return FALSE;
    }
    else
    {
        /* this is not the last slice */
        /* skip stuffing if any */
        delta_offset = SliceBitOffset(i) - offset;
        if (delta_offset < 8 && CB_SearchBits(t,delta_offset, 0, 0))
            return((RV_Boolean) SliceFlag(i));
    }
    return((RV_Boolean) SliceFlag(i - 1));

} /* CB_IsSegmentValid() */
#endif


