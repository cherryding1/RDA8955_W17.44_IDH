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





#include "cs_types.h"
// common/include
#include "gsm.h"
#include "baseband_defs.h"
// spal public headers
#include "spal_xcor.h"
#include "spal_cap.h"
#include "spal_itlv.h"
#include "spal_debug.h"
#include "spal_mem.h"
#include "spal_irq.h"
#include "spal_comregs.h"
#include "spal_tcu.h"
#if (SPC_IF_VER >= 4)
#include "spal_cordic.h"
#endif
// spp public headers
#include "spp_gsm.h"
#include "spp_profile_codes.h"
// spp private headers
#include "sppp_gsm_private.h"
#include "sppp_cfg.h"
#include "sppp_debug.h"
// spc public headers
#include "spc_ctx.h"

#ifdef __MBOX_DEBUG__ // DEBUG STUB
#include "spc_mailbox.h"
#include "global_macros.h"
#endif

extern VOID spal_InvalidateDcache(VOID);
//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

PRIVATE SPP_FCCH_PARAM_T g_sppFcchParam SPAL_BBSRAM_GLOBAL;
PRIVATE UINT16 g_sppFcchBurstNb  SPAL_BBSRAM_GLOBAL = 0;
PRIVATE UINT32* g_sppPtrNew SPAL_BBSRAM_GLOBAL;


//------------------------------------------------------------------------------
// Define
//------------------------------------------------------------------------------
#if (SPC_IF_VER >= 4)
#define ANGLE_USE_CORDIC
#define  CORDIC_ANGLE_LIMIT 0x6000
#endif


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//======================================================================
// FCCH Detection function
//----------------------------------------------------------------------
// This function implements FCCH detection over large reception windows
// (usually 9 Time Slots in @b sliced mode or 12 TDMA frames in @b Full
// mode). For both modes, the reception window must be sliced in sub
// parts. The function must be called for each sub part of the window
// and will return an FCCH detection status each time. See FCCH
// detection memory requirements.
// @param RxBuffer UINT32*. Start address of the reception buffer.
// @param Init BOOL. Takes following values:
// - TRUE : first call for this FCCH window,
// - FALSE: subsequent calls for a same FCCH window.
// .
// @param Mode SPP_FCCH_MODE_T. Takes following values:
// - #FCCH_SLICED for detection during dedicated mode,
// - #FCCH_FULL for initial cell syncronization process.
// .
// @param DelayType SPP_FCCH_DELAY_TYPE_T:
// -  #FCCH_DELAY2 for traditional delay 2 correlation
// -  #FCCH_DELAY4 for new updated delay 4 correlation
// @param Res SPP_FCCH_RESULT_T*. Pointer to the FCCH Result structure.
//
// @return Takes following values:
// - #FCCH_FOUND if fcch has been detected (results valid),
// - #FCCH_NOT_FOUND if no fcch has been detected,
// - #FCCH_INVALID if fcch has been detected but conditions
//   of detection (window boundaries, suspect frequency error
//   ...) are imposing to discard the detection results.
// .
//======================================================================
PUBLIC SPP_FCCH_RETURN_T spp_SearchFcch(UINT32* RxBuffer,
                                        BOOL Init,
                                        SPP_FCCH_MODE_T Mode,
                                        SPP_FCCH_DELAY_TYPE_T DelayType,
                                        SPP_FCCH_RESULT_T* Res)
{
    // FCCH Detection Process uses 3 floating windows (one for Power, one
    // for I-part correlation and one for Q-part correlation)
    //
    // Each window is divided in 3 parts of SPP_FCCH_SIZE symbols, one part
    // Wx is processed at every call to this function(x being 0->1->2->0->1...).
    //
    //   ----------------------------------------------------------------
    //   |      |      |      |      |      |      |      |      |      |
    //   | W0   |  W1  |  W2  |  W0  |  W1  |  W2  |  W0  |  W1  |  W2  |
    //   |      |      |      |      |      |      |      |      |      |
    //   ---------------------|--------------------|--------------------|
    //   |      Power         |         CI         |        CQ          |
    //   |                    |
    //   |< 3 * SPP_FCCH_SIZE >|
    //   |
    //   |
    //   *RxBuffer

    UINT32* Ptr_old;
    SPAL_DC_OFFSET_T Dco;
    BOOL fcchDetected=FALSE;
    VOID (*spal_XcorFchxc_func)
    ( UINT32* symb_ptr,
      UINT32* old_ptr,
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
      UINT32* out_ptr,
      INT32   accu_energy,
      INT32   accu_corrI,
      INT32   accu_corrQ,
#endif // (CHIP_XCOR_HAS_SLIDING_WINDOW)
      UINT16 size,
      UINT16 computeEnergy);

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
    UINT32* accu_outBuf =  (!g_sppGlobalParams.fcch_hw_sliding_window)? NULL :
                           (g_sppGlobalParams.fcch_sliding_energy)?
                           (UINT32*)(MEM_ACCESS_CACHED(RxBuffer) + 9*SPP_FCCH_SIZE):
                           (UINT32*)(MEM_ACCESS_CACHED(RxBuffer) + 6*SPP_FCCH_SIZE);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

#if (SPC_IF_VER < 4)
    spal_XcorFchxc_func = (spal_XcorFchxc);
#else
    spal_XcorFchxc_func = (DelayType == FCCH_DELAY2)? (spal_XcorFchxc) : (spal_XcorFchxc_Delay);
#endif

    if (Init) // First call for this FCCH search
    {
        UINT16 i;
        register UINT32* temp_ptr;
        register UINT32 temp_accu = 0;

        // Init pointers
        g_sppPtrNew = MEM_ACCESS_CACHED(RxBuffer);

        // point to CI
        temp_ptr = (g_sppGlobalParams.fcch_sliding_energy) ? g_sppPtrNew + 3*SPP_FCCH_SIZE : g_sppPtrNew;


        // Initialize globals
        g_sppFcchBurstNb = 1;
        g_sppFcchParam.FCCH_detected=0;
        g_sppFcchParam.Acc_Corr_I   =0;
        g_sppFcchParam.Acc_Corr_Q   =0;
        g_sppFcchParam.Acc_Energy   =0;
        g_sppFcchParam.samples_left =0;
        g_sppFcchParam.Max_Abs_Corr =0;
        g_sppFcchParam.Corr_order_I =0;
        g_sppFcchParam.Corr_order_Q =0;
        g_sppFcchParam.max_index    =0;
        g_sppFcchParam.counter_high =0;
        g_sppFcchParam.counter_low  =0;
        g_sppFcchParam.lastIT       =0;


        // Run DCOC
        g_sppFcchParam.Acc_Energy =
            spal_XcorDcoc((UINT32*)MEM_ACCESS_CACHED(RxBuffer),
                          SPP_FCCH_SIZE,
                          FALSE, // No derotation
                          BYPASS_DCOC, // no bypass dcoc,
                          &Dco);

#if (SPC_IF_VER >= 4)
        spal_InvalidateDcache();
#endif

        // Run XCOR in FCCH mode to generate Power, CI and CQ terms
        // This function flushes the Dcache
        spal_XcorFchxc_func( MEM_ACCESS_CACHED(RxBuffer),
                             NULL,                                   // init
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                             NULL,                                   // output
                             0,                                   // accu_Energy
                             0,                                   // accu_CorrI
                             0,                                   // accu_CorrQ
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)
                             SPP_FCCH_SIZE,                          // size
                             g_sppGlobalParams.fcch_sliding_energy); // compute energy


        // Calculate initial accumulated CI and CQ
        temp_accu = 0;
        for(i=0; i<SPP_FCCH_SIZE; i++)
            temp_accu += *(temp_ptr++);
        g_sppFcchParam.Acc_Corr_I = temp_accu;
        temp_ptr += 2*SPP_FCCH_SIZE; // point to CQ
        temp_accu = 0;
        for(i=0; i<SPP_FCCH_SIZE; i++)
            temp_accu += *(temp_ptr++);
        g_sppFcchParam.Acc_Corr_Q = temp_accu;

        // No detection the first time
        return FCCH_NOT_FOUND;
    }

    // Update pointers
    Ptr_old = g_sppPtrNew;
    g_sppPtrNew += SPP_FCCH_SIZE;
    g_sppPtrNew = (g_sppPtrNew==MEM_ACCESS_CACHED(RxBuffer+3*SPP_FCCH_SIZE))?
                  MEM_ACCESS_CACHED(RxBuffer):
                  g_sppPtrNew;

#if (SPC_IF_VER >= 4)
    if(g_sppGlobalParams.fcch_sliding_energy)
    {
        spal_XcorDcoc(  (UINT32*)g_sppPtrNew,
                        SPP_FCCH_SIZE,
                        FALSE, // No derotation
                        BYPASS_DCOC, // do not bypass dcoc,
                        &Dco);
    }
    else
#endif
    {
        g_sppFcchParam.Acc_Energy =
            spal_XcorDcoc(  (UINT32*)g_sppPtrNew,
                            SPP_FCCH_SIZE,
                            FALSE, // No derotation
                            BYPASS_DCOC, // do not bypass dcoc,
                            &Dco);
    }

#if (SPC_IF_VER >= 4)
    spal_InvalidateDcache();
#endif

    // Run XCOR in FCCH mode to generate Power, CI and CQ terms
    // This function flushes the Dcache
    spal_XcorFchxc_func( g_sppPtrNew,
                         Ptr_old,                                // init
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                         accu_outBuf,                           // output
                         g_sppFcchParam.Acc_Energy,             //
                         g_sppFcchParam.Acc_Corr_I,             //
                         g_sppFcchParam.Acc_Corr_Q,             //
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)
                         SPP_FCCH_SIZE,                          // size
                         g_sppGlobalParams.fcch_sliding_energy); // compute energy

    // Prevents the search mechanism to wait for a next IT
    // when we are in the last search window (slice mode only).
    if ((Mode == FCCH_SLICED) && (g_sppFcchBurstNb==(SPP_FCCH_NB_WIN-1)))
    {
        g_sppFcchParam.lastIT = TRUE;
    }

    SPP_PROFILE_FUNCTION_ENTRY(spp_FcchDetect);

    fcchDetected = (g_sppGlobalParams.fcch_detect_func)(
                       Ptr_old,
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                       (g_sppGlobalParams.fcch_hw_sliding_window)? accu_outBuf:g_sppPtrNew,
#else //(!CHIP_XCOR_HAS_SLIDING_WINDOW)
                       g_sppPtrNew,
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)
                       &g_sppFcchParam,Res);


    SPP_PROFILE_FUNCTION_EXIT(spp_FcchDetect);

    if (fcchDetected)
    {
#define SYMB_MARGIN 11
#define CI_PTR_OFFSET ((g_sppGlobalParams.fcch_sliding_energy) ? 1*3*SPP_FCCH_SIZE : 0*3*SPP_FCCH_SIZE)
#define CQ_PTR_OFFSET ((g_sppGlobalParams.fcch_sliding_energy) ? 2*3*SPP_FCCH_SIZE : 1*3*SPP_FCCH_SIZE)
        INT32 register temp_CI;
        INT32 register temp_CQ;
        INT32* temp_ptr;
        INT32* fcch_start;
        INT32* fcch_end;
        int start_cnt;                    //GLTODO Should it be INT8, INT16 or INT32 ??
        int end_cnt;                      //GLTODO Should it be INT8, INT16 or INT32 ??
        int cnt;                          //GLTODO Should it be INT8, INT16 or INT32 ??

        // DETECTION FILTERING
        // -------------------
        // Discard detections too close from the window's border
        if (((g_sppFcchBurstNb == 1) && // window's beginning
                (Res->TOf <= (BB_BURST_ACTIVE_SIZE-SPP_FCCH_SIZE)))
                ||
                ( g_sppFcchParam.lastIT && // End of sliced mode window
                  (Res->TOf >= (2*SPP_FCCH_SIZE-BB_BURST_ACTIVE_SIZE)))
           )
        {
            return FCCH_INVALID;
        }

        // FOF CALCULATION
        // ---------------
        // The FOf estimation window must be reduced to avoid taking "wrong"
        // symbols into account in the FOf calculation
        fcch_start = Ptr_old + Res->TOf;
        fcch_end = g_sppPtrNew + Res->TOf - 1;

        if (Res->TOf < 0)
        {
            end_cnt =(SPP_FCCH_SIZE - SPP_ABS(Res->TOf) - SYMB_MARGIN);
            start_cnt = (SPP_ABS(Res->TOf) - SYMB_MARGIN);

            if (SPP_ABS(Res->TOf) < SYMB_MARGIN)
                end_cnt = SPP_FCCH_SIZE-(2*SYMB_MARGIN);

            if (g_sppPtrNew==MEM_ACCESS_CACHED(RxBuffer))
                fcch_end += 3*SPP_FCCH_SIZE;
            if (Ptr_old==MEM_ACCESS_CACHED(RxBuffer))
                fcch_start += 3*SPP_FCCH_SIZE;
        }
        else
        {
            start_cnt =(SPP_FCCH_SIZE - (Res->TOf) - SYMB_MARGIN);
            end_cnt = ((Res->TOf) - SYMB_MARGIN);

            if ((Res->TOf) < SYMB_MARGIN)
                start_cnt = SPP_FCCH_SIZE-(2*SYMB_MARGIN);
        }

        temp_CI = temp_CQ = 0;

        temp_ptr= (INT32*)(CI_PTR_OFFSET + fcch_start + SYMB_MARGIN);
        for(cnt=0; cnt<start_cnt; cnt++)
            temp_CI += *(temp_ptr++);
        temp_ptr= (INT32*)(CQ_PTR_OFFSET + fcch_start + SYMB_MARGIN);
        for(cnt=0; cnt<start_cnt; cnt++)
            temp_CQ += *(temp_ptr++);

        temp_ptr= (INT32*)(CI_PTR_OFFSET + fcch_end - SYMB_MARGIN);
        for(cnt=0; cnt<end_cnt; cnt++)
            temp_CI += *(temp_ptr--);
        temp_ptr= (INT32*)(CQ_PTR_OFFSET + fcch_end - SYMB_MARGIN);
        for(cnt=0; cnt<end_cnt; cnt++)
            temp_CQ += *(temp_ptr--);
        // Fof calculation
#ifndef ANGLE_USE_CORDIC
#if (SPC_IF_VER < 4)
        temp_CI /= 0x5430; //  CI / (270833/(4*PI))
#else
        temp_CI /= ((DelayType == FCCH_DELAY2) ? 0x5430 : 0x2A18); // CI / (270833/(4*PI))   : CI / (270833/(8*PI))
#endif
#endif
        if (temp_CI == 0)
        {
            return FCCH_INVALID;
        }
        else
        {
#ifdef ANGLE_USE_CORDIC
            INT32 tempFof;

            // Delay2: atan(-x,-y)/(4*PI*(1/Fs)) ---> [angle(-x,-y)/PI*0x8000]  * (135417/2^16)
            // Delay4: atan(y/x)/(8*PI*(1/Fs)) ---> [angle(x,y)/PI*0x8000]  * (67708/2^16)

            temp_CI = (DelayType == FCCH_DELAY2) ? (-temp_CI): temp_CI;
            temp_CQ = (DelayType == FCCH_DELAY2) ? (-temp_CQ): temp_CQ;

            spal_CordicAngle(temp_CI, temp_CQ, &(Res->FOf));

            //when use cordic. +-pi is bound during average. but even if the result is near bound and
            // ambiguidty occors, abs(fof) result is always very large and can be
            // neglected . eg: 33.8KHz --> 1/2PI for Delay2 and 17KHz ---> 1/2PI for Delay4
            // CORDIC_ANGLE_LIMIT/0x8000 *PI is the limit

            if ( (Res->FOf > CORDIC_ANGLE_LIMIT) || (Res->FOf <-CORDIC_ANGLE_LIMIT))
            {
                return FCCH_INVALID;
            }

            tempFof = (INT32) (Res->FOf*((DelayType == FCCH_DELAY2) ? 135417 : 67708));
            Res->FOf = (INT16)(tempFof >> 16);
#else
            UINT32 algo_error;
            INT32 valid_thres;
            UINT32 reduced_Acc_Energy = (g_sppFcchParam.Acc_Energy >> 15);

            // FOf calculation
            Res->FOf = (INT16)(temp_CQ / temp_CI);

            // This is to reduce arctan approx error for large angles
            algo_error = (Res->FOf * Res->FOf) >> 17; // ~ 8E-6 x FOf^2
            Res->FOf += ((Res->FOf >0)? (-((INT16)algo_error)):(((INT16)algo_error)));

            // Validity threshold : avoid "wrapped out" FOf to be detected
            // This happens when Frequency error exceed +/- 28kHz
            //NOTE: 28KHz offset : pi/2. bound for y/x function.
            temp_CI = (temp_CI > 0) ? temp_CI : (-temp_CI);
            reduced_Acc_Energy = reduced_Acc_Energy?reduced_Acc_Energy:1;
            valid_thres = (temp_CI << 3) / reduced_Acc_Energy;

            // Discard FOf detection if we are out of validity bounds
            if (valid_thres < 5)
            {
                return FCCH_INVALID;
            }
#endif

            // Saturate FOf returned for large results
            Res->FOf = (Res->FOf > 13000) ? 13000 : Res->FOf;
            Res->FOf = (Res->FOf <-13000) ?-13000 : Res->FOf;
        }

        // TOF CALCULATION
        // ---------------
        // Calculate overall offset since the window's start
        Res->TOf = (INT16) ((g_sppFcchBurstNb-1)*SPP_FCCH_SIZE // passed bursts
                            + (INT16) Res->TOf // offset found
                            - ((BB_BURST_ACTIVE_SIZE-SPP_FCCH_SIZE)/2 - 1)); // win bias
        return FCCH_FOUND;
    }

    g_sppFcchBurstNb++;

    return FCCH_NOT_FOUND;

}



#if (SPC_IF_VER >= 4)
//======================================================================
// FCCH Detection function new algo with down sampling and LP filter.. Total (9+3)*142 UINT32 Needed
//----------------------------------------------------------------------
// This function implements FCCH detection over large reception windows
// (usually 9 Time Slots in @b sliced mode or 12 TDMA frames in @b Full
// mode). For both modes, the reception window must be sliced in sub
// parts. The function must be called for each sub part of the window
// and will return an FCCH detection status each time. See FCCH
// detection memory requirements.
// @param RxBuffer UINT32*. Start address of the reception buffer.
// @param Init BOOL. Takes following values:
// - TRUE : first call for this FCCH window,
// - FALSE: subsequent calls for a same FCCH window.
// .
// @param Mode SPP_FCCH_MODE_T. Takes following values:
// - #FCCH_SLICED for detection during dedicated mode,
// - #FCCH_FULL for initial cell syncronization process.
// .
// @param Res SPP_FCCH_RESULT_T*. Pointer to the FCCH Result structure.
//
// @return Takes following values:
// - #FCCH_FOUND if fcch has been detected (results valid),
// - #FCCH_NOT_FOUND if no fcch has been detected,
// - #FCCH_INVALID if fcch has been detected but conditions
//   of detection (window boundaries, suspect frequency error
//   ...) are imposing to discard the detection results.
// .
//======================================================================
PUBLIC SPP_FCCH_RETURN_T  spp_SearchFcchFilter(UINT32* RxBuffer, BOOL Init, SPP_FCCH_MODE_T Mode,
        SPP_FCCH_RESULT_T* Res)
{
    // FCCH Detection Process uses 3 floating windows (one for Power, one
    // for I-part correlation and one for Q-part correlation)
    //
    // Each window is divided in 3 parts of SPP_FCCH_SIZE symbols, one part
    // Wx is processed at every call to this function(x being 0->1->2->0->1...).
    //
    //   ----------------------------------------------------------------------------------------------------------
    //   |      |      |      |      |      |      |      |      |      |      |      |      |      |      |      |
    //   | W0   |  W1  |  W2  |  W0  |  W1  |  W2  |  W0  |  W1  |  W2  |  W0  |  W1  |  W2  |  W0  |  W1  |  W2  |
    //   |      |      |      |      |      |      |      |      |      |      |      |      |      |      |      |
    //   ---------------------|--------------------|--------------------|--------------------|--------------------|
    //   |     rx Power       |     filter out     |   filter out's pwr | rx Power's MA      |filter out's pwr MA |
    //   |                    |
    //   |< 3 * SPP_FCCH_SIZE >|
    //   |
    //   |
    //   *RxBuffer

    UINT32* Ptr_old;
    SPAL_DC_OFFSET_T Dco;
    BOOL fcchDetected=FALSE;
    UINT32 *accu_outBuf = (UINT32*)(MEM_ACCESS_CACHED(RxBuffer) + 9*SPP_FCCH_SIZE);

    if (Init) // First call for this FCCH search
    {
        UINT16 i;
        register UINT32* temp_ptr;
        register UINT32 temp_accu = 0;

        // Init pointers
        g_sppPtrNew = MEM_ACCESS_CACHED(RxBuffer);


        // Initialize globals
        g_sppFcchBurstNb = 1;
        g_sppFcchParam.FCCH_detected=0;
        g_sppFcchParam.Acc_Corr_I   =0;
        g_sppFcchParam.Acc_Corr_Q   =0;
        g_sppFcchParam.Acc_Energy   =0;
        g_sppFcchParam.samples_left =0;
        g_sppFcchParam.Max_Abs_Corr =0;
        g_sppFcchParam.Corr_order_I =0;
        g_sppFcchParam.Corr_order_Q =0;
        g_sppFcchParam.max_index    =0;
        g_sppFcchParam.counter_high =0;
        g_sppFcchParam.counter_low  =0;
        g_sppFcchParam.lastIT       =0;

        // Run DCOC
        g_sppFcchParam.Acc_Energy =
            spal_XcorDcoc((UINT32*)MEM_ACCESS_CACHED(RxBuffer),
                          SPP_FCCH_SIZE,
                          FALSE, // No derotation
                          BYPASS_DCOC, // no bypass dcoc,
                          &Dco);



        // filter input data
        // This function flushes the Dcache
        spal_XcorFchFilter(NULL, MEM_ACCESS_CACHED(RxBuffer), 0, NULL, 0, 0);

        g_sppFcchParam.Acc_Corr_I = 0; // I part is of no use in this patch.
        // Calculate initial accumulated  Filter Output Pwr
        temp_ptr = g_sppPtrNew + 6*SPP_FCCH_SIZE ;
        temp_accu = 0;
        for(i=0; i<SPP_FCCH_SIZE; i++)
            temp_accu += *(temp_ptr++);
        g_sppFcchParam.Acc_Corr_Q = temp_accu;

        // No detection the first time
        return FCCH_NOT_FOUND;
    }// END INIT

    // Update pointers
    Ptr_old = g_sppPtrNew;
    g_sppPtrNew += SPP_FCCH_SIZE;
    g_sppPtrNew = (g_sppPtrNew==MEM_ACCESS_CACHED(RxBuffer+3*SPP_FCCH_SIZE))?
                  MEM_ACCESS_CACHED(RxBuffer):
                  g_sppPtrNew;



//    g_sppFcchParam.Acc_Energy =
    spal_XcorDcoc(  (UINT32*)g_sppPtrNew,
                    SPP_FCCH_SIZE,
                    FALSE, // No derotation
                    BYPASS_DCOC, // do not bypass dcoc,
                    &Dco);
// filter input data
    // This function flushes the Dcache
    spal_XcorFchFilter(Ptr_old, g_sppPtrNew, g_sppFcchBurstNb&0x1, accu_outBuf,
                       g_sppFcchParam.Acc_Energy, g_sppFcchParam.Acc_Corr_Q);

    // Prevents the search mechanism to wait for a next IT
    // when we are in the last search window (slice mode only).
    if ((Mode == FCCH_SLICED) && (g_sppFcchBurstNb==(SPP_FCCH_NB_WIN-1)))
    {
        g_sppFcchParam.lastIT = TRUE;
    }

    fcchDetected = spp_FcchDetectFilterAsm(Ptr_old,accu_outBuf,&g_sppFcchParam,Res);

    if (fcchDetected)
    {
#define SYMB_MARGIN 11
#define CI_PTR_OFFSET2 SPP_FCCH_SIZE
#define CQ_PTR_OFFSET2 2*SPP_FCCH_SIZE
#define PRE_OFFSET 30
        INT32 register temp_CI;
        INT32 register temp_CQ;
        INT32* temp_ptr;
        INT32* fcch_start;
        INT32* fcch_end;
        int start_cnt;                    //GLTODO Should it be INT8, INT16 or INT32 ??
        int end_cnt;                      //GLTODO Should it be INT8, INT16 or INT32 ??
        int cnt;                          //GLTODO Should it be INT8, INT16 or INT32 ??
        INT16 fcch_start_len, fcch_end_len;


        Res->TOf -= 4;// tcj added for new algo

        // DETECTION FILTERING
        // -------------------
        // Discard detections too close from the window's border
        if (((g_sppFcchBurstNb == 1) && // window's beginning
                (Res->TOf <= (BB_BURST_ACTIVE_SIZE-SPP_FCCH_SIZE)))
                ||
                ( g_sppFcchParam.lastIT && // End of sliced mode window
                  (Res->TOf >= (2*SPP_FCCH_SIZE-BB_BURST_ACTIVE_SIZE)))
           )
        {
            return FCCH_INVALID;
        }

        // FOF CALCULATION
        // ---------------
        // The FOf estimation window must be reduced to avoid taking "wrong"
        // symbols into account in the FOf calculation
        fcch_start = Ptr_old + Res->TOf;
        fcch_end = g_sppPtrNew + Res->TOf - 1;

        if (Res->TOf < 0)
        {
            end_cnt =(SPP_FCCH_SIZE - SPP_ABS(Res->TOf) - SYMB_MARGIN);
            start_cnt = (SPP_ABS(Res->TOf) - SYMB_MARGIN);

            if (SPP_ABS(Res->TOf) < SYMB_MARGIN)
                end_cnt = SPP_FCCH_SIZE-(2*SYMB_MARGIN) - PRE_OFFSET;

            if (g_sppPtrNew==MEM_ACCESS_CACHED(RxBuffer))
                fcch_end += 3*SPP_FCCH_SIZE;
            if (Ptr_old==MEM_ACCESS_CACHED(RxBuffer))
                fcch_start += 3*SPP_FCCH_SIZE;

            fcch_start_len = SPP_ABS(Res->TOf);
            fcch_end_len = SPP_FCCH_SIZE - SPP_ABS(Res->TOf);
        }
        else
        {
            start_cnt =(SPP_FCCH_SIZE - (Res->TOf) - SYMB_MARGIN);
            end_cnt = ((Res->TOf) - SYMB_MARGIN);

            if ((Res->TOf) < SYMB_MARGIN)
                start_cnt = SPP_FCCH_SIZE-(2*SYMB_MARGIN) - PRE_OFFSET;

            fcch_start_len = SPP_FCCH_SIZE - SPP_ABS(Res->TOf);
            fcch_end_len = SPP_ABS(Res->TOf);
        }

        // run XCOR in FCCH mode. Perform complex auto-correlation(delay 2) on
        // IIR Filter output preparing for fof calculation.
        // This function flushes the Dcache
        spal_XcorFchxc_AutoCorr( fcch_start+3*SPP_FCCH_SIZE,
                                 SPP_FCCH_SIZE, // offset
                                 fcch_start_len);
        spal_XcorFchxc_AutoCorr( fcch_end+3*SPP_FCCH_SIZE-fcch_end_len+1,
                                 SPP_FCCH_SIZE, // offset
                                 fcch_end_len);

        temp_CI = temp_CQ = 0;

        temp_ptr= (INT32*)(3*SPP_FCCH_SIZE+CI_PTR_OFFSET2 + fcch_start + SYMB_MARGIN+PRE_OFFSET);
        for(cnt=PRE_OFFSET; cnt<start_cnt; cnt++)
            temp_CI += *(temp_ptr++);
        temp_ptr= (INT32*)(3*SPP_FCCH_SIZE+CQ_PTR_OFFSET2 + fcch_start + SYMB_MARGIN+PRE_OFFSET);
        for(cnt=PRE_OFFSET; cnt<start_cnt; cnt++)
            temp_CQ += *(temp_ptr++);

        temp_ptr= (INT32*)(3*SPP_FCCH_SIZE+CI_PTR_OFFSET2 + fcch_end - SYMB_MARGIN);
        for(cnt=0; cnt<end_cnt; cnt++)
            temp_CI += *(temp_ptr--);
        temp_ptr= (INT32*)(3*SPP_FCCH_SIZE+CQ_PTR_OFFSET2 + fcch_end - SYMB_MARGIN);
        for(cnt=0; cnt<end_cnt; cnt++)
            temp_CQ += *(temp_ptr--);
        // Fof calculation
#ifndef ANGLE_USE_CORDIC
        temp_CI = temp_CI / 0x5430; //  CI / (270833/(4*PI))
#endif
        if (temp_CI == 0)
        {
            return FCCH_INVALID;
        }
        else
        {
#ifdef ANGLE_USE_CORDIC
            INT32 tempFof;

            // Delay2: atan(x,y)/(4*PI*(1/Fs)) ---> [angle(x,y)/PI*0x8000]  * (135417/2^16)

            spal_CordicAngle(temp_CI, temp_CQ, &(Res->FOf));

            //when use cordic. +-pi is bound during average. but even if the result is near bound and
            // ambiguidty occors, abs(fof) result is always very large and can be
            // neglected . eg: 33.8KHz --> 1/2PI for Delay2 and 17KHz ---> 1/2PI for Delay4
            // CORDIC_ANGLE_LIMIT/0x8000 *PI is the limit

            if ( (Res->FOf > CORDIC_ANGLE_LIMIT) || (Res->FOf <-CORDIC_ANGLE_LIMIT))
            {
                return FCCH_INVALID;
            }

            tempFof = (INT32) (Res->FOf*135417);
            Res->FOf = (INT16)(tempFof >> 16);

#else
            UINT32 algo_error;
            INT32 valid_thres;
            UINT32 reduced_Acc_Energy = (g_sppFcchParam.Acc_Energy >> 15);

            // FOf calculation
            Res->FOf = (INT16)(temp_CQ / temp_CI);

            // This is to reduce arctan approx error for large angles
            algo_error = (Res->FOf * Res->FOf) >> 17; // ~ 8E-6 x FOf^2
            Res->FOf += ((Res->FOf >0)? (-((INT16)algo_error)):(((INT16)algo_error)));

            // Validity threshold : avoid "wrapped out" FOf to be detected
            // This happens when Frequency error exceed +/- 28kHz
            temp_CI = (temp_CI > 0) ? temp_CI : (-temp_CI);
            reduced_Acc_Energy = reduced_Acc_Energy?reduced_Acc_Energy:1;
            valid_thres = (temp_CI << 3) / reduced_Acc_Energy;



            // Discard FOf detection if we are out of validity bounds
            if (valid_thres < 5)
            {
                return FCCH_INVALID;
            }
#endif
            // Saturate FOf returned for large results
            Res->FOf = (Res->FOf > 13000) ? 13000 : Res->FOf;
            Res->FOf = (Res->FOf <-13000) ?-13000 : Res->FOf;
        }

        // TOF CALCULATION
        // ---------------
        // Calculate overall offset since the window's start
        Res->TOf = (INT16) ((g_sppFcchBurstNb-1)*SPP_FCCH_SIZE // passed bursts
                            + (INT16) Res->TOf // offset found
                            - ((BB_BURST_ACTIVE_SIZE-SPP_FCCH_SIZE)/2 - 1)); // win bias
        return FCCH_FOUND;
    }

    g_sppFcchBurstNb++;
    return FCCH_NOT_FOUND;


}
#endif // SPC_IF_VER >= 4

//======================================================================
// spp_DeinterleaveBlock
//----------------------------------------------------------------------
// DeInterleaving of a data block
//
// @param BufferIn UINT32*. INPUT. Pointer to equalized bursts buffer.
// @param InterleaverType UINT8. INPUT. InterleaverType may take the following values:
// - 0 : TYPE_1A. Valid for TCH/FS, TCH/EFS, FACCH/F, TCH/F2.4,
// TCH/AFS (speech, ratscch, sid_first).
// - 1 : TYPE_1B. Valid for SACCH, SDCCH, TCH/AFS(sid_update), PDTCH(CS-1 to CS-4),
// BCCH, PCH, AGCH, PACCH, PBCCH, PAGCH, PPCH, PNCH, PTCCH/D.
// - 2 : TYPE_1C. Valid for FACCH/H.
// - 3 : TYPE_2A. Valid for TCH/HS and TCH/AHS (speech, ratscch, sid_first).
// - 4 : TYPE_2B. Valid for TCH/AHS (sid_update).
// - 5 : TYPE_3. Valid for TCH/F14.4, TCH/F9.6, TCH/F4.8, TCH/H4.8
// and TCH/H2.4.
// .
// @param BurstOffset UINT8. INPUT. Offset position for deinterleaving (see ITLV
//                      documentation).
// @param BufferOut UINT32*. OUTPUT. Output pointer.
//======================================================================
PUBLIC VOID spp_DeinterleaveBlock(UINT32* BufferIn, UINT8 InterleaverType, UINT8 BurstOffset, UINT32* BufferOut)
{
    if (InterleaverType == ITLV_TYPE_2A)
        spal_ItlvDitlv(InterleaverType, BurstOffset, 228, BufferIn, BufferOut);
    else
        spal_ItlvDitlv(InterleaverType, BurstOffset, 456, BufferIn, BufferOut);

    // Mandatory as it freezes the module
    spal_ItlvIntClear();
}


//======================================================================
// spp_GetUSF
//----------------------------------------------------------------------
/// Uplink State Flag extraction
///
/// @param BufferIn UINT32*. Pointer to the beginning of the RF samples buffer.
/// @param CS UINT8. Coding scheme.
///
/// @return UINT8. Uplink State Flag
//======================================================================
PUBLIC UINT8 spp_GetUSF(UINT32* BufferIn, UINT8 CS)
{
    UINT8 i, j, nb_bits;
    UINT8 Usf = 0;
    UINT8* softBit = (UINT8*)BufferIn;
    INT16 distance, usfDistance = (INT16)0x8000;
    INT16 distance_max=0;
    UINT16 USF_pattern[8] = {0x000, 0xD0B, 0x6EC, 0xBE7, 0xBB0, 0x6BB, 0xD5C, 0x057};

    if (CS == GSM_CS1)
    {
        nb_bits = 6;
    }
    else    // CS2 & CS3 & CS4
    {
        nb_bits = 12;
    }

    for (j = 0; j < nb_bits; j++) distance_max+=(softBit[j]&0x7F);

    for (i = 0; i < 8; i++)
    {
        distance = 0;
        for (j = 0; j < nb_bits; j++)
        {
            if (((USF_pattern[i] >> j) & 1) ^ ((softBit[j] >> 7) & 1))
                distance -= (softBit[j] & 0x7f);
            else
                distance += (softBit[j] & 0x7f);
        }
        if (distance > usfDistance)
        {
            usfDistance = distance;
            Usf = i;
        }
    }


    //rau = usfDistance/distance_max;
    //if (rau>0.875) && (distance_max>24) return Usf

    //if(((usfDistance*16)>(distance_max*14))&&(distance_max>24)) return Usf;

    if(((usfDistance*16)>(distance_max*14))&&(distance_max>24)) return Usf;

    else return 0xFF;

}


//======================================================================
// spp_DecodeStealingBits
//----------------------------------------------------------------------
// Decode Stealing Flags of Normal Bursts.
//
// @param RxBuffer UINT32*. Pointer to the beginning of the RF samples buffer.
// @param StealMode BOOL. Takes following values:
// - TRUE for PDCH Steal flags type,
// - FALSE for TCH Steal flags type.
// .
// @param StealingResult UINT8*. Output pointer.
// @param Threshold INT16. Decision threshold (for TCH only).
//======================================================================
PUBLIC VOID spp_DecodeStealingBits(UINT32* RxBuffer, BOOL StealMode, UINT8* StealingResult, INT16 Threshold)
{
    int i, j;

    UINT8 cs_value[4] = {GSM_CS1,GSM_CS2,GSM_CS3,GSM_CS4};
    UINT8 cs[4] = {0xff, 0x13, 0x84, 0x68};
    UINT8 buffIdx = g_mailbox.pal2spc.win[g_spcCtx->currentSnap].rx.nBurst.bufIdx;

    if(g_spcCtx->statWinRxCfg[buffIdx].type == MBOX_CCH_CHN)
    {
        *StealingResult = GSM_CS1;
        return;
    }

    if (StealMode)//PDCH
    {
        UINT8* softBit;
        INT16 distance, csDistance = (INT16)0x8000;
        UINT8 csMode = 0;

        softBit = (UINT8*)(RxBuffer);

        for (i = 0; i < 4; i++)
        {
            distance = 0;
            for (j = 0; j < 8; j++)
            {
                if (((cs[i] >> j) & 1) ^ ((softBit[j] >> 7) & 1))
                    distance -= (softBit[j] & 0x7f);
                else
                    distance += (softBit[j] & 0x7f);

            }
            if (distance > csDistance)
            {
                csDistance = distance;
                csMode = cs_value[i];
            }
        }
        *StealingResult = csMode;
    }
    else//TCH
    {
        INT16 sum = 0;
        INT8* softBit = (INT8*)RxBuffer;

        for (i = 0; i < 8; i++)
        {
            if (softBit[i]&0x80)
            {
                sum -= (softBit[i] & 0x7f);
                sum--;
            }
            else
            {
                sum += (softBit[i] & 0x7f);
                sum++;
            }
        }
        *StealingResult = (sum > Threshold ? 0 : 1);
    }
}



//======================================================================
// spp_GetMonPower
//----------------------------------------------------------------------
// Calculate Power for Monitoring (and Interference) windows.
//
// @param MonIdx UINT8. Index of the monitoring (or interference) window.
// @param power UINT32*. Pointer to the resulting calculated power.
// @param step UINT8*. Pointer to the reached step (see AGC mechanism).
//======================================================================
PUBLIC VOID spp_GetMonPower(UINT8 MonIdx, UINT32* RxBuffer, UINT32 *ResPower)
{
    UINT32 pow;
    SPAL_DC_OFFSET_T Dco;
    pow = spal_XcorDcoc( (UINT32*)RxBuffer,
                         MON_WIN_SIZE,
                         FALSE,
                         BYPASS_DCOC,
                         &Dco);
    *ResPower = spp_10log10(pow / MON_WIN_SIZE);
}

//=============================================================================
// spp_GetChannelType
//-----------------------------------------------------------------------------
// TODO : temporary REALLY dirty Mode selection ...
//
// @param ChMode UINT16. ?? See spp_rx.c.
// @param AMRType UINT32. ?? See spp_rx.c.
// @param AMRMode UINT32. ?? See spp_rx.c.
//
// @return SPP_CODING_SCHEME_T.
//
//=============================================================================
PROTECTED SPP_CODING_SCHEME_T spp_GetChannelType(UINT16 ChMode, UINT32 AMRType, UINT32 AMRMode)
{
    switch ((ChMode >> 2) & 0x3)
    {
        case 0:     // Control channel
            switch ((ChMode >> 8) & 0x3)
            {
                case GSM_CS1:
                    return CS_CS1;
                    break;
                case GSM_CS2:
                    return CS_CS2;
                    break;
                case GSM_CS3:
                    return CS_CS3;
                    break;
                case GSM_CS4:
                    return CS_CS4;
                    break;
            }
            break;

        case 1:     // speech channel
            if (((ChMode >> 4) & 0x3) == 0) // HR or FR
            {
                if ((ChMode & 0x1) == 1) // HR
                {
                    return CS_HR;
                }
                else // FR
                {
                    return CS_FR;
                }
            }
            else if (((ChMode >> 4) & 0x3) == 1) // EFR
            {
                return CS_EFR;
            }
            else // AMR
            {
                if ((ChMode & 0x1) == 1) // AHS
                {
                    switch (AMRType)
                    {
                        case SPP_AHS_SID_UPDATE:
                            return CS_AHS_SID_UPDATE;
                        case SPP_AHS_SID_UPDATE_INH:
                            return CS_AHS_SID_UPDATE_INH;
                        case SPP_AHS_SID_FIRST_P1:
                            return CS_AHS_SID_FIRST_P1;
                        case SPP_AHS_SID_FIRST_P2:
                            return CS_AHS_SID_FIRST_P2;
                        case SPP_AHS_SID_FIRST_INH:
                            return CS_AHS_SID_FIRST_INH;
                        case SPP_AHS_ONSET:
                            return CS_AHS_ONSET;
                        case SPP_AHS_SPEECH:
                            switch (AMRMode)
                            {
                                case SPP_MR475_MODE:
                                    return CS_AHS_SPEECH_475;
                                case SPP_MR515_MODE:
                                    return CS_AHS_SPEECH_515;
                                case SPP_MR59_MODE:
                                    return CS_AHS_SPEECH_59;
                                case SPP_MR67_MODE:
                                    return CS_AHS_SPEECH_67;
                                case SPP_MR74_MODE:
                                    return CS_AHS_SPEECH_74;
                                case SPP_MR795_MODE:
                                    return CS_AHS_SPEECH_795;
                            }
                        case SPP_AHS_RATSCCH_MARKER:
                            return CS_AHS_RATSCCH_MARKER;
                        case SPP_AHS_RATSCCH_DATA:
                            return CS_AHS_RATSCCH_DATA;
                    }

                }
                else // AFS
                {
                    switch (AMRType)
                    {
                        case SPP_AFS_SID_UPDATE:
                            return CS_AFS_SID_UPDATE;
                        case SPP_AFS_SID_FIRST:
                            return CS_AFS_SID_FIRST;
                        case SPP_AFS_ONSET:
                            return CS_AFS_ONSET;
                        case SPP_AFS_SPEECH:
                            switch (AMRMode)
                            {
                                case SPP_MR475_MODE:
                                    return CS_AFS_SPEECH_475;
                                case SPP_MR515_MODE:
                                    return CS_AFS_SPEECH_515;
                                case SPP_MR59_MODE:
                                    return CS_AFS_SPEECH_59;
                                case SPP_MR67_MODE:
                                    return CS_AFS_SPEECH_67;
                                case SPP_MR74_MODE:
                                    return CS_AFS_SPEECH_74;
                                case SPP_MR795_MODE:
                                    return CS_AFS_SPEECH_795;
                                case SPP_MR102_MODE:
                                    return CS_AFS_SPEECH_102;
                                case SPP_MR122_MODE:
                                    return CS_AFS_SPEECH_122;
                            }
                        case SPP_AFS_RATSCCH:
                            return CS_AFS_RATSCCH;
                    }
                }
            }
            break;

        case 2:     // Data channel
            if (((ChMode&0x1)==0) && (((ChMode>>6)&0x3)==2))        // H96
            {
                return CS_F96;
            }
            else if(((ChMode&0x1) == 1) && (((ChMode>>6)&0x3)==1))  // H48
            {
                return CS_H48;
            }
            else if (((ChMode&0x1)==0) && (((ChMode>>6)&0x3)==1))   // F48
            {
                return CS_F48;
            }
            else if (((ChMode&0x1) == 1) && (((ChMode>>6)&0x3)==0)) // H24
            {
                return CS_H24;
            }
            else if (((ChMode&0x1)==0) && (((ChMode>>6)&0x3)==0))   // F24
            {
                return CS_F24;
            }
            else if (((ChMode&0x1)==0) && (((ChMode>>6)&0x3)==3))   // F144
            {
                return CS_F144;
            }
            break;
    }
    return 0;
}







//=============================================================================
// spp_DummyBst
//-----------------------------------------------------------------------------
// dummy burst check
//
// @param in UINT32*. INPUT. input ptr
// @param pwr UINT32. INPUT. input sample pwr.
// @param outI_ptr UINT32. INPUT. tmp buf i
// @param outQ_ptr UINT32. INPUT. tmp buf q
//
// @return BOOL. TRUE: dummy burst. FALSE: not dymmy burst
//
//=============================================================================
/*
PUBLIC BOOL spp_DummyBst(UINT32 *in, UINT32 pwr, UINT32 *outI_ptr, UINT32 *outQ_ptr)
{
#define SPP_DUMMY_A         4
#define SPP_DUMMY_B         2
    UINT32 CorrMax;
    CorrMax = spal_XcorCorrDummy(in, outI_ptr, outQ_ptr);
    CorrMax >>= 2;
    CorrMax = CorrMax*CorrMax;
    if ((SPP_DUMMY_A*CorrMax) > (SPP_DUMMY_B*pwr)) //thd: CorrMax^2/Pwr > A/B/16  (range:6...12)
        return TRUE;
    else
        return FALSE;
}
*/

