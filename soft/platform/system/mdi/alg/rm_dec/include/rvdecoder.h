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































#ifndef DECODER_H__
#define DECODER_H__


#include "beginhdr.h"


#if !defined(ARM) && !defined(_ARM_)

/*#if defined(INCLUDE_FRU) */
/*#include "hxcodec.h" */
/*#endif */

#if defined(RELIEF_CALLBACK)
#include "rmaengin.h"
#endif

#endif  /*  !defined(ARM) && !defined(_ARM_) */

/* The following undefs are required because windows.h is included */
/* with pmonctrs.h, and RV_Boolean will later be defined to use */
/* TRUE and FALSE in an enum. */
#undef TRUE
#undef FALSE

#include "cdrvdefs.h"
#include "rvmsg.h"
#include "realvdo.h"
#include "bsx.h"
//#include "erc.h" sheen
#include "decdefs.h"
#include "frame.h"
#include "frmlist.h"
//#include "smooth.h" sheen
#include "deblock.h"
//#include "deblock8.h" sheen
//#include "ixfrm.h" sheen
#include "dirb.h"
//#include "ai.h" sheen
//#include "basic.h" voc sheen
//#include "spinterp.h" voc sheen
//#include "decfru.h" sheen
#include "rvfid.h"
#if defined(APK) && (defined(ARM) || defined(_ARM_))
#include "imemcpy.h"
#endif

#ifdef MULTI_THREADED_DECODER
#include "rv89thread.h"
#endif


#if defined(_MSC_VER)
#pragma pack(push, 8)
#endif


#ifdef __cplusplus
extern "C"
{
#endif
/* */
/* CPU Scalability Threshold definitions. */
/* */
/* These constants define the thresholds for certain functionality based */
/* on the CPU Scalability parameter in */
/*             m_options.CPU_Scalability_Setting. */
/* For thresholds less than 50, the action is performed when the scalability */
/* setting is less than or equal to the threshold value. For thresholds greater */
/* than 50, the actions are performed when the scalability setting is greater */
/* than or equal to the threshold value. */
/* */
#define CPU_Scalability_UseSmoothingPostFilter   70

#define CPU_Scalability_NoBFrameDeblocking       45

#define CPU_Scalability_NoBFrameSubpel           40

#define MAX_NUM_RPR_SIZES  8

#define TR_WRAP  256
#define TR_WRAP_RV  8192
/* RealVideo bitstreams as of "G2 with SVT" use a 13-bit TR, */
/* giving millisecond granularity.  RealVideo "G2 Gold", and */
/* RV89Combo use an 8-bit TR. */





struct Global_Decoder_Options
{
    RV_Boolean     SmoothingPostFilter;
    /* The decoder should use the smoothing post-filter. */

    RV_Boolean     IntroduceLatency;
    /* In order to avoid dropping the first set of B frames, */
    /* this flag tells the decoder to wait until the next */
    /* refernce TR before drawing the current reference frame. */

    U32             CPU_Scalability_Setting;
    /* Decoder's CPU scalability setting, interface defined in */
    /* RV_I_CPU_Scalability */

    RV_Boolean     lossConcealment;
    /* Decoder's error concealment switch. */

    /* ====================== */
    /* legacy RPR settings */
    U32             Num_RPR_Sizes;
    /* Number of different RPR sizes used in RealVideo */

    U32             RPR_Sizes[2 * MAX_NUM_RPR_SIZES];
    /* TBD, why not make this an array of RV_Dimensions? */
    /* Array containing the different RPR sizes */
    /* for (i = 0; i < Num_RPR_Sizes; i++) { */
    /*     U32 horizontal_size[i] */
    /*     U32 vertical_size[i] */
    /* } */
    /* ====================== */


    RV_Boolean     FrameRateUpsampling;

#ifdef INTERLACED_CODE
    RV_Boolean      Interlaced;
#endif

    /* The following reserved members are place holders that allow us */
    /* to add new options, without changing the size of this structure. */
    /* They are intended to be used after a release, so that adding */
    /* new options does not affect the size of our persistent data. */

    U32         m_Reserved1;


};

/* This constructor simply gives each member a default value. */
void  Global_Decoder_Options_Init(struct Global_Decoder_Options *t);

RV_Boolean Global_Decoder_Options_isSmoothingRequested(struct Global_Decoder_Options *t);


/* The YUV_Displayer's output color convertors all use the following */
/* function prototype. */
/* Some of the parameters are unused by some of the convertors. */
/* This prototype is declared here because the decoder and YUV_Displayer */
/* share the YUV12ToYUV convertors. */

#define COLOR_CONVERTOR_PROTOTYPE \
    U8*,    /* source Y plane       */ \
    U8*,    /* source U plane       */ \
    U8*,    /* source V plane       */ \
    U32,    /* source Luma width    */ \
    U32,    /* source Luma height   */ \
    U32,    /* source Luma pitch    */ \
    U32,    /* source Chroma pitch  */ \
    U32,    /* 11/12's aspect ratio adjustment counter.  Every n'th line, \
             * where n is this parameter (typically 12 or 24), is discarded. \
             * Use a value greater than the height to suppress ARC. \
             */ \
    U8*,    /* destination buffer   */ \
    U32,    /* DCI destination offset.  Should be 0 for non-RGB formats. */ \
    U32,    /* destination offset to line 0.  Use 0 for non-RGB formats. */ \
    I32,    /* destination pitch    */ \
    I32     /* color convertor identifier.  Only used for RGB formats */

/* */
/* Define the calling convention used by the color convertors */
/* */

typedef void (RV_STDCALL T_POutputColorConvertor)(COLOR_CONVERTOR_PROTOTYPE);
/* The type of a pointer to a color convertor function */


void RV_STDCALL C_YUV12ToYUV(COLOR_CONVERTOR_PROTOTYPE);

/* The YUV12ToYUV functions convert a pitched YUV image to a non-pitched */
/* YUV buffer (i.e., the Y plane immediately followed by the U plane, */
/* immediately followed by the V plane). */
/* Aspect ratio correction is not supported, so the ARC parameter */
/* is ignored.  The destination pitch and convertor identifier are */
/* also ignored.  The DCI offset and offset to line 0 should be set */
/* to 0.  They are used in the asm implementation of this function, */
/* but not the C implementation. */
#ifdef APK
void RV_STDCALL IA_YUV12ToYUV(COLOR_CONVERTOR_PROTOTYPE);
#endif


#define                     MIN_NUM_VIDEO_BUFFERS 5
#define                     MAX_NUM_VIDEO_BUFFERS 30
#define                     EXTRA_VIDEO_BUFFERS   1


struct Decoder
{
    /* from RV_Decoder */
    struct RVDecoder           *m_realvideo;

    RV_FID         m_fid;
    /* This fid represents the image format that this decoder */
    /* was instantiated to decode.  If a decoder supports multiple */
    /* input formats, then m_fid will not necessarily match the */
    /* input format that is eventually agreed to. */

    struct RV_Image_Format  m_input_format;
    struct RV_Image_Format  m_output_format;
    /* m_input_format and m_output_format are set to the negotiated */
    /* formats during the derived class'es Start_Sequence method. */

    /*/// */

    RV_Boolean                 m_bLocalDecoder;
    /* TRUE  - Used locally by encoder.  No B frames processed and */
    /* no output color conversion. */
    /* FALSE - Used by a stand-alone decoder to decompress frames. */
    /* Note that the RV89COMBO codec does not use an encoder's decoder. */
    /* However, I left this member in, because it's simple to */
    /* implement, and provides flexibility down the road. */

    struct Global_Decoder_Options      m_options;


    RV_Boolean                  m_bIsRV8; /* is this RV8 or RV9 */


    RV_Boolean                  m_hideMacroblockInfoFromApp;
    /* If the internal list of macroblock types does not exactly */
    /* match those defined in the "trommsg.h" interface file, */
    /* then we set this to true, which has the effect of */
    /* disabling support for the custom messages */
    /* RV89COMBO_MSG_ID_Get_Macroblock_Info and */
    /* RV89COMBO_MSG_ID_Get_B_Frame_Macroblock_Info. */

    Implementation_ID           m_implementation_id;
    /* Indicates which CPU-specific flavor of algorithm to use. */

    /* The following function pointers are initialized during our */
    /* construction to CPU-specific implementations. */

    //T_ITransform4x4            *m_pInverseTransform; voc sheen
    T_POutputColorConvertor    *m_pYUVPitchChanger;
    T_DirectB_InterpolMB       *m_pDirectB_InterpolMB_Lu;
    T_DirectB_InterpolMB       *m_pDirectB_InterpolMB_Cr;

    /* voc sheen.
    T_PlanarPredict            *m_pPlanarPredict;
    T_Add4x4                   *m_pAdd4x4;
    T_Add4x4_Direct            *m_pAdd4x4_Direct;
    T_Add4x4                   *m_pAdd4x4_PredPitch4;
    T_Copy4x4                  *m_pCopy4x4;
    T_Copy4x4                  *m_pCopy4x4_PredPitch4;
    T_Copy4x4                  *m_pCopy4x4_SrcDstPitch;
    T_Dequant4x4               *m_Dequant4x4;
    T_Intra16x16RestoreDC      *m_Intra16x16RestoreDC;

    const T_InterpFnxTable     *m_InterpFunc;
    const T_InterpFnxTable     *m_InterpFunc_B;
    const T_InterpFnxTable     *m_InterpFunc_RV8;

    #ifdef INTERLACED_CODE
    const T_InterpFnxTable       *m_InterpFuncInterlaced;
    const T_InterpChromaFnxTable *m_InterpFuncChromaInterlaced;
    #endif
    const T_InterpChromaFnxTable *m_InterpFuncChroma;
    const T_InterpChromaFnxTable *m_InterpFuncChroma_RV8;
    */
#if defined(APK) && (defined(ARM) || defined(_ARM_))
    T_Imemcpy                   *m_imemcpy;
#endif


    F64                         m_initialSequenceNumber;
    F64                         m_initialTR;
    /* m_initialSequenceNumber and m_initialTR are the RV_Image */
    /* sequence_number and bitstream TR of the very first image */
    /* we decode after a Start_Sequence.  Typically these will */
    /* be zero, but not necessarily.  For example, when Start_Sequence */
    /* is called after "seeking" to an I frame in the middle of a */
    /* video stream, then these will be non-zero.  These values */
    /* are used, along with each bitstream's TR, to calculate */
    /* each picture's presentation time stamp.  The F64 type is */
    /* used since most calculations performed with these values */
    /* will use floating point arithmetic. */

    U32                         m_uTRWrap;
    /* This is the value at which the TR Wraps around for this  */
    /* particular sequence. */

    RV_Boolean                 m_isLatencyEnabled;
    /* Indicates whether we are delaying the display of frames */
    /* due to the fact that there might be B frames in the video */
    /* sequence.  Initially, latency is disabled.  However, if a */
    /* B frame has been seen or if the appropriate user interface */
    /* is used, then latency is enabled. */

    struct PictureHeader               m_invariantPictureHeader;
    /* m_invariantPictureHeader is where we store the invariant header */
    /* information that comes in with the custom message */
    /* RV_MSG_ID_Set_Picture_Header_Invariants. */
    /* The image dimensions are recorded here during Start_Sequence. */
    /* This information is used in each Decode() invocation, instead */
    /* of looking for it in the bitstream, when decoding REALVIDEO. */
    /* The non-invariant members in m_invariantPictureHeader */
    /* are irrelevant, as they will be parsed from the bitstream. */

    struct CRealVideoBs                   *m_pBitStream;
    struct CRealVideoBs                   *m_pBitStreamCopy;
    /* We allocate this object only once (in our constructor), */
    /* and use it to parse all of our bitstreams. */
    /* TBD, if this is so, why not make it a member object, instead */
    /* of a pointer?  Member objects have the advantage that their */
    /* destructors are automatically called in our destructor. */
    /* TBD, rename the bitstream class to, say, BitStream? */

    //struct LossConcealment             m_lossConcealment; sheen


    /* Declare space for our YUV reference frames and B frames. */
    /* We keep a doubly-linked list of reference frames (i.e., */
    /* non B-frames).  Currently the list contains at most two decoded */
    /* reference frames.  An additional single buffer is maintained for */
    /* decoding B frames, and this buffer is not linked into the reference */
    /* frame list (although its previous() and future() pointers will */
    /* point into the reference frame list). */

    struct DecodedFrameList2            m_freeFrames;
    /* At Start_Sequence, all of our frame buffers are */
    /* available, and are on this list. */

    struct DecodedFrameList            m_referenceFrames;
    /* This is our doubly-linked list of existing decoded */
    /* reference frames.  "Head" points to the oldest frame, and */
    /* "Tail" to the most recent.  When it is time to decode a non-B */
    /* bitstream, and our free list is empty, we bump the oldest */
    /* frame off the head of this list and move it to the tail, */
    /* where it is used to decode the new bitstream. */

    struct DecodedFrame*                m_pBFrame;
    /* pointer to decoded B frame */

    struct DecodedFrame                m_BFrame;
    /* This is the buffer that we decode B frames into. */
    /* in non-hw video memory mode */

#if !defined (HW_VIDEO_MEMORY) && defined (INCLUDE_FRU)
    struct DecodedFrame                m_BFrame2;
    /* This is a second buffer that we decode frames into. */
    /* in non-hw video memory mode.  We need this extra */
    /* buffers when FRU is in use so that we can FRU between */
    /* two B frames */
#endif

#if defined(HW_VIDEO_MEMORY)
    struct DecodedFrame*                m_pLatestDisplayFrame;
    /* pointer to latest frame ready for display */

    RV_Boolean                  m_bHwVideoMemory;

#endif


    struct DecodedFrame*                m_pAllocatedFrames[MAX_NUM_VIDEO_BUFFERS];
    /* array of allocated frames */
    U32                         m_uVideoBuffers;
    /* number of allocated video frames */

    struct DecodedFrame         m_postBuffer;
    /* If post processing is required on a reference frame, or if */
    /* post-processing entails a copy, then this buffer is used */
    /* for the additional copy. */

    struct DecodedFrame               *m_pCurrentFrame;
    /* This points to either m_BFrame, or one of the frames in our */
    /* reference frame list.  It is the frame that we are currently */
    /* parsing in Decode().  It's previous() and future() methods */
    /* can be used to access the frames it is predicted from. */
    /* After a successful call to Decode(), this remains a valid */
    /* pointer, so that the application can use custom messages to */
    /* extract bitstream information for the just-decoded picture. */

    struct DecodedFrame               *m_pDisplayFrame;
    /* This points to either m_BFrame, or one of the frames in our */
    /* reference frame list.  It is the frame that we are going to */
    /* return from our Decode() invocation (or that we have returned */
    /* from our most recent Decode() invocation, if we're outside */
    /* the context of Decode()).  Due to B-frame latency, this may */
    /* not be the frame we are actually decoding.  Due to post */
    /* processing, the YUV data that we eventually display might */
    /* actually reside in one of our post processing buffers. */


//        struct SmoothingFilter             m_smoothingPostFilter; sheen
    /* Provide smoothing post filtering. */

    float                           m_dSmoothingTime;
    /* How long does smoothing take? */

#ifdef INCLUDE_FRU
    struct FrameRateUpsampler          m_FRU;

    struct DecodedFrame*               m_pFRUFrame;
    /* pointer to FRU frame */

    struct DecodedFrame                m_FRUFrame;
    /* This is the buffer that we FRU frames into */
    /* in non-hw video memory mode */

    struct DecodedFrame*               m_pPreviousDisplayFrame;
#endif

    /*#if defined(ADAPTIVE_SMOOTH)  // legacy for RV8 smoothing */
    /* TBD, some of these values carry over from between Decode() */
    /* invocations, and thus must be initialized at Start_Sequence. */
    /* The others should probably be members of the DecodedFrame class. */

    U32                         m_num_active_mbs;
    /* counts number of active macroblocks for use in determining */
    /* the smoothing strength */

    U32                         m_global_adjusted_smoothing_strength;
    /* adjusted smoothing strength based on number of active */
    /* macroblocks ( and distance of key frame from previous  */
    /* key frame) */
    U32                         m_size_adjustment_factor;
    /* for image sizes smaller than a certain size we */
    /* need to filter for smaller QPs, due to slight blockiness */
    /* visible on certain graphics cards, for double-size */
    /* or full screen, and DirectDraw is ON. */
    I32                         m_num_strong;
    I32                         m_num_weak;
    /* used for hysteresis, we can't change too often, to avoid */
    /* flickering */

    I32                         m_num_highQP;
    /* another hysteresis mechanism. We don't want to enable */
    /* smoothing for just a few frames. This will momentarily */
    /* bog down the system, resulting in CPU scalability */
    /* problems. */

    /*#endif // defined(ADAPTIVE_SMOOTH) */



    struct DeblockingFilter            m_inLoopDeblockingFilter;
    /* Filter for in-the-loop (and B frame) de-blocking. */
    /* Filtering is done in-place. */
    /* The memory allocation for this object is not dependent */
    /* on the frame size. */

    //struct DeblockingFilterRV8        m_inLoopDeblockingFilter_RV8;// sheen
    /* legacy deblocking */


    RV_Boolean                 m_bIniIntroduceLatency;
    RV_Boolean                 m_bIniSmoothingPostFilter;
    RV_Boolean                 m_bIniSmoothingStrength;
    RV_Boolean                 m_bIniLossConcealment;


    U8                         *m_pParsedData;
    /* This points to a huge, monolithic buffer that contains data */
    /* derived from parsing the current frame.  It contains motion */
    /* vectors for a single frame, and MB info for one reference */
    /* frame and one B frame, among other things. */
    /* Logically this information belongs in the DecodedFrame class. */
    /* However, algorithmically, we only need to keep around this */
    /* information for the most recent reference frame and B frame */
    /* that we decoded.  Thus, as a space saving optimization, we */
    /* allocate this information in the Decoder class rather than */
    /* in the DecodedFrame class. */

    U32                         m_parsedDataLength;
    struct RV_Dimensions              m_paddedParsedDataSize;
    /* m_pParsedData's allocated size is remembered so that a */
    /* re-allocation is done only if size requirements exceed the */
    /* existing allocation. */
    /* m_paddedParsedDataSize contains the image dimensions, rounded */
    /* up to a multiple of 16, that were used to calculate */
    /* m_parsedDataLength.  If the image dimensions change, while */
    /* the allocated buffer is already large enough, then we don't */
    /* reallocate the buffer, but we still need to adjust the various */
    /* pointers that point into the buffer. */

    I32                        *m_pQuantBuf[2];
    /* Buffer (within m_pParsedData) to store coefficients for */
    /* one macroblock, and perform inverse quantization and inverse */
    /* transform.  Even though the size of this buffer is independent */
    /* of image dimensions, we allocate it in m_pParsedData so that */
    /* we can control its alignment.  It must be 8-byte aligned */
    /* for optimal inverse transform performance. */
    I32                        *m_pQuantBufBase[2];
    /* Base start of buffer containing multiple m_pQuantBuf buffers. */
#define NUM_QUANT_BUFFER_ELEMENTS (64*6 + 4)
    /* Number of quant buffer elements in one MB for which space is allocated. */
    /* Add four additional entries, in case channel bit errors cause */
    /* too many coefficients to be present. */

    Bool8                      *m_pIsSubBlockEmpty[2];
    /* Small buffer, only 16 elements, to indicate which subblocks */
    /* in the current macroblock have no transmitted coefficients. */
    /* CBP only gives 8x8 block granularity. */
    /* We further detect 4x4 subblock coding, since non-coded */
    /* subblocks can be more cheaply reconstructed. */
    /* When m_pIsSubBlockEmpty[S] is true for subblock S, then */
    /* m_pQuantBuf is left uninitialized for S. */
    /* m_pIsSubBlockEmpty is not used for SKIPPED macroblocks. */
    /* Note this is currently only used for luma subblocks. */
    /* We can extend it to chroma if found beneficial. */
    /* This is allocated within m_pParsedData to control its */
    /* alignment.  If 4-byte aligned, then we can safely access */
    /* it as an array of U32's. */
    Bool8                      *m_pIsSubBlockEmptyBase[2];
    /* Base start of buffer containing multiple m_pIsSubBlockEmpty buffers. */
#define NUM_SB_EMPTY_ELEMENTS 24
    /* Number of sub block empty elements in one MB for which space is allocated. */

    DecoderIntraType           *m_pAboveSubBlockIntraTypes[2];
    DecoderIntraType           *m_pMBIntraTypes[2];
    /* The differential coding of intra-coding types uses the intra */
    /* type of the above and left subblocks as predictors. */
    /* We don't need to store the intra type for all subblocks in */
    /* the whole frame, but rather just for those in the current */
    /* macroblock being decoded (m_pMBIntraTypes) and for the bottom */
    /* row of subblocks in the previously decoded row of macroblocks */
    /* (m_pAboveSubblockIntratypes).  Both of these small buffers */
    /* reside in m_pParsedData. */
    DecoderIntraType           *m_pMBIntraTypesBase[2];
    /* Base start of buffer containing multiple m_pMBIntraTypes buffers. */
#define NUM_INTRA_TYPE_ELEMENTS 16
    /* Number of intra type elements in one MB for which space is allocated. */

    struct DecoderPackedMotionVector  *m_pReferenceMotionVectors;
    /* Buffer (within m_pParsedData) used to store motion vectors */
    /* for each 4x4 subblock of a reference frame. */
    /* Note that we may leave the motion vectors unassigned */
    /* when decoding a key frame, so beware. */

    struct DecoderPackedMotionVector  *m_pBMotionVectors;
    /* Buffer (within m_pParsedData) used to store motion vectors */
    /* for each 4x4 subblock of a B frame. */
    /* Note that we may leave the motion vectors unassigned */
    /* when decoding a key frame, so beware. */

    struct DecoderPackedMotionVector  *m_pBidirMotionVectors;
    /* Buffer (within m_pParsedData) used to store backward  */
    /* motion vectors for each 16x16 macroblock of a B frame. */
    /* Note that we may leave the motion vectors unassigned */
    /* when decoding a key frame, so beware. */

    struct DecoderPackedMotionVector  *m_pMotionVectors;
    /* Pointer that is initialized during prepareDecodeBuffers. */
    /* This points to either m_pBmotionVectors or m_pReferenceMotionVectors, */
    /* depending on what type of frame is being decoded. */

    struct DecoderMBInfo              *m_pReferenceMBInfo;
    /* Buffer (within m_pParsedData) used to store information about */
    /* each MB in the bitstream.  This buffer is only used when */
    /* decoding reference frames (i.e., non-B frames). */

    struct DecoderMBInfo              *m_pBFrameMBInfo;
    /* Buffer (within m_pParsedData) used to store information about */
    /* each MB in the bitstream.  This buffer is only used when */
    /* decoding B frames. */

    struct DecoderMBInfo              *m_pMBInfo;
    /* Initialized during prepareDecodeBuffers, this points to */
    /* either m_pReferenceMBInfo or m_pBFrameMBInfo, depending */
    /* on what type of frame is being decoded. */

    U8                          *m_pDirBTmpBuf[2];
    /* Empty buffer of memory used to store motion compensated blocks */
    /* for calculation of direct b mode macroblock prediction. */
    U8                         *m_pPredBuf[2];
    /* Temp buffer for MB predictor pels. Initially defined to be used */
    /* for motion-compensated predictors, may also be useful for intra */
    /* predictors. */

    U32                 m_ulNumThreads;
    U32                 m_ulNumCPUs;

    U32                 m_uNumMissingMBs;

    RV_Boolean          m_bBobInHardware;
    RV_Boolean          m_bForceBob;

#ifdef MULTI_THREADED_DECODER

#define MAX_THREADS 2  // do not change this

    RV_Status           m_last_RV_Status[MAX_THREADS];

    I32                 m_currentSliceNum;
    I32                 m_numSlices;
    U32                 m_uThreadNum;

    RV_Boolean          m_bDataIsLost;
    RV_Boolean          m_bDecoderBetaStream;
    U32                 m_uLastGoodMBNum[128];
    I32                 m_iSliceMBA[128];

    RV_Boolean          m_bRunning[MAX_THREADS];

    RV89Mutex *             m_pSliceTypeMutex;
    RV89Mutex *             m_pCheckOutSlice;
    RV89Mutex *             m_pLastGoodMutex;
#if defined (MULTI_THREADED_DECODER_SUPPORT_BETA_BITSTREAMS)
    RV_Boolean              m_bIsWaiting[MAX_THREADS];
    RV89Mutex *             m_pWaitingMutex;
#endif
    RV89Event *             m_pEventDone[MAX_THREADS];
    RV89Event *             m_pEventDecoderIsReady[MAX_THREADS];
    RV89Thread *            m_pThread[MAX_THREADS];
    RV89Event *             m_pEventSliceDone[MAX_THREADS];
#endif




#if !defined(ARM) && !defined(_ARM_)
#if defined(RELIEF_CALLBACK)
    IRMACallback*               m_pReliefCallback;
    /* pointer to relief callback function, i.e. function to call */
    /* every so often during decode so that the renderer may have */
    /* a chance to blit (flip) a frame. */
#endif  /*  defined(RELIEF_CALLBACK) */
#endif  /*  !defined(ARM) && !defined(_ARM_) */



#if defined(DEC_TIMING_DETAIL)

#define NUM_PERF_COUNTERS 11
    U32 m_uTimeCount_DecodeMB;
    /* MB decode timing counter */
    U32 m_uTimeCount_Deblock;           /* deblocking filter timing counter */
    U32 m_uTimeCount_Postfilter;        /* smoothing post filter timing counter */
    U32 m_uTimeCount_Deblock_PrePost;   /* before and after actual deblock filter */
    U32 m_uTimeCount_Smooth_PrePost;    /* before and after actual smoothing filter */
    U32 m_uTimeCount_decMBheader;       /* MB header parse in MB loop */
    U32 m_uTimeCount_decCoeffs;         /* vld, dequant, inv transform in MB loop */
    U32 m_uTimeCount_recMB;             /* motion comp in MB loop */
    U32 m_uTimeCount_Utility[3];        /* for temporary development use */
#endif

};


void Decoder_Init
(
    struct Decoder *t,
    RV_Boolean  bLocalDecoder,
    Implementation_ID,
    /* Ignored unless bLocalDecoder is true. */
    /* For the encoder's decoder, the encoder */
    /* specifies which implementation to use. */
    RV_FID,
    struct RVDecoder*,
    RV_Status*
);




/* */
/* Declare some interface methods needed for REALVIDEO */
/* */
void            Decoder_SetSegmentProperties(struct Decoder *t,
        const U32 nslices,
        RV_Segment_Info *pSegInfo);

RV_Status                  Decoder_allocateParsedData
(
    struct Decoder *t,
    const struct RV_Dimensions*,
    RV_Boolean exactSizeRequested
);
/* Reallocate m_pParsedData, if necessary, and initialize all the */
/* various pointers that point into it.  This gets called */
/* during Start_Sequence, and at each Decode invocation, based */
/* on the incoming image dimensions. */
/* If exactSizeRequested is false, then any existing */
/* allocated buffer, even if way too big, will be reused without */
/* being deallocated.  Othwerwise, any existing allocated buffer */
/* will be first deallocated if is not exactly the size needed. */

void                        Decoder_deallocateParsedData(struct Decoder *t);



RV_Status       Decoder_ReleaseFrame(struct Decoder *t, struct DecodedFrame *pFrame);


Implementation_ID   Decoder_selectImplementation
(
    struct Decoder *t,
    struct RVDecoder*,
    Bool32  isEncodersDecoder,
    Implementation_ID encodersImpl
);
/* This is called during our constructor to select our */
/* implementation, and to initialize our CPU-specific */
/* function pointers. */
/* encodersImpl is ignored unless isEncodersDecoder is true. */
/* For the encoder's decoder, the encoder specifies which */
/* implementation to use. */

/* This method is used in our constructor, to read */
/* private configuration options from our ini file. */

void Decoder_getBackdoorOptions(struct Decoder *t);

RV_Status
Decoder_Get_CPU_Scalability_Setting(struct Decoder *t, U32 * iValue);


RV_Status
Decoder_Get_Default_CPU_Scalability_Setting(struct Decoder *t, U32 * iValue);

RV_Status
Decoder_Set_CPU_Scalability_Setting(struct Decoder *t, U32 iValue);


RV_Status Decoder_Propose_Format(      struct Decoder *t,
                                       U32 position,
                                       const struct RV_Image_Format*,
                                       struct RV_Image_Format*);

RV_Status      Decoder_CheckInput(struct Decoder *t,
                                  const struct RV_Image_Format*);

RV_Status      Decoder_CheckTransform(struct Decoder *t,
                                      const struct RV_Image_Format*,
                                      const struct RV_Image_Format*);


RV_Status      Decoder_Get_YUV_Destination_Format
(
    struct Decoder *t,
    const struct RV_Image_Format *src_fmt,
    struct RV_Image_Format *dst_fmt
);

RV_Status      Decoder_applyPostFilters
(
    struct Decoder *t,
    struct DecodedFrame                       *pDisplayFrame,
    const struct RV_Image_Format             *srcFormat,
    struct RV_Image                          *dst,
    RV_Decoder_Decompression_Notes    *notes,
    struct DecodedFrame               **pDisplayBuffer
);
/* This method is used by outputAFrame(), to apply any */
/* requested post filters to the image being displayed. */
/* On entry, pDisplayFrame should point to the image to be */
/* displayed (typically m_pDisplayFrame). */
/* 'notes' will be updated to reflect any filtering that is done. */
/* */
/* If the postfilter can write directly to the application's */
/* output buffer, it will.  In this case pDisplayBuffer will be */
/* set to NULL upon return (indicating the output has already */
/* been written), and for RealVideo 'dst.format' will be updated */
/* with the bitstream image dimensions. */
/* */
/* Otherwise, pDisplayBuffer will be assigned upon return to */
/* a possibly filtered buffer (either pDisplayFrame or */
/* &m_postBuffer), which must yet be passed back to the */
/* application. */

RV_Status      Decoder_outputAFrame
(
    struct Decoder *t,
    struct DecodedFrame     *pDisplayFrame,
    const struct RV_Image        *src,
    struct RV_Image        *dst,
    RV_Decoder_Decompression_Notes *notes,
    I32              temporal_offset
);
/* outputAFrame is a worker method used to perform postfiltering */
/* and output color conversion to the application's destination */
/* buffer.  pDisplayFrame must be a non-NULL pointer to the */
/* image to be displayed. */

RV_Status      Decoder_prepareDecodeBuffers
(
    struct Decoder *t,
    const struct RV_Image                       *src,
    struct RV_Image                       *dst,
    const RV_Decoder_Decompression_Flags  flags,
    RV_Decoder_Decompression_Notes *notes,
    I32                             temporal_offset
);
/* prepareDecodeBuffers is called at the beginning of Decode(), */
/* to set up m_pCurrentFrame, m_pDisplayFrame and m_pMBInfo for */
/* the decoding of the incoming frame.  The 'notes' parameter */
/* is also initialized here, based on the frame that is going */
/* to be displayed.  If upon return the 'notes' indicate */
/* RV_DDN_DONT_DRAW, then m_pCurrentFrame may still point to */
/* a frame that should be decoded, though nothing should */
/* be displayed.  If we have to display a frame prior to */
/* decoding, the display is handled here and m_pDisplayframe */
/* will be left NULL. */

#ifdef MULTI_THREADED_DECODER
/* functions for multi-threading */
I32 Decoder_CheckOutSlice(struct Decoder *t);
I32 Decoder_FindLastGoodMBNum(struct Decoder *t,I32 iSliceNum);
I32 Decoder_FindNextGoodMBNum(struct Decoder *t,I32 iSliceNum);
U32 Decoder_InsertAllMissingMBs(struct Decoder *t);


RV_Status   Decoder_parseBitstream_Threaded (
    struct Decoder *t,
    U32 uThreadNum);

void        Decoder_makeMutexes(struct Decoder *t);
void        Decoder_deleteMutexes(struct Decoder *t);
void        Decoder_deleteEvents(struct Decoder *t);

void        Decoder_startThreads(struct Decoder *t);

RV_Status   Decoder_Terminate_Decoder_Threads(struct Decoder *t);

void        Decoder_lockMutex(RV89Mutex *mx);
void        Decoder_unlockMutex(RV89Mutex *mx);
void        Decoder_makeMutex(RV89Mutex **mx);
void        Decoder_deleteMutex(RV89Mutex *mx);


void        Decoder_resetEvent(RV89Event *ev);
void        Decoder_waitEvent(RV89Event *ev);

#endif


RV_Status       Decoder_Get_Performance_Counters(const U32 length, PU32 array);
/* Get performance counters data */



void RV_STDCALL FrameCopy (
    U8 *pInputPlane,          /* Address of input data. */
    U8 *pOuptutPlane,         /* Address of output data. */
    U32 uFrameHeight,         /* Lines to copy. */
    U32 uFrameWidth,          /* Columns to copy. */
    U32 uPitch) GNUSTDCALL;   /* Pitch. */

void RV_STDCALL PitchFrameCopy (
    U8 *pInputPlane,          /* Address of input data. */
    U8 *pOuptutPlane,         /* Address of output data. */
    U32 uFrameHeight,         /* Lines to copy. */
    U32 uFrameWidth,          /* Columns to copy. */
    U32 uSrcPitch,            /* Source plane pitch. */
    U32 uDstPitch) GNUSTDCALL; /* Destination plane pitch. */

void RV_STDCALL FrameMirror (
    U8 *pInputPlane,          /* Address of input data. */
    U8 *pOuptutPlane,         /* Address of output data. */
    U32 uFrameHeight,         /* Lines to copy. */
    U32 uFrameWidth,          /* Columns to copy. */
    U32 uPitch) GNUSTDCALL;   /* Pitch. */

void RV_STDCALL PitchFrameMirror (
    U8 *pInputPlane,          /* Address of input data. */
    U8 *pOuptutPlane,         /* Address of output data. */
    U32 uFrameHeight,         /* Lines to copy. */
    U32 uFrameWidth,          /* Columns to copy. */
    U32 uSrcPitch,            /* Source plane pitch. */
    U32 uDstPitch) GNUSTDCALL; /* Destination plane pitch. */

#ifdef SHEEN_VC_DEBUG

//added by huangx
#define VOC_CFG_PRED_DMA_EADDR_MASK (0xFFFFFFFF)//(0x3FFFFFF)

#else

#define VOC_CFG_PRED_DMA_EADDR_MASK (0x3FFFFFF)

#endif

#define TRUNCATE_LO(val, lim, tmp)      \
    (tmp) = (lim);                      \
    if ((tmp) < (val))                  \
        (val) = (tmp);

#define TRUNCATE_HI(val, lim, tmp)      \
    (tmp) = (lim);                      \
    if ((tmp) > (val))                  \
        (val) = (tmp);



RV_Status Decoder_reconstructLumaIntraMacroblock
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const U32 mbXOffset,
    const U32 mbYOffset,
    const DecoderIntraType *pMBIntraTypes,
    const Bool8 *pIsSubBlockEmpty,
    I32 *pQuantBuf,
    U8 *pPredBuf
);

void Decoder_reconstructChromaIntraBlock_16x16
(
    struct Decoder *t,
    const I32   pTransmitted[8*8],
    const struct DecoderMBInfo*,
    U8   *pChromaPlane,
    const U32   mbXOffset,
    const U32   mbYOffset,
    const U32   init_subblock,
    const DecoderIntraType *pMBIntraTypes,
    const Bool8 *pIsSubBlockEmpty,
    U8 *pPredBuf
);


void Decoder_reconstructChromaIntraBlock
(
    struct Decoder *t,
    const I32   pTransmitted[8*8],
    const struct DecoderMBInfo*,
    U8   *pChromaPlane,
    const U32   mbXOffset,
    const U32   mbYOffset,
    const DecoderIntraType *pMBIntraTypes,
    const Bool8            *pIsSubBlockEmpty,
    U8             *pPredBuf,
    const U32   init_subblock
#ifdef INTERLACED_CODE
    , Bool32    bIsInterlaced
#endif
);
/* Reconstructs one 8x8 chroma block.  Called twice per intra- */
/* macroblock, once for the U plane and once for the V plane. */

/* Decode the luma and chroma coefficients for a macroblock, */
/* placing them in m_pQuantBuf and performing the inverse transform */
/* on them. */
RV_Status Decoder_reconstructLumaIntraMacroblock_16x16
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const U32 mbXOffset,
    const U32 mbYOffset,
    const DecoderIntraType *pMBIntraTypes,
    const Bool8 *pIsSubBlockEmpty,
    I32 *pQuantBuf,
    U8 *pPredBuf
);

#ifdef INTERLACED_CODE
RV_Status
Decoder_reconstructLumaIntraMacroblocki
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMB,
    const U32 mbXOffset,
    const U32 mbYOffset,
    const DecoderIntraType *pMBIntraTypes,
    const Bool8 *pIsSubBlockEmpty,
    I32 *pQuantBuf,
    U8 *pPredBuf
);

RV_Status Decoder_reconstructLumaMacroblocki
(
    struct Decoder *t,
    U32 mbType,
    struct DecoderPackedMotionVector*, /* subblock 0's */
    const U8 *pRefYPlane,
    const U32 mbXOffset,
    const U32 mbYOffset,
    const Bool8 *pIsSubBlockEmpty,
    const I32 *pQuantBuf,
    U8 *pPredBuf
);
#endif

RV_Status Decoder_reconstructLumaMacroblock
(
    struct Decoder *t,
    U32 mbType,
    struct DecoderPackedMotionVector*, /* subblock 0's */
    const U8 *pRefYPlane,
    const U32 mbXOffset,
    const U32 mbYOffset
);
/* reconstructLumaMacroblock is only called for macroblocks */
/* that have transmitted motion vectors (i.e., it will not be */
/* called when mbType is D_MBTYPE_INTRA or D_MBTYPE_SKIPPED). */
RV_Status Decoder_reconstructLumaMacroblock_RV8
(
    struct Decoder *t,
    U32 mbType,
    struct DecoderPackedMotionVector*, /* subblock 0's */
    const U8 *pRefYPlane,
    const U32 mbXOffset,
    const U32 mbYOffset
);

void Decoder_reconDirectInterpLuma
(
    struct Decoder *t,
    U8 *const       pDst,
    const U8 *const pPrev,
    const U8 *const pFutr,
    const U32       uPredPitch,
    const U32       pitch,
    const U32       mbType,
    const Bool32    bSkipInterp,
    const I32       ratio0,
    const I32       ratio1,
    const Bool8 *pIsSubBlockEmpty,
    I32 *pQuantBuf
);
/* interpolates and reconstructs one 16x16 macroblock  */
/* from given previous and future blocks (called by */
/* reconstructLumaMacroblockDirect) */

RV_Status Decoder_reconstructLumaMacroblockDirect
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMVReference,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
);
/* Reconstructs one direct mode luma macroblock */
/* Macroblock type must be MBTYPE_SKIPPED or MBTYPE_DIRECT */



RV_Status Decoder_reconstructLumaMacroblockDirect_RV8
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
);


RV_Status Decoder_reconstructLumaMacroblockBidir
(
    struct Decoder *t,
    const U32               mbType,
    struct DecoderPackedMotionVector *pMVp,
    struct DecoderPackedMotionVector *pMVf,
    const U8               *pRefYPlaneFuture,
    const U8               *pRefYPlanePrevious,
    const U32               mbXOffset,
    const U32               mbYOffset
);


void Decoder_reconDirectInterpChroma
(
    struct Decoder *t,
    U8 *const           pDst,
    const I32 *const    pTransmit,
    const U8 *const     pPrev,
    const U8 *const     pFutr,
    const U32           uPredPitch,
    const U32           pitch,
    const U32           mbType,
    const Bool32    bSkipInterp,
    const I32           ratio0,
    const I32           ratio1,
    const U32           init_subblock,
    const Bool8 *pIsSubBlockEmpty
);

void Decoder_reconstructChromaMacroBlockDirect
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
);
void Decoder_reconstructChromaMacroBlockDirect_RV8
(
    struct Decoder *t,
    const U32               mbType,
    const U32               mbTypeRef,
    const struct DecoderPackedMotionVector *pMV,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset,
    const I32               iRatio0,
    const I32               iRatio1
);

void Decoder_reconstructChromaMacroBlockBidir
(
    struct Decoder *t,
    const struct DecoderPackedMotionVector *pMVp,
    const struct DecoderPackedMotionVector *pMVf,
    const U8               *pRefPlaneFuture,
    const U8               *pRefPlanePrevious,
    const U8               *pRefPlaneFuture1,
    const U8               *pRefPlanePrevious1,
    const U32               mbXOffset,
    const U32               mbYOffset
);


void Decoder_reconstructChromaBlock
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const struct DecoderPackedMotionVector*,
    const U8   *pRef,
    const U8   *pRef1,
    const U32   mbXOffset,
    const U32   mbYOffset
);
/* Performs motion compensation and reconstruction for one */
/* 8x8 chroma block.  Only used when the containing macroblock */
/* has transmitted motion vectors. */
void Decoder_reconstructChromaBlock_RV8
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const struct DecoderPackedMotionVector*,
    const U8   *pRef,
    const U8   *pRef1,
    const U32   mbXOffset,
    const U32   mbYOffset
);

RV_Status Decoder_decodeIntraTypes
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const DecoderIntraType pAboveIntraTypes[4],
    DecoderIntraType *pMBIntraTypes,
    struct CRealVideoBs *pBitstream
#ifdef INTERLACED_CODE
    , Bool32 bIsInterlaced
#endif
);
RV_Status Decoder_decodeIntraTypes_RV8
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const DecoderIntraType pAboveIntraTypes[4],
    DecoderIntraType *pMBIntraTypes,
    struct CRealVideoBs *pBitstream
);
/* Decode the intra-coding type for each subblock in the given */
/* macroblock, storing the results in m_pMBIntraTypes. */
/* pAboveIntraTypes contains the subblock intra-types for */
/* subblocks 12..15 in the macroblock above.  The right column */
/* of m_pMBIntraTypes contains the subblock intra-types for */
/* the macroblock to the left (though this column will get */
/* updated with the current MB's intra types upon return). */

RV_Status Decoder_decodeMacroBlockType(struct Decoder *t,
                                       struct DecoderMBInfo *pMBInfo,
                                       U8 quant_prev,
                                       struct CRealVideoBs *pBitstream,
                                       U32 * ulSkipModesLeft
                                      );

RV_Status Decoder_decodeMacroBlockType_RV8(
    struct Decoder *t,
    U8 *mbtype,
    U8 *QP,
    U8 quant_prev,
    struct CRealVideoBs *pBitstream
);

U32 Decoder_decodeCBP(struct Decoder *t, U32 mbtype, struct CRealVideoBs *pBitstream);
/* Decode and return the coded block pattern. */
/* Return 255 is there is an error in the CBP. */


RV_Status Decoder_computeMotionVectorPredictors
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const struct DecoderPackedMotionVector*,
    const I32 block,    /* block or subblock number */
    I32 *pMVx,
    I32 *pMVy     /* resulting MV predictor */
);
RV_Status Decoder_computeMotionVectorPredictors_RV8
(
    struct Decoder *t,
    const struct DecoderMBInfo*,
    const struct DecoderPackedMotionVector*,
    const I32 block,    /* block or subblock number */
    I32 *pMVx,
    I32 *pMVy     /* resulting MV predictor */
);

RV_Status Decoder_computeMotionVectorPredictorsBFrame
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    I32 *pMVx,    /* resulting MV predictors */
    I32 *pMVy,
    const U32 mbnum
);

RV_Status Decoder_computeMotionVectorPredictorsBFrameBidir
(
    struct Decoder *t,
    const struct DecoderMBInfo *pMBInfo,
    const struct DecoderPackedMotionVector *pMV,
    I32 *pMVx,    /* resulting MV predictors */
    I32 *pMVy,
    const U32 mbnum,
    const U32 block
);


RV_Status Decoder_decodeMotionVectors
(
    struct Decoder *t,
    struct DecoderMBInfo*,
    struct DecoderPackedMotionVector*, /* subblock 0's */
    const U32 mbnum,
    struct CRealVideoBs *pBitstream
);



void Decoder_decodeLumaAndChromaCoefficients_SVLC
(
    struct Decoder *t,
    U32     cbp,
    Bool32  intra,
    Bool32  intra16x16,
    Bool32  inter16x16,
    U32     QP,
    U32     EntropyQP,
    struct CRealVideoBs *pBitstream
);

void Decoder_resetAllEdgeTypes(struct Decoder *t);


void Decoder_setMacroBlockEdgeType(struct Decoder *t, const U32, const U32, const U8);
void Decoder_setSliceEdgeTypes(struct Decoder *t, const U32 mbAddress);

void Decoder_insertMissingMacroBlocks
(
    struct Decoder *t,
    const U32   mba1,
    const U32   mba2,
    U32  *mbRow,
    U32  *mbCol,
    struct DecoderPackedMotionVector **pMV
);
/* insertMissingMacroBlocks compensates for a missing slice, */
/* by filling in the reconstructed frame for macroblocks */
/* numbered mba1, mba1+1, mba1+2, ... mba2-1. */
/* On entry, mbRow and mbCol are the luma plane position of */
/* macroblock mba1.  On exit they are the position */
/* of mba2-1.  Similarly pMV is the MV store for macroblock */
/* mba1 on entry, and mba2-1 on exit. */


void Decoder_setDeblockCoefs(
    struct Decoder *t,
    struct DecoderMBInfo  *pMB,
    struct DecoderPackedMotionVector *pMV,
    U32 uDirectBRefMBType
);
void Decoder_setDeblockCoefs_RV8(
    struct Decoder *t,
    struct DecoderMBInfo  *pMB,
    struct DecoderPackedMotionVector *pMV,
    U32 uDirectBRefMBType
);
/* Set cbpcoef MB info for one MB */


void Decoder_startNextFrame(struct Decoder *t);
/* expands planes and resets edge types */

void Decoder_AdaptiveSmoothingAdjustRV8(struct Decoder *t);
/* AdaptiveSmoothingAdjustment for RV8 */





RV_Status Decoder_parseBitStream(struct Decoder *t, I32 iSliceNum, I32 iThread);
/* Decode m_pBitStream into m_pCurrentFrame */


#ifdef __cplusplus
}
#endif

#if defined(_MSC_VER)
#pragma pack(pop)
#endif



#endif /* DECODER_H__ */
