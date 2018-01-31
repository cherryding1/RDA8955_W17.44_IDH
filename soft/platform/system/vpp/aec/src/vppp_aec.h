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


















#ifndef VPPP_HELLO_H
#define VPPP_HELLO_H

#include "cs_types.h"

///@defgroup vppp_hello_struct
///@{

// ============================================================================
// Types
// ============================================================================


// ============================================================================
// VPP_HELLO_CODE_CFG_T
// ----------------------------------------------------------------------------
/// Internal VPP Hello configuration structure
// ============================================================================

typedef struct
{
    /// pointer to the hi code
    INT32* hiPtr;
    /// pointer to the bye code
    INT32* byePtr;
    /// pointer to the data constants
    INT32* constPtr;

} VPP_HELLO_CODE_CFG_T;



// ============================================================================
// VPP_SPEECH_AEC_INIT_T
// ----------------------------------------------------------------------------
/// Internal VPP AEC initialisation structure
// ============================================================================
typedef struct
{
    //ProcessPara
    INT16 ProcessMode;
    INT16 InitRegularFactor;
    INT16 AFUpdateFactor;
    INT16 AFCtrlSpeedUpFactor;
    INT16 AFFilterAmp;

    INT16 EchoEstMethod;
    INT16 NoiseUpdateFactor;
    INT16 SNRUpdateFactor;
    INT16 SNRLimit;
    INT16 NoiseOverEstFactor;
    INT16 NoiseGainLimit;
    INT16 EchoUpdateFactor;
    INT16 SERUpdateFactor;
    INT16 SERLimit;
    INT16 EchoOverEstFactor;
    INT16 EchoGainLimit;
    INT16 CNGFactor;
    INT16 AmpGain;
    INT32 NoiseMin;


    INT32 PFPrevEchoEstLimit;
    INT16 PFDTEchoOverEstFactor;

    INT16 pPFEchoGainLimitAmp[9];



    //soft limiter

//  INT16 SoftLimiter_Beta_order;
//  INT16 SoftLimiter_Scale;
//  INT32 SoftLimiter_T;


    //NonLinear filter

//  INT16 NonLinear_Beta_order;
//  INT16 NonLinear_Scale;
//  INT32 NonLinear_T;


    //pre-filter

//  INT16 g_PE_PPFCoef[5]; //g_PE_PPFilterLen 5
//  INT16 NoiseWeight_threshold;

    INT16 NoiseGainLimitStep;
    INT16 AmpThr;

    INT16 PFCLDataSmoothFactor;
    INT16 PFCLNoiseUpdateFactor;
    INT16 PFCLThrSigDet;
    INT16 PFCLThrDT;

    INT16 DataSmoothFactor;
    INT16 PFCLChanlGain;

    INT16 PFCLThrNT;
    INT16 PFCLThrDTDiff;
    INT16 PFCLDTHoldTime;
    INT16 PFCLDTStartTime;

    INT16 PFCLDTDUpdateFactor;
    INT16 PFCLDTDThreshold;
    INT16 PFCLDTD2ndThreshold;
    INT16 StrongEchoFlag;

    INT16 PFCLDTDThrRatio;

} VPP_SPEECH_AEC_INIT_T;






///  @} <- End of the vppp_hello_struct group


#endif  // VPPP_HELLO_H
