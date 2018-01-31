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


















#ifndef BSX_H__
#define BSX_H__

#include "beginhdr.h"
/*#include "cdrvdefs.h" */

#include "rvtypes.h"
#include "rvstatus.h"
#include "rvstruct.h"
/*#include "rvmain.h" */


#ifdef __cplusplus
extern "C" {
#endif

/* LossRecovery return status */
typedef enum
{
    BSLR_STATUS_ERROR,  /* Loss recovery did not detect lost packet, or */
    /* couldn't recover. */
    BSLR_STATUS_OK,     /* Loss recovery detected lost packet and recovered */
    BSLR_STATUS_EOS     /* Loss recovery detected lost packet but end of */
    /* stream encountered (i.e. no more packet avail.) */
} BSLR_Status;

typedef enum
{
    RV_INTRAPIC,        /* 0 (00) */
    RV_FORCED_INTRAPIC, /* 1 (01) */
    RV_INTERPIC,        /* 2 (10) */
    RV_TRUEBPIC         /* 3 (11) */
} EnumRVPicCodType;



typedef struct
{
    Bool32      is_valid;
    U32         offset;
} Bitstream_Segment_Info;
/* The Bitstream_Segment_Info structure *MUST* be structurally equivalent */
/* to the PNCODEC_SEGMENTINFO structure defined in the RealVideo */
/* front-end. */




/* real-video picture header format */
typedef struct
{
    EnumRVPicCodType    ptype;
    RV_Boolean          rtype; /* RV8 */
    RV_Boolean          deblocking_filter_passthrough;
    I32                 mba;
    U32                 tr;
    U8                  pquant;
    U8                  trb;
    U8                  dbq;
    U8                  pctsz; /* RV8 */
    U32                 pwidth;
    U32                 pheight;
    U32                 pwidth_prev;
    U32                 pheight_prev;
#ifdef INTERLACED_CODE
    RV_Boolean          x10;
    RV_Boolean          interlaced;
    RV_Boolean          interlacedTopFieldFirst;
    RV_Boolean          RepeatField;
    U32                 DeblockingQpAdjust;
#endif

} RV_BS_Info;

struct CRealVideoBs
{
    /* m_pbs points to the current position of the buffer. */
    PU8 m_pbs;

    /* m_pbsBase points to the first byte of the buffer. */
    PU8 m_pbsBase;

    /* Indicates the bit position (0 to 7) in the byte pointed by m_pbs. */
    U32 m_bitOffset;

    /* Indicates if the associated buffer is locally owned by the object. */
    /* If true, the destructor will need to take care of deallocation. */
    /* Note: This is always FALSE in our current usage of the bitstream */
    /*       objects in the ILVC codec. */
    RV_Boolean m_localBs;

    /* Maximum buffer size in bytes. */
    U32 m_maxBsSize;

    U32     m_fid;          /* stream type */

    RV_Boolean  m_slice;        /* slice option flag */
    RV_Boolean m_isForcedKey;   /* forced keyframe */

    U32         m_nGOBs;        /* number of GOBs per frame */
    U32         m_nMBs;         /* number of MBs per frame */
    U32         m_mbaSize;      /* size of MBA field */

    U32         m_gob;          /* current gob index */
    U32         m_MBRowNum;     /* TRGardos - current MB row number. */
    U32         m_mb;           /* current mb relative to current gob */
    U32         m_mbID;         /* current mb relative to entire frame */

    U32         m_quant;        /* these are for H.263 RTP extension */
    I8          m_hmv1;
    I8          m_vmv1;
    I8          m_hmv2;
    I8          m_vmv2;

    RV_BS_Info  m_rv;           /* RV2 bitstream picture header info */

    /* slice information for packetization w/o rtp extension */
    U32                 m_nSlices;
    Bitstream_Segment_Info  *m_sliceInfo;

    /* tracks the base ptr, used to size the current slice */
    U8*         m_pbsSliceBase;

    /* size of PCTSZ (RealVideo RPR size index) */
    U32         m_pctszSize;

    /* number of RPR sizes */
    U32         m_numSizes;

    /* location of RealVideo RPR size array */
    U32         *m_pSizes;
};


/* macros for getting slice valid flag and slice offset */
#define SliceFlag(x)        (t->m_sliceInfo[(x)].is_valid)
#define SliceOffset(x)      (t->m_sliceInfo[(x)].offset)
#define SliceBitOffset(x)   (SliceOffset(x) << 3)

#define FIELDLEN_SSC 24         /* Slice start code */
#define FIELDLEN_SSC_RV8 17     /* Slice start code */




/**/
/* Functions from CBaseBitstream */
/**/


/* Constructs a new object.  Sets base pointer to point to the
   bitstream buffer with given buffer size. */
void CB_CBaseBitstream(
    struct CRealVideoBs *t,
    U8* const           pbs,
    const U32           maxsize);

/* Returns the size of the bitstream data in bytes based on the
   current position of the buffer pointer, m_pbs.*/
U32 CB_GetBsSize(struct CRealVideoBs *t);

/* Returns the bit position of the buffer pointer relative to the
   beginning of the buffer. */
U32 CB_GetBsOffset(struct CRealVideoBs *t);

/* Returns the base pointer to the beginning of the bitstream.*/
PU8 CB_GetBsBase(struct CRealVideoBs *t);

/* Returns the maximum bitstream size.*/
U32 CB_GetMaxBsSize(struct CRealVideoBs *t);

/* Checks if read/write passed the maximum buffer size.*/
RV_Boolean CB_CheckBsLimit(struct CRealVideoBs *t);


/* Obtains current position of the buffer pointer.  This is only*/
/* used to maintain the bitstream state when calling an assembly  */
/* routine that accesses the bitstream.*/
void CB_GetState(
    struct CRealVideoBs *t,
    U8**        pbs,
    U32*        bitOffset);

/* Advances buffer pointer with given number of bits.  This is only*/
/* used to maintain the bitstream state when calling an assembly  */
/* routine that accesses the bitstream.*/
void CB_UpdateState(
    struct CRealVideoBs *t,
    const U32   nbits);

/* Reads one general VLC code from bitstream and returns its length.*/
/* The info portion of the code is returned via the 'info' paramter.*/
U32 CB_GetVLCBits(struct CRealVideoBs *t, U32 *info);


/* Resets pointer to point to a new bitstream.*/
void CB_Reset(
    struct CRealVideoBs *t,
    U8* const   pb,
    const U32   maxsize);

/* Reads bits from buffer.*/
U32 CB_GetBits(
    struct CRealVideoBs *t,
    const U32   nbits);

/* Reads one bit from the buffer.*/
U32 CB_Get1Bit(struct CRealVideoBs *t);

/* Searches for a code with known number of bits. */
RV_Boolean  CB_SearchBits(
    struct CRealVideoBs *t,
    const U32   nbits,
    const U32   code,
    const U32   lookahead);

/* Searches for a code with known number of bits.  Skips only zero bits.*/
/* Fails search if code has not been found and non-zero bit encountered.*/
RV_Boolean  CB_SearchBits_IgnoreOnly0(
    struct CRealVideoBs *t,
    const U32   nbits,
    const U32   code,
    const U32   lookahead);


/**/
/**/
/**/



/*
 *  Start of C translated portion
 */




U32             CB_GetSuperCBP(struct CRealVideoBs *t, U32 mbType, U32 QP);
#if 0//hx chg
void            CB_DecodeSuperSubBlockCoefficients
(
    struct CRealVideoBs *t,
    I16    *pDst,
    U32     mbType,
    RV_Boolean    isChroma,
    U32     QP
);
#endif

RV_Boolean      CB_GetCPFMT(struct CRealVideoBs *t, struct PictureHeader *);


void CB_Init1(
    struct CRealVideoBs *t,
    const RV_FID    fid,
    U8* const       pb,
    const U32       maxsize);

void CB_Init2(
    struct CRealVideoBs *t,
    const RV_FID    fid);

void CB_Delete(struct CRealVideoBs *t);


void            CB_Reset(
    struct CRealVideoBs *t,
    U8* const   pb,
    const U32   maxsize);


RV_Boolean      CB_GetForcedKeyState(struct CRealVideoBs *t);

void            CB_GetSliceOffsets(
    struct CRealVideoBs *t,
    U32                *nOffsets,
    Bitstream_Segment_Info *pSegInfo);

void            CB_PutSliceOffsets(
    struct CRealVideoBs *t,
    const U32          nOffsets,
    Bitstream_Segment_Info *pOffsets);

U32             CB_GetNumSlices(struct CRealVideoBs *t);

RV_Boolean      CB_IsSliceValid(struct CRealVideoBs *t,I32 iSliceNum);

void            CB_SetSliceOffset(struct CRealVideoBs *t,I32 iSliceNum);


void            CB_copyState(struct CRealVideoBs *t,struct CRealVideoBs *pBs);

void            CB_GetPicSize(struct CRealVideoBs *t,U32 *width, U32 *height);



void            CB_GetDQUANT(
    struct CRealVideoBs *t,
    U8  *quant,
    const U8    quant_prev,
    const RV_Boolean mq);

void            CB_SetRPRSizes(struct CRealVideoBs *t, U32, U32*);

RV_Status       CB_GetPictureHeader(struct CRealVideoBs *t, struct PictureHeader* hdr);
RV_Status       CB_GetPictureHeader_RV8(struct CRealVideoBs *t, struct PictureHeader* hdr);


RV_Boolean      CB_GetSSC(struct CRealVideoBs *t);
RV_Boolean      CB_GetSSC_RV8(struct CRealVideoBs *t);

RV_Boolean      CB_GetSliceHeader(
    struct CRealVideoBs *t,
    RV_Boolean  bFirst,
    I32*        iMBA,
    I32*        iGFID,
    I32*        iSQUANT,
    struct PictureHeader *hdr);
RV_Boolean      CB_GetSliceHeader_RV8(
    struct CRealVideoBs *t,
    RV_Boolean  bFirst,
    I32*        iMBA,
    I32*        iGFID,
    I32*        iSQUANT,
    struct PictureHeader *hdr);

BSLR_Status     CB_LossRecovery(struct CRealVideoBs *t, RV_Boolean bUpdateState);

RV_Boolean      CB_IsDataValid(
    struct CRealVideoBs *t,
    const U32   offset);

RV_Boolean      CB_IsSegmentValid(struct CRealVideoBs *t);

void            CB_SetFID(struct CRealVideoBs *t, const RV_FID fid);

void            CB_SetDimensions(struct CRealVideoBs *t, struct PictureHeader *hdr);


#ifdef __cplusplus
}
#endif


#endif /* BSX_H__ */
