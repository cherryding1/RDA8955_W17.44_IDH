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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#endif
#include "rm_parse.h"
#include "rv_depack.h"
#include "rv_decode.h"
#include "ra_depack.h"
#include "ra_decode.h"
#include "rm_parser_internal.h"
#include "helix_types.h"
#include "rm_dec.h"
#include "rm_memory_default.h"
#include "rm_io_default.h"
#include "packet_defines.h"
#include "frame.h"
#include "rvcfg.h"
#include "ra8lbr_utils.h"
#include "mdi_alg_common.h"

#define RM2YUV_INITIAL_READ_SIZE 16

typedef struct
{
    FILE_HDL      fpOut;
    rv_depack* pDepack;
    rv_decode* pDecode;
    BYTE*      pOutFrame;
    UINT32     ulOutFrameSize;
    UINT32     ulWidth;
    UINT32     ulHeight;
    UINT32     ulNumInputFrames;
    UINT32     ulNumOutputFrames;
} rm2yuv_info;

typedef struct
{
    FILE_HDL      fpOut;
    ra_depack* pDepack;
    ra_decode* pDecode;
    BYTE*      pOutBuf;
    UINT32     ulOutBufSize;
    UINT32     ulDataBytesWritten;
} rm2wav_info;


/*
globle parameters
for rm_init() rm_destroy()
add sheen
*/

#define RA_TEMPNUF_SIZE 2048
#define RA_SUPERBLK_SIZE 25020

static rm_parser* pParser;
static rm_stream_header*   pRvHdr;
static rm_stream_header*   pRaHdr;
static rm_packet*          pRvPacket;
static rm_packet*          pRaPacket;
static rv_depack*          pRvDepack;
static ra_depack*          pRaDepack;
static rv_format_info*     pRvInfo;
static ra_format_info*     pRaInfo;
static rv_decode*          pRvDecode;
static ra_decode*          pRaDecode;
static UINT16              usRvStreamNum;
static UINT16              usRaStreamNum;
//static BYTE*               pRvOutFrame;
static BYTE*               pRaOutBuf;
static UINT32              RaReadLen;
static UINT32              RaOutLen;
static BYTE*               pRaTempBuf;
static BYTE*               pRaTempBufPos;
static UINT32              RaTempRemain;
static rm2yuv_info         rvInfo;
static rm2wav_info         raInfo;
static char RV_GET_ONE_FRAME;// 1=y 0=n, add sheen
volatile UINT32 rv_Timestamp_dis;
//static UINT32 rv_Timestamp_dec;
//static UINT32 rv_cumulativeTR_dec;
static UINT32 ra_Timestamp_start;
static UINT32 raFileOffset;
static UINT32 rvFileOffset;
BYTE needBackUpFileOffset;
//extern VocWorkStruct *pRmVocWorkState;
extern VocWorkStruct vocWorkState;
extern I8 rm_video_key_seek_event;
INT8 RV_SKIP_B_FLAG;// 0=dec B frame, 1=skip B frame
extern U32          g_number_of_loads ;

#define RAAC_HEAD_SIZE 7 //AAC
#define RAAC_SR_TAB_NUM 12
static UINT8 raacHead[RAAC_HEAD_SIZE];
const UINT32 sampRateTab[RAAC_SR_TAB_NUM] =
{
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025,  8000
};


typedef struct
{
    /*original stream data*/
    UINT8 *stream;
    /*the byte readen/written */
    UINT8 current;
    /*the number of bits in the current byte*/
    UINT32 nbBits;
} rm_BitToInt ;


//add sheen
#define WRITE_RAAC_HEAD(sampLen)\
{\
    raacHead[3]&=0xfc;\
    raacHead[4]=0;\
    raacHead[5]&=0x1f;\
    raacHead[5]|=(((sampLen)&0x07)<<5);\
    raacHead[4]=(((sampLen)&0x07f8)>>3);\
    raacHead[3]|=(((sampLen)&0x1800)>>11);\
}

void rm_bit_to_int(rm_BitToInt *bs, INT32 value, INT32 nBits)
{
    value <<= sizeof (INT32) * 8 - nBits;
    while (--nBits >= 0)
    {
        bs->current <<= 1;
        bs->current |= (UINT8)(value < 0);
        if (++ bs->nbBits == 8)
        {
            bs->nbBits = 0;
            *(bs->stream)=bs->current;
            bs->stream+=1;
            bs->current = 0;
        }
        value <<= 1;
    }
}
//add end

void rm2yuv_error(void* pError, HX_RESULT result, const char* pszMsg)
{
    rm_printf("rm2yuv_error pError=0x%08x result=0x%08x msg=%s\n", pError, result, pszMsg);
}

void rm2wav_error(void* pError, HX_RESULT result, const char* pszMsg)
{
    rm_printf("rm2wav_error pError=0x%08x result=0x%08x msg=%s\n", pError, result, pszMsg);
}

//static U32 tk1,tk2;

HX_RESULT rv_frame_available(void* pAvail, UINT32 ulSubStreamNum, rv_frame* pFrame)
{
    HX_RESULT retVal = HXR_FAIL;
    UINT32 ulWidth, ulHeight, ulOutFrameSize;

    if (pAvail && pFrame)
    {
        /* Get the info pointer */
        rm2yuv_info* pInfo = (rm2yuv_info*) pAvail;
        if (pInfo->pDepack && pInfo->pDecode && pInfo->pOutFrame)
        {
            /* Put the frame into rv_decode */
            retVal = rv_decode_stream_input(pInfo->pDecode, pFrame);
#ifndef SHEEN_VC_DEBUG
//tk2=hal_TimGetUpTime();
//rm_printf("read data  time=%d ", (tk2-tk1)*1000/16384);
#endif
            if (HX_SUCCEEDED(retVal))
            {
                /* Increment the number of input frames */
                pInfo->ulNumInputFrames++;
                /* Decode frames until there aren't any more */
                do
                {
                    /* Decode a frame */
                    retVal = rv_decode_stream_decode(pInfo->pDecode, pInfo->pOutFrame);
                    if (HX_SUCCEEDED(retVal))
                    {
                        /* Is there a valid output frame? */
                        if (rv_decode_frame_valid(pInfo->pDecode))
                        {
                            /* Get the dimensions and size of output frame */
                            rv_decode_get_output_dimensions(pInfo->pDecode, &ulWidth, &ulHeight);
                            ulOutFrameSize = rv_decode_get_output_size(pInfo->pDecode);

                            /* Check to see if dimensions have changed */
                            if (ulWidth != pInfo->ulWidth || ulHeight != pInfo->ulHeight)
                            {
                                rm_printf( "Warning: YUV output dimensions changed from "
                                           "%lux%lu to %lux%lu at frame #: %lu\n", pInfo->ulWidth,
                                           pInfo->ulHeight, ulWidth, ulHeight, pInfo->ulNumInputFrames);
                                /* Don't bother resizing to display dimensions */
                                pInfo->ulWidth = ulWidth;
                                pInfo->ulHeight = ulHeight;
                                pInfo->ulOutFrameSize = ulOutFrameSize;
                            }

                            /* Write out the frame */
                            //fwrite(pInfo->pOutFrame, 1, pInfo->ulOutFrameSize, pInfo->fpOut);
                            RV_GET_ONE_FRAME=1;
                            rv_Timestamp_dis=pInfo->pDecode->pOutputParams.timestamp;
                            //rv_Timestamp_dec=(U32)((struct Decoder*)(((struct RVDecoder*)pInfo->pDecode->pDecodeState)->m_coredecoder))->m_pCurrentFrame->m_cumulativeTR;
                            //rv_cumulativeTR_dec=(U32)((struct Decoder*)(((struct RVDecoder*)pInfo->pDecode->pDecodeState)->m_coredecoder))->m_pCurrentFrame->m_timeStamp;
                            /* Increment the number of output frames */
                            pInfo->ulNumOutputFrames++;
                        }
                    }
                }
                while (HX_SUCCEEDED(retVal) && rv_decode_more_frames(pInfo->pDecode));
            }
            /* XXXMEH - I assume I can destroy the input frame now */
            rv_depack_destroy_frame(pInfo->pDepack, &pFrame);
        }
    }

    return retVal;
}


HX_RESULT ra_block_available(void* pAvail, UINT32 ulSubStream, ra_block* pBlock)
{
    HX_RESULT retVal = HXR_FAIL;

    if (pAvail && pBlock && pBlock->pData && pBlock->ulDataLen)
    {

        if(pRaOutBuf!=HXNULL)
        {
            UINT32 ulRaCodec4CC;
            ulRaCodec4CC = ra_depack_get_codec_4cc(pRaDepack, 0);
            if(ulRaCodec4CC==RA_CODEC_COOK)
            {
                if(RaOutLen+pBlock->ulDataLen<RaReadLen)
                {
                    memcpy(pRaOutBuf, pBlock->pData, pBlock->ulDataLen);
                    pRaOutBuf+=pBlock->ulDataLen;
                    RaOutLen+=pBlock->ulDataLen;
                }
                else if(RaOutLen<RaReadLen)
                {
                    memcpy(pRaOutBuf, pBlock->pData, RaReadLen-RaOutLen);
                    pRaOutBuf+=RaReadLen-RaOutLen;

                    if(RA_SUPERBLK_SIZE-RaTempRemain<pBlock->ulDataLen-(RaReadLen-RaOutLen)) return retVal;
                    memcpy(pRaTempBuf+RaTempRemain, pBlock->pData+(RaReadLen-RaOutLen), pBlock->ulDataLen-(RaReadLen-RaOutLen));
                    RaTempRemain+=pBlock->ulDataLen-(RaReadLen-RaOutLen);
                    RaOutLen=RaReadLen;
                }
                else
                {
                    if(RA_SUPERBLK_SIZE-RaTempRemain<pBlock->ulDataLen) return retVal;
                    memcpy(pRaTempBuf+RaTempRemain, pBlock->pData, pBlock->ulDataLen);
                    RaTempRemain+=pBlock->ulDataLen;
                }
            }
            else if(ulRaCodec4CC==RA_CODEC_RAAC || ulRaCodec4CC==RA_CODEC_RACP)
            {
                WRITE_RAAC_HEAD(pBlock->ulDataLen);

                if(RaOutLen+pBlock->ulDataLen+RAAC_HEAD_SIZE<RaReadLen)
                {
                    memcpy(pRaOutBuf, raacHead, RAAC_HEAD_SIZE);
                    memcpy(pRaOutBuf+RAAC_HEAD_SIZE, pBlock->pData, pBlock->ulDataLen);
                    pRaOutBuf+=pBlock->ulDataLen+RAAC_HEAD_SIZE;
                    RaOutLen+=pBlock->ulDataLen+RAAC_HEAD_SIZE;
                }
                else if(RaOutLen<RaReadLen)
                {
                    INT32 size;
                    size=RaReadLen-RaOutLen;
                    if(size>=RAAC_HEAD_SIZE)
                    {
                        memcpy(pRaOutBuf, raacHead, RAAC_HEAD_SIZE);
                        memcpy(pRaOutBuf+RAAC_HEAD_SIZE, pBlock->pData, size-RAAC_HEAD_SIZE);
                        pRaOutBuf+=size;

                        if(RA_TEMPNUF_SIZE-RaTempRemain<pBlock->ulDataLen-(size-RAAC_HEAD_SIZE)) return retVal;
                        memcpy(pRaTempBuf+RaTempRemain, pBlock->pData+(size-RAAC_HEAD_SIZE), pBlock->ulDataLen-(size-RAAC_HEAD_SIZE));
                        RaTempRemain+=pBlock->ulDataLen-(size-RAAC_HEAD_SIZE);
                        RaOutLen=RaReadLen;
                    }
                    else
                    {
                        memcpy(pRaOutBuf, raacHead, size);
                        pRaOutBuf+=size;

                        if(RA_TEMPNUF_SIZE-RaTempRemain<pBlock->ulDataLen+(RAAC_HEAD_SIZE-size)) return retVal;
                        memcpy(pRaTempBuf+RaTempRemain, raacHead+size, RAAC_HEAD_SIZE-size);
                        RaTempRemain+=RAAC_HEAD_SIZE-size;
                        memcpy(pRaTempBuf+RaTempRemain, pBlock->pData, pBlock->ulDataLen);
                        RaTempRemain+=pBlock->ulDataLen;
                        RaOutLen=RaReadLen;
                    }
                }
                else
                {
                    if(RA_TEMPNUF_SIZE-RaTempRemain<pBlock->ulDataLen+RAAC_HEAD_SIZE) return retVal;
                    memcpy(pRaTempBuf+RaTempRemain, raacHead, RAAC_HEAD_SIZE);
                    memcpy(pRaTempBuf+RaTempRemain+RAAC_HEAD_SIZE, pBlock->pData, pBlock->ulDataLen);
                    RaTempRemain+=pBlock->ulDataLen+RAAC_HEAD_SIZE;
                }
            }

            retVal = HXR_OK;
        }
#if 0
        /* Init local variables */
        UINT32 ulBytesConsumed = 0;
        UINT32 ulTotalConsumed = 0;
        UINT32 ulNumSamplesOut = 0;
        UINT32 ulBytesLeft     = 0;
        UINT32 ulBytesWritten  = 0;
        UINT32 i               = 0;
        BYTE*  pTmp            = HXNULL;
        BYTE   ucTmp           = 0;
        /* Get the info pointer */
        rm2wav_info* pInfo = (rm2wav_info*) pAvail;
        /* Sanity check to make sure pInfo has what we need */
        if (pInfo->fpOut && pInfo->pDepack && pInfo->pDecode &&
                pInfo->pOutBuf && pInfo->ulOutBufSize)
        {
            /* Clear the return value */
            retVal = HXR_OK;
            /* Init the bytes left */
            ulBytesLeft = pBlock->ulDataLen;
            /* Loop until we've consumed all the data */
            while (retVal == HXR_OK && ulBytesLeft)
            {
                /* Decode the block */

                retVal = ra_decode_decode(pInfo->pDecode,
                                          pBlock->pData + ulTotalConsumed,
                                          pBlock->ulDataLen - ulTotalConsumed,
                                          &ulBytesConsumed,
                                          (UINT16*) pInfo->pOutBuf,
                                          pInfo->ulOutBufSize / 2,
                                          &ulNumSamplesOut,
                                          pBlock->ulDataFlags);

                if (retVal == HXR_OK)
                {
                    /* Byte-swap the samples
                    for (i = 0; i < ulNumSamplesOut; i++)
                    {
                        pTmp    = pInfo->pOutBuf + (i << 1);
                        ucTmp   = pTmp[0];
                        pTmp[0] = pTmp[1];
                        pTmp[1] = ucTmp;
                    }*/
                    /* Write out the samples to the file */
                    ulBytesWritten = ulNumSamplesOut << 1;
                    if (ulBytesWritten)
                    {
                        fwrite(pInfo->pOutBuf, 1, ulBytesWritten, pInfo->fpOut);
                    }
                    /* Update the number of data bytes written */
                    pInfo->ulDataBytesWritten += ulBytesWritten;
                    /* Update the total amount consumed */
                    ulTotalConsumed += ulBytesConsumed;
                    /* Update the bytes left */
                    ulBytesLeft -= ulBytesConsumed;
                }
            }
        }
#endif
    }

    return retVal;
}


/*
*rm parse and initialize
*add sheen
*/
HX_RESULT rm_init(FILE_HDL fpIn, rmInfo *pRmInfo)
{
    HX_RESULT           retVal         = HXR_OK;
    INT32               lBytesRead     = 0;
    unsigned char       ucBuf[RM2YUV_INITIAL_READ_SIZE];
    UINT32              ulNumStreams   = 0;
    UINT32              i              = 0;
    //UINT32              ulFramesPerSec = 0;
    //UINT32              ulOutFrameSize = 0;
    rm_stream_header*   pHdr=HXNULL;
    UINT32              ulRaCodec4CC     = 0;
    //UINT32              ulMaxSamples   = 0;
    rm_parser_internal* pInt = HXNULL;

    pParser        = HXNULL;
    pRvHdr           = HXNULL;
    pRaHdr      =HXNULL;
    usRvStreamNum    = 0;
    usRaStreamNum    = 0;
    pRvPacket        = HXNULL;
    pRaPacket       =HXNULL;
    pRvDepack        = HXNULL;
    pRaDepack    =HXNULL;
    pRvInfo          = HXNULL;
    pRaInfo          = HXNULL;
    pRvDecode        = HXNULL;
    pRaDecode        = HXNULL;
    //pRvOutFrame      = HXNULL;
    pRaOutBuf    =HXNULL;
    pRaTempBuf= HXNULL;
    pRaTempBufPos=HXNULL;
    RaTempRemain=0;
    RaReadLen=0;
    RaOutLen=0;
    RV_GET_ONE_FRAME=0;
    rv_Timestamp_dis=0;
    ra_Timestamp_start=0;
    raFileOffset=0;
    rvFileOffset=0;
    needBackUpFileOffset=0;

    g_number_of_loads=0;

    memset(&rvInfo, 0, sizeof(rm2yuv_info));
    memset(&raInfo, 0, sizeof(rm2wav_info));

    retVal=rm_memory_default_init();
    if(retVal!=0)
    {
        rm_printf("rm_memory_default_init fail!!!\n");
        retVal=HXR_OUTOFMEMORY;
        goto cleanup;
    }

#if 1
    /* Read the first few bytes of the file */
    lBytesRead = (INT32) rm_io_default_read((void*)fpIn, (void*) ucBuf, RM2YUV_INITIAL_READ_SIZE);
    if (lBytesRead != RM2YUV_INITIAL_READ_SIZE)
    {
        rm_printf("Could not read %d bytes at the beginning of rm file, lBytesRead=%d\n",
                  RM2YUV_INITIAL_READ_SIZE, lBytesRead);
        retVal=HXR_INVALID_FILE;
        goto cleanup;
    }
    /* Seek back to the beginning */
    rm_io_default_seek((void*)fpIn, 0, HX_SEEK_ORIGIN_SET);

    /* Make sure this is an .rm file */
    if (!rm_parser_is_rm_file(ucBuf, RM2YUV_INITIAL_READ_SIZE))
    {
        rm_printf("it is not an .rm file.\n");
        retVal=HXR_NOT_SUPPORTED;
        goto cleanup;
    }

    /* Create the parser struct */
    pParser = rm_parser_create(NULL, rm2yuv_error);
    if (!pParser)
    {
        retVal=HXR_DEC_INIT_FAILED;
        rm_printf("rm_parser_create err!\n");
        goto cleanup;
    }

    /* Set the FILE* into the parser */
    retVal = rm_parser_init_stdio(pParser, fpIn);
    if (retVal != HXR_OK)
    {
        rm_printf("rm_parser_init_stdio err!\n");
        goto cleanup;
    }

    /* Read all the headers at the beginning of the .rm file */
    retVal = rm_parser_read_headers(pParser);
    if (retVal != HXR_OK)
    {
        rm_printf("rm_parser_read_headers err!\n");
        goto cleanup;
    }

    /* Get the number of streams */
    ulNumStreams = rm_parser_get_num_streams(pParser);
    if (ulNumStreams == 0)
    {
        rm_printf("Error: rm_parser_get_num_streams() returns 0\n");
        retVal=HXR_INVALID_STREAM;
        goto cleanup;
    }

    /* Now loop through the stream headers and find the video */
    for (i = 0; i < ulNumStreams && retVal == HXR_OK; i++)
    {
        retVal = rm_parser_get_stream_header(pParser, i, &pHdr);
        if (retVal == HXR_OK)
        {
            if (rm_stream_is_realvideo(pHdr))
            {
                usRvStreamNum = (UINT16) i;
                pRvHdr=pHdr;
                if(pRaHdr!=HXNULL)break;
            }
            else if(rm_stream_is_realaudio(pHdr))
            {
                usRaStreamNum = (UINT16) i;
                pRaHdr=pHdr;
                if(pRvHdr!=HXNULL)break;
            }
            else
            {
                /* Destroy the stream header */
                rm_parser_destroy_stream_header(pParser, &pHdr);
            }
        }
    }

    /* Do we have RealVideo and RealAudio stream in this .rm file? */
    if (!pRvHdr)
    {
        rm_printf("There is no RealVideo stream in this file.\n");
        retVal=HXR_UNSUPPORTED_VIDEO;
        goto cleanup;
    }
    else if(!pRaHdr)
    {
        rm_printf("There is no RealAudio stream in this file.\n");
        retVal=HXR_UNSUPPORTED_AUDIO;
        goto cleanup;
    }

    /* Create the RealVideo depacketizer */
    pRvDepack = rv_depack_create((void*) &rvInfo,
                                 rv_frame_available,
                                 NULL,
                                 rm2yuv_error);
    if (!pRvDepack)
    {
        retVal=HXR_DEC_INIT_FAILED;
        rm_printf("rv_depack_create err!\n");
        goto cleanup;
    }

    /* Assign the rv_depack pointer to the info struct */
    rvInfo.pDepack = pRvDepack;

    /* Create the RealAudio depacketizer */

    pRaDepack = ra_depack_create((void*) &raInfo,
                                 ra_block_available,
                                 NULL,
                                 rm2wav_error);
    if (!pRaDepack)
    {
        retVal=HXR_DEC_INIT_FAILED;
        rm_printf("ra_depack_create err!\n");
        goto cleanup;
    }

    /* Assign the ra_depack pointer to the info struct */
    raInfo.pDepack = pRaDepack;

    /* Initialize the RV depacketizer with the stream header */
    retVal = rv_depack_init(pRvDepack, pRvHdr);
    if (retVal != HXR_OK)
    {
        rm_printf("rv_depack_init err!\n");
        goto cleanup;
    }

    /* Initialize the RA depacketizer with the stream header */
    retVal = ra_depack_init(pRaDepack, pRaHdr);
    if (retVal != HXR_OK)
    {
        rm_printf("ra_depack_init err!\n");
        goto cleanup;
    }

    /* Get the bitstream header information */
    retVal = rv_depack_get_codec_init_info(pRvDepack, &pRvInfo);
    if (retVal != HXR_OK)
    {
        rm_printf("rv_depack_get_codec_init_info err!\n");
        goto cleanup;
    }

    /* Fill in the width and height */
    rvInfo.ulWidth  = (UINT32) pRvInfo->usWidth;
    rvInfo.ulHeight = (UINT32) pRvInfo->usHeight;

    /*
    * Get the codec 4CC of substream 0. We
    * arbitrarily choose substream 0 here.
    */
    ulRaCodec4CC = ra_depack_get_codec_4cc(pRaDepack, 0);

    /*
    * Get the bitstream header information. Again,
    * we arbitrarily choose substream 0.
    */
    retVal = ra_depack_get_codec_init_info(pRaDepack, 0, &pRaInfo);
    if (retVal != HXR_OK)
    {
        rm_printf("ra_depack_get_codec_init_info err!\n");
        goto cleanup;
    }

    /*
     * Print out the width and height so the user
     * can input this into their YUV-viewing program.
     */
    //rm_printf( "Video dimensions %lu x %lu pixels (width x height)\n",
    //          info.ulWidth, info.ulHeight);

    /*
     * Get the frames per second. This value is in 32-bit
     * fixed point, where the upper 16 bits is the integer
     * part of the fps, and the lower 16 bits is the fractional
     * part. We're going to truncate to integer, so all
     * we need is the upper 16 bits.
     */
    //ulFramesPerSec = pInfo->ufFramesPerSecond >> 16;

    /* Create an rv_decode object */
    pRvDecode = rv_decode_create(HXNULL, rm2yuv_error);
    if (!pRvDecode)
    {
        rm_printf("rv_decode_create err!\n");
        retVal=HXR_DEC_INIT_FAILED;
        goto cleanup;
    }

    /* Assign the decode object into the rm2yuv_info struct */
    rvInfo.pDecode = pRvDecode;

    /* Create the ra_decode object */
    /*
    pRaDecode = ra_decode_create(HXNULL, rm2wav_error);

    if (!pRaDecode)
    {
        goto cleanup;
    }*/

    /* Assign the decode object into the rm2wav_info struct */
    //raInfo.pDecode = pRaDecode;

    /* Init the rv_decode object */
    retVal = rv_decode_init(pRvDecode, pRvInfo);
    if (retVal != HXR_OK)
    {
        rm_printf("rv_decode_init err!\n");
        goto cleanup;
    }

    /* Init the ra_decode object */
    /*  retVal = ra_decode_init(pRaDecode, ulRaCodec4CC, HXNULL, 0, pRaInfo);
      if (retVal != HXR_OK)
      {
          goto cleanup;
      }*/

#if 0
    /* Get the size of an output frame */
    ulOutFrameSize = rv_decode_max_output_size(pRvDecode);
    if (!ulOutFrameSize)
    {
        retVal=HXR_UNSUPPORTED_VIDEO;
        rm_printf("rv_decode_max_output_size err!\n");
        goto cleanup;
    }

    /* Allocate an output frame buffer */
    pRvOutFrame = (BYTE*) rm_memory_default_malloc(0, ulOutFrameSize);
#else
    /*
    20 bytes in head store 5 int value: [0]Y pos [1]U pos [2]V pos [3]width [4]high.
    then the padding U V buffer for reorder interlace line UV data.
    Y use the decoder buffer.sheen
    */
    //if(rvInfo.ulWidth && rvInfo.ulHeight)
    //  pRvOutFrame = (BYTE*) rm_memory_default_malloc(0, 4*5 + (rvInfo.ulWidth+YUV_UV_PADDING*4)*(rvInfo.ulHeight>>1));
#endif
    /*
    if (!pRvOutFrame)
    {
         retVal=HXR_OUTOFMEMORY;
         rm_printf("pRvOutFrame err!\n");
        goto cleanup;
    }*/

    /* Assign the output frame into the info struct */
    // rvInfo.pOutFrame      = pRvOutFrame;
    // rvInfo.ulOutFrameSize = ulOutFrameSize;

    /* Get the maximum number of output samples */
    /*   retVal = ra_decode_getmaxsize(pRaDecode, &ulMaxSamples);
       if (retVal != HXR_OK)
       {
           goto cleanup;
       }*/
    /* Compute the size of the output buffer */
    // ulOutFrameSize = ulMaxSamples * sizeof(UINT16);
    /* Allocate an output buffer */
    /*
    pOutBuf = (BYTE*) malloc(ulOutFrameSize);
    if (!pOutBuf)
    {
        goto cleanup;
    }*/

    /* Assign the output frame into the info struct */
    /*
    raInfo.pOutBuf      = pOutBuf;
    raInfo.ulOutBufSize = ulOutBufSize;
    */
#endif

    if(ulRaCodec4CC==RA_CODEC_COOK )
        pRaTempBuf=  (BYTE*) rm_memory_default_malloc(0, RA_SUPERBLK_SIZE);
    else//for AAC
        pRaTempBuf=  (BYTE*) rm_memory_default_malloc(0, RA_TEMPNUF_SIZE);

    if (!pRaTempBuf)
    {
        retVal=HXR_OUTOFMEMORY;
        rm_printf("pRaTempBuf err!\n");
        goto cleanup;
    }
    pRaTempBufPos=pRaTempBuf;

    pInt = (rm_parser_internal*) pParser;
    raFileOffset=rvFileOffset=pInt->ulCurFileOffset;

    if(ulRaCodec4CC==RA_CODEC_COOK && pRaInfo->ulOpaqueDataSize > 0)
    {
        ra8lbr_data codecData = {0, 0, 0, 0, 0, 0, 0};

        retVal = ra8lbr_unpack_opaque_data(&codecData, &pRaInfo->pOpaqueData, &pRaInfo->ulOpaqueDataSize);
        if (retVal == HXR_OK)
        {
            pRmInfo->ra_SamplePerFrame=codecData.nSamples;
            pRmInfo->ra_Regions=codecData.nRegions;
            pRmInfo->ra_cplStart=codecData.cplStart;
            pRmInfo->ra_cplQBits=codecData.cplQBits;
        }
    }

    /*output rm information*/
    pRmInfo->rm_Duration=((rm_parser_internal*) pParser)->pMediaPropsHdr[usRvStreamNum].duration; // use video time to repalce a/v longest time rm_parser_get_duration( pParser);
    pRmInfo->rm_NumStream=rm_parser_get_num_streams( pParser);
    pRmInfo->rv_Format=pRvInfo->ulSubMOFTag;
    pRmInfo->rv_Width=rvInfo.ulWidth;
    pRmInfo->rv_Height=rvInfo.ulHeight;
    pRmInfo->rv_Fps= pRvInfo->ufFramesPerSecond >> 16;

    pRmInfo->ra_Format= ulRaCodec4CC;
    pRmInfo->ra_SampleRate= pRaInfo->ulSampleRate;
    pRmInfo->ra_BitsPerSample= pRaInfo->usBitsPerSample;
    pRmInfo->ra_NumChannels= pRaInfo->usNumChannels;
    pRmInfo->ra_BitsPerFrame=pRaInfo->ulBitsPerFrame;

    if(ulRaCodec4CC==RA_CODEC_RAAC || ulRaCodec4CC==RA_CODEC_RACP)
    {
        rm_BitToInt bs;
        BYTE aacType;
        INT32 sampRateIdx;
        memset(raacHead, 0, RAAC_HEAD_SIZE);
        bs.stream=raacHead;
        bs.current=0;
        bs.nbBits=0;

        aacType=ra_depack_get_codec_aac_type(pRaDepack, 0);
        for(sampRateIdx=0; sampRateIdx<RAAC_SR_TAB_NUM; sampRateIdx++)
        {
            if(pRaInfo->ulSampleRate==sampRateTab[sampRateIdx])
                break;
        }

        rm_bit_to_int(&bs, 0xFFF, 12);/*sync*/
        rm_bit_to_int(&bs, ((aacType&0x7)==0)?0:1, 1);/*1=mpeg2 aac / 0=mpeg4 aac*/
        rm_bit_to_int(&bs, 0, 2); /*layer*/
        rm_bit_to_int(&bs, 1, 1); /* protection_absent. 0 = CRC word follows, 1 = no CRC word */
        rm_bit_to_int(&bs, /*(aacType>>3)*/1, 2); /*profile, 0 = main, 1 = LC, 2 = SSR, 3 = reserved, only LC now */
        rm_bit_to_int(&bs, sampRateIdx, 4);/* sample rate index range = [0, 11] */
        rm_bit_to_int(&bs, 0, 1);/* ignore */
        rm_bit_to_int(&bs, pRaInfo->usNumChannels, 3);/* channel, 0 = implicit, >0 = use default table */
        rm_bit_to_int(&bs, 0, 4);/* ignore */
        rm_bit_to_int(&bs, 0, 13);//frame size.13bit will be rewrite when every sample read
        rm_bit_to_int(&bs, 0x7FF, 11);/* number of 32-bit words left in enc buffer, 0x7FF = VBR */
        rm_bit_to_int(&bs, 0, 2);/* number of raw data blocks in frame */

    }

    rm_video_key_seek_event=0;

    return retVal;


cleanup:

    rm_destroy(fpIn);

    return retVal;
}

/*
*rm destroy and free
*add sheen
*/
HX_RESULT rm_destroy(FILE_HDL fpIn)
{

#if 1

    /* Free the frame buffer */
    /*
    if (pRvOutFrame)
    {
        rm_memory_default_free(0, pRvOutFrame);
        pRvOutFrame = HXNULL;
    }*/
    /* Destroy the codec init info */
    if (pRvInfo)
    {
        rv_depack_destroy_codec_init_info(pRvDepack, &pRvInfo);
    }

    if (pRaInfo)
    {
        ra_depack_destroy_codec_init_info(pRaDepack, &pRaInfo);
    }

    /* Destroy the depacketizer */
    if (pRvDepack)
    {
        rv_depack_destroy(&pRvDepack);
    }

    if (pRaDepack)
    {
        ra_depack_destroy(&pRaDepack);
    }
    /* If we have a packet, destroy it */
    if (pRvPacket)
    {
        rm_parser_destroy_packet(pParser, &pRvPacket);
    }
    if (pRaPacket)
    {
        rm_parser_destroy_packet(pParser, &pRaPacket);
    }

    /* Destroy the rv stream header */
    if (pRvHdr)
    {
        rm_parser_destroy_stream_header(pParser, &pRvHdr);
    }
    /* Destroy the ra stream header */
    if (pRaHdr)
    {
        rm_parser_destroy_stream_header(pParser, &pRaHdr);
    }
    /* Destroy the rv_decode object */
    if (pRvDecode)
    {
        rv_decode_destroy(pRvDecode);
        pRvDecode = HXNULL;
    }
    /* Destroy the ra_decode object */
    /*if (pRaDecode)
    {
        ra_decode_destroy(pRaDecode);
        pRaDecode = HXNULL;
    }*/
    /* Destroy the parser */
    if (pParser)
    {
        rm_parser_destroy(&pParser);
    }

    if(pRaTempBuf)
    {
        rm_memory_default_free(0, pRaTempBuf);
    }
    /* Close the output file */
    /*if (fpOut)
    {
        fclose(fpOut);
        fpOut = HXNULL;
    }*/
    /* Close the input file */
    /*
    if (fpIn)
    {
        fclose(fpIn);
        //fpIn = HXNULL;
    }*/

    rm_memory_default_free_all();
#endif

    return HXR_OK;
}

/*
*rm read video packet and decode one frame
*add sheen
*/
HX_RESULT rm_getOneFrameVideo(INT8* pOut, /*VocWorkStruct *pVocState, */INT8 skipBFlag)
{
    HX_RESULT           retVal         = HXR_OK;
    //UINT32 *pOutInfo=(UINT32*)pOut;

    rm_parser_internal* pInt = (rm_parser_internal*) pParser;

    if(!pOut /*|| !pVocState*/) return HXR_INVALID_PARAMETER;
    rvInfo.pOutFrame=pOut;
    //pRmVocWorkState=pVocState;
    RV_SKIP_B_FLAG=skipBFlag;

#ifndef SHEEN_VC_DEBUG
//tk1=hal_TimGetUpTime();
#endif
    if(rvFileOffset!=pInt->ulCurFileOffset)
    {
        pInt->fpSeek( pInt->pUserRead , rvFileOffset, HX_SEEK_ORIGIN_SET);
        pInt->ulCurFileOffset=rvFileOffset;
    }

    /* Now keep getting packets until we receive an error */
    while (retVal == HXR_OK)
    {
        /* Get the next packet */
        retVal = rm_parser_get_packet(pParser, &pRvPacket, usRvStreamNum);
        if (retVal == HXR_OK)
        {
            /* Is this a video packet? */
            if (pRvPacket->usStream == usRvStreamNum)
            {
                /*
                 * Put the packet into the depacketizer. When frames
                 * are available, we will get a callback to
                 * rv_frame_available().
                 */
                retVal = rv_depack_add_packet(pRvDepack, pRvPacket);
            }
            /* Destroy the packet */
            rm_parser_destroy_packet(pParser, &pRvPacket);
        }

        if(RV_GET_ONE_FRAME==1)
        {
            rvFileOffset=pInt->ulCurFileOffset;
            RV_GET_ONE_FRAME=0;
            /*
            pOutInfo[0]=(UINT32)rvInfo.pOutFrame;//Y
            pOutInfo[1]=(UINT32)(rvInfo.pOutFrame+rvInfo.ulWidth*rvInfo.ulHeight);//U
            pOutInfo[2]=(UINT32)(rvInfo.pOutFrame+((rvInfo.ulWidth*rvInfo.ulHeight*5)>>2));//V
            pOutInfo[3]=(UINT32)rvInfo.ulWidth;//width
            pOutInfo[4]=(UINT32)rvInfo.ulHeight;//height
            */
            //memcpy(pOutInfo, rvInfo.pOutFrame, 4*5);
            return retVal;
        }
        else if(retVal==HXR_SKIP_BFRAME)
        {
            rvFileOffset=pInt->ulCurFileOffset;
            return retVal;
        }
    }


    /* Flush the decoder in case there is a latency frame */
    if (rv_decode_stream_flush(rvInfo.pDecode, rvInfo.pOutFrame) == HXR_OK)
    {
        /* Is there a valid output frame? */
        if (rv_decode_frame_valid(rvInfo.pDecode))
        {
            /* Write out the frame */
            //fwrite(info.pOutFrame, 1, info.ulOutFrameSize, info.fpOut);
            /* Increment the number of output frames */
            rvInfo.ulNumOutputFrames++;
            /*
            pOutInfo[0]=(UINT32)rvInfo.pOutFrame;//Y
            pOutInfo[1]=(UINT32)(rvInfo.pOutFrame+rvInfo.ulWidth*rvInfo.ulHeight);//U
            pOutInfo[2]=(UINT32)(rvInfo.pOutFrame+((rvInfo.ulWidth*rvInfo.ulHeight*5)>>2));//V
            pOutInfo[3]=(UINT32)rvInfo.ulWidth;//width
            pOutInfo[4]=(UINT32)rvInfo.ulHeight;//height
            */
            //memcpy(pOutInfo, rvInfo.pOutFrame, 4*5);
        }
    }

    return retVal;
}

/*
*rm get some audio packet
return real get length
*add sheen
*/
UINT32 rm_getAudioData(char *pOutBuf, UINT32 readLen)
{
    HX_RESULT           retVal         = HXR_OK;
    rm_parser_internal* pInt = (rm_parser_internal*) pParser;
    UINT32 retLen=0;
    UINT32 lastFileOffset=pInt->ulCurFileOffset;

    if(pOutBuf==HXNULL)
    {
        return HXR_INVALID_PARAMETER;
    }

    if(RaTempRemain>0)
    {
        if(RaTempRemain>readLen)
        {
            memcpy(pOutBuf, pRaTempBufPos, readLen);
            pRaTempBufPos+=readLen;
            RaTempRemain-=readLen;
            retLen=readLen;
            return retLen;
        }
        else if(RaTempRemain==readLen)
        {
            memcpy(pOutBuf, pRaTempBufPos, readLen);
            pRaTempBufPos=pRaTempBuf;
            RaTempRemain=0;
            retLen=readLen;
            return retLen;
        }
        else
        {
            memcpy(pOutBuf, pRaTempBufPos, RaTempRemain);
            pRaTempBufPos=pRaTempBuf;
            pRaOutBuf=pOutBuf+RaTempRemain;
            RaReadLen=readLen-RaTempRemain;
            retLen=RaTempRemain;
            RaTempRemain=0;
        }
    }
    else
    {
        pRaOutBuf=pOutBuf;
        RaReadLen=readLen;
    }

    if(raFileOffset!=pInt->ulCurFileOffset)
    {
        pInt->fpSeek( pInt->pUserRead , raFileOffset, HX_SEEK_ORIGIN_SET);
        pInt->ulCurFileOffset=raFileOffset;
    }

    /* Now keep getting packets until we receive an error */
    while (retVal == HXR_OK)
    {
        /* Get the next packet */
        retVal = rm_parser_get_packet(pParser, &pRaPacket, usRaStreamNum);
        if (retVal == HXR_OK)
        {
            /* Is this an audio packet? */
            if (pRaPacket->usStream == usRaStreamNum)
            {
                if(ra_Timestamp_start==0)
                {
                    //record the first packet time stamp.
                    ra_Timestamp_start=(pRaPacket->ulTime>0)?pRaPacket->ulTime:1;
                }

                /*
                 * Put the packet into the depacketizer. When audio
                 * blocks are available, we will get a callback
                 * to ra_block_available()
                 */
                retVal = ra_depack_add_packet(pRaDepack, pRaPacket);
            }
            /* Destroy the packet */
            rm_parser_destroy_packet(pParser, &pRaPacket);
        }

        if(RaOutLen>0 )
        {
            if( RaOutLen<RaReadLen)
            {
                retLen+=RaOutLen;
                RaReadLen-=RaOutLen;
                RaOutLen=0;
                continue;
            }
            else
            {
                retLen+=RaOutLen;
                break;
            }
        }

    }

    raFileOffset=pInt->ulCurFileOffset;
    pRaOutBuf=HXNULL;
    RaOutLen=0;
    RaReadLen=0;

    if(needBackUpFileOffset==1)
    {
        pInt->fpSeek( pInt->pUserRead , lastFileOffset, HX_SEEK_ORIGIN_SET);
        pInt->ulCurFileOffset=lastFileOffset;
        needBackUpFileOffset=0;
    }
    return retLen;
}

UINT32 rm_getAudioTempBufRemain()
{
    return RaTempRemain;
}

/*
*rm seek postion (ms)
*add sheen
*/
HX_RESULT rm_seek( UINT32 ulTime)
{
    HX_RESULT           retVal         = HXR_OK;
    rm_parser_internal* pInt = (rm_parser_internal*) pParser;

    retVal=rm_parser_seek(pParser, ulTime);

    raFileOffset=rvFileOffset=pInt->ulCurFileOffset;
    pRaTempBufPos=pRaTempBuf;
    RaTempRemain=0;

    rm_video_key_seek_event=2;
    ra_Timestamp_start=0;

    return retVal;
}

/*
*rm video seek postion (ms)
*add sheen
*/
HX_RESULT rm_seek_video( UINT32 ulTime)
{
    HX_RESULT           retVal         = HXR_OK;
    rm_parser_internal* pInt = (rm_parser_internal*) pParser;

    retVal=rm_parser_seek(pParser, ulTime);

    rvFileOffset=pInt->ulCurFileOffset;

    rm_video_key_seek_event=2;

    return retVal;
}

/*
*
*add sheen
*/
UINT32 rm_getKeyTimeStamp( VOID )
{
    UINT32           retVal         = 0;
    rm_parser_internal* pInt = (rm_parser_internal*) pParser;

    if(pInt)
    {
        if(pInt->pStreamInfo)
            retVal=pInt->pStreamInfo[usRvStreamNum].keyFramePacket.ulTimestamp;
    }

    return retVal;
}
/*
seek to next key frame and get real time stamp.
add sheen
*/
UINT32 rm_getNextKeyTimeStamp(UINT32 ulTime)
{
    HX_RESULT           retVal         = HXR_OK;
    rm_parser_internal* pInt = (rm_parser_internal*) pParser;
    /*
    retVal=rm_parser_seek(pParser, ulTime);
    if(retVal==HXR_OK)
    {
        if(pInt->pStreamInfo)
            retVal=pInt->pStreamInfo[usRvStreamNum].keyFramePacket.ulTimestamp;
    }
    else
        retVal=0;
    */
//rm_printf("1 ulTime=%d",ulTime);

    if (pInt && pInt->pStreamInfo)
    {
        HX_RESULT status       = HXR_FAIL;
        UINT32    ulFoundTime  = 0;
        UINT32    ulDataOffset = 0;
        struct rm_stream_info* pInfo = &pInt->pStreamInfo[usRvStreamNum];

        if (pInfo)
        {
            status = rm_parseri_search_seek_table(&pInfo->seekTable,
                                                  ulTime,&ulFoundTime, &ulDataOffset);
        }

        if (status == HXR_OK)
        {
            retVal =ulFoundTime;
        }
        else if (pInt->propHdr.index_offset )
        {
            /*
             * Try to find the offset via the index chunks
             * at the end of the file
             */
            struct rm_index_hdr hdr;
            struct rm_index_rec rec;
            UINT32 ulChunkID     = 0;
            UINT32 ulStreamNum   = 0;
            UINT32 i             = 0;
            UINT32 ulIndexOffset = pInt->propHdr.index_offset;

            /* Clear the return value */
            retVal = HXR_OK;
            /* Loop through all the index chunks */
            while (HX_SUCCEEDED(retVal) && ulIndexOffset)
            {
                /* Seek the file to the next index chunk */
                rm_parseri_file_seek(pInt, ulIndexOffset, HX_SEEK_ORIGIN_SET);
                /* Read the index header */
                retVal = rm_parseri_read_next_header(pInt, &ulChunkID);
                if (retVal == HXR_OK)
                {
                    /* Assume the worst */
                    retVal = HXR_FAIL;
                    /* Make sure this is an INDX header */
                    if (ulChunkID == RM_INDEX_OBJECT)
                    {
                        /* Parse the index header */
                        retVal = rm_parseri_unpack_index_hdr(pInt, &hdr);
                        if (retVal == HXR_OK)
                        {
                            /*
                             * Translate the stream number. If the
                             * stream number doesn't translate, then
                             * we will not consider it an error - we'll
                             * just go on to the next index chunk.
                             */
                            ulStreamNum = rm_parseri_translate_stream_number(pInt, hdr.stream_num);
                            //if (ulStreamNum != RM_NO_STREAM_SET)
                            if (ulStreamNum == usRvStreamNum)
                            {
                                /* Read all the index records in this chunk */
                                for (i = 0; i < hdr.num_recs && retVal == HXR_OK; i++)
                                {
                                    retVal = rm_parseri_read_next_index_rec(pInt, &rec);
                                    if (retVal == HXR_OK)
                                    {
                                        /*
                                         * Set this record into the on-the-fly
                                         * seek table.
                                         */
                                        rm_parseri_update_seek_table(pInt,
                                                                     ulStreamNum,
                                                                     rec.timestamp,
                                                                     rec.offset,
                                                                     HX_KEYFRAME_FLAG);

                                        rm_printf(" rec.timestamp=%d",rec.timestamp);

                                        /*
                                         * Is the timestamp of the record
                                         * greater than or equal to the seek time?
                                         */
                                        if (rec.timestamp >= ulTime)
                                        {
                                            retVal=rec.timestamp;
                                            ulIndexOffset=0;
                                            /*
                                             * Now we can stop looking at index records
                                             * for this stream
                                             */
                                            break;
                                        }

                                    }
                                }
                            }
                            /*
                             * Set the next index header. If there are
                             * no more index chunks, next_index_hdr will
                             * be zero.
                             */
                            if(!ulIndexOffset)break;//get the stamp.
                            ulIndexOffset = hdr.next_index_hdr;
                        }
                    }
                }
            }
        }
    }

    return retVal;
}

/*
call after rm_getOneFrameVideo and return the output frame time stamp.
add sheen.
*/
UINT32 rm_getLastFrameTimeStamp(VOID)
{
    return rv_Timestamp_dis;
}

/*
get first packet time stamp after rm_getAudioData()
NOTE:call after rm_getAudioData()
*/
UINT32 rm_getAudioTimeStampStart(VOID)
{
    return ra_Timestamp_start;
}

#ifdef SHEEN_VC_DEBUG
//for test
UINT8 VOC_WORK;
static void vid_voc_isr()
{
    VOC_WORK=0;
}

int main(int argc, char* argv[])
{
    HX_RESULT           retVal         = HXR_OK;
    FILE*               fp             = HXNULL;
    FILE*               fpOutYUV          = HXNULL;
    FILE*               fpOutAUD          = HXNULL;
    UINT32              i              = 0;
    UINT32              j              = 0;
    rmInfo rm_Info;
    char *pRa=HXNULL;
    UINT32 rvOut[5];//Y,U,V,W,H
    UINT32 raLen=0;
    U8 *pTemp;
    VocWorkStruct VocState;
    I8 skip;

    /* Check the arguments */
    if (argc != 3)
    {
        rm_printf("Usage: %s <rm_file> <yuv_file>\n", argv[0]);
        retVal         = HXR_FAIL;
        return retVal;
    }


    /* Open the input file */
    fp = fopen((const char*) argv[1], "rb");
    if (!fp)
    {
        rm_printf("Could not open %s for reading.\n", argv[1]);
        retVal         = HXR_FAIL;
        return retVal;
    }

    /* Open the output file */
    fpOutYUV = fopen((const char*) argv[2], "wb");
    if (!fpOutYUV)
    {
        rm_printf("Could not open %s for writing.\n", argv[2]);
        retVal         = HXR_FAIL;
        return retVal;
    }

    fpOutAUD = fopen("F://测试文件//test.aac", "wb");
    if (!fpOutAUD)
    {
        rm_printf("Could not open test.aac for writing.\n");
        retVal         = HXR_FAIL;
        return retVal;
    }

    pRa=malloc(8*1024);

    retVal=rm_init(fp, &rm_Info);
    if(retVal != HXR_OK)
        return retVal;

    printf("duration=%d ms\n", rm_Info.rm_Duration);
    printf("num_stream=%d \n", rm_Info.rm_NumStream);
    printf("rv format=%x \n", rm_Info.rv_Format);
    printf("width=%d \n", rm_Info.rv_Width);
    printf("height=%d \n", rm_Info.rv_Height);
    printf("fps=%d \n", rm_Info.rv_Fps);

    printf("ra format=%x \n", rm_Info.ra_Format);
    printf("samplerate=%d \n", rm_Info.ra_SampleRate);
    printf("ra_BitsPerSample=%d \n", rm_Info.ra_BitsPerSample);
    printf("ra_NumChannels=%d \n", rm_Info.ra_NumChannels);

    for(i=0; i<300000; i++)
    {
#if 1
        if(i==0)
        {
#if 0
            raLen=rm_getAudioTempBufRemain();
            raLen= rm_getAudioData(pRa, raLen);
            if(raLen<=0)
            {
                printf("rm rm_getAudioData err retVal=%d\n", raLen);
                //return raLen;
            }

            fwrite(pRa, 1, raLen, fpOutAUD);
            /* Display results */
            rm_printf( "Audio stream remain raLen=%d\n", raLen);
#endif
            retVal=rm_seek(rm_Info.rm_Duration);//init seek table
            retVal=rm_seek(0);
            if(retVal!=HXR_OK)
            {
                printf("rm seek err retVal=%d\n", retVal);
                return retVal;
            }
        }
        else if(i%8==0)
        {
#if 1
            UINT32 timestamp;

            timestamp=rm_getLastFrameTimeStamp();
            timestamp=rm_getNextKeyTimeStamp(timestamp+8000);
            rm_seek_video(timestamp);
            timestamp=rm_getKeyTimeStamp();
#endif
#if 0
            raLen=rm_getAudioTempBufRemain();
            raLen= rm_getAudioData(pRa, raLen);
            if(raLen<=0)
            {
                printf("rm rm_getAudioData err retVal=%d\n", raLen);
                return raLen;
            }

            fwrite(pRa, 1, raLen, fpOutAUD);
            /* Display results */
            rm_printf( "Audio stream remain raLen=%d\n", raLen);

            retVal=rm_seek( 2000);
            if(retVal!=HXR_OK)
            {
                printf("rm seek err retVal=%d\n", retVal);
                return retVal;
            }
#endif
        }
        else if(rm_getLastFrameTimeStamp()==4000)
        {
            UINT32 timestamp;
            //  timestamp=rm_getNextKeyTimeStamp(4000+26*40);
            //rm_seek_video(timestamp);
            //rm_seek(1840);
        }
#endif

        /*output yuv*/
#if 1
        VOC_WORK=0;
        VocState.pVOC_WORK=&VOC_WORK;
        VocState.vid_voc_isr=vid_voc_isr;

        skip=0;
        //if((i&0x3)==3)skip=1;
        retVal=rm_getOneFrameVideo(&rvOut, &VocState, skip);

        if(retVal==HXR_OK)
        {
            //fwrite(rvOut[0], 1, rvOut[3]*rvOut[4]*3/2, fpOutYUV);
            /*
            fwrite(rvOut[0], 1, rvOut[3]*rvOut[4], fpOutYUV);
            fwrite(rvOut[1], 1, rvOut[3]*rvOut[4]>>2, fpOutYUV);
            fwrite(rvOut[2], 1, rvOut[3]*rvOut[4]>>2, fpOutYUV);
            */

            pTemp=(U8*)rvOut[0];
            for(j=0; j<rvOut[4]; j++)
            {
                fwrite(pTemp+ j*rvOut[3], 1, rvInfo.ulWidth, fpOutYUV);
            }

            pTemp=(U8*)rvOut[1];
            for(j=0; j<(rvOut[4])>>1; j++)
            {
                fwrite(pTemp+ (j*rvOut[3]>>1), 1, rvInfo.ulWidth>>1, fpOutYUV);
            }

            pTemp=(U8*)rvOut[2];
            for(j=0; j<(rvOut[4])>>1; j++)
            {
                fwrite(pTemp+ (j*rvOut[3]>>1), 1, rvInfo.ulWidth>>1, fpOutYUV);
            }

            /* Display results */
            rm_printf("RM: F in=%lu, F out=%lu rv_Timestamp_dis=%d\n",
                      rvInfo.ulNumInputFrames, rvInfo.ulNumOutputFrames, rv_Timestamp_dis);
        }
        else if(retVal==HXR_SKIP_BFRAME)
        {
            printf("skip B frame!\n", retVal);
        }
        else
        {
            printf("rm rm_getOneFrameVideo err retVal=%d\n", retVal);
            return retVal;
        }


#endif
        /*output audio*/
#if 1
        raLen=0;
        raLen= rm_getAudioData(pRa, 4096);
        if(raLen<=0)
        {
            printf("rm rm_getAudioData err retVal=%d\n", raLen);
            //return raLen;
        }

        fwrite(pRa, 1, raLen, fpOutAUD);
        /* Display results */
        rm_printf("Audio stream raLen=%d\n", raLen);
#endif

    }

    rm_destroy(fp);

    /* Close the output file */
    if (fpOutYUV)
    {
        fclose(fpOutYUV);
        fpOutYUV = HXNULL;
    }

    if (fpOutAUD)
    {
        fclose(fpOutAUD);
        fpOutAUD = HXNULL;
    }

    if(fp)
    {
        fclose(fp);
        fp = HXNULL;
    }

    return retVal;
}
#endif
