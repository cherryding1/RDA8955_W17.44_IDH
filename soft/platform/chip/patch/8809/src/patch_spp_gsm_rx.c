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

// TOP
// include
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
PRIVATE SPP_FCCH_PARAM_T g_sppFcchParam SPAL_INTSRAM_PATCH_UCDATA;
PRIVATE UINT16 g_sppFcchBurstNb  SPAL_INTSRAM_PATCH_UCDATA = 0;
PRIVATE UINT32* g_sppPtrNew SPAL_INTSRAM_PATCH_UCDATA;
#if (SPC_IF_VER >= 4)
#define ANGLE_USE_CORDIC
#define  CORDIC_ANGLE_LIMIT 0x6000
#endif
#ifdef ENABLE_PATCH_SPP_GETUSF
UINT8 SPAL_INTSRAM_PATCH_UCDATA g_mailbox_pal2spc_statWin_rx_Usf[4] = {0xFF,0xFF,0xFF,0xFF};
extern UINT8  g_mailbox_pal2spc_statWin_rx_Usf_temp;
extern UINT8  g_rx_Usf_Decode_temp;

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
// SPAL_BBSRAM_PATCH_DATA    INT16 distance, usfDistance = (INT16)0x8000;
// SPAL_BBSRAM_PATCH_DATA    INT16 distance_max=0;
// SPAL_BBSRAM_PATCH_DATA    INT16   usfDistance_num  = 16, distance_max_num = 14, distance_max1_num = 24, usfvalue = 0;
SPAL_INTSRAM_PATCH_UCDATA   UINT8   usfDistance_num  = 20, distance_max_num = 13, distance_max1_num = 12;
PUBLIC UINT8 SPAL_INTSRAM_PATCH_TEXT  patch_spp_GetUSF(UINT32* BufferIn, UINT8 CS)
{

    UINT8 i, j, nb_bits;
    UINT8 Usf = 0;
    UINT8* softBit = (UINT8*)BufferIn;
    INT16 distance, usfDistance = (INT16)0x8000;
    INT16 distance_max=0;
    UINT16 UsfPatternTem = 0;
    UINT16 USF_pattern[8] = {0x000, 0xD0B, 0x6EC, 0xBE7, 0xBB0, 0x6BB, 0xD5C, 0x057};

    if (CS == GSM_CS1)
    {
        nb_bits = 6;
    }
    else    // CS2 & CS3 & CS4
    {
        nb_bits = 12;
    }

    for (j = 0; j < nb_bits; j++)
        distance_max+=((INT16)(softBit[j] & 0x7F));

    for (i = 0; i < 8; i++)
    {
        distance = 0;
        for (j = 0; j < nb_bits; j++)
        {
            if (((USF_pattern[i] >> j) & 1) ^ ((softBit[j] >> 7) & 1))
                distance -= ((INT16)(softBit[j] & 0x7F));
            else
                distance += ((INT16)(softBit[j] & 0x7F));
        }
        if (distance > usfDistance)
        {
            usfDistance = distance;
            Usf = i;
        }
    }

    if(((usfDistance*usfDistance_num)>(distance_max*distance_max_num))&&(distance_max>distance_max1_num))
    {
        g_rx_Usf_Decode_temp = Usf;
    }
    else
    {
        g_rx_Usf_Decode_temp = 0xFF;
    }

    if (g_mailbox_pal2spc_statWin_rx_Usf_temp == 0xFF)
    {
        return g_rx_Usf_Decode_temp;
    }

    UsfPatternTem = USF_pattern[g_mailbox_pal2spc_statWin_rx_Usf_temp];
    Usf = g_mailbox_pal2spc_statWin_rx_Usf_temp;
    distance = 0;
    for (j = 0; j < nb_bits; j++)
    {
        if (((UsfPatternTem >> j) & 1) ^ ((softBit[j] >> 7) & 1))
            distance -= ((INT16)(softBit[j] & 0x7F));
        else
            distance += ((INT16)(softBit[j] & 0x7F));
    }
    usfDistance = distance;

    if(((usfDistance*usfDistance_num)>(distance_max*distance_max_num))&&(distance_max>distance_max1_num))
    {
        return Usf;
    }
    else
    {
        return 0xFF;
    }

}

#endif
#ifdef FREQ_LARGESCALE_SUPPORT 
PUBLIC SPAL_INTSRAM_PATCH_TEXT SPP_FCCH_RETURN_T  patch_spp_SearchFcch(UINT32* RxBuffer,
                                        BOOL Init,
                                        SPP_FCCH_MODE_T Mode,
                                        SPP_FCCH_DELAY_TYPE_T DelayType,
                                        SPP_FCCH_RESULT_T* Res)
{
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
        g_sppPtrNew = MEM_ACCESS_CACHED(RxBuffer);
        temp_ptr = (g_sppGlobalParams.fcch_sliding_energy) ? g_sppPtrNew + 3*SPP_FCCH_SIZE : g_sppPtrNew;
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
        g_sppFcchParam.Acc_Energy =
            spal_XcorDcoc((UINT32*)MEM_ACCESS_CACHED(RxBuffer),
                          SPP_FCCH_SIZE,
                          FALSE, // No derotation
                          BYPASS_DCOC, // no bypass dcoc,
                          &Dco);
#if (SPC_IF_VER >= 4)
        spal_InvalidateDcache();
#endif
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
        temp_accu = 0;
        for(i=0; i<SPP_FCCH_SIZE; i++)
            temp_accu += *(temp_ptr++);
        g_sppFcchParam.Acc_Corr_I = temp_accu;
        temp_ptr += 2*SPP_FCCH_SIZE; // point to CQ
        temp_accu = 0;
        for(i=0; i<SPP_FCCH_SIZE; i++)
            temp_accu += *(temp_ptr++);
        g_sppFcchParam.Acc_Corr_Q = temp_accu;
        return FCCH_NOT_FOUND;
    }
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
        if (((g_sppFcchBurstNb == 1) && // window's beginning
                (Res->TOf <= (BB_BURST_ACTIVE_SIZE-SPP_FCCH_SIZE)))
                ||
                ( g_sppFcchParam.lastIT && // End of sliced mode window
                  (Res->TOf >= (2*SPP_FCCH_SIZE-BB_BURST_ACTIVE_SIZE)))
           )
        {
            return FCCH_INVALID;
        }
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
            INT32 tempFofnew;
            temp_CI = (DelayType == FCCH_DELAY2) ? (-temp_CI): temp_CI;
            temp_CQ = (DelayType == FCCH_DELAY2) ? (-temp_CQ): temp_CQ;
            spal_CordicAngle(temp_CI, temp_CQ, &(Res->FOf));
            if ( (Res->FOf > CORDIC_ANGLE_LIMIT) || (Res->FOf <-CORDIC_ANGLE_LIMIT))
            {
                return FCCH_INVALID;
            }
            tempFof = (INT32) (Res->FOf*((DelayType == FCCH_DELAY2) ? 67708 : 33854));
            tempFofnew = tempFof >> 15;
            if (tempFofnew > 32767)
            {
                tempFofnew = 32767;
            }
            if (tempFofnew < -32768)
            {
                tempFofnew = -32768;
            }            
            Res->FOf = (INT16)(tempFofnew);
#else
            UINT32 algo_error;
            INT32 valid_thres;
            UINT32 reduced_Acc_Energy = (g_sppFcchParam.Acc_Energy >> 15);
            Res->FOf = (INT16)(temp_CQ / temp_CI);
            algo_error = (Res->FOf * Res->FOf) >> 17; // ~ 8E-6 x FOf^2
            Res->FOf += ((Res->FOf >0)? (-((INT16)algo_error)):(((INT16)algo_error)));
            temp_CI = (temp_CI > 0) ? temp_CI : (-temp_CI);
            reduced_Acc_Energy = reduced_Acc_Energy?reduced_Acc_Energy:1;
            valid_thres = (temp_CI << 3) / reduced_Acc_Energy;
            if (valid_thres < 5)
            {
                return FCCH_INVALID;
            }
#endif
        }
        Res->TOf = (INT16) ((g_sppFcchBurstNb-1)*SPP_FCCH_SIZE // passed bursts
                            + (INT16) Res->TOf // offset found
                            - ((BB_BURST_ACTIVE_SIZE-SPP_FCCH_SIZE)/2 - 1)); // win bias
        return FCCH_FOUND;
    }
    g_sppFcchBurstNb++;
    return FCCH_NOT_FOUND;
}
#endif