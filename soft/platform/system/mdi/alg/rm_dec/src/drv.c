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














#include "stdlib.h"
#include "rvtypes.h"
#include "hxtypes.h"
#include "rv30dec.h"
#ifdef REAL_FORMAT_SDK
#include "rv30backend.h"
#endif
/*#include "drmcmpid.h" */
/*#include "rmadrmil.h" */

/*#include "hiverv.h" */
/*#include "piadec.h" */
/*#include "trommsg.h" */
#include "rvmsg.h"
#include "frame.h"
#include "rvdebug.h"
#include "realvdo.h"
#include "drvi.h"
#include "rvdecoder.h"
#include "rm_memory_default.h"

/* Define the version information that describes our generated bitstream. */
/* See the description of the RV_MSG_ID_Get_Bitstream_Version message */
/* for the meaning of these values. */

#define RV_BITSTREAM_MAJOR_VERSION          4
#define RV_BITSTREAM_MAJOR_VERSION_RV8      3
#define RV_BITSTREAM_MINOR_VERSION          0
#define RV_BITSTREAM_MINOR_VERSION_RV8      2
#define RV_BITSTREAM_RELEASE_VERSION        0
#define RAW_BITSTREAM_MINOR_VERSION       128

/* */
/* Define messages specific to a decoder. */
/* */



/* Define a table that maps RV_Status values to HX_RESULTs */

static HX_RESULT hxresult_map[RV_NUMBER_OF_STATUS_CODES + 1] =
{

#define DEFRVSTATUS(pia, vfw, am, rv) rv,
#include "rvdefs.h"
    0  /* prevents a compiler warning about an uncovered trailing comma */
};


HX_RESULT
RV_Status_to_HX_RESULT(RV_Status ps)
{
    if (ps >= 0 && ps < RV_NUMBER_OF_STATUS_CODES)
        return hxresult_map[ps];

    return HXR_FAIL;
}




HX_RESULT
_RV40toYUV420Init(void *prv10Init, void **decoderState)
{
    HX_RESULT                   result;
    RV_Status                  ps = RV_S_OK;
    struct RVDecoder     *dhive = 0;
    RV10_INIT                  *pRV10_Init = (RV10_INIT *)prv10Init;
    struct RV_Image_Format            in_format;
    struct RV_Image_Format            out_format;
    RV_MSG_Simple              simple_msg;
    RV_Boolean                  bIsRV8 = FALSE;
    U32 major, minor;

#if defined (RV_DEBUG_LOG)
    /* Save this in case debug output is needed. */
#if defined(DEBUG)
    RealVideo_Hive::s_debug_fp = fopen("c:\\kol\\tmp\\decode.log","w"); /* Flawfinder: ignore */
    if (!RealVideo_Hive::s_debug_fp)
    {
        fprintf(stderr,"ERROR -- failed to open log file\n");
    }
    else
    {
        RealVideo_Hive::s_debug_trace_level = RV_Trace_Level(RV_TL_PICT);
    }
#endif
#endif

    if (!pRV10_Init || !decoderState)
        return HXR_POINTER;

    /* Do the bitstream version check outside of the for-loop below, */
    /* because we have no RV_Status value that maps to HXR_INVALID_VERSION. */
    /* As of this writing, we support one Tromso bitstream version. */

    major = HX_GET_MAJOR_VERSION(pRV10_Init->ulStreamVersion);
    minor = HX_GET_MINOR_VERSION(pRV10_Init->ulStreamVersion);

    if (   major == RV_BITSTREAM_MAJOR_VERSION
            && minor == RV_BITSTREAM_MINOR_VERSION
       )
    {
        in_format.fid = RV_FID_REALVIDEO30;
    }
    else if ( major == RV_BITSTREAM_MAJOR_VERSION_RV8
              && minor == RV_BITSTREAM_MINOR_VERSION_RV8
            )
    {
        in_format.fid = RV_FID_REALVIDEO30;
        /* tell the decoder to decode RV8 */
        bIsRV8 = TRUE;

    }
    else if ( minor == RAW_BITSTREAM_MINOR_VERSION)
    {
        in_format.fid = RV_FID_RV89COMBO;
        if (major == RV_BITSTREAM_MAJOR_VERSION_RV8)
            bIsRV8 = TRUE;
    }
    else
        return HXR_INVALID_VERSION;


    for (;;)
    {
        if (!RVDecoder_Load())
        {
            ps = RV_S_ERROR;
            break;
        }

        /* in_format.fid has been set in the version number check */
        in_format.dimensions.width = pRV10_Init->pels;
        in_format.dimensions.height = pRV10_Init->lines;
        DecodedFrame_Propagate_Defaults(&in_format);

        out_format.fid = RV_FID_YUV12;
        out_format.dimensions = in_format.dimensions;
        DecodedFrame_Propagate_Defaults(&out_format);

        ps = RV_S_OK;
        dhive = (struct RVDecoder *)rm_memory_default_malloc(0, sizeof(struct RVDecoder));
        RVDecoder_Init(dhive, in_format.fid, &ps);
        /*dhive = new RealVideo_Decoder_Hive(in_format.fid, ps); */
        if (!dhive)
            ps = RV_S_OUT_OF_MEMORY;
        if (ps != RV_S_OK)
            break;

        /* Remember the image dimensions and fid, so we can use them */
        /* to call Start_Sequence. */
        memcpy(&dhive->m_in_format,  &in_format,  sizeof(struct RV_Image_Format)); /* Flawfinder: ignore */
        memcpy(&dhive->m_out_format, &out_format, sizeof(struct RV_Image_Format)); /* Flawfinder: ignore */

        simple_msg.message_id = RV_MSG_ID_Set_Picture_Header_Invariants;
        simple_msg.value1 = pRV10_Init->ulInvariants;
        simple_msg.value2 = 0;
        ps = Decoder_Custom_Message(dhive->m_coredecoder, &simple_msg.message_id);
        if (ps != RV_S_OK)
            break;

        /* put decoder in "latency" mode if the B-frame SPO flag is set */
        if (pRV10_Init->ulInvariants & RV40_SPO_FLAG_BFRAMES)
        {
            simple_msg.message_id = RV_MSG_ID_Latency_Display_Mode;
            simple_msg.value1 = RV_MSG_ENABLE;// malloc or not for b frame at init.sheen
            simple_msg.value2 = 0; /* not used */
            ps = Decoder_Custom_Message(dhive->m_coredecoder, &simple_msg.message_id);
            if (ps != RV_S_OK)
                break;
        }

#       define GET_PRODUCT_RELEASE(ver) ((ver >> 12) & 0xFF)

        /* if the stream version indicates "preview" bitstream, don't */
        /* allow multi-threading */
        if (GET_PRODUCT_RELEASE(pRV10_Init->ulStreamVersion) < 2)
        {
            simple_msg.message_id = RV_MSG_ID_Decoder_Beta_Stream;
            simple_msg.value1 = RV_MSG_ENABLE;
            simple_msg.value2 = 0;
            Decoder_Custom_Message(dhive->m_coredecoder, &simple_msg.message_id);
        }
        else
        {
            simple_msg.message_id = RV_MSG_ID_Decoder_Beta_Stream;
            simple_msg.value1 = RV_MSG_DISABLE;
            simple_msg.value2 = 0;
            Decoder_Custom_Message(dhive->m_coredecoder, &simple_msg.message_id);
        }

        if (bIsRV8)
        {
            simple_msg.message_id = RV_MSG_ID_RealVideo8;
            simple_msg.value1 = RV_MSG_ENABLE;
            simple_msg.value2 = 0;
            Decoder_Custom_Message(dhive->m_coredecoder, &simple_msg.message_id);
        }


        ps = Decoder_Start_Sequence(dhive->m_coredecoder, &in_format, &out_format);
        break;
    }

    if (ps != RV_S_OK)
    {
        Decoder_Delete(dhive->m_coredecoder);
        rm_memory_default_free(0, dhive);
        dhive = 0;
    }
    *decoderState = dhive;

    result = RV_Status_to_HX_RESULT(ps);
    return result;
}

HX_RESULT
_RV40toYUV420Free(void *global)
{

#if defined (RV_DEBUG_LOG)
    /* Save this in case debug output is needed. */
#if defined(DEBUG)
    if (RealVideo_Hive::s_debug_fp)
    {
        (void) fclose(RealVideo_Hive::s_debug_fp);
        RealVideo_Hive::s_debug_fp = 0;
    }
#endif
#endif

    struct RVDecoder     *dhive;
    dhive = (struct RVDecoder *)global;

    RVDecoder_Delete(dhive);
    rm_memory_default_free(0, dhive);

    if (!RVDecoder_Unload())
        return HXR_FAIL;

    /*  fclose (g_fdebug); */

    return HXR_OK;
}



HX_RESULT
_RV40toYUV420Transform
(
    UCHAR     *pRV20Packets,
    UCHAR     *pDecodedFrameBuffer,
    void      *pInputParams,
    void      *pOutputParams,
    void      *global
)
{
    HX_RESULT       result;
    RV_Status      ps;
    void            *decoder;
    struct RV_Image       src;
    struct RV_Image       dst;
    I32             temporal_offset = 0;
    RV_Decoder_Decompression_Flags flags = 0;
    RV_Decoder_Decompression_Notes notes = 0;
    struct RVDecoder * dhive = (struct RVDecoder *)global;
    H263DecoderInParams  * pRV20In  = (H263DecoderInParams *)pInputParams;
    H263DecoderOutParams * pRV20Out = (H263DecoderOutParams *)pOutputParams;
    RV_Segment_Info_MSG seg_msg;

    if (!RVDecoder_is_active(dhive) || !pRV20In || !pRV20Out)
        return HXR_POINTER;

    pRV20Out->numFrames = 0;  /* Until we learn otherwise */
    pRV20Out->notes = 0;
    pRV20Out->timestamp = 0;
    if (pRV20In->flags & RV_DECODE_MORE_FRAMES)
        flags |= RV_DDF_MORE_FRAMES;
    if (pRV20In->flags & RV_DECODE_DONT_DRAW)
        flags |= RV_DDF_DONT_DRAW;
    if (pRV20In->flags & RV_DECODE_LAST_FRAME)
        flags |= RV_DDF_LAST_FRAME;

    decoder = dhive->m_coredecoder;

    /* Init segment info if RealVideo format */
    if (((struct Decoder *)decoder)->m_fid != RV_FID_RV89COMBO)
    {
        seg_msg.message_id = RV_MSG_ID_Set_Decode_Segment_Info;
        seg_msg.number_of_segments = pRV20In->numDataSegments;
        RVAssert(sizeof(RV_Segment_Info) == sizeof(HXCODEC_SEGMENTINFO));
        seg_msg.segment_info = (RV_Segment_Info*) pRV20In->pDataSegments;
        ps = Decoder_Custom_Message(decoder, &seg_msg.message_id);
        if (ps != RV_S_OK && (pRV20In->flags & RV_DECODE_LAST_FRAME)==0)
            return RV_Status_to_HX_RESULT(ps);
        // continue if last frame
    }
    src.format = dhive->m_in_format;
    src.size = pRV20In->dataLength;
    src.sequence_number = pRV20In->timestamp;  /* presentation time */
    DecodedFrame_Set_Buffer_Pointers(&src, pRV20Packets);

    dst.format = dhive->m_out_format;
    dst.size = 0;
    /* We could calculate 'size' based on the format id and dimensions, */
    /* but let's let the decoder fill it in for us. */
    dst.sequence_number = src.sequence_number;
    DecodedFrame_Set_Buffer_Pointers(&dst, pDecodedFrameBuffer);

    ps = Decoder_Decode(decoder, &src, &dst, flags, &notes, temporal_offset);

    /* Pass the displayed image size to the front-end.  Due to RPR, this */
    /* might not be the src image size. */
    pRV20Out->width = dst.format.dimensions.width;
    pRV20Out->height = dst.format.dimensions.height;

    /* Assign the number of frames out */
    if (ps == RV_S_OK && !(notes & RV_DDN_DONT_DRAW))
        pRV20Out->numFrames = 1;

    /* Assign the notes fields to the appropriate RV_DECODE_... notes */
    if (notes & RV_DDN_MORE_FRAMES)
        pRV20Out->notes |= RV_DECODE_MORE_FRAMES;
    if (notes & RV_DDN_DONT_DRAW)
        pRV20Out->notes |= RV_DECODE_DONT_DRAW;
    if (notes & RV_DDN_KEY_FRAME)
        pRV20Out->notes |= RV_DECODE_KEY_FRAME;
    if (notes & RV_DDN_B_FRAME)
        pRV20Out->notes |= RV_DECODE_B_FRAME;

    /* Assign the presentation time for the display frame */
    pRV20Out->timestamp = dst.sequence_number + temporal_offset;

    result = RV_Status_to_HX_RESULT(ps);
    return result;
}

HX_RESULT
_RV40toYUV420CustomMessage(RV_Custom_Message_ID *msg_id, void *global)
{
    RV_Status  ps;
    HX_RESULT   result;
    struct RVDecoder *dhive;
    dhive = (struct RVDecoder *)global;

    if (!msg_id || !RVDecoder_is_active(dhive))
        return HXR_POINTER;

    ps = Decoder_Custom_Message(dhive->m_coredecoder, msg_id);

    result = RV_Status_to_HX_RESULT(ps);
    return result;
}

HX_RESULT
_RV40toYUV420HiveMessage(ULONG32 *msg_id, void *global)
{
    RV_Status                      ps;
    HX_RESULT                       result;
    struct RVDecoder *dhive;
    dhive = (struct RVDecoder *)global;

    if (!msg_id || !RVDecoder_is_active(dhive))
        return HXR_POINTER;

    ps = RV_S_UNSUPPORTED;  /* Until we learn otherwise */

    switch (*msg_id)
    {
        default:
            RVDebug((RV_TL_ALWAYS,
                     "ERROR HiveMessage -- bad message id %ud", *msg_id));
            break;

        case RV_MSG_ID_Get_Decoder_API_Version:

        {
            RV_Boolean bIsRV8 = FALSE;
            RV_MSG_Simple              simple_msg;

            simple_msg.message_id = RV_MSG_ID_RealVideo8;
            simple_msg.value1 = RV_MSG_GET;
            simple_msg.value2 = 0;
            Decoder_Custom_Message(dhive->m_coredecoder,&simple_msg.message_id);

            if (simple_msg.value2)
                bIsRV8 = TRUE;

            if (bIsRV8)
            {
                ((RVDecoder_API_Version_Message*)msg_id)->version =
                    HX_ENCODE_PROD_VERSION(RVDecoder_API_Tromso_Major,
                                           RVDecoder_API_Tromso_Minor,
                                           0,
                                           0);
            }
            else
            {
                ((RVDecoder_API_Version_Message*)msg_id)->version =
                    HX_ENCODE_PROD_VERSION(RVDecoder_API_Margate_Major,
                                           RVDecoder_API_Margate_Minor,
                                           0,
                                           0);
            }

            ps = RV_S_OK;
        }
        break;


        case RV_MSG_ID_Change_Decoder_Output_Dimensions:
        {
            struct RV_Image_Format fmt;

            fmt.fid = RV_FID_YUV12;
            fmt.dimensions.width =
                ((RVDecoder_Output_Dimensions_Message*)msg_id)->width;
            fmt.dimensions.height =
                ((RVDecoder_Output_Dimensions_Message*)msg_id)->height;
            DecodedFrame_Propagate_Defaults(&fmt);
            ps = Decoder_Change_Output_Format(dhive->m_coredecoder, &fmt);
        }
        break;

        case RV_MSG_ID_Decoder_Start_Sequence:
        {
            ps = Decoder_Start_Sequence(dhive->m_coredecoder,
                                        &dhive->m_in_format, &dhive->m_out_format);
        }
        break;
    }

    result = RV_Status_to_HX_RESULT(ps);
    return result;
}


#if 0
HX_RESULT
GetGUID(UCHAR* pGUID)
{
    if (!pGUID)
    {
        return HXR_FAIL;
    }
    UCHAR guidBaked[16] = DRV4_BIN_GUID;
    memcpy(pGUID, guidBaked, 16); /* Flawfinder: ignore */
    return HXR_OK;
}
#endif
