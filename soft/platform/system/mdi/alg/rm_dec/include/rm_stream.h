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



#ifndef RM_STREAM_H
#define RM_STREAM_H

#include "helix_types.h"
#include "helix_result.h"
#include "rm_property.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

/*
 * Stream header definition.
 *
 * Users are strongly encouraged to use the accessor
 * functions below to retrieve information from the
 * rm_stream_header struct, since the definition
 * may change in the future.
 */
typedef struct rm_stream_header_struct
{
    char*        pMimeType;
    char*        pStreamName;
    UINT32       ulStreamNumber;
    UINT32       ulMaxBitRate;
    UINT32       ulAvgBitRate;
    UINT32       ulMaxPacketSize;
    UINT32       ulAvgPacketSize;
    UINT32       ulDuration;
    UINT32       ulPreroll;
    UINT32       ulStartTime;
    UINT32       ulOpaqueDataLen;
    BYTE*        pOpaqueData;
    UINT32       ulNumProperties;
    rm_property* pProperty;
} rm_stream_header;

/*
 * These are the accessor functions used to retrieve
 * information from the stream header
 */
UINT32      rm_stream_get_number(rm_stream_header* hdr);
UINT32      rm_stream_get_max_bit_rate(rm_stream_header* hdr);
UINT32      rm_stream_get_avg_bit_rate(rm_stream_header* hdr);
UINT32      rm_stream_get_max_packet_size(rm_stream_header* hdr);
UINT32      rm_stream_get_avg_packet_size(rm_stream_header* hdr);
UINT32      rm_stream_get_start_time(rm_stream_header* hdr);
UINT32      rm_stream_get_preroll(rm_stream_header* hdr);
UINT32      rm_stream_get_duration(rm_stream_header* hdr);
const char* rm_stream_get_name(rm_stream_header* hdr);
const char* rm_stream_get_mime_type(rm_stream_header* hdr);
UINT32      rm_stream_get_properties(rm_stream_header* hdr, rm_property** ppProp);
HXBOOL      rm_stream_is_realaudio(rm_stream_header* hdr);
HXBOOL      rm_stream_is_realvideo(rm_stream_header* hdr);
HXBOOL      rm_stream_is_realevent(rm_stream_header* hdr);
HXBOOL      rm_stream_is_realaudio_mimetype(const char* pszStr);
HXBOOL      rm_stream_is_realaudio_lossless_mimetype(const char* pszStr);
HXBOOL      rm_stream_is_realvideo_mimetype(const char* pszStr);
HXBOOL      rm_stream_is_realevent_mimetype(const char* pszStr);
HXBOOL      rm_stream_is_real_mimetype(const char* pszStr);
HX_RESULT   rm_stream_get_property_int(rm_stream_header* hdr,
                                       const char*       pszStr,
                                       UINT32*           pulVal);
HX_RESULT   rm_stream_get_property_buf(rm_stream_header* hdr,
                                       const char*       pszStr,
                                       BYTE**            ppBuf,
                                       UINT32*           pulLen);
HX_RESULT   rm_stream_get_property_str(rm_stream_header* hdr,
                                       const char*       pszStr,
                                       char**            ppszStr);

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* #ifndef RM_STREAM_H */
