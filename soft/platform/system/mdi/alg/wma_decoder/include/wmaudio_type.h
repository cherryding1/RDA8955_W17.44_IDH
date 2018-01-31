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

#ifndef _WMAUDIO_IMPL_H_
#define _WMAUDIO_IMPL_H_

#include "wmatypes.h"
//#include "macros.h"


typedef int WMAERR;
#define WMAERR_OK               0
#define WMAERR_FAIL             1
#define WMAERR_INVALIDARG       2
#define WMAERR_BUFFERTOOSMALL   3
#define WMAERR_INVALIDHEADER    4
#define WMAERR_OUTOFMEMORY      5
#define WMAERR_CORRUPTDATA      6
#define WMAERR_NOTDONE          7 /* need more data to finish the work */
#define WMAERR_INVALIDSTATE     8


//typedef void* WMARawDecHandle;

/* parser state  */
typedef enum
{
    csWMA_NotValid = 0,   /*0*/

    csWMA_HeaderStart,  /*1*/
    csWMA_HeaderError,  /*2*/

    csWMA_NewAsfPacket, /*3*/

    /* packet parsing states */

    csWMA_DecodePayloadStart,   /*4*/
    csWMA_DecodePayload,            /*5*/
    csWMA_DecodePayloadHeader,  /*6*/
    csWMA_DecodeLoopStart,      /*7*/
    csWMA_DecodePayloadEnd,     /*8*/
    csWMA_DecodeCompressedPayload,/*9*/

    csWMA_End_                      /*10*/

} tWMAParseState;

/* 8-bit signed type  */
typedef uint8 tWMA_U8;
/* 16-bit signed type  */
typedef short tWMA_I16;
/* 16-bit unsigned type  */
typedef short tWMA_U16;
/* 32-bit unsigned type  */
typedef unsigned long tWMA_U32;
/* Bool */
typedef long tWMA_Bool;
/* 64-bit unsigned type  */
typedef unsigned long int    tWMA_U64;


#define DESC_NAME_MAX_LENGTH   64
typedef struct _MarkerEntry
{
    QWORD   m_qOffset;
    QWORD   m_qtime;
    tWMA_U16    m_wEntryLen;
    tWMA_U32   m_dwSendTime;
    tWMA_U32   m_dwFlags;
    tWMA_U32   m_dwDescLen;
    tWMA_U16   m_pwDescName[DESC_NAME_MAX_LENGTH];
} MarkerEntry;

typedef struct _ECD_DESCRIPTOR
{
    tWMA_U16         cbName;
    tWMA_U16     *pwszName;
    tWMA_U16         data_type;
    tWMA_U16         cbValue;
    union
    {
        tWMA_U16 *pwszString;
        tWMA_U8 *pbBinary;
        tWMA_Bool *pfBool;
        tWMA_U32 *pdwDword;
        tWMA_U64 *pqwQword;
        tWMA_U16  *pwWord;
    } uValue;
} ECD_DESCRIPTOR;

typedef struct tagWMAExtendedContentDescription
{
    tWMA_I16 cDescriptors;             // number of descriptors
    ECD_DESCRIPTOR *pDescriptors;  // pointer to all the descriptors
} tWMAExtendedContentDesc;

/* internal structure for the ASF header parsing */
typedef struct tagWMAFileHdrStateInternal
{
    void* pInput;
    DWORD currPacketOffset;     /* not just for packets */
    DWORD nextPacketOffset;

    /* ASF header */

    DWORD   cbHeader;
    DWORD   cbPacketSize;
    DWORD   cbAudioSize;
    DWORD   cPackets;
    DWORD   msDuration;
    DWORD   msPreroll;
    DWORD   cbFirstPacketOffset;
    DWORD   cbLastPacketOffset;

    /* audio prop */

    DWORD    nVersion;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    //DWORD   nBlockAlign;
    unsigned short nBlockAlign;
    DWORD    nChannels;
    DWORD   nSamplesPerBlock;
    DWORD    nEncodeOpt;

    /* DRM */

    BYTE    pbSecretData[32];
    DWORD   cbSecretData;
    BYTE    pbType[16];
    BYTE    pbKeyID[32];

    /* Content Description */

    DWORD   cbCDOffset;
    WORD    cbCDTitle;
    WORD    cbCDAuthor;
    WORD    cbCDCopyright;
    WORD    cbCDDescription;
    WORD    cbCDRating;

    /* Extended Content Description */

    tWMAExtendedContentDesc *m_pECDesc;

    /* Marker */
    DWORD   m_dwMarkerNum;
    MarkerEntry *m_pMarker;

    /* License Store */
    DWORD   m_dwLicenseLen;
    BYTE   *m_pLicData;


// Members added by Amit
    DWORD   wAudioStreamId;

} tWMAFileHdrStateInternal;


/* internal structure for the WMAudio library state */
typedef struct tagWMAFileStateInternal
{
    tWMAFileHdrStateInternal hdr_parse;

    int32 playprogress;
    tWMAParseState parse_state;// parse_state
    /*

        DWORD cbPayloadOffset;    //payload parsing

        unsigned short cbPayloadLeft;

        boolean  bBlockStart;

        unsigned short cbBlockLeft;





        DWORD iPayload;    // packet parsing

        DWORD wPayStart;



        boolean    bHasDRM;// other states

        DWORD   nSampleCount;



        void* hWMA;    //objects

        WORD bDecInWaitState;
        WORD bAllFramesDone;
        WORD bFirst;
        BYTE *pCallBackBuffer;*/
} tWMAFileStateInternal;



//typedef enum {TT_NONE = 0, TT_SIMPLE} TRANSCODETYPE;
//typedef enum {STEREO_INVALID = -1, STEREO_LEFTRIGHT = 0, STEREO_SUMDIFF} StereoMode;
typedef enum {CODEC_NULL, CODEC_STEADY, CODEC_DONE, CODEC_BEGIN, CODEC_LAST, CODEC_ONHOLD} Status;
//typedef enum {FMU_DONE = 0, FMU_QUANTIZE, FMU_PROCESS} FMU_STATUS;
//typedef enum {FFT_FORWARD = 0, FFT_INVERSE} FftDirection;

//#define MAX_SUBFRAMES       16

typedef enum {LPC_MODE=0,BARK_MODE} WeightMode;

//typedef enum {BEGIN_PACKET, BEGIN_FRAME, BEGIN_SUBFRAME, DECODE_SUBFRAME, END_SUBFRAME1, END_SUBFRAME2} DecodeStatus;
//typedef enum {SUBFRM_HDR, SUBFRM_COEF, SUBFRM_DONE} SubFrmDecodeStatus;
//typedef enum {HDR_SIZE, HDR_QUANT, HDR_NOISE1, HDR_NOISE2, HDR_MSKUPD, HDR_BARK, HDR_DONE} HdrDecodeStatus; //and a lot more
//typedef enum {VLC, FRAME_END, NORMAL, ESCAPE} RunLevelStatus;


typedef enum tagWMAFileStatus
{
    cWMA_NoErr=0,             /*0*/    /* -> always first entry */
    /* remaining entry order is not guaranteed */
    cWMA_Failed,            /*1*/
    cWMA_BadArgument,       /*2*/
    cWMA_BadAsfHeader,  /*3*/
    cWMA_BadPacketHeader,/*4*/
    cWMA_BrokenFrame,       /*5*/
    cWMA_NoMoreFrames,  /*6*/
    cWMA_BadSamplingRate,/*7*/
    cWMA_BadNumberOfChannels,/*8*/
    cWMA_BadVersionNumber,  /*9*/
    cWMA_BadWeightingMode,  /*10*/
    cWMA_BadPacketization,  /*11*/

    cWMA_BadDRMType,            /*12*/
    cWMA_DRMFailed,         /*13*/
    cWMA_DRMUnsupported,        /*14*/

    cWMA_DemoExpired,           /*15*/

    cWMA_BadState,              /*16*/
    cWMA_Internal,            /*17*/    /* really bad */
    cWMA_NoMoreDataThisTime /*18*/
} tWMAFileStatus;


#endif //_WMAUDIO_IMPL_H_



