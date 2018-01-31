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




#include "gsm.h"
#include "opl_fixed.h"
#include "spal_xcor.h"
#include "spalp_private.h"

//------------------------------------------------------------------------------
// LUTs
//------------------------------------------------------------------------------


#define SVAL_ZERO                          0
#define SVAL_ONE                        2047
#define SVAL_NEGONE                     2048
#define SVAL_INVSQRT                    1448
#define SVAL_COS3PI8                     784
#define SVAL_SIN3PI8                    1892

// Xcor shifter range
#define SPAL_XCOR_SHIFT_RANGE_MIN        0
#define SPAL_XCOR_SHIFT_RANGE_MAX       15

#define SPAL_XCOR_SHIFT_CHECK_RANGE(x,id) \
                                        if (!(((x) >= SPAL_XCOR_SHIFT_RANGE_MIN) && ((x) <= SPAL_XCOR_SHIFT_RANGE_MAX))) \
                                        { \
                                            printf("warning (%d) : requested shift value % d is out of range [%d, %d]\n", id, x, SPAL_XCOR_SHIFT_RANGE_MIN, SPAL_XCOR_SHIFT_RANGE_MAX); \
                                        } \

// Xcor multiplier range
#define SPAL_XCOR_MULT_SIZE             14
#define SPAL_XCOR_MULT_IN_MAX           ((1<<SPAL_XCOR_MULT_SIZE) - 1)
#define SPAL_XCOR_MULT_IN_MIN           (~((1<<SPAL_XCOR_MULT_SIZE) - 1))

#define SPAL_XCOR_MULT_IN_SAT(in)       (((in) >= SPAL_XCOR_MULT_IN_MAX) ? SPAL_XCOR_MULT_IN_MAX : \
                                         ((in) <  SPAL_XCOR_MULT_IN_MIN) ? SPAL_XCOR_MULT_IN_MIN : \
                                          (in))

#define SPAL_XCOR_MAC_32(in1,in2,res)   OPL_MAC_32(SPAL_XCOR_MULT_IN_SAT(in1),SPAL_XCOR_MULT_IN_SAT(in2),res)
#define SPAL_XCOR_MSU_32(in1,in2,res)   OPL_MSU_32(SPAL_XCOR_MULT_IN_SAT(in1),SPAL_XCOR_MULT_IN_SAT(in2),res)
#define SPAL_XCOR_MULF_16(in1,in2,res)  OPL_MULF_16(SPAL_XCOR_MULT_IN_SAT(in1),SPAL_XCOR_MULT_IN_SAT(in2),res)


CONST SPAL_COMPLEX_T LUT_8PSK[8] =
{
    {-SVAL_INVSQRT,  SVAL_INVSQRT}, // 0
    {-SVAL_NEGONE ,  SVAL_ZERO   }, // 1
    { SVAL_ZERO   ,  SVAL_ONE    }, // 2
    { SVAL_INVSQRT,  SVAL_INVSQRT}, // 3
    { SVAL_ZERO   , -SVAL_NEGONE }, // 4
    {-SVAL_INVSQRT, -SVAL_INVSQRT}, // 5
    { SVAL_INVSQRT, -SVAL_INVSQRT}, // 6
    { SVAL_ONE    ,  SVAL_ZERO   }  // 7
};


CONST SPAL_COMPLEX_T PSK8_ROTATE[16] =
{
    { SVAL_ONE    ,  SVAL_ZERO   },
    { SVAL_COS3PI8, -SVAL_SIN3PI8},
    {-SVAL_INVSQRT, -SVAL_INVSQRT},
    {-SVAL_SIN3PI8,  SVAL_COS3PI8},
    { SVAL_ZERO   ,  SVAL_ONE    },
    { SVAL_SIN3PI8,  SVAL_COS3PI8},
    { SVAL_INVSQRT, -SVAL_INVSQRT},
    {-SVAL_COS3PI8, -SVAL_SIN3PI8},
    {-SVAL_NEGONE ,  SVAL_ZERO   },
    {-SVAL_COS3PI8,  SVAL_SIN3PI8},
    { SVAL_INVSQRT,  SVAL_INVSQRT},
    { SVAL_SIN3PI8, -SVAL_COS3PI8},
    { SVAL_ZERO   ,  -SVAL_NEGONE},
    {-SVAL_SIN3PI8, -SVAL_COS3PI8},
    {-SVAL_INVSQRT,  SVAL_INVSQRT},
    { SVAL_COS3PI8,  SVAL_SIN3PI8}
};

CONST SPAL_COMPLEX_T GMSK_ROTATE[16] =
{
    { SVAL_ONE    ,  SVAL_ZERO   },
    { SVAL_ZERO   , -SVAL_NEGONE },
    {-SVAL_NEGONE ,  SVAL_ZERO   },
    { SVAL_ZERO   ,  SVAL_ONE    }
};




//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_XcorActive
//-----------------------------------------------------------------------------
/// Check if XCOR is activated
///
//=============================================================================
PRIVATE BOOL spal_XcorActive()
{
    // Driver
//    if (((hwp_xcor->status) & XCOR_OP_PENDING)) {
//        return TRUE;
//    }
//    else {
//        return FALSE;
//    }
    return FALSE;
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
PUBLIC VOID spal_XcorTscxcNb(UINT32 GsmTscNburst, UINT32* symb_ptr, INT16* out_ptr)
{
    // Driver
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorTscxcNb);
//     UINT32 cmd =   XCOR_TSCXC_EN|XCOR_PACK_IQ|
//                 XCOR_NB_SYMB(NB_TSC_XCOR_NBR)|
//                 XCOR_NB_ILOOP(NB_TSC_XCOR_SIZE);
//
//     // TODO : check the >>5 shift
//     hwp_xcor->hv0 = (g_sppGsmTscNburst[TscNbr]>>5) << 2;
//     hwp_xcor->addr0 = (UINT32)symb_ptr;
//     hwp_xcor->data[2] = (UINT32)out_ptr;
//     hwp_xcor->command = cmd;
//
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorTscxcNb);


    // Software equivalent
    INT8 i;
    INT8 n;
    INT16 train[NB_TSC_XCOR_SIZE+1];
    INT16* in;
    INT32 tempi;
    INT32 tempq;

    // Start at the 6th sample because the first 5 samples of the training sequence
    // are just repetition of the last 5
    GsmTscNburst=(GsmTscNburst>>5);

    in=(INT16*) symb_ptr;
    for(i=0; i<NB_TSC_XCOR_SIZE+1; i++)
    {
        train[i]=1-2* ((INT16)((GsmTscNburst>>i)& (0x00000001)));
    }

    for(n=0; n<NB_TSC_XCOR_NBR+1; n++)
    {
        tempi=0;
        tempq=0;
        for(i=0; i<NB_TSC_XCOR_SIZE+1; i++)
        {
            SPAL_XCOR_MAC_32(in[2*(n+i)],train[i],tempi);
            SPAL_XCOR_MAC_32(in[2*(n+i)+1],train[i],tempq);
        }
        out_ptr[2*n]=(INT16)tempi;
        out_ptr[2*n+1]=(INT16)tempq;
    }
}

//=============================================================================
// spp_XcorTscxcSch
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
    // Driver
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorTscxcSch);
//     UINT32 cmd =   XCOR_TSCXC_EN|
//                 XCOR_NB_SYMB(SCH_TSC_XCOR_NBR)|
//                 XCOR_NB_ILOOP(SCH_TSC_XCOR_SIZE);
//
//     hwp_xcor->hv0 = (GSM_TSC_SCH0<<2)|((GSM_TSC_SCH1>>30) & 0x3);
//     hwp_xcor->hv1 = (GSM_TSC_SCH1<<2)|((GSM_TSC_SCH0>>30) & 0x3);
//     hwp_xcor->addr0 = (UINT32)symb_ptr;
//     hwp_xcor->data[2] = (UINT32)outI_ptr;
//     hwp_xcor->data[3] = (UINT32)outQ_ptr;
//     hwp_xcor->command = cmd;
//
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorTscxcSch);

    // Software equivalent
    INT8 i;
    INT8 n;
    INT16 train[SCH_TSC_XCOR_SIZE+1];
    INT32* tmpi;
    INT32* tmpq;
    INT16* in;

    in=(INT16*) symb_ptr;
    tmpi=(INT32*) outI_ptr;
    tmpq=(INT32*) outQ_ptr;

    for(i=0; i<32; i++)
    {
        train[i]=1-2* ((INT16)((GSM_TSC_SCH0>>i)& (0x00000001)));
    }
    for(i=32; i<SCH_TSC_XCOR_SIZE+1; i++)
    {
        train[i]=1-2* ((INT16)((GSM_TSC_SCH1>>(i-31)& (0x00000001))));
    }

    for(n=0; n<SCH_TSC_XCOR_NBR+1; n++)
    {
        tmpi[n] = 0;
        tmpq[n] = 0;
        for(i=0; i<SCH_TSC_XCOR_SIZE+1; i++)
        {
            SPAL_XCOR_MAC_32(in[2*(n+i)],train[i],tmpi[n]);
            SPAL_XCOR_MAC_32(in[2*(n+i)+1],train[i],tmpq[n]);
        }
    }

}



//=============================================================================
// spal_XcorFchxc
//-----------------------------------------------------------------------------
// Xcorrelation mode for FCH burst detection
// Power term overwrites symbols,
// CI term is stored at Offset fom Power term
// and CQ term is stored at Offset from CI term.
//
// @param symb_ptr UINT32*. ??
// @param Offset UINT16. ??
// @param size UINT16. ??
// @param init UINT8
//=============================================================================
PUBLIC VOID spal_XcorFchxc( UINT32* symb_ptr,
                            UINT32* old_ptr,
#if (CHIP_XCOR_HAS_SLIDING_WINDOW)
                            UINT32* out_ptr,
                            INT32   accu_energy,
                            INT32   accu_corrI,
                            INT32   accu_corrQ,
#endif // (CHIP_XCOR_HAS_SLIDING_WINDOW)
                            UINT16 size,
                            UINT16 computeEnergy)
{
    // Driver
//    SPP_PROFILE_FUNCTION_ENTRY(spp_XcorFchxc);
//    static UINT32 Rkm1 = 0;
//    static UINT32 Rkm2 = 0;
//    UINT32 cmd =   XCOR_FCHXC_EN |
//                XCOR_DEROTATION_EN | // for datapath setting
//                XCOR_NB_SYMB(size);
//    if (init)
//    {
//        // First call for an FCCH detection : R(k-1) and R(k-2)
//        // are not defined
//        Rkm1 = 0;
//        Rkm2 = 0;
//    }
//    hwp_xcor->data[0] = (UINT32)symb_ptr[0];           // R(k)
//    hwp_xcor->hv0     = (UINT32)Rkm1;                  // R(k-1)
//    hwp_xcor->data[1] = (UINT32)Rkm2;                  // R(k-2)
//    hwp_xcor->addr0   = (UINT32)symb_ptr;              // @R(k)
//    hwp_xcor->data[2] = (UINT32)&(symb_ptr[0*Offset]); // @ResPW
//    hwp_xcor->data[3] = (UINT32)&(symb_ptr[1*Offset]); // @ResCI
//    hwp_xcor->data[5] = (UINT32)&(symb_ptr[2*Offset]); // @ResCQ
//    // Following calls are supposed to be done on successive
//    // buffers representing a continuous symbol reception.
//    Rkm1 = symb_ptr[size-1]; // last symbol becomes R(k-1)
//    Rkm2 = symb_ptr[size-2]; // before last symbol becomes R(k-2)
//    // Run XCOR
//    hwp_xcor->command = cmd;
//    // Flush the Dcache to enable later cached accesses
//    spp_InvalidateDcache();
//
//    // Poll until the end of processing
//    while (spp_XcorActive()) ;
//    SPP_PROFILE_FUNCTION_EXIT(spp_XcorFchxc);

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
    // Driver
//    SPP_PROFILE_FUNCTION_ENTRY(spp_XcorBiterror);
//    hwp_xcor->addr0 = (UINT32)sofvalues_ptr;
//    hwp_xcor->data[2] = (UINT32)hardvalues_ptr;
//    hwp_xcor->command =
//        XCOR_BITERR_EN|
//        XCOR_NB_ILOOP(8) |
//        XCOR_NB_SYMB(NbSVwords);
//    // Poll until the end of processing
//    while (spp_XcorActive()) ;
//    *biterror = hwp_xcor->accuI;
//    SPP_PROFILE_FUNCTION_EXIT(spp_XcorBiterror);

    UINT16 i;
    UINT16 local_nberror=0;
    UINT8* soft;

    soft=(INT8*) sofvalues_ptr;

    for (i=0; i<NbSVwords; i++)
    {
        if ( (((soft[i])&((UINT8)0x80))>>7) != ((UINT8)((hardvalues_ptr[i/32]>>(i%32)) & 0x1)))
            local_nberror++;
    }
    biterror[0]=local_nberror;
}

//=============================================================================
// spal_XcorChest
//-----------------------------------------------------------------------------
// Performs LSE channel estimation.
//
// @param RxBuffer INT16*. INPUT. Pointer to the buffer of received samples.
// The samples are complex with RxBuffer[2*t] the inphase part of sample t and
// RxBuffer[2*t+1] the quadrature part of sample t.
// @param Tsc UINT8. INPUT. Index of the training sequence used in the received
// buffer.
// @param taps INT16*. OUTPUT. Estimated taps of the channel. The taps are complex.
// taps[2*t] is the inphase part of taps t,
// taps[2*t+1] is the quadrature part of taps t.
//=============================================================================
PUBLIC VOID spal_XcorChest(INT16* RxBuffer, INT16* ChestTab, INT16* taps, UINT8 chTapNb, INT8  fracBits)
{
    // Driver
//     int i;
//     UINT32 local_table[11];
//
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorChest);
//     for(i=0; i<SPP_ISI; i++)
//     {
//         register UINT8 j;
//         register UINT32 *tmp_src = (UINT32*)&(g_sppChestTab[Tsc][i][0]);
//         register UINT32 *tmp_dst = local_table;
//         // Copy table locally
//         for (j=0;j<11;j++) // 22(or 21) INT16 values
//         {
//             *tmp_dst++ = *tmp_src++;
//         }
//         // We skip 2 symbols on both sides of the NB TSC
//         // NOTE : the Matrix for SCH is calculated for the same
//         //        burst position as NB (it doesn't correspond to
//         //        SCH Tsc border)
//         hwp_xcor->addr0    = (UINT32)&(RxBuffer[2*(3+57+1+2)]);
//         hwp_xcor->data[2]  =   (UINT32)local_table;
//         hwp_xcor->command  =   XCOR_CHEST_EN |
//                                     XCOR_DEROTATION_EN | // needed for datapath setup
//                                     XCOR_NB_SYMB(22)|
//                                     XCOR_NB_ILOOP(3);
//         // Poll until the end of processing
//         while (spp_XcorActive()) ;
//
//         // Get results
//         taps[2*i] = (INT16)((INT32)hwp_xcor->hv0 >> 15);
//         taps[2*i+1] = (INT16)((INT32)hwp_xcor->hv1 >> 15);
//     }
//
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorChest);

    // Software equivalent
    INT32 temp[2];
    INT8 i;
    INT8 j;

    SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,1);

    for(i=0; i<chTapNb; i++)
    {
        temp[0]=0;
        temp[1]=0;

        for(j=0; j<(27-CHIP_CHEST_LEN); j++)
        {
            SPAL_XCOR_MAC_32(ChestTab[i*(27-CHIP_CHEST_LEN)+j],RxBuffer[2*j],temp[0]);
            SPAL_XCOR_MAC_32(ChestTab[i*(27-CHIP_CHEST_LEN)+j],RxBuffer[2*j+1],temp[1]);
        }

        OPL_SHR_32(temp[0],fracBits,temp[0]);
        OPL_SAT_16(temp[0],taps[2*i]);

        OPL_SHR_32(temp[1],fracBits,temp[1]);
        OPL_SAT_16(temp[1],taps[2*i+1]);
    }
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
PUBLIC VOID spal_XcorSrec(INT16* taps, UINT8 size, UINT32* HardBits,UINT32* refBurst, UINT8 chTapNb)
{
    // Driver
//     UINT32* UINT32taps = (UINT32*)taps;
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorSrec);
//
//     hwp_xcor->hv0  =   HardBits[0];
//     hwp_xcor->hv1  =   HardBits[1]; // only used for SCH
//     // Goes backward for HW simplicity -> give end address
//     hwp_xcor->addr0    =   (UINT32)&(refBurst[size-1]);
//     // Taps must be 32-bits aligned (i.e. IQ packed as samples)
//     hwp_xcor->data[0]  =   UINT32taps[0];
//     hwp_xcor->data[1]  =   UINT32taps[1];
//     hwp_xcor->data[2]  =   UINT32taps[2];
//     hwp_xcor->data[3]  =   UINT32taps[3];
//     hwp_xcor->data[4]  =   UINT32taps[4];
//     hwp_xcor->command  =   XCOR_SREC_EN|XCOR_PACK_IQ|
//                                 XCOR_NB_SYMB((size))|
//               Srec                  XCOR_NB_ILOOP((SPP_ISI-1));
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorSrec);



    // Software equivalent
    INT8 k;
    INT8 i;
    INT8 bit;
    INT8 symbol;
    INT16* refBurst_INT16;


    refBurst_INT16=(INT16*)refBurst;


    for(k=0; k<size; k++)
    {
        refBurst_INT16[2*k]=0;
        refBurst_INT16[2*k+1]=0;
        for (i=0; i<chTapNb; i++)
        {
            bit=(k+i)%32;
            symbol=(k+i-bit)/32;
            if((HardBits[symbol]>>bit)& 0x00000001)
            {
                refBurst_INT16[2*k]-=taps[2*(chTapNb-1-i)];
                refBurst_INT16[2*k+1]-=taps[2*(chTapNb-1-i)+1];
            }
            else
            {
                refBurst_INT16[2*k]+=taps[2*(chTapNb-1-i)];
                refBurst_INT16[2*k+1]+=taps[2*(chTapNb-1-i)+1];
            }
        }
    }
}

//=============================================================================
// spal_XcorSrecPSK8
//-----------------------------------------------------------------------------
// ReconstructS a reference 8-PSK burst from channel taps and estimated/known hard bits.
//
// @param taps     INT16*.  INPUT.  Pointer to the channel taps.
// @param size     UINT8.   INPUT.  Length of the reconstructed burst.
// @param HardBits UINT8*.  INPUT.  Pointert to the Hard decision bits. to be reconstructed.
// @param refBurst UINT32*. OUTPUT. Pointer to the samples of the reconstructed burst.
// @parm  chTapNb  UINT8.   INPUT.  Number of the channel taps.
// @param regShift UINT8.   INPUT.  Rescale parameter.
//
//
//
//
//=============================================================================
PUBLIC VOID spal_XcorSrecPSK8(INT16* taps, UINT8 size, UINT32* HardBits,UINT32* refBurst, UINT8 chTapNb, UINT8 regShift)
{
    // Software equivalent
    INT16 k,j=0;
    INT16 i;

    INT16* refBurst_INT16;
    INT8 tmp_symb [150] = {0};
    INT32 temp[2]= {0};
    INT8 temp1 =0;
    UINT8 HardTmp[444] = {0};

    refBurst_INT16 =(INT16*)refBurst;
    for (i=0; i<3*size; i++)
    {
        HardTmp[i]=((HardBits[i/32])>>(i%32))&0x1;
    }
    for (i =0; i <(3*size - 2); i = i+3)
    {
        OPL_SHL_U8(HardTmp[i], 1, temp1);
        OPL_OR_8(temp1, HardTmp[i+1], temp1);
        OPL_SHL_U8(temp1, 1, temp1);
        OPL_OR_8(temp1, HardTmp[i+2], tmp_symb[j]);
        j = j+1;
    }
    for(k=0; k<(size-chTapNb) ; k++)
    {
        OPL_MOV_32((INT32)0, temp[0]);
        OPL_MOV_32((INT32)0, temp[1]);

        for (i=0; i<chTapNb; i++)
        {
            SPAL_XCOR_MAC_32(taps[2*(chTapNb-1-i)],LUT_8PSK[tmp_symb[i+k]].i,temp[0]);
            OPL_MSU_32(taps[2*(chTapNb-1-i)+1],LUT_8PSK[tmp_symb[i+k]].q,temp[0]);
            SPAL_XCOR_MAC_32(taps[2*(chTapNb-1-i)],LUT_8PSK[tmp_symb[i+k]].q,temp[1]);
            SPAL_XCOR_MAC_32(taps[2*(chTapNb-1-i)+1],LUT_8PSK[tmp_symb[i+k]].i,temp[1]);
        }
        OPL_SHR_32(temp[0], regShift, temp[0]);
        OPL_SHR_32(temp[1], regShift, temp[1]);

        OPL_SAT_16(temp[0], refBurst_INT16[2*k]);
        OPL_SAT_16(temp[1], refBurst_INT16[2*k+1]);
    }
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
    // Driver
//     UINT32 tmp_SumTable[8];
//     INT16* INT16_tmp_SumTable = (INT16*)SP_UNCACHED((UINT32)tmp_SumTable);
//     INT32 tmp_i, tmp_q;
//     UINT32* SumTable32 = (UINT32*)SumTable;
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorBmsum);
//
//     // Compute Table for Forward trellis
//
//     // Use the reconstruction mode on 3 taps with a special pattern:
//     // 0010111000b(0xB8) gives 0,1,3,7,6,5,2,4
//     hwp_xcor->hv0  =   0x0B8;
//     // Goes backward for HW simplicity -> give end address
//     hwp_xcor->addr0    =   (UINT32)(&tmp_SumTable[(8-1)]);
//     hwp_xcor->data[2]  =   ((taps[3]<<16)|(taps[2]&0xffff));
//     hwp_xcor->data[3]  =   ((taps[5]<<16)|(taps[4]&0xffff));
//     hwp_xcor->data[4]  =   ((taps[7]<<16)|(taps[6]&0xffff));
//     hwp_xcor->command  =   XCOR_SREC_EN|XCOR_PACK_IQ|
//                                 XCOR_NB_SYMB(8)|
//                                 XCOR_NB_ILOOP(2);
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//
//     // Reorder and add constant
//     tmp_i = taps[2*SPP_DELAY]   + taps[0];
//     tmp_q = taps[2*SPP_DELAY+1] + taps[1];
//
//     // 0 -> 0 & 8
//     SumTable32[ 8] =
//     SumTable32[ 0] = ((INT16_tmp_SumTable[0]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[1]   + tmp_q) << 16);
//     // 1 -> 1 & 12
//     SumTable32[12] =
//     SumTable32[ 1] = ((INT16_tmp_SumTable[1*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[1*2+1] + tmp_q) << 16);
//     // 2 -> 3 & 14
//     SumTable32[14] =
//     SumTable32[ 3] = ((INT16_tmp_SumTable[2*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[2*2+1] + tmp_q) << 16);
//     // 3 -> 7 & 15
//     SumTable32[15] =
//     SumTable32[ 7] = ((INT16_tmp_SumTable[3*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[3*2+1] + tmp_q) << 16);
//     // 4 -> 6 & 11
//     SumTable32[11] =
//     SumTable32[ 6] = ((INT16_tmp_SumTable[4*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[4*2+1] + tmp_q) << 16);
//     // 5 -> 5 & 13
//     SumTable32[13] =
//     SumTable32[ 5] = ((INT16_tmp_SumTable[5*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[5*2+1] + tmp_q) << 16);
//     // 6 -> 2 & 10
//     SumTable32[10] =
//     SumTable32[ 2] = ((INT16_tmp_SumTable[6*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[6*2+1] + tmp_q) << 16);
//     // 7 -> 4 & 9
//     SumTable32[ 9] =
//     SumTable32[ 4] = ((INT16_tmp_SumTable[7*2]   + tmp_i) & 0xFFFF) |
//                      ((INT16_tmp_SumTable[7*2+1] + tmp_q) << 16);
//
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorBmsum);

    // Software equivalent
    INT32 Table[2*(VIT_NSTATES/2)];
    CONST INT32 maxin=0x00007FFF;
    CONST INT32 minin=0xFFFF8000;
    INT16 local_temp_16;
    INT8 reg;
    INT8 n;
    INT8 l;

    for (n=0; n<(VIT_NSTATES/2); n++)
    {
        OPL_ADD_32((INT32)taps[2*SPAL_DELAY],(INT32)taps[2*0],Table[2*n]);
        OPL_ADD_32((INT32)taps[2*SPAL_DELAY+1],(INT32)taps[2*0+1],Table[2*n+1]);

        reg=n;
        for (l=1; l<SPAL_DELAY; l++)
        {
            local_temp_16=(1-2*(reg%2));
            SPAL_XCOR_MAC_32(local_temp_16,taps[2*l],Table[2*n]);
            SPAL_XCOR_MAC_32(local_temp_16,taps[2*l+1],Table[2*n+1]);
            OPL_SHR_8(reg,1,reg);
        }
        /*********************saturation on 15 signed bits**************/
        if ( Table[2*n] > maxin ) Table[2*n] = maxin;
        if ( Table[2*n] < minin ) Table[2*n] = minin;
        if ( Table[2*n+1] > maxin ) Table[2*n+1] = maxin;
        if ( Table[2*n+1] < minin ) Table[2*n+1] = minin;
        // Return results
        SumTable[2*n]=(INT16)Table[2*n];
        SumTable[2*n+1]=(INT16)Table[2*n+1];
    }
    // Fill the Table for Backward trellis
    SumTable[2*8]=SumTable[0];
    SumTable[2*8+1]=SumTable[1];
    SumTable[2*12]=SumTable[2*1];
    SumTable[2*12+1]=SumTable[2*1+1];
    SumTable[2*14]=SumTable[2*3];
    SumTable[2*14+1]=SumTable[2*3+1];
    SumTable[2*15]=SumTable[2*7];
    SumTable[2*15+1]=SumTable[2*7+1];
    SumTable[2*11]=SumTable[2*6];
    SumTable[2*11+1]=SumTable[2*6+1];
    SumTable[2*13]=SumTable[2*5];
    SumTable[2*13+1]=SumTable[2*5+1];
    SumTable[2*10]=SumTable[2*2];
    SumTable[2*10+1]=SumTable[2*2+1];
    SumTable[2*9]=SumTable[2*4];
    SumTable[2*9+1]=SumTable[2*4+1];
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
    // Driver
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorErrCalc);
//     hwp_xcor->data[2]  =   (UINT32)(Original);
//     hwp_xcor->addr0    =   (UINT32)(Reference);
//     hwp_xcor->command  =   (XCOR_SPROC_EN|XCOR_PACK_IQ) |
//                                 XCOR_NB_SYMB(size)|
//                                 XCOR_NB_ILOOP(4);
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorErrCalc);
//     return (hwp_xcor->hv0);


    // Software equivalent
    UINT8 i;
    UINT32 noise_power=0;
    INT16 diff = 0;
    INT16* Original_INT16;
    INT16* Reference_INT16;

    Original_INT16=(INT16*) Original;
    Reference_INT16=(INT16*) Reference;
    for (i=0; i<size; i++)
    {
        OPL_SUB_16(Original_INT16[2*i],Reference_INT16[2*i],diff);
        SPAL_XCOR_MAC_32( diff, diff,noise_power);
        OPL_SUB_16(Original_INT16[2*i+1],Reference_INT16[2*i+1],diff);
        SPAL_XCOR_MAC_32(diff,diff,noise_power);
    }
    return (UINT32) noise_power;
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

    // Driver
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorIcorCalc);
//     hwp_xcor->data[2]  =   (UINT32)(Original);
//     hwp_xcor->addr0    =   (UINT32)(Reference);
//     hwp_xcor->command  =   XCOR_SPROC_EN |
//                                 XCOR_NB_SYMB(size)|
//                                 XCOR_NB_ILOOP(3);
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorIcorCalc);
//     return ((INT32)hwp_xcor->hv0);

    // Software equivalent
    UINT8 i;
    INT16* Original_INT16;
    INT16* Reference_INT16;
    INT32 CorrI;

    Original_INT16=(INT16*) Original;
    Reference_INT16=(INT16*) Reference;
    CorrI=0;

    // do the convolution by the estimated channels coeffs
    for (i=0; i<size; i++)
    {
        SPAL_XCOR_MAC_32(Original_INT16[2*i],Reference_INT16[2*i+1],CorrI);
        SPAL_XCOR_MSU_32(Original_INT16[2*i+1],Reference_INT16[2*i],CorrI);
    }
    return CorrI;
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
    // Driver
//    SPP_PROFILE_FUNCTION_ENTRY(spp_XcorHvextract);
//     hwp_xcor->addr0    =   (UINT32)softvalues;
//     hwp_xcor->data[2]  =   (UINT32)hardvalues;
//     hwp_xcor->command  =   XCOR_BEXT_EN|
//                                 XCOR_NB_SYMB(size)|
//                                 XCOR_NB_ILOOP(7);
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorHvextract);

    // Software equivalent
    UINT16 i;
    for(i=0; i<4*size; i++)
    {
        if (softvalues[i] >> 7)
            hardvalues[i/32] |= (1 << (i%32));
        else
            hardvalues[i/32] &= ~(1 << (i%32));
    }
}



//=============================================================================
// spal_XcorDcoc
//-----------------------------------------------------------------------------
// DC offset correction function, derotation and power measurement function.
//
// @param RxBuffer UINT32*. INPUT. Pointer to the received samples buffer.
// @param size UINT8. INPUT. Length of the burst.
// @param  BOOL. INPUT. Triggers derotation on and off:
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
    // Driver
//     INT32 Accu_I,MeanI;
//     INT32 Accu_Q,MeanQ;
//     INT32 MeanIsup;
//     INT32 MeanQsup;
//     INT16 IOffset;
//     INT16 QOffset;
//     UINT16 Isup;
//     UINT16 Qsup;
//     UINT32 cmd =0;
//
//     SPP_PROFILE_FUNCTION_ENTRY(spp_XcorDcoc);
//     hwp_xcor->hv0 = 0;
//     hwp_xcor->hv1 = 0;
//     hwp_xcor->data[0] = 0;
//     hwp_xcor->data[1] = 0;
//     hwp_xcor->data[2] = 0;
//     hwp_xcor->data[3] = 0;
//     hwp_xcor->data[4] = 0;
//     hwp_xcor->data[5] = 0;
//     hwp_xcor->accuI = 0;
//     hwp_xcor->accuQ = 0;
//     hwp_xcor->addr0 = 0;
//     hwp_xcor->command = 0;
//
//     // Program XCOR for 1st Pass
//     hwp_xcor->addr0 = (UINT32)RxBuffer;
//     hwp_xcor->command =    XCOR_DCO1P_EN |
//                                 XCOR_NB_SYMB(size);
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//
//     // get first pass results
//     Accu_I = (INT32)hwp_xcor->accuI;
//     Accu_Q = (INT32)hwp_xcor->accuQ;
//     MeanI = Accu_I / size;
//     MeanQ = Accu_Q / size;
//
//     // Program XCOR for 2nd Pass
//     hwp_xcor->addr0 = (UINT32)RxBuffer;
//     hwp_xcor->data[1] = (MeanQ<<16)|(0xFFFF&MeanI);
//     hwp_xcor->command =    XCOR_DCO2P_EN |
//                                 XCOR_NB_SYMB(size);
//
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//
//     // get 2nd pass results
//     Isup = hwp_xcor->data[2] >> 2;
//     Qsup = hwp_xcor->data[3] >> 2;
//
//
//     MeanIsup = Isup?((INT32)hwp_xcor->accuI / Isup):0;
//     MeanQsup = Qsup?((INT32)hwp_xcor->accuQ / Qsup):0;
//
//     IOffset = (Isup==size)? MeanIsup :
//                             (MeanIsup + (Accu_I-(INT32)hwp_xcor->accuI)/(size-Isup))/2;
//     QOffset = (Qsup==size)? MeanQsup :
//                             (MeanQsup + (Accu_Q-(INT32)hwp_xcor->accuQ)/(size-Qsup))/2;
//     // Program XCOR for 3rd Pass
//     hwp_xcor->addr0 = (UINT32)&RxBuffer[0]; // RD address
//     hwp_xcor->data[2] = (UINT32)&RxBuffer[0]; // WR address
// #ifdef __SPP_BYPASS_DCOC__
//     hwp_xcor->data[1] = 0;
// #else
//     hwp_xcor->data[1] = ((-QOffset)<<16)|(0xFFFF&(-IOffset));
// #endif
//     Dco->I = IOffset;
//     Dco->Q = QOffset;
//
//     cmd = (derotate) ? XCOR_DEROTATION_EN : 0;
//     cmd |=  XCOR_DCO3P_EN | XCOR_PACK_IQ |
//             XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
//
//     hwp_xcor->command =    cmd;
//
//     // Flush the Dcache to enable later cached accesses
//     //spp_InvalidateDcache();
//
//     // Poll until the end of processing
//     while (spp_XcorActive()) ;
//
//     // Return Power
//     SPP_PROFILE_FUNCTION_EXIT(spp_XcorDcoc);
//     return (hwp_xcor->hv0);



    // Software equivalent
    INT32 Power=0;
    INT16 Offset_I=0;
    INT16 Offset_Q=0;
    INT32 Acc_Offset_I=0;
    INT32 Acc_Offset_Q=0;
    INT32 Acc_Offset_I_sup=0;
    INT32 Acc_Offset_Q_sup=0;
    INT16* Sample;
    INT8 nb_I_sup=0;
    INT8 nb_Q_sup=0;
    INT16 i=0;
    INT8 margin;
    INT8 N0=156-40;              // SP_BURST_TOTAL_SIZE = 156
    INT8 N1=156-80;              // SP_BURST_TOTAL_SIZE = 156
    margin=40;
    N0=156-margin;              // SP_BURST_TOTAL_SIZE = 156
    N1=156-2*margin;              // SP_BURST_TOTAL_SIZE = 156
    Sample=(INT16*)RxBuffer;

    // First Pass : Averaging on I and Q
    for(i=margin; i<N0; i++)
    {
        Acc_Offset_I+=Sample[2*i];
        Acc_Offset_Q+=Sample[2*i+1];
    }

    Offset_I=(INT16)(Acc_Offset_I/N1);
    Offset_Q=(INT16)(Acc_Offset_Q/N1);

    // Second Pass : Sup mean
    for(i=margin; i<N0; i++)
    {
        if (Sample[2*i]>Offset_I)
        {
            Acc_Offset_I_sup+=Sample[2*i];
            nb_I_sup++;
        }
        if (Sample[2*i+1]>Offset_Q)
        {
            Acc_Offset_Q_sup+=Sample[2*i+1];
            nb_Q_sup++;
        }
    }

    // compute the offset
    if((nb_I_sup>0)&&(nb_I_sup<N1))
        Offset_I = (INT16) (Acc_Offset_I_sup/nb_I_sup +(Acc_Offset_I-Acc_Offset_I_sup)/(N1-nb_I_sup))/2;
    else
        Offset_I = 0;

    if((nb_Q_sup>0)&&(nb_Q_sup<N1))
        Offset_Q = (INT16) (Acc_Offset_Q_sup/nb_Q_sup +(Acc_Offset_Q-Acc_Offset_Q_sup)/(N1-nb_Q_sup))/2;
    else
        Offset_Q = 0;

    Dco->I=Offset_I;
    Dco->Q=Offset_Q;

    for(i=0; i<(N0+margin); i++)
    {
//        INT16 tmp_i=Sample[2*i]   - (INT16)Offset_I;
//        INT16 tmp_q=Sample[2*i+1] - (INT16)Offset_Q;
        INT16 tmp_i=Sample[2*i] ;
        INT16 tmp_q=Sample[2*i+1] ;

        if ((i>=4)&&(i<156)) Power += tmp_i*tmp_i + tmp_q*tmp_q; // SP_BURST_TOTAL_SIZE = 156, SP_RX_NBURST_MARGIN =4
        switch (i&0x3)
        {
            case 0:
                Sample[2*i]  = tmp_i;
                Sample[2*i+1]= tmp_q;
                break;

            case 1:
                Sample[2*i]  = tmp_q;
                Sample[2*i+1]=-tmp_i;
                break;

            case 2:
                Sample[2*i]  =-tmp_i;
                Sample[2*i+1]=-tmp_q;
                break;

            case 3:
                Sample[2*i]  =-tmp_q;
                Sample[2*i+1]= tmp_i;
                break;

            default:
                break;
        }
    }
    return (UINT32)Power;
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
// @param Dco SPAL_DC_OFFSET_T*. OUTPUT. Pointer to the following structure:
// - Dco.I, DC offset on I,
// - Dco.Q, DC offset on Q.
// This DC value (calculated on first part of the burst) will be corrected.
// .
//
// @return UINT32. Return the power of the burst after DC offset correction.
//=============================================================================
PUBLIC UINT32 spal_XcorDcocHburst(UINT32* RxBuffer, UINT8 size, BOOL derotate, BOOL bypassDcoc, SPAL_DC_OFFSET_T Dco)
{
//    UINT32 cmd;

//    SPAL_PROFILE_FUNCTION_ENTRY(spal_XcorDcoc);
//    hwp_xcor->addr0 = (UINT32)&RxBuffer[0]; // RD address
//    hwp_xcor->data[2] = (UINT32)&RxBuffer[0]; // WR address
//    hwp_xcor->data[1] = ((-(Dco->Q))<<16)|(0xFFFF&(-(Dco->I)));
//
//    cmd = (derotate) ? XCOR_DEROTATION_EN : 0;
//    cmd |=  XCOR_DCO3P_EN | XCOR_PACK_IQ |
//            XCOR_NB_ILOOP(2) | XCOR_NB_SYMB(size);
//
//    hwp_xcor->command =    cmd;
//
//    // Flush the Dcache to enable later cached accesses
//    //spp_InvalidateDcache();
//
//    // Poll until the end of processing
//    while (spal_XcorActive()) ;
//
//    // Return Power
//    SPAL_PROFILE_FUNCTION_EXIT(spal_XcorDcoc);
//    return (hwp_xcor->hv0);
    return((UINT32)1);
}




//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_XcorBmmlZf
//-----------------------------------------------------------------------------
// realizes
//
// @param taps SPAL_COMPLEX_T*. INPUT, Q11. Address of complex estimated channel coeffients.
// @param metric PINT16. OUTPUT, Buffer containing the branch metrics.
// @author Zhang mingjie
// @updated by Yordan Tabakov
// @date 06/Nov/07
//
//=============================================================================

// compute the BMML or partial ZF (two taps)
PUBLIC VOID spal_XcorBmmlZf( INT16* taps,
                             INT16* metric,
                             INT8    fracBits)
{
    UINT8 i,j,k;
    SPAL_CMPLX32_T Accu;
    SPAL_COMPLEX_T * pm         = (SPAL_COMPLEX_T *) &metric[2];
    SPAL_COMPLEX_T * taps_Cmp   = (SPAL_COMPLEX_T *) taps ;
    SPAL_COMPLEX_T * metric_Cmp = (SPAL_COMPLEX_T *) metric ;


    SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,2);

    // --------------------------------
    // part executed on the accelerator

    for (k = 0; k < ALL_TAP_LEN/2; k++)
    {
        for (i = 0; i < STATE_LEN; i++)
        {
            for (j = 0; j < STATE_LEN; j++)
            {
                OPL_MOV_32((INT32)0, Accu.i);
                OPL_MOV_32((INT32)0, Accu.q);

                SPAL_XCOR_MAC_32(taps_Cmp[2*k+0].i, LUT_8PSK[i].i, Accu.i);
                SPAL_XCOR_MSU_32(taps_Cmp[2*k+0].q, LUT_8PSK[i].q, Accu.i);
                SPAL_XCOR_MAC_32(taps_Cmp[2*k+0].i, LUT_8PSK[i].q, Accu.q);
                SPAL_XCOR_MAC_32(taps_Cmp[2*k+0].q, LUT_8PSK[i].i, Accu.q);

                SPAL_XCOR_MAC_32(taps_Cmp[2*k+1].i, LUT_8PSK[j].i, Accu.i);
                SPAL_XCOR_MSU_32(taps_Cmp[2*k+1].q, LUT_8PSK[j].q, Accu.i);
                SPAL_XCOR_MAC_32(taps_Cmp[2*k+1].i, LUT_8PSK[j].q, Accu.q);
                SPAL_XCOR_MAC_32(taps_Cmp[2*k+1].q, LUT_8PSK[j].i, Accu.q);

                OPL_SHR_32(Accu.i, fracBits, Accu.i)
                OPL_SHR_32(Accu.q, fracBits, Accu.q);

                OPL_SAT_16(Accu.i, pm->i);
                OPL_SAT_16(Accu.q, pm->q);

                pm++;
            }
        }
    }

    // end part executed on the accelerator
    // ------------------------------------


    // re-arrange the main branch matric
    for (j = 0; j < STATE_LEN; j++)
    {
        OPL_MOV_16(metric_Cmp[STATE_LEN * j + STATE_LEN].i, metric_Cmp[STATE_LEN * j].i);
        OPL_MOV_16(metric_Cmp[STATE_LEN * j + STATE_LEN].q, metric_Cmp[STATE_LEN * j].q);
    }



}


//====================================================================================
//    spal_XcorComplexMatrixMultiply
//
//    This function is used to compute double convolution in the DC IQ estimation function.
//
//  @param inVector  SPAL_COMPLEX_T*  input vector
//  @param inMatrix  SPAL_COMPLEX_T*  input matrix
//  @param outMatrix SPAL_COMPLEX_T*  output matrix (multiplication result)
//  @param columnLen UINT8             number of input matrix columns
//  @param rowLen    UINT8             number of input matrix rows
//  @param conjugate UINT8             conjugate input vector
//  @param fracBits  UINT8             fractional bits of the data format
//
//  @author Yordan Tabakov
//
//  @date 26/11/07
//====================================================================================
PUBLIC SPAL_CMPLX32_T spal_XcorComplexMatrixMultiply(   INT16* inVector,
        INT16* inMatrix,
        INT16* outMatrix,
        UINT8   columnLen,
        UINT8   rowLen,
        INT8    fracBits)
{
    UINT8 i,j;
    SPAL_CMPLX32_T Accu;

    SPAL_COMPLEX_T* inVectCmp  =  (SPAL_COMPLEX_T*) inVector;
    SPAL_COMPLEX_T* inMatrCmp  =  (SPAL_COMPLEX_T*) inMatrix;
    SPAL_COMPLEX_T* outMatrCmp =  (SPAL_COMPLEX_T*) outMatrix;

    if (outMatrix)
    {
        SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,3);
    }

    for (i = 0; i < columnLen; i++)
    {

        OPL_MOV_32((INT32)0, Accu.i);
        OPL_MOV_32((INT32)0, Accu.q);

        for (j = 0; j < rowLen; j++)
        {
            SPAL_XCOR_MAC_32(inMatrCmp[rowLen*i+j].i, inVectCmp[j].i, Accu.i);
            SPAL_XCOR_MSU_32(inMatrCmp[rowLen*i+j].q, inVectCmp[j].q, Accu.i);
            SPAL_XCOR_MAC_32(inMatrCmp[rowLen*i+j].i, inVectCmp[j].q, Accu.q);
            SPAL_XCOR_MAC_32(inMatrCmp[rowLen*i+j].q, inVectCmp[j].i, Accu.q);
        }

        if (outMatrix)
        {
            OPL_SHR_32(Accu.i,fracBits,Accu.i);
            OPL_SAT_16(Accu.i,outMatrCmp[i].i);
            OPL_SHR_32(Accu.q,fracBits,Accu.q);
            OPL_SAT_16(Accu.q,outMatrCmp[i].q);
        }
    }

    return (Accu);

}

//=============================================================================
// spal_XcorComplexConvolution
//-----------------------------------------------------------------------------
// Complex convolution with enable rescale
//
// @param Xseq SPAL_COMPLEX_T.INPUT.The first input sequence.
// @param Xlen UINT16 .INPUT. The length of the first input sequence.
// @param Yseq SPAL_COMPLEX_T.INPUT.The second input sequence.
// @param Ylen UINT16.INPUT.The length of the second input sequence.
// @param Zseq VOID*.OUTPUT.The output sequence (16 or 32-bit complex).
// @param Zoff UINT16.INPUT. Offset in the ouptut sequence
// @param Zlen UINT16.INPUT. Max length of the output sequence (discard if null)
// @param fracBits INT8.INPUT.Rescale parameter.
//
// @author Yordan Tabakov
// @date 22/11/2007
//=============================================================================
PUBLIC VOID spal_XcorComplexConvolution (    INT16 *Xseq,
        UINT16 Xlen,
        INT16 *Yseq,
        UINT16 Ylen,
        INT16 *Zseq,
        UINT16 Zoff,
        UINT16 Zlen,
        INT8 fracBits  )
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
    SPAL_COMPLEX_T * regPtrX    = XseqCmp - Ylen;
    SPAL_COMPLEX_T * regPtrY    = YseqCmp - 1;
    SPAL_COMPLEX_T * regPtrZ    = ZseqCmp;


    SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,4);


    // executed by the HW accelerator
    //--------------------------------

    for (; regLoopExt > 0; regLoopExt--)
    {
        //               "internal registers"
        UINT16            regLoopInt = MIN(regMinLen, MIN(regPtrIdx, regAllLen - regPtrIdx));
        SPAL_COMPLEX_T * regRdPtrX  = regPtrX + MAX(regYLen,regPtrIdx);
        SPAL_COMPLEX_T * regRdPtrY  = regPtrY + MIN(regYLen,regPtrIdx);
        SPAL_CMPLX32_T   regAccu    = {0,0};

        for (; regLoopInt > 0; regLoopInt--)
        {
            SPAL_XCOR_MAC_32(regRdPtrX->i, regRdPtrY->i, regAccu.i);
            SPAL_XCOR_MSU_32(regRdPtrX->q, regRdPtrY->q, regAccu.i);
            SPAL_XCOR_MAC_32(regRdPtrX->i, regRdPtrY->q, regAccu.q);
            SPAL_XCOR_MAC_32(regRdPtrX->q, regRdPtrY->i, regAccu.q);

            regRdPtrX++;
            regRdPtrY--;
        }

        OPL_SHR_32(regAccu.i, regShift, regAccu.i);
        OPL_SHR_32(regAccu.q, regShift, regAccu.q);

        OPL_SAT_16(regAccu.i, regPtrZ->i);
        OPL_SAT_16(regAccu.q, regPtrZ->q);

        regPtrZ++;
        regPtrIdx++;
    }


    //--------------------------------

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
// @author Yordan Tabakov
// @date 22/11/2007
//=============================================================================
PUBLIC UINT32 spal_XcorComplexPower (    INT16 *seq,
        INT16 *out,
        UINT16 len,
        INT8 fracBits)
{
    UINT16 i;
    UINT32 allPwr = 0;
    SPAL_COMPLEX_T* SeqCmp = (SPAL_COMPLEX_T*) seq;

    if (out)
    {
        SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,5);
    }

    for (i = 0; i < len; i++)
    {
        INT32 Pwr = 0;

        SPAL_XCOR_MAC_32(SeqCmp[i].i,SeqCmp[i].i,Pwr);
        SPAL_XCOR_MAC_32(SeqCmp[i].q,SeqCmp[i].q,Pwr);
        OPL_ADD_32(allPwr,Pwr,allPwr);

        if (out)
        {
            OPL_SHR_32(Pwr,fracBits,Pwr);
            OPL_SAT_16(Pwr,out[i]);
        }
    }

    // this shift is done is soft
    OPL_SHR_U32(allPwr,fracBits,allPwr);

    return (allPwr);

}


//======================================================================
// spal_XcorDerotate
//----------------------------------------------------------------------
// This function is used for 8PSK or GMSK signal phase derotate
//
// @param in        SPAL_COMPLEX_T*  INPUT.  Pointer to the input sequence.
// @param out       SPAL_COMPLEX_T*  OUTPUT. Pointer to the output sequence.
// @param Length    UINT16            INPUT.  Length of the input sequence.
// @param fracBits  INT8              INPUT.  Rescale parameter.
// @param ModType   UINT8             INPUT.  Defines the modulation type.
//
// @author Zhao Yang, modified by Nadia Touliou
// @date 30/11/07
//======================================================================

PUBLIC VOID spal_XcorDerotate( INT16* in,
                               INT16* out,
                               UINT16 Length,
                               UINT8 ModType)

{
    INT16 i;
    SPAL_COMPLEX_T* inCmp = (SPAL_COMPLEX_T*) in;
    SPAL_COMPLEX_T* outCmp = (SPAL_COMPLEX_T*) out;

    SPAL_COMPLEX_T* pTable = (ModType == GMSK) ? (SPAL_COMPLEX_T*)GMSK_ROTATE : (SPAL_COMPLEX_T*)PSK8_ROTATE;
    UINT8            modulo = (ModType == GMSK) ? 0x3 : 0xf;


    for (i = 0; i < Length; i++)
    {
        SPAL_CMPLX32_T accu;

        OPL_MOV_32(0,accu.i);
        OPL_MOV_32(0,accu.q);

        SPAL_XCOR_MAC_32(inCmp[i].i, pTable[i & modulo].i, accu.i);
        SPAL_XCOR_MSU_32(inCmp[i].q, pTable[i & modulo].q, accu.i);
        SPAL_XCOR_MAC_32(inCmp[i].i, pTable[i & modulo].q, accu.q);
        SPAL_XCOR_MAC_32(inCmp[i].q, pTable[i & modulo].i, accu.q);

        OPL_SHR_32(accu.i, SVAL_FRAC_BITS, accu.i);
        OPL_SHR_32(accu.q, SVAL_FRAC_BITS, accu.q);

        OPL_SAT_16(accu.i, outCmp[i].i);
        OPL_SAT_16(accu.q, outCmp[i].q);
    }
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
// @author Nadia Touliou
// @date 10/12/07
//======================================================================
PUBLIC VOID spal_XcorDcIqCompensate(     INT16* inOut,
        INT16* dc_compens,
        INT16* iq_compens,
        UINT16 length,
        INT8 fracBits)

{

    UINT16 i;
    SPAL_COMPLEX_T* inOutCmp      = (SPAL_COMPLEX_T*) inOut;
    SPAL_COMPLEX_T* dc_compensCmp = (SPAL_COMPLEX_T*) dc_compens;
    SPAL_COMPLEX_T* iq_compensCmp = (SPAL_COMPLEX_T*) iq_compens;

    SPAL_COMPLEX_T inDcCorrected;
    INT16 temp1;

    SPAL_XCOR_SHIFT_CHECK_RANGE(fracBits,6);

    opl_SetFormat16(fracBits);

    for (i = 0; i < length; i++)
    {
        //INT16 temp2;

        // DC removed from input
        // inDcCorected.i = inOut.i - dc_compens.i
        // inDcCorected.q = inOut.q - dc_compens.q
        OPL_SUB_16(inOutCmp[i].i, dc_compensCmp->i, inDcCorrected.i);
        OPL_SUB_16(inOutCmp[i].q, dc_compensCmp->q, inDcCorrected.q);

        // Simple Correction => inOut[i].i = (1-iq_compens.i)*(in.i - dc_compens.i)
        OPL_SUB_16 (  2048, iq_compensCmp->i,       temp1);
        SPAL_XCOR_MULF_16( temp1, inDcCorrected.i,  inOutCmp[i].i);

        // Full Correction => inOut[i].i = (iq_compens.q * inDcCorrected.q - temp1) / delta
        // SPAL_XCOR_MULF_16(  iq_compens.q,  inDcCorrected.q,       temp2);
        // OPL_SUB_16 (         temp2,            temp1,       temp2);
        // OPL_DIV_16 (         temp2,            delta,  inOut[i].i);

        // Simple Correction => inOut[i].q  = (1+iq_compens.i)*(in.q - dc_compens.q)
        OPL_ADD_16 (  2048,    iq_compensCmp->i,      temp1);
        SPAL_XCOR_MULF_16( temp1, inDcCorrected.q, inOutCmp[i].q);

        // Full Correction => inOut[i].q = (iq_compens.q * inDcCorrected.i - temp1) / delta
        // SPAL_XCOR_MULF_16(  iq_compens.q, inDcCorrected.i,      temp2);
        // OPL_SUB_16 (         temp2,           temp1,      temp2);
        // OPL_DIV_16 (         temp2,           delta, inOut[i].q);

    }

    opl_SetFormat16(0);

}

