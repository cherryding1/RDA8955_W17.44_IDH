/*************************************************************************/
/*                                                                       */
/*              Copyright (C) 2006, Coolsand Technologies, Inc.          */
/*                            All Rights Reserved                        */
/*                                                                       */
/*************************************************************************/
/*                                                                       */
/* FILE NAME                                                             */
/*      vppp_speech.h                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*     This is private header of VPP Speech.                             */
/*                                                                       */
/*************************************************************************/
///
///   @file vppp_speech.h
///   This is private header of VPP Speech.
///

///
///  @mainpage Granite VOC Processing Control API
///  Granite VOC Processing Control (VPP) API description.
///  @author Coolsand Technologies, Inc.
///  @version 1.0
///  @date    march 2007
///
///  @par Purpose
///
///  This document describes the Granite software VPP layer.
///
///  @par
///
///  VPP API is composed of the following division :
///  - @ref vpp_struct "vpp structures and defines" \n
///  - @ref vpp_func "VPP API functions " \n
///

#ifndef VPPP_SPEECH_H
#define VPPP_SPEECH_H

#include "cs_types.h"

///@defgroup vppp_struct
///@{

// ============================================================================
// Types
// ============================================================================

// ============================================================================
// VPP_SPEECH_CODE_IDX_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech code pointer indexes
// ============================================================================
typedef enum
{
   VPP_SPEECH_EFR_AMR_IDX = 0,
   VPP_SPEECH_FR_IDX,
   VPP_SPEECH_HR_IDX,
   VPP_SPEECH_AMR_IDX,
   VPP_SPEECH_EFR_IDX,
   VPP_SPEECH_AMR_ENC_475_IDX,
   VPP_SPEECH_AMR_ENC_515_IDX,
   VPP_SPEECH_AMR_ENC_59_IDX,
   VPP_SPEECH_AMR_ENC_67_IDX,
   VPP_SPEECH_AMR_ENC_74_IDX,
   VPP_SPEECH_AMR_ENC_795_IDX,
   VPP_SPEECH_AMR_ENC_102_IDX,
   VPP_SPEECH_AMR_ENC_122_IDX,
   VPP_SPEECH_AMR_DEC_475_IDX,
   VPP_SPEECH_AMR_DEC_515_IDX,
   VPP_SPEECH_AMR_DEC_59_IDX,
   VPP_SPEECH_AMR_DEC_67_IDX,
   VPP_SPEECH_AMR_DEC_74_IDX,
   VPP_SPEECH_AMR_DEC_795_IDX,
   VPP_SPEECH_AMR_DEC_102_IDX,
   VPP_SPEECH_AMR_DEC_122_IDX,
   VPP_SPEECH_CODE_QTY

} VPP_SPEECH_CODE_IDX_T;


// ============================================================================
// VPP_SPEECH_CONST_IDX_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech const pointer indexes
// ============================================================================
typedef enum
{
   VPP_SPEECH_EFR_AMR_CONST_X_IDX = 0,
   VPP_SPEECH_EFR_AMR_CONST_Y_IDX,
   VPP_SPEECH_HR_CONST_X_IDX,
   VPP_SPEECH_HR_CONST_Y_IDX,
   VPP_SPEECH_CONST_QTY

} VPP_SPEECH_CONST_IDX_T;


// ============================================================================
// VPP_SPEECH_IRQGEN_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech indexes for interrupt generation
// ============================================================================
typedef enum
{
   VPP_SPEECH_IRQGEN_NONE    = 0,
   VPP_SPEECH_IRQGEN_ENC     = 1,
   VPP_SPEECH_IRQGEN_DEC     = 2,
   VPP_SPEECH_IRQGEN_DECENC  = 3

} VPP_SPEECH_IRQGEN_T;


// ============================================================================
// VPP_SPEECH_INIT_T
// ----------------------------------------------------------------------------
/// Internal VPP Speech initialisation structure
// ============================================================================
typedef struct
{
    INT16 codeStateTab[4];
    INT32 *codePtrTab[VPP_SPEECH_CODE_QTY];
    INT32 *constPtrTab[VPP_SPEECH_CONST_QTY];
    INT16 reset;
    INT16 irqgen;

} VPP_SPEECH_INIT_T;



// ============================================================================
// VPP_SPEECH_AEC_INIT_T
// ----------------------------------------------------------------------------
/// Internal VPP AEC initialisation structure
// ============================================================================
typedef struct
{
	//enable flag
	INT16 AecEnbleFlag;
	INT16 AgcEnbleFlag;

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


typedef struct
{
    INT16 DelaySampleNum;
    INT16 FrameCntLimitNum[3];
    INT16 NoiseCntLimitNum[4];
    //INT16 NoiseGainLimitMax;
    //INT16 NoiseGainLimitMin;
    //INT16 NoiseGainLimitSNRTh1;
    //INT16 NoiseGainLimitSNRTh2;
    INT16 NoiseWeightSNRLimitMax;
    INT16 NoiseWeightSNRLimitMin;
    INT16 NoiseWeightSNRTh1;
    INT16 NoiseWeightSNRTh2;
    INT16 NoiseWeightSNRLimitStep;
    INT16 NoiseWeightSub1DecayFactor;
    INT16 ResidualEchoEnable;
    INT16 AGCRrrSpkRelation;
    INT16 AddAttDB;
    INT16 MinAttDB;
    INT16 MaxAttDB;
    INT16 NPowRiseFactor;
    INT16 WorkFactFactor;
    INT16 AttExtFactor;
    INT16 AttRiseFactor;
    INT16 AttDecayFactor;
    INT16 Log2LinFactor;
} VPP_SPEECH_AEC_EXTRA_T;





///  @} <- End of the vppp_struct group


#endif  // VPPP_SPEECH_H
