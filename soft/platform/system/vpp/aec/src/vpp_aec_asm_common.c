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




#include "vppp_aec_asm_common.h"
#include "vppp_aec_asm_map.h"
#include "vppp_aec_asm_sections.h"

#include "voc2_library.h"
#include "voc2_define.h"


#if 0

// voc_enum mode
voc_alias VPP_HELLO_MODE_NO              -1, x
voc_alias VPP_HELLO_MODE_HI               0, x
voc_alias VPP_HELLO_MODE_BYE              1, x
// voc_enum_end

// voc_enum error
voc_alias VPP_HELLO_ERROR_NO              0, x
voc_alias VPP_HELLO_ERROR_YES            -1, x
// voc_enum_end

// voc_enum hello status
voc_alias VPP_HELLO_SAYS_HI          0x0111, x
voc_alias VPP_HELLO_SAYS_BYE         0x0B1E, x
// voc_enum_end


// voc_enum AEC_ProcessMode
voc_alias PassThrough   0,x
voc_alias AdaptiveFilter 1,x
voc_alias AdaptiveFilterPostFilter 2,x
voc_alias NoiseSuppression  3,x
voc_alias LowDelayOpenLoopAF 4,x
voc_alias TwoStepNoiseSuppression  5,x
voc_alias AdaptiveFilterPostFilterEnhanced  6,x
// voc_enum_end

// voc_enum PF_ECHOEstMethod
voc_alias ECHO_COHF   0,x
voc_alias ECHO_TF 1,x
// voc_enum_end

// voc_enum NonLinearTransform
//  voc_alias NonLinearTransform_Beta_order               0x0000, x
//  voc_alias NonLinearTransform_T_HI                   0x0001, x
//  voc_alias NonLinearTransform_T_LO                     0x0000, x
//  voc_alias NonLinearTransform_Scale                    0x7fff, x
//voc_alias NonLinearTransform_CosPhaseBitNum             0x0006, x
// voc_enum_end



voc_struct VPP_HELLO_CFG_STRUCT __attribute__((export)),y

// ----------------
// common config
// ----------------
voc_short GLOBAL_MODE
voc_short GLOBAL_RESET_ADDR

voc_struct_end


voc_struct VPP_HELLO_STATUS_STRUCT __attribute__((export)),y

// ----------------
// common status
// ----------------
voc_short GLOBAL_USED_MODE
voc_short GLOBAL_ERR_STATUS

// ----------------
// hello status
// ----------------
voc_word  GLOBAL_HELLO_STATUS

voc_struct_end


voc_struct VPP_HELLO_CODE_CFG_STRUCT __attribute__((export)),y

voc_word  GLOBAL_HI_CODE_PTR
voc_word  GLOBAL_BYE_CODE_PTR
voc_word  GLOBAL_CONST_PTR

voc_struct_end


voc_struct VPP_HELLO_CONST_STRUCT,x

voc_word VPP_HELLO_CONST_SAYS_HI
voc_word VPP_HELLO_CONST_SAYS_BYE

voc_struct_end

//global variables
voc_short GLOBAL_INPUT_MIC_PCM_ADDR,160,x
voc_short GLOBAL_INPUT_SPK_PCM_ADDR,160,x

voc_short GLOBAL_OUTPUT_MIC_PCM_ADDR,160,x

voc_short r_SpkAnaFilterDelayLine[PROTOTYPE_FILTER_LEN],x       // spk Analysis Filter bank Delay Line
voc_short r_MicAnaFilterDelayLine[PROTOTYPE_FILTER_LEN],x       // mic Analysis Filter bank Delay Line
voc_short r_OutSynFilterDelayLine[PROTOTYPE_FILTER_LEN],x       // Out Synthesis Filter bank Delay Line

voc_short g_FilterBank_MulBuf[PROTOTYPE_FILTER_LEN],x           // Filter Bank Inlternal Mul buffer

voc_short g_SpkAnaFilterOutBufI[SUBBAND_CHANNEL_NUM],x      // Spk Analysis Filter Output Buffer
voc_short g_SpkAnaFilterOutBufQ[SUBBAND_CHANNEL_NUM],x      // Spk Analysis Filter Output Buffer
voc_short g_MicAnaFilterOutBufI[SUBBAND_CHANNEL_NUM],x      // Mic Analysis Filter Output Buffer
voc_short g_MicAnaFilterOutBufQ[SUBBAND_CHANNEL_NUM],x      // Mic Analysis Filter Output Buffer

voc_short g_OutSynFilterInBufI[SUBBAND_CHANNEL_NUM],x       // Out Synthesis Filter Input Buffer
voc_short g_OutSynFilterInBufQ[SUBBAND_CHANNEL_NUM],x       // Out Synthesis Filter Input Buffer



voc_short r_AF_SpkSigBufI[SUBBANDFILTER_NUM*FILTER_LEN],x   // Spk Sig Buffer for Adaptive Filter
voc_short r_AF_SpkSigBufQ[SUBBANDFILTER_NUM*FILTER_LEN],x   // Spk Sig Buffer for Adaptive Filter


voc_word r_AF_FilterBufI[SUBBANDFILTER_NUM*FILTER_LEN],x    // Filter Buf for Adaptive Filter
voc_word r_AF_FilterBufQ[SUBBANDFILTER_NUM*FILTER_LEN],x    // Filter Buf for Adaptive Filter


voc_short g_AF_FilterOutBufI[SUBBANDFILTER_NUM],x           // Adaptive Filter Output
voc_short reserve00_addr,x
voc_short g_AF_FilterOutBufQ[SUBBANDFILTER_NUM],x           // Adaptive Filter Output
voc_short reserve01_addr,x


voc_short r_AF_OldSpkDataBufI[SUBBANDFILTER_NUM],x          // Old Spk Data for Adatpive Filter (AF)
voc_short reserve02_addr,x
voc_short r_AF_OldSpkDataBufQ[SUBBANDFILTER_NUM],x          // Old Spk Data for Adatpive Filter (AF)
voc_short reserve03_addr,x

voc_short g_AF_ErrDataBufI[SUBBANDFILTER_NUM],x             // Err Data Buf for AF
voc_short reserve04_addr,x
voc_short g_AF_ErrDataBufQ[SUBBANDFILTER_NUM],x             // Err Data Buf for AF
voc_short reserve05_addr,x

voc_short g_AF_ShortFilterBufI[FILTER_LEN],x                        // Short Buffer
voc_short g_AF_ShortFilterBufQ[FILTER_LEN],x                    // Short Buffer

voc_word g_AF_tempCmplxBufI[FILTER_LEN],x                       // Temp Complex Buffer
voc_word g_AF_tempCmplxBufQ[FILTER_LEN],x                       // Temp Complex Buffer


voc_word r_AF_SpkSigPower[SUBBANDFILTER_NUM],x              // Spk Sig Power for Adaptive Filter
voc_word r_AF_SpkSigSmoothPower[SUBBANDFILTER_NUM],x        // Spk Sig Smooth Power for Adaptive Filter

voc_word r_AF_RErrSpkI[SUBBANDFILTER_NUM],x             // Correlation between Mic and Spk for Adaptive Filter
voc_word r_AF_RErrSpkQ[SUBBANDFILTER_NUM],x             // Correlation between Mic and Spk for Adaptive Filter


voc_short g_AF_StepSize[SUBBANDFILTER_NUM],x                // Adaptive Step Size
voc_short reserve06_addr,x


voc_word r_PF_SmoothErrPower[SUBBANDFILTER_NUM],x           // Smooth Err Power
voc_word r_PF_NoisePowerEstBuf[SUBBANDFILTER_NUM],x         // Noise Power Est Buffer for Post Filter (PF)

voc_short r_PF_NoiseFloorDevCnt[SUBBANDFILTER_NUM],x        // Noise Floor Deviation Count (PF)
voc_short g_DTD_ch,x

voc_short g_PF_PostSToNRatio[SUBBANDFILTER_NUM],x           // Post SNR for PF
voc_short CLChnl,x


voc_word r_PF_PrevSigPowerNBuf[SUBBANDFILTER_NUM],x     // Prev Sig Power for Noise Suppression (PF)
voc_word r_PF_PrevSigPowerEBuf[SUBBANDFILTER_NUM],x     // Prev Sig Power for Echo Suppression (PF)

voc_short g_PF_NoiseWeight[SUBBANDFILTER_NUM],x             // Noise Weight
// Echo Canceller Registers
voc_short r_EC_ErrToOutRatio,x      // Err to Out Ratio


voc_short g_PF_NoiseAmpWeight[SUBBANDFILTER_NUM],x          // Noise Amp Weight
voc_short m_AF_FrameCnt,x
//  voc_short reserve09_addr,x



voc_short m_Taps[CNGTapLen],x
voc_short m_Registers[CNGRegLen],x

voc_short TEMP_r_AF_SpkSigBufIQ_addr_addr[2],x
voc_short TEMP_r_AF_FilterBufIQ_addr_addr[2],x
voc_short TEMP_g_AF_FilterOutBufIQ_addr_addr[2],x
voc_short TEMP_r_AF_OldSpkDataBufIQ_addr_addr[2],x
voc_short TEMP_g_AF_ErrDataBufIQ_addr_addr[2],x
voc_short TEMP_g_MicAnaFilterOutBufIQ_addr_addr[2],x
voc_short TEMP_g_SpkAnaFilterOutBufIQ_addr_addr[2],x
voc_short TEMP_r_AF_SpkSigPower_addr_addr[2],x
//  voc_short temp_buffer_addr[8],x

//constant tables

voc_short AnaFilterCoef[PROTOTYPE_FILTER_LEN],x
voc_short SynFilterCoef[PROTOTYPE_FILTER_LEN],x
voc_short wnreal[8],x
voc_short wnimag[8],x
voc_short AdaptTable[32],x

voc_short g_counter_subframe,x
voc_short g_ch,x


//  voc_word CONST_32bit0_addr,x
//  voc_short CONST_1_addr           ,x

voc_word real_int,16,x
voc_word imag_int,16,x


//voc_word UpLimit,x
//voc_word DnLimit,x

//  voc_word Minus_UpLimit,x
//  voc_word Minus_DnLimit,x


//voc_short g_PE_PPFilterBuf[g_PE_PPFilterLen+1],x                              // Filter Buffers
//voc_word g_PE_PPFilterMulBuf[g_PE_PPFilterLen],x



//add new
voc_word r_PF_CL_MicSmoothPower[PF_CL_CHNLNUM],x                            // Mic Smooth Power for PF Control Logic
voc_word r_PF_CL_MicNoise[PF_CL_CHNLNUM],x                                  // Mic Noise for PF Control Logic
voc_word r_PF_CL_SpkSmoothPower[PF_CL_CHNLNUM],x                            // Spk Smooth Power for PF Control Logic
voc_word r_PF_CL_SpkNoise[PF_CL_CHNLNUM],x                                  // Spk Noise for PF Control Logic

voc_short r_PF_CL_MicNoiseFloorDevCnt[PF_CL_CHNLNUM],x                      // Mic Noise Floor Deviation count for PF Control Logic
voc_short r_PF_CL_SpkNoiseFloorDevCnt[PF_CL_CHNLNUM],x                      // Spk Noise Floor Deviation count for PF Control Logic

voc_short r_PF_CL_ifMicSigOn[PF_CL_CHNLNUM],x                                   // if Sig on state for PF Control Logic
voc_short r_PF_CL_ifSpkSigOn[PF_CL_CHNLNUM],x                                   // if Sig on state for PF Control Logic
voc_short r_PF_CL_ChnlState[PF_CL_CHNLNUM],x                                    // State for PF control logic


voc_short r_PF_CL_State,x                                       // State for PF control logic
voc_short r_PF_CL_DTCountDown,x                             // DT Count Down Reg for PF Control Logic


voc_word r_PF_NoisePowerSmoothEstBuf[SUBBANDFILTER_NUM],x           // Noise Power Est Buffer for Post Filter (PF)

voc_short g_PF_PriorSToNRatio[SUBBANDFILTER_NUM],x
voc_short g_PF_PriorSToERatio[SUBBANDFILTER_NUM],x          // Prior SER for PF


voc_word r_PF_SpkPowerBuf[SUBBANDFILTER_NUM],x          // Spk Power Buf for PF


voc_word r_PF_CCErrSpkVecBufI[SUBBANDFILTER_NUM*FILTER_LEN],x       // Cross Corr between Err and Spk (PF) Vec
voc_word r_PF_CCErrSpkVecBufQ[SUBBANDFILTER_NUM*FILTER_LEN],x       // Cross Corr between Err and Spk (PF) Vec


voc_word r_PF_EchoPowerEstBuf[SUBBANDFILTER_NUM],x          // Echo Power Est Buffer for Post Filter (PF)

voc_short g_PF_EchoWeight[SUBBANDFILTER_NUM],x              // Echo Weight

voc_short r_PF_CL_DTCountUp,x


voc_word r_PF_CL_ErrSmoothPower[PF_CL_CHNLNUM],x                            // Spk Smooth Power for PF Control Logic

voc_short r_AF_FilterAmpI[SUBBANDFILTER_NUM],x          // Filter Amp I
voc_short r_AF_FilterAmpQ[SUBBANDFILTER_NUM],x              // Filter Amp Q



voc_word r_PF_PrevEchoPowerEst[SUBBANDFILTER_NUM],x     // Prev Echo Power Est for Post Filter (PF)
voc_word r_PF_PrevSpkPower[SUBBANDFILTER_NUM],x             // Prev Spk Power Est for Post Filter (PF)


voc_word  r_PF_CL_DTDXcorrMicErrI[PF_CL_CHNLNUM],x                          // Xcorr Mic and Err for DTD
voc_word  r_PF_CL_DTDXcorrMicErrQ[PF_CL_CHNLNUM],x                          // Xcorr Mic and Err for DTD
voc_word  r_PF_CL_DTDXcorrMicEchoI[PF_CL_CHNLNUM],x                         // Xcorr Mic and Echo for DTD
voc_word  r_PF_CL_DTDXcorrMicEchoQ[PF_CL_CHNLNUM],x                         // Xcorr Mic and Echo for DTD
voc_word  r_PF_CL_DTDMicSmoothPower[PF_CL_CHNLNUM],x                    // Micpower for DTD
voc_word  r_PF_CL_DTDErrSmoothPower[PF_CL_CHNLNUM],x                    // Errpower for DTD
voc_word  r_PF_CL_DTDEchoSmoothPower[PF_CL_CHNLNUM],x                       // Echopower for DTD
voc_word  r_PF_CL_DTDMicNoisePower[PF_CL_CHNLNUM],x                         // Mic Noise for DTDs
voc_word  r_PF_CL_DTDMicNoiseFloorDevCnt[PF_CL_CHNLNUM],x                   // Mic Noise Floor Deviation count for PF DTD Control Logic
voc_word  r_PF_CL_DTDDecision[PF_CL_CHNLNUM],x                              // Decision Value for DTD
voc_word  r_PF_CL_DTD2ndDecision[PF_CL_CHNLNUM],x                           // 2nd Decision Value for DTD



voc_short pInstantSNR_t,x
voc_short pInstantSNR_f,x

voc_word SumMicPower,x
voc_word SumSpkPower,x
voc_word SumErrPower,x

voc_short PFCLDTDOut[PF_CL_CHNLNUM],x







voc_struct m_AECProcPara_ADDR __attribute__((export)),y

voc_short m_AECProcPara_ProcessMode
voc_short m_AECProcPara_InitRegularFactor
voc_short m_AECProcPara_AFUpdateFactor
voc_short m_AECProcPara_AFCtrlSpeedUpFactor;
voc_short m_AECProcPara_AFFilterAmp;

voc_short m_AECProcPara_EchoEstMethod
voc_short m_AECProcPara_NoiseUpdateFactor
voc_short m_AECProcPara_SNRUpdateFactor
voc_short m_AECProcPara_SNRLimit
voc_short m_AECProcPara_NoiseOverEstFactor
voc_short m_AECProcPara_NoiseGainLimit
voc_short m_AECProcPara_EchoUpdateFactor
voc_short m_AECProcPara_SERUpdateFactor
voc_short m_AECProcPara_SERLimit
voc_short m_AECProcPara_EchoOverEstFactor
voc_short m_AECProcPara_EchoGainLimit
voc_short m_AECProcPara_CNGFactor
voc_short m_AECProcPara_AmpGain
voc_word m_AECProcPara_NoiseMin


voc_word m_AECProcPara_PFPrevEchoEstLimit;

voc_short m_AECProcPara_PFDTEchoOverEstFactor;

voc_short m_AECProcPara_pPFEchoGainLimitAmp[9];



//voc_short SoftLimiter_Beta_order_ADDR
//  voc_short SoftLimiter_Scale_ADDR
//  voc_word SoftLimiter_T_ADDR

//voc_short NonLinear_Beta_order_ADDR
//voc_short NonLinear_Scale_ADDR
//voc_word NonLinear_T_ADDR

//  voc_short g_PE_PPFCoef_ADDR[g_PE_PPFilterLen]               //= {6585,2430,22692,2430,6585};// Filter Coef
//  voc_short NoiseWeight_threshold_ADDR


voc_short m_AECProcPara_NoiseGainLimitStep
voc_short m_AECProcPara_AmpThr_ADDR

voc_short m_AECProcPara_PFCLDataSmoothFactor
voc_short m_AECProcPara_PFCLNoiseUpdateFactor
voc_short m_AECProcPara_PFCLThrSigDet
voc_short m_AECProcPara_PFCLThrDT

voc_short m_AECProcPara_DataSmoothFactor;
voc_short m_AECProcPara_PFCLChanlGain;


voc_short m_AECProcPara_PFCLThrNT;
voc_short m_AECProcPara_PFCLThrDTDiff;
voc_short m_AECProcPara_PFCLDTHoldTime;
voc_short m_AECProcPara_PFCLDTStartTime;


voc_short m_AECProcPara_PFCLDTDUpdateFactor;
voc_short m_AECProcPara_PFCLDTDThreshold;
voc_short m_AECProcPara_PFCLDTD2ndThreshold;
voc_short m_AECProcPara_StrongEchoFlag;


voc_short AECProcPara_PFCLDTDThrRatio;
voc_short AECProcPara_reserve;


voc_struct_end


#endif


// ----------------------------
// fct : vpp_HelloMain
// in  :
// out :
// not modified :
// used : all
// ----------------------------

void vpp_HelloMain(void)
{
    // set the stack pointers
    VoC_lw16i(SP16,SP16_ADDR);
    VoC_lw16i(SP32,SP32_ADDR);


    // Init : clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);


HELLO_LABEL_MAIN_LOOP:

    // Enable the SOF0 event only
    VoC_cfg_lg(CFG_WAKEUP_SOF0,CFG_WAKEUP_MASK);
    // Stall the execution and generate an interrupt
    VoC_cfg(CFG_CTRL_STALL|CFG_CTRL_IRQ_TO_XCPU);

// VoC_directive: PARSER_OFF
    // used for simulation
    VoC_cfg_stop;
// VoC_directive: PARSER_ON


    VoC_NOP();

    VoC_NOP();


    // Clear all pending status
    VoC_cfg_lg(CFG_WAKEUP_ALL,CFG_WAKEUP_STATUS);

    // Disable all programed events, enable DMAI event.
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_MASK);

    // push the wakeup mask on the stack
    VoC_push16(REG0,DEFAULT);

    // check if code and constants reload is necessary
    // (compare the new and the old (status) mode


    VoC_jal(Coolsand_AECReset);

    // ----------------------------
    // fct : vpp_HelloCodeConstReload
    // in :  REG0 -> GLOBAL_MODE
    // not modified : REG0
    // unused : REG45, RL67, ACC1
    // ----------------------------
    VoC_lw16_d(REG0,GLOBAL_MODE);
    VoC_jal(vpp_HelloCodeConstReload);

    VoC_jal(Coolsand_AEC);


    VoC_jump(HELLO_LABEL_MAIN_LOOP);

}


// ----------------------------
// fct : vpp_HelloCodeConstReload
// in :  REG0 -> GLOBAL_MODE
// not modified : REG0
// unused : REG45, RL67, ACC1
// ----------------------------
void vpp_HelloCodeConstReload(void)
{
    // old (status) mode
    VoC_lw16_d(REG1,GLOBAL_USED_MODE);
    VoC_be16_rr(CODE_RELOAD_NO,REG0,REG1);

    VoC_push16(RA,DEFAULT);

    VoC_lw16i_short(REG5,VPP_HELLO_MODE_NO,IN_PARALLEL);
    VoC_lw16i_short(REG6,VPP_HELLO_MODE_HI,DEFAULT);
    VoC_lw16i_short(REG7,VPP_HELLO_MODE_BYE,IN_PARALLEL);

    // if the old mode is NO, reload the constants
    VoC_bne16_rr(CODE_RELOAD_CONST,REG1,REG5);


CODE_RELOAD_CONST:

    // check which section to reload
    VoC_lw32_d(ACC0,GLOBAL_HI_CODE_PTR);
    VoC_lw16i(REG2,SECTION_HI_START);
    VoC_lw16i(REG3,SECTION_HI_SIZE);

    VoC_be16_rr(CODE_RELOAD_DO,REG0,REG6);

    VoC_lw32_d(ACC0,GLOBAL_BYE_CODE_PTR);
    VoC_lw16i(REG2,SECTION_BYE_START);
    VoC_lw16i(REG3,SECTION_BYE_SIZE);

CODE_RELOAD_DO:

    VoC_jal(vpp_HelloRunDma);

    VoC_pop16(RA,DEFAULT);
    VoC_lw16i_short(REG1,1,IN_PARALLEL);

    // update the mode status
    VoC_sw16_d(REG0,GLOBAL_USED_MODE);

    // re-enable the global reset
    VoC_sw16_d(REG1,GLOBAL_RESET_ADDR);

CODE_RELOAD_NO:

    VoC_return;
}


// ----------------------------
// fct : vpp_HelloRunDma
// in :  REG2 -> LADDR
//       REG3 -> SIZE
//       ACC0 -> EADDR
// modified : none
// ----------------------------
void vpp_HelloRunDma(void)
{
    VoC_sw16_d(REG2,CFG_DMA_LADDR);
    VoC_sw16_d(REG3,CFG_DMA_SIZE);
    VoC_sw32_d(ACC0,CFG_DMA_EADDR);
    VoC_cfg(CFG_CTRL_STALL);
    VoC_NOP();
    VoC_cfg_lg(CFG_WAKEUP_DMAI,CFG_WAKEUP_STATUS);
    VoC_return;
}

/**************************************************************************************
 * Function:    Coolsand_GlobalReset
 *
 * Description: Reset the AEC.
 *
 * Inputs:      no
 *
 * Outputs:     no
 *
 * Used :REG0,REG1,REG2,ACC0,INC0,INC1,INC2
 *
 * Version 1.0  Created by  Xuml       07/23/2010
 * Version 2.0  Created by  Xuml       05/19/2011
 **************************************************************************************/
void Coolsand_AECReset(void)
{
    VoC_push16(RA,DEFAULT);
    VoC_lw16_d(REG0,GLOBAL_RESET_ADDR);
    VoC_bez16_r(Coolsand_AnalysisProcessing_L1,REG0)

    VoC_lw32z(ACC0,DEFAULT);

    VoC_lw16i_set_inc(REG0,r_SpkAnaFilterDelayLine,2);
    VoC_lw16i_set_inc(REG1,r_MicAnaFilterDelayLine,2);
    VoC_lw16i_set_inc(REG2,r_OutSynFilterDelayLine,2);

    VoC_loop_i(0,PROTOTYPE_FILTER_LEN/2)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,r_AF_SpkSigBufI,2);
    VoC_lw16i_set_inc(REG1,r_AF_SpkSigBufQ,2);
    VoC_lw16i_set_inc(REG2,r_AF_FilterBufI,2);
    VoC_lw16i_set_inc(REG3,r_AF_FilterBufQ,2);


    VoC_loop_i(0,(SUBBANDFILTER_NUM*FILTER_LEN)/2)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,g_AF_FilterOutBufI,1);
    VoC_lw16i_set_inc(REG1,g_AF_FilterOutBufQ,1);
    VoC_lw16i_set_inc(REG2,r_AF_OldSpkDataBufI,1);
    VoC_lw16i_set_inc(REG3,r_AF_OldSpkDataBufQ,1);

    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG3,DEFAULT);
    VoC_endloop0


    VoC_lw16i_set_inc(REG0,r_AF_SpkSigPower,2);
    VoC_lw16i_set_inc(REG1,r_AF_RErrSpkI,2);
    VoC_lw16i_set_inc(REG2,r_AF_RErrSpkQ,2);
    VoC_lw16i_set_inc(REG3,r_AF_SpkSigSmoothPower,2);

    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    VoC_sw16_d(ACC0_LO,m_AF_FrameCnt);

    /*
            VoC_lw16i(REG0,AdaptiveFilter);
            VoC_lw16i(REG1,32);
            VoC_lw16i(REG2,4);
            VoC_lw16i(REG3,ECHO_COHF);
            VoC_sw16_d(REG0,m_AECProcPara_ProcessMode);
            VoC_sw16_d(REG1,m_AECProcPara_AFUpdateFactor);
            VoC_sw16_d(REG2,m_AECProcPara_InitRegularFactor);
            VoC_sw16_d(REG3,m_AECProcPara_EchoEstMethod);


            VoC_lw16i_short(REG0,16,DEFAULT);
            VoC_lw16i_short(REG1,4,IN_PARALLEL);
            VoC_lw16i_short(REG2,13,DEFAULT);
            VoC_lw16i_short(REG3,1,IN_PARALLEL);
            VoC_sw16_d(REG0,m_AECProcPara_NoiseUpdateFactor);
            VoC_sw16_d(REG1,m_AECProcPara_SNRUpdateFactor);
            VoC_sw16_d(REG2,m_AECProcPara_SNRLimit);
            VoC_sw16_d(REG3,m_AECProcPara_NoiseOverEstFactor);

            VoC_lw16i(REG0,40);
            VoC_lw16i(REG1,26);
            VoC_lw16i(REG2,38);
            VoC_lw16i(REG3,6);
            VoC_sw16_d(REG0,m_AECProcPara_NoiseGainLimit);
            VoC_sw16_d(REG1,m_AECProcPara_EchoUpdateFactor);
            VoC_sw16_d(REG2,m_AECProcPara_SERUpdateFactor);
            VoC_sw16_d(REG3,m_AECProcPara_SERLimit);

            VoC_lw16i_short(REG0,2,DEFAULT);
            VoC_lw16i_short(REG1,4,IN_PARALLEL);
            VoC_lw16i_short(REG2,2,DEFAULT);
            VoC_lw16i_short(REG4,4,IN_PARALLEL);
            VoC_lw16i_short(REG5,0,DEFAULT);
            VoC_lw16i_short(REG6,90,IN_PARALLEL);
            VoC_sw16_d(REG0,m_AECProcPara_EchoOverEstFactor);
            VoC_sw16_d(REG1,m_AECProcPara_EchoGainLimit);
            VoC_sw16_d(REG2,m_AECProcPara_CNGFactor);
            VoC_sw32_d(REG45,m_AECProcPara_NoiseMin);
            VoC_sw16_d(REG6,m_AECProcPara_AmpGain);*/




    VoC_lw16i_set_inc(REG0,m_Taps,2);

    VoC_lw16i_short(REG4,31,DEFAULT);
    VoC_lw16i_short(REG5,27,IN_PARALLEL);
    VoC_lw16i_short(REG6,26,DEFAULT);
    VoC_lw16i_short(REG7,0,IN_PARALLEL);

    VoC_sw32inc_p(REG45,REG0,DEFAULT);
    VoC_lw16i_short(REG4,1,IN_PARALLEL);

    VoC_lw16i_set_inc(REG1,m_Registers,1);

    VoC_sw32inc_p(REG67,REG0,DEFAULT);
    VoC_lw16i_short(REG3,0,IN_PARALLEL);

    VoC_sw16inc_p(REG4,REG1,DEFAULT);

    VoC_loop_i(0,31)
    VoC_sw16inc_p(REG3,REG1,DEFAULT);
    VoC_endloop0


//      VoC_lw32z(RL6,DEFAULT);
//       VoC_lw16i_short(REG0,1,DEFAULT);
    //       VoC_sw32_d(RL6,CONST_32bit0_addr);
    //     VoC_sw16_d(REG0,CONST_1_addr);


    VoC_lw32z(ACC0,DEFAULT);
    VoC_lw16i_set_inc(REG0,r_PF_SmoothErrPower,2);
    VoC_lw16i_set_inc(REG1,r_PF_NoisePowerEstBuf,2);
    VoC_lw16i_set_inc(REG2,r_PF_PrevSigPowerNBuf,2);
    VoC_lw16i_set_inc(REG3,r_PF_PrevSigPowerEBuf,2);

    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);
    VoC_endloop0

    VoC_lw16i_set_inc(REG0,r_PF_NoiseFloorDevCnt,1);
    VoC_lw16i_set_inc(REG1,g_PF_PostSToNRatio,1);
    VoC_lw16i_set_inc(REG2,g_PF_EchoWeight,1);
    VoC_lw16i_set_inc(REG3,g_PF_NoiseWeight,1);


    VoC_lw16i(REG7,127);

    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_sw16inc_p(REG7,REG2,DEFAULT);
    VoC_sw16inc_p(REG7,REG3,DEFAULT);
    VoC_endloop0

    VoC_sw32_d(ACC0,r_PF_CL_MicNoise);
    VoC_sw32_d(ACC0,r_PF_CL_MicNoise+2);
    VoC_sw32_d(ACC0,r_PF_CL_MicSmoothPower);
    VoC_sw32_d(ACC0,r_PF_CL_MicSmoothPower+2);

    VoC_sw32_d(ACC0,r_PF_CL_SpkNoise);
    VoC_sw32_d(ACC0,r_PF_CL_SpkNoise+2);
    VoC_sw32_d(ACC0,r_PF_CL_SpkSmoothPower);
    VoC_sw32_d(ACC0,r_PF_CL_SpkSmoothPower+2);

    VoC_sw32_d(ACC0,r_PF_CL_MicNoiseFloorDevCnt);
    VoC_sw32_d(ACC0,r_PF_CL_SpkNoiseFloorDevCnt);

    VoC_sw16_d(ACC0_LO,r_PF_CL_DTCountDown);

    VoC_sw16_d(ACC0_LO,r_PF_CL_DTCountUp);

    VoC_sw32_d(ACC0,r_PF_CL_ErrSmoothPower);
    VoC_sw32_d(ACC0,r_PF_CL_ErrSmoothPower+2);

    VoC_lw16i_short(ACC1_HI,0,DEFAULT);
    VoC_lw16i_short(ACC1_LO,32,IN_PARALLEL);

    VoC_lw16i(REG7,0x7f);

    VoC_lw16i_set_inc(REG0,r_PF_NoisePowerEstBuf,2);
    VoC_lw16i_set_inc(REG1,r_PF_NoisePowerSmoothEstBuf,2);

    VoC_lw16i_set_inc(REG2,g_PF_PriorSToERatio,1);

    VoC_lw16i_set_inc(REG3,r_PF_EchoPowerEstBuf,2);


    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw32inc_p(ACC1,REG0,DEFAULT);
    VoC_sw32inc_p(ACC1,REG1,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG2,DEFAULT);
    VoC_sw32inc_p(ACC0,REG3,DEFAULT);

    VoC_endloop0


    VoC_lw16i_set_inc(REG0,r_PF_SpkPowerBuf,2);
    VoC_lw16i_set_inc(REG1,r_PF_PrevEchoPowerEst,2);
    VoC_lw16i_set_inc(REG2,r_PF_PrevSpkPower,2);

    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_sw32inc_p(ACC0,REG2,DEFAULT);
    VoC_endloop0


    VoC_lw16i_set_inc(REG0,r_PF_CCErrSpkVecBufI,2);
    VoC_lw16i_set_inc(REG1,r_PF_CCErrSpkVecBufQ,2);


    VoC_loop_i(0,72)
    VoC_sw32inc_p(ACC0,REG0,DEFAULT);
    VoC_sw32inc_p(ACC0,REG1,DEFAULT);
    VoC_endloop0


    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicErrI);
    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicErrQ);
    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicEchoI);
    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicEchoQ);
    VoC_sw32_d(ACC0,r_PF_CL_DTDMicSmoothPower);
    VoC_sw32_d(ACC0,r_PF_CL_DTDErrSmoothPower);
    VoC_sw32_d(ACC0,r_PF_CL_DTDEchoSmoothPower);
    VoC_sw32_d(ACC0,r_PF_CL_DTDMicNoisePower);
    VoC_sw32_d(ACC0,r_PF_CL_DTDMicNoiseFloorDevCnt);
    VoC_sw32_d(ACC0,r_PF_CL_DTDDecision);
    VoC_sw32_d(ACC0,r_PF_CL_DTD2ndDecision);

    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicErrI+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicErrQ+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicEchoI+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDXcorrMicEchoQ+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDMicSmoothPower+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDErrSmoothPower+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDEchoSmoothPower+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDMicNoisePower+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDMicNoiseFloorDevCnt+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTDDecision+2);
    VoC_sw32_d(ACC0,r_PF_CL_DTD2ndDecision+2);




    VoC_lw16_d(REG2,m_AECProcPara_AFFilterAmp);


    VoC_lw16i_set_inc(REG0,r_AF_FilterAmpI,1);
    VoC_lw16i_set_inc(REG1,r_AF_FilterAmpQ,1);


    VoC_loop_i(0,SUBBANDFILTER_NUM)
    VoC_sw16inc_p(REG2,REG0,DEFAULT);
    VoC_sw16inc_p(ACC0_LO,REG1,DEFAULT);
    VoC_endloop0

    VoC_lw32_d(ACC0,GLOBAL_CONST_PTR);
    VoC_lw16i(REG2,AnaFilterCoef/2);
    VoC_lw16i(REG3,GLOBAL_CONST_SIZE);
    VoC_jal(vpp_HelloRunDma);

//{6585,2430,22692,2430,6585}



    /*      VoC_lw16i_set_inc(REG0,g_PE_PPFilterBuf,1);

            VoC_loop_i(0,g_PE_PPFilterLen)
                VoC_sw16inc_p(ACC0_LO,REG0,DEFAULT);
            VoC_endloop0*/

    /*
            VoC_lw16i_set_inc(REG0,g_PE_PPFCoef_ADDR,1);
            VoC_lw16i(REG1,7470);
            VoC_lw16i(REG2,3896);
            VoC_lw16i(REG3,19711);

            VoC_sw16inc_p(REG1,REG0,DEFAULT);
            VoC_sw16inc_p(REG2,REG0,DEFAULT);
            VoC_sw16inc_p(REG3,REG0,DEFAULT);
            VoC_sw16inc_p(REG2,REG0,DEFAULT);
            VoC_sw16inc_p(REG1,REG0,DEFAULT); */

Coolsand_AnalysisProcessing_L1:

    VoC_pop16(RA,DEFAULT);
    VoC_NOP();
    VoC_return;
}

