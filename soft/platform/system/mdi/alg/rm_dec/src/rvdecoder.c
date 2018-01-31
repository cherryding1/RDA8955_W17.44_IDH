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





































#if !defined(ARM) && !defined(_ARM_)
/*#include "pncodec.h" */
#if defined(RELIEF_CALLBACK)
#include "rmaengin.h"
#endif  /* defined(RELIEF_CALLBACK) */
#endif  /* !defined(ARM) && !defined(_ARM_) */
#undef FALSE
#undef TRUE

#include "hxtypes.h"
#if defined(_WIN32)
/* <windows.h> is needed for registry and ini file access */
//#include "hxtypes.h"
#include <windows.h>
#endif /* defined(_WIN32) */



#include "string.h"

#include "beginhdr.h"
#include "rvdecoder.h"
#include "deblock.h"
//#include "deblock8.h" sheen
//#include "rprscal.h"
#include "tables.h"
#include "supervlc.h"
#ifndef SHEEN_VC_DEBUG
#include "sxr_tls.h"
#endif
#include "mdi_alg_common.h"

#ifdef HW_VIDEO_MEMORY
#include "ciddefs.h"  // for CID definitions (include/ciddefs.h) 
#include "hwpfdefs.h" // for HW Postfilter definitions (include/hwpfdefs.h) 
#endif

#if defined(INTERLACED_BOB)
#include "bob.h"
#endif

#ifdef _MACINTOSH
#include <Multiprocessing.h>
#endif

#if defined(DEBUG_FRAMELIST) || defined (DEBUG_NEWMODE)
#include <stdio.h>
#endif

#if defined (MULTI_THREADED_DECODER) && defined (_UNIX_THREADS_SUPPORTED)
#include "unistd.h"
#endif

#if defined(DEBUG_ADAPTIVE_SMOOTHING)
FILE_HDL g_dbg;
#endif


#if defined(DEBUG_FRAMELIST)
FILE_HDL g_framelist;
#endif

#if defined(DEBUG_NEWMODE)
FILE_HDL g_newmode;
#endif

#define WEAK_SMOOTHING 3
#define MEDIUM_SMOOTHING 2
#define STRONG_SMOOTHING 1
#define NO_SMOOTHING 0
#define OFFSET_STRONGER_FILTER 4


#define SMOOTH_LOG_FILENAME     "c:\\smooth.log"
#define FRAMELIST_LOG_FILENAME  "c:\\frmlist.log"
#define NEWMODE_FILENAME        "c:\\newmode.log"




////////////////////////////////////////////////
/// Registry over-rides for custom codec options
////////////////////////////////////////////////
#if defined (_WIN32) && !defined(WINCE)
static const char* g_szKeyName = "Software\\RealNetworks\\RV9";
#endif



static int ReadDWORDFromRegistry(char *szRegString, U32 *uRegValue)
{
#if defined (_WIN32) && !defined(WINCE)
    {

        LONG    rval;
        HANDLE    hKey = 0;
        DWORD   vType, vSize;
        U32     value;
        int retval = 0;

        rval = RegOpenKeyEx(HKEY_LOCAL_MACHINE, g_szKeyName, 0, KEY_QUERY_VALUE, &hKey);

        if (rval == ERROR_SUCCESS)
        {
            vSize = sizeof(value);
            rval = RegQueryValueEx(hKey, szRegString,
            NULL, &vType, (LPBYTE)&value, &vSize);
            if (rval == ERROR_SUCCESS && vType == REG_DWORD
            && vSize == sizeof(value))
            {
                *uRegValue = value;
                retval = 1;
            }
            (void) RegCloseKey(hKey);
            return retval;
        }
        return retval;
    }

#else
    return 0;
#endif
}




RV_Status
Decoder_Load(void)
{
    int res;
    res=decode_4x4_init();
    if(res!=0) return RV_S_OUT_OF_MEMORY;
    return RV_S_OK;
}

/*//////////////////////////////////////////////////////////////////////////// */
/* Get_YUV_Destination_Format */
/*   The format is adjusted for the internal format. */
/*//////////////////////////////////////////////////////////////////////////// */
RV_Status
Decoder_Get_YUV_Destination_Format
(
    struct Decoder *t,
    const struct RV_Image_Format *src_fmt,
    struct RV_Image_Format *dst_fmt
)
{
    U32 padded_width;
    /* Assume CheckInput has already verified src_fmt. */
    dst_fmt->fid        = RV_FID_YUV12;
    dst_fmt->dimensions = src_fmt->dimensions;
    DecodedFrame_Propagate_Defaults(dst_fmt);

    /* Note that this method can be called before input/output formats */
    /* have been negotiated, so don't depend on the layer list having */
    /* been initialized.  I.e, recalculate the pitch here. */

    padded_width  = (dst_fmt->rectangle.width  + 15) & ~15;

    dst_fmt->yuv_info.y_pitch = padded_width + 2 * YUV_Y_PADDING;
    dst_fmt->yuv_info.u_pitch = dst_fmt->yuv_info.y_pitch>>1;//dst_fmt->yuv_info.y_pitch; sheen
    dst_fmt->yuv_info.v_pitch = dst_fmt->yuv_info.y_pitch>>1;//dst_fmt->yuv_info.y_pitch; sheen

    return RV_S_OK;
}

Implementation_ID
Decoder_selectImplementation
(
    struct Decoder *t,
    struct RVDecoder *pRealVideo,
    Bool32  isEncodersDecoder,
    Implementation_ID encodersImpl
)
{
    Implementation_ID impl = C_IMPL;

    /* */
    /* Since this method is called during the Decoder's constructor, */
    /* do not depend on any members of the Decoder having been */
    /* initialized yet.  This is why we pass in the RealVideo pointer */
    /* instead of using m_realvideo. */
    /* */


    //t->m_pCopy4x4_SrcDstPitch = NULL;//C_Copy4x4_SrcDstPitch; voc sheen.

    /* We have no choice for the pure CPK */

    //t->m_pInverseTransform = NULL;//C_ITransform4x4; voc sheen.
    t->m_pYUVPitchChanger = C_YUV12ToYUV;
    t->m_pDirectB_InterpolMB_Lu = NULL;//C_DirectB_InterpolMB_Lu; sheen
    t->m_pDirectB_InterpolMB_Cr = NULL;//C_DirectB_InterpolMB_Cr; sheen
    /*
    t->m_pAdd4x4 = NULL;//C_Add4x4; voc sheen.
    t->m_pAdd4x4_Direct = NULL;//C_Add4x4_Direct; voc sheen.
    t->m_pAdd4x4_PredPitch4 = NULL;//C_Add4x4_PredPitch4; voc sheen.
    t->m_pCopy4x4 = NULL;//C_Copy4x4; voc sheen.
    t->m_pCopy4x4_PredPitch4 = NULL;//C_Copy4x4_PredPitch4; voc sheen
    t->m_pCopy4x4_SrcDstPitch = NULL;//C_Copy4x4_SrcDstPitch; voc sheen.
    t->m_Dequant4x4 = NULL;//C_Dequant4x4; voc sheen.
    t->m_Intra16x16RestoreDC = NULL;//C_Intra16x16RestoreDC; voc sheen.


    t->m_InterpFunc = NULL;//&gc_C_InterpolateTable4; voc sheen.
    t->m_InterpFunc_B = NULL;//&gc_C_InterpolateTable4; voc sheen.
    t->m_InterpFunc_RV8 = NULL;//&gc_C_InterpolateTable; voc sheen.
    t->m_InterpFuncChroma_RV8 = NULL;//&gc_C_InterpolateChromaTable; voc sheen.
    t->m_InterpFuncChroma = NULL;//&gc_C_InterpolateChromaTable4; voc sheen.
    */
    return impl;
}

/*//////////////////////////////////////////////////////////////////////////// */
/* Decoder constructor */
/*//////////////////////////////////////////////////////////////////////////// */
void Decoder_Init
(
    struct Decoder *t,
    RV_Boolean         bLocalDecoder,
    Implementation_ID   encodersImpl,
    RV_FID             fid,
    struct RVDecoder               *pRealVideo,
    RV_Status         *ps
)
{
    U32 i;
    struct DecodedFrame *pFrame;

    t->m_realvideo = pRealVideo;
    t->m_fid = fid;

    t->m_bLocalDecoder= bLocalDecoder;
    t->m_hideMacroblockInfoFromApp = FALSE;
    t->m_pBitStream = 0;
    t->m_initialSequenceNumber=0.0;
    t->m_initialTR=0.0;
    t->m_uTRWrap=0;
    t->m_isLatencyEnabled=FALSE;

    t->m_pCurrentFrame=0;
    t->m_pDisplayFrame=0;

    t->m_pParsedData=0;
    t->m_parsedDataLength=0;
    t->m_pReferenceMotionVectors=0;
    t->m_pBMotionVectors=0;
    t->m_pBidirMotionVectors=0;
    t->m_pMotionVectors=0;
    t->m_pReferenceMBInfo=0;
    t->m_pMBInfo=0;
    t->m_pBFrameMBInfo=0;

    t->m_bIniIntroduceLatency=FALSE;
    t->m_bIniSmoothingPostFilter=FALSE;
    t->m_bIniSmoothingStrength=FALSE;
    t->m_bIniLossConcealment=FALSE;
    t->m_ulNumThreads=0;
    t->m_ulNumCPUs=0;
    t->m_bIsRV8 = 0;
    t->m_bBobInHardware = FALSE;
    t->m_bForceBob = FALSE;

    t->m_num_active_mbs=0;
    t->m_global_adjusted_smoothing_strength=MEDIUM_SMOOTHING;
    t->m_size_adjustment_factor=0;
    t->m_num_strong=10;
    t->m_num_weak=10;
    t->m_num_highQP=20;

#if defined(HW_VIDEO_MEMORY)
    t->m_bHwVideoMemory=FALSE;
    t->m_uVideoBuffers=0;
    t->m_pBFrame=0;
#if defined(INCLUDE_FRU)
    t->m_pFRUFrame=0;
    t->m_pPreviousDisplayFrame=0;
#endif
    t->m_pLatestDisplayFrame=NULL;
#endif
#if defined(DEC_TIMING_DETAIL)
    t->m_uTimeCount_DecodeMB=0;
    t->m_uTimeCount_Deblock=0;
    t->m_uTimeCount_Postfilter=0;
#endif
#if defined(RELIEF_CALLBACK)
    t->m_pReliefCallback=NULL;
#endif

    Global_Decoder_Options_Init(&t->m_options);

    //DeblockingFilter_Init(&t->m_inLoopDeblockingFilter); voc sheen.
    //DeblockingFilterRV8_Init(&t->m_inLoopDeblockingFilter_RV8); voc sheen.

    //SmoothingFilter_Init(&t->m_smoothingPostFilter, FALSE); /* MMX usage will be specified below */ sheen

#ifdef ERROR_CONCEALMENT
    LossConcealment_Init(&t->m_lossConcealment, fid);
#endif

    DecodedFrame_Init(&t->m_BFrame);
    DecodedFrame_Init(&t->m_postBuffer);
#if !defined (HW_VIDEO_MEMORY) && defined (INCLUDE_FRU)
    DecodedFrame_Init(&t->m_BFrame2);
#endif

    DecodedFrameList2_Init(&t->m_freeFrames);
    DecodedFrameList_Init(&t->m_referenceFrames);

    t->m_implementation_id =
        Decoder_selectImplementation(t, pRealVideo, bLocalDecoder, encodersImpl);

#ifdef INCLUDE_FRU
    FrameRateUpsampler_Init(&t->m_FRU);
#endif


#if defined(DEC_TIMING_DETAIL)
    ALLOC_TIME_DETAIL_INST
#endif

#if defined(DEBUG_ADAPTIVE_SMOOTHING)
    g_dbg = fopen (SMOOTH_LOG_FILENAME, "w");
#endif


#if defined(DEBUG_NEWMODE)
    g_newmode = fopen (NEWMODE_FILENAME, "w"); /* Flawfinder: ignore */
#endif

#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
    g_framelist = fopen (FRAMELIST_LOG_FILENAME, "w"); /* Flawfinder: ignore */
#endif


    RVDebug((RV_TL_INIT, "::Decoder(%s) Implementation_ID is %ld",
             (bLocalDecoder ? "local" : "standalone"),
             (U32)t->m_implementation_id));

    //SmoothingFilter_SetMMX(&t->m_smoothingPostFilter, t->m_implementation_id >= MMX_IMPL ); sheen



#if defined(MULTI_THREADED_DECODER)
    t->m_bRunning[0] = FALSE;
    t->m_bRunning[1] = FALSE;

    Decoder_makeMutexes(t);

    t->m_bDecoderBetaStream = FALSE;

    t->m_pEventDone[0] = NULL;
    t->m_pEventDone[1] = NULL;


#if defined MULTI_THREADED_DECODER_SUPPORT_BETA_BITSTREAMS
    t->m_pEventSliceDone[0] = NULL;
    t->m_pEventSliceDone[1] = NULL;
#endif

    t->m_pEventDecoderIsReady[0] = NULL;
    t->m_pEventDecoderIsReady[1] = NULL;

    t->m_pThread[0] = NULL;
    t->m_pThread[1] = NULL;


#if defined(_WIN32)
    {

        SYSTEM_INFO SystemInfo;
        int k, l, m, n;

        /* get Win32 system info */
        GetSystemInfo(&SystemInfo);

        /* calc # of active CPUs */
        l = 0;
        m = SystemInfo.dwActiveProcessorMask;
        n = SystemInfo.dwNumberOfProcessors;
        if (n >= 1 && n < 32 && m)
        {
            for (k = 0; k < n; k ++)
            {
                if (m & (1U << k))
                    l ++;
            }
        }

        /* check if we should enable multi-threaded execution */
        if (l > 1)
        {
            t->m_ulNumCPUs = l;
            t->m_ulNumThreads = 2;
        }

#if 0 /* experimental code for WinExplorer proc affinity mask problem */
        {
            HANDLE hProcessHandle;
            DWORD dwProcessorAffinityMask = 0;
            DWORD dwSystemAffinityMask = 0;
            HXBOOL retval;

            hProcessHandle = GetCurrentProcess();

            retval = GetProcessAffinityMask(hProcessHandle,
                                            &dwProcessorAffinityMask,
                                            &dwSystemAffinityMask);

            /*
            if (retval)
            {
                if (dwProcessorAffinityMask != dwSystemAffinityMask)
                {
                    retval =
                        SetProcessAffinityMask(hProcessHandle,
                            dwSystemAffinityMask);
                }

            } */

        }
#endif

    }
#else  /* !win32 */
    {

        int nc = 1;
#if !defined _WIN32 && !defined _HPUX
        /* Needs modifications :
           Mac? which UNIX flavors support? */
#if (defined(_MACINTOSH) || defined(_MAC_UNIX)) && defined (THREADS_SUPPORTED)
        nc = MPProcessors();
#else
        nc = sysconf(  _SC_NPROCESSORS_CONF);
        /* _SC_NPROCESSORS_CONF works on Linux and Solaris
           Irix should be _SC_NPROC_CONF */
#endif

        /*fprintf(stderr,"UNIX: detected %d CPUs\n", nc); */
        if (nc > 1)
        {
            t->m_ulNumCPUs = nc;
            t->m_ulNumThreads = 2;
#if defined(_SOLARIS)

            /* Solaris needs to be told how many threads
            // or LWPs (Light Weight Processes) to enable.
            // Unless set by thr_setconcurrency(), all
            // threads will run on one CPU.
            //  -- karll@real.com  03/14/2001  */

            int conc;
            conc = thr_getconcurrency();
            thr_setconcurrency(nc);
#endif
        }
#endif
    }
#endif



#if defined(FORCE_THREADS)
    t->m_ulNumThreads = 2;
#endif

#endif /* MULTI_THREADED_DECODER */



    /* Give default values to m_invariantPictureHeader. */
    /* This will be completely filled in later. */
    (void) memset(&t->m_invariantPictureHeader, 0, sizeof(t->m_invariantPictureHeader));
    t->m_invariantPictureHeader.pixel_aspect_ratio = PARC_SQUARE;


    for (; *ps == RV_S_OK; )
        /* Not really a loop; use break instead of goto on error */
    {
        Decoder_getBackdoorOptions(t);

        /* Create our reference frame free list, with 2 elements */



        for (i = 0; i < MAX_NUM_VIDEO_BUFFERS; i++)
            t->m_pAllocatedFrames[i] = NULL;

        t->m_uVideoBuffers = 2;

        for (i = 0; i < 2; i++)
        {
            pFrame = (struct DecodedFrame *)RV_Allocate(sizeof(struct DecodedFrame), 0);

            DecodedFrame_Init(pFrame);
            if (!pFrame)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR ::Decoder -- "
                         "failed to allocated DecodedFrame"));
                *ps = RV_S_OUT_OF_MEMORY;
                break;
            }
            pFrame->m_uFrameNumber = i;
            DecodedFrameList2_append(&t->m_freeFrames, pFrame);
            t->m_pAllocatedFrames[i] = pFrame;
        }

        if (*ps != RV_S_OK)
            break;

        /*t->m_pBitStream = new CRealVideoBs; */
        t->m_pBitStream = (struct CRealVideoBs *)RV_Allocate(sizeof(struct CRealVideoBs), 0);
        CB_Init2(t->m_pBitStream,fid);

        if (!t->m_pBitStream)
            *ps = RV_S_OUT_OF_MEMORY;
        if (*ps != RV_S_OK)
        {
            CB_Delete(t->m_pBitStream);

            RV_Free(t->m_pBitStream);
            t->m_pBitStream = NULL;
            RVDebug((RV_TL_ALWAYS, "ERROR: Decoder -- "
                     "Bit stream object construction problem"));
            break;
        }

        /*        t->m_pBitStreamCopy = new CRealVideoBs; */
        t->m_pBitStreamCopy = (struct CRealVideoBs *)RV_Allocate(sizeof(struct CRealVideoBs), 0);
        CB_Init2(t->m_pBitStreamCopy,fid);

        if (!t->m_pBitStreamCopy)
            *ps = RV_S_OUT_OF_MEMORY;
        if (*ps != RV_S_OK)
        {
            CB_Delete(t->m_pBitStreamCopy);
            RV_Free (t->m_pBitStreamCopy);
            t->m_pBitStreamCopy = NULL;
            RVDebug((RV_TL_ALWAYS, "ERROR: Decoder -- "
                     "Bit stream object construction problem"));
            break;
        }


        break;
    }
}

/*//////////////////////////////////////////////////////////////////////////// */
/* Start_Sequence: This method should be called if there are significant */
/*                 changes in the input format in the compressed bit stream. */
/*                 This method must be called between construction */
/*                 of the Decoder object and the first time a decompress */
/*                 is done. */
/*//////////////////////////////////////////////////////////////////////////// */
RV_Status
Decoder_Start_Sequence
(
    void *decoder,
    const struct RV_Image_Format *src_fmt,
    const struct RV_Image_Format *dst_fmt
)
{
    struct DecodedFrame *pFrame, *pCurr;
//  U32 i;
    RV_Status ps = RV_S_OK;

    struct Decoder *t;

    t = (struct Decoder *)decoder;



    for (;;)
        /* Not really a loop.  Use "break" instead of "goto" on error. */
    {
        U32 i;
        ps = Decoder_CheckTransform(t, src_fmt, dst_fmt);
        if (ps != RV_S_OK)
            break;

        /* tell the smoothing filter whether or not to smooth */
        //SmoothingFilter_SetRV8(&t->m_smoothingPostFilter, t->m_bIsRV8); sheen

        /*        t->m_input_format = *src_fmt; */
        DecodedFrame_CopyFormat(&t->m_input_format, src_fmt);

        switch (src_fmt->fid)
        {
            case RV_FID_REALVIDEO30:
                t->m_realvideo->m_picture_clock_frequency = 1000.0;
                t->m_uTRWrap = TR_WRAP_RV;
                break;
            case RV_FID_RV89COMBO:
                t->m_realvideo->m_picture_clock_frequency = 29.97;
                t->m_uTRWrap = TR_WRAP;
                break;
            default:
                RVAssert(0);
                ps = RV_S_ERROR;
                break;
        }

        if (ps != RV_S_OK)
            break;

        {
            U32 BobInHardware;
            U32 ForceBob;
            if (ReadDWORDFromRegistry("BobInHardware", &BobInHardware))
            {
                if (BobInHardware)
                    t->m_bBobInHardware = TRUE;
            }

            if (ReadDWORDFromRegistry("ForceBob", &ForceBob))
            {
                if (ForceBob)
                    t->m_bForceBob = TRUE;
            }
        }

        CB_SetFID(t->m_pBitStream, src_fmt->fid);


#ifdef ERROR_CONCEALMENT
        LossConcealment_SetFID(&t->m_lossConcealment, src_fmt->fid);
#endif

        t->m_isLatencyEnabled = (t->m_options.IntroduceLatency ? TRUE : FALSE);

        t->m_pDisplayFrame = 0;
        t->m_pCurrentFrame = 0;
        /* Any previously displayed or decoded frame */
        /* will no longer be valid. */

        /* Reallocate all of our buffers to the exact size needed. */
        /* To reduce memory fragmentation, delete all the memory before */
        /* reallocating any.  But, for performance reasons, try not to */
        /* delete anything that's already the correct size.  Remember, */
        /* Start_Sequence might get called during a seek operation, so */
        /* performance is a concern. */

        {
            RV_Boolean isPostBufferProbablyNeeded = Global_Decoder_Options_isSmoothingRequested(&t->m_options);

#if !defined(HW_VIDEO_MEMORY)
            RV_Boolean isBFrameBufferProbablyNeeded;
            /* !! is used instead of a (bool) cast to avoid an */
            /* MSVC compilation warning about performance. */
#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION //sheen
            if(src_fmt->dimensions.width*src_fmt->dimensions.height > LARGE_DIMENSION_SIZE)
                isBFrameBufferProbablyNeeded = FALSE;
            else
                isBFrameBufferProbablyNeeded = !!(t->m_options.IntroduceLatency);
#else
            isBFrameBufferProbablyNeeded = !!(t->m_options.IntroduceLatency);
#endif

#else
            RV_Boolean isBframeBufferProbablyNeeded = FALSE;
#endif

            /*          while (pFrame = m_freeFrames.detachHead()) {}; */
            /*          RVAssert(m_freeFrames.m_uLength == 0); */

            while ((pFrame = DecodedFrameList_detachHead(&t->m_referenceFrames)) != 0)
            {
                DecodedFrame_release(pFrame);
            }
            RVAssert(t->m_referenceFrames.m_uLength == 0);

#if defined (HW_VIDEO_MEMORY)
            if (t->m_bHwVideoMemory)
            {

                RV_Boolean bFree[MAX_NUM_VIDEO_BUFFERS];
#if defined(DEBUG) && defined (DEBUG_FRAMELIST)
                fprintf(g_framelist, "START_SEQUENCE\n");
#endif

                pFrame = t->m_freeFrames.m_pHead;

                for (i= 0; i < t->m_uVideoBuffers; i++)
                    bFree[i] = FALSE;

                for (; pFrame; pFrame = pFrame->m_pFutureFrame2)
                {
                    bFree[pFrame->m_uFrameNumber] = TRUE;
                }

                for (i= 0; i < t->m_uVideoBuffers; i++)
                {
                    if (!bFree[i])
                    {
                        if (!DecodedFrame_getrefcount(t->m_pAllocatedFrames[i]))
                        {
                            /* stuck frame */
                            Decoder_ReleaseFrame(t, t->m_pAllocatedFrames[i]);
#if defined(DEBUG) && defined (DEBUG_FRAMELIST)
                            fprintf(g_framelist, "Releasing *** stuck frame %d\n",
                                    t->m_pAllocatedFrames[i]->m_uFrameNumber);
#endif
                        }
                    }

                }
            }
            else
            {
                while (DecodedFrameList2_detachHead(&t->m_freeFrames)) {};
                RVAssert(t->m_freeFrames.m_uLength == 0);

                for (i = 0; i < t->m_uVideoBuffers; i++)
                {
                    DecodedFrameList2_append(&t->m_freeFrames, t->m_pAllocatedFrames[i]);
                    while (DecodedFrame_getrefcount(t->m_pAllocatedFrames[i]))
                    {
                        DecodedFrame_release(t->m_pAllocatedFrames[i]);
                    }
                }
                RVAssert(t->m_freeFrames.m_uLength ==
                         t->m_uVideoBuffers);

            }
#else /* !HW_VIDEO_MEMORY */
            while (DecodedFrameList2_detachHead(&t->m_freeFrames)) {};
            RVAssert(t->m_freeFrames.m_uLength == 0);

            for (i = 0; i < t->m_uVideoBuffers; i++)
            {
                DecodedFrameList2_append(&t->m_freeFrames, t->m_pAllocatedFrames[i]);
                while (DecodedFrame_getrefcount(t->m_pAllocatedFrames[i]))
                {
                    DecodedFrame_release(t->m_pAllocatedFrames[i]);
                }
            }
            RVAssert(t->m_freeFrames.m_uLength ==
                     t->m_uVideoBuffers);
#endif



#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
            fprintf(g_framelist, "Decoder::Start_Sequence\n");
            fprintf(g_framelist, "free length: %d\n",
                    t->m_freeFrames.m_uLength);
            fprintf(g_framelist, "m_uVideoBuffers: %d\n",
                    t->m_uVideoBuffers);
            fflush(g_framelist);
#endif


            pCurr = t->m_freeFrames.m_pHead;

            for (; pCurr; pCurr = pCurr->m_pFutureFrame2)
                DecodedFrame_conditionalDeallocate(pCurr, &src_fmt->dimensions);

#if !defined(HW_VIDEO_MEMORY)
            if (isBFrameBufferProbablyNeeded)
            {
                DecodedFrame_conditionalDeallocate(&t->m_BFrame,&src_fmt->dimensions);
#ifdef INCLUDE_FRU
                DecodedFrame_conditionalDeallocate(&t->m_BFrame2,&src_fmt->dimensions);
                DecodedFrame_conditionalDeallocate(&t->m_FRUFrame,&src_fmt->dimensions);
#endif
            }
            else
            {
                DecodedFrame_deallocate(&t->m_BFrame);
#ifdef INCLUDE_FRU
                DecodedFrame_deallocate(&t->m_BFrame2);
                DecodedFrame_deallocate(&t->m_FRUFrame);
#endif
            }

#endif
            if (isPostBufferProbablyNeeded)
                DecodedFrame_conditionalDeallocate(&t->m_postBuffer,&src_fmt->dimensions);
            else
                DecodedFrame_deallocate(&t->m_postBuffer);

            /* */
            /* Now reallocate. */
            /* */
            /* This is just an optimization to speed up decoding the */
            /* frist frame.  For safety, all buffers are automatically */
            /* allocated or reallocated as needed during Decode(). */
            /* Note that allocateParsedData may also perform deallocation. */
            /* */

            ps = Decoder_allocateParsedData(t, &src_fmt->dimensions, TRUE);
            if (ps != RV_S_OK)
                break;

            pCurr = t->m_freeFrames.m_pHead;
            for (; pCurr; pCurr = pCurr->m_pFutureFrame2)
            {
                ps = DecodedFrame_allocate(pCurr,&src_fmt->dimensions);
                if (ps != RV_S_OK)
                    break;
            }

            if (ps != RV_S_OK)
                break;

#if !defined(HW_VIDEO_MEMORY)
            if (isBFrameBufferProbablyNeeded)
            {
                ps = DecodedFrame_allocate(&t->m_BFrame,&src_fmt->dimensions);
#ifdef INCLUDE_FRU
                ps = DecodedFrame_allocate(&t->m_BFrame2,&src_fmt->dimensions);
                ps = DecodedFrame_allocate(&t->m_FRUFrame,&src_fmt->dimensions);
#endif
                if (ps != RV_S_OK)
                    break;
            }
#endif

            if (isPostBufferProbablyNeeded)
            {
                ps = DecodedFrame_allocate(&t->m_postBuffer,&src_fmt->dimensions);
                if (ps != RV_S_OK)
                    break;
            }
        }

        t->m_output_format = *dst_fmt;




        break;
    }

    /* Check for multiple processors.  If appropriate,
    // create a second decoder thread, and then start threads
    // on both decoder threads */
#ifdef MULTI_THREADED_DECODER

    /* Windows registry over-ride options*/
#if defined (_WIN32)
    {

        TCHAR Registry_Key_Path[] =
            TEXT("Software\\RealNetworks\\RV9");

        LONG    rval;
        HANDLE    hKey = 0;
        DWORD   vType, vSize;
        U32     value;

        rval = RegOpenKeyEx(HKEY_CURRENT_USER, Registry_Key_Path, 0,
                            KEY_QUERY_VALUE, &hKey);

        if (rval == ERROR_SUCCESS)
        {
            // NoDisableThreading
            vSize = sizeof(value);
            rval = RegQueryValueEx(hKey, "NoDisableThreading",
                                   NULL, &vType, (LPBYTE)&value, &vSize);
            if (rval == ERROR_SUCCESS && vType == REG_DWORD
                    && vSize == sizeof(value))
            {
                if (value > 0)
                    if (t->m_ulNumCPUs > 1)
                        t->m_ulNumThreads = 2;
            }
            // EnableThreading
            vSize = sizeof(value);
            rval = RegQueryValueEx(hKey, "EnableThreading",
                                   NULL, &vType, (LPBYTE)&value, &vSize);
            if (rval == ERROR_SUCCESS && vType == REG_DWORD
                    && vSize == sizeof(value))
            {
                if (value > 0)
                    t->m_ulNumThreads = 2;
            }
            // EnableThreading
            vSize = sizeof(value);
            rval = RegQueryValueEx(hKey, "DisableThreading",
                                   NULL, &vType, (LPBYTE)&value, &vSize);
            if (rval == ERROR_SUCCESS && vType == REG_DWORD
                    && vSize == sizeof(value))
            {
                if (value > 0)
                    t->m_ulNumThreads = 0;
            }


            (void) RegCloseKey(hKey);
        }
    }

#endif

    if (t->m_ulNumThreads > 1 && src_fmt->fid == RV_FID_REALVIDEO30 &&
            (!t->m_bRunning[0] || !t->m_bRunning[1]))
    {
        /* create controller events and start threads */
        Decoder_startThreads(t);
    }

#endif /* MULTI_THREADED_DECODER */



    return ps;
}

/*//////////////////////////////////////////////////////////////////////////// */
/* Change_Output_Format */
/* */
/* The changed format is for the output of the decoder. */
/* If the decoder is comprised of a Decoder and a */
/* YUV_Displayer, set the output format of the YUV_Displayer. */
/* Otherwise, change the output format for the Decoder. */
/*//////////////////////////////////////////////////////////////////////////// */
RV_Status
Decoder_Change_Output_Format(void *decoder,
                             const struct RV_Image_Format *dst_fmt)
{
    RV_Status Status = RV_S_OK;
    struct Decoder *t;

    t = (struct Decoder *)decoder;

    Status = Decoder_CheckTransform(t, &t->m_input_format, dst_fmt);
    if (RV_S_OK != Status)
        return Status;

    DecodedFrame_CopyFormat(&t->m_output_format, dst_fmt);

    return Status;
}

/*//////////////////////////////////////////////////////////////////////////// */
/* Decoder Destructor */
/*//////////////////////////////////////////////////////////////////////////// */
void Decoder_Delete(void *decoder)
{
    struct Decoder *t;

    U32 i;

    t = (struct Decoder *)decoder;

    //DeblockingFilter_Delete(&t->m_inLoopDeblockingFilter); voc sheen.
    //DeblockingFilterRV8_Delete(&t->m_inLoopDeblockingFilter_RV8); voc sheen.
    //SmoothingFilter_Delete(&t->m_smoothingPostFilter); sheen

#ifdef ERROR_CONCEALMENT
    LossConcealment_Delete(&t->m_lossConcealment);
#endif

    DecodedFrame_Delete(&t->m_BFrame);
    DecodedFrame_Delete(&t->m_postBuffer);
#if !defined (HW_VIDEO_MEMORY) && defined (INCLUDE_FRU)
    DecodedFrame_Delete(&t->m_BFrame2);
#endif

#ifdef DEBUG_ADAPTIVE_SMOOTHING
    if (g_dbg)
    {
        fclose(g_dbg);
        g_dbg = 0;
    }
#endif


#ifdef DEBUG_NEWMODE
    if (g_newmode)
    {
        fclose(g_newmode);
        g_newmode = 0;
    }
#endif

    while (DecodedFrameList_detachHead(&t->m_referenceFrames)) {};
    while (DecodedFrameList2_detachHead(&t->m_freeFrames)) {};
    DecodedFrameList2_Delete(&t->m_freeFrames);
    DecodedFrameList_Delete(&t->m_referenceFrames);

    for (i = 0; i < MAX_NUM_VIDEO_BUFFERS; i++)
    {
        if (t->m_pAllocatedFrames[i])
        {
            DecodedFrame_Delete(t->m_pAllocatedFrames[i]);
            RV_Free( t->m_pAllocatedFrames[i]);
            t->m_pAllocatedFrames[i] = 0;
        }
    }



#if defined MULTI_THREADED_DECODER
    Decoder_deleteMutexes(t);

    if (t->m_ulNumThreads > 1)
        Decoder_Terminate_Decoder_Threads(t);

    Decoder_deleteEvents(t);

#endif



#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
    if (g_framelist)
    {
        fflush(g_framelist);
        fclose(g_framelist);
        g_framelist = 0;
    }
#endif


    RV_Free( t->m_pBitStream);
    t->m_pBitStream = NULL;

    RV_Free( t->m_pBitStreamCopy);
    t->m_pBitStreamCopy = NULL;

    Decoder_deallocateParsedData(t);




#if defined(DEC_TIMING_DETAIL)
    FREE_TIME_DETAIL_INST
#endif
}

/*//////////////////////////////////////////////////////////////////////////// */
/*  Custom_Message */
/*      Handle our custom decoder messages */
/*//////////////////////////////////////////////////////////////////////////// */
RV_Status
Decoder_Custom_Message(void *decoder,
                       RV_Custom_Message_ID *p_msgid)
{
    RV_Status ps = RV_S_UNSUPPORTED;
    RV_MSG_Simple *simple = (RV_MSG_Simple*)p_msgid;  /* It just might be! */
    struct Decoder *t;

    t = (struct Decoder *) decoder;

    switch (*p_msgid)
    {
        case RV_MSG_ID_Frame_Rate_Upsampling:
            if (simple->value1 == RV_MSG_GET)
            {
                simple->value2 = t->m_options.FrameRateUpsampling;
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_DISABLE)
            {
                t->m_options.FrameRateUpsampling = FALSE;
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_ENABLE)
            {
                t->m_options.FrameRateUpsampling = TRUE;
                ps = RV_S_OK;
            }
            else
            {
                RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                         " invalid operation %ld for frame rate upsampling",
                         simple->value1));
            }
            break;

        case RV_MSG_ID_Smoothing_Postfilter:
            if (simple->value1 == RV_MSG_GET)
            {
                simple->value2 = t->m_options.SmoothingPostFilter;
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_DISABLE)
            {
                if (!t->m_bIniSmoothingPostFilter)
                {
                    t->m_options.SmoothingPostFilter = FALSE;
                }
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_ENABLE)
            {
                if (!t->m_bIniSmoothingPostFilter)
                {
                    t->m_options.SmoothingPostFilter = TRUE;
                }
                ps = RV_S_OK;
            }
            else
            {
                RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                         " invalid operation %ld for smoothing post filter",
                         simple->value1));
            }
            break;
#if 0//sheen
        case RV_MSG_ID_Smoothing_Strength:
            if (simple->value1 == RV_MSG_GET)
            {
                simple->value2 = SmoothingFilter_GetStrength(&t->m_smoothingPostFilter);
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_SET)
            {
                if (!t->m_bIniSmoothingStrength)
                {
                    ps = SmoothingFilter_SetStrength(&t->m_smoothingPostFilter, simple->value2);
                }
            }
            else
            {
                RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                         " invalid operation %ld for smoothing strength",
                         simple->value1));
            }
            break;
#endif
        case RV_MSG_ID_Latency_Display_Mode:
            if (simple->value1 == RV_MSG_GET)
            {
                simple->value2 = t->m_options.IntroduceLatency;
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_DISABLE)
            {
                if (!t->m_bIniIntroduceLatency)
                {
                    t->m_options.IntroduceLatency = FALSE;
                    t->m_isLatencyEnabled = FALSE;
                }
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_ENABLE)
            {
                if (!t->m_bIniIntroduceLatency)
                {
                    t->m_options.IntroduceLatency = TRUE;
                    t->m_isLatencyEnabled = TRUE;
                }
                ps = RV_S_OK;
            }
            else
            {
                RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                         " invalid operation %ld for introducing latency",
                         simple->value1));
            }
            break;

#ifdef ERROR_CONCEALMENT
        case RV_MSG_ID_Error_Concealment:
            if (simple->value1 == RV_MSG_GET)
            {
                simple->value2 = t->m_options.lossConcealment;
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_DISABLE)
            {
                if (!t->m_bIniLossConcealment)
                {
                    t->m_options.lossConcealment = FALSE;
                }
                ps = RV_S_OK;
            }
            else if (simple->value1 == RV_MSG_ENABLE)
            {
                if (!t->m_bIniLossConcealment)
                {
                    t->m_options.lossConcealment = TRUE;
                }
                ps = RV_S_OK;
            }
            else
            {
                RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                         " invalid operation %ld for error concealment",
                         simple->value1));
            }
            break;
#endif

        case RV_MSG_ID_Set_Decode_Segment_Info:
        {
            RV_Segment_Info_MSG *si = (RV_Segment_Info_MSG *) p_msgid;
            if (!si->segment_info)
            {
                RVDebug((RV_TL_ALWAYS,
                         "ERROR  Decoder::Custom_Message(%ld)"
                         " -- NULL segment_info", *p_msgid));
                ps = RV_S_NULL_PARAM;
                break;
            }
            Decoder_SetSegmentProperties(t, si->number_of_segments + 1, si->segment_info);
            ps = RV_S_OK;
        }
        break;

        case RV_MSG_ID_Set_Picture_Header_Invariants:
        {
            t->m_invariantPictureHeader.SS = (RV_Boolean)(
                                                 (simple->value1 & RV_MSG_SPHI_Slice_Structured_Mode) != 0);

            ps = RV_S_OK;
        }
        break;

        case RV_MSG_ID_Set_RVDecoder_RPR_Sizes:
            /* legacy RPR settings */
        {
            U32 i;
            RV_MSG_RVDecoder_RPR_Sizes *rrs = (RV_MSG_RVDecoder_RPR_Sizes *) p_msgid;

            if (rrs->num_sizes < 0 || rrs->num_sizes > MAX_NUM_RPR_SIZES)
            {
                RVDebug((RV_TL_ALWAYS,
                         "ERROR  Decoder::Custom_Message(%ld)"
                         " -- Number of RPR sizes out of range", *p_msgid));

                ps = RV_S_ERROR;
                break;
            }
            t->m_options.Num_RPR_Sizes = rrs->num_sizes;

            for (i = 0; i < 2 * rrs->num_sizes; i++)
            {
                t->m_options.RPR_Sizes[i] = rrs->sizes[i];
            }

            ps = RV_S_OK;

        }
        break;


        case RV_MSG_ID_Get_Decoder_Timings:
        {
            RV_MSG_Get_Decoder_Timings *gdt = (RV_MSG_Get_Decoder_Timings *) p_msgid;
            gdt->smoothing_time = t->m_dSmoothingTime;
#ifdef INCLUDE_FRU
            gdt->fru_time = t->m_FRU.m_dFruTime;
#else
            gdt->fru_time = 0;
#endif
            ps = RV_S_OK;

        }
        break;

        case RV89COMBO_MSG_ID_Decoder_CPU_Scalability:
            if (simple->value1 == RV_MSG_GET)
            {
                U32 value;
                ps = Decoder_Get_CPU_Scalability_Setting(t, &value);
                simple->value2 = value;
            }
            else if (simple->value1 == RV_MSG_SET)
            {
                U32 value = (U32)(simple->value2);
                ps = Decoder_Set_CPU_Scalability_Setting(t, value);
            }
            else
            {
                RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                         " invalid operation %ld for decoder CPU scalability",
                         simple->value1));
            }
            break;
#if defined HW_VIDEO_MEMORY

        case RV_MSG_ID_Hw_Video_Memory:
        {
            RV_MSG_Simple *simple = (RV_MSG_Simple*)p_msgid;

            switch (simple->value1)
            {
                case RV_MSG_SET:

#if defined (DEBUG_NEWMODE)
                    fprintf(g_newmode, "Trying to enable new mode\n");
#endif
#if 0  /* no reason no to, as long as compiled with HW_VIDEO_MEMORY */
                    if (t->m_implementation_id < MMX_IMPL)
                    {
                        ps = RV_S_UNSUPPORTED;
                        break;
                    }
#endif

                    simple->value2 += 0;/*EXTRA_VIDEO_BUFFERS;  // provide extra buffers */

                    if (simple->value2 >= MIN_NUM_VIDEO_BUFFERS &&
                            simple->value2 < MAX_NUM_VIDEO_BUFFERS)
                    {
                        DecodedFrame_deallocate(&t->m_postBuffer); /* don't need this here */

                        if ((U32)simple->value2 > t->m_uVideoBuffers)
                        {
                            U32 i, prevNumVideoBuffers;
                            struct DecodedFrame *pFrame;

                            while (DecodedFrameList2_detachHead(&t->m_freeFrames)) {};
                            RVAssert(t->m_freeFrames.m_uLength == 0);

                            while (DecodedFrameList_detachHead(&t->m_referenceFrames)) {};
                            RVAssert(t->m_referenceFrames.m_uLength == 0);

                            for (i = 0; i < t->m_uVideoBuffers; i++)
                            {
                                DecodedFrameList2_append(&t->m_freeFrames,t->m_pAllocatedFrames[i]);
                            }

                            RVAssert(t->m_freeFrames.m_uLength ==
                                     t->m_uVideoBuffers);

                            /* now make the switch */
                            prevNumVideoBuffers = t->m_uVideoBuffers;
                            t->m_uVideoBuffers = simple->value2;

                            /* and allocate more frame buffers */
                            for (i = prevNumVideoBuffers; i < t->m_uVideoBuffers; i++)
                            {
                                pFrame = (struct DecodedFrame *)RV_Allocate(sizeof(struct DecodedFrame), 0);
                                DecodedFrame_Init(pFrame);
                                if (!pFrame)
                                {
                                    RVDebug((RV_TL_ALWAYS, "ERROR ::Decoder -- "
                                             "failed to allocate DecodedFrame"));
                                    ps = RV_S_OUT_OF_MEMORY;
                                    break;
                                }
                                pFrame->m_uFrameNumber = i;
                                DecodedFrameList2_append(&t->m_freeFrames,pFrame);
                                t->m_pAllocatedFrames[i] = pFrame;

                            }

                            RVAssert(t->m_freeFrames.m_uLength ==
                                     t->m_uVideoBuffers);

                            pFrame = t->m_freeFrames.m_pHead;
                            for (; pFrame; pFrame = pFrame->m_pFutureFrame2)
                            {
                                while (DecodedFrame_getrefcount(pFrame))
                                {
                                    DecodedFrame_release(pFrame);
                                }
                                ps = DecodedFrame_allocate(pFrame,&t->m_input_format.dimensions);
                                if (ps != RV_S_OK)
                                    break;
                            }

                            if (ps != RV_S_OK)
                                break;


                        }
                        t->m_bHwVideoMemory = TRUE;
                        ps = RV_S_OK;

#if defined (DEBUG_NEWMODE)
                        fprintf(g_newmode, "Enabled new mode\n");
#endif


                    }
                    else
                    {
                        ps = RV_S_ERROR;
                    }

                    break;
                case RV_MSG_GET:
                    simple->value1 = t->m_bHwVideoMemory;

                    if (t->m_bHwVideoMemory)
                        simple->value2 = t->m_uVideoBuffers;
                    else
                        simple->value2 = 0;

                    ps = RV_S_OK;
                    break;

                default:
                    RVDebug((RV_TL_ALWAYS,"ERROR Decoder::Custom_Message"
                             " invalid operation %ld for Hw_Video_Memory", simple->value1));
                    ps = RV_S_UNSUPPORTED;
                    break;
            }
        }
        break;
#if 0// sheen
        case RV_MSG_ID_PostfilterFrame:

        {
            RV_MSG_PostfilterFrame *pff;

            ps = RV_S_UNSUPPORTED;

            pff = (RV_MSG_PostfilterFrame *)p_msgid;
            if (pff)
            {
                if (pff->cid_dest_color_format == CID_DVPF)
                {
                    struct DecodedFrame *pFrame;
                    DVPFHeader *dvpfheader =
                        (DVPFHeader *)pff->dest_buffer;
                    pFrame = (struct DecodedFrame *)pff->data;

                    SmoothingFilter_GetStrengthValues(&t->m_smoothingPostFilter,
                                                      pFrame->m_adjusted_smoothing_strength +
                                                      t->m_size_adjustment_factor,
                                                      pFrame->m_pMBInfo[0].QP,
                                                      &(dvpfheader->lRV8Strengths[0]));

                    ps = RV_S_OK;
                }
            }

            if (t->m_bIsRV8) /* postfilter is turned off in RealVideo 9. */
            {
                RV_MSG_PostfilterFrame *pff =
                    (RV_MSG_PostfilterFrame *)p_msgid;



                if (pff)
                {
                    PU8 pDstYPlane, pDstUPlane, pDstVPlane;
                    U32 dstYWidth, dstYHeight;
                    U32 dstYPitch, dstUVPitch;

                    struct DecodedFrame *pFrame;
                    pFrame = (struct DecodedFrame *)pff->data;

                    RVAssert(pFrame);

                    pDstYPlane = pff->dest_buffer;
                    /*pDstVPlane = pff->dest_buffer + dstYWidth*dstYHeight; */
                    /*pDstUPlane = pff->dest_buffer + dstYWidth*dstYHeight + (dstYWidth*dstYHeight>>2); */


                    pDstYPlane = pff->dest_buffer;
                    dstYPitch  = pff->dest_pitch;


                    /*              if (dstYPitch == 488) */
                    /*                  dstYPitch = 480; */

                    dstUVPitch = pff->dest_pitch>>1;
                    dstYWidth = pFrame->m_lumaSize.width;
                    /* padded width */
                    dstYHeight = pFrame->m_dstHeight;
                    /* cropped height */

                    if (dstYPitch <  dstYWidth)
                        dstYWidth = dstYPitch;



                    if (t->m_implementation_id >= MMX_IMPL)
                    {
                        switch (pff->cid_dest_color_format)
                        {
                            case CID_YV12:
                                pDstVPlane = pDstYPlane +
                                             dstYHeight*
                                             pff->dest_pitch;
                                pDstUPlane = pDstVPlane +
                                             (dstYHeight>>1) *
                                             dstUVPitch;
#if defined (DEBUG_NEWMODE)
                                fprintf(g_newmode, "postfilter new mode: YV12\n");
#endif
                                ps = RV_S_OK;
                                break;
                            case CID_I420: /* YV12 with U and V swapped */
                                pDstUPlane = pDstYPlane +
                                             dstYHeight*
                                             pff->dest_pitch;
                                pDstVPlane = pDstUPlane +
                                             (dstYHeight>>1) *
                                             dstUVPitch;
                                ps = RV_S_OK;
                                break;
#if defined (DEBUG_NEWMODE)
                                fprintf(g_newmode, "postfilter new mode: I420\n");
#endif
                            case CID_YUY2:
                            case CID_UYVY:
                                ps = RV_S_OK;
#if defined (DEBUG_NEWMODE)
                                fprintf(g_newmode, "postfilter new mode: YUY2/UYVY\n");
#endif
                                break;

                            case CID_DVPF:
                                ps = RV_S_OK;
                                break;

                            default:
                                RVDebug((RV_TL_ALWAYS," Custom_Message -- "
                                         "RV_MSG_ID_Postfilterframe: "
                                         "unsupported color format"));
                                ps = RV_S_UNSUPPORTED;
                                break;
                        }
                    }
                    else
                    {
                        RVDebug((RV_TL_ALWAYS," Custom_Message -- "
                                 "RV_MSG_ID_Postfilterframe: "
                                 "unsupported color format"));
                        ps = RV_S_UNSUPPORTED;
                        break;
                    }

                    if (ps == RV_S_OK)
                    {
                        SmoothingFilter_SetStrength(&t->m_smoothingPostFilter,
                                                    pFrame->m_adjusted_smoothing_strength + t->m_size_adjustment_factor);


                        /* TBD use new smoothing filters (directly to various YUV formats) */
                        /* Applying RV89COMBO combined smoothing-deblocking post filter */
                        ps = SmoothingFilter_FilterYUVFrame(
                                 &t->m_smoothingPostFilter,
                                 pFrame->m_pYPlane,
                                 pFrame->m_pVPlane,
                                 pFrame->m_pUPlane,
                                 pFrame->m_pitch,
                                 pFrame->m_pitch>>1,//pFrame->m_pitch, sheen
                                 pDstYPlane,
                                 pDstVPlane,
                                 pDstUPlane,
                                 dstYPitch,
                                 dstUVPitch,
                                 dstYWidth,
                                 dstYHeight,
                                 pFrame->m_pMBInfo,
                                 (pff->cid_dest_color_format==CID_I420? FID_I420 : FID_NOTSUPPORTED));



                        t->m_dSmoothingTime = (float)SmoothingFilter_GetTiming(&t->m_smoothingPostFilter);

#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
                        if (pFrame)
                        {
                            fprintf(g_framelist, "Sizp (%d): %d x %d\n",
                                    pFrame->m_uFrameNumber, dstYWidth, dstYHeight);
                            fprintf(g_framelist, "postfilter: %d (timestamp %d)   %f\n",
                                    pFrame->m_uFrameNumber, pFrame->m_timeStamp,
                                    t->m_dSmoothingTime);
                        }
                        else
                        {
                            fprintf(g_framelist, "postfilter: null pointer\n");
                        }
#endif

                    }

                }
            }
#if defined(INTERLACED_BOB)
            else
            {
                struct DecodedFrame *pFrame;
                pFrame = (struct DecodedFrame *)pff->data;
                RVAssert(pFrame);

                if (pFrame->m_pictureHeader.Interlaced || t->m_bForceBob)
                {
                    PU8 pDstYPlane, pDstUPlane, pDstVPlane;
                    U32 dstYWidth, dstYHeight;
                    U32 dstYPitch, dstUVPitch;

                    pDstYPlane = pff->dest_buffer;

                    pDstYPlane = pff->dest_buffer;
                    dstYPitch  = pff->dest_pitch;

                    dstUVPitch = pff->dest_pitch>>1;
                    dstYWidth = pFrame->m_lumaSize.width;
                    /* padded width */
                    dstYHeight = pFrame->m_dstHeight;
                    /* cropped height */

                    if (t->m_bBobInHardware)
                        dstYHeight /= 2;



                    if (dstYPitch <  dstYWidth)
                        dstYWidth = dstYPitch;

                    switch (pff->cid_dest_color_format)
                    {
                        case CID_YV12:
                            pDstVPlane = pDstYPlane +
                                         dstYHeight*
                                         pff->dest_pitch;
                            pDstUPlane = pDstVPlane +
                                         (dstYHeight>>1) *
                                         dstUVPitch;
                            ps = RV_S_OK;
                            break;
                        case CID_I420: /* YV12 with U and V swapped */
                            pDstUPlane = pDstYPlane +
                                         dstYHeight*
                                         pff->dest_pitch;
                            pDstVPlane = pDstUPlane +
                                         (dstYHeight>>1) *
                                         dstUVPitch;
                            ps = RV_S_OK;
                            break;
                        case CID_YUY2:
                        case CID_UYVY:
                            ps = RV_S_OK;
                            break;

                        default:
                            RVDebug((RV_TL_ALWAYS," Custom_Message -- "
                                     "RV_MSG_ID_Postfilterframe: "
                                     "unsupported color format"));
                            ps = RV_S_UNSUPPORTED;
                            break;
                    }

                    if(ps == RV_S_OK)
                    {
                        if (t->m_bBobInHardware)
                        {
                            /* Frame to field */
                            frame_to_field(
                                pFrame->m_pYPlane,
                                pFrame->m_pUPlane,
                                pFrame->m_pVPlane,
                                pFrame->m_pitch,
                                pDstYPlane,
                                pDstUPlane,
                                pDstVPlane,
                                dstYWidth,
                                dstYHeight,
                                dstYPitch,
                                dstUVPitch,
                                pff->flags,
                                pFrame->m_pictureHeader.InterlacedTopFieldFirst,
                                pff->cid_dest_color_format,
                                pFrame->m_pictureHeader.PQUANT,
                                1);
                        }
                        else
                        {
                            frame_bob(
                                pFrame->m_pYPlane,
                                pFrame->m_pUPlane,
                                pFrame->m_pVPlane,
                                pFrame->m_pitch,
                                pDstYPlane,
                                pDstUPlane,
                                pDstVPlane,
                                dstYWidth,
                                dstYHeight,
                                dstYPitch,
                                dstUVPitch,
                                pff->flags,
                                pFrame->m_pictureHeader.InterlacedTopFieldFirst,
                                pff->cid_dest_color_format,
                                pFrame->m_pictureHeader.PQUANT,
                                1);
                        }
                    }// NOT OK
                }
            }
#endif
        }
        break;

#endif

        case RV_MSG_ID_ReleaseFrame:
        {
            RV_MSG_ReleaseFrame *rf =
                (RV_MSG_ReleaseFrame *)p_msgid;

            Decoder_ReleaseFrame(t, (struct DecodedFrame *)rf->data);
            ps = RV_S_OK;

        }
        break;

#if 0  /* when the front-end retrieves a frame like this,
        it expects to receive a pointer to frame class. The ANSI
        C decoder does not contain a compatible class */
        case RV_MSG_ID_RetrieveFrame:
        {
            RV_MSG_RetrieveFrame *rf =
                (RV_MSG_RetrieveFrame *)p_msgid;
            if (rf)
            {
                if (t->m_pLatestDisplayFrame)
                {
                    rf->frame = (void *)t->m_pLatestDisplayFrame;
                    ps = RV_S_OK;
                }
                else
                {
                    ps = RV_S_ERROR;
                }
            }
        }
        break;
#else

        case RV_MSG_ID_RetrieveFrameData:
        {
            RV_MSG_RetrieveFrameData *rf =
                (RV_MSG_RetrieveFrameData *)p_msgid;
            if (rf)
            {
                if (t->m_pLatestDisplayFrame)
                {
                    struct DecodedFrame *d = t->m_pLatestDisplayFrame;
                    rf->m_data  = (void *)d;
                    rf->m_pYPlane = d->m_pYPlane;
                    rf->m_pUPlane = d->m_pUPlane;
                    rf->m_pVPlane = d->m_pVPlane;
                    rf->m_pitch = d->m_pitch;
                    rf->m_width = d->m_dstWidth;
                    rf->m_height = d->m_dstHeight;
#ifdef INTERLACED_CODE
                    rf->m_interlaced = t->m_pLatestDisplayFrame->m_pictureHeader.Interlaced;
#else
                    rf->m_interlaced = FALSE;
#endif

                    ps = RV_S_OK;
                }
                else
                {
                    ps = RV_S_ERROR;
                }
            }
        }
        break;
#endif


#endif

#if defined(RELIEF_CALLBACK)
        case RV_MSG_ID_SetReliefCallback:
#if !defined(ARM) && !defined(_ARM_)
        {
            RV_MSG_SetReliefCallback *src =
                (RV_MSG_SetReliefCallback *)p_msgid;

            if (src)
            {
                PN_CMTM * cmtm = (PN_CMTM *)src->cmtm;
                t->m_pReliefCallback = cmtm->pCallback;
                ps = RV_S_OK;
            }
        }
#else
        ps = RV_S_UNSUPPORTED;
#endif  /*  !defined(ARM) && !defined(_ARM_) */
        break;
#endif  /* defined(RELIEF_CALLBACK) */

#ifdef MULTI_THREADED_DECODER
        case RV_MSG_ID_Decoder_Multi_Threading:
        {
            switch (simple->value1)
            {
                case RV_MSG_DISABLE:
                {
                    t->m_ulNumThreads = 0;
                    ps = RV_S_OK;
                }
                break;

                case RV_MSG_SET:
                {
                    if (simple->value2 == 0 || simple->value2 == 2)
                    {
                        t->m_ulNumThreads = simple->value2;
                        ps = RV_S_OK;
                    }
                }
                break;
            }
        }
        break;

        case RV_MSG_ID_Decoder_Beta_Stream:
        {
            switch (simple->value1)
            {
                case RV_MSG_ENABLE:
                {
                    t->m_bDecoderBetaStream = TRUE;
                    ps = RV_S_OK;
                }
                break;
                case RV_MSG_DISABLE:
                {
                    t->m_bDecoderBetaStream = FALSE;
                    ps = RV_S_OK;
                }
                break;
            }
        }
        break;

#endif




        case RV_MSG_ID_RealVideo8:
        {
            switch (simple->value1)
            {
                case RV_MSG_ENABLE:
                {
                    t->m_bIsRV8 = TRUE;
                    ps = RV_S_OK;
                }
                break;
                case RV_MSG_DISABLE:
                {
                    t->m_bIsRV8 = FALSE;
                    ps = RV_S_OK;
                }
                break;

                case RV_MSG_GET:
                {
                    simple->value2 = t->m_bIsRV8;
                    ps = RV_S_OK;
                }
                break;
            }
        }
        break;


        default:
            /* The custom message is not recognized by the Decoder. */
            /* Check if any of the included objects recognize it. */

            break;
    }

    if (ps != RV_S_OK && ps != RV_S_UNSUPPORTED && ps != RV_S_DONT_DRAW)
    {
        RVDebug((RV_TL_ALWAYS,"ERROR  Decoder::Custom_Message(%ld)"
                 " returning %x", *p_msgid, ps));
    }
    return ps;
}


#if defined(HW_VIDEO_MEMORY)
/*///////////////////////////////////////////////////////////////////////// */
/*  ReleaseFrame */
/*///////////////////////////////////////////////////////////////////////// */
RV_Status
Decoder_ReleaseFrame(struct Decoder *t, struct DecodedFrame *pFrame)
{
    if (pFrame)
    {

        struct DecodedFrame *pCurr;
#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
        fprintf(g_framelist, "Releasing # %d, timestamp %d\n",
                pFrame->m_uFrameNumber, pFrame->m_timeStamp);
#endif

        DecodedFrame_release(pFrame);

        if (DecodedFrame_getrefcount(pFrame))
        {
#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
            fprintf(g_framelist, "Still in use # %d, refcount %d\n",
                    pFrame->m_uFrameNumber, DecodedFrame_getrefcount(pFrame));
#endif
            return RV_S_OK;
        }

        pCurr = t->m_freeFrames.m_pHead;

        while (pCurr)
        {
            if (pCurr == pFrame)
            {
                /* frame already free */
#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
                fprintf(g_framelist, "releasing already free "
                        "frame : %d\n", pFrame->m_uFrameNumber);
#endif
                return RV_S_OK;
            }
            pCurr = pCurr->m_pFutureFrame2;

        };


        DecodedFrameList2_append(&t->m_freeFrames,pFrame);
        return RV_S_OK;
    }

    return RV_S_ERROR;

}
#else
RV_Status
Decoder_ReleaseFrame(struct Decoder *t, struct DecodedFrame *pFrame)
{
    return RV_S_UNSUPPORTED;
}
#endif

/*///////////////////////////////////////////////////////////////////////// */
/*  SetSegmentProperties */
/*      This function is used to initialize segment/slice properties in */
/*      the decode bitstream object.  This needs to be called before */
/*      calling Decode or LocalDecode on an RV bitstream. */
/*///////////////////////////////////////////////////////////////////////// */
void
Decoder_SetSegmentProperties
(
    struct Decoder *t,
    const U32 nslices,
    RV_Segment_Info *pSegInfo
)
{
    CB_PutSliceOffsets(t->m_pBitStream, nslices, (Bitstream_Segment_Info *) pSegInfo);
}

#if defined TEST_BITSTREAM_ERROR_RESILIENT
static int get_random_number(int max)
{
    int r;
    double d;

    r = rand();
    d = r / (double)(RAND_MAX);
    r = (int)(d * (max+1));

    if (r > max)
        r = max;
    return r;
}
#endif

I8 rm_video_key_seek_event;//for clear the first B frame reference.sheen
extern INT8 RV_SKIP_B_FLAG;
extern volatile UINT32 rv_Timestamp_dis;

RV_Status
Decoder_prepareDecodeBuffers
(
    struct Decoder *t,
    const struct RV_Image *src,
    struct RV_Image *dst,
    const RV_Decoder_Decompression_Flags  flags,
    RV_Decoder_Decompression_Notes *notes,
    I32                             temporal_offset
)
{
    RV_Status ps = RV_S_OK;
    struct PictureHeader pictureHeader;
    struct DecodedFrame *pPreviousTail;
    RV_Boolean wasFrameDisplayedBeforeDecode = FALSE;
    RV_Boolean between;
    struct RV_Image_Format    incomingSrcFormat;
    /* sheen
    struct DecodedFrame    *pNewBuffer;
    struct DecodedFrame           *pOldBuffer;

    T_RPR_Frame             oldFrame, newFrame;
    T_RPR_EdgeParams        edgeParams ;
    T_RPR_FillParam         fillParam;
    */
    struct DecodedFrame *pFrame;

    t->m_pCurrentFrame = 0;
    t->m_pDisplayFrame = 0;
    /* These will get initialized below if there is any */
    /* decoding and/or displaying to do. */

#define ERROR_PERCENTAGE 5

#ifdef BITSTREAM_ERROR_RESILIENT
#ifdef TEST_BITSTREAM_ERROR_RESILIENT

    if (get_random_number(100) >= 100 - ERROR_PERCENTAGE)
    {
        int bit_pos, byte_pos, bit_offset;

        bit_pos = get_random_number(8*src->size);
        byte_pos = bit_pos / 8;
        bit_offset = bit_pos - byte_pos * 8;

        RVDebug((RV_TL_ALWAYS,
                 "byte_pos: %d",byte_pos));

        t->m_pBitStream->m_pbs[byte_pos] = get_random_number(255);
    }



#endif
#endif

    for (;;)
        /* Not really a loop, just use break instead of goto on error */
    {
        if (src->format.fid == RV_FID_REALVIDEO30)
        {
            t->m_invariantPictureHeader.dimensions.width = src->format.dimensions.width;
            t->m_invariantPictureHeader.dimensions.height = src->format.dimensions.height;
            pictureHeader = t->m_invariantPictureHeader;
        }

        if (t->m_bIsRV8)
        {
            CB_SetRPRSizes(t->m_pBitStream, t->m_options.Num_RPR_Sizes, &t->m_options.RPR_Sizes[0]);
            ps = CB_GetPictureHeader_RV8(t->m_pBitStream, &pictureHeader);
        }
        else
        {
            ps = CB_GetPictureHeader(t->m_pBitStream, &pictureHeader);
        }

        if (ps == RV_S_BAD_FORMAT)
        {
            /* TBD, what is the special meaning of RV_S_BAD_FORMAT here, */
            /* and why do we return RV_S_OK? */
            /* Note that m_pCurrentFrame and m_pDisplayFrame are NULL. */
            ps = RV_S_OK;
            break;
        }

        if (ps != RV_S_OK)
        {
            RVDebug((RV_TL_ALWAYS,
                     "ERROR  ::prepareDecodeBuffers -- Bit stream"
                     " picture header problem"));
            break;
        }

        /* */
        /* Based on the incoming picture type and latency mode, set up our */
        /* m_pCurrentFrame and m_pDisplayFrame pointers for the current decode. */
        /* Recall that m_pCurrentFrame is the incoming frame that we are */
        /* going to parse and decode, while m_pDisplayFrame is the frame */
        /* we are going to return for display. */
        /* */
        /* The following table summarizes which frame should be displayed. */
        /* Across the top is the incoming picture type. */
        /* Down the left side is the current state of latency mode, */
        /* and the number of decoded reference frames already present */
        /* in the m_referenceFrames list. */
        /* Within the table, "ERROR" indicates a situation that should not */
        /* occur, "Current" indicates the current frame being decoded, */
        /* and "PrevTail" indicates the most recently decoded *reference* */
        /* frame (a.k.a., m_referenceFrames.tail()) unless it has already */
        /* been displayed, in which case PrevTail should be considered NULL. */
        /* */
        /*  Latency, ref frames  |    I          P              B        | */
        /*  =====================+=======================================+ */
        /*                       |                                       | */
        /*  Off, any             | Current    Current    enable latency, | */
        /*                       |                       discard and     | */
        /*                       |                       return DONT_DRAW| */
        /*                       |                                       | */
        /*                       |                                       | */
        /*  On,  0               | Current     ERROR          ERROR      | */
        /*                       |                                       | */
        /*                       |                                       | */
        /*  On,  1               | PrevTail    PrevTail       ERROR      | */
        /*                       |                                       | */
        /*                       |                                       | */
        /*  On,  2 (or more)     | PrevTail    PrevTail      Current     | */
        /*                       |                                       | */
        /*  =====================+=======================================+ */
        /* */
        /* The best way to verify this table is to walk through some examples */
        /* by hand, keeping track of what's in the reference frame list */
        /* when Decode() is entered. */
        /* */
        /* Typically we decode the incoming bitstream, and then display */
        /* (i.e., postfilter and color convert) a frame.  However, in some */
        /* cases we need to do the display first, followed by the decode. */
        /* This is true when latency mode is on and we are going to display */
        /* PrevTail (the previous reference frame), because some information */
        /* in m_pParsedData is used by the postfilters and m_pIInfoDisplay. */
        /* We need to let such post processing use the information in */
        /* m_pParsedData that pertains to PrevTail, before letting it be */
        /* overwritten when we decode the incoming bitstream. */

        pPreviousTail = t->m_referenceFrames.m_pTail;

        if (ISBFRAME(pictureHeader))
        {
            if (!t->m_isLatencyEnabled)
            {
                t->m_isLatencyEnabled = TRUE;
            }

            if (flags & RV_DDF_DONT_DRAW)
            {
                /* No need to decode if user doesn't want it displayed */
                ps = RV_S_OK;
                break;
            }

            if (t->m_bLocalDecoder)
            {
                RVDebug((RV_TL_ALWAYS, "ERROR ::prepareDecodeBuffers()"
                         " -- B-frame seen for encoder's decoder"));
                ps = RV_S_ERROR;
                break;
            }

            /* We better have two reference frames available */
            if (!t->m_referenceFrames.m_pTail
                    || !t->m_referenceFrames.m_pTail->m_pPreviousFrame)
            {
#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
                fprintf(g_framelist, "B-frame without reference frames (%d)\n",
                        pictureHeader.TR);
                fprintf(g_framelist, "Length of ref list: %d\n", t->m_referenceFrames.m_uLength);
#endif
                RVDebug((RV_TL_ALWAYS, "ERROR ::prepareDecodeBuffers"
                         " -- B-frame seen without two reference frames"));
                ps = RV_S_OK; /* will result in DDN_DONT_DRAW note */
                break;
            }

            /* Verify that this B frame lies temporally between the */
            /* existing reference frames.  It may not if a reference */
            /* frame is lost in transmission. */

            {
                U32 previousTR = t->m_referenceFrames.m_pTail->m_pPreviousFrame->
                                 m_pictureHeader.TR;
                U32 futureTR = t->m_referenceFrames.m_pTail->m_pictureHeader.TR;
                U32 thisTR = pictureHeader.TR;

                between = FALSE;

                if (futureTR > previousTR)
                    between = (previousTR < thisTR) && (thisTR < futureTR);
                else if (futureTR < previousTR)
                    /* reference frame TR's have wrapped around */
                    between = (thisTR < futureTR) || (thisTR > previousTR);
                /* else, futureTR == previousTR, let between = false; */

                if(rm_video_key_seek_event)// sheen
                {
                    between = FALSE;
                }

                if (!between)
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR ::prepareDecodeBuffers"
                             " -- B-frame seen out of order"
                             " (prevTR %ld, B %ld, futureTR %ld)",
                             previousTR, thisTR, futureTR));

                    /* Return RV_S_OK rather than an error. */
                    /* This will cause RV_DDN_DONT_DRAW to be returned */
                    /* in the notes (at the end of this function). */
                    /* Returning an error might be too harsh in some */
                    /* environments, causing the application to stop */
                    /* decoding completely.  In the spirit of loss */
                    /* concealment, just suppress the B frame. */
                    ps = RV_S_OK;
                    break;
                }
            }

            if (t->m_referenceFrames.m_pTail->m_wasDisplayed)
            {
                /* It's no use decoding this B frame, since we already */
                /* displayed its future reference. */
                ps = RV_S_OK;
                break;
            }

            /* The reference frame image dimensions better match the B frame. */
            /* (I suppose it is conceivable that if the future frame is */
            /* an I frame that differs in dimensions from the previous */
            /* reference frame, we could resample the previous frame and */
            /* proceed.  But it is my understanding that the encoder does */
            /* not allow this behavior.) */

            if (
                pictureHeader.dimensions.width !=
                t->m_referenceFrames.m_pTail->m_pictureHeader.dimensions.width
                ||
                pictureHeader.dimensions.height !=
                t->m_referenceFrames.m_pTail->m_pictureHeader.dimensions.height
                ||
                pictureHeader.dimensions.width !=
                t->m_referenceFrames.m_pTail->m_pPreviousFrame->m_pictureHeader.dimensions.width
                ||
                pictureHeader.dimensions.height !=
                t->m_referenceFrames.m_pTail->m_pPreviousFrame->m_pictureHeader.dimensions.height
            )
            {
                RVDebug((RV_TL_ALWAYS, "ERROR ::prepareDecodeBuffers"
                         " -- B-frame image size (%ld x %ld) does not match both"
                         " reference frames",
                         pictureHeader.dimensions.width,
                         pictureHeader.dimensions.height));
                ps = RV_S_ERROR;
                break;
            }

            /* Decode into m_BFrame. */

#if defined (HW_VIDEO_MEMORY)

            if (t->m_bHwVideoMemory)
            {
                t->m_pBFrame = DecodedFrameList2_detachHead(&t->m_freeFrames);
                /*              t->m_pBFrame->addref(); */
            }
            else
            {
                t->m_pBFrame = &t->m_BFrame;
            }

            if (!t->m_pBFrame)
            {
                RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                         "ERROR: could not find a free frame to decode a "
                         "B frame into"));
                ps = RV_S_ERROR;
                RVAssert(t->m_pBFrame);
                break;
            }


#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
            fprintf(g_framelist, "getting B frame # %d (%d)\n",
                    t->m_pBFrame->m_uFrameNumber, pictureHeader.TR);
#endif


#else
#ifdef INCLUDE_FRU
            if (t->m_pBFrame == &t->m_BFrame)
                t->m_pBFrame = &t->m_BFrame2;
            else
#endif
                t->m_pBFrame = &t->m_BFrame;
#endif
            t->m_pCurrentFrame = t->m_pBFrame;

            t->m_pCurrentFrame->m_pPreviousFrame = (t->m_referenceFrames.m_pTail->m_pPreviousFrame);
            t->m_pCurrentFrame->m_pFutureFrame = (t->m_referenceFrames.m_pTail);

            /* B frames are displayed immediately */

            t->m_pDisplayFrame = t->m_pCurrentFrame;
        }
        else
        {

            if(rm_video_key_seek_event)
            {
                //after decode I P. then B have 2 reference
                rm_video_key_seek_event-=1;
            }


            /* Due to bugs in some video environments, it was shown */
            /* to be possible that the decoder might receive the same */
            /* bitstream twice in a row.  The ILVC decoder tried to detect */
            /* this case, and would return early here.  If I recall, that */
            /* problem only occurred with key frames, and I think this */
            /* decoder can successfully decode the same key frame twice. */
            /* So we don't do any such checking. */

            if (!t->m_referenceFrames.m_pHead)
            {
                /* Record timing information from the very first frame we see */
                t->m_initialSequenceNumber = (F64)(src->sequence_number);
                t->m_initialTR = (F64)(pictureHeader.TR);

                if (ISPFRAME(pictureHeader))
                {
                    RVDebug((RV_TL_ALWAYS, "ERROR ::prepareDecodeBuffers --"
                             " P-frame seen without an existing reference frame"));
                    ps = RV_S_ERROR;
                    break;
                }
            }

            /* To avoid dangling references, let's invalidate our B frame */
            /* buffer's reference frame pointers.  This shouldn't strictly */
            /* be necessary, but perhaps it will let us catch some bugs. */
#if !defined (HW_VIDEO_MEMORY)
            t->m_BFrame.m_pPreviousFrame = 0;
            t->m_BFrame.m_pFutureFrame = 0;
#endif

            /* If we have to display prior to decode, this is the point */
            /* at which we do the display. */

            if (t->m_isLatencyEnabled
                    && pPreviousTail && !pPreviousTail->m_wasDisplayed
                    && !(flags & RV_DDF_DONT_DRAW)
               )
            {
                t->m_pDisplayFrame = pPreviousTail;

                t->m_pMBInfo = t->m_pReferenceMBInfo;
                /* m_pMBInfo is used by outputAFrame and applyPostFilters */
                /* When we are done displaying this frame, we'll */
                /* potentially update m_pMBInfo below for decoding */
                /* the incoming bitstream. */

#ifdef INCLUDE_FRU
                /* Perform FRU if turned on and this is */
                /* the first time Decode has been called with this bitstream (i.e. */
                /* the "flags" field does not indicate RV_DDF_MORE_FRAMES) -- This */
                /* is done to prevent FRU from beening performed more than once */
                /* on the same input frame. */
                if ((t->m_options.FrameRateUpsampling) &&
                        !(flags & RV_DDF_MORE_FRAMES))
                {
                    U32 FruFrames;

                    FruFrames = FrameRateUpsampler_SetUpFRU(&t->m_FRU,
                                                            t->m_pPreviousDisplayFrame,
                                                            t->m_pDisplayFrame,
                                                            pPreviousTail->m_pPreviousFrame,
                                                            pPreviousTail,
                                                            t->m_pBFrameMBInfo,
                                                            t->m_pReferenceMBInfo,
                                                            t->m_pBMotionVectors,
                                                            t->m_pReferenceMotionVectors);

                    if (FruFrames)
                    {
                        struct PictureHeader FRUPictureHeader;
                        /* If we can retrieve FRU frames then get one and  */
                        /* output it. */
                        for(;;)
                        {
#if defined (HW_VIDEO_MEMORY)
                            if (t->m_bHwVideoMemory)
                                t->m_pFRUFrame = t->m_freeFrames.detachHead();
                            else
#endif
                                t->m_pFRUFrame = &t->m_FRUFrame;

#if defined (HW_VIDEO_MEMORY)
                            if (!t->m_pFRUFrame)
                            {
                                RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                         "WARNING: no available frame to FRU into"));

                                if (t->m_uVideoBuffers == MAX_NUM_VIDEO_BUFFERS)
                                {
                                    RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                             "ERROR: could not find a free frame to FRU into"));
                                    break;
                                }

                                t->m_pFRUFrame = new DecodedFrame();
                                DecodedFrame_Init(t->m_pFRUFrame);

                                if (!t->m_pFRUFrame)
                                {
                                    RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                             "ERROR: could not allocate a frame to FRU into "));
                                    break;
                                }

                                t->m_pFRUFrame->m_uFrameNumber = t->m_uVideoBuffers;
                                t->m_pAllocatedFrames[t->m_uVideoBuffers] = t->m_pFRUFrame;
                                t->m_uVideoBuffers++;

#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
                                fprintf(g_framelist, "Allocating Frame %d for FRU\n",
                                        t->m_pFRUFrame->m_uFrameNumber);
#endif
                            }
#endif
                            /* Make sure the decode buffer is large enough, */
                            /* and clear its state. */
                            FRUPictureHeader = t->m_invariantPictureHeader;
                            FRUPictureHeader.PicCodType = FRUPIC;
                            FRUPictureHeader.dimensions = t->m_pDisplayFrame->m_pictureHeader.dimensions;

                            ps = DecodedFrame_allocate(t->m_pFRUFrame, &FRUPictureHeader.dimensions);
                            if (ps == RV_S_OK)
                            {
                                t->m_pFRUFrame->m_pictureHeader = FRUPictureHeader;
                                t->m_pFRUFrame->m_pPreviousFrame = pPreviousTail->m_pPreviousFrame;
                                t->m_pFRUFrame->m_pFutureFrame = pPreviousTail;
                                /*t->m_pFRUFrame->setFrameHeader(FRUPictureHeader); */
                                /*t->m_pFRUFrame->setPrevious(pPreviousTail->m_pPreviousFrame); */
                                /*t->m_pFRUFrame->setFuture(pPreviousTail); */

                                ps = FrameRateUpsampler_FillAFRUFrame(&t->m_FRU, t->m_pFRUFrame);

                                if (ps == RV_S_OK)
                                {
                                    ps = Decoder_outputAFrame(t, t->m_pFRUFrame, src, dst, notes, temporal_offset);

                                    if (ps == RV_S_OK)
                                    {
                                        *notes |= RV_DDN_MORE_FRAMES;
                                        if (FrameRateUpsampler_IsThereMoreFRUFrames(&t->m_FRU))
                                        {
                                            t->m_pDisplayFrame = 0;
                                            return ps;
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
#endif
                /* If "notes" says that there are more frames, that means */
                /* that FRU has generated a frame.  If so, then  */
                /* outputAFrame() has already been called and the */
                /* calling app will call Decode again to output the */
                /* m_pDisplayFrame. */
                if ((*notes & RV_DDN_MORE_FRAMES) == 0)
                {
                    ps = Decoder_outputAFrame(t, t->m_pDisplayFrame, src, dst,
                                              notes, temporal_offset);
                    if (ps != RV_S_OK)
                        break;

                    t->m_pDisplayFrame = 0;
                    wasFrameDisplayedBeforeDecode = TRUE;
                }
            }

            /* Validate the incoming bitstream's image dimensions. */
            /* Remember, these can change on a frame-by-frame basis, */
            /* so we need to check them on each frame. */

            {

                incomingSrcFormat.fid = src->format.fid;
                incomingSrcFormat.dimensions.width = pictureHeader.dimensions.width;
                incomingSrcFormat.dimensions.height = pictureHeader.dimensions.height;
                DecodedFrame_Propagate_Defaults(&incomingSrcFormat);
                ps = Decoder_CheckInput(t, &incomingSrcFormat);
                if (ps != RV_S_OK)
                    break;
            }

            /* If we have to perform reference picture resampling, make sure */
            /* all buffer allocations succeed prior to changing our state. */
            /* In case of an error, this will leave us in a more consistent */
            /* state. */

#ifdef RPR_REQUIRED

            if (ISPFRAME(pictureHeader)
                    &&
                    (pictureHeader.dimensions.width
                     != t->m_referenceFrames.m_pTail->m_pictureHeader.dimensions.width
                     ||
                     pictureHeader.dimensions.height
                     != t->m_referenceFrames.m_pTail->m_pictureHeader.dimensions.height)
               )
            {
                /* Okay, we need to resample the prior reference frame. */
                /* It's likely that our post processing buffer is large */
                /* enough for the new size (because all buffers were */
                /* allocated at Start_Sequence with the display size). */
                /* So, let's resample into the post processing buffer, */
                /* and then simply swap buffer pointers between the */
                /* post processing buffer and the reference buffer. */

                pNewBuffer = &t->m_postBuffer;
                pOldBuffer = t->m_referenceFrames.m_pTail;
                edgeParams.iDeltaBottom = 0;
                edgeParams.iDeltaLeft = 0;
                edgeParams.iDeltaRight = 0;
                edgeParams.iDeltaTop = 0;

#if defined (HW_VIDEO_MEMORY)
                ps = DecodedFrame_allocate(pNewBuffer,&t->m_referenceFrames.m_pTail->m_pictureHeader.dimensions);
                /* we did not allocate postbuffer, because in this mode we don't */
                /* normally need it.  However, in the case of RPR, it is needed.   */
                /* In order to make sure color convert works, which is going to */
                /* be called with the wrong (previous) size for one frame, make  */
                /* sure we allocate enough memory up-front (the previous size) */
#endif
                ps = DecodedFrame_allocate(pNewBuffer, &pictureHeader.dimensions);
                if (ps != RV_S_OK)
                    break;

                /* Wouldn't it be nice if the resampling interface */
                /* simply took a pointer to a PaddedYUVBuffer? */
                oldFrame.PlanePtrs.pYPlane = pOldBuffer->m_pYPlane;
                oldFrame.PlanePtrs.pUPlane = pOldBuffer->m_pUPlane;
                oldFrame.PlanePtrs.pVPlane = pOldBuffer->m_pVPlane;
                oldFrame.uPitch            = pOldBuffer->m_pitch;
                oldFrame.uYWidth           =
                    pOldBuffer->m_pictureHeader.dimensions.width;
                oldFrame.uYPaddedWidth     = pOldBuffer->m_lumaSize.width;
                oldFrame.uYHeight          =
                    pOldBuffer->m_pictureHeader.dimensions.height;
                oldFrame.uYPaddedHeight    = pOldBuffer->m_lumaSize.height;

                newFrame.PlanePtrs.pYPlane = pNewBuffer->m_pYPlane;
                newFrame.PlanePtrs.pUPlane = pNewBuffer->m_pUPlane;
                newFrame.PlanePtrs.pVPlane = pNewBuffer->m_pVPlane;
                newFrame.uPitch            = pNewBuffer->m_pitch;
                newFrame.uYWidth           = pictureHeader.dimensions.width;
                newFrame.uYPaddedWidth     = pNewBuffer->m_lumaSize.width;
                newFrame.uYHeight          = pictureHeader.dimensions.height;
                newFrame.uYPaddedHeight    = pNewBuffer->m_lumaSize.height;

                if (pictureHeader.RPR)
                {
                    edgeParams = pictureHeader.RPRP.RPREdgeParams;
                    fillParam  = pictureHeader.RPRP.RPRFillParam;
                }
                else
                {
                    fillParam.uMode = FILL_CLIP;
                }

                ResampleYUVFrame(&oldFrame, &newFrame,
                                 &edgeParams,
                                 &fillParam,
                                 FALSE,
                                 t->m_implementation_id >= MMX_IMPL);

                DecodedFrame_copyState(pNewBuffer, pOldBuffer);
                /* We assume that if the original image was filtered, */
                /* then so is the resampled image. */

                /* Move the resampled image from the post processing buffer */
                /* into the reference frame buffer, and update the reference */
                /* buffer's picture header to reflect the new image size. */

                DecodedFrame_swap(pOldBuffer, pNewBuffer);
                /*pOldBuffer->accountForResample(pictureHeader.dimensions); */
                pOldBuffer->m_pictureHeader.dimensions.width = pictureHeader.dimensions.width;
                pOldBuffer->m_pictureHeader.dimensions.height = pictureHeader.dimensions.height;


                pOldBuffer->m_dstWidth = ( pictureHeader.dimensions.width);
                pOldBuffer->m_dstHeight = (pictureHeader.dimensions.height);

#if 0
                /* If we delayed our early frame display, do it now. */
                if (t->m_pDisplayFrame && !t->m_pDisplayFrame->m_wasDisplayed)
                {
                    ps = outputAFrame(t->m_pDisplayFrame, src, dst,
                                      notes, temporal_offset);
                    if (ps != RV_S_OK)
                        break;

                    t->m_pDisplayFrame = 0;
                    wasFrameDisplayedBeforeDecode = true;
                }
#endif /* if 0 */
            }
#endif /* RPR_REQUIRED */

            /* Grab a frame buffer from our free list and add it to our tail. */
            /* If none are free, then move one from our head to our tail. */


#if defined (HW_VIDEO_MEMORY)

            if (t->m_bHwVideoMemory)
            {
                struct DecodedFrame *pCurr;

                if (t->m_referenceFrames.m_uLength == 2)
                {
                    pCurr = DecodedFrameList_detachHead(&t->m_referenceFrames);

                    DecodedFrame_release(pCurr);
                    /* indicate this frame is ready to be released */
                    /* we don't use it as reference frame */
                    if (!DecodedFrame_getrefcount(pCurr))
                        Decoder_ReleaseFrame(t,pCurr);
                }
                pFrame = DecodedFrameList2_detachHead( &t->m_freeFrames);
                if (!pFrame)
                {
                    RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                             "WARNING: no available frame decode a P frame into"));

                    if (t->m_uVideoBuffers == MAX_NUM_VIDEO_BUFFERS)
                    {
                        RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                 "ERROR: could not find a free frame to "
                                 "decode a P frame into"));
                        ps = RV_S_ERROR;
#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
                        fflush(g_framelist);
#endif
                        RVAssert(pFrame);
                        break;
                    }

                    pFrame = (struct DecodedFrame *)RV_Allocate(sizeof(struct DecodedFrame), 0);
                    DecodedFrame_Init(pFrame);

                    if (!pFrame)
                    {
                        RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                 "ERROR: could not allocate a free frame to "
                                 "decode a P frame into"));
                        ps = RV_S_ERROR;
#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
                        fflush(g_framelist);
#endif
                        RVAssert(pFrame);
                        break;
                    }

                    pFrame->m_uFrameNumber = t->m_uVideoBuffers;
                    t->m_pAllocatedFrames[t->m_uVideoBuffers] = pFrame;
                    t->m_uVideoBuffers++;

#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
                    fprintf(g_framelist, "Allocating Frame %d for P frame decode\n",
                            pFrame->m_uFrameNumber);
#endif
                }
            }
            else
            {
                pFrame = DecodedFrameList2_detachHead(&t->m_freeFrames);
                if (!pFrame)
                    pFrame = DecodedFrameList_detachHead(&t->m_referenceFrames);
            }

#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
            if (ISKEYFRAME(pictureHeader))
            {
                fprintf(g_framelist, "getting I frame # %d (%d)\n",
                        pFrame->m_uFrameNumber, pictureHeader.TR);
            }
            else
            {
                fprintf(g_framelist, "getting P frame # %d (%d)\n",
                        pFrame->m_uFrameNumber, pictureHeader.TR);
            }
#endif

#else
            pFrame = DecodedFrameList2_detachHead(&t->m_freeFrames);
            if (!pFrame)
                pFrame = DecodedFrameList_detachHead(&t->m_referenceFrames);
#endif

            RVAssert(pFrame);

            DecodedFrameList_append(&t->m_referenceFrames, pFrame);
            DecodedFrame_addref(pFrame);
            /* this is a reference frame, need to addref it */

            t->m_pCurrentFrame = pFrame;

            if (!t->m_isLatencyEnabled || !pPreviousTail)
                t->m_pDisplayFrame = t->m_pCurrentFrame;
        }

        /* Make sure the decode buffer is large enough, */
        /* and clear its state. */
#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION
        if(!ISBFRAME(pictureHeader) || (pictureHeader.dimensions.width*pictureHeader.dimensions.height <=LARGE_DIMENSION_SIZE &&
                                        RV_SKIP_B_FLAG==0 ))
#else
        if(!ISBFRAME(pictureHeader) || RV_SKIP_B_FLAG==0 )
#endif
        {
            ps = DecodedFrame_allocate(t->m_pCurrentFrame, &pictureHeader.dimensions);
            if (ps != RV_S_OK)
                break;
        }

        t->m_pCurrentFrame->m_pictureHeader = pictureHeader;

        /* Set the current frame's time stamp and cumulative TR */

        if (!t->m_pCurrentFrame->m_pPreviousFrame )
        {
            t->m_pCurrentFrame->m_cumulativeTR = ((F64)(pictureHeader.TR));
            t->m_pCurrentFrame->m_timeStamp = (src->sequence_number);
        }
        else
        {
            I32 TRDelta = pictureHeader.TR -
                          t->m_pCurrentFrame->m_pPreviousFrame->m_pictureHeader.TR;
            if (TRDelta < 0)
            {
                TRDelta += t->m_uTRWrap;
            }

            t->m_pCurrentFrame->m_cumulativeTR = (
                    t->m_pCurrentFrame->m_pPreviousFrame->m_cumulativeTR + (F64)(TRDelta));

            if (ISBFRAME(pictureHeader))
            {
                if (src->format.fid != RV_FID_REALVIDEO30)
                {
                    /* Convert TRDelta to milliseconds. */
                    /* It already is in milliseconds, for real video. */
                    TRDelta =
                        (I32)(((F64)(1000.0) / t->m_realvideo->m_picture_clock_frequency)
                              * TRDelta);
                }

                t->m_pCurrentFrame->m_timeStamp = (
                                                      t->m_pCurrentFrame->m_pPreviousFrame->m_timeStamp
                                                      + (U32)(TRDelta));

#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION
                if(pictureHeader.dimensions.width*pictureHeader.dimensions.height >LARGE_DIMENSION_SIZE)
                {
                    //not decode b frame. sheen
                    rv_Timestamp_dis=t->m_pCurrentFrame->m_timeStamp;
                    ps = RV_S_SKIP_BFRAME;
                    break;
                }
#endif
                if(RV_SKIP_B_FLAG!=0)
                {
                    //not decode b frame. sheen
                    rv_Timestamp_dis=t->m_pCurrentFrame->m_timeStamp;
                    ps = RV_S_SKIP_BFRAME;
                    break;
                }
            }
            else
                t->m_pCurrentFrame->m_timeStamp = (src->sequence_number);
        }

        if( rm_video_key_seek_event>0)
        {
            t->m_pCurrentFrame->m_cumulativeTR=t->m_pCurrentFrame->m_timeStamp;
        }

        //rm_printf("[RM]cur frame: m_cumulativeTR=%d m_timeStamp=%d\n",
        //  (U32)t->m_pCurrentFrame->m_cumulativeTR,(U32)t->m_pCurrentFrame->m_timeStamp);

        ps = Decoder_allocateParsedData(t, &pictureHeader.dimensions, FALSE);
        if (ps != RV_S_OK)
            break;

        if (ISBFRAME(t->m_pCurrentFrame->m_pictureHeader))
        {
            t->m_pMBInfo = t->m_pBFrameMBInfo;
            t->m_pMotionVectors=t->m_pBMotionVectors;
        }
        else
        {
            t->m_pMBInfo = t->m_pReferenceMBInfo;
            t->m_pMotionVectors=t->m_pReferenceMotionVectors;
        }

        /* Don't display a frame more than once */
        if (t->m_pDisplayFrame && t->m_pDisplayFrame->m_wasDisplayed)
            t->m_pDisplayFrame = 0;


        break;
    }

    if (ps != RV_S_OK || (flags & RV_DDF_DONT_DRAW)
            || (!t->m_pDisplayFrame && !wasFrameDisplayedBeforeDecode)
       )
    {
        *notes |= RV_DDN_DONT_DRAW;
        t->m_pDisplayFrame = 0;
    }

    return ps;
}

#ifdef MULTI_THREADED_DECODER

I32
Decoder_CheckOutSlice(struct Decoder *t)
{

    I32 retval;
    if (t->m_currentSliceNum < t->m_numSlices)
    {
        retval = t->m_currentSliceNum;
        t->m_currentSliceNum ++;
    }
    else
    {
        retval = -1;
    }
    return retval;
}


I32
Decoder_FindLastGoodMBNum(struct Decoder *t,I32 iSliceNum)
{
    I32 i;
    i = iSliceNum;
    while (!t->m_uLastGoodMBNum[i])
    {
        i--;
        if (i < 0)
        {
            return -1;
        }
    }
    return t->m_uLastGoodMBNum[i];
}

I32
Decoder_FindNextGoodMBNum(struct Decoder *t,I32 iSliceNum)
{
    I32 i;
    i = iSliceNum;
    while (t->m_iSliceMBA[i]<0)
    {
        i++;
        if (i > 127)
        {
            return (t->m_pCurrentFrame->m_macroBlockSize.width  * t->m_pCurrentFrame->m_macroBlockSize.height);
        }
    }
    return t->m_iSliceMBA[i];
}


U32
Decoder_InsertAllMissingMBs(struct Decoder *t)
{
    I32 i, mbnum;
    I32 iNumSlices, iSliceMBA;
    U32 mbXOffset, mbYOffset;
    struct DecoderPackedMotionVector *pMV;
    U32 uNumMissingMBs = 0;


    iNumSlices = t->m_pBitStream->m_nSlices;
    for (i = 0; i < iNumSlices; i++)
    {
        if (!CB_IsSliceValid(t->m_pBitStream,i))
        {
            mbnum = Decoder_FindLastGoodMBNum(t,i);
            mbnum ++;
            while (!CB_IsSliceValid(t->m_pBitStream,i+1) && i < 127)
                i++;
            iSliceMBA = Decoder_FindNextGoodMBNum(t,i);

            RVAssert(iSliceMBA >= (I32)mbnum);
            if ((I32)mbnum < iSliceMBA)
            {

                uNumMissingMBs += iSliceMBA - mbnum;
                mbYOffset = mbnum / t->m_pCurrentFrame->m_macroBlockSize.width;
                mbXOffset = mbnum % t->m_pCurrentFrame->m_macroBlockSize.width;
                pMV = t->m_pMotionVectors + mbXOffset * 4 +
                      mbYOffset * 4 * t->m_pCurrentFrame->m_subBlockSize.width;
                mbYOffset *= 16;
                mbXOffset *= 16;
                Decoder_insertMissingMacroBlocks(t,mbnum, (U32)iSliceMBA,
                                                 &mbXOffset, &mbYOffset, &pMV);
            }
        }
    }

    return uNumMissingMBs;
}

#endif


/* RealNetworks' adaptive smoothing, adapts the smoothing strength
// to the activity of the video sequence. Maintains a high level
// of smoothing for periods where blocking would normally occur,
// while keeping it very low for low action, talking head kind
// of content. */

void
Decoder_AdaptiveSmoothingAdjustRV8(struct Decoder *t)
{

    U32 t_active_frame, t_very_active;
    U32 t_num_mbs_filter_more, num_mbs;

    num_mbs = t->m_pCurrentFrame->m_macroBlockSize.width *
              t->m_pCurrentFrame->m_macroBlockSize.height;
    t_active_frame = (num_mbs >> 2);
    t_very_active  = (num_mbs >> 1);
    t_num_mbs_filter_more  = 396; // CIF

    if (t->m_num_active_mbs < t_active_frame)
    {
        t->m_num_weak++;
        t->m_num_strong--;
    }
    else if (t->m_num_active_mbs < t_very_active)
    {
        t->m_num_weak--;
        t->m_num_strong++;
    }
    else
    {
        t->m_num_weak -= 2;
        t->m_num_strong += 2;
    }

    t->m_size_adjustment_factor = 0;
    if (num_mbs <= t_num_mbs_filter_more)
    {
        t->m_size_adjustment_factor =
            OFFSET_STRONGER_FILTER;
    }


    /* Adjust high QP counter. */
    if (t->m_pCurrentFrame->m_pictureHeader.PQUANT < 8)
    {
        t->m_num_highQP--;
    }
    else if (t->m_pCurrentFrame->m_pictureHeader.PQUANT < 20)
    {
        t->m_num_highQP++;
    }
    else if (t->m_pCurrentFrame->m_pictureHeader.PQUANT < 24)
    {
        t->m_num_highQP += 2;
    }
    else
    {
        t->m_num_highQP += 4;
    }

    t->m_num_weak =   MIN(20,MAX(0,t->m_num_weak));
    t->m_num_strong = MIN(20,MAX(0,t->m_num_strong));

    t->m_num_highQP = MIN(70,MAX(0,t->m_num_highQP));

    /* What we want to achieve here is to prevent
    // the smoothing postfilter from going on
    // just for a few frames, since this will make
    // it very difficult to maintain a good measure
    // of decode times for the outside world's cpu
    // scalability.
    // When m_num_highQP has reached the point
    // where smoothing is enabled, we want it
    // to stay on for a while.  */

    if (!ISBFRAME(t->m_pCurrentFrame->m_pictureHeader))
    {
        switch (t->m_global_adjusted_smoothing_strength)
        {
            case NO_SMOOTHING:
                if (t->m_num_highQP > 30)
                    t->m_global_adjusted_smoothing_strength = MEDIUM_SMOOTHING;
                /* setting the strength to MEDIUM here will
                // make sure the strength is set to the
                // correct value (depending on the activity
                // measurement) in the next switch statement. */
                break;
            default:
                if (t->m_num_highQP < 15)
                    t->m_global_adjusted_smoothing_strength = NO_SMOOTHING;
                /* QP has been below the low threshold for so long
                // it's time to disable the smoothing completely. */
                break;
        }
    }

    /* based on our activity measurement, set the
    // smoothing strength. */

    switch (t->m_global_adjusted_smoothing_strength)
    {
        case NO_SMOOTHING:
            break;
        case WEAK_SMOOTHING:
            /* change from weak to medium? */
            if (t->m_num_strong > t->m_num_weak + 3)
                t->m_global_adjusted_smoothing_strength =
                    MEDIUM_SMOOTHING;
            break;
        case MEDIUM_SMOOTHING:
            /* change from medium to strong? */
            if (t->m_num_strong > t->m_num_weak + 8)
                t->m_global_adjusted_smoothing_strength =
                    STRONG_SMOOTHING;
            /* change from medium to weak */
            if (t->m_num_weak > t->m_num_strong + 10)
                t->m_global_adjusted_smoothing_strength =
                    WEAK_SMOOTHING;
            break;
        case STRONG_SMOOTHING:
            /* change from strong to medium? */
            if (t->m_num_weak > t->m_num_strong + 5)
                t->m_global_adjusted_smoothing_strength =
                    MEDIUM_SMOOTHING;
            break;
        default:
            RVAssert(0);
            break;
    }


    t->m_pCurrentFrame->m_adjusted_smoothing_strength =
        t->m_global_adjusted_smoothing_strength;



#ifdef DEBUG_ADAPTIVE_SMOOTHING
    fprintf(g_dbg, "%5d s=%5d (%2d:%2d): %s %3d/%3d : =%d= %2d|%2d|%2d\n",
            t->m_pCurrentFrame->m_pictureHeader.TR,
            num_mbs,
            t->m_pCurrentFrame->m_pictureHeader.PQUANT,
            t->m_pReferenceMBInfo[0].QP,
            ISKEYFRAME(t->m_pCurrentFrame->m_pictureHeader) ? (CB_GetForcedKeyState(t->m_pBitStream) ? "I*": "I ") :
            (ISPFRAME(t->m_pCurrentFrame->m_pictureHeader) ? "P " : "B "),
            t->m_num_active_mbs,
            t->m_pCurrentFrame->m_macroBlockSize.width *
            t->m_pCurrentFrame->m_macroBlockSize.height,
            t->m_pCurrentFrame->m_adjusted_smoothing_strength,
            t->m_num_weak,
            t->m_num_strong,
            t->m_num_highQP
           );
#endif
}

//extern VocWorkStruct *pRmVocWorkState;
extern VocWorkStruct vocWorkState;

/* transfer just enough decoder state from one decoder to another to
   enable new decoder to start decoding at any P frame (any B frames
   recived after the transfer before the first P frame is decoded will
   be lost) */

RV_Status
Decoder_Transfer_State
(
    void *d,
    void *s
)
{
    RV_Status status = RV_S_OK;
    struct Decoder *dst, *src;
    I32 width, height, pitch, j;
    U8 *dd, *ss;
    struct DecodedFrame *srcFrame;
    struct DecodedFrame *dstFrame;
    struct RV_Dimensions paddedSize;

    dst = (struct Decoder *)d;
    src = (struct Decoder *)s;

    RVDebug((RV_TL_ALWAYS, "Decoder_Transfer_State :: transfer state"));

    /* move a free frame to list of reference frames in new decoder */
    DecodedFrameList_append(&dst->m_referenceFrames,
                            DecodedFrameList2_detachHead(&dst->m_freeFrames));

    dstFrame = dst->m_referenceFrames.m_pHead;
    srcFrame = src->m_referenceFrames.m_pTail;

    /* addref since this is a reference frame */
    DecodedFrame_addref(dstFrame);

    /* copy state info */
    DecodedFrame_copyState(dstFrame,
                           srcFrame);

    dstFrame->m_lumaSize.width  = srcFrame->m_lumaSize.width;
    dstFrame->m_lumaSize.height = srcFrame->m_lumaSize.height;
    dstFrame->m_pitch = pitch   = srcFrame->m_pitch;
    dstFrame->m_timeStamp       = srcFrame->m_timeStamp;

    /* in case of HHR, the destination frame's buffer pointers need to be corrected
       to match the new pitch */
    dstFrame->m_pYPlane = dstFrame->m_pBuffer + YUV_Y_PADDING * (dstFrame->m_pitch + 1);
    //dstFrame->m_pUPlane = dstFrame->m_pBuffer + YUV_UV_PADDING +
    //  dstFrame->m_pitch * (2*YUV_Y_PADDING + dstFrame->m_lumaSize.height + YUV_UV_PADDING); sheen
    dstFrame->m_pUPlane = dstFrame->m_pBuffer + YUV_UV_PADDING +
                          dstFrame->m_pitch * (2*YUV_Y_PADDING + dstFrame->m_lumaSize.height) + (dstFrame->m_pitch>>1)*YUV_UV_PADDING;
    //dstFrame->m_pVPlane = dstFrame->m_pUPlane + (dstFrame->m_lumaSize.width >> 1) + YUV_Y_PADDING; sheen
    dstFrame->m_pVPlane = dstFrame->m_pUPlane + (dstFrame->m_pitch>>1)*((dstFrame->m_lumaSize.height>>1)+(YUV_UV_PADDING<<1));


    paddedSize.width  = (dstFrame->m_lumaSize.width  + 15) & ~15;
    paddedSize.height = (dstFrame->m_lumaSize.height + 15) & ~15;

    dstFrame->m_macroBlockSize.width  = paddedSize.width  >> 4;
    dstFrame->m_macroBlockSize.height = paddedSize.height >> 4;

    dstFrame->m_subBlockSize.width    = dstFrame->m_macroBlockSize.width  << 2;
    dstFrame->m_subBlockSize.height   = dstFrame->m_macroBlockSize.height << 2;


    /* after a decoder switch mid-stream the bitstream needs to know
       the correct dimensions */
    width = srcFrame->m_pictureHeader.dimensions.width;
    height = srcFrame->m_pictureHeader.dimensions.height;
    dst->m_pBitStream->m_rv.pwidth_prev = width;
    dst->m_pBitStream->m_rv.pheight_prev = height;
    dst->m_pBitStream->m_rv.pwidth = width;
    dst->m_pBitStream->m_rv.pheight = height;

    width = paddedSize.width;
    height = paddedSize.height;

    /* RPR and other options settings (RPR required for RV8) */
    memcpy(&dst->m_options, &src->m_options, sizeof(struct Global_Decoder_Options));

    /* memcpy reference frame picture header */
    memcpy(&dstFrame->m_pictureHeader,
           &srcFrame->m_pictureHeader,
           sizeof(struct PictureHeader));

    /* memcpy invariant picture header (needed for RM files, not for
       console app decoder */
    memcpy(&dst->m_invariantPictureHeader,
           &src->m_invariantPictureHeader,
           sizeof(struct PictureHeader));
    /* TODO: it may be possible to copy just certain parts of the picture
       header, but since this is very small compared to the one reference
       frame, I am assuming this detail may not be necessary */

    /* simple memcpy of reference frame data */
    dd = dstFrame->m_pYPlane;
    ss = srcFrame->m_pYPlane;
    for (j = 0; j < height; j++)
    {
        memcpy(dd,ss,width);
        dd += pitch;
        ss += pitch;
    }

    dd = dstFrame->m_pUPlane;
    ss = srcFrame->m_pUPlane;
    for (j = 0; j < height/2; j++)
    {
        memcpy(dd,ss,width/2);
        dd += pitch;
        ss += pitch;
    }

    dd = dstFrame->m_pVPlane;
    ss = srcFrame->m_pVPlane;
    for (j = 0; j < height/2; j++)
    {
        memcpy(dd,ss,width/2);
        dd += pitch;
        ss += pitch;
    }

    /* expand the reference frame */
    DecodedFrame_expand(dstFrame);
    /* if the expanded frame is copied above, this step is not necessary,
       but assuming a minimal amount of data is to be transferred, it
       is necessary */

    return status;

}


RV_Status Decoder_Decode(
    void *decoder,
    const struct RV_Image *src,
    struct RV_Image *dst,
    const RV_Decoder_Decompression_Flags  flags,
    RV_Decoder_Decompression_Notes *notes,
    I32                             temporal_offset
)
{
    RV_Status  ps = RV_S_OK;
    U32 numMissingMBs;
    struct Decoder *t;
    U32 i;

    t = (struct Decoder *)decoder;

    /* */
    /* First do a little bit of sanity checking */
    /* */

    /* Is the user trying to change input formats from what we agreed */
    /* upon during Start_Sequence?  Or was Start_Sequence not even called? */
    if (src->format.fid != t->m_input_format.fid
            || t->m_input_format.fid == RV_FID_UNDEFINED)
    {
        RVDebug((RV_TL_ALWAYS,
                 "ERROR ::Decode() called prior to Start_Sequence, or"
                 " with different format"));
        *notes |= RV_DDN_DONT_DRAW;
        return RV_S_ERROR;
    }

    /* If the RV_DDF_MORE_FRAMES flag is set, just retrieve */
    /* a previously decoded frame */
    if (flags & RV_DDF_MORE_FRAMES)
    {
#ifdef INCLUDE_FRU
        struct PictureHeader FRUPictureHeader;

        /* If there are more FRU frame to generate, go ahead and do it. */
        if (FrameRateUpsampler_IsThereMoreFRUFrames(&t->m_FRU))
        {
#if defined (HW_VIDEO_MEMORY)
            if (t->m_bHwVideoMemory)
                t->m_pFRUFrame = t->m_freeFrames.detachHead();
            else
#endif
                t->m_pFRUFrame = &t->m_FRUFrame;

#if defined (HW_VIDEO_MEMORY)
            if (!t->m_pFRUFrame)
            {
                RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                         "WARNING: no available frame to FRU into"));

                if (t->m_uVideoBuffers == MAX_NUM_VIDEO_BUFFERS)
                {
                    RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                             "ERROR: could not find a free frame to FRU into"));
                    return ps;
                }

                t->m_pFRUFrame = new DecodedFrame();
                DecodedFrame_Init(t->m_pFRUFrame);

                if (!t->m_pFRUFrame)
                {
                    RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                             "ERROR: could not allocate a frame to FRU into "));
                    return ps;
                }

                t->m_pFRUFrame->m_uFrameNumber = t->m_uVideoBuffers;
                t->m_pAllocatedFrames[t->m_uVideoBuffers] = t->m_pFRUFrame;
                t->m_uVideoBuffers++;

#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
                fprintf(g_framelist, "Allocating Frame %d for FRU\n",
                        t->m_pFRUFrame->m_uFrameNumber);
#endif
            }
#endif
            /* Make sure the decode buffer is large enough, */
            /* and clear its state. */
            FRUPictureHeader = t->m_invariantPictureHeader;
            FRUPictureHeader.PicCodType = FRUPIC;
            FRUPictureHeader.dimensions = t->m_pPreviousDisplayFrame->m_pictureHeader.dimensions;

            ps = DecodedFrame_allocate(t->m_pFRUFrame, &FRUPictureHeader.dimensions);
            if (ps == RV_S_OK)
            {
                t->m_pFRUFrame->m_pictureHeader = FRUPictureHeader;
                t->m_pFRUFrame->m_pPreviousFrame = t->m_referenceFrames.m_pTail;
                t->m_pFRUFrame->m_pFutureFrame = t->m_referenceFrames.m_pTail->m_pPreviousFrame;
                /*t->m_pFRUFrame->setFrameHeader(FRUPictureHeader); */
                /*t->m_pFRUFrame->setPrevious(t->m_referenceFrames.m_pTail); */
                /*t->m_pFRUFrame->setFuture(t->m_referenceFrames.m_pTail->m_pPreviousFrame); */

                ps = FrameRateUpsampler_FillAFRUFrame(&t->m_FRU, t->m_pFRUFrame);

                if (ps == RV_S_OK)
                {
                    ps = Decoder_outputAFrame(t, t->m_pFRUFrame, src, dst, notes, temporal_offset);
                    if (ps == RV_S_OK)
                    {
                        *notes |= RV_DDN_MORE_FRAMES;
                        return ps;
                    }
                }
            }
        }
        else
#endif
            if(flags &  RV_DDF_LAST_FRAME)
                t->m_pDisplayFrame = t->m_referenceFrames.m_pTail;

        if (t->m_pDisplayFrame)
        {
            ps = Decoder_outputAFrame(t, t->m_pDisplayFrame, src, dst, notes, temporal_offset);
            t->m_pDisplayFrame = 0;
            return ps;
        }

    }

    if (src->size == 0)
    {
        if((flags &  RV_DDF_LAST_FRAME) && t->m_isLatencyEnabled)
        {
            t->m_pDisplayFrame = t->m_referenceFrames.m_pTail;
            if (t->m_pDisplayFrame)
            {
                ps = Decoder_outputAFrame(t, t->m_pDisplayFrame, src, dst, notes, temporal_offset);
                t->m_pDisplayFrame = 0;
                return ps;
            }
        }

        /* A zero-size image can be legitimately encountered, */
        /* when decoding from some AVI files. */
        RVDebug((RV_TL_INIT,"WARNING ::Decode -- image size is 0"));
        *notes |= RV_DDN_DONT_DRAW;
        return RV_S_OK;
    }

    /* When a sequence containing B frames is encoded to some file formats, */
    /* there will be an 8-byte, all zeroes, dummy frame in place of the */
    /* initial B frame. */
    /* There's nothing to decode right now, but turn on latency because */
    /* it's a good bet that B frames are coming. */
    /* TBD, we might want to put this check in prepareDecodeBuffers. */
    /* That would allow 8-byte dummies to be used to flush out the final */
    /* reference frame at the end of a B-frame sequence.  Need to be */
    /* careful though, and make sure InitDecClumpedFrames can handle this. */

    if (src->size == 8
            /* Need to access src->data as bytes, not as a couple of U32's, */
            /* because theoretically it might not be 4-byte aligned. */
            /* Check the later bytes first, since they're more likely to */
            /* be non-zero in the general case. */
            && src->yuv_info.y_plane[7] == 0 && src->yuv_info.y_plane[6] == 0
            && src->yuv_info.y_plane[5] == 0 && src->yuv_info.y_plane[4] == 0
            && src->yuv_info.y_plane[3] == 0 && src->yuv_info.y_plane[2] == 0
            && src->yuv_info.y_plane[1] == 0 && src->yuv_info.y_plane[0] == 0
       )
    {
        RVDebug((RV_TL_INIT,"::Decode -- ignoring 8-byte dummy"
                 " frame, enabling latency"));
        t->m_isLatencyEnabled = TRUE;
        *notes |= RV_DDN_DONT_DRAW;
        return RV_S_OK;
    }

#if defined(DEC_TIMING_DETAIL)
    /* initialize per-frame timing counters */
    t->m_uTimeCount_DecodeMB = 0;
    t->m_uTimeCount_Deblock = 0;
    t->m_uTimeCount_Postfilter = 0;
    t->m_uTimeCount_Deblock_PrePost = 0;
    t->m_uTimeCount_Smooth_PrePost = 0;
    t->m_uTimeCount_decMBheader = 0;
    t->m_uTimeCount_decCoeffs = 0;
    t->m_uTimeCount_recMB = 0;
    t->m_uTimeCount_Utility[0] = 0;
    t->m_uTimeCount_Utility[1] = 0;
    t->m_uTimeCount_Utility[2] = 0;
#endif

    t->m_pCurrentFrame = 0;
    /* This prevents the previously decoded frame, if any, from having */
    /* its bitstream info exposed to the application. */

    CB_Reset(t->m_pBitStream, src->yuv_info.y_plane, src->size);
    for(;;)
    {
        ps = Decoder_prepareDecodeBuffers(t, src, dst, flags, notes, temporal_offset);

#ifdef INCLUDE_FRU
        /* If we've already got a frame to display, and the is more FRU */
        /* frames to retrieve, just return so that this frame is displayed. */
        if ((*notes & RV_DDN_MORE_FRAMES) && FrameRateUpsampler_IsThereMoreFRUFrames(&t->m_FRU))
            return ps;
#endif

        if (ps != RV_S_OK || !t->m_pCurrentFrame)
        {
            RVAssert(*notes & RV_DDN_DONT_DRAW);
            break;
        }

        numMissingMBs = 0;

#ifndef SHEEN_VC_DEBUG
        //wait over before expand edge.
        i=0;
        while(*(vocWorkState.pVOC_VID)!=0)
        {
            i++;

            if(i>0x2000000)
            {
                rm_printf("[RM] wait over at decode start.");
                return RV_S_ERROR;//about 60ms
            }
            //sxr_Sleep(1);

        }
#endif

        Decoder_startNextFrame(t);

        ps = Decoder_parseBitStream(t, -1, 0);
        if (ps != RV_S_OK)
        {
            *notes |= RV_DDN_DONT_DRAW;
            break;
        }
        numMissingMBs = t->m_uNumMissingMBs;

        if (t->m_bIsRV8)
        {
            Decoder_AdaptiveSmoothingAdjustRV8(t);
        }

        break; /* get out of for(;;) loop */
    }
    /* */
    /* Now post filter m_pDisplayFrame and copy it to the user's output buffer */
    /* */

#if defined(MULTI_THREADED_DECODER)
exit_for_loop:
#endif

    if (*notes & RV_DDN_DONT_DRAW)
    {
        RVDebug((RV_TL_PICT,
                 "::Decode -- %8ld msecs (%ld bytes) => RV_DDN_DONT_DRAW",
                 src->sequence_number, src->size));
    }
    else if (t->m_pDisplayFrame && (*notes & RV_DDN_MORE_FRAMES) == 0 && (flags & RV_DDF_LAST_FRAME)==0)
    {

#ifdef INCLUDE_FRU
        /* Before we call outputAFrame() on m_pDisplayFrame, lets try FRU. */
        /* We checked that (notes & RV_DDN_MORE_FRAMES) == 0 so that */
        /* FRU is only attempted on the first Decode call pn a particular */
        /* frame. Now we'll check if FRU is turned on. */
        if (t->m_options.FrameRateUpsampling)
        {
            U32 FruFrames;

            FruFrames = FrameRateUpsampler_SetUpFRU(&t->m_FRU,
                                                    t->m_pPreviousDisplayFrame,
                                                    t->m_pDisplayFrame,
                                                    t->m_referenceFrames.m_pTail->m_pPreviousFrame,
                                                    t->m_referenceFrames.m_pTail,
                                                    t->m_pBFrameMBInfo,
                                                    t->m_pReferenceMBInfo,
                                                    t->m_pBMotionVectors,
                                                    t->m_pReferenceMotionVectors);

            if (FruFrames)
            {
                struct PictureHeader FRUPictureHeader;
                /* If we can retrieve FRU frames then get one and  */
                /* output it. */
#if defined (HW_VIDEO_MEMORY)
                if (t->m_bHwVideoMemory)
                    t->m_pFRUFrame = t->m_freeFrames.detachHead();
                else
#endif
                    t->m_pFRUFrame = &t->m_FRUFrame;

#if defined (HW_VIDEO_MEMORY)
                if (!t->m_pFRUFrame)
                {
                    RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                             "WARNING: no available frame to FRU into"));

                    if (t->m_uVideoBuffers == MAX_NUM_VIDEO_BUFFERS)
                    {
                        RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                 "ERROR: could not find a free frame to FRU into"));
                        return ps;
                    }

                    t->m_pFRUFrame = new DecodedFrame();
                    DecodedFrame_Init(t->m_pFRUFrame);

                    if (!t->m_pFRUFrame)
                    {
                        RVDebug((RV_TL_ALWAYS,"Decoder::prepareDecodeBuffers -- "
                                 "ERROR: could not allocate a frame to FRU into "));
                        return ps;
                    }

                    t->m_pFRUFrame->m_uFrameNumber = m_uVideoBuffers;
                    t->m_pAllocatedFrames[t->m_uVideoBuffers] = t->m_pFRUFrame;
                    t->m_uVideoBuffers++;

#if defined (DEBUG) && defined(DEBUG_FRAMELIST)
                    fprintf(g_framelist, "Allocating Frame %d for FRU\n",
                            t->m_pFRUFrame->m_uFrameNumber);
#endif
                }
#endif
                /* Make sure the decode buffer is large enough, */
                /* and clear its state. */
                FRUPictureHeader = t->m_invariantPictureHeader;
                FRUPictureHeader.PicCodType = FRUPIC;
                FRUPictureHeader.dimensions = t->m_pDisplayFrame->m_pictureHeader.dimensions;

                ps = DecodedFrame_allocate(t->m_pFRUFrame, &FRUPictureHeader.dimensions);
                if (ps == RV_S_OK)
                {
                    t->m_pFRUFrame->m_pictureHeader = FRUPictureHeader;
                    t->m_pFRUFrame->m_pPreviousFrame = t->m_referenceFrames.m_pTail->m_pPreviousFrame;
                    t->m_pFRUFrame->m_pFutureFrame = t->m_referenceFrames.m_pTail;
                    /*t->m_pFRUFrame->setFrameHeader(FRUPictureHeader); */
                    /*t->m_pFRUFrame->setPrevious(t->m_referenceFrames.m_pTail->m_pPreviousFrame); */
                    /*t->m_pFRUFrame->setFuture(t->m_referenceFrames.m_pTail); */

                    ps = FrameRateUpsampler_FillAFRUFrame(&t->m_FRU, t->m_pFRUFrame);

                    if (ps == RV_S_OK)
                    {
                        ps = Decoder_outputAFrame(t, t->m_pFRUFrame, src, dst, notes, temporal_offset);

                        if (ps == RV_S_OK)
                            *notes |= RV_DDN_MORE_FRAMES;
                    }
                }
            }
        }
#endif
        /* If the notes field has not been set with RV_DDN_MORE_FRAMES, then FRU */
        /* was not successful in generating an output frame. If this is the case */
        /* we can go ahead and output m_pDisplayFrame. */
        if ((*notes & RV_DDN_MORE_FRAMES) == 0)
        {
            ps = Decoder_outputAFrame(t, t->m_pDisplayFrame, src, dst, notes, temporal_offset);
        }
    }

    if(flags & RV_DDF_LAST_FRAME && t->m_isLatencyEnabled)
        *notes |= RV_DDN_MORE_FRAMES;

    if (ps != RV_S_OK)
    {
        RVDebug((RV_TL_ALWAYS,
                 "::Decode ERROR -- encountered (%x)", ps));

        t->m_pCurrentFrame = 0;
        /* This prevents the application from trying to read the */
        /* bitstream info corresponding to this broken image. */
    }

#if defined(DEBUG) && defined(DEBUG_FRAMELIST)
    fflush(g_framelist);
#endif

    return ps;
}

RV_Status
Decoder_outputAFrame
(
    struct Decoder *t,
    struct DecodedFrame                   *pDisplayFrame,
    const struct RV_Image                        *src,
    struct RV_Image                        *dst,
    RV_Decoder_Decompression_Notes  *notes,
    I32                              temporal_offset
)
{
    RV_Status ps = RV_S_OK;
    struct DecodedFrame *pDisplayBuffer;
    struct DecodedFrame *tmp;

    for (;;)
        /* Not really a loop, just use break instead of goto on error */
    {
        tmp = pDisplayFrame;

#ifdef INCLUDE_FRU
        /* Point to the last frame to be output */
        if (pDisplayFrame)
            t->m_pPreviousDisplayFrame = pDisplayFrame;
#endif

        if (!pDisplayFrame)
        {
            RVAssert(pDisplayFrame);
            ps = RV_S_ERROR;
            break;
        }

        if (ISKEYFRAME(pDisplayFrame->m_pictureHeader))
            *notes |= RV_DDN_KEY_FRAME;
        else if (ISBFRAME(pDisplayFrame->m_pictureHeader))
            *notes |= RV_DDN_B_FRAME;
        else if (ISFRUFRAME(pDisplayFrame->m_pictureHeader))
            *notes |= RV_DDN_FRU_FRAME;

        /* Set the display presentation time, in milleseconds */

        if (src->format.fid == RV_FID_REALVIDEO30)
        {
            dst->sequence_number = pDisplayFrame->m_timeStamp;
        }
        else
        {
            dst->sequence_number =
                (U32)(
                    t->m_initialSequenceNumber +
                    ((F64)(1000.0) / t->m_realvideo->m_picture_clock_frequency)
                    * (pDisplayFrame->m_cumulativeTR - t->m_initialTR)
                );
        }


        RVAssert(t->m_pMBInfo);

#if defined(INFODISPLAY)
        if (t->m_pIInfoDisplay)
        {
            t->m_pIInfoDisplay->display(t->m_pMBInfo, dst->sequence_number);
        }
#endif /* defined(INFODISPLAY) */

        /* */
        /* Apply any requested postfilters */
        /* */

#if defined (HW_VIDEO_MEMORY)

        if (t->m_bHwVideoMemory)
        {
            U8 lastQP;
            U32 i;
            U32 totalMBs = pDisplayFrame->m_macroBlockSize.width * pDisplayFrame->m_macroBlockSize.height;

            lastQP = (!(ISKEYFRAME(pDisplayFrame->m_pictureHeader) ||
                        ISPFRAME(pDisplayFrame->m_pictureHeader))) ?
                     pDisplayFrame->m_pFutureFrame->m_pictureHeader.PQUANT :
                     pDisplayFrame->m_pictureHeader.PQUANT;

            for (i = 0; i < totalMBs; i ++)
            {
                if (t->m_pMBInfo[i].mbtype == MBTYPE_SKIPPED)
                {
                    if (!(ISKEYFRAME(pDisplayFrame->m_pictureHeader) ||
                            ISPFRAME(pDisplayFrame->m_pictureHeader)))
                        t->m_pMBInfo[i].QP = t->m_pReferenceMBInfo[i].QP;
                    else
                        t->m_pMBInfo[i].QP = lastQP;
                }
                lastQP = t->m_pMBInfo[i].QP;

                pDisplayFrame->m_pMBInfo[i].QP = t->m_pMBInfo[i].QP;
                /*              pDisplayFrame->m_pMBInfo[i].QP = 15; */
                pDisplayFrame->m_pMBInfo[i].mbtype = t->m_pMBInfo[i].mbtype;
                pDisplayFrame->m_pMBInfo[i].cbpcoef = t->m_pMBInfo[i].cbpcoef;
                /*              pDisplayFrame->m_pMBInfo[i].cbpcoef = t->m_pReferenceMBInfo[i].cbpcoef; */
                /*              pDisplayFrame->m_pMBInfo[i].cbpcoef = 0xffffff; */
            }
            pDisplayBuffer = 0; /* make sure nothing is displayed */
        }
        else
#endif
        {
            pDisplayBuffer = pDisplayFrame;
            /* sheen
            if (t->m_bIsRV8)
                ps = Decoder_applyPostFilters(t, pDisplayFrame, &src->format, dst,
                                      notes, &pDisplayBuffer);
            */
            if (ps != RV_S_OK)
                break;
        }

        if (src->format.fid == RV_FID_REALVIDEO30)
        {
            dst->format.dimensions.width = pDisplayFrame->m_pictureHeader.dimensions.width;
            dst->format.dimensions.height = pDisplayFrame->m_pictureHeader.dimensions.height;
            dst->format.rectangle.width  = dst->format.dimensions.width;
            dst->format.rectangle.height  = dst->format.dimensions.height;
        }

#if defined (HW_VIDEO_MEMORY)
        t->m_pLatestDisplayFrame = pDisplayFrame;
        /* store latest frame output to use in the custom */
        /* message that the front-end calls to retrieve */
        /* the latest frame from the decoder (in display */
        /* or output order) */
        /*      RVAssert(t->m_pDisplayFrame == pDisplayFrame); */

        DecodedFrame_addref(pDisplayFrame);
        pDisplayFrame->m_dstHeight =  dst->format.rectangle.height;
        pDisplayFrame->m_dstWidth =  dst->format.rectangle.width;


#if defined (DEBUG) && defined (DEBUG_FRAMELIST)
        {
            RV_Boolean bFree[17];
            U32 i;
            struct DecodedFrame *pCurr;

            fprintf(g_framelist, "Size (%d): %d x %d\n",
                    pDisplayFrame->m_uFrameNumber,
                    dst->format.rectangle.width, dst->format.rectangle.height);

            fprintf(g_framelist, "Free frames: ");
            pCurr = t->m_freeFrames.m_pHead;
            for (i= 0; i < 17; i++)
                bFree[i] = FALSE;
            for (; pCurr; pCurr = pCurr->m_pFutureFrame2)
            {
                fprintf(g_framelist, "%d [%d], ",
                        pCurr->m_uFrameNumber,
                        /*                  pCurr->timeStamp(), */
                        DecodedFrame_getrefcount(pCurr));
                bFree[pCurr->m_uFrameNumber] = TRUE;
            }
            fprintf(g_framelist, "\n");

            fprintf(g_framelist, "Used frames: ");
            for (i= 0; i < t->m_uVideoBuffers; i++)
            {
                if (!bFree[i])
                {
                    if (!DecodedFrame_getrefcount(t->m_pAllocatedFrames[i]))
                    {
                        fprintf(g_framelist, "***");
                    }
                    fprintf(g_framelist, "%d [%d], ",
                            t->m_pAllocatedFrames[i]->m_uFrameNumber,
                            /*                  m_pAllocatedFrames[i]->timeStamp(), */
                            DecodedFrame_getrefcount(t->m_pAllocatedFrames[i]));
                }

            }
            fprintf(g_framelist, "\n");

            fprintf(g_framelist, "Ref  frames: ");
            pCurr = t->m_referenceFrames.m_pHead;
            for (; pCurr; pCurr = pCurr->m_pFutureFrame)
            {
                fprintf(g_framelist, "%d, ", pCurr->m_uFrameNumber);
            }
            fprintf(g_framelist, "\n");

        }
#endif


#endif




        /* Perform output color conversion and video effects, if we didn't */
        /* already write our output to the application's buffer. */

        if (pDisplayBuffer)
        {
            {
                t->m_pYUVPitchChanger
                (
                    pDisplayBuffer->m_pYPlane,
                    pDisplayBuffer->m_pVPlane,
                    pDisplayBuffer->m_pUPlane,
                    dst->format.rectangle.width,
                    dst->format.rectangle.height,
                    pDisplayBuffer->m_pitch,
                    pDisplayBuffer->m_pitch>>1,//pDisplayBuffer->m_pitch,     /* assumes U pitch == V pitch */ sheen
                    9999, /* shaping (ARC) param not used */
                    dst->yuv_info.y_plane,
                    0, /* DCI offset not relevant */
                    0, /* offset to line 0 not relevant */
                    0, /* Output pitch not used */
                    0  /* CC id not used */
                );

            }
        }

        pDisplayFrame->m_wasDisplayed = TRUE;

        break;
    }

    RVDebug((RV_TL_ALWAYS,
             "::Decode -- %8ld msecs (%4ld bytes) => %8ld msecs (%d, %d)",
             src->sequence_number, src->size,
             dst->sequence_number, dst->format.rectangle.width, dst->format.rectangle.height));

    return ps;
}

#if 0//sheen

RV_Status
Decoder_applyPostFilters
(
    struct Decoder *t,
    struct DecodedFrame                       *pDisplayFrame,
    const struct RV_Image_Format             *srcFormat,
    struct RV_Image                          *dst,
    RV_Decoder_Decompression_Notes    *notes,
    struct DecodedFrame               **pDisplayBuffer
)
{
    RV_Status ps = RV_S_OK;
    struct DecodedFrame *pDstBuffer;
    RV_Boolean    writeToDst;
    U32 uPitchYSrc;
    U32 uPitchVSrc;
    PU8 pYSrc;
    PU8 pUSrc ;
    PU8 pVSrc ;
    PU8 pYDst ;
    PU8 pUDst ;
    PU8 pVDst ;
    U8     *pDstYPlane, *pDstUPlane, *pDstVPlane;
    U32     dstYPitch, dstUVPitch, dstYWidth, dstYHeight;
    struct RV_Image   rvImage;
    U8 lastQP;
    U32 totalMBs, i,j,chroma_h,chroma_w;



    for (;;)
        /* Not really a loop. */
        /* Just allows us to use "break" instead of "goto" on errors. */
    {
        *pDisplayBuffer = pDisplayFrame;

        if (Global_Decoder_Options_isSmoothingRequested(&t->m_options) && !(*pDisplayBuffer)->m_isSmoothed)
        {
            /* The smoothing filter is a copy filter.  If the */
            /* application's output buffer is suitably aligned */
            /* and padded, write the output directly into it. */
            /* Otherwise, write to m_postBuffer. */
            /* */
            /* Be aware that the size of the image to be drawn */
            /* is not pDisplayFrame->m_lumaSize, but rather */
            /* pDisplayFrame->m_pictureHeader.dimensions.  The former */
            /* was rounded up to a macroblock boundary for coding */
            /* simplicity, while the latter represents the original */
            /* size of the picture that was coded. */

            pDstBuffer = &t->m_postBuffer;
            writeToDst = FALSE;

            /* First determine if we can write directly to dst. */

            {
                if (srcFormat->fid == RV_FID_REALVIDEO30)
                {
                    /* Recall that for RealVideo, we write the bitstream's */
                    /* image dimensions, not dst's. */
                    rvImage.format.fid = dst->format.fid;
                    rvImage.format.dimensions.width =
                        pDisplayFrame->m_pictureHeader.dimensions.width;
                    rvImage.format.dimensions.height =
                        pDisplayFrame->m_pictureHeader.dimensions.height;

                    DecodedFrame_Propagate_Defaults(&rvImage.format);
                    /* Note that we assume the RealVideo output buffer */
                    /* is not pitched. */

                    writeToDst =
                        SmoothingFilter_CheckOutput(&t->m_smoothingPostFilter,
                                                    rvImage.format.fid,
                                                    rvImage.format.rectangle.width,
                                                    rvImage.format.rectangle.height,
                                                    rvImage.format.yuv_info.y_pitch,
                                                    rvImage.format.yuv_info.u_pitch,
                                                    rvImage.format.yuv_info.v_pitch)
                        == RV_S_OK;
                }
                else
                {
                    /* For now the ARM RV89Combo player app is using the output */
                    /* buffers supplied by the decoder, so here the use of */
                    /* the caller's buffers is disallowed. */
#if !defined(ARM) && !defined(_ARM_)
                    /* We can only write to the user's ouptut buffer if */
                    /* no resampling is needed. */
                    writeToDst =
                        (dst->format.rectangle.width
                         == pDisplayFrame->m_pictureHeader.dimensions.width) &&
                        (dst->format.rectangle.height
                         == pDisplayFrame->m_pictureHeader.dimensions.height)
                        && (SmoothingFilter_CheckOutput(&t->m_smoothingPostFilter,
                                                        dst->format.fid,
                                                        dst->format.rectangle.width,
                                                        dst->format.rectangle.height,
                                                        dst->format.yuv_info.y_pitch,
                                                        dst->format.yuv_info.u_pitch,
                                                        dst->format.yuv_info.v_pitch) == RV_S_OK);
#endif
                }
            }


            /* Now set up the postfilter's destination buffer and image width. */
            /* When not writing directly to dst, we go ahead and filter the */
            /* whole image (i.e., we use the macroblock-padded dimensions). */
            /* This may be a little wasteful when the desired display */
            /* width is not a multiple of 16 (which is rare, by the way). */
            /* However we *must* ensure in this case that the image width */
            /* is a multiple of 8, due to optimizations in the postfilter. */
            /* And using the macroblock-padded dimensions gives us this */
            /* assurance. */

            if (writeToDst)
            {
                if (srcFormat->fid == RV_FID_REALVIDEO30
                        && ((pDisplayFrame->m_pictureHeader.dimensions.width
                             != dst->format.rectangle.width) ||
                            (pDisplayFrame->m_pictureHeader.dimensions.height
                             != dst->format.rectangle.height))
                   )
                {
                    /* Update 'dst' with the actual bitstream dimensions. */
                    dst->format.dimensions.width =
                        pDisplayFrame->m_pictureHeader.dimensions.width;
                    dst->format.dimensions.height =
                        pDisplayFrame->m_pictureHeader.dimensions.height;

                    DecodedFrame_Propagate_Defaults(&dst->format);

                    /* Also update dst's U and V plane pointers, so that the */
                    /* output is not padded and the U and V planes immediately */
                    /* follow the Y plane. */

                    dst->yuv_info.u_plane = dst->yuv_info.y_plane
                                            + dst->format.dimensions.width *
                                            dst->format.dimensions.height;

                    dst->yuv_info.v_plane = dst->yuv_info.u_plane
                                            + ((dst->format.dimensions.width*dst->format.dimensions.height) >> 2);
                }

                pDstYPlane = dst->yuv_info.y_plane;
                pDstUPlane = dst->yuv_info.u_plane;
                pDstVPlane = dst->yuv_info.v_plane;
                dstYPitch  = dst->format.yuv_info.y_pitch;
                dstUVPitch = dst->format.yuv_info.u_pitch;
                dstYWidth  = dst->format.rectangle.width;
                dstYHeight = dst->format.rectangle.height;
            }
            else
            {
                RVAssert(pDstBuffer != *pDisplayBuffer);

                ps = DecodedFrame_allocate(pDstBuffer, &(*pDisplayBuffer)->m_lumaSize);
                if (ps != RV_S_OK)
                    break;

                /* Don't lose any existing filtering information */
                DecodedFrame_copyState(pDstBuffer, *pDisplayBuffer);

                pDstYPlane = pDstBuffer->m_pYPlane;
                pDstUPlane = pDstBuffer->m_pUPlane;
                pDstVPlane = pDstBuffer->m_pVPlane;
                dstYPitch  = (*pDisplayBuffer)->m_pitch;
                dstUVPitch = (*pDisplayBuffer)->m_pitch>>1;//(*pDisplayBuffer)->m_pitch;
                dstYWidth  = pDstBuffer->m_lumaSize.width;
                dstYHeight = pDstBuffer->m_lumaSize.height;
            }

            /* Assign QPs for non-coded macroblocks.  In B frames, QPs */
            /* from the reference P frame are used instead. */
            lastQP = ISBFRAME(pDisplayFrame->m_pictureHeader) ?
                     pDisplayFrame->m_pFutureFrame->m_pictureHeader.PQUANT :
                     pDisplayFrame->m_pictureHeader.PQUANT;

            totalMBs = pDisplayFrame->m_macroBlockSize.width
                       * pDisplayFrame->m_macroBlockSize.height;
            for (i = 0; i < totalMBs; i ++)
            {
                if (t->m_pMBInfo[i].mbtype == MBTYPE_SKIPPED)
                {
                    if (ISBFRAME(pDisplayFrame->m_pictureHeader))
                        t->m_pMBInfo[i].QP = t->m_pReferenceMBInfo[i].QP;
                    else
                        t->m_pMBInfo[i].QP = lastQP;
                }
                lastQP = t->m_pMBInfo[i].QP;
            }



            /* Applying RV89COMBO combined smoothing-deblocking post filter */
            ps = SmoothingFilter_FilterYUVFrame(
                     &t->m_smoothingPostFilter,
                     (*pDisplayBuffer)->m_pYPlane,
                     (*pDisplayBuffer)->m_pVPlane,
                     (*pDisplayBuffer)->m_pUPlane,
                     (*pDisplayBuffer)->m_pitch,
                     (*pDisplayBuffer)->m_pitch>>1,//(*pDisplayBuffer)->m_pitch, sheen
                     pDstYPlane,
                     pDstVPlane,
                     pDstUPlane,
                     dstYPitch,
                     dstUVPitch,
                     dstYWidth,
                     dstYHeight,
                     t->m_pMBInfo,
                     FID_I420);

            if (ps == RV_S_DONT_DRAW)
            {
                /* The filter did not do anything */
                /* Do a simple memcpy to the */
                /* destination buffer */

                /* copy luma plane */
                uPitchYSrc = (*pDisplayBuffer)->m_pitch;
                uPitchVSrc = (*pDisplayBuffer)->m_pitch>>1;//(*pDisplayBuffer)->m_pitch;
                pYSrc = (*pDisplayBuffer)->m_pYPlane;
                pUSrc = (*pDisplayBuffer)->m_pUPlane;
                pVSrc = (*pDisplayBuffer)->m_pVPlane;
                pYDst = pDstYPlane;
                pUDst = pDstUPlane;
                pVDst = pDstVPlane;
                for (i = 0; i < dstYHeight; i ++)
                {
                    memcpy(pYDst, pYSrc, dstYWidth); /* Flawfinder: ignore */
                    pYSrc += uPitchYSrc;
                    pYDst += dstYPitch;
                }
                /* copy chroma planes */
                chroma_h = (dstYHeight>>1);
                chroma_w = (dstYWidth>>1);
                for (j = 0; j < chroma_h; j ++)
                {
                    memcpy(pUDst, pUSrc, chroma_w); /* Flawfinder: ignore */
                    memcpy(pVDst, pVSrc, chroma_w); /* Flawfinder: ignore */
                    pUSrc += uPitchVSrc;
                    pUDst += dstUVPitch;
                    pVSrc += uPitchVSrc;
                    pVDst += dstUVPitch;
                }
                ps = RV_S_OK;
            }

            t->m_dSmoothingTime = (float)SmoothingFilter_GetTiming(&t->m_smoothingPostFilter);

            /* The smoothing filter will return RV_S_BAD_FORMAT if the */
            /* destination buffer was not suitably aligned/padded.  This */
            /* should never happen, because our m_postBuffer should always */
            /* be suitable for filtering into, and we called CheckOutput */
            /* if writing to the application's buffer. */
            /* If for some strange reason this error does occur, simply */
            /* return RV_S_OK, and let our caller display what's in */
            /* pDisplayBuffer, without filtering it. */

            if (ps == RV_S_BAD_FORMAT)
            {
                ps = RV_S_OK;
                break;
            }

            if (ps != RV_S_OK)
                break;


            *notes |= RV_DDN_DEBLOCKING_FILTER;

            if (writeToDst)
            {
                *pDisplayBuffer = 0;
                /* Prevents caller from redisplaying it */
            }
            else
            {
                *pDisplayBuffer = pDstBuffer;
                /* Tell caller to display the image in pDstBuffer. */

                /* This filter is both a deblocking and deringing filter. */
                (*pDisplayBuffer)->m_isSmoothed = 1;
                (*pDisplayBuffer)->m_isDeblocked = 1;
            }

        }

        break;
    }


    return ps;
}
#endif

void
Decoder_deallocateParsedData(struct Decoder *t)
{
    if (t->m_pParsedData)
    {
        /* Free the old buffer. */
        RV_Free(t->m_pParsedData);
        t->m_pParsedData = 0;
    }
    t->m_parsedDataLength = 0;
    t->m_paddedParsedDataSize.width = 0;
    t->m_paddedParsedDataSize.height = 0;
    t->m_pMotionVectors = 0;
    t->m_pBMotionVectors = 0;
    t->m_pBidirMotionVectors = 0;
    t->m_pReferenceMotionVectors = 0;
    t->m_pDirBTmpBuf[0] = 0;
    t->m_pDirBTmpBuf[1] = 0;
    t->m_pPredBuf[0] = 0;
    t->m_pPredBuf[1] = 0;
    t->m_pQuantBuf[0] = 0;
    t->m_pQuantBuf[1] = 0;
    t->m_pQuantBufBase[0] = 0;
    t->m_pQuantBufBase[1] = 0;
    t->m_pIsSubBlockEmpty[0] = 0;
    t->m_pIsSubBlockEmpty[1] = 0;
    t->m_pIsSubBlockEmptyBase[0] = 0;
    t->m_pIsSubBlockEmptyBase[1] = 0;
    t->m_pAboveSubBlockIntraTypes[0] = 0;
    t->m_pAboveSubBlockIntraTypes[1] = 0;
    t->m_pMBIntraTypes[0] = 0;
    t->m_pMBIntraTypes[1] = 0;
    t->m_pMBIntraTypesBase[0] = 0;
    t->m_pMBIntraTypesBase[1] = 0;
    t->m_pReferenceMBInfo = 0;
    t->m_pBFrameMBInfo = 0;
    t->m_pMBInfo = 0;
}

RV_Status
Decoder_allocateParsedData(struct Decoder *t, const struct RV_Dimensions *size, RV_Boolean exactSizeRequested)
{
    U32     MB_Frame_Width ;
    U32     MB_Frame_Height  ;
    U32     Sub_Block_Width  ;
    U32     Sub_Block_Height ;
    //U32       quantBufferSize ; sheen
    U32     subblockRowIntraTypeSize;
    U32     macroblockIntraTypeSize ;
    U32     motionVectorSize ;
    U32     MBInfoSize ;
    //U32       dirBTmpBufSize ; sheen
    //U32     uPredBufSize ;    /* 16x16 buffer required for one luma MB */ sheen
    //U32     uSubBlockEmptySize ; sheen
    U32     uNumBatchMB;
    U32     totalSize ;


    RV_Status      ps = RV_S_OK;
    struct RV_Dimensions  desiredPaddedSize;
    U8     *pAlignedParsedData;
    U32     offset;


    desiredPaddedSize.width  = (size->width  + 15) & ~15;
    desiredPaddedSize.height = (size->height + 15) & ~15;

    /* If our buffer and internal pointers are already set up for this */
    /* image size, then there's nothing more to do. */
    /* But if exactSizeRequested, we need to see if our existing */
    /* buffer is oversized, and perhaps reallocate it. */

    if (t->m_paddedParsedDataSize.width == desiredPaddedSize.width &&
            t->m_paddedParsedDataSize.height == desiredPaddedSize.height && !exactSizeRequested)
        return ps;

    /* */
    /* Determine how much space we need */
    /* */

    MB_Frame_Width   = desiredPaddedSize.width >> 4;
    MB_Frame_Height  = desiredPaddedSize.height >> 4;

    Sub_Block_Width  = MB_Frame_Width << 2;
    Sub_Block_Height = MB_Frame_Height << 2;

    //quantBufferSize = NUM_QUANT_BUFFER_ELEMENTS * sizeof(t->m_pQuantBuf[0][0]); sheen

    subblockRowIntraTypeSize = Sub_Block_Width
                               * sizeof(DecoderIntraType);

    macroblockIntraTypeSize = NUM_INTRA_TYPE_ELEMENTS * sizeof(t->m_pMBIntraTypes[0][0]);

    motionVectorSize = Sub_Block_Width * Sub_Block_Height *
                       sizeof(struct DecoderPackedMotionVector);

    MBInfoSize = MB_Frame_Width * MB_Frame_Height * sizeof(struct DecoderMBInfo);

    /* need equivalent of 2*16 rows of the image */
    //dirBTmpBufSize = 32 * (desiredPaddedSize.width + (YUV_Y_PADDING * 2)); sheen

    //uPredBufSize = 256;   /* 16x16 buffer required for one luma MB */ sheen

    //uSubBlockEmptySize = NUM_SB_EMPTY_ELEMENTS * sizeof(t->m_pIsSubBlockEmpty[0][0]); sheen

    /* Add space for multiple MBs for coefficients, empty block flags, */
    /* and intra block type info. */
    uNumBatchMB = MIN(MB_Frame_Width*MB_Frame_Height, MAX_DEC_BATCH_MB);

    //quantBufferSize *= uNumBatchMB; sheen
    macroblockIntraTypeSize *= uNumBatchMB;
    //uSubBlockEmptySize *= uNumBatchMB; sheen

    totalSize = 0
                //2*dirBTmpBufSize      /* needs 16-byte alignment */ sheen
                /* By putting it first, we get YUV_ALIGMENT, which */
                /* is 32 currently. */
                //+ 2*uPredBufSize      /* needs 16-byte alignment */ sheen
                //+ 2*quantBufferSize   /* needs 8-byte alignment */ sheen
                + motionVectorSize
#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION //sheen
                + ((desiredPaddedSize.width*desiredPaddedSize.height>LARGE_DIMENSION_SIZE)? 0 : motionVectorSize)
                + ((desiredPaddedSize.width*desiredPaddedSize.height>LARGE_DIMENSION_SIZE)? 0 : motionVectorSize/4) /* Backward motion vectors for bidir */
#else
                + motionVectorSize
                + motionVectorSize / 4 /* Backward motion vectors for bidir */
#endif
                + 2*macroblockIntraTypeSize
                //+ 2*uSubBlockEmptySize sheen
                + 2*(subblockRowIntraTypeSize + 7)
                /* m_pAboveSubBlockIntraTypes will be 8-byte aligned */
                /* However, subblockRowIntraTypeSize is only */
                /* guaranteed to be a multiple of 4.  So we need */
                /* to align back up to 8, thus the "+7". */
                + MBInfoSize  /* reference frame MB Info */
#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION //sheen
                + ((desiredPaddedSize.width*desiredPaddedSize.height>LARGE_DIMENSION_SIZE)?
                   0 : (7 + (t->m_bLocalDecoder ? 0 : MBInfoSize)))
#else
                + 7 + (t->m_bLocalDecoder ? 0 : MBInfoSize)  /* B frame MB Info */
#endif
                + YUV_ALIGNMENT;

    /* */
    /* Reallocate our buffer if its size is not appropriate. */
    /* */

    if (t->m_parsedDataLength < totalSize
            ||
            (exactSizeRequested && (t->m_parsedDataLength != totalSize))
       )
    {
        Decoder_deallocateParsedData(t);

        t->m_pParsedData = (U8 *)RV_Allocate(totalSize, TRUE);
        if (!t->m_pParsedData)
        {
            RVDebug((RV_TL_ALWAYS,"ERROR Decoder::allocateParsedData"
                     " --  Could not allocate m_pParsedData buffer"));
            return RV_S_OUT_OF_MEMORY;
        }

        t->m_parsedDataLength = totalSize;
    }

    /* Reassign our internal pointers if need be */

    if (t->m_paddedParsedDataSize.width != desiredPaddedSize.width ||
            t->m_paddedParsedDataSize.height != desiredPaddedSize.height)
    {
        t->m_paddedParsedDataSize.width = desiredPaddedSize.width;
        t->m_paddedParsedDataSize.height = desiredPaddedSize.height;

        offset = 0;
        pAlignedParsedData = (U8 *)ALIGN_RV(t->m_pParsedData, YUV_ALIGNMENT);

        /* m_pDirBTmpBuf must be 16-byte aligned, is 32-byte aligned */
        /* sheen
        RVAssert((offset & 0xf) == 0);
        t->m_pDirBTmpBuf[0] = (U8 *)(pAlignedParsedData + offset);
        offset += dirBTmpBufSize;
        RVAssert((offset & 0xf) == 0);
        t->m_pDirBTmpBuf[1] = (U8 *)(pAlignedParsedData + offset);
        offset += dirBTmpBufSize;
        */

        /* m_pPredBuf must be 16-byte aligned, is 32-byte aligned */
        /* sheen
        RVAssert((offset & 0xf) == 0);
        t->m_pPredBuf[0] = (U8 *)(pAlignedParsedData + offset);
        offset += uPredBufSize;
        RVAssert((offset & 0xf) == 0);
        t->m_pPredBuf[1] = (U8 *)(pAlignedParsedData + offset);
        offset += uPredBufSize;
        */

        /* m_pReferenceMotionVectors must be 8-byte aligned, is 32-byte aligned */
        RVAssert((offset & 0x7) == 0);
        t->m_pReferenceMotionVectors =
            (struct DecoderPackedMotionVector *)(pAlignedParsedData + offset);
        offset += motionVectorSize;

#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION //sheen
        if(desiredPaddedSize.width*desiredPaddedSize.height <= LARGE_DIMENSION_SIZE)
#endif
        {
            /* t->m_pBMotionVectors must be 8-byte aligned, is 32-byte aligned */
            RVAssert((offset & 0x7) == 0);
            t->m_pBMotionVectors =
                (struct DecoderPackedMotionVector *)(pAlignedParsedData + offset);
            offset += motionVectorSize;

            /* t->m_pBbidirMotionVectors must be 8-byte aligned, is 32-byte aligned */
            /* These are the backward (pointing to future) motion vectors */
            RVAssert((offset & 0x7) == 0);
            t->m_pBidirMotionVectors =
                (struct DecoderPackedMotionVector *)(pAlignedParsedData + offset);
            offset += (motionVectorSize>>2);  /* only one vector per MB */
        }

        /* t->m_pQuantBuf must be 8-byte aligned, is 32-byte aligned */
        /* sheen
        RVAssert((offset & 0x7) == 0);
        t->m_pQuantBuf[0] = (I32 *)(pAlignedParsedData + offset);
        t->m_pQuantBufBase[0] = t->m_pQuantBuf[0];
        offset += quantBufferSize;
        RVAssert((offset & 0x7) == 0);
        t->m_pQuantBuf[1] = (I32 *)(pAlignedParsedData + offset);
        t->m_pQuantBufBase[1] = t->m_pQuantBuf[1];
        offset += quantBufferSize;
        */

        /* t->m_pMBIntraTypes must be 4-byte aligned, is 16-byte aligned */
        RVAssert((offset & 0x7) == 0);
        t->m_pMBIntraTypes[0] = (DecoderIntraType*)(pAlignedParsedData + offset);
        t->m_pMBIntraTypesBase[0] = t->m_pMBIntraTypes[0];
        offset += macroblockIntraTypeSize;
        RVAssert((offset & 0x7) == 0);
        t->m_pMBIntraTypes[1] = (DecoderIntraType*)(pAlignedParsedData + offset);
        t->m_pMBIntraTypesBase[1] = t->m_pMBIntraTypes[1];
        offset += macroblockIntraTypeSize;

        /* t->m_pIsSubBlockEmpty must be 4-byte aligned, is 16-byte aligned */
        /*
        RVAssert((offset & 0x7) == 0);
        t->m_pIsSubBlockEmpty[0] = (Bool8 *)(pAlignedParsedData + offset);
        t->m_pIsSubBlockEmptyBase[0] = t->m_pIsSubBlockEmpty[0];
        offset += uSubBlockEmptySize;
        RVAssert((offset & 0x7) == 0);
        t->m_pIsSubBlockEmpty[1] = (Bool8 *)(pAlignedParsedData + offset);
        t->m_pIsSubBlockEmptyBase[1] = t->m_pIsSubBlockEmpty[1];
        offset += uSubBlockEmptySize;
        */

        /* t->m_pAboveSubBlockIntraTypes must be 4-byte aligned, is 8-byte aligned */
        RVAssert((offset & 0x7) == 0);
        t->m_pAboveSubBlockIntraTypes[0] =
            (DecoderIntraType*)(pAlignedParsedData + offset);
        offset += subblockRowIntraTypeSize;
        /* align to 4-byte boundary */
        if (offset & 0x7)
            offset = (offset + 7) & ~7;
        RVAssert((offset & 0x7) == 0);
        t->m_pAboveSubBlockIntraTypes[1] =
            (DecoderIntraType*)(pAlignedParsedData + offset);
        offset += subblockRowIntraTypeSize;

        /* align to 8-byte boundary */
        if (offset & 0x7)
            offset = (offset + 7) & ~7;

        RVAssert((offset & 0x7) == 0);
        t->m_pReferenceMBInfo = (struct DecoderMBInfo*)(pAlignedParsedData + offset);
        offset += MBInfoSize;

#ifdef BFRAME_SKIP_IN_LARGE_DIMENSION //sheen
        if(desiredPaddedSize.width*desiredPaddedSize.height <= LARGE_DIMENSION_SIZE)
#endif
        {
            if (t->m_bLocalDecoder)
            {
                t->m_pBFrameMBInfo = 0;
            }
            else
            {
                /* align to 8-byte boundary */
                if (offset & 0x7)
                    offset = (offset + 7) & ~7;
                RVAssert((offset & 0x7) == 0);
                t->m_pBFrameMBInfo = (struct DecoderMBInfo*)(pAlignedParsedData + offset);
                offset += MBInfoSize;
            }
        }

        RVAssert(offset <= totalSize);

        /*memset(pAlignedParsedData,0,offset); */
    }

    return ps;
}


/*//////////////////////////////////////////////////////////////////////////// */
/* depitchPlane */
/* */
/* Support routine for C_YUV12ToYUV. */
/* Copies a pitched plane to a non-pitched buffer. */
/*//////////////////////////////////////////////////////////////////////////// */
#ifdef SHEEN_VC_DEBUG
static void
depitchPlane
(
    U8 *pInput,
    U8 *pOutput,
    U32 width,
    U32 height,
    U32 inputPitch
)
{

    U32 row;
    U8 *pSrc;
    U8 *pDst;

    pSrc = pInput;
    pDst = pOutput;

    for (row = 0; row < height; row++)
    {
        memcpy(pDst, pSrc, width); /* Flawfinder: ignore */
        pSrc += inputPitch;
        pDst += width;
    }
}
#endif

void RV_STDCALL
C_YUV12ToYUV
(
    U8 *pYPlane,
    U8 *pVPlane,
    U8 *pUPlane,
    U32 lumaWidth,
    U32 lumaHeight,
    U32 lumaPitch,
    U32 chromaPitch,
    U32 na0, /* ARC counter, not used */
    U8 *pOut,
    U32 na1, /* pOut pitch, not used.  Assumed to be luma and chroma widths. */
    U32 na2, /* DCI offset.  Not used, assumed to be 0. */
    I32 na3,  /* offset to line 0.  Not used, assumed to be 0. */
    I32 na4 /* color convertor id, not used. */
)
{
    U32 chromaWidth, chromaHeight;
    U32 *pTemp;//sheen
    //U32 row;
    //U8 *pSrc, *pDst, *pDst2;
    U32 chromaPitch2;//for output uv stride
    chromaWidth  = lumaWidth  >> 1;
    chromaHeight = lumaHeight >> 1;
    chromaPitch2=chromaWidth + YUV_UV_PADDING*2;

    pTemp=(U32*)pOut;
    pTemp[0]=(U32)pYPlane;//use the decoder Y
    //pTemp[1]=(U32)(pOut+4*5);//u
    pTemp[1]=(U32)pUPlane;//u
    //pTemp[2]=(U32)(pOut+4*5 + chromaPitch2*chromaHeight);//v
    pTemp[2]=(U32)pVPlane;

    pTemp[3]=lumaPitch;
    pTemp[4]=lumaHeight;

    return;

#if 0
    depitchPlane(pYPlane, pOut, lumaWidth, lumaHeight, lumaPitch);

    pOut += lumaWidth * lumaHeight;
    depitchPlane(pUPlane, pOut, chromaWidth, chromaHeight, chromaPitch);

    pOut += chromaWidth * chromaHeight;
    depitchPlane(pVPlane, pOut, chromaWidth, chromaHeight, chromaPitch);
#else
    /*
        //uv
        pSrc=pUPlane;
        pDst=(U8*)pTemp[1];
        pDst2=(U8*)pTemp[2];
        for (row = 0; row < chromaHeight; row++)
        {
            //u
            memcpy(pDst, pSrc, chromaWidth);
            pSrc += chromaPitch2;
            //v
            memcpy(pDst2, pSrc, chromaWidth);
            pSrc += chromaPitch2;

            pDst += chromaPitch2;
            pDst2 += chromaPitch2;
        }
    */
#endif
}


void
Decoder_getBackdoorOptions(struct Decoder *t)
{
    /*#if defined(_WIN32) && !defined(_WIN32_WCE) */
#if 0

    U32 unResult;

#define SECTION_NAME    TEXT("Decode")

    /* Read the options from the INI file */
    RVDebug((RV_TL_INIT,
             "Decoder::getBackdoorOptions -- Reading backdoor decoder options"));

    /* Smoothing post filter */
    unResult = GetPrivateProfileInt(SECTION_NAME, TEXT("SmoothingPostFilter"), 2, INI_FILE_NAME);
    t->m_bIniSmoothingPostFilter = ((2 == unResult) ? FALSE : TRUE);
    t->m_options.SmoothingPostFilter =
        (t->m_bIniSmoothingPostFilter ? ((1 == unResult) ? TRUE : FALSE) : FALSE);

    /* Smoothing post filter strength */
    unResult = GetPrivateProfileInt(SECTION_NAME, TEXT("SmoothingStrength"),
                                    RV_Maximum_Smoothing_Strength+1, INI_FILE_NAME);
    t->m_bIniSmoothingStrength = (((RV_Maximum_Smoothing_Strength+1) == unResult) ? FALSE : TRUE);
    if (t->m_bIniSmoothingStrength &&
            (unResult >= 0)         &&
            (unResult <= RV_Maximum_Smoothing_Strength))
    {
        SmoothingFilter_SetStrength(&t->m_smoothingPostFilter, unResult);
    }

    /* Error concealment */
    unResult = GetPrivateProfileInt(SECTION_NAME, TEXT("ErrorConcealment"), 2, INI_FILE_NAME);
    t->m_bIniLossConcealment = ((2 == unResult) ? FALSE : TRUE);
    t->m_options.lossConcealment = (t->m_bIniLossConcealment ? ((1 == unResult) ? TRUE : FALSE) : TRUE);

    /* Introduce latency to wait for highest layer and not to skip first B frames. */
    unResult = GetPrivateProfileInt(SECTION_NAME, TEXT("IntroduceLatency"), 2, INI_FILE_NAME);
    t->m_bIniIntroduceLatency = ((2 == unResult) ? FALSE : TRUE);
    t->m_options.IntroduceLatency = (t->m_bIniIntroduceLatency ? ((1 == unResult) ? TRUE : FALSE) : FALSE);

    /* INFODISPLAY */
    unResult = GetPrivateProfileInt(SECTION_NAME, TEXT("EnableInfoDisplay"), 0, INI_FILE_NAME);
    t->m_bIniInfoDisplay = (unResult != 0);


#if defined(DEBUG)
    {
        if (t->m_bIniSmoothingPostFilter)
        {
            RVDebug((RV_TL_INIT,"Decoder::getBackdoorOptions: "
                     "Smoothing postfilter %s",
                     t->m_options.SmoothingPostFilter ? "ON" : "OFF"));
        }

        if (t->m_bIniSmoothingStrength)
        {
            RVDebug((RV_TL_INIT,"Decoder::getBackdoorOptions: "
                     "Smoothing strength %d",
                     SmoothingFilter_GetStrength(&t->m_smoothingPostFilter)));
        }

        if (t->m_bIniLossConcealment)
        {
            RVDebug((RV_TL_INIT,"Decoder::getBackdoorOptions: "
                     "Error concealment %s",
                     (0 == t->m_options.lossConcealment) ? "OFF" : "ON"));
        }

        if (t->m_bIniIntroduceLatency)
        {
            RVDebug((RV_TL_INIT,"Decoder::getBackdoorOptions: "
                     "Latency %s",
                     (0 == t->m_options.IntroduceLatency) ? "OFF" : "ON"));
        }

        if (t->m_bIniInfoDisplay)
        {
            RVDebug((RV_TL_INIT,
                     "Decoder::getBackdoorOptions: INFODISPLAY ON"));
        }
    }
#endif /* DEBUG */
#endif /* defined(_WIN32) */
}

/* This constructor simply gives each member a default value. */
void  Global_Decoder_Options_Init(struct Global_Decoder_Options *t)
{
    t->SmoothingPostFilter          = FALSE;
    t->IntroduceLatency             = FALSE;
    t->CPU_Scalability_Setting      = CPU_Scalability_Default;
    t->lossConcealment              = TRUE;
    t->FrameRateUpsampling          = FALSE;
    t->Num_RPR_Sizes                 = 0;
#ifdef INTERLACED_CODE
    t->Interlaced                   = FALSE;
#endif

    t->m_Reserved1                  = 0;
}

RV_Boolean Global_Decoder_Options_isSmoothingRequested(struct Global_Decoder_Options *t)
{
    return t->SmoothingPostFilter
           || t->CPU_Scalability_Setting >=
           CPU_Scalability_UseSmoothingPostFilter;
}






