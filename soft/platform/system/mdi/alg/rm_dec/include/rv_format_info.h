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



#ifndef RV_FORMAT_INFO_H
#define RV_FORMAT_INFO_H

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

#include "helix_types.h"

typedef struct rv_format_info_struct
{
    UINT32   ulLength;
    UINT32   ulMOFTag;
    UINT32   ulSubMOFTag;
    UINT16   usWidth;
    UINT16   usHeight;
    UINT16   usBitCount;
    UINT16   usPadWidth;
    UINT16   usPadHeight;
    UFIXED32 ufFramesPerSecond;
    UINT32   ulOpaqueDataSize;
    BYTE*    pOpaqueData;
} rv_format_info;

/*
 * RV frame struct.
 */
typedef struct rv_segment_struct
{
    HXBOOL bIsValid;
    UINT32 ulOffset;
} rv_segment;

typedef struct rv_frame_struct
{
    UINT32             ulDataLen;
    BYTE*              pData;
    UINT32             ulTimestamp;
    UINT16             usSequenceNum;
    UINT16             usFlags;
    HXBOOL             bLastPacket;
    UINT32             ulNumSegments;
    rv_segment* pSegment;
} rv_frame;

#define BYTE_SWAP_UINT16(A)  ((((UINT16)(A) & 0xff00) >> 8) | \
                              (((UINT16)(A) & 0x00ff) << 8))
#define BYTE_SWAP_UINT32(A)  ((((UINT32)(A) & 0xff000000) >> 24) | \
                              (((UINT32)(A) & 0x00ff0000) >> 8)  | \
                              (((UINT32)(A) & 0x0000ff00) << 8)  | \
                              (((UINT32)(A) & 0x000000ff) << 24))

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef RV_FORMAT_INFO_H */
