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
#include "opl_fixed.h"

// ESPAL public header
#include "spal_vitac.h"
#include "spal_xcor.h"

// ESPAL private header
#include "spalp_private.h"
#include "spalp_common_model.h"




//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------


//=============================================================================
// spal_VitacBusy
//-----------------------------------------------------------------------------
// Checks whether the VITAC is busy ?
//
//=============================================================================
PROTECTED BOOL spal_VitacBusy()
{
    // Driver
//    UINT32 busy;
//    busy =  (hwp_vitac->status) &
//            (VITAC_EQU_PENDING|VITAC_DEC_PENDING|VITAC_TB_PENDING);
//    if (busy) {
//        return TRUE;
//    }
//    else {
//        return FALSE;
//    }

    return FALSE;
}



//=============================================================================
// spal_VitacEqualize
//-----------------------------------------------------------------------------
// Run Equalization Viterbi Process on a half burst without traceback.
//
// @param trellis BOOL. INPUT. This parameters makes the Viterbi equalize the
// backward part of the treillis of the forward part of the treillis. Two defines
// have been specifically designed for this purpose:
// - BACKWARD_TRELLIS (defined as TRUE in spal_vitac.h) is used for backward operation,
// - FORWARD_TRELLIS (defined as FALSE in spal_vitac.h) is used for forward operation.
//
// @param param SPAL_VITAC_EQU_PARAM_T*. INPUT/OUTPUT. param sets the working buffer,
// the operation mode, provides the input as well as the output.
// Parameters:
// - param.H0. UINT32. INPUT. Is the value of the first complex channel tap where
// ((INT16*)param.H0)[0] is the real part (inphase) of the channel coefficient and
// ((INT16*)param.H0)[1] is the imaginary part (quadrature) of the channel coefficient.
// - param.HL. UINT32. INPUT. Is the value of the last (5th) complex channel tap where
// ((INT16*)param.HL)[0] is the real part (inphase) of the channel coefficient and
// ((INT16*)param.HL)[1] is the imaginary part (quadrature) of the channel coefficient.
// - param.bmSum. INT16*. INPUT. The complex branch metric for a specific state of the
// register are precalculated by the spal_XcorBmsum function and stored in a
// INT16 buffer. ((INT16*)param.bmSum)[2*i] is the real part (inphase) of the branch metric
// corresponding to the ith register state and ((INT16*)param.bmSum)[2*i+1] is the
// imaginary part (quadrature) of the branch metric corresponding to the ith register
// state.
// - param.length. UINT8. INPUT. Number of samples in half burst including tail samples.
// - param.shift_bm. UINT8. INPUT. Branch metric shift value. The path metric are
// accumulated by adding branch metric. Each computed branch metric is shifted by
// param.shift_bm before being added to the path metric. param.shift_bm is computed outside
// of the viterbi equalizer and depends on the SNR. Hardware limitations impose that
// param->shift_bm should belong to [4,13],
// - param.shift_sb. UINT8. INPUT. Soft bit shift value. The soft bits are computed as
// a difference between two path metrics. The soft bits are on 8 bits whereas the
// path metrics are on 16 bits, therefore a shift is sometimes required to determine
// the most appropriate 8 bits to output as a soft bit. param.shift_sb is computed outside
// of the viterbi equalizer and depends on the SNR. In practice param.shift_sb=0 until
// param.shift_bm is maximum (13). Hardware limitations impose that param->shift_sb
// should belong to [0,9],
// - first_state. UINT8. INPUT. The initial state of the register is determined by
// the first and last bits of the training sequence for respectively the backward
// and forward treillis equalization.
// .
// Working buffers:
// - param.pm UINT16*. INPUT/OUTPUT. Sets the buffer where path metrics (a.k.a. node
// metrics) are stored. When the VITAC finishes equalization, the buffer can
// be used to select the most likely end state when the tail bits do not close
// the treillis.
// .
// Input/Output buffers:
// - in. UINT32*. INPUT. Pointer to the input buffer. It contains the complex
// received samples where ((INT16*)in)[2*i] is the real part (inphase) of the
// ith complex received sample and ((INT16*)in)[2*i+1] is the imaginary part
// (quadrature) of the ith complex received sample.
// - out. UINT8*. OUTPUT. Pointer to the output buffer. It contains the 8 bits
// softbits for each state, for each sample (before trace back) coded as 1 sign
// bit and 7 amplitude bits and where path_diff[n+(m*VIT_NSTATES)] is the soft
// bit for the nth state for the mth sample.
// .
//
// @return SPAL_VITAC_ERROR_T. Indicates valid or invalid VITAC processing.
//=============================================================================
PROTECTED UINT8 spal_VitacEqualize(BOOL trellis, SPAL_VITAC_EQU_PARAM_T* param)
{
    // Driver
//     UINT8 bm_shift_lev;
//     UINT8 sv_shift_lev;
//     UINT32 cmd;
//     int i;
//
//     // TODO : no error return anymore, was useless and misleading
//     // as errors were not handled in upper levels...
//
//     // derive/saturate shift parameter for Branch metrics
//     if (param->shift_bm < 4)
//         bm_shift_lev = 9;
//     else if (param->shift_bm > 13)
//         bm_shift_lev = 0;
//     else
//         bm_shift_lev = 13 - param->shift_bm;
//
//     // derive/saturate shift parameter for Softvalues
//     if  (param->shift_sb > 9)
//         sv_shift_lev = 0;
//     else
//         sv_shift_lev = 9 - param->shift_sb;
//
//     // Init Path Metrics
//     for (i=0;i<(EQU_NB_STATES/2);i++)
//     {
//         *(UINT32*)(&(param->pm[2*i])) = EQU_PM_INIT;
//     }
//     // Put PM of starting state to min value
//     param->pm[param->first_state] = 0;
//
//     // Symbol buffer start address
//     hwp_vitac->ch_symb_addr = (UINT32)param->in;
//
//     // Path Metrics base address
//     hwp_vitac->pm_base_addr = (UINT32)param->pm;
//
//     // Output start address
//     hwp_vitac->out_base_addr = (UINT32)param->out;
//
//     if (trellis == BACKWARD_TRELLIS)
//     {
//         // First and last channel parameters
//         hwp_vitac->H0_param = param->HL;
//         hwp_vitac->HL_param = param->H0;
//
//         // Precalculated sum terms base address
//         hwp_vitac->exp_symb_addr = (UINT32)&(param->bmSum[EQU_NB_STATES]);
//
//         cmd = VITAC_BKWD_TRELLIS;
//     }
//     else
//     {
//         hwp_vitac->H0_param = param->H0;
//         hwp_vitac->HL_param = param->HL;
//
//         // Precalculated sum terms base address
//         hwp_vitac->exp_symb_addr = (UINT32)&(param->bmSum[0]);
//
//         cmd = 0;
//     }
//
//     // Rescaling threshold
//     hwp_vitac->rescale = EQU_RESCALE_THRESH;
//
//     // Command
//     cmd |=  VITAC_START_EQU |
//             //VITAC_INT_MASK | // Masked by default
//             VITAC_NB_STATES(VITAC_16_STATES) |
//             VITAC_BM_SHIFT_LEV(bm_shift_lev) |
//             VITAC_SV_SHIFT_LEV(sv_shift_lev) |
//             VITAC_NB_SYMBOLS(param->length);
//
//
//     hwp_vitac->command = cmd;
//
//     return vitac_err_none;


    // Software equivalent
    INT8 n;
    INT8 m;
    INT8 k;
    INT8 Ib;
    INT8 Ob;
    //INT8 nStt=16;
    //UINT16 NodeMetric[16][2];
    UINT16* NodeMetric;
    INT16 MAX_16=(INT16)0x7fff;
    INT16 MIN_16=(INT16)0x8000;
    UINT16 min_path=(INT16)0x7fff;
    UINT16 max_path=0;
    INT16* input_add;
    INT8 inc;
    INT16 Y[2];
    INT16 W[2];
    INT16 Z[2];
    INT16 taps0;
    INT16 taps1;
    UINT16 max_in=1<<11;
    INT8 soft_write_counter=0;
    INT8 max_soft_write_counter=3;
    INT32 Bi;
    INT32 Bq;
    INT32 max_bm;
    INT32 B[2];
    UINT16 path[2];
    INT8 rescale=0;
    INT8 rescale_count=0;
    UINT16 threshold=((INT16)0x7fff)/2;
    UINT16 diff;
    UINT8* path_diff;
    INT8 History[16][65];      // SHOULD BE param->length
    INT16 tapsdelay0;
    INT16 tapsdelay1;
    UINT16 max_sb;
    INT16* bmSum;

    path_diff=(UINT8*)(param->out);
    NodeMetric=(UINT16*)(param->pm);
    // Initializes variables
    if (trellis == BACKWARD_TRELLIS)
    {
        inc=-1;
        bmSum=param->bmSum+16;
    }
    else
    {
        inc=1;
        bmSum=param->bmSum;
    }
    taps0=((INT16)(param->H0 & 0x0000FFFF));
    taps1=((INT16)(param->H0 >> 16));
    tapsdelay0=((INT16)(param->HL & 0x0000FFFF));
    tapsdelay1=((INT16)(param->HL >> 16));

    max_bm=1<<12;
    max_sb=(1<<7)-1;





    // derive/saturate shift parameter for Branch metrics
    if (param->shift_bm < 4)
        param->shift_bm = 4;
    else if (param->shift_bm > 13)
        param->shift_bm = 13;
    if  (param->shift_sb > 9)
        param->shift_sb = 9;


    /*  intialize metric   */
    for (n = 0; n < VIT_NSTATES; n++) NodeMetric[n + 0*VIT_NSTATES] = MAX_16/2;
    /*  setup intiation state */
    NodeMetric[param->first_state+ 0*VIT_NSTATES] = 0;

    for (m = 0; m < param->length; m++)
    {
        min_path=MAX_16;
        max_path=0;
        // init for one transition
        input_add = (INT16*)(param->in + inc*m);

        Y[0]=input_add[0];//Q10
        Y[1]=input_add[1];//Q10

        W[0]=2*taps0;//Q10
        W[1]=2*taps1;//Q10
        /*********************saturation on 13 signed bits**************/
        if ( W[0] >  2*max_in-1 ) {W[0] =  2*max_in-1;}//printf("overflow on 2h0\n");}
        if ( W[0] < -2*max_in   ) {W[0] = -2*max_in  ;}//printf("overflow on 2h0\n");}
        if ( W[1] >  2*max_in-1 ) {W[1] =  2*max_in-1;}//printf("overflow on 2h0\n");}
        if ( W[1] < -2*max_in   ) {W[1] = -2*max_in  ;}//printf("overflow on 2h0\n");}

        W[0]+=input_add[0];//Q10
        W[1]+=input_add[1];//Q10

        /*********************saturation on 14 signed bits**************/
        if ( W[0] >  4*max_in-1 ) {W[0] =  4*max_in-1;}//printf("overflow on y[n]+2h0\n");}
        if ( W[0] < -4*max_in   ) {W[0] = -4*max_in  ;}//printf("overflow on y[n]+2h0\n");}
        if ( W[1] >  4*max_in-1 ) {W[1] =  4*max_in-1;}//printf("overflow on y[n]+2h0\n");}
        if ( W[1] < -4*max_in   ) {W[1] = -4*max_in  ;}//printf("overflow on y[n]+2h0\n");}

        Z[0]=2*tapsdelay0;//Q10
        Z[1]=2*tapsdelay1;//Q10
        /*********************saturation on 13 signed bits**************/
        if ( Z[0] >  2*max_in-1 ) {Z[0] =  2*max_in-1;}//printf("overflow on 2h5\n");}
        if ( Z[0] < -2*max_in   ) {Z[0] = -2*max_in  ;}//printf("overflow on 2h5\n");}
        if ( Z[1] >  2*max_in-1 ) {Z[1] =  2*max_in-1;}//printf("overflow on 2h5\n");}
        if ( Z[1] < -2*max_in   ) {Z[1] = -2*max_in  ;}//printf("overflow on 2h5\n");}

        soft_write_counter=0;
        // loop on the 16 butterflies
        for (n = 0; n < VIT_NSTATES/2; n++)
        {

            // one butterfly update with this loop
            for (k=0; k<2; k++)
            {
                Ib=k%2;
                for (Ob=0; Ob<2; Ob++)
                {
                    Bi = (INT16)((1-Ib)*Y[0]+Ib*W[0]+Ob*Z[0]);//Q10
                    Bq = (INT16)((1-Ib)*Y[1]+Ib*W[1]+Ob*Z[1]);//Q10
                    /*********************saturation on 15 signed bits**************/
                    if ( Bi >  8*max_in-1 ) Bi =  8*max_in-1;
                    if ( Bi < -8*max_in   ) Bi = -8*max_in  ;
                    if ( Bq >  8*max_in-1 ) Bq =  8*max_in-1;
                    if ( Bq < -8*max_in   ) Bq = -8*max_in  ;

                    Bi = (INT16)(Bi-bmSum[2*n]);//Q10
                    Bq = (INT16)(Bq-bmSum[2*n+1]);//Q10
                    /*********************saturation on 12 signed bits**************/
                    if ( Bi >  max_bm/2-1 ) Bi=  max_bm/2-1;
                    if ( Bi < -max_bm/2   ) Bi= -max_bm/2  ;
                    if ( Bq >  max_bm/2-1 ) Bq=  max_bm/2-1;
                    if ( Bq < -max_bm/2   ) Bq= -max_bm/2  ;
                    B[Ob] = Bi*Bi+Bq*Bq;//Q20
                    //fprintf(bm,"%d\n",B[Ob]);
                    B[Ob] = B[Ob]>>param->shift_bm;//Q10 sur 16 bits non signe
                    if ( B[Ob] > max_bm - 1 )
                    {
                        B[Ob] = max_bm - 1;//Q10 sur 12 bits non signes
                        //printf("branch metric clipped\n");
                    }
                }

                path[0] = (UINT16)(NodeMetric[n+((m%2)*VIT_NSTATES)] - rescale*threshold + B[0]);
                path[1] = (UINT16)(NodeMetric[n+VIT_NSTATES/2+((m%2)*VIT_NSTATES)] - rescale*threshold + B[1]);


                if (path[0]<=path[1])
                {
                    if (path[0]<min_path) min_path=path[0];
                    if (path[0]>max_path) max_path=path[0];
                    NodeMetric[2*n+k+(((m+1)%2)*VIT_NSTATES)] = path[0];
                    History[2*n+k][m]=0;
                    //if (m>32) fprintf(sb,"%d\n",(path[1]-path[0]));
                    diff = (path[1]-path[0])>>param->shift_sb;//Q6 sur 15 bits signes
                    path_diff[(2*n+k)+(m*VIT_NSTATES)] = (UINT8)((diff>max_sb)? max_sb: diff);

                }
                else
                {
                    if (path[1]<min_path) min_path=path[1];
                    if (path[1]>max_path) max_path=path[1];
                    NodeMetric[2*n+k+(((m+1)%2)*VIT_NSTATES)]= path[1];
                    History[2*n+k][m]=1;
                    //if (m>32) fprintf(sb,"%d\n",(path[0]-path[1]));
                    diff = (path[0]-path[1])>> param->shift_sb;//Q6 sur 16 bits signes
                    path_diff[(2*n+k)+(m*VIT_NSTATES)] = (UINT8)((diff>max_sb)? max_sb: diff);
                    path_diff[(2*n+k)+(m*VIT_NSTATES)] += (UINT8)(0x80);
                }


                soft_write_counter++;
                soft_write_counter=soft_write_counter&max_soft_write_counter;

            }//end loop for k=0..2

        }//end loop for n=0..nStt/2
        if (min_path>threshold)
        {

            rescale = 1;
            rescale_count++;
            //printf("rescaling of the path metrics\n");
        }
        else
        {
            rescale =0;
        }

    }
    // end of trellis


    return vitac_err_none;

}


//=============================================================================
// spal_VitacFullDecoding
//-----------------------------------------------------------------------------
// This function performs the Viterbi decoding of a coded block. It receives
// the soft bits and coding scheme and outputs the decoded bits. The parameters
// corresponding to each coding scheme are stored inside SPAL internal
// variables.
//
// @param cs_type SPAL_CODING_SCHEME_T. INPUT. SPAL coding scheme definition.
// FACCH_H, FACCH_F, SACCH, SDCCH, BCCH and AGCH are not included as they are
// considered as CS_CS1.
// @param softBit_ptr UINT32*. INPUT. Pointer to the soft bit buffer. The soft
// bit are coded on 8 bits with 1 sign bit and 7 amplitude bits.
// @param output_ptr UINT32*. OUTPUT. Pointer to the decoded bits buffer.
//
// @return SPAL_VITAC_ERROR_T. Indicates valid or invalid VITAC processing.
//=============================================================================
PUBLIC UINT8 spal_VitacFullDecoding(   SPAL_VITAC_DEC_CFG_T *pCfg,
                                       UINT32* output_ptr)
{

// Driver
//#define VITAC_PM_RESCALE_THRESH 16383
//    UINT32 SurvivorMEM[(64*171)/(16*2)]; // Worst case for AFS7.95 Speech
//    UINT32 PathMetric_ptr[64];
//    UINT16* PathMetric = (UINT16*)SP_UNCACHED((UINT32)PathMetric_ptr);
//    UINT32 localPuncTable[SPP_PUNCTURE_TABLE_SIZE];
//    UINT32 cmd = 0;
//
//    UINT16 nbStates = 16;
//    UINT8  tail = 4;
//    UINT16 i;
//    UINT16 nb_symbols = g_sppVitacDecParam[cs_type].nbSymbol;
//    SPP_PROFILE_FUNCTION_ENTRY(spp_VitacFullDecoding);
//
//    if (g_sppVitacDecParam[cs_type].is_punct)
//    {
//        UINT32* pPunct = (UINT32*)g_sppCsPuncturingTable[cs_type];
//        for (i=0; i < SPP_PUNCTURE_TABLE_SIZE; i++)
//            localPuncTable[i] = pPunct[i];
//        cmd = VITAC_PUNCTURED;
//    }
//    if (g_sppVitacDecParam[cs_type].states == VITAC_64_STATES)
//    {
//        nbStates = 64;
//        tail = 6;
//    }
//    // Init Path Metrics
//    for (i=0;i<(nbStates/2);i++)
//    {
//        PathMetric_ptr[i] = EQU_PM_INIT;
//    }
//    // Starting state
//    PathMetric[0] = 0;
//    // First check if VITAC is busy
//    if (spp_VitacBusy())
//    {
//        return vitac_err_busy;
//    }
//    else
//    {
//        // VITERBI TRELLIS
//        // Symbol buffer start address
//        hwp_vitac->ch_symb_addr = (UINT32)softBit_ptr;
//
//        // Puncturing Matrix base address
//        hwp_vitac->exp_symb_addr = (UINT32)localPuncTable;
//
//        // Path Metrics base address
//        hwp_vitac->pm_base_addr = (UINT32)PathMetric;
//        // Output start address
//        hwp_vitac->out_base_addr = (UINT32)SurvivorMEM;
//        // Polynom results
//        hwp_vitac->res_poly[0] = g_sppVitacPolyres[g_sppVitacDecParam[cs_type].resPoly0];
//        hwp_vitac->res_poly[1] = g_sppVitacPolyres[g_sppVitacDecParam[cs_type].resPoly1];
//        hwp_vitac->res_poly[2] = g_sppVitacPolyres[g_sppVitacDecParam[cs_type].resPoly2];
//        // Rescaling threshold
//        hwp_vitac->rescale = VITAC_PM_RESCALE_THRESH;
//        // Command
//        cmd |=  VITAC_START_DEC |
//                //VITAC_INT_MASK | // Masked by default
//                VITAC_NB_STATES(g_sppVitacDecParam[cs_type].states) |
//                VITAC_CODE_RATE(g_sppVitacDecParam[cs_type].rate) |
//                VITAC_NB_SYMBOLS(nb_symbols);
//        hwp_vitac->command = cmd;
//        // Poll
//        while(spp_VitacBusy());
//
//        // Clear Interrupt : Mandatory as it freeze VITAC !
//        spp_VitacIntClear();
//        // TRACEBACK
//        // Survivor Memory end address
//        hwp_vitac->exp_symb_addr = (UINT32)(&(((UINT16*)SurvivorMEM)[(nbStates/16)*(nb_symbols-1)]));
//
//        // Output start address
//        hwp_vitac->out_base_addr = (UINT32)(&output_ptr[(nb_symbols-tail-1)/32]);
//        // Command
//        hwp_vitac->command =
//            VITAC_START_TB |
//            //VITAC_INT_MASK | // Masked by default
//            VITAC_NB_STATES(g_sppVitacDecParam[cs_type].states) |
//            VITAC_NB_SYMBOLS((nb_symbols-tail));
//        // Poll
//        while(spp_VitacBusy());
//
//        // Clear Interrupt : Mandatory as it freeze VITAC !
//        spp_VitacIntClear();
//    }
//    SPP_PROFILE_FUNCTION_EXIT(spp_VitacFullDecoding);
//    return vitac_err_none;

    // SOFTWARE EQUIVALENT

    // Working buffers
    // Working variables

    // Control (Parameters) variables


    // Working variables
    UINT32        poly[6]= {0,0,0,0,0,0};
    UINT16        B[2];
    UINT16        NodeMetric[64][2];
    UINT16        path[2];
    UINT16        m, n, k, s, i;
    UINT8        *input;
    UINT8        *input_tmp;
    UINT8        out_bit[612];

    UINT8        trace[64][612];
    UINT16        survivor;
    UINT8        rescale=0;
    UINT8        rescale_count=0;
    UINT8        unpunctured[6]= {1,1,1,1,1,1};

    // Control (Parameters) variables
    CONST UINT32* CONST puncture_table= pCfg->punctTable;
    UINT8* out;
    UINT32 rate;
    UINT16 length;
    UINT8 nStt;
    UINT8 tail;
    UINT16 threshold=MAX_INT16/2;
    UINT16 min_path=MAX_INT16;

    input = (UINT8*) (pCfg->inSoftBit);
    out = (UINT8*) output_ptr;

    rate=(pCfg->vitacDecParam).rate;
    length=(UINT16)(pCfg->vitacDecParam).nbSymbol;

    if (VITAC_64_STATES == (pCfg->vitacDecParam).states)
    {
        nStt=64;
        tail=6;
    }
    else if (VITAC_16_STATES == (pCfg->vitacDecParam).states)
    {
        nStt=16;
        tail=4;
    }
    else
    {
        //exit("Invalid Viterbi Decoding State \n");
    }



    // In the following the treillis is determined by the generator polynoms.
    // Specific cases are mentionned.
    // Furthermore:
    // - CS_RACH and CS_PRACH are just emitted (not decoded)
    // - CS_CS4, CS_AFS_SID_FIRST, CS_AFS_ONSET, CS_AHS_SID_UPDATE_INH, CS_AHS_SID_FIRST_P1,
    //       CS_AHS_SID_FIRST_P2, CS_AHS_SID_FIRST_INH, CS_AHS_ONSET, CS_AHS_RATSCCH_MARKER do not
    //       need decoding by VITAC.
    // - FACCH_H, FACCH_F, SACCH, SDCCH, BCCH, AGCH (control channels) are considered to be CS_CS1
    // - The following case (default case) are decoded just like CS_FR (Speech Full Rate):
    //       CS_SCH, CS_CS1, CS_CS2, CS_CS3, CS_EFR, CS_H48, CS_F96, CS_F144, CS_AFS_SPEECH_122,
    //       CS_AHS_SPEECH_795, CS_AHS_SPEECH_74, CS_AHS_SPEECH_67, CS_AHS_SPEECH_59.
    switch (rate)
    {
        case VITAC_RATE_1_4:
            poly[0]=pCfg->convPoly0;
            poly[1]=pCfg->convPoly1;
            poly[2]=pCfg->convPoly2;
            poly[3]=pCfg->convPoly2;
            break;
        case VITAC_RATE_1_5:
            poly[0]=pCfg->convPoly0;
            poly[1]=pCfg->convPoly0;
            poly[2]=pCfg->convPoly1;
            poly[3]=pCfg->convPoly2;
            poly[4]=pCfg->convPoly2;
            break;
        case VITAC_RATE_1_6:
            poly[0]=pCfg->convPoly0;
            poly[1]=pCfg->convPoly1;
            poly[2]=pCfg->convPoly2;
            poly[3]=pCfg->convPoly0;
            poly[4]=pCfg->convPoly1;
            poly[5]=pCfg->convPoly2;
            break;
        case VITAC_RATE_1_3:
            poly[0]=pCfg->convPoly0;
            poly[1]=pCfg->convPoly1;
            poly[2]=pCfg->convPoly2;
            break;
        default:
            poly[0]=pCfg->convPoly0;
            poly[1]=pCfg->convPoly1;
            break;
    }


    if((pCfg->vitacDecParam).is_header)
    {
        for (n = 0; n < nStt; n++)
        {
            NodeMetric[n][0] = 0;
        }
    }
    else
    {
        // intialize metric
        for (n = 0; n < nStt; n++)
        {
            NodeMetric[n][0] = MAX_INT16/2;
        }
        // setup intiation state
        NodeMetric[0][0] = 0;
    }

    for (m = 0; m < length; m++)                                        // length/rate
    {
        min_path=MAX_INT16;
        // init for one transition
        if (puncture_table != NULL)
        {
            for(i=0; i<rate; i++)
            {
                if(  puncture_table[(rate*m+i)/32] & (1 << ((rate*m+i)%32) )  )
                    unpunctured[i]=1;
                else
                    unpunctured[i]=0;
            }
        }

        // loop on the 8 or 32 butterflies
        for (n = 0; n < nStt/2; n++)
        {

            B[0]=B[1]=0;
            input_tmp=input;

            for(i=0; i<rate; i++)
            {
                if(unpunctured[i])
                {
                    if( (UINT8)((poly[i]>>n)&0x1) == ((*input_tmp)>>7) )
                    {
                        B[1]+=(*input_tmp)&0x7F; //the branch1 is penalized
                        B[1]+=1;
                    }
                    else
                    {
                        B[0]+=(*input_tmp)&0x7F; //the branch0 is penalized
                        B[0]+=1;
                    }
                    input_tmp+=1;
                }
            }


            // one butterfly update with this loop
            for (k=0; k<2; k++)
            {

                path[0] = NodeMetric[n][(m)%2]        - rescale*threshold + B[0+k];
                path[1] = NodeMetric[n+nStt/2][(m)%2] - rescale*threshold + B[1-k];

                // swap the branch metric for the next state update
                if (path[0]<=path[1])
                {
                    if (path[0]<min_path) min_path=path[0];
                    NodeMetric[2*n+k][(m+1)%2] = path[0];
                    trace[2*n+k][m]=0;
                }
                else
                {
                    if (path[1]<min_path) min_path=path[1];
                    NodeMetric[2*n+k][(m+1)%2] = path[1];
                    trace[2*n+k][m]=1;
                }

            }//end loop for k=0..2

        }//end loop for n=0..nStt/2
        if (min_path>threshold)
        {

            rescale = 1;
            rescale_count++;
            //printf("rescaling of the path metrics\n");
        }
        else
        {
            rescale =0;
        }

        // update the pointer on softbits
        for(i=0; i<rate; i++)
        {
            if(unpunctured[i])
            {
                input+=1;
            }
        }
    }
    // end of trellis



    if((pCfg->vitacDecParam).is_header)
    {
        survivor=0;
        for (s = length -1; s >tail-1; s--)
        {
            if ( ((s-(2*length/3+tail))<0) && ((s-(length/3+tail))>=0))
            {
                out_bit[s-(length/3+tail)]   = trace[survivor][s];      //survivorData[survivor*612 + ll];
            }

            survivor = survivor/2+trace[survivor][s]*(nStt/2);        // survivorData[survivor*612 + ll]*(nStt/2);
            if(s<length/3+tail)
                break;
        }

        for (s = 0; s < (length-tail )/8+1; s++)
        {
            out[s] = 0;
        }

        for (s = 0; s < length/3 ; s++)
        {
            out[s / 8] |= ( (out_bit[s])<<(s % 8) );
        }

    }

    else
    {
        //  traceback
        survivor=0;
        for (s = (UINT16)length -1; s > tail-1; s--)                     // ((UINT16)length)/((UINT16)rate)
        {
            out_bit[s-tail]   = trace[survivor][s];
            survivor = survivor/2+trace[survivor][s]*(nStt/2);

        }// end for m=0 to length-2
        for (s = 0; s < ((UINT16)length-tail)/8+1; s++)                   // ((UINT16)length)/((UINT16)rate)
        {
            out[s] = 0;
        }

        for (s = 0; s < (UINT16)length-tail; s++)                       // ((UINT16)length)/((UINT16)rate)
        {
            out[s / 8] |= ( (out_bit[s])<<(s % 8) );
        }

    }




    return vitac_err_none;
}



//UINT8 spp_VitacGetRescales()
//{
//    UINT8 rescalings;
//
//    rescalings = ((hwp_vitac->status) >> 4) & 0xFF;
//    return(rescalings);
//}

//=============================================================================
// spal_VitacIntClear
//-----------------------------------------------------------------------------
// Clear the processing done interrupt
//
//=============================================================================
PROTECTED VOID spal_VitacIntClear()
{
    // Driver
//    /* Clear the end of process interrupt */
//    hwp_vitac->int_clear = VITAC_IT_CLEAR;
}






//=============================================================================
// spal_VitacDdfseHalfBurst()
//-----------------------------------------------------------------------------
/// Calculate the partial metrics used for the branch metrics and the ZF.
///
/// @param pSample SPAL_COMPLEX_T*     address of input samples buffer.
/// @param length UINT8                 length og the equalization.
/// @param inc INT8                     direction of the equalization.
/// @param StartHistory PUINT8          history for the metric initialization
/// @param BranchMetric SPAL_COMPLEX_T Partial branch metrics
/// @param SoftOutput INT16             soft-bit output
/// @author Tabakov Yordan
/// @date 16/08/06
///
//=============================================================================
PUBLIC VOID spal_VitacDdfseHalfBurst(UINT32* pSample,
                                     UINT8   length,
                                     INT8    inc,
                                     UINT8   logNoisePwr,
                                     PUINT8  StartHistory,
                                     UINT32* BmmlMetric,
                                     UINT32* ZfMetric,
                                     UINT8*  SoftOutput)
{

    // registry
    UINT8             i, j, m;
    INT8              k;
    INT8              bmShift, svShift;
    UINT8             ZfHist[STATE_LEN][ZF_TAP_LEN][2];
    INT16             NodeMetric[STATE_LEN][2];

//    PRIVATE UINT8     burst = 0;
//    FILE *pf;
//    FILE *pZ;
//    FILE *pN;
//    SPAL_COMPLEX_T *pS;
//    UINT8* pH;
//    INT32 idx;
//
//
//    if (!burst)
//    {
//
//        pZ = (inc > 0) ? fopen("vitacEqualizeZfBankForward.dat","w") :
//                         fopen("vitacEqualizeZfBankBackward.dat","w");
//
//        pN = (inc > 0) ? fopen("vitacEqualizeNodeMetricBestForward.dat","w") :
//                         fopen("vitacEqualizeNodeMetricBestBackward.dat","w");
//
//        pS = (inc > 0) ? pSample : &pSample[-length+1];
//        pf = (inc > 0) ? fopen("vitacEqualizeDataInForward.dat","w") :
//                         fopen("vitacEqualizeDataInBackward.dat","w");
//
//        for (idx = 0; idx < length; ++idx)
//        {
//            fprintf(pf,"0x%04x%04x,\n", pS->q & 0xffff, pS->i & 0xffff);
//            pS++;
//        }
//
//        fclose (pf);
//
//        pH = (inc > 0) ? StartHistory : &StartHistory[-ZF_TAP_LEN];
//        pf = (inc > 0) ? fopen("vitacEqualizeStartHistoryForward.dat","w") :
//                         fopen("vitacEqualizeStartHistoryBackward.dat","w");
//
//        for (idx = 0; idx < ZF_TAP_LEN + 1; ++idx)
//        {
//            fprintf(pf,"0x%02x,\n", pH[idx] & 0xff);
//        }
//
//        fclose (pf);
//
//        pS = (SPAL_COMPLEX_T *)&BmmlMetric[0];
//        pf = (inc > 0) ? fopen("vitacEqualizeBmmlZfForward.dat","w") :
//                         fopen("vitacEqualizeBmmlZfBackward.dat","w");
//
//        for (idx = 0; idx < ALL_TAP_LEN * STATE_LEN * STATE_LEN / 2; ++idx)
//        {
//            fprintf(pf,"0x%04x%04x,\n", pS->q & 0xffff, pS->i & 0xffff);
//            pS++;
//        }
//
//        fclose (pf);
//    }
//




    // initialize the Node Metric registry
    for (i = 0; i < STATE_LEN; i++)
    {
        OPL_MOV_16(NODE_MAX, NodeMetric[i][0]);
    }

    OPL_MOV_16(NODE_MIN, NodeMetric[StartHistory[ZF_TAP_LEN * inc]][0]);


    // initialize the ZF History array registry
    for (i = 0; i < STATE_LEN; i++)
    {
        for (j = 0, k = 0; j < ZF_TAP_LEN; j++, k += inc)
        {
            OPL_MOV_8(StartHistory[k], ZfHist[i][ZF_TAP_LEN - 1 - j][0]);
        }
    }

    // compute the shifts depending on the logNoisePwr (empirical)
    bmShift = (logNoisePwr < 10 ) ? 4 : logNoisePwr - 3;
    svShift = (bmShift > 9) ? 9 : bmShift;

    //bmShift = (logNoisePwr > 13 ) ?               13 : (logNoisePwr < 4 ) ? 4 : logNoisePwr;
    //svShift = (logNoisePwr > 13 ) ? logNoisePwr - 13 : 0;


    for (m = 0; m < length; m++, pSample += inc)
    {
        INT16 NMMin;
        SPAL_COMPLEX_T ZfBank[STATE_LEN];
        SPAL_COMPLEX_T * pBmml = (SPAL_COMPLEX_T *) BmmlMetric;
        SPAL_COMPLEX_T * pZfm  = (SPAL_COMPLEX_T *) ZfMetric;
        SPAL_COMPLEX_T * pSmpl = (SPAL_COMPLEX_T *) pSample;

        // compute the residual of each source node
        for (i = 0; i < STATE_LEN; i++)
        {
            OPL_ADD_16(pSmpl->i,    pZfm[STATE_LEN * STATE_LEN * 0 + STATE_LEN * ZfHist[i][0][m&1] + ZfHist[i][1][m&1]].i, ZfBank[i].i);
            OPL_ADD_16(pSmpl->q,    pZfm[STATE_LEN * STATE_LEN * 0 + STATE_LEN * ZfHist[i][0][m&1] + ZfHist[i][1][m&1]].q, ZfBank[i].q);
            OPL_ADD_16(ZfBank[i].i, pZfm[STATE_LEN * STATE_LEN * 1 + STATE_LEN * ZfHist[i][2][m&1] + ZfHist[i][3][m&1]].i, ZfBank[i].i);
            OPL_ADD_16(ZfBank[i].q, pZfm[STATE_LEN * STATE_LEN * 1 + STATE_LEN * ZfHist[i][2][m&1] + ZfHist[i][3][m&1]].q, ZfBank[i].q);
        }

//        if ((m < 10) && (!burst))
//        {
//            for (idx = 0; idx < STATE_LEN; ++idx)
//            {
//                fprintf(pZ,"0x%04x%04x,\n", ZfBank[idx].q & 0xffff, ZfBank[idx].i & 0xffff);
//            }
//
//            fprintf(pZ,"\n");
//        }


        // loop on the destination nodes for each symbol
        for (j = 0; j < STATE_LEN; j++)
        {
            UINT8 BestState;
            UINT8 NodeIdx;
            UINT8 NodeIdxBest[2];
            INT16 NodeMetricBest[3][2];

            // loop on the source nodes for each destination node
            for (i = 0; i < STATE_LEN; i++)
            {

                INT32 Accu;
                UINT16 Accu12;
                SPAL_COMPLEX_T Temp;

                // start with index 7
                NodeIdx = (i - 1) & 7;

                // Residual computation (the BranchMetric[0] table is re-arranged, see spal_XcorBmmlZf)
                OPL_ADD_16(ZfBank[NodeIdx].i, pBmml->i, Temp.i);
                OPL_ADD_16(ZfBank[NodeIdx].q, pBmml->q, Temp.q);

                pBmml++;

                // compute euclidian distance, rescale and compute the temporary path metric
                OPL_MOV_32((INT32)0,          Accu);
                OPL_MAC_32(Temp.i,   Temp.i,  Accu);
                OPL_MAC_32(Temp.q,   Temp.q,  Accu);
                OPL_SHR_32(Accu,     bmShift, Accu);

                Accu12 = (Accu > NODE_MAX) ? NODE_MAX : (UINT16)Accu;

                OPL_ADD_16(NodeMetric[NodeIdx][m&1], Accu12, Accu12);

                // Fast compare
                if (NodeIdx == 7 || NodeIdx == 0) // init MNTs
                {
                    OPL_MOV_8(NodeIdx,NodeIdxBest[NodeIdx & 0x1]);
                    OPL_MOV_16(Accu12,NodeMetricBest[0][!!(NodeIdx & 0x1)]);
                    OPL_MOV_16(Accu12,NodeMetricBest[1][!!(NodeIdx & 0x2)]);
                    OPL_MOV_16(Accu12,NodeMetricBest[2][!!(NodeIdx & 0x4)]);
                }
                else
                {
                    for (k = 0; k < 3; k++)
                    {
                        UINT8 bit = !!(NodeIdx & (1<<k));
                        if (Accu12 < NodeMetricBest[k][bit])
                        {
                            if (k==0)
                            {
                                OPL_MOV_8(NodeIdx,NodeIdxBest[bit]);
                            }
                            OPL_MOV_16(Accu12,NodeMetricBest[k][bit]);
                        }
                    }


                }

//                if ((m < 10) && (!burst))
//                {
//                    for (idx = 0; idx < 3; ++idx)
//                    {
//                        fprintf(pN,"0x%04x, 0x%04x\n", NodeMetricBest[idx][1] & 0xffff, NodeMetricBest[idx][0] & 0xffff);
//                    }
//                    fprintf(pN,"0x%04x, 0x%04x\n", NodeIdxBest[1], NodeIdxBest[0]);
//                    fprintf(pN,"0x%04x, 0x%04x\n", Accu & 0xffff,  Accu16 & 0xffff);
//                    fprintf(pN,"\n");
//                }


            } // end of source nodes (branch loop)


            // condition allows finishing the traceback
            // on the known value of the training sequence
            BestState = (NodeMetricBest[0][0] < NodeMetricBest[0][1]) ? NodeIdxBest[0] : NodeIdxBest[1];

            // loop on each soft bit
            for (i = 0; i < 3; i++)
            {
                INT16 A,B,C;
                UINT8 Soft8;

                // select the operands
                A  = (!(BestState & (1<<i))) ? NodeMetricBest[i][1] : NodeMetricBest[i][0];
                B  = (!(BestState & (1<<i))) ? NodeMetricBest[i][0] : NodeMetricBest[i][1];

                // compute the soft value weight
                OPL_SUB_16(A,B,C);
                OPL_SHR_16(C, 9 - svShift, C);

                // cast to 7 bits
                Soft8 = (C >  0x7f) ? (UINT8)0x7f : (UINT8)C;

                // add sign bit
                SoftOutput[j*HALF_BURST_LEN*4 + m*4+2-i] = (((!!(BestState & (1<<i)))<<7) | Soft8);
            }

            // for the traceback
            SoftOutput[j*HALF_BURST_LEN*4 + m*4+3] = BestState;


            // Update node metric
            NodeMetric[j][(m + 1) & 1] = (NodeMetricBest[0][0] < NodeMetricBest[0][1]) ? NodeMetricBest[0][0] : NodeMetricBest[0][1];


            // Update history
            OPL_MOV_8(BestState, ZfHist[j][0][(m+1)&1]);

            for (i = 0; i < ZF_TAP_LEN - 1; i++)
            {
                OPL_MOV_8(ZfHist[BestState][i][m&1], ZfHist[j][i+1][(m+1)&1]);
            }

        } // end of destination node (node loop)

        // rescale the node metrics
        NMMin = NodeMetric[0][(m + 1) & 1];

        for (i = 1; i < STATE_LEN; i++)
        {
            if (NMMin > NodeMetric[i][(m + 1) & 1])
            {
                OPL_MOV_16(NodeMetric[i][(m + 1) & 1], NMMin);
            }
        }

        for (i = 0; i < STATE_LEN; i++)
        {
            OPL_SUB_16(NodeMetric[i][(m + 1) & 1], NMMin, NodeMetric[i][(m + 1) & 1]);
        }

    }  // end of sample loop


//    if (!burst)
//    {
//        FILE *pf;
//        UINT8* pH;
//        INT32 j;
//
//        pH = &SoftOutput[0][0];
//        pf = (inc > 0) ? fopen("vitacEqualizeSoftOutForward.dat","w") :
//                         fopen("vitacEqualizeSoftOutBackward.dat","w");
//
//        for (idx = 0; idx < HALF_BURST_LEN; ++idx)
//        {
//            for (j = 0; j < STATE_LEN; ++j)
//
//            fprintf(pf,"0x%02x%02x%02x%02x,\n", pH[j*HALF_BURST_LEN*4 + 4*idx+3] & 0xff, pH[j*HALF_BURST_LEN*4 + 4*idx+0] & 0xff, pH[j*HALF_BURST_LEN*4 + 4*idx+1] & 0xff, pH[j*HALF_BURST_LEN*4 + 4*idx+2] & 0xff);
//        }
//
//        fclose (pf);
//        fclose (pN);
//        fclose (pZ);
//
//        if (inc < 0) burst = 1;
//    }


}
























































































































































