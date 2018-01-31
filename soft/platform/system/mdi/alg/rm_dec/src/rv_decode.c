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





#include <string.h>
#include "rvstruct.h"
#include "rv_decode.h"
#include "rv20backend.h"
#include "rv30backend.h"
#include "codec_defines.h"
#include "helix_utils.h"
#include "rm_memory_default.h"

#ifdef SHEEN_VC_DEBUG
#include "vpp_rmvb.h"
#endif

static void rv_decode_recover_packets(rv_decode* pFrontEnd, UINT8* pData,
                                      rv_backend_in_params* pInputParams);


/* rv_decode_create()
 * Creates RV decoder frontend struct, copies memory utilities.
 * Returns struct pointer on success, NULL on failure. */
rv_decode*
rv_decode_create(void*              pUserError,
                 rm_error_func_ptr  fpError)
{
    return rv_decode_create2(pUserError,
                             fpError,
                             HXNULL,
                             rm_memory_default_malloc,
                             rm_memory_default_free);
}

rv_decode*
rv_decode_create2(void*               pUserError,
                  rm_error_func_ptr   fpError,
                  void*               pUserMem,
                  rm_malloc_func_ptr  fpMalloc,
                  rm_free_func_ptr    fpFree)
{
    rv_decode* pRet = (rv_decode*)HXNULL;

    if (fpMalloc && fpFree)
    {
        pRet = (rv_decode*) fpMalloc(pUserMem, sizeof(rv_decode));

        if (pRet)
        {
            /* Zero out the struct */
            memset((void*) pRet, 0, sizeof(rv_decode));
            /* Assign the error function */
            pRet->fpError = fpError;
            pRet->pUserError = pUserError;
            /* Assign the memory functions */
            pRet->fpMalloc = fpMalloc;
            pRet->fpFree = fpFree;
            pRet->pUserMem = pUserMem;

            /* Allocate the backend interface struct */
            pRet->pDecode = (rv_backend*) fpMalloc(pUserMem, sizeof(rv_backend));

            if (pRet->pDecode == HXNULL)
            {
                fpFree(pUserMem, pRet);
                pRet = (rv_decode *)HXNULL;
            }
        }
    }

    return pRet;
}

/* rv_decode_destroy()
 * Deletes decoder and backend instance, followed by frontend. */
void
rv_decode_destroy(rv_decode* pFrontEnd)
{
    rm_free_func_ptr fpFree;
    void* pUserMem;

    if (pFrontEnd && pFrontEnd->fpFree)
    {
        /* Save a pointer to fpFree and pUserMem */
        fpFree   = pFrontEnd->fpFree;
        pUserMem = pFrontEnd->pUserMem;

        if (pFrontEnd->pDecode)
        {
            if (pFrontEnd->pDecode->fpFree && pFrontEnd->pDecodeState)
            {
                /* Free the decoder instance */
                pFrontEnd->pDecode->fpFree(pFrontEnd->pDecodeState);
                pFrontEnd->pDecodeState = HXNULL;
            }
            /* Free the backend interface */
            fpFree(pUserMem, pFrontEnd->pDecode);
        }
        /* Free the rv_decode struct memory */
        fpFree(pUserMem, pFrontEnd);
    }
}

/* rv_decode_init()
 * Reads bitstream header, selects and initializes decoder backend.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT
rv_decode_init(rv_decode* pFrontEnd, rv_format_info* pHeader)
{
    HX_RESULT result = HXR_OK;
    UINT8* baseptr;
    UINT8 ucTmp;
    UINT32 i;
    RV_MSG_RVDecoder_RPR_Sizes rvSizesMsg;
    RV_MSG_Simple msg;
    int w = 0xFF; /* For run-time endianness detection */
#ifdef SHEEN_VC_DEBUG
    //open VOC
    vpp_RmvbOpen(NULL,VPP_WAKEUP_SW0);
#endif

    if((pFrontEnd==HXNULL)||(pHeader==HXNULL)) return HXR_FAIL;

    if (pHeader->ulMOFTag != HX_MEDIA_VIDEO)
        return HXR_FAIL;

    pFrontEnd->ulSPOExtra = 0;
    pFrontEnd->ulEncodeSize = 0;
    pFrontEnd->ulNumResampledImageSizes = 0;

    switch (pHeader->ulSubMOFTag)
    {
        case HX_RV20VIDEO_ID:
        case HX_RVG2VIDEO_ID:
#ifndef BUILD_ONLY_RB89COMBO
            /* Select the decoder backend */
            pFrontEnd->pDecode->fpInit = _RV20toYUV420Init;
            pFrontEnd->pDecode->fpDecode = _RV20toYUV420Transform;
            pFrontEnd->pDecode->fpCustomMessage = _RV20toYUV420CustomMessage;
            pFrontEnd->pDecode->fpHiveMessage = _RV20toYUV420HiveMessage;
            pFrontEnd->pDecode->fpFree = _RV20toYUV420Free;
            /* set stream specific params */
            if (pHeader->ulSubMOFTag == HX_RV20VIDEO_ID)
                pFrontEnd->ulECCMask = 0x20;
            else
                pFrontEnd->ulECCMask = 0;
            break;
#else
            return HXR_NOT_SUPPORTED;
            break;
#endif
        case HX_RV30VIDEO_ID:
        case HX_RV40VIDEO_ID:
        case HX_RV89COMBO_ID:
            /* Select the decoder backend */
            pFrontEnd->pDecode->fpInit = _RV40toYUV420Init;
            pFrontEnd->pDecode->fpDecode = _RV40toYUV420Transform;
            pFrontEnd->pDecode->fpCustomMessage = _RV40toYUV420CustomMessage;
            pFrontEnd->pDecode->fpHiveMessage = _RV40toYUV420HiveMessage;
            pFrontEnd->pDecode->fpFree = _RV40toYUV420Free;
            /* set stream specific params */
            if (pHeader->ulSubMOFTag == HX_RV30VIDEO_ID)
                pFrontEnd->ulECCMask = 0x20;
            else if (pHeader->ulSubMOFTag == HX_RV40VIDEO_ID)
                pFrontEnd->ulECCMask = 0x80;
            else
                pFrontEnd->ulECCMask = 0;
            break;
        default:
            /* unknown format */
            return HXR_NOT_SUPPORTED;
            break;
    }

    if (pHeader->ulOpaqueDataSize > 0)
    {
        /* Decode opaque data */
        baseptr = pHeader->pOpaqueData;

        /* ulSPOExtra contains CODEC options */
        memmove(&pFrontEnd->ulSPOExtra, baseptr, sizeof(UINT32));
        pFrontEnd->ulSPOExtra = IS_BIG_ENDIAN(w) ? pFrontEnd->ulSPOExtra :
                                BYTE_SWAP_UINT32(pFrontEnd->ulSPOExtra);
        baseptr += sizeof(UINT32);

        /* ulStreamVersion */
        memmove(&pFrontEnd->ulStreamVersion, baseptr, sizeof(UINT32));
        pFrontEnd->ulStreamVersion = IS_BIG_ENDIAN(w) ? pFrontEnd->ulStreamVersion :
                                     BYTE_SWAP_UINT32(pFrontEnd->ulStreamVersion);
        baseptr += sizeof(UINT32);

        pFrontEnd->ulMajorBitstreamVersion = HX_GET_MAJOR_VERSION(pFrontEnd->ulStreamVersion);
        pFrontEnd->ulMinorBitstreamVersion = HX_GET_MINOR_VERSION(pFrontEnd->ulStreamVersion);

        /* Decode extra opaque data */
        if (!(pFrontEnd->ulMinorBitstreamVersion & RAW_BITSTREAM_MINOR_VERSION))
        {
            if (pFrontEnd->ulMajorBitstreamVersion == RV20_MAJOR_BITSTREAM_VERSION ||
                    pFrontEnd->ulMajorBitstreamVersion == RV30_MAJOR_BITSTREAM_VERSION)
            {
                /* RPR (Reference Picture Resampling) sizes */
                pFrontEnd->ulNumResampledImageSizes = (pFrontEnd->ulSPOExtra & RV40_SPO_BITS_NUMRESAMPLE_IMAGES)
                                                      >> RV40_SPO_BITS_NUMRESAMPLE_IMAGES_SHIFT;
                /* loop over dimensions of possible resampled images sizes              */
                /* This byzantine method of extracting bytes is required to solve       */
                /* misaligned write problems in UNIX                                    */
                /* note 2 byte offset in pDimensions buffer for resampled sizes         */
                /* these 2 bytes are later filled with the native pels and lines sizes. */
                for(i = 0; i < pFrontEnd->ulNumResampledImageSizes; i++)
                {
                    memmove(&ucTmp, baseptr, sizeof(UINT8));
                    *(pFrontEnd->pDimensions + 2*i + 2) = (UINT32)(ucTmp<<2); /* width */
                    baseptr+=sizeof(UINT8);
                    memmove(&ucTmp, baseptr, sizeof(UINT8));
                    *(pFrontEnd->pDimensions + 2*i + 3) = (UINT32)(ucTmp<<2); /* height */
                    baseptr+=sizeof(UINT8);
                }
            }
            else if (pFrontEnd->ulMajorBitstreamVersion == RV40_MAJOR_BITSTREAM_VERSION)
            {
                /* RV9 largest encoded dimensions */
                if (pHeader->ulOpaqueDataSize >= 12)
                {
                    memmove((UINT8*)pFrontEnd->ulEncodeSize, baseptr, sizeof(UINT32));
                    pFrontEnd->ulEncodeSize = IS_BIG_ENDIAN(w) ? pFrontEnd->ulEncodeSize :
                                              BYTE_SWAP_UINT32(pFrontEnd->ulEncodeSize);
                }
            }
        }
    }

    /* Set largest encoded dimensions */
    pFrontEnd->ulLargestPels = ((pFrontEnd->ulEncodeSize >> 14) & 0x3FFFC);
    if (pFrontEnd->ulLargestPels == 0)
        pFrontEnd->ulLargestPels = pHeader->usWidth;

    pFrontEnd->ulLargestLines = ((pFrontEnd->ulEncodeSize << 2) & 0x3FFFC);
    if (pFrontEnd->ulLargestLines == 0)
        pFrontEnd->ulLargestLines = pHeader->usHeight;

    /* Prepare decoder init parameters */
    pFrontEnd->pInitParams.usPels = (UINT16)pFrontEnd->ulLargestPels;
    pFrontEnd->pInitParams.usLines = (UINT16)pFrontEnd->ulLargestLines;
    pFrontEnd->pInitParams.usPadWidth = pHeader->usPadWidth;
    pFrontEnd->pInitParams.usPadHeight = pHeader->usPadHeight;
    pFrontEnd->pInitParams.bPacketization = TRUE;
    pFrontEnd->pInitParams.ulInvariants = pFrontEnd->ulSPOExtra;
    pFrontEnd->pInitParams.ulStreamVersion = pFrontEnd->ulStreamVersion;

    pFrontEnd->ulOutSize = WIDTHBYTES(pFrontEnd->ulLargestPels * pFrontEnd->ulLargestLines * 12);

    /* Call the decoder backend init function */
    result = pFrontEnd->pDecode->fpInit((void*)&pFrontEnd->pInitParams, (void**)&pFrontEnd->pDecodeState);
    if (result != HXR_OK)
        return result;

    /* Send version-specific init messages, if required */
    if ((pFrontEnd->ulMajorBitstreamVersion == RV20_MAJOR_BITSTREAM_VERSION ||
            pFrontEnd->ulMajorBitstreamVersion == RV30_MAJOR_BITSTREAM_VERSION) &&
            (pFrontEnd->ulNumResampledImageSizes != 0))
    {
        rvSizesMsg.message_id = RV_MSG_ID_Set_RVDecoder_RPR_Sizes;
        rvSizesMsg.num_sizes = pFrontEnd->ulNumResampledImageSizes + 1; /* includes native size */
        pFrontEnd->pDimensions[0] = pFrontEnd->pInitParams.usPels;    /* native width */
        pFrontEnd->pDimensions[1] = pFrontEnd->pInitParams.usLines;   /* native height */
        rvSizesMsg.sizes = (U32*)(pFrontEnd->pDimensions);

        result = pFrontEnd->pDecode->fpCustomMessage((UINT32*)&(rvSizesMsg.message_id), (void*)pFrontEnd->pDecodeState);
        if(result != HXR_OK)
            return result;
    }

    /* Post-smoothing filter is disabled by default */
    msg.message_id = RV_MSG_ID_Smoothing_Postfilter;
    msg.value1 = RV_MSG_DISABLE;
    msg.value2 = 0;
    result = pFrontEnd->pDecode->fpCustomMessage ((UINT32*)&(msg.message_id), (void*)pFrontEnd->pDecodeState);

    return result;
}

/* rv_decode_stream_input()
 * Reads frame header and fills decoder input parameters struct. If there
 * is packet loss and ECC packets exist, error correction is attempted.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT
rv_decode_stream_input(rv_decode* pFrontEnd, rv_frame* pFrame)
{
    //HX_RESULT result = HXR_OK;
    UINT32 ulSegOff;
    UINT32 i;

    if((pFrontEnd==HXNULL)||(pFrame==HXNULL)) return HXR_FAIL;

    pFrontEnd->pInputFrame = pFrame;

    if (!(pFrontEnd->ulMinorBitstreamVersion & RAW_BITSTREAM_MINOR_VERSION))
    {
        /* Identify frame type and determine quantization factor */
        pFrontEnd->bInputFrameIsReference = FALSE;

        for (i = 0; i < pFrame->ulNumSegments; i++)
        {
            if (pFrame->pSegment[i].bIsValid == TRUE)
            {
                ulSegOff = pFrame->pSegment[i].ulOffset;
                if (ulSegOff + 1 < pFrame->ulDataLen)
                {
                    if (pFrontEnd->ulMajorBitstreamVersion == RV20_MAJOR_BITSTREAM_VERSION)
                    {
                        pFrontEnd->ulInputFrameQuant = (pFrame->pData[ulSegOff] & 0x1F);
                        pFrontEnd->bInputFrameIsReference = ((pFrame->pData[ulSegOff] & 0xC0) == 0xC0)?(FALSE):(TRUE);
                    }
                    else if (pFrontEnd->ulMajorBitstreamVersion == RV30_MAJOR_BITSTREAM_VERSION)
                    {
                        pFrontEnd->ulInputFrameQuant = ((pFrame->pData[ulSegOff + 1] & 0xe0)>>5)+((pFrame->pData[ulSegOff] & 0x03)<<3);
                        pFrontEnd->bInputFrameIsReference = ((pFrame->pData[ulSegOff] & 0x18) == 0x18)?(FALSE):(TRUE);
                    }
                    else if (pFrontEnd->ulMajorBitstreamVersion == RV40_MAJOR_BITSTREAM_VERSION)
                    {
                        pFrontEnd->ulInputFrameQuant = (pFrame->pData[ulSegOff] & 0x1F);
                        pFrontEnd->bInputFrameIsReference = ((pFrame->pData[ulSegOff] & 0x60) == 0x60)?(FALSE):(TRUE);
                    }
                }
                break;
            }
        }
    }

    /* Set decoder input parameters */
    pFrontEnd->pInputParams.dataLength = pFrame->ulDataLen;
    pFrontEnd->pInputParams.bInterpolateImage = FALSE;
    pFrontEnd->pInputParams.numDataSegments = pFrame->ulNumSegments - 1;
    pFrontEnd->pInputParams.pDataSegments = pFrame->pSegment;
    pFrontEnd->pInputParams.timestamp = pFrame->ulTimestamp;
    pFrontEnd->pInputParams.flags = 0;

    /* Use ECC to recover lost packets */
    rv_decode_recover_packets(pFrontEnd, pFrame->pData, &pFrontEnd->pInputParams);

    return HXR_OK;
}

/* rv_decode_stream_decode()
 * Calls decoder backend to decode issued frame and produce an output frame.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT
rv_decode_stream_decode(rv_decode* pFrontEnd, UINT8* pOutput)
{
    HX_RESULT result;
    if(pFrontEnd==HXNULL) return HXR_FAIL;

    result = pFrontEnd->pDecode->fpDecode(pFrontEnd->pInputFrame->pData,
                                          pOutput,
                                          &pFrontEnd->pInputParams,
                                          &pFrontEnd->pOutputParams,
                                          pFrontEnd->pDecodeState);

    return result;
}

/* rv_decode_stream_flush()
 * Flushes the latency frame from the decoder backend after the last frame
 * is delivered and decoded before a pause or the end-of-file.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT
rv_decode_stream_flush(rv_decode* pFrontEnd, UINT8* pOutput)
{
    HX_RESULT result;
    if(pFrontEnd==HXNULL) return HXR_FAIL;
    /* Set decoder input parameters for flush */
    pFrontEnd->pInputParams.dataLength = 0;
    pFrontEnd->pInputParams.bInterpolateImage = FALSE;
    pFrontEnd->pInputParams.numDataSegments = 0;
    pFrontEnd->pInputParams.pDataSegments = (rv_segment *)HXNULL;
    pFrontEnd->pInputParams.timestamp = 0;
    pFrontEnd->pInputParams.flags = RV_DECODE_LAST_FRAME;

    result = pFrontEnd->pDecode->fpDecode((UCHAR*)HXNULL,
                                          pOutput,
                                          &pFrontEnd->pInputParams,
                                          &pFrontEnd->pOutputParams,
                                          pFrontEnd->pDecodeState);

    return result;
}

/* rv_decode_custom_message()
 * Sends a custom message to the decoder backend.
 * Returns zero on success, negative result indicates failure. */
HX_RESULT
rv_decode_custom_message(rv_decode* pFrontEnd, RV_Custom_Message_ID* pMsg_id)
{
    if((pFrontEnd==HXNULL)||(pMsg_id==HXNULL)) return HXR_FAIL;
    return pFrontEnd->pDecode->fpCustomMessage((UINT32*)pMsg_id, pFrontEnd->pDecodeState);
}
INT32
rv_decode_ext_maxbuf_sizes(rv_decode* pFrontEnd)
{
    HX_RESULT result;
    RV_MSG_ExtGetMaxSize msg;
    msg.message_id  = RV_MSG_ID_ExtGetMaxSize;
    msg.cmd         = RV_MSG_GET;
    msg.size        = 0;
    result = rv_decode_custom_message(pFrontEnd,&msg.message_id);
    if(result) return -1;
    else return msg.size;
}
INT32
rv_decode_ext_getnextfree_idx(rv_decode* pFrontEnd)
{
    HX_RESULT result;
    RV_MSG_ExtGetIndex msg;
    msg.message_id  = RV_MSG_ID_ExtGetIndex;
    msg.cmd         = RV_MSG_GET;
    msg.idx         = -1;
    result = rv_decode_custom_message(pFrontEnd,&msg.message_id);
    if(result) return -1;
    else return msg.idx;
}
INT32
rv_decode_ext_getsize_idx(rv_decode* pFrontEnd, INT32 idx)
{
    HX_RESULT result;
    RV_MSG_ExtGetSize_Idx msg;
    msg.message_id  = RV_MSG_ID_ExtGetSize_Idx;
    msg.cmd         = RV_MSG_GET;
    msg.idx         = idx;
    msg.size        = -1;
    result = rv_decode_custom_message(pFrontEnd,&msg.message_id);
    if(result) return -1;
    else return msg.size;
}
INT32
rv_decode_ext_setptr_idx(rv_decode* pFrontEnd, INT32 idx, void* ptr)
{
    HX_RESULT result;
    RV_MSG_ExtSetPtr_Idx msg;
    msg.message_id  = RV_MSG_ID_ExtSetPtr_Idx;
    msg.cmd         = RV_MSG_SET;
    msg.idx         = idx;
    msg.ptr         = ptr;
    result = rv_decode_custom_message(pFrontEnd,&msg.message_id);
    if(result) return -1;
    else return 0;
}
HX_RESULT
rv_decode_ext_exestart_seq(rv_decode* pFrontEnd)
{
    UINT32 msg = RV_MSG_ID_Decoder_Start_Sequence;
    if(pFrontEnd==HXNULL) return HXR_FAIL;
    return pFrontEnd->pDecode->fpHiveMessage(&msg,pFrontEnd->pDecodeState);
}

HX_RESULT
rv_decode_frame_info(rv_decode* pFrontEnd, struct RV_Image* pImageInfo)
{

    RV_MSG_GetDisplayed_Image_Info msg; // This variable is used for Message deliver

    pImageInfo->format.fid = RV_FID_UNDEFINED;

    pImageInfo->yuv_info.y_plane = NULL;
    pImageInfo->yuv_info.u_plane = NULL;
    pImageInfo->yuv_info.v_plane = NULL;

    pImageInfo->format.yuv_info.y_pitch = 0;
    pImageInfo->format.yuv_info.u_pitch = 0;
    pImageInfo->format.yuv_info.v_pitch = 0;

    pImageInfo->format.dimensions.width  = 0;
    pImageInfo->format.dimensions.height = 0;

    pImageInfo->format.rectangle.width  = 0;
    pImageInfo->format.rectangle.height = 0;

    msg.message_id  = RV_MSG_ID_GetDisplayed_Image_Info;
    msg.cmd         = RV_MSG_GET;
    msg.format_info = RV_FID_UNDEFINED;
    msg.dim_width   = 0;
    msg.dim_height  = 0;

    msg.pYplane = NULL;
    msg.pUplane = NULL;
    msg.pVplane = NULL;

    msg.y_pitch = 0;
    msg.u_pitch = 0;
    msg.v_pitch = 0;

    return rv_decode_frame_info_YUV420(pFrontEnd,pImageInfo,&msg);
}

HX_RESULT
rv_decode_frame_info_YUV420(rv_decode* pFrontEnd, struct RV_Image* pImageInfo, RV_MSG_GetDisplayed_Image_Info* pMsg)
{
    if((pFrontEnd==HXNULL)||(pImageInfo==HXNULL)||(pMsg==HXNULL)) return HXR_FAIL;
    if(
        (rv_decode_custom_message(pFrontEnd,&pMsg->message_id)==HXR_OK)&&
        (pMsg->format_info == RV_FID_YUV12)
    )
    {
        pImageInfo->format.fid               = pMsg->format_info;
        pImageInfo->format.dimensions.width  = pMsg->dim_width;
        pImageInfo->format.dimensions.height = pMsg->dim_height;

        pImageInfo->yuv_info.y_plane         = pMsg->pYplane;
        pImageInfo->yuv_info.u_plane         = pMsg->pUplane;
        pImageInfo->yuv_info.v_plane         = pMsg->pVplane;

        pImageInfo->format.yuv_info.y_pitch  = pMsg->y_pitch;
        pImageInfo->format.yuv_info.u_pitch  = pMsg->u_pitch;
        pImageInfo->format.yuv_info.v_pitch  = pMsg->v_pitch;

        return HXR_OK;

    }
    else return HXR_NOT_SUPPORTED;
}
/**************** Accessor Functions *******************/
/* rv_decode_max_output_size()
 * Returns maximum size of YUV 4:2:0 output buffer in bytes. */
UINT32
rv_decode_max_output_size(rv_decode* pFrontEnd)
{
    if(pFrontEnd==HXNULL) return 0;
    return pFrontEnd->ulOutSize;
}

/* rv_decode_get_output_size()
 * Returns size of most recent YUV 4:2:0 output buffer in bytes. */
UINT32
rv_decode_get_output_size(rv_decode* pFrontEnd)
{
    if(pFrontEnd==HXNULL) return 0;
    return (pFrontEnd->pOutputParams.width * pFrontEnd->pOutputParams.height * 12) >> 3;
}

/* rv_decode_get_output_dimensions()
 * Returns width and height of most recent YUV output buffer. */
HX_RESULT
rv_decode_get_output_dimensions(rv_decode* pFrontEnd, UINT32* pWidth, UINT32* pHeight)
{
    if((pFrontEnd==HXNULL)||(pWidth==HXNULL)||(pHeight==HXNULL))return HXR_FAIL;
    *pWidth = pFrontEnd->pOutputParams.width;
    *pHeight = pFrontEnd->pOutputParams.height;

    return HXR_OK;
}

/* rv_decode_frame_valid()
 * Checks decoder output parameters to see there is a valid output frame.
 * Returns non-zero value if a valid output frame exists, else zero. */
UINT32
rv_decode_frame_valid(rv_decode* pFrontEnd)
{
    if(pFrontEnd==HXNULL) return 0;
    return !(pFrontEnd->pOutputParams.notes & RV_DECODE_DONT_DRAW);
}

/* rv_decode_more_frames()
 * Checks decoder output parameters to see if more output frames can be
 * produced without additional input frames.
 * Returns non-zero value if more frames can be
 * produced without additional input, else zero. */
UINT32
rv_decode_more_frames(rv_decode* pFrontEnd)
{
    if(pFrontEnd==HXNULL) return 0;
    return (pFrontEnd->pOutputParams.notes & RV_DECODE_MORE_FRAMES);
}

/* rv_decode_is_key_frame()
 * Checks decoder output parameters to see if the current frame is key frame
 * or not.
 * Returns non-zero value if the current frame is key frame, else zero. */
UINT32
rv_decode_is_key_frame(rv_decode* pFrontEnd)
{
    if(pFrontEnd==HXNULL) return 0;
    return (pFrontEnd->pOutputParams.notes & RV_DECODE_KEY_FRAME);
}

/**************** Private Functions *******************/
/* rv_decode_recover_packets()
 * Looks for lost packets and ECC packets. Reconstructs missing packets
 * with ECC data if possible. */
static void
rv_decode_recover_packets(rv_decode* pFrontEnd, UINT8* pData, rv_backend_in_params* pInputParams)
{
    UINT32 packet, byte_in;
    UINT32 lostPacket=0;
    INT32 ECCPacket;

    UINT32 ulNumECCPackets = 0;
    UINT32 ulNumLostPackets = 0;
    UINT32 ulNumSegments = 0;

    UINT32 ulECCPacketSize;
    UINT32 ulLostPacketSize;
    UINT32 ulGoodPacketSize;

    UINT8 *pECCPacket;
    UINT8 *pLostPacket;
    UINT8 *pGoodPacket;

    if((pFrontEnd==HXNULL)||(pData==HXNULL)||(pInputParams==HXNULL)) return;


    /* Count the number of lost packets and ECC packets */
    for (packet = 0; packet < pInputParams->numDataSegments + 1; packet++)
    {
        if (pInputParams->pDataSegments[packet].bIsValid == FALSE)
        {
            ulNumLostPackets++;
        }
        else if (pData[pInputParams->pDataSegments[packet].ulOffset] & pFrontEnd->ulECCMask)
        {
            pECCPacket = pData + pInputParams->pDataSegments[packet].ulOffset;
            if (packet == pInputParams->numDataSegments)
            {
                ulECCPacketSize = pInputParams->dataLength - pInputParams->pDataSegments[packet].ulOffset;
            }
            else
            {
                ulECCPacketSize = pInputParams->pDataSegments[packet+1].ulOffset - pInputParams->pDataSegments[packet].ulOffset;
            }
            ulNumECCPackets = (pECCPacket[ulECCPacketSize-1] & 0x0F);
        }
    }

    /* If there are any lost packets, we'll look for ECC packets and see what we can correct with them */
    if ((ulNumLostPackets > 0) && (ulNumECCPackets > 0))
    {
        ulNumSegments = pInputParams->numDataSegments + 1 - ulNumECCPackets;

        /* If we've lost B-frame packets, drop the whole frame */
        if (!(pFrontEnd->bInputFrameIsReference))
        {
            for (packet = 0; packet < pInputParams->numDataSegments + 1; packet++)
            {
                pInputParams->pDataSegments[packet].bIsValid = FALSE;
                pInputParams->pDataSegments[packet].ulOffset = 0;
            }
            memset(pData,0,pInputParams->dataLength);
            pInputParams->dataLength = 0;
            return;
        }

        for (ECCPacket = pInputParams->numDataSegments; ECCPacket >= 0; ECCPacket--)
        {
            if ((pInputParams->pDataSegments[ECCPacket].bIsValid == TRUE) &&
                    (pData[pInputParams->pDataSegments[ECCPacket].ulOffset] & pFrontEnd->ulECCMask))
            {
                //UINT32 FourByteOffsets = FALSE;
                UINT32 startPacket = 0;
                UINT32 stopPacket = 0;
                UINT32 numPacketsCovered = 0;
                UINT32 eccNum = 0;

                pECCPacket = pData + pInputParams->pDataSegments[ECCPacket].ulOffset;

                /* Read the last byte of the ECC for some info */
                if (ECCPacket == (INT32)pInputParams->numDataSegments)
                {
                    ulECCPacketSize = pInputParams->dataLength - pInputParams->pDataSegments[ECCPacket].ulOffset;
                }
                else
                {
                    ulECCPacketSize = pInputParams->pDataSegments[ECCPacket+1].ulOffset - pInputParams->pDataSegments[ECCPacket].ulOffset;
                }
                eccNum = ((pECCPacket[ulECCPacketSize-1] & 0xF0) >> 4) & 0x0F;

                /* Find packets covered */
                startPacket = (eccNum)*ulNumSegments/ulNumECCPackets;
                stopPacket = (eccNum+1)*ulNumSegments/ulNumECCPackets;

                if (eccNum == ulNumECCPackets - 1)
                {
                    stopPacket = ulNumSegments;
                }

                numPacketsCovered = stopPacket - startPacket;

                if (ulNumECCPackets > 1)
                {
                    /* Read the offsets for the packets covered and correct them */
                    byte_in = ulECCPacketSize-2;
                    packet = numPacketsCovered;
                    while (1)
                    {
                        UINT32 offset;
                        if (pECCPacket[byte_in] & 0x01)
                        {
                            offset =
                                ((pECCPacket[byte_in]) & 0x000000FF) |
                                ((pECCPacket[byte_in-1] << 8) & 0x0000FF00) |
                                ((pECCPacket[byte_in-2] << 16) & 0x00FF0000) |
                                ((pECCPacket[byte_in-3] << 24) & 0xFF000000);
                            pInputParams->pDataSegments[startPacket+packet].ulOffset = ((offset >> 1) & 0x7FFFFFFF);
                            byte_in -= 4;
                        }
                        else
                        {
                            offset =
                                ((pECCPacket[byte_in]) & 0x000000FF) |
                                ((pECCPacket[byte_in-1] << 8) & 0x0000FF00);
                            pInputParams->pDataSegments[startPacket+packet].ulOffset = (offset >> 1);
                            byte_in -= 2;
                        }
                        if (packet == 0)
                            break;
                        packet--;
                    }
                }

                /* Lets count the number of lost packets this ECC covers */
                ulNumLostPackets = 0;
                for (packet = startPacket; packet < stopPacket; packet++)
                {
                    if (pInputParams->pDataSegments[packet].bIsValid == FALSE)
                    {
                        lostPacket = packet;
                        ulNumLostPackets++;
                    }
                }

                /* If there is a single lost packet that this ECC can correct, then correct it. */
                if (ulNumLostPackets == 1)
                {
                    pLostPacket = pData + pInputParams->pDataSegments[lostPacket].ulOffset;
                    if (lostPacket == pInputParams->numDataSegments)
                    {
                        ulLostPacketSize = pInputParams->dataLength - pInputParams->pDataSegments[lostPacket].ulOffset;
                    }
                    else
                    {
                        ulLostPacketSize = pInputParams->pDataSegments[lostPacket+1].ulOffset - pInputParams->pDataSegments[lostPacket].ulOffset;
                    }

                    /* The lost packet size can be zero if the following packet is also lost. */
                    /* In that case, we'll say that the lost packet size is the same as the ECC packet size. */
                    if (ulLostPacketSize == 0)
                    {
                        ulLostPacketSize = ulECCPacketSize;
                    }

                    /* Clear the lost packet */
                    memset(pLostPacket,0,ulLostPacketSize);

                    /* Correct the lost Packet */
                    for (packet = startPacket; packet < stopPacket; packet++)
                    {
                        if (pInputParams->pDataSegments[packet].bIsValid == TRUE)
                        {
                            if (packet == pInputParams->numDataSegments)
                            {
                                ulGoodPacketSize = pInputParams->dataLength - pInputParams->pDataSegments[packet].ulOffset;
                            }
                            else
                            {
                                ulGoodPacketSize = pInputParams->pDataSegments[packet+1].ulOffset - pInputParams->pDataSegments[packet].ulOffset;
                            }
                            if (ulLostPacketSize < ulGoodPacketSize)
                                ulGoodPacketSize = ulLostPacketSize;

                            pGoodPacket = pData + pInputParams->pDataSegments[packet].ulOffset;
                            for (byte_in = 0; byte_in < ulGoodPacketSize; byte_in++)
                            {
                                pLostPacket[byte_in] ^= pGoodPacket[byte_in];
                            }
                        }
                    }

                    pGoodPacket = pData + pInputParams->pDataSegments[ECCPacket].ulOffset;
                    for (byte_in = 0; byte_in < ulLostPacketSize; byte_in++)
                    {
                        pLostPacket[byte_in] ^= pGoodPacket[byte_in];
                    }

                    /* Assign the first byte of lost packet, and make it valid */
                    pLostPacket[0] = (UINT8)(pData[pInputParams->pDataSegments[ECCPacket].ulOffset] & (~pFrontEnd->ulECCMask));
                    pInputParams->pDataSegments[lostPacket].bIsValid = TRUE;
                }
            }
        }
    }

    /* Invalidate ECC packets at the end */
    while (((pInputParams->pDataSegments[pInputParams->numDataSegments].bIsValid == TRUE) &&
            (pData[pInputParams->pDataSegments[pInputParams->numDataSegments].ulOffset] & pFrontEnd->ulECCMask)) ||
            (pInputParams->pDataSegments[pInputParams->numDataSegments].bIsValid == FALSE))
    {
        pInputParams->pDataSegments[pInputParams->numDataSegments].bIsValid = FALSE;
        pInputParams->dataLength = pInputParams->pDataSegments[pInputParams->numDataSegments].ulOffset;
        if (pInputParams->numDataSegments > 0)
        {
            pInputParams->numDataSegments--;
        }
        else
        {
            break;
        }
    }
}
