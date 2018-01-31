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
#include "chip_id.h"
// chip/include
#include "global_macros.h"
#include "xcor.h"
// spal public header
#include "spal_xcor.h"
#include "spal_mem.h"
// spal private header
#include "spalp_private.h"
#include "spalp_debug.h"

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------

#if (CHIP_XCOR_8PSK_SUPPORTED)

#define XCOR_DEROTATION_EN XCOR_DEROTATION

#endif

#if (SPC_IF_VER >= 4)
#ifndef SPP_FCCH_SIZE
#include "spc_mailbox.h"
#define SPP_FCCH_SIZE       (BB_FCCH_SIZE)
#endif

#define SPAL_XCORFLT_ASM
#endif // SPC_IF_VER >= 4
//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

PRIVATE INT32 SPAL_BBSRAM_GLOBAL g_spalDoubleRegx;
PRIVATE INT32 SPAL_BBSRAM_GLOBAL g_spalDoubleRegy;

PRIVATE UINT32 SPAL_BBSRAM_GLOBAL g_spalRkm1;
PRIVATE UINT32 SPAL_BBSRAM_GLOBAL g_spalRkm2;
#if (SPC_IF_VER >= 4)
PRIVATE UINT32 SPAL_BBSRAM_GLOBAL g_nspalRkm1;
PRIVATE UINT32 SPAL_BBSRAM_GLOBAL g_nspalRkm2;
PRIVATE UINT32 SPAL_BBSRAM_GLOBAL g_nspalRkm3;
PRIVATE UINT32 SPAL_BBSRAM_GLOBAL g_nspalRkm4;
#endif

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------

//=============================================================================
// spal_XcorResetFchFilterParam
//-----------------------------------------------------------------------------
/// Reset the parameters of FCH filter.
///
//=============================================================================
PUBLIC VOID spal_XcorResetFchFilterParam(VOID)
{
    g_spalDoubleRegx = 0;
    g_spalDoubleRegy = 0;
}


//=============================================================================
// spal_XcorActive
//-----------------------------------------------------------------------------
/// Check if XCOR is activated
///
//=============================================================================
PRIVATE BOOL spal_XcorActive()
{
    if (((hwp_xcor->status) & XCOR_OP_PENDING))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



//=============================================================================
// spal_XcorTscxcNb
//-----------------------------------------------------------------------------
// This function performs the I and Q correlations of a normal burst training
// sequence with some received samples. 8 + SPAL_ISI = 13 consecutives correlation
// are performed.
//
// @param GsmTscNburst UINT32. INPUT. Training sequence. The correlation only
// considers 16 bits (bits 5 to 21).
// @param symb_ptr UINT32*. INPUT. Pointer to the received sample buffer where
// I and Q samples are 16bits samples (INT16) with I samples on the LSB
// and Q samples on the MSB. E.g. :
// - Isample[0]=((INT16*) symb_ptr)[0]; ,
// - Qsample[0]=((INT16*) symb_ptr)[1]; .
// .
// @param out_ptr INT16*. OUTPUT. Pointer to the I and Q correlation with
// out_ptr[2*t] the I-correlation on the t time slot and out_ptr[2*t+1]
// the Q-correlation on the t time slot.
//=============================================================================
PUBLIC VOID spal_XcorTscxcNb(
    UINT32 GsmTscNburst,
    UINT32* symb_ptr,
    INT16* out_ptr)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorTscxcNb);
#if (CHIP_XCOR_8PSK_SUPPORTED)
    UINT32 cmd =   XCOR_TSCXC_EN|XCOR_PACK_IQ;
    hwp_xcor->convolution_0 = XCOR_NB_SYMB(NB_TSC_XCOR_NBR)|
                              XCOR_NB_ILOOP(NB_TSC_XCOR_SIZE);
#else
    UINT32 cmd =   XCOR_TSCXC_EN|XCOR_PACK_IQ|
                   XCOR_NB_SYMB(NB_TSC_XCOR_NBR)|
                   XCOR_NB_ILOOP(NB_TSC_XCOR_SIZE);
#endif

    // TODO : check the >>5 shift
    // move MSB 2 bits to the end. update by tcj @20110829
    hwp_xcor->hv0 = ((GsmTscNburst>>5) << 2)
                    | ( (GsmTscNburst>>(NB_TSC_XCOR_SIZE+4)) & 0x3 );
    hwp_xcor->addr0 = (UINT32)symb_ptr;
    hwp_xcor->data[2] = (UINT32)out_ptr;
    hwp_xcor->command = cmd;

    // Poll until the end of processing
    while (spal_XcorActive()) ;
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorTscxcNb);
}

//=============================================================================
// spal_XcorTscxcSch
//-----------------------------------------------------------------------------
// Training Sequence Xcorrelation mode for SCH burst
// I and Q results are stored on separated words
//
// @param symb_ptr UINT32*. ??
// @param outI_ptr UINT32*. ??
// @param outQ_ptr UINT32*. ??
//=============================================================================
PUBLIC VOID spal_XcorTscxcSch(UINT32* symb_ptr, UINT32* outI_ptr, UINT32* outQ_ptr)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorTscxcSch);
#if (CHIP_XCOR_8PSK_SUPPORTED)
    UINT32 cmd =   XCOR_TSCXC_EN;
    hwp_xcor->convolution_0 = XCOR_NB_SYMB(SCH_TSC_XCOR_NBR)|
                              XCOR_NB_ILOOP(SCH_TSC_XCOR_SIZE);
#else
    UINT32 cmd =   XCOR_TSCXC_EN|
                   XCOR_NB_SYMB(SCH_TSC_XCOR_NBR)|
                   XCOR_NB_ILOOP(SCH_TSC_XCOR_SIZE);
#endif

    hwp_xcor->hv0 = (GSM_TSC_SCH0<<2)|((GSM_TSC_SCH1>>30) & 0x3);
    hwp_xcor->hv1 = (GSM_TSC_SCH1<<2)|((GSM_TSC_SCH0>>30) & 0x3);
    hwp_xcor->addr0 = (UINT32)symb_ptr;
    hwp_xcor->data[2] = (UINT32)outI_ptr;
    hwp_xcor->data[3] = (UINT32)outQ_ptr;
    hwp_xcor->command = cmd;

    // Poll until the end of processing
    while (spal_XcorActive()) ;
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorTscxcSch);
}



//=============================================================================
// spal_XcorFchxc
//-----------------------------------------------------------------------------
// Xcorrelation mode for FCH burst detection
// Power term overwrites symbols,
// CI term is stored at Offset fom Power term
// and CQ term is stored at Offset from CI term.
//
// @param symb_ptr      UINT32*. Pointer to current symbol window.
// @param old_ptr       UINT32*. Pointer to previous symbol window.
// @param out_ptr       UINT32*. Pointer to the output buffer.
// @param size          UINT16.  Size of the window.
// @param computeEnergy UINT16.  Flag that defines if energy needs to be calculated.
//=============================================================================
PUBLIC VOID spal_XcorFchxc( UINT32* symb_ptr,
                            UINT32* old_ptr,
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                            UINT32* out_ptr,
                            INT32   accu_energy,
                            INT32   accu_corrI,
                            INT32   accu_corrQ,
#endif // (CHIP_XCOR_HAS_SLIDING_WINDOW)
                            UINT16  size,
                            UINT16  computeEnergy)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorFchxc);
    UINT32 cmd;
#define offset (3*size)

#if (CHIP_XCOR_8PSK_SUPPORTED)
    cmd =  XCOR_FCHXC_EN | XCOR_DEROTATION_EN;
    hwp_xcor->convolution_0 = XCOR_NB_SYMB(size);
#else
    cmd =  XCOR_FCHXC_EN |
           XCOR_DEROTATION_EN | // for datapath setting
           XCOR_NB_SYMB(size);
#endif

    if (old_ptr == NULL)
    {
        // First call for an FCCH detection : R(k-1) and R(k-2)
        // are not defined
        g_spalRkm1 = 0;
        g_spalRkm2 = 0;
    }

    hwp_xcor->data[0] = (UINT32)symb_ptr[0];           // R(k)
    hwp_xcor->hv0     = (UINT32)g_spalRkm1;            // R(k-1)
    hwp_xcor->data[1] = (UINT32)g_spalRkm2;            // R(k-2)
    hwp_xcor->addr0   = (UINT32)symb_ptr;              // @R(k)
    hwp_xcor->data[2] = (UINT32)&(symb_ptr[0*offset]); // @ResPW
    if (computeEnergy)
    {
        hwp_xcor->data[3] = (UINT32)&(symb_ptr[1*offset]); // @ResCI
        hwp_xcor->data[5] = (UINT32)&(symb_ptr[2*offset]); // @ResCQ
    }
    else
    {
        // overwrite energy buffer
        hwp_xcor->data[3] = (UINT32)&(symb_ptr[0*offset]); // @ResCI
        hwp_xcor->data[5] = (UINT32)&(symb_ptr[1*offset]); // @ResCQ
    }
    // Following calls are supposed to be done on successive
    // buffers representing a continuous symbol reception.
    g_spalRkm1 = symb_ptr[size-1]; // last symbol becomes R(k-1)
    g_spalRkm2 = symb_ptr[size-2]; // before last symbol becomes R(k-2)
    // Run XCOR
    hwp_xcor->command = cmd;

#if (CHIP_XCOR_HAS_SLIDING_WINDOW)

    if ((old_ptr != NULL) &&  (out_ptr != NULL))
    {
        UINT32* tmpOld = old_ptr;
        UINT32* tmpNew = symb_ptr;
        UINT32* tmpOut = out_ptr;

        // Poll until the end of processing
        while (spal_XcorActive()) ;

        if(computeEnergy)
        {
            // Update Energy
            hwp_xcor->hv0        =    accu_energy; // Init with average value
            hwp_xcor->addr0        =    (UINT32)&tmpOld[0]; // Old pointer
            hwp_xcor->data[2]    =    (UINT32)&tmpNew[0]; // New pointer
            hwp_xcor->data[3]    =    (UINT32)&tmpOut[0]; // Output
            hwp_xcor->command    =    XCOR_SLDWIN_EN|XCOR_NB_SYMB(size);

            // Poll until the end of processing
            while (spal_XcorActive());

            tmpOld += offset;
            tmpNew += offset;
            tmpOut += size;
        }

        // Update I
        hwp_xcor->hv0        =    accu_corrI; // Init with average value
        hwp_xcor->addr0        =    (UINT32)&tmpOld[0]; // Old pointer
        hwp_xcor->data[2]   =    (UINT32)&tmpNew[0]; // New pointer
        hwp_xcor->data[3]   =    (UINT32)&tmpOut[0]; // Output
        hwp_xcor->command    =    XCOR_SLDWIN_EN|XCOR_NB_SYMB(size);

        // Poll until the end of processing
        while (spal_XcorActive()) ;

        tmpOld += offset;
        tmpNew += offset;
        tmpOut += size;

        hwp_xcor->hv0        =    accu_corrQ; // Init with average value
        hwp_xcor->addr0        =    (UINT32)&tmpOld[0]; // Old pointer
        hwp_xcor->data[2]   =    (UINT32)&tmpNew[0]; // New pointer
        hwp_xcor->data[3]   =    (UINT32)&tmpOut[0]; // Output
        hwp_xcor->command    =    XCOR_SLDWIN_EN|XCOR_NB_SYMB(size);
    }

#endif // (CHIP_XCOR_HAS_SLIDING_WINDOW)

    // Flush the Dcache to enable later cached accesses
    spal_InvalidateDcache();

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorFchxc);
}



//=============================================================================
// spal_XcorBiterror
//-----------------------------------------------------------------------------
// Biterror calculation mode
//
// @param NbSVwords UINT16. INPUT. Number of softvalue words to be treated.
// @param sofvalues_ptr UINT32*. INPUT. Pointer to the buffer of soft values
// coded on 8 bits with 1 sign bit and 7 amplitude bits.
// @param hardvalues_ptr UINT32*. INPUT. Pointer to the hard decisions after
// decoding
// @param biterror UINT16*. Number of soft bits corrected into an hard bit
// of opposite sign.
//=============================================================================
PUBLIC VOID spal_XcorBiterror(UINT16 NbSVwords, UINT32* sofvalues_ptr, CONST UINT32* hardvalues_ptr, UINT16* biterror)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorBiterror);
    hwp_xcor->addr0 = (UINT32)sofvalues_ptr;
    hwp_xcor->data[2] = (UINT32)hardvalues_ptr;
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0 = XCOR_NB_ILOOP(8) | XCOR_NB_SYMB(NbSVwords);
    hwp_xcor->command = XCOR_BITERR_EN;
#else
    hwp_xcor->command = XCOR_BITERR_EN|
                        XCOR_NB_ILOOP(8) |
                        XCOR_NB_SYMB(NbSVwords);
#endif

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    *biterror = hwp_xcor->accuI;
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorBiterror);
}

//=============================================================================
// spal_XcorChest
//-----------------------------------------------------------------------------
// Performs LSE channel estimation. CHIP_CHEST_LEN gives the maximum number of taps that
// can be estimated.
//
// @param RxBuffer INT16*. INPUT. Pointer to the buffer of received samples.
// The samples are complex with RxBuffer[2*t] the inphase part of sample t and
// RxBuffer[2*t+1] the quadrature part of sample t.
// @param ChestTab CONST INT16*. INPUT. channel estimation table
// @param taps INT16*. OUTPUT. Estimated taps of the channel. The taps are complex.
// @param chTapNb UINT8. INPUT. number of channel taps to estimate. Should be smaller or equal to CHIP_CHEST_LEN
// taps[2*t] is the inphase part of taps t,
// taps[2*t+1] is the quadrature part of taps t.
//=============================================================================
PUBLIC VOID spal_XcorChest(INT16* RxBuffer, INT16 *ChestTab, INT16* taps, UINT8 chTapNb, INT8  fracBits)
{
    int i;
    // 11 when CHIP_CHEST_LEN = 5
    // 10 when CHIP_CHEST_LEN = 8
    UINT32 local_table[(27+1-CHIP_CHEST_LEN)/2];

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorChest);
    for(i=0; i<chTapNb; i++)
    {
        register UINT8 j;
        register UINT16 *tmp_src = (UINT16*)&(ChestTab[i*(27-CHIP_CHEST_LEN)]);
        register UINT16 *tmp_dst = (UINT16*)local_table;
        // Copy table locally
        //for (j=0;j<(27+1-CHIP_CHEST_LEN);j++)
        // the following line changed by tcj bug removed @20110519.
        for (j=0; j<(27-CHIP_CHEST_LEN); j++)
        {
            *tmp_dst++ = *tmp_src++;
        }

        hwp_xcor->addr0    = (UINT32)RxBuffer;
        hwp_xcor->data[2]  =   (UINT32)local_table;
#if (CHIP_XCOR_8PSK_SUPPORTED)
        hwp_xcor->convolution_0 = XCOR_NB_SYMB((27-CHIP_CHEST_LEN))|
                                  XCOR_NB_ILOOP(3);
        hwp_xcor->command  =   XCOR_CHEST_EN | XCOR_DEROTATION_EN;
#else
        hwp_xcor->command  =   XCOR_CHEST_EN |
                               XCOR_DEROTATION_EN | // needed for datapath setup
                               XCOR_NB_SYMB((27-CHIP_CHEST_LEN))|
                               XCOR_NB_ILOOP(3);
#endif
        // Poll until the end of processing
        while (spal_XcorActive()) ;

        // Get results
        taps[2*i] = (INT16)((INT32)hwp_xcor->hv0 >> fracBits);
        taps[2*i+1] = (INT16)((INT32)hwp_xcor->hv1 >> fracBits);
    }

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorChest);
}



//=============================================================================
// spal_XcorSrec
//-----------------------------------------------------------------------------
// Reconstruct a reference GMSK burst from channel taps and estimated/known hard bits.
//
// @param taps INT16*. INPUT. Taps of the channel.
// @param size UINT8. INPUT. Length of the reconstructed burst.
// @param HardBits UINT32*. INPUT. Hard bits to be reconstructed.
// @param refBurst UINT32*. OUTPUT. Pointer to the samples of the reconstructed burst.
// refBurst[2*t] is the inphase part of sample t,
// refBurst[2*t+1] is the quadrature part of sample t.
//=============================================================================
PUBLIC VOID spal_XcorSrec(INT16* taps, UINT8 size, UINT32* HardBits, UINT32* refBurst, UINT8 chTapNb)
{
    UINT32* UINT32taps = (UINT32*)taps;
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorSrec);

    hwp_xcor->hv0  =   HardBits[0];
    hwp_xcor->hv1  =   HardBits[1]; // only used for SCH
    // Goes backward for HW simplicity -> give end address
    hwp_xcor->addr0    =   (UINT32)&(refBurst[size-1]);
    // Taps must be 32-bits aligned (i.e. IQ packed as samples)
    hwp_xcor->data[0]  =   UINT32taps[0];
    hwp_xcor->data[1]  =   UINT32taps[1];
    hwp_xcor->data[2]  =   UINT32taps[2];
    hwp_xcor->data[3]  =   UINT32taps[3];
    hwp_xcor->data[4]  =   UINT32taps[4];
    // TODO: see if we really need 6 ISI support (not in HW for now)
    //hwp_xcor->Data[5]  =   UINT32taps[5];
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0 = XCOR_NB_SYMB((size))|XCOR_NB_ILOOP((SPAL_ISI-1));
    hwp_xcor->command  =   XCOR_SREC_EN|XCOR_PACK_IQ;
#else
    hwp_xcor->command  =   XCOR_SREC_EN|XCOR_PACK_IQ|
                           XCOR_NB_SYMB((size))|
                           XCOR_NB_ILOOP((SPAL_ISI-1));
#endif
    // Poll until the end of processing
    while (spal_XcorActive()) ;
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorSrec);
}


//=============================================================================
// spal_XcorBmsum
//-----------------------------------------------------------------------------
// This functions computes the sum tables required by the viterbi.
// It receives channel taps and computes for any register state,
// the corresponding reference sample.
//
// The functions computes the table for a 16 states viterbi (channel of length 5).
// Therefore the functions returns 16 complex reference samples for forward
// treillis and 16 complex reference samples for backward treillis.
//
// @param taps INT16*. INPUT. Pointer to the channel taps with:
// - taps[2*t] I part of the complex tap t,
// - taps[2*t+1] Q part of the complex tap t.
// .
// @param SumTable INT16*. OUTPUT. Pointer to the output buffer.
// The first 16 complex samples are the reference samples
// for forward treillis, the last 16 complex samples are the reference samples for
// backward treillis with:
// - SumTable[2*t] I part of the complex sample t,
// - SumTable[2*t+1] Q part of the complex sample t.
// .
//=============================================================================
PUBLIC VOID spal_XcorBmsum(INT16* taps, INT16* SumTable)
{
    UINT32 tmp_SumTable[8];
    INT16* INT16_tmp_SumTable = (INT16*)MEM_ACCESS_UNCACHED((UINT32)tmp_SumTable);
    INT32 tmp_i, tmp_q;
    UINT32* SumTable32 = (UINT32*)SumTable;
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorBmsum);

    // Compute Table for Forward trellis

    // Use the reconstruction mode on 3 taps with a special pattern:
    // 0010111000b(0xB8) gives 0,1,3,7,6,5,2,4
    hwp_xcor->hv0  =   0x0B8;
    // Goes backward for HW simplicity -> give end address
    hwp_xcor->addr0    =   (UINT32)(&tmp_SumTable[(8-1)]);
    hwp_xcor->data[2]  =   ((taps[3]<<16)|(taps[2]&0xffff));
    hwp_xcor->data[3]  =   ((taps[5]<<16)|(taps[4]&0xffff));
    hwp_xcor->data[4]  =   ((taps[7]<<16)|(taps[6]&0xffff));
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0 = XCOR_NB_SYMB(8)| XCOR_NB_ILOOP(2);
    hwp_xcor->command  =   XCOR_SREC_EN|XCOR_PACK_IQ;
#else
    hwp_xcor->command  =   XCOR_SREC_EN|XCOR_PACK_IQ|
                           XCOR_NB_SYMB(8)|
                           XCOR_NB_ILOOP(2);
#endif
    // Poll until the end of processing
    while (spal_XcorActive()) ;

    // Reorder and add constant
    tmp_i = taps[2*SPAL_DELAY]   + taps[0];
    tmp_q = taps[2*SPAL_DELAY+1] + taps[1];

    // 0 -> 0 & 8
    SumTable32[ 8] =
        SumTable32[ 0] = ((INT16_tmp_SumTable[0]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[1]   + tmp_q) << 16);
    // 1 -> 1 & 12
    SumTable32[12] =
        SumTable32[ 1] = ((INT16_tmp_SumTable[1*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[1*2+1] + tmp_q) << 16);
    // 2 -> 3 & 14
    SumTable32[14] =
        SumTable32[ 3] = ((INT16_tmp_SumTable[2*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[2*2+1] + tmp_q) << 16);
    // 3 -> 7 & 15
    SumTable32[15] =
        SumTable32[ 7] = ((INT16_tmp_SumTable[3*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[3*2+1] + tmp_q) << 16);
    // 4 -> 6 & 11
    SumTable32[11] =
        SumTable32[ 6] = ((INT16_tmp_SumTable[4*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[4*2+1] + tmp_q) << 16);
    // 5 -> 5 & 13
    SumTable32[13] =
        SumTable32[ 5] = ((INT16_tmp_SumTable[5*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[5*2+1] + tmp_q) << 16);
    // 6 -> 2 & 10
    SumTable32[10] =
        SumTable32[ 2] = ((INT16_tmp_SumTable[6*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[6*2+1] + tmp_q) << 16);
    // 7 -> 4 & 9
    SumTable32[ 9] =
        SumTable32[ 4] = ((INT16_tmp_SumTable[7*2]   + tmp_i) & 0xFFFF) |
                         ((INT16_tmp_SumTable[7*2+1] + tmp_q) << 16);

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorBmsum);
}

//=============================================================================
// spal_XcorErrCalc
//-----------------------------------------------------------------------------
// Estimates noise from a reference received sequence to an actually received
// sequence.
//
// @param Original UINT32*. INPUT. Pointer to the received sequence samples.
// Original[2*t] is the inphase part of received sample t,
// Original[2*t+1] is the quadrature part of received sample t.
// @param Reference UINT32*. INPUT. Pointer to the ideal sequence samples.
// Reference[2*t] is the inphase part of reference sample t,
// Reference[2*t+1] is the quadrature part of reference sample t.
// @param size UINT8. INPUT. Number of samples for noise estimation.
//
// @return UINT32. OUTPUT. Total noise power (linear).
//=============================================================================
PUBLIC UINT32 spal_XcorErrCalc(UINT32* Original, UINT32* Reference, UINT8 size)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorErrCalc);
    hwp_xcor->data[2]  =   (UINT32)(Original);
    hwp_xcor->addr0    =   (UINT32)(Reference);
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0 = XCOR_NB_SYMB(size)| XCOR_NB_ILOOP(4);
    hwp_xcor->command  =   (XCOR_SPROC_EN|XCOR_PACK_IQ);
#else
    hwp_xcor->command  =   (XCOR_SPROC_EN|XCOR_PACK_IQ) |
                           XCOR_NB_SYMB(size)| XCOR_NB_ILOOP(4);
#endif
    // Poll until the end of processing
    while (spal_XcorActive()) ;

    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorErrCalc);
    return (hwp_xcor->hv0);
}


//=============================================================================
// spal_XcorIcorCalc
//-----------------------------------------------------------------------------
// Performs a complex multiplications of a sequence with the conjugate of
// another sequence and returns the sum of the imaginary parts
// of the result sequence.
//
// With the input below, the functions returns:
// return = Sum(Original(2*t)*Reference(2*t+1)-Original(2*t+1)*Reference(2*t));
//
// @param Original UINT32*. INPUT. Pointer to the complex sequence of received
// samples where:
// - ((INT16*)Original)[2*t] is the real part (inphase) of sample t,
// - ((INT16*)Original)[2*t+1] is the imaginary part (quadrature) of sample t.
// @param Reference UINT32*. INPUT. Pointer to the complex sequence of reconstructed
// samples (samples rebuilt after equalization) where:
// - ((INT16*)Reference)[2*t] is the real part (inphase) of sample t,
// - ((INT16*)Reference)[2*t+1] is the imaginary part (quadrature) of sample t.
// @param size UINT8. INPUT. The length of both sequences.
//
// @return INT32. The sum of the imaginary part of the multiplication of
// Original samples with conjugate Reference samples.
//=============================================================================
PUBLIC INT32 spal_XcorIcorCalc(UINT32* Original, UINT32* Reference, UINT8 size)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorIcorCalc);
    hwp_xcor->data[2]  =   (UINT32)(Original);
    hwp_xcor->addr0    =   (UINT32)(Reference);
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0  = XCOR_NB_SYMB(size)| XCOR_NB_ILOOP(3);
    hwp_xcor->command  =   XCOR_SPROC_EN;
#else
    hwp_xcor->command  =   XCOR_SPROC_EN |
                           XCOR_NB_SYMB(size)|
                           XCOR_NB_ILOOP(3);
#endif
    // Poll until the end of processing
    while (spal_XcorActive()) ;
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorIcorCalc);
    return ((INT32)hwp_xcor->hv0);
}


//=============================================================================
// spal_XcorHvextract
//-----------------------------------------------------------------------------
// This function reads a buffer of soft bits and outputs the corresponding
// hard bit.
//
// @param softvalues UINT8*. INPUT. Pointer to the soft bit buffer. The soft
// bits shall be coded on 8 bits with 1 sign bit (MSB) and 7 amplitude bits.
// @param size UINT8. INPUT. Length of the soft bit input buffer SEEN AS A
// BUFFER OF 32 BITS WORDS !!! For example if we want to extract 4 hard bits
// from the corresponding soft bits, size=1, in other words
// size=nb_soft_bits*8/32.
// @param hardvalues UINT32*. OUTPUT. Pointer to the output buffer presented
// as a buffer of 32 bits words. The first hard bit is on the LSB of the first
// 32 bits word, i.e.
// hard(softvalues[1])=hardvalues[0] & 0x00000001;
//=============================================================================
PUBLIC VOID spal_XcorHvextract(UINT8* softvalues, UINT8 size, UINT32* hardvalues)
{
    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorHvextract);
    hwp_xcor->addr0    =   (UINT32)softvalues;
    hwp_xcor->data[2]  =   (UINT32)hardvalues;
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0  = XCOR_NB_SYMB(size)| XCOR_NB_ILOOP(7);
    hwp_xcor->command  =   XCOR_BEXT_EN;
#else
    hwp_xcor->command  =   XCOR_BEXT_EN|
                           XCOR_NB_SYMB(size)|
                           XCOR_NB_ILOOP(7);
#endif
    // Poll until the end of processing
    while (spal_XcorActive()) ;
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorHvextract);
}


//=============================================================================
// spal_XcorDcoc
//-----------------------------------------------------------------------------
// DC offset correction function, derotation and power measurement function.
//
// @param RxBuffer UINT32*. INPUT. Pointer to the received samples buffer.
// @param size UINT8. INPUT. Length of the burst.
// @param derotate BOOL. INPUT. Triggers derotation on and off:
// - TRUE, derotation applied,
// - FALSE, derotation not applied.
// .
// @param bypassDcoc BOOL. INPUT. Switch DC Offset Correction on/off.
// @param Dco SPAL_DC_OFFSET_T*. OUTPUT. Pointer to the following structure:
// - Dco.I, DC offset on I,
// - Dco.Q, DC offset on Q.
// .
//
// @return UINT32. Return the power of the burst after DC offset correction.
//=============================================================================
PUBLIC UINT32 spal_XcorDcoc(UINT32* RxBuffer, UINT8 size, BOOL derotate, BOOL bypassDcoc, SPAL_DC_OFFSET_T* Dco)
{
    INT32 Accu_I,MeanI;
    INT32 Accu_Q,MeanQ;
    INT32 MeanIsup;
    INT32 MeanQsup;
    INT16 IOffset = 0;
    INT16 QOffset = 0;
    UINT16 Isup;
    UINT16 Qsup;
    UINT32 cmd =0;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDcoc);

    if (bypassDcoc)
    {
        // Program XCOR for 3rd Pass Only
        hwp_xcor->addr0 = (UINT32)&RxBuffer[0]; // RD address
        hwp_xcor->data[2] = (UINT32)&RxBuffer[0]; // WR address
        hwp_xcor->data[1] = 0;
    }
    else
    {
        // First and last symbols are "margin" ones, they should be skipped
        // in the calculation as they can be irrelevant
#define SPAL_DCOC_MARGIN 4
        size -= 2*SPAL_DCOC_MARGIN;

        hwp_xcor->hv0 = 0;
        hwp_xcor->hv1 = 0;
        hwp_xcor->data[0] = 0;
        hwp_xcor->data[1] = 0;
        hwp_xcor->data[2] = 0;
        hwp_xcor->data[3] = 0;
        hwp_xcor->data[4] = 0;
        hwp_xcor->data[5] = 0;
        hwp_xcor->accuI = 0;
        hwp_xcor->accuQ = 0;
        hwp_xcor->addr0 = 0;
        hwp_xcor->command = 0;

        // Program XCOR for 1st Pass
        hwp_xcor->addr0 = (UINT32)(&(RxBuffer[SPAL_DCOC_MARGIN]));
#if (CHIP_XCOR_8PSK_SUPPORTED)
        hwp_xcor->convolution_0 =  XCOR_NB_SYMB(size);
        hwp_xcor->command =    XCOR_DCO1P_EN;
#else
        hwp_xcor->command =    XCOR_DCO1P_EN |
                               XCOR_NB_SYMB(size);
#endif
        // Poll until the end of processing
        while (spal_XcorActive()) ;

        // get first pass results
        Accu_I = (INT32)hwp_xcor->accuI;
        Accu_Q = (INT32)hwp_xcor->accuQ;
        MeanI = Accu_I / size;
        MeanQ = Accu_Q / size;

        // Program XCOR for 2nd Pass
        hwp_xcor->addr0 = (UINT32)(&(RxBuffer[SPAL_DCOC_MARGIN]));
        hwp_xcor->data[1] = (MeanQ<<16)|(0xFFFF&MeanI);
#if (CHIP_XCOR_8PSK_SUPPORTED)
        hwp_xcor->convolution_0 =  XCOR_NB_SYMB(size);
        hwp_xcor->command =    XCOR_DCO2P_EN;
#else
        hwp_xcor->command =    XCOR_DCO2P_EN |
                               XCOR_NB_SYMB(size);
#endif

        // Poll until the end of processing
        while (spal_XcorActive()) ;

        // get 2nd pass results
        Isup = hwp_xcor->data[2] >> 2;
        Qsup = hwp_xcor->data[3] >> 2;

        MeanIsup = Isup?((INT32)hwp_xcor->accuI / Isup):0;
        MeanQsup = Qsup?((INT32)hwp_xcor->accuQ / Qsup):0;

        IOffset = (Isup==size)? MeanIsup :
                  (MeanIsup + (Accu_I-(INT32)hwp_xcor->accuI)/(size-Isup))/2;
        QOffset = (Qsup==size)? MeanQsup :
                  (MeanQsup + (Accu_Q-(INT32)hwp_xcor->accuQ)/(size-Qsup))/2;
        // Program XCOR for 3rd Pass
        // 3rd Pass must be performed over all samples

        size += 2*SPAL_DCOC_MARGIN;
        hwp_xcor->addr0 = (UINT32)&RxBuffer[0]; // RD address
        hwp_xcor->data[2] = (UINT32)&RxBuffer[0]; // WR address
        hwp_xcor->data[1] = ((-QOffset)<<16)|(0xFFFF&(-IOffset));


    }

    Dco->I = IOffset;
    Dco->Q = QOffset;

    cmd = (derotate) ? XCOR_DEROTATION_EN : 0;
    cmd |=  XCOR_DCO3P_EN | XCOR_PACK_IQ;
#if (CHIP_XCOR_8PSK_SUPPORTED)
    hwp_xcor->convolution_0 =  XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
#else
    cmd |=  XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
#endif

    hwp_xcor->command =    cmd;

    // Flush the Dcache to enable later cached accesses
    //spal_InvalidateDcache();

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    // Return Power
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDcoc);
    return (hwp_xcor->hv0);
}

//=============================================================================
// spal_XcorDcocHburst
//-----------------------------------------------------------------------------
// DC offset correction and derotation for second part of the burst when using
// the EQU_HBURST_MODE.
//
// @param RxBuffer UINT32*. INPUT. Pointer to the received samples buffer.
// @param size UINT8. INPUT. Length of the burst.
// @param derotate BOOL. INPUT. Triggers derotation on and off:
// - TRUE, derotation applied,
// - FALSE, derotation not applied.
// Note that the number of samples derotated during first part of the burst
// must be multiple of 4 in order to have relevant rotation on second part
// of the burst.
// .
// @param bypassDcoc BOOL. INPUT. Switch DC Offset Correction on/off.
// @param Dco SPAL_DC_OFFSET_T*. INPUT. Pointer to the following structure:
// - Dco.I, DC offset on I,
// - Dco.Q, DC offset on Q.
// This DC value (calculated on first part of the burst) will be corrected.
// .
//
// @return UINT32. Return the power of the burst after DC offset correction.
//=============================================================================
PUBLIC UINT32 spal_XcorDcocHburst(UINT32* RxBuffer, UINT8 size, BOOL derotate, BOOL bypassDcoc, SPAL_DC_OFFSET_T Dco)
{
    UINT32 cmd;

    //SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDcoc);
    hwp_xcor->addr0 = (UINT32)&RxBuffer[0]; // RD address
    hwp_xcor->data[2] = (UINT32)&RxBuffer[0]; // WR address
    if (bypassDcoc)
    {
        hwp_xcor->data[1] = 0;
    }
    else
    {
        hwp_xcor->data[1] = ((-(Dco.Q))<<16)|(0xFFFF&(-(Dco.I)));
    }

    cmd = (derotate) ? XCOR_DEROTATION_EN : 0;
#if (CHIP_XCOR_8PSK_SUPPORTED)
    cmd |=  XCOR_DCO3P_EN | XCOR_PACK_IQ;
    hwp_xcor->convolution_0 = XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
#else
    cmd |=  XCOR_DCO3P_EN | XCOR_PACK_IQ |
            XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
#endif
    hwp_xcor->command =    cmd;

    // Flush the Dcache to enable later cached accesses
    //spal_InvalidateDcache();

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    // Return Power
    //SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDcoc);
    return (hwp_xcor->hv0);
}


// Following functions are only supported by 8PSK enabled XCOR versions
#if (CHIP_XCOR_8PSK_SUPPORTED)

// -----------------------------------------------------------------------
// TODO: the complex channel tap vector MUST be flipped outside the driver
// -----------------------------------------------------------------------

//=============================================================================
// spal_XcorBmmlZf()
//-----------------------------------------------------------------------------
/// Calculate the partial metrics used for the branch metrics and the ZF.
///
/// @param taps INT16*. address of input taps array.
/// @param metrics INT16. array containing the metrics.
///
//=============================================================================
// compute the BMML or partial ZF (two taps)
PUBLIC VOID spal_XcorBmmlZf( INT16* taps,
                             INT16* metric,
                             INT8             fracBits)
{

    SPAL_COMPLEX_T * pm         = (SPAL_COMPLEX_T *) &metric[2];
    SPAL_COMPLEX_T * taps_Cmp   = (SPAL_COMPLEX_T *) taps ;

    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorBmmlZf);

    hwp_xcor->hv1       = (UINT32)(&pm[0]);//start pointer of addr bbsram
    hwp_xcor->data[0]    = ((taps_Cmp[0].i)<<16)|((taps_Cmp[0].q)&0xFFFF);//(UINT32)(taps_Cmp[0]);
    hwp_xcor->data[1]    = ((taps_Cmp[1].i)<<16)|((taps_Cmp[1].q)&0xFFFF);//(UINT32)(taps_Cmp[1]);
    hwp_xcor->data[2]    = ((taps_Cmp[2].i)<<16)|((taps_Cmp[2].q)&0xFFFF);//(UINT32)(taps_Cmp[2]);
    hwp_xcor->data[3]    = ((taps_Cmp[3].i)<<16)|((taps_Cmp[3].q)&0xFFFF);//(UINT32)(taps_Cmp[3]);
    hwp_xcor->data[4]    = ((taps_Cmp[4].i)<<16)|((taps_Cmp[4].q)&0xFFFF);//(UINT32)(taps_Cmp[4]);
    hwp_xcor->data[5]    = ((taps_Cmp[5].i)<<16)|((taps_Cmp[5].q)&0xFFFF);//(UINT32)(taps_Cmp[5]);
    hwp_xcor->convolution_0   = XCOR_NB_SYMB(2) |
                                XCOR_NB_ILOOP(64) |
                                XCOR_MAX_ITERATION(2);

    hwp_xcor->command    =   XCOR_BMML_EN |
                             XCOR_MODE_8PSK |
                             XCOR_DEROTATION |
                             XCOR_STORE_IN_BBSRAM |
                             XCOR_PACK_IQ |
                             XCOR_CONV_SHIFT(fracBits);

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorBmmlZf);
}

//====================================================================================
//    spal_XcorComplexMatrixMultiply
//
//    This function is used to compute double convolution in the DC IQ estimation function.
//
//  @param inVector  INT16*  input data
//  @param inMatrix  INT16*  input matrix
//  @param outMatrix INT16*  output matrix (multiplication result)
//  @param columnLen UINT8             number of input matrix columns
//  @param rowLen    UINT8             number of input matrix rows
//  @param fracBits  UINT8             fractional bits of the data format
//  @return          SPAL_CMPLX32_T   the last accumulation result
//
//====================================================================================
PUBLIC SPAL_CMPLX32_T spal_XcorComplexMatrixMultiply(   INT16* inVector,
        INT16* inMatrix,
        INT16* outMatrix,
        UINT8   columnLen,
        UINT8   rowLen,
        INT8    fracBits)
{
    SPAL_CMPLX32_T Accu;

    UINT32* inVectCmp  =  (UINT32*) inVector;
    UINT32* inMatrCmp  =  (UINT32*) inMatrix;
    UINT32* outMatrCmp =  (UINT32*) outMatrix;


    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexMatrixMultiply);

    hwp_xcor->addr0      = (UINT32)(&inVectCmp[0]);          // get sample in the bbsram
    hwp_xcor->data[2] = (UINT32)(&inMatrCmp[0]);          // get sample in the bbsram
    hwp_xcor->hv1     = (UINT32)(&outMatrCmp[0]);          //start pointer of addr bbsram

    hwp_xcor->convolution_0 =   XCOR_NB_SYMB(rowLen) |
                                XCOR_NB_ILOOP(columnLen);

    hwp_xcor->command         =   XCOR_MMULT_EN |
                                  XCOR_READ_SRAM |
                                  XCOR_STORE_IN_BBSRAM |
                                  XCOR_PACK_IQ |
                                  XCOR_CONV_SHIFT(fracBits);

    Accu.i=hwp_xcor->accuI;
    Accu.q=hwp_xcor->accuQ;

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexMatrixMultiply);

    // TODO: Check if the result is correct with Samia
    return Accu;
}

//=============================================================================
// spal_XcorComplexConvolution
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param Xseq INT16.INPUT.The first input sequence.
// @param Xlen UINT16 .INPUT. The length of the first input sequence.
// @param Yseq INT16.INPUT.The second input sequence.
// @param Ylen UINT16.INPUT.The length of the second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence (16 or 32-bit complex).
// @param Zoff UINT16.INPUT. Offset in the ouptut sequence
// @param Zlen UINT16.INPUT. Max length of the output sequence (discard if negative)
// @param frac_bits INT8.INPUT.Rescale parameter.
//
//=============================================================================
PUBLIC VOID spal_XcorComplexConvolution (    INT16* Xseq,
        UINT16 Xlen,
        INT16* Yseq,
        UINT16 Ylen,
        INT16* Zseq,
        UINT16 Zoff,
        UINT16 Zlen,
        INT8 fracBits   )
{
    SPAL_COMPLEX_T *XseqCmp =  (SPAL_COMPLEX_T*) Xseq;
    SPAL_COMPLEX_T *YseqCmp =  (SPAL_COMPLEX_T*) Yseq;
    SPAL_COMPLEX_T *ZseqCmp =  (SPAL_COMPLEX_T*) Zseq;

    UINT16 OutLoopLen = Xlen + Ylen - 1 - Zoff;

    //               "config registers"
    UINT16            regLoopExt = ((OutLoopLen > Zlen) && (Zlen > 0)) ? Zlen : OutLoopLen;
    UINT16            regPtrIdx  = Zoff + 1;
    UINT16            regMinLen  = (Xlen < Ylen) ? Xlen : Ylen;
    UINT16            regAllLen  = Xlen + Ylen;
    UINT16            regYLen    = Ylen;
    INT8              regShift   = fracBits;
    SPAL_COMPLEX_T * regPtrX     = XseqCmp - Ylen;
    SPAL_COMPLEX_T * regPtrY     = YseqCmp - 1;
    SPAL_COMPLEX_T * regPtrZ     = ZseqCmp;


    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexConvolution);

    hwp_xcor->addr0      = (UINT32)(&regPtrX[0]);      // get sample in the bbsram
    hwp_xcor->data[2] = (UINT32)(&regPtrY[0]);      // get sample in the bbsram
    hwp_xcor->hv1     = (UINT32)(&regPtrZ[0]);      // start pointer of addr bbsram

    hwp_xcor->convolution_0 = XCOR_NB_ILOOP(regLoopExt) | // NB_LOOP==regLoopExt
                              XCOR_NB_SYMB(regYLen) |   // NB_SYMB==regLoopInt
                              XCOR_REVERSE |
                              XCOR_OFFSET_IDX_PTR(regPtrIdx);

    hwp_xcor->convolution_1 = XCOR_MIN_LENGHT(regMinLen) |
                              XCOR_ALLENGHT(regAllLen);

    hwp_xcor->command       = XCOR_CCONV_EN |
                              XCOR_PACK_IQ |
                              XCOR_READ_SRAM |
                              XCOR_STORE_IN_BBSRAM |
                              XCOR_CONV_SHIFT(regShift);

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexConvolution);
}


//=============================================================================
// spal_XcorComplexPower
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param seq INT16*.INPUT. input sequence.
// @param out INT16*.OUTPUT. output sequence (may be null).
// @param len UINT16 .INPUT. The length of the input sequence.
// @param frac_bits INT8.INPUT.Rescale parameter.
// @return INT32.Power of the complex sequence
//
//=============================================================================
PUBLIC UINT32 spal_XcorComplexPower (    INT16 *seq,
        INT16 *out,
        UINT16 len,
        INT8 fracBits)
{
    INT32 allPwr = 0;
    SPAL_COMPLEX_T* SeqCmp = (SPAL_COMPLEX_T*) seq;


    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorComplexPower);
    hwp_xcor->addr0      = (UINT32)(&SeqCmp[0]);        // get sample in the bbsram
    hwp_xcor->data[2] = (UINT32)(&SeqCmp[0]);        // get sample in the bbsram
    hwp_xcor->hv1     = (UINT32)(&out[0]);           // start pointer of addr bbsram or USE hwp_xcor->hv0 TO get the register value

    hwp_xcor->convolution_0 =   XCOR_CONJUG_SEQ2 |
                                XCOR_NB_SYMB(len) |
                                XCOR_NB_ILOOP(8);

    hwp_xcor->command        =  XCOR_POWER_EN |
                                XCOR_READ_SRAM |
                                XCOR_STORE_IN_BBSRAM |
                                XCOR_CONV_SHIFT(fracBits);

    allPwr=hwp_xcor->data[6];

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorComplexPower);

    return (allPwr);
}


//======================================================================
// spal_XcorDerotate
//----------------------------------------------------------------------
// This function is used for 8PSK or GMSK signal phase derotate
//
// @param in        INT16*  INPUT.  Pointer to the input sequence.
// @param out       INT16*  OUTPUT. Pointer to the output sequence.
// @param Length    UINT16            INPUT.  Length of the input sequence.
// @param fracBits  INT8              INPUT.  Rescale parameter.
// @param ModType   UINT8             INPUT.  Defines the modulation type.
//
//======================================================================
PUBLIC VOID spal_XcorDerotate ( INT16* in,
                                INT16* out,
                                UINT16 Length,
                                UINT8 ModType )
{
    SPAL_COMPLEX_T* inCmp = (SPAL_COMPLEX_T*) in;
    SPAL_COMPLEX_T* outCmp = (SPAL_COMPLEX_T*) out;



    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDerotate);
    hwp_xcor->addr0         = (UINT32)(&inCmp[0]);   // get sample in the bbsraM
    hwp_xcor->hv1           = (UINT32)(&outCmp[0]);  // start pointer of addr bbsram

    hwp_xcor->convolution_0 =   XCOR_NB_SYMB(1) |
                                XCOR_NB_ILOOP(28);
    if (ModType == 0x00000001)//(ModType == GMSK)//
    {
        hwp_xcor->command       =   XCOR_DEROT_EN |
                                    XCOR_DEROTATION |
                                    XCOR_PACK_IQ |
                                    XCOR_READ_SRAM |
                                    XCOR_STORE_IN_BBSRAM |
                                    XCOR_CONV_SHIFT(11);
    }
    else
    {
        hwp_xcor->command      =   XCOR_DEROT_EN |
                                   XCOR_DEROTATION |
                                   XCOR_MODE_8PSK |
                                   XCOR_PACK_IQ |
                                   XCOR_READ_SRAM |
                                   XCOR_STORE_IN_BBSRAM |
                                   XCOR_CONV_SHIFT(11);
    }

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDerotate);
}



//======================================================================
// spal_XcorDcIqCompensate
//----------------------------------------------------------------------
// This function is used in order to calculate from the 13 correlations
//
// @param inOut          INT16*    INPUT/OUTPUT.   Input/Output Data Buffer
// @param dc_compens     INT16*    INPUT.          Dc compensate
// @param iq_compens     INT16*    INPUT.          Iq compensate
// @param length         UINT16    INPUT.          Length of the Input/Output sequence.
// @param fracBits       INT8      INPUT           shift performed on the outut samples
//
//======================================================================
PUBLIC VOID spal_XcorDcIqCompensate(     INT16* inOut,
        INT16* dc_compens,
        INT16* iq_compens,
        UINT16 length,
        INT8 fracBits)
{
    //UINT16 i;
    SPAL_COMPLEX_T* inOutCmp      = (SPAL_COMPLEX_T*) inOut;
    SPAL_COMPLEX_T* dc_compensCmp = (SPAL_COMPLEX_T*) dc_compens;
    SPAL_COMPLEX_T* iq_compensCmp = (SPAL_COMPLEX_T*) iq_compens;

    //SPAL_COMPLEX_T inDcCorrected;
    //INT16 temp1;

    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorDcIqCompensate);
    hwp_xcor->addr0        = (UINT32)(&inOutCmp[0]);       // get sample in the bbsram
    hwp_xcor->data[2]       = (UINT32)(iq_compensCmp);      // Put the :w!CSTE in the register
    hwp_xcor->data[3]       = (UINT32)(dc_compensCmp);      // Put DC value in the register
    hwp_xcor->hv1           = (UINT32)(&inOutCmp[0]);          // start pointer of addr bbsram

    hwp_xcor->convolution_0 =   XCOR_NB_ILOOP(length) |
                                XCOR_NB_SYMB(1);

    hwp_xcor->command           =   XCOR_IQ_EN |
                                    XCOR_PACK_IQ |
                                    XCOR_STORE_IN_BBSRAM;

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDcIqCompensate);
}

//=============================================================================
// spal_XcorSrecPSK8
//-----------------------------------------------------------------------------
/// ReconstructS a reference 8-PSK burst from channel taps and estimated/known hard bits.
///
/// @param taps     INT16*.  INPUT.  Pointer to the channel taps.
/// @param size     UINT8.   INPUT.  Length of the reconstructed burst.
/// @param HardBits UINT8*.  INPUT.  Pointert to the Hard decision bits. to be reconstructed.
/// @param refBurst UINT32*. OUTPUT. Pointer to the samples of the reconstructed burst.
/// @param chTapNb  UINT8.   INPUT.  Number of the channel taps.
/// @param regShift UINT8.   INPUT.  Rescale parameter.
///
//=============================================================================
PUBLIC VOID spal_XcorSrecPSK8(    INT16* taps,
                                  UINT8 size,
                                  UINT32* HardBits,
                                  UINT32* refBurst,
                                  UINT8 chTapNb,
                                  UINT8 regShift)
{
    SPAL_PROFILE_FUNCTION_ENTER(spal_XcorSrecPSK8);
    hwp_xcor->addr0   = (UINT32)(&HardBits[0]);      // get sample in the bbsram
    hwp_xcor->hv1     = (UINT32)(&refBurst[192]);          // start pointer of addr bbsram
    hwp_xcor->data[0] = (UINT32)(taps[0]);
    hwp_xcor->data[1] = (UINT32)(taps[1]);
    hwp_xcor->data[2] = (UINT32)(taps[2]);
    hwp_xcor->data[3] = (UINT32)(taps[3]);
    hwp_xcor->data[4] = (UINT32)(taps[4]);
    hwp_xcor->data[5] = (UINT32)(taps[5]);
    hwp_xcor->data[6] = (UINT32)(taps[6]);
    hwp_xcor->data[7] = (UINT32)(taps[7]);

    hwp_xcor->convolution_0 =   XCOR_NB_SYMB(chTapNb) |
                                XCOR_NB_ILOOP(32) |
                                XCOR_MAX_ITERATION(4);

    hwp_xcor->command       =    XCOR_SREC_EN |
                                 XCOR_MODE_8PSK |
                                 XCOR_STORE_IN_BBSRAM |
                                 XCOR_PACK_IQ |
                                 XCOR_DEC_ADDR4 |
                                 XCOR_CONV_SHIFT(regShift);


    // Poll until the end of processing
    while (spal_XcorActive());

    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorSrecPSK8);
}
#endif //CHIP_XCOR_8PSK_SUPPORTED


#if (SPC_IF_VER >= 4)
//======================================================================
// spal_XcorFchFilter
//----------------------------------------------------------------------
// This function implements IIR Filtering for FCCH detection, special for gallite.
// 1st step: 1/4 Fs downsampling
// 2nd step: LP IIR Filtering
// 3rd step: Moving Average for RxData's Pwr and Filter Output's Pwr.
// 3 Mem Block Allocation:
// | RxData Pwr|  (-j)^n Filtered IQ | (-j)^nFiltered Pwr |
// 2 output MA Allocation at accu_outBuf:
// | RxData Pwr's MA | (-j)^nFiltered Pwr's MA |
//
// @param pold UINT32*. Start address of old buffer
// @param RxBuffer UINT32*. Start address of the reception buffer.
// @param CntFlag UINT8. Counter for calling this function
// @param accu_outBuf UINT32*. Output buffer addr for RxData Pwr's MA and Filtered Pwr's MA.
// @param accu_energy INT32.Init value for RxData Pwr's MA
// @param accu_corrQ INT32. Init value for Filtered Pwr's MA
//
//======================================================================
PUBLIC VOID  spal_XcorFchFilter(UINT32* pold, UINT32* RxBuffer, UINT8 CntFlag,
                                UINT32* accu_outBuf,
                                INT32   accu_energy,
                                INT32   accu_corrQ)
{
#define     Offset  (3*SPP_FCCH_SIZE)
    INT32 FltIn;
    INT32 Xn, Yn;
    UINT32 cmd;
    INT32 tmp_buf[SPP_FCCH_SIZE];
    INT32 *FltOut = (INT32*)(RxBuffer + Offset);
    INT32 *FltPwr = (INT32*)(RxBuffer + 2*Offset);
    INT32 *pt0;

    INT32 MASK1,MASK2;

    // 1st Step: derotate: 1/4 Fs downsampling
    hwp_xcor->addr0   = (UINT32)RxBuffer;    // RD address
    hwp_xcor->data[2] = (UINT32)FltOut;             // WR address
    hwp_xcor->data[1] = 0;                              //OFFSET I & Q

    cmd=  XCOR_DEROTATION_EN | XCOR_DCO3P_EN | XCOR_PACK_IQ |
          XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(SPP_FCCH_SIZE);

    hwp_xcor->command = cmd;

    // Flush the Dcache to enable later cached accesses
    spal_InvalidateDcache();

    while (spal_XcorActive()) ;

    // 2nd Step: LP filter by CPU
    MASK1 = 0xffff0000;
    MASK2 = 0xffff;

    pt0 = (INT32 *)FltOut;
    FltIn = *pt0;

    if(CntFlag==0)
    {
        Xn = g_spalDoubleRegx + (((FltIn<<16) - g_spalDoubleRegx)>>3);
        Yn = g_spalDoubleRegy + (((FltIn & MASK1) - g_spalDoubleRegy)>>3);
    }
    else
    {
        Xn = g_spalDoubleRegx - (((FltIn<<16) + g_spalDoubleRegx)>>3);
        Yn = g_spalDoubleRegy - (((FltIn & MASK1) + g_spalDoubleRegy)>>3);
    }

    FltIn = (Yn & MASK1) | ((Xn>>16) & MASK2);
    *pt0++ = FltIn;

#ifdef SPAL_XCORFLT_ASM
    spal_XcorFchFilterAsm((UINT32 *)pt0, CntFlag, (UINT32 *)(&Xn), (UINT32 *)(&Yn));
#else
    UINT32 i;
    if(CntFlag==0)
    {
        for (i=SPP_FCCH_SIZE-1; i>0; i--)
        {
            FltIn  = *pt0;

            Xn = Xn + (((FltIn<<16) - Xn)>>3);
            Yn = Yn + (((FltIn & MASK1) - Yn)>>3);

            FltIn = (Yn & MASK1) | ((Xn>>16) & MASK2);

            *pt0++ = FltIn;
        }
    }
    else
    {
        for (i=SPP_FCCH_SIZE-1; i>0; i--)
        {
            FltIn  = *pt0;
            Xn = Xn - (((FltIn<<16) + Xn)>>3);
            Yn = Yn - (((FltIn & MASK1) + Yn)>>3);

            FltIn = (Yn & MASK1) | ((Xn>>16) & MASK2);

            *pt0++ = FltIn;
        }
    }
#endif    // XCORFLT_ASM

    g_spalDoubleRegx = Xn;
    g_spalDoubleRegy = Yn;

    // 3rd Step: Moving Average for RxData's Pwr and Filter Output's Pwr.

    //FltIn (RxData) power: use FCHXC's power and correlation function.
    cmd =  XCOR_FCHXC_EN |XCOR_DEROTATION_EN | XCOR_NB_SYMB(SPP_FCCH_SIZE);

    hwp_xcor->addr0   = (UINT32)RxBuffer;      // @R(k)
    hwp_xcor->data[0] = (UINT32)RxBuffer[0];  // R(k)
    hwp_xcor->data[2] = (UINT32)RxBuffer;      // @ResPW
    hwp_xcor->data[3] = (UINT32)tmp_buf;        // @ResCI
    hwp_xcor->data[5] = (UINT32)tmp_buf;        // @ResCQ

    hwp_xcor->command = cmd;

    // Poll until the end of processing
    while (spal_XcorActive()) ;

    //Fltout power
    hwp_xcor->addr0   = (UINT32)FltOut;        // @R(k)
    hwp_xcor->data[0] = (UINT32)FltOut[0];     // R(k)
    hwp_xcor->data[2] = (UINT32)FltPwr;          // @ResPW
    hwp_xcor->data[3] = (UINT32)tmp_buf;       // @ResCI
    hwp_xcor->data[5] = (UINT32)tmp_buf;       // @ResCQ

    hwp_xcor->command = cmd;

    // Use XCOR's sliding function to calculate:
    // Moving Average of (Input Power & Filter Ouput Power)
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)

    if (pold != NULL)
    {
        // Poll until the end of processing
        while (spal_XcorActive()) ;
        // Moving Average
        // Update Energy
        hwp_xcor->hv0     = accu_energy; // Init with average value
        hwp_xcor->addr0  = (UINT32)&pold[0]; // Old pointer
        hwp_xcor->data[2]    = (UINT32)&RxBuffer[0]; // New pointer
        hwp_xcor->data[3]    = (UINT32)accu_outBuf; // Output
        hwp_xcor->command = XCOR_SLDWIN_EN|XCOR_NB_SYMB(SPP_FCCH_SIZE);

        // Poll until the end of processing
        while (spal_XcorActive());


        // Update Filter Ouput Pwr
        hwp_xcor->hv0        = accu_corrQ; // Init with average value
        hwp_xcor->addr0     = (UINT32)&pold[2*Offset]; // Old pointer
        hwp_xcor->data[2]   = (UINT32)&FltPwr[0]; // New pointer
        hwp_xcor->data[3]   = (UINT32)&accu_outBuf[SPP_FCCH_SIZE]; // Output
        hwp_xcor->command = XCOR_SLDWIN_EN|XCOR_NB_SYMB(SPP_FCCH_SIZE);
    }

    // Flush the Dcache to enable later cached accesses
    spal_InvalidateDcache();
    // Poll until the end of processing
    while (spal_XcorActive()) ;

#endif //  CHIP_XCOR_HAS_SLIDING_WINDOW
}


//=============================================================================
// spal_XcorFchxc_AutoCorr
//-----------------------------------------------------------------------------
// Perform complex auto-correlation(delay 2)
//
// @param symb_ptr      UINT32*. Pointer to current symbol window.
// @param XOffset      UINT16. Offset
// @param size          UINT16.  Size of the window.
// @param computeEnergy UINT16.  Flag that defines if energy needs to be calculated.
//=============================================================================

PUBLIC VOID  spal_XcorFchxc_AutoCorr(UINT32* symb_ptr, UINT16 XOffset, UINT16 size)
{

    UINT32 spalRkm1 = 0;
    UINT32 spalRkm2 = 0;
#if (CHIP_XCOR_8PSK_SUPPORTED)
    UINT32 cmd =  XCOR_FCHXC_EN | XCOR_DEROTATION_EN;
    hwp_xcor->convolution_0 = XCOR_NB_SYMB(size);
#else
    UINT32 cmd =  XCOR_FCHXC_EN |
                  XCOR_DEROTATION_EN | // for datapath setting
                  XCOR_NB_SYMB(size);
#endif

    hwp_xcor->data[0] = (UINT32)symb_ptr[0];           // R(k)
    hwp_xcor->hv0     = (UINT32)spalRkm1;                  // R(k-1)
    hwp_xcor->data[1] = (UINT32)spalRkm2;                  // R(k-2)
    hwp_xcor->addr0   = (UINT32)symb_ptr;              // @R(k)
    hwp_xcor->data[2] = (UINT32)&(symb_ptr[0*XOffset]); // @ResPW
    hwp_xcor->data[3] = (UINT32)&(symb_ptr[1*XOffset]); // @ResCI
    hwp_xcor->data[5] = (UINT32)&(symb_ptr[2*XOffset]); // @ResCQ
    // Run XCOR
    hwp_xcor->command = cmd;

    // Flush the Dcache to enable later cached accesses
    spal_InvalidateDcache();

    // Poll until the end of processing
    while (spal_XcorActive()) ;

}



//=============================================================================
// spal_XcorFchxcn_Delay
//-----------------------------------------------------------------------------
// Xcorrelation mode for FCH burst detection
// Power term overwrites symbols,
// CI term is stored at Offset fom Power term
// and CQ term is stored at Offset from CI term.
//
// @param symb_ptr      UINT32*. Pointer to current symbol window.
// @param old_ptr       UINT32*. Pointer to previous symbol window.
// @param out_ptr       UINT32*. Pointer to the output buffer.
// @param size          UINT16.  Size of the window.
// @param computeEnergy UINT16.  Flag that defines if energy needs to be calculated.
//=============================================================================
PUBLIC VOID spal_XcorFchxc_Delay( UINT32* symb_ptr,
                                  UINT32* old_ptr,
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                                  UINT32* out_ptr,
                                  INT32   accu_energy,
                                  INT32   accu_corrI,
                                  INT32   accu_corrQ,
#endif
                                  UINT16  size,
                                  UINT16  computeEnergy)
{
    UINT32 cmd;
#define offset (3*size)

#if (CHIP_XCOR_DELAY8)
    //8810 XCOR_NB_ILOOP(n): 0: delay2. 1:delay4. 2:delay8
    cmd =  XCOR_FCHXC_EN |XCOR_DEROTATION_EN | XCOR_NB_ILOOP(1) | XCOR_NB_SYMB(size);
#else
    //8808/8808s/8809 XCOR_NB_ILOOP(n): 0: delay2. 1:delay3. 2:delay4
    cmd =  XCOR_FCHXC_EN |XCOR_DEROTATION_EN | XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
#endif

    if (old_ptr == NULL)
    {
        // First call for an FCCH detection : R(k-1) and R(k-2)
        // are not defined
        g_nspalRkm1 = 0;
        g_nspalRkm2 = 0;
        g_nspalRkm3 = 0;
        g_nspalRkm4 = 0;
    }

#if (CHIP_XCOR_DELAY8)
    //8810 initial
    hwp_xcor->data[0] = (UINT32)symb_ptr[0];           // R(k)
    hwp_xcor->data_e[0] = (UINT32)g_nspalRkm1;      // R(k-1)
    hwp_xcor->data_e[1] = (UINT32)g_nspalRkm2;      // R(k-2)
    hwp_xcor->data_e[2] = (UINT32)g_nspalRkm3;          // R(k-3)
    hwp_xcor->data_e[3] = (UINT32)g_nspalRkm4;          // R(k-4)
#else
    //8809 initial
    hwp_xcor->data[0] = (UINT32)symb_ptr[0];           // R(k)
    hwp_xcor->data_e[0] = (UINT32)g_nspalRkm1;      // R(k-1)
    hwp_xcor->data_e[1] = (UINT32)g_nspalRkm2;      // R(k-2)
    hwp_xcor->hv0      = (UINT32)g_nspalRkm3;          // R(k-3)
    hwp_xcor->data[1] = (UINT32)g_nspalRkm4;          // R(k-4)
#endif


    hwp_xcor->addr0   = (UINT32)symb_ptr;              // @R(k)
    hwp_xcor->data[2] = (UINT32)&(symb_ptr[0*offset]); // @ResPW
    if (computeEnergy)
    {
        hwp_xcor->data[3] = (UINT32)&(symb_ptr[1*offset]); // @ResCI
        hwp_xcor->data[5] = (UINT32)&(symb_ptr[2*offset]); // @ResCQ
    }
    else
    {
        // overwrite energy buffer
        hwp_xcor->data[3] = (UINT32)&(symb_ptr[0*offset]); // @ResCI
        hwp_xcor->data[5] = (UINT32)&(symb_ptr[1*offset]); // @ResCQ
    }
    // Following calls are supposed to be done on successive
    // buffers representing a continuous symbol reception.
    g_nspalRkm1 = symb_ptr[size-1]; // last symbol becomes R(k-1)
    g_nspalRkm2 = symb_ptr[size-2]; // before last symbol becomes R(k-2)
    g_nspalRkm3 = symb_ptr[size-3]; // last symbol becomes R(k-1)
    g_nspalRkm4 = symb_ptr[size-4]; // before last symbol becomes R(k-2)

    // Run XCOR
    hwp_xcor->command = cmd;


#if (CHIP_XCOR_HAS_SLIDING_WINDOW)

    if ((old_ptr != NULL) &&  (out_ptr != NULL))
    {
        UINT32* tmpOld = old_ptr;
        UINT32* tmpNew = symb_ptr;
        UINT32* tmpOut = out_ptr;

        // Poll until the end of processing
        while ((hwp_xcor->status) & XCOR_OP_PENDING);


        if(computeEnergy)
        {
            // Update Energy
            hwp_xcor->hv0        =    accu_energy; // Init with average value
            hwp_xcor->addr0        =    (UINT32)&tmpOld[0]; // Old pointer
            hwp_xcor->data[2]    =    (UINT32)&tmpNew[0]; // New pointer
            hwp_xcor->data[3]    =    (UINT32)&tmpOut[0]; // Output
            hwp_xcor->command    =    XCOR_SLDWIN_EN|XCOR_NB_SYMB(size);

            // Poll until the end of processing
            while (spal_XcorActive());

            tmpOld += offset;
            tmpNew += offset;
            tmpOut += size;
        }

        // Update I
        hwp_xcor->hv0        =    accu_corrI; // Init with average value
        hwp_xcor->addr0        =    (UINT32)&tmpOld[0]; // Old pointer
        hwp_xcor->data[2]   =    (UINT32)&tmpNew[0]; // New pointer
        hwp_xcor->data[3]   =    (UINT32)&tmpOut[0]; // Output
        hwp_xcor->command    =    XCOR_SLDWIN_EN|XCOR_NB_SYMB(size);

        // Poll until the end of processing
        while (spal_XcorActive()) ;

        tmpOld += offset;
        tmpNew += offset;
        tmpOut += size;

        hwp_xcor->hv0        =    accu_corrQ; // Init with average value
        hwp_xcor->addr0        =    (UINT32)&tmpOld[0]; // Old pointer
        hwp_xcor->data[2]   =    (UINT32)&tmpNew[0]; // New pointer
        hwp_xcor->data[3]   =    (UINT32)&tmpOut[0]; // Output
        hwp_xcor->command    =    XCOR_SLDWIN_EN|XCOR_NB_SYMB(size);
    }

#endif // (CHIP_XCOR_HAS_SLIDING_WINDOW)

    // Poll until the end of processing
    while ((hwp_xcor->status) & XCOR_OP_PENDING);
}
#endif // SPC_IF_VER >= 4


//=============================================================================
// spal_XcorChest8taps
//-----------------------------------------------------------------------------
// channel estimation function
//
// @param RxBuffer      INT16*. Pointer to received samples.
// @param ChestTab     INT16*. channel estimation tabel used in LS.
// @param taps           INT16*. Pointer to the output channel estimation results.
// @param chTapNb      UINT8.  Number of channel estimation taps.
// @param fracBits        INT8.  Shift indicator.
//=============================================================================
PUBLIC VOID spal_XcorChest8taps(INT16* RxBuffer, INT16 *ChestTab, INT16* taps, UINT8 chTapNb, INT8  fracBits)
{
    int i;
    // 11 when CHIP_CHEST_LEN = 5
    // 10 when CHIP_CHEST_LEN = 8
    UINT32 local_table[(27+1-8)/2];

    //SPAL_PROFILE_FUNC_ENTER(spal_XcorChest);
    for(i=0; i<chTapNb; i++)
    {
        register UINT8 j;
        register UINT16 *tmp_src = (UINT16*)&(ChestTab[i*(27-8)]);
        register UINT16 *tmp_dst = (UINT16*)local_table;
        // Copy table locally
        //for (j=0;j<(27+1-CHIP_CHEST_LEN);j++)
        // the following line changed by tcj bug removed @20110519.
        for (j=0; j<(27-8); j++)
        {
            *tmp_dst++ = *tmp_src++;
        }

        hwp_xcor->addr0    = (UINT32)RxBuffer;
        hwp_xcor->data[2]  =   (UINT32)local_table;
#if (CHIP_XCOR_8PSK_SUPPORTED)
        hwp_xcor->convolution_0 = XCOR_NB_SYMB((27-CHIP_CHEST_LEN))|
                                  XCOR_NB_ILOOP(3);
        hwp_xcor->command  =   XCOR_CHEST_EN | XCOR_DEROTATION_EN;
#else
        hwp_xcor->command  =   XCOR_CHEST_EN |
                               XCOR_DEROTATION_EN | // needed for datapath setup
                               XCOR_NB_SYMB((27-8))|
                               XCOR_NB_ILOOP(3);
#endif
        // Poll until the end of processing
        while (((hwp_xcor->status) & XCOR_OP_PENDING));

        // Get results
        taps[2*i] = (INT16)((INT32)hwp_xcor->hv0 >> fracBits);
        taps[2*i+1] = (INT16)((INT32)hwp_xcor->hv1 >> fracBits);
    }
    //SPAL_PROFILE_FUNC_EXIT(spal_XcorChest);
}


