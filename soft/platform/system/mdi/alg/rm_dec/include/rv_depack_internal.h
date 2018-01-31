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



#ifndef RV_DEPACK_INTERNAL_H
#define RV_DEPACK_INTERNAL_H

#include "helix_types.h"
#include "helix_result.h"
#include "rm_memory.h"
#include "rm_error.h"
#include "rv_depack.h"
#include "stream_hdr_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Frame type enum */
typedef enum
{
    RVFrameTypePartial,
    RVFrameTypeWhole,
    RVFrameTypeLastPartial,
    RVFrameTypeMultiple
} RVFrameType;

/* Struct which holds frame header info */
typedef struct rv_frame_hdr_struct
{
    RVFrameType eType;
    UINT32      ulPacketNum;
    UINT32      ulNumPackets;
    UINT32      ulFrameSize;
    UINT32      ulPartialFrameSize;
    UINT32      ulPartialFrameOffset;
    UINT32      ulTimestamp;
    UINT32      ulSeqNum;
    UINT32      ulHeaderSize;
    UINT32      ulHeaderOffset;
    HXBOOL      bBrokenUpByUs;
} rv_frame_hdr;

/*
 * Internal rv_depack struct
 */
typedef struct rv_depack_internal_struct
{
    void*                   pAvail;
    rv_frame_avail_func_ptr fpAvail;
    rm_error_func_ptr       fpError;
    void*                   pUserError;
    rm_malloc_func_ptr      fpMalloc;
    rm_free_func_ptr        fpFree;
    void*                   pUserMem;
    UINT32                  ulZeroTimeOffset;
    HX_BITFIELD             bHasRelativeTimeStamps;
    rm_rule_map             rule2Flag;
    rm_multistream_hdr      multiStreamHdr;
    rv_format_info*         pSubStreamHdr;
    HXBOOL*                 bIgnoreSubStream;
    HXBOOL                  bStreamSwitchable;
    UINT32                  ulActiveSubStream;
    rv_frame*               pCurFrame;
    HXBOOL                  bBrokenUpByUs;
    HXBOOL                  bCreatedFirstFrame;
    UINT32                  ulLastSeqNumIn;
    UINT32                  ulLastSeqNumOut;
} rv_depack_internal;

/*
 * Internal rv_depack functions
 */
void*     rv_depacki_malloc(rv_depack_internal* pInt, UINT32 ulSize);
void      rv_depacki_free(rv_depack_internal* pInt, void* pMem);
HX_RESULT rv_depacki_init(rv_depack_internal* pInt, rm_stream_header* hdr);
HX_RESULT rv_depacki_unpack_rule_map(rv_depack_internal* pInt,
                                     rm_rule_map*        pMap,
                                     BYTE**              ppBuf,
                                     UINT32*             pulLen);
HX_RESULT rv_depacki_unpack_multistream_hdr(rv_depack_internal* pInt,
        BYTE**              ppBuf,
        UINT32*             pulLen);
HX_RESULT rv_depacki_unpack_opaque_data(rv_depack_internal* pInt,
                                        BYTE*               pBuf,
                                        UINT32              ulLen);
void      rv_depacki_cleanup_format_info(rv_depack_internal* pInt,
        rv_format_info*     pInfo);
void      rv_depacki_cleanup_format_info_array(rv_depack_internal* pInt);
HX_RESULT rv_depacki_unpack_format_info(rv_depack_internal* pInt,
                                        rv_format_info*     pInfo,
                                        BYTE**              ppBuf,
                                        UINT32*             pulLen);
HX_RESULT rv_depacki_check_rule_book(rv_depack_internal* pInt,
                                     rm_stream_header*   hdr);
HX_RESULT rv_depacki_copy_format_info(rv_depack_internal* pInt,
                                      rv_format_info*     pSrc,
                                      rv_format_info*     pDst);
HX_RESULT rv_depacki_add_packet(rv_depack_internal* pInt,
                                rm_packet*          pPacket);
UINT32    rv_depacki_rule_to_flags(rv_depack_internal* pInt, UINT32 ulRule);
UINT32    rv_depacki_rule_to_substream(rv_depack_internal* pInt, UINT32 ulRule);
HX_RESULT rv_depacki_parse_frame_header(rv_depack_internal*  pInt,
                                        BYTE**               ppBuf,
                                        UINT32*              pulLen,
                                        rm_packet*           pPacket,
                                        rv_frame_hdr* pFrameHdr);
HX_RESULT rv_depacki_read_14_or_30(BYTE**  ppBuf,
                                   UINT32* pulLen,
                                   HXBOOL* pbHiBit,
                                   UINT32* pulValue);
HX_RESULT rv_depacki_handle_partial(rv_depack_internal*  pInt,
                                    BYTE**               ppBuf,
                                    UINT32*              pulLen,
                                    rm_packet*           pPacket,
                                    rv_frame_hdr* pFrameHdr);
HX_RESULT rv_depacki_handle_one_frame(rv_depack_internal*  pInt,
                                      BYTE**               ppBuf,
                                      UINT32*              pulLen,
                                      rm_packet*           pPacket,
                                      rv_frame_hdr* pFrameHdr);
HX_RESULT rv_depacki_send_current_frame(rv_depack_internal* pInt);
void      rv_depacki_cleanup_frame(rv_depack_internal* pInt,
                                   rv_frame**   ppFrame);
HX_RESULT rv_depacki_create_frame(rv_depack_internal*  pInt,
                                  rm_packet*           pPacket,
                                  rv_frame_hdr* pFrameHdr,
                                  rv_frame**    ppFrame);
void      rv_depacki_check_to_clear_keyframe_flag(BYTE*                pBuf,
        UINT32               ulLen,
        rm_packet*           pkt,
        rv_frame_hdr* hdr);
HX_RESULT rv_depacki_seek(rv_depack_internal* pInt, UINT32 ulTime);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef RV_DEPACK_INTERNAL_H */
