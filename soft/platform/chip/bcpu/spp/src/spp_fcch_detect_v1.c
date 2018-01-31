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
// spp public headers
#include "spp_gsm.h"
#include "spp_profile_codes.h"
// spp private headers
#include "sppp_gsm_private.h"
#include "sppp_cfg.h"

#include "spal_debug.h"

// DBG
#include "global_macros.h"
#include "bb_ifc.h"
// DBGEND

//------------------------------------------------------------------------------
//-------------- PROCESSING SPEED OPTIONS --------------------------------------
//------------------------------------------------------------------------------

// Used an approximated version of the original correlation power computation.
// It computes corr' = 3/2*(Max + 3/9*Min) rather than corr = Max + 3/8*Min
// It compares corr' > energy (that is corr > 2/3*energy) rather than corr > 3/4*energy
// Gain at 39M : 15 祍

// Used an alternative description of the detection state machine (lo/hi counters)
// Gain at 39M : 5 祍

// Used frame-based rather than sliding window energy computation
// Do not forget to enable the equivalent flag in spp_gsm_rx.c (!)
// Gain at 39M : 45 祍

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#define FCCH_THRES_HIGH_LOG2 4
#define FCCH_THRES_HIGH_MASK ((1 << FCCH_THRES_HIGH_LOG2) - 1)
#define FCCH_THRES_LOW  5
#define MAX_SEARCH_WIN  70

#ifndef SPP_FCCH_SIZE
#include "spc_mailbox.h"
#define SPP_FCCH_SIZE       (BB_FCCH_SIZE)
#define offsetCorrQ         (3*SPP_FCCH_SIZE*sizeof(UINT32))
#endif


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spp_FcchDetect
//-----------------------------------------------------------------------------
// FCCH detection optimized code
//
/// @param (UINT32)(ptrOld UINT32*. ?? See spp_fcchDetect.S
/// @param (UINT32)(ptrNew UINT32*. ?? See spp_fcchDetect.S
/// @param Params SPP_FCCH_PARAM_T*. ?? See spp_fcchDetect.S
/// @param Res SPP_FCCH_RESULT_T*. ?? See spp_fcchDetect.S
//
/// @return BOOL.
//
// @author Yordan Tabakov
//
//=============================================================================
PUBLIC BOOL spp_FcchDetect(UINT32* Ptr_old, UINT32* Ptr_new, SPP_FCCH_PARAM_T* Params, SPP_FCCH_RESULT_T* Res)
{
    INT32 accCorrI    ;  // t1
    INT32 accCorrQ    ;  // t2
    INT32 accEnergy   = Params->Acc_Energy; // t0
    INT16 samplesLeft = Params->samples_left; // initially on t6
    UINT8 counterHigh = Params->counter_high; // t8
    UINT8 counterLow  = Params->counter_low; // t9

    INT32 loopCounter = 0; // t7

#if (!CHIP_XCOR_HAS_SLIDING_WINDOW)

#define      offsetCorrI   0
#ifndef offsetCorrQ
    CONST UINT32 offsetCorrQ = 3*SPP_FCCH_SIZE*sizeof(UINT32);
#endif // offsetCorrQ
    register UINT8 * ptrNew = (UINT8 *)Ptr_new;
    register UINT8 * ptrOld = (UINT8 *)Ptr_old;

    accCorrI    = Params->Acc_Corr_I;
    accCorrQ    = Params->Acc_Corr_Q;

#endif //(!CHIP_XCOR_HAS_SLIDING_WINDOW)


    if (Params->FCCH_detected != 0)
    {

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
        INT32* ptrCrossI = (INT32*)(Ptr_new);
        INT32* ptrCrossQ = (INT32*)(Ptr_new + SPP_FCCH_SIZE);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

        // Energy update is not necessary because FCCH was already found
        // so at the next IT the fcch_detect fct will not be called
        // A loop
        loopCounter = samplesLeft;
        do
        {
            INT32 absI;
            INT32 absQ;
            INT32 absCorr;

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
            accCorrI = *ptrCrossI++;
            accCorrQ = *ptrCrossQ++;
#else // (!CHIP_XCOR_HAS_SLIDING_WINDOW)
            // update Corr_I
            accCorrI += *(UINT32 *)(ptrNew + offsetCorrI);
            accCorrI -= *(UINT32 *)(ptrOld + offsetCorrI);
            // update Corr_Q
            accCorrQ += *(UINT32 *)(ptrNew + offsetCorrQ);
            accCorrQ -= *(UINT32 *)(ptrOld + offsetCorrQ);
            // update the pointers
            ptrNew += sizeof(UINT32);
            ptrOld += sizeof(UINT32);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

            // AbsI = Abs(Acc_Corr_I)
            absI = (accCorrI >> 31) ^ accCorrI;
            // AbsQ = Abs(Acc_Corr_Q)
            absQ = (accCorrQ >> 31) ^ accCorrQ;

            // Abs_Corr = Max + 3/9 Min
            // Abs_Corr' = (absI+absQ) + 1/2 abs(absI-absQ) = 3/2 Abs_Corr
            absCorr  = (absI - absQ) >> 1;
            absCorr ^= (absCorr >> 31);
            absCorr += (absI + absQ);

            // if (Abs_Corr>(global->Max_Abs_Corr))
            if(absCorr > (INT32)Params->Max_Abs_Corr)
            {
                Params->Max_Abs_Corr = absCorr;
                Params->max_index = (samplesLeft - loopCounter);
            }

        }
        while(--loopCounter);

        // if (global->max_index > samples left)
        UINT32 max_index=Params->max_index;
        if((INT32)max_index >= samplesLeft)
        {
            max_index-=SPP_FCCH_SIZE;
        }
        Res->TOf = max_index; // save TOf
        return TRUE;
    }
    else
    {

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
        INT32* ptrCrossI =  (INT32*)(Ptr_new);
        INT32* ptrCrossQ =  (INT32*)(Ptr_new + SPP_FCCH_SIZE);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

        // B loop
        loopCounter = SPP_FCCH_SIZE;
        do
        {
            INT32 absI;
            INT32 absQ;
            INT32 absCorr;

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
            accCorrI    =  *ptrCrossI;
            accCorrQ    =  *ptrCrossQ;
#else //(!CHIP_XCOR_HAS_SLIDING_WINDOW)
            // update Corr_I
            accCorrI  += *(UINT32 *)(ptrNew + offsetCorrI);
            accCorrI  -= *(UINT32 *)(ptrOld + offsetCorrI);
            // update Corr_Q
            accCorrQ  += *(UINT32 *)(ptrNew + offsetCorrQ);
            accCorrQ  -= *(UINT32 *)(ptrOld + offsetCorrQ);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

            // AbsI = Abs(Acc_Corr_I)
            absI = (accCorrI >> 31) ^ accCorrI;
            // AbsQ = Abs(Acc_Corr_Q)
            absQ = (accCorrQ >> 31) ^ accCorrQ;


            // Abs_Corr = Max + 3/9 Min
            // Abs_Corr' = (absI+absQ) + 1/2 abs(absI-absQ) = 3/2 Abs_Corr
            absCorr  = (absI - absQ) >> 1;
            absCorr ^= (absCorr >> 31);
            absCorr += (absI + absQ);

            // if (Abs_Corr > 2/3*Acc_Energy)
            // if (Abs_Corr' > Acc_Energy)
            if(absCorr > accEnergy)
            {
                if((counterHigh == 0) || (counterLow > (FCCH_THRES_LOW + FCCH_THRES_HIGH_MASK) ))
                {
                    // new detection sequence
                    // or ongoing detection sequence
                    // with too many non-detections
                    counterHigh = 0;
                    counterLow = 0;
                }

                counterHigh++;
                counterHigh &= FCCH_THRES_HIGH_MASK;

                // if (Abs_Corr>(global->Max_Abs_Corr))
                if(absCorr > (INT32)Params->Max_Abs_Corr)
                {
                    Params->Max_Abs_Corr = absCorr;
                    Params->max_index = (SPP_FCCH_SIZE - loopCounter);
                }

                if(counterHigh == 0)
                {
                    //if Acc_Energy>threshold_energy
                    if((accEnergy>>17) != 0)
                    {
                        Params->FCCH_detected = TRUE;
                        break; // j        __FCCH_detected
                    } // __B_no_FCCH_Detected:

                    Params->Max_Abs_Corr = 0;

                } //__B_thres_not_passed:
            }

            counterLow++;


#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
            // update the pointers
            ptrCrossI++;
            ptrCrossQ++;
#else //(!CHIP_XCOR_HAS_SLIDING_WINDOW)
            // update the pointers
            ptrNew += sizeof(UINT32);
            ptrOld += sizeof(UINT32);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

        }
        while(--loopCounter);
        // exit the loop B

        loopCounter = (SPP_FCCH_SIZE - loopCounter);

        if(Params->FCCH_detected)
        {
            //__FCCH_detected:
            samplesLeft = loopCounter + MAX_SEARCH_WIN-SPP_FCCH_SIZE+1; // samples_left = i + 70 - 156
            Params->samples_left = 0; // reset samples_left
            UINT32 end = loopCounter + MAX_SEARCH_WIN; // end = i+70
            if(samplesLeft > 0)
            {
                if(!Params->lastIT)
                {
                    end = SPP_FCCH_SIZE-1; // end = 156
                    Params->samples_left = samplesLeft;
                    //j __C_no_new_max // usefull if t7 is already at 155
                    // -> here we need a precond loop
                }
                else
                {
                    // __C_lastIT
                    end = SPP_FCCH_SIZE-1; // end = 156
                }
            } // __C_no_samples_left:
            // __C_loop_start:

            // note: changing to precond loop
            // TODO: add a flag if precond is really only needed on specified case, currently assue it's always good
            while(loopCounter < (INT32)end)
            {
                INT32 absI;
                INT32 absQ;
                INT32 absCorr;

                loopCounter++;

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                accCorrI  = *(++ptrCrossI);
                accCorrQ  = *(++ptrCrossQ);
#else //(!CHIP_XCOR_HAS_SLIDING_WINDOW)
                ptrNew += sizeof(UINT32);
                ptrOld += sizeof(UINT32);
                // update Corr_I
                accCorrI  += *(UINT32 *)(ptrNew + offsetCorrI);
                accCorrI  -= *(UINT32 *)(ptrOld + offsetCorrI);
                // update Corr_Q
                accCorrQ  += *(UINT32 *)(ptrNew + offsetCorrQ);
                accCorrQ  -= *(UINT32 *)(ptrOld + offsetCorrQ);
#endif //(CHIP_XCOR_HAS_SLIDING_WINDOW)

                // AbsI = Abs(Acc_Corr_I)
                absI = (accCorrI >> 31) ^ accCorrI;
                // AbsQ = Abs(Acc_Corr_Q)
                absQ = (accCorrQ >> 31) ^ accCorrQ;

                // Abs_Corr = Max + 3/9 Min
                // Abs_Corr' = (absI+absQ) + 1/2 abs(absI-absQ) = 3/2 Abs_Corr
                absCorr  = (absI - absQ) >> 1;
                absCorr ^= (absCorr >> 31);
                absCorr += (absI + absQ);

                // if (Abs_Corr>(global->Max_Abs_Corr))
                if(absCorr > (INT32)Params->Max_Abs_Corr)
                {
                    Params->Max_Abs_Corr = absCorr;
                    Params->max_index = loopCounter;
                }
            }; // __C_loop_start

            // if(samples_left==0)
            if(Params->samples_left == 0)
            {
                // if (global->max_index > loop cnt)
                UINT32 max_index=Params->max_index;
                if((INT32)max_index >= loopCounter)
                {
                    max_index-=SPP_FCCH_SIZE;
                }
                Res->TOf = max_index; // save TOf
                return TRUE;
            }
        }
        //__end_on_FALSE:

        Params->Acc_Corr_I = accCorrI;
        Params->Acc_Corr_Q = accCorrQ;
        Params->counter_high = counterHigh;
        Params->counter_low = counterLow;
        return FALSE;
    }
    return FALSE;
}




