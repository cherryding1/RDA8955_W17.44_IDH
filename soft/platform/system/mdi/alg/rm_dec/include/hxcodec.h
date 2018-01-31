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




#ifndef __HXCODEC_H_
#define __HXCODEC_H_

#include "hxtypes.h"
#include "hxresult.h"
#include "hxcom.h"



#if !defined(BUILD_FOR_REALPRODUCER8)
#define ADD_HXCODEC_INPUT
#if defined (_WIN32)
#define HW_VIDEO_MEMORY_FRONTEND
#endif
#endif
// rv30win.pcf and rv4xwin.pcf will define BUILD_FOR_REALPRODUCER8 if
// an environment variable BUILD_FOR_REALPRODUCER8 exists

#ifndef __cplusplus
typedef _INTERFACE IHXCallback IHXCallback;
#endif

#ifdef __cplusplus

//#include "hxengin.h"
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

#if defined( _WIN32 ) || defined( _WINDOWS )
#pragma pack(1)
//  disable warning on: zero-sized array in struct/union
#pragma warning( disable : 4200 )
#endif

// Byte alignment settings for Mac
#if defined( _MACINTOSH )
#pragma options align=mac68k
#endif

#ifdef __cplusplus
class IHXUnknown;
class IHX20Codec;
class IHX20Stream;
class IHX20MemoryAllocator;
#else // !__cplusplus
typedef void * IHXUnknown;
typedef void * IHX20Codec;
typedef void * IHX20Stream;
typedef void * IHX20MemoryAllocator;
#endif // __cplusplus

typedef IHXUnknown* PIHXUnknown;
typedef ULONG32 HX_IID;

#define IID_IHXUnknown  0x00000000
#define IID_IHX20Codec  0x00000001
#define IID_IHX20Stream 0x00000002
#define IID_IHX20MemoryAllocator 0x00000003

typedef void *HXCODEC;
typedef void *HXSTREAM;
typedef void *HXMEMORY;
typedef void *HXPACKETBUFF;

typedef ULONG32 HX_MOFTAG;

#define STRINGTOMOFTAG(str) (           \
            ((HX_MOFTAG)(unsigned char*)str[0])<<24 |   \
            ((HX_MOFTAG)(unsigned char*)str[1])<<16 |   \
            ((HX_MOFTAG)(unsigned char*)str[2])<<8  |   \
            ((HX_MOFTAG)(unsigned char*)str[3]))

#define HXCODEC_INPUT               1   // Read: Codec, please fill out the input field
#define HXCODEC_OUTPUT              2   // Read: Codec, please fill out the output field
#define HXCODEC_API_VERSION         0x20000000 //(2.0 in PN Version format)
#define HXCODEC_MAX_NAME_LENGTH     32

// HXCODEC return values and errors
//#define HXR_OK                    0
//#define HXR_OUTOFMEMORY               1
#define HXCODEC_LOAD_ERR            101
#define HXCODEC_INVALID_ERR         102
#define HXCODEC_BAD_FORMAT_ERR      103
#define HXCODEC_SETTINGS_ERR        104
#define HXSTREAM_DATA_READY         105
#define HXSTREAM_OPEN_ERR           106
#define HXSTREAM_BUF_SIZE_ERR       107
#define HXSTREAM_INVALID_ERR        108
#define HXSTREAM_BAD_FORMAT_ERR     109
#define HXSTREAM_NO_LOAD_BACKEND    110
#define HXSTREAM_BAD_STATE          111
#define HXSTREAM_PROP_NOT_SUPPORTED 112
#define HXSTREAM_PROP_NOSET_AFTERINPUT 113
#define HXCODEC_NOT_FOUND           114

#define HX_NO_INTERFACE             4096


// Stream property flags
#define SP_INTERPOLATE              0x00010000
#define SP_SPEED_QUALITY            0x00010001  // ULONG32: 1=max speed, 100=max quality
#define SP_LIVE                 0x00010002  // BOOL: TRUE=live source, FALSE=static source
#define SP_BITRATE              0x00010003  // ULONG32: bitrate to encode at
#define SP_VARIABLE_FRAME_RATE          0x00010004  // BOOL: TRUE allow frame rate to vary, FALSE

//  Fixed frame rate.
#define SP_QUALITY_MOTION           0x00010005
#define SP_DITHER                   0x00010006  // ptr to dithering colors.
#define SP_POSTFILTER               0x00010007
#define SP_ADVANCED_MP              0x00010008  // Temporary for fiji-preview
#define SP_TEMPORALINTERP           0x00010009
#define SP_MAX_FRAMERATE            0x00010010
#define SP_INLOOP_DEBLOCKING        0x0001000a
#define SP_LASTENCODETIME           0x0001000B
#define SP_CPUCONSUMPTION           0x0001000C
#define SP_PRIORITY                 0x0001000D
#define SP_FORCEDMV                 0x0001000E
#define SP_HIGHDURESSMODE           0x0001000F
#define SP_MAX_FRAMERATE            0x00010010
#define SP_FRAMERATE                0x00010011


#define SP_CODING_MODE              0x00010012
#define SP_PREVIEW_MODE             0x00010013
#define SP_QUALITY1             0x00010014
#define SP_QUALITY2             0x00010015
#define SP_QUALITY3             0x00010016
#define SP_NEXT_FRAME_TIMESTAMP 0x00010017
#define SP_WILL_ENCODE_FRAME 0x00010018

#define SP_DECODER_CPU_SCALABILITY_STATUS 0x00010019
#define SP_OUPUT_QUEUE_STATUS  0x0001001a
#define SP_50_COMPATIBLE            0x0001001B
#define SP_OUPUT_QUEUE_STATUS2  0x0001001c
#define SP_SEEK         0x0001001d
#define SP_DECODE_B_FRAMES  0x0001001e

#define SP_CODEC_SETUP_DONE 0x0001001f  // pass TRUE for this prop when setup of stream properties is done
#define SP_G2GOLD_COMPATIBLE    0x00010020  // Makes the stream compatible with G2 Gold players
#define SP_ECC                  0x00010021  // Sets the percentage of the bitstream dedicated to error correction
#define ECC_NONE                0
#define ECC_LOW                 1
#define ECC_MED                 2
#define ECC_HIGH                3
#define SP_PREFILTERING_MODE    0x00010022  // Sets the prefiltering mode
#define PREFILTERING_MODE_NONE  0
#define PREFILTERING_MODE_LOW   1
#define PREFILTERING_MODE_HIGH  2
#define SP_ANALYSIS_FILENAME    0x00010023
#define SP_MAXBITRATE           0x00010024
#define SP_KEYFRAMERATE         0x00010025
#define SP_NUMBFRAMES           0x00010026
#define SP_MIN_REDUCEDFRAMERATE 0x00010027
#define SP_TARGET_LATENCY       0x00010028
#define SP_INTERLACE_PREFILTER  0x00010029
#define SP_3_2_PULLDOWN_PREFILTER 0x0001002a
#define SP_ALLOW_DIFFERENT_OUPUT_SIZES 0x0001002b
#define SP_RESIZING_ACCURACY    0x0001002c
#define SP_ALLOW_SIDEBANDINFO   0x0001002d  //flag for allowing sideband info to be add to data stream
#define SP_BITSTREAM_VERSION    0x0001002e  //bit stream versioning
#define SP_CALLBACKFUNCTION            0x0001002f      //so core can validate for secure content
#define SP_BACKENDFUNCTIONPOINTERS     0x00010030     //so core can validate for secure content
#define SP_QUALITY              0x00010031
#define SP_AUTO_PREFILTER       0x00010032
#define SP_COMPONENTGUID        0x00010033
#define SP_BACKENDDECODERCOMPONENTGUID     0x00010034
#define SP_HW_VIDEO_MEMORY      0x00010035
#define SP_NOFULLDECODE         0x00010036
#define SP_NODEBLOCKBFRAMES     0x00010037
#define SP_COOP_MULTITASK_MODE  0x00010038
#define SP_SECURE_MODE      0x00010039
#define SP_DVPF_SURFACE         0x0001003a

#if 0
// co-operative multi-tasking callback
typedef struct tag_HX_CMTM
{
    ULONG32 ulInterval; // desired callback interval in ms (may not be obeyed)
    IHXCallback* pCallback;
} HX_CMTM;

#endif


// output queue status object
typedef struct tag_HX_OQS
{
    ULONG32     queueDepth;     // current size of the queue
    ULONG32     maxQueueDepth;  // desired maximum queue size
    // decode is halted if more than this number of frames is
    // in the queue
    ULONG32     oldestQueueElementTimeStamp;    // timestamp of the oldest thing in the queue
    ULONG32     newestQueueElementTimeStamp;    // timestamp of the youngest thing in the queue
    ULONG32     currentTimeStamp;   // current timestamp used to display elements of the queue
} HX_OQS;

// HX_OQS2 starts with HX_OQS so it can be used for either set property call, the new members
// are just ignored by the SP_OUTPUT_QUEUE_STATUS handler
typedef struct tag_HX_OQS2
{
    ULONG32     queueDepth;     // current size of the queue
    ULONG32     maxQueueDepth;  // desired maximum queue size
    // decode is halted if more than this number of frames is
    // in the queue
    ULONG32     oldestQueueElementTimeStamp;    // timestamp of the oldest thing in the queue
    ULONG32     newestQueueElementTimeStamp;    // timestamp of the youngest thing in the queue
    ULONG32     currentTimeStamp;   // current timestamp used to display elements of the queue
    ULONG32 nonFRUDroppedFrames;
    ULONG32 TotalDroppedFrames;
} HX_OQS2;


/*  SPCODINGMODEs are the allowed values when setting the SP_CODING_MODE param in  */
/*  PNStream_SetProperty  */
#define SPCODINGMODE_VIDEO  0x00000000
#define SPCODINGMODE_POWERPOINT 0x00000001
#define SPCODINGMODE_HIGHDURESS 0x00000002
#define SPCODINGMODE_SLIDESHOW 0x00000003
#define SPCODINGMODE_ANALYSIS 0x00000004
#define SPCODINGMODE_FROMFILE 0x00000005
#define SPCODINGMODE_ENCODE   0x00000006

/*  allowed values for SP_PREVIEW MODE are HX_RGB24_ID or HX_YUV420_ID  */


// Flags used in the data_ready callback function
#define HX_RELIABLE_FLAG            0x0001
#define HX_KEYFRAME_FLAG            0x0002
#define HX_LASTFRAME_FLAG           0x0004
#define HX_INTERFRAME_MODE_FLAG     0x0008


#define HX_INLOOP_DEBLOCKING_FLAG   0x0010
#define HX_POSTFILTER_FLAG          0x0020  // refers to smoothing postfilter OR deblocking postfilter
#define HX_FRU_FLAG                 0x0040
#define HX_SEGMENT_CONTAINS_OUTPUT_SIZE_FLAG 0x0100
#define HX_SEGMENT_CONTAINS_OUTPUT_YUVDATA   0x0200

/*
 * IMPORTANT!!
 * Do not used flag value 0x0008 it is defined as follows in ravents.h
 * #define HX_EVENT_FLAG            0x0008
*/

// HX_MOF base Media object format struct
typedef struct tag_HX_MOF
{
    ULONG32     cbLength;   // size of structure in bytes
    HX_MOFTAG   moftag;     // identifier of media format
    HX_MOFTAG   submoftag;  // identifier of object format
} HX_MOF;

// Media Native HX_MOF struct
typedef struct tag_HX_FORMAT_NATIVE
{
    ULONG32     cbLength;   // the size of this struct in bytes
    HX_MOFTAG   moftag;     // always == HX_MEDIA_NATIVE
    HX_MOFTAG   submoftag;  // media format identifier
    UINT8       data[1];    // format native initialization data
} HX_FORMAT_NATIVE;

// Generic Audio HX_MOF struct
typedef struct tag_HX_FORMAT_AUDIO
{
    ULONG32     cbLength;           // the size of this struct in bytes
    HX_MOFTAG   moftag;             // always == HX_MEDIA_AUDIO
    HX_MOFTAG   submoftag;          // audio format identifier
    // General attributes of the audio stream independent of bitwise encoding
    UINT16      uiBitsPerSample;    // number of bits per audio sample
    UINT16      uiNumChannels;      // number of audio channels in stream
    UINT16      uiBytesPerSample;   // number of bytes per sample
    ULONG32     ulSampleRate;       // audio sample rate
    ULONG32     ulAvgBytesPerSec;   // average bytes/sec of audio stream
} HX_FORMAT_AUDIO;

// Generic Image HX_MOF struct
typedef struct tag_HX_FORMAT_IMAGE
{
    ULONG32     cbLength;           // the size of this struct in bytes
    HX_MOFTAG   moftag;             // always == HX_MEDIA_IMAGE
    HX_MOFTAG   submoftag;          // image format identifier
    // General attributes of the image stream independent of bitwise encoding
    UINT16      uiWidth;            // width of the image in pixels
    UINT16      uiHeight;           // height of the image in pixels
    UINT16      uiBitCount;         // color depth in bits
    UINT16      uiPadWidth;         // number of padded columns for codecs that
    // need certian block sizes e.g. 8x8
    UINT16      uiPadHeight;        // number of padded rows for codecs that
    // need certian block sizes e.g. 8x8
} HX_FORMAT_IMAGE;

// Generic Image HX_MOF struct
typedef struct tag_HX_FORMAT_IMAGE2
{
    ULONG32     cbLength;           // the size of this struct in bytes
    HX_MOFTAG   moftag;             // always == HX_MEDIA_IMAGE
    HX_MOFTAG   submoftag;          // image format identifier
    // General attributes of the image stream independent of bitwise encoding
    UINT16      uiWidth;            // width of the image in pixels
    UINT16      uiHeight;           // height of the image in pixels
    UINT16      uiBitCount;         // color depth in bits
    UINT16      uiPadWidthLeft;         // number of padded columns for codecs that
    // need certian block sizes e.g. 8x8
    UINT16      uiPadHeightTop;     // number of padded rows for codecs that
    // need certian block sizes e.g. 8x8
    UINT16      uiPadWidthRight;            // number of padded columns for codecs that
    // need certian block sizes e.g. 8x8
    UINT16      uiPadHeightBottom;      // number of padded rows for codecs that
    // need certian block sizes e.g. 8x8
} HX_FORMAT_IMAGE2;



// Generic Video HX_MOF struct
typedef struct tag_HX_FORMAT_VIDEO
{
    ULONG32     cbLength;           // the size of this struct in bytes
    HX_MOFTAG   moftag;             // always == HX_MEDIA_VIDEO
    HX_MOFTAG   submoftag;          // video format identifier
    // General attributes of the video stream independent of bitwise encoding
    UINT16      uiWidth;            // width of the image in pixels
    UINT16      uiHeight;           // height of the image in pixels
    UINT16      uiBitCount;         // color depth in bits
    UINT16      uiPadWidth;         // number of padded columns for codecs that
    // need certian block sizes e.g. 8x8
    UINT16      uiPadHeight;        // number of padded rows for codecs that
    // need certian block sizes e.g. 8x8
    UFIXED32    framesPerSecond;    // frames per second

} HX_FORMAT_VIDEO;


typedef struct tag_HX_FORMAT_TRANSFORM
{
    ULONG32 cbLength;               //total transform header length in bytes;
    HX_MOFTAG moftag;               //always == HX_MEDIA_VIDEO
    HX_MOFTAG submoftag;            //fourcc name of transform pluggin
    ULONG32 subcbLength;            //size of specific opaque transform header in bytes
    INT32   viewLeft;               //replicates HXxRect structure for initial viewing position
    INT32   viewTop;
    INT32   viewRight;
    INT32   viewBottom;
} HX_FORMAT_TRANSFORM;


// Unfortunately we did not 4 byte align the HX_FORMAT_VIDEO struct. Since this struct must be 26 bytes in size
// we cannot do sizeof(HX_FORMAT_VIDEO) on several UNIX platforms and PowerPC macs. We are therefore going to
// hardcode the size to 26 bytes.
#define SIZE_OF_HX_FORMAT_VIDEO 26

#define HX_FORMAT_PACKETIZER_SIZE (3 * sizeof(ULONG32))

typedef BOOL (HXEXPORT_PTR FP_MEDIAFORMATSCALLBACK)(void *userData,
        HX_MOF * pmof);

#ifdef _MACINTOSH
#pragma export on
#endif

// HXCODEC_INIT struct used in HXStream_Open()
typedef struct tag_HXCODEC_INIT
{
    HX_MOF          *pInMof;
    HX_MOF          *pOutMof;
    HXMEMORY        memoryRef;
} HXCODEC_INIT;


// HXCODEC_SETMENTINFO
#ifndef TAG_HXCODEC_SEGMENTINFO
#define TAG_HXCODEC_SEGMENTINFO
typedef struct tag_HXCODEC_SEGMENTINFO
{
    LONG32 bIsValid;
    ULONG32 ulSegmentOffset;
} HXCODEC_SEGMENTINFO;
#define HXCODEC_SEGMENTINFO_SIZE    (8)
#endif


// HXCODEC_SETMENTINFO
/*
typedef struct tag_HXCODEC_SEGMENTINFO
{
    BOOL bIsValid;
    ULONG32 ulSegmentOffset;
} HXCODEC_SEGMENTINFO;
#define HXCODEC_SEGMENTINFO_SIZE    (4 + sizeof(BOOL)) // BOOL is an int which changes size
*/



// HXCODEC_DATA struct used in PNStream_Input() and data_callback functions.
typedef struct tag_HXCODEC_DATA
{
    ULONG32     dataLength;
    UCHAR       *data;
    ULONG32     timestamp;
    UINT16      sequenceNum;
    UINT16      flags;
    BOOL        lastPacket;
    ULONG32     numSegments;
    HXCODEC_SEGMENTINFO Segments[1];
} HXCODEC_DATA;



// Since this is an internal structure for which we don't know the alignment
// changed to use sizeof srobinson 11/98
#define HXCODEC_DATA_SIZE   (sizeof(HXCODEC_DATA))
#define HXCODEC_PTR_POPULATED_SIZE(x)   (((UINT8*) (&((x)->Segments[(x)->numSegments]))) - ((UINT8*) (x)))


HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_Open)(HX_MOFTAG moftFormatTag,HXCODEC *codecRef);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_Close)(HXCODEC codecRef);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_GetUIName)(HXCODEC codecRef,char *nameBuf);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_GetVersion)(HXCODEC codecRef, ULONG32 *pVersion);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_QueryMediaFormat)(HXCODEC codecRef,HX_MOF * pmofIn,
        HX_MOF * pmofOut, UINT16 ioDirection);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_PreferredMediaFormat)(HXCODEC codecRef,HX_MOF * pmofIn,
        HX_MOF * pmofOut, UINT16 ioDirection);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_GetMediaFormats)(HXCODEC codecRef, UINT16 ioDirection,
        FP_MEDIAFORMATSCALLBACK fpCallback, void *userData);

#if defined ADD_HXCODEC_INPUT
HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_Input)(HXCODEC codecRef, HXCODEC_DATA *pData);
#endif


// FP_STREAM_DATA typedef
typedef HX_RESULT (HXEXPORT_PTR FP_STREAM_DATA)(HXSTREAM streamRef,
        HXSTREAM fromstreamRef, HXCODEC_DATA *pData);

HX_RESULT HXEXPORT ENTRYPOINT(PNCodec_StreamOpen)(HXCODEC codecRef,HXSTREAM *streamRef,HXCODEC_INIT
        *params);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_Close)(HXSTREAM streamRef);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_SetDataCallback)(HXSTREAM streamRef,HXSTREAM callbackRef,
        HXMEMORY memoryRef, FP_STREAM_DATA data_callback);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_GetProperty)(HXSTREAM streamRef, ULONG32 prop,
        void *pValue);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_SetProperty)(HXSTREAM streamRef, ULONG32 prop,
        void *pValue);

#ifdef HW_VIDEO_MEMORY_FRONTEND
HX_RESULT HXEXPORT ENTRYPOINT(PNStream_PostProcess)(HXSTREAM streamRef,
        HXCODEC_DATA *pncData,
        ULONG32 uCurrentTime,
        UCHAR *pDestBuffer,
        UINT32 uDestPitch,
        INT32 cidDestColorFormat);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_ReleaseFrame)(HXSTREAM streamRef,
        HXCODEC_DATA *pncData);
#endif

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_GetStreamHeaderSize)(HXSTREAM streamRef,
        ULONG32 *puSize);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_GetStreamHeader)(HXSTREAM streamRef, HX_MOF *pHeader);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_Input)(HXSTREAM streamRef,HXSTREAM   fromStreamRef,
        HXCODEC_DATA *pData);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_SetOutputPacketSize)(HXSTREAM streamRef,ULONG32 prefSize,
        ULONG32 maxSize,ULONG32 *actualSize);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_GetInputBufferSize)(HXSTREAM streamRef,ULONG32 *puSize);

HX_RESULT HXEXPORT ENTRYPOINT(PNStream_OpenSettingsBox)(HXSTREAM streamRef, void * platformData);

PIHXUnknown HXEXPORT ENTRYPOINT(PNStream_GetIPNUnknown)(HXSTREAM streamRef);

#ifdef _MACINTOSH
#pragma export off
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#ifdef __cplusplus
class IHXUnknown
{
public:
    virtual HX_RESULT QueryInterface(HX_IID iid, void** ppvObj) = 0;
    virtual ULONG32 AddRef() = 0;
    virtual ULONG32 Release() = 0;
};

class IHX20Codec : public IHXUnknown
{
public:
    virtual HX_RESULT PNCodec_Close() = 0;
    virtual HX_RESULT PNCodec_GetUIName(char *pnameBuf) = 0;
    virtual HX_RESULT PNCodec_GetVersion(ULONG32 *pVersion) = 0;
    virtual HX_RESULT PNCodec_QueryMediaFormat(HX_MOF * pmofIn,
            HX_MOF * pmofOut, UINT16 ioDirection) = 0;
    virtual HX_RESULT PNCodec_PreferredMediaFormat(HX_MOF * pmofIn,
            HX_MOF * pmofOut, UINT16 ioDirection) = 0;
    virtual HX_RESULT PNCodec_GetMediaFormats(UINT16 ioDirection,
            FP_MEDIAFORMATSCALLBACK fpCallback, void *userData) = 0;
    virtual HX_RESULT PNCodec_StreamOpen(HXSTREAM * pStreamRef,
                                         HXCODEC_INIT * pParams) = 0;
#if defined ADD_HXCODEC_INPUT
    virtual HX_RESULT PNCodec_Input(HXCODEC_DATA *pData) = 0;
#endif
};

class IHX20Stream : public IHXUnknown
{
public:
    virtual HX_RESULT HXStream_Init(HXCODEC_INIT * pInitParams) = 0;
    virtual HX_RESULT PNStream_SetDataCallback(HXSTREAM callbackRef,
            HXMEMORY memoryRef, FP_STREAM_DATA fpDataCallback) = 0;
    virtual HX_RESULT PNStream_GetStreamHeaderSize(ULONG32 * puSize) = 0;
    virtual HX_RESULT PNStream_GetStreamHeader(HX_MOF * pmofStreamHeader) = 0;
    virtual HX_RESULT PNStream_Close() = 0;
    virtual HX_RESULT PNStream_Input(HXSTREAM fromStreamRef,
                                     HXCODEC_DATA * pData) = 0;
    virtual HX_RESULT PNStream_SetOutputPacketSize(ULONG32 PrefSize,
            ULONG32 MaxSize, ULONG32 *pActualSize) = 0;
    virtual HX_RESULT PNStream_GetInputBufferSize(ULONG32 * puSize) = 0;
    virtual HX_RESULT PNStream_SetProperty(ULONG32 prop, void *pValue) = 0;
    virtual HX_RESULT PNStream_GetProperty(ULONG32 prop, void *pValue) = 0;
#ifdef HW_VIDEO_MEMORY_FRONTEND
    virtual HX_RESULT PNStream_PostProcess(
        HXCODEC_DATA *pncData,
        ULONG32 uCurrentTime,
        UCHAR *pDestBuffer,
        UINT32 uDestPitch,
        INT32 cidDestColorFormat) = 0;
    virtual HX_RESULT PNStream_ReleaseFrame(
        HXCODEC_DATA *pncData) = 0;
#endif
    virtual HX_RESULT PNStream_OpenSettingsBox(void * platformData) = 0;
    virtual IHXUnknown * PNStream_GetIPNUnknown() = 0;
};

typedef struct allocatorProps
{
    ULONG32 uBufferSize;
    ULONG32 nNumBuffers;
} HX20ALLOCPROPS;

class IHX20MemoryAllocator : public IHXUnknown
{
public:
    virtual UCHAR * GetPacketBuffer(IHXUnknown ** pPacketBuffer) = 0;
    virtual HX_RESULT SetProperties(HX20ALLOCPROPS* pRequest, HX20ALLOCPROPS* pActual) =  0;
    virtual HX_RESULT GetProperties(HX20ALLOCPROPS* pProps) = 0;
    virtual UINT16 AddRefPacketPtr(UCHAR * memPtr) = 0;
    virtual UINT16 ReleasePacketPtr(UCHAR * memPtr) = 0;
};

#define USER_DATA_REALAUDIO_SURROUND 0x00000001



#endif // __cplusplus

#if defined( _WIN32 ) || defined( _WINDOWS )
#pragma pack()

//  Restore warnings
#pragma warning( default : 4200 )
#endif

// Byte alignment settings for Mac
#if defined( _MACINTOSH )
#pragma options align=reset
#endif


#endif  //  __HXCODEC_H_
